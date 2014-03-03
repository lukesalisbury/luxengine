/****************************
Copyright © 2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#ifndef FFI_LAYER_H
#define FFI_LAYER_H

#include <stdint.h>

void Lux_FFI_Layer_Rotation( int8_t layer, int16_t roll, int16_t pitch, int16_t yaw );
void Lux_FFI_Layer_Offset( int8_t layer, int32_t x, int32_t y );
void Lux_FFI_Layer_Colour( int8_t layer, uint32_t colour );

#endif // FFI_LAYER_H
