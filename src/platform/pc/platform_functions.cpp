/****************************
Copyright © 2006-2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include <string>
#include <SDL/SDL.h>
#include "stdheader.h"
#include "platform_functions.h"

uint32_t GetPixel(SDL_Surface *surface, int32_t x, int32_t y)
{
	if ( surface == NULL )
		return 0;

	if ( x > surface->w || y > surface->h || x < 0 || y < 0 )
		return 0;

	int bpp = surface->format->BytesPerPixel;
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

	switch(bpp) {
		case 1:
			return *p;

		case 2:
			return *(Uint16 *)p;

		case 3:
			if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
				return p[0] << 16 | p[1] << 8 | p[2];
			else
				return p[0] | p[1] << 8 | p[2] << 16;

		case 4:
			return *(uint32_t *)p;

		default:
			return 0;	   /* shouldn't happen, but avoids warnings */
	}
}

void PutPixel(SDL_Surface *surface, uint32_t x, uint32_t y, uint32_t pixel)
{
	if ( surface == NULL )
		return;

	int bpp = surface->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to set */
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

	switch(bpp) {
		case 1:
			*p = pixel;
			break;

		case 2:
			*(Uint16 *)p = pixel;
			break;

		case 3:
			if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
				p[0] = (pixel >> 16) & 0xff;
				p[1] = (pixel >> 8) & 0xff;
				p[2] = pixel & 0xff;
			} else {
				p[0] = pixel & 0xff;
				p[1] = (pixel >> 8) & 0xff;
				p[2] = (pixel >> 16) & 0xff;
			}
			break;

		case 4:
			*(uint32_t *)p = pixel;
			break;
	}
}

void SaveBMPFile(SDL_Surface *surface, std::string file)
{
	SDL_SaveBMP(surface, file.c_str());
}

#ifdef OPENGLENABLED
#include "gles/gles.hpp"
#include <sstream>
bool Lux_OGL_QueryExtension(char *extName)
{
	char *p = (char *) glGetString(GL_EXTENSIONS);
	char *end = p + strlen(p);
	while (p < end) {
		unsigned int n = strcspn(p, " ");
		if ( ( strlen(extName) == n) && ( strncmp(extName,p,n) == 0) )
		{
			return true;
		}
		p += (n + 1);
	}
	return false;
}
/*
SDL_Surface * Lux_OGL_ScreenSuface( Texture * texture )
{
	SDL_Surface * screen = SDL_CreateRGBSurface(SDL_SWSURFACE, texture->tw, texture->th, 32, RMASK, GMASK, BMASK, AMASK);
	if ( screen )
	{
		glGetTexImage( GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, screen->pixels );
	}
	return screen;
}
void Lux_OGL_ScreenSuface2( void * texture, std::string str )
{
	SDL_SaveBMP( Lux_OGL_ScreenSuface( (Texture*)texture ), str.c_str() );
}

void Lux_OGL_ScreenSuface3( uint8_t n )
{
	SDL_Surface * screen = SDL_CreateRGBSurface(SDL_SWSURFACE, 8, 8, 32, RMASK, GMASK, BMASK, AMASK);
	if ( screen )
	{
		glGetTexImage( GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, screen->pixels );
	}
	std::stringstream title_msg;
	title_msg << "temp/" << (int)n << ".bmp";
	SDL_SaveBMP( screen, title_msg.str().c_str() );
}
*/
#endif
