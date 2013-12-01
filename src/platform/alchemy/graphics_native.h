/****************************
Copyright © 2006-2011 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef _DISPLAYNATIVE_H_
	#define _DISPLAYNATIVE_H_

	bool Lux_NATIVE_Init(int width, int height, int bpp, bool fullscreen);
	void Lux_NATIVE_Destory();
	void Lux_NATIVE_Background(LuxColour fillcolor);
	void Lux_NATIVE_Update(LuxRect rect);
	void Lux_NATIVE_Show();

	void Lux_NATIVE_DrawSprite( LuxSprite * sprite, LuxRect dest_rect, Effects image_effects );
	void Lux_NATIVE_DrawRect( LuxRect dest_rect, Effects image_effects );
	void Lux_NATIVE_DrawLine( LuxRect points, Effects image_effects ); 
	void Lux_NATIVE_DrawText (std::string text, LuxRect dest_rect, Effects image_effects);
	LuxSprite * Lux_NATIVE_PNGtoSprite( char * data, int32_t size  );
	void Lux_NATIVE_DrawCircle( LuxRect dest_rect, Effects image_effects );
	void Lux_NATIVE_DrawPolygon ( short * x_point, short *y_point, short point_count, Effects image_effects, void * texture );

	bool Lux_NATIVE_LoadSpriteSheet ( std::string name, std::map<std::string, LuxSprite *> * children);
	bool Lux_NATIVE_FreeSpriteSheet( std::map<std::string, LuxSprite *> * children);
#endif
