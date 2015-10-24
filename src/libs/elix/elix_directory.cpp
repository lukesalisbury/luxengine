/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include <algorithm>
#include <sys/stat.h>
#include <dirent.h>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <cstring>
#include <ctype.h>
#include "elix/elix_path.hpp"
#include "elix/elix_string.hpp"
#include "elix/elix_program.hpp"

#ifdef __GNUWIN32__
	#ifndef _WIN32_IE
		#define _WIN32_IE 0x0600
	#endif
	#include <io.h>
	#include <shlobj.h>
	#include <direct.h>
#endif

#ifdef ANDROID_NDK
	#include <SDL.h>
	#include <android/native_activity.h>
#endif



namespace elix {
	namespace directory {
		inline void AppendDirectory( std::string & path, std::string subdirectory )
		{
			elix::string::StripUnwantedCharacters( subdirectory );
			if ( subdirectory.length() )
			{
				path.append( 1, ELIX_DIR_SEPARATOR );
				path.append( subdirectory );

			}
		}
		inline void AppendFileName( std::string & path, std::string filename )
		{
			elix::string::StripUnwantedCharacters( filename );
			if ( filename.length() )
			{
				path.append( 1, ELIX_DIR_SEPARATOR );
				path.append( filename );
			}
		}

	}
	/* User's Directory Functions */
	#if defined(DREAMCAST) || defined(__NDS__) || defined(GEKKO) || defined(FLASCC)
	namespace directory {
		std::string User( std::string subdirectory, bool roaming, std::string filename )
		{
			#if defined (DREAMCAST)
			std::string full_directory = "/sd/";
			#else
			std::string full_directory = "/";
			#endif

			AppendDirectory( full_directory, subdirectory );
			AppendFileName( full_directory, filename );

			return full_directory;
		}

		std::string Documents( bool shared, std::string filename )
		{
			std::string full_directory = "/";
			AppendFileName( full_directory, filename );
			return full_directory;
		}
		std::string Cache( std::string filename )
		{
		#if defined (DREAMCAST)
			std::string full_directory = "/sd/cache/";
		#else
			std::string full_directory = "/cache/";
		#endif
			AppendFileName( full_directory, filename );
			return full_directory;
		}

		std::string Resources( std::string subdirectory, std::string filename )
		{
		#if defined (DREAMCAST)
			std::string full_directory = "/cd/";
		#else
			std::string full_directory = "/";
		#endif
			AppendDirectory( full_directory, subdirectory );
			AppendFileName( full_directory, filename );
			return full_directory;

		}
	}

	#elif defined(ANDROID_NDK)
	#include <android/log.h>
	namespace directory {
		std::string User( std::string subdirectory, bool roaming, std::string filename )
		{
			std::string full_directory;

			ANativeActivity * activity = (ANativeActivity*)SDL_AndroidGetActivity();

			full_directory = SDL_AndroidGetExternalStoragePath();
			full_directory.append(1, ELIX_DIR_SEPARATOR);

			AppendDirectory( full_directory, subdirectory );

			elix::path::Create( full_directory );

			AppendFileName( full_directory, filename );


			__android_log_print(ANDROID_LOG_INFO, "elix", "directory::user = %s", full_directory.c_str() );

			return full_directory;
		}

		std::string Documents( bool shared, std::string filename )
		{
			return User(elix::program::document_name, false, filename);
		}

		std::string Cache( std::string filename )
		{
			return User( ".cache", false, filename);
		}

		std::string Resources( std::string subdirectory, std::string filename )
		{
			return User(subdirectory, false, filename);
		}
	}
	#else

	namespace directory {
		std::string User( std::string subdirectory, bool roaming, std::string filename )
		{
			std::string full_directory = "./";

			#if defined(__WINDOWS__) || defined(__GNUWIN32__)
			bool valid = true;
			char directory[MAX_PATH];
			valid = SHGetSpecialFolderPath(NULL, directory, (roaming ? CSIDL_APPDATA : CSIDL_LOCAL_APPDATA), 1);
			if ( valid )
				full_directory = directory;
			#else
			/* POSIX + XDG */
			char * home_path = getenv( "XDG_DATA_HOME" );
			/* If XDG_DATA_HOME not set use HOME instead  */
			if ( home_path == NULL )
			{
				home_path = getenv( "HOME" );
				if ( home_path )
				{
					full_directory.assign(home_path);
					full_directory.append("/.local/share");
				}
				else
				{
					/* Fallback to current directory */
					full_directory.assign("./");
				}
			}
			else
			{
				full_directory.assign(home_path);
			}

			#endif

			AppendDirectory( full_directory, elix::program::_user );
			elix::path::Create( full_directory );

			AppendDirectory( full_directory, subdirectory );
			elix::path::Create( full_directory );

			AppendFileName( full_directory, filename );


			return full_directory;
		}


		std::string Documents( bool shared, std::string filename )
		{
			/* If not set with SetProgramName, return user directory */
			if ( !elix::program::document_name.length() )
			{
				return User("documents");
			}

			std::string full_directory = "./";
			bool valid = true;

		#ifdef __GNUWIN32__
			char directory[MAX_PATH];
			valid = SHGetSpecialFolderPath(NULL, directory, (shared ? CSIDL_COMMON_DOCUMENTS : CSIDL_PROFILE), 0);
			if ( valid )
				full_directory.assign(directory);
			AppendDirectory( full_directory, elix::program::document_name );
		#else
			if ( shared )
			{
				char * home_path = getenv( "XDG_DATA_DIRS" );
				if ( !home_path )
				{
					full_directory.assign("/usr/share/");
					full_directory.append(elix::program::document_name);
				}
				else
				{
					std::string path = home_path;
					std::vector<std::string> results;
					elix::string::Split( home_path, ":", &results );

					valid = false;

					if ( results.size()  )
					{
						for( uint32_t i = 0; i < results.size(); i++ )
						{
							path = results.at(i);
							path.append(elix::program::document_name);
							if ( elix::path::Exist(path) )
							{
								full_directory.assign(path);
								valid = true;
							}
						}
					}
				}

			}
			else
			{
				/* POSIX + XDG */
				char * home_path = getenv( "XDG_DATA_HOME" );
				/* If XDG_DATA_HOME not set use HOME instead  */
				if ( home_path == NULL )
				{
					home_path = getenv( "HOME" );
					if ( home_path )
					{
						full_directory.assign(home_path);
					}
					else
					{
						/* Fallback to current directory */
						full_directory.assign("./");
					}
				}
				else
				{
					full_directory.assign(home_path);
				}
				full_directory.append(elix::program::document_name);
			}
		#endif
			elix::path::Create( full_directory );

			AppendFileName( full_directory, filename );

			return full_directory;
		}

		std::string Cache( std::string filename )
		{
		#if defined (__GNUWIN32__)
			 return elix::directory::User( "cache", true, filename );
		#else
			std::string full_directory;
			char * home_path = NULL;
			home_path = getenv( "XDG_CACHE_HOME" );
			if ( !home_path )
			{
				home_path = getenv( "HOME" );
				if ( home_path )
				{
					full_directory.assign(home_path);
					full_directory.append("/.ElixCache/");
					elix::path::Create( full_directory );
				}
				else
				{
					full_directory.assign("./.ElixCache/");
				}
				elix::path::Create( full_directory );
			}
			else
			{
				full_directory.assign( home_path );
			}
			AppendDirectory( full_directory, elix::program::_user );
			elix::path::Create( full_directory );

			AppendFileName( full_directory, filename );

			return full_directory;
		#endif


		}

		std::string Resources( std::string subdirectory, std::string filename )
		{
			std::string full_directory = "./";
			std::string share_directory = elix::program::RootDirectory();

			AppendDirectory( share_directory, "share" );

			if ( elix::path::Exist( share_directory ) )
			{
				full_directory = share_directory;
			}
			else
			{
				share_directory = elix::program::RootDirectory();

				/* Takes into account the the binaries are stored in bin subdirectory */
				size_t pos = share_directory.find_last_of(ELIX_DIR_SEPARATOR);
				if ( pos > 1 )
					full_directory = share_directory.substr(0, pos);
				else
					full_directory = "..";

				AppendDirectory( full_directory, "share" );
			}

			AppendDirectory( full_directory, elix::program::_user );
			elix::path::Create( full_directory );

			AppendDirectory( full_directory, subdirectory );
			elix::path::Create( full_directory );

			AppendFileName( full_directory, filename );

			return full_directory;
		}
	}
	#endif

}

