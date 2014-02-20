/****************************
Copyright © 2006-2011 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "engine.h"
#include "display.h"
#include "elix_string.hpp"
#include "mokoi_game.h"
#include "entity_section.h"
#include "world.h"
#include "game_config.h"
#include "misc_functions.h"
#include <cstdlib>

/* Map ID Limitations */
/*
Sections: 2^12 - 1 = 4095
Each Section has 4096 maps

Map Files: 2^20 - 1 = 1048575

*/

void PrintLuxMapIdent( LuxMapIdent id, std::ostream & stream  )
{
	stream << "Value:" << id.value;
	if ( id.grid.id )
		stream << " [Grid] Section:" << id.grid.id << " Map id: " << id.grid.localmap;
	else
		stream << " [File] Map id:" << id.file.map;

}



WorldSystem::WorldSystem()
{
	lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << " > Opening the World" << std::endl;
	this->active_map = this->_nextmap = NULL;
	this->active_section = this->next_section = NULL;

	this->_entities = new EntitySection("main", 0);

	this->current_position[0] = this->current_position[1] = this->current_position[2] = MAKE_INT_FIXED(1);
	this->next_position[0] = this->next_position[1] = this->next_position[2] = -1;
	this->current_map_ident = 0;
	this->_cachecount = 0;
	this->map_movement_direction = 0;
}

WorldSystem::~WorldSystem()
{
	lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << " < Closing the World" << std::endl;
	if ( this->_objectscache.size() )
	{
		while ( this->_objectscache.begin() != this->_objectscache.end() )
		{
			lux::display->RemoveObject(this->_objectscache.begin()->second->layer, this->_objectscache.begin()->second);
			delete this->_objectscache.begin()->second;
			this->_objectscache.erase( this->_objectscache.begin() );
		}
	}

}

bool WorldSystem::Init( )
{

	/* Map ID */
	this->map_counter.value = 0;

	/* Set up global Enitities */
	if ( !this->_entities->parent->loaded )
	{
		Lux_FatalError("Game requires a Main Script");
		return false;
	}
	//this->_entities->Init();

	/* Get the starting map and check if it's valid */
	std::string starting = lux::config->GetString("map.start");
	if ( starting.length() < 1 )
	{
		Lux_FatalError("No Starting Map. Please set 'map.start'");
		return false;
	}
	this->LoadMap( starting, true, false );

	if ( !this->_nextmap || !this->_nextmap->Valid() )
	{
		Lux_FatalError("No Starting Map");
		return false;
	}
	this->SwitchMap( this->_nextmap, false );


	return true;
}

bool WorldSystem::PreClose()
{
	if ( this->_entities )
		this->_entities->PreClose();
	return true;
}


bool WorldSystem::Close()
{
	this->_collision.clear();



	for( std::map<uint32_t, MokoiMap *>::iterator iter = map_list.begin(); iter != map_list.end(); ++iter )
	{
		delete iter->second;
		iter->second = NULL;
	}
	this->map_list.clear();

	for( std::map<uint32_t, WorldSection *>::iterator iter = section_list.begin(); iter != section_list.end(); ++iter )
	{
		delete iter->second;
	}
	this->section_list.clear();

	if ( this->_entities )
		delete this->_entities;

	return true;
}


void WorldSystem::Print()
{
	for( std::map<uint32_t, MokoiMap *>::iterator iter = this->map_list.begin(); iter != map_list.end(); ++iter )
	{
		LuxMapIdent ident;
		ident.value = iter->first;
		PrintLuxMapIdent( ident, lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR) );
		lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR) << "  " << iter->second->Name() << std::endl;
	}
	for( std::map<uint32_t, WorldSection *>::iterator iter = section_list.begin(); iter != section_list.end(); ++iter )
	{
		lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR) << "Hash:" << iter->first << " " << iter->second->file << std::endl;
	}
}

void WorldSystem::Loop( LuxState engine_state )
{
	if ( this->_nextmap )
	{
		this->SwitchMap( this->_nextmap, true );
		this->_nextmap = NULL;
		if ( !this->active_map->PartOfSection() )
		{
			this->active_section = NULL;
		}
	}

	if ( this->next_section )
	{
		this->active_section = this->next_section;
		this->next_section = NULL;

		this->SetMap( this->next_grid_position[0], this->next_grid_position[1] );
		this->next_position[0] = this->next_position[1] = this->next_position[2] = -1;
		this->SwitchMap( this->_nextmap, false );
		this->_nextmap = NULL;
	}

	if ( !this->active_map )
	{
		Lux_SetState( INVALIDPOSITION );
		return;
	}

	this->current_map_ident = this->active_map->Ident();
	this->active_map->SetPosition( this->current_position );


	lux::display->debug_msg << "Map '" << this->active_map->Name() << "' '"<< this->active_map->Ident() << "'" << std::endl;
	if ( this->active_section != NULL )
	{
		lux::display->debug_msg << "Section '" << this->active_section->file << "' '"<< (int)this->_grid[0] << "'x'"<< (int)this->_grid[1] << "'" << this->active_section->getGrid(this->_grid[0]+1, this->_grid[1]) << std::endl;
	}

	if ( this->active_map->PartOfSection() && this->active_map->_wrap != 3 )
	{
		/* Part of Grid System */
		uint8_t xpos = this->_grid[0];
		uint8_t ypos = this->_grid[1];

		if ( this->active_map->_wrap != MAP_WRAPXAXIS )
		{
			if ( this->current_position[0] < 0 )
			{
				xpos -= 1;
				this->map_movement_direction = 1;
			}
			else if ( this->active_map->map_width < this->current_position[0] )
			{
				xpos += this->active_map->dimension_width;
				this->map_movement_direction = 2;
			}
		}

		if ( this->active_map->_wrap != MAP_WRAPYAXIS )
		{
			if ( this->current_position[1] < 0 )
			{
				ypos -= 1;
				this->map_movement_direction = 3;
			}
			else if ( this->active_map->map_height < this->current_position[1] )
			{
				ypos += this->active_map->dimension_height;
				this->map_movement_direction = 6;
			}
		}

		bool has_moved = false;
		if ( this->_grid[0] != xpos )
		{
			has_moved = true;
		}
		if ( this->_grid[1] != ypos )
		{
			has_moved = true;
		}
		if ( has_moved )
		{
			this->SetMap( xpos, ypos );
		}
	}

	this->_entities->Loop();
	this->active_map->Loop();

}

void WorldSystem::SetPosition( fixed x, fixed y, fixed z, bool change )
{
	this->current_position[0] = x;
	this->current_position[1] = y;
	this->current_position[2] = z;
}

fixed WorldSystem::GetPosition( uint8_t axis, uint8_t to )
{
	if ( axis == 'x')
		return this->current_position[0];
	else if ( axis == 'z')
		return this->current_position[2];
	else
		return this->current_position[1];
}

bool WorldSystem::MovePosition( fixed * position )
{
	bool map_changed = false;
	if ( this->active_map->map_width < position[0] )
	{
		position[0] -= this->active_map->map_width;
		map_changed = true;
	}
	if ( this->active_map->map_height < position[1] )
	{
		position[1] -= this->active_map->map_height;
		map_changed = true;
	}
	return map_changed;
}

/* Map Handling */
MokoiMap * WorldSystem::LoadMap( std::string map_file, bool set_current, bool create )
{
	MokoiMap * new_map = new MokoiMap(map_file);

	if ( create )
	{
		new_map->SaveFile();
	}

	this->map_counter.file.map++;
	new_map->SetFileIdent( this->map_counter.file.map );

	this->map_list.insert( std::make_pair(new_map->Ident(), new_map) );

	if ( set_current )
		this->_nextmap = new_map;

	return new_map;
}


void WorldSystem::SwitchMap( MokoiMap * next_map, bool move_entities )
{
	if ( !next_map )
		return;

	if ( this->active_map )
	{
		this->active_map->Close();

		if ( this->map_movement_direction == 6 )
			this->current_position[1] -= this->active_map->map_height;
		else if ( this->map_movement_direction == 3 )
		{
			this->current_position[1] +=  next_map->map_height;
			this->_grid[0] -= next_map->dimension_height -1;
		}
		else if ( this->map_movement_direction == 2 )
			this->current_position[0] -= this->active_map->map_width;
		else if ( this->map_movement_direction == 1 )
		{
			this->current_position[0] += next_map->map_width;
			this->_grid[0] -= next_map->dimension_width - 1;
		}

		if ( move_entities && this->_entities )
		{
			if ( this->map_movement_direction == 6 )
				this->_entities->Switch( 0, -this->active_map->map_height );
			else if ( this->map_movement_direction == 3 )
				this->_entities->Switch( 0, next_map->map_height );
			else if ( this->map_movement_direction == 2 )
				this->_entities->Switch( -this->active_map->map_width, 0 );
			else if ( this->map_movement_direction == 1 )
				this->_entities->Switch( next_map->map_width, 0 );
			this->map_movement_direction = 0;
		}
	}

	this->active_map = next_map;
	if ( this->next_position[0] != -1)
		this->current_position[0] = this->next_position[0];
	if ( this->next_position[1] != -1)
		this->current_position[1] = this->next_position[1];
	this->active_map->Init();
	this->_nextmap = NULL;

	/* Add Global Display objects */
	if ( this->_objectscache.size() )
	{
		std::map<uint32_t, MapObject *>::iterator p;
		for ( p = this->_objectscache.begin(); p != this->_objectscache.end(); p++ )
		{
			lux::display->AddObjectToLayer(p->second->layer, p->second, true);
		}
	}
}


MokoiMap * WorldSystem::GetMap( uint32_t id )
{
	if ( !id )
		return NULL;
	std::map<uint32_t, MokoiMap *>::iterator p;
	for ( p = this->map_list.begin(); p != this->map_list.end(); p++ )
	{
		if ( id == p->second->Ident() )
		{
			return p->second;
		}
	}
	return NULL;
}

uint32_t WorldSystem::GetMapID( std::string name )
{
	uint32_t ident = 0;

	std::map<uint32_t, MokoiMap *>::iterator p;
	for ( p = this->map_list.begin(); p != this->map_list.end(); p++ )
	{
		if ( !name.compare(  p->second->Name() ) && !p->second->PartOfSection() )
		{
			ident =  p->second->Ident();
			p = this->map_list.end();
			break;
		}
	}
	return ident;
}


bool WorldSystem::SetMap( std::string name, int32_t mapx, int32_t mapy  )
{
	bool unloaded_map = true;
	std::map<uint32_t, MokoiMap *>::iterator p;
	for ( p = this->map_list.begin(); p != this->map_list.end(); p++ )
	{
		if ( !name.compare(  p->second->Name() ) && !p->second->PartOfSection() )
		{
			unloaded_map = false;
			this->_nextmap = p->second;
			this->next_position[0] = mapx;
			this->next_position[1] = mapy;
			this->active_section = this->next_section = NULL;
			return true;
		}
	}
	if ( unloaded_map )
	{
		if ( lux::game->HasFile("./maps/" + name + ".xml") )
		{
			this->LoadMap( name, true, false );
			if ( this->_nextmap )
			{
				this->next_position[0] = mapx;
				this->next_position[1] = mapy;

				if ( !this->_nextmap->PartOfSection() )
					this->active_section = this->next_section = NULL;
			}

			return true;
		}
		else
		{
			lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR, __FILE__ , __LINE__) << " | '" << name << "' map not found." << std::endl;
		}
	}
	return false;
}

bool WorldSystem::SetMap( uint32_t id, int32_t mapx, int32_t mapy  )
{
	std::map<uint32_t, MokoiMap *>::iterator p = this->map_list.find( id );

	if ( p != this->map_list.end() )
	{
		this->_nextmap = p->second;
		this->next_section = NULL;
		this->next_position[0] = mapx;
		this->next_position[1] = mapy;
		return true;
	}
	else
	{
		return false;
	}
}


/* Section System */
WorldSection * WorldSystem::LoadSection( std::string name, bool set_current )
{
	WorldSection * new_section = NULL;

	if ( lux::game->HasFile("./sections/" + name + ".txt") )
	{

		new_section = new WorldSection( name, this->map_counter.grid.id, 64, 64 );
		this->map_counter.grid.id++;
		this->section_list.insert( std::make_pair( elix::string::Hash(name), new_section ) );

		if ( set_current )
			this->next_section = new_section;
	}
	return new_section;
}

WorldSection * WorldSystem::GetSection( std::string name )
{
	WorldSection * requested_section = NULL;
	if ( name.length() && this->section_list.size() )
	{
		uint32_t hash = elix::string::Hash(name);
		std::map<uint32_t,WorldSection *>::iterator iter = this->section_list.find(hash);
		if ( iter != this->section_list.end() )
			requested_section = iter->second;
	}

	return requested_section;
}



uint32_t WorldSystem::SwitchSection( std::string name, uint8_t gridx, uint8_t gridy )
{
	if ( !name.length() )
		return 0;

	if ( this->section_list.size() )
	{
		uint32_t hash = elix::string::Hash(name);
		std::map<uint32_t,WorldSection *>::iterator iter = this->section_list.find(hash);
		if( iter != this->section_list.end() )
			this->next_section = iter->second;
		else
			this->LoadSection(name, true);
	}
	else
	{
		this->LoadSection(name, true);
	}


	if ( this->next_section )
	{
		this->next_grid_position[0] = gridx;
		this->next_grid_position[1] = gridy;
	}

	return ( this->next_section == NULL ? 0 : 1);
}

void WorldSystem::DeleteSection( uint32_t section )
{

	std::map<uint32_t, MokoiMap *>::iterator p = this->map_list.begin();
	while( p != this->map_list.end() )
	{
		if ( p->second->InSection(section) && !p->second->_keep )
		{
			lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << " Section[" << std::dec << section << "]: " << p->second->_name  << " deleted" << std::endl;
			delete p->second;
			this->map_list.erase(p++);
		}
		else
		{
			++p;
		}
	}

}


bool WorldSystem::SetMap( uint8_t grid_x, uint8_t grid_y )
{
	if ( !this->active_section )
		return false;

	uint16_t grid_map = this->active_section->getGrid(grid_x, grid_y);
	if ( grid_map != 0xFFFF )
	{
		uint32_t map_id = this->active_section->getMapID(grid_map);
		this->_nextmap = this->map_list[map_id];
		this->_grid[0] = grid_x;
		this->_grid[1] = grid_y;
		return true;
	}
	this->_nextmap = NULL;
	return false;
}

bool WorldSystem::GetMapGrid( uint8_t & gridx, uint8_t & gridy )
{
	if ( !this->active_section )
		return false;

	gridx = this->_grid[0];
	gridy = this->_grid[1];
	return true;
}

uint16_t WorldSystem::GetMapID( std::string section_name, uint8_t grid_x, uint8_t grid_y )
{
	if ( section_name.length() )
	{
		WorldSection * section = NULL;
		uint32_t hash = elix::string::Hash(section_name);
		if ( this->section_list.size() )
		{
			std::map<uint32_t,WorldSection *>::iterator iter = this->section_list.find(hash);
			if( iter != this->section_list.end() )
				section = iter->second;
		}


		if ( section )
			return section->getGrid(grid_x, grid_y);
	}
	else if ( this->active_section )
		return this->active_section->getGrid(grid_x, grid_y);

	return 0xFFFF;
}





/* Object Handling */
uint32_t WorldSystem::AddObject(MapObject * object, bool is_static)
{
	object->SetData(NULL, object->type);
	if ( !object->has_data )
	{
		return 0;
	}
	object->global = true;
	if ( is_static )
	{
		this->_cachecount++;
		object->static_map_id = this->_cachecount;
		this->_objectscache[object->static_map_id] = object;
		lux::display->AddObjectToLayer(object->layer, object, true);
		return object->static_map_id;
	}
	else
	{
		lux::display->AddObjectToLayer(object->layer, object, false);
	}

	return 0;
}

MapObject * WorldSystem::GetObject( uint32_t ident )
{
	if ( this->_objectscache.size() )
	{
		std::map<uint32_t,MapObject *>::iterator iter = this->_objectscache.find(ident);
		if( iter != this->_objectscache.end() )
			return iter->second;
	}
	return NULL;
}

void WorldSystem::ClearObjects()
{
	std::map<uint32_t, MapObject*>::iterator l_object;
	if ( this->_objectscache.size() )
	{
		for ( l_object = this->_objectscache.begin(); l_object != this->_objectscache.end(); l_object++ )
		{
			l_object->second->FreeData();
			this->_objectscache.erase(l_object);
			delete l_object->second;
		}
	}
}

bool WorldSystem::RemoveObject(uint32_t ident)
{
	MapObject * object = this->GetObject(ident);
	if ( object != NULL )
	{
		if ( lux::display->RemoveObject(object->layer, object) )
		{
			object->FreeData();
			this->_objectscache.erase(ident);
			delete object;
			return true;
		}
	}
	return false;
}

/* Colllision Functions */
void WorldSystem::AddCollision( uint32_t name, CollisionObject * rect)
{
	if ( !rect->added )
	{
		rect->added = true;
		this->_collision.insert(std::make_pair(name,rect));
	}
}

void WorldSystem::CheckCollision( uint32_t name, CollisionObject *rect)
{

}

void WorldSystem::DrawCollisions()
{
	if ( this->_collision.size() )
	{
		LuxRect screen_location;
		ObjectEffect effects;

		std::multimap<uint32_t, CollisionObject*>::iterator p;
		for( p = this->_collision.begin(); p != this->_collision.end(); p++ )
		{
			if ( p->second->rect.w != 0 && p->second->rect.h != 0 )
			{
				screen_location = p->second->rect;
				screen_location.z = 6999;
				screen_location.x -= MAKE_FIXED_INT(this->active_map->_position[0]);
				screen_location.y -= MAKE_FIXED_INT(this->active_map->_position[1]);
				effects.primary_colour.r = (p->second->type %5) * 128;
				effects.primary_colour.b = (p->second->type /5) * 64;
				effects.primary_colour.g = p->second->type  * 25;
				effects.primary_colour.a = 128;
				lux::display->graphics.DrawRect(screen_location, effects);
				screen_location.y -= 16;
			}
		}
	}
}

void WorldSystem::ClearCollisions()
{
	this->_collision.clear();
}

void WorldSystem::ReturnCollisions( std::vector<CollisionResult*> * hits, uint32_t entity, int16_t count, LuxRect rect )
{
	if ( this->_collision.size() )
	{
		std::multimap<uint32_t, CollisionObject*>::iterator p;
		for ( p = this->_collision.begin(); p != this->_collision.end(); p++ )
		{
			if ( p->first != entity )
			{
				if ( p->second->rect.w != 0 && p->second->rect.h != 0 )
				{
					if ( Lux_Util_RectCollide(p->second->rect, rect) )
					{
						CollisionResult * result = new CollisionResult;
						result->rect = count;
						result->rect_x = rect.x + (rect.w/2);
						result->rect_y = rect.y + (rect.h/2);
						result->type = p->second->type;
						result->hit_rect = p->second->id;
						result->hit_x = p->second->rect.x + (p->second->rect.w/2);
						result->hit_y = p->second->rect.y + (p->second->rect.h/2);
						result->entity_id = p->second->entity;
						hits->push_back(result);
					}
				}
			}
		}
	}
}

void WorldSystem::RemoveCollisions( uint32_t name )
{
	this->_collision.erase(name);
}



/* Save System */
bool WorldSystem::Save( elix::File *current_save_file )
{
	uint32_t count = 0;

	/* Save Global Entity Section */
	this->_entities->Save( current_save_file );

	/* Save Sections */
	count = this->section_list.size();
	current_save_file->WriteWithLabel( "Sections", count );
	if ( count )
	{
		std::map<uint32_t, WorldSection *>::iterator iter = this->section_list.begin();
		while( iter != this->section_list.end() )
		{
			iter->second->Save( current_save_file );
			iter++;
		}
	}

	/* Save Maps */
	count = this->map_list.size();
	current_save_file->WriteWithLabel( "Maps", count );
	if ( count )
	{
		std::map<uint32_t, MokoiMap *>::iterator iter = this->map_list.begin();
		while( iter != this->map_list.end() )
		{
			current_save_file->WriteWithLabel( "Map ID", iter->first );
			iter->second->Save( current_save_file );
			iter++;
		}
	}

	/* Save Global Display Object */
	count = this->_objectscache.size();
	current_save_file->WriteWithLabel( "Global Display Object", count );
	if ( count )
	{
		std::map<uint32_t, MapObject *>::iterator iter = this->_objectscache.begin();
		while( iter != this->_objectscache.end() )
		{
			current_save_file->WriteWithLabel( "Global Object ID", iter->first );
			iter->second->Save( current_save_file );
			iter++;
		}
	}

	/* Save Position and Section */
	std::string current_section_name = "";

	if ( this->active_section )
	{
		current_section_name = this->active_section->file;
	}

	current_save_file->WriteWithLabel( "Current Map", this->active_map->Ident() ); //Current Map ID
	current_save_file->WriteWithLabel( "Current Section", current_section_name );
	current_save_file->WriteWithLabel( "Current X", (uint32_t)this->active_map->GetPosition(0) );
	current_save_file->WriteWithLabel( "Current Y", (uint32_t)this->active_map->GetPosition(1) );
	current_save_file->WriteWithLabel( "Current Z", (uint32_t)this->active_map->GetPosition(2) );
	return true;
}

bool WorldSystem::Restore( elix::File * current_save_file )
{
	uint32_t i = 0;
	uint32_t count = 0;
	uint32_t display_object_id = 0, restore_map_id = 0, active_map_id = 0;
	std::string active_section_name;

	this->map_counter.file.unused = 0;
	this->map_counter.file.map = 1;
	this->ClearObjects();

	if ( !this->_entities->parent->loaded )
	{
		Lux_FatalError("Game requires a Main Script");
		return false;
	}
	this->_entities->Init();

	/* Load Global Entity Section */
	this->_entities->Restore( current_save_file );

	/* Load Sections */
	count = current_save_file->Read_uint32WithLabel( "Sections", true );
	if ( count )
	{
		WorldSection * new_section = NULL;
		for( i = 0; i < count; i++ )
		{
			if (Lux_GetState() != LOADING )
				return false;

			new_section = new WorldSection( 64, 64 );
			new_section->Restore( current_save_file );

			this->map_counter.grid.id = std::max( this->map_counter.grid.id, new_section->_id );

			this->section_list.insert( std::make_pair( elix::string::Hash(new_section->file), new_section ) );
		}
	}

	/* Load Maps */
	count = current_save_file->Read_uint32WithLabel( "Maps", true );
	if ( count )
	{
		MokoiMap * restoring_map = NULL;
		for( i = 0; i < count; i++ )
		{
			if (Lux_GetState() != LOADING )
				return false;

			restore_map_id = current_save_file->Read_uint32WithLabel( "Map ID", true );

			restoring_map = this->GetMap(restore_map_id);
			if ( !restoring_map )
			{
				restoring_map = new MokoiMap(current_save_file);
				restoring_map->SetIdent(restore_map_id);
				lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << "Map not already loaded " << restore_map_id << std::endl;
			}
			else
			{
				lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << "Map already has been loaded " << restore_map_id << std::endl;
				restoring_map->Restore( current_save_file );
			}

			this->map_list[restore_map_id] = restoring_map;
			this->map_counter.file.map = std::max( this->map_counter.file.map, restoring_map->FileIdent() );
		}
	}

	if (Lux_GetState() != LOADING )
		return false;

	/* Load Global Display Object */
	count = current_save_file->Read_uint32WithLabel( "Global Display Object",  true );
	if ( count )
	{
		for( i = 0; i < count; i++)
		{
			if (Lux_GetState() != LOADING )
				return false;
			display_object_id = current_save_file->Read_uint32WithLabel( "Global Object ID", true );
			this->_objectscache[display_object_id] = new MapObject();
			this->_objectscache[display_object_id]->Restore( current_save_file );
			lux::display->AddObjectToLayer(this->_objectscache[display_object_id]->layer, this->_objectscache[display_object_id], true); //???
		}
	}

	this->Print();

	/* Load Position and Section */
	active_map_id = current_save_file->Read_uint32WithLabel( "Current Map", true ); //Current Map ID
	current_save_file->ReadWithLabel( "Current Section", &active_section_name );
	this->current_position[0] = (fixed)current_save_file->Read_uint32WithLabel( "Current X", true );
	this->current_position[1] = (fixed)current_save_file->Read_uint32WithLabel( "Current Y", true );
	this->current_position[2] = (fixed)current_save_file->Read_uint32WithLabel( "Current Z", true );

	this->active_section = this->GetSection(active_section_name);
	this->next_section = NULL;



	this->_nextmap =  this->active_map = NULL;


	std::map<uint32_t, MokoiMap *>::iterator p = this->map_list.find( active_map_id );
	if ( p != this->map_list.end() )
	{
		this->SwitchMap( (*p).second, false );
		//this->map = (*p).second;
		//this->map->Init();
		//this->map->SetPosition( this->_position );
		//this->_nextmap = this->map;
		return true;
	}
	return false;

}
