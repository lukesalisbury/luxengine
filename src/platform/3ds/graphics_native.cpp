/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

Some Code taken from Easy GL2D http://rel.betterwebber.com
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

#include <3ds.h>

/* Global Variables */
struct NativeTexture {
	sf2d_texture * texture;
	int32_t address;
	uint32_t w, h, tw, th;
	bool pot, loaded;
	std::string file;
};

gfxScreen_t current_screen;

uint32_t native_screen_assignment = 0xC0000040; // 1100 0000 0000 0000 0000 0000 0100 0000

GraphicSystem GraphicsNative = {
	&Lux_NATIVE_Init,
	&Lux_NATIVE_Destory,
	&Lux_GRAPHICS_Display2Screen,
	&Lux_GRAPHICS_TextSprites,
	&Lux_NATIVE_PreShow,
	&Lux_GRAPHICS_UpdateRect,
	&Lux_NATIVE_Show,

	&Lux_GRAPHICS_SetRotation,
	&Lux_NATIVE_BackgroundObject,
	&Lux_GRAPHICS_SetFullscreen,
	&Lux_GRAPHICS_Resize,
	&Lux_GRAPHICS_DisplayPointer,

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
	&Lux_GRAPHICS_DrawPolygon,
	&Lux_NATIVE_DrawLine,
	&Lux_NATIVE_DrawText,
	&Lux_NATIVE_DrawMessage,

	NULL,
	NULL

};

DisplayBitFont * dsgraphics_bitfont = NULL;

/* Local Functions */
void Lux_NATIVE_SetFrame(LuxRect & rect, bool line)
{
	return;
	if ( current_screen == GFX_TOP )
	{
		if ( rect.y >= 240 )
		{
			//sf2d_end_frame();
			//current_screen = GFX_BOTTOM;
			//sf2d_start_frame( current_screen, GFX_LEFT);
			rect.y -= 240;
			rect.x -= 40;
			if (line)
			{
				rect.h -= 240;
				rect.w -= 40;
			}
		}

	}
	else
	{
		if ( rect.y < 240 )
		{
			//sf2d_end_frame();
			//current_screen = GFX_TOP;
			//sf2d_start_frame( current_screen, GFX_LEFT);
		}
	}


}

/* Public Function */

/* Creation,destruction and loop Functions */
/**
 * @brief Lux_NATIVE_Init
 * @param width
 * @param height
 * @param bpp
 * @param actual_width
 * @param actual_height
 * @return
 */
bool Lux_NATIVE_Init(LuxRect screen_dimension, LuxRect *display_dimension )
{
	if ( actual_width )
		*actual_width = 400;

	if ( actual_height )
		*actual_height = 240;

	dsgraphics_bitfont = new DisplayBitFont( );


	current_screen = GFX_TOP;
	return true;
}

/**
 * @brief Lux_NATIVE_Destory
 */
void Lux_NATIVE_Destory()
{
	delete dsgraphics_bitfont;
}

/**
 * @brief Lux_NATIVE_Display2Screen
 * @param x
 * @param y
 */
void Lux_NATIVE_Display2Screen( int32_t * x, int32_t * y )
{
	*x = (int32_t)((float)*x * 1.0);
	*y = (int32_t)((float)*y * 1.0);
}

/**
 * @brief Lux_NATIVE_Background
 * @param fillcolor
 */
void Lux_NATIVE_Background(LuxColour fillcolor)
{
	sf2d_set_clear_color( RGBA8(fillcolor.r, fillcolor.g, fillcolor.b, fillcolor.a) );
}

/**
 * @brief Lux_NATIVE_BackgroundObject
 * @param background
 */
void Lux_NATIVE_BackgroundObject( MapObject background )
{
	sf2d_set_clear_color( RGBA8(background.effects.primary_colour.r, background.effects.primary_colour.g, background.effects.primary_colour.b, background.effects.primary_colour.a) );
}
/**
* @brief Lux_NATIVE_PreShow
*/
void Lux_NATIVE_PreShow( uint8_t screen )
{
	if ( screen < 32 )
	{
		gfxScreen_t next_screen = IS_BIT_SET(native_screen_assignment, screen) ?  GFX_BOTTOM : GFX_TOP;
		if ( current_screen != next_screen )
		{
			sf2d_start_frame( next_screen, GFX_LEFT);
		}
		current_screen = next_screen;
	}

}

/**
 * @brief Lux_NATIVE_Update
 * @param rect
 */
void Lux_NATIVE_Update( uint8_t screen, LuxRect rect)
{

}

/**
 * @brief Lux_NATIVE_Show
 */
void Lux_NATIVE_Show( uint8_t screen )
{
	if ( screen > 30 )
	{
		sf2d_swapbuffers();
	}
	else
	{

		gfxScreen_t next_screen = IS_BIT_SET(native_screen_assignment, screen+1) ?  GFX_BOTTOM : GFX_TOP;
		if ( current_screen != next_screen )
		{
			sf2d_end_frame();
		}
	}


}


/* Drawing Functions */
/**
 * @brief Lux_NATIVE_DrawSprite
 * @param sprite
 * @param dest_rect
 * @param effects
 */
void Lux_NATIVE_DrawSprite( LuxSprite * sprite, LuxRect dest_rect, ObjectEffect effect )
{
	if ( !sprite->data )
		return;

	Lux_NATIVE_SetFrame(dest_rect, false);

	NativeTexture * surface = (NativeTexture*) sprite->GetData(effect);

	double angle = (double)effect.rotation;
	uint8_t flipmode = 0;

	LuxRect draw;
	LuxPath point;
	LuxRect repeat;

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
		flipmode = 1;
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


				sf2d_draw_texture_blend( surface->texture,draw.x, draw.y, RGBA8(effect.primary_colour.r, effect.primary_colour.g, effect.primary_colour.b, effect.primary_colour.a));

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
		sf2d_draw_texture_blend( surface->texture,draw.x, draw.y, RGBA8(effect.primary_colour.r, effect.primary_colour.g, effect.primary_colour.b, effect.primary_colour.a));
	}

}

void Lux_NATIVE_DrawRect( LuxRect dest_rect, ObjectEffect effects)
{
	Lux_NATIVE_SetFrame(dest_rect, false);
	sf2d_draw_rectangle(dest_rect.x, dest_rect.y, dest_rect.w, dest_rect.h, RGBA8(effects.primary_colour.r, effects.primary_colour.g, effects.primary_colour.b, effects.primary_colour.a)  );

}

void Lux_NATIVE_DrawLine( LuxRect points, ObjectEffect effects )
{
	Lux_NATIVE_SetFrame(points, true);
	sf2d_draw_line(points.x, points.y, points.w, points.h, RGBA8(effects.primary_colour.r, effects.primary_colour.g, effects.primary_colour.b, effects.primary_colour.a) );
}

int32_t Lux_NATIVE_DrawChar( int32_t cchar, int32_t x, int32_t y, ObjectEffect effects, bool allow_custom )
{
	sf2d_texture * 	texture = NULL;
	LuxColour c = effects.primary_colour;
	LuxRect area;
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


	if ( sprite_data )
	{
		ntexture = (NativeTexture*)sprite_data->GetData();

		texture = ntexture->texture;

		area.w = ntexture->tw;
		area.h = ntexture->th;

		offset = ntexture->w;

	}
	else
	{
		texture = dsgraphics_bitfont->GetTexture(cchar);

		offset = 8;
		area.w = 8;
		area.h = 8;
	}


	if ( texture )
	{
		sf2d_draw_texture_blend(texture, area.x, area.y, RGBA8(effects.primary_colour.r, effects.primary_colour.g, effects.primary_colour.b, effects.primary_colour.a) 	);
	}
	return offset;
}

void Lux_NATIVE_DrawText( std::string text, LuxRect dest_rect, ObjectEffect effects, bool allow_custom )
{
	Lux_NATIVE_SetFrame(dest_rect, false);
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
			y += 10;
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

void Lux_NATIVE_TextSprites( bool able )
{

}

void Lux_NATIVE_DrawPolygon( int16_t * x_point, int16_t *y_point, uint16_t point_count, ObjectEffect effects, void * texture )
{

}

void Lux_NATIVE_DrawCircle( LuxRect dest_rect, ObjectEffect effects )
{

}

void Lux_NATIVE_DrawMessage( std::string message, uint8_t alignment )
{
	std::string::iterator object;
	LuxRect rect, area, draw;
	int16_t w = 400, h = 240;

	bool watch_for_color = false;
	bool is_whitspace = false;
	LuxColour font_color = { 255, 255,255, 255 };

	rect.x = rect.y = 0;

	Lux_Util_SetRectFromText( rect, message, 7, 10, 240 );

	area = rect;

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
			if ( !Lux_Util_CheckTextColour( cchar, font_color, watch_for_color ) )
			{
				sf2d_texture * texture = dsgraphics_bitfont->GetTexture(cchar);

				if ( texture )
				{
					sf2d_draw_texture_blend(texture, draw.x, draw.y, RGBA8(font_color.r, font_color.g, font_color.b, font_color.a) 	);
				}
				else
				{
					sf2d_draw_rectangle(draw.x, draw.y, draw.w, draw.h, RGBA8(font_color.r, font_color.g, font_color.b, font_color.a)  );
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

/* Resource Functions */

/**
 * @brief Lux_NATIVE_FreeSprite
 * @param sprite
 * @return
 */
bool Lux_NATIVE_FreeSprite ( LuxSprite * sprite )
{
	if ( sprite == NULL )
		return false;
	if ( sprite->data )
	{
		NativeTexture * texture = (NativeTexture*)sprite->data;
		sf2d_free_texture(texture->texture);
		delete texture;
	}
	sprite->data = NULL;
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

	texture->texture = sf2d_create_texture( rect.w, rect.h, TEXFMT_RGBA8, SF2D_PLACE_RAM );
	texture->w = texture->tw = rect.w;
	texture->h = texture->th = rect.h;
	sprite->data = texture;

	texture->tw = texture->texture->pow2_w;
	texture->th = texture->texture->pow2_h;

	uint32_t size = texture->tw*texture->th;
	/* PNG image */
	uint32_t * pixels = new uint32_t[size]();
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
		//sf2d_fill_texture_from_RGBA8(texture->texture, pixels, texture->tw, texture->th);
		memcpy(texture->texture->data, pixels, size*4);
		sf2d_texture_tile32(texture->texture);
	}
	delete pixels;
	/*  ^ PNG Image */

	return true;
}

#include "display/reusable_graphics_system.cpp"
