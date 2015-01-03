/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include <algorithm>

#include "core.h"
#include "game_config.h"
#include "engine.h"
#include "elix_string.hpp"
#include "elix_path.hpp"
#include "mokoi_game.h"
#include "tinyxml/tinyxml2ext.h"
#include "worker.h"
#include "game_info.h"
#include "portal.h"
#include "gui.h"
#include "downloader.h"
#include "project_media.h"


#define GUI_PORTAL_PLAY -2
#define GUI_PORTAL_ONLINE -10
#define GUI_PORTAL_BROWSE -11
#define GUI_PORTAL_RECENT -12
#define GUI_PORTAL_DEMOS -13
#define GUI_PORTAL_NEXTPAGE -20
#define GUI_PORTAL_PREVPAGE -21
#define GUI_PORTAL_BROWSE_AGAIN -14

#ifndef GUI_PORTAL_URL
	#define GUI_PORTAL_URL "mokoi.info/games/xml"
//	#define GUI_PORTAL_URL "localhost/xml"
#endif

namespace LuxPortal {
	UserInterface * interface = NULL;
	std::vector<GameInfo *> visible_games_list;
	std::vector<GameInfo *> complete_games_list;
	std::list<std::string> previous;

	bool use = true;
	bool active = true;

	int16_t mode = 1;
	LuxRect region = {0, 0, 512, 384, 0};
	uint16_t page_padding = 0;

	LuxSprite * logo = NULL;

	uint32_t offset = 0;
	uint32_t max_page_item = 10;

	std::string root_url = GUI_PORTAL_URL;

	std::string browse_path = "";
	std::string browse_path_default = "";

	GameInfoValues selected_game = { GI_INVALID, "" };

	int32_t display_page( LuxRect page_rect );
	void page_refresh( LuxRect page_rect, uint32_t start, uint32_t length );
	bool frame();
	void clear_complete_games_list();

	GameInfoValues get_selected()
	{
		return selected_game;
	}
	int32_t set_selected_game(int32_t return_value);

}

/**
 * @brief LuxPortal::add_previous_game
 * @param path
 */
void LuxPortal::add_previous_game( std::string path )
{
	if ( path.at(0) == '\'' && path.length() > 3 )
		path = path.substr(1, path.length() - 2);

	#ifdef __GNUWIN32__
	elix::string::Replace( path, "\\\\", "\\" );
	elix::string::Replace( path, "/", "\\" );
	#endif

	LuxPortal::previous.push_back(path);

	LuxPortal::previous.sort();
	LuxPortal::previous.unique();
}

/**
 * @brief LuxPortal::clear_game_vector
 * @param list
 */
void LuxPortal::clear_complete_games_list(  )
{
	if ( LuxPortal::complete_games_list.begin() != LuxPortal::complete_games_list.end() )
	{
		while ( LuxPortal::complete_games_list.begin() != LuxPortal::complete_games_list.end())
		{
			GameInfo * info = (*LuxPortal::visible_games_list.begin());
			delete info;

			LuxPortal::complete_games_list.erase( LuxPortal::complete_games_list.begin() );
		}
	}
}

/**
 * @brief LuxPortal::set_selected_game
 * @param return_value
 * @return
 */
int32_t LuxPortal::set_selected_game(int32_t return_value)
{
	GameInfo * selected = LuxPortal::complete_games_list.at( return_value - 1 );
	if ( selected )
	{
		if ( selected->type == GI_DIR )
		{
			LuxPortal::browse_path = selected->url;
			return_value = GUI_PORTAL_BROWSE_AGAIN;
		}
		else
		{
			selected_game.url.assign( selected->url );
			selected_game.type = selected->type;

			LuxPortal::add_previous_game( selected_game.url );

			return_value = GUI_PORTAL_PLAY;
		}
	}
	return return_value;
}

/**
 * @brief LuxPortal::page_refresh
 * @param page_rect
 * @param start
 * @param length
 * @param list
 */
void LuxPortal::page_refresh(LuxRect page_rect, uint32_t start, uint32_t length )
{
	/* Clear Old ones */
	if ( LuxPortal::visible_games_list.begin() != LuxPortal::visible_games_list.end() )
	{
		while ( LuxPortal::visible_games_list.begin() != LuxPortal::visible_games_list.end())
		{
			GameInfo * info = (*LuxPortal::visible_games_list.begin());
			info->ClearGUI();
			LuxPortal::visible_games_list.erase( LuxPortal::visible_games_list.begin() );
		}
	}

	if ( start > LuxPortal::complete_games_list.size() || start < 0 )
	{
		return;
	}

	/* */
	for( uint32_t i = start; i < start + length && i < LuxPortal::complete_games_list.size(); i++ )
	{
		GameInfo * info = LuxPortal::complete_games_list.at(i);
		if ( info->type != GI_INVALID )
		{
			// Add Button to GUI
			page_rect.y += info->SetGUI( i+1, page_rect.x, page_rect.y, page_rect.w, 40  );
			LuxPortal::visible_games_list.push_back( info );
		}
	}
}


/**
 * @brief LuxPortal::display_page
 * @param page_rect
 * @param games_list
 * @return
 */
int32_t LuxPortal::display_page( LuxRect page_rect )
{
	int32_t return_value = 0;
	uint32_t start = 0, length = LuxPortal::max_page_item;

	/* Create Button for On screen item */
	LuxPortal::page_refresh( page_rect, start, length );

	/* Show UI */
	while ( !return_value )
	{
		return_value = LuxPortal::interface->Loop();
		if ( return_value == GUI_PORTAL_NEXTPAGE )
		{
			if ( start + length < LuxPortal::complete_games_list.size() )
			{
				start += length;
				LuxPortal::page_refresh( page_rect, start, length );
			}
			return_value = 0;
		}
		else if ( return_value == GUI_PORTAL_PREVPAGE )
		{
			if (start >= length)
			{
				start -= length;
				LuxPortal::page_refresh( page_rect, start, length );
			}
			return_value = 0;
		}
	}

	if ( return_value > 0 )
	{
		return_value = LuxPortal::set_selected_game( return_value );
	}


	/* Clear GUI */
	LuxPortal::page_refresh( page_rect, -1, length );

	return return_value;
}

/**
 * @brief LuxPortal::recent_page
 * @param page_rect
 * @return
 */
int32_t LuxPortal::recent_page( LuxRect page_rect )
{
	int32_t return_value = 0;

	/* Create */
	std::list<std::string>::iterator iter = LuxPortal::previous.begin();
	while( iter != LuxPortal::previous.end() )
	{
		GameInfo * info = new GameInfo( LuxPortal::interface, *iter );
		if ( info->type != GI_INVALID )
		{
			LuxPortal::complete_games_list.push_back( info );
		}
		else
		{
			delete info;
		}
		iter++;
	}

	/* Display list */
	return_value = LuxPortal::display_page( page_rect );

	LuxPortal::clear_complete_games_list();

	return return_value;
}

/**
 * @brief LuxPortal::demos_page
 * @param page_rect
 * @return
 */
int32_t LuxPortal::demos_page( LuxRect page_rect )
{
	int32_t return_value = 0;

	/* Create */
	std::vector<std::string> list;
	std::string dir;

	#ifdef apple
	dir = "../../../MokoiGamingExamples";
	#else
	dir = elix::directory::Resources("examples");
	#endif

	elix::path::Children( dir, "", list, false, true, true );

	std::vector<std::string>::iterator iter = list.begin();
	while( iter != list.end() )
	{
		GameInfo * info = new GameInfo( LuxPortal::interface, *iter  );
		if ( info->type != GI_INVALID )
		{
			LuxPortal::complete_games_list.push_back( info );
		}
		else
		{
			delete info;
		}
		iter++;
	}
	list.clear();

	/* Display list */
	return_value = LuxPortal::display_page( page_rect );

	LuxPortal::clear_complete_games_list();

	return return_value;
}

/**
 * @brief LuxPortal::browse_page
 * @param page_rect
 * @return
 */
int32_t LuxPortal::browse_page( LuxRect page_rect )
{
	int32_t return_value = 0;
	uint32_t start = 0, length = LuxPortal::max_page_item;

	/* Read items */
	std::vector<std::string> list;

	elix::path::Children( LuxPortal::browse_path, "", list, false, true, true );

	std::vector<std::string>::iterator iter = list.begin();
	while( iter != list.end() )
	{
		GameInfo * info = new GameInfo( LuxPortal::interface, *iter );
		if ( info->type != GI_INVALID )
		{
			LuxPortal::complete_games_list.push_back( info );
		}
		else
		{
			delete info;
		}
		iter++;
	}
	list.clear();

	/* Display Page */
	return_value = LuxPortal::display_page( page_rect);

	LuxPortal::clear_complete_games_list();

	return return_value;
}

/**
 * @brief LuxPortal::online_page
 * @param page_rect
 * @return
 */
int32_t LuxPortal::online_page( LuxRect page_rect )
{
	tinyxml2::XMLDocument xml;
	int32_t return_value = 0;

	/**/
	Lux_Util_FileDownloaderBackground( LuxPortal::root_url + "/games.xml", elix::directory::Cache("portal.xml"),  LuxPortal::interface );
	xml.LoadFile( elix::directory::Cache("portal.xml").c_str() );

	if ( xml.Error() )
	{
		lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR) << "Portal Error:" << xml.GetErrorStr1() << std::endl;
		return GUI_PORTAL_RECENT;
	}

	tinyxml2::XMLElement * root = xml.RootElement();
	tinyxml2::XMLElement * child = root->FirstChildElement()->FirstChildElement();
	if ( child )
	{
		while( child )
		{
			GameInfo * info = new GameInfo( LuxPortal::interface, child );
			LuxPortal::complete_games_list.push_back( info );
			child = child->NextSiblingElement();
		}
	}


	/**/
	return_value = LuxPortal::display_page( page_rect );

	LuxPortal::clear_complete_games_list();

	return return_value;
}

/**
 * @brief LuxPortal::open
 */
void LuxPortal::open()
{
	std::string buffer;
	elix::File * gamelist = new elix::File( elix::directory::Cache("recent.txt") , false );
	if ( gamelist->Exist() )
	{
		while ( gamelist->ReadLine(&buffer) )
		{
			elix::string::Trim(&buffer);
			if ( !buffer.empty() )
			{
				LuxPortal::add_previous_game( buffer );
			}
			buffer.clear();
		}
	}
	delete gamelist;
}

/**
 * @brief LuxPortal::close
 */
void LuxPortal::close()
{
	LuxPortal::previous.sort();
	LuxPortal::previous.unique();

	/* save games.txt */
	elix::File * gamelist = new elix::File( elix::directory::Cache("recent.txt") , true );
	std::list<std::string>::iterator iter1 = LuxPortal::previous.begin();
	while ( iter1 != LuxPortal::previous.end() )
	{
		gamelist->WriteString( *iter1 );
		gamelist->Write((uint8_t)10);
		iter1++;
	}
	delete gamelist;


}

/**
 * @brief LuxPortal::frame
 * @return
 */
bool LuxPortal::frame()
{
	bool results = false;
	int32_t return_value = 0;
	Widget * footer_area, * button_exit, * button_browse, * button_online, * button_demos, * button_recent, * text_header, * button_next, * button_prev;
	Widget * logo_area, * page_area;

	LuxRect footer_rect = { 0, 0, 512, 24, 0 };
	LuxRect page_rect = { 64, 106, 384, 36, 0 };
	LuxRect item_rect = { 64, 106, 384, 36, 0 };
	LuxRect header_rect = { 64, 84, 197, 20, 0 };
	LuxRect image_rect = { 0, 0, 197, 84, 0 };

	footer_rect.w = LuxPortal::region.w;
	footer_rect.y = LuxPortal::region.h - 24;
	page_rect.w = header_rect.w = LuxPortal::region.w - 128;

	page_rect.y = lux_media::portal_title_height + 22;
	page_rect.h = LuxPortal::region.h - (lux_media::portal_title_height + 48);
	header_rect.y = lux_media::portal_title_height;

	image_rect.x = LuxPortal::page_padding;
	image_rect.w = lux_media::portal_title_width;
	image_rect.h = lux_media::portal_title_height;

	item_rect = page_rect;
	item_rect.x += LuxPortal::page_padding;
	item_rect.w -= LuxPortal::page_padding * 2;
	item_rect.y += LuxPortal::page_padding;
	item_rect.h = 40;

	if ( logo )
	{
		logo_area = LuxPortal::interface->AddChild( image_rect, IMAGE, "Image Missing" );
		logo_area->SetData( LuxPortal::logo );
	}
	else
	{
		image_rect.x = 0;
		logo_area = LuxPortal::interface->AddChild( image_rect, TEXT, PROGRAM_NAME );
	}

	footer_area = LuxPortal::interface->AddChild( footer_rect, EMPTYWINDOW, "" );
	page_area = LuxPortal::interface->AddChild( page_rect, BOX, "" );
	text_header = LuxPortal::interface->AddChild( header_rect, TEXT, PROGRAM_NAME );

	button_exit = LuxPortal::interface->AddWidgetChild( footer_area, -40, 0, 36, 16, BUTTON );
	button_exit->SetText("Exit", 4);
	button_exit->SetValue(GUI_EXIT);

	button_online = LuxPortal::interface->AddWidgetChild( footer_area, 4, 0, 52, 16, BUTTON );
	button_online->SetText("Online", 6);
	button_online->SetValue(GUI_PORTAL_ONLINE);

	button_browse = LuxPortal::interface->AddWidgetChild( footer_area, 64, 0, 52, 16, BUTTON );
	button_browse->SetText("Browse", 6);
	button_browse->SetValue(GUI_PORTAL_BROWSE);

	button_recent = LuxPortal::interface->AddWidgetChild( footer_area, 124, 0, 52, 16, BUTTON );
	button_recent->SetText("Recent", 6);
	button_recent->SetValue(GUI_PORTAL_RECENT);

	button_demos = LuxPortal::interface->AddWidgetChild( footer_area, 184, 0, 52, 16, BUTTON );
	button_demos->SetText("Demos", 5);
	button_demos->SetValue(GUI_PORTAL_DEMOS);

	button_next = LuxPortal::interface->AddWidgetChild( text_header, -20, 2, 16, 16, BUTTON );
	button_next->SetText(">",1);
	button_next->SetValue(GUI_PORTAL_NEXTPAGE);

	button_prev = LuxPortal::interface->AddWidgetChild( text_header, -40, 2, 16, 16, BUTTON );
	button_prev->SetText("<",1);
	button_prev->SetValue(GUI_PORTAL_PREVPAGE);


	switch ( LuxPortal::mode )
	{
		case GUI_PORTAL_RECENT:
			text_header->SetText( "Recently Played", 15 );
			return_value = LuxPortal::recent_page(item_rect);
			break;
		case GUI_PORTAL_ONLINE:
			text_header->SetText( "Online Listing", 14 );
			return_value = LuxPortal::online_page(item_rect);
			break;
		case GUI_PORTAL_BROWSE:
			text_header->SetText( "Game Browser:" + LuxPortal::browse_path, 13 + LuxPortal::browse_path.length() );
			return_value = LuxPortal::browse_page(item_rect);
			break;
		case GUI_PORTAL_DEMOS:
			text_header->SetText( "Demos", 5 );
			return_value = LuxPortal::demos_page(item_rect);
			break;
	}

	if ( return_value == GUI_PORTAL_ONLINE )
	{
		LuxPortal::mode = GUI_PORTAL_ONLINE;
	}
	else if ( return_value == GUI_PORTAL_BROWSE )
	{
		LuxPortal::browse_path = LuxPortal::browse_path_default;
		LuxPortal::mode = GUI_PORTAL_BROWSE;
	}
	else if ( return_value == GUI_PORTAL_BROWSE_AGAIN )
	{
		LuxPortal::mode = GUI_PORTAL_BROWSE;
	}
	else if ( return_value == GUI_PORTAL_RECENT )
	{
		LuxPortal::mode = GUI_PORTAL_RECENT;
	}
	else if ( return_value == GUI_PORTAL_DEMOS )
	{
		LuxPortal::mode = GUI_PORTAL_DEMOS;
	}
	else if ( return_value == GUI_EXIT)
	{
		LuxPortal::mode = 0;
		LuxPortal::active = false;
		results = false;
	}
	else if ( return_value == GUI_CONFIRM )
	{
		LuxPortal::mode = 0;
		results = true;
	}

	return_value = LuxPortal::mode;

	LuxPortal::interface->RemoveAll();

	NULLIFY( page_area );
	NULLIFY( footer_area );
	NULLIFY( button_exit );
	NULLIFY( button_browse );
	NULLIFY( button_online );
	NULLIFY( button_recent );
	NULLIFY( text_header );

	return results;
}

/**
 * @brief LuxPortal::run
 * @return
 */
bool LuxPortal::run()
{
	if ( !LuxPortal::use )
		return true;

	bool results = false;
	std::string css = "";
	lux::config = new GameConfig();

	/* Read CSS file */
	elix::File * portalcss = new elix::File( elix::directory::Resources("", "portal.css") );
	css = portalcss->ReadString();
	delete portalcss;

	lux::config->SetString("gui.style", css);
	lux::config->SetBoolean("portal.active", true);
	lux::config->SetBoolean("debug.enable", false);

	LuxPortal::browse_path_default = elix::directory::Documents(false);
	LuxPortal::browse_path = LuxPortal::browse_path_default;
	LuxPortal::mode = GUI_PORTAL_DEMOS;

	LuxPortal::interface = new UserInterface( new DisplaySystem( PROGRAM_NAME" Game Portal "PROGRAM_VERSION, 800, 600, 32, 0) );

	LuxPortal::region = LuxPortal::interface->ui_region;

	LuxPortal::interface->Show();

	LuxPortal::previous.sort();
	LuxPortal::previous.unique();


	uint16_t button_height = LuxPortal::interface->GetCSSParser()->GetSize(IMAGEBUTTON, ENABLED, "min-height")+5;
	LuxPortal::page_padding = LuxPortal::interface->GetCSSParser()->GetSize(BOX, ENABLED, "padding");

	LuxPortal::max_page_item = (LuxPortal::region.h - ( (lux_media::portal_title_height + 22 + LuxPortal::page_padding )*2))  / button_height;

	LuxPortal::logo = LuxPortal::interface->GetDisplaySystem()->graphics.PNGtoSprite(lux_media::portal_title, lux_media::portal_title_size);

	while ( LuxPortal::mode )
	{
		results = LuxPortal::frame();
	}

	NULLIFY( LuxPortal::interface );
	NULLIFY( lux::config );


	LuxPortal::mode = 1;

	return results;
}
