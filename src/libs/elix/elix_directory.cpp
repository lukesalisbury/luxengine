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
		inline void AppendDirectoryAndCreate( std::string & path, std::string subdirectory )
		{
			elix::string::StripUnwantedCharacters( subdirectory );
			if ( subdirectory.length() )
			{
				path.append( 1, ELIX_DIR_SEPARATOR );
				path.append( subdirectory );
				elix::path::Create( path );
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
}
/* User's Directory Functions */
#if defined(DREAMCAST) || defined(__NDS__) || defined(GEKKO) || defined(__3DS__)
	#include "elix_directory_console.cpp"
#elif defined(ANDROID_NDK)
	#include "elix_directory_android.cpp"
#elif defined(PLATFORM_WINDOWS)
	#include "elix_directory_windows.cpp"
#else
	#include "elix_directory_std.cpp"
#endif

