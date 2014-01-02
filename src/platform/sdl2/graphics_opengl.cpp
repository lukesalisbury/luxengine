/****************************
Copyright (c) 2006-2012 Luke Salisbury
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

#include "platform_functions.h"

#define GL_GLEXT_PROTOTYPES 1



GraphicSystem GraphicsOpenGL = {
	&Lux_OGL_Init,
	&Lux_OGL_Destory,
	&Lux_OGL_Display2Screen,
	&Lux_GLES_TextSprites,
	&Lux_OGL_Update,
	&Lux_OGL_Show,

	&Lux_OGL_SetRotation,
	&Lux_OGL_BackgroundObject,
	&Lux_OGL_SetFullscreen,
	&Lux_OGL_Resize,
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

/* Shared */
extern Uint32 native_render_flags;
extern Uint32 native_window_flags;
extern SDL_Window * native_window;
extern SDL_Renderer * native_renderer;
extern std::string native_window_title;
extern SDL_Rect native_graphics_dimension;

/* Global Variables */
LuxColour gles_graphics_colour = { 0, 128, 0, 255 };

SDL_GLContext native_context;
float opengl_graphic_ratio_width = 1.00;
float opengl_graphic_ratio_height = 1.00;
LuxPolygon * opengl_graphic_cursor = NULL;
SDL_Rect native_screen_position;
bool native_screen_stretching = false;

/* Local Function */


/* Global Function */

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
	int window_width = width;
	int window_height = height;

	#ifndef OPENGLONLY
	if ( lux::config->GetString("display.mode") != "OpenGL" )
		return false;
	#endif

	/* Set Init Flags */
	native_window_flags = SDL_WINDOW_SHOWN|SDL_WINDOW_OPENGL;

	if ( fullscreen )
		native_window_flags |= SDL_WINDOW_FULLSCREEN;

	native_render_flags = SDL_RENDERER_ACCELERATED;
	native_window_title = lux::config->GetString("project.title") + " (OpenGL Rendering)";


	if ( width > height )
	{
		SDL_SetHintWithPriority( SDL_HINT_ORIENTATIONS, "LandscapeLeft", SDL_HINT_OVERRIDE );
	}

	native_window = lux::core->GetWindow();

	SDL_SetWindowSize(native_window, width, height );
	SDL_GetWindowSize(native_window, &window_width, &window_height);


	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_SetHintWithPriority( SDL_HINT_RENDER_OPENGL_SHADERS, "1", SDL_HINT_OVERRIDE );


	native_context = SDL_GL_CreateContext(native_window);
	if ( !native_context )
	{
		lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << __FILE__ << ":" << __LINE__ << " | Couldn't create Renderer. " << SDL_GetError() << std::endl;
		return false;
	}

	SDL_GL_MakeCurrent(native_window, native_context);



	lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << __FILE__ << ": | Video Driver: OpenGL " << std::endl;
	lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << __FILE__ << ": | Vendor: " << glGetString(GL_VENDOR) << std::endl;
	lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << __FILE__ << ": | Renderer: " << glGetString(GL_RENDERER) << std::endl;
	lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << __FILE__ << ": | Version: " << glGetString(GL_VERSION) << std::endl;
	lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << __FILE__ << ": | GL_ARB_texture_non_power_of_two: " << SDL_GL_ExtensionSupported((char *)"GL_ARB_texture_non_power_of_two") << std::endl;

	native_graphics_dimension.w = width;
	native_graphics_dimension.h = height;

	Lux_OGL_Resize( window_width, window_height);


	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0d, (double)width, (double)height, 0, -10.0d, 10.0d);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST );
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	OpenGLShader::Check();
	Lux_GLES_LoadFont();


	glEnable(GL_TEXTURE_2D);
	glClearColor((float)gles_graphics_colour.r / 255.0f, (float)gles_graphics_colour.g / 255.0f, (float)gles_graphics_colour.b / 255.0f, 1.0f);


	opengl_graphic_cursor = new LuxPolygon(3);

	opengl_graphic_cursor->SetPoint(0, 0, 0);
	opengl_graphic_cursor->SetPoint(1, 8, 8);
	opengl_graphic_cursor->SetPoint(2, 0, 12);


	SDL_RenderSetViewport(native_renderer, &native_graphics_dimension);

	SDL_RenderSetLogicalSize(native_renderer, width, height);

	Lux_SDL2_SetWindowIcon( native_window );
	SDL_SetWindowTitle( native_window, native_window_title.c_str() );


	SDL_DisableScreenSaver();

	return true;
}

/* Lux_OGL_Destory
 * Closes down the video mode.
 */
LUX_DISPLAY_FUNCTION void Lux_OGL_Destory()
{
	Lux_GLES_UnloadFont();
	SDL_GL_DeleteContext(native_context);
	SDL_EnableScreenSaver();
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
	SDL_GL_SwapWindow(native_window);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/* Lux_OGL_Display2Screen
 * Converts display location to screen location
 @ x:
 @ y:
 */
LUX_DISPLAY_FUNCTION void Lux_OGL_Display2Screen( int32_t * x, int32_t * y)
{
	*x = (int32_t)((float)*x * opengl_graphic_ratio_width);
	*y = (int32_t)((float)*y * opengl_graphic_ratio_height);
}

/* Lux_OGL_Display2Screen
 * Converts display location to screen location
 @ width:
 @ height:
 */
LUX_DISPLAY_FUNCTION bool Lux_OGL_Resize( uint16_t width, uint16_t height)
{
	opengl_graphic_ratio_width = (float)native_graphics_dimension.w/(float)width;
	opengl_graphic_ratio_height = (float)native_graphics_dimension.h/(float)height;


	glViewport(0, 0, width, height);
	/*
	SDL_RenderSetViewport(native_renderer, &native_graphics_dimension);
	SDL_RenderSetLogicalSize(native_renderer, width, height);
	*/
	return false;
}

/* Lux_OGL_Display2Screen
 * Converts display location to screen location
 @ width:
 @ height:
 */
LUX_DISPLAY_FUNCTION bool Lux_OGL_SetFullscreen( bool able )
{

	SDL_SetWindowFullscreen( native_window, ( able ? SDL_WINDOW_FULLSCREEN : 0 ) );

	return true;
}



/* Lux_OGL_BackgroundObject
 * Set the background objects
 @ background:
 */
LUX_DISPLAY_FUNCTION void Lux_OGL_BackgroundObject( MapObject background  )
{
	gles_graphics_colour = background.effects.primary_colour;
	glClearColor((float)gles_graphics_colour.r / 255.0f, (float)gles_graphics_colour.g / 255.0f, (float)gles_graphics_colour.b / 255.0f, 1.0f);
}


/* Lux_OGL_DisplayPointer
 *
 */
LUX_DISPLAY_FUNCTION void Lux_OGL_DisplayPointer( uint8_t player, int16_t x, int16_t y, ObjectEffect effect )
{
	LuxRect position;

	position.x = x;
	position.y = y;
	position.z = 7000;

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
	/*  ^ PNG Image */


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
		sprite = new LuxSprite( GraphicsOpenGL );
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

}
