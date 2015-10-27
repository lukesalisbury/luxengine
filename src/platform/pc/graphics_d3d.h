/****************************
Copyright (c) 2006-2009 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef _DISPLAYD3D_H_
	#define _DISPLAYD3D_H_

	bool Lux_D3D_Init( uint16_t  width, uint16_t height, uint8_t bpp, uint16_t * actual_width, uint16_t * actual_height, bool fullscreen );
	void Lux_D3D_Destory();
	void Lux_D3D_Background(LuxColour fillcolor);
	void Lux_D3D_Update( uint8_t screen, LuxRect rect);
	void Lux_D3D_Show( uint8_t screen );

	LuxSprite * Lux_D3D_PNGtoSprite( uint8_t * data, int32_t size );
	void Lux_D3D_DrawSprite( LuxSprite * sprite, LuxRect dest_rect, ObjectEffect effects );
	void Lux_D3D_DrawRect( LuxRect dest_rect, ObjectEffect effects );
	void Lux_D3D_DrawPolygon( int16_t * x_point, int16_t * y_point, uint16_t point_count, ObjectEffect effects, void * texture  );
	void Lux_D3D_DrawCircle( LuxRect dest_rect, ObjectEffect effects );
	void Lux_D3D_DrawLine( LuxRect points, ObjectEffect effects ); 
	void Lux_D3D_DrawText( std::string text, LuxRect dest_rect, ObjectEffect effects);
	void Lux_D3D_SetRotation( int16_t roll, int16_t pitch, int16_t yaw );
	
	bool Lux_D3D_LoadSpriteSheet( std::string name, std::map<uint32_t, LuxSprite *> * children);
	bool Lux_D3D_FreeSpriteSheet( std::map<uint32_t, LuxSprite *> * children);
	
#endif
