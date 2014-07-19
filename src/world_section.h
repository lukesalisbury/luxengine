/****************************
Copyright © 2006-2014 Luke Salisbury
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
	#include "options.h"

	class WorldSection: public Options
	{
		public:
			WorldSection( LuxMapIdent ident, std::string name, uint8_t width, uint8_t height  );
			WorldSection( elix::File * current_save_file );

			~WorldSection();

		private:
			void InitialSetup( uint8_t width, uint8_t height );
			void LoadFile( );
			void SaveFile( );

		private:
			uint32_t Ident() { return id.grid.section; }
			std::string Name() { return name; }

			bool IsBasic() { return ( this->allocated_size == 1 ? true : false ); }

			/* Save */
			bool Save( elix::File * current_save_file );
			bool Restore( elix::File * current_save_file );


			/* Grid System */
			uint32_t BuildMapID( const uint16_t grid_id );
			uint32_t GetMapID( const uint8_t grid_x, const uint8_t grid_y );
			uint16_t GetGridID( const uint8_t grid_x, const uint8_t grid_y );

			MokoiMap * GetMap( const uint8_t grid_x, const uint8_t grid_y  );
			MokoiMap * GetMap( const uint16_t grid_id  );

			/* Child Maps */
			bool AddMap( std::string file_name, const uint8_t grid_x, const uint8_t grid_y );
			bool AddMap( MokoiMap * map_object, const uint8_t grid_x, const uint8_t grid_y );

		private:
			/* Details */
			std::string name;
			LuxMapIdent id;

			/* Save */
			uint8_t flag;

			/* Setting */

			/* Grid System */
			uint16_t allocated_size;

			uint8_t width;
			uint8_t height;

			uint16_t * grid;
	};

#endif
