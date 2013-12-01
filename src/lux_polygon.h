/****************************
Copyright © 2013 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#ifndef LUXPOLYGON_HPP
#define LUXPOLYGON_HPP

#include "stdheader.h"

class LuxPolygon
{
	public:
		LuxPolygon(uint16_t point);
		LuxPolygon(std::string file);
		~LuxPolygon();

	public:
		int16_t * x, * y;
		uint16_t count;
		mem_pointer texture;

		void SetPoint( uint16_t point, int16_t x_value, int16_t y_value );

};

#endif // LUXPOLYGON_HPP
