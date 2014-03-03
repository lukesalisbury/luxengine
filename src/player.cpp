/****************************
Copyright © 2006-2011 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "engine.h"
#include "elix_string.hpp"
#include "core.h"
#include "game_config.h"

#include "world.h"
#include "display_functions.h"
#include "platform_media.h"

extern ObjectEffect default_fx;

/*
- Controller Layout
axis 0: X/Y/Z -255 to 255.
axis 1: A/B/C -255 to 255.
pointer: 2d location.
button 00: 0 - unpressed, 1 - Just pressed, 2 - holding (action)
button 01: 0 - unpressed, 1 - Just pressed, 2 - holding (action)
button 02: 0 - unpressed, 1 - Just pressed, 2 - holding (action)
button 03: 0 - unpressed, 1 - Just pressed, 2 - holding (action)
button 04: 0 - unpressed, 1 - Just pressed, 2 - holding (action)
button 05: 0 - unpressed, 1 - Just pressed, 2 - holding (action)
button 06: 0 - unpressed, 1 - Just pressed, 2 - holding (menu)
button 07: 0 - unpressed, 1 - Just pressed, 2 - holding (right)
button 08: 0 - unpressed, 1 - Just pressed, 2 - holding (left)
button 09: 0 - unpressed, 1 - Just pressed, 2 - holding (down)
button 10: 0 - unpressed, 1 - Just pressed, 2 - holding (up)
button 11: 0 - unpressed, 1 - Just pressed, 2 - holding (undefined)
button 12: 0 - unpressed, 1 - Just pressed, 2 - holding (undefined)
button 13: 0 - unpressed, 1 - Just pressed, 2 - holding (undefined)
button 14: 0 - unpressed, 1 - Just pressed, 2 - holding (undefined)
button 15: 0 - unpressed, 1 - Just pressed, 2 - holding (undefined)
internal button 16: (confirm) 0 - unpressed, 1 - Just pressed, 2 - holding
internal button 17: (cancel) 0 - unpressed, 1 - Just pressed, 2 - holding
internal button 18: (pointer press) 0 - unpressed, 1 - Just pressed, 2 - holding
internal button 19: (shutdown) 0 - unpressed, 1 - Just pressed, 2 - holding
*/

inline int KeyboardAxis(int key1, int key2)
{
	if ( key1 && !key2 )
		return -255;
	else if ( !key1 && key2 )
		return 255;
	return 0;
}

Player::Player(uint32_t id, uint8_t control)
{
	this->_id = id;
	this->SetControls((uint8_t)id);
	this->_control = control;
	this->timer = 0;
	this->_entity = NULL;
	this->_pointer[0] = this->_pointer[1] = this->_pointer[2] = 10;



	this->PlayerColour = default_fx;
	this->PlayerColour.primary_colour.r = this->PlayerColour.primary_colour.b = this->PlayerColour.primary_colour.g = this->PlayerColour.primary_colour.a = 200;

}

Player::~Player()
{
	this->ClearController();
	if ( this->_entity )
	{
		this->_entity->Delete();
		this->_entity = NULL;
	}
}

void Player::SetControls( std::string controller_name )
{
	this->control_name = controller_name;
	this->SetupController( controller_name );
}

void Player::SetControls(uint8_t preset)
{
	std::string control_value;
	std::string control_name = "player.controller";
	control_name.append( 1, (char)(preset + '0'));

	control_value = lux::config->GetString(control_name);

	this->SetControls( control_value );

}

/* Cache Input */
void Player::CachePointerValues()
{
	switch (this->_pointerConfig.device)
	{
		case CONTROLAXIS:
		{
			this->_pointer[0] += lux::core->GetInput(CONTROLAXIS, this->_pointerConfig.device_number,this->_pointerConfig.sym[0])/64;
			this->_pointer[1] += lux::core->GetInput(CONTROLAXIS, this->_pointerConfig.device_number,this->_pointerConfig.sym[2])/64;
			this->_pointer[2] = 1;
			break;
		}
		case MOUSEAXIS:
		{
			this->_pointer[0] = lux::core->GetInput(MOUSEAXIS, 0,0);
			this->_pointer[1] = lux::core->GetInput(MOUSEAXIS, 0,1);
			this->_pointer[2] = 1;
			break;
		}
		default:
		{
			this->_pointer[0] = this->_pointer[1] = this->_pointer[2] = 0;
			break;
		}
	}
}

void Player::CacheAxisValues( uint8_t n )
{
	switch (this->_controllerConfig[n].device)
	{
		case KEYBOARD:
		{
			int key1, key2;
			/* X axis */
			key1 = lux::core->GetInput(KEYBOARD, 0,this->_controllerConfig[n].sym[0]);
			key2 = lux::core->GetInput(KEYBOARD, 0,this->_controllerConfig[n].sym[1]);
			this->_controller[(n*3)+0] = KeyboardAxis(key1, key2);
			/* y axis */
			key1 = lux::core->GetInput(KEYBOARD, 0,this->_controllerConfig[n].sym[2]);
			key2 = lux::core->GetInput(KEYBOARD, 0,this->_controllerConfig[n].sym[3]);
			this->_controller[(n*3)+1] = KeyboardAxis(key1, key2);
			/* z axis */
			key1 = lux::core->GetInput(KEYBOARD, 0,this->_controllerConfig[n].sym[4]);
			key2 = lux::core->GetInput(KEYBOARD, 0,this->_controllerConfig[n].sym[5]);
			this->_controller[(n*3)+2] = KeyboardAxis(key1, key2);
			break;
		}
		case CONTROLAXIS:
		{
			this->_controller[(n*3)+0] = lux::core->GetInput(CONTROLAXIS, this->_controllerConfig[n].device_number,this->_controllerConfig[n].sym[0]);
			this->_controller[(n*3)+1] = lux::core->GetInput(CONTROLAXIS, this->_controllerConfig[n].device_number,this->_controllerConfig[n].sym[2]);
			this->_controller[(n*3)+2] = lux::core->GetInput(CONTROLAXIS, this->_controllerConfig[n].device_number,this->_controllerConfig[n].sym[4]);
			break;
		}
		default:
			break;
	}

}

void Player::CacheButtonValues(uint8_t n)
{
	int key = lux::core->GetInput( this->_buttonConfig[n].device, this->_buttonConfig[n].device_number, this->_buttonConfig[n].sym );
	if ( key )
		this->_button[n] = (this->_button[n] ? 2 : 1);
	else
		this->_button[n] = 0;
}

void Player::Loop()
{
	if (_control == LOCAL)
	{
		/* Pointer */
		this->CachePointerValues();

		/* Button/Axis */
		for (uint8_t n = 0; n < 19; n++)
		{
			if (n < 2)
			{
				this->CacheAxisValues(n);
			}
			this->CacheButtonValues(n);

		}

		#ifdef NETWORKENABLED
		if ( lux::core->GetTime() > (this->timer + 333) )
		{
			this->timer = lux::core->GetTime();
			if ( this->_entity )
			{
				this->_entity->displaymap = lux::world->active_map->Ident();
				if ( lux::core->CreateMessage((uint8_t)4, true) )
				{
					lux::core->MessageAppend(this->_entity->x);
					lux::core->MessageAppend(this->_entity->y);
					lux::core->MessageAppend(this->_entity->displaymap);
					lux::core->MessageSend();
				}
			}
		}
		#endif
	}
	else if ( this->_control == REMOTE )
	{

	}

}

LuxSprite * Player::GetInputSprite( int8_t axis, int8_t key, int8_t pointer )
{
	LuxSprite * sprite = NULL;
	if ( lux::media )
	{
		if ( axis >= 0 && axis < 12 )
		{
			uint8_t a = axis/6; // Axis (Either 1 or 2)
			uint8_t s = axis%6; // Axis Direction

			sprite = lux::media->GetInputImage(this->_controllerConfig[a].device, this->_controllerConfig[a].device_number, this->_controllerConfig[a].sym[s]);
		}
		else if ( key >= 0 && key < 19 )
		{
			sprite = lux::media->GetInputImage(this->_buttonConfig[key].device, this->_buttonConfig[key].device_number, this->_buttonConfig[key].sym);
		}
		else if ( pointer >= 0 )
		{
			sprite = lux::media->GetInputImage(this->_pointerConfig.device, this->_pointerConfig.device_number, this->_pointerConfig.sym[0]);
		}

	}
	return sprite;
}

int16_t Player::GetControllerAxis(uint8_t axis)
{
	if (axis < 6)
	{
		return (int16_t)this->_controller[axis];
	}
	return -1;
}

int16_t Player::GetButton(uint8_t key)
{
	if (key < 19)
	{
		return (int16_t)this->_button[key];
	}
	return -1;
}

int16_t Player::GetPointer(uint8_t axis)
{
	if (axis < 2)
	{
		return (int16_t)this->_pointer[axis];
	}
	return -1;
}

void Player::SetControllerAxis(uint8_t axis, int16_t value)
{
	if (axis < 4)
	{
		this->_pointer[axis] = value;
	}
}

void Player::SetButton(uint8_t key, int16_t value)
{
	if (key < 16)
	{
		this->_pointer[key] = value;
	}
}

void Player::SetPointer(uint8_t axis, int16_t value)
{
	if ( axis < 2 )
	{
		this->_pointer[axis] = value;
	}
}

MapObject *Player::GetPointerObject()
{
	return NULL;
}

/* Entity */
Entity * Player::GetEntity()
{
	return this->_entity;
}

void Player::SetEntity(Entity * entity)
{
	this->_entity = entity;
}

void Player::SetEntityPostion( fixed x, fixed y, fixed z, uint32_t mapid )
{
	if ( this->_entity )
	{
		#ifdef NETWORKENABLED
		lux::core->NetLock();
		#endif
		this->_entity->x = x;
		this->_entity->y = y;
		this->_entity->z = z;
		this->_entity->displaymap = mapid;
		#ifdef NETWORKENABLED
		lux::core->NetUnlock();
		#endif
	}
}


void Player::Message( int32_t * data, uint32_t size )
{
	if ( this->_entity )
	{
		#ifdef NETWORKENABLED
		lux::core->NetLock();
		#endif
		this->_entity->callbacks->Push( this->_entity->_data, size );
		this->_entity->callbacks->PushArray( this->_entity->_data, data, size, NULL );
		this->_entity->callbacks->Push( this->_entity->_data, 0 );
		this->_entity->Call("NetMessage", (char*)"");
		#ifdef NETWORKENABLED
		lux::core->NetUnlock();
		#endif
	}
}


void Player::SetName(std::string name)
{
	this->_name = name;
}

std::string Player::GetName( )
{
	return this->_name;
}
