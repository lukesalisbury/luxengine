/****************************
Copyright © 2006-2011 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "audio.h"
#include "config.h"
#include "core.h"
#include "display.h"
#include "engine.h"
#include "mokoi_game.h"
#include "entity_manager.h"
#include "pawn_helper.h"
#include "world.h"

extern const AMX_NATIVE_INFO Maps_Natives[];


/** Mask Functions */
/** pawnMaskRefresh
* native MaskRefresh();
*
*/
static cell AMX_NATIVE_CALL pawnMaskRefresh(AMX *amx, const cell *params)
{
	if ( lux::world->active_map != NULL )
	{
		lux::world->active_map->BuildMask();
		return 1;
	}
	return 0;
}

/** pawnMaskGetValue
* native MaskGetValue(x, y, layer = 0);
*
*/
static cell AMX_NATIVE_CALL pawnMaskGetValue(AMX *amx, const cell *params)
{
	if (lux::world->active_map != NULL)
		return (cell)lux::world->active_map->GetMaskValue((uint16_t) params[1], (uint16_t) params[2]);
	return -1;
}

/** pawnMaskFill
* native MaskFill(x, y, w,h, value = 0);
*
*/
static cell AMX_NATIVE_CALL pawnMaskFill(AMX *amx, const cell *params)
{
	if (lux::world->active_map != NULL)
	{
		lux::world->active_map->FillMask((uint16_t) params[1], (uint16_t) params[2], (uint16_t) params[3], (uint16_t) params[4], (uint8_t) params[5]);
		return 1;
	}
	return 0;
}

/** Map Functions */
/** pawnMapSetOffset
* native MapSetOffset(Fixed:x, Fixed:y);
* Set the Current map offset
*/
static cell AMX_NATIVE_CALL pawnMapSetOffset(AMX *amx, const cell *params)
{
	lux::world->SetPosition( params[1], params[2] );
	return 0;
}

/** pawnMapGetOffset
* native Fixed:MapGetOffset();
*
*/
static cell AMX_NATIVE_CALL pawnMapGetOffset(AMX *amx, const cell *params)
{
	return (cell)lux::world->GetPosition(params[1], 'm');
}

/* File Code */


/** pawnMapCreate
* native MapCreate(map[], create_new = 0, maxlength=sizeof map);
*
*/
static cell AMX_NATIVE_CALL pawnMapCreate(AMX *amx, const cell *params)
{
	std::string map_name = Lux_PawnEntity_GetString( amx, params[1] );
	if ( map_name.length() )
	{
		MokoiMap * map = lux::world->LoadMap(map_name, false, !!params[2]);
		return map->Ident();
	}
	return 0;
}

/** pawnMapChange
* native MapChange(mapid, offsetx = -1, offsety = -1);
*
*/
static cell AMX_NATIVE_CALL pawnMapChange(AMX *amx, const cell *params)
{
	if (params[1] != 0)
	{
		uint32_t id = lux::world->SetMap((uint32_t)params[1], (int32_t)params[2], (int32_t)params[3]);
		return (cell)id;
	}
	return 0;
}

/** pawnMapReset
* native MapReset();
*
*/
static cell AMX_NATIVE_CALL pawnMapReset(AMX *amx, const cell *params)
{
	if ( lux::world->active_map )
		return (cell) lux::world->active_map->Reset();
	return -1;
}

/** pawnMapValid
* native MapValid(mapid);
*
*/
static cell AMX_NATIVE_CALL pawnMapValid(AMX *amx, const cell *params)
{
	MokoiMap * map = lux::world->GetMap( (uint32_t)params[1] );
	return (map != NULL);
}

/** pawnMapCurrent
* native MapCurrent(map[], maxlength = sizeof map);
*
*/
static cell AMX_NATIVE_CALL pawnMapCurrent(AMX *amx, const cell *params)
{
	if (lux::world->active_map)
	{
		if ( params[2] > 2 )
		{
			cell * cptr = amx_Address(amx, params[1]);
			Lux_PawnEntity_SetString(cptr, lux::world->active_map->Name().c_str(), params[2]);
		}
		return (cell) lux::world->active_map->Ident();
	}
	return 0;
}
/** pawnMapIdent
* native MapIdent();
*
*/
static cell AMX_NATIVE_CALL pawnMapIdent(AMX *amx, const cell *params)
{
	if (lux::world->active_map)
	{
		return (cell) lux::world->active_map->Ident();
	}
	return 0;
}


/** pawnMapID
* native MapID(map[], maxlength = sizeof map);
*
*/
static cell AMX_NATIVE_CALL pawnMapID(AMX *amx, const cell *params)
{
	std::string map_file = Lux_PawnEntity_GetString( amx, params[1] );
	MokoiMap * map = NULL;
	uint32_t map_id = lux::world->GetMapID(map_file);
	if ( !map_id )
	{
		map = lux::world->LoadMap(map_file, false, false);
		map_id = map->Ident();
	}

	return (cell)map_id;
}

/** pawnMapSave
* native MapSave();
*
*/
static cell AMX_NATIVE_CALL pawnMapSave(AMX *amx, const cell *params)
{
	return 0;
}

/** pawnMapSaveToFile
* native MapSaveToFile();
*
*/
static cell AMX_NATIVE_CALL pawnMapSaveToFile(AMX *amx, const cell *params)
{
	return 0;
}

/** pawnMapGetDimension
* native MapGetDimension( &w, &h);
* Get Dimension of Map or Screen
*/
static cell AMX_NATIVE_CALL pawnMapGetDimension(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 2 );

	cell responed = 0;
	if ( lux::world->active_map != NULL )
	{
		write_amx_address( amx, params[1], MAKE_FIXED_INT(lux::world->active_map->map_width) );
		write_amx_address( amx, params[2], MAKE_FIXED_INT(lux::world->active_map->map_height) );
		responed = 1;
	}
	return responed;
}



/** Map Collision Functions */
/** pawnMapCollisionList
* native MapCollisionList();
*
*/
static cell AMX_NATIVE_CALL pawnMapCollisionList(AMX *amx, const cell *params)
{
	return -1;
}

/** pawnMapCollisionCurrent
* native MapCollisionCurrent(objecta[], objectb[], &angle, &dist, &rect, length_a = sizeof objecta, length_b = sizeof objectb);
*
*/
static cell AMX_NATIVE_CALL pawnMapCollisionCurrent(AMX *amx, const cell *params)
{
	return -1;
}

/** Section Functions */
/** pawnSectionSet
* native SectionSet(section[] = SELF, gridx = -1, gridy = -1, maxlength = sizeof section );
*
*/
static cell AMX_NATIVE_CALL pawnSectionSet(AMX *amx, const cell *params)
{
	uint8_t gridx = (uint8_t)params[2];
	uint8_t gridy = (uint8_t)params[3];


	if ( params[2] == -1 )
		gridx = ( !lux::world->active_section ? 0 : lux::world->_grid[0]);
	if ( params[3] == -1 )
		gridy = ( !lux::world->active_section ? 0 : lux::world->_grid[1]);

	if ( Lux_PawnEntity_HasString(amx, params[1]) )
	{
		std::string section = Lux_PawnEntity_GetString(amx, params[1]);
		return lux::world->SwitchSection( section, gridx, gridy );
	}
	else
	{
		return lux::world->SetMap( gridx, gridy );
	}

	return 0;
}

/** pawnSectionGet
* native SectionGet(name[], &gridx , &gridy, maxlength = sizeof section);
*
*/
static cell AMX_NATIVE_CALL pawnSectionGet(AMX *amx, const cell *params)
{
	cell * cptr = amx_Address(amx, params[1]);
	cell * xptr = amx_Address(amx, params[2]);
	cell * yptr = amx_Address(amx, params[3]);

	if ( lux::world->active_section )
	{
		std::string section = lux::world->active_section->file;

		if ( xptr )
			*xptr = lux::world->_grid[0];
		if ( yptr )
			*yptr = lux::world->_grid[1];

		Lux_PawnEntity_SetString(cptr, section.c_str(), params[4]);
		return lux::world->active_section->_id;
	}
	else
	{
		if ( xptr )
			*xptr = -1;
		if ( yptr )
			*yptr = -1;
		Lux_PawnEntity_SetString(cptr, (char*)"Not in a section", params[4]);
	}
	return 0;
}

/** pawnSectionLoad
* native SectionLoad(section[], unload = 0, maxlength = sizeof section );
*
*/
static cell AMX_NATIVE_CALL pawnSectionLoad(AMX *amx, const cell *params)
{
	std::string section_name = "";
	if ( Lux_PawnEntity_HasString(amx, params[1]) )
	{
		section_name = Lux_PawnEntity_GetString(amx, params[1]);

		lux::world->LoadSection( section_name, false );
	}

	return 0;
}

/** pawnSectionValid
* native SectionValid(section[] = SELF, gridx, gridy, maxlength = sizeof section );
*
*/
static cell AMX_NATIVE_CALL pawnSectionValid(AMX *amx, const cell *params)
{
	uint16_t map_id = 0xFF;
	std::string section = "";
	if ( Lux_PawnEntity_HasString(amx, params[1]) )
	{
		section = Lux_PawnEntity_GetString(amx, params[1]);
	}

	map_id = lux::world->GetMapID( section, params[2], params[3] );

	if ( map_id >= 0x00 && map_id < 0x1000 )
	{
		return 1;
	}
	return 0;
}



const AMX_NATIVE_INFO Maps_Natives[] = {
	/** Mask Functions */
	{ "MaskRefresh", pawnMaskRefresh},
	{ "MaskGetValue", pawnMaskGetValue},
	{ "MaskFill", pawnMaskFill},
	/** Map Functions */
	{ "MapCreate", pawnMapCreate},
	{ "MapChange", pawnMapChange},
	{ "MapValid", pawnMapValid},
	{ "MapCurrent", pawnMapCurrent},
	{ "MapIdent", pawnMapIdent},
	{ "MapID", pawnMapID},
	{ "MapSave", pawnMapSave},
	{ "MapSaveToFile", pawnMapSaveToFile},
	{ "MapReset", pawnMapReset },
	/** Map Functions */
	{ "MapSetOffset", pawnMapSetOffset},
	{ "MapGetOffset", pawnMapGetOffset},
	{ "MapGridValid", pawnDeprecatedFunction},
	{ "MapGetGrid", pawnDeprecatedFunction},
	{ "MapGetFromGrid", pawnDeprecatedFunction},
	{ "MapSetFromGrid", pawnDeprecatedFunction},
	{ "MapGetID", pawnDeprecatedFunction},
	{ "MapSet", pawnDeprecatedFunction},
	/** Map collision Functions */
	{ "MapCollisionCurrent", pawnMapCollisionCurrent },
	{ "MapCollisionList", pawnMapCollisionList },
	/** Map Object Functions */
	{ "MapObjectCurrent", pawnDeprecatedFunction },
	{ "MapObjectList", pawnDeprecatedFunction },
	/** Section Functions */
	{ "SectionSet", pawnSectionSet },
	{ "SectionGet", pawnSectionGet },
	{ "SectionValid", pawnSectionValid },
	{ "SectionLoad", pawnSectionLoad },
	{ 0, 0 }
};
