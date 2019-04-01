/****************************
Copyright © 2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "ffi_game.h"

#include "engine.h"
#include "save_system.h"
/**
 * @brief Lux_FFI_Game_State Get/Set Global State.
 * @param state
 * @return Global State
 */
int32_t Lux_FFI_Game_State( int32_t state )
{
	return lux::engine->GameState( state );
}

/**
 * @brief Lux_FFI_Game_Frame
 * @return length of last frame in milliseconds.
 */
uint32_t Lux_FFI_Game_Frame()
{
	uint32_t time = lux::core->GetFrameDelta();
	return time;
}

/**
 * @brief Lux_FFI_Game_GameSave Save Current Game.
 * @param save_slot
 * @param detail
 * @return
 */
uint8_t Lux_FFI_Game_Save( uint8_t save_slot, int32_t detail[64] )
{
	return lux::engine->WriteSaveGame( save_slot, detail, 64 );
}

/**
 * @brief Lux_FFI_Game_Save_Hibernate
 * @return
 */
uint8_t Lux_FFI_Game_Save_Hibernate( )
{
	return lux::engine->WriteSaveGame( 0x00, nullptr, 0 );
}

/**
 * @brief Lux_FFI_Game_Save_Exist
 * @param save_slot
 * @return
 */
uint8_t Lux_FFI_Game_Save_Exist( uint8_t save_slot )
{
	LuxSaveState save_check;
	return save_check.Exists( save_slot );
}

/**
 * @brief Lux_FFI_Game_Save_Remove
 * @param save_slot
 * @return
 */
uint8_t Lux_FFI_Game_Save_Remove( uint8_t save_slot )
{
	return false;
}

/**
 * @brief Lux_FFI_Game_Load
 * @param save_slot
 * @return
 */
uint8_t Lux_FFI_Game_Load( uint8_t save_slot )
{
	return lux::engine->RestoreSaveGame( save_slot );
}

/**
 * @brief Lux_FFI_Game_Details
 * @param project_id
 * @param save_slot
 * @param data_size
 * @return
 */
int32_t * Lux_FFI_Game_Details( const uint32_t project_id, const uint8_t save_slot, uint8_t * data_size )
{
	int32_t * cookie_data = new int32_t[64];

	*data_size = 64;

	if ( !lux::engine->ReadSaveInfo( save_slot, cookie_data, 64, project_id) )
	{
		*data_size = 0;
		delete [] cookie_data;
		return nullptr;
	}
	return cookie_data;
}




/** Language Functions */

/**
 * @brief Lux_FFI_Language_String
 * @param line
 * @return
 */
const char * Lux_FFI_Language_String( uint32_t line )
{
	std::string dialog_string = lux::engine->GetString(line);
	return dialog_string.c_str();
}

/**
 * @brief Lux_FFI_Language_Set
 * @param language
 * @return
 */
uint8_t Lux_FFI_Language_Set( const char * language )
{
	return lux::engine->LoadLanguage(language);
}


/**
 * @brief Lux_FFI_Dialog_String
 * @param line
 * @return
 */
const char *  Lux_FFI_Dialog_String( uint32_t line )
{
	std::string dialog_string;
	dialog_string = lux::engine->GetDialogString(line);
	return dialog_string.c_str();
}

