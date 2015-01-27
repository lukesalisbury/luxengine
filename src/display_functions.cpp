/****************************
Copyright © 2006-2015 Luke Salisbury
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


uint8_t Lux_HexCharToUint8( char a, char b )
{
	if ( (a >= 'A' && a <= 'F') || (b >= 'a' && b <= 'f') )
		a -= 55;
	else if ( a >= '0' && a <= '9' )
		a -= 48;
	else
		a = 0;

	if ( (b >= 'A' && b <= 'F') || (b >= 'a' && b <= 'f') )
		b -= 55;
	else if ( b >= '0' && b <= '9' )
		b -= 48;
	else
		b = 0;

	return (uint8_t)((a*16) + b);
}



/**
 * @brief Lux_Hex2Int
 * @param color
 * @return
 */
uint32_t Lux_Hex2Int(std::string color)
{
	LuxColour2 colour = { 0xFFFFFFFF };

	if (color.at(0) == '#' && color.length() >= 9 )
	{
		for (uint8_t n = 0; n < 4; n++)
		{
			if( n == 0 )
				colour.r = Lux_HexCharToUint8( color.at((n*2)+1), color.at((n*2)+2) );
			else if( n == 1 )
				colour.g = Lux_HexCharToUint8( color.at((n*2)+1), color.at((n*2)+2) );
			else if( n == 2 )
				colour.b = Lux_HexCharToUint8( color.at((n*2)+1), color.at((n*2)+2) );
			else if( n == 3 )
				colour.a = Lux_HexCharToUint8( color.at((n*2)+1), color.at((n*2)+2) );
		}
	}
	return colour.hex;
}


/**
 * @brief Lux_Hex2Colour
 * @param color
 * @return
 */
LuxColour Lux_Hex2Colour(std::string color)
{
	LuxColour colour = {128, 128, 128, 255};
	if (color.at(0) == '#' && color.length() >= 9 )
	{
		for (uint8_t n = 0; n < 4; n++)
		{
			if( n == 0 )
				colour.r = Lux_HexCharToUint8( color.at((n*2)+1), color.at((n*2)+2) );
			else if( n == 1 )
				colour.g = Lux_HexCharToUint8( color.at((n*2)+1), color.at((n*2)+2) );
			else if( n == 2 )
				colour.b = Lux_HexCharToUint8( color.at((n*2)+1), color.at((n*2)+2) );
			else if( n == 3 )
				colour.a = Lux_HexCharToUint8( color.at((n*2)+1), color.at((n*2)+2) );
		}
	}
	return colour;
}

/**
 * @brief DetermineObjectType
 * @param obj_type
 * @return
 */
uint32_t Lux_DetermineObjectType( std::string obj_type )
{
	uint32_t type = OBJECT_UNKNOWN;

	if ( obj_type == "sprite" )
	{
		type = OBJECT_SPRITE;
	}
	else if ( obj_type == "rect" )
	{
		type = OBJECT_RECTANGLE;
	}
	else if ( obj_type == "line" )
	{
		type = OBJECT_LINE;
	}
	else if (obj_type == "circle")
	{
		type = OBJECT_CIRCLE;
	}
	else if (obj_type == "text")
	{
		type = OBJECT_TEXT;
	}
	else if (obj_type == "polygon")
	{
		type = OBJECT_POLYGON;
	}
	else if (obj_type == "canvas")
	{
		type = OBJECT_CANVAS;
	}

	return type;
}

