/****************************
Copyright (c) 2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#ifndef SHADER_IMPL_SHARED
#define SHADER_IMPL_SHARED

#include "gles.hpp"
#include "gl_platform.hpp"
#include "shaders.hpp"


namespace OpenGLShaderImpl
{
	void CompileShader(GLhandleARB shaderObj)
	{
		glCompileShader(shaderObj);
	}

	GLhandleARB CreateProgramObject(void)
	{
		return glCreateProgramObject();
	}

	GLhandleARB CreateShaderObject(GLenum shaderType)
	{
		return glCreateShaderObject(shaderType);
	}

	void DeleteObject(GLhandleARB obj)
	{
		glDeleteObject(obj);
	}

	GLint GetUniformLocation(GLhandleARB programObj, const GLchar* name)
	{
		return glGetUniformLocation(programObj, name);
	}

	void LinkProgram(GLhandleARB programObj)
	{
		glLinkProgram(programObj);
	}

	void ShaderSource(GLhandleARB shaderObj, GLsizei count, const GLchar ** string, const GLint *length)
	{
		glShaderSource( shaderObj, count, string, length);
	}

	void UseProgramObject(GLhandleARB programObj)
	{
		glUseProgramObject( programObj );
	}

	void SetUniform4f( GLhandleARB programObj, const GLchar* name, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3 )
	{
		GLint location = glGetUniformLocation(programObj, name);
		if ( location != -1 )
		{
			glUniform4f( location,  v0,  v1,  v2,  v3);
		}
	}

	void SetUniformMatrix4fv( GLhandleARB programObj, const GLchar* name, const GLfloat *value )
	{
		GLint location = glGetUniformLocation(programObj, name);
		if ( location != -1 )
		{
			glUniformMatrix4fv( location, 1, GL_TRUE, &value[0] );
		}
	}

	void SetUniform1i( GLhandleARB programObj, const GLchar* name, GLint value )
	{
		GLint location = glGetUniformLocation(programObj, name);
		if ( location != -1 )
		{
			glUniform1i( location, value );
		}
	}


}

namespace OpenGLShader
{
	bool Check()
	{
		std::cout << __FILE__ << ": | Checking Shader Support" << std::endl;
		if ( OpenGLShaderImpl::Init() )
		{
			for ( uint8_t i = 0; i < NUM_SHADERS; ++i)
			{
				//CompileShader( &shaders[i] );
			}
			return true;
		}
		return false;
	}

	bool FreeShader( uint8_t i )
	{
		if ( OpenGLShaderImpl::supported )
		{
			if (i < NUM_SHADERS )
			{
				OpenGLShaderImpl::DeleteObject( OpenGLShaderImpl::shaders[i].vert_shader );
				OpenGLShaderImpl::DeleteObject( OpenGLShaderImpl::shaders[i].frag_shader );
				OpenGLShaderImpl::DeleteObject( OpenGLShaderImpl::shaders[i].program );
			}
			return true;
		}
		return false;
	}

	bool Free()
	{
		std::cout << __FILE__ << ": | Freeing Shader" << std::endl;
		if ( OpenGLShaderImpl::supported )
		{
			for ( uint8_t i = 0; i < NUM_SHADERS; ++i)
			{
				OpenGLShaderImpl::DeleteObject( OpenGLShaderImpl::shaders[i].vert_shader );
				OpenGLShaderImpl::DeleteObject( OpenGLShaderImpl::shaders[i].frag_shader );
				OpenGLShaderImpl::DeleteObject( OpenGLShaderImpl::shaders[i].program );
			}
			return true;
		}
		return false;
	}

	bool CompileCode(GLenum shader, const char *source)
	{
		if ( !OpenGLShaderImpl::supported )
			return false;

		GLint status;

		OpenGLShaderImpl::ShaderSource( shader, 1, &source, NULL );
		OpenGLShaderImpl::CompileShader( shader);
		OpenGLShaderImpl::GetShaderiv( shader, GL_COMPILE_STATUS, &status );
		if ( status == 0 )
		{
			GLint length;
			char *info = NULL;

			OpenGLShaderImpl::GetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
			info = new char[length+1];
			OpenGLShaderImpl::GetShaderInfoLog(shader, length, NULL, info);
			std::cout << "Failed to compile shader: " << info << std::endl;
			std::cout << "-----------------------------------------" << std::endl;
			std::cout << "Source: " << source << std::endl;
			std::cout << "-----------------------------------------" << std::endl;
			delete[] info;

		}
		return !!status;
	}

	void CompileShader( ShaderData * data )
	{
		if (!OpenGLShaderImpl::supported)
			return;

		data->program = OpenGLShaderImpl::CreateProgramObject();

		/* Create the vertex shader */
		data->vert_shader = OpenGLShaderImpl::CreateShaderObject(GL_VERTEX_SHADER);
		if (!CompileCode(data->vert_shader, data->vert_source))
		{
			CheckError(__FILE__, __LINE__);
			return;
		}

		/* Create the fragment shader */
		data->frag_shader = OpenGLShaderImpl::CreateShaderObject(GL_FRAGMENT_SHADER);
		if (!CompileCode(data->frag_shader, data->frag_source))
		{
			CheckError(__FILE__, __LINE__);
			return;
		}

		OpenGLShaderImpl::AttachShader(data->program, data->vert_shader);
		OpenGLShaderImpl::AttachShader(data->program, data->frag_shader);
		OpenGLShaderImpl::LinkProgram(data->program);
	}

	void SetPrimaryColor( float r, float g, float b, float a )
	{
		OpenGLShaderImpl::primary_colour[0] = r;
		OpenGLShaderImpl::primary_colour[1] = g;
		OpenGLShaderImpl::primary_colour[2] = b;
		OpenGLShaderImpl::primary_colour[3] = a;
	}

	void SetSecondaryColor( float r, float g, float b, float a )
	{
		OpenGLShaderImpl::secondary_colour[0] = r;
		OpenGLShaderImpl::secondary_colour[1] = g;
		OpenGLShaderImpl::secondary_colour[2] = b;
		OpenGLShaderImpl::secondary_colour[3] = a;

	}

	void ClearShader( )
	{
		OpenGLShaderImpl::UseProgramObject( 0 );
	}
}

#endif // SHADER_HPP
