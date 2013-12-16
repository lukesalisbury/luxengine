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
#include "mokoi_game.h"
#include "game_config.h"
#include "gui.h"

bool LuxEngine::LoadDefaultLanguage()
{
	std::string defaultlang = lux::config->GetString("language.default");

	//lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << "defaultlang:"<< defaultlang << std::endl;

	if ( lux::display )
	{
		uint32_t x_cen = 320, y_cen = 240;
		LuxRect region = {320,240,160,120,0};
		std::vector<std::string> langlist;

		langlist = lux::config->GetArray("language.available");

		if ( langlist.size() > 1 )
		{
			/* Display Language Chooser */
			uint16_t answer = 0;
			uint32_t list_height = langlist.size() * 10;
			x_cen = lux::display->screen_dimension.w /2;
			y_cen = lux::display->screen_dimension.h /2;

			region.x = x_cen - (x_cen / 2);
			region.y = y_cen - list_height - 16;
			region.w = x_cen;
			region.h = (list_height*2) + 20;

			lux::gui = new UserInterface( region, lux::display );
			lux::gui->AddChild(region, DIALOG, (LuxColour){150, 150, 200, 200}, "Choose Langauge");

			Widget * widget;
			region.x += 10;
			region.w -= 20;
			region.h = 18;

			for( uint16_t i = 0; i < langlist.size(); i++ )
			{
				region.y += 20;
				widget = lux::gui->AddChild(region, BUTTON, (LuxColour){200, 200, 230, 255}, langlist[i].substr(3));
				widget->SetValue(i+1);
			}

			answer = lux::gui->ReturnResult()-1;
			if ( answer < langlist.size())
			{
				defaultlang = langlist[answer];
			}
			delete lux::gui; lux::gui = NULL;
		}
		langlist.clear();
	}
	return this->LoadLanguage( defaultlang.substr(0,2) );
}

bool LuxEngine::LoadLanguage(std::string lang)
{
	std::string buffer;

	if ( lux::game->HasFile("./dialog/" + lang + ".txt") && lux::game->HasFile("./lang/" + lang + ".txt"))
	{
		std::stringstream dialog_file("");
		if ( lux::game->GetStream("./dialog/" + lang + ".txt", &dialog_file) )
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

		std::stringstream file("");
		if ( lux::game->GetStream("./lang/" + lang + ".txt", &file) )
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
