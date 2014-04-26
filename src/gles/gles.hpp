/****************************
Copyright (c) 2012 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#ifndef _GLES_HEADER_
	#define _GLES_HEADER_

#include "gl_platform.hpp"


#ifndef GLES_DEFAULT_PRIMITIVE
	#define GLES_DEFAULT_PRIMITIVE GL_TRIANGLE_STRIP
#endif

#include "display_types.h"
#include "lux_types.h"

#define GLES_TEXTURE_32 GL_UNSIGNED_BYTE
#define GLES_TEXTURE_16 GL_UNSIGNED_SHORT_5_5_5_1
#define NATIVEFLOATTYPE float



enum {
	SHADER_DEFAULT,
	SHADER_GLOW,
	SHADER_NEGATIVE,
	SHADER_REPLACE,
	SHADER_GREY,
	SHADER_CUSTOM1,
	SHADER_CUSTOM2,
	SHADER_CUSTOM3,
	SHADER_CUSTOM4,
	SHADER_CUSTOM5,
	SHADER_CUSTOM6,
	NUM_SHADERS
};

struct Texture {
	GLuint pointer;
	uint32_t w, h, tw, th;
	bool pot, loaded;
	std::string file;
};

typedef struct {
	GLuint program;
	GLuint vert_shader;
	GLuint frag_shader;
	const char * vert_source;
	const char * frag_source;
} ShaderData;

typedef struct {

	GLuint texture;
	GLuint frame;
	uint32_t width;
	uint32_t height;
} FrameBufferObject;


typedef struct {
	NATIVEFLOATTYPE x, y, z, u, v;
	void set( NATIVEFLOATTYPE _x, NATIVEFLOATTYPE _y, NATIVEFLOATTYPE _z )
	{
		x = _x;
		y = _y;
		z = _z;
		u = 0;
		v = 1;
	};
	void set2( NATIVEFLOATTYPE _x, NATIVEFLOATTYPE _y, NATIVEFLOATTYPE _z, NATIVEFLOATTYPE _u, NATIVEFLOATTYPE _v )
	{
		x = _x;
		y = _y;
		z = _z;
		u = _u;
		v = _v;
	};
	void addDimension( NATIVEFLOATTYPE _w, NATIVEFLOATTYPE _h )
	{
		x += _w;
		y += _h;
	}
} LuxVertex;

namespace gles {
	void setOrtho(float left, float right, float bottom, float top, float nearV, float farV );
	void render( GLenum mode, LuxVertex * vertexs, uint16_t count, Texture * texture, LuxVertex dest, LuxColour * colour, float angle, LuxVertex scale, LuxVertex rotation, uint8_t shader );

}

namespace OpenGLShader
{
	bool Check();
	bool Free();
	bool CompileCode(GLenum shader, const char *source);
	void CompileShader( ShaderData * data );
	void ClearShader( );
	void ApplyShader( uint8_t s );

	void SetPrimaryColor( float r, float g, float b, float a );
	void SetSecondaryColor( float r, float g, float b, float a );

	void CheckError( const char * file, int line );
}

#endif
