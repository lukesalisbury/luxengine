/****************************
Copyright (c) 2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/


#ifndef SHADER_HPP
#define SHADER_HPP


namespace OpenGLShaderImpl
{
	extern ShaderData shaders[NUM_SHADERS];
	extern bool supported;
	extern float primary_colour[4];
	extern float secondary_colour[4];

	bool Init();

	void GetShaderiv( GLuint shader, GLenum pname, GLint *params);
	void GetShaderInfoLog( GLuint shader, GLsizei maxLength, GLsizei * length, GLchar * infoLog );

	void AttachShader( GLhandleARB containerObj, GLhandleARB obj );

	void CompileShader( GLhandleARB shaderObj );
	GLhandleARB CreateProgramObject( void);
	GLhandleARB CreateShaderObject( GLenum shaderType );
	void DeleteObject( GLhandleARB obj);

	GLint GetUniformLocation( GLhandleARB programObj, const GLchar * name);
	void LinkProgram( GLhandleARB programObj);
	void ShaderSource( GLhandleARB shaderObj, GLsizei count, const GLchar ** string, const GLint *length);

	void SetUniform4f( GLhandleARB programObj, const GLchar* name, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
	void SetUniformMatrix4fv( GLhandleARB programObj, const GLchar* name, const GLfloat *value);
	void SetUniform1i(GLhandleARB programObj, const GLchar* name, GLint value );

	void UseProgramObject(GLhandleARB programObj);

	GLint GetAttribLocation( GLhandleARB programObj, const GLchar * name );
	void VertexAttribPointer( GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid * pointer );
	void EnableVertexAttribArray( GLuint index );
	void DisableVertexAttribArray( GLuint index );
}

namespace OpenGLShaderData
{

}
#endif // SHADER_HPP
