/****************************
Copyright © 2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include "ffi_functions.h"

#include "game_system.h"
#include "audio.h"
#include "elix_string.hpp"

/**
 * @brief Lux_FFI_Object_Get
 * @param object_id
 * @return
 */
MapObject * Lux_FFI_Object_Get( uint32_t object_id )
{
	MapObject * object = NULL;

	if ( lux::gamesystem && object_id )
	{
		object = lux::gamesystem->GetObject(object_id);
	}
	return object;
}

/**
 * @brief Lux_FFI_Audio_Play_Music
 * @param file
 * @param loops
 * @param fadeLength
 * @return
 */
int32_t Lux_FFI_Audio_Play_Music( const char * file, int32_t loops, int32_t fadeLength )
{
	return lux::audio->PlayMusic( file, loops, fadeLength );
}

/**
 * @brief Lux_FFI_Audio_Play_Sound
 * @param file
 * @param x
 * @param y
 * @return
 */
int32_t Lux_FFI_Audio_Play_Sound( const char * file, int32_t x, int32_t y  )
{
	return lux::audio->PlayEffect( file, x, y );
}

/**
 * @brief Lux_FFI_Audio_Play_Dialog
 * @param line
 * @return
 */
int32_t Lux_FFI_Audio_Play_Dialog( uint32_t line )
{
	return lux::audio->PlayDialog( line, 0 );
}

/**
 * @brief Lux_FFI_String_Hash
 * @param string
 * @return
 */
uint32_t Lux_FFI_String_Hash( const char * string )
{
	std::string s = string;
	return elix::string::Hash( s );
}
