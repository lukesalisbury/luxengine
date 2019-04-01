/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "engine.h"
#include "display/display.h"
#include "gui.h"
#include "core.h"
#include "misc_functions.h"

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
	lux::display->Display(PAUSED);
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



bool LuxEngine::CreateDialog(std::string text, LuxWidget dialog, std::string * answer )
{
	if ( lux::display == nullptr )
	{
		lux::core->SystemMessage(SYSTEM_MESSAGE_LOG) << "No Display Available. Message: " << text << std::endl;
		return false;
	}

	LuxRect region = { 20, 29, 64, 50, 0 };

	Lux_Util_SetRectFromText( region, text, 8, 12, (lux::display->screen_dimension.w/8) - 8  );

	region.h += 16; //padding
	region.w += 16; //padding
	region.x = (lux::display->screen_dimension.w - region.w) / 2;
	region.y = (lux::display->screen_dimension.h / 2) - (region.h/2);

	lux::gui = new UserInterface(lux::display);
	lux::gui->SetRegion( region );
	lux::gui->AddChild( region, dialog, text );

	if ( dialog == DIALOGTEXT && answer )
	{
		lux::gui->main_widget->SetText( *answer );
		this->msgdialog_return = answer;
	}
	else
	{
		this->msgdialog_return = nullptr;
	}
	this->msgdialog = 0;

	return true;
}

bool LuxEngine::ShowDialog(std::string text, LuxWidget dialog, std::string * answer )
{
	if ( this->CreateDialog(text, dialog, answer) )
	{
		while ( !this->msgdialog )
		{
			this->LoopDialog();
		}
		return this->EndDialog( dialog );
	}
	return true;
}


void LuxEngine::FatalError(std::string reason)
{
	this->state = GAMEERROR;
	if ( lux::core == nullptr )
	{
		std::cout << reason << std::endl;
		std::cerr << reason << std::endl;
	}
	else
	{
		lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR, reason);

		if ( lux::display == nullptr )
		{
			lux::display = new DisplaySystem( );
		}
		this->ShowDialog(reason, DIALOGOK);
	}

}


