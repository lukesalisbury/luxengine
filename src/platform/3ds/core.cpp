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
circlePosition circle_pos;
uint32 key_states, key_held;
uint32 timer_ticks = 0;
u16 width, height;

bool top_screen = false;
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

CoreSystem::CoreSystem()
{
	this->touch_time = 0;
	this->state = RUNNING;

	this->primary_screen = GFX_TOP;
	this->secondary_screen = GFX_BOTTOM;


	/*
	aptInit();
	hidInit(NULL);
	gfxInit(GSP_BGR8_OES,GSP_BGR8_OES,false);
	*/
	sf2d_init();
	//consoleInit(this->secondary_screen, NULL);


	this->good = true;
}

CoreSystem::~CoreSystem()
{

	gfxExit();
	hidExit();
	aptExit();
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
	return svcGetSystemTick();
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
	//aptMainLoop();
}



LuxState CoreSystem::HandleFrame(LuxState old_state)
{
	if ( this->state > PAUSED || old_state >= SAVING )
	{
		this->state = old_state;
	}
	this->internal_ms = (this->GetTime() - this->time);
	this->frame_ms = clamp( this->internal_ms, 0, 33);

	hidScanInput();

	hidCircleRead(&circle_pos);
	hidTouchRead(&touch_screen);

	key_states = hidKeysDown();


	if ( !aptMainLoop() )
		this->state = EXITING;


	if ( key_held & KEY_START && key_held & KEY_SELECT )
		this->state = EXITING;

	this->time = this->GetTime();
	if ( this->state == RUNNING )
		this->animation_ms = this->frame_ms;
	else
		this->animation_ms = 0;
	return this->state;
}

void CoreSystem::RefreshInput( DisplaySystem * display )
{

	hidScanInput();

	hidCircleRead(&circle_pos);
	hidTouchRead(&touch_screen);

	key_states = hidKeysDown();
	key_held = hidKeysHeld();
}

bool CoreSystem::InputLoopGet(  DisplaySystem * display, uint16_t & key )
{
	if ( key_states & KEY_SELECT ) { key = 27; }
	if ( !aptMainLoop() ) { key = 27; }
	return 0;
}

int16_t CoreSystem::GetInput(InputDevice device, uint32_t device_number, int32_t symbol)
{
	uint32_t bitsymbol = BIT(symbol);

	hidCircleRead(&circle_pos);
	hidTouchRead(&touch_screen);

	key_states = hidKeysDown();
	key_held = hidKeysHeld();

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
		case CONTROLBUTTON:
		{
			return ( key_held & bitsymbol );
		}
		case CONTROLAXIS:
		{


			if ( bitsymbol == KEY_CPAD_RIGHT || bitsymbol == KEY_CPAD_LEFT )
				return (circle_pos.dx * 2) - (circle_pos.dx / 2);
			if ( bitsymbol == KEY_CPAD_UP || bitsymbol == KEY_CPAD_DOWN )
				return -(circle_pos.dy * 2) - (circle_pos.dy / 2);

			if ( bitsymbol == KEY_CSTICK_RIGHT || bitsymbol == KEY_CSTICK_LEFT )
				return circle_pos.dx / 128;
			if ( bitsymbol == KEY_CSTICK_UP || bitsymbol == KEY_CSTICK_DOWN )
				return circle_pos.dy / 128;



			return 0;
		}
		case KEYBOARD:
		{
			return ( key_states & bitsymbol );
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
