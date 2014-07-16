/****************************
Copyright © 2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include "ffi_system.h"
#include "config.h"
#include "game_config.h"
#include "engine.h"
#include "display.h"
#include "audio.h"

#define	VOLUME_MASTER 0
#define	VOLUME_EFFECTS 1
#define	VOLUME_MUSIC 2
#define	VOLUME_DIALOG 3

#define	COMMAND_EXIT 1
#define	COMMAND_RESET 2
#define	COMMAND_RESOLUTION 3

int8_t Lux_FFI_System_Audio_Volume( const uint8_t device, const int8_t level )
{
	if ( device == VOLUME_MUSIC )
	{
		return lux::audio->SetMusicVolume( level );
	}
	else if ( device == VOLUME_EFFECTS )
	{
		return lux::audio->SetEffectsVolume( level );
	}
	else if ( device == VOLUME_MASTER )
	{
		return lux::audio->SetMasterVolume( level );
	}
	else
	{
		return -1;
	}
}

int32_t Lux_FFI_System_Command( const uint8_t command, const int32_t value )
{
	int32_t results = 0;
	switch ( command )
	{
		case COMMAND_EXIT:
		{
			lux::engine->SetState(EXITING);
			results = 1;
			break;
		}
		case COMMAND_RESET:
		{
			lux::engine->SetState(RESETGAME);
			results = 1;
			break;
		}
		case COMMAND_RESOLUTION:
		{
			lux::display->UpdateResolution();
			results = 1;
			break;
		}
	}

	return results;
}


uint32_t Lux_FFI_System_List_Count( const uint8_t list )
{
	return 0;
}

const char * Lux_FFI_System_List_Item( const uint8_t list, const uint32_t item )
{
	return NULL;
}


const char * Lux_FFI_Config_String_Get( const char * key )
{
	std::string value_string;

	value_string = lux::config->GetString( key, false );

	return value_string.c_str();
}

int8_t Lux_FFI_Config_String_Set( const char * key, const char * value )
{
	lux::config->SetString(key, value);
	return 0;
}

int32_t Lux_FFI_Config_Number_Get( const char * key )
{
	return lux::config->GetNumber( key );
}

int8_t Lux_FFI_Config_Number_Set( const char * key, const int32_t value )
{
	lux::config->SetNumber(key, value);
	return 0;
}


int8_t Lux_FFI_Config_Save()
{
	return lux::config->Save();
}



