/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "luxengine.h"
#include "display.h"
#include "mokoi_game.h"
#include "elix_endian.hpp"

ObjectEffect default_fx( (LuxColour){255,255,255,255}, (LuxColour){255,255,255,255} );


/*
int16_t Memory_ReadShort( uint8_t * from, uint32_t offset )
{
	int16_t data;
	memcpy( &data, from + offset, sizeof(int16_t) );
	return elix::endian::host16(data);
}
*/

LuxSprite::LuxSprite( GraphicSystem graphic )
{
	this->graphics = graphic;
	this->data = NULL;

	this->hash = 0;
	this->flag = 0;

	this->mask_value = 0;
	this->mask_file = "";


	this->has_collision_areas = 0;

	this->animated = 0;
	this->animation_timer = 0;
	this->animation_length = 0;
	this->animation_size = 0;

	this->has_sprite_border = false;

	memset(this->collisions, 0, sizeof(this->collisions) );
	memset(this->border, 0, sizeof(this->border) );

}

LuxSprite::~LuxSprite()
{
	if ( this->data )
	{
		this->FreeData( );
	}
	this->_cache.clear();
}

/* Data */
mem_pointer LuxSprite::GetData( ObjectEffect fx )
{
	if ( this->graphics.CacheSprite )
	{
		if ( !(default_fx == fx) )
		{
			return this->graphics.CacheSprite( this, fx );
		}
	}

	return this->data;
}

mem_pointer LuxSprite::GetData(  )
{
	return this->data;
}

void LuxSprite::FreeData(  )
{
	if ( this->graphics.FreeSprite )
	{
		this->graphics.FreeSprite( this );
	}
	else
	{
		if ( this->data )
			lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << "LuxSprite Memory Leak" << std::endl;
	}
}

/* Animation */
void LuxSprite::AddFrame( uint32_t name, int16_t x, int16_t y,	uint32_t ms, uint8_t flip )
{

}

void LuxSprite::AddFrame( LuxSprite * sprite, int16_t x, int16_t y,	uint32_t ms, uint8_t flip )
{
	SpriteFrame frame;
	frame.sprite = sprite;
	frame.name = 0;
	frame.x = x;
	frame.y = y;
	frame.ms = ms;
	frame.f = flip;
	this->frames.push_back( frame );

	this->sheet_area.w = sprite->sheet_area.w > this->sheet_area.w ? sprite->sheet_area.w : this->sheet_area.w;
	this->sheet_area.h = sprite->sheet_area.h > this->sheet_area.h ? sprite->sheet_area.h : this->sheet_area.h;

	this->animation_length += frame.ms;
	this->animation_size++;
}

bool LuxSprite::RemoveFrame( uint32_t name )
{



	return false;
}
