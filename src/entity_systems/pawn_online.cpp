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
#include "worker.h"
#include "pawn_helper.h"

int32_t Lux_Util_FileDownloaderEntityCallback( void * data );

extern const AMX_NATIVE_INFO Online_Natives[];

/** pawnEntityNetwork
* native EntityNetwork();
*
*/
static cell pawnOnline(AMX *amx, const cell *params)
{
	return 0;
}

/** pawnOnlineData
* native OnlineData(callback[32], url[128]);
*
*/
static cell pawnOnlineData(AMX *amx, const cell *params)
{
	char * url = NULL;

	amx_StrParam_Type(amx, params[2], url, char*);

	if ( url )
	{
		DownloadEntityRequest * request = new DownloadEntityRequest;
		request->entity = Lux_PawnEntity_GetParent(amx);
		request->url = url;
		request->callback = "ReceivedData";

		WorkerThread * thread = new WorkerThread(Lux_Util_FileDownloaderEntityCallback, (void*) request);
		return 1;
	}
	else
	{
		return 0;
	}

}

const AMX_NATIVE_INFO Online_Natives[] = {
	{ "OnlineUpdate", pawnOnline}, ///native GameUpdate();
	{ "OnlineSubmitScore", pawnOnline}, ///native OnlineSubmitScore(data[4]);
	{ "OnlineSave", pawnOnline}, ///native OnlineSubmit(data[64]);
	{ "OnlineData", pawnOnlineData}, ///native OnlineData(callback[32], url[128]);
	{ 0, 0 }
};
