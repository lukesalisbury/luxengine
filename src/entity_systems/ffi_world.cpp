/****************************
Copyright © 2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include "ffi_world.h"

#include "world.h"

/**
 * @brief Lux_FFI_World_Get
 * @param section_hash
 * @param grid_x
 * @param grid_y
 * @return Map ID for selected location
 */
uint32_t Lux_FFI_World_Get( const uint32_t section_hash, const uint8_t grid_x, const uint8_t grid_y )
{
	uint32_t map_ident = 0;

	if ( lux::gameworld )
	{
		WorldSection * section = lux::gameworld->GetSection( section_hash );
		MokoiMap * map = section->GetMap( grid_x, grid_y );

		map_ident = map->Ident();
	}

	return map_ident;
}

/**
 * @brief Lux_FFI_World_Set
 * @param section_hash
 * @param grid_x
 * @param grid_y
 * @return Map ID for the new location
 */
uint32_t Lux_FFI_World_Set( const uint32_t section_hash, const uint8_t grid_x, const uint8_t grid_y )
{
	uint32_t map_ident = 0;

	if ( lux::gameworld )
	{
		map_ident = lux::gameworld->SetMap( section_hash, grid_x, grid_y, -1, -1, -1 );
	}

	return map_ident;
}

/**
 * @brief Lux_FFI_World_Exist
 * @param section_name
 * @return
 */
uint32_t Lux_FFI_World_Exist( const char * section_name )
{
	uint32_t section_hash = 0;
	if ( lux::gameworld )
	{
		WorldSection * section = lux::gameworld->GetSection( section_name, false );
		if ( section )
		{
			section_hash = section->Ident();
		}
	}
	return section_hash;
}

/**
 * @brief Lux_FFI_World_Edit_New
 * @param section_name
 * @param width
 * @param height
 * @return
 */
uint32_t Lux_FFI_World_Edit_New( const char * section_name, const uint8_t width, const uint8_t height )
{
	uint32_t section_hash = 0;

	if ( (width > 0 && width <= 64) && (height > 0 && height <= 64) )
	{
		if ( lux::gameworld )
		{
			WorldSection * section = lux::gameworld->NewSection( section_name, width, height );
			if ( section )
			{
				section->SetOption(1, 1);
				section_hash = section->Ident();
			}
		}
	}

	return section_hash;

}

/**
 * @brief Lux_FFI_World_Edit_Set
 * @param section_name
 * @param map_ident
 * @param x
 * @param y
 * @return
 */
uint8_t Lux_FFI_World_Edit_Set( uint32_t section_hash, uint32_t map_ident, const uint8_t x, const uint8_t y )
{
	if ( lux::gameworld )
	{
		WorldSection * section = lux::gameworld->GetSection( section_hash );
		if ( section && section->GetOption(1) == true )
		{
			MokoiMap * map = lux::gameworld->GetMap( map_ident );

			return section->AddMap(map, x, y);
		}
	}

	return false;
}

/**
 * @brief Lux_FFI_World_Edit_Save
 * @param section_name
 * @return
 */
uint8_t Lux_FFI_World_Edit_Save( uint32_t section_hash )
{
	if ( lux::gameworld )
	{
		WorldSection * section = lux::gameworld->GetSection( section_hash );
		if ( section && section->GetOption(1) == true )
		{
			section->SaveFile();
			return true;
		}
	}

	return false;

}
