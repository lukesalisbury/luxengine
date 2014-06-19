/****************************
Copyright © 2006-2011 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include "game_info.h"
#include "elix_path.hpp"
#include "elix_string.hpp"
#include "tinyxml/tinyxml2ext.h"

GameInfo::GameInfo( std::string game_path, UserInterface * gui )
{
	this->button = NULL;
	this->icon = NULL;
	this->gui = gui;

	this->url = game_path;
	this->isDir = elix::path::Exist( this->url );
	this->file = new MokoiGame( this->url, true);
	this->valid = this->file->valid;

	this->information = this->file->GetTitle() + "\nAuthor: " + this->file->GetAuthor() + "\nPath: " + this->file->GetFilename();

	this->hash = elix::string::Hash( this->url );


}

GameInfo::GameInfo( tinyxml2::XMLElement * xml_element, UserInterface * gui )
{
	this->button = NULL;
	this->icon = NULL;
	this->gui = gui;

	tinyxml2::QueryStringAttribute( xml_element->FirstChildElement("download"), "url", this->url );

	this->isDir = false;
	this->file = new MokoiGame( this->url, true);
	this->valid = this->file->valid;

	std::string title = xml_element->FirstChildElement("title")->GetText();
	std::string author = xml_element->FirstChildElement("author")->GetText();
	this->information = title + "\nAuthor: " + author + "\nURL: " + this->url;

	this->hash = elix::string::Hash( this->url );


}



GameInfo::~GameInfo( )
{
	this->ClearGUI();
/*
	if ( this->file )
		delete this->file;

	if ( this->button )
		delete this->button;
		*/
}

void GameInfo::ClearGUI()
{
	if ( this->button )
	{
		if ( this->gui )
		{
			this->gui->RemoveChild( this->button );
		}
	}
}

uint32_t GameInfo::SetGUI( int32_t value, int32_t x, int32_t y, uint16_t width, uint16_t height  )
{
	std::string button_text;

	this->buttonArea.x = x;
	this->buttonArea.y = y;
	this->buttonArea.w = width;
	this->buttonArea.h = height;

	button_text = this->information;

	elix::string::TruncateLines( button_text, width / 8 );

	if ( CreateButton( ) )
	{
		this->button->SetRegion( this->buttonArea );
		this->button->SetText( button_text );
		this->button->SetValue( value );

		if ( this->gui )
			this->gui->AddChild( this->button );

		return this->button->GetAreaHeight() + 4;
	}
	else
	{
		return 0;
	}
}


bool GameInfo::CreateButton( )
{
	if ( this->gui )
	{
		this->button = new Widget( buttonArea, IMAGEBUTTON, this->gui->GetCSSParser() );
		this->button->SetText( this->information );
		return true;
	}
	else
	{
		this->button = NULL;

		return false;
	}
}

void GameInfo::AddButtonToGUI()
{
	if ( CreateButton( ) )
	{
		this->gui->AddChild( this->button );
	}
}

bool GameInfo::SetIcon( DisplaySystem * display )
{
	if ( this->file->png && this->file->png_length )
	{
		this->icon = display->graphics.PNGtoSprite( this->file->png, this->file->png_length );
		if ( this->icon )
			this->button->SetData( this->icon );
	}
	return (this->icon ? true : false);
}

bool GameInfo::ClearIcon( DisplaySystem * display )
{
	if ( this->icon )
	{
		display->graphics.FreeSprite( this->icon );
		this->icon = NULL;
	}
	return (this->icon ? false : true);
}



