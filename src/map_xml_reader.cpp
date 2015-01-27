/****************************
Copyright © 2013-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "map_xml_reader.h"
#include "engine.h"
#include "elix_string.hpp"
#include "mokoi_game.h"
#include "map_object.h"
#include "masks.h"
#include "map.h"
#include "xml_functions.h"
#include "entity.h"
#include "entity_manager.h"
#include "config.h"
#include "core.h"

#include "display_functions.h"



/**
 * @brief MapXMLReader::Load
 * @param file
 * @return
 */
bool MapXMLReader::Load( std::string file )
{
	this->xml_file = MokoiGame_GetXML(file);
	if ( this->xml_file->Error() )
	{
		lux::core->SystemMessage(__FILE__ , __LINE__, SYSTEM_MESSAGE_LOG) << this->xml_file->GetErrorStr1() <<  std::endl;
		return false;
	}

	this->root = this->xml_file->RootElement();
	if ( !this->root || strcmp( this->root->Value(), "map") )
	{
		lux::core->SystemMessage(__FILE__ , __LINE__, SYSTEM_MESSAGE_LOG) << file << " not a valid map file." << std::endl;
		return false;
	}


	return true;
}

/**
 * @brief MapXMLReader::GetTextString
 * @param object_element
 * @param default_text
 * @return
 */
std::string MapXMLReader::GetTextString( tinyxml2::XMLElement * object_element, std::string default_text )
{
	int32_t language_string_id = -1;
	std::string result = default_text;

	tinyxml2::XMLElement * setting_element;
	for ( setting_element = object_element->FirstChildElement("setting"); setting_element; setting_element = setting_element->NextSiblingElement("setting") )
	{
		std::string attrkey;
		if (  tinyxml2::QueryStringAttribute(setting_element, "key", attrkey) == tinyxml2::XML_SUCCESS )
		{
			if ( !attrkey.compare("text-string") )
			{
				tinyxml2::QueryIntAttribute( setting_element, "value", language_string_id, -1 );

				if ( language_string_id > -1 ) // -1: Use default text instead of gettig from the language;
				{
					result = lux::engine->GetString( (uint32_t)language_string_id );
				}

			}
		}
	}
	return result;
}

/**
 * @brief MapXMLReader::ReadPolygon
 * @param object
 * @param object_element
 */
void MapXMLReader::ReadPolygon( MapObject * object, tinyxml2::XMLElement * object_element )
{
	uint16_t points = 0;
	uint16_t point_count = 0;
	tinyxml2::XMLElement * point_element;

	for ( point_element = object_element->FirstChildElement("point"); point_element; point_element = point_element->NextSiblingElement("point") )
	{
		points++;
	}

	if ( points )
	{
		LuxPolygon * polygon = new LuxPolygon(points);

		for ( point_element = object_element->FirstChildElement("point"); point_element; point_element = point_element->NextSiblingElement("point") )
		{
			int16_t x = 0, y = 0;

			x = point_element->IntAttribute( "x" );
			y = point_element->IntAttribute( "y" );

			polygon->SetPoint(point_count, x, y);

			point_count++;
		}

		object->type = OBJECT_POLYGON;
		object->SetPolygon( polygon );

	}

}

/**
 * @brief MapXMLReader::ReadEntity
 * @param object
 * @param object_element
 * @param map
 * @return
 */
void MapXMLReader::ReadLocalEntity( tinyxml2::XMLElement * object_element, MapObject * object, MokoiMap * map )
{
	/* <entity value="%s" language="%s" global="%s"/> */

	tinyxml2::XMLElement * entity_element = object_element->FirstChildElement("entity");
	if ( entity_element )
	{
		std::string entity_file_name = "";

		tinyxml2::QueryStringAttribute( entity_element, "value", entity_file_name);

		if ( entity_file_name.length() )
		{
			Entity * current_entity = lux::entities->NewEntity(object->ident, entity_file_name, ( map ? map->Ident() : 0 ) );
			if ( current_entity )
			{
				cell_colour temporary_colour_value;

				current_entity->x = MAKE_INT_FIXED(object->position.x);
				current_entity->y = MAKE_INT_FIXED(object->position.y);
				current_entity->z_layer = MAKE_INT_FIXED(object->layer);

				for ( tinyxml2::XMLElement * setting_element = object_element->FirstChildElement("setting"); setting_element; setting_element = setting_element->NextSiblingElement("setting") )
				{
					std::string attrvalue;
					std::string attrkey;
					if ( tinyxml2::QueryStringAttribute(setting_element, "value", attrvalue) == tinyxml2::XML_SUCCESS && tinyxml2::QueryStringAttribute(setting_element, "key", attrkey) == tinyxml2::XML_SUCCESS )
					{
						if ( attrvalue.compare("true") == 0 )
							current_entity->AddSetting( attrkey, 1 );
						else if ( attrvalue.compare("false") == 0 )
							current_entity->AddSetting( attrkey, 0 );
						else if ( elix::string::HasPrefix(attrkey, "object-colour-") )
							current_entity->AddSetting( attrkey, Lux_Hex2Int(attrvalue) );
						else
							current_entity->AddSetting( attrkey, attrvalue );
					}
				}

				current_entity->AddSetting("object-id", object->GetStaticMapID() );
				current_entity->AddSetting("object-content", object->sprite );
				current_entity->AddSetting("object-type", (uint32_t)object->type );
				current_entity->AddSetting("object-width", (uint32_t)object->position.w);
				current_entity->AddSetting("object-height", (uint32_t)object->position.h);
				current_entity->AddSetting("object-flip", (uint32_t)object->effects.flip_image);
				current_entity->AddSetting("object-style", (uint32_t)object->effects.style);

				temporary_colour_value.rgba = object->effects.primary_colour;
				current_entity->AddSetting("object-colour-primary", temporary_colour_value.hex );


			}
		}
	}
}

/**
 * @brief MapXMLReader::ReadGlobalEntity
 * @param entity_element
 * @param object_element
 */
void MapXMLReader::ReadGlobalEntity( tinyxml2::XMLElement * entity_element, tinyxml2::XMLElement * object_element )
{
	/* <entity value="%s" language="%s" global="%s"/> */
	std::string entity_file_name;
	std::string object_type, object_content, object_ident;

	Entity * current_entity = NULL;
	tinyxml2::XMLElement * position_element;

	if ( tinyxml2::QueryStringAttribute( entity_element, "value", entity_file_name) == tinyxml2::XML_SUCCESS )
	{
		tinyxml2::QueryStringAttribute( object_element, "id", object_ident );

		current_entity = lux::entities->NewEntity( object_ident, entity_file_name, 0 );
		if ( current_entity )
		{
			for ( tinyxml2::XMLElement * setting_element = object_element->FirstChildElement("setting"); setting_element; setting_element = setting_element->NextSiblingElement("setting") )
			{
				std::string attrvalue;
				std::string attrkey;
				if ( tinyxml2::QueryStringAttribute(setting_element, "value", attrvalue) == tinyxml2::XML_SUCCESS && tinyxml2::QueryStringAttribute(setting_element, "key", attrkey) == tinyxml2::XML_SUCCESS )
				{
					if ( attrvalue.compare("true") == 0 )
						current_entity->AddSetting( attrkey, 1 );
					else if ( attrvalue.compare("false") == 0 )
						current_entity->AddSetting( attrkey, 0 );
					else if ( elix::string::HasPrefix(attrkey, "object-colour-") )
						current_entity->AddSetting( attrkey, Lux_Hex2Int(attrvalue) );
					else
						current_entity->AddSetting( attrkey, attrvalue );
				}
			}

			if ( object_element )
			{
				tinyxml2::QueryStringAttribute(object_element, "type", object_type );
				tinyxml2::QueryStringAttribute(object_element, "value", object_content );

				current_entity->AddSetting("object-content", object_content );
				current_entity->AddSetting("object-type", Lux_DetermineObjectType(object_type) );

				/* */
				position_element = object_element->FirstChildElement("position");
				if ( position_element )
				{
					current_entity->x = MAKE_INT_FIXED(position_element->IntAttribute( "x" ));
					current_entity->y = MAKE_INT_FIXED(position_element->IntAttribute( "y" ));
					ReadZLayer( position_element, current_entity->z_layer );

					current_entity->AddSetting("object-width", position_element->IntAttribute( "w" ) );
					current_entity->AddSetting("object-height", position_element->IntAttribute( "h" ) );

					current_entity->AddSetting("object-flip", this->ReadObjectFlipmode(position_element) );
				}

			}
		}

	}
}

/**
 * @brief MapXMLReader::ReadPath
 * @param object
 * @param object_element
 */
void MapXMLReader::ReadPath( MapObject * object, tinyxml2::XMLElement * object_element )
{
	tinyxml2::XMLElement * path_element = object_element->FirstChildElement("path");
	if ( path_element )
	{
		for ( tinyxml2::XMLElement * point_element = path_element->FirstChildElement("point"); point_element; point_element = point_element->NextSiblingElement("point") )
		{
			int32_t point;
			LuxPath points = { 0, 0, 0 };

			points.x = point_element->IntAttribute( "x" );
			points.y = point_element->IntAttribute( "y" );
			points.ms_length = point_element->IntAttribute( "time" );

			object->_path.push_back( points );
		}

		object->path_current_x = MAKE_INT_FIXED(object->position.x);
		object->path_current_y = MAKE_INT_FIXED(object->position.y);
	}
}

/**
 * @brief MapXMLReader::ReadZLayer
 * @param position_element
 * @param z
 * @param layer
 */
uint16_t MapXMLReader::ReadZLayer(tinyxml2::XMLElement* position_element, uint8_t & layer )
{
	uint16_t z = 1;
	uint32_t z_temporary_value = position_element->IntAttribute( "z" );
	if ( z_temporary_value < 10 && z_temporary_value > 0 )
	{
		z = (uint16_t)(z_temporary_value*1000) + (object_count_z%1000);
		layer = (uint8_t)z_temporary_value;
	}
	else
	{
		z = (uint16_t)z_temporary_value  + (object_count_z%1000);;
		layer = (uint8_t)(z_temporary_value/1000);
	}
	return z;
}

/**
 * @brief MapXMLReader::ReadObjectFlipmode
 * @param position_element
 */
uint8_t MapXMLReader::ReadObjectFlipmode(tinyxml2::XMLElement* position_element)
{
	if ( position_element )
	{
		uint16_t rotation_temporary_value = 0;
		uint8_t flip_temporary_value = 0;

		/* note: map rotate refer to flip mode not rotation value */
		rotation_temporary_value = position_element->IntAttribute("r"); //Stored in degrees
		tinyxml2::QueryUint8Attribute( position_element, "f", flip_temporary_value);

		return (uint8_t)(rotation_temporary_value / 90) + ( flip_temporary_value > 0 ? 16 : 0 );
	}
	return 0;
}

/**
 * @brief MapXMLReader::ReadObjectPosition
 * @param object_element
 * @param object
 */
void MapXMLReader::ReadObjectPosition(tinyxml2::XMLElement* object_element, MapObject* object)
{
	tinyxml2::XMLElement* position_element = object_element->FirstChildElement("position");
	if ( position_element )
	{
		object->position.x = position_element->IntAttribute( "x" );
		object->position.y = position_element->IntAttribute( "y" );
		object->position.w = position_element->IntAttribute( "w" );
		object->position.h = position_element->IntAttribute( "h" );
		object->position.z = this->ReadZLayer(position_element, object->layer );
		object->effects.flip_image = this->ReadObjectFlipmode( position_element);
	}
}

/**
 * @brief MapXMLReader::ReadObjectEffect
 * @param object_element
 * @param object
 */
void MapXMLReader::ReadObjectEffect(tinyxml2::XMLElement* object_element, MapObject* object)
{
	tinyxml2::XMLElement* effect_element = object_element->FirstChildElement("color");
	if ( effect_element )
	{
		tinyxml2::QueryUint8Attribute( effect_element, "red", object->effects.primary_colour.r );
		tinyxml2::QueryUint8Attribute( effect_element, "green", object->effects.primary_colour.g );
		tinyxml2::QueryUint8Attribute( effect_element, "blue", object->effects.primary_colour.b );
		tinyxml2::QueryUint8Attribute( effect_element, "alpha", object->effects.primary_colour.a );
	}
}

/**
 * @brief MapXMLReader::ReadObjectType
 * @param object_element
 * @param obj_type
 * @param object
 */
void MapXMLReader::ReadObjectType(tinyxml2::XMLElement* object_element, std::string obj_type, MapObject* object)
{
	object->type = OBJECT_UNKNOWN;

	if ( obj_type == "sprite" )
	{
		if ( !object->sprite.compare( 0, 8, "Virtual:" ) )
		{
			object->type = OBJECT_VIRTUAL_SPRITE;
		}
		else
		{
			object->type = OBJECT_SPRITE;
		}
	}
	else if ( obj_type == "rect" )
	{
		object->type = OBJECT_RECTANGLE;
	}
	else if ( obj_type == "line" )
	{
		object->type = OBJECT_LINE;
	}
	else if (obj_type == "circle")
	{
		object->type = OBJECT_CIRCLE;
	}
	else if (obj_type == "text")
	{
		object->type = OBJECT_TEXT;
		object->sprite = this->GetTextString( object_element, object->sprite );
	}
	else if (obj_type == "polygon")
	{
		this->ReadPolygon( object, object_element );
	}
	else if (obj_type == "canvas")
	{

	}
	else
	{
		object->type = OBJECT_TEXT;
		object->sprite = "Unknown Type";
	}
}

/**
 * @brief MapXMLReader::ReadObjectSetting
 * @param object_element
 * @param object
 */
void MapXMLReader::ReadObjectSetting( tinyxml2::XMLElement* object_element, MapObject* object)
{
	tinyxml2::XMLElement* settings_element = object_element->FirstChildElement("setting");
	if ( settings_element )
	{
		for ( ; settings_element; settings_element = settings_element->NextSiblingElement("setting") )
		{
			std::string attrkey = "";

			if ( tinyxml2::QueryStringAttribute(settings_element, "key", attrkey) == tinyxml2::XML_SUCCESS )
			{
				if ( attrkey.compare("object-style") == 0 )
				{
					/* return char value instead of a number */
					tinyxml2::QueryUint8Attribute( settings_element, "value", object->effects.style );
				}
				else if ( !attrkey.compare("object-colour-secondary") )
				{
					std::string attrvalue = "#FFFFFFFF";
					tinyxml2::QueryStringAttribute(settings_element, "value", attrvalue);
					object->effects.secondary_colour = Lux_Hex2Colour(attrvalue);
				}
			}
		}
	}
}

/**
 * @brief MapXMLReader::ReadObject
 * @param object_element
 * @return
 */
MapObject * MapXMLReader::ReadObject( tinyxml2::XMLElement * object_element, const bool global, const uint32_t counter )
{
	std::string obj_type;

	MapObject * object = new MapObject();

	tinyxml2::QueryStringAttribute(object_element, "type", obj_type );
	tinyxml2::QueryStringAttribute(object_element, "value", object->sprite );
	tinyxml2::QueryStringAttribute(object_element, "id", object->ident );

	if ( object->ident.length() )
	{
		elix::string::Trim( &object->ident );
		object->ident_hash = elix::string::Hash( object->ident );
	}
	object->SetStaticMapID( counter, global );

	this->ReadObjectPosition( object_element, object );
	this->ReadObjectEffect( object_element, object );
	this->ReadObjectSetting( object_element, object );
	this->ReadObjectType( object_element, obj_type, object );
	this->ReadPath( object, object_element);

	return object;
}

/**
 * @brief MapXMLReader::ReadObjects
 * @param object_array
 * @param object_cache_count
 * @param map
 */
void MapXMLReader::ReadObjectsWithEntities( MapObjectList & object_array, MokoiMap * map )
{
	uint32_t object_cache_count = 0;
	tinyxml2::XMLElement * child_element = NULL;
	tinyxml2::XMLElement * entity_element = NULL;

	for( child_element = root->FirstChildElement("object"); child_element; child_element = child_element->NextSiblingElement("object") )
	{
		if ( strcmp(child_element->Value(), "object") )
			continue;

		MapObject * object = NULL;
		bool is_global_object = ( map ? false : true );

		/* Check to see if object is global
		 * As Object on a map is a global entity, we don't create a Object
		 */
		entity_element = child_element->FirstChildElement("entity");
		if ( entity_element )
		{
			entity_element->QueryBoolAttribute("global", &is_global_object);
		}

		if ( is_global_object && entity_element )
		{
			this->ReadGlobalEntity( entity_element, child_element );
		}
		else
		{
			object = this->ReadObject(child_element, is_global_object, ++object_cache_count);
			if ( object )
			{
				this->ReadLocalEntity( child_element, object, NULL );

				if ( object->type == OBJECT_VIRTUAL_SPRITE )
				{
					LuxVirtualSprite * sprite = object->InitialiseVirtual( );
					sprite->PushObjectsToMap( object, object_array, object_cache_count, is_global_object  );
				}

				/* Local object so we add it to the list */
				object_array.insert( MAP_OBJECT_PAIR( object->GetStaticMapID(), object ) );
				++object_count_z;
			}
		}

	}
}

/**
 * @brief Used by Canvas and Virtual Sprite
 * @param object_array
 * @param is_global
 */
void MapXMLReader::ReadObjects(MapObjectList & object_array, bool is_global )
{
	uint32_t object_cache_count = 0;
	tinyxml2::XMLElement * child_element = NULL;

	for( child_element = root->FirstChildElement("object"); child_element; child_element = child_element->NextSiblingElement("object") )
	{
		if ( strcmp(child_element->Value(), "object") )
			continue;

		MapObject * object = this->ReadObject(child_element, is_global, ++object_cache_count);
		if ( object )
		{
			/* Local object so we add it to the list */
			object_array.insert( MAP_OBJECT_PAIR( object->GetStaticMapID(), object ) );

			if ( object->type == OBJECT_VIRTUAL_SPRITE )
			{
				LuxVirtualSprite * sprite = object->InitialiseVirtual( );
				sprite->PushObjectsToMap( object, object_array, object_cache_count, is_global  );
			}

			++object_count_z;
		}
	}
}

/**
 * @brief MapXMLReader::ReadDimension
 * @param rect
 */
void MapXMLReader::ReadDimension( LuxRect & rect )
{
	tinyxml2::XMLElement * settings_element = root->FirstChildElement("settings");

	if ( !settings_element )
	{
		return;
	}

	tinyxml2::XMLElement * dimensions = settings_element->FirstChildElement("dimensions");
	if ( dimensions )
	{
		rect.w = dimensions->UnsignedAttribute( "width" );
		rect.h = dimensions->UnsignedAttribute( "height" );
	}

}

/**
 * @brief MapXMLReader::ReadSettings
 * @param map
 * @param settings
 */
void MapXMLReader::ReadSettings( MokoiMap * map, std::map<std::string, std::string> & settings )
{
	tinyxml2::XMLElement * settings_element = root->FirstChildElement("settings");

	if ( !settings_element )
	{
		return;
	}

	tinyxml2::XMLElement * dimensions = settings_element->FirstChildElement("dimensions");
	tinyxml2::XMLElement * background = settings_element->FirstChildElement("color");
	tinyxml2::XMLElement * options = NULL;

	if ( dimensions )
	{
		map->dimension_width = dimensions->UnsignedAttribute( "width" );
		map->dimension_height = dimensions->UnsignedAttribute( "height" );
		map->map_width = map->dimension_width * map->default_map_width;
		map->map_height = map->dimension_height * map->default_map_height;
	}

	if ( background )
	{
		tinyxml2::QueryUint8Attribute( background, "red", map->base_background_colour.r );
		tinyxml2::QueryUint8Attribute( background, "green", map->base_background_colour.g );
		tinyxml2::QueryUint8Attribute( background, "blue", map->base_background_colour.b );
		map->background_object.effects.primary_colour = map->base_background_colour;
	}

	tinyxml2::QueryStringAttribute( settings_element, "image", map->background_object.sprite );

	if ( settings_element->Attribute("entity") )
	{
		map->entity_file_name.assign( settings_element->Attribute("entity") );
	}

	for ( options = settings_element->FirstChildElement("option"); options; options = options->NextSiblingElement("option") )
	{
		std::string attrkey = "";
		std::string attrvalue = "";
		if ( tinyxml2::QueryStringAttribute(options, "name", attrkey) == tinyxml2::XML_SUCCESS  && tinyxml2::QueryStringAttribute(options, "value", attrvalue) == tinyxml2::XML_SUCCESS)
		{
			settings.insert( std::pair<std::string,std::string>(attrkey, attrvalue) );
		}
	}

}
