/****************************
Copyright © 2014-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "global_objects.h"
#include "display.h"
#include "game_system.h"
#include "misc_functions.h"

GlobalObjects::GlobalObjects()
{


}

GlobalObjects::~GlobalObjects()
{

}

bool GlobalObjects::Init()
{
	this->object_cache_count = 0;
	return true;
}

void GlobalObjects::Loop(LuxState engine_state)
{

}

bool GlobalObjects::Close()
{
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

	return true;
}


/* Save System */

/**
 * @brief GlobalObjects::Save
 * @param current_save_file
 * @return
 */
bool GlobalObjects::Save( elix::File *current_save_file )
{
	/* Save Global Display Object */
	uint32_t count = this->object_cache.size();

	current_save_file->WriteWithLabel( "Global Display Object", count );
	if ( count )
	{
		MapObjectList::iterator iter = this->object_cache.begin();
		while( iter != this->object_cache.end() )
		{
			current_save_file->WriteWithLabel( "Global Object ID", iter->first );
			iter->second->Save( current_save_file );
			iter++;
		}
	}

	return true;
}

/**
 * @brief GlobalObjects::Restore
 * @param current_save_file
 * @return
 */
bool GlobalObjects::Restore( elix::File * current_save_file )
{
	this->ClearObjects();

	/* Load Global Display Object */
	uint32_t count = current_save_file->ReadUint32WithLabel( "Global Display Object",  true );
	if ( count )
	{
		for( uint32_t i = 0; i < count; i++)
		{
			if ( Lux_GetState() != LOADING )
				return false;

			uint32_t display_object_id = current_save_file->ReadUint32WithLabel( "Global Object ID", true );
			MapObject * object = new MapObject();

			object->Restore( current_save_file );
			object->SetStaticMapID( this->object_cache_count, true );

			lux::display->AddObjectToLayer( object->layer, object, true );

			this->object_cache_count = std::max( this->object_cache_count, display_object_id );
		}
	}
	return true;

}



/* Object Handling */

/**
 * @brief GlobalObjects::AddObject
 * @param object
 * @param is_static
 * @return
 */
uint32_t GlobalObjects::AddObject( MapObject * object, bool is_static )
{
	uint32_t ident = 0;
	object->SetData(object->type);

	if ( object->has_data )
	{
		if ( is_static )
		{
			this->object_cache_count++;

			object->SetStaticMapID( this->object_cache_count, true );
			ident = object->GetStaticMapID();

			this->object_cache[ident] = object;

		}
		lux::display->AddObjectToLayer( object->layer, object, is_static );

		return ident;
	}
	return ident;
}

/**
 * @brief GlobalObjects::GetObject
 * @param ident
 * @return
 */
MapObject * GlobalObjects::GetObject( uint32_t ident )
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
 * @brief GlobalObjects::ClearObjects
 */
void GlobalObjects::ClearObjects()
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
 * @brief GlobalObjects::RemoveObject
 * @param ident
 * @return
 */
bool GlobalObjects::RemoveObject(uint32_t ident)
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

/**
 * @brief GlobalObjects::AddCollision
 * @param name
 * @param rect
 */
void GlobalObjects::AddCollision( uint32_t name, CollisionObject * rect)
{
	if ( !rect->added )
	{
		rect->added = true;
		this->collision_list.insert(std::make_pair(name,rect));
	}
}

/**
 * @brief GlobalObjects::CheckCollision
 * @param name
 * @param rect
 */
void GlobalObjects::CheckCollision( uint32_t name, CollisionObject *rect)
{

}

/**
 * @brief GlobalObjects::DrawCollisions
 */
void GlobalObjects::DrawCollisions()
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
				screen_location.x -= MAKE_FIXED_INT(lux::gamesystem->active_map->GetPosition('x') );
				screen_location.y -= MAKE_FIXED_INT(lux::gamesystem->active_map->GetPosition('y') );
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

/**
 * @brief GlobalObjects::ClearCollisions
 */
void GlobalObjects::ClearCollisions()
{
	this->collision_list.clear();
}

/**
 * @brief GlobalObjects::ReturnCollisions
 * @param hits
 * @param entity
 * @param count
 * @param rect
 */
void GlobalObjects::ReturnCollisions( std::vector<CollisionResult*> * hits, uint32_t entity, int16_t count, LuxRect rect )
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

/**
 * @brief GlobalObjects::RemoveCollisions
 * @param name
 */
void GlobalObjects::RemoveCollisions( uint32_t name )
{
	this->collision_list.erase(name);
}
