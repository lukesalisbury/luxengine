/****************************
Copyright © 2013 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include "platform_media.h"
#include "sprite_sheet.h"
#include "elix_string.hpp"
#include "pc/keyboard.h"
#include "pc/joystick_generic.h"
#include "pc/joystick_xbox360.h"
#include "pc/mouse_generic.h"

#include "core.h"

PlatformMedia::PlatformMedia()
{

}

PlatformMedia::~PlatformMedia()
{

}

void PlatformMedia::Init(GraphicSystem graphic)
{
	/* Load Keyboard Sheet */
	elix::Image * image_keyboard = new elix::Image(keyboard_image_data, keyboard_image_size);
	this->keyboard_sheet = new LuxSheet(graphic);
	this->keyboard_sheet->SetName("Input Icons: Keyboard");
	for ( int32_t c = 0; c < keyboard_sheet_size; c++ )
	{
		this->keyboard_sheet->ParseSimpleText( keyboard_sheet_data[c] );
	}
	this->keyboard_sheet->LoadFromImage( image_keyboard );
	delete image_keyboard;


	/* Load Generic JoyStick */
	elix::Image * image_generic_joystick = new elix::Image(joystick_generic_image_data, joystick_generic_image_size);
	this->joystick_generic_sheet = new LuxSheet(graphic);
	this->joystick_generic_sheet->SetName("Input Icons: Generic Gamepad");
	for ( int32_t c = 0; c < joystick_generic_sheet_size; c++ )
	{
		this->joystick_generic_sheet->ParseSimpleText( joystick_generic_sheet_data[c] );
	}
	this->joystick_generic_sheet->LoadFromImage( image_generic_joystick );
	delete image_generic_joystick;

	/* Load XBOX 360 JoyStick */
	elix::Image * image_joystick_xbox360 = new elix::Image(joystick_xbox360_image_data, joystick_xbox360_image_size);
	this->joystick_xbox360_sheet = new LuxSheet(graphic);
	this->joystick_xbox360_sheet->SetName("Input Icons: Xbox 360 Gamepad");
	for ( int32_t c = 0; c < joystick_generic_sheet_size; c++ )
	{
		this->joystick_xbox360_sheet->ParseSimpleText( joystick_xbox360_sheet_data[c] );
	}
	this->joystick_xbox360_sheet->LoadFromImage( image_joystick_xbox360 );
	delete image_joystick_xbox360;

	/* Load Generic Mouse */
	elix::Image * image_mouse_generic = new elix::Image(mouse_generic_image_data, mouse_generic_image_size);
	this->mouse_sheet = new LuxSheet(graphic);
	this->mouse_sheet->SetName("Input Icons: Mouse");
	for ( int32_t c = 0; c < mouse_generic_sheet_size; c++ )
	{
		this->mouse_sheet->ParseSimpleText( mouse_generic_sheet_data[c] );
	}
	this->mouse_sheet->LoadFromImage( image_mouse_generic );
	delete image_mouse_generic;

}

void PlatformMedia::Free()
{
	delete this->joystick_xbox360_sheet;
	delete this->joystick_generic_sheet;
	delete this->keyboard_sheet;
	delete this->mouse_sheet;
}


LuxSprite * PlatformMedia::GetInputImage( InputDevice device, uint32_t device_number, int32_t symbol )
{
	if (device == NOINPUT )
	{
		return NULL;
	}

	switch (device)
	{
		case KEYBOARD:
		{
			if ( this->keyboard_sheet )
			{
				std::string key_name = elix::string::FromInt(symbol);
				return this->keyboard_sheet->GetSprite(key_name);
			}
			return NULL;
		}
		case MOUSEAXIS:
		{
			if ( this->mouse_sheet )
			{
				std::string key_name = elix::string::FromInt(symbol);
				return this->mouse_sheet->GetSprite(key_name);
			}
			return NULL;
		}

		case MOUSEBUTTON:
		{
			if ( this->mouse_sheet )
			{
				std::string key_name = "button";
				key_name.append(elix::string::FromInt(symbol));
				return this->mouse_sheet->GetSprite(key_name);
			}
			return NULL;
		}
		case CONTROLAXIS:
		{
			std::string key_name = "axis";
			key_name.append("A");
			if ( symbol > 0 )
			{
				key_name.append("+");
			}

			key_name.append(elix::string::FromInt(symbol));

			if ( lux::core->JoystickMode( device_number ) == 3 )
			{
				if ( this->joystick_xbox360_sheet )
				{
					return this->joystick_xbox360_sheet->GetSprite(key_name);
				}
			}
			else
			{
				if ( this->joystick_generic_sheet )
				{
					return this->joystick_generic_sheet->GetSprite(key_name);
				}
			}
			return NULL;
		}
		case CONTROLBUTTON:
		{
			std::string key_name = "button";
			key_name.append(elix::string::FromInt(symbol));
			if ( lux::core->JoystickMode( device_number ) == 3 )
			{
				if ( this->joystick_xbox360_sheet )
				{
					return this->joystick_xbox360_sheet->GetSprite(key_name);
				}
			}
			else
			{
				if ( this->joystick_generic_sheet )
				{
					return this->joystick_generic_sheet->GetSprite(key_name);
				}
			}
			return NULL;
		}
		case NOINPUT:
		default:
			return NULL;
	}
}
