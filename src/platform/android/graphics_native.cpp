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
#include "elix_png.h"
#include "core.h"

namespace colour {
	extern LuxColour white;
	extern LuxColour black;
	extern LuxColour red;
	extern LuxColour blue;
	extern LuxColour green;
}

#include <jni.h>

#include <EGL/egl.h>
#include <GLES/gl.h>
#include <android/log.h>
#include <android_native_app_glue.h>
#include <android/native_window.h>

#include "gles/gles.hpp"
#include "gles/gles_display.hpp"
#include "gles/texture.inc.hpp"

#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,"luxengine",__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,"luxengine",__VA_ARGS__)
#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN,"luxengine",__VA_ARGS__)
#ifndef LUX_DISPLAY_FUNCTION
	#define LUX_DISPLAY_FUNCTION
#endif

void Lux_GLES_LoadFont();
void Lux_GLES_UnloadFont();

/* Global Variables */
LuxColour gles_graphics_colour = { 0, 0, 0, 255 };
EGLDisplay android_display = EGL_NO_DISPLAY;
EGLSurface android_surface = EGL_NO_CONTEXT;
EGLContext android_context = EGL_NO_SURFACE;

/* Local Function */


/* Global Function */

/* Lux_NATIVE_Init
 * Init video mode
 @ width:
 @ height:
 @ bpp:
 @ fullscreen:
 - Returns true if successfull
 */
LUX_DISPLAY_FUNCTION bool Lux_NATIVE_Init(uint16_t width, uint16_t height, uint8_t bpp, bool fullscreen )
{


	LOGI("Lux_NATIVE_Init");
	const EGLint attribs[] = {
			EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
			EGL_BLUE_SIZE, 5,
			EGL_GREEN_SIZE, 6,
			EGL_RED_SIZE, 5,
			EGL_NONE
	};
	EGLint w, h, dummy, format, major, minor;
	EGLint numConfigs;
	EGLConfig config;

	android_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	eglInitialize(android_display, NULL, NULL);

	LOGI("eglChooseConfig");
	eglChooseConfig(android_display, attribs, &config, 1, &numConfigs);
	LOGI("eglGetConfigAttrib");
	eglGetConfigAttrib(android_display, config, EGL_NATIVE_VISUAL_ID, &format);

	LOGI("ANativeWindow_setBuffersGeometry");
	ANativeWindow_setBuffersGeometry(lux::core->GetAndroidApp()->window, width, height, format);

	LOGI("eglCreateWindowSurface");
	android_surface = eglCreateWindowSurface(android_display, config, lux::core->GetAndroidApp()->window, NULL);
	LOGI("eglCreateContext");
	android_context = eglCreateContext(android_display, config, NULL, NULL);

	LOGI("eglMakeCurrent");
	if (eglMakeCurrent(android_display, android_surface, android_surface, android_context) == EGL_FALSE)
	{
		LOGW("Unable to eglMakeCurrent");
		return false;
	}

	eglQuerySurface(android_display, android_surface, EGL_WIDTH, &w);
	eglQuerySurface(android_display, android_surface, EGL_HEIGHT, &h);

	LOGI("EGL Created: %dx%d", w, h);

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrthof(0.0f, (float)width, (float)height, 0, -10.0f, 10.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	Lux_GLES_LoadFont();

	return true;

}

LUX_DISPLAY_FUNCTION void Lux_NATIVE_Destory()
{
	Lux_GLES_UnloadFont();

	if (android_display != EGL_NO_DISPLAY)
	{
		eglMakeCurrent(android_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		if (android_context != EGL_NO_CONTEXT)
		{
			eglDestroyContext(android_display, android_context);
		}
		if (android_surface != EGL_NO_SURFACE)
		{
			eglDestroySurface(android_display, android_surface);
		}
		eglTerminate(android_display);
	}
	android_display = EGL_NO_DISPLAY;
	android_context = EGL_NO_CONTEXT;
	android_surface = EGL_NO_SURFACE;

}

LUX_DISPLAY_FUNCTION void Lux_NATIVE_BackgroundObject( MapObject background  )
{
	gles_graphics_colour = background.effects.primary_colour;
	glClearColor((float)gles_graphics_colour.r / 255.0f, (float)gles_graphics_colour.g / 255.0f, (float)gles_graphics_colour.b / 255.0f, 1.0f);
}

LUX_DISPLAY_FUNCTION void Lux_NATIVE_UpdateRect(LuxRect rect)
{

}


LUX_DISPLAY_FUNCTION void Lux_NATIVE_Show()
{
	if ( android_display != EGL_NO_DISPLAY )
	{
		eglSwapBuffers(android_display, android_surface);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
}

/* resources */

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
		Texture * texture = (Texture*)sprite->data;
		lux::OpenGLTexture::destroy(texture);
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

	Texture * texture = new Texture;

	texture->w = sprite->rect.w;
	texture->h = sprite->rect.h;
	texture->tw = lux::OpenGLTexture::power(texture->w);
	texture->th = lux::OpenGLTexture::power(texture->h);

	sprite->data = texture;

	/* PNG image */
	uint32_t * pixels = new uint32_t[texture->tw*texture->th];
	if ( pixels )
	{
		memset(pixels, 0x00, texture->tw * texture->th * 4 );
		for( uint16_t y = 0; y < texture->h; y++ )
		{
			for( uint16_t x = 0; x < texture->w; x++ )
			{
				uint32_t q = (texture->tw * y) + x;
				pixels[q] = png->GetPixel(rect.x + x, rect.y + y);
			}
		}
		lux::OpenGLTexture::create(texture, pixels, texture->tw, texture->th, GLES_TEXTURE_32);
	}
	delete pixels;
	/*  ^ PNG Image */

	return true;
}


/* Lux_NATIVE_LoadSpriteSheet
 *
 @ name:
 @ children:
 -
 */
LUX_DISPLAY_FUNCTION bool Lux_NATIVE_LoadSpriteSheet( std::string name, std::map<uint32_t, LuxSprite *> * children )
{
	/* PNG Image */
	uint8_t * data = NULL;
	uint32_t size;
	elix::Image * png = new elix::Image;
	if ( lux::game )
	{
		size = lux::game->GetFile("./sprites/" + name, &data, false);
		if ( size )
		{
			png->LoadFile(data, size);
		}
	}
	delete data;
	/*  ^ PNG Image */

	if ( png->HasContent() )
	{
		std::map<uint32_t, LuxSprite *>::iterator p = children->begin();
		for( ; p != children->end(); p++ )
		{
			if ( !p->second->animated )
			{
				Lux_NATIVE_CreateSprite( p->second, p->second->rect, png );
			}
		}
		delete png;
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
		sprite = new LuxSprite(true);
		sprite->rect.x = sprite->rect.y = 0;
		sprite->rect.w = png->Width();
		sprite->rect.h = png->Height();

		Lux_NATIVE_CreateSprite( sprite, sprite->rect, png );
	}
	delete png;
	return sprite;

}


