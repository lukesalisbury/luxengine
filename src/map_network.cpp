/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include "engine.h"
#include "elix/elix_string.hpp"
#include "display/display.h"
#include "mokoi_game.h"
#include "map_object.h"
#include "masks.h"
#include "map.h"
#include "entity.h"
#include "entity_manager.h"
#include "config.h"
#include "core.h"


void MokoiMap::NetworkMapSwitch()
{
	if ( this->server && this->ident.value )
	{
		lux::screen::display("Syncing with Server. (Connection may have died)" );
		lux::core->NetworkLock();
		if ( lux::core->CreateMessage(7, 1) )
		{
			lux::core->MessageAppend(this->ident.value);
			lux::core->MessageSend();
			lux::engine->SetState(NOUPDATE);
		}
		lux::core->NetworkUnlock();
	}
}

void MokoiMap::NetworkMapLoop()
{
	if ( this->server )
	{
		lux::core->NetworkLock();
		if (this->entities)
			this->entities->Switch(0,0);
		lux::core->NetworkUnlock();
		this->server = false;
	}
}

