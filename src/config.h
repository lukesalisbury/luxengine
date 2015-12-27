/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef _CONFIG_H_
	#define _CONFIG_H_

	#include "stdheader.h"
	#include <map>
	#include <vector>
	#include <list>
	#include <sstream>
	#include "elix/elix_file.hpp"


	class ConfigResource
	{
		public:
			ConfigResource(): spliter('|') {}
			virtual ~ConfigResource() { }
		private:
			std::map<std::string, std::string> values;
			std::list<std::string> modified_values;

			char spliter;
			bool watch_modified;

		public:
			std::string platform;

			void Set( std::string key, std::string value );
			void MarkModified(std::string key);

			virtual bool Has(std::string key) = 0;

			virtual std::string GetString(std::string key, bool alphanumeric = false) = 0;
			virtual int32_t GetNumber( std::string key ) = 0;
			virtual bool GetBoolean( std::string key ) = 0;
			virtual std::vector<std::string> GetArray( std::string key )  = 0;
			virtual fixed GetFixed( std::string key ) = 0;

			virtual void SetString( std::string key, std::string value ) = 0;
			virtual void SetNumber( std::string key, int32_t value ) = 0;
			virtual void SetBoolean( std::string key, bool value ) = 0;
			virtual void SetArray( std::string key, std::vector<std::string> value ) = 0;

		private:
			void ParseLine( std::string & buffer );
			bool LoadFromFile( elix::File * file );
			bool LoadFromString( std::string content );
			bool LoadFromStream(std::stringstream & config_file);
			void Print();

	};

	class Config: public ConfigResource
	{
		public:
			Config();
			~Config();

		public:
			bool Has(std::string key);

			std::string GetString(std::string key, bool alphanumeric = false);
			int32_t GetNumber( std::string key );
			bool GetBoolean( std::string key );
			std::vector<std::string> GetArray( std::string key );
			fixed GetFixed( std::string key );

			void SetString( std::string key, std::string value );
			void SetNumber( std::string key, int32_t value );
			void SetBoolean( std::string key, bool value );
			void SetArray( std::string key, std::vector<std::string> value );

		private:
			std::string path;
			void PlatformSettings();

	};


namespace lux
{
	extern Config * global_config;
}
#endif
