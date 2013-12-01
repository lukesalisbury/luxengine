/****************************
Copyright © 2009-2012  Luke Salisbury

http://creativecommons.org/licenses/by-nc-sa/3.0/

You are free:
to Share — to copy, distribute and transmit the work
to Remix — to adapt the work

Under the following conditions:
Attribution — You must attribute the work in the manner specified by the author or licensor (but not in any way that suggests that they endorse you or your use of the work).
Noncommercial — You may not use this work for commercial purposes.
Share Alike — If you alter, transform, or build upon this work, you may distribute the resulting work only under the same or similar license to this one.

With the understanding that:
Waiver — Any of the above conditions can be waived if you get permission from the copyright holder.
Public Domain — Where the work or any of its elements is in the public domain under applicable law, that status is in no way affected by the license.
Other Rights — In no way are any of the following rights affected by the license:
 - Your fair dealing or fair use rights, or other applicable copyright exceptions and limitations;
 - The author's moral rights;
 - Rights other persons may have either in the work itself or in how the work is used, such as publicity or privacy rights.
Notice — For any reuse or distribution, you must make clear to others the license terms of this work. The best way to do this is with a link to this web page.

****************************/
#include "engine.h"
#include "portal.h"
#include "core.h"
#include "config.h"
#include "elix_string.h"

#ifdef FLASCC
#include <AS3/AS3.h>

#define MODULE_EXPORT extern "C"

MODULE_EXPORT void update_lux_engine()
{
	std::cout << "update_lux_engine" << std::endl;
	if ( lux::engine->state > NOTRUNNING )
	{
		lux::engine->Refresh();
	}
}

int main(int argc, char * argv[])
{
	lux::engine = new LuxEngine( "/test.game", "/" );
	if ( lux::engine->Start() )
	{
		while ( lux::engine->state > NOTRUNNING )
		{
			update_lux_engine();
		}
	}
	lux::engine->Close();
	delete lux::engine;
}



#else

void main_args( int argc, char *argv[] )
{
	int c = argc;
	while ( c > 1 ) {
		--c;
		if ( strcmp(argv[c], "--portal") == 0 )
		{
			luxportal::use = true;
		}
		else if ( strcmp(argv[c], "--window") == 0 )
		{
			lux::config->SetBoolean("display.fullscreen", false);
		}
		else if ( strcmp(argv[c], "--fullscreen") == 0 )
		{
			lux::config->SetBoolean("display.fullscreen", true);
		}
		else if ( strcmp(argv[c], "--test") == 0 )
		{
			luxportal::testmode = true;
		}
		else if ( strcmp(argv[c], "--ogl") == 0 )
		{
			luxportal::opengl = true;
			luxtest::opengl = true;
			lux::config->SetString("display.mode", "OpenGL");
		}
		else if ( strcmp(argv[c], "--native") == 0 )
		{
			luxportal::opengl = false;
			luxtest::opengl = false;
			lux::config->SetString("display.mode", "native");
		}
		else if ( strcmp(argv[c], "--limitframes") == 0 )
		{
			lux::config->SetBoolean("display.limit", true);
		}
		else if ( argv[c][0] == '-' && strlen(argv[c]) == 6 )
		{
			if (argv[c][2] == 'b' && argv[c][3] == 'p' && argv[c][4] == 'p' )
			{
				if (argv[c][5] >= 48 && argv[c][5] < 52)
				{
					lux::config->SetNumber("display.bpp", argv[c][5] - 48);
				}
			}
		}
		else if ( argv[c][0] != '-' )
		{
			lux::config->SetString("project.file", argv[c]);
			luxportal::add( argv[c] );
			luxportal::use = false;
			luxportal::active = false;
		}
	}
}

#define DEFAULT_LOCATION ""


bool StartEngine()
{
	std::string quest_filename = lux::config->GetString("project.file");
	std::string classic_filename = lux::config->GetString("directory.program") + "ozc";


	#ifdef __GNUWIN32__
	if ( !classic_filename.compare("./ozc") )
	{
		classic_filename = "ozc";
	}
	#endif



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
			classic_filename.append(" ");
			classic_filename.append(quest_filename);
			classic_filename.append("");

			std::cout << "classic_filename " << classic_filename << std::endl;

			system( classic_filename.c_str() );
			return true;
		}
	}

	if ( lux::engine->Start() )
	{
		lux::engine->Loop();
	}
	lux::engine->Close();
	return true;
}


extern "C" int main( int argc, char *argv[] )
{

	std::string base_directory;

	base_directory = elix::path::GetBase( (argc ? argv[0] : DEFAULT_LOCATION ), true );
	lux::engine = new LuxEngine( base_directory + "demos"LUX_DIR_SSEPARATOR"Collision"LUX_DIR_SSEPARATOR, base_directory );
	main_args( argc, argv );


	if ( luxportal::testmode )
	{
		luxtest::run();
		return 0;
	}

	luxportal::open();

	if ( luxportal::use )
	{
		while ( luxportal::active )
		{
			if ( luxportal::run() )
			{
				StartEngine();
			}
		}

	}
	else
	{
		StartEngine();
	}
	luxportal::close();
	delete lux::engine;


	return 0;
}


#endif




