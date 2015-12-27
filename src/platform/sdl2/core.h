/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef _CORE_H_
	#define _CORE_H_

	#include <SDL.h>
	#include <SDL_thread.h>
	#include <SDL_touch.h>
	#include "base_core.h"


	typedef struct {
		InputDevice device;
		int16_t state;
		LuxRect rect;
		int16_t radius;
		MapObject * object;
	} VirtualGamepadButton;

	typedef struct {
		uint32_t type;
		uint32_t id;
		float x, y, dx, dy, pressure;
	} TouchEvent;

	class CoreSystem: public BaseCoreSystem
	{
		public:
			CoreSystem( const void * window_ptr );
			CoreSystem();
			~CoreSystem();
		private:
			const uint8_t * keystate;
			int keystate_count;
			bool mouse_button[6];
			int32_t mouse_scroll;
			int32_t mouse_position[2];


			SDL_GameController * controller[8];
			SDL_Window * native_window;
			SDL_Window * message_window;


			TouchEvent touch_events[10];
			std::map<uint32_t, VirtualGamepadButton*> virtual_input;
			void CheckTouch( DisplaySystem * display, uint8_t touch_events_count );

			uint32_t fps, fps_time, fps_last, frame_length;
			bool lockfps;
			bool mouse_focus;

		public:
			SDL_Window * GetWindow() { return native_window; }
			SDL_Window * GetMessageWindow() { return message_window; }

			bool GamepadAdded( int32_t joystick_index );
			const char * GamepadName( uint32_t device_number );

			void VirtualGamepadAddItem( uint32_t ident, InputDevice device, std::string value );
			void VirtualGamepadRemoveItem( uint32_t ident );

			void SystemMessage(uint8_t type, std::string message);
			std::ostream& SystemMessage( const char *file, int line, uint8_t type );
			std::ostream& SystemMessage(uint8_t type);

			void AbleOutput(bool able);
			uint32_t WasInit(uint32_t flag);
			void QuitSubSystem(uint32_t flag);
			bool InitSubSystem(uint32_t flag);
			bool Good();
			uint32_t GetTime();
			uint32_t GetFrameDelta();
			bool DelayIf(uint32_t diff = 5);
			void Idle();

			LuxState HandleFrame(LuxState old_state);
			void RefreshInput( DisplaySystem * display );
			int16_t GetInput(InputDevice device, uint32_t device_number, int32_t symbol);
			bool InputLoopGet(DisplaySystem * display, uint16_t & key);
			bool TextListen( bool able );

			bool RunExternalProgram( std::string program, std::string argument );

			void RecordFunctionTimer( uint8_t timer, const char * name, uint64_t length_msec );
			void OutputInformation();
	};

namespace lux
{
	extern CoreSystem * core;
}

#endif
