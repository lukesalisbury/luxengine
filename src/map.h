/****************************
Copyright © 2006-2011 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef _MAP_H_
#define _MAP_H_

#include "map_object.h"
#include "luxengine.h"

#include "map_screen.h"
#include "entity_section.h"

#define MAP_WRAPNONE	0
#define MAP_WRAPXAXIS	1
#define MAP_WRAPYAXIS	2
#define MAP_WRAPBOTH	3



class MokoiMap
{
		friend class EntityManager;
		friend class WorldSystem;
		friend class WorldSection;
		friend class MapXMLReader;
	public:
		MokoiMap( std::string name );
		MokoiMap( elix::File *current_save_file );
		~MokoiMap();

	private:
		/* Settings */
		std::string _name;

		bool _centerview; //Center View on middle of map or top-left
		bool _loaded;
		bool active;
		bool _keep; //Keeps map in memory
		bool _server;
		bool resetmap;
		uint8_t _wrap;

		LuxMapIdent _ident;
		MapObject _background;
		LuxColour _colour;

		/* Entities */
		std::string _entity;
		EntitySection * _entities;

		/* Object Handling */
		uint32_t object_cache_count;
		std::vector<MapObject *> _objects;
		std::map<uint32_t, MapObject *> object_cache;
		void AddObjectToScreens(MapObject * object);
		void RemoveObjectFromScreens(MapObject * object);

		/* Positions */
		std::map<uint32_t, MokoiMapScreen *> _screen;
		std::map<uint32_t, MokoiMapScreen *> activescreens;
		fixed _position[3];

		/* Dimensions */
		uint32_t dimension_width;
		uint32_t dimension_height;
		fixed map_width;
		fixed map_height;
		fixed default_map_width;
		fixed default_map_height;
		fixed screen_width;
		fixed screen_height;

		bool has_width_different;
		bool has_height_different;

	public:
		bool Init();
		bool Loop();
		bool Close();



		/* Settings */
		void SetIdent( uint32_t ident );
		void SetGridIdent( uint32_t local, uint32_t section );
		void SetFileIdent( uint32_t local );
		uint32_t Ident();
		uint32_t GridIdent();
		uint32_t FileIdent();

		bool InSection(uint32_t section);
		bool PartOfSection();

		std::string Name();
		void SetBackgroundModifier( LuxColour mod );



		/* Data  */
		bool Valid();
		bool LoadDimension();
		bool LoadFile();
		bool SaveFile();

		bool Reset();

		/* Entities */
		EntitySection * GetEntities();

		/* Masks (See map_masks.cpp) */
		uint16_t GetMaskValue(uint16_t x, uint16_t y);
		void FillMask(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t value);
		void BuildMask();
		void DrawMask();

		/* Object Handling */
		MokoiMapScreen * GetScreen( uint32_t screen_number, bool init_new = true );
		uint32_t AddObject( MapObject * object, bool is_static );
		MapObject * GetObject( uint32_t ident );
		bool RemoveObject( uint32_t ident );
		void ReplaceObjectsSheets( std::string old_sheet, std::string new_sheet );

		/* Positions */
		void SetPosition( fixed current_position[3] );
		fixed GetPosition( uint8_t axis );
		void SetScrolling( bool scroll );
		uint32_t XY2Screen(int32_t x, int32_t y, int32_t w);


		/* Save */
		bool Save( elix::File *current_save_file );
		bool Restore(elix::File *current_save_file);

	private:
		void InitialSetup( std::string name );


		/* Network (see map_network.cpp) */
#ifdef NETWORKENABLED
		void NetworkMapSwitch();
		void NetworkMapLoop();
#else
		void NetworkMapSwitch() { };
		void NetworkMapLoop() { };
#endif
};

#endif
