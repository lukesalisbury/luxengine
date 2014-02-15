/****************************
Copyright © 2006-2011 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef _ENTITY_SYSTEM_H_
#define _ENTITY_SYSTEM_H_

#include "stdheader.h"
class Entity;

typedef struct {
		uint8_t system_identifier;
		bool (*Init) ( std::string entity_id, std::string entity_base, mem_pointer & entity_data, Entity * entity );
		void (*Destroy) ( mem_pointer entity_data );
		void (*Restore) ( elix::File * current_save_file, mem_pointer entity_data );
		void (*Save) ( elix::File * current_save_file, mem_pointer entity_data );
		bool (*Run) ( mem_pointer entity_data, bool & scriptcontinue );
		int32_t (*Call) ( mem_pointer entity_data, char * function, native_pointer stack_mem  );
		bool (*PushArray) ( mem_pointer entity_data, int32_t array[], uint32_t size, native_pointer stack_mem);
		bool (*PushString) ( mem_pointer entity_data, std::string str, native_pointer stack_mem);
		bool (*Push) ( mem_pointer entity_data, int32_t value);
	#ifdef __x86_64__
		bool (*PushArrayNative) ( mem_pointer entity_data, native_type array[], uint32_t size, native_pointer stack_mem);
	#else
		bool (*PushArrayNative) ( mem_pointer entity_data, int32_t array[], uint32_t size, native_pointer stack_mem);
	#endif

		
} LuxEntityCallback;

void Lux_PawnCache_Init();
void Lux_PawnCache_Cleanup();
void Lux_PawnCache_SetSaveMode(uint8_t mode);
extern LuxEntityCallback EntitySystemPawn;

class EntitySystem
{
		EntitySystem()
		{
			Lux_PawnCache_Init();
		}
		~EntitySystem()
		{
			Lux_PawnCache_Cleanup();
		}

		std::string GetMainScriptFile()
		{
			#ifdef __x86_64__
			return "./c/scripts/main.amx64";
			#else
			return "./c/scripts/main.amx";
			#endif
		}

		void SetSaveMode(uint8_t mode)
		{
			Lux_PawnCache_SetSaveMode(mode);
		}


		LuxEntityCallback * GetSystem( uint8_t identifier )
		{
			if ( identifier == 'p' )
			{

			}
			return &EntitySystemPawn;
		}

		LuxEntityCallback * GetSystem( std::string filename )
		{
			std::string ext = ".amx";
			std::string::size_type pos;
			pos = filename.find_last_of('.');
			if ( pos != std::string::npos )
			{
				ext = filename.substr(pos+1);
			}

			if ( ext == ".js" )
			{

			}
			else if ( ext == ".sq" )
			{

			}
			else if ( ext == ".lua" )
			{

			}
			return &EntitySystemPawn;
		}

};

namespace lux
{
	extern EntitySystem * entitysystems;
}
#endif
