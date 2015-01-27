/****************************
Copyright © 2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#ifndef FFI_OBJECT_H
#define FFI_OBJECT_H

#include <stdint.h>


uint32_t Lux_FFI_Object_Create( const uint8_t global, const uint8_t type, const int32_t x, const int32_t y, const int32_t z, const uint16_t w, const uint16_t h, const uint32_t colour, const char * sprite );
int32_t Lux_FFI_Object_Position(uint32_t object_id, const int32_t x, const int32_t y, const int32_t z, const uint16_t w, const uint16_t h );
int32_t Lux_FFI_Object_Info( const uint32_t object_id, uint16_t * w, uint16_t * h, int32_t * x, int32_t * y );
int32_t Lux_FFI_Object_Effect( uint32_t object_id, uint32_t primary_colour, uint32_t secondary_colour, uint16_t rotation, uint16_t scale_xaxis, uint16_t scale_yaxis, uint8_t flip_image, uint8_t style );
int32_t Lux_FFI_Object_Replace( uint32_t object_id, uint8_t type, const char * sprite );
int16_t Lux_FFI_Object_Flag( uint32_t object_id, uint8_t key, int16_t value );
uint8_t Lux_FFI_Object_Delete( uint32_t object_id );


uint32_t Lux_FFI_Animation_Length( const char * sheet, const char * sprite );
uint32_t Lux_FFI_Animation_Create( const char * animation_name );
uint32_t Lux_FFI_Animation_Insert( const char * animation_name, const char * sprite, const uint32_t time_ms );

void Lux_FFI_Text_Sprites( const uint8_t able, const char * sheet );

uint32_t Lux_FFI_Polygon_Create( const char * name );
uint32_t Lux_FFI_Polygon_Add_Point( const char * name, int32_t x, int32_t y );

uint32_t Lux_FFI_Canvas_Child_Info(uint32_t object_id, uint32_t child_id, int32_t * x, int32_t * y, uint16_t * w, uint16_t * h  );
int32_t Lux_FFI_Canvas_Child_Set_Effect( uint32_t object_id, uint32_t child_id, uint32_t primary_colour, uint32_t secondary_colour, uint16_t rotation, uint16_t scale_xaxis, uint16_t scale_yaxis, uint8_t flip_image, uint8_t style );
#endif // FFI_OBJECT_H
