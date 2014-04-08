
/****************************
Copyright © 2007-2014 Luke Salisbury
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

#include "elix_file.hpp"
#include "elix_string.hpp"


bool StartEngine()
{







}

	bool is_classic_quest( std::string quest_filename )
	{
		if ( quest_filename.at(0) == '\'' )
		{
			quest_filename = quest_filename.substr(1, quest_filename.length() -2);
		}

		std::vector<std::string> name_split;
		elix::string::Split(quest_filename, ".", &name_split);
		if ( name_split.size() >= 2 )
		{
			if ( !name_split.at(name_split.size()-1).compare("qst") )
			{
				return true;
			}
		}
		return false;

	}

	bool launch_classic( std::string game )
	{
		std::string classic_filename = lux::config->GetString("directory.program") + "ozc";
		#ifdef __GNUWIN32__
		if ( !classic_filename.compare("./ozc") )
		{
			classic_filename = "ozc";
		}
		#endif

		classic_filename.append(" ");
		classic_filename.append(quest_filename);
		classic_filename.append("");

		lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << "classic_filename " << classic_filename << std::endl;

		system( classic_filename.c_str() );


	}


extern "C" void lux_engine_loop()
{
	std::string quest_filename = lux::config->GetString("project.file");
	if ( is_classic_quest( quest_filename ) )
	{
		launch_classic( quest_filename );
	}
	else
	{
		if ( lux::engine->Start() )
		{
			lux::engine->Loop();
		}
		lux::engine->Close();
	}
}

#if defined(STANDALONE)
	extern "C" int main( int argc, char *argv[] )
	{
		if ( argc )
		{
			std::string base_executable = (argc ? argv[0] : "/lux" );
			lux::engine = new LuxEngine( base_executable );
			lux::global_config->SetString("project.file", argv[0]);
			lux_engine_loop();
			delete lux::engine;
		}
		return 0;
	}

#else


	void main_args( int argc, char *argv[], bool skipProjectFile )
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
				if ( !skipProjectFile )
				{
					lux::global_config->SetString("project.file", argv[c]);
					luxportal::add( argv[c] );
					luxportal::use = false;
					luxportal::active = false;
				}
			}
			else
			{
				// Unknown argument.
			}
		}
	}


	namespace Mokoi {
		int32_t gameSignatureOffset( elix::File * file );
	}

	bool checkForAttachedGame( char * argv )
	{
		bool result = false;
		elix::File * file = new elix::File( std::string(argv), false );

		result = Mokoi::gameSignatureOffset( file ) > 1 ? true : false;


		return result;
	}

	extern "C" void lux_engine_init()
	{

	}



	extern "C" int main( int argc, char *argv[] )
	{
		bool gameAttached = false;
		std::string base_executable = (argc ? argv[0] : "/luxengine" );

		lux::engine = new LuxEngine( base_executable );

		main_args( argc, argv, gameAttached );
	/*
		lux::global_config->SetString("project.file", "/sdcard/Android/data/info.mokoi.lux/files/mokoi-games/puttytris.game");
		luxportal::use = false;
		luxportal::active = false;
	*/
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
					lux_engine_loop();
				}
			}

		}
		else
		{
			lux_engine_loop();
		}
		luxportal::close();


		delete lux::engine;



		return 0;
	}

#endif





