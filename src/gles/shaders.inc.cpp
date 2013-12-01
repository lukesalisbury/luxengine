/****************************
Copyright (c) 2012 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#ifndef _GLES_SHADERS_
#define _GLES_SHADERS_

#include "gles.hpp"
#include "gl_platform.hpp"

#include "default_shaders.inc.hpp"

#if USING_GLES == 1
typedef GLuint GLhandleARB;
#endif

void glError(const char *file, int line)
{
	GLenum err (glGetError());

	while(err!=GL_NO_ERROR) {
		std::string error;

		switch(err) {
			case GL_INVALID_OPERATION:      error="INVALID_OPERATION";      break;
			case GL_INVALID_ENUM:           error="INVALID_ENUM";           break;
			case GL_INVALID_VALUE:          error="INVALID_VALUE";          break;
			case GL_OUT_OF_MEMORY:          error="OUT_OF_MEMORY";          break;
		}

		std::cout<<"GL_"<<error<<" - "<<file<<':'<<line<<std::endl;
		err=glGetError();
	}
}

namespace OpenGLShaderImpl {
	bool supported = false;

	bool Init();

	void GetShaderiv( GLuint shader, GLenum pname, GLint *params);
	void GetShaderInfoLog( GLuint shader, GLsizei maxLength, GLsizei * length, GLchar * infoLog );

	void AttachShader(GLhandleARB containerObj, GLhandleARB obj);

	void CompileShader(GLhandleARB shaderObj);
	GLhandleARB CreateProgramObject(void);
	GLhandleARB CreateShaderObject(GLenum shaderType);
	void DeleteObject(GLhandleARB obj);

	GLint GetUniformLocation(GLhandleARB programObj, const GLchar * name);
	void LinkProgram(GLhandleARB programObj);
	void ShaderSource(GLhandleARB shaderObj, GLsizei count, const GLchar ** string, const GLint *length);
	void SetUniform4f(GLhandleARB programObj, const GLchar* name, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
	void UseProgramObject(GLhandleARB programObj);

}

#if USING_GLES == 1
namespace OpenGLShaderImpl
{
	bool Init()
	{
		return false;
	}

	void GetShaderiv( GLuint shader, GLenum pname, GLint *params)
	{

	}
	void GetShaderInfoLog( GLuint shader, GLsizei maxLength, GLsizei * length, GLchar * infoLog )
	{

	}

	void AttachShader(GLhandleARB containerObj, GLhandleARB obj)
	{

	}

	void CompileShader(GLhandleARB shaderObj)
	{

	}
	GLhandleARB CreateProgramObject(void)
	{
		return 0;
	}
	GLhandleARB CreateShaderObject(GLenum shaderType)
	{
		return 0;
	}

	void DeleteObject(GLhandleARB obj)
	{

	}

	GLint GetUniformLocation(GLhandleARB programObj, const GLchar* name)
	{
		return 0;
	}
	void LinkProgram(GLhandleARB programObj)
	{

	}
	void ShaderSource(GLhandleARB shaderObj, GLsizei count, const GLchar ** string, const GLint *length)
	{

	}
	void SetUniform4f(GLhandleARB programObj, const GLchar* name, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
	{

	}
	void UseProgramObject(GLhandleARB programObj)
	{

	}

}
#endif

#if USING_GLES >= 2 || USING_GLDESKTOP >= 4
namespace OpenGLShaderImpl
{
	bool Init()
	{
		supported = true;
		return supported;
	}

	void GetShaderiv( GLuint shader, GLenum pname, GLint *params)
	{
		glGetShaderiv( shader, pname, params);
	}
	void GetShaderInfoLog( GLuint shader, GLsizei maxLength, GLsizei * length, GLchar * infoLog )
	{
		glGetShaderInfoLog( shader, maxLength, length, infoLog );
	}
	void AttachShader(GLhandleARB containerObj, GLhandleARB obj)
	{
		glAttachShader(containerObj, obj);
	}
}
#endif

#if USING_GLDESKTOP < 4 && USING_GLDESKTOP >= 1
/* Desktop OpenGL */
bool Lux_OGL_QueryExtension(char *extName);

#if !defined(__APPLE__)

static PFNGLCOMPILESHADERARBPROC glCompileShader = NULL;

static PFNGLGETUNIFORMLOCATIONARBPROC glGetUniformLocation = NULL;
static PFNGLLINKPROGRAMARBPROC glLinkProgram = NULL;
static PFNGLSHADERSOURCEARBPROC glShaderSource = NULL;
static PFNGLUNIFORM4FARBPROC glUniform4f = NULL;
#endif

static PFNGLUSEPROGRAMOBJECTARBPROC glUseProgramObject = NULL;
static PFNGLATTACHOBJECTARBPROC glAttachObjectARB = NULL;

static PFNGLCREATEPROGRAMOBJECTARBPROC glCreateProgramObject = NULL;
static PFNGLCREATESHADEROBJECTARBPROC glCreateShaderObject = NULL;
static PFNGLDELETEOBJECTARBPROC glDeleteObject = NULL;
static PFNGLGETINFOLOGARBPROC glGetInfoLogARB = NULL;
static PFNGLGETOBJECTPARAMETERIVARBPROC glGetObjectParameterivARB = NULL;


namespace OpenGLShaderImpl
{
	bool Init()
	{
		std::cout << __FILE__ << ": | GL_ARB_shader_objects :" << Lux_OGL_QueryExtension("GL_ARB_shader_objects") << std::endl;
		std::cout << __FILE__ << ": | GL_ARB_shading_language_100 :" << Lux_OGL_QueryExtension("GL_ARB_shading_language_100") << std::endl;
		std::cout << __FILE__ << ": | GL_ARB_vertex_shader :" << Lux_OGL_QueryExtension("GL_ARB_vertex_shader") << std::endl;
		std::cout << __FILE__ << ": | GL_ARB_fragment_shader :" << Lux_OGL_QueryExtension("GL_ARB_fragment_shader") << std::endl;

#if defined(__APPLE__)
		std::cout << __FILE__ << ": | Shaders disabled on OS X" << std::endl;
		return false;
#else

		if ( Lux_OGL_QueryExtension("GL_ARB_shader_objects") && Lux_OGL_QueryExtension("GL_ARB_shading_language_100") && Lux_OGL_QueryExtension("GL_ARB_vertex_shader") && Lux_OGL_QueryExtension("GL_ARB_fragment_shader") )
		{

			glAttachObjectARB = (PFNGLATTACHOBJECTARBPROC) SDL_GL_GetProcAddress("glAttachObjectARB");
			glCompileShader = (PFNGLCOMPILESHADERARBPROC) SDL_GL_GetProcAddress("glCompileShaderARB");
			glCreateProgramObject = (PFNGLCREATEPROGRAMOBJECTARBPROC) SDL_GL_GetProcAddress("glCreateProgramObjectARB");
			glCreateShaderObject = (PFNGLCREATESHADEROBJECTARBPROC) SDL_GL_GetProcAddress("glCreateShaderObjectARB");
			glDeleteObject = (PFNGLDELETEOBJECTARBPROC) SDL_GL_GetProcAddress("glDeleteObjectARB");
			glGetInfoLogARB = (PFNGLGETINFOLOGARBPROC) SDL_GL_GetProcAddress("glGetInfoLogARB");
			glGetObjectParameterivARB = (PFNGLGETOBJECTPARAMETERIVARBPROC) SDL_GL_GetProcAddress("glGetObjectParameterivARB");
			glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONARBPROC) SDL_GL_GetProcAddress("glGetUniformLocationARB");
			glLinkProgram = (PFNGLLINKPROGRAMARBPROC) SDL_GL_GetProcAddress("glLinkProgramARB");
			glShaderSource = (PFNGLSHADERSOURCEARBPROC) SDL_GL_GetProcAddress("glShaderSourceARB");
			glUniform4f = (PFNGLUNIFORM4FARBPROC) SDL_GL_GetProcAddress("glUniform4fARB");
			glUseProgramObject = (PFNGLUSEPROGRAMOBJECTARBPROC) SDL_GL_GetProcAddress("glUseProgramObjectARB");
			supported = true;
		}

		return supported;
#endif
	}

	void GetShaderiv( GLuint shader, GLenum pname, GLint *params)
	{
		glGetObjectParameterivARB( shader, pname, params );
	}
	void GetShaderInfoLog( GLuint shader, GLsizei maxLength, GLsizei * length, GLchar * infoLog )
	{
		glGetInfoLogARB( shader, maxLength, length, infoLog );

	}
	void AttachShader(GLhandleARB containerObj, GLhandleARB obj)
	{
		glAttachObjectARB(containerObj, obj);
	}
}
#endif



#if USING_GLES >= 2 || USING_GLDESKTOP >= 1
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


	void SetUniform4f(GLhandleARB programObj, const GLchar* name, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
	{
		GLint location = glGetUniformLocation(programObj, name);
		if ( location != -1 )
		{
			glUniform4f( location,  v0,  v1,  v2,  v3);
		}

	}

}
#endif

#if USING_GLES >= 2 || USING_GLDESKTOP >= 1
namespace OpenGLShader
{
	float primary_colour[4];
	float secondary_colour[4];

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

	bool Free()
	{
		std::cout << __FILE__ << ": | Freeing Shader" << std::endl;
		if ( OpenGLShaderImpl::supported )
		{
			for ( uint8_t i = 0; i < NUM_SHADERS; ++i)
			{
				OpenGLShaderImpl::DeleteObject(shaders[i].vert_shader);
				OpenGLShaderImpl::DeleteObject(shaders[i].frag_shader);
				OpenGLShaderImpl::DeleteObject(shaders[i].program);
			}
			return true;
		}
		return false;
	}

	bool CompileCode(GLenum shader, const char *source)
	{
		if (!OpenGLShaderImpl::supported)
			return false;

		GLint status;

		OpenGLShaderImpl::ShaderSource(shader, 1, &source, NULL);
		OpenGLShaderImpl::CompileShader(shader);
		OpenGLShaderImpl::GetShaderiv(shader, GL_COMPILE_STATUS, &status);
		if ( status == 0 )
		{
			GLint length;
			char *info = NULL;

			OpenGLShaderImpl::GetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
			info = new char(length+1);
			OpenGLShaderImpl::GetShaderInfoLog(shader, length, NULL, info);
			std::cout << "Failed to compile shader (" << source << "):" << info << std::endl;
			delete info;

		}
		return !!status;
	}

	void CompileShader( ShaderData * data )
	{
		if (!OpenGLShaderImpl::supported)
			return;
		/* Create one program object to rule them all */
		data->program = OpenGLShaderImpl::CreateProgramObject();

		/* Create the vertex shader */
		data->vert_shader = OpenGLShaderImpl::CreateShaderObject(GL_VERTEX_SHADER);
		if (!CompileCode(data->vert_shader, data->vert_source))
		{
			glError(__FILE__, __LINE__);
			return;
		}

		/* Create the fragment shader */
		data->frag_shader = OpenGLShaderImpl::CreateShaderObject(GL_FRAGMENT_SHADER);
		if (!CompileCode(data->frag_shader, data->frag_source))
		{
			glError(__FILE__, __LINE__);
			return;
		}

		/* ... and in the darkness bind them */
		OpenGLShaderImpl::AttachShader(data->program, data->vert_shader);
		glError(__FILE__, __LINE__);
		OpenGLShaderImpl::AttachShader(data->program, data->frag_shader);
		glError(__FILE__, __LINE__);
		OpenGLShaderImpl::LinkProgram(data->program);
		glError(__FILE__, __LINE__);
	}

	void SetPrimaryColor( float r, float g, float b, float a )
	{
		primary_colour[0] = r;
		primary_colour[1] = g;
		primary_colour[2] = b;
		primary_colour[3] = a;
	}

	void SetSecondaryColor( float r, float g, float b, float a )
	{
		secondary_colour[0] = r;
		secondary_colour[1] = g;
		secondary_colour[2] = b;
		secondary_colour[3] = a;

	}

	void ClearShader( )
	{
		OpenGLShaderImpl::UseProgramObject( 0 );
	}

	void ApplyShader( uint8_t s )
	{

		if (!OpenGLShaderImpl::supported)
			return;

		if ( s < NUM_SHADERS && s != SHADER_DEFAULT )
		{
			if ( shaders[s].program == 0)
			{
				OpenGLShader::CompileShader( &shaders[s] );
			}
			if ( shaders[s].program != 0)
			{
				OpenGLShaderImpl::UseProgramObject( shaders[s].program );
				OpenGLShaderImpl::SetUniform4f(shaders[s].program, "FirstColour", primary_colour[0], primary_colour[1], primary_colour[2], primary_colour[3] );
				OpenGLShaderImpl::SetUniform4f(shaders[s].program, "SecondColour", secondary_colour[0], secondary_colour[1], secondary_colour[2], secondary_colour[3] );
			}
		}
	}
}
#else
namespace OpenGLShader
{
	bool Check()
	{
		return false;
	}

	bool Free()
	{
		return false;
	}

	bool CompileCode(GLenum shader, const char *source)
	{
		return false;
	}

	void CompileShader( ShaderData * data )
	{
		return;
	}

	void SetPrimaryColor( float r, float g, float b, float a )
	{
	}

	void SetSecondaryColor( float r, float g, float b, float a )
	{

	}

	void ClearShader( )
	{

	}

	void ApplyShader( uint8_t s )
	{

	}
}
#endif

#endif
