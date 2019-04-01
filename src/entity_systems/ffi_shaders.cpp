/****************************
Copyright © 2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include "ffi_shaders.h"
#include "stdheader.h"
#include "map_object.h"

#include "display/display.h"
#include "display/layers.h"


/**
 * @brief Lux_FFI_Layer_Apply_Shader
 * @param layer
 * @param shader
 * @return
 */
int32_t Lux_FFI_Layer_Apply_Shader( uint32_t layer, uint8_t shader )
{
	if ( lux::display == nullptr )
		return 0;

	Layer * requested_layer = nullptr;

	requested_layer = lux::display->GetLayer( layer );

	if ( requested_layer != nullptr )
	{
		requested_layer->SetShader( shader );
		return 1;
	}
	else
	{
		return 0;
	}

}

/**
 * @brief Lux_FFI_Object_Apply_Shader
 * @param object_id
 * @param shader
 * @return
 */
int32_t Lux_FFI_Object_Apply_Shader(uint32_t object_id, uint8_t shader )
{
	return 0;
}

/**
 * @brief Lux_FFI_Shaders_Create
 * @param shader
 * @param vertex_shader
 * @param fragment_shader
 * @return
 */
int32_t Lux_FFI_Shaders_Create( uint8_t shader, char * vertex_shader, char * fragment_shader )
{
	return 0;
}



