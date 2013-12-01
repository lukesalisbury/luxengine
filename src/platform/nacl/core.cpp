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
#include "elix_path.h"
#include <algorithm>
#include <sys/stat.h>
#include <dirent.h>
#include <fstream>
#ifdef __GNUWIN32__
	#define _WIN32_IE 0x0400
	#include <io.h>
	#include <shlobj.h>
#endif

#ifdef WIIUSE
#include "../../../../ThirdParty/wiiuse/wiiuse.h"
wiimote ** wiimotes = NULL;
#endif

/* Network Thread */
#ifdef NETWORKENABLED
	extern SDL_Thread * sdlcore_thread;
	extern bool net_active;
	bool pc_network_init();
#endif

/* Local functions */
CoreSystem::CoreSystem()
{
	SDL_Init( SDL_INIT_TIMER );
	AbleOutput(true);

	if ( !SDL_InitSubSystem( SDL_INIT_JOYSTICK ) )
	{
		uint32_t js_num = SDL_NumJoysticks();
		if ( js_num )
		{
			this->joystick = (SDL_Joystick **)calloc(1, js_num * sizeof(SDL_Joystick **));
			for ( uint32_t i = 0; i < js_num; i++ )
			{
				this->joystick[i] = SDL_JoystickOpen(i);
				if ( this->joystick[i] != NULL )
				{
					std::cout << __FUNCTION__ << ": Found Joystick (" << i << "): " << SDL_JoystickName(i) << " Axes:" << SDL_JoystickNumAxes(this->joystick[i]) << " Buttons:" << SDL_JoystickNumButtons(this->joystick[i]) << " Hats:" << SDL_JoystickNumHats(this->joystick[i]) << " Balls:" << SDL_JoystickNumBalls(this->joystick[i]) << std::endl;
				}
			}
		}
	}

#ifdef WIIUSE
	int found, connected;

	wiimotes = wiiuse_init(4);
	found = wiiuse_find(wiimotes, 4, 5);
	connected = wiiuse_connect(wiimotes, 4);

	std::cout << __FUNCTION__ << ": Found '" << found << "' Wiimote(s). " << connected << " are conected." << std::endl;
	wiiuse_set_leds(wiimotes[0], WIIMOTE_LED_1 | WIIMOTE_LED_4);
	wiiuse_rumble(wiimotes[0], 1);
	wiiuse_rumble(wiimotes[0], 0);

	wiiuse_motion_sensing(wiimotes[0], 1);
	wiiuse_set_ir(wiimotes[0], 1);

#endif

	this->state = RUNNING;
	this->lockfps = true;
	this->keystate = SDL_GetKeyboardState(NULL);
	this->good = true;
	this->time = this->GetTime();
	
}

CoreSystem::~CoreSystem() 
{
	this->QuitSubSystem(LUX_INIT_NETWORK);
	
	if ( SDL_NumJoysticks() )
	{
		for ( int32_t i = 0; i < SDL_NumJoysticks(); i++ )
		{
			if ( this->joystick[i] != NULL )
				SDL_JoystickClose(this->joystick[i]);
		}
	}
	
#ifdef WIIUSE
	wiiuse_cleanup(wiimotes, 4);
#endif

	AbleOutput(false);
	if ( this->good )
	{
		SDL_Quit();
		this->good = false;
	}

}


void CoreSystem::AbleOutput(bool able)
{
	if ( able )
	{
		/* Redirect Console output to text files */
		//std::cout.sync_with_stdio(true);
		std::string log_filename = elix::path::User("") + "luxengine.log";
		std::string error_filename = elix::path::User("") + "error.log";
	
		freopen( error_filename.c_str(), "w", stderr );
		freopen( log_filename.c_str(), "w", stdout );
		//std::cout.sync_with_stdio(false);
		std::cout << "-------------------------------------" << std::endl;
		std::cout << PROGRAM_NAME << " - Version " << PROGRAM_VERSION << std::endl;
		std::cout << "-------------------------------------" << std::endl;
		std::cerr << "-------------------------------------" << std::endl;
		std::cerr << PROGRAM_NAME << " Error Log - Version " << PROGRAM_VERSION << std::endl;
		std::cerr << "-------------------------------------" << std::endl;

	}
	else
	{
		freopen( NULL, "w", stderr );
		freopen( NULL, "w", stdout );
	}

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
			std::cerr << __FILE__ << ":" << __LINE__ << " | Couldn't init subsystems. " << SDL_GetError() << std::endl;
			return false;
		}
		return true;
	}
	return false;
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
	return this->frame;
}

bool CoreSystem::DelayIf(uint32_t diff)
{
	return true;
}

void CoreSystem::Idle()
{
	SDL_Delay(30);
}

#include "../../world.h"
LuxState CoreSystem::HandleFrame(LuxState old_state)
{
	SDL_Event event;

	this->frame = clamp( (this->GetTime() - this->time), 0, 33);

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
				if ( event.key.keysym.sym == SDLK_ESCAPE ) { this->state = EXITING; }
				if ( event.key.keysym.sym == SDLK_PAUSE ) { this->state = (this->state == PAUSED ? RUNNING : PAUSED); }
				if ( event.key.keysym.sym == SDLK_F5 ) { this->state = SAVING; }
				if ( event.key.keysym.sym == SDLK_F6 ) { this->state = LOADING; }
				if ( event.key.keysym.sym == SDLK_F12 ) { this->state = GUIMODE; }
				if ( event.key.keysym.sym == SDLK_F1 ) { lux::display->show_3d = !lux::display->show_3d; }
				if ( event.key.keysym.sym == SDLK_F9 ) { this->lockfps = !this->lockfps; }
				if ( event.key.keysym.sym == SDLK_F10 ) { lux::display->show_debug = !lux::display->show_debug; }
				if ( event.key.keysym.sym == SDLK_F11 ) { lux::display->show_mask = !lux::display->show_mask; }
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

	this->keystate = SDL_GetKeyboardState(NULL);
	this->mousestate = SDL_GetMouseState(&this->mouseposition[0], &this->mouseposition[1]);
	if ( lux::display->Display2Screen )
	{
		lux::display->Display2Screen(&this->mouseposition[0], &this->mouseposition[1]);
	}

	SDL_JoystickUpdate();
	#ifdef WIIUSE
	wiiuse_poll(wiimotes, 4);
	#endif
	this->time = this->GetTime();
	return this->state;
}

int16_t CoreSystem::GetInput(InputDevice device, uint32_t device_number, uint32_t symbol)
{
	if (device == NOINPUT)
	{
		return 0;
	}
	
	switch (device)
	{
		case KEYBOARD:
			return this->keystate[SDL_GetScancodeFromKey(symbol)];
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
			if ( SDL_JoystickOpened(device_number) )
			{
				if (symbol < (uint32_t)SDL_JoystickNumAxes(this->joystick[device_number]) )
				{
					return (int16_t)SDL_JoystickGetAxis(this->joystick[device_number], symbol) / 128;
				}
			}
			return 0;
		}
		case CONTROLBUTTON:
		{
			if ( SDL_JoystickOpened(device_number)  )
			{
				if (symbol < (uint32_t)SDL_JoystickNumButtons(this->joystick[device_number]) )
				{
					return (int16_t)SDL_JoystickGetButton(this->joystick[device_number], symbol);
				}
			}
			return 0;
		}
		case NOINPUT:
		default:
			return 0;
	}
}

