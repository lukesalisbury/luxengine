/****************************
Copyright © 2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include "ffi_input.h"
#include "entity_manager.h"
#include "engine.h"

/**
 * @brief Lux_FFI_Keyboard_Watch
 * @param hash_entity
 * @param able
 */
void Lux_FFI_Keyboard_Watch( uint32_t hash_entity, uint8_t able )
{
	if ( able )
	{
		Entity * wanted = lux::entities->GetEntity(hash_entity);
		if ( wanted )
			lux::entities->_keyboard = wanted;
		else
			lux::entities->_keyboard = nullptr;
	}
	else
	{
		lux::entities->_keyboard = nullptr;
	}
}

/**
 * @brief Lux_FFI_Input_Button
 * @param player
 * @param button
 * @return the button value for Player
 */
int16_t Lux_FFI_Input_Button( uint32_t player, uint8_t button )
{
	if ( player > 0 )
		return lux::engine->GetPlayerButton( player, button );
	else if ( player == 0 )
		return lux::engine->GetPlayerButton( lux::engine->default_player, button );
	return 0;
}

/**
 * @brief Lux_FFI_Input_Axis
 * @param player
 * @param axis
 * @return Axis value for Player
 */
int16_t Lux_FFI_Input_Axis( uint32_t player, uint8_t axis )
{
	if ( player > 0 )
		return lux::engine->GetPlayerAxis( player, axis );
	else if ( player == 0 )
		return lux::engine->GetPlayerAxis( lux::engine->default_player, axis );
	return 0;
}

/**
 * @brief Lux_FFI_Input_Pointer
 * @param player
 * @param axis
 * @return Pointer value for Player
 */
int16_t Lux_FFI_Input_Pointer( uint32_t player, uint8_t axis )
{

	if ( player > 0 )
		return lux::engine->GetPlayerPointer( player, axis );
	else if ( player == 0 )
		return lux::engine->GetPlayerPointer( lux::engine->default_player, axis );
	return 0;
}

/**
 * @brief Set Button value for the Player
 * @param player
 * @param button
 * @param value
 * @return
 */
int16_t Lux_FFI_Input_Button_Set( uint32_t player, uint8_t button, uint16_t value )
{

	if ( player > 0 )
		lux::engine->SetPlayerButton( player, value, button );
	else if ( player == 0 )
		lux::engine->SetPlayerButton( lux::engine->default_player, value, button );
	return 0;
}

/**
 * @brief Set Axis value for the Player
 * @param player
 * @param axis
 * @param value
 * @return
 */
int16_t Lux_FFI_Input_Axis_Set( uint32_t player, uint8_t axis, uint16_t value )
{

	if ( player > 0 )
		lux::engine->SetPlayerAxis( player, value, axis );
	else if ( player == 0 )
		lux::engine->SetPlayerAxis( lux::engine->default_player, value, axis );
	return 0;
}

/**
 * @brief Set Pointer value for the Player
 * @param player
 * @param axis
 * @param value
 * @return
 */
int16_t Lux_FFI_Input_Pointer_Set( uint32_t player, uint8_t axis, uint16_t value )
{
	if ( player == 0 )
	{
		lux::engine->SetPlayerPointer( lux::engine->default_player, value, axis );
	}
	else
	{
		lux::engine->SetPlayerPointer( player, value, axis );
	}
	return 0;
}

/**
 * @brief Set the default player
 * @param player
 * @return Default Player
 */
int16_t Lux_FFI_Input_Set_Default( uint32_t player )
{
	lux::engine->SetDefaultPlayer( player );
	return lux::engine->default_player;
}
