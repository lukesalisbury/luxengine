/****************************
Copyright © 2006-2011 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "core.h"
#include "config.h"
#include "engine.h"
#include "elix_string.h"
#include "elix_path.h"
#include "mokoi_game.h"
#include "tinyxml/tinyxml.h"
#include "worker.h"
#include "game_info.h"
#include "portal.h"
#include "gui.h"
#include "downloader.h"

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
#endif
namespace luxportal {
	DisplaySystem * display = NULL;
	UserInterface * interface = NULL;
	std::vector<luxportal::game *> games;
	std::list<std::string> previous;
	bool use = true;
	bool active = true;
	bool opengl = false;
	int16_t mode = 1;
	LuxRect region = {0, 0, 512, 384, 0};
	Effects background0 = { {119,142,179,255}, {109,132,169,255}, 0, 1000, 1000, 0, 0, STYLE_VGRADIENT };
	Effects header_bg = { {10,142,10,255}, {0,132,0,255}, 0, 1000, 1000, 0, 0, STYLE_VGRADIENT };

	uint32_t offset = 0;

	std::string root_url = GUI_PORTAL_URL;

	std::string browse_path = "";
	std::string browse_path_default = "";

	void recent_page_refresh( LuxRect page_rect, uint32_t start, uint32_t length );
	void browse_page_refresh( LuxRect page_rect, uint32_t start, uint32_t length, std::vector<char*> & list );
	void online_page_refresh( LuxRect page_rect, uint32_t start, uint32_t length );
}

void luxportal::add( std::string path )
{
	if ( path.at(0) == '\'' && path.length() > 3)
		path = path.substr(1, path.length() - 2);
	#ifdef __GNUWIN32__
	elix::string::Replace( path, "\\\\", "\\" );
	elix::string::Replace( path, "/", "\\" );
	#endif

	luxportal::previous.push_back( path );
	luxportal::previous.unique();
}

void luxportal::recent_page_refresh( LuxRect page_rect, uint32_t start, uint32_t length )
{
	uint32_t c = 0;
	std::list<std::string>::iterator iter1;

	luxportal::clear();

	iter1 = luxportal::previous.begin();
	while( iter1 != luxportal::previous.end() )
	{
		if ( c > start)
		{
			luxportal::game * info = new luxportal::game( *iter1, luxportal::interface );
			if ( info->valid )
			{
				luxportal::games.push_back( info );
				info->button = luxportal::interface->AddChild( page_rect, IMAGEBUTTON, colour::white, info->file->title + "\nAuthor: " + info->file->author + "\nPath: " + info->file->filename );
				info->button->SetValue( luxportal::games.size() );

				info->SetIcon( luxportal::display );
				if ( info->icon )
				{
					if ( info->icon->data )
					{
						info->button->SetData( info->icon->data );
					}
				}
				page_rect.y += 40;
				c++;
			}
			else
			{
				iter1 = luxportal::previous.erase(iter1);
				iter1--;
			}
		}
		else
			c++;

		if ( c >= start + length )
			iter1 = luxportal::previous.end();
		else
			iter1++;
	}
}

int32_t luxportal::recent_page( LuxRect page_rect )
{
	int32_t return_value = 0;
	uint32_t start = 0, length = 8;

	page_rect.h = 36;
	luxportal::recent_page_refresh( page_rect, start, length );


	while ( !return_value )
	{
		return_value = luxportal::interface->Loop();
		if ( return_value == GUI_PORTAL_NEXTPAGE )
		{
			if (start + length < luxportal::previous.size())
			{
				start += length;
				luxportal::recent_page_refresh( page_rect, start, length );
			}
			return_value = 0;
		}
		else if ( return_value == GUI_PORTAL_PREVPAGE )
		{
			if (start >= length)
			{
				start -= length;
				luxportal::recent_page_refresh( page_rect, start, length );
			}
			return_value = 0;
		}
		
	}

	if ( return_value > 0 )
	{
		luxportal::game * selected = luxportal::games.at( return_value-1 );
		if ( lux::config )
			lux::config->SetString( "project.file", selected->file->filename );
		luxportal::add( selected->file->filename );
		return_value = GUI_PORTAL_PLAY;
	}
	return return_value;
}

int32_t luxportal::browse_demos( LuxRect page_rect )
{
	int32_t return_value = 0;
	uint32_t start = 0, length = 8;
	page_rect.h = 36;

	std::vector<char*> list;

	std::string dir;
	
	#ifdef apple
		if ( lux::config )
			dir = lux::config->GetString( "directory.program" ) + "../../../MokoiGamingDemos";
	#else
		if ( lux::config )
			dir = lux::config->GetString( "directory.program" ) + "demos";
	#endif

	elix::path::Children( dir, "", &list, false, true, true );

	std::vector<char*>::iterator iter = list.begin();
	while( iter != list.end() )
	{
		luxportal::game * info = new luxportal::game( *iter );
		if ( !info->valid )
			iter = list.erase( iter );
		else
			++iter;
		delete info;
	}

	luxportal::browse_page_refresh( page_rect, start, length, list );

	while ( !return_value )
	{
		return_value = luxportal::interface->Loop();
	}

	list.clear();
	if ( return_value > 0 )
	{
		luxportal::game * selected = luxportal::games.at( return_value-1 );
		if (selected)
		{
			if ( lux::config )
				lux::config->SetString( "project.file", selected->file->filename );
			luxportal::add( selected->file->filename );
			return_value = GUI_PORTAL_PLAY;
		}
		else
		{
			return_value = GUI_PORTAL_RECENT;
		}
	}
	return return_value;
}

void luxportal::browse_page_refresh( LuxRect page_rect, uint32_t start, uint32_t length, std::vector<char*> & list )
{
	luxportal::clear();
	page_rect.h = 36;

	if (start > list.size() )
	{
		return;
	}

	for( uint32_t i = start; i < start + length && i < list.size(); i++ )
	{
		luxportal::game * info = new luxportal::game( list.at(i), luxportal::interface );
		if ( info->valid )
		{
			luxportal::games.push_back( info );

			info->button = luxportal::interface->AddChild( page_rect, IMAGEBUTTON, colour::white, info->file->title + "\nAuthor: " + info->file->author + "\nPath: " + info->file->filename );
			info->button->SetValue( luxportal::games.size() );
			info->SetIcon( luxportal::display );
			if ( info->icon )
			{
				if ( info->icon->data )
				{
					info->button->SetData( info->icon->data );
				}
			}
			page_rect.y += 40;
		}
		else if ( info->isDir )
		{
			luxportal::games.push_back( info );

			info->button = luxportal::interface->AddChild( page_rect, IMAGEBUTTON, colour::white, info->url + "\nDirectory" );
			info->button->SetValue( luxportal::games.size() );
			page_rect.y += 40;
		}
		/* CUSTOM CODE */
		else if ( info->url.find(".qst") != std::string::npos )
		{
			luxportal::games.push_back( info );
			info->file->filename = info->url;
			info->button = luxportal::interface->AddChild( page_rect, IMAGEBUTTON, colour::white, "Classic Quest\nPath: " + info->file->filename );
			info->button->SetValue( luxportal::games.size() );
			page_rect.y += 40;
		}
		/* CUSTOM CODE */
	}
}

int32_t luxportal::browse_page( LuxRect page_rect )
{
	int32_t return_value = 0;
	uint32_t start = 0, length = 8;

	std::vector<char*> list;

	elix::path::Children( luxportal::browse_path, "", &list, false, true, true );

	std::vector<char*>::iterator iter = list.begin();
	while( iter != list.end() )
	{
		luxportal::game * info = new luxportal::game( *iter );
		if ( info->valid || info->isDir || info->url.find(".qst") != std::string::npos )
			++iter;
		else
			iter = list.erase( iter );
		delete info;
	}

	luxportal::browse_page_refresh( page_rect, start, length, list );

	while ( !return_value )
	{
		return_value = luxportal::interface->Loop();
		if ( return_value == GUI_PORTAL_NEXTPAGE )
		{
			if (start + length < list.size())
			{
				start += length;
				luxportal::browse_page_refresh( page_rect, start, length, list );
			}
			return_value = 0;
		}
		else if ( return_value == GUI_PORTAL_PREVPAGE )
		{
			if (start >= length)
			{
				start -= length;
				luxportal::browse_page_refresh( page_rect, start, length, list );
			}
			return_value = 0;
		}
		
	}

	list.clear();
	if ( return_value > 0 )
	{
		luxportal::game * selected = luxportal::games.at( return_value-1 );
		if (selected)
		{
			if ( selected->isDir )
			{
				luxportal::browse_path = selected->url;
				return_value = GUI_PORTAL_BROWSE_AGAIN;
			}
			else
			{
				if ( lux::config )
					lux::config->SetString( "project.file", selected->file->filename );
				luxportal::add( selected->file->filename );
				return_value = GUI_PORTAL_PLAY;
			}
		}
		else
		{
			return_value = GUI_PORTAL_BROWSE_AGAIN;
		}
	}
	return return_value;
}

void luxportal::online_page_refresh( LuxRect page_rect, uint32_t start, uint32_t length )
{
	uint32_t c = 0;
	std::list<std::string>::iterator iter1;

	luxportal::clear();

	iter1 = luxportal::previous.begin();
	while( iter1 != luxportal::previous.end() )
	{
		if ( c > start)
		{
			luxportal::game * info = new luxportal::game( *iter1, luxportal::interface );
			if ( info->valid )
			{
				luxportal::games.push_back( info );
				info->button = luxportal::interface->AddChild( page_rect, IMAGEBUTTON, colour::white, info->file->title + "\nAuthor: " + info->file->author + "\nPath: " + info->file->filename );
				info->button->SetValue( luxportal::games.size() );

				info->SetIcon( luxportal::display );
				if ( info->icon )
				{
					if ( info->icon->data )
					{
						info->button->SetData( info->icon->data );
					}
				}
				page_rect.y += 40;
				c++;
			}
			else
			{
				iter1 = luxportal::previous.erase(iter1);
				iter1--;
			}
		}
		else
			c++;

		if ( c >= start + length )
			iter1 = luxportal::previous.end();
		else
			iter1++;
	}
}

int32_t luxportal::online_page( LuxRect page_rect )
{
	TiXmlDocument xml;
	int32_t return_value = 0;
	page_rect.h = 36;

	Lux_Util_FileDownloaderBackground( luxportal::root_url + "/games.xml", elix::path::Cache("portal.xml") ,  luxportal::interface );
	xml.LoadFile( elix::path::Cache("portal.xml") );

	if ( xml.Error() )
	{
		std::cerr << "Portal Error:" << xml.ErrorDesc() << std::endl;
		return GUI_PORTAL_RECENT;
	}

	TiXmlElement * root = xml.RootElement();
	TiXmlElement * child = root->FirstChildElement()->FirstChildElement();
	if ( child )
	{
		while( child )
		{
			luxportal::game * info = new luxportal::game(luxportal::interface);
			info->file->title = child->FirstChildElement("title")->GetText();
			info->file->author = child->FirstChildElement("author")->GetText();
			info->file->filename = "";
			info->url = child->FirstChildElement("download")->Attribute("url");

			luxportal::games.push_back( info );

			info->button = luxportal::interface->AddChild( page_rect, BUTTON, (LuxColour){255, 0, 128, 200}, info->file->title + "\nAuthor: " + info->file->author + "\nURL: " + info->url );
			info->button->SetValue( luxportal::games.size() );

			page_rect.y += 40;

			child = child->NextSiblingElement();
		}
	}

	while ( !return_value )
	{
		return_value = luxportal::interface->Loop();
	}
	luxportal::interface->RemoveAll();

	if ( return_value > 0 )
	{
		luxportal::game * selected = luxportal::games.at(return_value-1);
		Lux_Util_FileDownloaderBackground(selected->url.substr(7), elix::path::Cache("temp.game"), luxportal::interface);
		lux::config->SetString("project.file", elix::path::Cache("temp.game"));
		luxportal::add( elix::path::Cache("temp.game") );
		return_value = GUI_PORTAL_PLAY;
	}

	return return_value;
}

void luxportal::clear()
{
	std::vector<luxportal::game *>::iterator iter1 = luxportal::games.begin();
	while ( iter1 != luxportal::games.end() )
	{
		(*iter1)->ClearIcon( luxportal::display );
		delete *iter1;
		iter1++;
	}
	luxportal::games.clear();
}

void luxportal::open()
{
	std::string buffer;
	elix::File * gamelist = new elix::File( elix::path::Cache("recent.txt") , false );
	if ( gamelist->Exist() )
	{
		while ( gamelist->ReadLine(&buffer) )
		{
			elix::string::Trim(&buffer);
			if ( !buffer.empty() )
			{
				if ( buffer.at(0) == '\'' && buffer.length() > 3)
					buffer = buffer.substr(1, buffer.length() - 2);
				luxportal::add( buffer );
			}
			buffer.clear();
		}
	}
	delete gamelist;
}

void luxportal::close()
{
	/* save games.txt */
	luxportal::previous.sort();
	luxportal::previous.unique();

	elix::File * gamelist = new elix::File( elix::path::Cache("recent.txt") , true );
	std::list<std::string>::iterator iter1 = luxportal::previous.begin();
	while ( iter1 != luxportal::previous.end() )
	{
		gamelist->WriteString( *iter1 );
		gamelist->Write((uint8_t)10);
		iter1++;
	}
	delete gamelist;

	luxportal::clear();
}


bool luxportal::run()
{
	if ( !luxportal::use )
		return true;

	if ( lux::config )
	{
		lux::config->SetNumber("display.width", 640);
		lux::config->SetNumber("display.height", 480);
		lux::config->SetNumber("screen.width", 640);
		lux::config->SetNumber("screen.height", 480);
		lux::config->Reset();
		lux::config->SetString("project.id", "cache");
		lux::config->SetString("project.title", PROGRAM_NAME);
		lux::config->SetString("display.mode", (luxportal::opengl ?"OpenGL" : "native") );
		lux::config->SetBoolean("portal.active", true);
		lux::config->SetString("display.fullscreen", "false");
		lux::config->SetBoolean("debug.enable", true);


		std::string css = "";
		elix::File * portalcss = new elix::File( lux::config->GetString("directory.program") + "portal.css");
		css = portalcss->ReadString();
		lux::config->SetString("gui.style", css);
		delete portalcss;


	}

	elix::path::Documents(&luxportal::browse_path_default, true);
	luxportal::browse_path_default.append("MokoiGaming");
	luxportal::browse_path =luxportal::browse_path_default;
	luxportal::mode = GUI_PORTAL_BROWSE;
	bool results = false;
	luxportal::display = new DisplaySystem();
	luxportal::region = luxportal::display->screen_dimension;
	luxportal::interface = new UserInterface(luxportal::region, luxportal::display);
	//luxportal::interface->SetBackground( luxportal::background0 );
	luxportal::interface->Show();
	luxportal::previous.sort();
	luxportal::previous.unique();

	while ( luxportal::mode )
	{
		int32_t return_value = 0;
		Widget * footer_area, * button_exit, * button_browse, * button_online, * button_demos, * button_recent, * text_header, * button_next, * button_prev;

		LuxRect footer_rect = { 0, 0, 512, 24, 0 };
		LuxRect page_rect = { 12, 26, 384, 36, 0 };
		LuxRect header_rect = { 0, 0, 512, 24, 0 };

		footer_rect.w = header_rect.w = luxportal::region.w;
		footer_rect.y = luxportal::region.h - 24;
		page_rect.w = luxportal::region.w - 24;

		footer_area = luxportal::interface->AddChild( footer_rect, EMPTYWINDOW, colour::white, "" );

		button_exit = luxportal::interface->AddWidgetChild( footer_area, -40, 0, 36, 16, BUTTON );
		button_exit->SetText("Exit");
		button_exit->SetValue(GUI_EXIT);

		button_online = luxportal::interface->AddWidgetChild( footer_area, 4, 0, 52, 16, BUTTON );
		button_online->SetText("Online");
		button_online->SetValue(GUI_PORTAL_ONLINE);

		button_browse = luxportal::interface->AddWidgetChild( footer_area, 60, 0, 52, 16, BUTTON );
		button_browse->SetText("Browse");
		button_browse->SetValue(GUI_PORTAL_BROWSE);

		button_recent = luxportal::interface->AddWidgetChild( footer_area, 116, 0, 52, 16, BUTTON );
		button_recent->SetText("Recent");
		button_recent->SetValue(GUI_PORTAL_RECENT);

		button_demos = luxportal::interface->AddWidgetChild( footer_area, 172, 0, 52, 16, BUTTON );
		button_demos->SetText("Demos");
		button_demos->SetValue(GUI_PORTAL_DEMOS);

		text_header = luxportal::interface->AddChild( header_rect, TEXT, colour::white, PROGRAM_NAME"\nRecently Played" );

		button_next = luxportal::interface->AddWidgetChild( text_header, -20, 4, 16, 16, BUTTON );
		button_next->SetText(">");
		button_next->SetValue(GUI_PORTAL_NEXTPAGE);

		button_prev = luxportal::interface->AddWidgetChild( text_header, -40, 4, 16, 16, BUTTON );
		button_prev->SetText("<");
		button_prev->SetValue(GUI_PORTAL_PREVPAGE);


		switch ( luxportal::mode )
		{
			case GUI_PORTAL_RECENT:
				text_header->SetText( PROGRAM_NAME"\nRecently Played" );
				return_value = luxportal::recent_page(page_rect);
				luxportal::clear();
				break;
			case GUI_PORTAL_ONLINE:
				text_header->SetText( PROGRAM_NAME"\nOnline Listing" );
				return_value = luxportal::online_page(page_rect);
				luxportal::clear();
				break;
			case GUI_PORTAL_BROWSE:
				text_header->SetText( PROGRAM_NAME"\nGame Browser:" + luxportal::browse_path );
				return_value = luxportal::browse_page(page_rect);
				luxportal::clear();
				break;
			case GUI_PORTAL_DEMOS:
				text_header->SetText( PROGRAM_NAME"\nDemos" );
				return_value = luxportal::browse_demos(page_rect);
				luxportal::clear();
				break;
		}

		if ( return_value == GUI_PORTAL_ONLINE )
			luxportal::mode = GUI_PORTAL_ONLINE;
		else if ( return_value == GUI_PORTAL_BROWSE )
		{
			luxportal::browse_path = luxportal::browse_path_default;
			luxportal::mode = GUI_PORTAL_BROWSE;
		}
		else if ( return_value == GUI_PORTAL_BROWSE_AGAIN )
		{
			luxportal::mode = GUI_PORTAL_BROWSE;
		}
		else if ( return_value == GUI_PORTAL_RECENT )
			luxportal::mode = GUI_PORTAL_RECENT;
		else if ( return_value == GUI_PORTAL_DEMOS )
			luxportal::mode = GUI_PORTAL_DEMOS;
		else if ( return_value == GUI_EXIT)
		{
			luxportal::mode = 0;
			luxportal::active = false;
			results = false;
		}
		else if ( return_value == GUI_CONFIRM )
		{
			luxportal::mode = 0;
			results = true;
		}

		luxportal::interface->RemoveAll();

		if ( footer_area )
			delete footer_area;
		if ( button_exit )
			delete button_exit;
		if ( button_browse )
			delete button_browse;
		if ( button_online )
			delete button_online;
		if ( button_recent )
			delete button_recent;
		if ( text_header )
			delete text_header;
	}

	if ( luxportal::interface )
		delete luxportal::interface;
	if ( luxportal::display )
		delete luxportal::display;

	luxportal::mode = 1;
	return results;
}
