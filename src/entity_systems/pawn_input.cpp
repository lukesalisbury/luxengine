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

extern const AMX_NATIVE_INFO Input_Natives[];


/** Input Functions */

/** pawnKeyboardWatch
* native KeyboardWatch( able );
*
*/
static cell pawnKeyboardWatch(AMX *amx, const cell *params)
{
	if ( params[1] )
		lux::entities->_keyboard = Lux_PawnEntity_GetParent(amx);
	else
		lux::entities->_keyboard = NULL;
	return 0;
}



/** pawnInputButton
* native InputButton(input, player = 0);
* Returns the button value for Player
*/
static cell pawnInputButton(AMX *amx, const cell *params)
{
	return Lux_FFI_Input_Button((uint32_t)params[2], (uint8_t)params[1]);
}

/** pawnInputAxis
* native InputAxis(input, player = 0);
* Returns Axis value for Player
*/
static cell pawnInputAxis(AMX *amx, const cell *params)
{
	return Lux_FFI_Input_Axis((uint32_t)params[2], (uint8_t)params[1]);
}

/** pawnInputPointer
* native InputPointer(input, player = 0);
* Returns Pointer value for Player
*/
static cell pawnInputPointer(AMX *amx, const cell *params)
{
	return Lux_FFI_Input_Pointer((uint32_t)params[2], (uint8_t)params[1]);
}

/** pawnInputButtonSet
* native InputButtonSet(input, value, player = 0);
* Set Button value for the Player
*/
static cell pawnInputButtonSet(AMX *amx, const cell *params)
{
	return Lux_FFI_Input_Button_Set((uint32_t)params[3], (uint8_t)params[1], (int16_t)params[2]);
}

/** pawnInputAxisSet
* native InputAxisSet(input, value, player = 0);
* Set Axis value for the Player
*/
static cell pawnInputAxisSet(AMX *amx, const cell *params)
{
	return Lux_FFI_Input_Axis_Set((uint32_t)params[3], (uint8_t)params[1], (int16_t)params[2]);
}

/** pawnInputPointerSet
* native InputPointerSet(input, value, player = 0);
* Set Pointer value for the Player
*/
static cell pawnInputPointerSet(AMX *amx, const cell *params)
{
	return Lux_FFI_Input_Pointer_Set((uint32_t)params[3], (uint8_t)params[1], (int16_t)params[2]);
}

/** pawnInputSetDefault
* native InputSetDefault(player);
* Set the default player
*/
static cell pawnInputSetDefault(AMX *amx, const cell *params)
{
	return Lux_FFI_Input_Set_Default( (uint32_t)params[1] );
}

const AMX_NATIVE_INFO Input_Natives[] = {

	/** Input Functions */
	{ "KeyboardWatch", pawnKeyboardWatch },
	{ "InputButton",  pawnInputButton },
	{ "InputAxis",  pawnInputAxis },
	{ "InputPointer",  pawnInputPointer },
	{ "InputButtonSet",  pawnInputButtonSet },
	{ "InputAxisSet",  pawnInputAxisSet },
	{ "InputPointerSet",  pawnInputPointerSet },
	{ "InputSetDefault",  pawnInputSetDefault },

	{ 0, 0 }
};
