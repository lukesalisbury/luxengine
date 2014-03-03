/****************************
Copyright © 2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#ifndef FFI_COLLISIONS_H
#define FFI_COLLISIONS_H

#include <stdint.h>

int32_t Lux_FFI_Collision_Set( uint32_t hash_entity, int32_t rect, int32_t type, int32_t x, int32_t y, int32_t w, int32_t h );
uint8_t Lux_FFI_Collision_Get( uint32_t hash_entity, int32_t rect, int32_t * x, int32_t * y, int32_t * w, int32_t * h );
int32_t Lux_FFI_Collision_Check( uint32_t hash_entity_first, uint32_t hash_entity_second, int32_t rect_first, int32_t rect_second );
int32_t Lux_FFI_Collision_Calculate( uint32_t hash_entity );
int32_t Lux_FFI_Collision_Calculate_Current( uint32_t hash_entity, uint32_t * entity_hit, int32_t * angle, int32_t * dist, int32_t * rect, int32_t * type );
int32_t Lux_FFI_Collision_Set_From_Object( uint32_t hash_entity, uint32_t object_id, int32_t type );



#endif // FFI_COLLISIONS_H
