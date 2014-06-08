/****************************
Copyright (c) 2006-2012 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "config.h"
#include <SDL_keycode.h>


void Config::PlatformSettings()
{
	#ifdef __GNUWIN32__
	platform = "win32";
	#else
	platform =  SDL_GetPlatform();
	#endif

	this->SetNumber("display.bpp", 0);
	this->SetBoolean("display.fullscreen", false);
	this->SetNumber("display.scale", 1);
	this->SetNumber("display.limit", 60);
	this->SetNumber("display.width", 512);
	this->SetNumber("display.height", 384);
	this->SetNumber("map.width", 512);
	this->SetNumber("map.height", 384);
	this->SetNumber("screen.width", 512);
	this->SetNumber("screen.height", 384);
	this->SetBoolean("debug.enable", false);
	this->SetBoolean("audio.able", true);
	this->SetBoolean("entities.rebuild", false);
	this->SetString("project.title", "Lux Engine" );

	this->SetString("project.id", "unknowngame" );



	/* PC inputuse SDL_SCANCODE */
	/*
	SDL_SCANCODE_A = 4
	SDL_SCANCODE_S = 22
	SDL_SCANCODE_D = 7
	SDL_SCANCODE_Q = 20
	SDL_SCANCODE_W = 26
	SDL_SCANCODE_E = 8
	SDL_SCANCODE_RETURN = 40
	SDL_SCANCODE_ESCAPE = 41
	SDL_SCANCODE_RIGHT = 79
	SDL_SCANCODE_LEFT = 80
	SDL_SCANCODE_DOWN = 81
	SDL_SCANCODE_UP = 82
	SDL_SCANCODE_F4 = 61
	*/


	/* axis-1|axis-2|pointer|BUTTON_ACTION1|ACTION2|ACTION3|ACTION4|ACTION5|ACTION6|MENU|ARROWUP|ARROWDOWN|ARROWRIGHT|ARROWLEFT|MOUSELEFT|MOUSEMIDDLE|MOUSERIGHT|MOUSEUP|MOUSEDOWN|CONFIRM|CANCEL|POINTER_PRESS|shutdown*/

	this->SetString("controller.keyboard1", "arrows|mouse|mouse|k04|k022|k07|k020|k026|k08|k040|k079|k080|k081|k082|m01|m02|m03|m04|k041|k040|k041|m01|k061" );
	this->SetString("controller.virtual1", "arrows|n|mouse|r,-50x-50,40x40|n|n|n|n|n|m01|n|n|r,50x-40,30x30|r,10x-40,30x30|n|n|n|n|n|k040|k041|m01|k061" );


	this->SetString("player.controller1", "keyboard1" );
	this->SetString("player.controller2", "gamepad0" );
	this->SetString("player.controller3", "gamepad1" );
	this->SetString("player.controller4", "virtual1" );

	this->SetBoolean("save.allowed", true );
	this->SetString("server.ip", "127.0.0.1" );
	this->SetBoolean("server.able", false);
	this->SetString("client.entity", "client");
	this->SetBoolean("shader.able", true);

	this->SetBoolean("layer.static6", true);

}
