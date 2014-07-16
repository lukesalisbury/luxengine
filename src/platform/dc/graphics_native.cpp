/****************************
Copyright (c) 2006-2010 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

/* Required Headers */
#include "luxengine.h"
#include "map_object.h"
#include "mokoi_game.h"
#include "display.h"
#include "graphics_native.h"
#include "graphics_system.h"


#include "bitfont.h"
#include "elix_png.hpp"


#include <kos.h>
#include <math.h>
#include <plx/matrix.h>
#include <plx/prim.h>

/* Global Variables */

struct Texture {
	pvr_ptr_t pointer;
	uint32_t w, h, tw, th;
	bool pot, loaded;
	std::string file;
};

pvr_ptr_t font_texture[95];
float zcount = 0.0;
MapObject dreamcastbackground;
LuxRect dreamcastregion;
LuxPolygon * sdlgraphics_cursor = NULL;

GraphicSystem GraphicsNative = {
	&LuxGraphics_DC_Init,
	&LuxGraphics_DC_Destory,
	&Lux_GRAPHICS_Display2Screen,
	&Lux_GRAPHICS_TextSprites,
	&LuxGraphics_DC_UpdateRect,
	&LuxGraphics_DC_Show,

	&Lux_GRAPHICS_SetRotation,
	&LuxGraphics_DC_BackgroundObject,
	&Lux_GRAPHICS_SetFullscreen,
	&Lux_GRAPHICS_Resize,
	&LuxGraphics_DC_DisplayPointer,

	&LuxGraphics_DC_RefreshSpriteSheet,
	&LuxGraphics_DC_LoadSpriteSheet,
	&LuxGraphics_DC_LoadSpriteSheetImage,
	&LuxGraphics_DC_FreeSpriteSheet,
	NULL,
	&LuxGraphics_DC_FreeSprite,
	&LuxGraphics_DC_PNGtoSprite,

	&LuxGraphics_DC_DrawSprite,
	&LuxGraphics_DC_DrawRect,
	&LuxGraphics_DC_DrawCircle,
	&LuxGraphics_DC_DrawPolygon,
	&LuxGraphics_DC_DrawLine,
	&LuxGraphics_DC_DrawText

};


float nextzcount()
{
	zcount += 0.1;
	return zcount;
}

void dreamcastSetBits( uint32_t &v, uint8_t n, uint8_t p, uint8_t s )
{
	n = clamp( n, 0, (1<<s)-1);
	v |= (n << p);
}


int dreamcastPowerOfTwo( int value )
{
	int result = 1 ;
	while ( result < value )
		result *= 2 ;
	return result;
}

void dreamcastStartFrame()
{
	pvr_wait_ready();
	pvr_scene_begin();
	pvr_list_begin(PVR_LIST_TR_POLY);
	LuxGraphics_DC_DrawRect(dreamcastregion, dreamcastbackground.effects);
}

void dreamcastEndFrame()
{
	pvr_list_finish();
	pvr_scene_finish();
	zcount = 0.0;
}

bool dreamcastLoadTexture(Texture * texure)
{
	if ( !texure->pot )
		return false;
	if ( texure->loaded )
		return true;

	uint8_t * data = NULL;
	uint32_t size = 0;

	size = lux::game_data->GetFile(texure->file, &data, false);

	if ( size )
	{
		pvr_txr_load(data, texure->pointer, texure->tw* texure->th*2 );
		//pvr_txr_load_ex(data, texure->pointer, texure->tw, texure->th, PVR_TXRLOAD_16BPP);
		texure->loaded = true;
		delete data;
		return true;
	}
	return false;


}

void dreamcastCreateFont()
{
	uint8_t * font_point = &gfxPrimitivesFontdata[0];
	font_point += (32*8);
	uint8_t i = 0, q = 0;
	uint32_t * charflip = new uint32_t[8];

	pvr_set_pal_format(PVR_PAL_ARGB1555);
	pvr_set_pal_entry(0, 0xff0f0000);
	pvr_set_pal_entry(1, 0xFF00FFFF);
	pvr_set_pal_entry(2, 0xFFFF00ff);
	pvr_set_pal_entry(3, 0xDDDF0FFF);
	for ( uint8_t c = 0; c < 95; c++)
	{
		font_texture[c] = pvr_mem_malloc(32);
		for ( i = 0; i < 8; i++)
		{
			charflip[i] = 0;
			for (q = 0; q < 8; q++)
			{
				dreamcastSetBits( charflip[i], !!(font_point[i] & (1 << (8-q)) ), q*4, 4);
			}
		}
		pvr_txr_load_ex(charflip, font_texture[c], 8, 8, PVR_TXRLOAD_4BPP);
		font_point += 8;
	}
	delete[] charflip;
}

/* Public Function */
/* Creation,destruction and loop Functions */


LUX_DISPLAY_FUNCTION bool LuxGraphics_DC_Init(uint16_t width, uint16_t height, uint8_t bpp, bool fullscreen )
{
	pvr_set_bg_color(1.0f, 0.0f, 0.0f);
	dreamcastCreateFont();

	dreamcastregion.w = 640;
	dreamcastregion.h = 480;

	sdlgraphics_cursor = new LuxPolygon(3);

	sdlgraphics_cursor->SetPoint(0, 0, 0);
	sdlgraphics_cursor->SetPoint(1, 8, 8);
	sdlgraphics_cursor->SetPoint(2, 0, 12);



	dreamcastStartFrame();

	return true;
}

LUX_DISPLAY_FUNCTION void LuxGraphics_DC_Destory()
{
	dreamcastEndFrame();
}

LUX_DISPLAY_FUNCTION void LuxGraphics_DC_BackgroundObject( MapObject background  )
{
	dreamcastbackground = background;
	pvr_set_bg_color((float)background.effects.primary_colour.r / 255.0, (float)background.effects.primary_colour.g / 255.0, (float)background.effects.primary_colour.b / 255.0);
}


LUX_DISPLAY_FUNCTION void LuxGraphics_DC_UpdateRect(LuxRect rect)
{

}


LUX_DISPLAY_FUNCTION void LuxGraphics_DC_Show()
{
	dreamcastEndFrame();
	dreamcastStartFrame();
}

LUX_DISPLAY_FUNCTION void LuxGraphics_DC_DisplayPointer( uint8_t player, int16_t x, int16_t y, ObjectEffect effect )
{
	LuxRect position;

	position.x = x;
	position.y = y;

	LuxGraphics_DC_DrawPolygon(sdlgraphics_cursor->x, sdlgraphics_cursor->y, sdlgraphics_cursor->count, position, effect, NULL);

}


/* Resource Functions */

/* LuxGraphics_DC_FreeSprite
 *
 @ sprite:
 -
 */
LUX_DISPLAY_FUNCTION bool LuxGraphics_DC_FreeSprite( LuxSprite * sprite )
{
	if ( sprite == NULL )
		return false;
	if (sprite->data)
	{
		Texture * texture = (Texture*)sprite->data;
		pvr_mem_free(texture->pointer);
		delete texture;
	}
	sprite->data = NULL;
	return false;
}


/* LuxGraphics_DC_CreateSprite
 *
 @ sprite:
 @ rect:
 @ parent:
 -
 */
LUX_DISPLAY_FUNCTION bool LuxGraphics_DC_CreateSprite( LuxSprite * sprite, LuxRect rect, elix::Image * png )
{
	if ( !png->HasContent() )
	{
		lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR) << "PNG is empty" << std::endl;
		return false;
	}

	Texture * texture = new Texture;
	texture->w = sprite->sheet_area.w;
	texture->h = sprite->sheet_area.h;

	texture->tw = dreamcastPowerOfTwo(texture->w);
	texture->th = dreamcastPowerOfTwo(texture->h);

	texture->pot = ((texture->w == texture->tw) && (texture->h == texture->th));
	texture->loaded = false;

	/* PNG image */
	uint16_t * pixels = new uint16_t[texture->tw * texture->th];
	if ( pixels )
	{
		memset(pixels, 0x00, texture->tw * texture->th * 2 );
		for( uint16_t y = 0; y < texture->h; y++ )
		{
			for( uint16_t x = 0; x < texture->w; x++ )
			{
				uint32_t q = (texture->tw * y) + x;
				pixels[q] = png->GetPixel16(sprite->sheet_area.x + x, sprite->sheet_area.y + y);
			}
		}
		texture->pointer = pvr_mem_malloc(texture->tw * texture->th * 2);
		pvr_txr_load_ex(pixels, texture->pointer, texture->tw, texture->th, PVR_TXRLOAD_16BPP);
		texture->loaded = true;
		sprite->data = (void*)texture;
	}
	delete pixels;
	/* RAW Images */
	/*
	tex->loaded = false;
	tex->file = "./sprites/" + name + "-" + p->first;

	if ( dreamcastLoadTexture( tex ) )
	{
		p->second->data = (void*)tex;
		tex->loaded = true;
	}
	else
	{
		delete tex;
		p->second->data = NULL;
	}
	*/


	return true;
}


/* LuxGraphics_DC_LoadSpriteSheet
 *
 @ name:
 @ children:
 -
 */
LUX_DISPLAY_FUNCTION bool LuxGraphics_DC_LoadSpriteSheet(std::string name, std::map<uint32_t, LuxSprite *> * children)
{
	/* PNG Image */
	uint8_t * data = NULL;
	uint32_t size;
	elix::Image * png = new elix::Image;
	if ( lux::game_data )
	{
		size = lux::game_data->GetFile("./sprites/" + name, &data, false);
		if ( size )
		{
			png->LoadFile(data, size);
		}
	}
	delete data;
	/*  ^ PNG Image */

	if ( png->HasContent() )
	{
		std::map<uint32_t, LuxSprite *>::iterator p;
		for( p = children->begin(); p != children->end(); p++ )
		{
			if ( !p->second->animated )
			{
				LuxGraphics_DC_CreateSprite( p->second, p->second->sheet_area, png );
			}
		}
		delete png;
		return true;
	}
	delete png;
	return false;
}


/* LuxGraphics_DC_LoadSpriteSheetImage
 *
 @ image:
 @ children:
 -
 */
LUX_DISPLAY_FUNCTION bool LuxGraphics_DC_LoadSpriteSheetImage( elix::Image * image, std::map<uint32_t, LuxSprite *> * children)
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
				LuxGraphics_DC_CreateSprite( p->second, p->second->sheet_area, image );
			}
		}
		return true;
	}
	else
	{
		return false;
	}

}

/* LuxGraphics_DC_FreeSpriteSheet
 *
 @ children:
 -
 */
LUX_DISPLAY_FUNCTION bool LuxGraphics_DC_FreeSpriteSheet( std::map<uint32_t, LuxSprite *> * children)
{
	std::map<uint32_t, LuxSprite *>::iterator p;
	for( p = children->begin(); p != children->end(); p++ )
	{
		LuxGraphics_DC_FreeSprite( p->second );
	}
	return true;
}


/* LuxGraphics_DC_RefreshSpriteSheet
 *
 @ name:
 @ children:
 -
 */
LUX_DISPLAY_FUNCTION bool LuxGraphics_DC_RefreshSpriteSheet( std::string name, std::map<uint32_t, LuxSprite *> * children )
{
	if ( LuxGraphics_DC_FreeSpriteSheet( children ) )
		LuxGraphics_DC_LoadSpriteSheet( name, children );
	return true;
}

/* LuxGraphics_DC_PNGtoSprite
 *
 @ data:
 @ size:
 -
 */
LUX_DISPLAY_FUNCTION LuxSprite * LuxGraphics_DC_PNGtoSprite( uint8_t * data, uint32_t size )
{
	LuxSprite * sprite = NULL;
	elix::Image * src = new elix::Image(data, size);

	if ( src->HasContent() )
	{
		sprite = new LuxSprite( GraphicsNative );

		Texture * tex = new Texture;
		tex->pointer = NULL;
		tex->w = src->Width();
		tex->h = src->Height();
		tex->tw = dreamcastPowerOfTwo( tex->w );
		tex->th = dreamcastPowerOfTwo( tex->h );
		tex->pot = ((tex->w == tex->tw) && (tex->h == tex->th));
		tex->loaded = false;
		if ( !tex->pot )
			lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << tex->w << "(" << tex->tw << ")x" << tex->h << "(" << tex->th << ")"  << std::endl;

		uint16_t * pixels = new uint16_t[tex->tw*tex->th];
		if ( pixels )
		{
			memset(pixels, 0x00, tex->tw * tex->th * 2 );
			for( uint16_t y = 0; y < tex->h; y++ )
			{
				for( uint16_t x = 0; x < tex->w; x++ )
				{
					uint32_t q = (tex->tw * y) + x;
					pixels[q] = src->GetPixel16(x, y);
				}
			}
			tex->pointer = pvr_mem_malloc(tex->tw * tex->th * 2);
			pvr_txr_load_ex(pixels, tex->pointer, tex->tw, tex->th, PVR_TXRLOAD_16BPP);
			tex->loaded = true;
			sprite->data = tex;
		}
		else
		{
			delete sprite;
			sprite = NULL;
		}
		delete pixels;

	}
	delete src;
	return sprite;

}


/* Drawing Functions */

LUX_DISPLAY_FUNCTION void LuxGraphics_DC_DrawSprite(LuxSprite * sprite, LuxRect dest_rect, ObjectEffect effects)
{
	if ( sprite == NULL )
		return;

	Texture * tex = (Texture*) sprite->GetData(effects);
	if ( dest_rect.w == 0 )
		dest_rect.w = tex->tw;

	if ( dest_rect.w == tex->w )
		dest_rect.w = tex->tw;

	if ( dest_rect.h == 0 )
		dest_rect.h = tex->th;

	if ( dest_rect.h == tex->h )
		dest_rect.h = tex->th;

	uint32_t c1 = PVR_PACK_COLOR((float)effects.primary_colour.a / 255.0, (float)effects.primary_colour.r / 255.0, (float)effects.primary_colour.g / 255.0, (float)effects.primary_colour.b / 255.0);
	pvr_poly_hdr_t hdr;
	pvr_poly_cxt_t cxt;
	pvr_vertex_t vert;

	pvr_poly_cxt_txr(&cxt, PVR_LIST_TR_POLY, PVR_TXRFMT_ARGB1555, tex->tw, tex->th, tex->pointer, PVR_FILTER_NONE);
	pvr_poly_compile(&hdr, &cxt);

	pvr_prim(&hdr, sizeof(hdr));
	vert.flags = PVR_CMD_VERTEX;
	vert.x = (float)(dest_rect.x);
	vert.y = (float)(dest_rect.y + dest_rect.h);
	vert.z = nextzcount();
	vert.u = 0.0f;
	vert.v = 1.0f;

	vert.argb = c1;
	vert.oargb = 0;
	pvr_prim(&vert, sizeof(vert));

	vert.y = (float)dest_rect.y;
	vert.u = 0.0f;
	vert.v = 0.0f;
	pvr_prim(&vert, sizeof(vert));

	vert.x = (float)(dest_rect.x + dest_rect.w);
	vert.y = (float)(dest_rect.y + dest_rect.h);
	vert.u = 1.0f;
	vert.v = 1.0f;

	pvr_prim(&vert, sizeof(vert));

	vert.flags = PVR_CMD_VERTEX_EOL;
	vert.u = 1.0f;
	vert.v = 0.0f;
	vert.y = dest_rect.y;
	pvr_prim(&vert, sizeof(vert));
}

LUX_DISPLAY_FUNCTION void LuxGraphics_DC_DrawRect( LuxRect dest_rect, ObjectEffect effects)
{

	float z = (float)dest_rect.z/1000.0;
	pvr_poly_hdr_t hdr;
	pvr_poly_cxt_t cxt;
	pvr_vertex_t vert;

	pvr_poly_cxt_col(&cxt, PVR_LIST_TR_POLY);
	pvr_poly_compile(&hdr, &cxt);

	pvr_prim(&hdr, sizeof(hdr));


	vert.flags = PVR_CMD_VERTEX;
	vert.x = (float)dest_rect.x;
	vert.y = (float)(dest_rect.y + dest_rect.h);
	vert.z = nextzcount();
	vert.argb = PVR_PACK_COLOR((float)effects.primary_colour.a / 255.0, (float)effects.primary_colour.r / 255.0, (float)effects.primary_colour.g / 255.0, (float)effects.primary_colour.b / 255.0);
	vert.oargb = 0;
	pvr_prim(&vert, sizeof(vert));

	vert.y = (float)dest_rect.y;
	pvr_prim(&vert, sizeof(vert));

	vert.x = (float)(dest_rect.x + dest_rect.w);
	vert.y = (float)(dest_rect.y + dest_rect.h);
	pvr_prim(&vert, sizeof(vert));

	vert.y = (float)(dest_rect.y);
	vert.flags = PVR_CMD_VERTEX_EOL;

	pvr_prim(&vert, sizeof(vert));
}


LUX_DISPLAY_FUNCTION void LuxGraphics_DC_DrawLine( LuxRect points, ObjectEffect effects)
{
	float z = (float)points.z/1000.0;
	pvr_poly_hdr_t hdr;
	pvr_poly_cxt_t cxt;
	pvr_vertex_t vert;
	pvr_poly_cxt_col(&cxt, PVR_LIST_TR_POLY);
	pvr_poly_compile(&hdr, &cxt);
	pvr_prim(&hdr, sizeof(hdr));
	vert.flags = PVR_CMD_VERTEX;
	vert.x = points.x;
	vert.y = points.y;
	vert.z = nextzcount();
	vert.u = 0;
	vert.v = 0;
	vert.argb = PVR_PACK_COLOR((float)effects.primary_colour.a / 255.0, (float)effects.primary_colour.r / 255.0, (float)effects.primary_colour.g / 255.0, (float)effects.primary_colour.b / 255.0);
	vert.oargb = 0;
	pvr_prim(&vert, sizeof(vert));

	vert.x = points.w;
	vert.y = points.h;
	pvr_prim(&vert, sizeof(vert));

	vert.x = points.w + 1;
	vert.y = points.y;
	pvr_prim(&vert, sizeof(vert));

	vert.flags = PVR_CMD_VERTEX_EOL;
	vert.x = points.x + 1;
	vert.y = points.h;
	pvr_prim(&vert, sizeof(vert));

}

int32_t LuxGraphics_DC_DrawChar( int32_t cchar, int32_t x, int32_t y, int32_t z, ObjectEffect effects, bool allow_custom )
{
	pvr_ptr_t tex;

	/* Sprite Position */
	LuxRect dest;

	dest.x = x;
	dest.y = y;
	dest.h = 8;
	dest.w = 8;

	if ( false && allow_custom )
	{
		std::stringstream stream;
		stream << (int)cchar;
		LuxSprite * sdata = lux::display->GetSprite( lux::display->sprite_font, stream.str() );
		if (!sdata )
		{
			return 0;
		}

		Texture * texture = (Texture *)sdata->data;
		if ( !texture )
		{
			return 0;
		}
		dest.h = texture->th;
		dest.w = texture->th;
		tex = texture->pointer;
	}
	else
	{
		if ( cchar <= 32 )
		{
			tex = NULL;
		}
		else if ( cchar <= 128 )
		{
			tex = font_texture[cchar-32];
		}
		else
		{
			tex = font_texture[31];
		}
	}
	uint32_t c1 = PVR_PACK_COLOR((float)effects.primary_colour.a / 255.0, (float)effects.primary_colour.r / 255.0, (float)effects.primary_colour.g / 255.0, (float)effects.primary_colour.b / 255.0);

	pvr_poly_hdr_t hdr;
	pvr_poly_cxt_t cxt;
	pvr_vertex_t vert;
	pvr_poly_cxt_txr(&cxt, PVR_LIST_TR_POLY, PVR_TXRFMT_PAL4BPP, dest.w, dest.h, tex, PVR_FILTER_NONE);
	pvr_poly_compile(&hdr, &cxt);

	pvr_prim(&hdr, sizeof(hdr));

	vert.flags = PVR_CMD_VERTEX;
	vert.x = dest.x;
	vert.y = dest.y + dest.h;
	vert.z = nextzcount();
	vert.u = 0.0f;
	vert.v = 1.0f;
	vert.argb = c1;
	vert.oargb = 0;
	pvr_prim(&vert, sizeof(vert));

	vert.y = dest.y;
	vert.u = 0.0f;
	vert.v = 0.0f;
	pvr_prim(&vert, sizeof(vert));

	vert.x = dest.x + dest.w;
	vert.y = dest.y + dest.h;
	vert.u = 1.0f;
	vert.v = 1.0f;

	pvr_prim(&vert, sizeof(vert));

	vert.flags = PVR_CMD_VERTEX_EOL;
	vert.u = 1.0f;
	vert.v = 0.0f;
	vert.y = dest.y;
	pvr_prim(&vert, sizeof(vert));

	return dest.w-1;
}


LUX_DISPLAY_FUNCTION void LuxGraphics_DC_DrawText( std::string text, LuxRect dest_rect, ObjectEffect effects, bool allow_custom)
{

	uint16_t x = dest_rect.x;

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
			dest_rect.y += 10; // ( sdlgraphics_customtext && allow_custom ? sdlgraphics_customtext_height + 2 : 10);
			x = dest_rect.x;
		}
		else if ( cchar <= 32 )
		{
			x += 7;
		}
		else if ( cchar <= 128 )
		{
			x += LuxGraphics_DC_DrawChar(cchar, x, dest_rect.y,  dest_rect.z, effects, allow_custom);
		}
		else if ( cchar < 224 )
		{
			object++;
			uint32_t next = *object;

			cchar = ((cchar << 6) & 0x7ff) + (next & 0x3f);
			x += LuxGraphics_DC_DrawChar(cchar, x, dest_rect.y,  dest_rect.z, effects, allow_custom);

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

			x += LuxGraphics_DC_DrawChar(cchar, x, dest_rect.y, dest_rect.z, effects, allow_custom);
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

			x += LuxGraphics_DC_DrawChar(cchar, x, dest_rect.y,  dest_rect.z, effects, allow_custom);
		}
	}

}


LUX_DISPLAY_FUNCTION void LuxGraphics_DC_DrawPolygon( int16_t * x_point, int16_t *y_point, uint16_t point_count, LuxRect position, ObjectEffect effects, void * texture )
{

	pvr_poly_hdr_t hdr;
	pvr_poly_cxt_t cxt;
	pvr_vertex_t vert;
	pvr_poly_cxt_col(&cxt, PVR_LIST_TR_POLY);
	pvr_poly_compile(&hdr, &cxt);
	pvr_prim(&hdr, sizeof(hdr));

	vert.flags = PVR_CMD_VERTEX;
	vert.argb = PVR_PACK_COLOR((float)effects.primary_colour.a / 255.0, (float)effects.primary_colour.r / 255.0, (float)effects.primary_colour.g / 255.0, (float)effects.primary_colour.b / 255.0);
	vert.z = nextzcount();
	vert.u = 0;
	vert.v = 0;

	vert.oargb = 0;
	for ( uint16_t p = 0; p < point_count; p++ )
	{
		vert.x = x_point[p];
		vert.y = y_point[p];
		if ( p == point_count - 1 )
			vert.flags = PVR_CMD_VERTEX_EOL;
		pvr_prim(&vert, sizeof(vert));

	}




}

LUX_DISPLAY_FUNCTION void LuxGraphics_DC_DrawCircle( LuxRect dest_rect, ObjectEffect effects )
{

}


