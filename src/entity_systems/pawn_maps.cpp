/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "audio.h"
#include "config.h"
#include "core.h"
#include "display/display.h"
#include "engine.h"
#include "mokoi_game.h"
#include "entity_manager.h"
#include "pawn_helper.h"
#include "game_system.h"
#include "elix/elix_string.hpp"

#include "ffi_mask.h"
#include "ffi_map.h"
#include "ffi_world.h"

extern const AMX_NATIVE_INFO Maps_Natives[];


/* Mask Functions */
/**
 * @brief pawnMaskRefresh
 * @param amx
 * @param params ()
 * @return
 */
static cell pawnMaskRefresh(AMX *amx, const cell *params )
{
	return Lux_FFI_Mask_Refresh();
}

/**
 * @brief pawnMaskGetValue
 * @param amx
 * @param params (x, y, layer = 0)
 * @return
 */
static cell pawnMaskGetValue(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 3 );
	return Lux_FFI_Mask_Value_Get((uint16_t) params[1], (uint16_t) params[2], 0);
}

/**
 * @brief pawnMaskFill
 * @param amx
 * @param params (x, y, w, h, value = 0)
 * @return
 */
static cell pawnMaskFill(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 5 );

	uint16_t x = (uint16_t) params[1];
	uint16_t y = (uint16_t) params[2];
	uint16_t w = (uint16_t) params[3];
	uint16_t h = (uint16_t) params[4];
	uint8_t	value = (uint8_t) params[5];

	return Lux_FFI_Mask_Value_Fill( x, y, w, h, value );
}

/* Map Functions */
/**
 * @brief pawnMapSetOffset
 * @param amx
 * @param params (Fixed:x, Fixed:y)
 * @return
 */
static cell pawnMapSetOffset(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 2 );
	Lux_FFI_Map_Offset_Set( (int32_t)params[1], (int32_t)params[2] );
	return 0;
}

/**
 * @brief pawnMapGetOffset
 * @param amx
 * @param params (axis)
 * @return
 */
static cell pawnMapGetOffset(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 1 );
	int32_t result = (cell)Lux_FFI_Map_Offset_Get( (uint8_t) params[1] );

	return result;
}

/* File Code */
/**
 * @brief pawnMapCurrentIdent
 * @param amx
 * @param params
 * @return
 */
static cell pawnMapCurrentIdent(AMX *amx, const cell *params)
{
	cell responed = 0;
	if ( lux::game_system->active_map != NULL )
	{
		responed = lux::game_system->active_map->Ident();
	}
	return responed;

}

/**
 * @brief pawnMapCurrentGrid
 * @param amx
 * @param params ( &w, &h )
 * @return
 */
static cell pawnMapCurrentGrid(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 2 );

	cell responed = 0;
	if ( lux::game_system->active_section != NULL )
	{
		write_amx_address( amx, params[1], MAKE_FIXED_INT(lux::game_system->grid_position[0]) );
		write_amx_address( amx, params[2], MAKE_FIXED_INT(lux::game_system->grid_position[1]) );
		responed = 1;
	}
	return responed;
}

/**
 * @brief pawnMapCurrentDimension
 * @param amx
 * @param params ( &w, &h )
 * @return
 */
static cell pawnMapCurrentDimension(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 2 );

	cell responed = 0;
	if ( lux::game_system->active_map != NULL )
	{
		write_amx_address( amx, params[1], MAKE_FIXED_INT(lux::game_system->active_map->map_width) );
		write_amx_address( amx, params[2], MAKE_FIXED_INT(lux::game_system->active_map->map_height) );
		responed = 1;
	}
	return responed;
}

/**
 * @brief pawnMapSet
 * @param amx
 * @param params
 * @return
 */
static cell pawnMapSet(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 3 );

	uint32_t map_ident = (uint32_t)params[1];
	int32_t offset_x = (int32_t)params[2];
	int32_t offset_y = (int32_t)params[3];

	return Lux_FFI_Map_Set( map_ident, offset_x, offset_y );

}

/**
 * @brief pawnMapGetIdent
 * @param amx
 * @param params
 * @return
 */
static cell pawnMapGetIdent(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 2 );

	std::string map_file = Lux_PawnEntity_GetString( amx, params[1] );
	uint32_t ident = 0;
	if ( params[2] == 1 )
	{
		ident = Lux_FFI_Map_Create( map_file.c_str() );
	}
	else if ( params[2] == -1 )
	{
		ident = Lux_FFI_Map_Find_Ident( map_file.c_str() );
		if ( ident == 0)
		{
			ident = Lux_FFI_Map_Create( map_file.c_str() );
		}
	}
	else
	{
		ident = Lux_FFI_Map_Find_Ident( map_file.c_str() );
	}

	return ident;
}

/**
 * @brief pawnMapReset
 * @param amx
 * @param params
 * @return
 */
static cell pawnMapReset(AMX *amx, const cell *params)
{
	int32_t result = (cell)Lux_FFI_Map_Reset( );

	return result;
}

/**
 * @brief pawnMapSnapshot
 * @param amx
 * @param params
 * @return
 */
static cell pawnMapSnapshot(AMX *amx, const cell *params)
{
	int32_t result = (cell)Lux_FFI_Map_Snapshot( );

	return result;
}

/**
 * @brief pawnMapDelete
 * @param amx
 * @param params
 * @return
 */
static cell pawnMapDelete(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 1 );

	uint32_t map_ident = (uint32_t)params[1];

	return Lux_FFI_Map_Delete( map_ident );
}

/* Map Editing */

/**
 * @brief pawnMapEditNew
 * @param amx
 * @param params
 * @return
 */
static cell pawnMapEditNew(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 4 );

	uint32_t results = 0;

	std::string map_file = Lux_PawnEntity_GetString(amx, params[1]);
	uint32_t section_hash = (uint32_t)params[2];
	uint32_t width = (int32_t)params[3];
	uint32_t height = (int32_t)params[4];

	results = Lux_FFI_Map_Edit_New( map_file.c_str(), section_hash, width, height );

	return results;
}

/**
 * @brief pawnMapEditSave
 * @param amx
 * @param params
 * @return
 */
static cell pawnMapEditSave(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 1 );

	uint32_t results = 0;
	uint32_t map_ident = (uint32_t)params[1];

	results = Lux_FFI_Map_Edit_Save( map_ident );

	return results;
}

/**
 * @brief pawnMapEditObjectCreate
 * @param amx
 * @param params
 * @return
 */
static cell pawnMapEditObjectCreate(AMX *amx, const cell *params )
{
	ASSERT_PAWN_PARAM( amx, params, 9 );

	uint32_t results = 0;

	uint32_t map_ident = (uint32_t)params[1];
	std::string sprite = Lux_PawnEntity_GetString(amx, params[2]);
	uint8_t type = (uint8_t)params[3];

	int32_t x = params[4];
	int32_t y = params[5];
	int32_t z = params[6];
	uint16_t w = params[7];
	uint16_t h = params[8];
	uint32_t colour = params[9];

	results = Lux_FFI_Map_Edit_Object_Create( map_ident, type, x, y, z, w,  h, colour, sprite.c_str() );

	return results;

}

/**
 * @brief pawnMapEditObjectPosition
 * @param amx
 * @param params
 * @return
 */
static cell pawnMapEditObjectPosition(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 7 );

	uint32_t map_ident = (uint32_t)params[1];
	uint32_t object_id = params[2];
	int32_t x = params[3];
	int32_t y = params[4];
	int32_t z = params[5];
	uint16_t w = params[6];
	uint16_t h = params[7];

	return Lux_FFI_Map_Edit_Object_Postion( map_ident, object_id, x, y, z, w, h );
}


/**
 * @brief pawnMapEditObjectEffect
 * @param amx
 * @param params
 * @return
 */
static cell pawnMapEditObjectEffect(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 9 );

	uint32_t map_ident = (uint32_t)params[1];
	uint32_t object_id = params[2];
	uint32_t colour1 = params[3];
	uint32_t colour2 = params[9];

	uint16_t rotate = params[4];
	uint16_t scale_w = params[5];
	uint16_t scale_h = params[6];
	uint8_t flipmode = params[7];
	uint8_t style = params[8];

	return (cell)Lux_FFI_Map_Edit_Object_Effect( map_ident, object_id, colour1, colour2, rotate, scale_w, scale_h, flipmode, style );
}

/**
 * @brief pawnMapEditObjectReplace
 * @param amx
 * @param params
 * @return
 */
static cell pawnMapEditObjectReplace(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 4 );


	uint32_t map_ident = (uint32_t)params[1];
	uint32_t object_id = (uint32_t)params[2];
	std::string sprite = Lux_PawnEntity_GetString(amx, params[3]);
	uint8_t type = (uint8_t)params[4];

	return (cell)Lux_FFI_Map_Edit_Object_Replace( map_ident, object_id, type, sprite.c_str() );
}

/**
 * @brief pawnMapEditObjectFlag
 * @param amx
 * @param params
 * @return
 */
static cell pawnMapEditObjectFlag(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 4 );

	uint32_t map_ident = (uint32_t)params[1];
	uint32_t object_id = (uint32_t)params[2];

	uint8_t key = (uint8_t)params[3];
	int16_t value = (int16_t)params[4];

	return (cell)Lux_FFI_Map_Edit_Object_Flag( map_ident, object_id, key, value );
}

/**
 * @brief pawnMapEditObjectDelete
 * @param amx
 * @param params
 * @return
 */
static cell pawnMapEditObjectDelete(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 2 );

	uint32_t map_ident = (uint32_t)params[1];
	uint32_t object_id = (uint32_t)params[2];

	return (cell)Lux_FFI_Map_Edit_Object_Delete( map_ident, object_id );

}

/* Map Collision Functions */

/**
 * @brief pawnMapCollisionList
 * @param amx
 * @param params
 * @return
 */
static cell pawnMapCollisionList(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 3 );
	return -1;
}

/**
 * @brief pawnMapCollisionCurrent
 * @param amx
 * @param params
 * @return
 */
static cell pawnMapCollisionCurrent(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 3 );
	return -1;
}

/* World Functions */

/**
 * @brief WorldSet( section_hash, grid_x, grid_y )
 * @param amx
 * @param params
 * @return
 */
static cell pawnWorldSet(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 3 );

	cell result = 0;
	uint32_t section_hash = (uint32_t)params[1];
	uint8_t grid_x = (uint8_t)params[2];
	uint8_t grid_y = (uint8_t)params[3];

	result = (cell)Lux_FFI_World_Set( section_hash, grid_x, grid_y );

	return result;
}

/**
 * @brief pawnWorldGet
 * @param amx
 * @param params
 * @return Map ID for selected location
 */
static cell pawnWorldGetIdent(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 3 );

	cell result = 0;
	uint32_t section_hash = (uint32_t)params[1];
	uint8_t grid_x = (uint8_t)params[2];
	uint8_t grid_y = (uint8_t)params[3];

	result = (cell)Lux_FFI_World_Get( section_hash, grid_x, grid_y );

	return result;
}

/**
 * @brief pawnWorldExist
 * @param amx
 * @param params
 * @return
 */
static cell pawnWorldExist(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 1 );

	cell result = 0;
	std::string section_name = Lux_PawnEntity_GetString( amx, params[1] );

	result = (cell)Lux_FFI_World_Exist( section_name.c_str(), false );

	return result;
}

/**
 * @brief pawnWorldLoad
 * @param amx
 * @param params
 * @return
 */
static cell pawnWorldLoad(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 1 );

	cell result = 0;
	std::string section_name = Lux_PawnEntity_GetString( amx, params[1] );

	result = (cell)Lux_FFI_World_Exist( section_name.c_str(), true );

	return result;
}


/**
 * @brief pawnWorldEditNew
 * @param amx
 * @param params
 * @return
 */
static cell pawnWorldEditNew(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 1 );

	cell result = 0;
	char * section_name = NULL;
	uint8_t width = (uint8_t)params[2];
	uint8_t height = (uint8_t)params[3];

	result = (cell)Lux_FFI_World_Edit_New( section_name, width, height );

	return result;
}

/**
 * @brief pawnWorldEditSet
 * @param amx
 * @param params
 * @return
 */
static cell pawnWorldEditSet(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 1 );

	cell result = 0;
	uint32_t section_hash = (uint32_t)params[1];
	uint32_t map_ident = (uint32_t)params[2];
	uint8_t grid_x = (uint8_t)params[3];
	uint8_t grid_y = (uint8_t)params[4];

	result = (cell)Lux_FFI_World_Edit_Set( section_hash, map_ident, grid_x, grid_y );

	return result;
}

/**
 * @brief pawnWorldEditSave
 * @param amx
 * @param params
 * @return
 */
static cell pawnWorldEditSave(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 1 );

	cell result = 0;
	uint32_t section_hash = (uint32_t)params[1];

	result = (cell)Lux_FFI_World_Edit_Save( section_hash );

	return result;
}

const AMX_NATIVE_INFO Maps_Natives[] = {
	/* Mask Functions */
	{ "MaskRefresh", pawnMaskRefresh},
	{ "MaskGetValue", pawnMaskGetValue},
	{ "MaskFill", pawnMaskFill},

	/* Map Offset Functions */
	{ "MapOffsetSet", pawnMapSetOffset}, //MapSetOffset(Fixed:x, Fixed:y)
	{ "MapOffsetGet", pawnMapGetOffset}, //MapGetOffset(axis)

	/* Map Functions */
	{ "MapCurrentIdent", pawnMapCurrentIdent }, // MapCurrentIdent( )
	{ "MapCurrentDimension", pawnMapCurrentDimension }, // MapCurrentDimension(&w, &h )
	{ "MapCurrentGrid", pawnMapCurrentGrid }, // MapCurrentGrid(&w, &h )
	{ "MapSet", pawnMapSet}, //MapSet(map_ident, offset_x, offset_y )
	{ "MapGetIdent", pawnMapGetIdent}, //MapGetIdent( map_file[], create_new )
	{ "MapReset", pawnMapReset}, //MapReset()
	{ "MapSnapshot", pawnMapSnapshot}, //MapSnapshot()
	{ "MapDelete", pawnMapDelete}, //MapDelete( map_ident )

	/* Map Edit Functions */
	{ "MapEditNew", pawnMapEditNew}, //MapEditNew( map_file{}, section_hash, width, height)
	{ "MapEditSave", pawnMapEditSave}, //MapEditSave( map_ident )
	{ "MapEditObjectCreate", pawnMapEditObjectCreate}, //MapEditObjectCreate( map_ident, type, x, y, z, w, h, colour, sprite[] )
	{ "MapEditObjectPosition", pawnMapEditObjectPosition}, //MapEditObjectPostion( map_ident, object_id, x, y, z, w, h )

	{ "MapEditObjectEffect", pawnMapEditObjectEffect}, //MapEditObjectEffect( map_ident, object_id, primary_colour, secondary_colour, rotation, scale_xaxis, scale_yaxis, flip_image, style )
	{ "MapEditObjectReplace", pawnMapEditObjectReplace}, //MapEditObjectReplace( map_ident, object_id, type, sprite[] )
	{ "MapEditObjectFlag", pawnMapEditObjectFlag}, //MapEditObjectFlag( map_ident, object_id, key, value );
	{ "MapEditObjectDelete", pawnMapEditObjectDelete}, //MapEditObjectDelete( map_ident, object_id )

	/* Map collision Functions */
	{ "MapCollisionCurrent", pawnMapCollisionCurrent },
	{ "MapCollisionList", pawnMapCollisionList },

	/* Map Object Functions */
	{ "MapObjectCurrent", pawnDeprecatedFunction },
	{ "MapObjectList", pawnDeprecatedFunction },

	/* World Functions */
	{ "WorldSet", pawnWorldSet }, //WorldSet( section_hash, grid_x, grid_y )
	{ "WorldGetIdent", pawnWorldGetIdent }, //WorldGetIdent( section_hash, grid_x, grid_y )
	{ "WorldExist", pawnWorldExist }, //WorldExist( section_name{} )
	{ "WorldLoad", pawnWorldLoad }, //WorldExist( section_name{} )

	/* World Edit */
	{ "WorldEditNew", pawnWorldEditNew }, //WorldEditNew( section_name{}, width, height )
	{ "WorldEditSet", pawnWorldEditSet }, //WorldEditSet( section_hash, map_ident, x, y )
	{ "WorldEditSave", pawnWorldEditSave }, //WorldEditSave( section_hash )
	{ 0, 0 }
};
