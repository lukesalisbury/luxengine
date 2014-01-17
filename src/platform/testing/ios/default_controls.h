/****************************
Copyright © 2006-2011 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef _DEFAULT_CONTROLS_H_
	#define _DEFAULT_CONTROLS_H_

#include "../../luxengine.h"
#include <SDL/SDL_keysym.h>
#include <sstream>

void parse_pointer_settings(Player_Pointer * pointer, std::string config)
{
	if (config == "mouse")
	{
		pointer->device = MOUSEAXIS;
	}
	else
	{
		pointer->device = NOINPUT;
	}
}
void parse_button_settings(Player_Button * button, std::string config)
{
	uint16_t device = config.at(0);
	std::stringstream stream(config.substr(1));
	button->device_number = 0;
	button->sym = 0;
	stream >> button->sym;
	switch (device)
	{
		case 'k':
			button->device = KEYBOARD;
			break;
		case 'm':
			button->device = MOUSEBUTTON;
			break;
		case 'b':
			button->device = CONTROLBUTTON;
			button->device_number = (button->sym / 100);
			button->sym -= (button->device_number * 100);
			button->device_number -= 1;
			break;
		case 'h':
			button->device = CONTROLHAT;
			button->device_number = (button->sym / 100);
			button->sym -= (button->device_number * 100);
			button->device_number -= 1;
			break;
		default:
			button->device = NOINPUT;
			break;
	}
}

void parse_axis_settings(Player_Axis * axis, std::string config)
{
	if (config == "arrows")
	{
		axis->device = KEYBOARD;
		axis->sym[0] = SDLK_LEFT;
		axis->sym[1] = SDLK_RIGHT;
		axis->sym[2] = SDLK_UP;
		axis->sym[3] = SDLK_DOWN;
		axis->sym[4] = SDLK_PAGEUP;
		axis->sym[5] = SDLK_PAGEDOWN;
	}
	else if (config == "numpad")
	{
		axis->device = KEYBOARD;

	}
	else if (config == "keyboard-wasd")
	{
		axis->device = KEYBOARD;
		axis->sym[0] = SDLK_a;
		axis->sym[1] = SDLK_d;
		axis->sym[2] = SDLK_w;
		axis->sym[3] = SDLK_s;
		axis->sym[4] = SDLK_q;
		axis->sym[5] = SDLK_e;
	}
	else if (config == "mouse")
	{
		axis->device = MOUSEAXIS;
		axis->device_number = 0;
	}
	else if (config == "touchscreen")
	{
		axis->device = MOUSEAXIS;
		axis->device_number = 0;
	}
	else if ( config.substr(0,11) == "controlaxis" )
	{
		uint16_t contaxis = (char)config[12] - '0';
		axis->device = CONTROLAXIS;
		axis->device_number = (char)config[11] - 'A';
		axis->sym[0] = contaxis;
		axis->sym[1] = contaxis;
		axis->sym[2] = (contaxis + 1);
		axis->sym[3] = (contaxis + 1);
		axis->sym[4] = 0;
		axis->sym[5] = 0;
	}
	else
		axis->device = NOINPUT;
}

#endif
