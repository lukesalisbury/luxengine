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
				SDL_SetSurfaceBlendMode(temp_surface, SDL_BLENDMODE_BLEND);
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
		lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << "Scanning for window icon in resource directory." << std::endl;

		// Load Icon from resource
		elix::Image * image_file;
		elix::File * image_source_file;
		uint32_t size = 0;
		uint8_t * data = NULL;

		image_source_file = new elix::File( elix::directory::Resources("") + "window_icon.png" );
		size = image_source_file->ReadAll( (data_pointer*) &data);

		if ( size )
		{
			image_file = new elix::Image(data, size);
			if ( image_file->HasContent() )
			{
				icon = SDL_CreateRGBSurfaceFrom( image_file->GetPixels(), image_file->Width(), image_file->Height(), 32, image_file->Width()*4,0xff,0xff00,0xff0000, 0xff000000);
				SDL_SetSurfaceBlendMode(icon, SDL_BLENDMODE_BLEND);
				SDL_SetWindowIcon(native_window, icon);
				SDL_FreeSurface(icon);
			}
		}

	}

}

/* Lux_SDL2_SetRectFromText
 * Update Width and Height of a SDL_Rect base on a string of text
 @ area:
 @ text:
 @ text_width:
 @ text_height:
 */
void Lux_SDL2_SetRectFromText( SDL_Rect & area, std::string text, uint8_t text_width, uint8_t text_height )
{
	uint16_t max_length = 1;
	uint16_t length_count = 0;
	uint16_t lines = 1;
	std::string::iterator object;

	for ( object = text.begin(); object != text.end(); object++ )
	{
		uint8_t utfchar = *object;
		uint32_t cchar = utfchar;
		if ( cchar == '\n' || cchar == '\r' )
		{
			lines++;
			max_length = std::max(length_count, max_length);
			length_count = 0;
		}
		else if ( cchar <= 128 )
		{
			length_count++;
		}
		else if ( cchar < 224 )
		{
			object++;

			length_count++;
		}
		else if ( cchar < 240 )
		{
			object++;
			object++;

			length_count++;
		}
		else if ( cchar < 245 )
		{
			object++;
			object++;
			object++;

			length_count++;
		}
	}
	max_length = std::max(length_count, max_length);

	area.w = text_width * max_length;
	area.h = text_height * lines;

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
