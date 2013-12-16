/****************************
Copyright © 2012 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include "gles.hpp"
#include "bitfont.h"
#include "display.h"
#include "misc_functions.h"
#include "elix_string.hpp"
#include <math.h>
#ifndef LUX_DISPLAY_FUNCTION
#define LUX_DISPLAY_FUNCTION
#endif


#if USING_GLES == 1 || USING_GLDESKTOP != 4
#include "draw_vertex1.inc.cpp"
#else
#include "draw_vertex2.inc.cpp"
#endif

namespace gles {
	Texture font[95];
	bool customtext = false;
	uint32_t customtext_height = 10;
	LuxVertex scale = { 1, 1, 1 };
	LuxVertex rotation = { 0,0, 0 };
}

/* Helper Functions */
void Lux_GLES_UpdateColor( LuxColour * colors, ObjectEffect effect )
{
	if ( effect.style == STYLE_VGRADIENT )
	{
		colors[0] = effect.primary_colour;
		colors[1] = effect.primary_colour;
		colors[2] = effect.secondary_colour;
		colors[3] = effect.secondary_colour;
	}
	else if ( effect.style == STYLE_HGRADIENT )
	{
		colors[0] = effect.primary_colour;
		colors[1] = effect.secondary_colour;
		colors[2] = effect.primary_colour;
		colors[3] = effect.secondary_colour;
	}
	else if ( effect.style == STYLE_REPCOLOUR )
	{
		colors[0] = (LuxColour){255,255,255,255};
		colors[1] = (LuxColour){255,255,255,255};
		colors[2] = (LuxColour){255,255,255,255};
		colors[3] = (LuxColour){255,255,255,255};
	}
	else
	{
		colors[0] = effect.primary_colour;
		colors[1] = effect.primary_colour;
		colors[2] = effect.primary_colour;
		colors[3] = effect.primary_colour;
	}
}

uint8_t Lux_GLES_GetShader(  ObjectEffect effect )
{
	uint8_t s = NUM_SHADERS;
	if ( effect.style == STYLE_NEGATIVE )
	{
		s = SHADER_NEGATIVE;
	}
	else if ( effect.style == STYLE_REPCOLOUR )
	{
		s = SHADER_REPLACE;
	}
	else if ( effect.style == STYLE_GLOW || effect.style == STYLE_OUTLINE )
	{
		s = SHADER_GLOW;
	}
	return s;
}

/* Resource Functions */

/* Font Functions */
void Lux_GLES_UnloadFont()
{

	for ( uint8_t c = 0; c < 96; c++)
	{
		glDeleteTextures(1, &gles::font[c].pointer );
	}

}

void Lux_GLES_LoadFont()
{

	uint8_t * font_point = &gfxPrimitivesFontdata[0];
	font_point += (32*8);
	uint8_t i = 0, q;
	glEnable( GL_TEXTURE_2D );
	for ( uint8_t c = 0; c < 96; c++)
	{
		uint8_t * charflip = new uint8_t[64];
		for (i = 0; i < 8; i++)
		{
			for (q = 0; q < 8; q++)
			{
				charflip[(i*8) + q] =  (!!(font_point[i] & (1 << (8-q))) ? 0xFF : 0x00) ;
			}
		}
		glGenTextures(1, &gles::font[c].pointer );
		glBindTexture(GL_TEXTURE_2D, gles::font[c].pointer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 8, 8, 0, GL_ALPHA, GL_UNSIGNED_BYTE, charflip);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


		gles::font[c].w = gles::font[c].h = gles::font[c].tw = gles::font[c].th = 8;
		gles::font[c].pot = true;
		gles::font[c].loaded = true;

		delete charflip;
		font_point += 8;
	}
	glDisable( GL_TEXTURE_2D );
}

/* Disply Functions */
LUX_DISPLAY_FUNCTION void Lux_GLES_DrawCircle( LuxRect dest_rect, ObjectEffect effects )
{

	LuxVertex dest;
	LuxVertex coords[64];
	LuxColour colors[64];
	uint8_t using_shader = NUM_SHADERS;


	float angle;
	float xradius = (dest_rect.w / 2);
	float yradius = (dest_rect.h / 2);

	dest.set( dest_rect.x + xradius, dest_rect.y  + yradius, dest_rect.z/1000 );

	for(uint8_t i = 0; i < 64; i++)
	{
		angle = i*2*M_PI/64;
		colors[i] = effects.primary_colour;
		coords[i].set2( (cos(angle) * xradius),  (sin(angle) * yradius), 0.0, 0.0, 0.0 );
	}

	gles::render( GL_TRIANGLE_FAN, coords, 64, NULL, dest, colors, 0, gles::scale, gles::rotation, using_shader );
}

LUX_DISPLAY_FUNCTION void Lux_GLES_DrawLine( LuxRect points, ObjectEffect effects )
{
	LuxVertex dest;
	LuxVertex coords[4];
	LuxColour colors[4];
	uint8_t using_shader = NUM_SHADERS;

	colors[0] = effects.primary_colour;
	colors[1] = effects.primary_colour;

	dest.set( 0, 0, points.z/1000 );

	coords[0].set2( points.x, points.y, 0.0, 0.0, 0.0 );
	coords[1].set2( points.w, points.h, 0.0, 0.0, 1.0 );

	gles::render( GL_LINES, coords, 2, NULL, dest, colors, 0, gles::scale, gles::rotation, using_shader );
}

LUX_DISPLAY_FUNCTION void Lux_GLES_DrawPolygon( int16_t * x_point, int16_t *y_point, uint16_t point_count, LuxRect position, ObjectEffect effects, void * texture )
{
	// Make sure the point count isn't to high
	if ( point_count > 32 || point_count == 0 )
		return;

	LuxVertex dest;
	LuxVertex * coords = new LuxVertex[point_count];
	LuxColour * colors = new LuxColour[point_count];
	uint8_t using_shader = NUM_SHADERS;


	dest.set( position.x, position.y, position.z/1000 );

	for ( uint8_t i = 0; i < point_count; i++ )
	{
		colors[i] = effects.primary_colour;
		coords[i].set2( x_point[i],  y_point[i], 0.0, 0.0, 0.0 );
	}

	gles::render( GL_TRIANGLE_FAN, coords, point_count, NULL, dest, colors, 0, gles::scale, gles::rotation, using_shader );

}

LUX_DISPLAY_FUNCTION void Lux_GLES_DrawRect( LuxRect dest_rect, ObjectEffect effect)
{

	LuxVertex dest;
	LuxVertex scale = { 1, 1, 1 };
	LuxVertex rotation = { 0,0, 0 };
	LuxVertex coords[4];
	LuxColour colors[4];
	uint8_t using_shader = Lux_GLES_GetShader(effect);


	NATIVEFLOATTYPE half_wit = (NATIVEFLOATTYPE)(dest_rect.w/2);
	NATIVEFLOATTYPE half_hei = (NATIVEFLOATTYPE)(dest_rect.h/2);

	Lux_GLES_UpdateColor( colors, effect );
	OpenGLShader::SetPrimaryColor( (float)effect.primary_colour.r/255.0, (float)effect.primary_colour.g/255.0, (float)effect.primary_colour.b/255.0, (float)effect.primary_colour.a/255.0 );
	OpenGLShader::SetSecondaryColor( (float)effect.secondary_colour.r/255.0, (float)effect.secondary_colour.g/255.0, (float)effect.secondary_colour.b/255.0, (float)effect.secondary_colour.a/255.0 );

	/* Rotation */
	if ( effect.rotation )
	{
		rotation.z = effect.rotation;
	}

	dest.set( dest_rect.x + half_wit, dest_rect.y  + half_hei, dest_rect.z/1000 );

	coords[0].set2( -half_wit, -half_hei, 0.0, 0.0, 0.0);
	coords[1].set2( half_wit, -half_hei, 0.0, 1.0, 0.0 );
	coords[2].set2( -half_wit, half_hei, 0.0, 0.0, 1.0 );
	coords[3].set2( half_wit, half_hei, 0.0, 1.0, 1.0 );

	gles::render( GLES_DEFAULT_PRIMITIVE, coords, 4, NULL, dest, colors, 0, gles::scale, rotation, using_shader );
}


LUX_DISPLAY_FUNCTION void Lux_GLES_DrawSprite(LuxSprite * sprite, LuxRect dest_rect, ObjectEffect effect )
{
	if ( sprite == NULL )
		return;

	Texture * texture = (Texture*) sprite->GetData(effect);

	if ( texture == NULL )
		return;

	LuxRect area = dest_rect;
	LuxVertex dest;
	LuxVertex coords[4];
	LuxVertex scale = { 1, 1, 1 };
	LuxVertex rotation = { 0,0, 0 };
	LuxColour colors[4];

	bool texture_mirror = false;
	bool tile_sprite = false;


	uint8_t using_shader = Lux_GLES_GetShader(effect);


	NATIVEFLOATTYPE texture_width, texture_height;
	NATIVEFLOATTYPE sprite_width, sprite_height;
	NATIVEFLOATTYPE object_width, object_height;
	NATIVEFLOATTYPE object_half_width, object_half_height;
	NATIVEFLOATTYPE texture_x_topleft = 0.0, texture_x_topright = 1.0;
	NATIVEFLOATTYPE texture_x_bottomleft = 0.0, texture_x_bottemright = 1.0;
	NATIVEFLOATTYPE texture_y_topleft = 0.0, texture_y_topright = 0.0;
	NATIVEFLOATTYPE texture_y_bottomleft = 1.0, texture_y_bottemright = 1.0;
	NATIVEFLOATTYPE texture_start_xaxis = 0.00, texture_end_xaxis = 1.00;
	NATIVEFLOATTYPE texture_start_yaxis = 0.00, texture_end_yaxis = 1.00;
	NATIVEFLOATTYPE z_axis = dest_rect.z;

	z_axis /= 1000.0;

	/* Scale */
	scale.x = (NATIVEFLOATTYPE)effect.scale_xaxis / 1000.0;
	scale.y = (NATIVEFLOATTYPE)effect.scale_yaxis / 1000.0;

	/* Flip image, rotates image either 90, 180, 270 and/or mirrors. */
	if ( effect.flip_image&16 ) // Mirror Sprite.
	{
		effect.flip_image -= 16;
		texture_mirror = true;
	}

	tile_sprite = !((!dest_rect.w || dest_rect.w == texture->w) && (!dest_rect.h || dest_rect.h == texture->h));

	if ( effect.flip_image == 1 || effect.flip_image == 3 )// Switch Axis
	{
		texture_width = texture->th;
		texture_height = texture->tw;
		sprite_width = texture->h;
		sprite_height = texture->w;


		if ( dest_rect.w == 0 )
			area.w = texture->h;
		else
			area.w = (dest_rect.h / texture->h) * texture->h;
		object_width = (NATIVEFLOATTYPE)area.w;

		if ( dest_rect.h == 0 )
			area.h = texture->w;
		else
			area.h = (dest_rect.w / texture->w) * texture->w;
		object_height = (NATIVEFLOATTYPE)area.h;
	}
	else
	{
		texture_width = texture->tw;
		texture_height = texture->th;
		sprite_width = texture->w;
		sprite_height = texture->h;

		if ( dest_rect.w == 0 )
			dest_rect.w = texture->w;
		else
			dest_rect.w = (dest_rect.w / texture->w) * texture->w;
		object_width = (NATIVEFLOATTYPE)dest_rect.w;

		if ( dest_rect.h == 0 )
			dest_rect.h = texture->h;
		else
			dest_rect.h = (dest_rect.h / texture->h) * texture->h;
		object_height = (NATIVEFLOATTYPE)dest_rect.h;

	}



	if ( texture->pot || !tile_sprite )
	{
		/* If we using a texture with it's width matches the sprite width we can just use
		 * a single quad and repeat the texture.
		 */
		texture_end_xaxis = ( object_width / texture_width );
		texture_end_yaxis = ( object_height / texture_height );
	}

	/* Update Texture Position */
	switch ( effect.flip_image )
	{
		case 1:
		{
			texture_y_bottemright = texture_y_topright = ( texture_start_xaxis );
			texture_y_topleft = texture_y_bottomleft = ( texture_end_xaxis );
			texture_x_topleft = texture_x_topright = ( texture_mirror ? texture_end_yaxis : texture_start_yaxis );
			texture_x_bottemright = texture_x_bottomleft = ( texture_mirror ? texture_start_yaxis : texture_end_yaxis );
			break;
		}
		case 2:
		{
			texture_x_topleft = texture_x_bottomleft = ( texture_mirror ? texture_start_xaxis : texture_end_xaxis );
			texture_x_bottemright = texture_x_topright = ( texture_mirror ? texture_end_xaxis : texture_start_xaxis );
			texture_y_topleft = texture_y_topright = texture_end_yaxis;
			texture_y_bottomleft = texture_y_bottemright = texture_start_yaxis;
			break;
		}
		case 3:
		{
			texture_y_bottemright = texture_y_topright = ( texture_end_xaxis );
			texture_y_topleft = texture_y_bottomleft = ( texture_start_xaxis );
			texture_x_topleft = texture_x_topright = ( texture_mirror ? texture_start_yaxis : texture_end_yaxis );
			texture_x_bottemright = texture_x_bottomleft = ( texture_mirror ? texture_end_yaxis : texture_start_yaxis );
			break;
		}
		default:
		{
			texture_x_topleft = texture_x_bottomleft = ( texture_mirror ? texture_end_xaxis : texture_start_xaxis );
			texture_x_topright = texture_x_bottemright = ( texture_mirror ? texture_start_xaxis : texture_end_xaxis );
			texture_y_topleft = texture_y_topright = texture_start_yaxis;
			texture_y_bottomleft = texture_y_bottemright = texture_end_yaxis;
			break;
		}
	}

	/* Update Colour infomation */
	Lux_GLES_UpdateColor( colors, effect );
	OpenGLShader::SetPrimaryColor( (float)effect.primary_colour.r/255.0, (float)effect.primary_colour.g/255.0, (float)effect.primary_colour.b/255.0, (float)effect.primary_colour.a/255.0 );
	OpenGLShader::SetSecondaryColor( (float)effect.secondary_colour.r/255.0, (float)effect.secondary_colour.g/255.0, (float)effect.secondary_colour.b/255.0, (float)effect.secondary_colour.a/255.0 );

	/* Rotation */
	if ( effect.rotation )
	{
		rotation.z = effect.rotation;
	}

	if ( texture->pot )
	{
		/* Texture dimension are in a supported format */
		object_half_width = object_width / 2.0;
		object_half_height = object_height / 2.0;

		dest.set( area.x + object_half_width, area.y + object_half_height, z_axis );

		coords[0].set2( -object_half_width, -object_half_height, 0.0, texture_x_topleft, texture_y_topleft );
		coords[1].set2( +object_half_width, -object_half_height, 0.0, texture_x_topright, texture_y_topright );
		coords[2].set2( -object_half_width, +object_half_height, 0.0, texture_x_bottomleft, texture_y_bottomleft );
		coords[3].set2( +object_half_width, +object_half_height, 0.0, texture_x_bottemright, texture_y_bottemright );

		gles::render( GLES_DEFAULT_PRIMITIVE, coords, 4, texture, dest, colors, 0, scale, rotation, using_shader );
	}
	else
	{
		object_half_width = texture_width / 2.0;
		object_half_height = texture_height / 2.0;

		coords[0].set2( -object_half_width, -object_half_height, 0.0, texture_x_topleft, texture_y_topleft );
		coords[1].set2( +object_half_width, -object_half_height, 0.0, texture_x_topright, texture_y_topright );
		coords[2].set2( -object_half_width, +object_half_height, 0.0, texture_x_bottomleft, texture_y_bottomleft );
		coords[3].set2( +object_half_width, +object_half_height, 0.0, texture_x_bottemright, texture_y_bottemright );

		if ( tile_sprite )
		{
			// Sprite is tiled
			NATIVEFLOATTYPE yheight = 0.0;
			NATIVEFLOATTYPE xwidth = 0.0;
			for ( yheight = 0; yheight < object_height; yheight += sprite_height )
			{
				for ( xwidth = 0; xwidth < object_width; xwidth += sprite_width )
				{
					dest.set( area.x + xwidth + object_half_width, area.y + yheight + object_half_height, z_axis );
					gles::render( GLES_DEFAULT_PRIMITIVE, coords, 4, texture, dest, colors, 0, gles::scale, gles::rotation, using_shader );
				}
			}


		}
		else
		{
			dest.set( area.x + object_half_width, area.y + object_half_height, z_axis );
			gles::render( GLES_DEFAULT_PRIMITIVE, coords, 4, texture, dest, colors, 0, gles::scale, rotation, using_shader );
		}

	}


}


LUX_DISPLAY_FUNCTION int32_t Lux_GLES_DrawChar( int32_t cchar, int32_t x, int32_t y, int32_t z, ObjectEffect effects, bool allow_custom )
{
	LuxSprite * sprite_data = NULL;
	Texture * texture = NULL;

	/* Sprite Position */
	LuxVertex dest;
	LuxVertex coords[4];
	LuxColour colors[4];
	int32_t offset = 7;

	float width = 8.0;
	float height = 8.0;

	dest.set(x, y, z/1000);
	colors[0] = colors[1] = colors[2] = colors[3] = effects.primary_colour;

	int8_t axis;
	int8_t button;
	int8_t pointer;

	UnicodeToInput( cchar, &axis, &button, &pointer );


	if ( axis >= 0 || button >= 0 || pointer >= 0 )
	{
		colors[0] = colors[1] = colors[2] = colors[3] = colour::white;
		sprite_data = lux::display->GetInputSprite( 0, axis, button, pointer );
	}
	else if ( gles::customtext && allow_custom )
	{
		sprite_data = lux::display->GetSprite( lux::display->sprite_font, elix::string::FromInt(cchar) );
	}


	if ( sprite_data )
	{
		texture = (Texture*)sprite_data->GetData();
		if ( texture )
			offset = texture->w - 1;
	}
	else if ( cchar >= 32 && cchar <= 128 )
	{
		texture = &gles::font[cchar-32];
	}
	else
	{
		texture = &gles::font[31];
	}


	if ( texture )
	{
		width = texture->tw;
		height = texture->th;

		coords[0].set2( 0, 0, 0, 0.0, 0.0 );
		coords[1].set2( 0, height, 0, 0.0, 1.0 );
		coords[2].set2( width, 0, 0, 1.0, 0.0 );
		coords[3].set2( width, height, 0, 1.0, 1.0 );

		gles::render( GLES_DEFAULT_PRIMITIVE, coords, 4, texture, dest, colors, 0, gles::scale, gles::rotation, NUM_SHADERS );
	}
	else
	{

	}
	return offset;
}





LUX_DISPLAY_FUNCTION void Lux_GLES_DrawText( std::string text, LuxRect dest_rect, ObjectEffect effects, bool allow_custom)
{

	uint16_t x = dest_rect.x;
	uint16_t y = dest_rect.y;
	std::string::iterator object;

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
			y += ( gles::customtext && allow_custom ? gles::customtext_height + 2 : 10);
			x = dest_rect.x;
		}
		else if ( cchar < 32 )
		{

		}
		else if ( cchar <= 128 )
		{
			x += Lux_GLES_DrawChar(cchar, x, y, dest_rect.z/1000, effects, allow_custom);
		}
		else if ( cchar < 224 )
		{
			object++;
			uint32_t next = *object;

			cchar = ((cchar << 6) & 0x7ff) + (next & 0x3f);

			x += Lux_GLES_DrawChar(cchar, x, y, dest_rect.z/1000, effects, allow_custom);
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

			x += Lux_GLES_DrawChar(cchar, x, y, dest_rect.z/1000, effects, allow_custom);
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


			x += Lux_GLES_DrawChar(cchar, x, y, dest_rect.z/1000, effects, allow_custom);


		}
	}
}

LUX_DISPLAY_FUNCTION void Lux_GLES_TextSprites( bool able )
{
	if ( lux::display->sprite_font.length() )
	{
		gles::customtext = able;
		if ( gles::customtext )
		{
			LuxSprite * sdata = lux::display->GetSprite( lux::display->sprite_font, "48" );
			if ( sdata )
			{
				gles::customtext_height = sdata->sheet_area.h;
			}
		}
	}
	else
		gles::customtext = false;

}



