/****************************
Copyright © 2012-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/


namespace elix {
	namespace association {

		bool add( std::string path, std::string program, std::string name, std::string icon, std::string ext, std::string mime )
		{
			std::string reg("elix");
			std::string regname("elix");
			std::string run_path();
			std::string exe_path();

			size_t pos;

			while( (pos = icon.find_first_of("/")) != std::string::npos )
				icon.replace( pos, 1, "\\\\" );

			while( (pos = path.find_first_of("/")) != std::string::npos )
				path.replace( pos, 1, "\\\\" );
			path.append("\\", 2);

			run_path = path + program + " \"%1\"";
			exe_path = path + program;
			reg += ext;
			regname += name;

			while( (pos = regname.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890_-")) != std::string::npos )
				regname.erase( pos, 1 );

			if ( SHSetValue( HKEY_CURRENT_USER, reg.c_str(), NULL, REG_SZ, regname.c_str(), regname.length() ) != ERROR_SUCCESS )
			{
				return false;
			}

			if ( SHSetValue( HKEY_CURRENT_USER, regname.c_str(), NULL, REG_SZ, name.c_str(), name.length() ) != ERROR_SUCCESS )
			{
				return false;
			}

			regname += "\\shell\\open\\command";
			if ( SHSetValue( HKEY_CURRENT_USER, regname.c_str(), NULL, REG_SZ, run_path.c_str(), run_path.length() ) != ERROR_SUCCESS )
			{
				return false;
			}
		}

		bool remove(std::string program, std::string name, std::string ext, std::string mime )
		{
			std::string reg("elix");
			std::string regname("elix");
			reg += ext;
			regname += name;

			while( (pos = regname.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890_-")) != std::string::npos )
				regname.erase( pos, 1 );

			SHDeleteKey( HKEY_CURRENT_USER, reg.c_str() );
			SHDeleteKey( HKEY_CURRENT_USER, regname.c_str() );
		}
	}
}
