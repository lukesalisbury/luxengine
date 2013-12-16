/****************************
Copyright © 2006-2011 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef _ENGINE_H_
#define _ENGINE_H_

#include "lux_types.h"
#include "player.h"
#include "elix_file.hpp"
#include "platform_media.h"
#include <vector>
#include <string>

namespace colour {
	extern LuxColour white;
	extern LuxColour black;
	extern LuxColour red;
	extern LuxColour blue;
	extern LuxColour green;
	extern LuxColour yellow;
}
class LuxEngine
{
	public:
		~LuxEngine();
		LuxEngine( std::string executable = "/");

		bool ShowDialog(std::string text, LuxWidget dialog = DIALOGYESNO, std::string * answer = NULL);
		void FatalError(std::string reason);
		void SettingDialog( );

		bool Start( std::string project_file );
		bool Start();
		void Refresh();
		void Loop();
		void RunState();
		void Close();

		/* Engine State */
		LuxState state;
		int32_t game_state;
		void SetState(LuxState new_state);
		int32_t GameState(int32_t new_state);

		/* Players */
		void AddPlayer(Player * player);
		void RemovePlayer(Player * player);

		Player * GetPlayer(uint32_t player);
		void SetDefaultPlayer( uint32_t player );
		int16_t GetPlayerAxis(uint32_t player, uint8_t axis);
		int16_t GetPlayerButton(uint32_t player, uint8_t key);
		int16_t GetPlayerPointer(uint32_t player, uint8_t axis);
		void SetPlayerAxis(uint32_t player, int16_t value, uint8_t axis);
		void SetPlayerButton(uint32_t player, int16_t value, uint8_t key);
		void SetPlayerPointer(uint32_t player, int16_t value, uint8_t axis);

		/* Save System */
		bool RestoreSaveGame( uint8_t slot );
		bool WriteSaveGame( uint8_t slot, int32_t * info, uint32_t length );
		bool ReadSaveInfo( uint8_t slot, int32_t * info, uint32_t length, uint32_t project_id );

		/* Language System */
		bool LoadDefaultLanguage();
		bool LoadLanguage(std::string lang);
		std::string GetDialogString(uint32_t line);
		std::string GetString(uint32_t line);

	private:
		/* Players */
		uint32_t default_player;
		std::vector<Player*> _players;

		/* Save System */
		uint8_t save_system_slot;
		int32_t * save_system_info;
		uint32_t save_system_length;

		bool HandleSave();
		bool HandleLoad();
		bool HandleReload();

		/* Language System */
		std::vector<std::string> _dialog;
		std::vector<std::string> _strings;
		int32_t load_screen;

		/* message dialog */
		int32_t msgdialog;
		std::string * msgdialog_return;
		bool CreateDialog(std::string text, LuxWidget dialog, std::string * answer = NULL );
		bool LoopDialog();
		bool EndDialog( LuxWidget dialog = DIALOGYESNO );
		LuxState msgstate;

};

namespace lux
{
	extern LuxEngine * engine;
	extern PlatformMedia * media;
}
#endif /* _ENGINE_H_ */
