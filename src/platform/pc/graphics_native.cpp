/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

/* Required Headers */
#include "core.h"
#include "game_config.h"
#include "engine.h"
#include "display/display.h"
#include "misc_functions.h"
#include "mokoi_game.h"
#include "map_object.h"
#include "platform_functions.h"
#include "graphics_native.h"
#include "display/display_types.h"
#include "graphics_system.h"

#include "elix/elix_png.hpp"
#include "elix/elix_string.hpp"
#include <SDL/SDL_gfxPrimitives.h>
#include <SDL/SDL_rotozoom.h>
#include <SDL/SDL_image.h>

extern ObjectEffect default_fx;

GraphicSystem GraphicsNative = {
	&Lux_NATIVE_Init,
	&Lux_NATIVE_Destory,
	&Lux_NATIVE_Display2Screen,
	&Lux_NATIVE_TextSprites,
	&Lux_GRAPHICS_PreShow,
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
	&Lux_NATIVE_CacheSprite,
	&Lux_NATIVE_FreeSprite,
	&Lux_NATIVE_PNGtoSprite,

	&Lux_NATIVE_DrawSprite,
	&Lux_NATIVE_DrawRect,
	&Lux_NATIVE_DrawCircle,
	&Lux_NATIVE_DrawPolygon,
	&Lux_NATIVE_DrawLine,
	&Lux_NATIVE_DrawText

};



uint32_t Lux_Effect_Hex( ObjectEffect fx);

/* Global Variables */
Uint32 sdlgraphics_flags = 0;
LuxColour sdlgraphics_colour = {0,0,0,255};
SDL_Rect sdlgraphics_dimension = {0, 0, 320,240};
SDL_Rect * sdlgraphics_dirty = 0;
float sdlgraphics_ratio[2] = {1.0, 1.0};
int32_t sdlgraphics_scale = 0;

int sdlgraphics_dirtycount = 0;
SDL_Surface * sdlgraphics_screen = NULL;
SDL_Surface * sdlgraphics_realscreen = NULL;
std::string sdlgraphics_title;
uint32_t sdlgraphics_fps = 0, sdlgraphics_fpstime = 0;
bool sdlgraphics_customtext = false;
int32_t sdlgraphics_customtext_height = 10;

LuxPolygon * sdlgraphics_cursor = NULL;


/* Local Functions */
SDL_Surface * SDL_Surface_Colourise( SDL_Surface * src, LuxColour colour )
{
	if ( !src )
		return NULL;

	int32_t x, y;
	uint8_t r, g, b, a = 0;
	float fr, fg, fb, fa = 1.0f;
	SDL_Surface * temp = SDL_CreateRGBSurface(SDL_SWSURFACE, src->w, src->h, src->format->BitsPerPixel, src->format->Rmask, src->format->Gmask, src->format->Bmask, src->format->Amask);

	fr = (float)colour.r / 255;
	fg = (float)colour.g / 255;
	fb = (float)colour.b / 255;
	fa = (float)colour.a / 255;

	SDL_LockSurface(src);
	SDL_LockSurface(temp);
	for (x = 0; x < temp->w; x++)
	{
		for (y = 0; y < temp->h; y++)
		{
			SDL_GetRGBA(GetPixel(src, x, y), src->format,&r,&g,&b,&a);
			r = (Uint8)((float)r * fr);
			g = (Uint8)((float)g * fg);
			b = (Uint8)((float)b * fb);
			a = (Uint8)((float)a * fa);
			pixelRGBA(temp, x, y, r, g, b, a);
		}
	}
	SDL_UnlockSurface(temp);
	SDL_UnlockSurface(src);
	return temp;
}

SDL_Surface * SDL_Surface_SwitchColour( SDL_Surface * src, LuxColour colour, LuxColour colour2 )
{
	if ( !src )
		return NULL;

	int32_t x, y;
	uint8_t r, g, b, a = 0;
	SDL_Surface * temp = SDL_CreateRGBSurface(SDL_SWSURFACE, src->w, src->h, src->format->BitsPerPixel, src->format->Rmask, src->format->Gmask, src->format->Bmask, src->format->Amask);

	SDL_LockSurface(src);
	SDL_LockSurface(temp);
	for (x = 0; x < temp->w; x++)
	{
		for (y = 0; y < temp->h; y++)
		{
			SDL_GetRGBA(GetPixel(src, x, y), src->format,&r,&g,&b,&a);
			if ( r == colour.r && b == colour.b && g == colour.g && a == colour.a )
				pixelRGBA(temp, x, y, colour2.r, colour2.g, colour2.b, colour2.a);
			else
				pixelRGBA(temp, x, y, r, g, b, a);
		}
	}
	SDL_UnlockSurface(temp);
	SDL_UnlockSurface(src);
	return temp;
}

SDL_Surface * SDL_Surface_Negative( SDL_Surface * src )
{
	if ( !src )
		return NULL;

	int32_t x, y;
	uint8_t r, g, b, a = 0;
	SDL_Surface * temp = SDL_CreateRGBSurface(SDL_SWSURFACE, src->w, src->h, src->format->BitsPerPixel, src->format->Rmask, src->format->Gmask, src->format->Bmask, src->format->Amask);

	SDL_LockSurface(src);
	SDL_LockSurface(temp);
	for (x = 0; x < temp->w; x++)
	{
		for (y = 0; y < temp->h; y++)
		{
			SDL_GetRGBA(GetPixel(src, x, y), src->format,&r,&g,&b,&a);
			r = 255-r;
			g = 255-g;
			b = 255-b;
			pixelRGBA(temp, x, y, r, g, b, a);
		}
	}
	SDL_UnlockSurface(temp);
	SDL_UnlockSurface(src);
	return temp;
}

inline bool SDL_Surface_IsVisiblePixel( int32_t x, int32_t y, SDL_Surface * src )
{
	uint32_t pixel;
	uint8_t r, g, b, a = 0;
	pixel = GetPixel(src, x, y);
	if (pixel)
		SDL_GetRGBA(pixel, src->format, &r,&g,&b,&a);
	return (a);
}


SDL_Surface * SDL_Surface_Outline( SDL_Surface * src, LuxColour colour, uint8_t l  )
{
	if ( !src )
		return NULL;

	uint32_t pixel;
	uint8_t r, g, b, a = 0;
	int32_t x, y;
	SDL_Surface * temp = SDL_CreateRGBSurface(SDL_SWSURFACE, src->w, src->h, src->format->BitsPerPixel, src->format->Rmask, src->format->Gmask, src->format->Bmask, src->format->Amask);

	SDL_LockSurface(src);
	SDL_LockSurface(temp);
	SDL_FillRect(temp, NULL, SDL_MapRGBA(temp->format,0,0,0,0) );
	for (x = 0; x < src->w; x++)
	{
		for (y = 0; y < src->h; y++)
		{
			pixel = GetPixel(src, x, y);

			SDL_GetRGBA( pixel, src->format, &r,&g,&b,&a );
			if ( a )
			{
				pixelRGBA(temp, x+1, y+1, r, g, b, a);

				if ( !SDL_Surface_IsVisiblePixel( x-1, y, src ) )
					pixelRGBA(temp, x, y+1, colour.r, colour.g, colour.b, colour.a);

				if ( !SDL_Surface_IsVisiblePixel( x+1, y, src ) )
					pixelRGBA(temp, x+2, y+1, colour.r, colour.g, colour.b, colour.a);

				if ( !SDL_Surface_IsVisiblePixel( x, y-1, src ) )
					pixelRGBA(temp, x+1, y, colour.r, colour.g, colour.b, colour.a);

				if ( !SDL_Surface_IsVisiblePixel( x, y+1, src ) )
					pixelRGBA(temp, x+1, y+2, colour.r, colour.g, colour.b, colour.a);

			}

		}
	}
	SDL_UnlockSurface(src);
	SDL_UnlockSurface(temp);
	return temp;
}

SDL_Surface * SDL_Surface_Flip( SDL_Surface * original, uint8_t angle )
{
	if ( !original )
		return NULL;

	SDL_Surface * newsuface;

	int32_t x, y;
	int32_t xp, yp;
	uint32_t pixel;
	bool flip = false;
	bool swap = false;

	if ( angle >= 16 )
	{
		flip = true;
		angle -= 16;
	}
	swap = (angle == 1 || angle == 3);

	newsuface = SDL_CreateRGBSurface(SDL_SWSURFACE, (swap ? original->h : original->w), (swap ? original->w : original->h),
									 original->format->BitsPerPixel,
									 original->format->Rmask,
									 original->format->Gmask,
									 original->format->Bmask,
									 original->format->Amask);

	SDL_FillRect(newsuface, NULL, SDL_MapRGBA(newsuface->format, 255, 0, 0, 255));
	SDL_LockSurface(original);
	SDL_LockSurface(newsuface);

	switch ( angle )
	{
		case 1:
			for ( y = 0, yp = original->h-1; y < original->h; y++, yp-- ) {
				xp = ( flip ? original->w-1 : 0);
				for ( x = 0; x < original->w; x++ ) {
					pixel = GetPixel(original, xp, yp);
					PutPixel(newsuface, y, x, pixel);
					if ( flip )
						xp--;
					else
						xp++;
				}
			}
			break;
		case 2:
			for ( y = 0, yp = original->h-1; y < original->h; y++, yp-- ) {
				xp = ( !flip ? original->w-1 : 0);
				for ( x = 0; x < original->w; x++ ) {
					pixel = GetPixel(original, xp, yp);
					PutPixel(newsuface, x, y, pixel);
					if ( !flip )
						xp--;
					else
						xp++;
				}
			}
			break;
		case 3:
			for ( y = 0, yp = 0; y < original->h; y++, yp++ ) {
				xp = ( flip ? 0 : original->w-1);
				for ( x = 0; x < original->w; x++ ) {
					pixel = GetPixel(original, xp, yp);
					PutPixel(newsuface, y, x, pixel);
					if ( flip )
						xp++;
					else
						xp--;
				}
			}
			break;
		case 0:
			if ( flip )
			{
				for ( y = 0, yp = 0; y < original->h; y++, yp++) {
					for ( x = 0, xp = original->w-1; x < original->w; x++, xp-- ) {
						pixel = GetPixel(original, xp, yp);
						PutPixel(newsuface, x, y, pixel);
					}
				}
			}
			break;
	}
	SDL_UnlockSurface(original);
	SDL_UnlockSurface(newsuface);
	return newsuface;
}

SDL_Surface * SDL_Surface_Effects( SDL_Surface * orignal, ObjectEffect effects )
{
	if ( orignal == NULL )
		return NULL;

	bool update = false;
	bool colourize = false;
	bool rotozoom = false;
	bool flip = false;

	if ( effects.primary_colour.r != 255 || effects.primary_colour.b != 255 || effects.primary_colour.g != 255 || effects.primary_colour.a != 255 || effects.style == STYLE_NEGATIVE)
	{
		colourize = true;
		update = true;
	}
	if ( effects.rotation != 0 )
	{
		if ( effects.rotation >= 360 )
			effects.rotation = 0;
		else
		{
			rotozoom = true;
			update = true;
		}
	}
	if ( effects.scale_xaxis != 1000 )
	{

		if ( effects.scale_xaxis >= 10000 )
			effects.scale_xaxis = 1000;
		else
		{
			rotozoom = true;
			update = true;
		}
	}
	if ( effects.scale_yaxis != 1000 )
	{
		if ( effects.scale_yaxis >= 10000 )
			effects.scale_yaxis = 1000;
		else
		{
			rotozoom = true;
			update = true;
		}
	}

	if ( effects.flip_image )
	{
		update = true;
		flip = true;
	}

	if ( effects.style > STYLE_HGRADIENT ) //Gradients not supporting for SDL Mode
	{
		update = true;
	}

	if ( update )
	{
		SDL_Surface * new_surface = NULL;
		SDL_Surface * temp_surface = NULL;
		if ( effects.style == STYLE_REPCOLOUR )
		{
			new_surface = SDL_Surface_SwitchColour( orignal, effects.primary_colour, effects.secondary_colour );
		}
		else if ( colourize )
		{
			new_surface = (effects.style == STYLE_NEGATIVE ? SDL_Surface_Negative( orignal ) :SDL_Surface_Colourise(orignal, effects.primary_colour) );
		}

		if ( flip )
		{
			temp_surface = SDL_Surface_Flip( (new_surface ? new_surface : orignal), effects.flip_image);
			SDL_FreeSurface( new_surface );
			new_surface = temp_surface;
		}
		if ( effects.style == STYLE_GLOW || effects.style == STYLE_OUTLINE )
		{
			temp_surface = SDL_Surface_Outline((new_surface ? new_surface : orignal), effects.secondary_colour, 1);
			SDL_FreeSurface( new_surface );
			new_surface = temp_surface;
		}
		if ( rotozoom )
		{
			temp_surface = rotozoomSurfaceXY( (new_surface ? new_surface : orignal), (double)effects.rotation, (double)effects.scale_xaxis/1000.0f, (double)effects.scale_yaxis/1000.0f,0);
			SDL_FreeSurface( new_surface );
			new_surface = temp_surface;
		}
		if ( new_surface != orignal )
		{
			if ( new_surface )
			{
				SDL_Surface * optimize_surface = SDL_DisplayFormatAlpha(new_surface);
				SDL_FreeSurface(new_surface);
				return optimize_surface;
			}
			else
			{
				return orignal;
			}
		}
		return new_surface;
	}
	return orignal;
}

SDL_Surface * SDL_Surface_Rectangle( LuxRect in_rect, ObjectEffect effects, SDL_Rect *out_rect )
{
	(*out_rect).x = in_rect.x;
	(*out_rect).y = in_rect.y;
	(*out_rect).w = in_rect.w;
	(*out_rect).h = in_rect.h;

	/* Resize are if needed */
	if ( effects.style == STYLE_GLOW || effects.style == STYLE_OUTLINE )
	{
		(*out_rect).x -= 1;
		(*out_rect).y -= 1;
		(*out_rect).w += 2;
		(*out_rect).h += 2;
	}

	SDL_Surface * rectangle = SDL_CreateRGBSurface(SDL_SWSURFACE, (*out_rect).w, (*out_rect).h, 32, RMASK, GMASK, BMASK, AMASK );

	if ( effects.style == STYLE_VGRADIENT || effects.style == STYLE_HGRADIENT )
	{
		uint8_t a = 0;
		int32_t line_count = 0;
		float r, g, b, r1, g1, b1;

		r = (float)effects.primary_colour.r;
		g = (float)effects.primary_colour.g;
		b = (float)effects.primary_colour.b;
		a = effects.primary_colour.a;

		if ( effects.style == STYLE_VGRADIENT )
		{
			r1 = (float)(effects.secondary_colour.r - effects.primary_colour.r) / (float)(in_rect.h);
			b1 = (float)(effects.secondary_colour.b - effects.primary_colour.b) / (float)(in_rect.h);
			g1 = (float)(effects.secondary_colour.g - effects.primary_colour.g) / (float)(in_rect.h);
		}
		else
		{
			r1 = (float)(effects.secondary_colour.r - effects.primary_colour.r) / (float)(in_rect.w);
			b1 = (float)(effects.secondary_colour.b - effects.primary_colour.b) / (float)(in_rect.w);
			g1 = (float)(effects.secondary_colour.g - effects.primary_colour.g) / (float)(in_rect.w);
		}

		SDL_LockSurface(rectangle);
		if ( effects.style == STYLE_VGRADIENT )
		{
			for( line_count = 0; line_count < rectangle->h; line_count++ )
			{
				if ( line_count == 0)
					hlineRGBA(rectangle, 0, (*out_rect).w, line_count, effects.primary_colour.r, effects.primary_colour.g, effects.primary_colour.b, a);
				else
					hlineRGBA(rectangle, 0, (*out_rect).w, line_count, (Uint8)r, (Uint8)g, (Uint8)b, a);
				r += r1;
				g += g1;
				b += b1;
			}
		}
		else
		{
			for( line_count = 0; line_count < rectangle->w; line_count++ )
			{
				if ( line_count == 0 )
					vlineRGBA(rectangle, line_count, 0, (*out_rect).h, effects.primary_colour.r, effects.primary_colour.g, effects.primary_colour.b, a);
				else
					vlineRGBA(rectangle, line_count, 0, (*out_rect).h, (Uint8)r, (Uint8)g, (Uint8)b, a);
				r += r1;
				g += g1;
				b += b1;
			}
		}
		SDL_UnlockSurface(rectangle);
	}
	else if ( effects.style == STYLE_GLOW || effects.style == STYLE_OUTLINE )
	{
		rectangleRGBA(rectangle, 0, 0, (*out_rect).w+1, (*out_rect).h+1, effects.secondary_colour.r, effects.secondary_colour.g, effects.secondary_colour.b,effects.secondary_colour.a);
		if ( effects.primary_colour.a != 0 )
		{
			boxRGBA(rectangle, 1, 1, in_rect.w, in_rect.h, effects.primary_colour.r, effects.primary_colour.g, effects.primary_colour.b, effects.primary_colour.a);
		}
	}
	else if ( effects.style == STYLE_NEGATIVE )
	{
		boxRGBA(rectangle, 0, 0, in_rect.w, in_rect.h, 255-effects.primary_colour.r, 255-effects.primary_colour.g, 255-effects.primary_colour.b, effects.primary_colour.a);
	}
	else if ( effects.style == STYLE_REPCOLOUR )
	{
		boxRGBA(rectangle, 0, 0, in_rect.w, in_rect.h, 255, 255, 255, 255);
	}
	else
	{
		boxRGBA(rectangle, 0, 0, in_rect.w, in_rect.h, effects.primary_colour.r, effects.primary_colour.g, effects.primary_colour.b, effects.primary_colour.a);
	}


	if ( effects.rotation )
	{
		SDL_Surface * temp_surface = rotozoomSurfaceXY( rectangle, (double)effects.rotation, 1.0, 1.0,0);
		SDL_FreeSurface( rectangle );


		(*out_rect).x -= (temp_surface->w - in_rect.w) / 2;
		(*out_rect).y -= (temp_surface->h - in_rect.h) / 2;

		(*out_rect).w = temp_surface->w;
		(*out_rect).h = temp_surface->h;


		return temp_surface;
	}
	return rectangle;
}

// Loaded a file from the game file.
SDL_Surface * SDL_Surface_LoadImage( std::string file )
{
	uint8_t * data = NULL;
	uint32_t size;
	SDL_Surface * temp_surface = NULL;
	if ( lux::game_data )
	{
		size = lux::game_data->GetFile(file, &data, false);
		if ( size )
		{
			SDL_RWops * src = SDL_RWFromMem(data, size);
			if ( IMG_isPNG(src) )
			{
				temp_surface = IMG_Load_RW(src, 1);
				SDL_SetAlpha(temp_surface, 0, 255);
			}
			else
				lux::core->SystemMessage(SYSTEM_MESSAGE_LOG) << file << ": not a png image" << std::endl;
		}
	}
	return temp_surface;
}

// Switches 'sdlgraphics_screen' value
// Can be use to redirect drawing functions to another SDL_Surface
// Passing NULL will set back to the default.
bool Lux_NATIVE_ChangeOutput( SDL_Surface * output )
{
	if ( output )
		return (bool)(sdlgraphics_screen = output);
	else
		return (bool)(sdlgraphics_screen = SDL_GetVideoSurface());
}

/* Creation,destruction and loop Functions */
LUX_DISPLAY_FUNCTION bool Lux_NATIVE_Init(  uint16_t  width, uint16_t height, uint8_t bpp, uint16_t * actual_width, uint16_t * actual_height )
{
	/* Set Init Flags */
	if ( lux::config->GetString("display.surface") == "hardware" )
		sdlgraphics_flags |= SDL_HWSURFACE;
	else
		sdlgraphics_flags |= SDL_SWSURFACE;

	if ( lux::config->GetBoolean("display.dbuff") )
		sdlgraphics_flags |= SDL_DOUBLEBUF;

	if ( lux::config->GetBoolean("display.fullscreen") )
		sdlgraphics_flags |= SDL_FULLSCREEN;

	if ( SDL_WasInit(SDL_INIT_VIDEO) == SDL_INIT_VIDEO )
	{
		SDL_QuitSubSystem(SDL_INIT_VIDEO);
	}
	SDL_InitSubSystem(SDL_INIT_VIDEO);

	sdlgraphics_scale = lux::config->GetNumber("display.autoscale");

	if ( sdlgraphics_scale )
	{
		// Creates a fake screen to draw unscaled to, before drawing on real screen
		sdlgraphics_realscreen = SDL_SetVideoMode(width*2, height*2, bpp, sdlgraphics_flags );
		sdlgraphics_screen = SDL_CreateRGBSurface(SDL_SWSURFACE,
												  width, height, sdlgraphics_realscreen->format->BitsPerPixel,
												  sdlgraphics_realscreen->format->Rmask, sdlgraphics_realscreen->format->Gmask,
												  sdlgraphics_realscreen->format->Bmask, sdlgraphics_realscreen->format->Amask);

		sdlgraphics_ratio[0] = sdlgraphics_ratio[1] = 2.0;

		if ( !sdlgraphics_realscreen )
		{
			lux::core->SystemMessage(__FILE__, __LINE__, SYSTEM_MESSAGE_LOG) << " Couldn't set " << width << "x" << height << " video mode. " << SDL_GetError() << std::endl;
			return false;
		}
	}
	else
	{
		sdlgraphics_realscreen = NULL;
		sdlgraphics_screen = SDL_SetVideoMode(width, height, bpp, sdlgraphics_flags );
		if ( !sdlgraphics_screen )
		{
			lux::core->SystemMessage(__FILE__, __LINE__, SYSTEM_MESSAGE_LOG) << " Couldn't set " << width << "x" << height << " video mode. " << SDL_GetError() << std::endl;
			return false;
		}
	}
	sdlgraphics_dimension.w = width;
	sdlgraphics_dimension.h = height;

	SDL_Surface * icon = SDL_Surface_LoadImage("./resources/icon256.png");

	if ( !icon )
	{
		icon = SDL_Surface_LoadImage("./resources/icon32.png");
	}
	sdlgraphics_title = lux::config->GetString("project.title") + " (Software Rendering)";
	SDL_WM_SetCaption(sdlgraphics_title.c_str(), NULL);
	if (icon != NULL)
	{
		SDL_SetAlpha(icon, SDL_SRCALPHA, 0);
		SDL_WM_SetIcon(icon, NULL);
		SDL_FreeSurface(icon);
	}

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

	if ( sdlgraphics_realscreen )
		SDL_FreeSurface( sdlgraphics_screen );
	delete sdlgraphics_cursor;
}

LUX_DISPLAY_FUNCTION void Lux_NATIVE_UpdateRect(uint8_t screen,LuxRect rect)
{
	if (!sdlgraphics_dirty)
	{
		sdlgraphics_dirty = new SDL_Rect[64];
	}
	if (sdlgraphics_dirtycount < 63)
	{
		sdlgraphics_dirty[sdlgraphics_dirtycount].x = rect.x;
		sdlgraphics_dirty[sdlgraphics_dirtycount].y = rect.y;
		sdlgraphics_dirty[sdlgraphics_dirtycount].h = rect.h;
		sdlgraphics_dirty[sdlgraphics_dirtycount].w = rect.w;
		sdlgraphics_dirtycount++;
	}
}

LUX_DISPLAY_FUNCTION void Lux_NATIVE_Show( uint8_t screen )
{
	if ( sdlgraphics_screen )
	{
		if ( sdlgraphics_scale )
		{
			SDL_Surface * s = zoomSurface( sdlgraphics_screen, 2.0, 2.0, 0 );
			SDL_BlitSurface(s, NULL, sdlgraphics_realscreen, NULL);
			SDL_Flip( sdlgraphics_realscreen );
			SDL_FreeSurface( s );
		}
		else
		{
			if ( sdlgraphics_dirtycount > 0 )
			{
				SDL_UpdateRects(sdlgraphics_screen,sdlgraphics_dirtycount,sdlgraphics_dirty);
			}
			else
			{
				if ( SDL_Flip(sdlgraphics_screen) )
				{
					lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR) << "Lux_NATIVE_Show:" << SDL_GetError() << std::endl;
				}
			}
		}
		Uint32 new_color = SDL_MapRGB( sdlgraphics_screen->format,sdlgraphics_colour.r, sdlgraphics_colour.g, sdlgraphics_colour.b );
		SDL_FillRect(sdlgraphics_screen, &sdlgraphics_dimension, new_color);
		sdlgraphics_dirtycount = 0;

		if ( lux::core->GetTime() > (sdlgraphics_fpstime + 1000) )
		{
			std::stringstream title_msg;
			title_msg << sdlgraphics_title << " " << sdlgraphics_fps;
			sdlgraphics_fpstime = lux::core->GetTime();
			sdlgraphics_fps = 0;
			SDL_WM_SetCaption( title_msg.str().c_str(), NULL);
		}
		sdlgraphics_fps++;
	}
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
	lux::config->SetBoolean("display.fullscreen", able);
	Lux_NATIVE_ChangeOutput(NULL);
	if ( SDL_WM_ToggleFullScreen(sdlgraphics_screen) )
		return true;

	// Windows doesn't support SDL_WM_ToggleFullScreen so lets to it the hard way.
	lux::config->SetBoolean("display.fullscreen", able);

	SDL_FreeSurface( sdlgraphics_screen );
	uint8_t bpp = lux::config->GetNumber("display.bpp") * 8;
	Lux_NATIVE_Init( std::string title, sdlgraphics_dimension.w, sdlgraphics_dimension.h, bpp, able );
	return true;
}

LUX_DISPLAY_FUNCTION void Lux_NATIVE_DisplayPointer( uint8_t player, int16_t x, int16_t y, ObjectEffect effect )
{
	LuxRect position;

	position.x = x;
	position.y = y;

	Lux_NATIVE_DrawPolygon(sdlgraphics_cursor->x, sdlgraphics_cursor->y, sdlgraphics_cursor->count, position, effect, NULL);

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
	if ( sprite == NULL )
		return false;
	if ( !sprite->_cache.empty() )
	{
		std::map<uint32_t, mem_pointer>::iterator p = sprite->_cache.begin();
		while( p != sprite->_cache.end() )
		{
			if ( p->second )
				SDL_FreeSurface((SDL_Surface *)p->second);
			p++;
		}
		sprite->_cache.clear();
	}
	if (sprite->data)
	{
		SDL_FreeSurface((SDL_Surface *)sprite->data);
	}
	sprite->data = NULL;
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
					pixelRGBA(temp_sheet, x, y, r, g, b, a );
				}
			}
		}
		SDL_SetAlpha(temp_sheet, 0, 0);
		sprite->data = (mem_pointer)SDL_DisplayFormatAlpha(temp_sheet);
	}
	SDL_FreeSurface( temp_sheet );

	return true;
}


/* Lux_NATIVE_CacheSprite
 *
 @ sprite:
 @ Effects:
 -
 */
LUX_DISPLAY_FUNCTION void * Lux_NATIVE_CacheSprite( LuxSprite * sprite, ObjectEffect fx )
{
	uint32_t hex = fx.Hex();

	std::map<uint32_t, void *>::iterator p = sprite->_cache.begin();
	p = sprite->_cache.find(hex);
	if ( p != sprite->_cache.end() )
	{
		return p->second;
	}
	void * temp = (void*)SDL_Surface_Effects( (SDL_Surface *)sprite->data, fx );
	if ( temp != sprite->data )
	{
		sprite->_cache.insert( std::make_pair( hex, temp ) );
	}

	return temp;
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

			SDL_Surface * temp_sheet = SDL_CreateRGBSurface( sdlgraphics_flags, tmp_rect.w, tmp_rect.h, 32, RMASK, GMASK, BMASK, AMASK );
			if ( temp_sheet )
			{
				SDL_BlitSurface(parent_sheet, &tmp_rect, temp_sheet, NULL);
				SDL_SetAlpha(temp_sheet, 0, 0);

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
	LuxSprite * sprite = NULL;
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
LUX_DISPLAY_FUNCTION void Lux_NATIVE_DrawSprite( LuxSprite * sprite, LuxRect dest_rect, ObjectEffect effects )
{
	if ( sprite == NULL )
		return;

	if ( !sdlgraphics_screen )
	{
		lux::core->SystemMessage(__FILE__ , __LINE__, SYSTEM_MESSAGE_ERROR) << " | Not a valid target surface." << std::endl;
		return;
	}
	SDL_Surface * orignal_surface = (SDL_Surface*) sprite->GetData( default_fx );
	SDL_Surface * surface = (SDL_Surface*) sprite->GetData(effects);

	if ( surface == NULL )
		return;

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
			area.w = (dest_rect.h / surface->h) * surface->h;
			area.h = (dest_rect.w / surface->w) * surface->w;
		}
		else
		{
			area.w = (dest_rect.w / surface->w) * surface->w;
			area.h = (dest_rect.h / surface->h) * surface->h;
		}

		for( area.x = 0; area.x < area.w; area.x += surface->w )
		{
			for( area.y = 0; area.y < area.h; area.y += surface->h )
			{
				draw.x = dest_rect.x + area.x;
				draw.y = dest_rect.y + area.y;
				SDL_BlitSurface(surface, NULL, sdlgraphics_screen, &draw);
			}
		}
	}
	else
	{
		if ( effects.rotation )
		{
			if ( orignal_surface != NULL )
			{
				draw.x -= (surface->w - orignal_surface->w) / 2;
				draw.y -= (surface->h - orignal_surface->h) / 2;
			}
		}
		SDL_BlitSurface(surface, NULL, sdlgraphics_screen, &draw);
	}
}




LUX_DISPLAY_FUNCTION void Lux_NATIVE_DrawRect( LuxRect dest_rect, ObjectEffect effects)
{
	if ( !sdlgraphics_screen )
	{
		lux::core->SystemMessage(__FILE__, __LINE__, SYSTEM_MESSAGE_LOG) << " Not a valid surface." << std::endl;
		return;
	}

	SDL_Rect area;
	area.x = dest_rect.x;
	area.y = dest_rect.y;
	area.w = dest_rect.w;
	area.h = dest_rect.h;

	/* We are going to cheat */
	if ( (effects.style > STYLE_NORMAL && effects.style < STYLE_NEGATIVE) || effects.rotation )
	{
		SDL_Surface * surface = SDL_Surface_Rectangle( dest_rect, effects, &area );
		SDL_BlitSurface(surface, NULL, sdlgraphics_screen, &area);
		SDL_FreeSurface(surface);
	}
	else if ( effects.style == STYLE_NEGATIVE )
	{
		boxRGBA(sdlgraphics_screen, area.x, area.y, area.x + area.w, area.y + area.h, 255-effects.primary_colour.r, 255-effects.primary_colour.g, 255-effects.primary_colour.b,effects.primary_colour.a);
	}
	else
	{
		boxRGBA(sdlgraphics_screen, area.x, area.y, area.x + area.w, area.y + area.h, effects.primary_colour.r, effects.primary_colour.g, effects.primary_colour.b, effects.primary_colour.a);
	}

}

LUX_DISPLAY_FUNCTION void Lux_NATIVE_DrawPolygon ( int16_t * x_point, int16_t *y_point, uint16_t point_count, LuxRect position, ObjectEffect effects, void * texture )
{
	if ( !sdlgraphics_screen )
	{
		lux::core->SystemMessage(__FILE__, __LINE__, SYSTEM_MESSAGE_LOG) << " Not a valid surface." << std::endl;
		return;
	}

	if ( point_count < 3 || point_count > 100 )
	{
		return;
	}

	/* SDL_gfx doesn't allow for x/y position, so we need to update the points */
	for ( uint8_t pc = 0; pc < point_count; pc++ )
	{
		x_point[pc] += position.x;
		y_point[pc] += position.y;
	}


	if ( texture != NULL )
	{
		texturedPolygon(sdlgraphics_screen, x_point, y_point, point_count, (SDL_Surface *)texture, 0,0);
	}
	else
	{
		filledPolygonRGBA(sdlgraphics_screen, x_point, y_point, point_count, effects.primary_colour.r, effects.primary_colour.g, effects.primary_colour.b,effects.primary_colour.a);
	}
	//aapolygonRGBA(SDL_Surface * dst, Sint16 * vx, Sint16 * vy,int n, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

	/* Since we just modify the point we nee to reset them.
	 * Of course we shouldn't do it this way.
	 */
	for ( uint8_t pc = 0; pc < point_count; pc++ )
	{
		x_point[pc] -= position.x;
		y_point[pc] -= position.y;
	}

}

LUX_DISPLAY_FUNCTION void Lux_NATIVE_DrawCircle( LuxRect dest_rect, ObjectEffect effects )
{
	if ( !sdlgraphics_screen )
	{
		lux::core->SystemMessage(__FILE__, __LINE__, SYSTEM_MESSAGE_LOG) << " Not a valid surface." << std::endl;
		return;
	}
	filledEllipseRGBA(sdlgraphics_screen, dest_rect.x +(dest_rect.w/2), dest_rect.y+(dest_rect.h/2), (dest_rect.w/2), (dest_rect.h/2), effects.primary_colour.r, effects.primary_colour.g, effects.primary_colour.b,effects.primary_colour.a);
}

LUX_DISPLAY_FUNCTION void Lux_NATIVE_DrawLine( LuxRect points, ObjectEffect effects )
{
	lineRGBA(sdlgraphics_screen, points.x, points.y, points.w, points.h, effects.primary_colour.r, effects.primary_colour.g, effects.primary_colour.b,effects.primary_colour.a);
}

LUX_DISPLAY_FUNCTION int32_t Lux_NATIVE_DrawChar( int32_t cchar, int32_t x, int32_t y, int32_t z, ObjectEffect effects, bool allow_custom )
{
	SDL_Rect area;
	SDL_Surface * surface = NULL;
	LuxSprite * sprite_data = NULL;
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

		SDL_BlitSurface(surface, NULL, sdlgraphics_screen, &area);

		offset = surface->w + 2;
	}
	else
	{
		if ( cchar >= 32 && cchar <= 128 )
		{
			characterRGBA(sdlgraphics_screen, x, y, (int8_t)cchar, effects.primary_colour.r, effects.primary_colour.g, effects.primary_colour.b, effects.primary_colour.a );
		}
		else
		{
			characterRGBA(sdlgraphics_screen, x, y, '?', effects.primary_colour.r, effects.primary_colour.g, effects.primary_colour.b, effects.primary_colour.a );
		}

	}
	return offset;

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

