/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef BASE_CORE_H
	#define BASE_CORE_H

#define SYSTEM_MESSAGE_ERROR 1
#define SYSTEM_MESSAGE_WARNING 2
#define SYSTEM_MESSAGE_VISUAL_WARNING 3
#define SYSTEM_MESSAGE_DEBUG 4
#define SYSTEM_MESSAGE_INFO 5
#define SYSTEM_MESSAGE_LOG 6


#include "luxengine.h"
#include "lux_types.h"
#include "display/display.h"
#include <vector>
#include <string>
#include <iostream>

struct function_time {
	uint32_t calls;
	uint64_t msec;
	std::string function;

};

class BaseCoreSystem
{
public:
	BaseCoreSystem( const void * window_ptr ) {}
	BaseCoreSystem() {}
	virtual ~BaseCoreSystem() {}

protected:
	bool good;
	uint32_t time;
	uint32_t frame_ms;
	uint32_t animation_ms;
	uint32_t internal_ms;
	LuxState state;
	function_time timer[32];

public:
	virtual std::ostream& SystemMessage(uint8_t type) = 0;
	virtual void SystemMessage(uint8_t type, std::string message) = 0;
	virtual std::ostream& SystemMessage(const char *file, int line, uint8_t type ) = 0;
	virtual void AbleOutput(bool able) = 0;
	virtual uint32_t WasInit(uint32_t flag) = 0;
	virtual void QuitSubSystem(uint32_t flag) = 0;
	virtual bool InitSubSystem(uint32_t flag) = 0;
	virtual bool Good() = 0;
	virtual uint32_t GetTime() = 0;
	uint32_t GetFrameDelta() { return this->frame_ms; }
	uint32_t GetAnimationDelta() { return this->animation_ms; }
	uint32_t GetInternalDelta() { return this->internal_ms; }
	void UpdateDelta() {
		this->internal_ms = (this->GetTime() - this->time);
		this->frame_ms = clamp( this->internal_ms, 0, 33);
		this->animation_ms = this->frame_ms;
	}
	virtual bool DelayIf(uint32_t diff = 5) = 0;
	virtual void Idle() = 0;
	virtual LuxState HandleFrame(LuxState old_state) = 0;
	virtual void RefreshInput( DisplaySystem * display ) = 0;
	virtual int16_t GetInput(InputDevice device, uint32_t device_number, int32_t symbol) = 0;
	virtual bool InputLoopGet(DisplaySystem * display, uint16_t & key) = 0;
	virtual bool TextListen( bool able ) = 0;

	bool RunExternalProgram( std::string program, std::string argument ) { return false; }

	void NetworkLock() {  }
	void NetworkUnlock() {  }

	virtual void RecordFunctionTimer( uint8_t timer, const char * name, uint64_t length_msec ) = 0;
	virtual void OutputInformation() = 0;

};

#endif
