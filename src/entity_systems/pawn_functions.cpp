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
#include "display.h"
#include "engine.h"
#include "mokoi_game.h"
#include "entity_manager.h"
#include "pawn_helper.h"
#include "game_system.h"
#include "elix_string.hpp"
#include "save_system.h"

#include "ffi_functions.h"


extern const AMX_NATIVE_INFO Functions_Natives[];

/** Audio Functions */

/**
 * @brief pawnAudioPlayMusic
 * @param amx
 * @param params (file{}, loops = 1, fadeLength = 0)
 * @return
 */
static cell pawnAudioPlayMusic(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 3 );

	int32_t loops = params[2];
	int32_t fadeLength = params[3];
	std::string pname = Lux_PawnEntity_GetString(amx, params[1]);

	return Lux_FFI_Audio_Play_Music( pname.c_str(), loops, fadeLength );

}

/**
 * @brief pawnAudioPlaySound
 * @param amx
 * @param params (file{}, x = -1, y = -1);
 * @return
 */
static cell pawnAudioPlaySound(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 1 );

	std::string pname = Lux_PawnEntity_GetString(amx, params[1]);

	return Lux_FFI_Audio_Play_Sound( pname.c_str(), -1, -1 );
}

/**
 * @brief pawnAudioPlayDialog
 * @param amx
 * @param params (line)
 * @return
 */
static cell pawnAudioPlayDialog(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 1 );

	return Lux_FFI_Audio_Play_Dialog( (uint32_t)params[1] );
}

/** Misc Functions*/

/**
 * @brief pawnFixedToFloat
 * @param amx
 * @param params (value)
 * @return
 */
static cell pawnFixedToFloat(AMX *amx, const cell *params)
{
	float result = 0.0f;
	result = MAKE_FIXED_FLOAT(params[1]);
	return amx_ftoc(result);
}

/**
 * @brief pawnFloatToFixed
 * @param amx
 * @param params (value)
 * @return
 */
static cell pawnFloatToFixed(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 1 );

	float result = amx_ctof(params[1]);
	return MAKE_FLOAT_FIXED(result);
}

/**
 * @brief pawnStringHash
 * @param amx
 * @param params (name{})
 * @return
 */
static cell pawnStringHash(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 1 );

	std::string s = Lux_PawnEntity_GetString(amx, params[1]);

	return (cell)elix::string::Hash( s );
}

const AMX_NATIVE_INFO Functions_Natives[] = {

	/** Audio Functions */
	{ "AudioPlayDialog", pawnAudioPlayDialog}, ///native AudioPlayDialog(line);
	{ "AudioPlayMusic", pawnAudioPlayMusic}, ///native AudioPlayMusic(file[], loop, fadeLength);
	{ "AudioPlaySound", pawnAudioPlaySound}, ///native AudioPlaySound(file[], x = -1, y = -1);

	/** Misc Functions */
	{ "FixedToFloat", pawnFixedToFloat}, ///native FixedToFloat(value);
	{ "FloatToFixed", pawnFloatToFixed}, ///native FloatToFixed(value);
	{ "StringHash", pawnStringHash}, ///native StringHash(name{});
	{ "EntityHash", pawnStringHash}, ///native EntityHash(name{});
	{ 0, 0 }
};
