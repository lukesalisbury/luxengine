/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include <string>
#include <SDL.h>

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	#define RMASK 0xff000000
	#define GMASK 0x00ff0000
	#define BMASK 0x0000ff00
	#define AMASK 0x000000ff
#else
	#define RMASK 0x000000ff
	#define GMASK 0x0000ff00
	#define BMASK 0x00ff0000
	#define AMASK 0xff000000
#endif

SDL_Surface * Lux_SDL2_Image2Surface( std::string file );
void Lux_SDL2_SetWindowIcon( SDL_Window * native_window );
void Lux_SDL2_SetRectFromText( SDL_Rect & area, std::string text, uint8_t text_width, uint8_t text_height );


void Lux_SDL2_LoadFont( SDL_Renderer * renderer, SDL_Texture * (&font)[129] );
void Lux_SDL2_UnloadFont( SDL_Texture * (&font)[129] );
SDL_Texture * Lux_SDL2_GetCharTexture( uint8_t c );
bool Lux_SDL2_CheckTextColour( uint32_t cchar, SDL_Color & font_color, bool & watch_for_color );



void Lux_SDL2_OpenMessageWindow( );
void Lux_SDL2_PresentMessageWindow( );
void Lux_SDL2_DrawMessage( std::string message, uint8_t alignment );
void Lux_SDL2_CloseMessageWindow( );


#ifdef OPENGLENABLED
bool Lux_OGL_QueryExtension(char *extName);
#endif
