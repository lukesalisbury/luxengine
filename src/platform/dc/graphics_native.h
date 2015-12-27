/****************************
Copyright (c) 2006-2010 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef _DISPLAYNATIVE_H_
	#define _DISPLAYNATIVE_H_

#include "display/display_types.h"

#ifndef 
#define 
#endif

bool LuxGraphics_DC_Init( uint16_t  width, uint16_t height, uint8_t bpp, uint16_t * actual_width, uint16_t * actual_height, bool fullscreen );
void LuxGraphics_DC_Destory();
void LuxGraphics_DC_UpdateRect(uint8_t screen,LuxRect rect);
void LuxGraphics_DC_Show( uint8_t screen );

void LuxGraphics_DC_Display2Screen( int32_t * x, int32_t * y);
void LuxGraphics_DC_Background(LuxColour fillcolor);
void LuxGraphics_DC_BackgroundObject( MapObject background );
void LuxGraphics_DC_DisplayPointer( uint8_t player, int16_t x, int16_t y, ObjectEffect effect );

bool LuxGraphics_DC_SetFullscreen( bool able );
void LuxGraphics_DC_TextSprites( bool able );

void LuxGraphics_DC_DrawSprite( LuxSprite * sprite, LuxRect dest_rect, ObjectEffect effects );
void LuxGraphics_DC_DrawRect(LuxRect dest_rect, ObjectEffect effects );
void LuxGraphics_DC_DrawCircle( LuxRect dest_rect, ObjectEffect effects );
void LuxGraphics_DC_DrawPolygon( int16_t * x_point, int16_t * y_point, uint16_t point_count, LuxRect position, ObjectEffect effects, void * texture );
void LuxGraphics_DC_DrawLine( LuxRect points, ObjectEffect effects );
void LuxGraphics_DC_DrawText( std::string text, LuxRect dest_rect, ObjectEffect effects, bool allow_custom);

bool LuxGraphics_DC_RefreshSpriteSheet(std::string name, std::map<uint32_t, LuxSprite *> * children);
bool LuxGraphics_DC_LoadSpriteSheet( std::string name, std::map<uint32_t, LuxSprite *> * children);
bool LuxGraphics_DC_LoadSpriteSheetImage( elix::Image * image, std::map<uint32_t, LuxSprite *> * children);
bool LuxGraphics_DC_FreeSpriteSheet( std::map<uint32_t, LuxSprite *> * children);
void * LuxGraphics_DC_CacheSprite ( LuxSprite * sprite, ObjectEffect fx );
bool LuxGraphics_DC_FreeSprite ( LuxSprite * sprite );
LuxSprite * LuxGraphics_DC_PNGtoSprite( uint8_t * data, uint32_t size );
#endif
