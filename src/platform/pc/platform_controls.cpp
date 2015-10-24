/****************************
Copyright (c) 2006-2012 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include "elix/elix_string.hpp"
#include "player.h"
#include "game_config.h"
#include "core.h"
#include <SDL/SDL_keysym.h>

void Player::ClearController()
{

}

bool Player::SetupController( std::string name )
{
	memset(this->_buttonConfig, 0, sizeof(Player_Button)*20 );
	memset(this->_controllerConfig, 0, sizeof(Player_Axis)*2 );
	memset(&this->_pointerConfig, 0, sizeof(Player_Pointer) );


	std::vector<std::string> values;
	std::string settings;

	settings = lux::config->GetString("controller." + name);
	elix::string::Split(settings, "|", &values);

	if ( values.size() == 23 )
	{
		ParseAxis(&this->_controllerConfig[0], values[0]);
		ParseAxis(&this->_controllerConfig[1], values[1]);
		ParsePointer(&this->_pointerConfig, values[2]);
		ParseButton(&this->_buttonConfig[0], values[3]);
		ParseButton(&this->_buttonConfig[1], values[4]);
		ParseButton(&this->_buttonConfig[2], values[5]);
		ParseButton(&this->_buttonConfig[3], values[6]);
		ParseButton(&this->_buttonConfig[4], values[7]);
		ParseButton(&this->_buttonConfig[5], values[8]);
		ParseButton(&this->_buttonConfig[6], values[9]);
		ParseButton(&this->_buttonConfig[7], values[10]);
		ParseButton(&this->_buttonConfig[8], values[11]);
		ParseButton(&this->_buttonConfig[9], values[12]);
		ParseButton(&this->_buttonConfig[10], values[13]);
		ParseButton(&this->_buttonConfig[11], values[14]);
		ParseButton(&this->_buttonConfig[12], values[15]);
		ParseButton(&this->_buttonConfig[13], values[16]);
		ParseButton(&this->_buttonConfig[14], values[17]);
		ParseButton(&this->_buttonConfig[15], values[18]);
		ParseButton(&this->_buttonConfig[PLAYER_CONFIRM], values[19]);
		ParseButton(&this->_buttonConfig[PLAYER_CANCEL], values[20]);
		ParseButton(&this->_buttonConfig[PLAYER_POINTER], values[21]);
		ParseButton(&this->_buttonConfig[19], values[22]);
	}
	else
	{
		lux::core->SystemMessage(SYSTEM_MESSAGE_LOG) << "'" << name << "'' Invalid Control String '" << settings << "'"<< std::endl;
	}
	values.clear();



	return true;
}


void Player::ParsePointer(Player_Pointer * pointer, std::string config)
{
	if (config == "mouse")
	{
		pointer->device = MOUSEAXIS;
	}
	else if ( config.substr(0,11) == "controlaxis" )
	{

		uint8_t joystick_mode = 2;
		uint16_t contaxis = 1;
		pointer->device = CONTROLAXIS;
		pointer->device_number = (char)config[11] - 'A';

		joystick_mode = lux::core->JoystickMode( pointer->device_number );
		contaxis = ( ((uint8_t)config[12] - '0') * joystick_mode ) + 1;

		if ( joystick_mode == 3 && contaxis == 3 )
		{
			pointer->sym[0] = - contaxis; // X axis
			pointer->sym[1] = + contaxis; // X axis
			pointer->sym[2] = - (contaxis + 1);  // y axis
			pointer->sym[3] = + (contaxis + 1); // y axis
		}
		else
		{
			pointer->sym[0] = - contaxis; // X axis
			pointer->sym[1] = + contaxis; // X axis
			pointer->sym[2] = - (contaxis + 1);  // y axis
			pointer->sym[3] = + (contaxis + 1); // y axis
		}

		pointer->sym[4] = 0; // z axis
		pointer->sym[5] = 0; // z axis

	}
	else
	{
		pointer->device = NOINPUT;
	}

}

void Player::ParseButton(Player_Button * button, std::string config)
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

void Player::ParseAxis(Player_Axis * axis, std::string config)
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
		axis->sym[0] = SDLK_KP4;
		axis->sym[1] = SDLK_KP6;
		axis->sym[2] = SDLK_KP8;
		axis->sym[3] = SDLK_KP2;
		axis->sym[4] = SDLK_KP_MINUS;
		axis->sym[5] = SDLK_KP_PLUS;
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
		uint8_t joystick_mode = 2;
		uint16_t contaxis = 1;
		axis->device = CONTROLAXIS;
		axis->device_number = (char)config[11] - 'A';

		joystick_mode = lux::core->JoystickMode( axis->device_number );
		contaxis = ( ((uint8_t)config[12] - '0') * joystick_mode ) + 1;

		axis->sym[0] = - contaxis; // X axis
		axis->sym[1] = + contaxis; // X axis
		axis->sym[2] = - (contaxis + 1);  // y axis
		axis->sym[3] = + (contaxis + 1); // y axis

		if ( joystick_mode == 3 &&  contaxis == 1 )
		{
			axis->sym[4] = - (contaxis + 2); // z axis
			axis->sym[5] = + (contaxis + 2); // z axis
		}
		else
		{
			axis->sym[4] = 0; // z axis
			axis->sym[5] = 0; // z axis
		}
	}
	else
		axis->device = NOINPUT;
}
