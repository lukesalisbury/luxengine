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
#include <3ds.h>

typedef enum
{
	PAD_A       = (0),
	PAD_B       = (1),
	PAD_SELECT  = (2),
	PAD_START   = (3),
	PAD_DRIGHT  = (4),
	PAD_DLEFT   = (5),
	PAD_DUP     = (6),
	PAD_DDOWN   = (7),
	PAD_R       = (8),
	PAD_L       = (9),
	PAD_X       = (10),
	PAD_Y       = (11),
	PAD_ZL      = (14), // (new 3DS only)
	PAD_ZR      = (15), // (new 3DS only)
	PAD_TOUCH   = (20), // Not actually provided by HID
	PAD_CSTICK_RIGHT = (24), // c-stick (new 3DS only)
	PAD_CSTICK_LEFT  = (25), // c-stick (new 3DS only)
	PAD_CSTICK_UP    = (26), // c-stick (new 3DS only)
	PAD_CSTICK_DOWN  = (27), // c-stick (new 3DS only)
	PAD_CPAD_RIGHT = (28), // circle pad
	PAD_CPAD_LEFT  = (29), // circle pad
	PAD_CPAD_UP    = (30), // circle pad
	PAD_CPAD_DOWN  = (31), // circle pad
} PAD_KEY_NUM;

void Player::ClearController()
{

}
bool Player::SetupController( std::string name )
{
	memset(this->_buttonConfig, 0, sizeof(Player_Button)*20 );
	memset(this->_controllerConfig, 0, sizeof(Player_Axis)*2 );
	memset(&this->_pointerConfig, 0, sizeof(Player_Pointer) );

	this->_buttonConfig[0].set(CONTROLBUTTON, 0, PAD_A ); // ❶
	this->_buttonConfig[1].set(CONTROLBUTTON, 0, PAD_B ); // ❷
	this->_buttonConfig[2].set(CONTROLBUTTON, 0, PAD_X ); // ❸
	this->_buttonConfig[3].set(CONTROLBUTTON, 0, PAD_Y ); // ❹
	this->_buttonConfig[4].set(CONTROLBUTTON, 0, PAD_L ); // ❺
	this->_buttonConfig[5].set(CONTROLBUTTON, 0, PAD_R ); // ❻
	this->_buttonConfig[6].set(CONTROLBUTTON, 0, PAD_START ); // ❼
	this->_buttonConfig[7].set(CONTROLBUTTON, 0, PAD_DRIGHT ); // ❽
	this->_buttonConfig[8].set(CONTROLBUTTON, 0, PAD_DLEFT ); // ❾
	this->_buttonConfig[9].set(CONTROLBUTTON, 0, PAD_DDOWN ); // ❿
	this->_buttonConfig[10].set(CONTROLBUTTON, 0, PAD_DUP ); // ➀

	this->_buttonConfig[11].set(CONTROLBUTTON, 0, PAD_L ); // ➁
	this->_buttonConfig[12].set(CONTROLBUTTON, 0, PAD_R ); // ➂
	this->_buttonConfig[13].set(CONTROLBUTTON, 0, PAD_ZL ); // ➃
	this->_buttonConfig[14].set(CONTROLBUTTON, 0, PAD_ZR ); // ➄

	this->_buttonConfig[15].set(CONTROLBUTTON, 0, PAD_SELECT ); //➅
	this->_buttonConfig[16].set(CONTROLBUTTON, 0, PAD_A ); // ICON_CONFIRM "➆"
	this->_buttonConfig[17].set(CONTROLBUTTON, 0, PAD_B ); // ICON_CANCEL "➇"
	this->_buttonConfig[18].set(CONTROLBUTTON, 0, PAD_TOUCH ); // ICON_POINTER_PRESS "➈"
	this->_buttonConfig[19].set(NOINPUT, 0, PAD_TOUCH ); // SHUTDOWN

	this->_controllerConfig[0].set(CONTROLAXIS, 0, PAD_CPAD_LEFT, PAD_CPAD_RIGHT, PAD_CPAD_UP, PAD_CPAD_DOWN, 0, 0 );
	this->_controllerConfig[1].set(CONTROLAXIS, 0, PAD_CSTICK_LEFT, PAD_CSTICK_RIGHT, PAD_CSTICK_UP, PAD_CSTICK_DOWN, 0, 0 );

	this->_pointerConfig.set(TOUCHSCREEN, 0, 0, 0 );


	return true;
}
