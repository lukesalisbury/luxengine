/****************************
Copyright © 2013-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include "platform_media.h"
#include "sprite_sheet.h"
#include "elix/elix_string.hpp"
#include "pc/keyboard.h"
#include "pc/joystick_generic.h"
#include "dc/joystick_dreamcast.h"
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

	/* Load Generic JoyStick */
	elix::Image * image_generic_joystick = new elix::Image(joystick_dreamcast_image_data, joystick_dreamcast_image_size);
	this->joystick_dreamcast_sheet = new LuxSheet(graphic);
	this->joystick_dreamcast_sheet->SetName("Input Icons: Gamepad");
	for ( int32_t c = 0; c < joystick_generic_sheet_size; c++ )
	{
		this->joystick_dreamcast_sheet->ParseSimpleText( joystick_dreamcast_sheet_data[c] );
	}
	this->joystick_dreamcast_sheet->LoadFromImage( image_generic_joystick );
	delete image_generic_joystick;
}

void PlatformMedia::Free()
{
	delete this->joystick_dreamcast_sheet;
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
			return NULL;
		}
		case MOUSEAXIS:
		{
			return NULL;
		}
		case MOUSEBUTTON:
		{
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
			lux::core->SystemMessage(SYSTEM_MESSAGE_LOG) << key_name <<std::endl;
			if ( this->joystick_dreamcast_sheet )
			{
				return this->joystick_dreamcast_sheet->GetSprite(key_name);
			}
			return NULL;
		}
		case CONTROLBUTTON:
		{
			std::string key_name = "button";
			key_name.append(elix::string::FromInt(symbol));
			lux::core->SystemMessage(SYSTEM_MESSAGE_LOG) << key_name <<std::endl;
			if ( this->joystick_dreamcast_sheet )
			{
				return this->joystick_dreamcast_sheet->GetSprite(key_name);

			}
			return NULL;
		}
		case NOINPUT:
		default:
			return NULL;
	}
}
