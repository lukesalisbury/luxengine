/****************************
Copyright © 2006-2012 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "engine.h"
#include "portal.h"
#include "core.h"
#include "config.h"

void main_args( int argc, char *argv[] )
{
	int c = argc;
	while ( c > 1 ) {
		--c;
		if ( strcmp(argv[c], "--portal") == 0 )
		{
			luxportal::use = true;
			luxportal::active = true;
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
			luxportal::testmode = true;
		}
		else if ( strcmp(argv[c], "--ogl") == 0 )
		{
			luxportal::opengl = true;
			luxtest::opengl = true;
			lux::global_config->SetString("display.mode", "OpenGL");
		}
		else if ( strcmp(argv[c], "--native") == 0 )
		{
			luxportal::opengl = false;
			luxtest::opengl = false;
			lux::global_config->SetString("display.mode", "native");
		}
		else if ( argv[c][0] != '-' )
		{
			lux::global_config->SetString("project.file", argv[c]);
			luxportal::add( argv[c] );
			luxportal::use = false;
			luxportal::active = false;
		}
	}
}

extern "C" int main( int argc, char *argv[] )
{
	std::string base_directory;

	base_directory = elix::path::GetBase( (argc ? argv[0] : "" ), true );
	lux::engine = new LuxEngine( elix::path::Documents(false) + "Display/", base_directory );
	main_args( argc, argv );

	if ( luxportal::testmode )
	{
		luxtest::run();
	}

	luxportal::open();
	if ( luxportal::use )
	{
		while ( luxportal::active )
		{
			if ( luxportal::run() )
			{
				if ( lux::engine->Start() )
				{
					lux::engine->Loop();
				}
				lux::engine->Close();
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
	luxportal::close();


	delete lux::engine;



	return 0;
}







