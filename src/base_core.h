/****************************
Copyright © 2006-2011 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef _BASECORE_H_
	#define _BASECORE_H_

#define SYSTEM_MESSAGE_ERROR 1
#define SYSTEM_MESSAGE_INFO 2
#define SYSTEM_MESSAGE_DEBUG 3
#define SYSTEM_MESSAGE_WARNING 4


#include "luxengine.h"
#include "lux_types.h"
#include "display.h"
#include <vector>
#include <string>

class BaseCoreSystem
{
public:
	BaseCoreSystem() {};
	virtual ~BaseCoreSystem() {};

protected:
	bool good;
	uint32_t time;
	uint32_t frame_ms;
	uint32_t animation_ms;
	uint32_t internal_ms;
	LuxState state;

public:
	virtual void SystemMessage(uint8_t type, std::string message) = 0;
	virtual void AbleOutput(bool able) = 0;
	virtual uint32_t WasInit(uint32_t flag) = 0;
	virtual void QuitSubSystem(uint32_t flag) = 0;
	virtual bool InitSubSystem(uint32_t flag) = 0;
	virtual bool Good() = 0;
	virtual uint32_t GetTime() = 0;
	virtual uint32_t GetFrameDelta() = 0;
	uint32_t GetAnimationDelta() { return this->animation_ms; }
	uint32_t GetInternalDelta() { return this->internal_ms; }
	virtual bool DelayIf(uint32_t diff = 5) = 0;
	virtual void Idle() = 0;
	virtual LuxState HandleFrame(LuxState old_state) = 0;
	virtual void RefreshInput( DisplaySystem * display ) = 0;
	virtual int16_t GetInput(InputDevice device, uint32_t device_number, int32_t symbol) = 0;
	virtual bool InputLoop(DisplaySystem * display, uint16_t & key) = 0;

#ifdef NETWORKENABLED
	virtual bool CreateMessage(uint8_t type, bool reliable) = 0;
	virtual bool MessageAppend(fixed data) = 0;
	virtual bool MessageAppend(uint8_t data) = 0;
	virtual bool MessageAppend(uint32_t data) = 0;
	virtual bool MessageSend(bool wait = false) = 0;
	virtual int32_t ReadMessage(void * data) = 0;
	virtual void NetLock() = 0;
	virtual void NetUnlock() = 0;
#endif

};

#endif
