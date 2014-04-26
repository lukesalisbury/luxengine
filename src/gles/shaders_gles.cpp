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

#ifdef SHADER_IMPL_GLES
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

#include "shaders_shared.inc.cpp"

#endif // SHADER_IMPL_GLES
