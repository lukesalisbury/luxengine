/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "engine.h"
#include "portal/portal.h"
#include "core.h"
#include "config.h"

std::string project_file = "";

extern "C" int main( int argc, char *argv[] )
{
	std::string base_executable = (argc ? argv[0] : "/luxengine" );
	lux::engine = new LuxEngine( base_executable );
	LuxPortal::open();
	if ( LuxPortal::use )
	{
		while ( LuxPortal::active )
		{
			if ( LuxPortal::run() )
			{
				GameInfoValues selected_game = LuxPortal::get_selected();

				if ( selected_game.type == GI_FILE  )
				{
					if ( lux::engine->Start(selected_game.url) )
					{
						lux::engine->Loop();
					}
					lux::engine->Close();
				}
			}
		}
	}
	else
	{
		if ( lux::engine->Start(project_file) )
		{
			lux::engine->Loop();
		}
		lux::engine->Close();
	}
	LuxPortal::close();
	delete lux::engine;
	

	return 0;
}







