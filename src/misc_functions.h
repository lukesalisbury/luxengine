/****************************
Copyright © 2006-2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef _FUNCTION_H_
	#define _FUNCTION_H_

	#include "lux_types.h"
	#include <string>
	#include <vector>

	bool Lux_Util_RectCollide( LuxRect a, LuxRect b );
	bool Lux_Util_PointCollide( LuxRect a, int32_t x, int32_t y );
	void UnicodeToInput( int32_t cchar, int8_t * axis, int8_t * button, int8_t * pointer );
#endif
/* _FUNCTION_H_ */
