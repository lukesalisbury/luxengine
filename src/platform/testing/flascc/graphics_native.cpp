/****************************
Copyright © 2006-2011 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

/* Required Headers */
#include "core.h"
#include "config.h"
#include "engine.h"
#include "display/display.h"
#include "misc_functions.h"
#include "mokoi_game.h"
#include "map_object.h"
#include "graphics_native.h"
#include "graphics_system.h"

#include "elix/elix_png.hpp"
#include "elix/elix_string.hpp"

#include <AS3/AS3.h>


GraphicSystem GraphicsNative = {
	&Lux_NATIVE_Init,
	&Lux_NATIVE_Destory,
	&Lux_NATIVE_Display2Screen,
	&Lux_NATIVE_TextSprites,
	&Lux_NATIVE_UpdateRect,
	&Lux_NATIVE_Show,

	&Lux_GRAPHICS_SetRotation,
	&Lux_NATIVE_BackgroundObject,
	&Lux_NATIVE_SetFullscreen,
	&Lux_GRAPHICS_Resize,
	&Lux_NATIVE_DisplayPointer,

	&Lux_NATIVE_RefreshSpriteSheet,
	&Lux_NATIVE_LoadSpriteSheet,
	&Lux_NATIVE_LoadSpriteSheetImage,
	&Lux_NATIVE_FreeSpriteSheet,
	nullptr,
	&Lux_NATIVE_FreeSprite,
	&Lux_NATIVE_PNGtoSprite,

	&Lux_NATIVE_DrawSprite,
	&Lux_NATIVE_DrawRect,
	&Lux_NATIVE_DrawCircle,
	&Lux_NATIVE_DrawPolygon,
	&Lux_NATIVE_DrawLine,
	&Lux_NATIVE_DrawText

};

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

/* Global Variables */
Uint32 sdlgraphics_flags = 0;
LuxColour sdlgraphics_colour = {0,0,0,255};
SDL_Rect sdlgraphics_dimension = {0, 0, 320,240};
SDL_Rect * sdlgraphics_dirty = 0;
float sdlgraphics_ratio[2] = {1.0, 1.0};
int32_t sdlgraphics_scale = 0;

int sdlgraphics_dirtycount = 0;
SDL_Surface * sdlgraphics_screen = nullptr;
SDL_Surface * sdlgraphics_realscreen = nullptr;
std::string sdlgraphics_title;
uint32_t sdlgraphics_fps = 0, sdlgraphics_fpstime = 0;
bool sdlgraphics_customtext = false;
int32_t sdlgraphics_customtext_height = 32;

LuxPolygon * sdlgraphics_cursor = nullptr;

/* Local Functions */

// Loaded a file from the game file.
SDL_Surface * SDL_Surface_LoadImage( std::string file )
{
	uint8_t * data = nullptr;
	uint32_t size;
	SDL_Surface * temp_surface = nullptr;
	if ( lux::game )
	{
		size = lux::game->GetFile(file, &data, false);
		if ( size )
		{
			elix::Image * png = new elix::Image(data, size);
			if ( png->HasContent() )
			{
				temp_surface = SDL_CreateRGBSurfaceFrom( png->GetPixels(), png->Width(), png->Height(),32,png->Width()*4,0xff,0xff00,0xff0000, 0xff000000);
				SDL_SetAlpha(temp_surface, 0, 255);
			}
		}
	}
	return temp_surface;
}

// Switches 'sdlgraphics_screen' value
// Can be use to redirect drawing functions to another SDL_Surface
// Passing nullptr will set back to the default.
bool Lux_NATIVE_ChangeOutput( SDL_Surface * output )
{
	if ( output )
		return (bool)(sdlgraphics_screen = output);
	else
		return (bool)(sdlgraphics_screen = SDL_GetVideoSurface());
}

/* Creation,destruction and loop Functions */
LUX_DISPLAY_FUNCTION bool Lux_NATIVE_Init( uint16_t  width, uint16_t height, uint8_t bpp, uint16_t * actual_width, uint16_t * actual_height, bool fullscreen )
{
	sdlgraphics_scale = false;

	sdlgraphics_screen = SDL_SetVideoMode(512, 384, 32, SDL_SWSURFACE );
	if ( !sdlgraphics_screen )
	{
		lux::core->SystemMessage(SYSTEM_MESSAGE_LOG) << __FILE__ << ":" << __LINE__ << " | Couldn't set " << width << "x" << height << " video mode. " << SDL_GetError() << std::endl;
		return false;
	}

	sdlgraphics_dimension.w = width;
	sdlgraphics_dimension.h = height;

	char vd[15];
	SDL_VideoDriverName(vd, 14);
	lux::core->SystemMessage(SYSTEM_MESSAGE_LOG) << "Video System: " << vd << " - " << sdlgraphics_screen->w << "x" << sdlgraphics_screen->h << "x" << (int)sdlgraphics_screen->format->BitsPerPixel << "bpp" << std::endl;
	SDL_ShowCursor(false);


	sdlgraphics_cursor = new LuxPolygon(3);

	sdlgraphics_cursor->SetPoint(0, 0, 0);
	sdlgraphics_cursor->SetPoint(1, 8, 8);
	sdlgraphics_cursor->SetPoint(2, 0, 12);

	return true;
}

LUX_DISPLAY_FUNCTION void Lux_NATIVE_Destory()
{
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

LUX_DISPLAY_FUNCTION void Lux_NATIVE_UpdateRect(uint8_t screen,LuxRect rect)
{
	SDL_Flip(sdlgraphics_screen);
}

uint32_t GetPixel(SDL_Surface *surface, uint32_t x, uint32_t y)
{
	if ( surface == nullptr )
		return 0;

	if ( x > surface->w || y > surface->h )
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

LUX_DISPLAY_FUNCTION void Lux_NATIVE_Show( uint8_t screen )
{

	if ( sdlgraphics_screen )
	{
		//SDL_Flip(sdlgraphics_screen);
		// update the bitmap
		for(uint32_t x=0; x<sdlgraphics_screen->w; x++)
		{
			for(uint32_t y=0; y<sdlgraphics_screen->h; y++)
			{
				uint32_t c = GetPixel(sdlgraphics_screen, x, y);

				inline_as3(
					"import com.adobe.flascc.CModule;\n"
					"import flash.display.BitmapData;\n"
					"import flash.display.Graphics;\n"
					"import flash.display.Stage;\n"
					"var x:uint = %0;\n"
					"var y:uint = %1;\n"
					"var c:uint = %2;\n"
					"CModule.activeConsole.bmd.setPixel32(x, y, c);\n"
					: : "r"(x), "r"(y), "r"(c)
				);
			}
		}



		Uint32 new_color = SDL_MapRGB( sdlgraphics_screen->format,sdlgraphics_colour.r, sdlgraphics_colour.g, sdlgraphics_colour.b );
		SDL_FillRect(sdlgraphics_screen, &sdlgraphics_dimension, new_color);
		sdlgraphics_dirtycount = 0;



		if ( lux::core->GetTime() > (sdlgraphics_fpstime + 1000) )
		{
			sdlgraphics_fpstime = lux::core->GetTime();

			AS3_SendMetricInt("engine.fps", sdlgraphics_fps);
			lux::core->SystemMessage(SYSTEM_MESSAGE_LOG) << "sdlgraphics_fps" << sdlgraphics_fps << std::endl;
			sdlgraphics_fps = 0;
		}
		sdlgraphics_fps++;
	}
}

LUX_DISPLAY_FUNCTION void Lux_NATIVE_DisplayPointer( uint8_t player, int16_t x, int16_t y, ObjectEffect effect )
{
	LuxRect position;

	position.x = x;
	position.y = y;

	Lux_NATIVE_DrawPolygon(sdlgraphics_cursor->x, sdlgraphics_cursor->y, sdlgraphics_cursor->count, position, effect, nullptr);

}

/*
 - Converts display location to screen location
*/
LUX_DISPLAY_FUNCTION void Lux_NATIVE_Display2Screen( int32_t * x, int32_t * y)
{
	if ( sdlgraphics_scale )
	{
		*x = (int32_t)((float)*x * sdlgraphics_ratio[0]);
		*y = (int32_t)((float)*y * sdlgraphics_ratio[1]);
	}
}

LUX_DISPLAY_FUNCTION void Lux_NATIVE_BackgroundObject( MapObject background )
{
	/* TODO
	- Draw background object
	*/
	sdlgraphics_colour = background.effects.primary_colour;
}

LUX_DISPLAY_FUNCTION bool Lux_NATIVE_SetFullscreen( bool able )
{

	return true;
}

LUX_DISPLAY_FUNCTION void Lux_NATIVE_TextSprites( bool able )
{
	if ( lux::display->sprite_font.length() )
	{
		sdlgraphics_customtext = able;
		if ( sdlgraphics_customtext )
		{
			LuxSprite * sdata = lux::display->GetSprite( lux::display->sprite_font, "48" );
			if ( sdata )
				sdlgraphics_customtext_height = sdata->sheet_area.h;
			else
				sdlgraphics_customtext_height = 16;
		}
	}
	else
		sdlgraphics_customtext = false;
}

/* Resource Functions */

/* Lux_NATIVE_FreeSprite
 *
 @ sprite:
 -
 */
LUX_DISPLAY_FUNCTION bool Lux_NATIVE_FreeSprite( LuxSprite * sprite )
{
	if ( sprite == nullptr )
		return false;
	if ( !sprite->_cache.empty() )
	{
		std::map<uint32_t, mem_pointer>::iterator p = sprite->_cache.begin();
		while( p != sprite->_cache.end() )
		{
			SDL_FreeSurface((SDL_Surface *)p->second);
		}
		sprite->_cache.clear();
	}
	if (sprite->data)
	{
		SDL_FreeSurface((SDL_Surface *)sprite->data);
	}
	sprite->data = nullptr;
	return false;
}


/* Lux_NATIVE_CreateSprite
 *
 @ sprite:
 @ rect:
 @ parent:
 -
 */
LUX_DISPLAY_FUNCTION bool Lux_NATIVE_CreateSprite( LuxSprite * sprite, LuxRect rect, elix::Image * png )
{
	if ( !png->HasContent() )
	{
		lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR) << "PNG is empty" << std::endl;
		return false;
	}

	uint16_t x, y;
	uint8_t r, g, b, a;
	SDL_Rect tmp_rect;
	tmp_rect.x = rect.x;
	tmp_rect.y = rect.y;
	tmp_rect.w = rect.w;
	tmp_rect.h = rect.h;

	SDL_Surface * temp_sheet = SDL_CreateRGBSurface( sdlgraphics_flags, tmp_rect.w, tmp_rect.h, 32, RMASK, GMASK, BMASK, AMASK );
	if ( temp_sheet )
	{
		for (x = 0; x < temp_sheet->w; x++)
		{
			for (y = 0; y < temp_sheet->h; y++)
			{
				if ( png->GetPixel(rect.x + x, rect.y + y, r, g, b, a) )
				{
					//pixelRGBA(temp_sheet, x, y, r, g, b, a );
				}
			}
		}
		SDL_SetAlpha(temp_sheet, 0, 0);
		sprite->data = (mem_pointer)SDL_DisplayFormatAlpha(temp_sheet);
	}
	SDL_FreeSurface( temp_sheet );
	return false;
}

/* Lux_NATIVE_LoadSpriteSheet
 *
 @ name:
 @ children:
 -
 */
LUX_DISPLAY_FUNCTION bool Lux_NATIVE_LoadSpriteSheet( std::string name, std::map<uint32_t, LuxSprite *> * children )
{
	SDL_Surface * parent_sheet = SDL_Surface_LoadImage("./sprites/" + name);
	if ( !parent_sheet )
		return false;

	std::map<uint32_t, LuxSprite *>::iterator p;
	for( p = children->begin(); p != children->end(); p++ )
	{
		if ( !p->second->animated )
		{
			SDL_Rect tmp_rect;
			tmp_rect.x = p->second->sheet_area.x;
			tmp_rect.y = p->second->sheet_area.y;
			tmp_rect.w = p->second->sheet_area.w;
			tmp_rect.h = p->second->sheet_area.h;

			SDL_Surface * temp_sheet = SDL_CreateRGBSurface( sdlgraphics_flags, tmp_rect.w, tmp_rect.h, 16, RMASK, GMASK, BMASK, AMASK );
			SDL_BlitSurface(parent_sheet, &tmp_rect, temp_sheet, nullptr);
			SDL_SetAlpha(temp_sheet, 0, 0);

			if ( temp_sheet )
			{
				p->second->data = (void *)SDL_DisplayFormatAlpha(temp_sheet);
			}
			SDL_FreeSurface( temp_sheet );
		}
	}
	SDL_FreeSurface( parent_sheet );
	return true;
}

/* Lux_NATIVE_LoadSpriteSheetImage
 *
 @ image:
 @ children:
 -
 */
LUX_DISPLAY_FUNCTION bool Lux_NATIVE_LoadSpriteSheetImage( elix::Image * image, std::map<uint32_t, LuxSprite *> * children)
{
	if ( !image  )
	{
		return false;
	}

	if (  !children )
	{
		return false;
	}


	if ( image->HasContent() )
	{
		std::map<uint32_t, LuxSprite *>::iterator p = children->begin();
		for( ; p != children->end(); p++ )
		{
			if ( !p->second->animated )
			{
				Lux_NATIVE_CreateSprite( p->second, p->second->sheet_area, image );
			}
		}
		return true;
	}
	else
	{
		return false;
	}

}

/* Lux_NATIVE_FreeSpriteSheet
 *
 @ children:
 -
 */
LUX_DISPLAY_FUNCTION bool Lux_NATIVE_FreeSpriteSheet( std::map<uint32_t, LuxSprite *> * children )
{
	std::map<uint32_t, LuxSprite *>::iterator p;
	for( p = children->begin(); p != children->end(); p++ )
	{
		if ( !p->second->animated )
		{
			Lux_NATIVE_FreeSprite( p->second );
		}
	}
	return true;
}

/* Lux_NATIVE_RefreshSpriteSheet
 *
 @ name:
 @ children:
 -
 */
LUX_DISPLAY_FUNCTION bool Lux_NATIVE_RefreshSpriteSheet( std::string name, std::map<uint32_t, LuxSprite *> * children )
{
	if ( Lux_NATIVE_FreeSpriteSheet( children ) )
		Lux_NATIVE_LoadSpriteSheet( name, children );
	return true;
}

/* Lux_NATIVE_PNGtoSprite
 *
 @ data:
 @ size:
 -
 */
LUX_DISPLAY_FUNCTION LuxSprite * Lux_NATIVE_PNGtoSprite( uint8_t * data, uint32_t size )
{
	LuxSprite * sprite = nullptr;
	elix::Image * png = new elix::Image(data, size);

	if ( png->HasContent() )
	{
		sprite = new LuxSprite( GraphicsNative );
		sprite->sheet_area.x = sprite->sheet_area.y = 0;
		sprite->sheet_area.w = png->Width();
		sprite->sheet_area.h = png->Height();

		SDL_Surface * temp_sheet = SDL_CreateRGBSurfaceFrom(png->GetPixels(), png->Width(), png->Height(), 32, png->Width()*4, RMASK, GMASK, BMASK, AMASK );
		SDL_SetAlpha(temp_sheet, 0, 0);

		if ( temp_sheet )
		{
			sprite->data = (void *)SDL_DisplayFormatAlpha(temp_sheet);
		}
		SDL_FreeSurface( temp_sheet );
	}
	delete png;
	return sprite;

}



/* Drawing Functions */
LUX_DISPLAY_FUNCTION int32_t Lux_NATIVE_DrawChar( int32_t cchar, int32_t x, int32_t y, int32_t z, ObjectEffect effects, bool allow_custom )
{
	SDL_Rect area;
	SDL_Surface * surface = nullptr;
	LuxSprite * sprite_data = nullptr;
	int32_t offset = 7;

	int8_t axis;
	int8_t button;
	int8_t pointer;

	UnicodeToInput( cchar, &axis, &button, &pointer );

	if ( axis >= 0 || button >= 0 || pointer >= 0 )
	{
		sprite_data = lux::display->GetInputSprite( 0, axis, button, pointer );
	}
	else if ( sdlgraphics_customtext && allow_custom )
	{
		sprite_data = lux::display->GetSprite( lux::display->sprite_font, elix::string::FromInt(cchar) );
	}

	if ( sprite_data )
	{
		surface = (SDL_Surface*)sprite_data->GetData();

		area.x = x;
		area.y = y;
		area.w = surface->w;
		area.h = surface->h;

		SDL_BlitSurface(surface, nullptr, sdlgraphics_screen, &area);

		offset = surface->w + 2;
	}
	else
	{
		if ( cchar >= 32 && cchar <= 128 )
		{
			//characterRGBA(sdlgraphics_screen, x, y, (int8_t)cchar, effects.primary_colour.r, effects.primary_colour.g, effects.primary_colour.b, effects.primary_colour.a );
		}
		else
		{
			//characterRGBA(sdlgraphics_screen, x, y, '?', effects.primary_colour.r, effects.primary_colour.g, effects.primary_colour.b, effects.primary_colour.a );
		}

	}
	return offset;

}


LUX_DISPLAY_FUNCTION void Lux_NATIVE_DrawSprite( LuxSprite * sprite, LuxRect dest_rect, ObjectEffect effects )
{
	if ( sprite == nullptr )
		return;

	if ( !sdlgraphics_screen )
	{
		lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR, __FILE__ , __LINE__) << " | Not a valid target surface." << std::endl;
		return;
	}

	SDL_Surface * surface = (SDL_Surface*) sprite->GetData(effects);

	if ( surface->w == 0 || surface->h == 0 )
		return;

	SDL_Rect draw, area;
	draw.x = dest_rect.x;
	draw.y = dest_rect.y;
	draw.w = surface->w;
	draw.h = surface->h;

	if ( (dest_rect.w == draw.w) && (dest_rect.h == draw.h) )
	{
		dest_rect.w = dest_rect.h = 0;
		effects.tile_object = 0;
	}

	if ( effects.flip_image&16 )
	{
		effects.flip_image -= 16;
	}

	if ( (dest_rect.w > 0 || dest_rect.h > 0) && effects.tile_object && !effects.rotation)
	{
		if ( effects.flip_image == 1 || effects.flip_image == 3 )
		{
			area.w = dest_rect.h;
			area.h = dest_rect.w;
		}
		else
		{
			area.w = dest_rect.w;
			area.h = dest_rect.h;
		}

		for( area.x = 0; area.x < area.w; area.x += surface->w )
		{
			for( area.y = 0; area.y < area.h; area.y += surface->h )
			{
				draw.x = dest_rect.x + area.x;
				draw.y = dest_rect.y + area.y;
				SDL_BlitSurface(surface, nullptr, sdlgraphics_screen, &draw);
			}
		}
	}
	else
	{
		if ( effects.rotation )
		{
			draw.x -= (surface->w/2) - (dest_rect.w/2);
			draw.y -= (surface->h/2) - (dest_rect.h/2);
		}
		SDL_BlitSurface(surface, nullptr, sdlgraphics_screen, &draw);
	}
}

LUX_DISPLAY_FUNCTION void Lux_NATIVE_DrawRect( LuxRect dest_rect, ObjectEffect effects)
{
	if ( !sdlgraphics_screen )
	{
		lux::core->SystemMessage(SYSTEM_MESSAGE_LOG) << __FILE__ << ":" << __LINE__ << " | Not a valid surface." << std::endl;
		return;
	}

	Uint32 new_color;
	SDL_Rect tmp_rect;

	tmp_rect.x = dest_rect.x;
	tmp_rect.y = dest_rect.y;
	tmp_rect.w = dest_rect.w;
	tmp_rect.h = dest_rect.h;

	if ( effects.style == STYLE_NEGATIVE )
		new_color = SDL_MapRGB( sdlgraphics_screen->format,255-effects.primary_colour.r, 255-effects.primary_colour.g, 255-effects.primary_colour.b );
	else
		new_color = SDL_MapRGB( sdlgraphics_screen->format,effects.primary_colour.r, effects.primary_colour.g, effects.primary_colour.b );

	SDL_FillRect(sdlgraphics_screen, &tmp_rect, new_color);

}

LUX_DISPLAY_FUNCTION void Lux_NATIVE_DrawPolygon ( int16_t * x_point, int16_t *y_point, uint16_t point_count, LuxRect position, ObjectEffect effects, void * texture )
{

}

LUX_DISPLAY_FUNCTION void Lux_NATIVE_DrawCircle( LuxRect dest_rect, ObjectEffect effects )
{


}

LUX_DISPLAY_FUNCTION void Lux_NATIVE_DrawLine( LuxRect points, ObjectEffect effects)
{

}

LUX_DISPLAY_FUNCTION void Lux_NATIVE_DrawText( std::string text, LuxRect dest_rect, ObjectEffect effects, bool allow_custom)
{
	uint16_t x = dest_rect.x;
	uint16_t y = dest_rect.y;
	std::string::iterator object = text.begin();

	if ( text.empty() )
		return;

	for ( object = text.begin(); object != text.end(); object++ )
	{
		uint8_t utfchar = *object;
		uint32_t cchar = utfchar;
		/*
			194-223 2 bytes
			224-239 3 bytes
			240-244 4 bytes
		*/
		if (cchar == '\n' || cchar == '\r')
		{
			y += ( sdlgraphics_customtext && allow_custom ? sdlgraphics_customtext_height + 2 : 10);
			x = dest_rect.x;
		}
		else if ( cchar <= 128 )
		{
			x += Lux_NATIVE_DrawChar(cchar, x, y, dest_rect.z, effects, allow_custom);
		}
		else if ( cchar < 224 )
		{
			object++;
			uint32_t next = *object;

			cchar = ((cchar << 6) & 0x7ff) + (next & 0x3f);

			x += Lux_NATIVE_DrawChar(cchar, x, y, dest_rect.z, effects, allow_custom);
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

			x += Lux_NATIVE_DrawChar(cchar, x, y, dest_rect.z, effects, allow_custom);
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

			x += Lux_NATIVE_DrawChar(cchar, x, y, dest_rect.z, effects, allow_custom);
		}
	}


}
