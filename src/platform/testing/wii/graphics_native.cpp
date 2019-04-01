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
#include "platform_functions.h"
#include <bitfont.h>
#include <gccore.h>
#include <malloc.h>
#include <wiiuse/wpad.h>
#include <wiisprite.h>

struct Texture {
	GXTexObj texnum;
	void * data;
	uint32_t w, h, tw, th;
	bool pot, loaded;
	std::string file;
};

/* Global Variables */
GXTexObj font[128];



/* External Functions */
int Lux_NATIVE_PowerOfTwo( int value )
{
	int i;
	for ( i=31; i>=0; i--)
	{
		if (value & (1<<i))
			break;
	}
	if (value & ~-(1<<i))
		i++;

	return i;
}
void wii_SetBits( uint32_t &v, uint8_t n, uint8_t p, uint8_t s )
{
	n = clamp( n, 0, (1<<s)-1);
	v |= (n << p);
}

void Lux_NATIVE_CreateFont()
{
	uint8_t * font_point = &gfxPrimitivesFontdata[0];
	font_point += (32*8);
	uint8_t i = 0, q = 0, a = 0;
	uint32_t * charflip = new uint32_t[8];
	memset( charflip, 0, 64 );
	for ( uint8_t c = 0; c < 93; c++)
	{
		//DCFlushRange(charflip, 32);
		for ( i = 0; i < 8; i++)
		{
			for (q = 0; q < 8; q++)
			{
				wii_SetBits( charflip[i], !!(font_point[i] & (1 << 8-q) ), q*4, 4);
			}
		}
		GX_InitTexObj( &font[c], charflip, 8, 8, GX_TF_I4, GX_CLAMP, GX_CLAMP, GX_FALSE);
	//	GX_InvalidateTexAll();
		font_point += 8;
	}

	delete[] charflip;
}

wsp::GameWindow gwd;

void wii_initialise_video( bool console_only )
{
	gwd.InitVideo();
	gwd.SetBackground((GXColor){ 0, 0, 0, 255 });

}

bool Lux_NATIVE_Init( uint16_t  width, uint16_t height, uint8_t bpp, uint16_t * actual_width, uint16_t * actual_height, bool fullscreen )
{
	Lux_NATIVE_CreateFont();
	return true;
}


void Lux_NATIVE_Destory()
{

}

void Lux_NATIVE_Display2Screen( int32_t * x, int32_t * y )
{
	*x = (int32_t)((float)*x * 1.0);
	*y = (int32_t)((float)*y * 1.0);
}

void Lux_NATIVE_BackgroundObject( MapObject background )
{
	GX_SetCopyClear((GXColor) { background.effects.primary_colour.r, background.effects.primary_colour.g, background.effects.primary_colour.b, background.effects.primary_colour.a}, 0x00ffffff);
}

void Lux_NATIVE_UpdateRect(uint8_t screen,LuxRect rect)
{

}

void Lux_NATIVE_Show( uint8_t screen )
{
	gwd.Flush();
}

/*
bool Lux_NATIVE_LoadTexture(Texture * texure)
{
	if ( !texure->pot )
		return false;
	if ( texure->loaded )
		return true;
	
	uint8_t * data = nullptr;
	uint32_t size = 0;

	size = lux::game->GetFile(texure->file, &data, false);

	if ( size )
	{
		glGenTextures(1, &texure->texnum);
		glBindTexture(0, texure->texnum);
		glTexImage2D(0, 0, GL_RGB, texure->tw, texure->th, 0, GL_TEXTURE_WRAP_S|GL_TEXTURE_WRAP_T|TEXGEN_TEXCOORD, data);
		texure->loaded = true;
		delete data;
		return true;
	}

	return false;

}
*/

bool Lux_NATIVE_LoadSpriteSheet(std::string name, std::map<uint32_t, LuxSprite *> * children)
{
	/*
	std::map<uint32_t, LuxSprite *>::iterator p;
	for( p = children->begin(); p != children->end(); p++ )
	{
		if ( !p->second->animated )
		{
			Texture * tex = new Texture;
			tex->tw = Lux_NATIVE_PowerOfTwo( p->second->rect.w ) - 3;
			tex->th = Lux_NATIVE_PowerOfTwo( p->second->rect.h ) - 3;
			tex->w = p->second->rect.w;
			tex->h = p->second->rect.h;
			tex->pot = !((tex->w & (tex->w - 1)) && (tex->h & (tex->h - 1)));
			tex->loaded = false;
			tex->file = "./sprites/" + name + ":" + p->first;
			p->second->data = (void*)tex;
		}
	}
	*/
	return true;
}

bool Lux_NATIVE_FreeSpriteSheet( std::map<uint32_t, LuxSprite *> * children)
{
	/*
	std::map<uint32_t, LuxSprite *>::iterator p;
	for( p = children->begin(); p != children->end(); p++ )
	{
		delete (Texture*)p->second->data;
	}
	*/
	return true;
}




void Lux_NATIVE_DrawSprite( LuxSprite * sprite, LuxRect dest_rect, ObjectEffect effects )
{
	/*
	int16_t z = dest_rect.z/1000;
	if ( !data )
		return;
	GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
		GX_Position3f32(dest_rect.x, dest_rect.y, 0.0f);
		GX_Color4u8(effects.primary_colour.r,effects.primary_colour.g,effects.primary_colour.b,effects.primary_colour.a);
		GX_Position3f32(dest_rect.x + dest_rect.w, dest_rect.y, 0.0f);
		GX_Color4u8(effects.primary_colour.r,effects.primary_colour.g,effects.primary_colour.b,effects.primary_colour.a);
		GX_Position3f32(dest_rect.x + dest_rect.w, dest_rect.y + dest_rect.h, 0.0f);
		GX_Color4u8(effects.primary_colour.r,effects.primary_colour.g,effects.primary_colour.b,effects.primary_colour.a);
		GX_Position3f32(dest_rect.x, dest_rect.y + dest_rect.h, 0.0f);
		GX_Color4u8(effects.primary_colour.r,effects.primary_colour.g,effects.primary_colour.b,effects.primary_colour.a);
	GX_End();
	
	wsp::Quad quad;
	quad.SetPosition(0,0);
	quad.SetWidth(dest_rect.w);
	quad.SetHeight(dest_rect.y);
	quad.SetFillColor((GXColor) {effects.primary_colour.a,effects.primary_colour.b,effects.primary_colour.g,effects.primary_colour.r});
	quad.Draw(dest_rect.x, dest_rect.y);
	*/
}

void Lux_NATIVE_DrawRect( LuxRect dest_rect, ObjectEffect effects )
{
	wsp::Quad quad;
	quad.SetPosition(0,0);
	quad.SetWidth(dest_rect.w);
	quad.SetHeight(dest_rect.y);
	quad.SetFillColor((GXColor) {effects.primary_colour.a,effects.primary_colour.b,effects.primary_colour.g,effects.primary_colour.r});
	quad.Draw(dest_rect.x, dest_rect.y);
}

void Lux_NATIVE_DrawLine( LuxRect points, ObjectEffect effects )
{
	wsp::Quad quad;
	quad.SetPosition(0,0);
	quad.SetWidth(points.w);
	quad.SetHeight(points.y);
	quad.SetFillColor((GXColor) {effects.primary_colour.a,effects.primary_colour.b,effects.primary_colour.g,effects.primary_colour.r});
	quad.Draw(points.x, points.y);
}

void Lux_NATIVE_DrawText( std::string text, LuxRect dest_rect, ObjectEffect effects, bool allow_custom )
{

	uint16_t x = dest_rect.x;
	std::string::iterator object;

	GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);
	GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);

	for ( object = text.begin(); object != text.end(); object++ )
	{
		uint32_t cchar = *object;
		if (cchar == '\n' || cchar == '\r')
		{
			dest_rect.y += 10;
			x = dest_rect.x;
		}
		else if ( cchar <= 128 )
		{
			/*
			GX_InvalidateTexAll();
			GX_LoadTexObj(&font[(cchar-32)], GX_TEXMAP0);
			// Draw a Quad
			GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
				GX_Position2f32(x, dest_rect.y);
				GX_TexCoord2f32(0.0, 0.0);
				GX_Color4u8(effects.primary_colour.r, effects.primary_colour.g, effects.primary_colour.b, effects.primary_colour.a);

				GX_Position2f32(x + 7, dest_rect.y);
				GX_TexCoord2f32(1.0, 0.0);
				GX_Color4u8(effects.primary_colour.r, effects.primary_colour.g, effects.primary_colour.b, effects.primary_colour.a);

				GX_Position2f32(x + 7, dest_rect.y + 14);
				GX_TexCoord2f32(1.0, 1.0);
				GX_Color4u8(effects.primary_colour.r, effects.primary_colour.g, effects.primary_colour.b, effects.primary_colour.a);

				GX_Position2f32(x, dest_rect.y + 14);
				GX_TexCoord2f32(0.0, 1.0);
				GX_Color4u8(effects.primary_colour.r, effects.primary_colour.g, effects.primary_colour.b, effects.primary_colour.a);
			GX_End();
			*/
		}
		else if ( cchar < 224 )
			object++;
		else if ( cchar < 240 )
			object +=2;
		else if ( cchar < 245 )
			object +=3;

		if ( cchar > 31 )
			x += 8;

	}
	GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);
	GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);
}

void Lux_NATIVE_TextSprites( bool able )
{

}

void Lux_NATIVE_DrawPolygon( int16_t * x_point, int16_t *y_point, uint16_t point_count, ObjectEffect effects, void * texture )
{
	/*
	wsp::Quad quad;
	quad.SetPosition(0,0);
	quad.SetWidth(16);
	quad.SetHeight(16);
	quad.SetFillColor((GXColor) {effects.primary_colour.r,effects.primary_colour.g,effects.primary_colour.b,effects.primary_colour.a});
	quad.Draw(*x_point, *y_point);
	*/
	

	if (point_count < 20)
	{
		GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
		GX_SetVtxDesc(GX_VA_TEX0, GX_NONE);
		GX_Begin(GX_TRIANGLEFAN, GX_VTXFMT0, point_count);
		
		for ( uint16_t p = 0; p < point_count; p++ )
		{
			GX_Color4u8(effects.primary_colour.r, effects.primary_colour.g, effects.primary_colour.b, effects.primary_colour.a);
			GX_Position2f32(x_point[p], y_point[p]);
		}
		GX_End();
		GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);
		GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	}
	GX_DrawDone();
}

void Lux_NATIVE_DrawCircle( LuxRect dest_rect, ObjectEffect effects )
{

}

LuxSprite * Lux_NATIVE_PNGtoSprite( uint8_t * data, uint32_t size )
{
	return nullptr;
}

