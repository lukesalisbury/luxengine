#include "ffi_object.h"


#include "map_object.h"

/** Display Object Functions */

/** pawnObjectPosition
* native ObjectPosition(id, x, y, z, w, h, pos = POSITION_MAP);
*/
uint8_t Lux_FFI_Object_Set_Postion( uint32_t object, int32_t x, int32_t, y, int32_t z, uint16_t w, uint16_t h, uint32_t colour, char * sprite )
{


}

/** pawnObjectCreate
* native ObjectCreate(string[], type, x, y, z, w, h, c = 0xFFFFFFFF, pos = POSITION_MAP, string_size = sizeof string);
*
*/
uint32_t Lux_FFI_Object_Create( uint8_t global, uint8_t type, int32_t x, int32_t, y, int32_t z, uint16_t w, uint16_t h, uint32_t colour, char * sprite )
{
	MapObject * new_object = NULL;

	if ( lux::world->active_map )
	{
		new_object = new MapObject( type );

		new_object->position.x = params[3];
		new_object->position.y = params[4];
		new_object->SetZPos( (int32_t)params[5] );
		new_object->position.w = (uint16_t)params[6];
		new_object->position.h = (uint16_t)params[7];

		new_object->effects.primary_colour = colour.rgba;

		if ( type == 's' && sprite != NULL )
			new_object->sprite.assign( sprite );

		if ( global )
			return lux::world->AddObject(new_object, true);
		else
			return lux::world->active_map->AddObject(new_object, true);
	}
	return 0;

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

