#ifndef ELIX_PROGRAM_HPP
#define ELIX_PROGRAM_HPP

#include <string>

namespace elix {
	namespace program {
		extern std::string _user;
		extern std::string program_name;
		extern std::string document_name;
		extern std::string program_version;
	}

	namespace program {
		bool Set( std::string name, std::string version, std::string document, std::string executable );
		bool SetName( std::string name );
		bool SetVersion( std::string version );
		bool SetUserDocuments(  std::string document );
		bool SetExecutable(  std::string executable );

		/* Returns Binary Directory */
		std::string RootDirectory( );
	}
}
#endif // ELIX_PROGRAM_HPP
