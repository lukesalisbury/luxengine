/****************************
Copyright © 2006-2014 Luke Salisbury
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
#include "misc_functions.h"
#include "mokoi_game.h"
#include "elix_path.hpp"
#include "elix_program.hpp"
#include <algorithm>
#include "save_system.h"

namespace colour {
	LuxColour white = {255, 255, 255, 255};
	LuxColour black = {0, 0, 0, 255};
	LuxColour red = {255, 0, 0, 255};
	LuxColour green = {0, 255, 0, 255};
	LuxColour blue = {0, 0, 255, 255};
	LuxColour yellow = {255, 255, 0, 255};
}

namespace lux {
	LuxEngine * engine = NULL;
	CoreSystem * core = NULL;
	UserInterface * gui = NULL;
	Config * global_config = NULL;
	GameConfig * config = NULL;
	AudioSystem * audio = NULL;
	DisplaySystem * display = NULL;
	GameWorldSystem * gameworld = NULL;
	EntityManager * entities = NULL;
	MokoiGame * game_data = NULL;
	EntitySystem * entity_system = NULL;
	PlatformMedia * media = NULL;

	namespace screen {
		void display( std::string message );
	}
}

/* Engine Class */
LuxEngine::LuxEngine( std::string executable )
{
	elix::program::Set( PROGRAM_NAME, PROGRAM_VERSION_STABLE, PROGRAM_DOCUMENTS, executable );

	std::string base_directory = "/";

	this->game_state = 0;
	this->default_player = 1;
	this->game_fps = 0;
	this->game_fps_time = 0;
	this->game_fps_last = 0;


	lux::core = new CoreSystem();
	lux::global_config = new Config();
	lux::media = new PlatformMedia();

	lux::core->SystemMessage( SYSTEM_MESSAGE_INFO, PROGRAM_NAME" - Version "PROGRAM_VERSION" Log" );
	lux::core->SystemMessage( SYSTEM_MESSAGE_INFO, "-------------------------------------" );

	lux::core->SystemMessage( SYSTEM_MESSAGE_ERROR, PROGRAM_NAME" - Version "PROGRAM_VERSION" Error Log" );
	lux::core->SystemMessage( SYSTEM_MESSAGE_ERROR, "-------------------------------------" );

	if ( executable[0] != 0 )
	{
		base_directory = elix::path::GetBase( executable, true );

		lux::core->SystemMessage( SYSTEM_MESSAGE_INFO, "Program Location: " + executable );
	}

	lux::core->SystemMessage( SYSTEM_MESSAGE_INFO, "Base Directory: " + base_directory );
	lux::core->SystemMessage( SYSTEM_MESSAGE_INFO, "-------------------------------------" );
	lux::core->SystemMessage( SYSTEM_MESSAGE_INFO, "User: " + elix::directory::User("test", false, "test") );
	lux::core->SystemMessage( SYSTEM_MESSAGE_INFO, "User Documents: " + elix::directory::Documents( false ) );
	lux::core->SystemMessage( SYSTEM_MESSAGE_INFO, "Global Documents: " + elix::directory::Documents( true, "test.txt") );
	lux::core->SystemMessage( SYSTEM_MESSAGE_INFO, "Cache: " + elix::directory::Cache("")  );
	lux::core->SystemMessage( SYSTEM_MESSAGE_INFO, "Resources: " + elix::directory::Resources("") );
	lux::core->SystemMessage( SYSTEM_MESSAGE_INFO, "examples: " + elix::directory::Resources("examples") );
	lux::core->SystemMessage( SYSTEM_MESSAGE_INFO, "-------------------------------------" );

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
	lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << "<-----------------LuxEngine::Close-----------------|" << std::endl;
	lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR) << "<-----------------LuxEngine::Close-----------------|" << std::endl;

	if ( lux::gameworld )
	{
		lux::gameworld->Close();
	}

	NULLIFY(lux::entities);
	NULLIFY(lux::gameworld);
	NULLIFY(lux::audio);
	NULLIFY(lux::display);
	NULLIFY(lux::gui);
	NULLIFY(lux::entity_system);
	NULLIFY(lux::config);
	NULLIFY(lux::game_data);

	this->state = IDLE;

}

bool LuxEngine::Start()
{
	if ( this->state == GAMEERROR )
	{
		return false;
	}
	return this->Start( lux::global_config->GetString("project.file") );
}

bool LuxEngine::Start( std::string project_file )
{
	if ( this->state == GAMEERROR )
		return false;

	this->state = RUNNING;


	lux::core->SystemMessage(SYSTEM_MESSAGE_INFO, ">----------------LuxEngine::Start-------------------|");
	lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR, ">----------------LuxEngine::Start-------------------|");

	/* Strip quotes from name if they have it. */
	if ( project_file.at(0) == '\'' )
	{
		project_file = project_file.substr(1, project_file.length() -2);
		lux::global_config->SetString( "project.file", project_file );
	}

	lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << "Loading Game: " << project_file << std::endl;

	lux::entity_system = new EntitySystem();
	lux::game_data = new MokoiGame(project_file, false);
	lux::config = new GameConfig();

	if ( lux::game_data->valid )
	{

		lux::display = new DisplaySystem();
		lux::display->SetBackgroundColour((LuxColour){178,178,178,255});
		lux::display->graphics.Show();

		lux::display->Init();

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
				lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << "Patch " << patch_list[i] << " " << lux::game_data->AddPackage( patch_list[i] ) << std::endl;
			}
		}
	}
	else
	{
		lux::display = new DisplaySystem();
		lux::display->SetBackgroundColour((LuxColour){178,178,178,255});
		lux::display->graphics.Show();
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
	lux::gameworld = new GameWorldSystem();

	lux::gameworld->Init();

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

void LuxEngine::Refresh()
{

	if ( this->state > NOTRUNNING )
	{
		this->state = lux::core->HandleFrame( this->state );

		if ( this->state == PAUSED )
		{
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
		else if ( this->state == NOUPDATE )
		{
			lux::screen::display("Syncing with Server. (Connection may have died)" );
			lux::display->Loop(this->state);
			lux::core->Idle();
		}
		else if ( this->state == SAVING )
		{
			if ( !this->HandleSave() )
			{
				this->ShowDialog("Saving Error. Exiting", DIALOGOK);
				this->state = NOTRUNNING;
			}
			this->state = RUNNING;
		}
		else if ( this->state == LOADING )
		{
			if ( !this->HandleLoad() )
			{
				this->ShowDialog("Loaded Error. Exiting", DIALOGOK);
				this->state = NOTRUNNING;
			}
			this->state = RUNNING;

		}
		else if ( this->state == RESETGAME )
		{
			if ( !this->HandleReload() )
			{
				this->ShowDialog("Loaded Error. Exiting", DIALOGOK);
				this->state = NOTRUNNING;
			}
			this->state = RUNNING;

		}
		else if ( this->state == GUIMODE )
		{
			lux::display->DrawGameStatic();
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
	}
}

void LuxEngine::Loop()
{
	while ( this->state > NOTRUNNING )
	{
		this->Refresh();
	}
	this->Close();
}

void LuxEngine::SetState(LuxState new_state)
{
	#ifdef NETWORKENABLED
	lux::core->NetLock();
	#endif
	this->state = new_state;
	#ifdef NETWORKENABLED
	lux::core->NetUnlock();
	#endif
}

void LuxEngine::RunState()
{
	/* Game Loop
		Runs Player(s)
		World (Checks for Map Change)
		Entities
		Display ()
	*/




	#ifdef NETWORKENABLED
	lux::core->NetLock();
	#endif
	std::vector<Player *>::iterator iter = _players.begin();
	while( iter !=  _players.end() )
	{
		(*iter)->Loop();
		iter++;
	}
	#ifdef NETWORKENABLED
	lux::core->NetUnlock();
	#endif


	lux::gameworld->Loop( this->state );
	lux::audio->Loop( this->state );
	lux::display->Loop( this->state );


	if ( lux::core->GetTime() > (this->game_fps_time + 1000) )
	{
		this->game_fps_last = this->game_fps;
		this->game_fps_time = lux::core->GetTime();
		this->game_fps = 0;
	}
	this->game_fps++;
	lux::display->debug_msg << "FPS: " << (this->game_fps_last < 30 ? "§c" : "§a") << this->game_fps_last << std::endl;


}

/* Players */

void LuxEngine::AddPlayer(Player * player)
{
	this->_players.push_back(player);
}

void LuxEngine::RemovePlayer(Player * player)
{
	std::vector<Player*>::iterator iter = std::find(this->_players.begin(), this->_players.end(), player);
	if ( iter != this->_players.end() )
		this->_players.erase(iter);
}

void LuxEngine::SetDefaultPlayer( uint32_t player )
{
	if ( player )
	{
		this->default_player = player;
	}
}

Player * LuxEngine::GetPlayer( uint32_t player )
{
	uint32_t player_position = ( player == 0 ? this->default_player : player ) -1;
	if ( player_position < this->_players.size() )
	{
		return this->_players[player_position];
	}
	return NULL;
}

int16_t LuxEngine::GetPlayerAxis(uint32_t player_id, uint8_t axis)
{
	Player * player = this->GetPlayer( player_id );
	if ( player )
	{
		return player->GetControllerAxis(axis);
	}
	return 0;
}

int16_t LuxEngine::GetPlayerButton(uint32_t player_id, uint8_t key)
{
	Player * player = this->GetPlayer( player_id );
	if ( player )
	{
		return player->GetButton(key);
	}
	return 0;
}

int16_t LuxEngine::GetPlayerPointer(uint32_t player_id, uint8_t axis)
{
	Player * player = this->GetPlayer( player_id );
	if ( player )
	{
		return player->GetPointer(axis);
	}
	return 0;
}

void LuxEngine::SetPlayerAxis(uint32_t player_id, int16_t value, uint8_t axis)
{
	Player * player = this->GetPlayer( player_id );
	if ( player )
	{
		player->SetControllerAxis(axis, value);
	}
}

void LuxEngine::SetPlayerButton(uint32_t player_id, int16_t value, uint8_t key)
{
	Player * player = this->GetPlayer( player_id );
	if ( player )
	{
		player->SetButton(key, value);
	}

}

void LuxEngine::SetPlayerPointer(uint32_t player_id, int16_t value, uint8_t axis)
{
	Player * player = this->GetPlayer( player_id );
	if ( player )
	{
		player->SetPointer(axis, value);
	}
}

int32_t LuxEngine::GameState(int32_t new_state)
{
	if ( new_state != -1 )
	{
		this->game_state = new_state;
	}
	return this->game_state;
}


/* Save System */
bool LuxEngine::HandleSave()
{
	LuxSaveState saved_game;

	saved_game.SetSlot( this->save_system_slot );


	if ( saved_game.Save( lux::gameworld, lux::entities, NULL, 0 ) )
	{
		lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << "game saved" << std::endl;
		return true;
	}

	this->state = RUNNING;
	this->save_system_slot = 0xFF;
	return false;

}

bool LuxEngine::HandleReload()
{

	GameWorldSystem * restored_world = new GameWorldSystem();
	GameWorldSystem * current_world = lux::gameworld;

	EntityManager * restored_entity_manager = new EntityManager();
	EntityManager * current_entity_manager = lux::entities;

	lux::gameworld = restored_world;
	lux::entities = restored_entity_manager;

	delete current_world;
	delete current_entity_manager;

	this->state = PAUSED;

	return true;


}

bool LuxEngine::HandleLoad()
{
	bool successful = false;
	GameWorldSystem * restored_world = new GameWorldSystem();
	GameWorldSystem * current_world = lux::gameworld;

	EntityManager * restored_entity_manager = new EntityManager();
	EntityManager * current_entity_manager = lux::entities;

	LuxSaveState saved_game;

	saved_game.SetSlot( this->save_system_slot );

	if ( saved_game.Restore( restored_world, restored_entity_manager, NULL, 0 ) )
	{
		lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << "game restored" << std::endl;

		lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << "removing old world " << std::hex << current_world << " & enitites" << current_entity_manager << std::endl;
		delete current_world;
		delete current_entity_manager;

		lux::gameworld = restored_world;
		lux::entities = restored_entity_manager;



		successful = true;
	}
	else
	{
		lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << "removing failed world " << std::hex << current_world << " & enitites" << current_entity_manager << std::endl;
		delete restored_world;
		delete restored_entity_manager;
	}

	restored_world = current_world = NULL;
	restored_entity_manager = current_entity_manager = NULL;


	this->state = RUNNING;
	this->save_system_slot = 0xFF;

	return successful;


}

bool LuxEngine::RestoreSaveGame( uint8_t slot )
{
	this->save_system_slot = slot;

	return false;
}

bool LuxEngine::WriteSaveGame( uint8_t slot, int32_t * info, uint32_t length )
{
	LuxSaveState saved_game;
	saved_game.SetInformation( lux::game_data->ident, LUX_SAVE_COOKIE_TYPE, "", NULL );
	saved_game.SetSlot( slot );
	saved_game.Save( lux::gameworld, lux::entities, info, length );

	return true;
}

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
