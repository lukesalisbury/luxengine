/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "core.h"
#include "game_system.h"
#include "engine.h"
#include "pawn_helper.h"

#ifdef NETWORKENABLED
#include "network_types.h"
#endif

extern const AMX_NATIVE_INFO Network_Natives[];

/** pawnEntityActive
* native pawnEntityActive(id[] = SELF);
*
*/
static cell pawnEntityActive(AMX *amx, const cell *params)
{
	#ifdef NETWORKENABLED
	lux::core->NetworkLock();
	#endif
	cell result = -1;
	Entity * wanted = Lux_PawnEntity_GetEntity(amx, params[1]);
	if ( wanted != NULL )
	{
		result = (cell)wanted->onscreen;
	}
	#ifdef NETWORKENABLED
	lux::core->NetworkUnlock();
	#endif
	return result;
}

/** pawnEntityNetworkSync
* native EntityNetworkSync(bool:reliable);
*
*/
static cell pawnEntityNetworkSync(AMX *amx, const cell *params)
{
	cell result = -1;
	#ifdef NETWORKENABLED
	lux::core->NetworkLock();

	Entity * wanted = Lux_PawnEntity_GetParent(amx);
	if ( wanted != NULL )
	{
		if ( lux::core->CreateMessage( LENS_MAPUPDATE, (bool)params[1] ) )
		{
			lux::core->MessageAppend( wanted->_mapid );
			lux::core->MessageAppend( wanted->hashid );
			lux::core->MessageAppend( wanted->x );
			lux::core->MessageAppend( wanted->y );
			lux::core->MessageSend(false);
		}
	}
	lux::core->NetworkUnlock();
	#endif
	return result;
}

/** pawnNetworkConnect
* native NetworkConnect(name[], password[], email[]);
*
*/
extern uint8_t net_id;
static cell pawnNetworkConnect( AMX *amx, const cell *params )
{
	#ifdef NETWORKENABLED
	char * name = NULL;

	if ( lux::core->InitSubSystem( LUX_INIT_NETWORK ) )
	{
		amx_StrParam_Type(amx, params[1], name, char*);
		if ( lux::core->CreateMessage( LENS_LOGIN, true ) )
		{
			if ( name )
			{
				uint32_t c = 0;
				while ( name[c] )
				{
					lux::core->MessageAppend( (uint8_t)name[c] );
					c++;
				}
			}
			lux::core->MessageSend(false);

			return 1;
		}
	}
	else
	{
		if ( lux::engine->ShowDialog("Network Server connection failed.\nDo you want to exit?") )
		{
			lux::engine->SetState( GAMEERROR );
		}
	}
	#endif
	return 0;
}

/** pawnEntityNetwork
* native EntityNetwork();
*
*/
static cell pawnNetworkDisconnect(AMX *amx, const cell *params)
{
	lux::core->QuitSubSystem( LUX_INIT_NETWORK );
	return 0;
}

/** pawnNetworkMessage
* native NetworkMessage(reliable, server, message[], length, reallength = sizeof(message));
*
*/
static cell pawnNetworkMessage(AMX *amx, const cell *params)
{
	cell result = -1;
	#ifdef NETWORKENABLED
	cell * data;
	lux::core->NetworkLock();
	if ( params[4] <= params[5] && params[4] > 0 )
	{
		data = amx_Address(amx, params[3]);
		if ( params[2] == 2 )
		{
			if ( lux::core->CreateMessage( LENS_PLAYERMSG, (bool)params[1] ) )
			{
				for ( int32_t n = 0 ; n < params[4]; n++ )
				{
					lux::core->MessageAppend( (uint32_t)*data );
					data++;
				}
				lux::core->MessageSend(false);
			}
		}
		else
		{
			Entity * wanted = Lux_PawnEntity_GetParent(amx);
			if ( wanted != NULL )
			{
				if ( lux::core->CreateMessage( (params[2] ? LENS_SERVERMSG : LENS_ENTITYMSG), (bool)params[1] ) )
				{
					lux::core->MessageAppend( wanted->hashid );
					for ( int32_t n = 0 ; n < params[4]; n++ )
					{
						lux::core->MessageAppend( (uint32_t)*data );
						data++;
					}
					lux::core->MessageSend(false);
				}
			}
		}
	}
	lux::core->NetworkUnlock();
	#endif
	return result;
}

/** pawnNetworkPlayerName
* native NetworkPlayerName(player, name[], l = sizeof(name) );
*
*/
static cell pawnNetworkPlayerName(AMX *amx, const cell *params)
{
	cell * cptr;
	cptr = amx_Address(amx, params[2]);
	#ifdef NETWORKENABLED
	Player * player = NetworkGetPlayer( (uint32_t)params[1] );

	if( player )
	{
		amx_SetString(cptr, player->GetName().c_str(), 0, 0, params[3]);
		return 1;
	}
	else
	{
		amx_SetString(cptr, "Unknown", 0, 0, params[3]);
	}
	#else
	amx_SetString(cptr, "Unknown", 0, 0, params[3]);
	#endif
	return 0;
}

const AMX_NATIVE_INFO Network_Natives[] = {
	{ "EntityActive", pawnEntityActive}, ///native EntityActive();
	{ "EntityNetworkSync", pawnEntityNetworkSync}, ///native EntityNetworkSync(bool:reliable = true);
	{ "NetworkConnect", pawnNetworkConnect}, ///native NetworkConnect();
	{ "NetworkDisconnect", pawnNetworkDisconnect}, ///native NetworkDisconnect();
	{ "NetworkMessage", pawnNetworkMessage}, ///native NetworkMessage(reliable, server, message[], length, reallength = sizeof(message));
	{ "NetworkPlayerName", pawnNetworkPlayerName}, ///native NetworkPlayerName(player, name[], l = sizeof(name) );
	{ 0, 0 }
};
