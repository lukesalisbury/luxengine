/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

Some Code taken from Easy GL2D http://rel.betterwebber.com
****************************/

/* Required Headers */
#include "core.h"
#include "game_config.h"
#include "engine.h"
#include "display/display.h"
#include "misc_functions.h"
#include "mokoi_game.h"
#include "map_object.h"
#include "platform_functions.h"

#include "display/display_types.h"
#include "graphics_native.h"
#include "display/graphics_system.h"
#include "display_bitfont.h"

#include "elix/elix_string.hpp"
#include "elix/elix_png.hpp"
#include "bitfont.h"


#include <3ds.h>

/* Global Variables */
struct NativeTexture {
	int32_t texnum;
	int32_t address;
	uint32_t w, h, tw, th;
	bool pot, loaded;
	std::string file;
};

GraphicSystem GraphicsNative = {
	&Lux_NATIVE_Init,
	&Lux_NATIVE_Destory,
	&Lux_GRAPHICS_Display2Screen,
	&Lux_GRAPHICS_TextSprites,
	&Lux_GRAPHICS_UpdateRect,
	&Lux_NATIVE_Show,

	&Lux_GRAPHICS_SetRotation,
	&Lux_NATIVE_BackgroundObject,
	&Lux_GRAPHICS_SetFullscreen,
	&Lux_GRAPHICS_Resize,
	&Lux_GRAPHICS_DisplayPointer,

	&Lux_NATIVE_RefreshSpriteSheet,
	&Lux_NATIVE_LoadSpriteSheet,
	&Lux_NATIVE_LoadSpriteSheetImage,
	&Lux_NATIVE_FreeSpriteSheet,
	NULL,
	&Lux_NATIVE_FreeSprite,
	&Lux_NATIVE_PNGtoSprite,

	&Lux_NATIVE_DrawSprite,
	&Lux_NATIVE_DrawRect,
	&Lux_NATIVE_DrawCircle,
	&Lux_GRAPHICS_DrawPolygon,
	&Lux_NATIVE_DrawLine,
	&Lux_NATIVE_DrawText,
	&Lux_GRAPHICS_DrawMessage,

	NULL,
	NULL

};


/* Local Functions */


/* Public Function */

/* Creation,destruction and loop Functions */
bool Lux_NATIVE_Init( uint16_t width, uint16_t height, uint8_t bpp )
{


	return true;
}

void Lux_NATIVE_Destory()
{



}

void Lux_NATIVE_Display2Screen( int32_t * x, int32_t * y )
{
	*x = (int32_t)((float)*x * 1.0);
	*y = (int32_t)((float)*y * 1.0);
}

void Lux_NATIVE_Background(LuxColour fillcolor)
{


}

void Lux_NATIVE_BackgroundObject( MapObject background )
{

}

void Lux_NATIVE_Update(LuxRect rect)
{

}

void Lux_NATIVE_Show()
{
	// Flush and swap framebuffers
	gfxFlushBuffers();
	gfxSwapBuffers();

	//Wait for VBlank
	gspWaitForVBlank();
}

/* Resource Functions */

/* Lux_NATIVE_FreeSprite
 *
 @ sprite:
 -
 */
LUX_DISPLAY_FUNCTION bool Lux_NATIVE_FreeSprite ( LuxSprite * sprite )
{

	return false;
}



/* Lux_NATIVE_CreateSprite
 *
 @ sprite:
 @ rect:
 @ png:
 -
 */
LUX_DISPLAY_FUNCTION bool Lux_NATIVE_CreateSprite( LuxSprite * sprite, LuxRect rect, elix::Image * png )
{
	return false;
}

/* Lux_NATIVE_LoadTexture
 *
 @ sprite:
 @ sprite_filename:
 -
 */
LUX_DISPLAY_FUNCTION bool Lux_NATIVE_LoadTexture( LuxSprite * sprite, std::string sprite_filename )
{
	if ( sprite->data )
		return false;

	return false;

}

/* Lux_NATIVE_LoadSpriteSheet
 *
 @ name:
 @ children:
 -
 */
LUX_DISPLAY_FUNCTION bool Lux_NATIVE_LoadSpriteSheet(std::string name, std::map<uint32_t, LuxSprite *> * children)
{

	return false;
}

/* Lux_NATIVE_LoadSpriteSheetImage
 *
 @ image:
 @ children:
 -
 */
LUX_DISPLAY_FUNCTION bool Lux_NATIVE_LoadSpriteSheetImage( elix::Image * image, std::map<uint32_t, LuxSprite *> * children)
{
	if ( !image  )
	{
		return false;
	}

	if (  !children )
	{
		return false;
	}


	if ( image->HasContent() )
	{
		std::map<uint32_t, LuxSprite *>::iterator p = children->begin();
		for( ; p != children->end(); p++ )
		{
			if ( !p->second->animated )
			{
				Lux_NATIVE_CreateSprite( p->second, p->second->sheet_area, image );
			}
		}
		return true;
	}
	else
	{
		return false;
	}

}

/* Lux_NATIVE_FreeSpriteSheet
 *
 @ children:
 -
 */
LUX_DISPLAY_FUNCTION bool Lux_NATIVE_FreeSpriteSheet( std::map<uint32_t, LuxSprite *> * children)
{
	std::map<uint32_t, LuxSprite *>::iterator p;
	for( p = children->begin(); p != children->end(); p++ )
	{
		Lux_NATIVE_FreeSprite( p->second );
	}
	return true;
}

/* Lux_NATIVE_RefreshSpriteSheet
 *
 @ name:
 @ children:
 -
 */
LUX_DISPLAY_FUNCTION bool Lux_NATIVE_RefreshSpriteSheet( std::string name, std::map<uint32_t, LuxSprite *> * children )
{
	if ( Lux_NATIVE_FreeSpriteSheet( children ) )
		Lux_NATIVE_LoadSpriteSheet( name, children );
	return true;
}

LUX_DISPLAY_FUNCTION LuxSprite * Lux_NATIVE_PNGtoSprite( uint8_t * data, uint32_t size )
{
	return NULL;
}

/* Drawing Functions */
LUX_DISPLAY_FUNCTION void Lux_NATIVE_DrawSprite( LuxSprite * sprite, LuxRect dest_rect, ObjectEffect effects )
{
	if ( !sprite->data )
		return;

}

void Lux_NATIVE_DrawRect( LuxRect dest_rect, ObjectEffect effects)
{

}

void Lux_NATIVE_DrawLine( LuxRect points, ObjectEffect effects )
{

}

void Lux_NATIVE_DrawText( std::string text, LuxRect dest_rect, ObjectEffect effects, bool allow_custom )
{

}

void Lux_NATIVE_TextSprites( bool able )
{

}

void Lux_NATIVE_DrawPolygon( int16_t * x_point, int16_t *y_point, uint16_t point_count, ObjectEffect effects, void * texture )
{

}

void Lux_NATIVE_DrawCircle( LuxRect dest_rect, ObjectEffect effects )
{

}



