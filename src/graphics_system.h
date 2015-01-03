/****************************
Copyright © 2013-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef GRAPHICS_SYSTEM_FAKE_H
#define GRAPHICS_SYSTEM_FAKE_H

#include "stdheader.h"
#include "display_types.h"
#include "map_object.h"
#include "elix_png.hpp"
#ifndef LUX_DISPLAY_FUNCTION
#define LUX_DISPLAY_FUNCTION
#endif

bool Lux_GRAPHICS_Init( uint16_t width, uint16_t height, uint8_t bpp, bool fullscreen );
void Lux_GRAPHICS_Destory();
void Lux_GRAPHICS_UpdateRect(LuxRect rect);
void Lux_GRAPHICS_Show();

void Lux_GRAPHICS_Display2Screen( int32_t * x, int32_t * y);
void Lux_GRAPHICS_BackgroundObject( MapObject background );

void Lux_GRAPHICS_SetRotation( int16_t roll, int16_t pitch, int16_t yaw );
bool Lux_GRAPHICS_SetFullscreen( bool able );
bool Lux_GRAPHICS_Resize( uint16_t width, uint16_t height );
void Lux_GRAPHICS_DisplayPointer( uint8_t player, int16_t x, int16_t y, ObjectEffect effect );
void Lux_GRAPHICS_TextSprites( bool able );

void Lux_GRAPHICS_DrawSprite( LuxSprite * sprite, LuxRect dest_rect, ObjectEffect effects );
void Lux_GRAPHICS_DrawRect( LuxRect dest_rect, ObjectEffect effects );
void Lux_GRAPHICS_DrawCircle( LuxRect dest_rect, ObjectEffect effects );
void Lux_GRAPHICS_DrawPolygon( int16_t * x_point, int16_t * y_point, uint16_t point_count, LuxRect position, ObjectEffect effects, void * texture );
void Lux_GRAPHICS_DrawLine( LuxRect points, ObjectEffect effects );
void Lux_GRAPHICS_DrawText( std::string text, LuxRect dest_rect, ObjectEffect effects, bool allow_custom);
void Lux_GRAPHICS_DrawMessage( std::string message, uint8_t alignment );

bool Lux_GRAPHICS_RefreshSpriteSheet(std::string name, std::map<uint32_t, LuxSprite *> * children);
bool Lux_GRAPHICS_LoadSpriteSheet( std::string name, std::map<uint32_t, LuxSprite *> * children);
bool Lux_GRAPHICS_LoadSpriteSheetImage( elix::Image * image, std::map<uint32_t, LuxSprite *> * children);
bool Lux_GRAPHICS_FreeSpriteSheet( std::map<uint32_t, LuxSprite *> * children);
bool Lux_GRAPHICS_FreeSprite ( LuxSprite * sprite );
//void * Lux_GRAPHICS_CacheSprite ( LuxSprite * sprite, ObjectEffect fx ); //Use NULL instead
LuxSprite * Lux_GRAPHICS_PNGtoSprite( uint8_t * data, uint32_t size );

bool Lux_GRAPHICS_CacheDisplay( uint8_t layer );
bool Lux_GRAPHICS_DrawCacheDisplay(uint8_t layer, uint8_t shader);





#endif // GRAPHICS_SYSTEM_FAKE_H
