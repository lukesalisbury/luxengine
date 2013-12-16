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

namespace luxportal {
	game::game( std::string gamepack, UserInterface * gui )
	{
		this->isDir = elix::path::Exist(gamepack);
		this->url = gamepack;
		this->file = new MokoiGame(gamepack, true);
		this->valid = this->file->valid;
		this->button = NULL;
		this->icon = NULL;
		this->_gui = gui;
	}
	game::game( UserInterface * gui )
	{
		this->url = "";
		this->file = new MokoiGame();
		this->valid = this->file->valid;
		this->isDir = false;
		this->button = NULL;
		this->icon = NULL;
		this->_gui = gui;
	}

	game::~game(  )
	{
		if ( this->file )
			delete this->file;
		if ( this->icon )
			delete this->icon;
		if ( this->_gui)
			this->_gui->RemoveChild( this->button );
		if ( this->button )
			delete this->button;
	}

	bool game::SetIcon( DisplaySystem * display )
	{
		if ( this->file->png && this->file->png_length )
		{
			this->icon = display->graphics.PNGtoSprite( this->file->png, this->file->png_length );
		}
		return (this->icon ? true : false);
	}
	bool game::ClearIcon( DisplaySystem * display )
	{
		if ( this->icon )
		{
			display->graphics.FreeSprite( this->icon );
		}
		return (this->icon ? false : true);
	}
}


