/****************************
Copyright © 2013-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#ifndef OBJECT_EFFECT_H
#define OBJECT_EFFECT_H

#include "lux_types.h"
#include "display/display_styles.h"

namespace colour {
	extern LuxColour white;
	extern LuxColour black;
	extern LuxColour red;
	extern LuxColour blue;
	extern LuxColour green;
	extern LuxColour yellow;
}

class ObjectEffect
{
	ObjectEffect(LuxColour c1, LuxColour c2);
	ObjectEffect();
	ObjectEffect(LuxColour c1, LuxColour c2, uint8_t style);

public:
	uint32_t Hex();

public:
	LuxColour primary_colour;
	LuxColour secondary_colour;

	int16_t rotation;
	uint16_t scale_xaxis;
	uint16_t scale_yaxis;

	uint8_t flip_image;
	uint8_t tile_object;
	uint8_t style;

	bool operator == (const ObjectEffect &b) const;

};


#endif
