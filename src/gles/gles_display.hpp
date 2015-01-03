/****************************
Copyright © 2012-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/


void Lux_GLES_DrawCircle( LuxRect dest_rect, ObjectEffect effects );
void Lux_GLES_DrawLine( LuxRect points, ObjectEffect effects);
void Lux_GLES_DrawPolygon( int16_t * x_point, int16_t *y_point, uint16_t point_count, LuxRect position, ObjectEffect effects, void * texture );
void Lux_GLES_DrawRect( LuxRect dest_rect, ObjectEffect effects);
void Lux_GLES_DrawSprite(LuxSprite * sprite, LuxRect dest_rect, ObjectEffect effect );
void Lux_GLES_DrawText( std::string text, LuxRect dest_rect, ObjectEffect effects, bool allow_custom);

void Lux_GLES_TextSprites( bool able );
void Lux_GLES_UnloadFont();
void Lux_GLES_LoadFont();
