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

#include "elix/elix_file.hpp"
#include "elix/elix_string.hpp"

bool testMode = false;
void * clientWindowID = NULL;


#if defined(ANDROID_NDK)
#include <android/asset_manager.h>
	extern "C" int main( int argc, char *argv[] )
	{
		if ( argc )
		{
			std::string base_executable = (argc ? argv[0] : "/lux" );
			lux::engine = new LuxEngine( base_executable );
			if ( lux::engine->Start("puttytris.game") )
			{
				lux::engine->Loop();
			}
			lux::engine->Close();
			delete lux::engine;
		}
		std::cout << "Error can not file executable." << std::endl;
		return 0;
	}

#elif defined(STANDALONE)

	extern "C" int main( int argc, char *argv[] )
	{
		if ( argc )
		{
			std::string base_executable = (argc ? argv[0] : "/lux" );
			lux::engine = new LuxEngine( base_executable );
			//lux::global_config->SetString("project.file", );
			if ( lux::engine->Start(argv[0]) )
			{
				lux::engine->Loop();
			}
			lux::engine->Close();
			delete lux::engine;
		}
		std::cout << "Error can not file executable." << std::endl;
		return 0;
	}

#elif defined(EMSCRIPTEN)
#include <emscripten.h>

bool running = false;
extern "C" {
	void loopGame()
	{
		lux::engine->Refresh();
	}

	int32_t loadGame( char * file )
	{
		lux::engine = new LuxEngine( "/luxengine" );
		return lux::engine->Start( file );
	}

	void startGame( )
	{
		emscripten_set_main_loop( loopGame, 0, 0);
		running = true;
	}

	void pauseGame()
	{
		if ( running )
			emscripten_cancel_main_loop();
		else
			startGame();
	}

	void endGame()
	{
		emscripten_cancel_main_loop();
		lux::engine->Close();
	}

}
extern "C" int main( int argc, char *argv[] )
{
	return 0;
}

#else
std::string project_file = "";

void main_args( int argc, char *argv[] )
{
	int c = argc;
	while ( c > 1 ) {
		--c;
		if ( strcmp(argv[c], "--test") == 0 )
		{
			testMode = true;
		}
		else if ( strncmp(argv[c], "--windowid=",11) == 0 )
		{
			clientWindowID = (void*)strtoul( argv[c]+11, NULL, 0 );
			std::cout << "clientWindowID " << clientWindowID << std::endl;
		}
		else if ( argv[c][0] != '-' )
		{
			project_file = argv[c];
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

	main_args( argc, argv );

	if ( testMode )
	{
		luxtest::run();
	}
	else if ( clientWindowID != 0 )
	{
		lux::engine = new LuxEngine( clientWindowID );
		if ( lux::engine->Start(project_file) )
		{
			lux::engine->Loop();
		}
		lux::engine->Close();
		delete lux::engine;
	}
	else
	{
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
	}



	return 0;
}

#endif





