/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include <cstdlib>

#include "engine.h"
#include "display/display.h"
#include "elix/elix_string.hpp"
#include "mokoi_game.h"
#include "entity_section.h"
#include "game_system.h"
#include "game_config.h"
#include "misc_functions.h"


/**
 * @brief GameWorldSystem::GameWorldSystem
 */
GameSystem::GameSystem()
{
	this->active_map = this->next_map = NULL;
	this->active_section = this->next_section = NULL;

	this->current_offset_position[0] = this->current_offset_position[1] = this->current_offset_position[2] = MAKE_INT_FIXED(1);
	this->next_offset_position[0] = this->next_offset_position[1] = this->next_offset_position[2] = -1;
	this->map_movement_direction = 0;

	this->map_counter.value = 0xFFFFFFFF;
	this->map_counter.grid.section = 1; /* Section counter */

	this->physic_world = new b2World( b2Vec2(0.0, 9.8) );
	this->objects = new GlobalObjects();
	this->global_entities = new EntitySection("main", 0);

}

/**
 * @brief GameWorldSystem::~GameWorldSystem
 */
GameSystem::~GameSystem()
{
	NULLIFY( this->global_entities );
	NULLIFY( this->objects );
	NULLIFY( this->physic_world );
}


/* */

/**
 * @brief GameWorldSystem::Init
 * @return
 */
bool GameSystem::Init( )
{
	lux::core->SystemMessage(SYSTEM_MESSAGE_LOG) << " > Opening the Game" << std::endl;

	this->objects->Init();

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
	this->next_section = this->NewSection( "map/" + starting, 1, 1 );
	this->next_section->AddMap( starting, 0, 0 );

	this->SwitchActive();

	if ( !this->active_map || !this->active_map->Valid() )
	{
		Lux_FatalError("No Starting Map. Please set 'map.start' setting. ");
		return false;
	}

	return true;
}

/**
 * @brief GameWorldSystem::Loop
 * @param engine_state
 */
void GameSystem::Loop( LuxState engine_state )
{
	this->SwitchActive();

	if ( !this->active_map )
	{
		Lux_SetState( INVALIDPOSITION );
		return;
	}

	this->active_map->SetPosition( this->current_offset_position );

	if ( !this->active_section->IsSingleMap() && this->active_map->wrap_mode != MAP_WRAPBOTH )
	{
		this->CheckPosition();
	}

	//this->physic_world->Step(0.01667, 6, 2); //Disable Box2d for now

	this->objects->Loop( engine_state );
	this->global_entities->Loop();
	this->active_map->Loop();



}

/**
 * @brief GameWorldSystem::Close
 * @return
 */
bool GameSystem::Close()
{
	lux::core->SystemMessage(SYSTEM_MESSAGE_LOG) << " < Closing the Game" << std::endl;

	this->global_entities->Close();

	if ( this->active_map )
		this->active_map->Close();

	this->active_map = NULL;

	for( std::map<uint32_t, WorldSection *>::iterator iter = section_list.begin(); iter != section_list.end(); ++iter )
	{
		delete iter->second;
	}

	for( std::map<uint32_t, MokoiMap *>::iterator iter = map_list.begin(); iter != map_list.end(); ++iter )
	{
		delete iter->second;
	}


	this->map_list.clear();
	this->section_list.clear();


	this->objects->Close();


	return true;
}

/* Positions */

/**
 * @brief GameWorldSystem::SetPosition
 * @param offset_x
 * @param offset_y
 * @param offset_z
 */
void GameSystem::SetPosition(fixed offset_x, fixed offset_y, fixed offset_z )
{
	this->current_offset_position[0] = offset_x;
	this->current_offset_position[1] = offset_y;
	if ( offset_z == -1 )
		this->current_offset_position[2] = offset_z;
}

/**
 * @brief GameWorldSystem::GetPosition
 * @param axis
 * @return
 */
fixed GameSystem::GetPosition( uint8_t axis )
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
void GameSystem::CheckPosition()
{
	uint8_t updated_grid_position[2] = { 0,0 };

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
void GameSystem::SwitchActive()
{
	if ( this->next_section )
	{
		if ( this->next_map == NULL )
		{
			this->next_map = this->next_section->GetMap( this->next_grid_position[0], this->next_grid_position[1] );
		}

		if ( this->SwitchMap( this->next_map ) )
		{
			this->active_section = this->next_section;
			this->next_section = NULL;
			this->next_map = NULL;
			this->next_grid_position[0] = 0;
			this->next_grid_position[1] = 0;
		}
	}
}

/**
 * @brief GameWorldSystem::SwitchMap
 * @param next_map
 * @return
 */
bool GameSystem::SwitchMap( MokoiMap * next_map )
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
		else
		{
			if ( this->global_entities )
				this->global_entities->Switch( 0, 0 );
		}
		this->map_movement_direction = 0;

	}

	/* Set new Map to active and call it's Init */
	this->active_map = next_map;
	this->grid_position[0] = this->next_grid_position[0];
	this->grid_position[1] = this->next_grid_position[1];


	if ( this->next_offset_position[0] != -1 )
		this->current_offset_position[0] = this->next_offset_position[0];
	if ( this->next_offset_position[1] != -1 )
		this->current_offset_position[1] = this->next_offset_position[1];
	if ( this->next_offset_position[2] != -1 )
		this->current_offset_position[2] = this->next_offset_position[2];
	this->active_map->Init();

	/* Add Global Display objects */
	this->GetObjects()->ReorderObjects();

	return true;
}


/**
 * @brief Tells the engine to switch to new map based on x/y of a WorldSection
 * @param section
 * @param grid_x
 * @param grid_y
 * @return
 */
uint32_t GameSystem::SetMap( WorldSection * section, uint8_t grid_x, uint8_t grid_y, fixed position_x, fixed position_y, fixed position_z  )
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
uint32_t GameSystem::SetMap( std::string section_name, uint8_t grid_x, uint8_t grid_y, fixed position_x, fixed position_y, fixed position_z  )
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
uint32_t GameSystem::SetMap( uint32_t section_hash, uint8_t grid_x, uint8_t grid_y, fixed position_x, fixed position_y, fixed position_z  )
{
	if ( section_hash )
	{
		WorldSection * section = this->GetSection( section_hash );
		return this->SetMap( section, grid_x, grid_y,  position_x, position_y, position_z );
	}
	else
	{
		return 0;
	}
}

/**
 * @brief GameWorldSystem::SetMap
 * @param map
 * @param position_x
 * @param position_y
 * @param position_z
 * @return
 */
uint32_t GameSystem::SetMap( MokoiMap * map, fixed position_x, fixed position_y, fixed position_z )
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

		//if ( this->next_map && this->next_section )
		//	std::cout << "[Next Active] " << this->next_section->Name() << "  " << this->next_map->Name() << std::endl;
	}

	return ( this->next_map == NULL ? 0 : this->next_map->Ident() );

}

/* Get Display Object*/

/**
 * @brief GameWorldSystem::GetObject
 * @param object_id
 * @return
 */
MapObject * GameSystem::GetObject(uint32_t object_id)
{
	MapObject * object = NULL;
	if ( (object_id & OBJECT_GLOBAL_VALUE) == OBJECT_GLOBAL_VALUE )
		object = this->GetObjects()->GetObject(object_id);
	else if ( this->active_map )
		object = this->active_map->GetObject(object_id);

	return object;

}

/* Map Handling */

/**
 * @brief GameWorldSystem::InsertMap
 * @param ident
 * @param map
 * @return
 */
bool GameSystem::InsertMap( uint32_t ident, MokoiMap * map )
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
MokoiMap * GameSystem::CreateMap( std::string map_name, bool removeable, bool editable, uint32_t width, uint32_t height )
{

	/* If not editable map and file doesn't not exist, return NULL */
	if ( !editable )
	{
		if ( !lux::game_data->HasFile("./maps/" + map_name + ".xml") )
		{
			return NULL;
		}
	}

	MokoiMap * new_map = NULL;
	WorldSection * section = this->NewSection( "map/" + map_name, 1, 1);

	new_map = new MokoiMap( map_name, width, height );

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
MokoiMap * GameSystem::GetMap( uint32_t id )
{
	std::map<uint32_t, MokoiMap *>::iterator p;
	if ( id )
	{
		for ( p = this->map_list.begin(); p != this->map_list.end(); p++ )
		{
			if ( id == p->second->Ident() )
			{
				return p->second;
			}
		}
	}
	return NULL;
}

/**
 * @brief GameWorldSystem::DeleteMap
 * @param ident
 * @return
 */
bool GameSystem::DeleteMap( uint32_t ident )
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
MokoiMap * GameSystem::FindMap( std::string map_file )
{
	MokoiMap * map = NULL;

	std::map<uint32_t, MokoiMap *>::iterator p;
	for ( p = this->map_list.begin(); p != this->map_list.end(); p++ )
	{
		if ( map_file.compare( p->second->Name() ) == 0 )
		{
			map = p->second;
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
uint32_t GameSystem::GetMapID( std::string name )
{
	MokoiMap * map = this->FindMap(name);

	if ( map )
		return map->Ident();
	else
		return 0;
}

/* World Section System */

/**
 * @brief GameWorldSystem::InsertSection
 * @param section
 * @param hash_ident
 * @return
 */
bool GameSystem::InsertSection( WorldSection * section, uint32_t section_ident )
{
	this->section_list.insert( std::make_pair( section_ident, section ) );
	this->map_counter.grid.section++;
	return true;
}

/**
 * @brief GameWorldSystem::NewSection
 * @param section_name
 * @param width
 * @param height
 * @return
 */
WorldSection * GameSystem::NewSection( std::string section_name, const uint8_t width, const uint8_t height )
{
	return new WorldSection( this->map_counter, section_name, width, height );
}

/**
 * @brief GameWorldSystem::LoadSection
 * @param name
 * @param set_current
 * @return
 */
WorldSection * GameSystem::LoadSection( std::string name, bool set_current )
{
	WorldSection * new_section = NULL;

	if ( lux::game_data->HasFile("./worlds/" + name + ".tsv") )
	{
		new_section = this->NewSection( name, 64, 64 );

		if ( set_current )
		{
			this->next_section = new_section;
		}
	}
	return new_section;
}

/**
 * @brief GameWorldSystem::GetSection
 * @param section_name
 * @param load
 * @return
 */
WorldSection * GameSystem::GetSection( std::string section_name, bool load )
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
		requested_section = this->LoadSection( section_name, false );
	}

	return requested_section;
}

/**
 * @brief GameWorldSystem::GetSection
 * @param map
 * @return
 */
WorldSection * GameSystem::GetSection( MokoiMap * map )
{
	WorldSection * requested_section = NULL;

	if ( map != NULL )
	{
		if ( this->section_list.size() )
		{
			std::map<uint32_t, WorldSection *>::iterator p;
			for ( p = this->section_list.begin(); p != this->section_list.end(); p++ )
			{
				if ( map->InSection( p->second->SectionIdent() ) )
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
WorldSection * GameSystem::GetSection( uint32_t section_hash )
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
 * @brief Remove Maps for Global list
 * @param ident
 */
void GameSystem::DeleteSection( uint32_t ident )
{

	std::map<uint32_t, MokoiMap *>::iterator p = this->map_list.begin();
	while( p != this->map_list.end() )
	{
		MokoiMap * map = p->second;
		if ( map->InSection(ident) && !map->keep_memory )
		{
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
bool GameSystem::SetSectionGrid( uint8_t grid_x, uint8_t grid_y )
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
bool GameSystem::GetSectionGrid( uint8_t & gridx, uint8_t & gridy )
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
uint32_t GameSystem::GetMapID( std::string section_name, uint8_t grid_x, uint8_t grid_y )
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
	{
		return this->active_section->GetGridID(grid_x, grid_y);
	}

	return 0xFFFF;
}

/* Save System */

/**
 * @brief GameWorldSystem::Save
 * @param current_save_file
 * @return
 */
bool GameSystem::Save( elix::File *current_save_file )
{
	uint32_t count = 0;

	/* Save Global Entity Section */
	this->global_entities->Save( current_save_file );

	/* Save Maps */
	count = this->map_list.size();
	current_save_file->WriteWithLabel( "Maps", count );
	if ( count )
	{
		std::map<uint32_t, MokoiMap *>::iterator iter = this->map_list.begin();
		while( iter != this->map_list.end() )
		{
			iter->second->Save( current_save_file );
			iter++;
		}
	}


	/* Save Worlds */
	count = this->section_list.size();
	current_save_file->WriteWithLabel( "Worlds", count );
	if ( count )
	{
		std::map<uint32_t, WorldSection *>::iterator iter = this->section_list.begin();
		while( iter != this->section_list.end() )
		{
			iter->second->Save( current_save_file );
			iter++;
		}
	}

	this->objects->Save( current_save_file );

	/* Save Position and Section */
//	current_save_file->WriteWithLabel( "Current Section", this->active_section->Ident() );
	current_save_file->WriteWithLabel( "Current Map", this->active_map->Ident() );
	current_save_file->WriteWithLabel( "Current X", (uint32_t)this->active_map->GetPosition(0) );
	current_save_file->WriteWithLabel( "Current Y", (uint32_t)this->active_map->GetPosition(1) );
	current_save_file->WriteWithLabel( "Current Z", (uint32_t)this->active_map->GetPosition(2) );




	return true;
}

/**
 * @brief GameWorldSystem::Restore
 * @param current_save_file
 * @return
 */
bool GameSystem::Restore( elix::File * current_save_file )
{
	uint32_t i = 0;
	uint32_t count = 0;
	uint32_t active_map_id = 0, active_section_id = 0;

	this->map_counter.value = 0;

	/* Load Global Entity Section */
	if ( !this->global_entities->parent->loaded )
	{
		Lux_FatalError("Game requires a Main Script");
		return false;
	}
	this->global_entities->Restore( current_save_file );


	/* Load Maps */
	count = current_save_file->ReadUint32WithLabel( "Maps", true );
	if ( count )
	{
		MokoiMap * restoring_map = NULL;
		for( i = 0; i < count; i++ )
		{
			if (Lux_GetState() != LOADING )
				return false;
			lux::screen::push();
			restoring_map = new MokoiMap(current_save_file);
			this->InsertMap(restoring_map->Ident(), restoring_map );
		}
	}


	/* Load Sections */
	count = current_save_file->ReadUint32WithLabel( "Worlds", true );
	if ( count )
	{
		WorldSection * new_section = NULL;
		for( i = 0; i < count; i++ )
		{
			if ( Lux_GetState() != LOADING )
				return false;
			lux::screen::push();
			new_section = new WorldSection( current_save_file );

			this->section_list.insert( std::make_pair( elix::string::Hash(new_section->name), new_section ) );
		}
	}

	if ( Lux_GetState() != LOADING )
		return false;

	this->objects->Restore( current_save_file );

	/* Load Position and Section */
//	active_section_id = current_save_file->ReadUint32WithLabel( "Current Section", true );
	active_map_id = current_save_file->ReadUint32WithLabel( "Current Map", true ); //Current Map ID

	this->current_offset_position[0] = (fixed)current_save_file->ReadUint32WithLabel( "Current X", true );
	this->current_offset_position[1] = (fixed)current_save_file->ReadUint32WithLabel( "Current Y", true );
	this->current_offset_position[2] = (fixed)current_save_file->ReadUint32WithLabel( "Current Z", true );

	this->next_section = this->active_section = NULL;
	this->next_map =  this->active_map = NULL;

	std::map<uint32_t, MokoiMap *>::iterator p = this->map_list.find( active_map_id );
	if ( p != this->map_list.end() )
	{
		this->SetMap( (*p).second, this->current_offset_position[0], this->current_offset_position[1] , this->current_offset_position[2] );

		return true;
	}
	return false;
}

/* Messages */

/**
 * @brief GameWorldSystem::OutputInformation
 */
void GameSystem::OutputInformation()
{
	std::stringstream buffer;
	for( std::map<uint32_t, WorldSection *>::iterator iter = section_list.begin(); iter != section_list.end(); ++iter )
	{
		WorldSection * section = iter->second;

		buffer << section->Name() << std::endl;
		for( std::map<uint32_t, MokoiMap *>::iterator iter2 = this->map_list.begin(); iter2 != map_list.end(); ++iter2 )
		{
			MokoiMap * map = iter2->second;

			if ( map->InSection( section->SectionIdent() ) )
			{
				buffer << " - " <<  map->Name() << " §c" << map->GridIdent() << " §4" << map->Ident() << std::endl;

				if ( map == this->active_map )
				{
					buffer << " - " <<  map->GetInfo() << std::endl;
				}

			}
		}
	}
	buffer << "Global §6Objects:" << this->GetObjects()->object_cache.size() <<  " §cEntities:" << this->global_entities->children.size() << std::endl;
	lux::display->graphics.DrawMessage( buffer.str(), 1 );
}
