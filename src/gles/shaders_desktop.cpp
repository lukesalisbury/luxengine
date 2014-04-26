/****************************
Copyright (c) 2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include "gles.hpp"
#include "gl_platform.hpp"
#include "shaders.hpp"


#ifdef SHADER_IMPL_DESKTOP
/* Desktop OpenGL */
bool Lux_OGL_QueryExtension(char *extName);

static PFNGLCOMPILESHADERARBPROC glCompileShader = NULL;
static PFNGLGETUNIFORMLOCATIONARBPROC glGetUniformLocation = NULL;
static PFNGLLINKPROGRAMARBPROC glLinkProgram = NULL;
static PFNGLSHADERSOURCEARBPROC glShaderSource = NULL;
static PFNGLUNIFORM4FARBPROC glUniform4f = NULL;
static PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv = NULL;
static PFNGLUNIFORM1IPROC glUniform1i = NULL;

static PFNGLUSEPROGRAMOBJECTARBPROC glUseProgramObject = NULL;
static PFNGLATTACHOBJECTARBPROC glAttachObjectARB = NULL;

static PFNGLCREATEPROGRAMOBJECTARBPROC glCreateProgramObject = NULL;
static PFNGLCREATESHADEROBJECTARBPROC glCreateShaderObject = NULL;
static PFNGLDELETEOBJECTARBPROC glDeleteObject = NULL;
static PFNGLGETINFOLOGARBPROC glGetInfoLogARB = NULL;
static PFNGLGETOBJECTPARAMETERIVARBPROC glGetObjectParameterivARB = NULL;

#if USING_GLDESKTOP >= 4
static PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation = NULL;
static PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer = NULL;
static PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = NULL;
static PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray = NULL;
#endif

namespace OpenGLShaderImpl
{
	bool Init()
	{
	#if defined(__APPLE__)
		std::cout << __FILE__ << ": | Shaders disabled on OS X" << std::endl;
		return false;
	#else
		std::cout << __FILE__ << ": | GL_ARB_shader_objects :" << Lux_OGL_QueryExtension("GL_ARB_shader_objects") << std::endl;
		std::cout << __FILE__ << ": | GL_ARB_shading_language_100 :" << Lux_OGL_QueryExtension("GL_ARB_shading_language_100") << std::endl;
		std::cout << __FILE__ << ": | GL_ARB_vertex_shader :" << Lux_OGL_QueryExtension("GL_ARB_vertex_shader") << std::endl;
		std::cout << __FILE__ << ": | GL_ARB_fragment_shader :" << Lux_OGL_QueryExtension("GL_ARB_fragment_shader") << std::endl;

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
			glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC) SDL_GL_GetProcAddress("glUniformMatrix4fvARB");
			glUniform1i = (PFNGLUNIFORM1IPROC) SDL_GL_GetProcAddress("glUniform1iARB");
			glUseProgramObject = (PFNGLUSEPROGRAMOBJECTARBPROC) SDL_GL_GetProcAddress("glUseProgramObjectARB");



			#if USING_GLDESKTOP >= 4
			glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC) SDL_GL_GetProcAddress("glGetAttribLocation");
			glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC) SDL_GL_GetProcAddress("glVertexAttribPointer");
			glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC) SDL_GL_GetProcAddress("glEnableVertexAttribArray");
			glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC) SDL_GL_GetProcAddress("glDisableVertexAttribArray");
			#endif

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

	#if USING_GLDESKTOP >= 4
	void VertexAttribPointer( GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid * pointer )
	{
		glVertexAttribPointer( index, size, type, normalized, stride, pointer );
	}

	void EnableVertexAttribArray( GLuint index )
	{
		glEnableVertexAttribArray( index );
	}
	void DisableVertexAttribArray( GLuint index )
	{
		glDisableVertexAttribArray( index );
	}

	GLint GetAttribLocation( GLhandleARB programObj, const GLchar * name )
	{
		return glGetAttribLocation( programObj, name );
	}
	#endif

}

#include "shaders_shared.inc.cpp"


#endif // SHADER_IMPL
