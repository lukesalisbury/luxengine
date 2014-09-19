/****************************
Copyright © 2006-2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "engine.h"
#include "display.h"
#include "gui.h"
#include "core.h"

namespace lux {
	namespace screen {
		void display( std::string message );
	}
}

void Lux_FatalError( std::string reason )
{
	lux::engine->FatalError( reason );
}

void Lux_SetState( LuxState new_state )
{
	lux::engine->SetState( new_state );
}

LuxState Lux_GetState( )
{
	if ( lux::engine )
		return lux::engine->state;
	else
		return GAMEERROR;
}

bool LuxEngine::LoopDialog()
{
	lux::display->DrawGameStatic();
	this->msgdialog = lux::gui->ReturnResult();
	return ( this->msgdialog == 1 ? true : false);
}

bool LuxEngine::EndDialog( LuxWidget dialog )
{
	if ( dialog == DIALOGTEXT && this->msgdialog_return )
	{
		this->msgdialog_return->assign( lux::gui->main_widget->GetText() );
	}

	NULLIFY( lux::gui );

	lux::core->Idle();


	return ( this->msgdialog == 1 ? true : false);
}

void SetRectFromText( LuxRect & area, std::string & text, uint8_t text_width, uint8_t text_height, uint16_t wrap_length )
{
	uint16_t max_length = 1;
	uint16_t length_count = 0;
	uint16_t lines = 1;
	std::string::iterator object;

	for ( object = text.begin(); object != text.end(); object++ )
	{
		uint8_t utfchar = *object;
		uint32_t cchar = utfchar;
		if ( cchar == '\n' || cchar == '\r' )
		{
			lines++;
			max_length = std::max(length_count, max_length);
			length_count = 0;
		}
		else if ( cchar <= 128 )
		{
			length_count++;
		}
		else if ( cchar < 224 )
		{
			object++;

			length_count++;
		}
		else if ( cchar < 240 )
		{
			object++;
			object++;

			length_count++;
		}
		else if ( cchar < 245 )
		{
			object++;
			object++;
			object++;

			length_count++;
		}

		if (wrap_length == length_count + 2)
		{
			text.insert(object, '\n');
		}

	}
	max_length = std::max(length_count, max_length);

	area.w = text_width * max_length;
	area.h = text_height * lines;

}


bool LuxEngine::CreateDialog(std::string text, LuxWidget dialog, std::string * answer )
{
	if ( lux::display == NULL )
	{
		lux::core->SystemMessage(__FILE__, __LINE__, SYSTEM_MESSAGE_INFO) << "No Display. Message: " << text << std::endl;
		return false;
	}

	uint16_t text_length = text.length() * 8;
	uint16_t text_area_width;
	LuxRect region = { 20, 29, 64, 50, 0 };


	SetRectFromText( region, text, 8, 8, (lux::display->screen_dimension.w/8) - 8  );

	region.h += 16; //padding
	region.w += 16; //padding
	region.x = (lux::display->screen_dimension.w - region.w) / 2;
	region.y = (lux::display->screen_dimension.h / 2) - (region.h/2);


	if ( dialog == DIALOGTEXT )
	{
		region.y -= 20;
		region.w += 20;

	}
	else
	{
		region.h += 20;
	}

	lux::gui = new UserInterface(lux::display);
	lux::gui->SetRegion( region );
	lux::gui->AddChild(region, dialog, (LuxColour){150, 150, 200, 200}, text);
	if ( dialog == DIALOGTEXT && answer)
	{
		lux::gui->main_widget->SetText( *answer );
		this->msgdialog_return = answer;
	}
	else
	{
		this->msgdialog_return = NULL;
	}
	this->msgdialog = 0;

	return true;
}

bool LuxEngine::ShowDialog(std::string text, LuxWidget dialog, std::string * answer )
{
	if ( !this->CreateDialog(text, dialog, answer) )
	{
		return true;
	}
	while ( !this->msgdialog )
	{
		this->LoopDialog();
	}
	return this->EndDialog( dialog );
}


void LuxEngine::FatalError(std::string reason)
{
	this->state = GAMEERROR;
	if ( lux::core == NULL )
	{
		std::cout << reason << std::endl;
		std::cerr << reason << std::endl;
	}
	else
	{
		lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR, reason);

		if ( lux::display == NULL )
		{
			lux::display = new DisplaySystem( "System Error", 640, 480, 16, false );
		}
		this->ShowDialog(reason, DIALOGOK);
	}

}


