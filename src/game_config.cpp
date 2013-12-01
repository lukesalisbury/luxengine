/****************************
Copyright © 2013 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "game_config.h"
#include "core.h"
#include "mokoi_game.h"
#include "elix_string.h"

GameConfig::GameConfig()
{
	this->has_config_file = false;
	this->allowAll = false;

	if ( lux::game )
	{
		std::stringstream config_file;

		if ( lux::game->GetStream("./"DIRECTORY_FILE, &config_file) )
		{
			has_config_file = true;
		}
		else if ( lux::game->GetStream("./game.mokoi", &config_file) )
		{
			has_config_file = true;
		}

		//std::cout << "config_file" << config_file.str() << std::endl;
		if ( !has_config_file )
		{
			Lux_FatalError("Game Config file not found");
		}

		this->LoadFromStream(config_file);

		if ( lux::game->id )
		{
			this->path = lux::game->public_directory;

			elix::File * file = new elix::File( this->path + "game.config" );
			//this->LoadFromFile(file);
			delete file;
		}

		/*
		if ( !lux::config->GetNumber("screen.width") || !lux::config->GetNumber("screen.height") )
		{
			lux::config->SetNumber("screen.width", lux::config->GetNumber("map.width") );
			lux::config->SetNumber("screen.height", lux::config->GetNumber("map.height") );
		}
		*/
		if ( !this->GetBoolean("display.customsize") )
		{
			this->SetNumber("display.width", this->GetNumber("screen.width") );
			this->SetNumber("display.height", this->GetNumber("screen.height") );
		}

		config_file.str("");

	}
	else
	{
		this->allowAll = true;
	}
}

GameConfig::~GameConfig()
{
	/* Save if valid game */
	if ( lux::game && lux::game->id && has_config_file )
	{
		if ( this->modified_values.size() )
		{
			this->modified_values.sort();
			this->modified_values.unique();

			elix::File * file = new elix::File( this->path + "game.config", true );

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

}

bool GameConfig::Has(std::string key)
{
	std::map<std::string, std::string>::iterator iter = this->values.find(key);
	if ( iter == this->values.end() )
	{
		return lux::global_config->Has(key);
	}
	return true;
}

std::string GameConfig::GetString( std::string key, bool alphanumeric )
{
	std::map<std::string, std::string>::iterator iter = this->values.find(key);
	if ( iter == this->values.end() )
	{
		return lux::global_config->GetString(key);
	}
	return iter->second;
}

std::vector<std::string> GameConfig::GetArray( std::string key )
{
	std::string value_string = this->GetString(key);
	std::vector<std::string> a;

	elix::string::Split( value_string, "|", &a);

	return a;
}

fixed GameConfig::GetFixed( std::string key )
{
	std::string value_string = this->GetString(key);

	int32_t value = 0;
	std::stringstream stream(value_string);
	stream >> value;
	return MAKE_INT_FIXED(value);
}

int32_t GameConfig::GetNumber( std::string key )
{
	std::string value_string = this->GetString(key);

	int32_t value = 0;
	std::stringstream stream( value_string );
	stream >> value;
	return value;
}

bool GameConfig::GetBoolean( std::string key )
{
	std::string value_string = this->GetString(key);
	if ( !value_string.compare("true") )
		return true;
	else
		return false;
}

void GameConfig::SetString(std::string key, std::string value)
{
	if ( this->AllowChanges(key) )
	{
		this->values[key] = value;
		this->modified_values.push_back(key);
	}

}

void GameConfig::SetArray(std::string key, std::vector<std::string> value)
{
	if ( this->AllowChanges(key) )
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
		this->modified_values.push_back(key);
	}

}

void GameConfig::SetNumber( std::string key, int32_t value )
{
	if ( this->AllowChanges(key) )
	{
		std::ostringstream stream("");
		stream << value;

		this->values[key] = stream.str();
		this->modified_values.push_back(key);
	}
}

void GameConfig::SetBoolean( std::string key, bool value )
{
	if ( this->AllowChanges(key) )
	{
		this->values[key] = ( value ? "true" : "false");
		this->modified_values.push_back(key);
	}
}

bool GameConfig::AllowChanges( std::string key )
{
	if ( this->allowAll )
		return true;
	if ( !key.compare("server.ip") || !key.compare("display.mode") || !key.compare("display.fullscreen") )
	{
		return true;
	}
	return false;
}
