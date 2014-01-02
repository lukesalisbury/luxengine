/****************************
Copyright © 2006-2011 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include "lux_virtual_sprite.h"
#include "map_xml_reader.h"
#include "core.h"
LuxVirtualSprite::LuxVirtualSprite(  )
{

}

LuxVirtualSprite::LuxVirtualSprite( std::string file )
{
	std::string filename = file.substr(8);
	this->Load(filename);
}

LuxVirtualSprite::~LuxVirtualSprite()
{
	MapObject * object = NULL;
	std::vector<MapObject*>::iterator l_object;
	if ( this->_objects.size() )
	{
		for ( l_object = this->_objects.begin(); l_object != this->_objects.end(); l_object++ )
		{
			object = (*l_object);
			delete object;
		}
	}
}


bool LuxVirtualSprite::Load( std::string file )
{
	MapXMLReader reader;
	uint32_t object_cache_count = 0;

	if ( !reader.Load("./sprites/virtual/" + file + ".xml"))
	{
		lux::core->SystemMessage(SYSTEM_MESSAGE_LOG, __FILE__ , __LINE__) << " sprites/virtual/" + file + ".xml not a valid canvas file." << std::endl;
		return false;
	}

	reader.ReadDimension( this->rect );
	reader.ReadObjects( this->_objects, object_cache_count, NULL );

	return true;
}

bool LuxVirtualSprite::InsertToVector( MapObject * parent, std::vector<MapObject *> & object_array, uint32_t & object_cache_count, MokoiMap * map )
{
	if ( !this->_objects.size() )
		return false;

	MapObject * object = NULL;
	std::vector<MapObject*>::iterator l_object;
	int32_t sx = parent->position.x;
	int32_t sy = parent->position.y;


	for ( l_object = this->_objects.begin(); l_object != this->_objects.end(); l_object++ )
	{
		object = (*l_object);
		if ( object )
		{

/*
			if ( this->dimension_width > 1 || this->dimension_height > 1  )
			{
				while ( sx < x + width )
				{
					while ( sy < y + height )
					{
					}
				}
			}
*/
			object->static_map_id = ++object_cache_count;

			/* Local object so we add it to the list */
			object_array.push_back( object );

		}
	}

	return true;
}


