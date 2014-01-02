/****************************
Copyright © 2006-2011 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "engine.h"
#include "core.h"
#include "gui.h"
#include "game_config.h"
#include "audio.h"
#include "display.h"
#include "world.h"
#include "entity_manager.h"
#include "elix_file.hpp"
#include "elix_path.hpp"
#include "elix_string.hpp"
#include "mokoi_game.h"
#include "save_system.h"

uint8_t save_magic_mime[7] = {137, 'L', 'E', 'S', 'A', 'V', 'E' };

/*
 * world
 *
 *
 *
 *
 *
 *
 *
 *
 */





/* Save System */

void Lux_LoadFileFailed(std::string msg)
{
	lux::engine->state = RUNNING;
}




/* LuxSaveState::LuxSaveState
 *
 */
LuxSaveState::LuxSaveState( )
{
	this->save_game_slot = 0xFF;
	this->save_type = LUX_SAVE_COOKIE_TYPE;
}

/* LuxSaveState::LuxSaveState
 *
 */
LuxSaveState::~LuxSaveState()
{

}

/* LuxSaveState::Exists
 *
 */
bool LuxSaveState::Exists( uint8_t slot )
{
	bool save_file_exists = false;
	/* File Name */
	this->file_name = lux::game->public_directory;
	if ( slot == 0x00  )
	{
		this->save_type = LUX_SAVE_HIBERNATE_TYPE;
		this->file_name.append("0.save");
	}
	else if ( slot != 0xFF )
	{
		this->file_name.append( elix::string::FromInt( slot ) );
		this->file_name.append(".save");
	}
	else
	{
		return false;
	}

	this->save_file = new elix::File( this->file_name  );
	save_file_exists = this->save_file->Exist();

	delete this->save_file;
	return save_file_exists;
}


/* LuxSaveState::SetSlot
 *
 */
void LuxSaveState::SetSlot( uint8_t slot )
{
	this->save_game_slot = slot;


}

/* LuxSaveState::SetInformation
 *
 */
void LuxSaveState::SetInformation( uint32_t id, uint8_t save_file_type, std::string file_title, elix::Image * png_screen_shot )
{
	this->save_type = save_file_type;
	this->game_id = id;
}



/* LuxSaveState::PreSave
 *
 */
bool LuxSaveState::PreSave( EntityManager * entity_manager)
{
	uint8_t allow_access = 0;

	if ( lux::game->public_directory.length() == 0 )
	{
		lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << "Invalid Save Directory." <<  std::endl;
		return false;
	}

	if ( entity_manager->_global )
		entity_manager->_global->Call((char*)"save", (char*)"");
	lux::display->Loop( SAVING );


	/* File Name */
	this->file_name = lux::game->public_directory;

	if ( this->save_game_slot == 0x00  )
	{
		this->save_type = LUX_SAVE_HIBERNATE_TYPE;
		this->file_name.append("0.save");
	}
	else if ( this->save_game_slot != 0xFF )
	{
		this->file_name.append( elix::string::FromInt( this->save_game_slot ) );
		this->file_name.append(".save");
	}
	else
	{
		return false;
	}

	/* Open File */
	lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << "Saving to " << this->file_name << std::endl;

	/* Todo Add overwrite warning */
	this->save_file = new elix::File( this->file_name, true );
	if ( !this->save_file->Exist() )
	{
		lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR) << "===========================Saved Failed========================" << std::endl;
		delete this->save_file;
		return false;
	}
	else
	{
		this->save_file->Write( &save_magic_mime, 7, 1 );
		this->save_file->Write( (uint8_t)(this->save_type) );
		this->save_file->Write( (uint8_t)'\n' );

		allow_access = lux::config->GetBoolean("save.access");
		if ( lux::config->GetBoolean("save.allowed") )
		{
			allow_access += 2;
		}
		this->save_file->Write( allow_access );

		return true;
	}
}

/* LuxSaveState::Save
 *
 */
bool LuxSaveState::Save(WorldSystem * old_world, EntityManager * old_entity_manager , int32_t *info, uint32_t length)
{
	if ( !this->PreSave(old_entity_manager) )
	{
		return false;
	}

	lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR) << "================ Save ===============" << std::endl;
	lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR) << "============= Type: "  << +this->save_type << " =============" << std::endl;

	lux::entities->SetSaveMode( this->save_type );

	if ( this->save_type == LUX_SAVE_COOKIE_TYPE )
	{
		return this->SaveCookieFile( info, length );
	}
	else if ( this->save_type == LUX_SAVE_DATA_TYPE )
	{
		return this->SaveDataFile( old_world, old_entity_manager, info, length );
	}
	else if ( this->save_type == LUX_SAVE_HIBERNATE_TYPE )
	{
		return SaveHibernateFile( old_world, old_entity_manager );
	}
	else
	{
		return false;
	}

}

/* LuxSaveState::SaveDataFile
 *
 */
bool LuxSaveState::SaveDataFile(WorldSystem * old_world, EntityManager * old_entity_manager , int32_t *info, uint32_t length)
{

	old_entity_manager->SetSaveMode( LUX_SAVE_DATA_TYPE );

	for (uint8_t count = 0; count < length; count++)
	{
		this->save_file->Write( (uint32_t)info[count] );
	}

	old_world->Save( this->save_file );

	delete this->save_file;
	return true;
}

/* LuxSaveState::SaveCookieFile
 *
 */
bool LuxSaveState::SaveCookieFile( int32_t * info, uint32_t length   )
{
	for (uint8_t count = 0; count < length; count++)
	{
		this->save_file->Write( (uint32_t)info[count] );
	}

	delete this->save_file;
	return true;
}

/* LuxSaveState::SaveHibernateFile
 *
 */
bool LuxSaveState::SaveHibernateFile( WorldSystem * old_world, EntityManager * old_entity_manager )
{
	bool restoring_results = false;

	old_entity_manager->SetSaveMode( LUX_SAVE_HIBERNATE_TYPE );
	restoring_results = old_world->Save( this->save_file );

	delete this->save_file;
	return restoring_results;
}


/* LuxSaveState::PreLoad
 *
 */
bool LuxSaveState::PreLoad(EntityManager *entity_manager)
{
	if ( this->save_type != LUX_SAVE_COOKIE_TYPE )
	{
		if (entity_manager->_global)
			entity_manager->_global->Call((char*)"Load", (char*)"");
		lux::display->Loop( LOADING );
	}


	/* File Name */
	this->file_name = lux::game->public_directory;
	if ( this->save_game_slot == 0x00 )
	{
		this->save_type = LUX_SAVE_HIBERNATE_TYPE;
		this->file_name.append("0.save");
	}
	else if ( this->save_game_slot != 0xFF )
	{
		this->file_name.append( elix::string::FromInt( this->save_game_slot ) );
		this->file_name.append(".save");
	}
	else
	{
		return false;
	}

	/* Open File */
	lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << "Restoring " << this->file_name << std::endl;

	this->save_file = new elix::File( this->file_name, false );
	if ( !this->save_file->Exist() )
	{
		lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR) << "===========================Loading Failed========================" << std::endl;
		delete this->save_file;
		return false;
	}
	else
	{
		uint8_t allow_access = 0;
		uint8_t file_mime_magic[7];

		if ( !lux::game->id )
			return false;

		this->save_file->ErrorCallback = Lux_LoadFileFailed;

		if ( !this->save_file->Read( &file_mime_magic, sizeof(uint8_t), 7 ) )
		{
			delete this->save_file;
			return false;
		}

		if ( memcmp( &save_magic_mime, &file_mime_magic, 7 ) )
		{
			delete this->save_file;
			return false;
		}


		this->save_type = this->save_file->Read_uint8();


		this->save_file->Seek( this->save_file->Tell() + 1 );


		allow_access = this->save_file->Read_uint8();

		return true;
	}
}






bool LuxSaveState::LoadHibernateFile( WorldSystem * new_world, EntityManager * new_entity_manager )
{
	bool restoring_results = false;

	new_entity_manager->SetSaveMode( LUX_SAVE_HIBERNATE_TYPE );
	restoring_results = new_world->Restore( this->save_file );

	delete this->save_file;
	return restoring_results;
}

bool LuxSaveState::LoadDataFile( WorldSystem * new_world, EntityManager * new_entity_manager, int32_t * info, uint32_t length  )
{
	bool restoring_results = false;
	for (uint8_t count = 0; count < length; count++)
	{
		info[count] = this->save_file->Read_uint32( true );
	}

	restoring_results = new_world->Restore( this->save_file );

	delete this->save_file;
	return restoring_results;
}

bool LuxSaveState::LoadCookieFile(int32_t *info, uint32_t length)
{
	for (uint8_t count = 0; count < length; count++)
	{
		info[count] = this->save_file->Read_uint32( true );
	}
	delete this->save_file;
	return true;
}



bool LuxSaveState::Restore( WorldSystem * new_world, EntityManager * new_entity_manager, int32_t * info, uint32_t length  )
{
	if ( !this->PreLoad(lux::entities) )
	{
		return false;
	}

	lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR) << "================ Load ===============" << std::endl;
	lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR) << "============= Type: "  << +this->save_type << " =============" << std::endl;

	lux::entities->SetSaveMode( this->save_type );

	if ( this->save_type == LUX_SAVE_COOKIE_TYPE )
	{
		return this->LoadCookieFile( info, length );
	}
	else if ( this->save_type == LUX_SAVE_DATA_TYPE )
	{
		return this->LoadDataFile( new_world, new_entity_manager, info, length );
	}
	else if ( this->save_type == LUX_SAVE_HIBERNATE_TYPE )
	{
		return this->LoadHibernateFile( new_world, new_entity_manager );
	}
	else
	{
		return false;
	}

	return false;
}

