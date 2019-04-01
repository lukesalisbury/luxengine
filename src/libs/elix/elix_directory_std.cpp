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
			std::string full_directory = "./";

			char * home_path = getenv( "XDG_DATA_HOME" );
			/* If XDG_DATA_HOME not set use HOME instead  */
			if ( home_path == nullptr )
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

			AppendDirectoryAndCreate( full_directory, elix::program::_user );
			AppendDirectoryAndCreate( full_directory, subdirectory );
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
				if ( home_path == nullptr )
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
			elix::path::Create( full_directory );

			AppendFileName( full_directory, filename );

			return full_directory;
		}

		std::string Cache( std::string filename )
		{
			std::string full_directory;
			char * home_path = nullptr;
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
			AppendDirectoryAndCreate( full_directory, elix::program::_user );
			AppendFileName( full_directory, filename );

			return full_directory;
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

			AppendDirectoryAndCreate( full_directory, elix::program::_user );
			AppendDirectoryAndCreate( full_directory, subdirectory );
			AppendFileName( full_directory, filename );

			return full_directory;
		}
	}

}

