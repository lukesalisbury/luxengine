/****************************
Copyright © 2006-2011 Luke Salisbury
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
#include "config.h"

void LuxEngine::SettingDialog( )
{
	if ( lux::display == NULL || lux::gui != NULL || lux::global_config == NULL )
	{
		return;
	}


	LuxRect region = { (lux::display->screen_dimension.w / 2) - 100, (lux::display->screen_dimension.h / 2) - 100, 200, 200, 0 };

	lux::gui = new UserInterface(region, lux::display);

	Widget * parent = lux::gui->AddChild(region, DIALOGOK, (LuxColour){150, 150, 200, 200}, "Engine Settings");
	Widget * fs = lux::gui->AddWidgetChild(parent, 10, 16, 180, 18, CHECKBOX, "Full Screen Mode");
	Widget * ogl = lux::gui->AddWidgetChild(parent, 10, 36, 180, 18, CHECKBOX, "Use OpenGL");

	bool fullscreen = lux::global_config->GetBoolean("display.fullscreen");
	bool fullscreen_new = fullscreen;

	if ( lux::global_config->GetString("display.mode") == "OpenGL" )
		ogl->SetValue(1);

	fs->SetValue(fullscreen);

	this->msgdialog_return = NULL;
	this->msgdialog = 0;

	while ( !this->msgdialog )
	{
		this->msgdialog = lux::gui->ReturnResult();
	}

	if ( ogl->GetValue() )
		lux::global_config->SetString("display.mode", "OpenGL");
	else
		lux::global_config->SetString("display.mode", "native");

	fullscreen_new = !!fs->GetValue();
	if ( fullscreen != fullscreen_new )
	{
		lux::display->graphics.SetFullscreen( fullscreen_new );
		lux::global_config->SetBoolean("display.fullscreen", fullscreen_new);
	}
	delete lux::gui; lux::gui = NULL;



}

