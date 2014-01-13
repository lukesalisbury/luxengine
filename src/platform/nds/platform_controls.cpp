/****************************
Copyright (c) 2006-2012 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include "player.h"
#include <sstream>
#include <nds.h>

void Player::ClearController()
{

}

void parse_pointer_settings(Player_Pointer * pointer, std::string config)
{
	pointer->device = MOUSEAXIS;
}

void parse_button_settings(Player_Button * button, std::string config)
{
	if (config.at(0) == 't')
		button->device = TOUCHSCREEN;
	else if (config.at(0) == 'm')
		button->device = MOUSEBUTTON;
	else
		button->device = KEYBOARD;
	std::stringstream stream(config.substr(1));
	int key = 0;
	stream >> key;
	button->sym = BIT(key);
}

void parse_axis_settings(Player_Axis * axis, std::string config)
{
	axis->device = KEYBOARD;
	axis->sym[0] = KEY_LEFT;
	axis->sym[1] = KEY_RIGHT;
	axis->sym[2] = KEY_UP;
	axis->sym[3] = KEY_DOWN;
	axis->sym[4] = KEY_L;
	axis->sym[5] = KEY_R;
}
