/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef _DISPLAYTYPES_H_
#define _DISPLAYTYPES_H_

#include <map>

#include "stdheader.h"
#include "lux_types.h"
#include "sprite_types.h"
#include "display/display_styles.h"
#include "sprite_types.h"
#include "map_object.h"
#include "object_effect.h"
#include "elix/elix_png.hpp"
#include "lux_polygon.h"


#define GRAPHICS_SCREEN_FRAME  0xff
#define GRAPHICS_SCREEN_GUI  30
#define GRAPHICS_SCREEN_MESSAGE  31



extern ObjectEffect default_fx;


typedef struct {
	/* Plugin Graphics */
	bool (* InitGraphics) ( LuxRect * screen_dimension, LuxRect * display_dimension );
	void (* DestoryGraphics) ();
	void (* Display2Screen) ( int32_t * x, int32_t * y);
	void (* TextSprites) ( bool able );
	void (* PreShow) (  uint8_t screen );
	void (* UpdateRect) (  uint8_t screen, LuxRect rect );
	void (* PostShow) ( uint8_t screen );
	void (* SetRotation) ( int16_t roll, int16_t pitch, int16_t yaw );
	void (* SetBackground) ( MapObject background );
	bool (* SetFullscreen) ( bool able );
	bool (* Resize) ( uint16_t width, uint16_t height );
	void (* DisplayPointer) ( uint8_t player, int16_t x, int16_t y, ObjectEffect effect );

	bool (* RefreshSpriteSheet) ( std::string name, std::map<uint32_t, LuxSprite *> * children );
	bool (* LoadSpriteSheet) ( std::string name, std::map<uint32_t, LuxSprite *> * children );
	bool (* LoadSpriteSheetWithImage) ( elix::Image * image, std::map<uint32_t, LuxSprite *> * children);
	bool (* FreeSpriteSheet) ( std::map<uint32_t, LuxSprite *> * children );
	mem_pointer (* CacheSprite) ( LuxSprite * sprite, ObjectEffect fx );
	bool (* FreeSprite) ( LuxSprite * sprite );
	LuxSprite * (* PNGtoSprite) ( uint8_t * data, uint32_t size );

	void (* DrawSprite) ( LuxSprite * sprite, LuxRect dest_rect, ObjectEffect effect );
	void (* DrawRect) ( LuxRect dest_rect, ObjectEffect effects );
	void (* DrawCircle) ( LuxRect dest_rect, ObjectEffect effects );
	void (* DrawPolygon) ( int16_t * x_point, int16_t * y_point, uint16_t point_count, LuxRect position, ObjectEffect effects, void * texture );
	void (* DrawLine) ( LuxRect points, ObjectEffect effects );
	void (* DrawText) ( std::string text, LuxRect dest_rect, ObjectEffect effects, bool allow_text );
	void (* DrawMessage) ( std::string message, uint8_t alignment );


	bool (* CacheDisplay ) ( uint8_t layer );
	bool (* DrawCacheDisplay ) ( uint8_t layer, uint8_t shader );


} GraphicSystem;


#endif /* _DISPLAYTYPES_H_ */
