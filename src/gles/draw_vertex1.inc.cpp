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

	namespace gles {
		void render( GLenum mode, LuxVertex * vertexs, uint16_t count, Texture * texture, LuxVertex dest, LuxColour * colour, float angle, LuxVertex scale, LuxVertex rotation, uint8_t shader )
		{

			glPushMatrix();
			glTranslatef(dest.x, dest.y, dest.z);

			glScalef(scale.x, scale.y, scale.z);
			glRotatef(rotation.x, 1.0f, 0.0f, 0.0f);
			glRotatef(rotation.y, 0.0f, 1.0f, 0.0f);
			glRotatef(rotation.z, 0.0f, 0.0f, 1.0f);

			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(3, GL_FLOAT, sizeof(LuxVertex), vertexs);

			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(4, GL_UNSIGNED_BYTE, 0, colour);



			if ( texture )
			{
				glEnable(GL_TEXTURE_2D);
				OpenGLShader::ApplyShader( shader );
				glBindTexture(GL_TEXTURE_2D, texture->pointer );
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glTexCoordPointer(2, GL_FLOAT, sizeof(LuxVertex), &vertexs[0].u );
				glDrawArrays(mode, 0, count);

				glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				OpenGLShader::ClearShader( );
				glDisable(GL_TEXTURE_2D);

			}
			else
			{
				glDrawArrays(mode, 0, count);
			}

			glDisableClientState(GL_COLOR_ARRAY);
			glDisableClientState(GL_VERTEX_ARRAY);
			glPopMatrix();
		}
	}
#endif
