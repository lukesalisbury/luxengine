/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include "game_info.h"
#include "elix/elix_path.hpp"
#include "elix/elix_string.hpp"
#include "tinyxml/tinyxml2ext.h"

#include "portal.h"

/**
 * @brief GameInfo::GameInfo
 * @param gui
 * @param game_path
 */
GameInfo::GameInfo( UserInterface * gui, std::string game_path )
{
	this->type = GI_INVALID;
	this->gui = gui;
	this->button = nullptr;
	this->icon = nullptr;

	this->url = game_path;

	this->DetermitedType();
}

/**
 * @brief GameInfo::GameInfo
 * @param gui
 * @param xml_element
 */
GameInfo::GameInfo( UserInterface * gui, tinyxml2::XMLElement * xml_element )
{
	this->type = GI_INVALID;
	this->gui = gui;
	this->button = nullptr;
	this->icon = nullptr;
	this->hash = 0;

	tinyxml2::QueryStringAttribute( xml_element->FirstChildElement("download"), "url", this->url );

	if ( elix::string::HasPrefix( this->url, "http") )
	{
		std::string title = xml_element->FirstChildElement("title")->GetText();
		std::string author = xml_element->FirstChildElement("author")->GetText();

		this->information = title + "\nAuthor: " + author + "\nURL: " + this->url;

		this->type = GI_ONLINE;

		this->hash = elix::string::Hash( this->url );
	}
}

/**
 * @brief GameInfo::~GameInfo
 */
GameInfo::~GameInfo( )
{
	if ( this->button )
	{
		this->ClearGUI();
	}
	if ( this->icon )
	{
		this->gui->GetDisplaySystem()->graphics.FreeSprite( this->icon );
		NULLIFY( this->icon );
	}
}

/**
 * @brief GameInfo::ClearGUI
 */
void GameInfo::ClearGUI()
{
	this->gui->RemoveChild( this->button );

	NULLIFY( this->button );
}

/**
 * @brief GameInfo::SetGUI
 * @param value
 * @param x
 * @param y
 * @param width
 * @param height
 * @return
 */
uint32_t GameInfo::SetGUI( int32_t value, int32_t x, int32_t y, uint16_t width, uint16_t height  )
{
	if ( !this->button )
	{
		std::string button_text;

		this->buttonArea.x = x;
		this->buttonArea.y = y;
		this->buttonArea.w = width;
		this->buttonArea.h = height;

		button_text = this->information;

		if ( width > 8 )
		{
			elix::string::TruncateLines( button_text, (width-32) / 8 );
			//elix::string::TruncateLines( button_text, 20 );
		}

		this->button = this->gui->AddChild( buttonArea, IMAGEBUTTON, button_text );

		this->button->SetIcon( this->icon );
		this->button->SetValue( value );

		return this->button->GetAreaHeight() + 4;
	}
	return 0;
}

/**
 * @brief GameInfo::DetermitedType
 */
void GameInfo::DetermitedType()
{
	/* Check if Directory */
	if ( elix::path::Exist( this->url ) )
	{
		this->information = "Directory\n" + this->url;
		this->type = GI_DIR;
	}

	/* Check for Game */
	MokoiGame * game = new MokoiGame( this->url, false );
	if ( game->valid )
	{
		this->type = GI_FILE;
		this->information = game->GetTitle() + "\nAuthor: " + game->GetAuthor() + "\nPath: " + game->GetFilename();
		if ( game->png && game->png_length )
		{
			this->icon = this->gui->GetDisplaySystem()->graphics.PNGtoSprite( game->png, game->png_length );
		}
	}
	delete game;


	/* > Temporary hack */
	/* Check other support types */
	if ( elix::string::HasSuffix( this->url, ".qst.gz") )
	{
		this->type = GI_SPECIAL;
		this->information = "Classic Quest\nUnknown Author\nPath: " + this->url;
	}
	/* < Temporary hack */
}

