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
#include "display.h"
#include "engine.h"
#include "entity_manager.h"
#include "pawn_helper.h"
#include "game_system.h"
#include "misc_functions.h"
#include "display_functions.h"
#include "elix_endian.hpp"
#include "elix_string.hpp"

#include "ffi_object.h"
#include "ffi_spritesheet.h"
#include "ffi_path.h"
#include "ffi_layer.h"
#include "ffi_shaders.h"

extern const AMX_NATIVE_INFO Graphics_Natives[];

/**
 * @brief Lux_PawnEntity_GetObject
 * @param amx
 * @param object_id
 * @return
 */
MapObject * Lux_PawnEntity_GetObject(AMX * amx, uint32_t object_id )
{
	MapObject * object = NULL;
	Entity * wanted = Lux_PawnEntity_GetParent(amx);
	if ( wanted == NULL )
		return object;

	if ( lux::gamesystem )
	{
		object = lux::gamesystem->GetObject(object_id);
	}
	return object;
}

/** Sheet Functions */

/**
 * @brief pawnSheetReference
 * @param amx
 * @param params (sheet{}, rev)
 * @return
 */
static cell pawnSheetReference(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 2 );

	std::string sheet_name = Lux_PawnEntity_GetString(amx, params[1]);

	Lux_FFI_Sheet_Reference( sheet_name.c_str(), (int8_t)params[2] );

	return 0;
}


/**
 * @brief pawnSheetReplace
 * @param amx
 * @param params (old[], new[])
 * @return
 */
static cell pawnSheetReplace(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 2 );

	std::string old_sheet = Lux_PawnEntity_GetString(amx, params[1]);
	std::string new_sheet = Lux_PawnEntity_GetString(amx, params[2]);

	if ( old_sheet.length() && new_sheet.length() )
	{
		Lux_FFI_Sheet_Replace( old_sheet.c_str(), new_sheet.c_str() );
	}
	return 0;
}

/**
 * @brief pawnSheetSpriteDimension
 * @param amx
 * @param params (sprite[], &w, &h)
 * @return
 */
static cell pawnSheetSpriteDimension(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 3 );

	cell responed = 0;
	std::string request_string = Lux_PawnEntity_GetString(amx, params[1]);

	if ( request_string.size() )
	{
		LuxSprite * sdata = lux::display->GetSprite( request_string );
		if ( sdata )
		{
			write_amx_address( amx, params[2], sdata->sheet_area.w );
			write_amx_address( amx, params[3], sdata->sheet_area.h );
			responed = 1;
		}
	}
	return responed;
}

/** Graphics Functions */


/**
 * @brief pawnGraphicsDraw
 * @param amx
 * @param params
 * @return
 */
static cell pawnGraphicsDraw(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 8 );

	MapObject * new_object = new MapObject();
	new_object->sprite = Lux_PawnEntity_GetString(amx, params[1]);
	new_object->type = (uint8_t)params[2];
	new_object->position.x = params[3];
	new_object->position.y = params[4];
	new_object->SetZPos( (fixed)params[5] );
	new_object->position.w = params[6];
	new_object->position.h = params[7];

	cell_colour colour;
	colour.hex = elix::endian::host32(params[8]);
	new_object->effects.primary_colour = colour.rgba;

	if ( lux::display->AddObjectToLayer(new_object->layer, new_object, false) )
	{
		return 1;
	}
	delete new_object;

	return 0;
}

/** Animation */

/** pawnAnimationGetLength
* native AnimationGetLength(sheet[], anim[]);
*/
static cell pawnAnimationGetLength(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 2 );

	std::string sheet_name = Lux_PawnEntity_GetString(amx, params[1]);
	std::string sprite_name = Lux_PawnEntity_GetString(amx, params[2]);

	return Lux_FFI_Animation_Length( sheet_name.c_str(), sprite_name.c_str() );
}

/** pawnAnimationCreate
* native AnimationCreate(string[], length);
*/
static cell pawnAnimationCreate(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 1 );

	std::string animation_name = Lux_PawnEntity_GetString(amx, params[1]);

	return Lux_FFI_Animation_Create( animation_name.c_str() );
}

/** pawnAnimationAddFrame
* native AnimationAddFrame(string[], sprite[], string_size = sizeof string, sprite_size = sizeof sprite);
*/
static cell pawnAnimationAddFrame(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 3 );

	std::string animation_name = Lux_PawnEntity_GetString(amx, params[1]);
	std::string sprite_name = Lux_PawnEntity_GetString(amx, params[2]);

	return Lux_FFI_Animation_Insert( animation_name.c_str(), sprite_name.c_str(), params[3] );
}

/** Text */

/** pawnTextSprites
* native TextSprites(able);

*/
static cell pawnTextSprites(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 2 );

	bool enable_text_font = !!params[1];
	std::string new_text_sheet = Lux_PawnEntity_GetString(amx, params[2]);

	Lux_FFI_Text_Sprites( enable_text_font, new_text_sheet.c_str() );

	return (cell)enable_text_font;
}

/** Polygon */

/** pawnPolygonCreate
* native PolygonCreate(string[]);
*/
static cell pawnPolygonCreate(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 1 );
	std::string name = Lux_PawnEntity_GetString(amx, params[1]);

	return Lux_FFI_Polygon_Create( name.c_str() );
}

/** pawnPolygonAddpoint
* native PolygonAddpoint(string[], x, y, string_size = sizeof string);

*/
static cell pawnPolygonAddpoint(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 3 );

	std::string name = Lux_PawnEntity_GetString(amx, params[1]);
	return Lux_FFI_Polygon_Add_Point( name.c_str(), params[2], params[3] );

}


/** pawnCanvasChildInfo
* native CanvasChildInfo(object_id, child{}, &x, &y, &w, &h);

*/
static cell pawnCanvasChildInfo(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 6 );

	std::string name = Lux_PawnEntity_GetString(amx, params[2]);

	cell * xptr, * yptr;
	uint16_t w;
	uint16_t h;
	int32_t y;
	int32_t x;
	uint32_t object_id = params[1];
	uint32_t child_id = elix::string::Hash(name);

	child_id = Lux_FFI_Canvas_Child_Info( object_id, child_id, &x, &y, &w, &h  );
	if ( child_id )
	{
		xptr = amx_Address(amx, params[3]);
		yptr = amx_Address(amx, params[4]);
		if ( xptr )
			*xptr = x;

		if ( yptr )
			*yptr = y;

		xptr = amx_Address(amx, params[5]);
		yptr = amx_Address(amx, params[6]);
		if ( xptr )
			*xptr = w;

		if ( yptr )
			*yptr = h;


		return 1;
	}


	return Lux_FFI_Polygon_Add_Point( name.c_str(), params[2], params[3] );

}
/** Display Functions */

/** pawnObjectCreate
* native ObjectCreate(string[], type, x, y, z, w, h, c = 0xFFFFFFFF, global = false, string_size = sizeof string);
*
*/
static cell pawnObjectCreate(AMX *amx, const cell *p )
{
	ASSERT_PAWN_PARAM( amx, p, 9 );

	uint32_t results = 0;
	std::string sprite;

	sprite = Lux_PawnEntity_GetString(amx, p[1]);

	results = Lux_FFI_Object_Create( (uint8_t)p[9], p[2], p[3], p[4], (int32_t)p[5], (uint16_t)p[6], (uint16_t)p[7], p[8], sprite.c_str() );

	return results;

}

/** pawnObjectPosition
* native ObjectPosition(id, x, y, z, w, h, pos = POSITION_MAP);
*/
static cell pawnObjectPosition(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 7 );

	uint32_t object_id = params[1];
	int32_t x = params[2];
	int32_t y = params[3];
	int32_t z = params[4];
	uint16_t w = params[5];
	uint16_t h = params[6];

	return Lux_FFI_Object_Postion( object_id, x, y, z, w, h );
}

/** pawnObjectInfo
* native ObjectInfo(id, &w, &h);
*/
static cell pawnObjectInfo(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 3 );

	cell * xptr, * yptr;
	uint16_t w;
	uint16_t h;
	int32_t y;
	int32_t x;
	uint32_t object_id = params[1];

	if ( Lux_FFI_Object_Info( object_id, &w, &h, &x, &y ) )
	{
		xptr = amx_Address(amx, params[2]);
		yptr = amx_Address(amx, params[3]);

		if ( xptr )
			*xptr = w;

		if ( yptr )
			*yptr = h;


		xptr = amx_Address(amx, params[4]);
		yptr = amx_Address(amx, params[5]);

		if ( xptr )
			*xptr = x;

		if ( yptr )
			*yptr = y;

		return 1;
	}
	return 0;
}

/** pawnObjectEffect
* native ObjectEffect(id, colour, rotate, scale_w, scale_h, flipmode, _STYLES:style, colour2 = 0xFFFFFFFF);
*/
static cell pawnObjectEffect(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 8 );

	uint32_t object_id = params[1];
	uint32_t colour1 = params[2];
	uint32_t colour2 = params[8];

	uint16_t rotate = params[3];
	uint16_t scale_w = params[4];
	uint16_t scale_h = params[5];
	uint8_t flipmode = params[6];
	uint8_t style = params[7];

	return Lux_FFI_Object_Effect( object_id, colour1, colour2, rotate, scale_w, scale_h, flipmode, style );

}

/** pawnObjectReplace
* native ObjectReplace(id, string[], type);
*/
static cell pawnObjectReplace(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 3 );

	std::string sprite;
	uint32_t object_id = (uint32_t)params[1];
	uint8_t type = (uint8_t)params[3];

	sprite = Lux_PawnEntity_GetString(amx, params[2]);

	return Lux_FFI_Object_Replace( object_id, type, sprite.c_str() );
}


/** pawnObjectFlag
* native ObjectFlag(id, key, value);
*/
static cell pawnObjectFlag(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 3 );

	uint32_t object_id = (uint32_t)params[1];

	return Lux_FFI_Object_Flag( object_id, (uint8_t)params[2], (int16_t)params[3] );

}

/** pawnObjectDelete
* native ObjectDelete(id);
*/
static cell pawnObjectDelete(AMX *amx, const cell *params)
{
	ASSERT_PAWN_PARAM( amx, params, 1 );

	uint32_t object_id = (uint32_t)params[1];

	return Lux_FFI_Object_Delete( object_id );

}


/** Camera Functions */
/** pawnCameraSetScroll
* native CameraSetScroll(bool:scroll);
*/
static cell pawnCameraSetScroll(AMX *amx, const cell *params)
{
	lux::gamesystem->active_map->SetScrolling( (bool)params[1] );
	return 0;
}


/** pawnLayerSetRotation
* native LayerSetRotation(layer, roll, pitch, yaw);
*/
static cell pawnLayerSetRotation(AMX *amx, const cell *params)
{
	Lux_FFI_Layer_Rotation( params[1], params[2], params[3], params[4] );

	return 0;
}

/** pawnLayerSetOffset
* native LayerSetOffset(layer, x, y);
*/
static cell pawnLayerSetOffset(AMX *amx, const cell *params)
{
	Lux_FFI_Layer_Offset( params[1], params[2], params[3] );

	return 0;
}

/** pawnLayerSetEffect
* native LayerSetEffect(layer, x );
*/
static cell pawnLayerSetEffect(AMX *amx, const cell *params)
{
	Lux_FFI_Layer_Apply_Shader( params[1], params[2] );

	return 0;
}

/** pawnLayerColour
* native LayerColour(layer, colour);
*/
static cell pawnLayerColour(AMX *amx, const cell *params)
{
	Lux_FFI_Layer_Colour( params[1], params[2] );
	return 0;
}


const AMX_NATIVE_INFO Graphics_Natives[] = {
	/** Misc */
	{ "GraphicsDraw", pawnGraphicsDraw },
	{ "TextSprites", pawnTextSprites }, ///native TextSprites(able);

	/** Sheet */
	{ "SheetReference", pawnSheetReference }, ///native SheetReference(sheet[], ref = 1);
	{ "SheetReplace", pawnSheetReplace }, ///native SheetReplace(old[], new[]);
	{ "SheetSpriteDimension", pawnSheetSpriteDimension }, ///native SheetSpriteDimension(sprite[], &w, &h);

	/** Animation */
	{ "AnimationGetLength", pawnAnimationGetLength }, ///native AnimationGetLength(sheet[], anim[]);
	{ "AnimationCreate", pawnAnimationCreate }, ///native AnimationCreate(string[]);
	{ "AnimationAddFrame", pawnAnimationAddFrame }, ///native AnimationAddFrame(string[], sprite[], string_size = sizeof string, sprite_size = sizeof sprite);
	{ "AnimationDelete", pawnDeprecatedFunction }, ///native AnimationDelete(string[]);

	/** Polygon */
	{ "PolygonCreate", pawnPolygonCreate }, ///native PolygonCreate(string[]);
	{ "PolygonAddPoint", pawnPolygonAddpoint }, ///native PolygonAddpoint(string[], x, y, string_size = sizeof string);
	{ "PolygonDelete", pawnDeprecatedFunction }, ///native AnimationDelete(string[]);

	/** Polygon */
	{ "CanvasChildInfo", pawnCanvasChildInfo },

	/** Display Functions */
	{ "ObjectCreate", pawnObjectCreate }, ///native ObjectCreate(string[], type, c = 0xFFFFFFFF, string_size = sizeof string);
	{ "ObjectPosition", pawnObjectPosition }, ///native ObjectPosition(id, x, y, z, w, h);
	{ "ObjectEffect", pawnObjectEffect }, ///native ObjectEffect(id, colour = 0xFFFFFFFF, rotate, scale_w, scale_h, mirror, _STYLES:style, color2 = 0xFFFFFFFF);
	{ "ObjectReplace", pawnObjectReplace }, ///native ObjectReplace(id, string[], type, string_size = sizeof string);
	{ "ObjectDelete", pawnObjectDelete }, ///native ObjectDelete(id);
	{ "ObjectFlag", pawnObjectFlag }, ///native ObjectFlag(object:id, key, value);
	{ "ObjectInfo", pawnObjectInfo }, ///native ObjectInfo(object:id, &w, &h);


	/** Camera Functions */
	{ "CameraSetScroll", pawnCameraSetScroll }, ///native CameraSetScroll(bool:scroll);
	{ "CameraSetZoom", pawnDeprecatedFunction }, ///native CameraSetZoom(zoom);
	{ "LayerSetRotation", pawnLayerSetRotation }, ///native LayerSetRotation(layer, roll, pitch, yaw);
	{ "LayerSetOffset", pawnLayerSetOffset }, ///native LayerSetOffset(layer, x, y);
	{ "LayerSetEffect", pawnLayerSetEffect }, ///native LayerSetEffect(layer, effect);
	{ "LayerColour", pawnLayerColour }, ///native LayerColour(layer, colour = 0xFFFFFFFF);

	/** Shader Function */



	{ 0, 0 }
};
