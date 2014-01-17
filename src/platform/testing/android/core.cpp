/****************************
Copyright (c) 2006-2012 Luke Salisbury
Copyright (c) 2008 Mokoi Gaming
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include "core.h"
#include "luxengine.h"

#include <android/sensor.h>
#include <android/log.h>
#include <android_native_app_glue.h>
#include <android/native_window.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "luxengine", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "luxengine", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "luxengine", __VA_ARGS__))

bool android_window = false;

uint8_t android_keyinput[128];


static int32_t engine_handle_input(android_app* app, AInputEvent* event){
	if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION)
	{
		size_t pointerCount = AMotionEvent_getPointerCount(event);
		for (size_t i = 0; i < pointerCount; ++i)
		{
			LOGI("Received motion event from pointer %zu: (%.2f, %.2f)", i, AMotionEvent_getX(event, i), AMotionEvent_getY(event, i));
		}
		return 1;
	}
	else if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY)
	{
		LOGI("Received key event: %d", AKeyEvent_getKeyCode(event));
		return 1;
	}
	return 0;
}

static void engine_handle_cmd(android_app* app, int32_t cmd)
{
	if ( lux::core )
	{
		lux::core->HandleAndroidCommand(app, cmd);
	}
}


/* Local functions */
CoreSystem::CoreSystem()
{
	this->android_state = NULL;
	this->good = true;
}

void CoreSystem::SetAndroidApp(android_app * state)
{
	android_state = state;
	state->onAppCmd = engine_handle_cmd;
	state->onInputEvent = engine_handle_input;
}

void CoreSystem::HandleAndroidCommand(android_app* app, int32_t cmd)
{
	switch (cmd) {
		case APP_CMD_SAVE_STATE:
			LOGW("APP_CMD_SAVE_STATE");
			break;
		case APP_CMD_INIT_WINDOW:
			LOGW("APP_CMD_INIT_WINDOW");
			android_window = true;
			break;
		case APP_CMD_TERM_WINDOW:
			LOGW("APP_CMD_TERM_WINDOW");
			this->state = PAUSED;
			android_window = false;
			break;
		case APP_CMD_GAINED_FOCUS:
			LOGW("APP_CMD_GAINED_FOCUS");
			this->state = RUNNING;
			break;
		case APP_CMD_LOST_FOCUS:
			LOGW("APP_CMD_LOST_FOCUS");
			this->state = PAUSED;
			break;
	}
}

CoreSystem::~CoreSystem()
{

}


void CoreSystem::SystemMessage(uint8_t type, std::string message)
{
	LOGE(message.c_str());
}

uint32_t CoreSystem::WasInit(uint32_t flag)
{
	if ( flag == LUX_INIT_VIDEO )
	{
		return android_window;
	}
	return 1;
}

void CoreSystem::QuitSubSystem(uint32_t flag)
{

}
bool CoreSystem::InitSubSystem(uint32_t flag)
{
	if ( flag == LUX_INIT_VIDEO )
	{
		while ( !android_window )
		{
			RefreshInput( NULL );
		}
	}
	return true;
}

bool CoreSystem::Good()
{
	return this->good;
}

uint32_t CoreSystem::GetTime()
{
	return 0;
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

}

LuxState CoreSystem::HandleFrame(LuxState old_state)
{
	if ( this->state > PAUSED || old_state >= SAVING )
	{
		this->state = old_state;
	}

	this->RefreshInput(lux::display);
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

			return 0;
		}
		case CONTROLBUTTON:
		{
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
		// Read all pending events.
		int ident;
		int events;
		struct android_poll_source* source;

		// Read events and draw a frame of animation.
		if ((ident = ALooper_pollAll(0, NULL, &events, (void**)&source)) >= 0) {
			// Process this event.
			if (source != NULL) {
				source->process(android_state, source);
			}
		}

}

bool CoreSystem::InputLoopGet(uint16_t & key)
{
	// Keyboard Stuff
	return 0;
}



