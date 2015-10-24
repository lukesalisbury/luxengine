/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include <algorithm>
#include <iostream>
#include <cctype>
#include "elix/elix_path.hpp"
#include "elix/elix_program.hpp"
#include "elix_private.hpp"
#include "elix/elix_string.hpp"


namespace elix {
	namespace program {

		std::string _user = "/ElixTemporaryContent/";
		std::string program_name = "ElixTemporaryContent";
		std::string document_name = "";

		std::string executable_file = "elix";
		std::string binary_dir = "/";
		std::string program_version = "";

		void printInfo();


		bool SetExecutable( std::string executable )
		{
			binary_dir = elix::path::GetBase( executable, false );
			executable_file = elix::path::GetName( executable );
			return true;
		}

		bool SetVersion( std::string version )
		{
			program_version = version;
			return true;
		}

		bool SetUserDocuments( std::string document )
		{
			/* Clear non alphanumeric characters */
			elix::string::StripUnwantedCharacters(document);
			if ( document.length() )
			{
				document_name.assign( 1, ELIX_DIR_SEPARATOR );
				document_name.append( document );
				document_name.append( 1, ELIX_DIR_SEPARATOR );
				return true;
			}
			return false;
		}

		bool SetName( std::string name )
		{
			/* Clear non alphanumeric characters */
			elix::string::StripUnwantedCharacters(name);
			if ( name.length() )
			{
				program_name = name;
			}
			else
			{
				program_name = "ElixTemporaryContent";
			}

			if ( !program_version.length() )
			{
				program_name = ELIX_VERSION_STRING;
			}

			/* Use Lower case program_name followed by version */
			_user.assign( 1, ELIX_DIR_SEPARATOR );
			_user.append( program_name );
			_user.append( 1, '-');
			_user.append( program_version );
			_user.append( 1, ELIX_DIR_SEPARATOR );
			std::transform( _user.begin(), _user.end(), _user.begin(), tolower );

			return true;
		}



		bool Set( std::string name, std::string version, std::string document, std::string executable )
		{
			SetExecutable( executable );
			SetVersion( version );
			SetUserDocuments( document );
			SetName( name );

			return true;
		}


		/* Returns Binary Directory */
		std::string RootDirectory( )
		{
			return binary_dir;
		}


		void printInfo()
		{
			std::cout << "Program Name: " << program_name << std::endl;
			std::cout << "Program Version: " << program_version << std::endl;
			std::cout << "Binary Directory: " << binary_dir << std::endl;
			std::cout << "Executable file: " << executable_file << std::endl;


			std::cout << "User: " << elix::directory::User("") << std::endl;
			std::cout << "User Documents: " << elix::directory::Documents(false) << std::endl;
			std::cout << "Global Documents: " << elix::directory::Documents(true) << std::endl;
			std::cout << "Cache: " << elix::directory::Cache("") << std::endl;
			std::cout << "Resources: " << elix::directory::Resources("") << std::endl;
		}

	}
}

