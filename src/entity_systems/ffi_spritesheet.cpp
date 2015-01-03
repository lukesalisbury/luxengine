/****************************
Copyright © 2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include "ffi_spritesheet.h"

#include "display.h"
#include "game_system.h"

/** Lux_FFI_Sheet_Reference
*
*
*/
void Lux_FFI_Sheet_Reference( const char * sheet, int8_t ref )
{
	if ( sheet )
	{
		if ( lux::display )
		{
			if ( ref == -1 )
				lux::display->UnrefSheet( sheet );
			else
				lux::display->RefSheet( sheet );
		}
	}
}

/** Lux_FFI_Sheet_Replace
*
*
*/
void Lux_FFI_Sheet_Replace( const char * old_sheet, const char * new_sheet )
{

	if ( old_sheet && new_sheet )
	{
		if ( lux::gamesystem->active_map )
		{
			lux::gamesystem->active_map->ReplaceObjectsSheets( old_sheet, new_sheet );
		}
	}

}
