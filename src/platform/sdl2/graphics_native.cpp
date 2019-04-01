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

#ifndef M_PI_2
#define M_PI_2 1.5707963268
#endif


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
	&Lux_NATIVE_PreShow,
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
	nullptr,
	&Lux_NATIVE_FreeSprite,
	&Lux_NATIVE_PNGtoSprite,

	&Lux_NATIVE_DrawSprite,
	&Lux_NATIVE_DrawRect,
	&Lux_NATIVE_DrawCircle,
	&Lux_NATIVE_DrawPolygon,
	&Lux_NATIVE_DrawLine,
	&Lux_NATIVE_DrawText,
	&Lux_SDL2_DrawMessage,

	nullptr,
	nullptr

};

/* */
void SDL2_SystemInfo();
void SDL2_OuputRenderingInfo( SDL_RendererInfo * info );
int32_t SDL2_AcceratedRendering();

/* Shared */
Uint32 native_render_flags = 0;
Uint32 native_window_flags = 0;
SDL_Window * native_window = nullptr;
SDL_Renderer * native_renderer = nullptr;
std::string native_window_title;
SDL_Rect native_graphics_dimension = {0, 0, 320,240};
uint32_t native_screen_assignment = 0xC0000040; // 1100 0000 0000 0000 0000 0000 0100 0000
bool native_current_screen = true;


/* Global Variables */
DisplayBitFont * sdlgraphics_bitfont = nullptr;

LuxColour sdlgraphics_colour = {0,0,0,255};

SDL_Rect * sdlgraphics_dirty = 0;
float sdlgraphics_ratio[2] = {1.0, 1.0};
int32_t sdlgraphics_scale = 0;

bool sdlgraphics_customtext = false;
int32_t sdlgraphics_customtext_height = 32;

uint32_t sdlgraphics_texture_format = SDL_PIXELFORMAT_ABGR8888;
SDL_BlendMode sdlgraphics_blend_mode = SDL_BLENDMODE_BLEND;


/* Local Functions */

/**
 * @brief Init video mode
 * @param width
 * @param height
 * @param bpp
 * @return Returns true if successful
 */
bool Lux_NATIVE_Init(LuxRect * screen_dimension, LuxRect * display_dimension )
{
	int window_width, window_height, width, height;
	int32_t preferred_render = SDL2_AcceratedRendering();
	bool fullscreen = false;
	native_render_flags = SDL_RENDERER_ACCELERATED;
	native_window = lux::core->GetWindow();

	if ( screen_dimension == nullptr || screen_dimension->w == 0 || screen_dimension->h == 0 )
	{
		SDL_DisplayMode dm;
		if (SDL_GetDesktopDisplayMode(0, &dm) != 0) {
			return false;
		}
		fullscreen = lux::config->GetBoolean("display.fullscreen");
		if ( fullscreen )
		{
			window_width = width = dm.w;
			window_height = height = dm.h;

		}
		else
		{
			window_width = width = dm.w/2;
			window_height = height = dm.h/2;
		}

	}
	else
	{
		window_width = width = screen_dimension->w;
		window_height = height = screen_dimension->h;
	}

	if ( display_dimension != nullptr && screen_dimension->w != 0 && screen_dimension->h != 0 )
	{
		window_width = display_dimension->w;
		window_height = display_dimension->h;
	}
	else
	{
		window_width = width;
		window_height = height;
	}


	SDL_SetHintWithPriority( SDL_HINT_ORIENTATIONS, ( window_width > window_height  ? "LandscapeLeft" : "Portrait"), SDL_HINT_OVERRIDE );
	SDL_SetHintWithPriority( SDL_HINT_ORIENTATIONS, "Portrait", SDL_HINT_OVERRIDE );
	SDL_SetHintWithPriority( SDL_HINT_RENDER_VSYNC, "0", SDL_HINT_OVERRIDE );

	SDL_SetWindowSize(native_window, window_width, window_height );
	SDL_SetWindowPosition(native_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	SDL_GetWindowSize(native_window, &window_width, &window_height);
	SDL_SetWindowFullscreen(native_window, fullscreen ? SDL_TRUE : SDL_FALSE );
	SDL_ShowWindow(native_window);
	//Lux_NATIVE_SetFullscreen(true);

	native_renderer = SDL_CreateRenderer(native_window, preferred_render, native_render_flags);
	if ( !native_renderer )
	{
		lux::core->SystemMessage(__FILE__, __LINE__, SYSTEM_MESSAGE_LOG) << " Couldn't create Renderer. " << SDL_GetError() << std::endl;
		return false;
	}
	SDL_SetRenderDrawColor(native_renderer, 0, 0, 0, 255);

	SDL_RendererInfo info;
	SDL_GetRendererInfo(native_renderer, &info);
	SDL2_OuputRenderingInfo( &info );

	native_window_title = lux::config->GetString("window.title") + " [" + info.name + "]";
	SDL_SetWindowTitle( native_window, native_window_title.c_str() );
	Lux_SDL2_SetWindowIcon( native_window );


	native_graphics_dimension.x = 0;
	native_graphics_dimension.y = 0;
	native_graphics_dimension.w = width;
	native_graphics_dimension.h = height;

	SDL_RenderSetViewport(native_renderer, &native_graphics_dimension);
	SDL_RenderSetLogicalSize(native_renderer, width, height);

	sdlgraphics_bitfont = new DisplayBitFont( native_renderer );
	SDL_DisableScreenSaver();

	if ( display_dimension != nullptr )
	{
		display_dimension->w = window_width;
		display_dimension->h = window_height;
	}

	if ( screen_dimension != nullptr )
	{
		screen_dimension->w = width;
		screen_dimension->h = height;
	}

	std::cout << "Screen Dimension:" << width << "x" << height << std::endl;
	std::cout << "Display Dimension:" << window_width << "x" << window_height << std::endl;

	return true;
}


/**
  * @brief Closes down the video mode.
  */
void Lux_NATIVE_Destory()
{
	SDL_SetRenderDrawColor(native_renderer, 0, 0, 0, 255);
	SDL_RenderClear(native_renderer);

	delete sdlgraphics_bitfont;

	SDL_DestroyRenderer(native_renderer);
	SDL_EnableScreenSaver();

	Lux_SDL2_CloseMessageWindow( );
}

/**
  * @brief Adds an area of the screen that needs to be updated.
  * @param screen
  * @param rect area to updates
  */
void Lux_NATIVE_UpdateRect(uint8_t screen, LuxRect rect)
{

}

/**
 * @brief switches the display based on screen
 * @param screen
 */
void Lux_NATIVE_PreShow( uint8_t screen )
{
	if ( screen == GRAPHICS_SCREEN_MESSAGE )
	{
		if ( lux::display && lux::display->show_debug )
			Lux_SDL2_OpenMessageWindow();
	}
	else if ( screen <= GRAPHICS_SCREEN_GUI)
	{
		bool next_screen = IS_BIT_SET(native_screen_assignment, screen);
		if ( native_current_screen != next_screen )
		{

		}
	}
	else if ( screen == GRAPHICS_SCREEN_FRAME )
	{
		SDL_SetRenderDrawColor(native_renderer, sdlgraphics_colour.r, sdlgraphics_colour.g, sdlgraphics_colour.b, 255);
		SDL_RenderClear(native_renderer);

	}
}

/**
  * @brief Refreshs the display
  * @param screen
  */
void Lux_NATIVE_Show( uint8_t screen )
{
	if ( screen == GRAPHICS_SCREEN_MESSAGE )
	{
		Lux_SDL2_PresentMessageWindow();
	}
	else if ( screen <= GRAPHICS_SCREEN_GUI)
	{
		bool next_screen = IS_BIT_SET(native_screen_assignment, screen+1);
		if ( native_current_screen != next_screen )
		{

		}
	}
	else if ( screen == GRAPHICS_SCREEN_FRAME )
	{
		SDL_RenderPresent(native_renderer);
	}
}

/**
  * @brief Converts display location to screen location
  * @param x
  * @param y
  */
void Lux_NATIVE_Display2Screen( int32_t * x, int32_t * y)
{
	if ( sdlgraphics_scale )
	{
		*x = (int32_t)((float)*x * sdlgraphics_ratio[0]);
		*y = (int32_t)((float)*y * sdlgraphics_ratio[1]);
	}
}

/**
 * @brief Set the background objects
 * @param background
 */
void Lux_NATIVE_BackgroundObject( MapObject background )
{
	/* TODO
	- Draw background object
	*/
	sdlgraphics_colour = background.effects.primary_colour;
	SDL_SetRenderDrawColor(native_renderer, sdlgraphics_colour.r, sdlgraphics_colour.g, sdlgraphics_colour.b, 255);
}

/**
 * @brief Lux_NATIVE_SetFullscreen
 * @param able
 * @return
 */
bool Lux_NATIVE_SetFullscreen( bool able )
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

/**
 * @brief Lux_NATIVE_Resize
 * @param width
 * @param height
 * @return
 */
bool Lux_NATIVE_Resize( uint16_t width, uint16_t height)
{
	SDL_RenderSetViewport(native_renderer, &native_graphics_dimension);
	SDL_RenderSetLogicalSize(native_renderer, width, height);

	SDL_SetHintWithPriority( SDL_HINT_ORIENTATIONS, ( width > height ? "LandscapeLeft" : "Portrait"), SDL_HINT_OVERRIDE );

	return false;
}

/**
 * @brief Lux_NATIVE_DisplayPointer
 * @param player
 * @param x
 * @param y
 * @param effect
 */
void Lux_NATIVE_DisplayPointer( uint8_t player, int16_t x, int16_t y, ObjectEffect effect )
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

/**
 * @brief Lux_NATIVE_TextSprites
 * @param able
 */
void Lux_NATIVE_TextSprites( bool able )
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

/**
 * @brief Lux_NATIVE_DrawSprite
 * @param sprite
 * @param dest_rect
 * @param effect
 */
void Lux_NATIVE_DrawSprite( LuxSprite * sprite, LuxRect dest_rect, ObjectEffect effect )
{
	if ( sprite == nullptr )
		return;

	if ( !native_renderer )
	{
		lux::core->SystemMessage(__FILE__ , __LINE__, SYSTEM_MESSAGE_ERROR) << " | Not a valid target surface." << std::endl;
		return;
	}

	NativeTexture * surface = (NativeTexture*) sprite->GetData(effect);

	if ( surface == nullptr )
	{
		lux::core->SystemMessage(__FILE__ , __LINE__, SYSTEM_MESSAGE_ERROR) << " | Not a valid surface." << std::endl;
		return;
	}

	double angle = (double)effect.rotation;
	int flipmode = SDL_FLIP_NONE;

	SDL_Rect draw;
	SDL_Point point;
	SDL_Rect repeat;

	point.x = surface->w/2;
	point.y = surface->h/2;

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




	SDL_SetTextureColorMod(surface->texnum,effect.primary_colour.r,effect.primary_colour.g,effect.primary_colour.b);
	SDL_SetTextureAlphaMod(surface->texnum,effect.primary_colour.a);
	if ( effect.flip_image == 1 )// Switch Axis
	{
		//SDL_SetTextureColorMod(surface->texnum,255,0,0);
		dest_rect.x -= (point.x-point.y) + 1;
		dest_rect.y -= (point.y-point.x);
		angle += 90.0;
	}
	else if ( effect.flip_image == 2 )// Switch Axis
	{
		//SDL_SetTextureColorMod(surface->texnum,0,255,0);
		flipmode = flipmode ^ SDL_FLIP_VERTICAL;
		flipmode = flipmode ^ SDL_FLIP_HORIZONTAL;

	}
	else if ( effect.flip_image == 3 )// Switch Axis
	{
		//SDL_SetTextureColorMod(surface->texnum,0,0,255);
		angle -= 90.0;
		dest_rect.x -= (point.x-point.y);
		dest_rect.y -= (point.y-point.x) + 1;
	}

	draw.x = dest_rect.x;
	draw.y = dest_rect.y;
	draw.w = surface->w;
	draw.h = surface->h;

	if ( repeat.x > 1 || repeat.y > 1 )
	{
		for( int32_t rx = 0; rx < repeat.x; rx++ )
		{
			for( int32_t ry = 0; ry < repeat.y; ry++ )
			{
				draw.x = dest_rect.x + (repeat.w*rx);
				draw.y = dest_rect.y + (repeat.h*ry);
				draw.w = surface->w;
				draw.h = surface->h;
/*
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
*/



				SDL_RenderCopyEx( native_renderer, surface->texnum, nullptr, &draw, angle, &point, (SDL_RendererFlip)flipmode );
			}
		}
	}
	else
	{
		/*
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
		}*/
		SDL_RenderCopyEx( native_renderer, surface->texnum, nullptr, &draw, angle, &point, (SDL_RendererFlip)flipmode );
	}

}

/**
 * @brief Lux_NATIVE_DrawRect
 * @param dest_rect
 * @param effects
 */
void Lux_NATIVE_DrawRect( LuxRect dest_rect, ObjectEffect effects)
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
void Lux_NATIVE_DrawPolygon ( int16_t * x_point, int16_t *y_point, uint16_t point_count, LuxRect position, ObjectEffect effects, void * texture )
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
 void Lux_NATIVE_DrawCircle( LuxRect dest_rect, ObjectEffect effects )
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
	if ( yradius > 1 )
	{
		for(uint8_t i = 0; i < (yradius-1); i++)
		{
			angle = (M_PI_2) * i/yradius;
			cos = SDL_cos(angle) * (xradius);

			x1 = cx + cos;
			x2 = cx - cos;
			y1 = cy + i;
			y2 = cy - i;

			SDL_RenderDrawLine( native_renderer, x1, y1, x2, y1);
			SDL_RenderDrawLine( native_renderer, x1, y2, x2, y2);
		}
	}

}


/**
 * @brief Lux_NATIVE_DrawLine
 * @param points
 * @param effects
 */
void Lux_NATIVE_DrawLine( LuxRect points, ObjectEffect effects)
{
	SDL_SetRenderDrawColor(native_renderer,effects.primary_colour.r,effects.primary_colour.g,effects.primary_colour.b,effects.primary_colour.a);
	SDL_RenderDrawLine(native_renderer,points.x, points.y, points.w, points.h);
}

/**
 * @brief Lux_NATIVE_DrawChar
 * @param cchar
 * @param x
 * @param y
 * @param effects
 * @param allow_custom
 * @return
 */
int32_t Lux_NATIVE_DrawChar( int32_t cchar, int32_t x, int32_t y, ObjectEffect effects, bool allow_custom )
{
	LuxColour c = effects.primary_colour;
	SDL_Rect area;
	SDL_Texture * texture = nullptr;
	NativeTexture * ntexture = nullptr;
	LuxSprite * sprite_data = nullptr;
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
		SDL_SetTextureAlphaMod(texture, c.a);
		SDL_SetTextureBlendMode( texture, sdlgraphics_blend_mode);// sdlgraphics_blend_mode
		SDL_RenderCopy(native_renderer, texture, nullptr, &area);
	}
	return offset;
}

/**
 * @brief Lux_NATIVE_DrawText
 * @param text
 * @param dest_rect
 * @param effects
 * @param allow_custom
 */
void Lux_NATIVE_DrawText( std::string text, LuxRect dest_rect, ObjectEffect effects, bool allow_custom)
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


/**
 * @brief Lux_NATIVE_FreeSprite
 * @param sprite
 * @return
 */
bool Lux_NATIVE_FreeSprite( LuxSprite * sprite )
{
	if ( sprite == nullptr )
		return false;
	if ( sprite->data )
	{
		NativeTexture * texture = (NativeTexture*)sprite->data;
		SDL_DestroyTexture(texture->texnum);
		delete texture;
	}
	sprite->data = nullptr;
	return true;
}

/**
 * @brief Lux_NATIVE_CreateSprite
 * @param sprite
 * @param rect
 * @param png
 * @return
 */
bool Lux_NATIVE_CreateSprite( LuxSprite * sprite, LuxRect rect, elix::Image * png )
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
		SDL_UpdateTexture(texture->texnum, nullptr, pixels, texture->tw * 4);
	}
	delete pixels;
	/*  ^ PNG Image */

	return true;
}


#include "display/reusable_graphics_system.cpp"


