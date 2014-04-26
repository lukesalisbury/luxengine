/****************************
Copyright (c) 2012 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include "gles.hpp"
#include "gl_platform.hpp"
#include "shaders.hpp"

#if USING_GLES == 1
typedef GLuint GLhandleARB;
#endif
namespace OpenGLShaderImpl {

	ShaderData shaders[NUM_SHADERS] = {
		/* SHADER_DEFAULT */
		{ 0, 0, 0,
		/* vertex shader */
		"\
		attribute vec4 vPosition;					\
		attribute vec4 vColor;						\
		attribute vec2 vTexCoord;					\
		uniform mat4 mProjection;					\
		varying vec4 v_color;							\
		varying vec2 v_texCoord;					\
		void main()									\
		{											\
			gl_Position = mProjection * vPosition;	\
			v_color = vColor;						\
			v_texCoord = vTexCoord;					\
		}",
		/* fragment shader */
		"\
			uniform sampler2D sTexture;				\
			varying vec4 v_color;						\
			varying vec2 v_texCoord;					\
			void main()									\
			{											\
				gl_FragColor = v_color;				\
			}",
		},

		/* SHADER_GLOW */
		{ 0, 0, 0,
		/* vertex shader */
		"varying vec4 v_color;							\
		varying vec2 v_texCoord;						\
		void main()\
		{\
			gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\
			v_color = gl_Color;\
			v_texCoord = vec2(gl_MultiTexCoord0);\
		}",
		/* fragment shader */
		"varying vec4 v_color;\
		varying vec2 v_texCoord;\
		uniform sampler2D tex0;\
		uniform vec4 v_colour1;\
		uniform vec4 v_colour2;\
		void main()\
		{\
			vec4 color = texture2D(tex0, v_texCoord);\
			vec4 sum;\
			bool border = false;\
			if (color.a < 0.9)\
			{\
				sum = texture2D( tex0, abs(v_texCoord + vec2( 0.00, -0.025 )) );\
				if (sum.a >= 0.5)\
					border = true;\
				sum = texture2D( tex0, abs(v_texCoord + vec2( -0.025, 0.00 )) );\
				if (sum.a >= 0.5)\
					border = true;\
				sum = texture2D( tex0, abs(v_texCoord + vec2( 0.00, 0.025 )) );\
				if (sum.a >= 0.5)\
					border = true;\
				sum = texture2D( tex0, abs(v_texCoord + vec2( 0.025, 0.00 )) );\
				if (sum.a >= 0.5)\
					border = true;\
			}\
			if ( border )\
			{\
				gl_FragColor = v_colour2;\
			}\
			else\
			{\
				gl_FragColor = color;\
			}\
		}"
		},
		/* SHADER_NEGATIVE */
		{ 0, 0, 0,
		/* vertex shader */
		"varying vec4 v_color;\
		varying vec2 v_texCoord;\
		\
		void main()\
		{\
			gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\
			v_color = gl_Color;\
			v_texCoord = vec2(gl_MultiTexCoord0);\
		}",
		/* fragment shader */
		"varying vec4 v_color;\
		varying vec2 v_texCoord;\
		uniform sampler2D tex0;\
		uniform vec4 v_colour1;\
		uniform vec4 v_colour2;\
		\
		void main()\
		{\
			vec4 color = texture2D(tex0, v_texCoord);\
			vec4 result;\
			result.r=1.0-color.r;\
			result.g=1.0-color.g;\
			result.b=1.0-color.b;\
			result.a=color.a;\
			gl_FragColor = result;\
		}",
		},
		/* SHADER_REPLACE */
		{ 0, 0, 0,
		/* vertex shader */
		"varying vec4 v_color;\
		varying vec2 v_texCoord;\
		\
		void main()\
		{\
			gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\
			v_color = gl_Color;\
			v_texCoord = vec2(gl_MultiTexCoord0);\
		}",
		/* fragment shader */
		"varying vec4 v_color;\
		varying vec2 v_texCoord;\
		uniform sampler2D tex0;\
		uniform vec4 v_colour1;\
		uniform vec4 v_colour2;\
		\
		void main()\
		{\
			vec4 color = texture2D(tex0, v_texCoord);\
			if (v_colour1 == color) {\
				gl_FragColor = v_colour2;\
			} else {\
				gl_FragColor = color;\
			}\
		}"
		},
		/* SHADER_GREYSCALE */
		{ 0, 0, 0,
		/* vertex shader */
		"varying vec4 v_color;\
		varying vec2 v_texCoord;\
		\
		void main()\
		{\
			gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\
			v_color = gl_Color;\
			v_texCoord = vec2(gl_MultiTexCoord0);\
		}",
		/* fragment shader */
		"varying vec4 v_color;\
		varying vec2 v_texCoord;\
		uniform sampler2D tex0;\
		uniform vec4 v_colour1;\
		uniform vec4 v_colour2;\
		\
		void main()\
		{\
			vec4 color = texture2D(tex0, v_texCoord);\
			float gray = (color.r + color.g + color.b) / 3.0;\
			vec3 grayscale = vec3(gray);\
			gl_FragColor = vec4(grayscale, color.a);\
		}"
		},
		/* SHADER_CUSTOM1 */
		{ 0, 0, 0,
		/* vertex shader */
		"",
		/* fragment shader */
		""
		},
		/* SHADER_CUSTOM2 */
		{ 0, 0, 0,
		/* vertex shader */
		"",
		/* fragment shader */
		""
		},
		/* SHADER_CUSTOM3 */
		{ 0, 0, 0,
		/* vertex shader */
		"",
		/* fragment shader */
		""
		},
		/* SHADER_CUSTOM4 */
		{ 0, 0, 0,
		/* vertex shader */
		"",
		/* fragment shader */
		""
		},
		/* SHADER_CUSTOM5 */
		{ 0, 0, 0,
		/* vertex shader */
		"",
		/* fragment shader */
		""
		},
		/* SHADER_CUSTOM6 */
		{ 0, 0, 0,
		/* vertex shader */
		"",
		/* fragment shader */
		""
		}
	};

	float primary_colour[4];
	float secondary_colour[4];
	bool supported = false;
}

namespace OpenGLShader
{


	void CheckError( const char * file, int line )
	{
		GLenum err( glGetError() );

		while( err != GL_NO_ERROR )
		{
			std::cout << "OpenGLShader Error '";
			switch (err)
			{
				case GL_INVALID_OPERATION:
					std::cout << "INVALID_OPERATION";
					break;
				case GL_INVALID_ENUM:
					std::cout << "INVALID_ENUM";
					break;
				case GL_INVALID_VALUE:
					std::cout << "INVALID_VALUE";
					break;
				case GL_OUT_OF_MEMORY:
					std::cout << "OUT_OF_MEMORY";
					break;
			}

			std::cout << "' from " << file << ":" << line << std::endl;
			err = glGetError();
		}
	}
}

#if USING_GLES >= 2 || USING_GLDESKTOP >= 4
	namespace OpenGLShader
	{
		GLuint gvPositionHandle = 0;
		GLuint gvColorHandle = 0;

		void ApplyShader( uint8_t s )
		{

			if ( !OpenGLShaderImpl::supported )
				return;

			if ( OpenGLShaderImpl::shaders[SHADER_DEFAULT].program == 0)
			{
				OpenGLShader::CompileShader( &OpenGLShaderImpl::shaders[SHADER_DEFAULT] );
			}
			if ( OpenGLShaderImpl::shaders[SHADER_DEFAULT].program != 0)
			{
				OpenGLShaderImpl::UseProgramObject( OpenGLShaderImpl::shaders[SHADER_DEFAULT].program );
			}

			/*
			if ( s < NUM_SHADERS && s != SHADER_DEFAULT )
			{
				if ( OpenGLShaderImpl::shaders[s].program == 0)
				{
					OpenGLShader::CompileShader( &OpenGLShaderImpl::shaders[s] );
				}
				if ( OpenGLShaderImpl::shaders[s].program != 0)
				{
					OpenGLShaderImpl::UseProgramObject( OpenGLShaderImpl::shaders[s].program );
					OpenGLShaderImpl::SetUniform4f( OpenGLShaderImpl::shaders[s].program, "v_colour1", OpenGLShaderImpl::primary_colour[0], OpenGLShaderImpl::primary_colour[1], OpenGLShaderImpl::primary_colour[2], OpenGLShaderImpl::primary_colour[3] );
					OpenGLShaderImpl::SetUniform4f( OpenGLShaderImpl::shaders[s].program, "v_colour2", OpenGLShaderImpl::secondary_colour[0], OpenGLShaderImpl::secondary_colour[1], OpenGLShaderImpl::secondary_colour[2], OpenGLShaderImpl::secondary_colour[3] );
				}
			}
			*/
		}

	}
#endif

#if USING_GLDESKTOP == 2
	namespace OpenGLShader
	{
		void ApplyShader( uint8_t s )
		{
			if (!OpenGLShaderImpl::supported)
				return;

			if ( s < NUM_SHADERS && s != SHADER_DEFAULT )
			{
				if ( OpenGLShaderImpl::shaders[s].program == 0)
				{
					OpenGLShader::CompileShader( &OpenGLShaderImpl::shaders[s] );
				}
				if ( OpenGLShaderImpl::shaders[s].program != 0)
				{
					OpenGLShaderImpl::UseProgramObject( OpenGLShaderImpl::shaders[s].program );
					OpenGLShaderImpl::SetUniform4f( OpenGLShaderImpl::shaders[s].program, "v_colour1", OpenGLShaderImpl::primary_colour[0], OpenGLShaderImpl::primary_colour[1], OpenGLShaderImpl::primary_colour[2], OpenGLShaderImpl::primary_colour[3] );
					OpenGLShaderImpl::SetUniform4f( OpenGLShaderImpl::shaders[s].program, "v_colour2", OpenGLShaderImpl::secondary_colour[0], OpenGLShaderImpl::secondary_colour[1], OpenGLShaderImpl::secondary_colour[2], OpenGLShaderImpl::secondary_colour[3] );
				}
			}
		}
	}
#endif

