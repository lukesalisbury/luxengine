/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "engine.h"
#include "elix/elix_string.hpp"
#include "mokoi_game.h"
#include "game_config.h"
#include "gui/gui.h"

namespace colour {
	LuxColour lang_dialog = {150, 150, 200, 200};
	LuxColour lang_button = {200, 200, 230, 255};
}

bool LuxEngine::LoadDefaultLanguage()
{
	std::string defaultlang = lux::config->GetString("language.default");

	if ( lux::display )
	{
		LuxRect region = {320, 240, 160,120,0};
		LuxRect button_region = {0, 0, 0,0,0};
		std::vector<std::string> langlist;

		langlist = lux::config->GetArray("language.available");

		if ( langlist.size() > 1 )
		{
			/* Display Language Chooser */
			uint16_t answer = 0;
			uint16_t button_height = 30;
			uint32_t list_height = 200;
			lux::gui = new UserInterface( lux::display );
			Widget * dialog, * widget;



			button_height = lux::gui->GetCSSParser()->GetSize(BUTTON, ENABLED, "min-height") ? lux::gui->GetCSSParser()->GetSize(BUTTON, ENABLED, "min-height") : 20;

			list_height = ( 1 + langlist.size() ) * (button_height + 6);

			region.w = 200;
			region.h = list_height;

			region.x = (lux::display->screen_dimension.w - region.w) / 2;
			region.y = (lux::display->screen_dimension.h / 2) - (region.h/2);


			lux::gui->SetRegion( region );
			dialog = lux::gui->AddChild(region, DIALOG, "Choose Language");

			button_region.y = 12;
			button_region.h = button_height;
			button_region.w = region.w - 32;

			for( uint16_t i = 0; i < langlist.size(); i++ )
			{
				widget = lux::gui->AddWidgetChild(dialog, 4, button_region.y, button_region.w, button_region.h, BUTTON, langlist[i].substr(3));
				widget->SetValue(i+1);

				button_region.y += button_height+2;
			}

			answer = lux::gui->ReturnResult()-1;
			if ( answer < langlist.size() )
			{
				defaultlang = langlist[answer];
			}

			NULLIFY( lux::gui );
		}
		langlist.clear();
	}
	return this->LoadLanguage( defaultlang.substr(0,2) );
}

bool LuxEngine::LoadLanguage(std::string lang)
{
	std::string buffer;

	if ( lux::game_data->HasFile("./lang/" + lang + ".txt"))
	{
		if ( lux::game_data->HasFile("./dialog/" + lang + ".txt") )
		{
			std::stringstream dialog_file("");
			if ( lux::game_data->GetStream("./dialog/" + lang + ".txt", &dialog_file) )
			{
				this->_dialog.clear();
				while ( dialog_file.good() )
				{
					std::getline(dialog_file, buffer);
					elix::string::Trim(&buffer);
					elix::string::Replace(buffer, "\\n", "\n");
					this->_dialog.push_back(buffer);
				}

			}
			dialog_file.str("");
		}
		std::stringstream file("");
		if ( lux::game_data->GetStream("./lang/" + lang + ".txt", &file) )
		{
			this->_strings.clear();
			while ( file.good() )
			{
				std::getline(file, buffer);
				elix::string::Trim(&buffer);
				elix::string::Replace(buffer, "\\n", "\n");
				this->_strings.push_back(buffer);
			}
		}
		file.str("");
		return true;
	}
	return false;
}



std::string LuxEngine::GetDialogString(uint32_t line)
{
	if ( line < this->_dialog.size() )
	{
		return this->_dialog.at(line);
	}
	return "DIALOG NOT FOUND";
}

std::string LuxEngine::GetString(uint32_t line)
{
	if ( line < this->_strings.size() )
	{
		return this->_strings.at(line);
	}
	return "STRING NOT FOUND";
}
