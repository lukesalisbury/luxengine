/****************************
Copyright © 2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#ifndef FFI_INPUT_H
#define FFI_INPUT_H

#include <stdint.h>
void Lux_FFI_Keyboard_Watch( uint32_t hash_entity, uint8_t able );

int16_t Lux_FFI_Input_Button( uint32_t player, uint8_t button );
int16_t Lux_FFI_Input_Axis( uint32_t player, uint8_t axis );
int16_t Lux_FFI_Input_Pointer( uint32_t player, uint8_t axis );
int16_t Lux_FFI_Input_Button_Set( uint32_t player, uint8_t button, uint16_t value );
int16_t Lux_FFI_Input_Axis_Set( uint32_t player, uint8_t axis, uint16_t value );
int16_t Lux_FFI_Input_Pointer_Set( uint32_t player, uint8_t axis, uint16_t value );

int16_t Lux_FFI_Input_Set_Default( uint32_t player );


#endif // FFI_INPUT_H
