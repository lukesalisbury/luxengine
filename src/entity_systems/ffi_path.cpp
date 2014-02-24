/****************************
Copyright © 2013 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include "ffi_path.h"
#include "ffi_functions.h"


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
