/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "audio.h"
#include "game_config.h"
#include "core.h"
#include "display/display.h"
#include "engine.h"
#include "mokoi_game.h"
#include "entity_manager.h"
#include "pawn_helper.h"
#include "game_system.h"
#include "elix/elix_string.hpp"
#include "save_system.h"

#include "ffi_game.h"
#include "ffi_input.h"

extern const AMX_NATIVE_INFO Game_Natives[];


/** Game Functions */


/** pawnGameState
* native GameState(newstate = -1);
* Get/Set Global State.
*/
static cell pawnGameState(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 1 );

	return (cell)Lux_FFI_Game_State((int32_t) params[1]);
}

/** pawnGameFrame
* native GameFrame();
* Return the length of last frame in milliseconds.
*/
static cell pawnGameFrame(AMX *amx, const cell *params)
{
	return (cell)Lux_FFI_Game_Frame();
}

/** pawnGameSave
* native GameSave(slot, detail[64],  maxlength=sizeof detail );
* Save Current Game.
*/
static cell pawnGameSave(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 3 );

	if ( params[1] > 254 || params[1] < 0)
		return 0;

	uint8_t requested_save_slot = (uint8_t)params[1];
	int32_t * cookie_data = new int32_t[64];

	cell * cptr = amx_Address(amx, params[2]);
	for ( uint8_t count = 0; count < 64; count++ )
	{
		cookie_data[count] = (int32_t)*cptr;
		cptr++;
	}

	return Lux_FFI_Game_Save( requested_save_slot, cookie_data );

}


/** pawnGameSaveHibernate
* native GameSaveHibernate();
* Save Current Game.
*/
static cell pawnGameSaveHibernate(AMX *amx, const cell *params)
{
	return lux::engine->WriteSaveGame( 0x00, nullptr, 0 );
}

/** pawnGameHasSave
* native GameHasSave(requested_save_slot);
* Save Current Game.
*/
static cell pawnGameHasSave(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 1 );

	uint8_t requested_save_slot = (uint8_t)params[1];

	return Lux_FFI_Game_Save_Exist(requested_save_slot);
}


/** pawnGameRemoveSave
* native GameRemoveSave(requested_save_slot);
* Save Current Game.
*/
static cell pawnGameRemoveSave(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 1 );

	uint8_t requested_save_slot = (uint8_t)params[1];

	return Lux_FFI_Game_Save_Remove( requested_save_slot );
}


/** pawnGameLoad
* native GameLoad(slot);
* Load Game from Slot.
*/
static cell pawnGameLoad(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 1 );

	if ( params[1] > 254 || params[1] < 0)
		return 0;

	uint8_t requested_save_slot = (uint8_t)params[1];

	return Lux_FFI_Game_Load( requested_save_slot );
}

/** pawnGameGetDetails
* native GameGetDetails(slot, array[64], bool:pack_array = false, maxlength=sizeof array);
* Get Save Game Details.
*/
static cell pawnGameGetDetails(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 1 );

	if ( params[1] > 254 || params[1] < 0)
		return 0;

	if ( params[4] != 64 )
		return 0;

	cell read_successful = false;
	int32_t * cookie_data = nullptr;
	uint8_t data_size = 0;
	uint8_t requested_save_slot = (uint8_t)params[1];

	cell * cptr = amx_Address(amx, params[2]);

	if ( lux::engine )
	{
		cookie_data = Lux_FFI_Game_Details( lux::game_data->GetProjectIdent(), requested_save_slot, &data_size );
		if ( data_size )
		{
			for (uint8_t count = 0; count < data_size; count++)
			{
				*cptr = cookie_data[count];
				cptr ++;
			}
			read_successful = true;

			delete[] cookie_data;
		}
	}

	return read_successful;
}

/** pawnGameDetails
* native GameDetails(id, slot, array[64], bool:pack_array = false, maxlength=sizeof array);
* Get another games Save Game Details.
*/
static cell pawnGameDetails(AMX *amx, const cell *params)
{
	if ( params[2] > 254 || params[2] < 0)
		return 0;

	if ( params[5] != 64 )
		return 0;

	cell read_successful = false;
	uint32_t game_id = (uint32_t)params[1];
	uint8_t requested_save_slot = (uint8_t)params[2];
	int32_t * cookie_data = nullptr;
	uint8_t data_size = 0;


	cell * cptr = amx_Address(amx, params[3]);

	if ( lux::engine )
	{
		cookie_data = Lux_FFI_Game_Details( game_id, requested_save_slot, &data_size );
		if ( data_size )
		{
			for (uint8_t count = 0; count < 64; count++)
			{
				*cptr = cookie_data[count];
				cptr ++;
			}

			delete[] cookie_data;
			read_successful = true;
		}
	}



	return read_successful;
}

/** Language Functions */

/** pawnLanguageString
* native LanguageString(line, returnstring[], maxlength=sizeof returnstring);
*
*/
static cell pawnLanguageString(AMX *amx, const cell *params)
{
	std::string dialog_string;
	cell * cptr;
	cptr = amx_Address(amx, params[2]);

	dialog_string = lux::engine->GetString(params[1]);
	return amx_SetString(cptr, dialog_string.c_str(), 1, 0, params[3]);
}

/** pawnLanguageSet
* native LanguageSet(lang[]);
*
*/
static cell pawnLanguageSet(AMX *amx, const cell *params)
{
	char * lang_string;
	amx_StrParam_Type( amx, params[1], lang_string, char*);
	return (cell)lux::engine->LoadLanguage(lang_string);
	return 0;
}

/** Dialog Functions*/

/** pawnDialogShow
* native DialogShow(line, callback[] = "dialogbox");
* Calls 'callback'(line, string[]);
*/
static cell pawnDialogShow(AMX *amx, const cell *params)
{
	char * function_name;
	Entity * wanted_entity = lux::entities->_global;
	if ( wanted_entity != nullptr )
	{
		amx_StrParam_Type(amx, params[2], function_name, char*);
		wanted_entity->Call(function_name, (char*)"d", params[1]);
		/* Todo
		- Pass (parsed) Dialog string instead
		*/
		return 1;
	}
	return 0;
}



/** pawnDialogGetString
* native DialogGetString(line, returnstring{}, maxlength=sizeof returnstring);
*
*/
static cell pawnDialogGetString(AMX *amx, const cell *params)
{
	const char * dialog_string;
	cell * cptr;
	cptr = amx_Address(amx, params[2]);

	dialog_string = Lux_FFI_Dialog_String( (uint32_t)params[1] );

	return Lux_PawnEntity_SetString(cptr, dialog_string, params[3]*sizeof(cell) );
}


const AMX_NATIVE_INFO Game_Natives[] = {
	/** Engine Functions */
	{ "GameState", pawnGameState },
	{ "GameFrame", pawnGameFrame },

	/** Save Functions */
	{ "GameLoad", pawnGameLoad },
	{ "GameSave", pawnGameSave },
	{ "GameSaveHibernate", pawnGameSaveHibernate },
	{ "GameHasSave", pawnGameHasSave },
	{ "GameRemoveSave", pawnGameRemoveSave },
	{ "GameGetDetails", pawnGameGetDetails },
	{ "GameDetails", pawnGameDetails },

	/** Language Functions */
	{ "LanguageString", pawnLanguageString },
	{ "LanguageSet", pawnLanguageSet },

	/** Dialog Functions*/
	{ "DialogShow", pawnDialogShow},///native DialogShow(line, function[] = "dialogbox");
	{ "DialogGetString", pawnDialogGetString},///native DialogGetString(line, returnstring[], maxlength=sizeof returnstring);

	{ 0, 0 }
};
