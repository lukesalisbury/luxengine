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


void parse_pointer_settings(Player_Pointer * pointer, std::string config)
{
	pointer->device = NOINPUT;
	if ( config.substr(0,11) == "controlaxis" )
	{
		uint16_t contaxis = ((char)config.at(12) - '0')*2;
		pointer->device = CONTROLAXIS;
		pointer->device_number = (char)config.at(11) - 'A';
		pointer->sym[0] = contaxis;
		pointer->sym[1] = contaxis;
		pointer->sym[2] = (contaxis + 1);
		pointer->sym[3] = (contaxis + 1);
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
		case 'b':
			button->device = CONTROLBUTTON;
			button->device_number = (button->sym / 100);
			button->sym -= (button->device_number * 100);
			button->sym = (1 << button->sym);
			button->device_number -= 1;
			break;
		case 't':
			button->device = CONTROLAXIS;
			button->device_number = (button->sym / 10);
			button->sym -= (button->device_number * 10);
			button->device_number -= 1;
			break;
		default:
			button->device = NOINPUT;
			break;
	}
}

void parse_axis_settings(Player_Axis * axis, std::string config)
{
	if ( config.substr(0,11) == "controlaxis" )
	{
		uint16_t contaxis = ((char)config[12] - '0')*2;
		axis->device = CONTROLAXIS;
		axis->device_number = (char)config[11] - 'A';
		axis->sym[0] = contaxis;
		axis->sym[1] = contaxis;
		axis->sym[2] = (contaxis + 1);
		axis->sym[3] = (contaxis + 1);
		axis->sym[4] = 4;
		axis->sym[5] = 5;
	}
	else
		axis->device = NOINPUT;
}
#endif
