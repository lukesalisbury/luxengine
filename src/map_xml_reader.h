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
		MapXMLReader(): xml_file(NULL), object_z_offset(0) {}
		~MapXMLReader() { delete xml_file; }
	private:
		tinyxml2::XMLDocument * xml_file;
		tinyxml2::XMLElement * root;
		uint32_t object_z_offset;

		std::string GetTextString(tinyxml2::XMLElement * object_element, std::string default_text );

		void ReadObjectPosition(tinyxml2::XMLElement* object_element, MapObject* object);
		void ReadObjectEffect(tinyxml2::XMLElement* object_element, MapObject* object);
		void ReadObjectType(tinyxml2::XMLElement* object_element, std::string obj_type, MapObject* object);
		void ReadObjectSetting(tinyxml2::XMLElement* object_element, MapObject* object);

		fixed ReadZ(tinyxml2::XMLElement* position_element );
		uint8_t ReadLayer(tinyxml2::XMLElement* position_element );
		uint8_t ReadObjectFlipmode(tinyxml2::XMLElement *position_element);

		void ReadGlobalEntity(tinyxml2::XMLElement *entity_element, tinyxml2::XMLElement *object_element);
		void ReadLocalEntity(tinyxml2::XMLElement *object_element, MapObject *object, MokoiMap *map);

		void ReadSettings( MokoiMap * map , std::map<std::string, std::string> &settings );
		void ReadPolygon( MapObject * object, tinyxml2::XMLElement * object_element );
		void ReadPath( MapObject * object, tinyxml2::XMLElement * object_element );

		MapObject * ReadObject(tinyxml2::XMLElement * object_element, const bool global, const uint32_t counter );
		void ReadDimension( LuxRect & rect );
public:
		bool Load( std::string filename );

		void ReadObjectsWithEntities( MapObjectList & object_array, MokoiMap * map );
		void ReadObjects( MapObjectList & object_array, bool is_global );





};

#endif // MAP_XML_READER_H
