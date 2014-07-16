/****************************
Copyright © 2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#ifndef FFI_MAP_H
#define FFI_MAP_H


#include <stdint.h>

void Lux_FFI_Map_Offset_Set( int32_t x, int32_t y);
int32_t Lux_FFI_Map_Offset_Get( uint8_t axis );

uint8_t Lux_FFI_Map_Set( const uint32_t map_ident, const int32_t offset_x, const int32_t offset_y );
uint32_t Lux_FFI_Map_Get_Ident( const char * map_file, uint8_t create_new );
uint8_t Lux_FFI_Map_Reset();
uint8_t Lux_FFI_Map_Snapshot();
uint8_t Lux_FFI_Map_Delete( const uint32_t map_ident );

uint32_t Lux_FFI_Map_Edit_New( const char * map_file, const uint32_t section_hash, uint32_t width, uint32_t height);
uint8_t Lux_FFI_Map_Edit_Save( const uint32_t map_ident );

uint32_t Lux_FFI_Map_Edit_Object_Create( const uint32_t map_ident, const uint8_t type, const int32_t x, const int32_t y, const int32_t z, const uint16_t w, const uint16_t h, const uint32_t colour, const char * sprite );
int32_t Lux_FFI_Map_Edit_Object_Postion( const uint32_t map_ident, uint32_t object_id, const int32_t x, const int32_t y, const int32_t z, const uint16_t w, const uint16_t h );
int32_t Lux_FFI_Map_Edit_Object_Effect( const uint32_t map_ident,  uint32_t object_id, uint32_t primary_colour, uint32_t secondary_colour, uint16_t rotation, uint16_t scale_xaxis, uint16_t scale_yaxis, uint8_t flip_image, uint8_t style );
int32_t Lux_FFI_Map_Edit_Object_Replace( const uint32_t map_ident, uint32_t object_id, uint8_t type, const char * sprite );
int16_t Lux_FFI_Map_Edit_Object_Flag( const uint32_t map_ident, uint32_t object_id, uint8_t key, int16_t value );
uint8_t Lux_FFI_Map_Edit_Object_Delete( const uint32_t map_ident, uint32_t object_id );





#endif // FFI_MAP_H
