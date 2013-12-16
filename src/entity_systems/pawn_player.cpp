/****************************
Copyright © 2006-2011 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include "engine.h"
#include "pawn_helper.h"
#include "elix_endian.hpp"
extern const AMX_NATIVE_INFO Player_Natives[];

/** Player Management Functions  */
/** pawnPlayerAdd
* native PlayerAdd();
*
*/
static cell AMX_NATIVE_CALL pawnPlayerAdd(AMX *amx, const cell *params)
{
	return 0;
}
/** pawnPlayerRemove
* native PlayerRemove(ident);
*
*/
static cell AMX_NATIVE_CALL pawnPlayerRemove(AMX *amx, const cell *params)
{
	return 0;
}

/** pawnPlayerCount
* native PlayerCount();
*
*/
static cell AMX_NATIVE_CALL pawnPlayerCount(AMX *amx, const cell *params)
{
	return lux::engine->_players.size();
}
/** pawnPlayerUpdateControl
* native PlayerUpdateControl(player, controlset);
*
*/
static cell AMX_NATIVE_CALL pawnPlayerUpdateControl(AMX *amx, const cell *params)
{
	Player * player = NULL;
	uint32_t p = (uint32_t)params[1];
	uint8_t control_set = (uint8_t)params[2];

	player = lux::engine->GetPlayer( p );
	if( player )
	{
		player->SetControls(control_set);
		return 1;
	}
	return 0;

}


/** pawnPlayerSetColour
* native PlayerSetColour(player, colour);
*
*/
static cell AMX_NATIVE_CALL pawnPlayerSetColour(AMX *amx, const cell *params)
{
	Player * player = NULL;
	uint32_t p = (uint32_t)params[1];
	player = lux::engine->GetPlayer( p );
	if ( player )
	{
		cell_colour colour;
		colour.hex = elix::endian::host32( params[2] );
		player->PlayerColour.primary_colour = colour.rgba;
		return 1;
	}
	return 0;

}

/** pawnSetEntity
* native PlayerSetEntity(player, entity);
*
*/
static cell AMX_NATIVE_CALL pawnPlayerSetEntity(AMX *amx, const cell *params)
{
	Player * player = NULL;
	Entity * wanted = NULL;
	wanted = Lux_PawnEntity_GetEntity(amx, params[2]);
	if ( wanted != NULL )
	{
		uint32_t p = (uint32_t)params[1];
		player = lux::engine->GetPlayer( p );
		if( player )
		{
			player->SetEntity(wanted);
			return 1;
		}
	}
	return 0;
}

/** pawnPlayerSetName
* native PlayerSetName( player, name[] );
*
*/
static cell AMX_NATIVE_CALL pawnPlayerSetName(AMX *amx, const cell *params)
{
	std::string player_new_name;
	Player * player = NULL;
	uint32_t p = (uint32_t)params[1];

	player = lux::engine->GetPlayer( p );
	player_new_name = Lux_PawnEntity_GetString( amx, params[2] );
	if ( player_new_name.length() && player )
	{
		player->SetName(player_new_name);
		return 1;
	}
	return 0;
}

/** pawnPlayerGetName
* native PlayerGetName(player, entity[], s = sizeof(entity) );
*
*/
static cell AMX_NATIVE_CALL pawnPlayerGetName(AMX *amx, const cell *params)
{
	Player * player = NULL;
	uint32_t p = (uint32_t)params[1];

	player = lux::engine->GetPlayer( p );
	if ( player )
	{
		cell * cptr;
		cptr = amx_Address(amx, params[2]);

		return Lux_PawnEntity_SetString(cptr, player->GetName().c_str(), params[3]);
	}
	return 0;
}

const AMX_NATIVE_INFO Player_Natives[] = {
	/** Player Management Functions */
	{ "PlayerAdd", pawnPlayerAdd}, ///native PlayerAdd();
	{ "PlayerRemove", pawnPlayerRemove}, ///native PlayerRemove();
	{ "PlayerCount", pawnPlayerCount}, ///native PlayerCount();
	{ "PlayerUpdateControl", pawnPlayerUpdateControl}, ///native PlayerUpdateControl(player);
	{ "PlayerSetAI", pawnDeprecatedFunction}, ///native PlayerSetAI(player, entity);
	{ "PlayerSetColour", pawnPlayerSetColour},///native PlayerSetColour(player, colour);
	{ "PlayerSetEntity", pawnPlayerSetEntity}, ///native PlayerSetEntity(player, entity[]);
	{ "PlayerSetName", pawnPlayerSetName}, ///native PlayerSetName(player, entity[], s = sizeof(entity));
	{ "PlayerGetName", pawnPlayerGetName}, ///native PlayerSetName(player, entity[], s = sizeof(entity));
	{ 0, 0 }
};
