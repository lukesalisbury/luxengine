/****************************
Copyright © 2013 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include <algorithm>
#include <sstream>
#include "elix_string.hpp"
#include "object_effect.h"

namespace colour {
	extern LuxColour white;
	extern LuxColour black;
	extern LuxColour red;
	extern LuxColour blue;
	extern LuxColour green;
	extern LuxColour yellow;
}

uint8_t HexCharToNumber( char a, char b )
{
	if ( a >= 'A' && a <= 'F' )
		a -= 55;
	else if ( a >= '0' && a <= '9' )
		a -= 48;
	else
		a = 0;

	if ( b >= 'A' && b <= 'F' )
		b -= 55;
	else if ( b >= '0' && b <= '9' )
		b -= 48;
	else
		b = 0;

	return (uint8_t)((a*16) + b);
}


ObjectEffect::ObjectEffect(LuxColour c1, LuxColour c2) : rotation(0), scale_xaxis(1000), scale_yaxis(1000), flip_image(0), tile_object(1), style(STYLE_NORMAL)
{
	this->primary_colour = c1;
	this->secondary_colour = c2;
}

ObjectEffect::ObjectEffect() : rotation(0), scale_xaxis(1000), scale_yaxis(1000),  flip_image(0), tile_object(0), style(STYLE_NORMAL)
{
	this->primary_colour = colour::white;
	this->secondary_colour = colour::white;
}

ObjectEffect::ObjectEffect(LuxColour c1, LuxColour c2, uint8_t style) : rotation(0),  scale_xaxis(1000), scale_yaxis(1000), flip_image(1), tile_object(0)
{
	this->primary_colour = c1;
	this->secondary_colour = c2;
	this->style = style;
}



uint32_t ObjectEffect::Hex()
{
	uint32_t hex = 0;
	std::ostringstream code;

	code.flags(std::ios::hex);

	code << primary_colour.r << primary_colour.b << primary_colour.g << primary_colour.a << "|";
	code << secondary_colour.r << secondary_colour.b << secondary_colour.g << secondary_colour.a << "|";
	code << rotation << scale_xaxis << scale_yaxis << "|";
	code << flip_image << style;

	hex = elix::string::Hash(code.str());
	code.str("");

	return hex;

}

#include <ctype.h>
void ObjectEffect::SetColour( std::string hex_string )
{
	std::transform( hex_string.begin(), hex_string.end(), hex_string.begin(), toupper);
	if ( hex_string.at(0) == '#' && hex_string.length() >= 9 )
	{
		this->primary_colour.r = HexCharToNumber( hex_string.at(1), hex_string.at(2) );
		this->primary_colour.g = HexCharToNumber( hex_string.at(3), hex_string.at(4) );
		this->primary_colour.b = HexCharToNumber( hex_string.at(5), hex_string.at(6) );
		this->primary_colour.a = HexCharToNumber( hex_string.at(7), hex_string.at(8) );
	}

}

void ObjectEffect::SetSecondaryColour( std::string hex_string )
{
	std::transform( hex_string.begin(), hex_string.end(), hex_string.begin(), toupper);
	if ( hex_string.at(0) == '#' && hex_string.length() >= 9 )
	{
		this->secondary_colour.r = HexCharToNumber( hex_string.at(1), hex_string.at(2) );
		this->secondary_colour.g = HexCharToNumber( hex_string.at(3), hex_string.at(4) );
		this->secondary_colour.b = HexCharToNumber( hex_string.at(5), hex_string.at(6) );
		this->secondary_colour.a = HexCharToNumber( hex_string.at(7), hex_string.at(8) );
	}

}


void ObjectEffect::SetColour( LuxColour colour )
{
	this->primary_colour = colour;
}

void ObjectEffect::SetSecondaryColour( LuxColour colour )
{
	this->secondary_colour = colour;
}

bool ObjectEffect::operator == (const ObjectEffect &b) const
{
	if ( this->primary_colour.r !=  b.primary_colour.r || this->primary_colour.b !=  b.primary_colour.b || this->primary_colour.g !=  b.primary_colour.g || this->primary_colour.a !=  b.primary_colour.a )
		return false;
	if ( this->secondary_colour.r !=  b.secondary_colour.r || this->secondary_colour.b !=  b.secondary_colour.b || this->secondary_colour.g !=  b.secondary_colour.g || this->secondary_colour.a !=  b.secondary_colour.a )
		return false;
	if ( this->rotation !=  b.rotation )
		return false;
	if ( this->scale_xaxis !=  b.scale_xaxis )
		return false;
	if ( this->scale_yaxis !=  b.scale_yaxis )
		return false;
	if ( this->flip_image !=  b.flip_image )
		return false;
	if ( this->style !=  b.style )
		return false;
	return true;
}
