/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include "engine.h"
#include "core.h"
#include "gui/gui.h"
#include "game_config.h"
#include "audio.h"
#include "display/display.h"
#include "game_system.h"
#include "entity_manager.h"
#include "misc_functions.h"
#include "mokoi_game.h"
#include "elix/elix_path.hpp"
#include "elix/elix_program.hpp"
#include <algorithm>
#include "save_system.h"

namespace colour {
	LuxColour white = {255, 255, 255, 255};
	LuxColour black = {0, 0, 0, 255};
	LuxColour red = {255, 0, 0, 255};
	LuxColour green = {0, 255, 0, 255};
	LuxColour blue = {0, 0, 255, 255};
	LuxColour yellow = {255, 255, 0, 255};
	LuxColour grey = {178,178,178,255};
}

namespace lux {
	LuxEngine * engine = NULL;
	CoreSystem * core = NULL;
	UserInterface * gui = NULL;
	Config * global_config = NULL;
	GameConfig * config = NULL;
	AudioSystem * audio = NULL;
	DisplaySystem * display = NULL;
	GameSystem * game_system = NULL;
	EntityManager * entities = NULL;
	MokoiGame * game_data = NULL;
	EntitySystem * entity_system = NULL;
	PlatformMedia * media = NULL;

	GameSystem * oldgame = NULL;
	namespace screen {
		void display( std::string message );
	}
}

/* Engine Class */
LuxEngine::LuxEngine( const void * window_ptr )
{
	#ifdef SDL2
	elix::program::Set( PROGRAM_NAME, PROGRAM_VERSION_STABLE, PROGRAM_DOCUMENTS, "/" );

	this->default_player = 1;
	this->game_state = 0;
	this->game_fps = 0;
	this->game_fps_time = 0;
	this->game_fps_last = 0;

	lux::core = new CoreSystem( window_ptr );
	lux::global_config = new Config();

	if ( lux::core->Good() )
	{
		this->state = IDLE;
	}
	else
	{
		this->FatalError("Couldn't init Core.");
		this->state = GAMEERROR;
	}
	#endif
}


LuxEngine::LuxEngine( std::string executable )
{
	elix::program::Set( PROGRAM_NAME, PROGRAM_VERSION_STABLE, PROGRAM_DOCUMENTS, executable );

	std::string base_directory = "/";

	this->default_player = 1;
	this->game_state = 0;
	this->game_fps = 0;
	this->game_fps_time = 0;
	this->game_fps_last = 0;


	lux::core = new CoreSystem();
	lux::global_config = new Config();

	lux::core->SystemMessage( SYSTEM_MESSAGE_LOG, PROGRAM_NAME" - Version "PROGRAM_VERSION" Log" );
	lux::core->SystemMessage( SYSTEM_MESSAGE_LOG, "-------------------------------------" );

	lux::core->SystemMessage( SYSTEM_MESSAGE_ERROR, PROGRAM_NAME" - Version "PROGRAM_VERSION" Error Log" );
	lux::core->SystemMessage( SYSTEM_MESSAGE_ERROR, "-------------------------------------" );

	if ( executable[0] != 0 )
	{
		base_directory = elix::path::GetBase( executable, true );

		lux::core->SystemMessage( SYSTEM_MESSAGE_LOG, "Program Location: " + executable );
	}

	lux::core->SystemMessage( SYSTEM_MESSAGE_LOG, "Base Directory: " + base_directory );
	lux::core->SystemMessage( SYSTEM_MESSAGE_LOG, "-------------------------------------" );
	lux::core->SystemMessage( SYSTEM_MESSAGE_LOG, "User: " + elix::directory::User("test", false, "test") );
	lux::core->SystemMessage( SYSTEM_MESSAGE_LOG, "User Documents: " + elix::directory::Documents( false ) );
	lux::core->SystemMessage( SYSTEM_MESSAGE_LOG, "\t\t" + elix::directory::Documents( false, "test.txt") );
	lux::core->SystemMessage( SYSTEM_MESSAGE_LOG, "Global Documents: " + elix::directory::Documents( true) );
	lux::core->SystemMessage( SYSTEM_MESSAGE_LOG, "\t\t" + elix::directory::Documents( true, "test.txt") );
	lux::core->SystemMessage( SYSTEM_MESSAGE_LOG, "Cache: " + elix::directory::Cache("")  );
	lux::core->SystemMessage( SYSTEM_MESSAGE_LOG, "Resources: " + elix::directory::Resources("") );
	lux::core->SystemMessage( SYSTEM_MESSAGE_LOG, "\t\t" + elix::directory::Resources("examples") );
	lux::core->SystemMessage( SYSTEM_MESSAGE_LOG, "\t\t" + elix::directory::Resources("examples", "readme.txt") );
	lux::core->SystemMessage( SYSTEM_MESSAGE_LOG, "-------------------------------------" );

	if ( lux::core->Good() )
	{
		this->state = IDLE;
	}
	else
	{
		this->FatalError("Couldn't init Core.");
		this->state = GAMEERROR;
	}

}

LuxEngine::~LuxEngine()
{
	this->Close();

	NULLIFY(lux::global_config);
	NULLIFY(lux::media);
	NULLIFY(lux::core);
}

void LuxEngine::Close()
{
	lux::core->SystemMessage(SYSTEM_MESSAGE_LOG) << "<-----------------LuxEngine::Close-----------------|" << std::endl;
	lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR) << "<-----------------LuxEngine::Close-----------------|" << std::endl;

	if ( lux::game_system )
	{
		lux::game_system->Close();
	}

	NULLIFY(lux::entities);
	NULLIFY(lux::game_system);
	NULLIFY(lux::audio);
	NULLIFY(lux::gui);
	NULLIFY(lux::display);
	NULLIFY(lux::entity_system);
	NULLIFY(lux::config);
	NULLIFY(lux::game_data);

	this->state = IDLE;

}

bool LuxEngine::Start()
{
	if ( this->state == GAMEERROR )
		return false;

	return this->Start( lux::global_config->GetString("project.file") );
}

/**
 * @brief LuxEngine::Start
 * @param project_file
 * @return
 */
bool LuxEngine::Start( std::string project_file )
{
	if ( this->state == GAMEERROR )
		return false;

	this->state = RUNNING;
	this->next_state = NOTRUNNING;
	lux::core->SystemMessage(SYSTEM_MESSAGE_LOG, ">----------------LuxEngine::Start-------------------|");
	lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR, ">----------------LuxEngine::Start-------------------|");

	/* Strip quotes from name if they have it. */
	if ( project_file.at(0) == '\'' )
	{
		project_file = project_file.substr(1, project_file.length() -2);
		lux::global_config->SetString( "project.file", project_file );
	}

	lux::core->SystemMessage(SYSTEM_MESSAGE_LOG) << "Loading Game: " << project_file << std::endl;

	lux::entity_system = new EntitySystem();
	lux::game_data = new MokoiGame( project_file );
	lux::config = new GameConfig();

	if ( lux::game_data->valid )
	{
		lux::display = new DisplaySystem();
		lux::display->graphics.PreShow(GRAPHICS_SCREEN_FRAME);
		lux::display->SetBackgroundColour( colour::grey );
		lux::display->graphics.PostShow(GRAPHICS_SCREEN_FRAME);


//		lux::engine->ShowDialog("DIALOGOK", DIALOGOK, NULL );
//		lux::engine->ShowDialog("DIALOGYESNO", DIALOGYESNO, NULL );
//		lux::engine->ShowDialog("DIALOGYESNOCANCEL", DIALOGYESNOCANCEL, NULL );

//		lux::engine->ShowDialog("DIALOGTEXT", DIALOGTEXT, NULL );
//		return false;


		if ( this->state == GAMEERROR )
			return false;


		if ( lux::config->Has("package.main") )
		{
			std::string package_main_name = lux::config->GetString("package.main");
			lux::screen::display("Loading Content Package");
			if ( !lux::game_data->LoadPackage( package_main_name ) )
			{
				this->ShowDialog("Can not find " + package_main_name + "\n"PACKAGE_GET_URL + package_main_name + "\nExiting game.", DIALOGOK);
				this->state = GAMEERROR;
				return false;
			}
		}

		if ( lux::config->Has("package.music") )
		{
			std::string package_music_name = lux::config->GetString("package.music");
			lux::screen::display("Loading Content Package");
			if ( !lux::game_data->LoadPackage( package_music_name ) )
			{
				this->ShowDialog("Can not find " + package_music_name + "\n"PACKAGE_GET_URL + package_music_name + "\nExiting game.", DIALOGOK);
				this->state = GAMEERROR;
				return false;
			}
		}

		if ( lux::config->Has("package.sounds") )
		{
			std::string package_music_name = lux::config->GetString("package.sounds");
			lux::screen::display("Loading Content Package");
			if ( !lux::game_data->LoadPackage( package_music_name ) )
			{
				this->ShowDialog("Can not find " + package_music_name + "\n"PACKAGE_GET_URL + package_music_name + "\nExiting game.", DIALOGOK);
				this->state = GAMEERROR;
				return false;
			}
		}

		if ( lux::config->Has("package.graphics") )
		{
			std::string package_music_name = lux::config->GetString("package.graphics");
			lux::screen::display("Loading Content Package");
			if ( !lux::game_data->LoadPackage( package_music_name ) )
			{
				this->ShowDialog("Can not find " + package_music_name + "\n"PACKAGE_GET_URL + package_music_name + "\nExiting game.", DIALOGOK);
				this->state = GAMEERROR;
				return false;
			}
		}

		if ( lux::config->GetBoolean("patches.scan") )
		{
			lux::screen::display("Loading Patches");
			lux::game_data->LoadPatches( );
		}
		else if ( lux::config->GetBoolean("patches.listed") )
		{
			lux::screen::display("Loading Patches");
			std::vector<std::string> patch_list = lux::config->GetArray("patches.list");
			for( uint8_t i = 0; i < patch_list.size(); i++ )
			{
				lux::core->SystemMessage(SYSTEM_MESSAGE_LOG) << "Patch " << patch_list[i] << " " << lux::game_data->AddPackage( patch_list[i] ) << std::endl;
			}
		}
	}
	else
	{
		this->FatalError("Invalid Game: " + project_file);
		return false;
	}

	if ( !this->LoadDefaultLanguage() )
	{
		this->FatalError("Failed loading Language");
		return false;
	}

	if ( !lux::game_data->HasFile( lux::entity_system->GetMainScriptFile() ) || lux::config->GetBoolean("entities.rebuild") )
	{
		lux::screen::display("Rebuilding Entities");
		lux::game_data->CompileScripts();
	}

	this->default_player = 1;
	uint32_t players = lux::config->GetNumber("player.number");
	if ( !players )
	{
		this->FatalError("Game must have a player");
		return false;
	}
	else
	{
		for (uint32_t i = 0; i < players; i++)
		{
			Player * initPlayer =  new Player(i+1);
			this->_players.push_back(initPlayer);
		}
	}

	lux::audio = new AudioSystem();
	lux::entities = new EntityManager();
	lux::game_system = new GameSystem();

	lux::game_system->Init();

	// Networked Game
	if ( lux::config->GetBoolean("server.able") )
	{
		lux::screen::display("Connecting to Server");
		if ( !lux::core->InitSubSystem(LUX_INIT_NETWORK) )
		{
			if ( this->ShowDialog("Network Server connection failed.\nDo you want to exit?") )
			{
				this->state = GAMEERROR;
			}
		}
	}

	return (this->state == GAMEERROR ? false : true);
}

void LuxEngine::UpdateFPS()
{
	if ( lux::core->GetTime() > (this->game_fps_time + 1000) )
	{
		this->game_fps_last = this->game_fps;
		this->game_fps_time = lux::core->GetTime();
		this->game_fps = 0;
	}
	this->game_fps++;
}

/**
 * @brief LuxEngine::Refresh
 */
void LuxEngine::Refresh()
{
	if ( this->state > NOTRUNNING )
	{
		this->state = lux::core->HandleFrame( this->state );

		if ( this->state == RUNNING )
		{
			this->UpdateFPS();
			lux::core->SystemMessage(SYSTEM_MESSAGE_DEBUG) << "FPS: " << (this->game_fps_last < 30 ? "§c" : "§a") << this->game_fps_last << std::endl;
		}
		else
		{
			//lux::core->SystemMessage(SYSTEM_MESSAGE_DEBUG) << "State: §a" << this->state << " §c" << lux::core->GetTime() << std::endl;
		}

		if ( this->state == PAUSED )
		{
			this->UpdateFPS();
			lux::display->Display(this->state);
			lux::core->Idle();
		}
		else if ( this->state == NOUPDATE )
		{
			this->UpdateFPS();
			lux::display->Display(this->state);
			lux::core->Idle();
		}
		else if ( this->state == INVALIDPOSITION )
		{
			lux::core->Idle();
		}
		else if ( this->state == RUNNING )
		{
			this->RunState();
		}
		else if ( this->state == SAVING )
		{
			if ( !this->HandleSave() )
			{
				this->ShowDialog("Saving Error. Exiting", DIALOGOK);
				this->state = NOTRUNNING;
			}
			else
				this->state = RUNNING;
		}
		else if ( this->state == LOADING )
		{
			if ( !this->HandleLoad() )
			{
				this->ShowDialog("Loaded Error. Exiting", DIALOGOK);
				this->state = NOTRUNNING;
			}
			else
				this->state = RUNNING;

		}
		else if ( this->state == RESETGAME )
		{
			if ( !this->HandleReload() )
			{
				this->ShowDialog("Loaded Error. Exiting", DIALOGOK);
				this->state = NOTRUNNING;
			}
			else
				this->state = RUNNING;

		}
		else if ( this->state == GUIMODE )
		{
			if ( lux::gui )
			{
				if ( this->LoopDialog() )
				{
					this->EndDialog();
					this->state = GAMEERROR;
				}
			}
		}
		else if ( this->state == EXITING )
		{
			if ( this->ShowDialog("Do you want to exit?") )
				this->state = NOTRUNNING;
			else
				this->state = RUNNING;
		}
		else if ( this->state == RELOADENTITIES )
		{
			// Quick Hack
			lux::game_system->GetEntities()->ReloadEntities();
			lux::game_system->active_map->entities->ReloadEntities();
			this->state = RUNNING;
		}
	}
}

/**
 * @brief LuxEngine::Loop
 */
void LuxEngine::Loop()
{
	while ( this->state > NOTRUNNING )
	{
		this->Refresh();
		if ( this->next_state != NOTRUNNING )
		{
			this->state = this->next_state;
			this->next_state = NOTRUNNING;
		}
	}
	this->Close();
}

/**
 * @brief LuxEngine::SetState
 * @param new_state
 */
void LuxEngine::SetState(LuxState new_state)
{
	#ifdef NETWORKENABLED
	lux::core->NetworkLock();
	#endif
	this->state = new_state;
	#ifdef NETWORKENABLED
	lux::core->NetworkUnlock();
	#endif
}

/**
 * @brief LuxEngine::RunState
 */
void LuxEngine::RunState()
{
	/* Game Loop
		Runs Player(s)
		World (Checks for Map Change)
		Entities
		Display ()
	*/


	lux::core->NetworkLock();
	std::vector<Player *>::iterator iter = _players.begin();
	while( iter !=  _players.end() )
	{
		(*iter)->Loop();
		iter++;
	}
	lux::core->NetworkUnlock();
	TIMER_START("game_system->Loop");
	lux::game_system->Loop( this->state );
	TIMER_END("game_system->Loop");
	lux::audio->Loop( this->state );
	lux::display->Loop( this->state );


}

/* Players */

/**
 * @brief LuxEngine::AddPlayer
 * @param player
 */
void LuxEngine::AddPlayer(Player * player)
{
	this->_players.push_back(player);
}

/**
 * @brief LuxEngine::RemovePlayer
 * @param player
 */
void LuxEngine::RemovePlayer(Player * player)
{
	std::vector<Player*>::iterator iter = std::find(this->_players.begin(), this->_players.end(), player);
	if ( iter != this->_players.end() )
		this->_players.erase(iter);
}

/**
 * @brief LuxEngine::SetDefaultPlayer
 * @param player
 */
void LuxEngine::SetDefaultPlayer( uint32_t player )
{
	if ( player )
	{
		this->default_player = player;
	}
}

/**
 * @brief LuxEngine::GetPlayer
 * @param player
 * @return
 */
Player * LuxEngine::GetPlayer( uint32_t player )
{
	uint32_t player_position = ( player == 0 ? this->default_player : player ) -1;
	if ( player_position < this->_players.size() )
	{
		return this->_players[player_position];
	}
	return NULL;
}

/**
 * @brief LuxEngine::GetPlayerAxis
 * @param player_id
 * @param axis
 * @return
 */
int16_t LuxEngine::GetPlayerAxis(uint32_t player_id, uint8_t axis)
{
	Player * player = this->GetPlayer( player_id );
	if ( player )
	{
		return player->GetControllerAxis(axis);
	}
	return 0;
}

/**
 * @brief LuxEngine::GetPlayerButton
 * @param player_id
 * @param key
 * @return
 */
int16_t LuxEngine::GetPlayerButton(uint32_t player_id, uint8_t key)
{
	Player * player = this->GetPlayer( player_id );
	if ( player )
	{
		return player->GetButton(key);
	}
	return 0;
}
/**
 * @brief LuxEngine::GetPlayerPointer
 * @param player_id
 * @param axis
 * @return
 */
int16_t LuxEngine::GetPlayerPointer(uint32_t player_id, uint8_t axis)
{
	Player * player = this->GetPlayer( player_id );
	if ( player )
	{
		return player->GetPointer(axis);
	}
	return 0;
}
/**
 * @brief LuxEngine::SetPlayerAxis
 * @param player_id
 * @param value
 * @param axis
 */
void LuxEngine::SetPlayerAxis(uint32_t player_id, int16_t value, uint8_t axis)
{
	Player * player = this->GetPlayer( player_id );
	if ( player )
	{
		player->SetControllerAxis(axis, value);
	}
}
/**
 * @brief LuxEngine::SetPlayerButton
 * @param player_id
 * @param value
 * @param key
 */
void LuxEngine::SetPlayerButton(uint32_t player_id, int16_t value, uint8_t key)
{
	Player * player = this->GetPlayer( player_id );
	if ( player )
	{
		player->SetButton(key, value);
	}

}
/**
 * @brief LuxEngine::SetPlayerPointer
 * @param player_id
 * @param value
 * @param axis
 */
void LuxEngine::SetPlayerPointer(uint32_t player_id, int16_t value, uint8_t axis)
{
	Player * player = this->GetPlayer( player_id );
	if ( player )
	{
		player->SetPointer(axis, value);
	}
}

/**
 * @brief LuxEngine::GameState
 * @param new_state
 * @return
 */
int32_t LuxEngine::GameState(int32_t new_state)
{
	if ( new_state != -1 )
	{
		this->game_state = new_state;
	}
	return this->game_state;
}


/* Save System */
/**
 * @brief LuxEngine::HandleSave
 * @return
 */
bool LuxEngine::HandleSave()
{
	LuxSaveState saved_game;

	saved_game.SetSlot( this->save_system_slot );

	if ( saved_game.Save( lux::game_system, lux::entities, NULL, 0 ) )
	{
		lux::core->SystemMessage(SYSTEM_MESSAGE_LOG) << "game saved" << std::endl;
		return true;
	}

	this->state = RUNNING;
	this->save_system_slot = 0xFF;
	return false;

}

/**
 * @brief LuxEngine::HandleReload
 * @return
 */
bool LuxEngine::HandleReload()
{
	GameSystem * restored_world = new GameSystem();
	GameSystem * current_world = lux::game_system;

	EntityManager * restored_entity_manager = new EntityManager();
	EntityManager * current_entity_manager = lux::entities;

	lux::game_system = restored_world;
	lux::entities = restored_entity_manager;

	delete current_world;
	delete current_entity_manager;

	this->state = PAUSED;

	return true;
}

/**
 * @brief LuxEngine::HandleLoad
 * @return
 */
bool LuxEngine::HandleLoad()
{
	bool successful = false;

	GameSystem * restored_world = new GameSystem();
	EntityManager * restored_entity_manager = new EntityManager();

	GameSystem * current_world = lux::game_system;
	EntityManager * current_entity_manager = lux::entities;

	LuxSaveState saved_game;

	saved_game.SetSlot( this->save_system_slot );

	/* Map new varibles to global varibles */
	lux::game_system = restored_world;
	lux::entities = restored_entity_manager;

	if ( saved_game.Restore( restored_world, restored_entity_manager, NULL, 0 ) )
	{
		lux::core->SystemMessage(SYSTEM_MESSAGE_LOG) << "Game restored" << std::endl;
		lux::core->SystemMessage(SYSTEM_MESSAGE_LOG) << "removing old world  & enitites" << std::endl;

		lux::game_system = current_world;
		lux::entities = current_entity_manager;

		lux::oldgame = current_world;


		//current_world->Close();

		//TODO: (Either merge or check if GameSystem or EntityManager free something it shouldn't

		//delete current_world;
		//delete current_entity_manager;


		lux::game_system = restored_world;
		lux::entities = restored_entity_manager;

		successful = true;
	}
	else
	{
		lux::core->SystemMessage(SYSTEM_MESSAGE_LOG) << "removing failed world & enitites" << std::endl;

		lux::game_system = current_world;
		lux::entities = current_entity_manager;

		delete restored_world;
		delete restored_entity_manager;
	}

	this->state = RUNNING;
	this->save_system_slot = 0xFF;

	return successful;
}

/**
 * @brief LuxEngine::RestoreSaveGame
 * @param slot
 * @return
 */
bool LuxEngine::RestoreSaveGame( uint8_t slot )
{
	this->save_system_slot = slot;

	return false;
}
/**
 * @brief LuxEngine::WriteSaveGame
 * @param slot
 * @param info
 * @param length
 * @return
 */
bool LuxEngine::WriteSaveGame( uint8_t slot, int32_t * info, uint32_t length )
{
	LuxSaveState saved_game;
	saved_game.SetInformation( lux::game_data->ident, LUX_SAVE_COOKIE_TYPE, "", NULL );
	saved_game.SetSlot( slot );
	saved_game.Save( lux::game_system, lux::entities, info, length );

	return true;
}
/**
 * @brief LuxEngine::ReadSaveInfo
 * @param slot
 * @param info
 * @param length
 * @param project_id
 * @return
 */
bool LuxEngine::ReadSaveInfo(uint8_t slot, int32_t * info, uint32_t length, uint32_t project_id )
{
	LuxSaveState saved_game;

	saved_game.SetInformation( project_id, LUX_SAVE_COOKIE_TYPE, "", NULL );
	saved_game.SetSlot( slot );

	if ( saved_game.Restore( NULL, NULL, info, length ) )
	{
		return true;
	}

	return false;
}
