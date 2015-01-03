/****************************
Copyright © 2006-2013 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include "stdheader.h"
#include "mokoi_game.h"
#include "elix_png.hpp"
#include <string>
#include <SDL.h>
#include "core.h"
#include "config.h"
#include "bitfont.h"
#include "misc_functions.h"

/* Characters in SDL_Texture format
 * Use in Native and MessageWindow
 */

SDL_Texture * debug_font[128];

/* Debug Message */
SDL_Window * debug_window = NULL;
SDL_Renderer * debug_renderer = NULL;

void Lux_SDL2_LoadFont( SDL_Renderer * renderer, SDL_Texture * (&font)[128] )
{
	uint8_t * font_point = &gfxPrimitivesFontdata[0];
	uint8_t i = 0, q;
	for ( uint8_t c = 0; c < 128; c++)
	{
		uint16_t * charflip = new uint16_t[64];
		for (i = 0; i < 8; i++)
		{
			for (q = 0; q < 8; q++)
			{
				charflip[(i*8) + q] =  (!!(font_point[i] & (1 << (8-q))) ? 0xFFFF : 0x000F) ;
			}
		}
		font[c] = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB1555, SDL_TEXTUREACCESS_STATIC, 8, 8);
		if ( !font[c] )
			lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << "Lux_SDL2_LoadFont: " << SDL_GetError() << std::endl;
		SDL_SetTextureBlendMode( font[c],SDL_BLENDMODE_ADD);
		SDL_UpdateTexture( font[c], NULL, charflip, 16);

		font_point += 8;
	}
}


void Lux_SDL2_UnloadFont( SDL_Texture * (&font)[128] )
{
	for ( uint8_t c = 0; c < 128; c++)
	{
		SDL_DestroyTexture(font[c]);
	}
}

SDL_Texture * Lux_SDL2_GetCharTexture( SDL_Texture * font[128], uint8_t c )
{
	SDL_Texture * texture = NULL;
	if ( c > 31 && c < 128)
	{
		texture =  font[c];
	}
	return texture;
}



/* Lux_SDL2_OpenMessageWindow
 * Creates a new window to display messages.
 -
 */
void Lux_SDL2_OpenMessageWindow(  )
{
	if ( !debug_window )
	{
		/* Debug Messages */
		int32_t x;
		int32_t y;

		x = lux::global_config->GetNumber("debug.x");
		y = lux::global_config->GetNumber("debug.y");


		/* Account for Window Size*/
		if ( x < 10 )
			x = 10;

		if ( y < 10 )
			y = 10;

		debug_window = SDL_CreateWindow("Messages", static_cast<int>(x), static_cast<int>(y), 480, 360, SDL_WINDOW_SHOWN);
		debug_renderer = SDL_CreateRenderer(debug_window, -1, SDL_RENDERER_SOFTWARE );

		if ( !debug_renderer )
		{
			lux::core->SystemMessage(__FILE__, __LINE__, SYSTEM_MESSAGE_INFO) << " Couldn't create Renderer. " << SDL_GetError() << std::endl;

		}

		Lux_SDL2_LoadFont( debug_renderer, debug_font );
	}
}

void Lux_SDL2_DrawMessage( std::string message, uint8_t alignment )
{
	if ( !debug_window )
		return;

	std::string::iterator object;
	SDL_Rect draw;
	LuxRect rect;
	SDL_Rect area;
	int w,h;

	bool watch_for_color = false;
	bool is_whitspace = false;
	LuxColour font_color = { 255, 255,255, 255 };

	rect.x = rect.y = 0;

	SDL_GetWindowSize( debug_window, &w, &h );
	Lux_Util_SetRectFromText( rect, message, 8, 10, 240 );

	area.x = rect.x;
	area.y = rect.y;
	area.w = rect.w;
	area.h = rect.h;

	if ( alignment == 3 )
	{
		area.y = h - area.h;
	}
	else if ( alignment == 2 )
	{
		area.y = h - area.h;
		area.x = w - area.w;
	}
	else if ( alignment == 1 )
	{
		area.x = w - area.w;
	}

	SDL_SetRenderDrawColor( debug_renderer, 0, 0, 0, 255 );
	//SDL_RenderFillRect( debug_renderer, &area );

	draw = area;
	draw.w = draw.h = 8;

	for ( object = message.begin(); object != message.end(); object++ )
	{
		uint8_t utfchar = *object;
		uint32_t cchar = utfchar;

		is_whitspace = false;

		if (cchar == '\n' || cchar == '\r')
		{
			draw.y += 10;
			draw.x = area.x;
			cchar = 0;
			is_whitspace = true;
		}
		else if ( cchar <= 32 )
		{
			draw.x += 7;
			cchar = 0;
			is_whitspace = true;
		}
		else if ( cchar <= 128 )
		{

		}
		else if ( cchar < 224 )
		{
			object++;
			uint32_t next = *object;

			cchar = ((cchar << 6) & 0x7ff) + (next & 0x3f);
		}
		else if ( cchar < 240 )
		{
			uint32_t next;

			object++;
			next = (*object) & 0xff;
			cchar = ((cchar << 12) & 0xffff) + ((next << 6) & 0xfff);

			object++;
			next = (*object) & 0x3f;
			cchar += next;

		}
		else if ( cchar < 245 )
		{
			uint32_t next;

			object++;
			next = (*object) & 0xff;
			cchar = ((cchar << 18) & 0xffff) + ((next << 12) & 0x3ffff);

			object++;
			next = (*object) & 0xff;
			cchar += (next << 6) & 0xfff;

			object++;
			next = (*object) & 0x3f;
			cchar += next;
		}

		if ( cchar != 0 )
		{
			if ( cchar == 'S' )
			{
				int q = 0;
			}
			if ( !Lux_Util_CheckTextColour( cchar, font_color, watch_for_color ) )
			{
				SDL_Texture * texture = NULL;

				if ( cchar >= 32 && cchar <= 128 )
				{
					texture = Lux_SDL2_GetCharTexture(debug_font, cchar);
				}
				else
				{
					texture = Lux_SDL2_GetCharTexture(debug_font, '?');
				}

				if ( texture )
				{
					SDL_SetTextureColorMod( texture, font_color.r, font_color.g, font_color.b);
					SDL_SetTextureAlphaMod( texture, 255 );
					SDL_RenderCopy(debug_renderer, texture, NULL, &draw);
				}
				draw.x += 7;
			}
		}

		if ( is_whitspace )
		{
			/* Reset Colour if a whitespace occurs */
			font_color.r = font_color.g = font_color.b = 255;
		}
	}

}

void Lux_SDL2_PresentMessageWindow()
{
	SDL_RenderPresent(debug_renderer);
	SDL_RenderClear(debug_renderer);
}


/* Lux_SDL2_CloseMessageWindow
 * Closes the Message Window
 -
 */
void Lux_SDL2_CloseMessageWindow(  )
{
	if ( debug_renderer )
	{
		Lux_SDL2_UnloadFont( debug_font );
		SDL_DestroyRenderer(debug_renderer);
		debug_renderer = NULL;
	}
	if ( debug_window )
	{
		int x;
		int y;


		SDL_GetWindowPosition( debug_window, &x, &y );

		lux::global_config->SetNumber("debug.x", static_cast<int32_t>(x));
		lux::global_config->SetNumber("debug.y", static_cast<int32_t>(y));

		SDL_DestroyWindow(debug_window);
		debug_window = NULL;
	}

}

/* Lux_SDL2_Image2Surface
 * Creates a SDL_Surface from a file from the game file.
 @ file:
 -
 */
SDL_Surface * Lux_SDL2_Image2Surface( std::string file )
{
	uint8_t * data = NULL;
	uint32_t size;
	SDL_Surface * temp_surface = NULL;
	if ( lux::game_data )
	{
		size = lux::game_data->GetFile(file, &data, false);
		if ( size )
		{
			elix::Image * png = new elix::Image(data, size);
			if ( png->HasContent() )
			{
				temp_surface = SDL_CreateRGBSurfaceFrom( png->GetPixels(), png->Width(), png->Height(),32,png->Width()*4,0xff,0xff00,0xff0000, 0xff000000);
				SDL_SetSurfaceBlendMode(temp_surface, SDL_BLENDMODE_NONE);
			}
			delete png;
		}
	}
	return temp_surface;
}

/* Lux_SDL2_SetWindowIcon
 * Set Window Icon
 @ native_window:
 -
 */
void Lux_SDL2_SetWindowIcon( SDL_Window * native_window )
{
	SDL_Surface * icon = NULL;

	if ( lux::game_data )
	{
		if ( lux::game_data->HasFile("./resources/icon256.png") )
			icon = Lux_SDL2_Image2Surface("./resources/icon256.png");
		else
			icon = Lux_SDL2_Image2Surface("./resources/icon32.png");
	}

	if ( icon != NULL )
	{
		SDL_SetWindowIcon(native_window, icon);
		SDL_FreeSurface(icon);
	}
	else
	{
		// Load Icon from resource
		uint32_t size = 0;
		elix::File * image_source_file = NULL;
		uint8_t * data = NULL;

		image_source_file = new elix::File( elix::directory::Resources("", "window_icon.png") );
		size = image_source_file->ReadAll( (data_pointer*) &data);

		if ( size )
		{
			elix::Image * image_file = new elix::Image(data, size);
			if ( image_file->HasContent() )
			{
				icon = SDL_CreateRGBSurfaceFrom( image_file->GetPixels(), image_file->Width(), image_file->Height(), 32, image_file->Width()*4,0xff,0xff00,0xff0000, 0xff000000);
				SDL_SetSurfaceBlendMode(icon, SDL_BLENDMODE_BLEND);
				SDL_SetWindowIcon(native_window, icon);
				SDL_FreeSurface(icon);
			}
			delete image_file;
		}

		NULLIFY_ARRAY( data );
		NULLIFY( image_source_file );
	}

}



#ifdef OPENGLENABLED
#include "gles/gles.hpp"
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
#endif
