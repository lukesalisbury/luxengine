/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef _DISPLAYSDL_H_
#define _DISPLAYSDL_H_
#include "display/display_types.h"
#ifndef LUX_DISPLAY_FUNCTION
#define LUX_DISPLAY_FUNCTION
#endif

bool Lux_NATIVE_Init(  uint16_t  width, uint16_t height, uint8_t bpp, uint16_t * actual_width, uint16_t * actual_height );
void Lux_NATIVE_Destory();
void Lux_NATIVE_UpdateRect(uint8_t screen, LuxRect rect );
void Lux_NATIVE_Show( uint8_t screen );

void Lux_NATIVE_Display2Screen( int32_t * x, int32_t * y );
void Lux_NATIVE_Background( LuxColour fillcolor );
void Lux_NATIVE_BackgroundObject( MapObject background );
void Lux_NATIVE_DisplayPointer( uint8_t player, int16_t x, int16_t y, ObjectEffect effect );
bool Lux_NATIVE_SetFullscreen( bool able );
void Lux_NATIVE_TextSprites( bool able );

void Lux_NATIVE_DrawSprite( LuxSprite * sprite, LuxRect dest_rect, ObjectEffect effects );
void Lux_NATIVE_DrawRect( LuxRect dest_rect, ObjectEffect effects );
void Lux_NATIVE_DrawCircle( LuxRect dest_rect, ObjectEffect effects );
void Lux_NATIVE_DrawPolygon( int16_t * x_point, int16_t * y_point, uint16_t point_count, LuxRect position, ObjectEffect effects, void * texture );
void Lux_NATIVE_DrawLine( LuxRect points, ObjectEffect effects );
void Lux_NATIVE_DrawText( std::string text, LuxRect dest_rect, ObjectEffect effects, bool allow_custom);
void Lux_NATIVE_DrawMessage( std::string message, uint8_t alignment );


bool Lux_NATIVE_CreateSprite( LuxSprite * sprite, LuxRect rect, elix::Image * png );
bool Lux_NATIVE_FreeSprite( LuxSprite * sprite );

//void * Lux_NATIVE_CacheSprite ( LuxSprite * sprite, ObjectEffect fx );

#include "display/reusable_graphics_system.hpp"

#endif
