/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include <android/log.h>

namespace elix {
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
}

