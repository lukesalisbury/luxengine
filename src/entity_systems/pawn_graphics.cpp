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
#include "entity_manager.h"
#include "pawn_helper.h"
#include "world.h"
#include "misc_functions.h"
#include "display_functions.h"
#include "elix_endian.hpp"

#include "ffi_object.h"

extern const AMX_NATIVE_INFO Graphics_Natives[];


MapObject * Lux_PawnEntity_GetObject(AMX * amx, uint32_t object_id )
{
	MapObject * object = NULL;
	Entity * wanted = Lux_PawnEntity_GetParent(amx);
	if ( wanted == NULL )
		return object;

	if ( lux::world->active_map )
	{
		if ( wanted->_mapid == 0 )
			object = lux::world->GetObject(object_id);
		else
			object = lux::world->active_map->GetObject(object_id);
	}
	return object;
}





/** Misc Functions */

/** pawnGraphicsType
* native GraphicsType();
*/
static cell AMX_NATIVE_CALL pawnGraphicsType(AMX *amx, const cell *params)
{
	return 0;
}

/** pawnSheetRef
* native SheetReference(sheet[], rev);
*/
static cell AMX_NATIVE_CALL pawnSheetRef(AMX *amx, const cell *params)
{
	std::string sheet_name = Lux_PawnEntity_GetString(amx, params[1]);
	if ( sheet_name.length() )
	{
		if ( lux::display )
		{
			if ( params[2] == -1 )
				lux::display->UnrefSheet( sheet_name );
			else
				lux::display->RefSheet( sheet_name );
		}
	}
	return 0;
}

/** pawnSheetReplace
* native SheetReplace(old[], new[]);
*/
static cell AMX_NATIVE_CALL pawnSheetReplace(AMX *amx, const cell *params)
{
	std::string old_sheet = Lux_PawnEntity_GetString(amx, params[1]);
	std::string new_sheet = Lux_PawnEntity_GetString(amx, params[2]);
	if ( old_sheet.length() && new_sheet.length() )
	{
		if ( lux::world->active_map )
		{
			lux::world->active_map->ReplaceObjectsSheets( old_sheet, new_sheet );
		}
	}
	return 0;
}

/** pawnGraphicsDraw
* native GraphicsDraw(string[], type, x, y, z, w, h, c = 0xFFFFFFFF, string_size = sizeof string );
*/
static cell AMX_NATIVE_CALL pawnGraphicsDraw(AMX *amx, const cell *params)
{
	MapObject * new_object = new MapObject();
	new_object->sprite = Lux_PawnEntity_GetString(amx, params[1]);
	new_object->type = (uint8_t)params[2];
	new_object->position.x = params[3];
	new_object->position.y = params[4];
	new_object->SetZPos( (int32_t)params[5] );
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
static cell AMX_NATIVE_CALL pawnAnimationGetLength(AMX *amx, const cell *params)
{
	char * sprite_name, * sheet_name;
	amx_StrParam_Type(amx, params[1], sheet_name, char*);
	amx_StrParam_Type(amx, params[2], sprite_name, char*);
	return (cell)lux::display->isAnimation(sheet_name, sprite_name);
}

/** pawnAnimationCreate
* native AnimationCreate(string[], length);
*/
static cell AMX_NATIVE_CALL pawnAnimationCreate(AMX *amx, const cell *params)
{

	return -1;
}

/** pawnAnimationAddFrame
* native AnimationAddFrame(string[], sprite[], string_size = sizeof string, sprite_size = sizeof sprite);
*/
static cell AMX_NATIVE_CALL pawnAnimationAddFrame(AMX *amx, const cell *params)
{

	return -1;
}

/** Text */

/** pawnTextSprites
* native TextSprites(able);

*/
static cell AMX_NATIVE_CALL pawnTextSprites(AMX *amx, const cell *params)
{
	bool enable_text_font = !!params[1];
	std::string new_text_sheet = Lux_PawnEntity_GetString(amx, params[2]);

	lux::display->SetTextFont( enable_text_font, new_text_sheet );

	return (cell)enable_text_font;
}

/** Polygon */

/** pawnPolygonCreate
* native PolygonCreate(string[]);
*/
static cell AMX_NATIVE_CALL pawnPolygonCreate(AMX *amx, const cell *params)
{
	return -1;
}

/** pawnPolygonAddpoint
* native PolygonAddpoint(string[], x, y, string_size = sizeof string);

*/
static cell AMX_NATIVE_CALL pawnPolygonAddpoint(AMX *amx, const cell *params)
{

	return -1;
}

/** Display Functions */

/** pawnObjectCreate
* native ObjectCreate(string[], type, x, y, z, w, h, c = 0xFFFFFFFF, global = false, string_size = sizeof string);
*
*/
static cell AMX_NATIVE_CALL pawnObjectCreate(AMX *amx, const cell *p )
{
	uint32_t results = 0;
	std::string sprite;

	sprite = Lux_PawnEntity_GetString(amx, p[1]);

	results = Lux_FFI_Object_Create( (uint8_t)p[9], p[2], p[3], p[4], (int32_t)p[5], (uint16_t)p[6], (uint16_t)p[7], p[8], sprite.c_str() );

	return results;

}

/** pawnObjectPosition
* native ObjectPosition(id, x, y, z, w, h, pos = POSITION_MAP);
*/
static cell AMX_NATIVE_CALL pawnObjectPosition(AMX *amx, const cell *params)
{
	if ( params[1] == 0 )
		return -3;

	MapObject * object = Lux_PawnEntity_GetObject( amx, (uint32_t) params[1] );
	if ( object )
	{
		object->position.x = params[2];
		object->position.y = params[3];
		object->position.w = params[5];
		object->position.h = params[6];

		if ( params[4] >= 0 )
		{
			uint8_t current_layer = object->layer;

			object->SetZPos((int32_t)params[4]);

			if ( current_layer != object->layer )
			{
				lux::display->RemoveObject(current_layer, object);
				lux::display->AddObjectToLayer(object->layer, object, true);
			}
			else
			{
				lux::display->ResortLayer(object->layer);
			}
		}

		return 1;

	}
	return 0;
}

/** pawnObjectInfo
* native ObjectInfo(id, &w, &h);
*/
static cell AMX_NATIVE_CALL pawnObjectInfo(AMX *amx, const cell *params)
{
	if ( params[1] < 0 )
		return 0;
	cell * xptr, * yptr;
	Entity * wanted = Lux_PawnEntity_GetParent(amx);
	if ( wanted == NULL )
		return -2;

	if ( lux::world->active_map )
	{
		MapObject * object = NULL;
		if ( wanted->_mapid == 0 )
			object = lux::world->GetObject(params[1]);
		else
			object = lux::world->active_map->GetObject(params[1]);
		if ( object )
		{
			xptr  = amx_Address(amx, params[2]);
			if (xptr )
				*xptr = object->position.w;
			yptr = amx_Address(amx, params[3]);
			if (yptr)
				*yptr = object->position.h;
			return 1;
		}
	}
	return 0;
}

/** pawnObjectEffect
* native ObjectEffect(id, colour, rotate, scale_w, scale_h, flipmode, _STYLES:style, colour2 = 0xFFFFFFFF);
*/
static cell AMX_NATIVE_CALL pawnObjectEffect(AMX *amx, const cell *params)
{
	if ( params[1] < 0 )
		return 0;
	Entity * wanted = Lux_PawnEntity_GetParent(amx);
	if ( wanted == NULL )
		return -2;

	if ( lux::world->active_map )
	{
		MapObject * object = NULL;
		if ( wanted->_mapid == 0 )
			object = lux::world->GetObject(params[1]);
		else
			object = lux::world->active_map->GetObject(params[1]);
		if ( object )
		{
			cell_colour colour;
			colour.hex = elix::endian::host32(params[2]);
			object->effects.primary_colour = colour.rgba;

			colour.hex = elix::endian::host32(params[8]);
			object->effects.secondary_colour = colour.rgba;

			object->effects.rotation = params[3];
			object->effects.scale_xaxis = params[4];
			object->effects.scale_yaxis = params[5];
			object->effects.flip_image = params[6];
			object->effects.style = params[7];
			return 1;
		}
	}
	return 0;
}

/** pawnObjectReplace
* native ObjectReplace(id, string[], type, string_size = sizeof string);
*/
static cell AMX_NATIVE_CALL pawnObjectReplace(AMX *amx, const cell *params)
{
	if ( params[1] < 0 )
		return 0;

	std::string name;
	uint32_t object_id = (uint32_t)params[1];
	uint8_t type = (uint8_t)params[3];
	Entity * wanted = Lux_PawnEntity_GetParent(amx);

	if ( wanted == NULL )
		return -3;

	if ( lux::world->active_map )
	{
		MapObject * object = NULL;
		if ( wanted->_mapid == 0 )
			object = lux::world->GetObject(object_id);
		else
			object = lux::world->active_map->GetObject(object_id);


		name = Lux_PawnEntity_GetString(amx, params[2]);

		if ( object )
		{
			// Check if we are really changing object
			if ( object->type == type )
			{
				if ( !object->sprite.compare(name) )
					return 0;
			}

			if ( object->type == OBJECT_SPRITE && type != OBJECT_SPRITE )
			{
				object->position.w = object->position.w ? object->position.w : 32;
				object->position.h = object->position.h ? object->position.h : 32;
			}

			object->FreeData();

			object->sprite = name;
			object->type = type;
			object->timer = 0;



			//object->SetData(NULL, object->type);

			return object->has_data ? 1 : -1;
		}
	}
	return -2;
}

/** pawnObjectToggle
* native ObjectToggle(id, show);
*/
static cell AMX_NATIVE_CALL pawnObjectToggle(AMX *amx, const cell *params)
{
	if ( params[1] < 0 )
		return 0;
	Entity * wanted = Lux_PawnEntity_GetParent(amx);
	if ( wanted == NULL )
		return -2;

	MapObject * object = NULL;
	if ( wanted->_mapid == 0 )
		object = lux::world->GetObject(params[1]);
	else
		object = lux::world->active_map->GetObject(params[1]);
	if ( object )
	{
		if ( object->hidden == params[2] )
			object->ToggleHidden();
	}
	return 0;
}


/** pawnObjectFlag
* native ObjectFlag(id, key, value);
*/
static cell AMX_NATIVE_CALL pawnObjectFlag(AMX *amx, const cell *params)
{
	if ( params[1] < 0 )
		return 0;

	Entity * wanted = Lux_PawnEntity_GetParent(amx);
	if ( wanted == NULL )
		return -2;

	MapObject * object = NULL;
	if ( wanted->_mapid == 0 )
		object = lux::world->GetObject(params[1]);
	else
		object = lux::world->active_map->GetObject(params[1]);
	if ( object )
	{
		switch ( params[2] )
		{
			case 0:
				object->speed = (int16_t)params[3];
				break;
			case 1:
				object->loop = (bool)params[3];
				break;
			case 2:
				object->reverse = (bool)params[3];
				break;
			case 4:
				object->timer = (int16_t)params[3];
				break;
			case 5:
				object->auto_delete = (bool)params[3];
				break;
		}
	}

	return 0;
}

/** pawnObjectDelete
* native ObjectDelete(id);
*/
static cell AMX_NATIVE_CALL pawnObjectDelete(AMX *amx, const cell *params)
{
	if ( params[1] < 0 )
		return 0;

	Entity * wanted = Lux_PawnEntity_GetParent(amx);
	if ( wanted == NULL )
		return -2;


	if ( wanted->_mapid == 0 )
		return lux::world->RemoveObject(params[1]);
	else
		return lux::world->active_map->RemoveObject(params[1]);

}

/** pawnObjectFollowPath
* native ObjectFollowPath(object:id, speed, &x, &y, loop);
*/
#include <cmath>
#define FIXEDPI  3.1415926535897932384626433832795
static cell AMX_NATIVE_CALL pawnObjectFollowPath(AMX *amx, const cell *params)
{
	if ( params[1] < 0 )
		return 0;
	Entity * wanted = Lux_PawnEntity_GetParent(amx);
	if ( wanted == NULL )
		return -2;

	MapObject * object = NULL;
	if ( wanted->_mapid == 0 )
		object = lux::world->GetObject(params[1]);
	else
		object = lux::world->active_map->GetObject(params[1]);

	if ( object )
	{
		float movex, movey, speed, angle = 0;
		LuxPath next, prev;
		uint16_t next_path_point = 0;

		speed = MAKE_FIXED_FLOAT(params[1]) * (float)lux::core->GetFrameDelta();
		if ( !object->_path.size() )
		{
			return -2;
		}

		if ( speed > 0.0 )
		{
			next_path_point = object->path_point+1;
			if ( params[5] ) // Loop
			{
				if ( object->path_point >= object->_path.size()-1 )
				{
					next_path_point = 0;
				}
			}
			else
			{
				if ( object->path_point >= object->_path.size()-1 )
					return object->path_point;
			}

			prev.x = object->position.x;
			prev.y = object->position.y;
			next = object->_path.at(next_path_point);
		}
		else if ( speed < 0.0 )
		{
			next_path_point = object->path_point-1;
			if ( params[5] ) // Loop
			{
				if ( (object->path_point == 0) && object->path_point > object->_path.size() )
				{
					next_path_point = object->_path.size()-1;
				}
			}
			else
			{
				if ( (object->path_point == 0) && object->path_point > object->_path.size() )
					return object->path_point;
			}

			prev.x = object->position.x;
			prev.y = object->position.y;
			prev = object->_path.at(object->path_point);
			next = object->_path.at(next_path_point);
		}
		else
			return -1;

		angle = atan2( (float)(prev.y - next.y), (float)(prev.x - next.x));
		movey = sin(angle) * speed;
		movex = cos(angle) * speed;
		object->path_current_x -= MAKE_FLOAT_FIXED(movex);
		object->path_current_y -= MAKE_FLOAT_FIXED(movey);

		object->position.x = MAKE_FIXED_INT(object->path_current_x);
		object->position.y = MAKE_FIXED_INT(object->path_current_y);

		if (object->position.x == next.x && object->position.y == next.y)
		{
			object->path_current_x = MAKE_INT_FIXED(object->position.x);
			object->path_current_y = MAKE_INT_FIXED(object->position.y);
			object->path_point = next_path_point;
		}

		cell * xptr = NULL;
		xptr = amx_Address(amx, params[3]);
		if (xptr)
			*xptr = object->position.x;
		xptr = amx_Address(amx, params[4]);
		if (xptr)
			*xptr = object->position.y;
		return object->path_point;
	}
	return -1;
}


/** Camera Functions */
/** pawnCameraSetScroll
* native CameraSetScroll(bool:scroll);
*/
static cell AMX_NATIVE_CALL pawnCameraSetScroll(AMX *amx, const cell *params)
{
	lux::world->active_map->SetScrolling( (bool)params[1] );
	return 0;
}

/** pawnLayerSetRotation
* native LayerSetRotation(layer, roll, pitch, yaw);
*/
static cell AMX_NATIVE_CALL pawnLayerSetRotation(AMX *amx, const cell *params)
{

	/* Todo
	*
	*/
	lux::display->ChangeLayerRotation(params[1], params[2], params[3], params[4]);
	return 0;
}

/** pawnLayerSetOffset
* native LayerSetOffset(layer, x, y);
*/
static cell AMX_NATIVE_CALL pawnLayerSetOffset(AMX *amx, const cell *params)
{
	if ( params[1] == -1)
		lux::display->SetCameraView( params[2], params[3] );
	else
		lux::display->SetCameraView( params[1], params[2], params[3] );
	return 0;
}

/** pawnLayerColour
* native LayerColour(layer, colour);
*/
static cell AMX_NATIVE_CALL pawnLayerColour(AMX *amx, const cell *params)
{
	LuxColour temp = {255,255,255,255};
	cell_colour colour;
	colour.hex = elix::endian::host32(params[2]);
	temp = colour.rgba;

	if ( params[1] >= 0 )
		lux::display->SetLayerColour( (uint32_t)params[1], temp );
	return 0;
}


const AMX_NATIVE_INFO Graphics_Natives[] = {
	/** Misc */
	{ "GraphicsDraw", pawnGraphicsDraw },
	{ "GraphicsType", pawnGraphicsType },
	{ "TextSprites", pawnTextSprites }, //native TextSprites(able);
	{ "SheetReference", pawnSheetRef }, //native SheetReference(sheet[], ref = 1);
	{ "SheetReplace", pawnSheetReplace },
	/** Animation */
	{ "AnimationGetLength", pawnAnimationGetLength }, ///native AnimationGetLength(sheet[], anim[]);
	{ "AnimationCreate", pawnAnimationCreate }, ///native AnimationCreate(string[]);
	{ "AnimationAddFrame", pawnAnimationAddFrame }, ///native AnimationAddFrame(string[], sprite[], string_size = sizeof string, sprite_size = sizeof sprite);
	/** Polygon */
	{ "PolygonCreate", pawnPolygonCreate }, ///native PolygonCreate(string[]);
	{ "PolygonAddPoint", pawnPolygonAddpoint }, ///native PolygonAddpoint(string[], x, y, string_size = sizeof string);
	/** Display Functions */
	{ "ObjectCreate", pawnObjectCreate }, ///native ObjectCreate(string[], type, c = 0xFFFFFFFF, string_size = sizeof string);
	{ "ObjectPosition", pawnObjectPosition }, ///native ObjectPosition(id, x, y, z, w, h);
	{ "ObjectEffect", pawnObjectEffect }, ///native ObjectEffect(id, colour = 0xFFFFFFFF, rotate, scale_w, scale_h, mirror, _STYLES:style, color2 = 0xFFFFFFFF);
	{ "ObjectReplace", pawnObjectReplace }, ///native ObjectReplace(id, string[], type, string_size = sizeof string);
	{ "ObjectDelete", pawnObjectDelete }, ///native ObjectDelete(id);
	{ "ObjectFlag", pawnObjectFlag }, ///native ObjectFlag(object:id, key, value);
	{ "ObjectToggle", pawnObjectToggle }, ///native ObjectToggle(object:id, show);
	{ "ObjectInfo", pawnObjectInfo }, ///native ObjectInfo(object:id, &w, &h);
	{ "ObjectFollowPath", pawnObjectFollowPath }, ///native ObjectFollowPath(object:id, speed,&x, &y);
	/** Camera Functions */
	{ "CameraSetScroll", pawnCameraSetScroll }, ///native CameraSetScroll(bool:scroll);
	{ "LayerSetRotation", pawnLayerSetRotation }, ///native LayerSetRotation(layer, roll, pitch, yaw);
	{ "LayerSetOffset", pawnLayerSetOffset }, ///native LayerSetOffset(layer, x, y);
	{ "LayerColour", pawnLayerColour }, ///native LayerColour(layer, colour = 0xFFFFFFFF);
	{ 0, 0 }
};
