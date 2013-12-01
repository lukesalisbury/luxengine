/****************************
Copyright (c) 2006-2012 Luke Salisbury
Copyright (c) 2008 Mokoi Gaming
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include "core.h"
#include "luxengine.h"


/* Local functions */
CoreSystem::CoreSystem()
{

}

CoreSystem::~CoreSystem() 
{

}

uint32_t CoreSystem::WasInit(uint32_t flag)
{
	return 1;
}
	
void CoreSystem::QuitSubSystem(uint32_t flag)
{

}
bool CoreSystem::InitSubSystem(uint32_t flag)
{
	return true;
}

bool CoreSystem::Good()
{
	return this->good;
}

uint32_t CoreSystem::GetTime()
{
	return 0;
}
uint32_t CoreSystem::GetFrameDelta()
{
	return 15;
}
bool CoreSystem::DelayIf(uint32_t diff)
{
	return false;
}

void CoreSystem::Idle()
{

}

LuxState CoreSystem::HandleFrame(LuxState old_state)
{
	if ( this->state > PAUSED || old_state >= SAVING )
	{
		this->state = old_state;
	}	

	this->RefreshInput(lux::display);

	this->time = this->GetTime();

	return this->state;
}

int16_t CoreSystem::GetInput(InputDevice device, uint32_t device_number, int32_t symbol)
{
	if (device == NOINPUT)
	{
		return 0;
	}
	switch (device)
	{
		case CONTROLAXIS:
		{
			
			return 0;
		}
		case CONTROLBUTTON:
		{
			return 0;
		}
		case KEYBOARD:
		case MOUSEAXIS:
		case MOUSEBUTTON:
		case NOINPUT:
		default:
			return 0;
	}

	return 0;
}

void CoreSystem::RefreshInput( DisplaySystem * display )
{


}

bool CoreSystem::InputLoop(uint16_t & key)
{
	// Keyboard Stuff
	return 0;
}



