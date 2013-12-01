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
	this->platform = "flash";

	this->SetBoolean("debug.enable", false);
	this->SetBoolean("audio.able", false);
	this->SetString("project.title", "Lux Engine" );
	this->SetString("project.language", "en" );
	this->SetString("project.id", "unknowngame" );
	this->SetString("control.set1", "controlaxisA0|controlaxisA2|controlaxisA0|b102|b101|b110|b109|t14|t15|b101|b101|b104|b105|b106|b107|b101|b101|b101|b101" );
	this->SetString("control.buttons1", "b103|b101|b102" );
}
