/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

namespace elix {
	namespace directory {
		std::string User( std::string subdirectory, bool roaming, std::string filename )
		{
			std::string full_directory;
			#if defined (DREAMCAST)
			full_directory = "/sd/";
			#else
			if (roaming)
				full_directory = "/mokoi/global";
			else
				full_directory = "/mokoi/user";
			#endif

			AppendDirectory( full_directory, subdirectory );
			AppendFileName( full_directory, filename );

			return full_directory;
		}

		std::string Documents( bool shared, std::string filename )
		{
			std::string full_directory = "/mokoi/global/";
			if (shared)
				full_directory = "/mokoi/global/";
			else
				full_directory = "/mokoi/user/";
			AppendFileName( full_directory, filename );
			return full_directory;
		}
		std::string Cache( std::string filename )
		{
		#if defined (DREAMCAST)
			std::string full_directory = "/sd/cache/";
		#else
			std::string full_directory = "/mokoi/cache/";
		#endif
			AppendFileName( full_directory, filename );
			return full_directory;
		}

		std::string Resources( std::string subdirectory, std::string filename )
		{
		#if defined (DREAMCAST)
			std::string full_directory = "/cd/";
		#else
			std::string full_directory = "/mokoi/";
		#endif
			AppendDirectory( full_directory, subdirectory );
			AppendFileName( full_directory, filename );
			return full_directory;

		}
	}

}

