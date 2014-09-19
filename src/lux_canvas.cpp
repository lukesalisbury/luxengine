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
#include "map_object.h"
#include "map_xml_reader.h"


bool ObjectOnScreen(LuxRect o, uint8_t flipmode, LuxRect s);
bool ObjectSort(MapObject * a, MapObject * b );

LuxCanvas::LuxCanvas(  )
{

}

LuxCanvas::LuxCanvas( std::string file )
{
	this->Load(file);
}

LuxCanvas::~LuxCanvas()
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


bool LuxCanvas::Load( std::string file )
{
	MapXMLReader reader;

	if ( !reader.Load("./maps/" + file + ".xml"))
	{
		lux::core->SystemMessage(__FILE__ , __LINE__, SYSTEM_MESSAGE_ERROR) << " | maps/" + file + ".xml not a valid canvas file." << std::endl;
		return false;
	}
	uint32_t object_cache_count = 0;

	reader.ReadObjects( this->_objects, object_cache_count, NULL );

//	this->_objects.sort( ObjectSort );

	return true;
}

bool LuxCanvas::Draw( DisplaySystem * display, int32_t x, int32_t y, int32_t z)
{
	if ( !this->_objects.size() )
		return false;

	if ( !display )
		return false;

	MapObject * object = NULL;
	std::vector<MapObject*>::iterator l_object;
	LuxRect position;

	for ( l_object = this->_objects.begin(); l_object != this->_objects.end(); l_object++ )
	{
		object = (*l_object);
		if ( !object->hidden )
		{
			position = object->position;
			position.x += x;
			position.y += y;
			position.z = z;
			display->DrawMapObject( object, position, object->effects );
		}
	}

	return true;
}


