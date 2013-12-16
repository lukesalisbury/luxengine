/****************************
Copyright © 2013 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#ifndef SPRITE_SHEET_H
#define SPRITE_SHEET_H

class LuxSheet;

#include <map>

#include "stdheader.h"
#include "lux_sprite.h"
#include "display_types.h"
#include "elix_png.hpp"

class LuxSheet
{
public:
	LuxSheet( GraphicSystem graphic );
	LuxSheet( std::string file_name, GraphicSystem graphic );

	~LuxSheet();

	void SetName( std::string name );
	bool ParseXML();
	bool ParseSimpleText( const std::string content );
	void Print();

	bool Ref();
	bool Unref();

	bool Load();
	bool LoadFromImage( elix::Image *image );
	bool Refresh( GraphicSystem graphics );
	bool Unload();

	LuxSprite * GetSprite( std::string name );
	LuxSprite * GetSprite( uint32_t name_hash );
private:
	GraphicSystem graphics;
	std::string name;
	uint16_t requested;
	bool loaded;
	bool parsed;
	bool failed;
	std::map<uint32_t, LuxSprite *> children;
};



#endif // SPRITE_SHEET_H
