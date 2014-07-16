/****************************
Copyright © 2006-2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#define LENS_LOGIN 0
#define LENS_SETID 1
#define LENS_PLAYERADD 2
#define LENS_PLAYERREMOVE 3
#define LENS_PLAYERSYNC 4
#define LENS_PLAYERMSG 5
#define LENS_ENTITYMSG 6
#define LENS_MAPSYNCREQUEST 7
#define LENS_MAPUPDATE 8
#define LENS_MAPSYNCRETURN 9
#define LENS_FILEREQUEST 10
#define LENS_SERVERMSG 11

#include "enet/enet.h"
#include "elix_endian.hpp"

template <typename T> void ReadPacket( ENetPacket * packet, int pos, T & v);
Player * NetworkGetPlayer( uint32_t network_id );

