/****************************
Copyright © 2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#ifndef FFI_GAME_H
#define FFI_GAME_H

#include <stdint.h>

int32_t Lux_FFI_Game_State( int32_t state );
uint32_t Lux_FFI_Game_Frame();

uint8_t Lux_FFI_Game_Save( uint8_t save_slot, int32_t detail[64] );
uint8_t Lux_FFI_Game_Save_Hibernate( );
uint8_t Lux_FFI_Game_Save_Exist( uint8_t save_slot );
uint8_t Lux_FFI_Game_Save_Remove( uint8_t save_slot );
uint8_t Lux_FFI_Game_Load( uint8_t save_slot );
int32_t * Lux_FFI_Game_Details( const uint32_t project_id, const uint8_t save_slot, uint8_t * data_size );

const char * Lux_FFI_Language_String( uint32_t line );
uint8_t Lux_FFI_Language_Set( const char * language );

const char *  Lux_FFI_Dialog_String( uint32_t line );

#endif // FFI_GAME_H
