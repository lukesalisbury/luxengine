/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "luxengine.h"
#include "entity_section.h"
#include "entity_manager.h"
#include "core.h"

extern LuxEntityCallback EntitySystemPawn;

EntitySection::EntitySection( std::string parent_entity, uint32_t id )
{
	this->initialised = false;
	this->map_id = id;
	this->parent = new Entity(parent_entity, "parent", this->map_id, lux::entity_system->GetSystem(parent_entity) );
	if ( id == 0 )
	{
		lux::entities->_global = this->parent;
	}
}

EntitySection::~EntitySection()
{

	if ( this->children.size() )
	{
		std::vector<Entity *>::iterator iter = this->children.begin();
		while( iter != this->children.end() )
		{
			if (*iter)
			{
				delete (*iter);
			}
			iter = this->children.erase(iter);
		}
	}
	if ( this->parent )
		delete this->parent;

	this->children.clear();
	this->initialised = false;
}

/**
 * @brief EntitySection::Save
 * @param current_save_file
 * @return
 */
bool EntitySection::Save(elix::File * current_save_file)
{
	/* Cleared Deleted Entity */
	std::vector<Entity *>::iterator iter = this->children.begin();
	while( iter !=  this->children.end() )
	{
		if ( (*iter)->deleted )
		{
			delete (*iter);
			iter = this->children.erase(iter);
		}
		else
		{
			iter++;
		}
	}

	/* Now we save the section entity */
	uint32_t count = this->children.size();
	current_save_file->WriteWithLabel( "Section Entity Count", count );

	/* Parent Entity always exists, It Might have no content but it should exist */
	this->parent->Save( current_save_file );
	if ( count )
	{
		for ( uint32_t i = 0; i < count; i++ )
		{
			this->children[i]->Save( current_save_file );
		}
	}
	return true;
}

bool EntitySection::Restore( elix::File * current_save_file )
{
	lux::screen::display("Loading Saved Game");

	uint32_t count = current_save_file->ReadUint32WithLabel( "Section Entity Count", true );

	/* Parent Entity always exists, It Might have no content be t should exist */
	this->parent->Restore( current_save_file );

	if ( Lux_GetState() != LOADING )
		return false;

	if ( count)
	{
		for ( uint32_t i = 0 ; i < count; i++ )
		{
			if ( Lux_GetState() != LOADING )
				return false;

			Entity * restored_entity = new Entity( current_save_file, this->map_id );
			if ( restored_entity->loaded )
			{
				lux::entities->AddEntity( restored_entity );
				this->children.push_back( restored_entity );
			}
		}
	}
	return true;
}

void EntitySection::Init()
{
	uint32_t count = this->children.size();
	if ( this->parent )
	{
		this->parent->Init();
	}

	if ( count )
	{
		for( uint32_t i = 0; i < count; i++ )
		{
			this->children[i]->Init();
		}
	}
	this->initialised = true;
}

bool EntitySection::Loop()
{
	if ( !this->initialised )
		this->Init();

	if ( this->parent )
	{
		if ( !this->parent->Loop() )
			Lux_DisplayMessage("Section's Parent Entity not running");
	}
	else
	{
		Lux_DisplayMessage("Section's Parent Entity not running");
	}

	uint32_t count = this->children.size();
	std::vector<Entity *>::iterator iter = this->children.begin();
	if ( count )
	{
		for( uint32_t i = 0; i < count; i++ )
		{
			if (this->children[i])
				this->children[i]->Loop();
		}

		// Clean up
		iter = this->children.begin();
		while( iter != this->children.end() )
		{
			if ( (*iter)->deleted )
			{
				delete (*iter);
				iter = this->children.erase(iter);
			}
			else
			{
				iter++;
			}
		}
	}

	return true;
}

void EntitySection::Switch(fixed x, fixed y)
{
	if ( this->parent )
	{
		lux::core->SystemMessage(__FILE__, __LINE__, SYSTEM_MESSAGE_INFO) << " Section Entity Switch()" << std::endl;
		this->parent->Update();
	}

	if ( this->children.size() )
	{
		std::vector<Entity *>::iterator iter = this->children.begin();
		while( iter != this->children.end() )
		{
			lux::core->SystemMessage(__FILE__, __LINE__, SYSTEM_MESSAGE_INFO) << " Entity " << (*iter)->id << " Switch()" << std::endl;
			(*iter)->x += x;
			(*iter)->y += y;
			(*iter)->Update();
			iter++;
		}
	}
}

void EntitySection::Close()
{
	lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << " < EntitySection Close " << this->map_id << std::endl;
	if ( this->parent )
	{
		lux::core->SystemMessage(__FILE__, __LINE__, SYSTEM_MESSAGE_INFO) << " Section Entity Close()" << std::endl;
		this->parent->Close();
	}

	if ( this->children.size() )
	{
		/* Close Entities */
		std::vector<Entity *>::iterator iter = this->children.begin();
		while( iter !=  this->children.end() )
		{
			lux::core->SystemMessage(__FILE__, __LINE__, SYSTEM_MESSAGE_INFO) << " Entity " << (*iter)->id << " Close()" << std::endl;
			(*iter)->Close();
			iter++;
		}

		/* Remove Deleted Entities */
		iter = this->children.begin();
		while( iter !=  this->children.end() )
		{
			if ( (*iter)->deleted )
			{
				delete (*iter);
				iter = this->children.erase(iter);
			}
			else
			{
				iter++;
			}
		}
	}
}

void EntitySection::Append( Entity * enw )
{
	if ( enw != NULL )
	{
		this->_entities.insert( std::make_pair(enw->hashid, enw) );
		this->children.push_back(enw);

	}
}

Entity * EntitySection::GetEntity( uint32_t entity_id )
{
	std::map<uint32_t, Entity *>::iterator p;
	p = this->_entities.find(entity_id);
	if ( p != this->_entities.end() )
	{
		return p->second;
	}
	return NULL;
}
