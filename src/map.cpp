/****************************
Copyright © 2006-2011 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include "engine.h"
#include "elix_string.h"
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

bool RemoveMapObject( MapObject * object) { return object->can_remove; }

MokoiMap::MokoiMap( std::string name )
{
	std::cout << " > Opening map '" << name << "'"  << std::endl;

	this->InitialSetup(name);
}

MokoiMap::MokoiMap( elix::File * current_save_file )
{
	if ( this->Restore( current_save_file ) )
	{
		std::cout << " > Restore map '" << this->_name << "'"  << std::endl;
	}
}


MokoiMap::~MokoiMap()
{
	std::cout << " < Closing Map '" << this->_name << "'"<< std::endl;
	std::vector< MapObject * >::iterator p;
	for ( p = this->_objects.begin(); p != this->_objects.end(); p++ )
	{
		delete (*p);
		*p = NULL;
	}
	this->object_cache.clear();
	this->_objects.clear();

	if ( !this->_screen.empty() )
	{
		for ( std::map<uint32_t, MokoiMapScreen *>::iterator q = this->_screen.begin(); q != this->_screen.end(); q++ )
		{
			delete q->second;
		}
	}
	this->_screen.clear();

	if ( this->_entities )
	{
		delete this->_entities;
	}
}

/*
* MokoiMap::InitialSetup()
*
*/
void MokoiMap::InitialSetup( std::string map_name )
{
	this->_name = map_name;
	this->_loaded = false;
	this->active = false;

	this->_ident.value = 0;
	this->_centerview = lux::config->GetBoolean("map.centerview");

	this->object_cache_count = 0;
	this->_entities = NULL;
	this->_wrap = MAP_WRAPNONE;
	this->_server = false;
	this->_entity = "maps/" + this->_name;

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

	this->_position[0] = 0;
	this->_position[1] = 0;
	this->_position[2] = 0;
}

/* Settings */

/*
* MokoiMap::SetIdent()
*
*/
void MokoiMap::SetIdent( uint32_t ident )
{
	this->_ident.value = ident;
}

/*
* MokoiMap::SetGridIdent()
*
*/
void MokoiMap::SetGridIdent( uint32_t local, uint32_t section )
{
	this->_ident.value = 0;
	this->_ident.grid.localmap = local;
	this->_ident.grid.id = section;
}

/*
* MokoiMap::SetFileIdent()
*
*/
void MokoiMap::SetFileIdent( uint32_t local )
{
	this->_ident.value = 0;
	this->_ident.file.map = local;
	this->_ident.file.unused = 0;

}

/*
* MokoiMap::Ident()
*
*/
uint32_t MokoiMap::Ident()
{
	return this->_ident.value;
}

/*
* MokoiMap::GridIdent()
*
*/
uint32_t MokoiMap::GridIdent()
{
	return this->_ident.grid.localmap;
}

/*
* MokoiMap::FileIdent()
*
*/
uint32_t MokoiMap::FileIdent()
{
	return this->_ident.file.map;
}

/*
* MokoiMap::InSection()
*
*/
bool MokoiMap::InSection(uint32_t section)
{
	return (this->_ident.grid.id == section);
}

/*
* MokoiMap::PartOfSection()
*
*/
bool MokoiMap::PartOfSection()
{
	return !!this->_ident.grid.id;
}

/*
* MokoiMap::Name()
*
*/
std::string MokoiMap::Name()
{
	return this->_name;
}

/*
* MokoiMap::SetBackgroundMod()
* modify background colour
*/
void MokoiMap::SetBackgroundModifier( LuxColour mod )
{
	float fr, fg, fb;
	fr = (float)mod.r / 255;
	fg = (float)mod.g / 255;
	fb = (float)mod.b / 255;
	this->_background.effects.primary_colour.r = (uint8_t)((float)this->_colour.r * fr);
	this->_background.effects.primary_colour.g = (uint8_t)((float)this->_colour.g * fg);
	this->_background.effects.primary_colour.b = (uint8_t)((float)this->_colour.b * fb);
}


/*
* MokoiMap::Init()
* Init Map for running
*/
bool MokoiMap::Init()
{
	std::cout << " > MokoiMap Init " << this->_name << std::endl;

	lux::screen::display("Loading Map: " + this->_name);
	if ( this->LoadFile() )
	{
		this->active = true;
		lux::display->SetBackgroundObject( this->_background );
		this->_entities->Init();
		this->NetworkMapSwitch();

	}
	else
	{
		lux::engine->FatalError("Map " + this->_name + ".xml can't be loaded");
		this->active = false;
	}

	return this->active;
}

/*
* MokoiMap::Loop()
*
*/
bool MokoiMap::Loop()
{
	if ( !this->_loaded )
	{
		lux::engine->ShowDialog("MokoiMap not loaded\nFile Name: " + this->_name + "\n", DIALOGOK);
		return false;
	}

	if ( this->resetmap )
	{
		this->Close();
		this->LoadFile();
		this->resetmap = false;
	}


	this->NetworkMapLoop();

	lux::display->SetBackgroundObject( this->_background );

	this->_entities->Loop();

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
	else if ( this->_wrap )
	{
		/* Make sure the current screen and the 8 surround screens are loaded
		todo: unload non-active screens
		*/
		screen_number = this->XY2Screen( MAKE_FIXED_INT(this->_position[0]), MAKE_FIXED_INT(this->_position[1]), this->dimension_width );
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
		screen_number = this->XY2Screen( MAKE_FIXED_INT(this->_position[0]), MAKE_FIXED_INT(this->_position[1]), this->dimension_width );
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
* Keeps Map in memory, just onload every thing else.
*/
bool MokoiMap::Close()
{
	std::cout << " < MokoiMap Init " << this->_name << std::endl;

	this->active = false;
	this->_server = false;

	/* Reset Display */
	lux::display->ClearLayers(true);
	lux::display->SetCameraView( 0, 0 );
	lux::display->ResetBackgroundObject( );

	/* Clean up Screens */
	std::map<uint32_t, MokoiMapScreen *>::iterator q = this->_screen.begin();
	for ( ; q != this->_screen.end(); q++ )
	{
		q->second->Close();
	}

	/* Clean Up Map Object */
	std::vector<MapObject *>::iterator p = this->_objects.begin();
	for ( ; p != this->_objects.end(); p++ )
	{
		(*p)->FreeData();
	}

	/* Clean Up Entity */
	this->_entities->Close();

	return true;
}


bool MokoiMap::Reset()
{
	this->resetmap = true;


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

	object->SetData(NULL, object->type);

	if ( object->has_data )
	{
		this->object_cache_count++;
		if ( is_static )
		{
			object->static_map_id = this->object_cache_count;
			this->object_cache[object->static_map_id] = object;
			if ( this->active )
				lux::display->AddObjectToLayer(object->layer, object, true);
			return object->static_map_id;
		}
		else if ( this->active )
		{
			lux::display->AddObjectToLayer(object->layer, object, false);
		}
	}
	return object_id;
}

MapObject * MokoiMap::GetObject( uint32_t ident )
{
	if ( !this->object_cache.empty() )
	{
		std::map<uint32_t,MapObject *>::iterator iter = this->object_cache.find(ident);
		if( iter != this->object_cache.end() )
			return iter->second;
	}
	return NULL;
}

void MokoiMap::ReplaceObjectsSheets( std::string old_sheet, std::string new_sheet )
{
	std::vector< MapObject * >::iterator p;
	for ( p = this->_objects.begin(); p != this->_objects.end(); p++ )
	{
		MapObject * object = (*p);
		if ( object->type == 's' )
		{
			std::string sheet = object->image.substr(0, object->image.find_first_of(':'));
			if ( !old_sheet.compare( sheet ) )
			{
				object->FreeData();
				std::string sprite = object->image.substr(object->image.find_first_of(':'));
				object->image = new_sheet + sprite;
				object->SetData(NULL, 's');
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

		if ( !object->layer_ref )
			delete object;

		return true;
	}
	return false;
}

/* Positions */
void MokoiMap::SetPosition( fixed position[3] )
{
	this->_position[0] = position[0];
	this->_position[1] = position[1];
	this->_position[2] = position[2];

	lux::display->debug_msg << "Wrap: " << (int)this->_wrap << " - Centerview:" << (int)this->_centerview <<  std::endl;
	/*
		Keeps Offset for moving offscreen
	*/

	if ( this->_centerview )
	{
		fixed offset, wrap_offset = 0;

		/* X Axis Wrapping handling */
		if ( this->_wrap == MAP_WRAPXAXIS || this->_wrap == MAP_WRAPBOTH )
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
				this->_position[0] = 0;
			}
			else if ( position[0] > (this->map_width - (this->screen_width / 2)) )
			{
				this->_position[0] = this->map_width - this->screen_width;
			}
			else
			{
				this->_position[0] = position[0] - (this->screen_width/2);
			}
		}
		else if ( this->has_width_different ) // If Screen dimension is maps, then try to account for that
		{
			offset = this->map_width - this->screen_height;
			if ( position[0] > (this->screen_width - offset) )
			{
				this->_position[0] = this->map_width - this->screen_width;
			}
			else if ( position[0] > offset )
			{
				this->_position[0] -= offset;
			}
			else
			{
				this->_position[0] = 0;
			}
		}
		else// No view position change needed.
		{
			this->_position[0] = 0;
		}
		this->_position[0] += wrap_offset;



		wrap_offset = 0;
		if ( this->_wrap == MAP_WRAPBOTH || this->_wrap == MAP_WRAPYAXIS )
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
				this->_position[1] = 0;
			}
			else if ( position[1] > (this->map_height - (this->screen_height / 2)) )
			{
				this->_position[1] = this->map_height - this->screen_height;
			}
			else
			{
				this->_position[1] = position[1] - (this->screen_height / 2);
			}
		}
		else if ( this->has_height_different ) // If Screen dimension is maps, then try to account for that
		{
			offset = this->map_height - this->screen_height;
			if ( position[1] > (this->screen_height - offset) )
			{
				this->_position[1] = this->map_height - this->screen_height;
			}
			else if ( position[1] > offset )
			{
				this->_position[1] -= offset;
			}
			else
			{
				this->_position[1] = 0;
			}
		}
		else // No view position change needed.
		{
			this->_position[1] = 0;
		}
		this->_position[1] += wrap_offset;
	}
	else
	{
		/* Using the top left for position */
		if ( this->_wrap == MAP_WRAPNONE || this->_wrap == MAP_WRAPYAXIS )
		{
			this->_position[0] = clamp(this->_position[0], 0, this->map_width - this->screen_width);
		}
		if ( this->_wrap == MAP_WRAPNONE || this->_wrap == MAP_WRAPXAXIS )
		{
			this->_position[1] = clamp(this->_position[1], 0, this->map_height - this->screen_height);
		}
	}
}

fixed MokoiMap::GetPosition( uint8_t axis )
{
	if ( axis < 3 )
	{
		return this->_position[axis];
	}
	return 0;
}

void MokoiMap::SetScrolling( bool scroll )
{
	this->_centerview = scroll;
}

/* Entities */
EntitySection * MokoiMap::GetEntities()
{
	return this->_entities;
}

/* Save System */
bool MokoiMap::Save( elix::File * current_save_file )
{
	current_save_file->WriteWithLabel("Map Name", this->_name );
	current_save_file->WriteWithLabel("Map ID", this->_ident.value );
	current_save_file->WriteWithLabel("Map Loaded Flag", (uint8_t)this->_loaded );

	if ( this->_entities )
	{
		current_save_file->WriteWithLabel("Map has Entities", (uint8_t)1 );
		this->_entities->Save( current_save_file );
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
		std::map<uint32_t, MapObject *>::iterator iter = this->object_cache.begin();
		while( iter != this->object_cache.end() )
		{
			current_save_file->WriteWithLabel("Map Object ID", iter->first );
			iter->second->Save( current_save_file );
			iter++;
		}
	}
	return true;
}




bool MokoiMap::Restore( elix::File * current_save_file )
{
	uint32_t count = 0;
	uint8_t map_loading_flag = 0;
	uint8_t map_has_entities = 0;
	std::map<uint32_t, MapObject *>::iterator existing_object_iter;

	this->_loaded = false;
	current_save_file->ReadWithLabel("Map Name", &this->_name );
	this->_ident.value= current_save_file->Read_uint32WithLabel("Map ID", true );
	map_loading_flag = current_save_file->Read_uint8WithLabel("Map Loaded Flag");


	this->InitialSetup(this->_name);

	if ( map_loading_flag )
	{
		this->LoadFile();

		/* Marked the cached map objects, to see if we need to remove them, if needed */
		for ( std::map<uint32_t, MapObject *>::iterator iter = this->object_cache.begin(); iter != this->object_cache.end(); iter++ )
		{
			iter->second->can_remove = true;
			iter->second->hidden = true;
		}

		this->_loaded = true;

	}

	map_has_entities = current_save_file->Read_uint8WithLabel("Map has Entities");
	if ( map_has_entities && this->_entities )
	{
		this->_entities->Restore( current_save_file );
	}

	count = current_save_file->Read_uint32WithLabel("Map Display Objects", true );
	if ( count )
	{
		for( uint32_t i = 0; i < count; i++ )
		{
			//if (Lux_GetState() != LOADING )
			//	return false;

			MapObject * existing_object = NULL;
			uint32_t object_id = current_save_file->Read_uint32WithLabel("Map Object ID", true );

			existing_object_iter = this->object_cache.find(object_id);
			if ( existing_object_iter != this->object_cache.end() )
			{
				existing_object = existing_object_iter->second;
				existing_object->can_remove = false;
				existing_object->hidden = false;
			}
			else
			{
				existing_object = new MapObject();
				this->object_cache[object_id] = existing_object;
			}


			existing_object->Restore( current_save_file );
		}


		/* Reove unneed cached map objects. */

		for ( std::map<uint32_t, MokoiMapScreen *>::iterator q = this->_screen.begin(); q != this->_screen.end(); q++ )
		{
			q->second->Close();
			q->second->_objects.erase( std::remove_if(q->second->_objects.begin(), q->second->_objects.end(), RemoveMapObject), q->second->_objects.end() );
		}
		this->_objects.erase( std::remove_if(this->_objects.begin(), this->_objects.end(), RemoveMapObject), this->_objects.end() );


		for ( std::map<uint32_t, MapObject *>::iterator iter = this->object_cache.begin(); iter != this->object_cache.end(); iter++ )
		{
			if ( RemoveMapObject( iter->second ) )
			{
				this->object_cache.erase(iter);
				delete iter->second;
				iter->second = NULL;
				iter--;
			}
		}


	}
	return true;
}


/* Screen */
MokoiMapScreen * MokoiMap::GetScreen(uint32_t screen_number, bool init_new)
{
	if ( this->dimension_width == 1 && this->dimension_height == 1 )
		screen_number = 0;
	if ( screen_number > (this->dimension_width * this->dimension_height)-1 )
		return NULL;
	if ( !this->_screen.empty() )
	{
		std::map<uint32_t, MokoiMapScreen *>::iterator iter = this->_screen.find(screen_number);
		if( iter != this->_screen.end() )
			return iter->second;
	}
	MokoiMapScreen * new_screen = new MokoiMapScreen( Screen2X(screen_number, this->dimension_width), Screen2Y(screen_number, this->dimension_width), MAKE_FIXED_INT(this->default_map_width), MAKE_FIXED_INT(this->default_map_height) );
	this->_screen.insert( std::make_pair( screen_number, new_screen) );
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

uint32_t MokoiMap::XY2Screen(int32_t x, int32_t y, int32_t w)
{
	if ( this->_wrap )
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
/*
* MokoiMap::Valid()
* Todo: Optimise code.
*/
bool MokoiMap::Valid()
{
	if ( !this->_loaded )
	{
		tinyxml2::XMLDocument * xml_file = MokoiGame_GetXML("./maps/" + this->_name + ".xml");
		if ( xml_file->Error() )
		{
//			std::cerr << __FILE__ << ":" << __LINE__ << " | " << xml_file->ErrorDesc() << " Row: " << xml_file->ErrorRow() << std::endl;
			delete xml_file;
			return false;
		}

		if ( !xml_file->RootElement() || strcmp( xml_file->RootElement()->Value(), "map") )
		{
			std::cerr << __FILE__ << ":" << __LINE__ << " | maps/" + this->_name + ".xml not a valid map file." << std::endl;
			delete xml_file;
			return false;
		}

		tinyxml2::XMLElement * dimensions_elem = xml_file->RootElement()->FirstChildElement("settings")->FirstChildElement("dimensions");

		if ( dimensions_elem )
		{
			tinyxml2::QueryUnsignedAttribute( dimensions_elem, "width", this->dimension_width );
			tinyxml2::QueryUnsignedAttribute( dimensions_elem, "height", this->dimension_height );
		}

		this->map_width = MAKE_INT_FIXED( this->dimension_width * this->default_map_width );
		this->map_height = MAKE_INT_FIXED( this->dimension_height * this->default_map_height );
		delete xml_file;
		return true;
	}
	return false;
}

/*
* MokoiMap::LoadDimension()
* Todo: Optimise code.
*/
bool MokoiMap::LoadDimension()
{
	if ( !this->_loaded )
	{
		tinyxml2::XMLDocument * xml_file = MokoiGame_GetXML("./maps/" + this->_name + ".xml");
		if ( xml_file->Error() )
		{
			//std::cerr << __FILE__ << ":" << __LINE__ << " | " << xml_file->ErrorDesc() << " Row: " << xml_file->ErrorRow() << std::endl;
			delete xml_file;
			return false;
		}
		if ( !xml_file->RootElement() || strcmp( xml_file->RootElement()->Value(), "map") )
		{
			std::cerr << __FILE__ << ":" << __LINE__ << " | maps/" + this->_name + ".xml not a valid map file." << std::endl;
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

bool MokoiMap::SaveFile()
{
	lux::screen::display("Saving Map: " + this->_name);
	return false;
}



bool MokoiMap::LoadFile()
{
	lux::screen::display("Loading Map: " + this->_name);

	if ( this->_loaded )
		return this->_loaded;


	MapXMLReader reader;
	std::map<std::string, std::string> settings;

	if ( !reader.Load("./maps/" + this->_name + ".xml"))
	{
		std::cerr << __FILE__ << ":" << __LINE__ << " | maps/" + this->_name + ".xml not a valid map file." << std::endl;
		return false;
	}

	reader.ReadSettings( this, settings );

	this->_wrap = MAP_WRAPNONE;


	this->_entities = new EntitySection( this->_entity, this->_ident.value );

	for ( std::map<std::string, std::string>::iterator cs = settings.begin(); cs != settings.end(); cs++ )
	{
		if ( !cs->first.compare("wrap") )
		{
			this->_wrap = elix::string::ToIntU8(cs->second);
		}
		else if ( !cs->first.compare("centerview") )
		{
			//this->_centerview = !!elix::string::ToIntU16(cs->second);
		}

		if ( this->_entities->parent )
		{
			this->_entities->parent->AddSetting( cs->first, cs->second );
		}
	}

	reader.ReadObjects( this->_objects, this->object_cache_count, this );

	this->_loaded = true;

	/* Set up Map Screen */
	std::vector<MapObject *>::iterator p;
	for ( p = this->_objects.begin(); p != this->_objects.end(); p++ )
	{
		(*p)->SetData(NULL, (*p)->type);
		this->AddObjectToScreens((*p));
	}

	return true;
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
	std::map<uint32_t, MokoiMapScreen *>::iterator q = this->_screen.begin();
	for ( ; q != this->_screen.end(); q++ )
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
	std::map<uint32_t, MokoiMapScreen *>::iterator q = this->_screen.begin();
	for ( ; q != this->_screen.end(); q++ )
	{
		q->second->DrawMask(this->_position);
	}
}

