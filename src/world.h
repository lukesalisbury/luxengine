/****************************
Copyright © 2006-2011 Luke Salisbury
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
	#include "map.h"
	#include "map_object.h"
	#include "world_section.h"

	class WorldSystem
	{
		friend class EntityManager;
		friend class WorldSection;
		public:
			WorldSystem();
			~WorldSystem();

			bool Init( );
			void Loop(LuxState engine_state);
			bool PreClose();
			bool Close();

			/* Save */
			bool Save( elix::File *current_save_file );
			bool Restore(elix::File *current_save_file);

			/* Positions */
			WorldSection * active_section;
			MokoiMap * active_map;
			void SetPosition( fixed x, fixed y, fixed z = 0, bool change = false);
			fixed GetPosition( uint8_t axis, uint8_t to );
			bool MovePosition( fixed * current_position );

			/* Map Handling */
			MokoiMap * LoadMap( std::string map_file, bool set_current, bool create = false); // create_new will allow multiple version of maps and map that don't exist
			MokoiMap * GetMap( uint32_t id );
			uint32_t GetMapID( std::string map_file );
			bool SetMap( std::string map_file, int32_t mapx, int32_t mapy );
			bool SetMap( uint32_t id, int32_t mapx, int32_t mapy );

			void SwitchMap( MokoiMap * new_map, bool move_entities = false );

			/* Section System */
			uint8_t _grid[2];
			WorldSection * LoadSection( std::string active_section, bool set_current );
			WorldSection * GetSection( std::string name );
			uint32_t SwitchSection( std::string active_section, uint8_t gridx, uint8_t gridy );
			void DeleteSection( uint32_t active_section );
			bool SetMap( uint8_t gridx, uint8_t gridy );
			bool GetMapGrid(  uint8_t & gridx, uint8_t & gridy );
			uint16_t GetMapID(std::string section_name, uint8_t gridx, uint8_t gridy );

			/* Object Handling */
			uint32_t AddObject(MapObject * object, bool is_static);
			MapObject * GetObject( uint32_t ident );
			void ClearObjects();
			bool RemoveObject(uint32_t ident);
			EntitySection * GetEntities() { return this->_entities; }

			/* Colllision Functions */
			void AddCollision( uint32_t hashname, CollisionObject * rect );
			void CheckCollision( uint32_t hashname, CollisionObject * rect );
			void ClearCollisions();
			void ReturnCollisions( std::vector<CollisionResult *> * hits, uint32_t entity, int16_t count, LuxRect rect );
			void RemoveCollisions( uint32_t hashname );
			void DrawCollisions();

		private:
			MokoiMap * _nextmap;
			EntitySection * _entities;

			/* Positions */
			fixed current_position[3];
			fixed next_position[3];
			uint8_t map_movement_direction;

			/* Map Handling */
			uint32_t current_map_ident;
			LuxMapIdent map_counter;
			std::map<uint32_t, MokoiMap *> map_list;

			/* Grid System */
			WorldSection * next_section;
			std::map<uint32_t, WorldSection *> section_list;
			uint8_t next_grid_position[2];

			/* DisplayObject */
			uint32_t _cachecount;
			std::map<uint32_t, MapObject *> _objectscache;

			/* Colllision Functions */
			std::multimap<uint32_t, CollisionObject*> _collision;


			void Print();
	};


namespace lux {
	extern WorldSystem * world;
}
#endif
