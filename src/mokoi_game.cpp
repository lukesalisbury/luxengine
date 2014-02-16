/****************************
Copyright © 2006-2011 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include <algorithm>
#include <vector>
#ifdef NO_ZLIB
	#include "miniz.c"
#else
	#include <zlib.h>
#endif
#include <string>
#include <string.h>
#include <map>
#include "gui.h"
#include "downloader.h"
#include "mokoi_game.h"
#include "elix_path.hpp"
#include "elix_file.hpp"
#include "elix_intdef.h"
#include "elix_string.hpp"


namespace Mokoi {
	static uint8_t game_magic[6] = {137, 'M', 'o', 'k', 'o', 'i'};
	static uint8_t patch_magic[6] = {139, 'M', 'o', 'k', 'o', 'i'};
	static uint8_t resource_magic[6] = {138, 'M', 'o', 'k', 'o', 'i'};
	std::string folderlist_prefix;
	bool folder_check( std::pair<std::string, StoredFileInfo *> p )
	{
		return !p.first.compare( 2, folderlist_prefix.length(), folderlist_prefix );
	}
}



namespace Mokoi {
	int32_t gameSignatureOffset( elix::File * file )
	{
		uint8_t game_sign[8] = {137, 'M', 'o', 'k', 'o', 'i', '0', '\n'};
		int32_t p = -1;
		if ( file != NULL )
		{
			game_sign[6] = '1';
			p = file->Scan( 0, game_sign, 8 );

		}
		return p;
	}
}

MokoiGame::MokoiGame(  )
{
	this->id = 0;
	this->png = NULL;
	this->type = MOKOI_GAME_UNKNOWN;
	this->filename = "";
	this->valid = false;
}

MokoiGame::MokoiGame( std::string path, bool checkonly )
{
	this->id = 0;
	this->png = NULL;
	this->type = MOKOI_GAME_UNKNOWN;

	this->filename = path;

	type = this->GetType( this->filename, true );

	if ( type == MOKOI_GAME_UNKNOWN )
	{
		if ( !checkonly )
		{
			lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR) << "Invalid File: " << path << std::endl;
		}
		else
		{
			this->title = this->filename;
		}
		this->valid = false;
		return;
	}
	if ( !checkonly )
	{
		this->Scan( type, this->filename, "" );
		if ( this->SetProjectDirectory() )
		{
			this->Scan( MOKOI_GAME_DIRECTORY, this->public_directory, "");
		}
	}
	this->valid = true;
}

MokoiGame::~MokoiGame()
{
	for( std::map<std::string, StoredFileInfo *>::iterator iter = this->files.begin(); iter != this->files.end(); ++iter )
	{
		delete (*iter).second;
	}
	this->files.clear();
	while ( this->loaded_files.begin() != this->loaded_files.end())
	{
		delete (*this->loaded_files.begin());
		this->loaded_files.erase( this->loaded_files.begin() );
	}

	if ( this->png )
	{
		delete this->png;
	}
}

bool MokoiGame::SetProjectDirectory()
{
	if ( this->id )
	{
		std::stringstream project_id;
		project_id << std::hex << this->id;

		this->localid = project_id.str();

		this->public_directory =  elix::directory::User( this->localid );

		elix::File * title_file = new elix::File(this->public_directory + "title.txt", true);
		title_file->Write(this->title);
		delete title_file;

		lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << "Game Public Directory: " << this->public_directory << std::endl;


		return true;
	}
	else
	{
		this->public_directory = "";
	}
	return false;
}


uint8_t MokoiGame::GetType( std::string & path, bool info )
{
	if ( !path.compare(0, 4, "http")  )
	{
		return MOKOI_GAME_ONLINE;
	}

	if ( path.at( path.length()-1 ) == ELIX_DIR_SEPARATOR )
	{
		path.append(DIRECTORY_FILE);
	}
	else if ( elix::path::Exist(path) )
	{
		path.append(ELIX_DIR_SSEPARATOR);
		path.append(DIRECTORY_FILE);
	}

	uint8_t type = MOKOI_GAME_UNKNOWN;
	uint8_t buf[8];
	elix::File * file;
	std::string name = elix::path::GetName( path );

	if ( name == DIRECTORY_FILE )
	{
		file = new elix::File( path );
		if ( file->Exist() )
		{
			path.assign( elix::path::GetBase( path, true ) );
			if ( info )
			{
				std::string buffer;
				std::vector<std::string> array;
				while ( file->ReadLine(&buffer) )
				{
					elix::string::Trim(&buffer);
					if ( !buffer.empty() )
					{
						if ( buffer == "[Mokoi]" )
						{
							type = MOKOI_GAME_DIRECTORY;
						}
						else
						{
							array.clear();
							elix::string::Split(buffer, "=", &array);
							if ( array.size() == 2 )
							{
								if ( array[0] == "project.title" )
									this->title = array[1];
								else if ( array[0] == "project.author" )
									this->author = array[1];
								else if ( array[0] == "project.creation" )
									this->creation = array[1];
								else if ( array[0] == "project.id" )
									this->id = atoi(array[1].c_str());
							}
						}
					}
					buffer.clear();
				}
				if ( type == MOKOI_GAME_DIRECTORY )
				{
					elix::File * png_file = new elix::File( path + "resources"ELIX_DIR_SSEPARATOR"icon32.png" );
					if ( png_file->Exist() )
					{
						this->png_length = png_file->Length();
						if ( this->png_length )
						{
							this->png = new uint8_t[this->png_length];
							png_file->ReadAll( (void**)&png );
						}
					}
					delete png_file;
				}
			}
			else
			{
				file->Read( &buf, 1, 7 );
				buf[7] = 0;
				if ( memcmp(buf, "[Mokoi]", 7) == 0 )
				{
					type = MOKOI_GAME_DIRECTORY;
				}
			}
		}
		delete file;
	}
	else
	{
		file = new elix::File( path );
		if ( file->Exist() )
		{
			this->start_offset = Mokoi::gameSignatureOffset(file);
			if ( this->start_offset != -1 )
			{
				file->Seek(this->start_offset);
			}

			file->Read( &buf, 1, 8 );
			if ( buf[6] == '1' && buf[7] == 10 )
			{
				if ( memcmp(buf, Mokoi::game_magic, 6) == 0 )
				{
					type = ( start_offset > 0 ? MOKOI_GAME_EXECUTABLE : MOKOI_GAME_PACK);
					if ( info )
					{
						file->Read( &this->name, 1, 255);
						this->title.assign(this->name);
						file->Read_uint32( true );
						this->png_length = file->Read_uint32( true );
						if ( this->png_length )
						{
							this->png = new uint8_t[this->png_length];
							file->Read( this->png, 1, this->png_length);
						}
					}
				}
				else if ( memcmp(buf, Mokoi::patch_magic, 6) == 0 )
				{
					type = MOKOI_GAME_PATCH;
				}
				else if ( memcmp(buf, Mokoi::resource_magic, 6) == 0 )
				{
					type = MOKOI_GAME_RESOURCE;
				}
			}
			else
			{
				 this->start_offset = 0;
			}

		}
		delete file;
	}

	return type;
}

#include <sstream>
void MokoiGame::Print()
{
	lux::core->SystemMessage( SYSTEM_MESSAGE_LOG ) << "------MokoiGame::Print------" << std::endl;
	lux::core->SystemMessage( SYSTEM_MESSAGE_LOG ) << "Files: " << this->files.size() << std::endl;
	for( std::map<std::string, StoredFileInfo *>::iterator iter = this->files.begin(); iter != this->files.end(); ++iter )
	{
		StoredFileInfo * s = (*iter).second;

		lux::core->SystemMessage( SYSTEM_MESSAGE_LOG ) << (*iter).first << " ["<< (int)s->path_type << ":" << s->path << "]" <<  std::endl;
	}
	lux::core->SystemMessage( SYSTEM_MESSAGE_LOG) << "----------------------------" << std::endl;
}

bool MokoiGame::ScanPackage( uint8_t type, std::string path, std::string dest )
{
	bool result = false;
	uint8_t type_buffer[8];
	elix::File * file = new elix::File( path );
	uint32_t crc = 0;
	uint16_t name_length = 0;

	if ( !file->Exist() )
	{
		goto function_exit;
	}

	if ( type == MOKOI_GAME_EXECUTABLE )
	{
		if ( this->start_offset > 0 )
		{
			file->Seek(this->start_offset);
		}
	}

	file->Read( &type_buffer, 1, 8 );

	if ( type_buffer[6] != '1' || type_buffer[7] != 10 )
	{
		goto function_exit;
	}

	if ( (type == MOKOI_GAME_PACK || type == MOKOI_GAME_EXECUTABLE) && !memcmp(type_buffer, Mokoi::game_magic, 6) )
	{
		uint32_t logo_len = 0;

		this->id = file->Read_uint32( true );
		file->Read( &this->name, 1, 255);
		logo_len = file->Read_uint32( true );
		file->Seek( file->Tell() + logo_len );
		crc = file->Read_uint32( true ); // CRC
	}
	else if ( type == MOKOI_GAME_RESOURCE && !memcmp(type_buffer, Mokoi::resource_magic, 6) )
	{
		file->Seek( file->Tell() + 128+128+1 ); // Skip Title, Author and category
		lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << "scanning resource" << std::endl;
		crc = file->Read_uint32( true ); // CRC
	}
	else if ( type == MOKOI_GAME_PATCH && !memcmp(type_buffer, Mokoi::patch_magic, 6) )
	{
		uint32_t required_game_id = 0xFFFFFFFF;
		uint32_t required_game_crc = 0;

		required_game_id = file->Read_uint32( true );
		required_game_crc = file->Read_uint32( true );
		crc = file->Read_uint32( true );
		if ( (required_game_crc != crc ) || required_game_id != this->id )
		{
			lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << "Patch '" << path << "' is not compatable with this game." << std::endl;
			goto function_exit;
		}

	}
	else
	{
		goto function_exit;
	}


	// Scan file
	while ( !file->EndOfFile() )
	{
		name_length = file->Read_uint16( true );
		if ( name_length < 3 || name_length > 500 ) /* File Name should be longer in 3 char */
		{
			if ( name_length )
				lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR) << "Invalid File Name in Game Pack." << std::endl;
			break;
		}

		std::string file_name;

		StoredFileInfo * info = new StoredFileInfo();

		file->Read( &file_name, name_length );
		file_name.insert(0, dest);

		info->len = file->Read_uint32( true );
		info->compress = file->Read_uint32( true );
		info->offset = file->Tell();
		info->path = path;
		info->path_type = type;


		if ( type == MOKOI_GAME_RESOURCE )
		{
			//Resource file should not replace alreadly loaded files.
			if ( this->files.find(file_name) == this->files.end() )
				this->files[file_name] = info;
		}
		else
		{
			this->files[file_name] = info;
		}

		file->Seek( info->offset + info->compress );
	}
	result = true;


function_exit:

	delete file;
	return result;
}


bool MokoiGame::ScanDirectory( uint8_t type, std::string path, std::string dest, bool overwrite )
{
	std::vector<std::string> files;
	elix::path::Children( path, "", files, true, false, false );
	elix::path::Children( path, "c/scripts", files, true, false, false );
	elix::path::Children( path, "c/scripts/maps", files, false, false, false );
	elix::path::Children( path, "sprites/virtual", files, false, false, false );
	for( uint32_t i = 0; i < files.size(); i++ )
	{
		StoredFileInfo * info = new StoredFileInfo;

		std::string file_name = "./" + dest + files.at(i);
#if defined(__GNUWIN32__) || defined(__WINDOWS__)
		elix::string::Replace( file_name, "\\", "/" );
#endif

		info->len = 0;
		info->compress = 0;
		info->offset = 0;
		info->path = path + ELIX_DIR_SEPARATOR + files[i];
		info->path_type = type;

		if ( !overwrite )
		{
			//Resource file should not replace alreadly loaded files.
			if ( this->files.find(file_name) == this->files.end() )
				this->files[file_name] = info;
		}
		else
		{
			this->files[file_name] = info;
		}
	}
	return true;
}

bool MokoiGame::Scan( uint8_t type, std::string path, std::string dest )
{
	bool results = false;
	if ( type == MOKOI_GAME_PACK|| type == MOKOI_GAME_EXECUTABLE || type == MOKOI_GAME_RESOURCE || type == MOKOI_GAME_PATCH )
	{
		results = MokoiGame::ScanPackage( type, path, dest );
	}
	else if ( type == MOKOI_GAME_ONLINE )
	{
		if ( this->id )
		{
			StoredFileInfo * info = new StoredFileInfo;
			std::string basename = "sprites/" + elix::path::GetName(path);
			std::string savepath = this->public_directory + basename;
			std::string file_name = "./" + basename;

			Lux_Util_FileDownloaderBackground( path, savepath, lux::gui);

			this->files[file_name] = info;
			info->len = 0;
			info->compress = 0;
			info->offset = 0;
			info->path = savepath;
			info->path_type = type;
		}
		else
		{
			lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR) << "Project must have an ID to download files" << std::endl;
		}
	}
	else if ( type == MOKOI_GAME_DIRECTORY )
	{
		results =  this->ScanDirectory( type, path, dest, true );
	}
	return results;
}

bool MokoiGame::Add( std::string path )
{
	uint8_t type = MOKOI_GAME_UNKNOWN;
	std::string clean_path( path );

	type = this->GetType( clean_path, false );
	if ( type == MOKOI_GAME_UNKNOWN )
	{
		lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << "Invalid Mokoi Game File: '" << path <<"'"<< std::endl;
		return false;
	}
	this->Scan( type, clean_path, "" );
	return true;
}

bool MokoiGame::LoadPatches(  )
{
	if ( this->id )
	{
		std::vector<std::string> files;
		elix::path::Children( this->public_directory, "", files, false, false, false );
		for( uint32_t i = 0; i < files.size(); i++ )
		{
			this->Scan( MOKOI_GAME_PATCH,  this->public_directory + files[i], "" );
		}
		return true;
	}
	return false;
}

bool MokoiGame::LoadPackage( std::string package_filename  )
{
	bool package_found_successful = false;
	uint8_t package_type;
	std::string package_full_path;

	package_full_path += elix::directory::User("packages");
	package_full_path += package_filename;

	package_type = this->GetType( package_full_path, false );

	lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << "PACKAGE: " << package_full_path << " [" << (int)package_type << "]" << std::endl;


	if ( package_type == MOKOI_GAME_RESOURCE )
	{
		this->ScanPackage( MOKOI_GAME_RESOURCE,  package_full_path, "" );
		package_found_successful = true;
	}
	else if ( package_type == MOKOI_GAME_DIRECTORY )
	{
		this->ScanDirectory( MOKOI_GAME_DIRECTORY, package_full_path, "", false );
		package_found_successful = true;
	}
	return package_found_successful;
}

bool MokoiGame::FolderList(std::string folder, std::vector<std::string> *results)
{
	Mokoi::folderlist_prefix = folder;
	std::map< std::string, StoredFileInfo * >::iterator p;
	p = std::find_if( this->files.begin(), this->files.end(), Mokoi::folder_check );
	while ( p != this->files.end() )
	{
		results->push_back( p->first.substr( 3 + Mokoi::folderlist_prefix.length() ) );
		p++;
		p = std::find_if( p, this->files.end(), Mokoi::folder_check );
	}
	return true;
}

bool MokoiGame::HasFile(std::string file)
{
	#if DREAMCAST
	/* Temp fix for reading ISO 9660*/
	elix::string::Replace( file, "-", "_" );
	size_t q = file.find_first_of(".", 5);
	size_t l = file.find_last_of(".", std::string::npos);

	if ( l != std::string::npos && l != q )
	{
		file[q] = '_';
	}
	#endif

	std::map< std::string, StoredFileInfo * >::iterator p;
	p = this->files.find( file );

	if ( p != this->files.end() )
	{
		return true;
	}
	return false;
}

uint32_t MokoiGame::GetFile(std::string filename, uint8_t ** data, bool addnull)
{
	StoredFileInfo * info;

	#if DREAMCAST
	/* Temp fix for reading ISO 9660*/
	elix::string::Replace( filename, "-", "_" );
	size_t q = filename.find_first_of(".", 5);
	size_t l = filename.find_last_of(".", std::string::npos);

	if ( l != std::string::npos && l != q )
	{

		filename[q] = '_';
		lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << "ISO 9660:" << filename << std::endl;
	}

	#endif

	std::map< std::string, StoredFileInfo * >::iterator p;
	p = this->files.find( filename );

	if ( p != this->files.end() )
	{
		info = (StoredFileInfo *)p->second;
		elix::File * file = new elix::File( info->path );
		if ( file->Exist() )
		{
			if ( info->path_type == MOKOI_GAME_PACK || info->path_type == MOKOI_GAME_EXECUTABLE || info->path_type == MOKOI_GAME_PATCH || info->path_type == MOKOI_GAME_RESOURCE )
			{
				uint8_t * buffer = new uint8_t[info->compress];
				if (addnull)
				{
					*data = new uint8_t[info->len+1];
					(*data)[info->len] = 0;
					//memset(data, 0, info->len+1);
				}
				else
				{
					*data = new uint8_t[info->len];
					//memset(*data, 0, info->len);
				}
				file->Seek( info->offset );
				file->Read( buffer, 1, info->compress);
				unsigned long int tcompress = info->len;
				uncompress(*data, (uLongf*)&tcompress, buffer, info->compress);
				delete[] buffer;
			}
			else if ( info->path_type == MOKOI_GAME_DIRECTORY )
			{
				info->len = file->ReadAll( (data_pointer*)data, addnull );
			}
		}
		delete file;
		if ( data )
			return info->len;
	}
	return 0;
}

bool MokoiGame::GetStream(std::string file, std::stringstream * stream)
{
	uint8_t * data = NULL;
	if ( this->GetFile(file, &data, true ) )
	{
		if ( data )
			stream->str( (char *)data );
	}

	if ( data )
	{
		delete[] data;
		return true;
	}

	return false;
}

bool MokoiGame::CompileScripts()
{
	if ( this->type == MOKOI_GAME_DIRECTORY )
	{
		std::vector<std::string> scripts;
		elix::path::Children( this->filename, "scripts", scripts, false, false, false );
		elix::path::Children( this->filename, "scripts/maps", scripts, false, false, false );

		for( std::vector<std::string>::iterator iter = scripts.begin(); iter != scripts.end(); ++iter )
		{
			if ( elix::string::HasSuffix( (*iter), ".mps") )
			{
				lux::core->SystemMessage( SYSTEM_MESSAGE_LOG ) << (*iter) << " ["<< this->filename << "]" <<  std::endl;
				lux::core->RunExternalProgram( "pawn_compiler4", "--project="+ this->filename +" " + this->filename + (*iter) );
			}
		}
		this->ScanDirectory( MOKOI_GAME_DIRECTORY,  this->filename, "", false );
	}
	return true;
}

