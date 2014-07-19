/****************************
Copyright © 2006-2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef _LUX_SPRITE_H_
#define _LUX_SPRITE_H_

#include <vector>
#include <map>

#include "lux_types.h"
#include "sprite_types.h"
#include "display_types.h"
#include "object_effect.h"


class LuxSprite
{
public:
	LuxSprite(  GraphicSystem graphic );
	~LuxSprite();

private:
	std::map<uint32_t, mem_pointer> _cache;
	mem_pointer data;
	GraphicSystem graphics;

public:
	/* Image Data */
	mem_pointer GetData( ObjectEffect fx );
	mem_pointer GetData( );
	void FreeData(  );


	/* Animation */
	void AddFrame( uint32_t name, int16_t x, int16_t y,	uint32_t ms );
	void AddFrame( LuxSprite * sprite, int16_t x, int16_t y, uint32_t ms );
	bool RemoveFrame( uint32_t name );


public:
	LuxRect sheet_area;

	std::string name;
	std::string parent;
	uint32_t hash;
	uint8_t flag;

	uint8_t mask_value;
	std::string mask_file;


	bool has_collision_areas;
	LuxRect collisions[7];

	/* Sprite Border */
	bool has_sprite_border;
	SpriteBorder border[8];


	/* Animation */
	bool animated;
	std::vector<SpriteFrame> frames;
	uint32_t animation_timer;
	uint32_t animation_length;
	uint32_t animation_size;

};


#endif /* _LUX_SPRITE_H_ */
