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
#include "world.h"
#include <algorithm>
#include <sys/stat.h>
#include <dirent.h>
#include <fstream>

#include <SDL_joystick.h>
#include <SDL_hints.h>
#ifdef __GNUWIN32__
	#define _WIN32_IE 0x0600
	#include <io.h>
	#include <shlobj.h>
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
	//SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");

	SDL_Init( SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_HAPTIC | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER);

	AbleOutput(true);
	this->mouse_focus = false;

	memset( this->controller, 0, sizeof(SDL_GameController*)*8);


	this->native_window = SDL_CreateWindow(PROGRAM_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 320, 240, SDL_WINDOW_SHOWN|SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE );
	if ( !this->native_window )
	{
		std::cout << __FILE__ << ":" << __LINE__ << " | Couldn't create Window. " << SDL_GetError() << std::endl;
		this->good = false;
	}
	else
	{
		this->state = RUNNING;
		this->lockfps = true;
		this->keystate = SDL_GetKeyboardState(NULL);
		this->good = true;
		this->time = this->GetTime();

		int32_t nJoysticks = SDL_NumJoysticks();
		for ( int32_t i = 0; i < nJoysticks; i++ )
		{
			if ( SDL_IsGameController(i) )
			{
				this->GamepadAdded( i );
			}
		}
	}
}

CoreSystem::~CoreSystem()
{
	this->QuitSubSystem(LUX_INIT_NETWORK);

	for ( uint32_t i = 0; i < 8; i++ )
	{
		SDL_GameControllerClose( this->controller[i] );
	}



	SDL_DestroyWindow( this->native_window );

	AbleOutput(false);
	if ( this->good )
	{
		SDL_Quit();
		this->good = false;
	}

}

void CoreSystem::SystemMessage(uint8_t type, std::string message)
{
	SDL_Log("%d: %s", type, message.c_str() );

	std::cout << type << ":" << message.c_str() << std::endl;
}

void CoreSystem::AbleOutput(bool able)
{
	if ( able )
	{
		std::string error_filename = elix::path::User("") + "error.log";
		freopen( error_filename.c_str(), "w", stderr );
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
	return this->frame_ms;
}

bool CoreSystem::DelayIf(uint32_t diff)
{
	return true;
}

void CoreSystem::Idle()
{
	SDL_Delay(30);
}

LuxState CoreSystem::HandleFrame(LuxState old_state)
{
	SDL_Event event;

	this->internal_ms = (this->GetTime() - this->time);
	this->frame_ms = clamp( this->internal_ms, 0, 33);
	this->animation_ms = this->frame_ms;


	if ( this->state > PAUSED || old_state >= SAVING )
	{
		this->state = old_state;
	}

	SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);

	while( SDL_PollEvent(&event) )
	{
		switch( event.type )
		{
			case SDL_TEXTINPUT:

				if ( lux::entities )
				{
					if ( lux::entities->_keyboard )
					{
						int32_t key = event.text.text[0];
						lux::entities->_keyboard->Call((char*)"KeyboardInput", (char*)"d", key);
					}
				}
				break;
			case SDL_KEYDOWN:
			{
				switch ( event.key.keysym.sym )
				{
					case SDLK_ESCAPE:
						this->state = EXITING;
						break;
					case SDLK_PAUSE:
						this->state = (this->state == PAUSED ? RUNNING : PAUSED);
						break;
					case SDLK_F5: // Quick Save
					{
						if (this->state ==  RUNNING )
						{
							lux::engine->save_system_slot = 0x00;
							this->state = SAVING;
						}
						break;
					}
					case SDLK_F6: // Quick Load
					{
						if (this->state ==  RUNNING )
						{
							lux::engine->save_system_slot = 0x00;
							this->state = LOADING;
						}
						break;
					}
					case SDLK_F12:
					{
						lux::engine->SettingDialog();
						break;
					}
					case SDLK_F10:
					{
						lux::display->show_debug = !lux::display->show_debug;
						break;
					}
					case SDLK_F11:
					{
						lux::display->show_mask = !lux::display->show_mask;
						break;
					}
					case SDLK_BACKSPACE:
					{
						event.key.keysym.sym = 8;//OS X had issue
						break;
					}
					case SDLK_DELETE:
					{
						event.key.keysym.sym = 127;
						break;
					}
					case SDLK_RETURN:
					{
						SDL_Window *window = SDL_GetWindowFromID(event.key.windowID);
						if (window) {
							Uint32 flags = SDL_GetWindowFlags(window);
							lux::display->graphics.SetFullscreen( !!(flags & SDL_WINDOW_FULLSCREEN) );
						}
						break;
					}
					case SDLK_v:
					{
						if ( (event.key.keysym.mod & KMOD_CTRL) && SDL_HasClipboardText() )
						{
							int32_t key;
							uint32_t c = 0;
							char * clipboard = SDL_GetClipboardText();
							while ( clipboard[c] != 0 )
							{
								if ( lux::entities->_keyboard )
								{
									key = clipboard[c];
									lux::entities->_keyboard->Call((char*)"KeyboardInput", (char*)"d", key);
								}
								c++;
							}
						}
						break;
					}
				}

				if ( event.key.keysym.sym )
				{
					if ( lux::entities )
					{
						if ( lux::entities->_keyboard )
						{
							int32_t key = event.key.keysym.sym;
							lux::entities->_keyboard->Call((char*)"KeyboardInput", (char*)"d", key);
						}
					}
				}

				break;
			}
			case SDL_QUIT:
					this->state = EXITING;
				break;

			case SDL_WINDOWEVENT:
			{
				if ( this->state == RUNNING || this->state == PAUSED)
				{
					if ( event.window.event == SDL_WINDOWEVENT_MINIMIZED )
					{
						this->state = PAUSED;
					}
					else if ( event.window.event == SDL_WINDOWEVENT_RESTORED )
					{
						this->state = RUNNING;
					}
					else if ( event.window.event == SDL_WINDOWEVENT_MAXIMIZED )
					{
						lux::display->graphics.SetFullscreen(true);
					}
					else if ( event.window.event == SDL_WINDOWEVENT_ENTER && mouse_focus )
					{
						this->state = RUNNING;
					}
					else if ( event.window.event == SDL_WINDOWEVENT_LEAVE && mouse_focus )
					{
						this->state = PAUSED;
					}
					else if ( event.window.event == SDL_WINDOWEVENT_RESIZED )
					{
						lux::display->graphics.Resize( (uint16_t)event.window.data1, (uint16_t)event.window.data2 );
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

	this->mousestate = SDL_GetMouseState(&this->mouseposition[0], &this->mouseposition[1]);
	if ( display )
	{
		if ( display->graphics.Display2Screen )
		{
			display->graphics.Display2Screen(&this->mouseposition[0], &this->mouseposition[1]);
		}
	}
	SDL_JoystickUpdate();
}

bool CoreSystem::InputLoop( DisplaySystem * display, uint16_t & key )
{
	SDL_Event event;


	if ( SDL_PollEvent(&event) )
	{
		switch( event.type )
		{
			case SDL_TEXTINPUT:
				break;
			case SDL_KEYDOWN:
				key = event.key.keysym.sym;
				if ( event.key.keysym.sym == SDLK_ESCAPE ) { key = 27; }
				if ( event.key.keysym.sym == SDLK_RETURN && ( event.key.keysym.mod & KMOD_ALT ) ) {
					SDL_Window *window = SDL_GetWindowFromID(event.key.windowID);
					if (window) {
						Uint32 flags = SDL_GetWindowFlags(window);
						display->graphics.SetFullscreen( !!(flags & SDL_WINDOW_FULLSCREEN) );
					}
				}
				break;
			case SDL_QUIT:
				key = 27;
				break;
/*
			case SDL_TOUCHBUTTONDOWN:
			{
				break;
			}
*/
			case SDL_FINGERDOWN:
			{
				break;
			}
			case SDL_WINDOWEVENT:
			{
				if ( event.window.event == SDL_WINDOWEVENT_MINIMIZED )
				{
					this->state = PAUSED;
				}
				else if ( event.window.event == SDL_WINDOWEVENT_RESTORED )
				{
					this->state = RUNNING;
				}
				else if ( event.window.event == SDL_WINDOWEVENT_ENTER && mouse_focus )
				{
					this->state = RUNNING;
				}
				else if ( event.window.event == SDL_WINDOWEVENT_LEAVE && mouse_focus )
				{
					this->state = PAUSED;
				}
				else if ( event.window.event == SDL_WINDOWEVENT_RESIZED )
				{
					if ( display )
					{
						display->graphics.Resize( (uint16_t)event.window.data1, (uint16_t)event.window.data2 );
					}
				}
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

	switch ( device )
	{
		case KEYBOARD:
		{
			if ( this->keystate && symbol < 0x11A )
				return this->keystate[symbol];
			return 0;
		}
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
		case TOUCHSCREEN:
		{

			return 0;
		}
		case CONTROLAXIS:
		{
			if ( device_number >= 0 && device_number < 8 )
			{
				SDL_GameControllerAxis s = (SDL_GameControllerAxis)symbol;
				if ( this->controller[device_number] && s> -1 && s < SDL_CONTROLLER_AXIS_MAX )
				{
					return (int16_t)SDL_GameControllerGetAxis(this->controller[device_number], s) / 128;
				}
			}
			return 0;
		}
		case CONTROLBUTTON:
		{
			if ( device_number >= 0 && device_number < 8 )
			{
				SDL_GameControllerButton s = (SDL_GameControllerButton)symbol;
				if ( this->controller[device_number] && s < SDL_CONTROLLER_BUTTON_MAX )
				{
					return (int16_t)SDL_GameControllerGetButton(this->controller[device_number], s);
				}
			}
			return 0;
		}
		case NOINPUT:
		default:
			return 0;
	}
}

const char * CoreSystem::GamepadName( uint32_t device_number )
{
	if ( device_number < 8 )
	{
		return SDL_GameControllerName( this->controller[device_number] );
	}
	return NULL;

}

bool CoreSystem::GamepadAdded( int32_t joystick_index )
{
	int index = 0;
	while ( index < 8 )
	{
		if ( this->controller[index] == NULL )
		{
			this->controller[index] = SDL_GameControllerOpen(joystick_index);
			return true;
		}

		index++;
	}

	return false;
}
