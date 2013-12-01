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
	#ifdef __GNUWIN32__
	platform = "win32";
	#endif

	this->SetNumber("display.bpp", 0);
	this->SetString("display.surface", "software");
	this->SetBoolean("display.dbuff", true);
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
	this->SetString("project.title", "Lux Engine" );

	this->SetString("project.id", "unknowngame" );

	this->SetString("control.keyboard1", "arrows|mouse|mouse|k97|k115|k100|k113|k119|k101|k13|k273|k274|k275|k276|m1|m2|m3|k112|k27|k13|k27|m1|k27" );
	this->SetString("control.gamepad0", "controlaxisA0|controlaxisA1|controlaxisA1|b100|b101|b102|b103|b104|b105|b109|b106|b107|b108|b109|m1|m2|m3|k112|k27|b109|b108|b101|k27" );

	this->SetString("control.set1", "keyboard1" );
	this->SetString("control.set2", "gamepad0" );


	this->SetBoolean("save.allowed", true );
	this->SetString("server.ip", "127.0.0.1" );
	this->SetBoolean("server.able", false);
	this->SetString("client.entity", "client");
	this->SetBoolean("shader.able", true);

	this->SetBoolean("layer.static6", true);

}
