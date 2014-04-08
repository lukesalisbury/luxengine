/****************************
Copyright © 2013 Luke Salisbury
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
#include "map_internal.h"

LuxColour Lux_Hex2Colour(std::string color);


bool MapXMLReader::Load( std::string file )
{

	this->xml_file = MokoiGame_GetXML(file);
	if ( this->xml_file->Error() )
	{
		lux::core->SystemMessage(SYSTEM_MESSAGE_LOG, __FILE__ , __LINE__) << this->xml_file->GetErrorStr1() <<  std::endl;
		return false;
	}

	this->root = this->xml_file->RootElement();
	if ( !this->root || strcmp( this->root->Value(), "map") )
	{
		lux::core->SystemMessage(SYSTEM_MESSAGE_LOG, __FILE__ , __LINE__) << file << " not a valid map file." << std::endl;
		return false;
	}


	return true;
}

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


bool MapXMLReader::ReadEntity( MapObject * object, tinyxml2::XMLElement * object_element, MokoiMap * map )
{
	/* <entity value="%s" language="%s" global="%s"/> */
	bool is_global_entity = false;
	tinyxml2::XMLElement * entity_element = object_element->FirstChildElement("entity");
	if ( entity_element )
	{
		std::string entity_file_name = "";

		tinyxml2::QueryStringAttribute( entity_element, "value", entity_file_name);

		entity_element->QueryBoolAttribute("global", &is_global_entity);

		if ( entity_file_name.length() )
		{
			Entity * new_entity = lux::entities->NewEntity(object->ident, entity_file_name, ( is_global_entity ? 0 : map->Ident() ) );
			if ( new_entity )
			{
				cell_colour temporary_colour_value;

				new_entity->x = MAKE_INT_FIXED(object->position.x);
				new_entity->y = MAKE_INT_FIXED(object->position.y);
				new_entity->z = MAKE_INT_FIXED(object->layer);

				for ( tinyxml2::XMLElement * setting_element = object_element->FirstChildElement("setting"); setting_element; setting_element = setting_element->NextSiblingElement("setting") )
				{
					std::string attrvalue;
					std::string attrkey;
					if ( tinyxml2::QueryStringAttribute(setting_element, "value", attrvalue) == tinyxml2::XML_SUCCESS && tinyxml2::QueryStringAttribute(setting_element, "key", attrkey) == tinyxml2::XML_SUCCESS )
					{
						if ( attrvalue.compare("true") == 0 )
							attrvalue.assign("1");
						else if ( attrvalue.compare("false") == 0 )
							attrvalue.assign("0");
						new_entity->AddSetting( attrkey, attrvalue );
					}
				}

				if ( !is_global_entity )
				{
					new_entity->AddSetting("object-id", object->static_map_id );
					map->object_cache[object->static_map_id] = object;
				}

				new_entity->AddSetting("object-image", object->sprite );
				new_entity->AddSetting("object-type", (uint32_t)object->type );
				new_entity->AddSetting("object-width", (uint32_t)object->position.w);
				new_entity->AddSetting("object-height", (uint32_t)object->position.h);
				new_entity->AddSetting("object-flip", (uint32_t)object->effects.flip_image);
				new_entity->AddSetting("object-style", (uint32_t)object->effects.style);
				new_entity->AddSetting("object-x", object->position.x);
				new_entity->AddSetting("object-y", object->position.y);

				temporary_colour_value.rgba = object->effects.primary_colour;
				new_entity->AddSetting("object-colour-primary", temporary_colour_value.hex );

				temporary_colour_value.rgba = object->effects.secondary_colour;
				new_entity->AddSetting("object-colour-secondary", temporary_colour_value.hex );

			}
		}
	}
	return !is_global_entity;
}


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

MapObject * MapXMLReader::ReadObject( tinyxml2::XMLElement * object_element )
{
	std::string obj_type;

	tinyxml2::XMLElement * effect_element, * position_element, * settings_element;

	MapObject * object = new MapObject();


	tinyxml2::QueryStringAttribute(object_element, "type", obj_type );
	tinyxml2::QueryStringAttribute(object_element, "value", object->sprite );
	tinyxml2::QueryStringAttribute(object_element, "id", object->ident );

	if ( object->ident.length() )
	{
		elix::string::Trim( &object->ident );
	}


	position_element = object_element->FirstChildElement("position");
	effect_element = object_element->FirstChildElement("color");

	if ( position_element )
	{
		uint32_t z_temporary_value = 1;
		uint16_t rotation_temporary_value = 0;
		uint8_t flip_temporary_value = 0;

		object->position.x = position_element->IntAttribute( "x" );
		object->position.y = position_element->IntAttribute( "y" );
		object->position.w = position_element->IntAttribute( "w" );
		object->position.h = position_element->IntAttribute( "h" );
		z_temporary_value = position_element->IntAttribute( "z" );

		/* note: map rotate refer to flip mode not rotation value */
		rotation_temporary_value = position_element->IntAttribute("r"); //Stored in degrees
		tinyxml2::QueryUint8Attribute( position_element, "f", flip_temporary_value);

		if ( z_temporary_value < 10 && z_temporary_value > 0 )
		{
			object->position.z = (uint16_t)(z_temporary_value*1000);
			object->layer = (uint8_t)z_temporary_value;
		}
		else
		{
			object->position.z = (uint16_t)z_temporary_value;
			object->layer = (uint8_t)(z_temporary_value/1000);
		}


		/* note: map rotate refer to flip image not rotation value */
		object->effects.flip_image = (uint8_t)(rotation_temporary_value / 90);
		object->effects.flip_image += ( flip_temporary_value > 0 ? 16 : 0 );
	}

	if ( effect_element )
	{
		tinyxml2::QueryUint8Attribute( effect_element, "red", object->effects.primary_colour.r );
		tinyxml2::QueryUint8Attribute( effect_element, "green", object->effects.primary_colour.g );
		tinyxml2::QueryUint8Attribute( effect_element, "blue", object->effects.primary_colour.b );
		tinyxml2::QueryUint8Attribute( effect_element, "alpha", object->effects.primary_colour.a );
	}

	settings_element = object_element->FirstChildElement("setting");
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
		object->sprite = this->GetTextString(object_element, object->sprite);
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

	this->ReadPath(object, object_element);

	return object;
}

void MapXMLReader::ReadObjects( std::vector<MapObject *> & object_array, uint32_t & object_cache_count, MokoiMap * map )
{
	tinyxml2::XMLElement * child_element;
	for( child_element = root->FirstChildElement("object"); child_element; child_element = child_element->NextSiblingElement("object") )
	{
		if ( strcmp(child_element->Value(), "object") )
			continue;

		MapObject * object = this->ReadObject(child_element);

		/* Check for object and map, cause we could be running this from LuxCanvas */
		if ( object )
		{
			object->static_map_id = ++object_cache_count;
			if ( map )
			{

				if ( this->ReadEntity( object, child_element, map ) )
				{
					if ( object->type == OBJECT_VIRTUAL_SPRITE )
					{
						LuxVirtualSprite * sprite = object->InitialiseVirtual(  );
						sprite->InsertToVector( object, object_array, object_cache_count, map  );
					}
					/* Local object so we add it to the list */
					object_array.push_back( object );
				}
				else
				{
					//we not adding to the list so decrease object_cache_count and delete the object
					object_cache_count--;
					delete object;
				}
			}
			else
			{
				if ( object->type == OBJECT_VIRTUAL_SPRITE )
				{
					LuxVirtualSprite * sprite = object->InitialiseVirtual(  );
					sprite->InsertToVector( object, object_array, object_cache_count, map  );
				}
				/* Local object so we add it to the list */
				object_array.push_back( object );
			}

		}
	}
}

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
		tinyxml2::QueryUint8Attribute( background, "red", map->_colour.r );
		tinyxml2::QueryUint8Attribute( background, "green", map->_colour.g );
		tinyxml2::QueryUint8Attribute( background, "blue", map->_colour.b );
		map->_background.effects.primary_colour = map->_colour;
	}

	tinyxml2::QueryStringAttribute( settings_element, "image", map->_background.sprite );

	if ( settings_element->Attribute("entity") )
	{
		map->_entity.assign( settings_element->Attribute("entity") );
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
