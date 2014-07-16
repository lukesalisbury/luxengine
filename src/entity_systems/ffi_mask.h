/****************************
Copyright © 2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#ifndef FFI_MASK_H
#define FFI_MASK_H

#include <stdint.h>

uint8_t Lux_FFI_Mask_Refresh();
int32_t Lux_FFI_Mask_Value_Get( uint16_t x, uint16_t y, uint8_t layer );
int32_t Lux_FFI_Mask_Value_Fill( uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t value );

#endif // FFI_MASK_H
