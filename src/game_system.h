/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef _WORLD_H_
	#define _WORLD_H_
	#include <map>
	#include <vector>

	#include "map_object.h"
	#include "map.h"
	#include "world_section.h"
	#include "Box2D/Box2D.h"
	#include "entity_section.h"
	#include "global_objects.h"

	class GameSystem
	{
		public:
			GameSystem();
			~GameSystem();

			bool Init( );
			void Loop( LuxState engine_state );
			bool Close();

			/* Save */
			bool Save( elix::File *current_save_file );
			bool Restore( elix::File *current_save_file );


			/* Position */


			/* Position Movement */
			uint32_t SetMap( MokoiMap * map, fixed position_x, fixed position_y, fixed position_z );


			/* Map Handling */

			/* Objects */
			GlobalObjects * GetObjects() { return objects; }
			MapObject * GetObject( uint32_t ident );

			/* Physics */
			b2World * GetPhysicWorld() { return physic_world; }

			/* Entities */
			EntitySection * GetEntities() { return this->global_entities; }

			/* World Section System */

			/* World Section Options */

			/* World Section Editing */





			/* Debug Information*/
			void GetDebugInfo( std::ostream & stream );


                        void OutputInformation();
        private:
			/* Entities */
			EntitySection * global_entities;

			/* Objects */
			GlobalObjects * objects;

			/* Offset Positions */
			fixed current_offset_position[3];
			fixed next_offset_position[3];
			uint8_t map_movement_direction;

			void SetPosition(fixed offset_x, fixed offset_y, fixed offset_z = 0);
			fixed GetPosition(uint8_t axis);
			void CheckPosition();

			/* Position Movement */
			void SwitchActive();
			bool SwitchMap( MokoiMap * next_map );

			uint32_t SetMap( WorldSection * section, uint8_t grid_x, uint8_t grid_y, fixed position_x, fixed position_y, fixed position_z  );
			uint32_t SetMap( std::string section_name, uint8_t grid_x, uint8_t grid_y, fixed position_x, fixed position_y, fixed position_z );
			uint32_t SetMap( uint32_t section_hash, uint8_t grid_x, uint8_t grid_y, fixed position_x, fixed position_y, fixed position_z );

			/* Physics */
			b2World * physic_world;


			/* Map Handling */
			MokoiMap * next_map;
			MokoiMap * active_map;
			std::map<uint32_t, MokoiMap *> map_list; // Sections just store the ids not pointer

			LuxMapIdent map_counter;

			MokoiMap * GetActiveMap() { return this->active_map; }
			MokoiMap * CreateMap( std::string map_name, bool removeable, bool editable, uint32_t width, uint32_t height );
			MokoiMap * GetMap( uint32_t ident );
			MokoiMap * FindMap( std::string map_file );
			bool DeleteMap( uint32_t ident );
			bool InsertMap( uint32_t ident, MokoiMap * map );
			uint32_t GetMapID( std::string map_name );

			/* Map Editing */
			uint32_t CreateEditMap( std::string map_name, uint32_t section_ident );
			uint32_t AddObjectEditMap( uint32_t map_ident, MapObject * object );
			MapObject * GetObjectEditMap( uint32_t map_ident );
			bool SaveEditMap( uint32_t map_ident );

			/* World Section System */


			bool InsertSection(WorldSection * section, uint32_t section_ident );


			WorldSection * NewSection( std::string section_name, const uint8_t width, const uint8_t height );
			WorldSection * LoadSection( std::string file_name, bool set_current = false );
			WorldSection * GetSection( uint32_t hash );
			WorldSection * GetSection( std::string section_name, bool load );
			WorldSection * GetSection( MokoiMap * map );
			void DeleteSection( uint32_t ident );

			uint32_t GetSectionID( std::string section_name );
			uint32_t GetMapID( std::string section_name, uint8_t gridx, uint8_t gridy );

			void IncreaseSectionCounter() { this->map_counter.grid.section++; }

			bool SetSectionGrid( uint8_t gridx, uint8_t gridy );
			bool GetSectionGrid( uint8_t & gridx, uint8_t & gridy );

			/* World Section Options */
			int32_t SetWorldOption( uint32_t section_ident, uint8_t key, int32_t value );
			int32_t GetWorldOption( uint32_t section_ident, uint8_t key, int32_t value );

			/* World Section Editing */
			uint32_t CreateWorldEdit( std::string section_name );
			bool SetWorldEdit( uint32_t section_ident, uint32_t map_ident, uint8_t grid_x, uint8_t grid_y );
			bool SaveWorldEdit( uint32_t section_ident );

			/* Grid System */
			uint8_t grid_position[2];
			uint8_t next_grid_position[2];

			WorldSection * active_section;
			WorldSection * next_section;
			std::map<uint32_t, WorldSection *> section_list;



			void Print();
	};


namespace lux {
	extern GameSystem * gamesystem;
	extern GameSystem * oldgame;
}
#endif
