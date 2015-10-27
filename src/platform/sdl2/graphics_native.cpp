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

#include "display/display_types.h"
#include "graphics_native.h"
#include "display/graphics_system.h"
#include "display_bitfont.h"

#include "elix/elix_string.hpp"
#include "elix/elix_png.hpp"
#include "bitfont.h"

uint32_t Lux_Effect_Hex( ObjectEffect fx);
void SaveBMPFile(SDL_Surface *surface, std::string file);

struct NativeTexture {
	SDL_Texture * texnum;
	uint16_t w, h, tw, th;
	bool pot;
};

GraphicSystem GraphicsNative = {
	&Lux_NATIVE_Init,
	&Lux_NATIVE_Destory,
	&Lux_NATIVE_Display2Screen,
	&Lux_NATIVE_TextSprites,
	&Lux_GRAPHICS_PreShow,
	&Lux_GRAPHICS_UpdateRect,
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
	NULL,
	&Lux_NATIVE_FreeSprite,
	&Lux_NATIVE_PNGtoSprite,

	&Lux_NATIVE_DrawSprite,
	&Lux_NATIVE_DrawRect,
	&Lux_NATIVE_DrawCircle,
	&Lux_NATIVE_DrawPolygon,
	&Lux_NATIVE_DrawLine,
	&Lux_NATIVE_DrawText,
	&Lux_SDL2_DrawMessage,

	NULL,
	NULL

};

/* Shared */
Uint32 native_render_flags = 0;
Uint32 native_window_flags = 0;
SDL_Window * native_window = NULL;
SDL_Renderer * native_renderer = NULL;
std::string native_window_title;
SDL_Rect native_graphics_dimension = {0, 0, 320,240};

/* Global Variables */
DisplayBitFont * sdlgraphics_bitfont = NULL;

LuxColour sdlgraphics_colour = {0,0,0,255};

SDL_Rect * sdlgraphics_dirty = 0;
float sdlgraphics_ratio[2] = {1.0, 1.0};
int32_t sdlgraphics_scale = 0;

int sdlgraphics_dirtycount = 0;
SDL_Surface * sdlgraphics_screen = NULL;
SDL_Surface * sdlgraphics_realscreen = NULL;

uint32_t native_graphics_fps = 0, native_graphics_fpstime = 0;
bool sdlgraphics_customtext = false;
int32_t sdlgraphics_customtext_height = 32;

uint32_t sdlgraphics_texture_format = SDL_PIXELFORMAT_ABGR8888;
SDL_BlendMode sdlgraphics_blend_mode = SDL_BLENDMODE_BLEND;

void SDL2_SystemInfo();
void SDL2_OuputRenderingInfo( SDL_RendererInfo * info );
/* Local Functions */

int32_t SDL2_AcceratedRendering()
{
	int32_t n = SDL_GetNumRenderDrivers();
	int32_t c = 0;
	SDL_RendererInfo info;

	if ( n > 0 )
	{
		while (c < n)
		{
			if ( !SDL_GetRenderDriverInfo( c, &info) )
			{
				if (info.flags & SDL_RENDERER_ACCELERATED);
					return c;
			}
			c++;
		}

	}
	return -1;
}
/**
 * @brief Init video mode
 * @param width
 * @param height
 * @param bpp
 * @return Returns true if successful
 */
LUX_DISPLAY_FUNCTION bool Lux_NATIVE_Init(  uint16_t  width, uint16_t height, uint8_t bpp, uint16_t * actual_width, uint16_t * actual_height )
{
	int window_width = width;
	int window_height = height;

	/* Set Init Flags */
	native_window_flags = SDL_WINDOW_SHOWN|SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE;

	if ( lux::config->GetBoolean("display.fullscreen") )
		native_window_flags |= SDL_WINDOW_FULLSCREEN;

	native_render_flags = SDL_RENDERER_ACCELERATED;

	if ( width > height )
	{
		SDL_SetHintWithPriority( SDL_HINT_ORIENTATIONS, "LandscapeLeft", SDL_HINT_OVERRIDE );
	}
	else
	{
		SDL_SetHintWithPriority( SDL_HINT_ORIENTATIONS, "Portrait", SDL_HINT_OVERRIDE );
	}

	SDL_SetHintWithPriority( SDL_HINT_RENDER_VSYNC, "0", SDL_HINT_OVERRIDE );

	native_window = lux::core->GetWindow();

	SDL_SetWindowSize(native_window, width, height );
	SDL_GetWindowSize(native_window, &window_width, &window_height);

	int32_t preferred_render = SDL2_AcceratedRendering();

	native_renderer = SDL_CreateRenderer(native_window, preferred_render, native_render_flags);
	if ( !native_renderer )
	{
		lux::core->SystemMessage(__FILE__, __LINE__, SYSTEM_MESSAGE_LOG) << " Couldn't create Renderer. " << SDL_GetError() << std::endl;
		return false;
	}

	SDL_RendererInfo info;
	SDL_GetRendererInfo(native_renderer, &info);
	SDL2_SystemInfo();
	SDL2_OuputRenderingInfo( &info );

	native_window_title = lux::config->GetString("window.title") + " [" + info.name + "]";

	native_graphics_dimension.x = 0;
	native_graphics_dimension.y = 0;
	native_graphics_dimension.w = width;
	native_graphics_dimension.h = height;

	if ( actual_width )
		*actual_width = native_graphics_dimension.w;

	if ( actual_height )
		*actual_height = native_graphics_dimension.h;

	SDL_RenderSetViewport(native_renderer, &native_graphics_dimension);
	SDL_RenderSetLogicalSize(native_renderer, width, height);

	sdlgraphics_bitfont = new DisplayBitFont( native_renderer );

	SDL_SetWindowTitle( native_window, native_window_title.c_str() );
	Lux_SDL2_SetWindowIcon( native_window );

	SDL_DisableScreenSaver();

	return true;
}

/* Lux_NATIVE_Destory
 * Closes down the video mode.
 */
LUX_DISPLAY_FUNCTION void Lux_NATIVE_Destory()
{
	delete sdlgraphics_bitfont;
	SDL_DestroyRenderer(native_renderer);
	SDL_EnableScreenSaver();

	Lux_SDL2_CloseMessageWindow(  );
}

/* Lux_NATIVE_UpdateRect
 * Adds an area of the screen that needs to be updated.
 @ rect: area to updates
 */
LUX_DISPLAY_FUNCTION void Lux_NATIVE_UpdateRect(uint8_t screen,LuxRect rect)
{

}

/* Lux_NATIVE_PreShow
 * Refreshs the display
 */
LUX_DISPLAY_FUNCTION void Lux_NATIVE_PreShow( uint8_t screen )
{

}

/* Lux_NATIVE_Show
 * Refreshs the display
 */
LUX_DISPLAY_FUNCTION void Lux_NATIVE_Show( uint8_t screen )
{
	if ( native_renderer && !screen )
	{
		SDL_RenderPresent(native_renderer);

		SDL_SetRenderDrawColor(native_renderer, sdlgraphics_colour.r, sdlgraphics_colour.g, sdlgraphics_colour.b, 255);


		#ifndef EMSCRIPTEN
		SDL_RenderClear(native_renderer);
		#endif


		/* Debug Messages */
		if ( lux::display && lux::display->show_debug )
			Lux_SDL2_OpenMessageWindow();
		Lux_SDL2_PresentMessageWindow();
	}
}


/*
 *
 */

/* Lux_NATIVE_Display2Screen
 * Converts display location to screen location
 @ x:
 @ y:
 */
LUX_DISPLAY_FUNCTION void Lux_NATIVE_Display2Screen( int32_t * x, int32_t * y)
{
	if ( sdlgraphics_scale )
	{
		*x = (int32_t)((float)*x * sdlgraphics_ratio[0]);
		*y = (int32_t)((float)*y * sdlgraphics_ratio[1]);
	}
}

/* Lux_NATIVE_BackgroundObject
 * Set the background objects
 @ background:
 */
LUX_DISPLAY_FUNCTION void Lux_NATIVE_BackgroundObject( MapObject background )
{
	/* TODO
	- Draw background object
	*/
	sdlgraphics_colour = background.effects.primary_colour;

}

/* Lux_NATIVE_SetFullscreen
 * Set the background objects
 @ background:
 */
LUX_DISPLAY_FUNCTION bool Lux_NATIVE_SetFullscreen( bool able )
{
	lux::config->SetBoolean("display.fullscreen", able);
	if ( !SDL_SetWindowFullscreen(native_window, able ? SDL_TRUE : SDL_FALSE ) )
		return true;
	else
	{
		lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR) << "Can't not make the change the display size." << std::endl;
		return false;
	}
}

/* Lux_OGL_Resize
 *
 @ width:
 @ height:
 */
LUX_DISPLAY_FUNCTION bool Lux_NATIVE_Resize( uint16_t width, uint16_t height)
{
	SDL_RenderSetViewport(native_renderer, &native_graphics_dimension);
	SDL_RenderSetLogicalSize(native_renderer, width, height);

	if ( width > height )
	{
		SDL_SetHintWithPriority( SDL_HINT_ORIENTATIONS, "LandscapeLeft", SDL_HINT_OVERRIDE );
	}
	else
	{
		SDL_SetHintWithPriority( SDL_HINT_ORIENTATIONS, "Portrait", SDL_HINT_OVERRIDE );
	}

	return false;
}
/* Lux_NATIVE_DisplayPointer
 *
 @ able:
 */
LUX_DISPLAY_FUNCTION void Lux_NATIVE_DisplayPointer( uint8_t player, int16_t x, int16_t y, ObjectEffect effect )
{

	LuxRect position;

	effect.primary_colour = colour::black;

	position.x = x-3;
	position.y = y-3;
	position.w = 4;
	position.h = 5;
	Lux_NATIVE_DrawRect( position, effect );


	position.x = x-3;
	position.y = y-3;
	position.w = 5;
	position.h = 4;
	Lux_NATIVE_DrawRect( position, effect );

	effect.primary_colour = colour::yellow;

	position.x = x-2;
	position.y = y-2;
	position.w = 2;
	position.h = 3;
	Lux_NATIVE_DrawRect( position, effect );

	position.x = x-2;
	position.y = y-2;
	position.w = 3;
	position.h = 2;
	Lux_NATIVE_DrawRect( position, effect );


}



/* Lux_NATIVE_TextSprites
 * Allow the text system to use sprites instead of bitmap font
 @ able:
 */
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




/* Drawing Functions */


/* Lux_NATIVE_DrawSprite
 *
 @ sprite:
 @ dest_rect:
 @ effects:
 -
 */
LUX_DISPLAY_FUNCTION void Lux_NATIVE_DrawSprite( LuxSprite * sprite, LuxRect dest_rect, ObjectEffect effect )
{
	if ( sprite == NULL )
		return;

	if ( !native_renderer )
	{
		lux::core->SystemMessage(__FILE__ , __LINE__, SYSTEM_MESSAGE_ERROR) << " | Not a valid target surface." << std::endl;
		return;
	}

	NativeTexture * surface = (NativeTexture*) sprite->GetData(effect);

	double angle = (double)effect.rotation;
	SDL_RendererFlip flipmode = SDL_FLIP_NONE;

	SDL_Rect draw;
	SDL_Point point;
	SDL_Rect repeat;

	draw.x = dest_rect.x;
	draw.y = dest_rect.y;
	draw.w = surface->w;
	draw.h = surface->h;

//	point.x = draw.w/2;
//	point.y = draw.h/2;

	point.x = 0;
	point.y = 0;
	/* Flip image, rotates image either 90, 180, 270 and/or mirrors. */
	if ( effect.flip_image&16 ) // Mirror Sprite.
	{
		effect.flip_image -= 16;
		flipmode = SDL_FLIP_HORIZONTAL;
	}

	if ( effect.flip_image == 1 || effect.flip_image == 3 )
	{
		repeat.y = dest_rect.w / surface->w;
		repeat.x = dest_rect.h / surface->h;

		repeat.w = surface->h;
		repeat.h = surface->w;
	}
	else
	{
		repeat.x = dest_rect.w / surface->w;
		repeat.y = dest_rect.h / surface->h;

		repeat.w = surface->w;
		repeat.h = surface->h;
	}


	if ( effect.flip_image == 1 )// Switch Axis
	{
		angle += 90.0;

	}
	else if ( effect.flip_image == 2 )// Switch Axis
	{
		angle += 180.0;
	}
	else if ( effect.flip_image == 3 )// Switch Axis
	{
		angle += 270.0;
	}

	SDL_SetTextureColorMod(surface->texnum,effect.primary_colour.r,effect.primary_colour.g,effect.primary_colour.b);
	SDL_SetTextureAlphaMod(surface->texnum,effect.primary_colour.a);

	if ( repeat.x > 1 || repeat.y > 1 )
	{
		for( int32_t rx = 0; rx < repeat.x; rx++ )
		{
			for( int32_t ry = 0; ry < repeat.y; ry++ )
			{
				draw.x = dest_rect.x + (repeat.w*rx);
				draw.y = dest_rect.y + (repeat.h*ry);
				draw.w = repeat.w;
				draw.h = repeat.h;

				if ( effect.flip_image == 1 )// Switch Axis
				{
					draw.x += surface->h;
				}
				else if ( effect.flip_image == 2 )// Switch Axis
				{
					draw.y += surface->h;
					draw.x += surface->w;
				}
				else if ( effect.flip_image == 3 )// Switch Axis
				{
					draw.y += surface->w;
				}


				draw.w = surface->w;
				draw.h = surface->h;

				SDL_RenderCopyEx( native_renderer, surface->texnum, NULL, &draw, angle, &point, flipmode );
			}
		}
	}
	else
	{
		if ( effect.flip_image == 1 )// Switch Axis
		{
			draw.x += surface->h;
		}
		else if ( effect.flip_image == 2 )// Switch Axis
		{
			draw.y += surface->h;
			draw.x += surface->w;
		}
		else if ( effect.flip_image == 3 )// Switch Axis
		{
			draw.y += surface->w;
		}
		SDL_RenderCopyEx( native_renderer, surface->texnum, NULL, &draw, angle, &point, flipmode );
	}

}

LUX_DISPLAY_FUNCTION void Lux_NATIVE_DrawRect( LuxRect dest_rect, ObjectEffect effects)
{
	if ( !native_renderer )
	{
		lux::core->SystemMessage(__FILE__, __LINE__, SYSTEM_MESSAGE_LOG) << " Not a valid surface." << std::endl;
		return;
	}


	SDL_Rect tmp_rect;
	tmp_rect.x = dest_rect.x;
	tmp_rect.y = dest_rect.y;
	tmp_rect.w = dest_rect.w;
	tmp_rect.h = dest_rect.h;

	SDL_SetRenderDrawColor(native_renderer,effects.primary_colour.r,effects.primary_colour.g,effects.primary_colour.b, effects.primary_colour.a);
	SDL_SetRenderDrawBlendMode(native_renderer, SDL_BLENDMODE_BLEND);
	SDL_RenderFillRect(native_renderer, &tmp_rect);
	SDL_SetRenderDrawBlendMode(native_renderer, SDL_BLENDMODE_NONE);
}

/**
 * @brief Lux_NATIVE_DrawPolygon
 * @param x_point
 * @param y_point
 * @param point_count
 * @param position
 * @param effects
 * @param texture
 */
  LUX_DISPLAY_FUNCTION void Lux_NATIVE_DrawPolygon ( int16_t * x_point, int16_t *y_point, uint16_t point_count, LuxRect position, ObjectEffect effects, void * texture )
{
	if ( !native_renderer )
	{
		lux::core->SystemMessage(__FILE__, __LINE__, SYSTEM_MESSAGE_LOG) << " Not a valid surface." << std::endl;
		return;
	}

	SDL_Point * points = new SDL_Point[point_count];
	for(uint8_t i = 0; i < point_count; i++)
	{
		points[i].x = position.x + x_point[i];
		points[i].y = position.y + y_point[i];
	}


	SDL_SetRenderDrawColor(native_renderer,effects.primary_colour.r,effects.primary_colour.g,effects.primary_colour.b,effects.primary_colour.a);
	SDL_RenderDrawLines( native_renderer, points, point_count);

}

/**
 * @brief Lux_NATIVE_DrawCircle
 * @param dest_rect
 * @param effects
 */
LUX_DISPLAY_FUNCTION void Lux_NATIVE_DrawCircle( LuxRect dest_rect, ObjectEffect effects )
{
	if ( !native_renderer )
	{
		lux::core->SystemMessage(__FILE__, __LINE__, SYSTEM_MESSAGE_LOG) << " Not a valid surface." << std::endl;
		return;
	}


	SDL_SetRenderDrawColor(native_renderer,effects.primary_colour.r,effects.primary_colour.g,effects.primary_colour.b,effects.primary_colour.a);

	float cos;
	float angle;
	float xradius = (dest_rect.w / 2);
	float yradius = (dest_rect.h / 2);

	int cx = dest_rect.x + xradius;
	int cy = dest_rect.y + yradius;
	int x1,x2,y1,y2;
	for(uint8_t i = 0; i < yradius; i++)
	{
		angle = (i/yradius) * (M_PI/2);
		cos = SDL_cos(angle) * xradius;

		x1 = cx + cos;
		x2 = cx - cos;
		y1 = cy + i;
		y2 = cy - i;

		SDL_RenderDrawLine( native_renderer, x1, y1, x2, y1);
		SDL_RenderDrawLine( native_renderer, x1, y2, x2, y2);

	}


//	float angle;
//	float xradius = (dest_rect.w / 2);
//	float yradius = (dest_rect.h / 2);

//	int cx = dest_rect.x + xradius;
//	int cy = dest_rect.y + yradius;

//	int x1,x2,y1,y2;
//	for(float angle = 0; angle < 360; angle += 0.2)
//	{
//		float cos = SDL_cos(angle) * xradius;
//		float sin = SDL_sin(angle) * yradius;
//		x1 = cx + cos;
//		x2 = cx - cos;
//		y1 = cy + sin;
//		y2 = cy - sin;


//		SDL_SetRenderDrawColor(native_renderer,effects.primary_colour.r,effects.primary_colour.g,effects.primary_colour.b,effects.primary_colour.a);
//		SDL_RenderDrawLine( native_renderer, x1,y1, x2,y2);
//	}
}



LUX_DISPLAY_FUNCTION void Lux_NATIVE_DrawLine( LuxRect points, ObjectEffect effects)
{
	SDL_SetRenderDrawColor(native_renderer,effects.primary_colour.r,effects.primary_colour.g,effects.primary_colour.b,effects.primary_colour.a);
	SDL_RenderDrawLine(native_renderer,points.x, points.y, points.w, points.h);
}

LUX_DISPLAY_FUNCTION int32_t Lux_NATIVE_DrawChar( int32_t cchar, int32_t x, int32_t y, ObjectEffect effects, bool allow_custom )
{
	LuxColour c = effects.primary_colour;
	SDL_Rect area;
	SDL_Texture * texture = NULL;
	NativeTexture * ntexture = NULL;
	LuxSprite * sprite_data = NULL;
	int32_t offset = 7;

	int8_t axis;
	int8_t button;
	int8_t pointer;

	area.x = x;
	area.y = y;

	UnicodeToInput( cchar, &axis, &button, &pointer );

	if ( axis >= 0 || button >= 0 || pointer >= 0 )
	{
		c.r = c.g = c.b = 255;
		sprite_data = lux::display->GetInputSprite( 0, axis, button, pointer );
	}
	else if ( sdlgraphics_customtext && allow_custom )
	{
		sprite_data = lux::display->GetSprite( lux::display->sprite_font, elix::string::FromInt(cchar) );
	}



	if ( sprite_data )
	{
		ntexture = (NativeTexture*)sprite_data->GetData();

		texture = ntexture->texnum;

		area.w = ntexture->tw;
		area.h = ntexture->th;

		offset = ntexture->w;

	}
	else
	{
		texture = sdlgraphics_bitfont->GetTexture(cchar);

		offset = 8;
		area.w = 8;
		area.h = 8;
	}


	if ( texture )
	{
		SDL_SetTextureColorMod(texture, c.r, c.g, c.b);
		//SDL_SetTextureAlphaMod(texture, c.a);
		SDL_SetTextureBlendMode( texture, sdlgraphics_blend_mode);// sdlgraphics_blend_mode
		SDL_RenderCopy(native_renderer, texture, NULL, &area);
	}
	return offset;
}

LUX_DISPLAY_FUNCTION void Lux_NATIVE_DrawText( std::string text, LuxRect dest_rect, ObjectEffect effects, bool allow_custom)
{
	int32_t x, y;
	std::string::iterator object;
	bool watch_for_color = false;
	ObjectEffect current_effects = effects;

	x = dest_rect.x;
	y = dest_rect.y;

	for ( object = text.begin(); object != text.end(); object++ )
	{
		uint8_t utfchar = *object;
		uint32_t cchar = utfchar;


		if (cchar == '\n' || cchar == '\r')
		{
			y += ( sdlgraphics_customtext && allow_custom ? sdlgraphics_customtext_height + 2 : 10);
			x = dest_rect.x;
			cchar = 0;
			current_effects.primary_colour = effects.primary_colour; // reset effects
		}
		else if ( cchar <= 32 )
		{
			x += 7;
			cchar = 0;
			current_effects.primary_colour = effects.primary_colour; // reset effects
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
			if ( !Lux_Util_CheckTextColour( cchar, current_effects.primary_colour, watch_for_color ) )
			{
				x += Lux_NATIVE_DrawChar( cchar, x, y, current_effects, allow_custom );
			}
		}

	}
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
	if ( sprite->data )
	{
		NativeTexture * texture = (NativeTexture*)sprite->data;
		SDL_DestroyTexture(texture->texnum);
		delete texture;
	}
	sprite->data = NULL;
	return true;
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
		return false;
	}

	NativeTexture * texture = new NativeTexture;

	texture->texnum = SDL_CreateTexture( native_renderer, sdlgraphics_texture_format, SDL_TEXTUREACCESS_STATIC, rect.w, rect.h );
	texture->w = texture->tw = rect.w;
	texture->h = texture->th = rect.h;
	sprite->data = texture;
	/* PNG image */
	uint32_t * pixels = new uint32_t[texture->tw*texture->th]();
	if ( pixels )
	{
		for( uint16_t y = 0; y < texture->h; y++ )
		{
			for( uint16_t x = 0; x < texture->w; x++ )
			{
				uint32_t q = (texture->tw * y) + x;
				pixels[q] = png->GetPixel(rect.x + x, rect.y + y);
			}
		}

		SDL_SetTextureBlendMode(texture->texnum, sdlgraphics_blend_mode);
		SDL_UpdateTexture(texture->texnum, NULL, pixels, texture->tw * 4);
	}
	delete pixels;
	/*  ^ PNG Image */

	return true;
}


#include "display/reusable_graphics_system.cpp"
