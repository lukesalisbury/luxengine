/****************************
Copyright (c) 2006-2009 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifdef D3DENABLED

/* Required Headers */
#include "../luxengine.h"
#include "../functions.h"
#include "display_functions.h"

#include <windows.h>
#include <SDL/SDL_getenv.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_syswm.h>

#define WIN32_LEAN_AND_MEAN

#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr9.h>

/* Global Variables */
struct Texture {
	int w, h, tw, th;
};
struct d3dVertex
{
	float x, y, z;
	DWORD dwColor;
	float tu, tv;
};

Uint32 d3dGraphics_flags = 0;
LuxColour d3dGraphics_colour = {0,0,0,255};
SDL_Rect d3dGraphics_dimension = {0, 0, 320,240};
SDL_Surface * d3dGraphics_screen = NULL;

IDirect3D9 * d3dGraphics_object = NULL;
IDirect3DDevice9 * d3d_device = NULL;
ID3DXFont * d3d_font = NULL;

/* External Functions */
SDL_RWops * Lux_Game_Read(std::string file);

/* local Functions */
int Lux_D3D_PowerOfTwo( int value )
{
	int result = 1 ;
	while ( result < value )
		result *= 2 ;
	return result;
}

SDL_Surface * Lux_D3D_LoadSpriteImage(std::string file)
{
	SDL_Surface * temp_surface = NULL;
	SDL_RWops * sprite_rwops = Lux_Game_Read(file);
	if (sprite_rwops != NULL)
	{
		temp_surface = IMG_Load_RW(sprite_rwops, 1);
		if ( temp_surface == NULL )
		{
			std::cerr << __FILE__ << ":" << __LINE__ << " | Error Loading: " << file << ". " << IMG_GetError() << std::endl;
		}
		SDL_SetAlpha(temp_surface, 0, 255);
		
	}
	return temp_surface;
}

/* Functions */
bool Lux_D3D_Init(int width, int height, int bpp, bool fullscreen)
{
	if ( get_engine()->config->GetString("display.mode") != "Direct3D" )
		return false;

	/* Set Init Flags */
	d3dGraphics_flags = SDL_SWSURFACE;

	if ( get_engine()->config->GetBoolean("display.fullscreen") || fullscreen )
		d3dGraphics_flags |= SDL_FULLSCREEN;

	int scaled_width = get_engine()->config->GetNumber("display.width");
	int scaled_height = get_engine()->config->GetNumber("display.height");

	d3dGraphics_screen = SDL_SetVideoMode(scaled_width, scaled_height, bpp, d3dGraphics_flags );
	if ( !d3dGraphics_screen )
	{
		lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << __FILE__ << ":" << __LINE__ << " | Couldn't set " << scaled_width << "x" << scaled_height << " video mode. " << SDL_GetError() << std::endl;
		return false;
	}
	d3dGraphics_dimension.w = width;
	d3dGraphics_dimension.h =  height;

	static SDL_SysWMinfo pInfo;
	SDL_VERSION(&pInfo.version);
	SDL_GetWMInfo(&pInfo); 

	d3dGraphics_object = Direct3DCreate9(D3D_SDK_VERSION); 
	if( d3dGraphics_object == NULL )
	{	
		std::cerr << "Could not create Direct3D Object" << std::endl;
		return false;
	}

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.hDeviceWindow  = pInfo.window;
	d3dpp.Windowed = true;
	d3dpp.BackBufferWidth = scaled_width;
	d3dpp.BackBufferHeight = scaled_height;
	d3dpp.BackBufferFormat = D3DFMT_R5G6B5;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	d3dpp.BackBufferCount = 1;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	HRESULT hr = d3dGraphics_object->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,pInfo.window, D3DCREATE_SOFTWARE_VERTEXPROCESSING,&d3dpp,&d3d_device);
	if (FAILED(hr))
	{
		lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << __FILE__ << ":" << __LINE__ << " | " << DXGetErrorString9(hr) << "\t" << DXGetErrorDescription9(hr) << std::endl;
		return false;
	}
	D3DCAPS9 d3dCaps;
	d3dGraphics_object->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &d3dCaps);

	D3DADAPTER_IDENTIFIER9 d3dAdapterID;
	d3dGraphics_object->GetAdapterIdentifier(D3DADAPTER_DEFAULT, 0, &d3dAdapterID);
	lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << __FILE__ << ":" << __LINE__ << " | Video Driver: Direct3D" << std::endl;
	lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << __FILE__ << ":" << __LINE__ << " | Description: " <<  d3dAdapterID.Description<< std::endl;
	lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << __FILE__ << ":" << __LINE__ << " | Version: " << LOWORD(d3dAdapterID.DriverVersion.HighPart) << "." << HIWORD(d3dAdapterID.DriverVersion.LowPart) << std::endl;
	lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << __FILE__ << ":" << __LINE__ << " | Acceleration: HAL" << std::endl;
	lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << __FILE__ << ":" << __LINE__ << " | Max Texture Size: " << d3dCaps.MaxTextureWidth << "x" << d3dCaps.MaxTextureHeight << std::endl;

	SDL_WM_SetCaption("Lux Engine", NULL);

	d3d_device->SetFVF((D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1));

	D3DXMATRIX matOrtho;
	D3DXMATRIX matView;

	D3DXMatrixOrthoOffCenterLH(&matOrtho, 0.0f, (float)width, (float)height, 0.0f, 0.0f, 8.0f);
	D3DXMatrixIdentity(&matView);

	d3d_device->SetTransform(D3DTS_PROJECTION, &matOrtho);
	d3d_device->SetTransform(D3DTS_WORLD, &matView);
	d3d_device->SetTransform(D3DTS_VIEW, &matView);
	d3d_device->SetRenderState(D3DRS_LIGHTING, false);
	d3d_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	HFONT hFont = CreateFont(9, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Arial");
	D3DXCreateFont( d3d_device, hFont, &d3d_font );
	return true;
}
void Lux_D3D_Destory()
{
	/* TODO
	-
	*/
	d3d_device->Release();
	d3d_device = NULL;
	d3dGraphics_object->Release();
	d3dGraphics_object = NULL;
}

void Lux_D3D_Background(LuxColour fillcolor)
{
	/* TODO
	- Draw background colour
	*/
	d3dGraphics_colour = fillcolor;
}

void Lux_D3D_Update(LuxRect rect)
{

}

void Lux_D3D_Show()
{
	d3d_device->EndScene();
	d3d_device->Present(NULL,NULL,NULL,NULL);

	d3d_device->Clear(0,NULL,D3DCLEAR_TARGET ,D3DCOLOR_XRGB(d3dGraphics_colour.r,d3dGraphics_colour.g,d3dGraphics_colour.b),1.0f,0);
	d3d_device->BeginScene();
	//d3d_device->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
d3d_device->SetRenderState(D3DRS_LIGHTING, FALSE);
d3d_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
d3d_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
d3d_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

}

bool Lux_D3D_LoadSpriteSheet(std::string name, std::map<uint32_t, LuxSprite *> * children)
{
	SDL_Surface * parent_sheet = Lux_D3D_LoadSpriteImage("./sprites/" + name);
	if (!parent_sheet)
	{
		return false;
	}
	int w = 0, h = 0;
	std::map<uint32_t, LuxSprite *>::iterator p;
	for( p = children->begin(); p != children->end(); p++ )
	{
		if (!p->second->animated)
		{
			SDL_Rect tmp_rect;
			w = Lux_D3D_PowerOfTwo( p->second->rect.w );
			h = Lux_D3D_PowerOfTwo( p->second->rect.h );
			tmp_rect.x = p->second->rect.x;
			tmp_rect.y = p->second->rect.y;
			tmp_rect.w = p->second->rect.w;
			tmp_rect.h = p->second->rect.h;
			SDL_Surface * temp_sheet = SDL_CreateRGBSurface ( d3dGraphics_flags, w, h, 32, RMASK, GMASK, BMASK, AMASK );
			SDL_BlitSurface(parent_sheet, &tmp_rect, temp_sheet, NULL);
			SDL_SetAlpha(temp_sheet, 0, 0);

			HRESULT result;
			result = d3d_device->CreateTexture(w, h, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, (IDirect3DTexture9**)&p->second->data, NULL);
			if (FAILED(result)) {
				std::cerr << "CreateTexture() " << result << std::endl;
			}
			else
			{
				D3DLOCKED_RECT rect;
				result = ((IDirect3DTexture9*)p->second->data)->LockRect(0, &rect, 0, 0);
				if ( !FAILED(result) )
				{
					memcpy( rect.pBits, temp_sheet->pixels, w*h*4 );
				}
				else
					std::cerr << "LockRect() " << result << std::endl;
				result = ((IDirect3DTexture9*)p->second->data)->UnlockRect(0);
				if ( FAILED(result) )
					std::cerr << "UnlockRect() " << result << std::endl;
			}

			SDL_FreeSurface(temp_sheet);
		}
	}
	SDL_FreeSurface(parent_sheet);
	return true;
}

void Lux_D3D_FreeSpriteSheet(std::map<uint32_t, LuxSprite *> * children)
{
	std::map<uint32_t, LuxSprite *>::iterator p;
	for( p = children->begin(); p != children->end(); p++ )
	{
		if (!p->second->animated)
		{
			((IDirect3DTexture9*)p->second->data)->Release();
			delete (IDirect3DTexture9*)p->second->data;
		}
	}
}

void Lux_D3D_DrawSprite(LuxSprite * sprite, LuxRect dest_rect, ObjectEffect effects)
{

	if (!d3dGraphics_screen)
	{
		lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << __FILE__ << ":" << __LINE__ << " | Not a valid surface." << std::endl;
	}
	float l = (float)(dest_rect.x);
	float t = (float)(dest_rect.y);
	float r = (float)(dest_rect.x + dest_rect.w);
	float b = (float)(dest_rect.y + dest_rect.h);
	// Create the vertices for the sprite
	d3dVertex tile[4] = {
		{ l, t, 0.0f, D3DCOLOR_ARGB(effects.primary_colour.a, effects.primary_colour.r, effects.primary_colour.g, effects.primary_colour.b), 0.0f, 0.0f},
		{ l, b, 0.0f, D3DCOLOR_ARGB(effects.primary_colour.a, effects.primary_colour.r, effects.primary_colour.g, effects.primary_colour.b), 0.0f, 1.0f},
		{ r, t, 0.0f, D3DCOLOR_ARGB(effects.primary_colour.a, effects.primary_colour.r, effects.primary_colour.g, effects.primary_colour.b), 1.0f, 0.0f},
		{ r, b, 0.0f, D3DCOLOR_ARGB(effects.primary_colour.a, effects.primary_colour.r, effects.primary_colour.g, effects.primary_colour.b), 1.0f, 1.0f},
	};
	d3d_device->SetTexture(0, (IDirect3DTexture9*)data);
	d3d_device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, tile, sizeof(d3dVertex));
	d3d_device->SetTexture(0, NULL);
}

void Lux_D3D_DrawRect( LuxRect dest_rect, ObjectEffect effects)
{

	if (!d3dGraphics_screen)
	{
		lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << __FILE__ << ":" << __LINE__ << " | Not a valid surface." << std::endl;
	}
	float l = (float)(dest_rect.x);
	float t = (float)(dest_rect.y);
	float r = (float)(dest_rect.x + dest_rect.w);
	float b = (float)(dest_rect.y + dest_rect.h);
	
	// Create the vertices for the sprite
	d3dVertex tile[4] = {
		{ l, t, 0.0f, D3DCOLOR_ARGB(effects.primary_colour.a, effects.primary_colour.r, effects.primary_colour.g, effects.primary_colour.b), 0.0f, 0.0f},
		{ l, b, 0.0f, D3DCOLOR_ARGB(effects.primary_colour.a, effects.primary_colour.r, effects.primary_colour.g, effects.primary_colour.b), 0.0f, 0.0f},
		{ r, t, 0.0f, D3DCOLOR_ARGB(effects.primary_colour.a, effects.primary_colour.r, effects.primary_colour.g, effects.primary_colour.b), 0.0f, 0.0f},
		{ r, b, 0.0f, D3DCOLOR_ARGB(effects.primary_colour.a, effects.primary_colour.r, effects.primary_colour.g, effects.primary_colour.b), 0.0f, 0.0f},
	};

	d3d_device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, tile, sizeof(d3dVertex));
}

void Lux_D3D_DrawLine ( LuxRect points, ObjectEffect effects)
{
	float l = (float)(points.x);
	float t = (float)(points.y);
	float r = (float)(points.w);
	float b = (float)(points.h);
	
	// Create the vertices for the sprite
	d3dVertex tile[2] = {
		{ l, t, 0.0f, D3DCOLOR_ARGB(effects.primary_colour.a, effects.primary_colour.r, effects.primary_colour.g, effects.primary_colour.b)},
		{ r, b, 0.0f, D3DCOLOR_ARGB(effects.primary_colour.a, effects.primary_colour.r, effects.primary_colour.g, effects.primary_colour.b)},
	};

	d3d_device->DrawPrimitiveUP(D3DPT_LINESTRIP, 1, tile, sizeof(d3dVertex));
}

void Lux_D3D_DrawText (std::string text, LuxRect dest_rect, ObjectEffect effects)
{
	D3DCOLOR fontColor = D3DCOLOR_ARGB(effects.primary_colour.a, effects.primary_colour.r, effects.primary_colour.g, effects.primary_colour.b);  

	RECT rct;
	rct.left=dest_rect.x;
	rct.right=600;
	rct.top=dest_rect.y;
	rct.bottom=dest_rect.y+12;

	d3d_font->DrawText(text.c_str(),-1, &rct, DT_LEFT,fontColor);	
}

void Lux_D3D_DrawPolygon ( short * x_point, short *y_point, short point_count, ObjectEffect effects, void * texture )
{

}

void Lux_D3D_DrawCircle( LuxRect dest_rect, ObjectEffect effects )
{
}

#endif
