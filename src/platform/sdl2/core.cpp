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
#include "elix_string.hpp"
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

	SDL_Init( SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_HAPTIC | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER );

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
		this->keystate = SDL_GetKeyboardState( &this->keystate_count );
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

std::ostream& CoreSystem::SystemMessage(uint8_t type, const char * file, int line )
{
	if ( type == SYSTEM_MESSAGE_LOG )
	{
		if ( file )
		{
			std::cout << "(" << file << ":" << line << ") ";
		}

		return std::cout;
	}
	else if ( type == SYSTEM_MESSAGE_ERROR || type == SYSTEM_MESSAGE_WARNING )
	{
		if ( file )
		{
			std::cerr << "[" << file << ":" << line << "] ";
		}
		return std::cerr;
	}
	else
	{
		if ( file )
		{
			std::cout << "[" << file << ":" << line << "] ";
		}
		return std::cout;
	}
}

void CoreSystem::SystemMessage(uint8_t type, std::string message)
{
	if ( type == SYSTEM_MESSAGE_LOG )
	{
		//SDL_Log("%s", message.c_str() );
		std::cout << "LOG:" <<  message << std::endl;
	}
	else if ( type == SYSTEM_MESSAGE_ERROR || SYSTEM_MESSAGE_WARNING )
	{
		//SDL_Log("%s", message.c_str() );
		std::cerr << message << std::endl;
	}
	else
	{
		//SDL_Log("%d: %s", type, message.c_str() );
		std::cout << type << ":" << message << std::endl;
	}
}

void CoreSystem::AbleOutput(bool able)
{
	if ( able )
	{
		std::string error_filename = elix::directory::User("") + "error.log";
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
			this->SystemMessage(SYSTEM_MESSAGE_ERROR, __FILE__ , __LINE__) << " | Couldn't init subsystems. " << SDL_GetError() << std::endl;
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
	uint8_t touch_events_count = 0;

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
			{
				if ( lux::entities )
				{
					if ( lux::entities->_keyboard )
					{
						int32_t key = event.text.text[0];
						lux::entities->_keyboard->Call((char*)"KeyboardInput", (char*)"d", key);
					}
				}
				break;
			}
			case SDL_KEYDOWN:
			{
				switch ( event.key.keysym.sym )
				{
					case SDLK_ESCAPE:
					case SDLK_AC_BACK:
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
				break;
			}
			case SDL_FINGERDOWN:
			case SDL_FINGERUP:
			{

				if ( touch_events_count < 10 )
				{
					this->touch_events[touch_events_count].type = event.tfinger.type;
					this->touch_events[touch_events_count].x = event.tfinger.x;
					this->touch_events[touch_events_count].y = event.tfinger.y;
					this->touch_events[touch_events_count].dx = event.tfinger.dx;
					this->touch_events[touch_events_count].dy = event.tfinger.dy;
					this->touch_events[touch_events_count].pressure = event.tfinger.pressure;
					touch_events_count++;
				}
				break;
			}
#ifdef EMULATE_TOUCH
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
			{

				if ( touch_events_count < 10 )
				{
					float x = (float)event.button.x;
					float y = (float)event.button.y;
					float w = (float)lux::display->screen_dimension.w;
					float h = (float)lux::display->screen_dimension.h;

					this->touch_events[touch_events_count].type = event.button.type == SDL_MOUSEBUTTONDOWN ? SDL_FINGERDOWN : SDL_FINGERUP;
					this->touch_events[touch_events_count].x = (x / w);
					this->touch_events[touch_events_count].y = (y / h);
					this->touch_events[touch_events_count].dx = this->touch_events[touch_events_count].x + 0.2;
					this->touch_events[touch_events_count].dy = this->touch_events[touch_events_count].y + 0.2;
					this->touch_events[touch_events_count].pressure = 1.0;
					touch_events_count++;
				}
				break;
			}
#endif
			case SDL_QUIT:
			{
				this->state = EXITING;
				break;
			}
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

	this->CheckTouch( lux::display, touch_events_count );

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

bool CoreSystem::TextListen( bool able )
{
	if ( able )
	{
		SDL_StartTextInput();
	}
	else
	{
		SDL_StopTextInput();
	}
	return SDL_HasScreenKeyboardSupport();
}

void CoreSystem::CheckTouch( DisplaySystem * display, uint8_t touch_events_count )
{
	if ( display )
	{
		int32_t points[2];
		int16_t previ_state;


		for ( std::map<uint32_t, VirtualGamepadButton*>::iterator item = this->virtual_input.begin(); item != this->virtual_input.end(); item++ )
		{
			VirtualGamepadButton * button = (*item).second;

			if ( button->object )
			{


				if ( touch_events_count )
				{
					uint8_t c = 0;
					while ( c < touch_events_count )
					{
						bool hit = false;
						points[0] = (int32_t)(this->touch_events[c].x * display->screen_dimension.w);
						points[1] = (int32_t)(this->touch_events[c].y * display->screen_dimension.h);

						display->graphics.Display2Screen( &points[0], &points[1] );

						if ( button->object->position.x <= points[0] && button->object->position.x + button->object->position.w >= points[0] )
						{
							if ( button->object->position.y <= points[1] && button->object->position.y + button->object->position.h >= points[1] )
							{
								hit = true;
							}
						}

						if ( hit )
						{
							button->state = (this->touch_events[c].type = SDL_FINGERDOWN ? 1 : 0);
						}



						c++;
					}
				}
				else
				{
					button->state = 0;
				}

			}
			else
			{
				button->object = new MapObject(OBJECT_RECTANGLE);
				button->object->position.x = ( button->rect.x < 0 ? display->screen_dimension.w + button->rect.x : button->rect.x );
				button->object->position.y = ( button->rect.y < 0 ? display->screen_dimension.h + button->rect.y : button->rect.y );
				button->object->position.w = button->rect.w;
				button->object->position.h = button->rect.h;
				button->object->effects.primary_colour = { 255, 0,0, 255 };

				display->AddObjectToLayer( 0xFFFFFFFF, button->object, true );
			}

		}
	}
}

bool CoreSystem::InputLoopGet( DisplaySystem * display, uint16_t & key )
{
	SDL_Event event;
	uint8_t touch_events_count = 0;

	if ( SDL_PollEvent(&event) )
	{
		switch( event.type )
		{
			case SDL_TEXTINPUT:
			{
				key = event.text.text[0];
				break;
			}
			case SDL_KEYDOWN:
				key = event.key.keysym.sym;
				if ( event.key.keysym.sym == SDLK_ESCAPE ) { key = 27; }
				if ( event.key.keysym.sym == SDLK_RETURN && ( event.key.keysym.mod & KMOD_ALT ) ) {
					SDL_Window *window = SDL_GetWindowFromID(event.key.windowID);
					if (window)
					{
						Uint32 flags = SDL_GetWindowFlags(window);
						display->graphics.SetFullscreen( !!(flags & SDL_WINDOW_FULLSCREEN) );
					}
				}
				break;
			case SDL_QUIT:
				key = 27;
				break;

			case SDL_FINGERDOWN:
			case SDL_FINGERUP:
			{
				if ( touch_events_count < 10 )
				{
					this->touch_events[touch_events_count].type = event.tfinger.type;
					this->touch_events[touch_events_count].x = event.tfinger.x;
					this->touch_events[touch_events_count].y = event.tfinger.y;
					this->touch_events[touch_events_count].dx = event.tfinger.dx;
					this->touch_events[touch_events_count].dy = event.tfinger.dy;
					this->touch_events[touch_events_count].pressure = event.tfinger.pressure;
					touch_events_count++;
				}
				break;
			}

			case SDL_DROPFILE:
			{
				SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_INFORMATION, "File dropped on window", event.drop.file, NULL );
				SDL_free(event.drop.file);
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

		this->CheckTouch( display, touch_events_count );
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
			if ( this->keystate && symbol < keystate_count && symbol >= 0 )
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
		case MOUSEWHEEL:
		{
			if ( this->mousestate & SDL_BUTTON(symbol) )
			{
				return 1;
			}
			return 0;
		}
		case VIRTUALBUTTON:
		{
			std::map<uint32_t, VirtualGamepadButton*>::iterator item = this->virtual_input.find( (uint32_t)symbol );
			if ( item != this->virtual_input.end() )
			{
				VirtualGamepadButton * button = (*item).second;
				return button->state;
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



void CoreSystem::VirtualGamepadAddItem( uint32_t ident, InputDevice device, std::string value )
{
	// -10x-10,20x20
	VirtualGamepadButton * button = new VirtualGamepadButton();
	std::string axis;
	std::string dimension;
	std::string::size_type split_position;

	split_position = value.find_first_of(",", 0);

	if ( std::string::npos != split_position )
	{
		axis = value.substr(0, split_position);
		dimension = value.substr(split_position+1);

		//axis values
		split_position = axis.find_first_of("x", 0);
		if ( std::string::npos != split_position )
		{
			button->rect.x = elix::string::ToInt32( axis.substr(0, split_position) );
			button->rect.y = elix::string::ToInt32( axis.substr(split_position+1) );
		}

		//dimension values
		split_position = dimension.find_first_of("x", 0);
		if ( std::string::npos != split_position )
		{
			button->rect.w = elix::string::ToIntU16( dimension.substr(0, split_position) );
			button->rect.h = elix::string::ToIntU16( dimension.substr(split_position+1) );
		}

		button->device = device;

		this->virtual_input.insert( std::pair<uint32_t, VirtualGamepadButton*>( ident, button ) );
	}
	else
	{
		delete button;
	}


}

void CoreSystem::VirtualGamepadRemoveItem( uint32_t ident )
{
	this->virtual_input.erase( ident );
}

