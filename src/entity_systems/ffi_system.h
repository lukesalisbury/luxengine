/****************************
Copyright © 2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#ifndef FFI_SYSTEM_H
#define FFI_SYSTEM_H

#include <stdint.h>

int8_t Lux_FFI_System_Audio_Volume( const uint8_t device, const int8_t level );
int32_t Lux_FFI_System_Command( const uint8_t command, const int32_t value );
uint32_t Lux_FFI_System_List_Count( const uint8_t list );
const char * Lux_FFI_System_List_Item( const uint8_t list, const uint32_t item );

const char * Lux_FFI_Config_String_Get( const char * key );
int8_t Lux_FFI_Config_String_Set( const char * key, const char * value );
int32_t Lux_FFI_Config_Number_Get( const char * key );
int8_t Lux_FFI_Config_Number_Set( const char * key, const int32_t value );
int8_t Lux_FFI_Config_Save();


#endif // FFI_SYSTEM_H
