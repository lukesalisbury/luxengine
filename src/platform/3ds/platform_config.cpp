/****************************
Copyright (c) 2006-2012 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "config.h"

void Config::PlatformSettings()
{

	this->platform = "3ds";


	this->SetNumber("display.bpp", 0);
	this->SetBoolean("display.fullscreen", true);
	this->SetNumber("display.scale", 1);
	this->SetNumber("display.limit", 60);
	this->SetNumber("display.width", 320);
	this->SetNumber("display.height", 240);
	this->SetNumber("map.width", 512);
	this->SetNumber("map.height", 384);
	this->SetNumber("screen.width", 320);
	this->SetNumber("screen.height", 240);
	this->SetBoolean("debug.enable", true);
	this->SetBoolean("audio.able", true);
	this->SetBoolean("entities.rebuild", false);
	this->SetString("project.title", "Lux Engine" );

	this->SetString("project.id", "unknowngame" );
	this->SetString("window.title", "Game" );

	/* axis-1|axis-2|pointer|BUTTON_ACTION1|ACTION2|ACTION3|ACTION4|ACTION5|ACTION6|MENU|ARROWRIGHT|ARROWLEFT|ARROWDOWN|ARROWUP|MOUSELEFT|MOUSEMIDDLE|MOUSERIGHT|MOUSEUP|MOUSEDOWN|CONFIRM|CANCEL|POINTER_PRESS|shutdown*/
	this->SetString("controller.keyboard1", "arrows|mouse|mouse|k04|k022|k07|k020|k026|k08|k040|k079|k080|k081|k082|m01|m02|m03|m04|k041|k040|k041|m01|k061" );
	this->SetString("controller.virtual1", "arrows|n|mouse|nA1|nA2|nA3|nA4|nA5|nA6|m01|r,50x-40,30x30|r,10x-40,30x30|n|r,-50x-50,40x40|nML|nMM|nMR|nMU|nMD|k040|k041|m01|k061" );

	this->SetString("player.controller1", "default0" );
	this->SetString("player.controller2", "default0" );
	this->SetString("player.controller3", "default0" );
	this->SetString("player.controller4", "default0" );

	this->SetBoolean("save.allowed", true );
	this->SetString("server.ip", "127.0.0.1" );
	this->SetBoolean("server.able", false);
	this->SetString("client.entity", "client");
	this->SetBoolean("shader.able", true);

	this->SetBoolean("layer.static6", true);

}
