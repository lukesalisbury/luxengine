/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef _GAMEINFO_H_
	#define _GAMEINFO_H_

	#include "gui/gui.h"
	#include "lux_types.h"
	#include "mokoi_game.h"
	#include "display/display.h"
	#include "tinyxml/tinyxml2.h"

	#include "portal.h"

	class GameInfo
	{
		public:
			GameInfo( UserInterface * gui, std::string game_path );
			GameInfo( UserInterface * gui, tinyxml2::XMLElement * xml_element );
			~GameInfo();

		private:
			void DetermitedType();

		public:
			uint32_t SetGUI( int32_t value, int32_t x, int32_t y, uint16_t width, uint16_t height  );
			void ClearGUI();

		public:
			uint8_t type;

		private:
			uint32_t hash;
			LuxRect buttonArea;
			Widget * button;
			UserInterface * gui;

			std::string information;
			std::string url;

			LuxSprite * icon;


	};

#endif
