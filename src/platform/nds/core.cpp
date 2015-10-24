/****************************
Copyright (c) 2006-2009 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

/* NDS Core System for Lux Engine */
#include <filesystem.h>
#include <fat.h>
#include <nds.h>
#include <fstream>
#include <sys/dir.h>
#include "core.h"
#include "config.h"
#include "display/display.h"

/* Local variables */
touchPosition touch_screen;
uint32 key_states;
uint32_t fps = 0, fps_time = 0, fps_last = 0;
uint32 timer_ticks = 0;

extern bool dual_screen;

/* Local functions */
void nds_TimerInterrupt()
{
	timer_ticks++;
}

CoreSystem::CoreSystem()
{
	lcdMainOnBottom();

	/* Console Stuff */
	PrintConsole * bottomScreen;
	videoSetModeSub(MODE_5_2D);
	vramSetBankF(VRAM_F_MAIN_BG);
	bottomScreen = consoleInit(NULL, 0, BgType_Text4bpp, BgSize_T_256x256, 7, 0, false, true);
	consoleSelect(bottomScreen);
	bgSetPriority(0, 1);
	bgSetPriority(1, 0);

	defaultExceptionHandler();
	this->good = true;
	if( !nitroFSInit() )
		if( !fatInitDefault() )
			this->good = false;

	timer_ticks = 0;
	timerStart(2, ClockDivider_1024, TIMER_FREQ_1024(1000), nds_TimerInterrupt);
	this->touch_time = 0;


	this->state = RUNNING;

}

CoreSystem::~CoreSystem()
{
	swiSoftReset();
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

void CoreSystem::SystemMessage(uint8_t type, std::string message)
{

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

	if ( key_states & KEY_LID )
	{
		if ( this->state == PAUSED )
			this->state = RUNNING;
		else
			this->state = PAUSED;
	}
	if ( key_states & KEY_SELECT )
	{
		struct mallinfo mi = mallinfo();
		int usedMem = mi.uordblks;
		lux::core->SystemMessage(SYSTEM_MESSAGE_DEBUG) << "Mem: Used " << usedMem << std::endl;

	}
	if ( key_states & KEY_START && key_states & KEY_SELECT && key_states & KEY_L )
	{
		this->state = EXITING;
	}
	if ( key_states & KEY_TOUCH )
	{
		lux::core->SystemMessage(SYSTEM_MESSAGE_DEBUG) << "Touch " << touch_screen.px << " " << touch_screen.py << std::endl;
	}


	this->RefreshInput(lux::display);

	if ( this->GetTime() > (fps_time + 1000) )
	{
		fps_last = fps;
		fps_time = this->GetTime();
		fps = 0;
	}
	fps++;
	lux::core->SystemMessage(SYSTEM_MESSAGE_DEBUG) << "FPS " << fps_last << std::endl;


	int polygon_count;
	int vertex_count;
	glGetInt(GL_GET_VERTEX_RAM_COUNT, &vertex_count);
	glGetInt(GL_GET_POLYGON_RAM_COUNT, &polygon_count);

	lux::core->SystemMessage(SYSTEM_MESSAGE_DEBUG) << "Vertex ram: " << vertex_count << " - Polygon ram: " << polygon_count << std::endl;



	this->time = this->GetTime();
	this->state = RUNNING;

	return this->state;
}

void CoreSystem::RefreshInput( DisplaySystem * display )
{
	this->frame_ms = clamp( (this->GetTime() - this->time), 0, 33);
	scanKeys();
	touchRead( &touch_screen );
	key_states =  keysHeld();


	//lux::core->SystemMessage(SYSTEM_MESSAGE_LOG) << "touch_time" << "-" << this->touch_time << "-" << this->GetFrameDelta() << std::endl;
	this->last_press = this->touch_press;
	this->touch_press = (key_states & KEY_TOUCH);
}

bool CoreSystem::InputLoopGet(uint16_t & key)
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
				return touch_screen.py + (dual_screen ? 192 : 0);
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
