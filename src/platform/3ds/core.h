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

	#include <stdarg.h>
	#include <3ds.h>
	#include <sf2d.h>
	#include "base_core.h"

	typedef struct {
		uint8_t * buffer;
		uint16_t width, height;
	} screen_buffer;

	class CoreSystem: public BaseCoreSystem
	{
		public:
			CoreSystem();
			~CoreSystem();
		private:
			bool lockfps;
			int32_t touch_time;
			bool touch_press, last_press;
			uint32_t fps, fps_time, fps_last, frame_length;
			screen_buffer frame_buffer[2];
		public:
			void SystemMessage(uint8_t type, std::string message);
			std::ostream& SystemMessage( const char *file, int line, uint8_t type );
			std::ostream& SystemMessage(uint8_t type);

			void AbleOutput(bool able) {}
			uint32_t WasInit(uint32_t flag);
			void QuitSubSystem(uint32_t flag);
			bool InitSubSystem(uint32_t flag);
			bool Good();
			uint32_t GetTime();
			uint32_t GetFrameDelta();

			uint32_t GetAnimationDelta() { return this->animation_ms; }
			uint32_t GetInternalDelta() { return this->internal_ms; }

			bool DelayIf(uint32_t diff = 5);

			void Idle();
			LuxState HandleFrame(LuxState old_state);
			void RefreshInput( DisplaySystem * display );
			int16_t GetInput(InputDevice device, uint32_t device_number, int32_t symbol);
			bool InputLoopGet(DisplaySystem * display, uint16_t & key);
			bool TextListen(bool able) { return false; }

	public:
			gfxScreen_t primary_screen;
			gfxScreen_t secondary_screen;
	};

namespace lux
{
	extern CoreSystem * core;
}

#endif

