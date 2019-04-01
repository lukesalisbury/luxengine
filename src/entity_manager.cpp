/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include <ctime>
#include <cstdlib>
#include <sstream>

#include "entity_manager.h"
#include "game_system.h"
#include "elix/elix_string.hpp"
#include "display/display.h"
#include "entity_system.h"
#include "entity_systems/pawn.inc"


uint32_t entity_maphash = 0xD4824F64; //__map__
uint32_t entity_mainhash = 0x1D1A8BF1; //__main__

EntityManager::EntityManager()
{
	this->_count = 0;
	this->_global = this->_keyboard = nullptr;

	srand( time(nullptr) );
}

EntityManager::~EntityManager()
{
	this->_entities.clear();
}

void EntityManager::SetSaveMode(uint8_t mode)
{
	lux::entity_system->SetSaveMode( mode );
}

uint32_t EntityManager::GetRandomID( std::string * id )
{
	if ( !id->length() )
	{
		std::stringstream random_name;
		random_name << "__e" << this->_count << "_" << rand();
		id->assign( random_name.str() );
	}
	return elix::string::Hash(*id);
}

Entity * EntityManager::NewEntity(std::string id, std::string base, uint32_t map_id)
{
	uint32_t hash = this->GetRandomID( &id );
	if ( this->GetEntity(hash) )
	{
		return nullptr;
	}

	Entity * new_entity = new Entity( base, id, map_id, lux::entity_system->GetSystem(base) );
	if ( new_entity->loaded )
	{
		if ( this->AddEntity( new_entity ) )
		{
			if ( map_id )
			{
				MokoiMap * map = lux::game_system->GetMap(map_id);
				if ( map && map->entities )
				{
					map->entities->Append(new_entity);
					return new_entity;
				}
			}
			else if ( lux::game_system->GetEntities() )
			{
				lux::game_system->GetEntities()->Append(new_entity);
				return new_entity;
			}

		}
	}
	delete new_entity;
	return nullptr;
}

Entity * EntityManager::GetEntity(uint32_t entity_id )
{
	if ( entity_id == entity_maphash )
	{
		if ( lux::game_system->active_map != nullptr )
			if ( lux::game_system->active_map->entities != nullptr)
				return lux::game_system->active_map->entities->parent;
			else
				return nullptr;
		else
			return nullptr;
	}
	else if ( entity_id == entity_mainhash )
	{
		if ( lux::game_system != nullptr )
			return lux::game_system->GetEntities()->parent;
		else
			return nullptr;
	}

	std::map<uint32_t, Entity *>::iterator p;
	p = this->_entities.find(entity_id);
	if ( p != this->_entities.end() )
	{
		if ( p->second->isActive() )
		{
			return p->second;
		}
	}

	return nullptr;
}

bool EntityManager::AddEntity( Entity * enw )
{
	if ( this->GetEntity(enw->hashid) != nullptr )
		return false;
	this->_entities[enw->hashid] = enw;
	this->_count = this->_entities.size();
	return true;
}

bool EntityManager::RemoveEntity(uint32_t entity_id)
{
	return !!this->_entities.erase(entity_id);
}
