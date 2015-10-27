/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "engine.h"
#include "core.h"
#include "gui/gui.h"
#include "config.h"
#include "display/display.h"


namespace lux {
	namespace screen {
		int8_t count = 0;

		ObjectEffect background;//  = { {119,142,179,255}, {76,106,155,255}, 0, 1000, 1000, 0, 0, STYLE_VGRADIENT};
		ObjectEffect colour1;//  = { {122,122,122,255}, {200,200,200,255}, 0, 1000, 1000, 0, 0};
		ObjectEffect colour2;//  = { {166,166,166,255}, {76,106,155,255}, 0, 1000, 1000, 0, 0};
		ObjectEffect colour3;//  = { {210,210,210,255}, {200,200,200,255}, 0, 1000, 1000, 0, 0};
		ObjectEffect text;//  = { {64,64,64,255}, {99,99,99,255}, 0, 1000, 1000, 0, 0};

		LuxRect rect2 = { 0, 0, 4, 4, 0};
		LuxRect rect1 = { 0, 0, 0, 0, 0};

		std::string last_message = "";
		uint32_t time;

		void show( )
		{
			uint16_t width = lux::display->screen_dimension.w / 2;
			uint16_t height = (lux::display->screen_dimension.h / 2)+70;

			lux::display->graphics.PreShow(0);
			lux::display->graphics.DrawRect( lux::display->screen_dimension, background );

			int8_t i = 0;
			while( i < 6 )
			{
				rect2.y = height;
				rect2.x = width + (i * 12);

				if ( i == lux::screen::count )
					lux::display->graphics.DrawCircle( rect2, colour1 );
				else if ( i == (lux::screen::count-1) || i == (lux::screen::count+1) )
					lux::display->graphics.DrawCircle( rect2, colour2 );
				else
					lux::display->graphics.DrawCircle( rect2, colour3 );
				i++;
			}
			rect1.y =  height + 14;
			rect1.x = width - (last_message.length() * 4);

			lux::display->graphics.DrawText( last_message, rect1, text, false );
			lux::display->graphics.Show(0);

			lux::screen::count %= 5;
		}

		void display( std::string message )
		{
			time = lux::core->GetTime();
			last_message = message;
			lux::screen::show( );
			lux::screen::count++;
		}

		void push( )
		{
			uint32_t current_time = lux::core->GetTime();
			if ( current_time >  time + 30 )
			{
				lux::screen::show( );
				lux::screen::count++;
			}
		}
	}
}
