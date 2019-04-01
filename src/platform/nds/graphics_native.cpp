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
#include "luxengine.h"
#include "map_object.h"
#include "mokoi_game.h"
#include "display/display.h"
#include "graphics_native.h"
#include "graphics_system.h"

#include "bitfont.h"
#include "elix/elix_png.hpp"
#include "elix/elix_string.hpp"

#include <nds.h>
#include <nds/arm9/trig_lut.h>
//#include <malloc.h>

#define NDSMAXOBJECTS 100

/* */







/* Global Variables */

struct Texture {
	int32_t texnum;
	int32_t address;
	uint32_t w, h, tw, th;
	bool pot, loaded;
	std::string file;
};


int32_t font[128];
v16 nds_z = 0;
bool dual_screen = 0;
bool dual_screen_bottom = 0;

GraphicSystem GraphicsNative = {
	&LuxGraphics_NDS_Init,
	&LuxGraphics_NDS_Destory,
	&Lux_GRAPHICS_Display2Screen,
	&Lux_GRAPHICS_TextSprites,
	&Lux_GRAPHICS_PreShow,
	&Lux_GRAPHICS_UpdateRect,
	&LuxGraphics_NDS_Show,

	&Lux_GRAPHICS_SetRotation,
	&LuxGraphics_NDS_BackgroundObject,
	&Lux_GRAPHICS_SetFullscreen,
	&Lux_GRAPHICS_Resize,
	&Lux_GRAPHICS_DisplayPointer,

	&LuxGraphics_NDS_RefreshSpriteSheet,
	&LuxGraphics_NDS_LoadSpriteSheet,
	&LuxGraphics_NDS_LoadSpriteSheetImage,
	&LuxGraphics_NDS_FreeSpriteSheet,
	nullptr,
	&LuxGraphics_NDS_FreeSprite,
	&LuxGraphics_NDS_PNGtoSprite,

	&LuxGraphics_NDS_DrawSprite,
	&LuxGraphics_NDS_DrawRect,
	&LuxGraphics_NDS_DrawCircle,
	&Lux_GRAPHICS_DrawPolygon,
	&LuxGraphics_NDS_DrawLine,
	&LuxGraphics_NDS_DrawText

};


/* Local Functions */
static inline void glVertex3i(v16 x, v16 y, v16 z)
{
	GFX_VERTEX16 = (y << 16) | (x & 0xFFFF);
	GFX_VERTEX16 = ((uint32)(uint16)z);
}

static int nds_texture_size( uint32_t q )
{
	if (q > 256)
		return TEXTURE_SIZE_512;
	else if (q > 128)
		return TEXTURE_SIZE_256;
	else if (q > 64)
		return TEXTURE_SIZE_128;
	else if (q > 32)
		return TEXTURE_SIZE_64;
	else if (q > 16)
		return TEXTURE_SIZE_32;
	else if (q > 8)
		return TEXTURE_SIZE_16;
	else
		return TEXTURE_SIZE_8;
}



bool nds_offscreen(int16_t & x1, int16_t & y1, int16_t & x2, int16_t & y2)
{
	if (!dual_screen)
		return true;
	if ( !dual_screen_bottom )
	{
		if (y2 < 192) return false;

		y2 -= 192;
		y1 -= 192;
		return true;
	}
	else
	{
		if (y1 > 191) return false;
		return true;
	}

}

void ndsDrawBegin( )
{
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();

	glEnable( GL_BLEND );
	glEnable( GL_TEXTURE_2D );
	glDisable( GL_ANTIALIAS );
	glDisable( GL_OUTLINE );

	glColor( 0x7FFF );
	glPolyFmt( POLY_ALPHA(31) | POLY_CULL_NONE );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrthof32( 0, 256, 192, 0, -1024, 1 << 12 );

	glMatrixMode( GL_TEXTURE );
	glLoadIdentity();

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	nds_z = 0;
}

void ndsDrawEnd( void )
{
	glMatrixMode( GL_PROJECTION );
	glPopMatrix( 1 );
	glMatrixMode( GL_MODELVIEW );
	glPopMatrix( 1 );
}

void nds2DDrawQuad(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t z, uint32_t color, uint32_t color2, uint32_t color3, uint32_t color4, uint32_t texture, float u1, float v1, float u2, float v2 )
{
	if ( !nds_offscreen( x1, y1, x2, y2) )
	{
		return;
	}

	if (texture)
		glBindTexture(GL_TEXTURE_2D, texture);
	else
		glBindTexture(GL_TEXTURE_2D, 0);

	glBegin(GL_QUAD);
		GFX_COLOR = color;
		glTexCoord2f( u1, v1 );
		glVertex3i((x1), (y1), (nds_z));
		GFX_COLOR = color2;
		glTexCoord2f( u1, v2 );
		glVertex3i((x1), (y2), (nds_z));
		GFX_COLOR = color3;
		glTexCoord2f( u2, v2 );
		glVertex3i((x2), (y2), (nds_z));
		GFX_COLOR = color4;
		glTexCoord2f( u2, v1 );
		glVertex3i((x2), (y1), (nds_z));
	glEnd();
	nds_z++;
}

int ndsPowerOfTwo( int value )
{
	int result = 1 ;
	while ( result < value )
		result *= 2 ;
	return result;
}

void ndsScaleScreenView(int scaleX, int scaleY)
{
	MATRIX_SCALE = scaleX;
	MATRIX_SCALE = scaleY;
	MATRIX_SCALE = inttof32(1);
}


void ndsSetBits( uint16_t &v, uint8_t n, uint8_t p, uint8_t s )
{
	n = clamp( n, 0, (1<<s)-1);
	v |= (n << p);
}

void ndsCreateFont()
{
	uint8_t * font_point = &gfxPrimitivesFontdata[0];
	font_point += (32*8);
	uint8_t i = 0, q = 0, a = 0;
	uint16_t * textcolour = new uint16_t[4];
	uint16_t * charflip = new uint16_t[8];

	textcolour[0] = RGB15(31,0,0);
	textcolour[1] = RGB15(31,31,31);
	textcolour[2] = RGB15(31,8,8);
	textcolour[3] = RGB15(0,31,0);

	for ( uint8_t c = 0; c < 93; c++)
	{
		for ( i = 0; i < 8; i++)
		{
			for (q = 0; q < 8; q++)
			{
				ndsSetBits( charflip[i], (!!(font_point[i] & (1 << (8-q))))*1, q*2, 2);
			}
		}
		glGenTextures(1, &font[c]);
		glBindTexture(0, font[c]);
		glTexImage2D(0, 0, GL_RGB4, TEXTURE_SIZE_8, TEXTURE_SIZE_8, 0, GL_TEXTURE_WRAP_S|GL_TEXTURE_WRAP_T|GL_TEXTURE_COLOR0_TRANSPARENT , (uint8*)charflip);
		//gluTexLoadPal( textcolour, 4, GL_RGB4 );
		memset( charflip, 0, 16 );
		font_point += 8;
	}

	delete[] charflip;
}


void ndsInitSubSprites()
{
	oamInit(&oamSub, SpriteMapping_Bmp_2D_256, false);

	int x = 0;
	int y = 0;
	for(y = 0; y < 3; y++)
	for(x = 0; x < 4; x++)
	{
		u16 *offset = &SPRITE_GFX_SUB[(x * 64) + (y * 64 * 256)];
		oamSet(&oamSub, x + y * 4, x * 64, y * 64, 0, 15, SpriteSize_64x64, SpriteColorFormat_Bmp, offset, -1, false,false,false,false,false);
	}

	swiWaitForVBlank();

	oamUpdate(&oamSub);

	bgInitSub(3, BgType_Bmp16, BgSize_B16_256x256, 0, 0);
}


/* Public Function */
/* Creation,destruction and loop Functions */



bool LuxGraphics_NDS_Init( uint16_t  width, uint16_t height, uint8_t bpp, uint16_t * actual_width, uint16_t * actual_height, bool fullscreen )
{
	videoSetMode(MODE_5_3D);
	vramSetBankA( VRAM_A_TEXTURE );
	vramSetBankB( VRAM_B_TEXTURE );

	glInit();

	glEnable( GL_TEXTURE_2D );
	glEnable( GL_ANTIALIAS );

	glClearColor( 0, 0, 0, 31 );
	glClearPolyID( 63 );
	glClearDepth( GL_MAX_DEPTH );

	glViewport(0,0,255,191);

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrthof32( 0, 256, 192, 0, -1024, 1 << 12 );

	ndsCreateFont();

	if (height > 192)
	{
		videoSetModeSub(MODE_5_2D);
		dual_screen = 1;
		dual_screen_bottom = 0;
		ndsInitSubSprites();
	}

	ndsDrawBegin();
	return true;
}

void LuxGraphics_NDS_Destory()
{
	/* TODO
	-
	glResetTextures();
	glResetMatrixStack();
	*/
	ndsDrawEnd();

}

void LuxGraphics_NDS_Display2Screen( int32_t * x, int32_t * y )
{
	*x = (int32_t)((float)*x * 1.0);
	*y = (int32_t)((float)*y * 1.0);
}

void LuxGraphics_NDS_Background(LuxColour fillcolor)
{
	glClearColor(fillcolor.r/8,fillcolor.g/8,fillcolor.b/8, 31);

}

void LuxGraphics_NDS_BackgroundObject( MapObject background )
{
	glClearColor(background.effects.primary_colour.r/8,background.effects.primary_colour.g/8,background.effects.primary_colour.b/8, 31);
}

void LuxGraphics_NDS_Update( uint8_t screen, LuxRect rect)
{

}

void LuxGraphics_NDS_Show( uint8_t screen )
{
	ndsDrawEnd();

	glFlush( 0 );
	swiWaitForVBlank();



	if (dual_screen)
	{
		while(REG_DISPCAPCNT & DCAP_ENABLE);

		dual_screen_bottom ^= 1;

		if( dual_screen_bottom )
		{
			lcdMainOnBottom();
			vramSetBankC(VRAM_C_LCD);
			vramSetBankD(VRAM_D_SUB_SPRITE);
			REG_DISPCAPCNT = DCAP_BANK(2) | DCAP_ENABLE | DCAP_SIZE(3);
		}
		else
		{
			lcdMainOnTop();
			vramSetBankD(VRAM_D_LCD);
			vramSetBankC(VRAM_C_SUB_BG);
			REG_DISPCAPCNT = DCAP_BANK(3) | DCAP_ENABLE | DCAP_SIZE(3);
		}


	}

	ndsDrawBegin();
	nds_z = 0;
}

/* Resource Functions */

/* LuxGraphics_NDS_FreeSprite
 *
 @ sprite:
 -
 */
 bool LuxGraphics_NDS_FreeSprite ( LuxSprite * sprite )
{
	glDeleteTextures(1, &((Texture*)sprite->data)->texnum);
	/*
	for (int i = 0; i < 0x40000; ++i) {
		gfx[i] = 0;
	}
	*/
	return false;
}



/* LuxGraphics_NDS_CreateSprite
 *
 @ sprite:
 @ rect:
 @ png:
 -
 */
 bool LuxGraphics_NDS_CreateSprite( LuxSprite * sprite, LuxRect rect, elix::Image * png )
{
	return false;
}

/* LuxGraphics_NDS_LoadTexture
 *
 @ sprite:
 @ sprite_filename:
 -
 */
 bool LuxGraphics_NDS_LoadTexture( LuxSprite * sprite, std::string sprite_filename )
{
	if ( sprite->data )
		return false;
	uint8_t * data = nullptr;
	uint32_t size = 0;
	Texture * texure = new Texture;


	texure->tw = ndsPowerOfTwo( sprite->sheet_area.w );
	texure->th = ndsPowerOfTwo( sprite->sheet_area.h );
	texure->w = sprite->sheet_area.w;
	texure->h = sprite->sheet_area.h;
	texure->pot = ((texure->w == texure->tw) && (texure->h == texure->th));

	if ( texure->pot )
	{
		size = lux::game_data->GetFile(sprite_filename, &data, false);

		if ( size )
		{
			glGenTextures(1, &texure->texnum);
			glBindTexture(0, texure->texnum);
			glTexImage2D(0, 0, GL_RGB, nds_texture_size(texure->tw), nds_texture_size(texure->th), 0, GL_TEXTURE_WRAP_S|GL_TEXTURE_WRAP_T|TEXGEN_TEXCOORD, data);
			texure->loaded = true;

			sprite->data = texure;

			delete data;
			return true;
		}
	}
	delete texure;
	return false;

}

/* LuxGraphics_NDS_LoadSpriteSheet
 *
 @ name:
 @ children:
 -
 */
 bool LuxGraphics_NDS_LoadSpriteSheet(std::string name, std::map<uint32_t, LuxSprite *> * children)
{
	std::string filename;
	std::map<uint32_t, LuxSprite *>::iterator p;
	for( p = children->begin(); p != children->end(); p++ )
	{
		if ( !p->second->animated )
		{
			filename = "./c/sprites/" + name + "-" + elix::string::FromInt(p->first);
			LuxGraphics_NDS_LoadTexture( p->second, filename );
		}
	}
	return true;
}

/* LuxGraphics_NDS_LoadSpriteSheetImage
 *
 @ image:
 @ children:
 -
 */
 bool LuxGraphics_NDS_LoadSpriteSheetImage( elix::Image * image, std::map<uint32_t, LuxSprite *> * children)
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
				LuxGraphics_NDS_CreateSprite( p->second, p->second->sheet_area, image );
			}
		}
		return true;
	}
	else
	{
		return false;
	}

}

/* LuxGraphics_NDS_FreeSpriteSheet
 *
 @ children:
 -
 */
 bool LuxGraphics_NDS_FreeSpriteSheet( std::map<uint32_t, LuxSprite *> * children)
{
	std::map<uint32_t, LuxSprite *>::iterator p;
	for( p = children->begin(); p != children->end(); p++ )
	{
		LuxGraphics_NDS_FreeSprite( p->second );
	}
	return true;
}

/* LuxGraphics_NDS_RefreshSpriteSheet
 *
 @ name:
 @ children:
 -
 */
 bool LuxGraphics_NDS_RefreshSpriteSheet( std::string name, std::map<uint32_t, LuxSprite *> * children )
{
	if ( LuxGraphics_NDS_FreeSpriteSheet( children ) )
		LuxGraphics_NDS_LoadSpriteSheet( name, children );
	return true;
}


 LuxSprite * LuxGraphics_NDS_PNGtoSprite( uint8_t * data, uint32_t size )
{
	LuxSprite * sprite = nullptr;
	elix::Image * src = new elix::Image(data, size);
	if ( src->HasContent() )
	{
		sprite = new LuxSprite( GraphicsNative );

		Texture * tex = new Texture;
		tex->w = src->Width();
		tex->h = src->Height();
		tex->tw = ndsPowerOfTwo( tex->w );
		tex->th = ndsPowerOfTwo( tex->h );
		tex->pot = ((tex->w == tex->tw) && (tex->h == tex->th));
		tex->loaded = false;

		uint16_t * pixels = new uint16_t[tex->tw*tex->th];
		if ( pixels )
		{
			memset(pixels, 0, tex->tw * tex->th * 2 );
			for( uint16_t y = 0; y < tex->h; y++ )
			{
				for( uint16_t x = 0; x < tex->w; x++ )
				{
					uint32_t q = (tex->h * y) + x;
					pixels[q] = src->GetPixel16(x, y);
				}
			}
			glGenTextures(1, &tex->texnum);
			glBindTexture(0, tex->texnum);
			glTexImage2D(0, 0, GL_RGBA, nds_texture_size(tex->tw), nds_texture_size(tex->th), 0, GL_TEXTURE_WRAP_S|GL_TEXTURE_WRAP_T, (uint8*)pixels );
			sprite->data = tex;
			delete pixels;
		}
		else
		{
			delete sprite;
			sprite = nullptr;
		}


	}
	delete src;
	return sprite;
}

/* Drawing Functions */



 void LuxGraphics_NDS_DrawSprite( LuxSprite * sprite, LuxRect dest_rect, ObjectEffect effects )
{
	int16_t z = dest_rect.z/1000;
	if ( !sprite->data )
		return;

	Texture * surface = ((Texture*)sprite->data);

	float x_tile = 1.00, y_tile = 1.00;
	uint32_t c1 = ARGB16(effects.primary_colour.a/8, effects.primary_colour.r/8,effects.primary_colour.g/8,effects.primary_colour.b/8);
	uint32_t c2 = ARGB16(effects.secondary_colour.a/8, effects.secondary_colour.r/8,effects.secondary_colour.g/8,effects.secondary_colour.b/8);
	if ( dest_rect.w == 0 )
		dest_rect.w = surface->w;
	else
		x_tile = ( (float)dest_rect.w / (float)surface->w );

	if ( dest_rect.h == 0 )
		dest_rect.h = surface->h;
	else
		y_tile = ( (float)dest_rect.h / (float)surface->h );



	nds2DDrawQuad( (s16)dest_rect.x, (s16)dest_rect.y, (s16) (dest_rect.x + dest_rect.w), (s16)(dest_rect.y +dest_rect.h), z, c1, c1, c1, c1, surface->texnum, 0.0f, 0.0f, x_tile, y_tile );
}

void LuxGraphics_NDS_DrawRect( LuxRect dest_rect, ObjectEffect effects)
{
	int16_t z = dest_rect.z/1000;
	uint32_t c1 = ARGB16(effects.primary_colour.a/8, effects.primary_colour.r/8,effects.primary_colour.g/8,effects.primary_colour.b/8);
	uint32_t c2 = ARGB16(effects.secondary_colour.a/8, effects.secondary_colour.r/8,effects.secondary_colour.g/8,effects.secondary_colour.b/8);

	s16 x1 = dest_rect.x;
	s16 y1 = dest_rect.y;
	s16 x2 = (dest_rect.x + dest_rect.w);
	s16 y2 = (dest_rect.y + dest_rect.h);

	nds2DDrawQuad( x1, y1, x2, y2, z, c1, c1, c1, c1, 0, 0.0f, 0.0f, 1.0f, 1.0f );
}

void LuxGraphics_NDS_DrawLine( LuxRect points, ObjectEffect effects )
{
	s16 x1 = points.x;
	s16 y1 = points.y;
	s16 x2 = points.w;
	s16 y2 = points.h;
	int16_t z = points.z/1000;
	uint32_t c1 = ARGB16(effects.primary_colour.a/8, effects.primary_colour.r/8,effects.primary_colour.g/8,effects.primary_colour.b/8);

	glBindTexture( 0, 0 );
	glColor( c1 );
	glBegin( GL_TRIANGLES );
		glVertex3i((x1), (y2), (nds_z));
		glVertex3i((x1), (y2), (nds_z));
		glVertex3i((x1), (y2), (nds_z));
	glEnd();

	nds_z++;
}

void LuxGraphics_NDS_DrawText( std::string text, LuxRect dest_rect, ObjectEffect effects, bool allow_custom )
{
	int16_t z = dest_rect.z/1000;
	uint32_t c1 = RGB15(effects.primary_colour.r/8,effects.primary_colour.g/8,effects.primary_colour.b/8);
	uint16_t x = dest_rect.x;
	std::string::iterator object;
	//glPushMatrix();
	for ( object = text.begin(); object != text.end(); object++ )
	{
		uint8_t utfchar = *object;
		uint32_t cchar = utfchar;
		if (cchar == '\n' || cchar == '\r')
		{
			dest_rect.y += 10;
			x = dest_rect.x;
		}
		else if ( cchar == ' ' )
		{
		}
		else if ( cchar <= 128 )
		{
			//glBoxFilled( x, dest_rect.y, x+8, dest_rect.y+8, c1 );
			nds2DDrawQuad( (s16)x, (s16)dest_rect.y, (s16)x+8, (s16)dest_rect.y+8, z, c1, c1, c1, c1, font[(cchar-32)], 0.0f, 0.0f, 1.0f, 1.0f );
		}
		else if ( cchar < 224 )
			object++;
		else if ( cchar < 240 )
			object +=2;
		else if ( cchar < 245 )
			object +=3;

		if ( cchar > 31 )
			x += 7;

	}
	//glPopMatrix(1);
}

void LuxGraphics_NDS_TextSprites( bool able )
{

}

void LuxGraphics_NDS_DrawPolygon( int16_t * x_point, int16_t *y_point, uint16_t point_count, ObjectEffect effects, void * texture )
{

}

void LuxGraphics_NDS_DrawCircle( LuxRect dest_rect, ObjectEffect effects )
{

}



