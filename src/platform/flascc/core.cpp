/****************************
Copyright © 2006-2011 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include "core.h"
#include "engine.h"
#include "display.h"
#include "entity_manager.h"
#include "elix_path.hpp"
#include "world.h"
#include <algorithm>
#include <sys/stat.h>
#include <dirent.h>
#include <fstream>


/* Network Thread */
#ifdef NETWORKENABLED
	extern SDL_Thread * sdlcore_thread;
	extern bool net_active;
	bool pc_network_init();
#endif

/* Local functions */
CoreSystem::CoreSystem()
{
	SDL_Init( SDL_INIT_EVERYTHING );

	this->state = RUNNING;
	this->lockfps = true;
	this->keystate = SDL_GetKeyState(NULL);
	this->good = true;
	this->time = this->GetTime();
}

CoreSystem::~CoreSystem()
{
	this->QuitSubSystem(LUX_INIT_NETWORK);



	if ( this->good )
	{
		SDL_Quit();
		this->good = false;
	}

}


void CoreSystem::AbleOutput(bool able)
{

}


uint32_t CoreSystem::WasInit(uint32_t flag)
{
	return SDL_WasInit(flag);
}


void CoreSystem::QuitSubSystem(uint32_t flag)
{
	if ( flag == LUX_INIT_NETWORK )
	{
		#ifdef NETWORKENABLED
		if ( net_active )
		{
			net_active = false;
		}
		#endif
	}
	else
	{
		SDL_QuitSubSystem(flag);
	}
}

bool CoreSystem::InitSubSystem(uint32_t flag)
{
	if ( flag == LUX_INIT_NETWORK )
	{
		#ifdef NETWORKENABLED
			return pc_network_init();
		#endif
	}
	else
	{
		if ( SDL_InitSubSystem(flag) < 0 )
		{
			lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR, __FILE__ , __LINE__) << " | Couldn't init subsystems. " << SDL_GetError() << std::endl;
			return false;
		}
		return true;
	}
	return false;
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
	return SDL_GetTicks();
}

uint32_t CoreSystem::GetFrameDelta()
{
	return this->frame_ms;
}

bool CoreSystem::DelayIf(uint32_t diff)
{
	SDL_Delay(diff);
	return true;
}

void CoreSystem::Idle()
{
	//SDL_Delay(30);
}

LuxState CoreSystem::HandleFrame(LuxState old_state)
{
	SDL_Event event;

	this->frame_ms = clamp( (this->GetTime() - this->time), 0, 33);

	if ( this->state > PAUSED || old_state >= SAVING )
	{
		this->state = old_state;
	}

	//SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
	SDL_EnableUNICODE(1);

	while( SDL_PollEvent(&event) )
	{
		switch( event.type )
		{
			case SDL_KEYDOWN:
				if ( event.key.keysym.sym == SDLK_BACKSPACE ) { event.key.keysym.unicode = 8; } //OS X had issue
				if ( event.key.keysym.sym == SDLK_DELETE ) { event.key.keysym.unicode = 127; }
				if ( event.key.keysym.unicode )
				{
					if ( lux::entities )
					{
						if ( lux::entities->_keyboard )
						{
							int32_t key = event.key.keysym.unicode;
							lux::entities->_keyboard->Call((char*)"KeyboardInput", (char*)"d", key);
						}
					}
				}
				break;
			case SDL_QUIT:
					this->state = EXITING;
				break;
			case SDL_ACTIVEEVENT:
			{
				if ( this->state == RUNNING || this->state == PAUSED)
				{
					if ( event.active.state & SDL_APPACTIVE) {
						if ( event.active.gain )
							this->state = RUNNING;
						else
							this->state = PAUSED;
					}
				}
				break;
			}
			default:
				break;
		}
	}

	this->RefreshInput( lux::display );
	this->time = this->GetTime();
	return this->state;
}

void CoreSystem::RefreshInput( DisplaySystem * display )
{
	SDL_PumpEvents();
	this->keystate = SDL_GetKeyState(NULL);
	this->mousestate = SDL_GetMouseState(&this->mouseposition[0], &this->mouseposition[1]);
	if ( display )
		if ( display->graphics.Display2Screen )
			display->graphics.Display2Screen(&this->mouseposition[0], &this->mouseposition[1]);
}

bool CoreSystem::InputLoop(uint16_t & key)
{
	SDL_Event event;
	SDL_EnableUNICODE(1);

	if ( SDL_PollEvent(&event) )
	{
		switch( event.type )
		{
			case SDL_KEYDOWN:
				key = event.key.keysym.unicode;
				break;
			case SDL_QUIT:
				key = 27;
				break;
			case SDL_ACTIVEEVENT:
			{
				if (event.active.state & SDL_APPACTIVE)
					if ( !event.active.gain )
						SDL_Delay(33);
				break;
			}
			default:
				break;
		}
		return 1;
	}
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
		case KEYBOARD:
			return this->keystate[symbol];
		case MOUSEAXIS:
		{
			if ( symbol < 2 )
			{
				return this->mouseposition[symbol];
			}
			return 0;
		}
		case MOUSEBUTTON:
		{
			if ( this->mousestate & SDL_BUTTON(symbol) )
			{
				return 1;
			}
			return 0;
		}
		case CONTROLAXIS:
		{

			return 0;
		}
		case CONTROLBUTTON:
		{

			return 0;
		}

		case NOINPUT:
		default:
			return 0;
	}
}

