/****************************
Copyright (c) 2006-2009 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

/* 3DS Core System for Lux Engine */
#include "core.h"
#include "config.h"
#include "display/display.h"

/* Local variables */
touchPosition touch_screen;
uint32 key_states;
uint32_t fps = 0, fps_time = 0, fps_last = 0;
uint32 timer_ticks = 0;


u32 kDownOld = 0, kHeldOld = 0, kUpOld = 0;
	char keysNames[32][32] = {
			"KEY_A", "KEY_B", "KEY_SELECT", "KEY_START",
			"KEY_DRIGHT", "KEY_DLEFT", "KEY_DUP", "KEY_DDOWN",
			"KEY_R", "KEY_L", "KEY_X", "KEY_Y",
			"", "", "KEY_ZL", "KEY_ZR",
			"", "", "", "",
			"KEY_TOUCH", "", "", "",
			"KEY_CSTICK_RIGHT", "KEY_CSTICK_LEFT", "KEY_CSTICK_UP", "KEY_CSTICK_DOWN",
			"KEY_CPAD_RIGHT", "KEY_CPAD_LEFT", "KEY_CPAD_UP", "KEY_CPAD_DOWN"
		};

/* Local functions */
void nds_TimerInterrupt()
{
	timer_ticks++;
}

CoreSystem::CoreSystem()
{
	gfxInitDefault();
	consoleInit(GFX_BOTTOM, NULL);

	this->touch_time = 0;
	this->state = RUNNING;

}

CoreSystem::~CoreSystem()
{
	gfxExit();
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

std::ostream& CoreSystem::SystemMessage(const char *file, int line, uint8_t type  )
{
	switch ( type )
	{
		case SYSTEM_MESSAGE_VISUAL_WARNING:
		{
			if ( lux::display )
			{
				return lux::display->static_text;
			}
		}
		case SYSTEM_MESSAGE_ERROR:
		case SYSTEM_MESSAGE_WARNING:
		{
			if ( file )
			{
				std::cerr << std::dec <<  "[" << file << ":" << line << "] ";
			}
			return std::cerr;
		}
		case SYSTEM_MESSAGE_DEBUG:
		{
			if ( lux::display )
			{
				return lux::display->dynamic_text;
			}
		}
		default:
		{
			if ( file )
			{
				std::cout << std::dec << "(" << file << ":" << line << ") ";
			}
			return std::cout;
		}
	}
}
std::ostream& CoreSystem::SystemMessage( uint8_t type )
{
	return this->SystemMessage( NULL, 0, type );
}

void CoreSystem::SystemMessage( uint8_t type, std::string message )
{
	this->SystemMessage( NULL, 0, type ) << message << std::endl;
}

bool CoreSystem::Good()
{
	return this->good;
}

uint32_t CoreSystem::GetTime()
{
	return timer_ticks;
}

uint32_t CoreSystem::GetFrameDelta()
{
	return this->frame_ms;
}

bool CoreSystem::DelayIf(uint32_t diff)
{
	return false;
}

void CoreSystem::Idle()
{
	uint32_t start = this->GetTime();
	while (1)
	{
		if ((this->GetTime() - start) >= 30)
			break;
	}
}



LuxState CoreSystem::HandleFrame(LuxState old_state)
{
	if ( this->state > PAUSED || old_state >= SAVING )
	{
		this->state = old_state;
	}


	//Scan all the inputs. This should be done once for each frame
	hidScanInput();

	//hidKeysDown returns information about which buttons have been just pressed (and they weren't in the previous frame)
	u32 kDown = hidKeysDown();
	//hidKeysHeld returns information about which buttons have are held down in this frame
	u32 kHeld = hidKeysHeld();
	//hidKeysUp returns information about which buttons have been just released
	u32 kUp = hidKeysUp();

	if (kDown & KEY_START) this->state = EXITING; // break in order to return to hbmenu

	//Do the keys printing only if keys have changed
	if (kDown != kDownOld || kHeld != kHeldOld || kUp != kUpOld)
	{
		//Clear console
		consoleClear();

		//These two lines must be rewritten because we cleared the whole console
		printf("\x1b[0;0HPress Start to exit.");
		printf("\x1b[1;0HCirclePad position:");

		printf("\x1b[3;0H"); //Move the cursor to the fourth row because on the third one we'll write the circle pad position

		//Check if some of the keys are down, held or up
		int i;
		for (i = 0; i < 32; i++)
		{
			if (kDown & BIT(i)) printf("%s down\n", keysNames[i]);
			if (kHeld & BIT(i)) printf("%s held\n", keysNames[i]);
			if (kUp & BIT(i)) printf("%s up\n", keysNames[i]);
		}
	}

	//Set keys old values for the next frame
	kDownOld = kDown;
	kHeldOld = kHeld;
	kUpOld = kUp;

	circlePosition pos;

	//Read the CirclePad position
	hidCircleRead(&pos);

	//Print the CirclePad position
	printf("\x1b[2;0H%04d; %04d", pos.dx, pos.dy);


	this->time = this->GetTime();
	this->state = RUNNING;

	return this->state;
}

void CoreSystem::RefreshInput( DisplaySystem * display )
{

}

bool CoreSystem::InputLoopGet(  DisplaySystem * display, uint16_t & key)
{
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
		case TOUCHSCREEN:
		{
			return ( key_states & KEY_TOUCH );
		}
		case KEYBOARD:
		{
			return ( key_states & symbol );
		}
		case MOUSEAXIS:
		{
			if (symbol == 0 )
				return touch_screen.px;
			else if (symbol == 1 )
				return touch_screen.py;
			return 0;
		}
		case MOUSEBUTTON:
		{
			return ( key_states & KEY_TOUCH );
		}
		default:
			return 0;
	}

}
