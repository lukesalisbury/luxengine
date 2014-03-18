/****************************
Copyright © 2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#ifndef FFI_SHADERS_H
#define FFI_SHADERS_H

#include <stdint.h>

int32_t Lux_FFI_Layer_Apply_Shader( uint32_t layer, uint8_t shader );
int32_t Lux_FFI_Object_Apply_Shader( uint32_t object_id, uint8_t shader );
int32_t Lux_FFI_Shaders_Create( uint8_t shader, char * vertex_shader, char * fragment_shader );













#endif // FFI_SHADERS_H
