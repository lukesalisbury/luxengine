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
#include "test/test_code.h"
#include "core.h"
#include "config.h"

#include "elix_file.hpp"
#include "elix_string.hpp"

bool testMode = false;

#if defined(STANDALONE)

	extern "C" int main( int argc, char *argv[] )
	{
		if ( argc )
		{
			std::string base_executable = (argc ? argv[0] : "/lux" );
			lux::engine = new LuxEngine( base_executable );
			lux::global_config->SetString("project.file", argv[0]);
			if ( lux::engine->Start() )
			{
				lux::engine->Loop();
			}
			lux::engine->Close();
			delete lux::engine;
		}
		std::cout << "Error can not file executable." << std::endl
		return 0;
	}

#else
	void main_args( int argc, char *argv[] )
	{
		int c = argc;
		while ( c > 1 ) {
			--c;
			if ( strcmp(argv[c], "--portal") == 0 )
			{
				LuxPortal::use = true;
				LuxPortal::active = true;
			}
			else if ( strcmp(argv[c], "--window") == 0 )
			{
				lux::global_config->SetBoolean("display.fullscreen", false);
			}
			else if ( strcmp(argv[c], "--fullscreen") == 0 )
			{
				lux::global_config->SetBoolean("display.fullscreen", true);
			}
			else if ( strcmp(argv[c], "--test") == 0 )
			{
				testMode = true;
			}
			else if ( argv[c][0] != '-' )
			{
				lux::global_config->SetString("project.file", argv[c]);
				LuxPortal::add_previous_game( argv[c] );
				LuxPortal::use = false;
				LuxPortal::active = false;

			}
			else
			{
				// Unknown argument.
			}
		}
	}


	extern "C" int main( int argc, char *argv[] )
	{
		std::string base_executable = (argc ? argv[0] : "/luxengine" );

		lux::engine = new LuxEngine( base_executable );

		main_args( argc, argv );

		if ( testMode )
		{
			luxtest::run();
		}
		else
		{
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
							lux::global_config->SetString("project.file", selected_game.url );

							if ( lux::engine->Start() )
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
				if ( lux::engine->Start() )
				{
					lux::engine->Loop();
				}
				lux::engine->Close();
			}
			LuxPortal::close();
		}

		delete lux::engine;

		return 0;
	}

#endif





