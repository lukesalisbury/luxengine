/****************************
Copyright (c) 2006-2009 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

/* Gamecube Core System for Lux Engine */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <gccore.h>
#include <fat.h>
#include "core.h"
#include "config.h"
#include "display/display.h"
#include <wiiuse/wpad.h>


/* Local variables */
s8 HWButton = -1;
ir_t ir_player1;
u32 pressed;
/* Local functions */
 
/**
 * Callback for the reset button on the Wii.
 */
void WiiResetPressed()
{
	printf("WiiResetPressed\n");
	HWButton = SYS_RETURNTOMENU;
}
 
/**
 * Callback for the power button on the Wii.
 */
void WiiPowerPressed()
{
	printf("WiiPowerPressed\n");
	HWButton = SYS_POWEROFF_STANDBY;
	exit(0);
}
 
/**
 * Callback for the power button on the Wiimote.
 * @param[in] chan The Wiimote that pressed the button
 */
void WiimotePowerPressed(s32 chan)
{
	printf("WiimotePowerPressed\n");
	HWButton = SYS_RETURNTOMENU;
}

void wii_initialise_video(bool console_only);

CoreSystem::CoreSystem()
{
	wii_initialise_video(false);
	WPAD_Init();
	SYS_SetResetCallback(WiiResetPressed);
	SYS_SetPowerCallback(WiiPowerPressed);
	WPAD_SetPowerButtonCallback(WiimotePowerPressed);


	
	WPAD_SetVRes(0, 640, 480);
	WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR); 

	this->good = true;


	if (!fatInitDefault())
	{
		printf("fatInitDefault failure: terminating\n");
		return;
	}

}

CoreSystem::~CoreSystem() 
{

}

uint32_t CoreSystem::WasInit(uint32_t flag)
{
	return this->good;
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
	return 15;
}
uint32_t CoreSystem::GetFrameDelta()
{
	return 15;
}
bool CoreSystem::DelayIf(uint32_t diff)
{
/*
	uint32_t current_time = this->GetTime();
	if ( this->time + diff > current_time )
	{
		uint32_t delay_time = diff - (current_time - this->time);
		if (delay_time > 10)
		{
			while ( this->GetTime() < current_time + delay_time )
			{
				
			}
			this->time = current_time;
			return true;
		}
	}
	
	this->time = current_time;
	*/
	return false;
}

void CoreSystem::Idle()
{
	/*
	uint32_t current_time = this->GetTime();
	uint32_t delay_time = 30;
	while ( this->GetTime() < current_time + delay_time )
	{
		
	}
	current_time = this->GetTime();
	*/
}


LuxState CoreSystem::HandleFrame(LuxState old_state)
{
	time_t gc_time;
	gc_time = ::time(NULL);

	this->RefreshInput(lux::display);
}

void CoreSystem::RefreshInput( DisplaySystem * display )
{
	WPAD_IR(0, &ir_player1);
	WPAD_ScanPads();

	pressed = WPAD_ButtonsDown(0);
	if (pressed & WPAD_BUTTON_HOME) exit(0);

	if (HWButton != -1) SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);
	if(pressed & WPAD_BUTTON_A) printf("A \n ");
	if(pressed & WPAD_BUTTON_B) printf("B \n ");
	if(pressed & WPAD_BUTTON_1) printf("1 \n ");
	if(pressed & WPAD_BUTTON_2) printf("2 \n ");
	if(pressed & WPAD_BUTTON_MINUS) printf("MINUS \n ");
	if(pressed & WPAD_BUTTON_HOME) printf("HOME \n ");
	if(pressed & WPAD_BUTTON_PLUS) printf("PLUS \n ");

	if(pressed & WPAD_BUTTON_LEFT) printf("LEFT \n ");
	if(pressed & WPAD_BUTTON_RIGHT) printf("RIGHT \n ");
	if(pressed & WPAD_BUTTON_UP) printf("UP \n ");
	if(pressed & WPAD_BUTTON_DOWN) printf("DOWN \n ");

}

bool CoreSystem::InputLoopGet(uint16_t & key)
{
	if (HWButton != -1) key = 27;
	
	return 0;
}

int16_t CoreSystem::GetInput(InputDevice device, uint32_t device_number, int32_t symbol)
{
	if (device == NOINPUT)
	{
		return 0;
	}
	switch (device)
	{
		case MOUSEAXIS:
		{
			if (symbol == 0)
				return ir_player1.x;
			if (symbol == 1)
				return ir_player1.y;

			return 0;
		}
		case CONTROLAXIS:
		{
			return (pressed & WIIMOTE_BUTTON_UP)*255;
		}
		case CONTROLBUTTON:
		{
			return (pressed & WPAD_BUTTON_A);
		}
	
		case NOINPUT:
		default:
			return 0;
	}
}
