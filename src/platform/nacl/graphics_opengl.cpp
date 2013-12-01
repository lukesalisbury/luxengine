/****************************
Copyright © 2006-2011 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifdef OPENGLENABLED

/* Required Headers */
#include "core.h"
#include "config.h"
#include "engine.h"
#include "display.h"
#include "misc_functions.h"
#include "mokoi_game.h"
#include "map_object.h"
#include "platform_functions.h"

#include <math.h>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include "camera_opengl.h"
#include "bitfont.h"

#define RADTODEG(x) (x * 57.29578);
#define DEGTORAD(x) (x * 0.0174533);

GLuint font[96];

/* Global Variables */
struct Texture {
	GLuint texnum;
	GLint w, h, tw, th;
	bool pot;
};

Uint32 oglGraphics_flags = 0;
LuxColour oglGraphics_colour = { 0, 0, 0, 255 };
SDL_Rect oglGraphics_dimension = {0, 0, 320, 240 };
float oglGraphics_ratio_width = 1.00;
float oglGraphics_ratio_height = 1.00;
SDL_Surface * oglGraphics_screen = NULL;
glCamera * camera;
bool oglUseNPOT = true;
std::string oglGraphics_title;
uint32_t oglGraphics_fps = 0, oglGraphics_fpstime = 0;
bool oglGraphics_customtext = false;
int32_t oglGraphics_customtext_height = 32;

/* local Functions */
int Lux_OGL_PowerOfTwo( int value )
{
	int result = 1 ;
	while ( result < value )
		result *= 2 ;
	return result;
}

bool Lux_OGL_QueryExtension(char *extName)
{
	char *p = (char *) glGetString(GL_EXTENSIONS);
	char *end = p + strlen(p);
	while (p < end) {
		unsigned int n = strcspn(p, " ");
		if ( ( strlen(extName) == n) && ( strncmp(extName,p,n) == 0) )
		{
			return true;
		}
		p += (n + 1);
	}
	return false;
}

SDL_Surface * Lux_OGL_ScreenSuface( Texture * texture )
{
	SDL_Surface * screen = SDL_CreateRGBSurface(SDL_SWSURFACE, texture->tw, texture->th, 32, RMASK, GMASK, BMASK, AMASK);

	if ( screen )
	{
		glGetTexImage( GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, screen->pixels );
	}
	
	return screen;
}
void Lux_OGL_ScreenSuface2( void * texture, std::string str )
{
	SDL_SaveBMP( Lux_OGL_ScreenSuface( (Texture*)texture ), str.c_str() );
}

void Lux_OGL_ScreenSuface3( uint8_t n )
{
	SDL_Surface * screen = SDL_CreateRGBSurface(SDL_SWSURFACE, 8, 8, 32, RMASK, GMASK, BMASK, AMASK);
	if ( screen )
	{
		glGetTexImage( GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, screen->pixels );
	}
	std::stringstream title_msg;
	title_msg << "temp/" << (int)n << ".bmp";
	SDL_SaveBMP( screen, title_msg.str().c_str() );
}

/*
void Lux_OGL_CreateFont()
{
	GLubyte * font_point = &gfxPrimitivesFontdata[0];
	font_point += (32*8);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	fontlist = glGenLists(223);
	uint8_t q = 7, i = 0;
	
	for ( uint8_t c = 0; c < 223; c++)
	{
		GLubyte * charflip = new GLubyte[8];
		for ( q = 7, i = 0; i < 8; i++, q-- )
			charflip[q] = font_point[i];
		glNewList(fontlist + c, GL_COMPILE);
		glBitmap(8, 8, 0.0, 8.0, 8.0, 0.0, charflip);
		glEndList();
		font_point +=8;
	}
}
*/

void Lux_OGL_CreateFont()
{
	GLubyte * font_point = &gfxPrimitivesFontdata[0];
	font_point += (32*8);
	uint8_t i = 0, q;
	glEnable(GL_TEXTURE_2D);
	for ( uint8_t c = 0; c < 96; c++)
	{
		GLubyte * charflip = new GLubyte[64];
		for (i = 0; i < 8; i++)
		{
			for (q = 0; q < 8; q++)
			{
				charflip[(i*8) + q] = (!!(font_point[i] & (1 << (8-q))))*255;
			}
		}
		glGenTextures(1, &font[c]);
		glBindTexture(GL_TEXTURE_2D, font[c]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 8, 8, 0, GL_ALPHA, GL_UNSIGNED_BYTE, (uint8_t*)charflip);
		font_point += 8;
	}
	glDisable(GL_TEXTURE_2D);
}

SDL_Surface * Lux_OGL_LoadSpriteImage(std::string file)
{
	uint8_t * data = NULL;
	uint32_t size;
	SDL_Surface * temp_surface = NULL;
	if ( lux::game )
	{
		size = lux::game->GetFile(file, &data, false);
		if ( size )
		{
			SDL_RWops * src = SDL_RWFromMem(data, size);
			if ( IMG_isPNG(src) )
			{
				temp_surface = IMG_Load_RW(src, 1);
				SDL_SetAlpha(temp_surface, 0, 255);
			}
			else
				std::cout << "not a png image" << std::endl;
		}
	}
	return temp_surface;
}

/* Functions */
bool Lux_OGL_Init( uint16_t width, uint16_t height, uint8_t bpp, bool fullscreen )
{
	if ( lux::config->GetString("display.mode") != "OpenGL" )
		return false;

	fullscreen = lux::config->GetBoolean("display.fullscreen");
	/* Set Init Flags */
	if ( fullscreen )
		oglGraphics_flags = SDL_OPENGL | SDL_HWSURFACE | SDL_FULLSCREEN;
	else
		oglGraphics_flags = SDL_OPENGL | SDL_HWSURFACE;

	int scaled_width = lux::config->GetNumber("display.width");
	int scaled_height = lux::config->GetNumber("display.height");

	if ( bpp == 16 )
	{
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 6);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
	}
	else if ( bpp >= 24 )
	{
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	}
	else
	{
		bpp = 15;
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
	}
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, bpp );
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

	if ( SDL_WasInit(SDL_INIT_VIDEO) == SDL_INIT_VIDEO )
	{
		SDL_QuitSubSystem(SDL_INIT_VIDEO);
	}
	SDL_InitSubSystem(SDL_INIT_VIDEO);

	oglGraphics_screen = SDL_SetVideoMode(scaled_width, scaled_height, bpp, oglGraphics_flags );
	if ( !oglGraphics_screen )
	{
		std::cout << __FILE__ << ":" << __LINE__ << " | Couldn't set " << scaled_width << "x" << scaled_height << " video mode. " << SDL_GetError() << std::endl;
		return false;
	}
	oglGraphics_dimension.w = width;
	oglGraphics_dimension.h = height;

	oglGraphics_ratio_width = (float)width/(float)oglGraphics_screen->w;
	oglGraphics_ratio_height = (float)height/(float)oglGraphics_screen->h;

	SDL_Surface * icon = Lux_OGL_LoadSpriteImage("./sprites/__icon.png");
	oglGraphics_title = lux::config->GetString("project.title") + " (OpenGL)";
	SDL_WM_SetCaption( oglGraphics_title.c_str(), NULL );
	if ( icon != NULL )
	{
		SDL_WM_SetIcon(icon, NULL);
		SDL_FreeSurface(icon);
	}


	camera = new glCamera((float)width/2, 400.0f, -400.0f);
	camera->Set2DView( oglGraphics_dimension.w, oglGraphics_dimension.h );

	Lux_OGL_CreateFont();

	glEnable( GL_BLEND );
	//glEnable(GL_DEPTH_TEST);
	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	std::cout << __FILE__ << ":" << __LINE__ << " | Video Driver: OpenGL " << std::endl;
	std::cout << __FILE__ << ":" << __LINE__ << " | Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << __FILE__ << ":" << __LINE__ << " | Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << __FILE__ << ":" << __LINE__ << " | Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << __FILE__ << ":" << __LINE__ << " | Format: " << oglGraphics_screen->w << "x" << oglGraphics_screen->h << "x" << (int)oglGraphics_screen->format->BitsPerPixel << "bpp" << std::endl;
	std::cout << __FILE__ << ":" << __LINE__ << " | Pointer Offset: " << oglGraphics_ratio_width << "x" << oglGraphics_ratio_height << std::endl;
	std::cout << __FILE__ << ":" << __LINE__ << " | GL_ARB_texture_non_power_of_two: " << Lux_OGL_QueryExtension("GL_ARB_texture_non_power_of_two") << std::endl;


	SDL_WM_GrabInput(SDL_GRAB_OFF);
	return true;
}

void Lux_OGL_Destory()
{
	/* TODO
	-
	*/
	for ( uint8_t c = 0; c < 96; c++)
	{
		glDeleteTextures(1, &font[c]);
	}

	SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void Lux_OGL_Display2Screen( int32_t * x, int32_t * y )
{
	*x = (int32_t)((float)*x * oglGraphics_ratio_width);
	*y = (int32_t)((float)*y * oglGraphics_ratio_height);
}


void Lux_OGL_BackgroundObject( MapObject background )
{
	/* TODO
	- Draw background colour
	*/
	oglGraphics_colour = background.effects.colour;
	glClearColor((float)oglGraphics_colour.r / 255.0f, (float)oglGraphics_colour.g / 255.0f, (float)oglGraphics_colour.b / 255.0f, 1.0f);
}

void Lux_OGL_Update(LuxRect rect)
{

}

void Lux_OGL_Show()
{
	SDL_GL_SwapBuffers();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	SDL_WM_GrabInput(SDL_GRAB_OFF);

	if ( lux::core->GetTime() > (oglGraphics_fpstime + 1000) )
	{
		std::stringstream title_msg;
		title_msg << oglGraphics_title << " " << oglGraphics_fps;
		oglGraphics_fpstime = lux::core->GetTime();
		oglGraphics_fps = 0;
		SDL_WM_SetCaption( title_msg.str().c_str(), NULL);
	}
	oglGraphics_fps++;
}

bool Lux_OGL_LoadSpriteSheet(std::string name, std::map<std::string, LuxSprite *> * children)
{
	SDL_Surface * parent_sheet = Lux_OGL_LoadSpriteImage("./sprites/" + name);

	if ( !parent_sheet )
	{
		return false;
	}
	
	int w = 0, h = 0;
	std::map<std::string, LuxSprite *>::iterator p;
	glEnable(GL_TEXTURE_2D);
	for( p = children->begin(); p != children->end(); p++ )
	{
		if ( !p->second->animated )
		{
			if ( Lux_OGL_QueryExtension("GL_ARB_texture_non_power_of_two") )
			{
				w = p->second->rect.w;
				h = p->second->rect.h;
			}
			else
			{
				w = Lux_OGL_PowerOfTwo( p->second->rect.w );
				h = Lux_OGL_PowerOfTwo( p->second->rect.h );
			}

			SDL_Rect tmp_rect;
			tmp_rect.x = p->second->rect.x;
			tmp_rect.y = p->second->rect.y;
			tmp_rect.w = p->second->rect.w;
			tmp_rect.h = p->second->rect.h;
			SDL_Surface * temp_sheet = SDL_CreateRGBSurface ( oglGraphics_flags, w, h, 32, RMASK, GMASK, BMASK, AMASK );
			SDL_BlitSurface(parent_sheet, &tmp_rect, temp_sheet, NULL);
			SDL_SetAlpha(temp_sheet, 0, 0);
			if ( temp_sheet )
			{
				Texture * temp_texure = new Texture;
				glGenTextures(1, &temp_texure->texnum);
				glBindTexture(GL_TEXTURE_2D, temp_texure->texnum);
				temp_texure->tw = temp_sheet->w;
				temp_texure->th = temp_sheet->h;
				temp_texure->w = tmp_rect.w;
				temp_texure->h = tmp_rect.h;
				temp_texure->pot = ( temp_texure->tw == temp_texure->w && temp_texure->th == temp_texure->h) ? true : false;
				glTexImage2D(GL_TEXTURE_2D, 0, 4, temp_sheet->w, temp_sheet->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, temp_sheet->pixels);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
				p->second->data = (void*) temp_texure;
			}
			SDL_FreeSurface(temp_sheet);
		}
		
	}
	glDisable(GL_TEXTURE_2D);
	//delete parent_sheet;
	SDL_FreeSurface(parent_sheet);
	return true;
}

void Lux_OGL_FreeSpriteSheet(std::map<std::string, LuxSprite *> * children)
{
	std::map<std::string, LuxSprite *>::iterator p;
	for( p = children->begin(); p != children->end(); p++ )
	{
		if (!p->second->animated)
		{
			glDeleteTextures(1, &((Texture*)p->second->data)->texnum);
			delete (Texture*)p->second->data;
		}
	}
}

LuxSprite * Lux_OGL_PNGtoSprite( uint8_t * data, uint32_t size )
{
	LuxSprite * sprite = new LuxSprite;
	SDL_RWops * src = SDL_RWFromMem(data, size);
	if ( IMG_isPNG(src) )
	{
		SDL_Surface * surface = IMG_Load_RW(src, 0);
		if ( surface )
		{
			SDL_SetAlpha(surface, 0, 255);

			Texture * temp_texure = new Texture;
			glGenTextures(1, &temp_texure->texnum);
			glBindTexture(GL_TEXTURE_2D, temp_texure->texnum);

			if ( Lux_OGL_QueryExtension("GL_ARB_texture_non_power_of_two") )
			{
				temp_texure->tw = surface->w;
				temp_texure->th = surface->h;
			}
			else
			{
				temp_texure->tw = Lux_OGL_PowerOfTwo( surface->w );
				temp_texure->th = Lux_OGL_PowerOfTwo( surface->h );
			}
			temp_texure->w = surface->w;
			temp_texure->h = surface->h;
			temp_texure->pot = ( temp_texure->tw == temp_texure->w && temp_texure->th == temp_texure->h) ? true : false;
			
			SDL_Surface * temp_sheet = SDL_CreateRGBSurface( oglGraphics_flags, temp_texure->tw, temp_texure->th, 32, RMASK, GMASK, BMASK, AMASK );
			SDL_BlitSurface( surface, NULL, temp_sheet, NULL );
			SDL_SetAlpha(temp_sheet, 0, 0);

			glTexImage2D(GL_TEXTURE_2D, 0, 4, temp_sheet->w, temp_sheet->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, temp_sheet->pixels);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
			sprite->data = (void*) temp_texure;
			SDL_FreeSurface(temp_sheet);
			SDL_FreeSurface(surface);

			//Lux_OGL_ScreenSuface( (Texture*)sprite->data );
			return sprite;
		}
		else
		{
			std::cerr << "Lux_OGL_PNGtoSprite failed" << std::endl;
		}
	}
	delete sprite;
	return NULL;
}

bool Lux_OGL_FreeSprite( LuxSprite * sprite )
{
	glDeleteTextures(1, &((Texture*)sprite->data)->texnum);
	delete (Texture*)sprite->data;
	sprite->data = NULL;
	return false;
}

/*
	12
	34
*/
inline void Lux_OGL_ColouredVertex3(LuxColour c1, LuxColour c2, int8_t s, int8_t p, int32_t x, int32_t y, int32_t z)
{
	if ( s == STYLE_VGRADIENT && (p > 2) )
		glColor4ub(c2.r, c2.g, c2.b, c2.a);
	else if ( s == STYLE_HGRADIENT && (p == 2 || p == 3) )
		glColor4ub(c2.r, c2.g, c2.b, c2.a);
	else
		glColor4ub(c1.r, c1.g, c1.b, c1.a);

	glVertex3i(x, y, z);
}

void Lux_OGL_DrawRect( LuxRect dest_rect, Effects image_effects)
{
	if (!oglGraphics_screen)
	{
		std::cout << __FILE__ << ":" << __LINE__ << " | Not a valid surface." << std::endl;
	}
	int32_t z = dest_rect.z/1000;
	glDisable(GL_TEXTURE_2D);

	glBegin(GL_QUADS);
		Lux_OGL_ColouredVertex3(image_effects.colour, image_effects.colour2, image_effects.style, 1, dest_rect.x, dest_rect.y, z);
		Lux_OGL_ColouredVertex3(image_effects.colour, image_effects.colour2, image_effects.style, 2, dest_rect.x + dest_rect.w, dest_rect.y, z);
		Lux_OGL_ColouredVertex3(image_effects.colour, image_effects.colour2, image_effects.style, 3, dest_rect.x + dest_rect.w, dest_rect.y + dest_rect.h, z);
		Lux_OGL_ColouredVertex3(image_effects.colour, image_effects.colour2, image_effects.style, 4, dest_rect.x, dest_rect.y + dest_rect.h, z);
	glEnd();
	glEnable(GL_TEXTURE_2D);
}

int32_t Lux_OGL_DrawChar( int32_t cchar, int32_t x, int32_t y, int32_t z, Effects image_effects)
{
	std::stringstream stream;
	stream << (int)cchar;
	LuxSprite * sdata = lux::display->GetSprite( lux::display->sprite_font, stream.str() );
	if ( !sdata )
		return 0;

	Texture * surface = (Texture*)sdata->GetData(image_effects);
	if ( surface )
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, surface->texnum);
		glBegin(GL_QUADS);
			glTexCoord2f( 0.0, 0.0 );
			Lux_OGL_ColouredVertex3(image_effects.colour, image_effects.colour2, image_effects.style, 1, x, y, z);
			glTexCoord2f( 1.0, 0.0 );
			Lux_OGL_ColouredVertex3(image_effects.colour, image_effects.colour2, image_effects.style, 2, x + surface->w, y, z);
			glTexCoord2f( 1.0, 1.0 );
			Lux_OGL_ColouredVertex3(image_effects.colour, image_effects.colour2, image_effects.style, 3, x + surface->w, y + surface->h, z);
			glTexCoord2f( 0.0, 1.0 );
			Lux_OGL_ColouredVertex3(image_effects.colour, image_effects.colour2, image_effects.style, 4, x, y + surface->h, z );
		glEnd();
		glDisable(GL_TEXTURE_2D);
		return surface->w;
	}
	return 0;
}

void Lux_OGL_DrawText( std::string text, LuxRect dest_rect, Effects image_effects, bool allow_custom )
{
	//glPushMatrix();
	int32_t z = dest_rect.z/1000;
	glColor4ub(image_effects.colour.r, image_effects.colour.g, image_effects.colour.b, image_effects.colour.a);
	uint16_t x = dest_rect.x;
	std::string::iterator object;
	for ( object = text.begin(); object != text.end(); object++ )
	{
		uint32_t cchar = *object;
		/*
		194-223 2 bytes
		224-239 3 bytes
		240-244 4 bytes
		*/
		if (cchar == '\n' || cchar == '\r')
		{
			dest_rect.y += ( oglGraphics_customtext && allow_custom ? oglGraphics_customtext_height + 2 : 10);
			x = dest_rect.x;
		}
		else if ( cchar <= 32 )
		{
			x += 7;
		}
		else if ( cchar <= 128 )
		{
			if ( oglGraphics_customtext && allow_custom )
			{
				x += Lux_OGL_DrawChar((int32_t)cchar,  x, dest_rect.y, z, image_effects);
			}
			else
			{
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, font[cchar-32]);
				glBegin(GL_QUADS);
					glTexCoord2f( 0.0, 0.0 );
					Lux_OGL_ColouredVertex3(image_effects.colour, image_effects.colour2, image_effects.style, 1, x, dest_rect.y, z);
					glTexCoord2f( 1.0, 0.0 );
					Lux_OGL_ColouredVertex3(image_effects.colour, image_effects.colour2, image_effects.style, 2, x + 8, dest_rect.y, z);
					glTexCoord2f( 1.0, 1.0 );
					Lux_OGL_ColouredVertex3(image_effects.colour, image_effects.colour2, image_effects.style, 3, x + 8, dest_rect.y + 8, z);
					glTexCoord2f( 0.0, 1.0 );
					Lux_OGL_ColouredVertex3(image_effects.colour, image_effects.colour2, image_effects.style, 4, x, dest_rect.y + 8, z );
				glEnd();
				glDisable(GL_TEXTURE_2D);
				x += 7;
			}
		}
		else
		{
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, font['?'-32]);
			glBegin(GL_QUADS);
				glTexCoord2f( 0.0, 0.0 );
				Lux_OGL_ColouredVertex3(image_effects.colour, image_effects.colour2, image_effects.style, 1, x, dest_rect.y, z);
				glTexCoord2f( 1.0, 0.0 );
				Lux_OGL_ColouredVertex3(image_effects.colour, image_effects.colour2, image_effects.style, 2, x + 8, dest_rect.y, z);
				glTexCoord2f( 1.0, 1.0 );
				Lux_OGL_ColouredVertex3(image_effects.colour, image_effects.colour2, image_effects.style, 3, x + 8, dest_rect.y + 8, z);
				glTexCoord2f( 0.0, 1.0 );
				Lux_OGL_ColouredVertex3(image_effects.colour, image_effects.colour2, image_effects.style, 4, x, dest_rect.y + 8, z );
			glEnd();
			glDisable(GL_TEXTURE_2D);
			x += 7;
			if ( cchar < 224 )
				object++;
			else if ( cchar < 240 )
				object +=2;
			else if ( cchar < 245 )
				object +=3;
		}

	}
	glColor4f(1.0,1.0,1.0,1.0);
	//glPopMatrix();
}

void Lux_OGL_DrawSprite(LuxSprite * sprite, LuxRect dest_rect, Effects image_effects)
{
	if ( !oglGraphics_screen )
	{
		std::cout << __FILE__ << ":" << __LINE__ << " | Not a valid surface." << std::endl;
		return;
	}
	if ( data == NULL )
		return;

	Texture * surface = ((Texture*)data);
	uint32_t width, height;
	float tx1, tx2, tx3, tx4, ty1, ty2, ty3, ty4;
	float x_tile = 1.00, y_tile = 1.00, x_init = 0.00, y_init = 0.00;
	bool switch_axis = false, mirror = false, tile = false;
	int32_t z = dest_rect.z/1000;
	if ( image_effects.flipmode&16 )
	{
		image_effects.flipmode -= 16;
		mirror = true;
	}
	if ( dest_rect.w == 0 )
		dest_rect.w = surface->w;
	if ( dest_rect.h == 0 )
		dest_rect.h = surface->h;


	if ( image_effects.flipmode == 1 || image_effects.flipmode == 3 )
	{
		width = MAKE_FIXED_INT(image_effects.scale_w * surface->h );
		height = MAKE_FIXED_INT(image_effects.scale_h * surface->w );
//		width = surface->h;
//		height = surface->w;
		switch_axis = true;
	}
	else
	{
		width = MAKE_FIXED_INT(image_effects.scale_w * surface->w );
		height = MAKE_FIXED_INT(image_effects.scale_h * surface->h );
//		width = surface->w;
//		height = surface->h;
	}

	if ( image_effects.scale_w != 1000 )
		dest_rect.w = MAKE_FIXED_INT(image_effects.scale_w * dest_rect.w );

	if ( image_effects.scale_h != 1000 )
		dest_rect.h = MAKE_FIXED_INT(image_effects.scale_h * dest_rect.h );


	glPushMatrix();
	if ( dest_rect.w != width )
	{
		if ( mirror )
			x_init = ( (float)(switch_axis ? dest_rect.h : dest_rect.w) / (float)width );
		else
			x_tile = ( (float)(switch_axis ? dest_rect.h : dest_rect.w) / (float)width );
		tile = true;
	}
	
	if ( dest_rect.h != height )
	{
		y_tile = ( (float)(!switch_axis ? dest_rect.h : dest_rect.w)  / (float)height );
		tile = true;
	}

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, surface->texnum);
	glColor4ub(image_effects.colour.r, image_effects.colour.g, image_effects.colour.b, image_effects.colour.a);

	if ( image_effects.rotate )
	{
		glTranslatef(dest_rect.x + (dest_rect.w / 2), dest_rect.y + (dest_rect.h / 2), 0);
		glRotatef(image_effects.rotate, 0, 0, 1);
		glTranslatef(-(dest_rect.w / 2), -(dest_rect.h / 2), 0.0f);
		dest_rect.x = 0;
		dest_rect.y = 0;
	}

	if ( !surface->pot )
	{
		x_tile = y_tile = 1.00;
	}

	switch ( image_effects.flipmode )
	{
		case 1:
		{
			ty3 = ty2 = ( mirror ? x_tile : x_init );
			ty1 = ty4 = ( mirror ? x_init : x_tile );
			tx1 = tx2 = y_init;
			tx4 = tx3 = y_tile;
			break;
		}
		case 2:
		{
			tx1 = tx4 = ( mirror ? x_init : x_tile );
			tx3 = tx2 = ( mirror ? x_tile : x_init );
			ty1 = ty2 = y_tile;
			ty3 = ty4 = y_init;
			break;
		}
		case 3:
		{
			ty3 = ty2 = ( mirror ? x_init : x_tile );
			ty1 = ty4 = ( mirror ? x_tile : x_init );
			tx1 = tx2 = y_tile;
			tx4 = tx3 = y_init;
			break;
		}
		default:
		{
			tx1 = tx4 = ( mirror ? x_tile : x_init );
			tx2 = tx3 = ( mirror ? x_init : x_tile );
			ty1 = ty2 = y_init;
			ty3 = ty4 = y_tile;
			break;
		}
	}
	
	if ( !surface->pot )
	{
		GLuint index = glGenLists(1);

		glNewList(index, GL_COMPILE);
			glBegin(GL_QUADS);
				glTexCoord2f( tx1, ty1 );
				glVertex3i(0, 0, z);
				glTexCoord2f( tx2, ty2 );
				glVertex3i( (switch_axis ? surface->th : surface->tw), 0, z);
				glTexCoord2f( tx3, ty3 );
				glVertex3i((switch_axis ? surface->th : surface->tw), (!switch_axis ? surface->th : surface->tw), z);
				glTexCoord2f( tx4, ty4 );
				glVertex3i(0, (!switch_axis ? surface->th : surface->tw), z);
			glEnd();
		glEndList();

		if ( image_effects.flipmode == 2 )
		{
			dest_rect.x -= ( surface->tw - surface->w );
			dest_rect.y -= ( surface->th - surface->h );
		}
		else if ( image_effects.flipmode == 1 )
			dest_rect.x -= ( surface->th - surface->h );
		else if ( image_effects.flipmode == 3 )
			dest_rect.y -= ( surface->tw - surface->w );
		
		if ( tile )
		{
			int32_t yheight = 0;
			float fx, fy;
			while ( yheight < (!switch_axis ? dest_rect.h : dest_rect.w)  )
			{
				int32_t xwidth = 0;
				while ( xwidth < (switch_axis ? dest_rect.h : dest_rect.w) )
				{
					fx = (dest_rect.x + xwidth);
					fy = (dest_rect.y + yheight);
					glTranslatef(fx, fy, 0);
					glCallList(index);
					glTranslatef(-fx,-fy, 0);
					xwidth += width;
				}
				yheight += height;
			}
		}
		else
		{
			glTranslatef(dest_rect.x, dest_rect.y, 0);
			glCallList(index);
			glTranslatef(-dest_rect.x, -dest_rect.y, 0);
		}
		glDeleteLists(index, 1);
	}
	else
	{
		glBegin(GL_QUADS);
			glTexCoord2f( tx1, ty1 );
			glVertex3i(dest_rect.x, dest_rect.y, z);
			glTexCoord2f( tx2, ty2 );
			glVertex3i(dest_rect.x + (switch_axis ? dest_rect.h : dest_rect.w), dest_rect.y, z);
			glTexCoord2f( tx3, ty3 );
			glVertex3i(dest_rect.x + (switch_axis ? dest_rect.h : dest_rect.w), dest_rect.y + (!switch_axis ? dest_rect.h : dest_rect.w), z);
			glTexCoord2f( tx4, ty4 );
			glVertex3i(dest_rect.x, dest_rect.y + (!switch_axis ? dest_rect.h : dest_rect.w), z);
		glEnd();
	}
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

void Lux_OGL_DrawLine( LuxRect points, Effects image_effects )
{
	int32_t z = points.z/1000;
	glPushMatrix();
	glDisable(GL_TEXTURE_2D);

	glColor4ub(image_effects.colour.r, image_effects.colour.g, image_effects.colour.b, image_effects.colour.a);
	glLineWidth(1.00f);
	glBegin(GL_LINES);
		glVertex3i(points.x, points.y, z);
		glVertex3i(points.w, points.h, z);
	glEnd();
	glEnable(GL_TEXTURE_2D);
	glPopMatrix();
}

void Lux_OGL_DrawPolygon( int16_t * x_point, int16_t *y_point, uint16_t point_count, Effects image_effects, void * texture )
{
	//glPushMatrix();
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	/*
	glColor4ub(image_effects.colour.r, image_effects.colour.g, image_effects.colour.b, image_effects.colour.a);
	glVertex3i(dest_rect.x, dest_rect.y, dest_rect.z);
	glVertex3i(dest_rect.x + dest_rect.w, dest_rect.y, dest_rect.z);
	glVertex3i(dest_rect.x + dest_rect.w, dest_rect.y + dest_rect.h, dest_rect.z);
	glVertex3i(dest_rect.x, dest_rect.y + dest_rect.h, dest_rect.z);
	*/
	glEnd();
	//glEnable(GL_TEXTURE_2D);
}

void Lux_OGL_DrawCircle( LuxRect dest_rect, Effects image_effects )
{
	if (!oglGraphics_screen)
	{
		std::cout << __FILE__ << ":" << __LINE__ << " | Not a valid surface." << std::endl;
	}
	int32_t z = dest_rect.z/1000;
	float angle;
	float xradius = (dest_rect.w / 2);
	float yradius = (dest_rect.h / 2);
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_TRIANGLE_FAN);
		glColor4ub(image_effects.colour.r, image_effects.colour.g, image_effects.colour.b, image_effects.colour.a);
		for(uint8_t i = 0; i < 64; i++)
		{
			angle = i*2*M_PI/64;
			glVertex2f(dest_rect.x + (cos(angle) * xradius) + xradius, dest_rect.y + (sin(angle) * yradius) + yradius);
		}
	glEnd();
	glEnable(GL_TEXTURE_2D);
}

void Lux_OGL_SetRotation( int16_t roll, int16_t pitch, int16_t yaw )
{
	if ( roll || pitch || yaw )
	{
		camera->Set3DView(oglGraphics_dimension.w, oglGraphics_dimension.h);
	}
	else
	{
		camera->Set2DView(oglGraphics_dimension.w, oglGraphics_dimension.h);
	}
}

void Lux_OGL_TextSprites( bool able )
{
	if ( lux::display->sprite_font.length() )
	{
		oglGraphics_customtext = able;
		if ( oglGraphics_customtext )
		{
			LuxSprite * sdata = lux::display->GetSprite( lux::display->sprite_font, "48" );
			if ( sdata )
				oglGraphics_customtext_height = sdata->rect.h;
		}
	}
	else
		oglGraphics_customtext = false;
	
}

#endif
