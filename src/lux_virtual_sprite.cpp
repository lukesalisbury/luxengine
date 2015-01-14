/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include "lux_virtual_sprite.h"
#include "map_xml_reader.h"
#include "core.h"

#include <map>

/**
 * @brief LuxVirtualSprite::LuxVirtualSprite
 */
LuxVirtualSprite::LuxVirtualSprite(  )
{

}

/**
 * @brief LuxVirtualSprite::LuxVirtualSprite
 * @param file
 */
LuxVirtualSprite::LuxVirtualSprite( std::string file )
{
	std::string filename = file.substr(8); // strip Virtual:
	this->Load( filename );
}

/**
 * @brief LuxVirtualSprite::~LuxVirtualSprite
 */
LuxVirtualSprite::~LuxVirtualSprite()
{
	if ( this->objects.size() )
	{
		for ( MapObjectList::iterator q = this->objects.begin(); q != this->objects.end(); q++ )
		{
			delete q->second;
		}
	}
}

/**
 * @brief LuxVirtualSprite::Load
 * @param file
 * @return
 */
bool LuxVirtualSprite::Load( std::string file )
{
	MapXMLReader reader;
	uint32_t object_cache_count = 0;

	if ( !reader.Load("./sprites/virtual/" + file + ".xml"))
	{
		lux::core->SystemMessage(__FILE__ , __LINE__, SYSTEM_MESSAGE_LOG) << " sprites/virtual/" + file + ".xml not a valid canvas file." << std::endl;
		return false;
	}

	reader.ReadDimension( this->rect );
	reader.ReadObjects( this->objects, object_cache_count, NULL );

	return true;
}

/**
 * @brief LuxVirtualSprite::InsertToVector
 * @param parent
 * @param object_array
 * @param object_cache_count
 * @param global
 * @return
 */
bool LuxVirtualSprite::InsertToVector( MapObject * parent, MapObjectList & object_array, uint32_t & object_cache_count, const bool global )
{
	if ( !this->objects.size() )
		return false;

	MapObject * object = NULL;
	MapObject * cache_object = NULL;
	std::map<uint32_t, MapObject*>::iterator l_object;

	for ( l_object = this->objects.begin(); l_object != this->objects.end(); l_object++ )
	{
		object = l_object->second;
		if ( object )
		{
			if ( this->rect.w > 1 || this->rect.h > 1  )
			{
				for ( int32_t sx = 0; sx < parent->position.w; sx += this->rect.w )
				{
					for ( int32_t sy = 0; sy < parent->position.h; sy += this->rect.h )
					{
						cache_object = object->clone();

						cache_object->position.x += (parent->position.x + sx);
						cache_object->position.y += (parent->position.y + sy);

						cache_object->SetStaticMapID( ++object_cache_count, global );

						object_array.insert( MAP_OBJECT_PAIR( cache_object->GetStaticMapID(), cache_object ) );

						this->cache.push_back( cache_object ); // Add to cache
					}
				}
			}


		}
	}

	return true;
}


