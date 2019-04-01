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

namespace colour {
	extern LuxColour white;
	extern LuxColour black;
	extern LuxColour red;
	extern LuxColour blue;
	extern LuxColour green;
}

#include <jni.h>
#include <android/log.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include "GLES/texture.inc.hpp"

#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,"luxengine",__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,"luxengine",__VA_ARGS__)
#ifndef LUX_DISPLAY_FUNCTION
	#define LUX_DISPLAY_FUNCTION
#endif

/* Shaders */
enum {
	SHADER_DEFAULT,
	SHADER_GLOW,
	SHADER_NEGATIVE,
	SHADER_REPLACE,
	NUM_SHADERS
};

typedef struct {
	GLuint program;
	GLuint vert_shader;
	GLuint frag_shader;
	const char * vert_source;
	const char * frag_source;
} ShaderData;

bool shaders_supported = false;

char * shadertext = nullptr;

GLuint gvPositionHandle;


static ShaderData shaders[NUM_SHADERS] = {
	/* SHADER_DEFAULT */
	{ 0, 0, 0,
	/* vertex shader */
	"attribute vec4 vPosition;\n"
	"void main()\n"
	"{\n"
	"	gl_Position = vPosition;\n"
	"}",
	/* fragment shader */
	"precision mediump float;\n"
	"void main()\n"
	"{\n"
	"	gl_FragColor = vec4 ( 1.0, 1.0, 1.0, 1.0 );\n"
	"}"
	},
	/* SHADER_GLOW */
	{ 0, 0, 0,
	/* vertex shader */
	"varying vec4 v_color;\n"
	"varying vec2 v_texCoord;\n"
	"\n"
	"void main()\n"
	"{\n"
	"	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
	"	v_color = gl_Color;\n"
	"	v_texCoord = vec2(gl_MultiTexCoord0);\n"
	"}",
	/* fragment shader */
	"varying vec4 v_color;\n"
	"varying vec2 v_texCoord;\n"
	"uniform sampler2D tex0;\n"
	"uniform vec4 FirstColour;\n"
	"uniform vec4 SecondColour;\n"
	"void main()\n"
	"{\n"
	"	vec4 color = texture2D(tex0, v_texCoord);\n"
	"	vec4 sum;\n"
	"	bool border = false;\n"
	"	if (color.a < 0.9)\n"
	"	{\n"
	"		sum = texture2D( tex0, abs(v_texCoord + vec2( 0.00, -0.05 )) );\n"
	"		if (sum.a >= 0.5)\n"
	"			border = true;\n"
	"		sum = texture2D( tex0, abs(v_texCoord + vec2( -0.05, 0.00 )) );\n"
	"		if (sum.a >= 0.5)\n"
	"			border = true;\n"
	"		sum = texture2D( tex0, abs(v_texCoord + vec2( 0.00, 0.05 )) );\n"
	"		if (sum.a >= 0.5)\n"
	"			border = true;\n"
	"		sum = texture2D( tex0, abs(v_texCoord + vec2( 0.05, 0.00 )) );\n"
	"		if (sum.a >= 0.5)\n"
	"			border = true;\n"
	"	}\n"
	"	if ( border )\n"
	"	{\n"
	"		gl_FragColor = SecondColour;\n"
	"	}\n"
	"	else\n"
	"	{\n"
	"		gl_FragColor = color;\n"
	"	}\n"
	"}"
	},
	/* SHADER_NEGATIVE */
	{ 0, 0, 0,
	/* vertex shader */
	"varying vec4 v_color;\n"
	"varying vec2 v_texCoord;\n"
	"\n"
	"void main()\n"
	"{\n"
	"	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
	"	v_color = gl_Color;\n"
	"	v_texCoord = vec2(gl_MultiTexCoord0);\n"
	"}",
	/* fragment shader */
	"varying vec4 v_color;\n"
	"varying vec2 v_texCoord;\n"
	"uniform sampler2D tex0;\n"
	"uniform vec4 FirstColour;\n"
	"uniform vec4 SecondColour;\n"
	"\n"
	"void main()\n"
	"{\n"
	"	vec4 color = texture2D(tex0, v_texCoord);\n"
	"	vec4 result;\n"
	"	result.r=1.0-color.r;\n"
	"	result.g=1.0-color.g;\n"
	"	result.b=1.0-color.b;\n"
	"	result.a=color.a;\n"
	"	gl_FragColor = result;\n"
	"}",
	},
	/* SHADER_REPLACE */
	{ 0, 0, 0,
	/* vertex shader */
	"varying vec4 v_color;\n"
	"varying vec2 v_texCoord;\n"
	"\n"
	"void main()\n"
	"{\n"
	"	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
	"	v_color = gl_Color;\n"
	"	v_texCoord = vec2(gl_MultiTexCoord0);\n"
	"}",
	/* fragment shader */
	"varying vec4 v_color;\n"
	"varying vec2 v_texCoord;\n"
	"uniform sampler2D tex0;\n"
	"uniform vec4 FirstColour;\n"
	"uniform vec4 SecondColour;\n"
	"\n"
	"void main()\n"
	"{\n"
	"	vec4 color = texture2D(tex0, v_texCoord);\n"
	"	if (FirstColour == color) {\n"
	"		gl_FragColor = SecondColour;\n"
	"	} else {\n"
	"		gl_FragColor = color;\n"
	"	}\n"
	"}"
	}
};

bool Lux_OGL_QueryExtension(char *extName);

static bool CompileShaderCode(GLenum shader, const char *source)
{
	GLint status;

	glShaderSource(shader, 1, &source, nullptr);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == 0)
	{
		GLint length;
		char *info = nullptr;

		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		info = new char(length+1);
		glGetShaderInfoLog(shader, length, nullptr, info);
		lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR) << "Failed to compile shader (" << source << "):" << info << std::endl;
		delete info;

	}
	return !!status;
}

void CompileShader( ShaderData * data )
{
	/* Create one program object to rule them all */
	data->program = glCreateProgram();

	/* Create the vertex shader */
	data->vert_shader = glCreateShader(GL_VERTEX_SHADER);
	if (!CompileShaderCode(data->vert_shader, data->vert_source))
	{
		shaders_supported = false;
		return;
	}

	/* Create the fragment shader */
	data->frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	if (!CompileShaderCode(data->frag_shader, data->frag_source))
	{
		shaders_supported = false;
		return;
	}

	/* ... and in the darkness bind them */
	glAttachShader(data->program, data->vert_shader);
	glAttachShader(data->program, data->frag_shader);
	glLinkProgram(data->program);


}

void setShaders()
{
	for ( uint8_t i = 0; i < NUM_SHADERS; ++i)
	{
		CompileShader( &shaders[i] );
	}
	gvPositionHandle = glGetAttribLocation(shaders[0].program, "vPosition");
}

void closeShader()
{
	for ( uint8_t i = 0; i < NUM_SHADERS; ++i)
	{
		glDeleteShader(shaders[i].vert_shader);
		glDeleteShader(shaders[i].frag_shader);
		glDeleteProgram(shaders[i].program);
	}
}


void enableShader( uint8_t shader, ObjectEffect effect )
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

	glUseProgram( shaders[SHADER_DEFAULT].program );
	if ( s < NUM_SHADERS && s != SHADER_DEFAULT )
	{
		glUseProgram( shaders[s].program );
		GLint loc1 = glGetAttribLocation(shaders[s].program, "FirstColour");
		GLint loc2 = glGetAttribLocation(shaders[s].program, "SecondColour");
		glUniform4f(loc1, (float)effect.primary_colour.r/255.0, (float)effect.primary_colour.g/255.0, (float)effect.primary_colour.b/255.0, (float)effect.primary_colour.a/255.0 );
		glUniform4f(loc2, (float)effect.secondary_colour.r/255.0, (float)effect.secondary_colour.g/255.0, (float)effect.secondary_colour.b/255.0, (float)effect.secondary_colour.a/255.0 );
	}
}

/* Global Variables */
struct Texture {
	void *  pointer;
	uint32_t w, h, tw, th;
	bool pot, loaded;
	std::string file;
};

LuxColour gles_graphics_colour = { 0, 0, 0, 255 };

/* Local Function */
void gles_2DDrawQuad(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLfloat z, LuxColour color1, LuxColour color2, LuxColour color3, LuxColour color4, uint32_t texture )
{
	if (texture)
		glBindTexture(GL_TEXTURE_2D, texture);
	else
		glBindTexture(GL_TEXTURE_2D, 0);

	GLfloat verteces[12] = {
		x1, y1, z,
		x2, y1, z,
		x2, y2, z,
		x1, y2, z
	};

	GLfloat texCoords[8] = {
		0.0, 0.0,
		0.0, 1.0,
		1.0, 1.0,
		1.0, 0.0
	};
	GLubyte colors[16] = {
		color1.r, color1.b, color1.g, color1.a,
		color1.r, color1.b, color1.g, color1.a,
		color1.r, color1.b, color1.g, color1.a,
		color1.r, color1.b, color1.g, color1.a
	};

	glVertexAttribPointer(gvPositionHandle, 3, GL_FLOAT, GL_FALSE, 0, verteces);
	glEnableVertexAttribArray( gvPositionHandle );
	/*
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, texCoords);
	glVertexAttribPointer(0, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, colors);
	*/
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);


}

/* Global Function */
LUX_DISPLAY_FUNCTION bool Lux_NATIVE_Init(uint16_t  width, uint16_t height, uint8_t bpp, uint16_t * actual_width, uint16_t * actual_height, bool fullscreen )
{
	glViewport(0, 0, width, height);
	return true;
}

LUX_DISPLAY_FUNCTION void Lux_NATIVE_Destory()
{

}

LUX_DISPLAY_FUNCTION void Lux_NATIVE_BackgroundObject( MapObject background  )
{
	gles_graphics_colour = background.effects.primary_colour;
	glClearColor((float)gles_graphics_colour.r / 255.0f, (float)gles_graphics_colour.g / 255.0f, (float)gles_graphics_colour.b / 255.0f, 1.0f);
}

LUX_DISPLAY_FUNCTION void Lux_NATIVE_UpdateRect(uint8_t screen,LuxRect rect)
{

}

LUX_DISPLAY_FUNCTION void Lux_NATIVE_Show( uint8_t screen )
{
	gles_2DDrawQuad(1.0f, 10.0f, 100.0f, 100.0f, 1.0f, colour::red, colour::blue, colour::white, colour::white, 0 );
	//eglSwapBuffers( esContext->eglDisplay, esContext->eglSurface );
	glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );
}

LUX_DISPLAY_FUNCTION bool Lux_NATIVE_LoadSpriteSheet(std::string name, std::map<uint32_t, LuxSprite *> * children)
{
	return false;
}

LUX_DISPLAY_FUNCTION bool Lux_NATIVE_FreeSpriteSheet( std::map<uint32_t, LuxSprite *> * children)
{
	return false;
}

LUX_DISPLAY_FUNCTION void Lux_NATIVE_DrawSprite(LuxSprite * sprite, LuxRect dest_rect, ObjectEffect effects)
{

}

LUX_DISPLAY_FUNCTION void Lux_NATIVE_DrawRect( LuxRect dest_rect, ObjectEffect effects)
{

}


LUX_DISPLAY_FUNCTION void Lux_NATIVE_DrawLine( LuxRect points, ObjectEffect effects)
{

}

LUX_DISPLAY_FUNCTION void Lux_NATIVE_DrawText( std::string text, LuxRect dest_rect, ObjectEffect effects, bool allow_custom)
{
	uint16_t x = dest_rect.x;
	std::string::iterator object;
	if ( text.empty() )
		return;

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
			x += 8;
		}
		else if ( cchar <= 128 && cchar > 32)
		{
			//Draw Char
			x += 8;
		}
		else if ( cchar < 224 )
			object++;
		else if ( cchar < 240 )
			object +=2;
		else if ( cchar < 245 )
			object +=3;
	}
}

LUX_DISPLAY_FUNCTION void Lux_NATIVE_DrawPolygon( int16_t * x_point, int16_t *y_point, uint16_t point_count, ObjectEffect effects, void * texture )
{

}

LUX_DISPLAY_FUNCTION void Lux_NATIVE_DrawCircle( LuxRect dest_rect, ObjectEffect effects )
{

}

LUX_DISPLAY_FUNCTION LuxSprite * Lux_NATIVE_PNGtoSprite( uint8_t * data, uint32_t size )
{
	return nullptr;
}

