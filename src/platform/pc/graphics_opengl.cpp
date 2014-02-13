/****************************
Copyright © 2006-2011 Luke Salisbury
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
#include "elix_png.hpp"
#include "game_config.h"
#include "core.h"

#include "display_types.h"
#include "graphics_opengl.h"
#include "graphics_system.h"
#include "gles/gles.hpp"
#include "gles/gles_display.hpp"
#include "gles/gl_camera.hpp"
#include "gles/texture.inc.hpp"
#if REQUIRE_EGL
	#include "gles/egl_display.hpp"
#endif


#include "platform_functions.h"


SDL_Surface * SDL_Surface_LoadImage( std::string file );

GraphicSystem GraphicsOpenGL = {
	&Lux_OGL_Init,
	&Lux_OGL_Destory,
	&Lux_OGL_Display2Screen,
	&Lux_GLES_TextSprites,
	&Lux_OGL_Update,
	&Lux_OGL_Show,

	&Lux_OGL_SetRotation,
	&Lux_OGL_BackgroundObject,
	&Lux_GRAPHICS_SetFullscreen,
	&Lux_GRAPHICS_Resize,
	&Lux_OGL_DisplayPointer,

	&Lux_OGL_RefreshSpriteSheet,
	&Lux_OGL_LoadSpriteSheet,
	&Lux_OGL_LoadSpriteSheetImage,
	&Lux_OGL_FreeSpriteSheet,
	NULL,
	&Lux_OGL_FreeSprite,
	&Lux_OGL_PNGtoSprite,

	&Lux_GLES_DrawSprite,
	&Lux_GLES_DrawRect,
	&Lux_GLES_DrawCircle,
	&Lux_GLES_DrawPolygon,
	&Lux_GLES_DrawLine,
	&Lux_GLES_DrawText

};


/* Global Variables */
uint32_t opengl_window_flags = 0;
LuxColour opengl_graphics_colour = { 0, 0, 0, 255 };
SDL_Rect opengl_graphic_dimension = {0, 0, 320, 240 };
float opengl_graphic_ratio_width = 1.00;
float opengl_graphic_ratio_height = 1.00;
SDL_Surface * opengl_graphic_screen = NULL;
glCamera * opengl_graphic_camera;
bool oglUseNPOT = true;
LuxPolygon * opengl_graphic_cursor = NULL;


std::string opengl_window_title;
uint32_t opengl_graphics_fps = 0, opengl_graphics_fpstime = 0;



/* Lux_OGL_Init
 * Init video mode
 @ width:
 @ height:
 @ bpp:
 @ fullscreen:
 - Returns true if successfull
 */
LUX_DISPLAY_FUNCTION bool Lux_OGL_Init( uint16_t width, uint16_t height, uint8_t bpp, bool fullscreen )
{
	SDL_Surface * opengl_window_icon = NULL;
	if ( lux::config->GetString("display.mode") != "OpenGL" )
		return false;

	/* Set Init Flags */
#if REQUIRE_EGL
	opengl_window_flags = SDL_SWSURFACE;
	bpp = 16;
#else
	opengl_window_flags = SDL_OPENGL|SDL_HWSURFACE;
#endif
	if ( fullscreen )
		opengl_window_flags |= SDL_FULLSCREEN;

	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);

	if ( SDL_WasInit(SDL_INIT_VIDEO) == SDL_INIT_VIDEO )
	{
		SDL_QuitSubSystem(SDL_INIT_VIDEO);
	}
	SDL_InitSubSystem(SDL_INIT_VIDEO);


	/* Set Display Size */
	int32_t scaled_width = lux::config->GetNumber("display.width");
	int32_t scaled_height = lux::config->GetNumber("display.height");

	opengl_graphic_dimension.w = width;
	opengl_graphic_dimension.h = height;

	opengl_graphic_screen = SDL_SetVideoMode(scaled_width, scaled_height, bpp,
										  opengl_window_flags );
	if ( !opengl_graphic_screen )
	{
		lux::core->SystemMessage(SYSTEM_MESSAGE_INFO, __FILE__, __LINE__) << " Couldn't set "
				  << scaled_width << "x" << scaled_height << " video mode. "
				  << SDL_GetError() << std::endl;
		return false;
	}

#if REQUIRE_EGL
	if ( !gles::openDisplay(width, height, fullscreen , bpp) )
	{
		lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << "EGL Failed " << std::endl;
		return false;
	}
#endif


	opengl_graphic_ratio_width = (float)width/(float)opengl_graphic_screen->w;
	opengl_graphic_ratio_height = (float)height/(float)opengl_graphic_screen->h;


	/* */
	opengl_window_icon = SDL_Surface_LoadImage("./resources/icon256.png");

	if ( !opengl_window_icon )
	{
		lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << "256x256 icon not found." << std::endl;
		opengl_window_icon = SDL_Surface_LoadImage("./resources/icon32.png");
	}
	opengl_window_title = lux::config->GetString("project.title") + " (OpenGL Rendering)";
	SDL_WM_SetCaption( opengl_window_title.c_str(), NULL );
	if ( opengl_window_icon != NULL )
	{
		SDL_SetAlpha(opengl_window_icon, SDL_SRCALPHA, 255);
		SDL_WM_SetIcon(opengl_window_icon, NULL);
		SDL_FreeSurface(opengl_window_icon);
	}

	opengl_graphic_camera = new glCamera((float)width/2, 400.0f, -400.0f);
	opengl_graphic_camera->Set2DView( opengl_graphic_dimension.w, opengl_graphic_dimension.h );


	glEnable( GL_BLEND );
	//glEnable(GL_DEPTH_TEST);
	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << __FILE__ << ": | Video Driver: OpenGL " << std::endl;
	lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << __FILE__ << ": | Vendor: " << glGetString(GL_VENDOR) << std::endl;
	lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << __FILE__ << ": | Renderer: " << glGetString(GL_RENDERER) << std::endl;
	lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << __FILE__ << ": | Version: " << glGetString(GL_VERSION) << std::endl;
	lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << __FILE__ << ": | Format: " << opengl_graphic_screen->w << "x" << opengl_graphic_screen->h << "x" << (int)opengl_graphic_screen->format->BitsPerPixel << "bpp" << std::endl;
	lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << __FILE__ << ": | Pointer Offset: " << opengl_graphic_ratio_width << "x" << opengl_graphic_ratio_height << std::endl;
	lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << __FILE__ << ": | GL_ARB_texture_non_power_of_two: " << Lux_OGL_QueryExtension((char *)"GL_ARB_texture_non_power_of_two") << std::endl;


	Lux_GLES_LoadFont();
	OpenGLShader::Check();

	SDL_WM_GrabInput(SDL_GRAB_OFF);
	//SDL_ShowCursor(false);

	opengl_graphic_cursor = new LuxPolygon(3);

	opengl_graphic_cursor->SetPoint(0, 0, 0);
	opengl_graphic_cursor->SetPoint(1, 8, 8);
	opengl_graphic_cursor->SetPoint(2, 0, 12);

	return true;
}

/* Lux_OGL_Destory
 * Closes down the video mode.
 */
LUX_DISPLAY_FUNCTION void Lux_OGL_Destory()
{
	/* TODO
	-
	*/
	Lux_GLES_UnloadFont();
	OpenGLShader::Free();
	if ( SDL_WasInit(SDL_INIT_VIDEO) == SDL_INIT_VIDEO )
	{
		//SDL_QuitSubSystem(SDL_INIT_VIDEO);
	}

	delete opengl_graphic_cursor;

}

/* Lux_OGL_Display2Screen
 * Converts display location to screen location
 @ x:
 @ y:
 */
LUX_DISPLAY_FUNCTION void Lux_OGL_Display2Screen( int32_t * x, int32_t * y )
{
	*x = (int32_t)((float)*x * opengl_graphic_ratio_width);
	*y = (int32_t)((float)*y * opengl_graphic_ratio_height);
}

/* Lux_OGL_BackgroundObject
 * Set the background objects
 @ background:
 */
LUX_DISPLAY_FUNCTION void Lux_OGL_BackgroundObject( MapObject background )
{
	/* TODO
	- Draw background colour
	*/
	opengl_graphics_colour = background.effects.primary_colour;
	glClearColor((float)opengl_graphics_colour.r / 255.0f, (float)opengl_graphics_colour.g / 255.0f, (float)opengl_graphics_colour.b / 255.0f, 1.0f);
}

/* Lux_OGL_Update
 * Adds an area of the screen that needs to be updated.
 @ rect: area to updates
 */
LUX_DISPLAY_FUNCTION void Lux_OGL_Update(LuxRect rect)
{

}

/* Lux_OGL_Show
 * Refreshs the display
 */
LUX_DISPLAY_FUNCTION void Lux_OGL_Show()
{
	SDL_GL_SwapBuffers();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	SDL_WM_GrabInput(SDL_GRAB_OFF);

	/* Display FPS */
	/*
	if ( lux::core->GetTime() >q(opengl_graphics_fpstime + 1000) )
	{
		std::stringstream title_msg;
		title_msg << opengl_window_title << " " << opengl_graphics_fps;
		opengl_graphics_fpstime = lux::core->GetTime();
		opengl_graphics_fps = 0;
		SDL_WM_SetCaption( title_msg.str().c_str(), NULL);
	}
	opengl_graphics_fps++;
	*/
}

/* Lux_OGL_DisplayPointer
 *
 */
LUX_DISPLAY_FUNCTION void Lux_OGL_DisplayPointer( uint8_t player, int16_t x, int16_t y, ObjectEffect effect )
{
	LuxRect position;

	position.x = x;
	position.y = y;

	Lux_GLES_DrawPolygon(opengl_graphic_cursor->x, opengl_graphic_cursor->y, opengl_graphic_cursor->count, position, effect, NULL);
}

/* Resource Functions */
/* Lux_OGL_FreeSprite
 *
 @ sprite:
 -
 */
LUX_DISPLAY_FUNCTION bool Lux_OGL_FreeSprite( LuxSprite * sprite )
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


/* Lux_OGL_CreateSprite
 *
 @ sprite:
 @ rect:
 @ parent:
 -
 */
LUX_DISPLAY_FUNCTION bool Lux_OGL_CreateSprite( LuxSprite * sprite, LuxRect rect, elix::Image * png )
{
	if ( !png->HasContent() )
	{
		lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR) << "PNG is empty" << std::endl;
		return false;
	}

	Texture * texture = new Texture;
	texture->w = sprite->sheet_area.w;
	texture->h = sprite->sheet_area.h;

	if ( Lux_OGL_QueryExtension((char *)"GL_ARB_texture_non_power_of_two") )
	{
		texture->tw = texture->w;
		texture->th = texture->h;
	}
	else
	{
		texture->tw = lux::OpenGLTexture::power(texture->w);
		texture->th = lux::OpenGLTexture::power(texture->h);
	}
	texture->pot = ( texture->w == texture->tw && texture->h == texture->th );

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
	else
	{
		lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR) << "PNG is empty" << std::endl;
	}
	delete pixels;
	/* ^ PNG Image */


	return true;
}

/* Lux_OGL_LoadSpriteSheet
 *
 @ name:
 @ children:
 -
 */
LUX_DISPLAY_FUNCTION bool Lux_OGL_LoadSpriteSheet( std::string name, std::map<uint32_t, LuxSprite *> * children )
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
				Lux_OGL_CreateSprite( p->second, p->second->sheet_area, png );
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

/* Lux_OGL_LoadSpriteSheet
 *
 @ name:
 @ children:
 -
 */
LUX_DISPLAY_FUNCTION bool Lux_OGL_LoadSpriteSheetImage( elix::Image * image, std::map<uint32_t, LuxSprite *> * children)
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
				Lux_OGL_CreateSprite( p->second, p->second->sheet_area, image );
			}
		}
		return true;
	}
	else
	{
		return false;
	}

}

/* Lux_OGL_FreeSpriteSheet
 *
 @ children:
 -
 */
LUX_DISPLAY_FUNCTION bool Lux_OGL_FreeSpriteSheet( std::map<uint32_t, LuxSprite *> * children )
{
	std::map<uint32_t, LuxSprite *>::iterator p;
	for( p = children->begin(); p != children->end(); p++ )
	{
		if ( !p->second->animated )
		{
			Lux_OGL_FreeSprite( p->second );
		}
	}
	return true;
}

/* Lux_OGL_RefreshSpriteSheet
 *
 @ name:
 @ children:
 -
 */
LUX_DISPLAY_FUNCTION bool Lux_OGL_RefreshSpriteSheet( std::string name, std::map<uint32_t, LuxSprite *> * children )
{
	if ( Lux_OGL_FreeSpriteSheet( children ) )
		Lux_OGL_LoadSpriteSheet( name, children );
	return true;
}


/* Lux_OGL_PNGtoSprite
 *
 @ data:
 @ size:
 -
 */
LUX_DISPLAY_FUNCTION LuxSprite * Lux_OGL_PNGtoSprite( uint8_t * data, uint32_t size )
{
	LuxSprite * sprite = NULL;
	elix::Image * png = new elix::Image(data, size);

	if ( png->HasContent() )
	{
		sprite = new LuxSprite(GraphicsOpenGL);
		sprite->sheet_area.x = sprite->sheet_area.y = 0;
		sprite->sheet_area.w = png->Width();
		sprite->sheet_area.h = png->Height();

		Lux_OGL_CreateSprite( sprite, sprite->sheet_area, png );
	}
	delete png;
	return sprite;

}

/* Lux_OGL_SetRotation
 *
 @ roll:
 @ pitch:
 @ yaw:
 -
 */

LUX_DISPLAY_FUNCTION void Lux_OGL_SetRotation( int16_t roll, int16_t pitch, int16_t yaw )
{
	if ( roll || pitch || yaw )
	{
		opengl_graphic_camera->Set3DView(opengl_graphic_dimension.w, opengl_graphic_dimension.h);
	}
	else
	{
		opengl_graphic_camera->Set2DView(opengl_graphic_dimension.w, opengl_graphic_dimension.h);
	}
}


