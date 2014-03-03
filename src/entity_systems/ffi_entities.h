/****************************
Copyright © 2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/


#ifndef FFI_ENTITIES_H
#define FFI_ENTITIES_H


#include <stdint.h>

uint8_t Lux_FFI_Entity_Set_Position( uint32_t hash_entity, int32_t fixed_x, int32_t fixed_y, int32_t fixed_z  );
uint8_t Lux_FFI_Entity_Get_Position( uint32_t hash_entity, int32_t * fixed_x, int32_t * fixed_y, int32_t * fixed_z );
char * Lux_FFI_Entity_Get_Setting( uint32_t hash_entity, const char * key );
int32_t Lux_FFI_Entity_Get_Setting_Number(uint32_t hash_entity, const char * key);
uint32_t Lux_FFI_Entity_Create( const char * script_file, const char * ident, int32_t x, int32_t y, int32_t z, uint32_t map_id );
uint8_t Lux_FFI_Entity_Delete( uint32_t hash_entity );

uint32_t Lux_FFI_Entities_List( uint32_t map_id );
char * Lux_FFI_Entities_Next( uint32_t map_id );

#endif // FFI_ENTITIES_H



