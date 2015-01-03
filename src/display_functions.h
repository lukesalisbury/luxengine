/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef _DISPLAY_FUNCTION_H_
	#define _DISPLAY_FUNCTION_H_

	#include "lux_types.h"
	LuxColour Lux_Hex2Colour(std::string colour);
	ObjectEffect SetColourOnEffect(uint8_t R, uint8_t G, uint8_t B, uint8_t A);

#endif
/* _DISPLAY_FUNCTION_H_ */
