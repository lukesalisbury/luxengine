/****************************
Copyright (c) 2006-2010 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

/* Required Headers */
#include "luxengine.h"
#include "map_object.h"
#include "mokoi_game.h"
#ifndef 
	#define 
#endif

/* Global Variables */
struct Texture {
	void *  pointer;
	uint32_t w, h, tw, th;
	bool pot, loaded;
	std::string file;
};


 bool Lux_NATIVE_Init( std::string title, uint16_t  width, uint16_t height, uint8_t bpp, uint16_t * actual_width, uint16_t * actual_height, bool fullscreen )
{
	return true;
}

 void Lux_NATIVE_Destory()
{

}

 void Lux_NATIVE_BackgroundObject( MapObject background  )
{

}

 void Lux_NATIVE_UpdateRect(uint8_t screen,LuxRect rect)
{

}

 void Lux_NATIVE_Show( uint8_t screen )
{
}

 bool Lux_NATIVE_LoadSpriteSheet(std::string name, std::map<uint32_t, LuxSprite *> * children)
{
	return false;
}

 bool Lux_NATIVE_FreeSpriteSheet( std::map<uint32_t, LuxSprite *> * children)
{
	return false;
}

 void Lux_NATIVE_DrawSprite(LuxSprite * sprite, LuxRect dest_rect, ObjectEffect effects)
{

}

 void Lux_NATIVE_DrawRect( LuxRect dest_rect, ObjectEffect effects)
{

}


 void Lux_NATIVE_DrawLine( LuxRect points, ObjectEffect effects)
{

}

 void Lux_NATIVE_DrawText( std::string text, LuxRect dest_rect, ObjectEffect effects, bool allow_custom)
{
	uint16_t x = dest_rect.x;
	std::string::iterator object;
	if ( text.empty() )
		return;

	for ( object = text.begin(); object != text.end(); object++ )
	{
		uint8_t utfchar = *object;
		uint32_t cchar = utfchar;
		if (cchar == '\n' || cchar == '\r')
		{
			dest_rect.y += 10;
			x = dest_rect.x;
		}
		else if ( cchar == ' ' )
		{
			x += 8;
		}
		else if ( cchar <= 128 && cchar > 32)
		{
			//Draw Char
			x += 8;
		}
		else if ( cchar < 224 )
			object++;
		else if ( cchar < 240 )
			object +=2;
		else if ( cchar < 245 )
			object +=3;
	}
}

 void Lux_NATIVE_DrawPolygon( int16_t * x_point, int16_t *y_point, uint16_t point_count, ObjectEffect effects, void * texture )
{

}

 void Lux_NATIVE_DrawCircle( LuxRect dest_rect, ObjectEffect effects )
{

}

 LuxSprite * Lux_NATIVE_PNGtoSprite( uint8_t * data, uint32_t size )
{
	return NULL;
}

