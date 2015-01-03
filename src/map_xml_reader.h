/****************************
Copyright © 2013-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef MAP_XML_READER_H
#define MAP_XML_READER_H


#include "map.h"
#include "xml_functions.h"

class MapXMLReader
{
		MapXMLReader(): xml_file(NULL) {}
		~MapXMLReader() { delete xml_file; }
	private:
		tinyxml2::XMLDocument * xml_file;
		tinyxml2::XMLElement * root;
	public:
		bool Load( std::string filename );
		std::string GetTextString(tinyxml2::XMLElement * object_element , std::string default_text);
		MapObject * ReadObject( tinyxml2::XMLElement * object_element );
		void ReadDimension( LuxRect & rect );
		void ReadObjects(std::vector<MapObject *> & object_array, uint32_t & object_cache_count , MokoiMap *map);
		void ReadSettings( MokoiMap * map , std::map<std::string, std::string> &settings );
		void ReadPolygon( MapObject * object, tinyxml2::XMLElement * object_element );
		void ReadPath( MapObject * object, tinyxml2::XMLElement * object_element );
		bool ReadEntity( MapObject * object, tinyxml2::XMLElement * object_element, MokoiMap * map );
};

#endif // MAP_XML_READER_H
