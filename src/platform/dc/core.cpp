/****************************
Copyright (c) 2006-2010 Luke Salisbury
Copyright (c) 2008 Mokoi Gaming
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include "core.h"
#include "luxengine.h"
#include <kos.h>
#include <dc/maple.h>
#include <SDL/SDL.h>

KOS_INIT_FLAGS(INIT_DEFAULT);

/* Local variables */
cont_state_t * control[MAPLE_PORT_COUNT] = { NULL, NULL, NULL, NULL };
maple_device_t * control_addr[MAPLE_PORT_COUNT] = { NULL, NULL, NULL, NULL };

/* Local functions */
CoreSystem::CoreSystem()
{

	dbglog_set_level(DBG_ERROR);

	int dc_region = flashrom_get_region();
	int ct = vid_check_cable();

	if(dc_region == FLASHROM_REGION_EUROPE && ct != CT_VGA)
	{
		vid_set_mode(DM_640x480_PAL_IL, PM_RGB565);
	}
	else
	{
		vid_set_mode(DM_640x480_NTSC_IL, PM_RGB565);
	}
/*
	dbglog_set_level(DBG_DEAD);


	vid_set_mode(DM_640x480_NTSC_IL, PM_RGB565);
	*/
	this->good = !pvr_init_defaults();

	for (uint8_t p = 0; p < MAPLE_PORT_COUNT; p++)
	{
		control_addr[p] = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);
	}

	usleep(5*1000*1000);


}

CoreSystem::~CoreSystem()
{
	pvr_shutdown();
	arch_reboot();
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
void CoreSystem::SystemMessage(uint8_t type, std::string message)
{
}

bool CoreSystem::Good()
{
	return this->good;
}

uint32_t CoreSystem::GetTime()
{
	return timer_ms_gettime64();
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
	//thd_sleep(10);
}

LuxState CoreSystem::HandleFrame(LuxState old_state)
{
	if ( this->state > PAUSED || old_state >= SAVING )
	{
		this->state = old_state;
	}

	this->RefreshInput(lux::display);

	if ( GetInput(CONTROLBUTTON, 0, CONT_Y ) )  { this->state = EXITING; }


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
			if ( device_number >= 0 && device_number < MAPLE_PORT_COUNT )
			{
				if (control[device_number])
				{
					if (symbol == 0)
						return control[device_number]->joyx;
					else if (symbol == 1)
						return control[device_number]->joyy;
					else if (symbol == 2)
						return control[device_number]->joyx;
					else if (symbol == 3)
						return control[device_number]->joyy;
					else if (symbol == 4)
						return control[device_number]->ltrig;
					else if (symbol == 5)
						return control[device_number]->rtrig;
				}
			}
			return 0;
		}
		case CONTROLBUTTON:
		{
			if ( device_number >= 0 && device_number < MAPLE_PORT_COUNT )
			{
				if (control[device_number])
					if (control[device_number]->buttons & symbol)
						return 1;
			}
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
	for (uint8_t p = 0; p < MAPLE_PORT_COUNT; p++)
	{
		if ( control_addr[p] )
			control[p] = (cont_state_t *)maple_dev_status(control_addr[p]);
		else
			control[p] = NULL;
	}


}

bool CoreSystem::InputLoopGet(uint16_t & key)
{
	// Keyboard Stuff
	return 0;
}



