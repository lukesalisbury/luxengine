/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef _MOKOI_GAME_H_
#define _MOKOI_GAME_H_

#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include "elix_file.hpp"

#ifndef DIRECTORY_FILE
	#define DIRECTORY_FILE "game.mokoi"
#endif

typedef enum {
	MOKOI_GAME_UNKNOWN,
	MOKOI_GAME_PACK,
	MOKOI_GAME_PATCH,
	MOKOI_GAME_DIRECTORY,
	MOKOI_GAME_RESOURCE,
	MOKOI_GAME_EXECUTABLE,
	MOKOI_GAME_ONLINE
} MokoiGameType;

class StoredFileInfo {
	StoredFileInfo() : path_type(0), len(0), compress(0), offset(0)
	{

	}
	std::string path;
	uint8_t path_type;
	uint32_t len, compress;
	int32_t offset;
};

class MokoiGame
{
	public:
		MokoiGame(  );
		MokoiGame(std::string path, bool scan_files = true );
		~MokoiGame();

	private:
		uint32_t ident;
		char name[512];
		uint8_t * png;
		uint32_t png_length;
		std::string project_ident;
		std::string title;
		std::string author;
		std::string creation;
		std::string filename;
		std::string public_directory;
		bool has_public_directory;
		bool valid;

		uint8_t file_magic[6];
		uint8_t patch_magic[7];
		std::map< std::string, StoredFileInfo * > files;
		std::vector< elix::File * > loaded_files;
		uint8_t type;
		int32_t start_offset;

	public:

		bool CompileScripts();
		bool LoadPatches( );
		bool LoadPackage( std::string package_filename );
		bool AddPackage( std::string path );


		bool HasFile(std::string file);
		uint32_t GetFile(std::string file, uint8_t ** data, bool addnull);
		uint32_t SetFile(std::string file, uint8_t * data);
		bool GetStream(std::string file, std::stringstream * stream);

		bool FolderList(std::string folder, std::vector<std::string> * results);

		uint8_t GetType( );
		std::string GetTitle();
		std::string GetIdent();
		std::string GetAuthor();
		std::string GetCreation();
		std::string GetFilename();
		std::string GetPublicDirectory();
		uint32_t GetProjectIdent();

		void Print();
	private:
		bool Scan( uint8_t type, std::string path, std::string dest );
		bool ScanPackage( uint8_t type, std::string path, std::string dest );
		bool ScanDirectory(uint8_t type, std::string path, std::string dest, bool overwrite = false);

		bool HasProjectDirectory();
		bool SetProjectDirectory();
		uint8_t ReadType( std::string &path, bool read_info );
};


namespace lux
{
	extern MokoiGame * game_data;
}


#endif /* _MOKOI_GAME_H_ */
