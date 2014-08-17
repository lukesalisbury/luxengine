/****************************
Copyright © 2006-2014 Luke Salisbury
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

/*
void PrintLuxMapIdent( LuxMapIdent id, std::ostream & stream  )
{
	stream << "[Grid] Section:" << id.grid.section << " Map id: " << id.grid.map;
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

*/

GameWorldSystem::GameWorldSystem()
{
	this->active_map = this->next_map = NULL;
	this->active_section = this->next_section = NULL;

	this->current_offset_position[0] = this->current_offset_position[1] = this->current_offset_position[2] = MAKE_INT_FIXED(1);
	this->next_offset_position[0] = this->next_offset_position[1] = this->next_offset_position[2] = -1;
	this->object_cache_count = 0;
	this->map_movement_direction = 0;


	this->map_counter.grid.section = 1; /* Section counter */

	this->global_entities = new EntitySection("main", 0);

}

GameWorldSystem::~GameWorldSystem()
{
	NULLIFY( this->global_entities );

}

bool GameWorldSystem::Init( )
{
	lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << " > Opening the World" << std::endl;

	/* Set up global Enitities */
	if ( !this->global_entities->parent->loaded )
	{
		Lux_FatalError("Game requires a Main Script");
		return false;
	}

	/* Get the starting map and check if it's valid */
	std::string starting = lux::config->GetString("map.start");
	if ( starting.length() < 1 )
	{
		starting = "0";
	}

	this->next_grid_position[0] =  this->next_grid_position[1] = 0;
	this->next_section = this->NewSection( starting, 1, 1 );
	this->next_section->AddMap( starting, 0, 0 );

	this->SwitchActive();

	if ( !this->active_map || !this->active_map->Valid() )
	{
		Lux_FatalError("No Starting Map. Please set 'map.start' setting. ");
		return false;
	}

	return true;
}

void GameWorldSystem::Loop( LuxState engine_state )
{
	this->SwitchActive();


	if ( !this->active_map )
	{
		Lux_SetState( INVALIDPOSITION );
		return;
	}

	this->active_map->SetPosition( this->current_offset_position );


	lux::display->debug_msg << "Map '" << this->active_map->Name() << "' '"<< this->active_map->Ident() << "'" << std::endl;
	if ( this->active_section != NULL )
	{
		lux::display->debug_msg << "Section '" << this->active_section->Name() << "' '"<< (int)this->grid_position[0] << "'x'"<< (int)this->grid_position[1] << "'" << std::endl;
	}

	if ( this->active_section->IsBasic() && this->active_map->wrap_mode != 3 )
	{
		this->CheckPosition();
	}

	this->global_entities->Loop();
	this->active_map->Loop();




/*

*/
	std::stringstream buffer;

	for( std::map<uint32_t, WorldSection *>::iterator iter = section_list.begin(); iter != section_list.end(); ++iter )
	{
		WorldSection * section = iter->second;
		buffer << section->Name() << std::endl;
		for( std::map<uint32_t, MokoiMap *>::iterator iter2 = this->map_list.begin(); iter2 != map_list.end(); ++iter2 )
		{
			MokoiMap * map = iter2->second;

			if ( map->InSection( section->Ident() ) )
			{
				buffer << " - " <<  map->Name() << " [Grid] Section:" << map->GridIdent() << " id: " << map->Ident() << std::endl;
			}
		}
	}

	lux::display->graphics.DrawMessage( buffer.str(), 3 );

}

bool GameWorldSystem::Close()
{
	lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << " < Closing the World" << std::endl;

	this->global_entities->Close();

	for( std::map<uint32_t, WorldSection *>::iterator iter = section_list.begin(); iter != section_list.end(); ++iter )
	{
		delete iter->second;
	}

	for( std::map<uint32_t, MokoiMap *>::iterator iter = map_list.begin(); iter != map_list.end(); ++iter )
	{
		delete iter->second;
	}

	if ( this->object_cache.size() )
	{
		while ( this->object_cache.begin() != this->object_cache.end() )
		{
			MapObject * object = (MapObject *)this->object_cache.begin()->second;
			lux::display->RemoveObject( object->layer, object );
			delete object;

			this->object_cache.erase( this->object_cache.begin() );
		}
	}

	this->collision_list.clear();
	this->map_list.clear();
	this->section_list.clear();

	return true;
}



/* Positions */
/**
 * @brief GameWorldSystem::SetPosition
 * @param offset_x
 * @param offset_y
 * @param offset_z
 */
void GameWorldSystem::SetPosition(fixed offset_x, fixed offset_y, fixed offset_z )
{
	this->current_offset_position[0] = offset_x;
	this->current_offset_position[1] = offset_y;
	this->current_offset_position[2] = offset_z;
}
/**
 * @brief GameWorldSystem::GetPosition
 * @param axis
 * @return
 */
fixed GameWorldSystem::GetPosition( uint8_t axis )
{
	if ( axis == 'x')
		return this->current_offset_position[0];
	else if ( axis == 'y')
		return this->current_offset_position[1];
	else
		return this->current_offset_position[2];
}

/**
 * @brief Check to see if offset moves off map.
 */
void GameWorldSystem::CheckPosition()
{
	uint8_t updated_grid_position[2];

	updated_grid_position[0] = this->grid_position[0];
	updated_grid_position[1] = this->grid_position[1];

	/* Checks to see if map doesn't wraps around */
	if ( this->active_map->wrap_mode != MAP_WRAPXAXIS )
	{
		if ( this->current_offset_position[0] < 0 )
		{
			updated_grid_position[0] -= 1;
			this->map_movement_direction = 1;
		}
		else if ( this->active_map->map_width < this->current_offset_position[0] )
		{
			updated_grid_position[0] += this->active_map->dimension_width;
			this->map_movement_direction = 2;
		}
	}

	if ( this->active_map->wrap_mode != MAP_WRAPYAXIS )
	{
		if ( this->current_offset_position[1] < 0 )
		{
			updated_grid_position[1] -= 1;
			this->map_movement_direction = 3;
		}
		else if ( this->active_map->map_height < this->current_offset_position[1] )
		{
			updated_grid_position[1] += this->active_map->dimension_height;
			this->map_movement_direction = 6;
		}
	}

	/* Update Grid Location */
	bool has_moved = false;
	if ( this->grid_position[0] != updated_grid_position[0] )
	{
		has_moved = true;
	}
	if ( this->grid_position[1] != updated_grid_position[1] )
	{
		has_moved = true;
	}

	if ( has_moved )
	{
		this->SetMap( this->active_section, updated_grid_position[0], updated_grid_position[1], -1, -1, -1 );
	}
}


/* Position Movement */
/**
 * @brief Switch Active Sections and map.
 */
void GameWorldSystem::SwitchActive()
{
	if ( this->next_section )
	{
		if ( this->next_map == NULL )
		{
			this->next_map = this->next_section->GetMap( this->next_grid_position[0], this->next_grid_position[1] );
			this->grid_position[0] = this->next_grid_position[0];
			this->grid_position[1] = this->next_grid_position[1];
		}

		std::cout << "[Switch Active] " << this->next_section->Name() << "  " << this->next_map->Name() << std::endl;

		if ( this->SwitchMap( this->next_map ) )
		{
			this->active_section = this->next_section;
			this->next_section = NULL;
			this->next_map = NULL;
		}
	}
}

/**
 * @brief GameWorldSystem::SwitchMap
 * @param next_map
 * @return
 */
bool GameWorldSystem::SwitchMap( MokoiMap * next_map )
{
	if ( !next_map )
		return false;

	/* Close current map, move global entities if needed */
	if ( this->active_map )
	{
		this->active_map->Close();

		if ( this->map_movement_direction == 6 )
		{
			this->current_offset_position[1] -= this->active_map->map_height;

			if ( this->global_entities )
				this->global_entities->Switch( 0, -this->active_map->map_height );
		}
		else if ( this->map_movement_direction == 3 )
		{
			this->current_offset_position[1] +=  next_map->map_height;
			this->grid_position[0] -= next_map->dimension_height -1;

			if ( this->global_entities )
				this->global_entities->Switch( 0, next_map->map_height );
		}
		else if ( this->map_movement_direction == 2 )
		{
			this->current_offset_position[0] -= this->active_map->map_width;

			if ( this->global_entities )
				this->global_entities->Switch( -this->active_map->map_width, 0 );
		}
		else if ( this->map_movement_direction == 1 )
		{
			this->current_offset_position[0] += next_map->map_width;
			this->grid_position[0] -= next_map->dimension_width - 1;

			if ( this->global_entities )
				this->global_entities->Switch( next_map->map_width, 0 );
		}
		this->map_movement_direction = 0;

	}

	/* Set new Map to active and call it's Init */
	this->active_map = next_map;

	if ( this->next_offset_position[0] != -1 )
		this->current_offset_position[0] = this->next_offset_position[0];
	if ( this->next_offset_position[1] != -1 )
		this->current_offset_position[1] = this->next_offset_position[1];
	if ( this->next_offset_position[2] != -1 )
		this->current_offset_position[2] = this->next_offset_position[2];
	this->active_map->Init();

	/* Add Global Display objects */
	if ( this->object_cache.size() )
	{
		std::map<uint32_t, MapObject *>::iterator p;
		for ( p = this->object_cache.begin(); p != this->object_cache.end(); p++ )
		{
			lux::display->AddObjectToLayer(p->second->layer, p->second, true);
		}
	}

	return true;
}


/**
 * @brief Tells the engine to switch to new map based on x/y of a WorldSection
 * @param section
 * @param grid_x
 * @param grid_y
 * @return
 */
uint32_t GameWorldSystem::SetMap( WorldSection * section, uint8_t grid_x, uint8_t grid_y, fixed position_x, fixed position_y, fixed position_z  )
{
	if ( section )
	{
		this->next_section = section;

		this->next_grid_position[0] = grid_x;
		this->next_grid_position[1] = grid_y;

		this->next_map = this->next_section->GetMap( this->next_grid_position[0], this->next_grid_position[1] );

		this->next_offset_position[0] = position_x;
		this->next_offset_position[1] = position_y;
		this->next_offset_position[2] = position_z;

	}

	return ( this->next_map == NULL ? 0 : this->next_map->Ident() );

}

/**
 * @brief Tells the engine to switch to new map based on x/y of a WorldSection
 * @param section_name Name of Section
 * @param grid_x
 * @param grid_y
 * @return
 */
uint32_t GameWorldSystem::SetMap( std::string section_name, uint8_t grid_x, uint8_t grid_y, fixed position_x, fixed position_y, fixed position_z  )
{
	if ( !section_name.length() )
		return 0;

	WorldSection * section = this->GetSection( section_name, true );

	return this->SetMap( section, grid_x, grid_y, position_x, position_y, position_z );
}

/**
 * @brief GameWorldSystem::SetMap
 * @param section_hash
 * @param grid_x
 * @param grid_y
 * @return
 */
uint32_t GameWorldSystem::SetMap( uint32_t section_hash, uint8_t grid_x, uint8_t grid_y, fixed position_x, fixed position_y, fixed position_z  )
{
	WorldSection * section = this->GetSection( section_hash );

	return this->SetMap( section, grid_x, grid_y,  position_x, position_y, position_z );
}

/**
 * @brief GameWorldSystem::SetMap
 * @param map
 * @param position_x
 * @param position_y
 * @param position_z
 * @return
 */
uint32_t GameWorldSystem::SetMap( MokoiMap * map, fixed position_x, fixed position_y, fixed position_z )
{
	if ( map )
	{
		this->next_section = this->GetSection( map );

		this->next_grid_position[0] = map->GetGrid('x');
		this->next_grid_position[1] = map->GetGrid('y');

		this->next_map = map;

		this->next_offset_position[0] = position_x;
		this->next_offset_position[1] = position_y;
		this->next_offset_position[2] = position_z;

		if ( this->next_map && this->next_section )
			std::cout << "[Next Active] " << this->next_section->Name() << "  " << this->next_map->Name() << std::endl;
	}

	return ( this->next_map == NULL ? 0 : this->next_map->Ident() );

}



/*
bool GameWorldSystem::SetMap(std::string map_file, int32_t position_x, int32_t position_y  )
{
	bool unloaded_map = true;
	std::map<uint32_t, MokoiMap *>::iterator p;
	for ( p = this->map_list.begin(); p != this->map_list.end(); p++ )
	{
		if ( !map_file.compare(  p->second->Name() ) && !p->second->PartOfSection() )
		{
			unloaded_map = false;
			this->next_map = p->second;
			this->next_offset_position[0] = position_x;
			this->next_offset_position[1] = position_y;
			this->active_section = this->next_section = NULL;
			return true;
		}
	}
	if ( unloaded_map )
	{
		if ( lux::game_data->HasFile("./maps/" + map_file + ".xml") )
		{
			this->LoadMap( name, true, false );
			if ( this->next_map )
			{
				this->next_offset_position[0] = position_x;
				this->next_offset_position[1] = position_y;

				if ( !this->next_map->PartOfSection() )
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

bool GameWorldSystem::SetMap(uint32_t id )
{
	std::map<uint32_t, MokoiMap *>::iterator p = this->map_list.find( id );

	if ( p != this->map_list.end() )
	{
		this->next_map = p->second;
		this->next_section = NULL;
		this->next_offset_position[0] = position_x;
		this->next_offset_position[1] = position_y;
		return true;
	}
	else
	{
		return false;
	}
}
*/


/* Map Handling */
bool GameWorldSystem::InsertMap( uint32_t ident, MokoiMap * map )
{
	std::map<uint32_t, MokoiMap *>::iterator p = this->map_list.find( ident );

	if ( p == this->map_list.end() )
	{
		this->map_list.insert( std::pair<uint32_t, MokoiMap *>( ident, map ) );
		return true;
	}
	return false;
}

/**
 * @brief GameWorldSystem::CreateMap
 * @param map_name
 * @param removeable
 * @param editable
 * @param width
 * @param height
 * @return
 */
MokoiMap * GameWorldSystem::CreateMap( std::string map_name, bool removeable, bool editable, uint32_t width, uint32_t height )
{
	WorldSection * section = this->NewSection( "map/"+map_name, 1, 1);
	MokoiMap * new_map = NULL;

	if ( width && height )
	{
		new_map = new MokoiMap( map_name, width, height );
	}
	else
	{
		new_map = new MokoiMap( map_name );
	}

	if ( new_map )
	{
		if ( section->AddMap( new_map, 0, 0) )
		{
			new_map->SetOption( 0, editable );
			new_map->SetOption( 1, !removeable );
		}
	}
	return new_map;
}




/**
 * @brief GameWorldSystem::GetMap
 * @param id
 * @return
 */
MokoiMap * GameWorldSystem::GetMap( uint32_t id )
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

/**
 * @brief GameWorldSystem::DeleteMap
 * @param ident
 * @return
 */
bool GameWorldSystem::DeleteMap( uint32_t ident )
{
	std::map<uint32_t, MokoiMap *>::iterator p = this->map_list.find( ident );

	if ( p != this->map_list.end() )
	{
		if ( p->second->IsDeletable() )
		{
			delete p->second;
			this->map_list.erase(p);
			return true;
		}
	}
	return false;
}

/**
 * @brief GameWorldSystem::FindMap
 * @param map_file
 * @return
 */
MokoiMap * GameWorldSystem::FindMap( std::string map_file )
{
	MokoiMap * map = NULL;

	std::map<uint32_t, MokoiMap *>::iterator p;
	for ( p = this->map_list.begin(); p != this->map_list.end(); p++ )
	{
		if ( map_file.compare(  p->second->Name() ) == 0 )
		{
			map =  p->second;
			p = this->map_list.end();
			break;
		}
	}
	return map;

}

/**
 * @brief GameWorldSystem::GetMapID
 * @param name
 * @return
 */
uint32_t GameWorldSystem::GetMapID( std::string name )
{
	MokoiMap * map = NULL;

	map = this->FindMap(name);

	if ( map )
		return map->Ident();
	else
		return 0;
}

/* Section System */
/**
 * @brief GameWorldSystem::NewSection
 * @param section_name
 * @param width
 * @param height
 * @return
 */
WorldSection * GameWorldSystem::NewSection( std::string section_name, const uint8_t width, const uint8_t height )
{
	WorldSection * new_section = new WorldSection( this->map_counter, section_name, width, height );

	this->section_list.insert( std::make_pair( elix::string::Hash(section_name), new_section ) );

	this->IncreaseSectionCounter();

	return new_section;
}

/**
 * @brief GameWorldSystem::LoadSection
 * @param name
 * @param set_current
 * @return
 */
WorldSection * GameWorldSystem::LoadSection( std::string name, bool set_current )
{
	WorldSection * new_section = NULL;

	if ( lux::game_data->HasFile("./worlds/" + name + ".txt") )
	{
		new_section = this->NewSection( name, 64, 64 );

		if ( set_current )
			this->next_section = new_section;
	}
	return new_section;
}

/**
 * @brief GameWorldSystem::GetSection
 * @param section_name
 * @param load
 * @return
 */
WorldSection * GameWorldSystem::GetSection( std::string section_name, bool load )
{
	WorldSection * requested_section = NULL;
	if ( section_name.length() && this->section_list.size() )
	{
		uint32_t hash = elix::string::Hash( section_name );
		std::map<uint32_t, WorldSection *>::iterator iter = this->section_list.find(hash);
		if ( iter != this->section_list.end() )
			requested_section = iter->second;
	}

	if ( requested_section == NULL && load == true )
	{
		requested_section = this->LoadSection(section_name, false);
	}

	return requested_section;
}

/**
 * @brief GameWorldSystem::GetSection
 * @param map
 * @return
 */
WorldSection * GameWorldSystem::GetSection( MokoiMap * map )
{
	WorldSection * requested_section = NULL;

	if ( map != NULL )
	{
		if ( this->section_list.size() )
		{
			std::map<uint32_t, WorldSection *>::iterator p;
			std::cout << "MokoiMap" << " - " << map->Name() << ":" << std::hex << map->GridIdent() << "|" << map->Ident() << std::endl;
			for ( p = this->section_list.begin(); p != this->section_list.end(); p++ )
			{
				if ( map->InSection( p->second->Ident() ) )
				{
					requested_section = p->second;
					break;
				}
			}
		}
	}
	return requested_section;
}

/**
 * @brief GameWorldSystem::GetSection
 * @param section_hash
 * @return
 */

WorldSection * GameWorldSystem::GetSection( uint32_t section_hash )
{
	// if section_hash
	WorldSection * requested_section = NULL;
	if ( section_hash == 0 && this->section_list.size() )
	{
		requested_section = this->active_section;
	}
	else if ( this->section_list.size() )
	{
		std::map<uint32_t,WorldSection *>::iterator iter = this->section_list.find(section_hash);
		if ( iter != this->section_list.end() )
			requested_section = iter->second;
	}

	return requested_section;
}

/**
 * @brief GameWorldSystem::DeleteSection
 * @param ident
 */
void GameWorldSystem::DeleteSection(uint32_t ident )
{

	std::map<uint32_t, MokoiMap *>::iterator p = this->map_list.begin();
	while( p != this->map_list.end() )
	{
		MokoiMap * map = p->second;
		if ( map->InSection(ident) && !map->keep_memory )
		{
			lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << " Section[" << std::dec << ident << "]: " << map->Name()  << " deleted" << std::endl;
			delete map;
			this->map_list.erase(p++);
		}
		else
		{
			++p;
		}
	}

}

/**
 * @brief GameWorldSystem::SetSectionGrid
 * @param grid_x
 * @param grid_y
 * @return
 */
bool GameWorldSystem::SetSectionGrid( uint8_t grid_x, uint8_t grid_y )
{
	if ( !this->active_section )
		return false;

	uint16_t grid_map = this->active_section->GetGridID( grid_x, grid_y );
	if ( grid_map != 0xFFFF )
	{
		this->next_map = this->active_section->GetMap( grid_map );
		this->grid_position[0] = grid_x;
		this->grid_position[1] = grid_y;
		return true;
	}
	this->next_map = NULL;
	return false;
}

/**
 * @brief GameWorldSystem::GetSectionGrid
 * @param gridx
 * @param gridy
 * @return
 */
bool GameWorldSystem::GetSectionGrid( uint8_t & gridx, uint8_t & gridy )
{
	if ( !this->active_section )
		return false;

	gridx = this->grid_position[0];
	gridy = this->grid_position[1];
	return true;
}

/**
 * @brief GameWorldSystem::GetMapID
 * @param section_name
 * @param grid_x
 * @param grid_y
 * @return
 */
uint32_t GameWorldSystem::GetMapID( std::string section_name, uint8_t grid_x, uint8_t grid_y )
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
			return section->GetGridID(grid_x, grid_y);
	}
	else if ( this->active_section )
		return this->active_section->GetGridID(grid_x, grid_y);

	return 0xFFFF;
}

/* Object Handling */
/**
 * @brief GameWorldSystem::AddObject
 * @param object
 * @param is_static
 * @return
 */
uint32_t GameWorldSystem::AddObject(MapObject * object, bool is_static)
{
	object->SetData(object->type);
	if ( !object->has_data )
	{
		return 0;
	}

	if ( is_static )
	{
		this->object_cache_count++;
		object->SetStaticMapID( this->object_cache_count, true );
		this->object_cache[object->static_map_id] = object;

		lux::display->AddObjectToLayer(object->layer, object, true);
		return object->GetStaticMapID();
	}
	else
	{
		lux::display->AddObjectToLayer(object->layer, object, false);
	}

	return 0;
}

/**
 * @brief GameWorldSystem::GetObject
 * @param ident
 * @return
 */
MapObject * GameWorldSystem::GetObject( uint32_t ident )
{
	if ( this->object_cache.size() )
	{
		std::map<uint32_t,MapObject *>::iterator iter = this->object_cache.find(ident);
		if( iter != this->object_cache.end() )
			return iter->second;
	}
	return NULL;
}

/**
 * @brief GameWorldSystem::ClearObjects
 */
void GameWorldSystem::ClearObjects()
{
	std::map<uint32_t, MapObject*>::iterator l_object;
	if ( this->object_cache.size() )
	{
		for ( l_object = this->object_cache.begin(); l_object != this->object_cache.end(); l_object++ )
		{
			l_object->second->FreeData();
			this->object_cache.erase(l_object);
			delete l_object->second;
		}
	}
}

/**
 * @brief GameWorldSystem::RemoveObject
 * @param ident
 * @return
 */
bool GameWorldSystem::RemoveObject(uint32_t ident)
{
	MapObject * object = this->GetObject(ident);
	if ( object != NULL )
	{
		if ( lux::display->RemoveObject(object->layer, object) )
		{
			object->FreeData();
			this->object_cache.erase(ident);
			delete object;
			return true;
		}
	}
	return false;
}

/* Colllision Functions */
void GameWorldSystem::AddCollision( uint32_t name, CollisionObject * rect)
{
	if ( !rect->added )
	{
		rect->added = true;
		this->collision_list.insert(std::make_pair(name,rect));
	}
}

void GameWorldSystem::CheckCollision( uint32_t name, CollisionObject *rect)
{

}

void GameWorldSystem::DrawCollisions()
{
	if ( this->collision_list.size() )
	{
		LuxRect screen_location;
		ObjectEffect effects;

		std::multimap<uint32_t, CollisionObject*>::iterator p;
		for( p = this->collision_list.begin(); p != this->collision_list.end(); p++ )
		{
			if ( p->second->rect.w != 0 && p->second->rect.h != 0 )
			{
				screen_location = p->second->rect;
				screen_location.z = 6999;
				screen_location.x -= MAKE_FIXED_INT(this->active_map->GetPosition('x') );
				screen_location.y -= MAKE_FIXED_INT(this->active_map->GetPosition('y') );
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

void GameWorldSystem::ClearCollisions()
{
	this->collision_list.clear();
}

void GameWorldSystem::ReturnCollisions( std::vector<CollisionResult*> * hits, uint32_t entity, int16_t count, LuxRect rect )
{
	if ( this->collision_list.size() )
	{
		std::multimap<uint32_t, CollisionObject*>::iterator p;
		for ( p = this->collision_list.begin(); p != this->collision_list.end(); p++ )
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

void GameWorldSystem::RemoveCollisions( uint32_t name )
{
	this->collision_list.erase(name);
}



/* Save System */
bool GameWorldSystem::Save( elix::File *current_save_file )
{
	uint32_t count = 0;

	/* Save Global Entity Section */
	this->global_entities->Save( current_save_file );

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
	count = this->object_cache.size();
	current_save_file->WriteWithLabel( "Global Display Object", count );
	if ( count )
	{
		std::map<uint32_t, MapObject *>::iterator iter = this->object_cache.begin();
		while( iter != this->object_cache.end() )
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
		current_section_name = this->active_section->name;
	}

	current_save_file->WriteWithLabel( "Current Map", this->active_map->Ident() ); //Current Map ID
	current_save_file->WriteWithLabel( "Current Section", current_section_name );
	current_save_file->WriteWithLabel( "Current X", (uint32_t)this->active_map->GetPosition(0) );
	current_save_file->WriteWithLabel( "Current Y", (uint32_t)this->active_map->GetPosition(1) );
	current_save_file->WriteWithLabel( "Current Z", (uint32_t)this->active_map->GetPosition(2) );
	return true;
}

bool GameWorldSystem::Restore( elix::File * current_save_file )
{
	uint32_t i = 0;
	uint32_t count = 0;
	uint32_t display_object_id = 0, restore_map_id = 0, active_map_id = 0;
	std::string active_section_name;

	this->map_counter.value = 0;

	this->ClearObjects();

	if ( !this->global_entities->parent->loaded )
	{
		Lux_FatalError("Game requires a Main Script");
		return false;
	}
	this->global_entities->Init();

	/* Load Global Entity Section */
	this->global_entities->Restore( current_save_file );

	/* Load Sections */
	count = current_save_file->ReadUint32WithLabel( "Sections", true );
	if ( count )
	{
		WorldSection * new_section = NULL;
		for( i = 0; i < count; i++ )
		{
			if ( Lux_GetState() != LOADING )
				return false;

			new_section = new WorldSection( current_save_file );

			this->section_list.insert( std::make_pair( elix::string::Hash(new_section->name), new_section ) );
		}
	}

	/* Load Maps */
	count = current_save_file->ReadUint32WithLabel( "Maps", true );
	if ( count )
	{
		MokoiMap * restoring_map = NULL;
		for( i = 0; i < count; i++ )
		{
			if (Lux_GetState() != LOADING )
				return false;

			restore_map_id = current_save_file->ReadUint32WithLabel( "Map ID", true );

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
		}
	}

	if (Lux_GetState() != LOADING )
		return false;

	/* Load Global Display Object */
	count = current_save_file->ReadUint32WithLabel( "Global Display Object",  true );
	if ( count )
	{
		for( i = 0; i < count; i++)
		{
			if (Lux_GetState() != LOADING )
				return false;
			display_object_id = current_save_file->ReadUint32WithLabel( "Global Object ID", true );
			this->object_cache[display_object_id] = new MapObject();
			this->object_cache[display_object_id]->Restore( current_save_file );
			lux::display->AddObjectToLayer(this->object_cache[display_object_id]->layer, this->object_cache[display_object_id], true); //???
		}
	}

	/* Load Position and Section */
	active_map_id = current_save_file->ReadUint32WithLabel( "Current Map", true ); //Current Map ID
	current_save_file->ReadWithLabel( "Current Section", &active_section_name );
	this->current_offset_position[0] = (fixed)current_save_file->ReadUint32WithLabel( "Current X", true );
	this->current_offset_position[1] = (fixed)current_save_file->ReadUint32WithLabel( "Current Y", true );
	this->current_offset_position[2] = (fixed)current_save_file->ReadUint32WithLabel( "Current Z", true );

	//this->active_section = this->GetSection(active_section_name);
	this->next_section = NULL;



	this->next_map =  this->active_map = NULL;


	std::map<uint32_t, MokoiMap *>::iterator p = this->map_list.find( active_map_id );
	if ( p != this->map_list.end() )
	{
		this->SwitchMap( (*p).second );
		//this->map = (*p).second;
		//this->map->Init();
		//this->map->SetPosition( this->_position );
		//this->_nextmap = this->map;
		return true;
	}
	return false;

}
