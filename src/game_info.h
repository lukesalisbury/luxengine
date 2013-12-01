/****************************
Copyright © 2006-2011 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef _GAMEINFO_H_
	#define _GAMEINFO_H_

	#include "gui.h"
	#include "lux_types.h"
	#include "mokoi_game.h"
	#include "display.h"

namespace luxportal {
	class game
	{
		public:
			game( std::string gamepack, UserInterface * gui = NULL);
			game(UserInterface * gui= NULL);
			~game();

			bool SetIcon( DisplaySystem* display );
			bool ClearIcon( DisplaySystem* display );
			std::string url;
			Widget * button;
			UserInterface * _gui;
			LuxSprite * icon;
			MokoiGame * file;
			bool valid;
			bool isDir;
			DisplaySystem * display;
	};
}
#endif
