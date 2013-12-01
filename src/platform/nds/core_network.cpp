/****************************
Copyright (c) 2006-2009 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include "../engine.h"
#include "../data_types.h"

/* Network Thread */
#ifdef NETWORKENABLED


int nds_network_thread( void *data )
{
		return 0;
}

bool nds_network_init()
{

	return false;
}

bool CoreSystem::CreateMessage(uint8_t type, bool reliable)
{
	return false;
}

bool CoreSystem::MessageAppend(fixed data)
{
	return false;
}

bool CoreSystem::MessageAppend(uint8_t data)
{
	return false;
}

bool CoreSystem::MessageAppend(uint32_t data)
{
	return false;
}

bool CoreSystem::MessageSend()
{
	return false;
}

int32_t CoreSystem::ReadMessage(void * data)
{
	return 0;
}


#endif
