#ifndef GAME_CONFIG_H
#define GAME_CONFIG_H

#include "config.h"
#include <list>

class GameConfig : public ConfigResource
{
	public:
		GameConfig();
		~GameConfig();

	private:
		std::string local_config_path;
		bool has_config_file;
		bool allow_all;

	public:
		bool Save();
		bool Has(std::string key);
		bool AllowChanges( std::string key );

		std::string GetString(std::string key, bool alphanumeric = false);
		int32_t GetNumber( std::string key );
		bool GetBoolean( std::string key );
		std::vector<std::string> GetArray( std::string key );
		fixed GetFixed( std::string key );

		void SetString( std::string key, std::string value );
		void SetNumber( std::string key, int32_t value );
		void SetBoolean( std::string key, bool value );
		void SetArray( std::string key, std::vector<std::string> value );


};




namespace lux
{
	extern GameConfig * config;
}

#endif // GAME_CONFIG_H
