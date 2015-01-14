/****************************
Copyright © 2013-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#ifndef SAVE_SYSTEM_H
#define SAVE_SYSTEM_H

#include "stdheader.h"
#include "elix_file.hpp"
#include "game_system.h"
#include "entity_manager.h"


class LuxSaveState
{
	public:
		LuxSaveState( );
		~LuxSaveState();
	private:
		elix::File * save_file;

		uint8_t save_type;
		uint8_t save_game_slot;
		uint32_t game_id;

		bool public_access;
		std::string file_name;

	public:

	private:
		bool PreSave(EntityManager *entity_manager);
		bool PreLoad(EntityManager *entity_manager);

		uint8_t GetSaveType();

		bool SaveHibernateFile( GameSystem * old_game_world, EntityManager * old_entity_manager );
		bool SaveDataFile(GameSystem * old_game_world, EntityManager * old_entity_manager, int32_t * info, uint32_t length  );
		bool SaveCookieFile( int32_t * info, uint32_t length );

		bool LoadHibernateFile( GameSystem * new_game_world, EntityManager * new_entity_manager );
		bool LoadDataFile( GameSystem * new_game_world, EntityManager * new_entity_manager, int32_t * info, uint32_t length );
		bool LoadCookieFile( int32_t * info, uint32_t length  );

	public:
		bool Exists( uint8_t slot );
		void SetInformation(uint32_t id, uint8_t save_file_type, std::string file_title, elix::Image * png_screen_shot );
		void AllowPublicAccess();
		void SetSlot( uint8_t slot );
		bool Restore( GameSystem * new_game_world, EntityManager * new_entity_manager, int32_t * info, uint32_t length );
		bool Save( GameSystem * old_game_world, EntityManager * old_entity_manager, int32_t * info, uint32_t length );


		bool ReadCookieFile( uint32_t requested_game_id );

};


#endif // SAVE_SYSTEM_H
