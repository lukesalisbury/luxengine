/****************************
Copyright (c) 2006-2012 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "elix_string.hpp"
#include "player.h"
#include "game_config.h"
#include "core.h"
#include <SDL.h>
#include <SDL_keycode.h>
/*
typedef struct Player_Button {
		InputDevice device;
		uint32_t device_number;
		int32_t sym;
} Player_Button;

typedef struct Player_Axis {
		InputDevice device;
		uint32_t device_number;
		int32_t sym[6];
} Player_Axis;

typedef struct Player_Pointer {
		InputDevice device;
		uint32_t device_number;
		int32_t sym[4];
} Player_Pointer;
*/
bool Player::SetupController( std::string name )
{
	memset(this->_buttonConfig, 0, sizeof(Player_Button)*20 );
	memset(this->_controllerConfig, 0, sizeof(Player_Axis)*2 );
	memset(&this->_pointerConfig, 0, sizeof(Player_Pointer) );

	if ( name.compare(0, 7, "gamepad") == 0 )
	{
		//Using SDL Game Controller values
		uint32_t device_number = name.at(7)-'0';

		this->_buttonConfig[0].set(CONTROLBUTTON, device_number, SDL_CONTROLLER_BUTTON_A );
		this->_buttonConfig[1].set(CONTROLBUTTON, device_number, SDL_CONTROLLER_BUTTON_B );
		this->_buttonConfig[2].set(CONTROLBUTTON, device_number, SDL_CONTROLLER_BUTTON_X );
		this->_buttonConfig[3].set(CONTROLBUTTON, device_number, SDL_CONTROLLER_BUTTON_Y );
		this->_buttonConfig[4].set(CONTROLBUTTON, device_number, SDL_CONTROLLER_BUTTON_LEFTSHOULDER );
		this->_buttonConfig[5].set(CONTROLBUTTON, device_number, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER );
		this->_buttonConfig[6].set(CONTROLBUTTON, device_number, SDL_CONTROLLER_BUTTON_START );
		this->_buttonConfig[7].set(CONTROLBUTTON, device_number, SDL_CONTROLLER_BUTTON_DPAD_RIGHT );
		this->_buttonConfig[8].set(CONTROLBUTTON, device_number, SDL_CONTROLLER_BUTTON_DPAD_LEFT );
		this->_buttonConfig[9].set(CONTROLBUTTON, device_number, SDL_CONTROLLER_BUTTON_DPAD_DOWN );
		this->_buttonConfig[10].set(CONTROLBUTTON, device_number, SDL_CONTROLLER_BUTTON_DPAD_UP );

		this->_buttonConfig[11].set(CONTROLBUTTON, device_number, SDL_CONTROLLER_BUTTON_BACK );
		this->_buttonConfig[12].set(CONTROLBUTTON, device_number, SDL_CONTROLLER_BUTTON_LEFTSTICK );
		this->_buttonConfig[13].set(CONTROLBUTTON, device_number, SDL_CONTROLLER_BUTTON_RIGHTSTICK );
		this->_buttonConfig[14].set(NOINPUT, device_number, SDL_CONTROLLER_BUTTON_A );

		this->_buttonConfig[15].set(NOINPUT, device_number, SDL_CONTROLLER_BUTTON_A );
		this->_buttonConfig[16].set(NOINPUT, device_number, SDL_CONTROLLER_BUTTON_A );
		this->_buttonConfig[17].set(CONTROLBUTTON, device_number, SDL_CONTROLLER_BUTTON_B );
		this->_buttonConfig[18].set(CONTROLBUTTON, device_number, SDL_CONTROLLER_BUTTON_GUIDE );
		this->_buttonConfig[19].set(CONTROLBUTTON, device_number, SDL_CONTROLLER_BUTTON_GUIDE );

		this->_controllerConfig[0].set(CONTROLAXIS, device_number, SDL_CONTROLLER_AXIS_LEFTX, SDL_CONTROLLER_AXIS_LEFTX, SDL_CONTROLLER_AXIS_LEFTY, SDL_CONTROLLER_AXIS_LEFTY, SDL_CONTROLLER_AXIS_INVALID, SDL_CONTROLLER_AXIS_TRIGGERLEFT );
		this->_controllerConfig[1].set(CONTROLAXIS, device_number, SDL_CONTROLLER_AXIS_RIGHTX, SDL_CONTROLLER_AXIS_RIGHTX, SDL_CONTROLLER_AXIS_RIGHTY, SDL_CONTROLLER_AXIS_RIGHTY, SDL_CONTROLLER_AXIS_INVALID, SDL_CONTROLLER_AXIS_TRIGGERRIGHT );

		this->_pointerConfig.set(CONTROLAXIS, device_number, SDL_CONTROLLER_AXIS_RIGHTX, SDL_CONTROLLER_AXIS_RIGHTY );

	}
	else
	{
		std::vector<std::string> values;
		std::string settings;

		settings = lux::config->GetString("control." + name);
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
			lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << "'" << name << "'' Invalid Control String '" << settings << "'"<< std::endl;
		}
		values.clear();


	}
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
		uint16_t contaxis = ((char)config.at(12) - '0')*2;
		pointer->device = CONTROLAXIS;
		pointer->device_number = (char)config.at(11) - 'A';
		pointer->sym[0] = contaxis;
		pointer->sym[1] = contaxis;
		pointer->sym[2] = (contaxis + 1);
		pointer->sym[3] = (contaxis + 1);
	}
	else
	{
		pointer->device = NOINPUT;
	}

}

void Player::ParseButton(Player_Button * button, std::string config)
{
	if ( config.length() < 3)
	{
		lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR) << "Not a valid button '" << config << "'" << std::endl;
		return;
	}
	uint16_t device_type = config.at(0);
	uint16_t device_numb = config.at(1)-'0';
	std::string dev_butt = config.substr(2);

	button->device_number = device_numb;
	button->sym = elix::string::ToInt32(dev_butt);

	switch (device_type)
	{
		case 'k':
			/* Todo: Handle more then one */
			button->device = KEYBOARD;
			//lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << "Player Button: [KEYBOARD] #" << button->device_number << " - " << SDL_GetScancodeName((SDL_Scancode)button->sym) << std::endl;
			break;
		case 'm':
			/* Todo: Handle more then one */
			button->device = MOUSEBUTTON;
			//lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << "Player Button: [MOUSEBUTTON] #" << button->device_number << " - " << button->sym << std::endl;
			break;
		case 'b':
			button->device = CONTROLBUTTON;
			//lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << "Player Button: [CONTROLBUTTON] #" << button->device_number << " - " << button->sym << std::endl;
			break;
		case 'h':
			button->device = CONTROLHAT;
			//lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << "Player Button: [CONTROLHAT] #" << button->device_number << " - " << button->sym << std::endl;
			break;
		default:
			button->device = NOINPUT;
			//lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << "Player Button: [NOINPUT] " << std::endl;
			break;
	}



}

void Player::ParseAxis(Player_Axis * axis, std::string config)
{
	if (config == "arrows")
	{
		axis->device = KEYBOARD;
		axis->sym[0] = SDL_SCANCODE_LEFT;
		axis->sym[1] = SDL_SCANCODE_RIGHT;
		axis->sym[2] = SDL_SCANCODE_UP;
		axis->sym[3] = SDL_SCANCODE_DOWN;
		axis->sym[4] = SDL_SCANCODE_PAGEUP;
		axis->sym[5] = SDL_SCANCODE_PAGEDOWN;
	}
	else if (config == "numpad")
	{
		axis->device = KEYBOARD;
		axis->sym[0] = SDL_SCANCODE_KP_4;
		axis->sym[1] = SDL_SCANCODE_KP_6;
		axis->sym[2] = SDL_SCANCODE_KP_8;
		axis->sym[3] = SDL_SCANCODE_KP_2;
		axis->sym[4] = SDL_SCANCODE_KP_MINUS;
		axis->sym[5] = SDL_SCANCODE_KP_PLUS;
	}
	else if (config == "keyboard-wasd")
	{
		axis->device = KEYBOARD;
		axis->sym[0] = SDL_SCANCODE_A;
		axis->sym[1] = SDL_SCANCODE_D;
		axis->sym[2] = SDL_SCANCODE_W;
		axis->sym[3] = SDL_SCANCODE_S;
		axis->sym[4] = SDL_SCANCODE_Q;
		axis->sym[5] = SDL_SCANCODE_E;
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
