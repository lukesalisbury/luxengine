/****************************
Copyright © 2006-2012 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef _SECTION_H_
	#define _SECTION_H_
	#include "map.h"
	#include "map_object.h"

	class WorldSection
	{
		public:
			WorldSection( std::string file, uint32_t id, uint8_t width, uint8_t height );
			WorldSection( uint8_t width, uint8_t height );
			~WorldSection();

			void InitialSetup(uint8_t width, uint8_t height );
			void LoadFile( );

			/* Save */
			uint8_t flag;
			bool Save( elix::File *current_save_file );
			bool Restore( elix::File *current_save_file );

			/* Grid System */
			std::string file;
			uint8_t _width;
			uint8_t _height;
			uint32_t _id;
			uint16_t _grid[64][64];

			uint16_t getGrid( uint8_t grid_x, uint8_t grid_y )
			{
				if ( grid_x >= 64 )
					return 0xFFFF;
				if ( grid_y >= 64 )
					return 0xFFFF;
				return this->_grid[grid_x][grid_y];
			}
			uint32_t getMapID( uint16_t grid )
			{
				LuxMapIdent map;
				map.value = 0;
				map.grid.id = this->_id;
				map.grid.localmap = grid;
				return map.value;
			}
	};

#endif
