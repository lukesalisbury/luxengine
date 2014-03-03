/****************************
Copyright © 2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/


#ifndef FFI_PLAYER_H
#define FFI_PLAYER_H

#include <stdint.h>

uint8_t Lux_FFI_Player_Set_Controller( uint32_t player_number, const char * controller_name );
char * Lux_FFI_Player_Get_Controller( uint32_t player_number );
uint8_t Lux_FFI_Player_Set_Colour( uint32_t player_number );
uint8_t Lux_FFI_Player_Set_Entity( uint32_t player_number, uint32_t hash_entity );
uint8_t Lux_FFI_Player_Set_Name( uint32_t player_number, const char * player_name );
char * Lux_FFI_Player_Get_Name( uint32_t player_number );

char * Lux_FFI_Controller_Config( const char * controller_name );







#endif // FFI_PLAYER_H
