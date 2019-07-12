/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/


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
	namespace path {


		std::string GetBase( std::string path, bool trailing )
		{
			int lastslash = path.find_last_of( ELIX_DIR_SEPARATOR, path.length() );
			if ( lastslash == -1 )
			{
				return "." ELIX_DIR_SSEPARATOR;
			}
			if ( trailing )
			{
				lastslash += 1;
			}
			if ( lastslash >= 2 )
			{
				return path.substr( 0, lastslash );
			}
			return path;
		}

		std::string GetName( std::string path )
		{
			std::string new_path = path;
			int lastslash = new_path.find_last_of( ELIX_DIR_SEPARATOR, new_path.length() );
			if ( lastslash )
			{
				new_path = new_path.substr( lastslash + 1 );
			}
			if ( ELIX_DIR_SEPARATOR != '/' )
			{
				lastslash = new_path.find_last_of( ELIX_DIR_SEPARATOR, new_path.length() );
				if ( lastslash )
				{
					new_path = new_path.substr( lastslash + 1 );
				}
			}
			return new_path;
		}

		/* Directory Functions */
		/***********************
		 * elix::path::Exist
		 @ std::string directory
		 - return true if it's a directory exist
		*/
		bool Exist( std::string path )
		{
			if ( path.length() > 1 )
			{
				if ( path.at( path.length()-1 ) == ELIX_DIR_SEPARATOR )
				{
					path.erase(path.length()-1);
				}
				struct stat directory;
				if ( !stat( path.c_str(), &directory ) )
				{
					return S_ISDIR(directory.st_mode);
				}
			}
			return false;
		}

		/***********************
		 * elix::path::Create
		 @ std::string directory
		 - return true if it's a directory exist
		*/
		bool Create( std::string path )
		{
			if ( !Exist(path) )
			{
			#ifdef PLATFORM_WINDOWS
				mkdir( path.c_str() );
			#else
				mkdir( path.c_str(), 0744 );
			#endif
			}
			return true;
		}

		/***********************
		 * elix::path::Children
		 @ std::string path
		 @ std::string subpath
		 @ std::vector<std::string> & list
		 @ bool deep
		 @ bool storepath
		 @ bool storedirectories
		 - return true if it's a directory exist
		*/
		bool Children( std::string path, std::string sub_path, std::vector<std::string> & list, bool deep, bool storepath, bool storedirectories )
		{
			dirent * entry = nullptr;
			std::string dir_path = path + ELIX_DIR_SSEPARATOR + sub_path;
			std::string file_path = path;
			DIR * dir = opendir( dir_path.c_str() );

			if ( !dir )
			{
				return false;
			}

			while ( (entry = readdir(dir)) != nullptr )
			{
				if ( (strcmp(entry->d_name, ".") != 0) && (strcmp(entry->d_name, "..") != 0) )
				{
					file_path = dir_path + ELIX_DIR_SSEPARATOR + entry->d_name;
					if ( elix::path::Exist( file_path ) )
					{
						if ( deep )
						{
							std::string dir_store(entry->d_name);
							Children( path, dir_store, list, false, storepath, storedirectories );
						}
						if ( storedirectories )
						{
							std::string name_store;

							if ( storepath )
							{
								name_store = file_path + ELIX_DIR_SSEPARATOR;
							}
							else if ( sub_path.length() )
							{
								name_store = sub_path + ELIX_DIR_SSEPARATOR + entry->d_name + ELIX_DIR_SSEPARATOR;
							}
							else
							{
								name_store =  entry->d_name;
								name_store.append(ELIX_DIR_SSEPARATOR);
							}
							list.push_back( name_store );
						}
					}
					else
					{
						std::string name_store;
						if ( storepath )
						{
							name_store = file_path;
						}
						else if ( sub_path.length() )
						{
							name_store = sub_path + ELIX_DIR_SSEPARATOR + entry->d_name;
						}
						else
						{
							name_store = entry->d_name;
						}
						list.push_back( name_store );
					}
				}
			}
			closedir(dir);
			return true;
		}
	}
}

