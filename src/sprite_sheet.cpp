/****************************
Copyright © 2013-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include "sprite_sheet.h"
#include "xml_functions.h"
#include "elix/elix_string.hpp"
#include "display/display.h"


void Lux_Sprite_SetMask( LuxSprite * sprite, tinyxml2::XMLElement * element )
{
	/* Mask Value */
	std::string obj_mask;

	tinyxml2::QueryStringAttribute(element, "mask", obj_mask);

	if ( obj_mask.length() )
	{
		if ( isdigit(obj_mask.at(0)) )
		{
			sprite->mask_value = (uint8_t)elix::string::ToIntU16(obj_mask);
		}
		else
		{
			sprite->mask_file = obj_mask;
		}
	}
}

/**
 * @brief Lux_Sprite_SetCollisions
 * @param sprite
 * @param element
 */
void Lux_Sprite_SetCollisions( LuxSprite * sprite, tinyxml2::XMLElement * element )
{
	/* Collisions */
	if ( !element )
	{
		return;
	}

	uint32_t collision_id = 8;
	for ( ; element; element = element->NextSiblingElement("collision") )
	{
		if ( tinyxml2::QueryUnsignedAttribute(element, "id", collision_id) == tinyxml2::XML_SUCCESS )
		{
			if ( collision_id < 7 )
			{
				sprite->collisions[collision_id].x = element->IntAttribute("x");
				sprite->collisions[collision_id].y = element->IntAttribute("y");
				sprite->collisions[collision_id].w = element->IntAttribute("w");
				sprite->collisions[collision_id].h = element->IntAttribute("h");
				sprite->has_collision_areas = true;
			}
		}
	}
}

/**
 * @brief Lux_Sprite_SetBorderImages
 * @param sprite
 * @param element
 */
void Lux_Sprite_SetBorderImages( LuxSprite * sprite, tinyxml2::XMLElement * element )
{
	/* Border Children */
	if ( !element )
	{
		return;
	}

	std::string image_name;
	uint32_t border_position = 9;
	for ( ; element; element = element->NextSiblingElement("child") )
	{
		if ( tinyxml2::QueryStringAttribute(element, "name", image_name) == tinyxml2::XML_SUCCESS  )
		{
			tinyxml2::QueryUnsignedAttribute(element, "position", border_position);

			if ( border_position < 8 )
			{
				sprite->has_sprite_border = true;
				sprite->border[border_position].repeat = element->IntAttribute("repeat");
				sprite->border[border_position].name = elix::string::Hash(image_name);
			}
		}
	}

}


LuxSheet::LuxSheet( GraphicSystem graphic ) : graphics(graphic), requested(0), loaded(false), parsed(true), failed(false)
{

}

LuxSheet::LuxSheet( std::string file_name, GraphicSystem graphic ) : graphics(graphic), name(file_name), requested(0), loaded(false), parsed(false), failed(false)
{

}

LuxSheet::~LuxSheet()
{
	Unload();

	for( std::map<uint32_t, LuxSprite *>::iterator iter = this->children.begin(); iter != this->children.end(); ++iter )
	{
		delete (*iter).second;
		(*iter).second = nullptr;
	}
	this->children.clear();
}


void LuxSheet::SetName( std::string name )
{
	this->name = name;
}

bool LuxSheet::ParseXML()
{
	bool results = false;
	tinyxml2::XMLDocument * xml_file = nullptr;
	tinyxml2::XMLElement * root, * sprite_element = nullptr, * position_element = nullptr, * collision_element = nullptr, * border_element = nullptr, * frame_element = nullptr;

	xml_file = MokoiGame_GetXML("./sprites/" + this->name + ".xml");
	if ( xml_file->Error() )
	{
		lux::core->SystemMessage(__FILE__ , __LINE__, SYSTEM_MESSAGE_ERROR) << " | " << xml_file->GetErrorStr1() << " : " << xml_file->GetErrorStr2 () << std::endl;
		goto function_exit;
	}

	root = xml_file->RootElement();
	if ( !root || strcmp(root->Value(), "sheet") )
	{
		lux::core->SystemMessage(__FILE__ , __LINE__, SYSTEM_MESSAGE_ERROR) << " | sprites/" + this->name + ".xml not a valid sheet file." << std::endl;
		goto function_exit;
	}

	/* Sprites */
	for ( sprite_element = root->FirstChildElement("sprite"); sprite_element; sprite_element = sprite_element->NextSiblingElement("sprite") )
	{
		LuxSprite * t_sprite = new LuxSprite( graphics );
		std::string obj_name = "";


		if ( tinyxml2::QueryStringAttribute(sprite_element, "name", obj_name) == tinyxml2::XML_NO_ATTRIBUTE )
		{
			delete t_sprite;
		}
		else
		{
			t_sprite->name = obj_name;
			t_sprite->hash = elix::string::Hash( obj_name );
			t_sprite->parent = this->name;

			position_element = sprite_element->FirstChildElement("position");
			border_element = sprite_element->FirstChildElement("child");
			collision_element = sprite_element->FirstChildElement("collision");

			/* Sheet Area */
			if ( !position_element )
			{
				lux::core->SystemMessage( SYSTEM_MESSAGE_ERROR, "Sprite missing position on sheet.");
				delete t_sprite;
				break;
			}
			else
			{
				t_sprite->sheet_area.x = position_element->IntAttribute("x");
				t_sprite->sheet_area.y = position_element->IntAttribute("y");
				t_sprite->sheet_area.w = position_element->IntAttribute("w");
				t_sprite->sheet_area.h = position_element->IntAttribute("h");
			}

			Lux_Sprite_SetMask( t_sprite, sprite_element );
			Lux_Sprite_SetCollisions( t_sprite, collision_element );
			Lux_Sprite_SetBorderImages( t_sprite, border_element );

			std::pair<uint32_t, LuxSprite*> q( t_sprite->hash, t_sprite );
			this->children.insert( q );
			//this->children.insert( std::make_pair<uint32_t, LuxSprite*>( t_sprite->hash, t_sprite ) );
			//this->children.emplace( t_sprite->hash, t_sprite );
		}
	}

	for ( sprite_element = root->FirstChildElement("animation"); sprite_element; sprite_element = sprite_element->NextSiblingElement("animation") )
	{

		LuxSprite * t_sprite = new LuxSprite( graphics );
		std::string obj_name = "";

		if ( tinyxml2::QueryStringAttribute(sprite_element, "name", obj_name) == tinyxml2::XML_NO_ATTRIBUTE )
		{
			delete t_sprite;
		}
		else
		{
			t_sprite->name = obj_name;
			t_sprite->hash = elix::string::Hash( obj_name );
			t_sprite->animated = true;
			t_sprite->parent = this->name;

			frame_element = sprite_element->FirstChildElement("frame");
			border_element = sprite_element->FirstChildElement("child");
			collision_element = sprite_element->FirstChildElement("collision");

			Lux_Sprite_SetMask( t_sprite, sprite_element );
			Lux_Sprite_SetCollisions( t_sprite, collision_element );
			Lux_Sprite_SetBorderImages( t_sprite, border_element );

			t_sprite->sheet_area.x = 0;
			t_sprite->sheet_area.y = 0;
			t_sprite->sheet_area.w = 0;
			t_sprite->sheet_area.h = 0;

			/* Frames */
			if ( frame_element )
			{
				int32_t x = 0;
				int32_t y = 0;
				uint32_t ms = 0;
				uint8_t flip = 0;
				std::string frame_name;
				std::map<uint32_t, LuxSprite *>::iterator frame_sprite;
				for ( ; frame_element; frame_element = frame_element->NextSiblingElement("frame") )
				{

					if ( tinyxml2::QueryStringAttribute(frame_element, "sprite", frame_name) == tinyxml2::XML_SUCCESS )
					{
						frame_sprite = this->children.find( elix::string::Hash(frame_name) );
						if ( frame_sprite != this->children.end() )
						{
							tinyxml2::QueryIntAttribute( frame_element, "x", x, 0 );
							tinyxml2::QueryIntAttribute( frame_element, "y", y, 0 );
							tinyxml2::QueryUnsignedAttribute( frame_element, "ms", ms );
							tinyxml2::QueryUint8Attribute( frame_element, "f", flip );
							t_sprite->AddFrame( frame_sprite->second, x, y, ms, flip );
						}
						else
						{
							lux::core->SystemMessage( SYSTEM_MESSAGE_ERROR, "Animation frame not found.");
						}
					}

				}
			}

			//this->children.insert( std::make_pair<uint32_t, LuxSprite*>( t_sprite->hash, t_sprite ) );
			this->children.emplace( t_sprite->hash, t_sprite );
		}
	}
	/*
	 *	<sheet xmlns="http://mokoi.info/format/sheet">
		<sprite name="process_0" hidden="hidden" mask="" entity="">
			<position x="74" y="38" w="16" h="16" />
		</sprite>
		<sprite name="process_1" mask="" entity="">
			<position x="74" y="38" w="16" h="16" />
			<child name="process_0" position="0" repeat="0"/>
			<collision id="0" x="0" y="0" w="16" h="16"/>
		</sprite>
		<animation name="process_0" hidden="hidden" mask="" entity="">
			<frame sprite="process_0" x="0" y="0" ms="100"/>
			<frame sprite="process_1" x="0" y="0" ms="100"/>
			<frame sprite="process_2" x="0" y="0" ms="100"/>
		</animation>
	</sheet>
	*/

	results = true;

	function_exit:
	delete xml_file;

	return results;
}

void LuxSheet::Print()
{
	lux::core->SystemMessage(SYSTEM_MESSAGE_LOG) << "----------------------------" << std::endl;
	lux::core->SystemMessage(SYSTEM_MESSAGE_LOG) << "-" << this->name << std::endl;
	lux::core->SystemMessage(SYSTEM_MESSAGE_LOG) << "----------------------------" << std::endl;
	for( std::map<uint32_t, LuxSprite *>::iterator iter = this->children.begin(); iter != this->children.end(); ++iter )
	{
		LuxSprite  * s = (*iter).second;
		lux::core->SystemMessage(SYSTEM_MESSAGE_LOG) << "(" << (*iter).first << ")" << s->name << " [" << s->sheet_area.x << "x" << s->sheet_area.y << "] w:"<< s->sheet_area.w << " h:" << s->sheet_area.h << std::endl;
	}
	lux::core->SystemMessage(SYSTEM_MESSAGE_LOG) << "----------------------------" << std::endl;
}

bool LuxSheet::ParseSimpleText(const std::string content )
{
	std::vector<std::string> info_array;

	elix::string::Split(content, "\t", &info_array);
	if ( info_array.size() < 5 )
	{
		info_array.clear();

		return false;
	}

	/* SpriteSheet Format (Tab separated values)
	#Sprite Name, X, Y, Width, Height, Notes
	*/

	LuxSprite * t_sprite = new LuxSprite( graphics );

	t_sprite->sheet_area.x = elix::string::ToInt32(info_array[1]);
	t_sprite->sheet_area.y = elix::string::ToInt32(info_array[2]);
	t_sprite->sheet_area.w = elix::string::ToIntU16(info_array[3]);
	t_sprite->sheet_area.h = elix::string::ToIntU16(info_array[4]);

	//this->children.insert( this->children.end(), std::make_pair<uint32_t, LuxSprite*>( elix::string::Hash(info_array[0]), t_sprite ) );
	this->children.emplace( elix::string::Hash(info_array[0]), t_sprite );

	info_array.clear();

	this->parsed = true;

	return true;
}



bool LuxSheet::Ref()
{
	if ( !this->loaded )
	{
		this->Load();
	}
	this->requested++;

	return (this->requested);
}

bool LuxSheet::Unref()
{
	if ( this->requested > 0 )
		this->requested--;

	if ( this->requested == 0 )
	{
		if ( this->loaded )
		{
			this->Unload();
		}
	}
	return (this->requested);
}

bool LuxSheet::Load()
{
	if ( !this->name.length() )
	{
		int q =0;
	}
	if ( !this->loaded || !this->failed )
	{
		if ( !this->parsed )
		{
			this->ParseXML();
		}
		if ( this->graphics.LoadSpriteSheet(this->name, &this->children) )
		{
			//lux::core->SystemMessage(SYSTEM_MESSAGE_LOG) << "Loading Sheet '" << this->name << "'" << this->requested << std::endl;
			this->loaded = true;

			this->failed = false;
		}
		else
		{
			lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR) << "Failed loading Spritesheet '" << name << "'" << std::endl;
			this->failed = true;
		}
	}
	return this->loaded;
}

bool LuxSheet::LoadFromImage( elix::Image * image )
{
	if ( this->graphics.LoadSpriteSheetWithImage(image, &this->children) )
	{
		this->loaded = true;
		this->requested = 1;
	}
	else
	{
		this->failed = true;
	}
	return this->loaded;
}


bool LuxSheet::Refresh(GraphicSystem graphics)
{
	this->Unload();
	this->graphics = graphics; //Replace graphics system with latest one.
	this->Load();

	return !this->failed;
}

bool LuxSheet::Unload()
{
	if ( !this->name.length() )
	{
		int q =0;
	}
	lux::core->SystemMessage(SYSTEM_MESSAGE_LOG) << "Freeing sheet '" << this->name << "' (" << this->requested << ")" << std::endl;
	this->graphics.FreeSpriteSheet( &this->children );
	this->loaded = false;
	this->requested = 0;
	return true;
}


LuxSprite * LuxSheet::GetSprite( std::string name )
{
	if ( !this->loaded )
		this->Load();

	uint32_t name_hash = elix::string::Hash(name);
	LuxSpriteIter iter = this->children.find(name_hash);
	if( iter != this->children.end() )
	{
		this->Ref();
		return (LuxSprite *)(iter->second);
	}

	return nullptr;
}

LuxSprite * LuxSheet::GetSprite( uint32_t name_hash )
{
	if ( !this->loaded )
		this->Load();

	LuxSpriteIter iter = this->children.find(name_hash);
	if( iter != this->children.end() )
	{
		this->Ref();
		return (LuxSprite *)(iter->second);
	}

	return nullptr;
}
