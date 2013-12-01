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
#include <wiiuse/wpad.h>

void parse_pointer_settings(Player_Pointer * pointer, std::string config)
{
	pointer->device = MOUSEAXIS;
}

void parse_button_settings(Player_Button * button, std::string config)
{
	button->device = CONTROLBUTTON;

}

void parse_axis_settings(Player_Axis * axis, std::string config)
{
	axis->device = CONTROLBUTTON;
	axis->sym[0] = WPAD_BUTTON_LEFT;
	axis->sym[1] = WPAD_BUTTON_RIGHT;
	axis->sym[2] = WPAD_BUTTON_UP;
	axis->sym[3] = WPAD_BUTTON_DOWN;
	axis->sym[4] = WPAD_BUTTON_UP;
	axis->sym[5] = WPAD_BUTTON_DOWN;
}