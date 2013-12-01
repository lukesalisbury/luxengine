/****************************
Copyright © 2006-2011 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef _MAPSCREEN_H_
	#define _MAPSCREEN_H_

	#include "luxengine.h"
	#include "map_object.h"
	#include "masks.h"

	class MokoiMapScreen
	{
		public:
			MokoiMapScreen( uint32_t x, uint32_t y, uint32_t width, uint32_t height );
			~MokoiMapScreen();

			bool Init();
			bool Close();
		private:
			uint32_t _x;
			uint32_t _y;
			uint32_t _offset_x;
			uint32_t _offset_y;
			uint32_t _width;
			uint32_t _height;
			bool initialised;

			/* Masks */
			Lux_Mask * _mask;

		public:
			/* Masks */
			uint16_t GetMaskValue(uint16_t x, uint16_t y);
			void FillMask(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t value);
			void BuildMask();
			void DrawMask( fixed position[3] );

			/* Object Handling */
			std::vector<MapObject *> _objects;
	};


#endif
