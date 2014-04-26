/****************************
Copyright (c) 2012 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include "gl_platform.hpp"
/* Note
 * OpenGL ES 2 render is broken.
 */

#if USING_GLES == 2 || USING_GLDESKTOP == 4
	#include "gles.hpp"
	#include "shaders.hpp"




	namespace gles {
		float projection[16];
		GLuint gvPositionHandle;
		GLuint gvColorHandle;

		void setOrtho( float left, float right, float bottom, float top, float nearV, float farV )
		{
			projection[0] = 2.0f / (right - left);
			projection[1] = 0.0f;
			projection[2] = 0.0f;
			projection[3] = 0.0f;

			projection[4] = 0.0f;
			projection[5] = 2.0f / (top - bottom);
			projection[6] = 0.0f;
			projection[7] = 0.0f;

			projection[8] = 0.0f;
			projection[9] = 0.0f;
			projection[10] = 0.0f;
			projection[11] = 0.0f;

			projection[12] = -1.0f;
			projection[13] = 1.0f;
			projection[14] = 0.0f;
			projection[15] = 1.0f;

		}


		void render( GLenum mode, LuxVertex * vertexs, uint16_t count, Texture * texture, LuxVertex dest, LuxColour * colour, float angle, LuxVertex scale, LuxVertex rotation, uint8_t shader )
		{

			OpenGLShader::ApplyShader( shader );

			gvPositionHandle = OpenGLShaderImpl::GetAttribLocation( OpenGLShaderImpl::shaders[SHADER_DEFAULT].program, "vPosition" );
			gvColorHandle = OpenGLShaderImpl::GetAttribLocation( OpenGLShaderImpl::shaders[SHADER_DEFAULT].program, "vColor" );

			OpenGLShaderImpl::SetUniformMatrix4fv( OpenGLShaderImpl::shaders[SHADER_DEFAULT].program, "mProjection", projection );
			if ( texture )
				OpenGLShaderImpl::SetUniform1i( OpenGLShaderImpl::shaders[SHADER_DEFAULT].program, "sTexture", texture->pointer );
			else
				OpenGLShaderImpl::SetUniform1i( OpenGLShaderImpl::shaders[SHADER_DEFAULT].program, "sTexture", 0 );

			OpenGLShaderImpl::VertexAttribPointer( gvPositionHandle, 3, GL_FLOAT, GL_FALSE, 0, vertexs);
			OpenGLShaderImpl::VertexAttribPointer( gvColorHandle, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, colour);

			OpenGLShaderImpl::EnableVertexAttribArray( gvPositionHandle );
			OpenGLShaderImpl::EnableVertexAttribArray( gvColorHandle );

			glDrawArrays(mode, 0, count);

			OpenGLShaderImpl::DisableVertexAttribArray( gvColorHandle );
			OpenGLShaderImpl::DisableVertexAttribArray( gvPositionHandle );
			OpenGLShader::ClearShader( );
		}
	}


#endif
