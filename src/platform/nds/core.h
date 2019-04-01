/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef CORE_H
	#define CORE_H

	#include <stdarg.h>
	#include <nds.h>
	#include "base_core.h"

	class CoreSystem: public BaseCoreSystem
	{
		public:
			CoreSystem();
			~CoreSystem();
		private:
			bool lockfps;
			int32_t touch_time;
			bool touch_press, last_press;
		public:
			void SystemMessage(uint8_t type, std::string message);
			void AbleOutput(bool able) {};
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
			bool TextListen(bool able) { return false; }

			#ifdef NETWORKENABLED
			bool CreateMessage(uint8_t type, bool reliable);
			bool MessageAppend(fixed data);
			bool MessageAppend(uint8_t data);
			bool MessageAppend(uint32_t data);
			bool MessageSend(bool wait = false);
			int32_t ReadMessage(void * data);
			void NetLock();
			void NetUnlock();
			#endif

	};

namespace lux
{
	extern CoreSystem * core;
}

#endif

