/****************************
Copyright © 2006-2011 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef _DISPLAYNDS_H_
#define _DISPLAYNDS_H_
#include "display_types.h"

#ifndef LUX_DISPLAY_FUNCTION
#define LUX_DISPLAY_FUNCTION
#endif

bool LuxGraphics_NDS_Init( uint16_t width, uint16_t height, uint8_t bpp, bool fullscreen );
void LuxGraphics_NDS_Destory();
void LuxGraphics_NDS_Display2Screen( int32_t * x, int32_t * y);
void LuxGraphics_NDS_Background(LuxColour fillcolor);
void LuxGraphics_NDS_BackgroundObject( MapObject background );
void LuxGraphics_NDS_Update(LuxRect rect);
void LuxGraphics_NDS_Show();
void LuxGraphics_NDS_TextSprites( bool able );

LuxSprite * LuxGraphics_NDS_PNGtoSprite( uint8_t * data, uint32_t size );
void LuxGraphics_NDS_DrawSprite( LuxSprite * sprite, LuxRect dest_rect, ObjectEffect effects );
void LuxGraphics_NDS_DrawRect( LuxRect dest_rect, ObjectEffect effects );
void LuxGraphics_NDS_DrawCircle( LuxRect dest_rect, ObjectEffect effects );
void LuxGraphics_NDS_DrawPolygon( int16_t * x_point, int16_t * y_point, uint16_t point_count, ObjectEffect effects, void * texture );
void LuxGraphics_NDS_DrawLine( LuxRect points, ObjectEffect effects );
void LuxGraphics_NDS_DrawText( std::string text, LuxRect dest_rect, ObjectEffect effects, bool allow_custom);

bool LuxGraphics_NDS_RefreshSpriteSheet(std::string name, std::map<uint32_t, LuxSprite *> * children);
bool LuxGraphics_NDS_LoadSpriteSheet( std::string name, std::map<uint32_t, LuxSprite *> * children);
bool LuxGraphics_NDS_LoadSpriteSheetImage( elix::Image * image, std::map<uint32_t, LuxSprite *> * children);
bool LuxGraphics_NDS_FreeSpriteSheet( std::map<uint32_t, LuxSprite *> * children);
bool LuxGraphics_NDS_FreeSprite ( LuxSprite * sprite );

#endif
