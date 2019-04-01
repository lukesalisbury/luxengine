/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include "engine.h"
#include "display/display.h"
#include "map_object.h"
#include "map_xml_reader.h"


bool ObjectOnScreen(LuxRect o, uint8_t flipmode, LuxRect s);
bool ObjectSort(MapObject * a, MapObject * b );

LuxCanvas::LuxCanvas(  )
{
	is_virtual = false;
}

LuxCanvas::LuxCanvas( std::string file )
{
	is_virtual = !file.compare( 0, 8, "Virtual:" );

	this->Load(file);
}

LuxCanvas::~LuxCanvas()
{
	if ( this->objects.size() )
	{
		for ( MapObjectListIter l_object = this->objects.begin(); l_object != this->objects.end(); l_object++ )
		{
			delete l_object->second;
		}
	}
}

MapObject * LuxCanvas::FindChild(uint32_t ident)
{
	if ( this->objects.size() )
	{
		MapObjectListIter l_object = this->objects.find(ident);

		if ( l_object != this->objects.end() )
		{
			return l_object->second;
		}
	}
	return nullptr;
}


bool LuxCanvas::Load( std::string file )
{
	MapXMLReader reader;
	std::string file_path;

	if ( is_virtual )
	{
		file_path = "./sprites/virtual/" + file.substr(8) + ".xml";
	}
	else
	{
		file_path = "./maps/" + file + ".xml";
	}

	if ( !reader.Load(file_path))
	{
		lux::core->SystemMessage(__FILE__ , __LINE__, SYSTEM_MESSAGE_ERROR) << " | " + file_path +" not a valid canvas file." << std::endl;
		return false;
	}
	reader.ReadObjects( this->objects, false );

//	this->_objects.sort( ObjectSort );

	return true;
}

bool LuxCanvas::Draw( DisplaySystem * display, int32_t x, int32_t y, int32_t z )
{
	if ( !this->objects.size() )
		return false;

	if ( !display )
		return false;

	MapObject * object = nullptr;
	LuxRect position;

	for ( MapObjectListIter l_object = this->objects.begin(); l_object != this->objects.end(); l_object++ )
	{
		object = l_object->second;
		if ( !object->hidden )
		{
			position = object->position;
			position.x += x;
			position.y += y;
			position.z = z;
			if ( object->type == OBJECT_LINE )
			{
				position.w += x;
				position.h += y;
			}
			display->DrawMapObject( object, position, object->effects );
		}
	}

	return true;
}


