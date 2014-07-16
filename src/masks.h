/****************************
Copyright © 2006-2014 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef _MASKS_H_
	#define _MASKS_H_
	#include "stdheader.h"
	#include "lux_types.h"

	Lux_Mask * Lux_Mask_New(uint16_t width, uint16_t height);
	bool Lux_Mask_Load(std::string filename, Lux_Mask * mask);
	bool Lux_Mask_Free(Lux_Mask * mask);
	uint16_t Lux_Mask_GetValue(Lux_Mask * mask, uint16_t x, uint16_t y);
	void Lux_Mask_CopyArea(Lux_Mask * src, Lux_Mask * dest, uint16_t x, uint16_t y);
	void Lux_Mask_FillArea(Lux_Mask * dest, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t value);



#endif
