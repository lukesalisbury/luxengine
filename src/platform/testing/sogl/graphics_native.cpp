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



#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include "gles/gles.hpp"


#ifndef LUX_DISPLAY_FUNCTION
	#define LUX_DISPLAY_FUNCTION
#endif

void Lux_GLES_LoadFont();
void Lux_GLES_UnloadFont();

/* Global Variables */
LuxColour gles_graphics_colour = { 0, 0, 0, 255 };

/* Local Function */


/* Global Function */
LUX_DISPLAY_FUNCTION bool Lux_NATIVE_Init(uint16_t  width, uint16_t height, uint8_t bpp, uint16_t * actual_width, uint16_t * actual_height, bool fullscreen )
{
/*
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//glOrthof(0.0f, (float)width, (float)height, 0, -10.0f, 10.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
*/
	Lux_GLES_LoadFont();
	return true;
}

LUX_DISPLAY_FUNCTION void Lux_NATIVE_Destory()
{
	Lux_GLES_UnloadFont();
}

LUX_DISPLAY_FUNCTION void Lux_NATIVE_BackgroundObject( MapObject background  )
{
	gles_graphics_colour = background.effects.primary_colour;
	glClearColor((float)gles_graphics_colour.r / 255.0f, (float)gles_graphics_colour.g / 255.0f, (float)gles_graphics_colour.b / 255.0f, 1.0f);
}

LUX_DISPLAY_FUNCTION void Lux_NATIVE_UpdateRect(uint8_t screen,LuxRect rect)
{

}

LUX_DISPLAY_FUNCTION void Lux_NATIVE_Show( uint8_t screen )
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

LUX_DISPLAY_FUNCTION bool Lux_NATIVE_LoadSpriteSheet(std::string name, std::map<uint32_t, LuxSprite *> * children)
{
	return false;
}

LUX_DISPLAY_FUNCTION bool Lux_NATIVE_FreeSpriteSheet( std::map<uint32_t, LuxSprite *> * children)
{
	return false;
}

LUX_DISPLAY_FUNCTION LuxSprite * Lux_NATIVE_PNGtoSprite( uint8_t * data, uint32_t size )
{
	return NULL;
}


#include "gles/draw_vertex2.inc.cpp"

