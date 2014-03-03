/****************************
Copyright © 2009-2012  Luke Salisbury

http://creativecommons.org/licenses/by-nc-sa/3.0/

You are free:
to Share — to copy, distribute and transmit the work
to Remix — to adapt the work

Under the following conditions:
Attribution — You must attribute the work in the manner specified by the author or licensor (but not in any way that suggests that they endorse you or your use of the work).
Noncommercial — You may not use this work for commercial purposes.
Share Alike — If you alter, transform, or build upon this work, you may distribute the resulting work only under the same or similar license to this one.

With the understanding that:
Waiver — Any of the above conditions can be waived if you get permission from the copyright holder.
Public Domain — Where the work or any of its elements is in the public domain under applicable law, that status is in no way affected by the license.
Other Rights — In no way are any of the following rights affected by the license:
 - Your fair dealing or fair use rights, or other applicable copyright exceptions and limitations;
 - The author's moral rights;
 - Rights other persons may have either in the work itself or in how the work is used, such as publicity or privacy rights.
Notice — For any reuse or distribution, you must make clear to others the license terms of this work. The best way to do this is with a link to this web page.

****************************/
#include "config.h"

void Config::PlatformSettings()
{
	#ifdef __GNUWIN32__
	platform = "win32";
	#endif

	this->SetNumber("display.bpp", 0);
	this->SetString("display.surface", "software");
	this->SetString("display.mode", "OpenGL");
	this->SetBoolean("display.dbuff", true);
	this->SetBoolean("display.fullscreen", false);
	this->SetNumber("display.scale", 1);
	this->SetNumber("display.limit", 100);
	this->SetNumber("display.width", 512);
	this->SetNumber("display.height", 384);
	this->SetNumber("map.width", 512);
	this->SetNumber("map.height", 384);
	this->SetNumber("screen.width", 512);
	this->SetNumber("screen.height", 384);
	this->SetBoolean("debug.enable", false);
	this->SetBoolean("audio.able", true);

	this->SetString("project.title", "Open Zelda" );
	this->SetString("project.id", "0" );

	this->SetString("player.controller1", "arrows|mouse|mouse|k97|k115|k100|k113|k119|k101|k13|k273|k274|k275|k276|m1|m2|m3|k112|k27" );
	this->SetString("controller.buttons1", "k13|k27|m1" );

	this->SetString("player.controller2", "controlaxisA0|controlaxisA1|controlaxisA1|b100|b101|b102|b103|b104|b105|b108|b106|b107|b108|b109|m1|m2|m3|k112|k27" );
	this->SetString("controller.buttons2", "b107|b106|b101" );

	this->SetString("player.controller3", "controlaxisB0|controlaxisB1|controlaxisB1|b200|b201|b202|b203|b204|b205|b208|b206|b207|b208|b209|m1|m2|m3|k112|k27" );
	this->SetString("controller.buttons3", "b207|b206|b201" );




	this->SetString("controller.name", "Controller (XBOX 360 For Windows)");




	this->SetBoolean("save.allowed", true );

	this->SetBoolean("server.able", false);
	this->SetString("client.entity", "client");
	this->SetBoolean("update.auto", false);
	this->SetBoolean("patches.scan", false);

}
