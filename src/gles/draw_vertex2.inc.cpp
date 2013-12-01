/****************************
Copyright (c) 2012 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#ifndef _GLES_DRAW_
	#define _GLES_DRAW_

	#include "gles.hpp"


	GLuint gvPositionHandle;
	GLuint gvColorHandle;

	namespace gles {
		void render( GLenum mode, LuxVertex * vertexs, uint16_t count, Texture * texture, LuxVertex dest, LuxColour * colour, float angle, LuxVertex scale, LuxVertex rotation, uint8_t shader )
		{
			std::cout << "Lux_GLES_Draw" << std::endl;
			if ( shaders[SHADER_DEFAULT].program == 0)
			{
				lux::OpenGLShader::CompileShader( &shaders[SHADER_DEFAULT] );
			}
			OpenGLShaderImpl::UseProgramObject( shaders[SHADER_DEFAULT].program );
			OpenGLShader::ApplyShader( shader );

			glVertexAttribPointer(gvPositionHandle, 5, GL_FLOAT, GL_FALSE, 0, vertexs);

			glVertexAttribPointer(gvColorHandle, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, colour);
			glEnableVertexAttribArray( gvPositionHandle );
			glEnableVertexAttribArray( gvColorHandle );

			glDrawArrays(mode, 0, count);

			glDisableVertexAttribArray( gvColorHandle );
			glDisableVertexAttribArray( gvPositionHandle );
			OpenGLShader::ClearShader( 0 );
		}
	}

#endif
