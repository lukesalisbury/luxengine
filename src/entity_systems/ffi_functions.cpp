/****************************
Copyright © 2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include "ffi_functions.h"

#include "world.h"

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
