/****************************
Copyright © 2006-2011 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef _DISPLAYOPENGL_H_
	#define _DISPLAYOPENGL_H_

	bool Lux_OGL_Init( uint16_t width, uint16_t height, uint8_t bpp, bool fullscreen );
	void Lux_OGL_Destory();
	void Lux_OGL_Display2Screen( int32_t * x, int32_t * y);
	void Lux_OGL_BackgroundObject( MapObject background );
	void Lux_OGL_Update(LuxRect rect);
	void Lux_OGL_Show();
	void Lux_OGL_TextSprites( bool able );

	LuxSprite * Lux_OGL_PNGtoSprite( uint8_t * data, uint32_t size );
	void Lux_OGL_DrawSprite( LuxSprite * sprite, LuxRect dest_rect, Effects image_effects );
	void Lux_OGL_DrawRect( LuxRect dest_rect, Effects image_effects );
	void Lux_OGL_DrawPolygon( int16_t * x_point, int16_t * y_point, uint16_t point_count, Effects image_effects, void * texture  );
	void Lux_OGL_DrawCircle( LuxRect dest_rect, Effects image_effects );
	void Lux_OGL_DrawLine( LuxRect points, Effects image_effects ); 
	void Lux_OGL_DrawText( std::string text, LuxRect dest_rect, Effects image_effects, bool allow_custom);
	void Lux_OGL_SetRotation( int16_t roll, int16_t pitch, int16_t yaw );
	
	bool Lux_OGL_LoadSpriteSheet( std::string name, std::map<std::string, LuxSprite *> * children);
	bool Lux_OGL_FreeSpriteSheet( std::map<std::string, LuxSprite *> * children);
	bool Lux_OGL_FreeSprite ( LuxSprite * sprite );
#endif
