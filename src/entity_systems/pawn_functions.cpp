/****************************
Copyright © 2006-2011 Luke Salisbury
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
#include "world.h"
#include "elix_string.hpp"
#include "save_system.h"

extern const AMX_NATIVE_INFO Functions_Natives[];



/** Audio Functions */

/** pawnAudioPlayMusic
* native AudioPlayMusic(file[], loop = 1, fadeLength = 0);
*
*/
static cell AMX_NATIVE_CALL pawnAudioPlayMusic(AMX *amx, const cell *params)
{
	std::string pname = Lux_PawnEntity_GetString(amx, params[1]);
	if ( pname.length() )
		return (cell) lux::audio->PlayMusic ( pname, params[2], params[3] );
	return 0;
}

/** pawnAudioPlaySound
* native AudioPlaySound(file[], x = -1, y = -1);
*
*/
static cell AMX_NATIVE_CALL pawnAudioPlaySound(AMX *amx, const cell *params)
{
	std::string pname = Lux_PawnEntity_GetString(amx, params[1]);
	if ( pname.length() )
		return (cell) lux::audio->PlayEffect ( pname, params[2], params[3] );
	return 0;
}

/** pawnAudioPlayDialog
* native AudioPlayDialog(line);
*
*/
static cell AMX_NATIVE_CALL pawnAudioPlayDialog(AMX *amx, const cell *params)
{
	return lux::audio->PlayDialog( params[1] );
}

/** Misc Functions*/

/** pawnFixedToFloat
* native FixedToFloat(value);
*
*/
static cell AMX_NATIVE_CALL pawnFixedToFloat(AMX *amx, const cell *params)
{
	float result = 0.0f;
	result = MAKE_FIXED_FLOAT(params[1]);
	return amx_ftoc(result);
}

/** pawnFloatToFixed
* native FloatToFixed(value);
*
*/
static cell AMX_NATIVE_CALL pawnFloatToFixed(AMX *amx, const cell *params)
{
	float result = amx_ctof(params[1]);
	return MAKE_FLOAT_FIXED(result);
}

/** pawnStringHash
* native StringHash(name[]);
*
*/
static cell AMX_NATIVE_CALL pawnStringHash(AMX *amx, const cell *params)
{
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
	{ "StringHash", pawnStringHash}, ///native StringHash(name[]);
	{ "EntityHash", pawnStringHash}, ///native EntityHash(name[]);
	{ 0, 0 }
};
