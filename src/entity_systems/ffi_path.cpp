/****************************
Copyright © 2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#include "ffi_path.h"
#include "ffi_functions.h"
#include <cmath>

#define FIXEDPI  3.1415926535897932384626433832795

/** pawnObjectFollowPath
* native ObjectFollowPath(object:id, speed, &x, &y, loop);
*/
int32_t Lux_FFI_Path_Move_Object( uint32_t object_id, int32_t fixed_speed, int16_t * x, int16_t * y, uint8_t loop )
{
	float movex, movey, speed, angle = 0;
	LuxPath next, prev;
	uint16_t next_path_point = 0;

	MapObject * map_object = Lux_FFI_Object_Get( object_id );

	if ( map_object )
	{
		if ( !map_object->_path.size() )
		{
			return -2;
		}


		// Movement direction
		speed = MAKE_FIXED_FLOAT(fixed_speed) * (float)lux::core->GetFrameDelta();
		if ( speed > 0.0 )
		{
			next_path_point = map_object->path_point+1;
			if ( loop ) // Loop around
			{
				if ( map_object->path_point >= map_object->_path.size()-1 )
				{
					next_path_point = 0;
				}
			}
			else
			{
				if ( map_object->path_point >= map_object->_path.size()-1 )
					return map_object->path_point;
			}

			prev.x = map_object->position.x;
			prev.y = map_object->position.y;
			next = map_object->_path.at(next_path_point);
		}
		else if ( speed < 0.0 )
		{
			next_path_point = map_object->path_point-1;
			if ( loop ) // Loop around
			{
				if ( (map_object->path_point == 0) && map_object->path_point > map_object->_path.size() )
				{
					next_path_point = map_object->_path.size()-1;
				}
			}
			else
			{
				if ( (map_object->path_point == 0) && map_object->path_point > map_object->_path.size() )
					return map_object->path_point;
			}

			prev.x = map_object->position.x;
			prev.y = map_object->position.y;
			prev = map_object->_path.at(object->path_point);
			next = map_object->_path.at(next_path_point);
		}
		else
			return -1;

		angle = atan2( (float)(prev.y - next.y), (float)(prev.x - next.x));
		movey = sin(angle) * speed;
		movex = cos(angle) * speed;

		map_object->path_current_x -= MAKE_FLOAT_FIXED(movex);
		map_object->path_current_y -= MAKE_FLOAT_FIXED(movey);

		map_object->position.x = MAKE_FIXED_INT(object->path_current_x);
		map_object->position.y = MAKE_FIXED_INT(object->path_current_y);

		if ( map_object->position.x == next.x && map_object->position.y == next.y)
		{
			map_object->path_current_x = MAKE_INT_FIXED(object->position.x);
			map_object->path_current_y = MAKE_INT_FIXED(object->position.y);
			map_object->path_point = next_path_point;
		}


		if (x)
			*x = map_object->position.x;

		if (y)
			*y = map_object->position.y;

		return map_object->path_point;
	}
	return -1;
}

/** Lux_FFI_Path_Count
* native PathCount( object:id );
* uint32_t object_id
*/
int32_t Lux_FFI_Path_Count( uint32_t object_id )
{
	MapObject * map_object = Lux_FFI_Object_Get( object_id );
	if ( map_object )
	{
		return (int32_t)map_object->_path.size();
	}
	return -1;
}

/** Lux_FFI_Path_Point
*
* uint32_t object_id, uint8_t point, int16_t * x, int16_t * y, uint32_t *ms_length
*/
uint8_t Lux_FFI_Path_Point( uint32_t object_id, uint8_t point, int16_t * x, int16_t * y, uint32_t *ms_length )
{
	if ( point < 0 )
		return 0;

	LuxPath next;
	MapObject * map_object = Lux_FFI_Object_Get( object_id );

	if ( map_object )
	{
		next = object->_path.at(params[2]);
		if (x)
			*x = next.x;
		if (y)
			*y = next.y;
		if (ms_length)
			*ms_length = next.ms_length;

		return 1;
	}

	return 0;
}
