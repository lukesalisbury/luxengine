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

#ifdef SHADER_IMPL_NONE

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
	void SetUniformMatrix4fv( GLhandleARB programObj, const GLchar* name, const GLfloat *value)
	{

	}
	void UseProgramObject(GLhandleARB programObj)
	{

	}
}

namespace OpenGLShader
{
	bool Check()
	{
		return false;
	}
	bool FreeShader( uint8_t i )
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

	void CompileShader( ShaderData * data, char * vertex_shader, char * fragment_shader  )
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

#endif // SHADER_IMPL
