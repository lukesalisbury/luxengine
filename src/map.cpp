/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include "engine.h"
#include "elix_string.hpp"
#include "display.h"
#include "mokoi_game.h"
#include "map_object.h"
#include "masks.h"
#include "map.h"
#include "entity.h"
#include "entity_manager.h"
#include "game_config.h"
#include "core.h"
#include "map_internal.h"
#include "map_xml_reader.h"

#include <algorithm>



/**
 * @brief MokoiMap::MokoiMap
 * @param name
 * @param width
 * @param height
 */
MokoiMap::MokoiMap( std::string name, uint32_t width, uint32_t height)
{
	lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << " > Opening map '" << name << "'"  << std::endl;

	this->InitialSetup(name);

	if ( width )
		this->map_width = width;

	if ( height )
		this->map_height = height;

}

/**
 * @brief MokoiMap::MokoiMap
 * @param current_save_file
 */
MokoiMap::MokoiMap( elix::File * current_save_file )
{
	if ( this->Restore( current_save_file ) )
	{
		lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << " > Restore map '" << this->map_name << "'"  << std::endl;
	}
}

/**
 * @brief MokoiMap::~MokoiMap
 */
MokoiMap::~MokoiMap()
{
	lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << " < Closing Map '" << this->map_name << "'"<< std::endl;

	for ( MapObjectListIter p = this->object_cache.begin(); p != this->object_cache.end(); p++ )
	{
		delete p->second;
	}
	this->object_cache.clear();

	if ( !this->screens.empty() )
	{
		for ( std::map<uint32_t, MokoiMapScreen *>::iterator q = this->screens.begin(); q != this->screens.end(); q++ )
		{
			delete q->second;
		}
	}
	this->screens.clear();

	if ( this->entities )
	{
		delete this->entities;
	}
}

/**
 * @brief MokoiMap::InitialSetup
 * @param map_name
 */
void MokoiMap::InitialSetup( std::string map_name )
{
	this->map_name = map_name;
	this->loaded = false;
	this->active = false;
	this->reset_map = false;

	this->keep_savedata = false;
	this->keep_memory = false;

	this->ident.value = 0;
	this->centered_view = lux::config->GetBoolean("map.centerview");

	this->object_cache_count = 0;
	this->entities = NULL;
	this->wrap_mode = MAP_WRAPNONE;
	this->server = false;
	this->entity_file_name = "maps/" + this->map_name;

	this->screen_width = lux::config->GetFixed("screen.width");
	this->screen_height = lux::config->GetFixed("screen.height");
	this->default_map_width = lux::config->GetFixed("map.width");
	this->default_map_height = lux::config->GetFixed("map.height");

	this->dimension_width = 1;
	this->dimension_height = 1;
	this->has_width_different = ( this->default_map_width != this->screen_width );
	this->has_height_different = ( this->default_map_height != this->screen_height );

	this->map_width = this->default_map_width;
	this->map_height = this->default_map_height;

	this->offset_position[0] = 0;
	this->offset_position[1] = 0;
	this->offset_position[2] = 0;


	this->grid[0] = 0;
	this->grid[1] = 0;
}

/* Settings */

/**
 * @brief MokoiMap::SetIdent
 * @param ident
 */
void MokoiMap::SetIdent( uint32_t ident )
{
	this->ident.value = ident;
}

/**
 * @brief MokoiMap::SetGridIdent
 * @param local
 * @param section
 */
void MokoiMap::SetGridIdent( uint32_t local, uint32_t section )
{
	this->ident.value = 0;
	this->ident.grid.map = local;
	this->ident.grid.section = section;
}

/**
 * @brief MokoiMap::Ident
 * @return
 */
uint32_t MokoiMap::Ident()
{
	return this->ident.value;
}

/**
 * @brief MokoiMap::GridIdent
 * @return
 */
uint32_t MokoiMap::GridIdent()
{
	return this->ident.grid.section;
}

/**
 * @brief MokoiMap::InSection
 * @param section
 * @return
 */
bool MokoiMap::InSection(uint32_t section)
{
	return (this->ident.grid.section == section);
}

/**
 * @brief MokoiMap::Name
 * @return
 */
std::string MokoiMap::Name()
{
	return this->map_name;
}

/**
 * @brief modify background colour
 * @param mod LuxColour
 */
void MokoiMap::SetBackgroundModifier( LuxColour mod )
{
	float fr, fg, fb;
	fr = (float)mod.r / 255;
	fg = (float)mod.g / 255;
	fb = (float)mod.b / 255;

	this->background_object.effects.primary_colour.r = (uint8_t)((float)this->base_background_colour.r * fr);
	this->background_object.effects.primary_colour.g = (uint8_t)((float)this->base_background_colour.g * fg);
	this->background_object.effects.primary_colour.b = (uint8_t)((float)this->base_background_colour.b * fb);
}

/**
 * @brief MokoiMap::GetGrid
 * @param axis
 * @return
 */
uint8_t MokoiMap::GetGrid( uint8_t axis )
{
	if ( axis == 'x')
		return this->grid[0];
	else
		return this->grid[1];
}

/**
 * @brief MokoiMap::Init
 * @return
 */
bool MokoiMap::Init()
{
	lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << " > MokoiMap Init " << this->map_name << std::endl;

	//lux::screen::display("Loading Map: " + this->map_name);
	if ( this->LoadFile() )
	{
		lux::display->SetBackgroundObject( this->background_object );

		this->active = true;
		this->entities->Init();

		this->NetworkMapSwitch();
	}
	else
	{
		lux::engine->FatalError("Map " + this->map_name + ".xml can't be loaded");
		this->active = false;
	}

	return this->active;
}

/**
 * @brief MokoiMap::Loop
 * @return
 */
bool MokoiMap::Loop()
{
	if ( !this->loaded )
	{
		lux::engine->ShowDialog("Map not loaded\nFile Name: " + this->map_name + "\n", DIALOGOK);
		return false;
	}

	/* Cheats way of reseting map */
	if ( this->reset_map )
	{
		this->Close();
		this->LoadFile();
		this->reset_map = false;
	}

	this->NetworkMapLoop();

	lux::display->SetBackgroundObject( this->background_object );

	this->entities->Loop();

	/* Screens */
	uint32_t screen_number = 0;
	MokoiMapScreen * screen = NULL;
	if ( this->dimension_width == 1 && this->dimension_height == 1 )
	{
		screen = this->GetScreen(screen_number);
		if ( screen )
		{
			screen->Init();
		}
	}
	else if ( this->wrap_mode )
	{
		/* Make sure the current screen and the 8 surround screens are loaded
		todo: unload non-active screens
		*/
		screen_number = this->XY2Screen( MAKE_FIXED_INT(this->offset_position[0]), MAKE_FIXED_INT(this->offset_position[1]), this->dimension_width );
		int32_t x = -1;
		int32_t y = -1;
		int32_t cx = 2;
		int32_t cy = 2;
		int32_t ty = -1;

		if ( !(screen_number % this->dimension_width) ) // Left Row
			x = 0;
		if ( screen_number % (this->dimension_width-1) ) // Right Row
			cx = 1;
		if ( screen_number < this->dimension_width ) // Top Row
			y = 0;
		if ( screen_number > (this->dimension_height * (this->dimension_width-1))) // Bottom row
			cy = 1;

		lux::display->debug_msg << "Screen Loaded:";
		for (; x < cx; x++)
		{
			for (ty = y; ty < cy; ty++)
			{
				screen = this->GetScreen( (screen_number+x) + (ty*this->dimension_width) );
				if ( screen )
				{
					lux::display->debug_msg << (int)((screen_number+x) + (ty*this->dimension_width)) << "(" << Screen2X(screen_number,this->dimension_width)+x << "x" << Screen2Y(screen_number,this->dimension_width)+ty << "),";
					screen->Init();
				}
			}
		}
		lux::display->debug_msg << std::endl;
	}
	else
	{
		/* Make sure the current screen and the 8 surround screens are loaded
		todo: unload non-active screens
		*/
		screen_number = this->XY2Screen( MAKE_FIXED_INT(this->offset_position[0]), MAKE_FIXED_INT(this->offset_position[1]), this->dimension_width );
		int32_t x = -1;
		int32_t y = -1;
		int32_t cx = 2;
		int32_t cy = 2;
		int32_t ty = -1;
		if ( !(screen_number % this->dimension_width) ) // Left Row
			x = 0;
		if ( screen_number % (this->dimension_width-1) ) // Right Row
			cx = 1;
		if ( screen_number < this->dimension_width ) // Top Row
			y = 0;
		if ( screen_number > (this->dimension_height * (this->dimension_width-1))) // Bottom row
			cy = 1;
		lux::display->debug_msg << "Screen Loaded:";
		for (; x < cx; x++)
		{
			for (ty = y; ty < cy; ty++)
			{
				screen = this->GetScreen( (screen_number+x) + (ty*this->dimension_width) );
				if ( screen )
				{
					lux::display->debug_msg << (int)((screen_number+x) + (ty*this->dimension_width)) << "(" << Screen2X(screen_number,this->dimension_width)+x << "x" << Screen2Y(screen_number,this->dimension_width)+ty << "),";
					screen->Init();
				}
			}
		}
		lux::display->debug_msg << std::endl;
	}
	return true;
}

/*
* MokoiMap::Close()
* Keeps Map in memory, just unload every thing else.
*/
bool MokoiMap::Close()
{
	lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << " < MokoiMap Init " << this->map_name << std::endl;

	this->active = false;
	this->server = false;

	/* Reset Display */
	lux::display->ClearLayers(true);
	lux::display->SetCameraView( 0, 0 );
	lux::display->ResetBackgroundObject( );

	/* Clean up Screens */
	std::map<uint32_t, MokoiMapScreen *>::iterator q = this->screens.begin();
	for ( ; q != this->screens.end(); q++ )
	{
		q->second->Close();
	}

	/* Clean Up Map Object */
	for ( MapObjectListIter p = this->object_cache.begin(); p != this->object_cache.end(); p++ )
	{
		p->second->FreeData();
	}

	/* Clean Up Entity */
	this->entities->Close();

	return true;
}

std::string MokoiMap::GetInfo()
{
	std::stringstream s;
	s << "§6Objects:" << this->object_cache.size()<< " §cEntities:" << this->entities->children.size() << " §aScreen:" << this->screens.size();
	return s.str();
}

bool MokoiMap::Reset()
{
	this->reset_map = true;


	return true;
}

/* Object Handling */
void MokoiMap::AddObjectToScreens( MapObject * object )
{
	uint32_t screen_number = 0;
	MokoiMapScreen * screen = NULL;

	int32_t sx = (object->position.x/this->default_map_width)*this->default_map_width;
	int32_t sy = (object->position.y/this->default_map_height)*this->default_map_height;
	int32_t sw = object->position.w ? object->position.w : object->sprite_width;
	int32_t sh = object->position.h ? object->position.h : object->sprite_height;

	//note to self: may be wrong for lines
	if ( this->dimension_width > 1 || this->dimension_height > 1 )
	{
		while ( sx < object->position.x + sw )
		{
			sy = object->position.y;
			while ( sy < object->position.y + sh )
			{
				screen_number = XY2Screen( sx, sy, this->dimension_width );
				screen = this->GetScreen(screen_number);
				if ( screen )
				{
					screen->_objects.push_back(object);

				}
				sy += this->default_map_height;
			}
			sx += this->default_map_width;
		}
	}
	else
	{
		screen_number = XY2Screen( object->position.x, object->position.y, this->dimension_width );
		screen = this->GetScreen(screen_number);
		if ( screen )
			screen->_objects.push_back(object);
	}
}

uint32_t MokoiMap::AddObject(MapObject * object, bool is_static )
{
	uint32_t object_id = 0;

	object->SetData( object->type );

	if ( object->has_data )
	{
		this->object_cache_count++;
		if ( is_static )
		{
			object->SetStaticMapID( this->object_cache_count, false );
			object_id = object->GetStaticMapID();
			this->object_cache[object_id] = object;
		}

		if ( this->active )
		{
			lux::display->AddObjectToLayer(object->layer, object, is_static );
		}
	}
	return object_id;
}

MapObject * MokoiMap::GetObject( uint32_t ident )
{
	if ( !this->object_cache.empty() )
	{
		MapObjectListIter iter = this->object_cache.find(ident);
		if( iter != this->object_cache.end() )
			return iter->second;
	}
	return NULL;
}

void MokoiMap::ReplaceObjectsSheets( std::string old_sheet, std::string new_sheet )
{
	for ( MapObjectListIter p = this->object_cache.begin(); p != this->object_cache.end(); p++ )
	{
		MapObject * object = p->second;
		if ( object->type == OBJECT_SPRITE )
		{
			std::string sheet = object->sprite.substr(0, object->sprite.find_first_of(':'));
			if ( !old_sheet.compare( sheet ) )
			{
				object->FreeData();

				std::string sprite = object->sprite.substr(object->sprite.find_first_of(':'));
				object->sprite = new_sheet + sprite;
				object->SetData( OBJECT_SPRITE);
			}
		}
	}

}

bool MokoiMap::RemoveObject( uint32_t ident )
{
	MapObject * object = this->GetObject(ident);
	if ( object )
	{
		lux::display->RemoveObject(object->layer, object);
		this->object_cache.erase(ident);

		if ( !object->layer_reference )
			delete object;

		return true;
	}
	return false;
}

/* Positions */
void MokoiMap::SetPosition( fixed position[3] )
{
	this->offset_position[0] = position[0];
	this->offset_position[1] = position[1];
	this->offset_position[2] = position[2];

	lux::display->debug_msg << "Wrap: " << (int)this->wrap_mode << " - Centerview:" << (int)this->centered_view <<  std::endl;
	/*
		Keeps Offset for moving offscreen
	*/

	if ( this->centered_view )
	{
		fixed offset, wrap_offset = 0;

		/* X Axis Wrapping handling */
		if ( this->wrap_mode == MAP_WRAPXAXIS || this->wrap_mode == MAP_WRAPBOTH )
		{
			if ( position[0] > this->map_width )
			{
				wrap_offset = (position[0] / this->map_width) * this->map_width;
				position[0] %= this->map_width;
			}
		}

		/* X Axis handle*/
		if ( this->dimension_width > 1 ) // Center the View
		{
			if ( position[0] < (screen_width / 2) )
			{
				this->offset_position[0] = 0;
			}
			else if ( position[0] > (this->map_width - (this->screen_width / 2)) )
			{
				this->offset_position[0] = this->map_width - this->screen_width;
			}
			else
			{
				this->offset_position[0] = position[0] - (this->screen_width/2);
			}
		}
		else if ( this->has_width_different ) // If Screen dimension is maps, then try to account for that
		{
			offset = this->map_width - this->screen_height;
			if ( position[0] > (this->screen_width - offset) )
			{
				this->offset_position[0] = this->map_width - this->screen_width;
			}
			else if ( position[0] > offset )
			{
				this->offset_position[0] -= offset;
			}
			else
			{
				this->offset_position[0] = 0;
			}
		}
		else// No view position change needed.
		{
			this->offset_position[0] = 0;
		}
		this->offset_position[0] += wrap_offset;



		wrap_offset = 0;
		if ( this->wrap_mode == MAP_WRAPBOTH || this->wrap_mode == MAP_WRAPYAXIS )
		{
			if ( position[1] > this->map_height )
			{
				wrap_offset = (position[1] / this->map_height) * this->map_height;
				position[1] %= this->map_height;
			}
		}

		/* Y Axis handle*/
		if ( this->dimension_height > 1 ) // Center the View
		{
			if ( position[1] < (screen_height / 2) )
			{
				this->offset_position[1] = 0;
			}
			else if ( position[1] > (this->map_height - (this->screen_height / 2)) )
			{
				this->offset_position[1] = this->map_height - this->screen_height;
			}
			else
			{
				this->offset_position[1] = position[1] - (this->screen_height / 2);
			}
		}
		else if ( this->has_height_different ) // If Screen dimension is maps, then try to account for that
		{
			offset = this->map_height - this->screen_height;
			if ( position[1] > (this->screen_height - offset) )
			{
				this->offset_position[1] = this->map_height - this->screen_height;
			}
			else if ( position[1] > offset )
			{
				this->offset_position[1] -= offset;
			}
			else
			{
				this->offset_position[1] = 0;
			}
		}
		else // No view position change needed.
		{
			this->offset_position[1] = 0;
		}
		this->offset_position[1] += wrap_offset;
	}
	else
	{
		/* Using the top left for position */
		if ( this->wrap_mode == MAP_WRAPNONE || this->wrap_mode == MAP_WRAPYAXIS )
		{
			this->offset_position[0] = clamp(this->offset_position[0], 0, this->map_width - this->screen_width);
		}
		if ( this->wrap_mode == MAP_WRAPNONE || this->wrap_mode == MAP_WRAPXAXIS )
		{
			this->offset_position[1] = clamp(this->offset_position[1], 0, this->map_height - this->screen_height);
		}
	}
}

fixed MokoiMap::GetPosition( uint8_t axis )
{
	if ( axis < 3 )
	{
		return this->offset_position[axis];
	}
	return 0;
}

void MokoiMap::SetScrolling( bool scroll )
{
	this->centered_view = scroll;
}

/* Entities */
EntitySection * MokoiMap::GetEntities()
{
	return this->entities;
}

/* Save System */

/**
 * @brief MokoiMap::Save
 * @param current_save_file
 * @return
 */
bool MokoiMap::Save( elix::File * current_save_file )
{
	current_save_file->WriteWithLabel("Map Name", this->map_name );
	current_save_file->WriteWithLabel("Map ID", this->ident.value );
	current_save_file->WriteWithLabel("Map Width", (uint32_t)this->map_width);
	current_save_file->WriteWithLabel("Map Height", (uint32_t)this->map_height);
	current_save_file->WriteWithLabel("Map Background", this->base_background_colour.r );
	current_save_file->WriteWithLabel("Map Background", this->base_background_colour.b );
	current_save_file->WriteWithLabel("Map Background", this->base_background_colour.g );
	current_save_file->WriteWithLabel("Map Loaded Flag", (uint8_t)this->loaded );

	if ( this->entities )
	{
		current_save_file->WriteWithLabel("Map has Entities", (uint8_t)1 );
		this->entities->Save( current_save_file );
	}
	else
	{
		current_save_file->WriteWithLabel("Map has Entities", (uint8_t)0 ); // Write 0 for empty  Entity Section
	}

	/* Map Display Object */
	uint32_t count = this->object_cache.size();
	current_save_file->WriteWithLabel("Map Display Objects", count );
	if ( count )
	{
		MapObjectList::iterator iter = this->object_cache.begin();
		while( iter != this->object_cache.end() )
		{
			iter->second->Save( current_save_file );
			iter++;
		}
	}
	return true;
}

/**
 * @brief MokoiMap::Restore
 * @param current_save_file
 * @return
 */
bool MokoiMap::Restore( elix::File * current_save_file )
{
	uint32_t count = 0;
	uint8_t map_loading_flag = 0;
	uint8_t map_has_entities = 0;

	current_save_file->ReadWithLabel("Map Name", &this->map_name );
	this->InitialSetup(this->map_name);

	this->ident.value= current_save_file->ReadUint32WithLabel("Map ID", true );
	this->map_width = current_save_file->ReadUint32WithLabel("Map Width");
	this->map_height = current_save_file->ReadUint32WithLabel("Map Height");

	this->background_object.effects.primary_colour.r = this->base_background_colour.r = current_save_file->ReadUint8WithLabel("Map Background");
	this->background_object.effects.primary_colour.g = this->base_background_colour.g = current_save_file->ReadUint8WithLabel("Map Background");
	this->background_object.effects.primary_colour.b = this->base_background_colour.b = current_save_file->ReadUint8WithLabel("Map Background");

	map_loading_flag = current_save_file->ReadUint8WithLabel("Map Loaded Flag");

	/* Entities */
	this->entities = new EntitySection( this->entity_file_name, this->ident.value );
	map_has_entities = current_save_file->ReadUint8WithLabel("Map has Entities");
	if ( map_has_entities )
	{
		this->entities->Restore( current_save_file );
	}

	/* Map Display Object */
	this->object_cache_count = current_save_file->ReadUint32WithLabel("Map Display Objects", true );
	if ( this->object_cache_count )
	{
		for( uint32_t i = 0; i < this->object_cache_count; i++ )
		{
			MapObject * existing_object = NULL;
			existing_object = new MapObject( current_save_file );
			this->object_cache[existing_object->GetStaticMapID()] = existing_object;

			if ( existing_object->type == OBJECT_VIRTUAL_SPRITE )
			{
				LuxVirtualSprite * sprite = existing_object->InitialiseVirtual( );
				sprite->PushObjectsToMap( existing_object, this->object_cache, ++object_cache_count, false  );
			}

		}
	}



	this->loaded = true;
	return true;
}

/* Screen */

/**
 * @brief MokoiMap::GetScreen
 * @param screen_number
 * @param init_new
 * @return
 */
MokoiMapScreen * MokoiMap::GetScreen(uint32_t screen_number, bool init_new)
{
	if ( this->dimension_width == 1 && this->dimension_height == 1 )
		screen_number = 0;
	if ( screen_number > (this->dimension_width * this->dimension_height)-1 )
		return NULL;
	if ( !this->screens.empty() )
	{
		std::map<uint32_t, MokoiMapScreen *>::iterator iter = this->screens.find(screen_number);
		if( iter != this->screens.end() )
			return iter->second;
	}
	MokoiMapScreen * new_screen = new MokoiMapScreen( Screen2X(screen_number, this->dimension_width), Screen2Y(screen_number, this->dimension_width), MAKE_FIXED_INT(this->default_map_width), MAKE_FIXED_INT(this->default_map_height) );
	this->screens.insert( std::make_pair( screen_number, new_screen) );
	//if (init_new)
	//     new_screen->Init();
	return new_screen;
}

/*
MokoiMapScreen * MokoiMap::UnloadScreen(uint32_t screen_number, bool init_new)
{
	if ( this->width == 1 && this->height == 1 )
		screen_number = 0;
	if ( screen_number > (this->width * this->height)-1 )
		return NULL;
	std::map<uint32_t, MokoiMapScreen *>::iterator iter = this->_screen.find(screen_number);
	if( iter != this->_screen.end() )
		return iter->second;
	MokoiMapScreen * new_screen = new MokoiMapScreen( Screen2X(screen_number, this->width), Screen2Y(screen_number, this->width), this->default_map_width, this->default_map_height );
	this->_screen.insert( std::make_pair( screen_number, new_screen) );
	return new_screen;
}

MokoiMapScreen * MokoiMap::LoadAllScreen(uint32_t screen_number, bool init_new)
{
	if ( this->width == 1 && this->height == 1 )
		screen_number = 0;
	if ( screen_number > (this->width * this->height)-1 )
		return NULL;
	std::map<uint32_t, MokoiMapScreen *>::iterator iter = this->_screen.find(screen_number);
	if( iter != this->_screen.end() )
		return iter->second;
	MokoiMapScreen * new_screen = new MokoiMapScreen( Screen2X(screen_number, this->width), Screen2Y(screen_number, this->width), this->default_map_width, this->default_map_height );
	this->_screen.insert( std::make_pair( screen_number, new_screen) );
	return new_screen;
}
*/

/**
 * @brief MokoiMap::XY2Screen
 * @param x
 * @param y
 * @param w
 * @return
 */
uint32_t MokoiMap::XY2Screen(int32_t x, int32_t y, int32_t w)
{
	if ( this->wrap_mode )
	{
		int32_t mw = MAKE_FIXED_INT(this->map_width);
		int32_t mh = MAKE_FIXED_INT(this->map_height);
		while ( x > mw )
		{
			x -= mw;
		}
		while ( y > mh )
		{
			y -= mh;
		}
		return (uint32_t)(((y/this->default_map_height)*w)+(x/this->default_map_height));
	}
	else
	{
		return (uint32_t)(((y/this->default_map_height)*w)+(x/this->default_map_height));
	}
}

/* Data */

/**
 * @brief MokoiMap::Valid
 * @return
 */
bool MokoiMap::Valid()
{
	return this->loaded;
}

/*
* MokoiMap::LoadDimension()
* Todo: Optimise code.
*/
bool MokoiMap::LoadDimension()
{
	if ( !this->loaded )
	{
		tinyxml2::XMLDocument * xml_file = MokoiGame_GetXML("./maps/" + this->map_name + ".xml");
		if ( xml_file->Error() )
		{
			//lux::core->SystemMessage(__FILE__ , __LINE__, SYSTEM_MESSAGE_ERROR) << " | " << xml_file->ErrorDesc() << " Row: " << xml_file->ErrorRow() << std::endl;
			delete xml_file;
			return false;
		}
		if ( !xml_file->RootElement() || strcmp( xml_file->RootElement()->Value(), "map") )
		{
			lux::core->SystemMessage(__FILE__ , __LINE__, SYSTEM_MESSAGE_ERROR) << " | maps/" + this->map_name + ".xml not a vamap_nameap file." << std::endl;
			delete xml_file;
			return false;
		}

		tinyxml2::XMLElement * dimensions_elem = xml_file->RootElement()->FirstChildElement("settings")->FirstChildElement("dimensions");

		if ( dimensions_elem )
		{
			tinyxml2::QueryUnsignedAttribute( dimensions_elem, "width", this->dimension_width);
			tinyxml2::QueryUnsignedAttribute( dimensions_elem, "height", this->dimension_height);
		}

		this->map_width = MAKE_INT_FIXED( this->dimension_width * this->default_map_width );
		this->map_height = MAKE_INT_FIXED( this->dimension_height * this->default_map_height );
		delete xml_file;
		return true;
	}
	return false;
}

/**
 * @brief MokoiMap::SaveFile
 * @return
 */
bool MokoiMap::SaveFile()
{
	lux::screen::display("Saving Map: " + this->map_name);
	return false;
}

/**
 * @brief MokoiMap::LoadFile
 * @return
 */
bool MokoiMap::LoadFile()
{
	if ( this->loaded )
		return this->loaded;

	MapXMLReader reader;
	std::map<std::string, std::string> settings;

	this->wrap_mode = MAP_WRAPNONE;

	this->entities = new EntitySection( this->entity_file_name, this->ident.value );

	if ( lux::game_data->HasFile("./maps/" + this->map_name + ".xml") )
	{
		if ( !reader.Load("./maps/" + this->map_name + ".xml"))
		{
			lux::core->SystemMessage(__FILE__ , __LINE__, SYSTEM_MESSAGE_ERROR) << " | maps/" + this->map_name + ".xml not a valid file." << std::endl;
			return false;
		}

		reader.ReadSettings( this, settings );

		for ( std::map<std::string, std::string>::iterator cs = settings.begin(); cs != settings.end(); cs++ )
		{
			if ( !cs->first.compare("wrap") )
			{
				this->wrap_mode = elix::string::ToIntU8(cs->second);
			}

			if ( this->entities->parent )
			{
				this->entities->parent->AddSetting( cs->first, cs->second );
			}
		}

		/* Read Display Object */
		reader.ReadObjectsWithEntities( this->object_cache,  this );
		this->object_cache_count = this->object_cache.size();

		/* Set up Map Screen */
		for ( MapObjectListIter p = this->object_cache.begin(); p != this->object_cache.end(); p++ )
		{
			p->second->SetData( p->second->type );
			this->AddObjectToScreens( p->second );
		}


		std::map<uint32_t, MokoiMapScreen *>::iterator q = this->screens.begin();
		for ( ; q != this->screens.end(); q++ )
		{
			q->second->SortObjects();
		}

	}
	this->loaded = true;

	return true;
}

bool MokoiMap::HasSnapshot()
{

	return false;
}

bool MokoiMap::SaveSnapshot()
{
	return false;
}

bool MokoiMap::LoadSnapshot()
{
	return false;
}


/* Mask Functions */
uint16_t MokoiMap::GetMaskValue(uint16_t x, uint16_t y)
{
	MokoiMapScreen * screen = NULL;
	screen = this->GetScreen( XY2Screen( x, y, this->dimension_width ) );
	if ( screen )
		return screen->GetMaskValue(x, y);
	return 0;
}

void MokoiMap::FillMask(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t value)
{
	uint32_t screen_number = 0;
	MokoiMapScreen * screen = NULL;

	int32_t sx = x;
	int32_t sy = y;

	if ( this->dimension_width > 1 || this->dimension_height > 1  )
	{
		while ( sx < x + width )
		{
			while ( sy < y + height )
			{
				screen_number = XY2Screen( sx, sy, this->dimension_width );
				screen = this->GetScreen(screen_number);
				if ( screen )
					screen->FillMask(x, y, width, height, value);
				sy += this->default_map_height;
			}
			sx += this->default_map_width;
		}
	}
	else
	{
		screen = this->GetScreen(screen_number);
		if ( screen )
			screen->FillMask(x, y, width, height, value);
	}
}

void MokoiMap::BuildMask()
{
	std::map<uint32_t, MokoiMapScreen *>::iterator q = this->screens.begin();
	for ( ; q != this->screens.end(); q++ )
	{
		q->second->BuildMask();
	}
	//Lux_Mask * sprite_mask = Lux_Mask_New(0, 0);
	//Lux_Mask_Load("test.mask", sprite_mask);
	//Lux_Mask_CopyArea(sprite_mask, map_mask, 10, 10);
	//Lux_Mask_FillArea(map_mask, 5, 0, 10, 20, 69);
}

void MokoiMap::DrawMask()
{
	std::map<uint32_t, MokoiMapScreen *>::iterator q = this->screens.begin();
	for ( ; q != this->screens.end(); q++ )
	{
		q->second->DrawMask(this->offset_position);
	}
}

