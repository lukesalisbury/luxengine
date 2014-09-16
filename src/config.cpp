/****************************
Copyright © 2006-2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "luxengine.h"
#include "core.h"
#include "config.h"
#include "mokoi_game.h"
#include "elix_file.hpp"
#include "elix_string.hpp"
#include "elix_program.hpp"

void ConfigResource::Print()
{
	std::map<std::string, std::string>::iterator iter;
	for ( iter = this->values.begin(); iter != this->values.end(); iter++ )
	{
		lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << iter->first << "=" << iter->second << std::endl;
	}

}

void ConfigResource::Set( std::string key, std::string value )
{
	this->values[key] = value;
}

void ConfigResource::ParseLine( std::string & buffer )
{
	if ( buffer.empty() )
	{
		return;
	}

	std::vector<std::string> config_array;
	elix::string::Trim( &buffer );
	elix::string::Split(buffer, "=", &config_array);
	if ( config_array.size() == 3 )
	{
		if ( !config_array[0].compare(this->platform) )
		{
			this->Set(config_array[1], config_array[2] );
		}
	}
	else if ( config_array.size() == 2 )
	{
		this->Set(config_array[0], config_array[1] );
	}

	buffer.erase();
	config_array.clear();

}

bool ConfigResource::LoadFromFile(elix::File *file)
{
	std::string buffer;

	if ( file->Exist() )
	{
		while ( file->ReadLine( &buffer ) )
		{
			this->ParseLine(buffer);
		}
		return true;
	}
	return false;
}

bool ConfigResource::LoadFromString(std::string content)
{
	std::string buffer;
	std::stringstream config_file( content );
	do
	{
		std::getline(config_file, buffer);
		this->ParseLine(buffer);
	} while ( !config_file.eof() );
	return true;
}

bool ConfigResource::LoadFromStream(std::stringstream & config_file)
{
	std::string buffer;
	do
	{
		std::getline(config_file, buffer);
		this->ParseLine(buffer);
	} while ( !config_file.eof() );
	return true;
}

void ConfigResource::MarkModified(std::string key)
{
	this->modified_values.push_back(key);
}




Config::Config( )
{
	this->path = elix::program::RootDirectory();
	this->platform = "posix";

	this->SetString( "project.file", this->path + ELIX_DIR_SSEPARATOR"game"ELIX_DIR_SSEPARATOR ); /* Path to Default Game */
	this->SetString( "project.title", PROGRAM_NAME" "PROGRAM_VERSION );
	this->SetString( "language.default", "en" );
	this->SetString( "language.available", "en;English" );
	this->SetNumber( "player.number", 1 );
	this->SetBoolean( "server.able", false );
	this->SetBoolean( "save.allowed", true );
	this->SetBoolean( "map.centerview", true );
	this->SetBoolean( "display.fullscreen", false );
	this->SetBoolean( "patches.auto", true );

	this->PlatformSettings();

	/* Load Global Settings */
	elix::File * file = new elix::File( elix::directory::User("", "mokoi.config") );
	this->LoadFromFile(file);
	delete file;

	/* Check for in directory config */
	file = new elix::File( this->path + "mokoi.config" );
	this->LoadFromFile(file);
	delete file;

}

Config::~Config()
{

	if ( this->modified_values.size() )
	{
		this->modified_values.sort();
		this->modified_values.unique();

		elix::File * file = new elix::File( this->path + "mokoi.config", true );

		if ( file->Exist() )
		{
			file->WriteString("[Mokoi]\n");
			std::list<std::string>::iterator iter = this->modified_values.begin();
			while ( iter != this->modified_values.end() )
			{
				file->WriteString(*iter);
				file->WriteString("=");
				file->WriteString(this->values[*iter]);
				file->WriteString("\n");
				iter++;
			}
		}
		delete file;
	}


}


bool Config::Has(std::string key)
{
	std::map<std::string, std::string>::iterator iter = this->values.find(key);
	if ( iter == this->values.end() )
	{
		return false;
	}
	return true;
}

std::string Config::GetString( std::string key, bool alphanumeric )
{
	std::map<std::string, std::string>::iterator iter = this->values.find(key);
	if ( iter == this->values.end() )
	{
		return "not found";
	}
	return iter->second;
}

std::vector<std::string> Config::GetArray( std::string key )
{
	std::string value_string = this->GetString(key);
	std::vector<std::string> a;

	elix::string::Split( value_string, "|", &a);

	return a;
}

fixed Config::GetFixed( std::string key )
{
	std::string value_string = this->GetString(key);
	int32_t value = 0;
	std::stringstream stream(value_string);
	stream >> value;
	return MAKE_INT_FIXED(value);
}

int32_t Config::GetNumber( std::string key )
{
	std::string value_string = this->GetString(key);
	int32_t value = 0;
	std::stringstream stream(value_string);
	stream >> value;
	return value;
}

bool Config::GetBoolean( std::string key )
{
	std::string value_string = this->GetString(key);
	if ( value_string.compare("true") == 0 )
		return true;
	else
		return false;
}

void Config::SetString(std::string key, std::string value)
{
	this->values[key] = value;

	this->MarkModified(key);
}

void Config::SetArray(std::string key, std::vector<std::string> value)
{

	std::string str;
	uint32_t c = value.size();
	if ( c )
	{
		for( uint32_t i = 0; i < c; i++ )
		{
			str.append(value[i]);
			if ( i < c - 1 )
				str.append(1, this->spliter);
		}
	}
	this->values[key] = str;
	this->MarkModified(key);
}

void Config::SetNumber(std::string key, int32_t value)
{
	std::ostringstream stream("");
	stream << value;
	this->values[key] = stream.str();
	this->MarkModified(key);
}

void Config::SetBoolean(std::string key, bool value)
{
	this->values[key] = ( value ? "true" : "false");
	this->MarkModified(key);
}
