/****************************
Copyright © 2006-2012 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "engine.h"
#include "portal.h"
#include "core.h"
#include "config.h"
#include <errno.h>

#include <EGL/egl.h>
#include <GLES/gl.h>
#include <jni.h>
#include <android/sensor.h>
#include <android/log.h>
#include <android_native_app_glue.h>
#include <android/native_window.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "luxengine", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "luxengine", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "luxengine", __VA_ARGS__))

android_app * android_app_details = NULL;





void android_main( struct android_app * state)
{
	// Make sure glue isn't stripped.
	app_dummy();


	LOGW("android_main %p", state);






	std::string external_path = state->activity->externalDataPath;


	LOGW("externalDataPath %s", state->activity->externalDataPath);



	lux::engine = new LuxEngine("/storage/sdcard0/puttytris.game", external_path);
	lux::core->SetAndroidApp(state);

	luxportal::open();
	luxportal::Test();
	luxportal::close();
/*
	lux::engine = new LuxEngine("/storage/sdcard0/puttytris.game", external_path);
	lux::core->SetAndroidApp(state);
	if ( lux::engine->Start() )
	{
		lux::engine->Loop();
	}
	else
	{
		LOGW("No Game Found");
	}
	*/



}

