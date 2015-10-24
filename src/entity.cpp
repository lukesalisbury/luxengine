/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include <cstdarg>
#include <sstream>
#include "core.h"
#include "game_system.h"
#include "elix/elix_string.hpp"


bool Lux_Util_RectCollide(LuxRect a, LuxRect b);

Entity::Entity(std::string base, std::string id, uint32_t mapid, LuxEntityCallback * callbacks )
{
	this->DefaultSetup( mapid );

	this->_base = base;
	this->callbacks = callbacks;

	this->InitialSetup( id );

	if ( this->callbacks )
	{
		this->_data = this->callbacks->Init( this->id.c_str(), this->_base.c_str(), this );
		if ( !this->_data )
		{
			lux::core->SystemMessage(__FUNCTION__, __LINE__, SYSTEM_MESSAGE_LOG) << "No data for " << this->id << "|" << this->_base << std::endl;
		}
		else
		{
			this->loaded = true;
		}
	}

}


/*  */
Entity::Entity( elix::File * current_save_file, uint32_t mapid )
{
	this->DefaultSetup( mapid );
	this->Restore(current_save_file);
}

Entity::~Entity()
{
	if ( !this->deleted )
	{
		this->Delete();
	}
	FreeData();
}


void Entity::DefaultSetup( uint32_t mapid )
{
	this->_data = NULL;
	this->loaded = false;
	this->firstran = false;
	this->active = true;
	this->deleted = false;
	this->closing = false;
	this->sleeping = false;
	this->active_state_flags = 0xFF;
	this->_mapid = this->displaymap = mapid;

	this->x = this->y = this->z_layer = MAKE_INT_FIXED(5);

}

void Entity::InitialSetup( std::string id )
{
	this->id = id;
	this->hashid = elix::string::Hash( this->id );

	this->_used_collisions = 7;
	for ( uint8_t n = 0; n < this->_used_collisions; n++)
	{
		this->_collisions[n].id = n;
		this->_collisions[n].added = false;
		this->_collisions[n].entity = this->hashid;
		this->_collisions[n].rect.w = this->_collisions[n].rect.h =  0;
	}

}

void Entity::Init()
{
	if ( !this->firstran )
	{
		this->SetMapCollsion();
		this->callbacks->Call(this->_data, (char*)"@AutoPreInit", NULL);
		this->callbacks->Call(this->_data, (char*)"Init", NULL);
		this->callbacks->Call(this->_data, (char*)"@AutoPostInit", NULL);
		this->firstran = true;
		this->closing = false;
	}
}

bool Entity::Loop()
{
	if ( id.compare("player") == 0 )
	{
		int q =0;
	}

	if ( this->deleted )
	{
		return false;
	}

	if ( this->_data )
	{
		if ( this->active && !this->deleted)
		{
			starting_run_time = lux::core->GetTime();
			if ( !this->firstran )
			{
				this->Init();
			}

			#ifdef NETWORKENABLED
			lux::core->NetworkLock();
			this->onscreen = (lux::gameworld->active_map->Ident() == this->displaymap ? true : false);
			this->active = this->callbacks->Run( this->_data, this->sleeping );
			lux::core->NetworkUnlock();
			#else
			this->onscreen = true;

			starting_run_time = lux::core->GetTime();
			this->active = this->callbacks->Run( this->_data, this->sleeping );
			#endif

		}

	}
	else
	{
		this->deleted = true;
	}
	return this->active;
}

void Entity::Update()
{
	if ( !this->firstran )
	{
		this->Init();
	}
	this->callbacks->Call(this->_data, (char*)"UpdatePosition", NULL);
	this->callbacks->Call(this->_data, (char*)"@AutoUpdatePosition", NULL);
}

void Entity::Close()
{
	if ( this->_data && !this->closing && !this->deleted )
	{
		this->closing = true;
		this->ClearMapCollsion();
		if ( this->callbacks )
		{
			this->callbacks->Call(this->_data, (char*)"@AutoPreClose", NULL);
			this->callbacks->Call(this->_data, (char*)"Close", NULL);
			this->callbacks->Call(this->_data, (char*)"@AutoPostClose", NULL);
		}

	}
	this->firstran = false;
}

void Entity::Delete()
{
	this->Close();
	this->active = false;
	this->deleted = true;
}

bool Entity::isActive()
{
	if ( this->_data && !this->closing && !this->deleted )
	{
		return true;
	}
	else
	{
		return false;
	}
}

void Entity::FreeData()
{
	if ( this->_data )
	{
		this->callbacks->Destroy(this->_data);
	}
	this->_data = NULL;
}

int32_t Entity::Call(std::string function, char * format, ...)
{
	if ( this->deleted )
		return -1;

	if ( this->_data != NULL )
	{
		uint8_t c = 0;
		int32_t return_value = 0;
		va_list argptr;
		va_start( argptr, format );
		mem_pointer stack_mem = NULL;
		if ( strlen(format) )
		{
			while( format[c] != '\0' )
			{
				if( format[c] == 'n' )
				{
					this->callbacks->Push( this->_data, va_arg( argptr, int32_t ) );
				}
				else if( format[c] == 'd' )
				{
					this->callbacks->Push( this->_data, va_arg( argptr, int32_t ) );
				}
				else if( format[c] == 's' )
				{
					std::string str = va_arg( argptr, char * );
					this->callbacks->PushString( this->_data, str, (stack_mem ? NULL : &stack_mem) );
				}
				c++;
			}
		}
		va_end( argptr );
		this->starting_run_time = lux::core->GetTime();
		return_value = this->callbacks->Call(this->_data, (char*)function.c_str(), stack_mem);

		if ( return_value == -1 )
			lux::core->SystemMessage(__FUNCTION__, __LINE__, SYSTEM_MESSAGE_ERROR ) <<  "| Call from " << this->id << " Failed" << std::endl;
		return return_value;
	}
	lux::core->SystemMessage( __FUNCTION__, __LINE__, SYSTEM_MESSAGE_LOG ) << "'" << this->id << "' Call Function Failed '" << function << "'" << std::endl;
	return -1;
}

/**
 * @brief Entity::Save
 * @param current_save_file
 */
void Entity::Save( elix::File * current_save_file )
{
	if ( this->deleted || !current_save_file )
		return;

	/* Save Entity Details */
	current_save_file->WriteWithLabel( "id", this->id );
	current_save_file->WriteWithLabel( "base", this->_base );
	current_save_file->WriteWithLabel( "x", (uint32_t)this->x );
	current_save_file->WriteWithLabel( "y", (uint32_t)this->y );
	current_save_file->WriteWithLabel( "z_layer", (uint32_t)this->z_layer );
	current_save_file->WriteWithLabel( "active state flags", this->active_state_flags  );

	/* Save Entity Data */
	if ( this->callbacks )
	{
		current_save_file->WriteWithLabel("EntitySystem", this->callbacks->system_identifier );
		this->callbacks->Save( current_save_file, this->_data );
	}
	else
	{
		current_save_file->WriteWithLabel("EntitySystem", (uint8_t)0 );
	}

}

/**
 * @brief Entity::Restore
 * @param current_save_file
 */
void Entity::Restore( elix::File * current_save_file )
{
	lux::screen::display("Loading Saved Game");

	uint8_t entity_system_id = 0;

	/* Read Entity Details */
	current_save_file->ReadWithLabel( "id", &this->id );
	current_save_file->ReadWithLabel( "base", &this->_base );
	this->x = current_save_file->ReadUint32WithLabel( "x", true );
	this->y = current_save_file->ReadUint32WithLabel( "y", true );
	this->z_layer = current_save_file->ReadUint32WithLabel( "z_layer", true );
	this->active_state_flags = current_save_file->ReadUint8WithLabel( "active state flags" );

	/* Read Entity Data */
	entity_system_id = current_save_file->ReadUint8WithLabel("EntitySystem" );
	this->callbacks = lux::entity_system->GetSystem( entity_system_id );


	if ( this->callbacks )
	{
		this->InitialSetup( this->id );
		this->callbacks->Restore( current_save_file, this->_data );
		this->loaded = true;
		this->firstran = true;
	}



}

/* Entity Collisions */

/**
 * @brief Entity::AddCollision
 * @param rect
 * @param x
 * @param y
 * @param w
 * @param h
 * @param type
 * @return
 */
int32_t Entity::AddCollision(int32_t rect, int32_t x, int32_t y, int32_t w, int32_t h, int32_t type)
{
	if ( !this->_data || this->deleted )
	{
		return 0;
	}
	if ( rect >= 0 && rect < 7 )
	{
		this->_collisions[rect].rect.x = x;
		this->_collisions[rect].rect.y = y;
		this->_collisions[rect].rect.w = w;
		this->_collisions[rect].rect.h = h;
		this->_collisions[rect].type = type;
		if ( !this->_collisions[rect].added )
		{
			this->SetMapCollsion();
		}
		return 2;
	}
	else
	{
		this->ClearMapCollsion();
		return 1;
	}
	return 0;
}

/**
 * @brief Entity::CheckCollision
 * @param rect
 * @param otherRect
 * @return
 */
int32_t Entity::CheckCollision(int32_t rect, LuxRect otherRect)
{
	if ( this->_used_collisions == -1 )
	{
		return -1;
	}

	if ( rect == -1 )
	{
		int32_t ireturn = 0;
		for( int32_t n = 0; n < this->_used_collisions; n++ )
		{
			if ( this->_collisions[rect].rect.w != 0 && this->_collisions[rect].rect.h != 0 )
				ireturn += Lux_Util_RectCollide(this->_collisions[n].rect, otherRect);
		}
		return ireturn;
	}
	else if ( rect < 0 )
	{

	}
	else if ( rect < this->_used_collisions )
	{
		if ( this->_collisions[rect].rect.w != 0 && this->_collisions[rect].rect.h != 0 )
			return Lux_Util_RectCollide(this->_collisions[rect].rect, otherRect);
	}

	return -1;
}

/**
 * @brief Entity::SetCollisionCount
 * @param count
 */
void Entity::SetCollisionCount(int32_t count)
{
	this->_used_collisions = count;
	this->ClearMapCollsion();
	this->SetMapCollsion();
}

/**
 * @brief Entity::GetHits
 * @param type
 * @return
 */
int32_t Entity::GetHits( int32_t type )
{
	this->_hits.clear();
	if (lux::game_system != NULL)
	{
		for(int32_t n = 0; n < this->_used_collisions; n++)
		{
			if (this->_collisions[n].rect.w != 0 && this->_collisions[n].rect.h != 0)
			{
				lux::game_system->GetObjects()->ReturnCollisions(&this->_hits, this->hashid, n, this->_collisions[n].rect);
			}
		}
	}
	return this->_hits.size();
}

/**
 * @brief Entity::SetMapCollsion
 */
void Entity::SetMapCollsion()
{
	if ( lux::game_system != NULL )
	{
		if ( this->_used_collisions > -1 )
		{
			for(int32_t n = 0; n < this->_used_collisions; n++)
			{
				if ( this->_collisions[n].added == false )
				{
					if (this->_collisions[n].rect.w != 0 && this->_collisions[n].rect.h != 0)
					{
						lux::game_system->GetObjects()->AddCollision(this->hashid, &this->_collisions[n]);
					}
				}
			}
		}
	}
}

/**
 * @brief Entity::ClearMapCollsion
 */
void Entity::ClearMapCollsion()
{
	if ( lux::game_system != NULL )
	{
		for( int32_t n = 0; n < this->_used_collisions; n++)
		{
			this->_collisions[n].rect.w = this->_collisions[n].rect.h = 0;
			this->_collisions[n].added = false;
		}
		lux::game_system->GetObjects()->RemoveCollisions(this->hashid);
	}
}

/**
 * @brief Entity::GetHitsCount
 * @return
 */
int32_t Entity::GetHitsCount()
{
	return this->_hits.size();
}

/**
 * @brief Entity::GetCurrentHit
 * @return
 */
CollisionResult Entity::GetCurrentHit()
{
	CollisionResult current_hit = this->_hits.back();
	this->_hits.pop_back();
	return current_hit;
}

/* Settings */
/**
 * @brief Entity::AddSetting
 * @param key
 * @param value
 */
void Entity::AddSetting(std::string key, std::string value)
{
	std::string set_key(key), set_value(value);
	this->_settings.insert( std::make_pair( set_key, set_value ) );
}

/**
 * @brief Entity::AddSetting
 * @param key
 * @param value
 */
void Entity::AddSetting(std::string key, uint32_t value)
{
	std::stringstream stream;
	stream << value;
	this->_settings.insert( make_pair( key, stream.str() ) );
}

/**
 * @brief Entity::AddSetting
 * @param key
 * @param value
 */
void Entity::AddSetting(std::string key, int32_t value)
{
	std::stringstream stream;
	stream << value;
	this->_settings.insert( make_pair( key, stream.str() ) );
}

/**
 * @brief Entity::GetSetting
 * @param key
 * @return
 */
std::string Entity::GetSetting( char * key )
{
	std::string strkey;
	std::string str = "";
	if ( key != NULL )
	{
		strkey.assign(key);
		if ( !this->_settings.empty() )
		{
			std::map<std::string,std::string>::iterator iter = this->_settings.end();
			iter = this->_settings.find(strkey);
			if ( iter != this->_settings.end() )
			{
				str.assign(iter->second);
			}
		}
	}
	return str;
}

/**
 * @brief Entity::GetSetting
 * @param key
 * @return
 */
std::string Entity::GetSetting( std::string key )
{
	std::string str = "";
	if ( !this->_settings.empty() )
	{
		std::map<std::string,std::string>::iterator iter = this->_settings.find(key);
		if ( iter != this->_settings.end() )
		{
			str.assign(iter->second);
		}
		iter = this->_settings.begin();
	}
	return str;
}
/**
 * @brief Entity::GetSettingAsInt
 * @param key
 * @return
 */
int32_t Entity::GetSettingAsInt(std::string key)
{
	int32_t value = 0;
	std::stringstream stream( this->GetSetting(key) );
	stream >> value;
	return value;
}

/**
 * @brief Entity::GetSettingAsFloat
 * @param key
 * @return
 */
float Entity::GetSettingAsFloat(std::string key)
{
	float value = 0.0;
	std::stringstream stream( this->GetSetting(key) );
	stream >> value;
	return value;
}

/**
 * @brief Entity::Infomation
 * @return
 */
std::string Entity::Infomation()
{
	std::stringstream contents;
	std::map<std::string,std::string>::iterator settings_iter;

	contents << "Name: " << this->id << "#" << this->hashid << " from " << this->_base << '\n';
	contents << "Location: " << this->x << "," << this->y << "," << this->z_layer << " on map " << this->displaymap << '\n';
	contents << "Callbacks: " << this->callbacks << " | Collision Rectangle:" << this->_used_collisions << '\n';
	contents << "Settings: " << this->_settings.size() << '\n';

	for ( settings_iter = this->_settings.begin(); settings_iter != this->_settings.end(); settings_iter++ )
	{
		contents << "-" << settings_iter->first << "=" << settings_iter->second << '\n';
	}


	return contents.str();
}
