/****************************
Copyright (c) 2006-2012 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

void Config::PlatformSettings()
{
	this->platform = "base";

	this->SetNumber("display.limit", 60);
	this->SetNumber("display.width", 256);
	this->SetNumber("display.height", 192);
	this->SetNumber("display.autoscale", 1);
	this->SetNumber("map.width", 256);
	this->SetNumber("map.height", 192);
	this->SetNumber("screen.width", 256);
	this->SetNumber("screen.height", 192);
	this->SetBoolean("debug.enable", false);
	this->SetBoolean("audio.able", false);
	this->SetString("player.controller1", "arrows|mouse|mouse|k0|k1|k10|k11|k8|k9|k2|k4|k5|k6|k7|m1|k3|k13|k12|k12" );
	this->SetString("controller.buttons1", "k2|k1|t1" );

}
