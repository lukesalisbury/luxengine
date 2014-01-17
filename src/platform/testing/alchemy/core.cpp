/****************************
Copyright © 2006-2011 Luke Salisbury
Copyright (c) 2008 Mokoi Gaming
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include "../luxengine.h"

/* Local variables */

/* Local functions */
CoreSystem::CoreSystem()
{

	this->good = true;

}


CoreSystem::~CoreSystem() 
{
	Quit();
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
bool CoreSystem::Init(uint32_t flag)
{
	this->time = this->GetTime();
	this->state = RUNNING;
	return this->good;
}

void CoreSystem::Quit()
{
	this->good = false;
}
bool CoreSystem::Good()
{
	return this->good;
}

uint32_t CoreSystem::GetTime()
{
	return 15;
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

Lux_State CoreSystem::HandleFrame(Lux_State old_state)
{

	return this->state;
}

int16_t CoreSystem::GetInput(InputDevice device, uint32_t device_number, uint32_t symbol)
{
	if (device == NOINPUT)
	{
		return 0;
	}
	return 0;
}

int32_t CoreSystem::HostToNetwork(int32_t number)
{
	return number;
}
int16_t CoreSystem::HostToNetwork(int16_t number)
{
	return number;
}
int32_t CoreSystem::NetworkToHost(int32_t number)
{
	return number;
}
int16_t CoreSystem::NetworkToHost(int16_t number)
{
	return number;
}
int32_t CoreSystem::HostToNetwork(float f)
{
	int32_t p;
	uint32_t sign;

	if (f < 0) { sign = 1; f = -f; }
	else { sign = 0; }

	p = ((((int32_t)f)&0x7fff)<<16) | (sign<<31); // whole part and sign
	p |= (int32_t)(((f - (int)f) * 65536.0f))&0xffff; // fraction

	return p;
}

float CoreSystem::NetworkToHostFloat(int32_t p)
{
	float f = ((p>>16)&0x7fff); // whole part
	f += (p&0xffff) / 65536.0f; // fraction

	if (((p>>31)&0x1) == 0x1) { f = -f; } // sign bit set

	return f;
}