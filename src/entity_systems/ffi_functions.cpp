#include "ffi_functions.h"

MapObject * Lux_FFI_Object_Get( uint32_t object_id )
{
	MapObject * object = NULL;
	if ( lux::world->active_map )
	{
		if ( object_id >= OBJECT_GLOBAL_VALUE )
			object = lux::world->GetObject(object_id);
		else
			object = lux::world->active_map->GetObject(object_id);
	}
	return object;

}
