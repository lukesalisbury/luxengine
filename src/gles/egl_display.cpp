/****************************
Copyright © 2013-2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/


#include "egl_display.hpp"

#if REQUIRE_EGL
#include <EGL/egl.h>

namespace egl_display {
	EGLDisplay egl_display = 0;
	EGLContext egl_context = 0;
	EGLSurface egl_surface = 0;
	EGLConfig egl_config = 0;

	int TestEGLError( void )
	{
		EGLint iErr = eglGetError();
		while (iErr != EGL_SUCCESS)
		{
			printf("EGL failed (%d).\n", iErr);
			return 0;
		}

		return 1;
	}

	bool open(uint32_t window_width, uint32_t window_height, bool window_fullscreen, uint8_t colour_depth)
	{
		//EGL/eglplatform.h
		const EGLint egl_config_attribs[] = {
			EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
			EGL_BLUE_SIZE, 5,
			EGL_GREEN_SIZE, 6,
			EGL_RED_SIZE, 5,
			EGL_RENDERABLE_TYPE, EGL_OPENGL_ES_BIT,
			EGL_NONE
		};
		EGLNativeDisplayType native_display = 0;
		EGLNativeWindowType native_window = 0;
		Display *x11Display = NULL;

		x11Display = XOpenDisplay(NULL);
		if (!x11Display)
		{
			lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << "ERROR: unable to get display!" << std::endl;
			return 0;
		}

		egl_display = eglGetDisplay((EGLNativeDisplayType)x11Display );
		if ( egl_display == EGL_NO_DISPLAY )
		{
			TestEGLError( );
			lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << "Unable to initialise EGL display. a" << std::endl;
			return false;
		}


		// Initialise egl
		if ( !eglInitialize(egl_display, NULL, NULL ))
		{
			TestEGLError( );
			lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << "Unable to initialise EGL display." << std::endl;
			return 0;
		}
		/*
		EGLint numConfigs = 0;
		if ( !eglGetConfigs(egl_display, NULL, 0, &numConfigs) )
		{
			TestEGLError( );
			lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << "Unable to get a config for EGL " << numConfigs << std::endl;
			return 0;
		}


		if ( eglChooseConfig(egl_display, egl_config_attribs, &egl_config, 1, &numConfigs) != EGL_TRUE)
		{
			TestEGLError( );
			lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << "Unable to find a config for EGL " << numConfigs << std::endl;
			return 0;
		}
		lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << "Unable to find a config for EGL " << numConfigs << std::endl;
		*/

		// Bind GLES and create the context
		eglBindAPI(EGL_OPENGL_ES_API);
		EGLint contextParams[] = {EGL_CONTEXT_CLIENT_VERSION, USING_GLES, EGL_NONE};

		egl_context = eglCreateContext( egl_display, egl_config, EGL_NO_CONTEXT, contextParams);
		if (egl_context == EGL_NO_CONTEXT)
		{
			lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << "Unable to create GLES context!" << eglGetError() << std::endl;
			return 0;
		}

		/*

		SDL_SysWMinfo info;
		SDL_VERSION(&info.version);


		if ( SDL_GetWMInfo(&info) == 1 )
		{
			//native_display = (EGLNativeDisplayType)info.info.x11.display;
			//native_display = (EGLNativeDisplayType)info.info.x11.gfxdisplay;
			native_window = (EGLNativeWindowType)info.info.x11.window;

			//native_window = (EGLNativeWindowType)info.info.x11.wmwindow;
		}
		*/

		egl_surface = eglCreateWindowSurface(egl_display, egl_config, native_window, NULL);
		if ( egl_surface == EGL_NO_SURFACE )
		{
			TestEGLError( );

			lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << "Unable to create EGL surface!" << std::endl;
			return 0;
		}



		if ( eglMakeCurrent( egl_display,  egl_surface,  egl_surface, egl_context) == EGL_FALSE )
		{
			lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << "Unable to make GLES context current" << std::endl;
			return 0;
		}


		return true;
	}

	bool close()
	{
		return false;
	}

	void swapBuffer()
	{
	}
}





#endif
