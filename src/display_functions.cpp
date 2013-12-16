/****************************
Copyright © 2006-2011 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "luxengine.h"
#include "mokoi_game.h"
#include "elix_string.hpp"
#include "elix_endian.hpp"
#include <sstream>
#include <algorithm>
#ifndef toupper
	#include <ctype.h>
#endif

/*
ObjectEffect SetColourOnEffect(uint8_t R,uint8_t G,uint8_t B,uint8_t A)
{
	ObjectEffect new_one = { {R,G,B,A}, {R,G,B,A}, 0, 1000, 1000, 0, 0, STYLE_NORMAL};
	return new_one;
}
*/

LuxColour Lux_Hex2Colour(std::string color)
{
	LuxColour colour = {128, 128, 128, 255};
	std::transform(color.begin(), color.end(), color.begin(), toupper);
	if (color.at(0) == '#' && color.length() >= 9 )
	{
		for (uint8_t n = 0; n < 4; n++)
		{
			uint8_t temp = 0;
			uint8_t hex1 = 0, hex2 = 0;

			temp = color.at((n*2)+1);
			if ( temp >= 48)
				hex1 = temp - 48;
			if ( temp >= 65)
				hex1 = temp - 55;
			if (hex1 > 15 )
				hex1 = 0;

			temp = color.at((n*2)+2);
			if ( temp >= 48)
				hex2 = temp - 48;
			if ( temp >= 65)
				hex2 = temp - 55;
			if (hex2 > 15 )
				hex2 = 0;
			if( n == 0 )
				colour.r = (hex1*16) + hex2;
			else if( n == 1 )
				colour.g = (hex1*16) + hex2;
			else if( n == 2 )
				colour.b = (hex1*16) + hex2;
			else if( n == 3 )
				colour.a = (hex1*16) + hex2;
		}
	}
	return colour;
}


bool Lux_Path_Load(std::string filename, std::vector<LuxPath> * path )
{
	uint8_t * data = NULL;
	uint32_t length, c = 0;
	length = lux::game->GetFile("./paths/" + filename, &data, false);
	if ( length )
	{
		while ( c < length )
		{
			LuxPath point = { 0, 0, 0 };
			memcpy( &point, data + c, 8 );
			point.x = (int16_t)elix::endian::host16((uint16_t)point.x);
			point.y = (int16_t)elix::endian::host16((uint16_t)point.y);
			point.ms_length = elix::endian::host32(point.ms_length);
			path->push_back( point );
			c += 8;
		}
	}
	if ( data )
	{
		delete data;
		return length ? true : false;
	}
	else
		return false;
}

