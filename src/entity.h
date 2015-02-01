/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef _ENTITY_H_
#define _ENTITY_H_


#include "luxengine.h"
#include "entity_system.h"
#include <map>

class Entity
{
	public:
		Entity(std::string base, std::string id, uint32_t mapid, LuxEntityCallback * callbacks);
		Entity(elix::File * current_save_file, uint32_t mapid);
		~Entity();

	private:
		/* Setting Functions */
		std::map<std::string,std::string> _settings;

	private:
		void DefaultSetup( uint32_t mapid );
		void InitialSetup( std::string id );

	public:
		bool Loop();
		void Init();
		void Update();
		void Close();
		void Delete();

		/**/
		void Save( elix::File * current_save_file );
		void Restore( elix::File * current_save_file );

		int32_t Call(std::string function, char * arg_format, ...);

		/* Info */
		std::string _base;
		std::string id;
		uint32_t hashid;
		LuxEntityCallback * callbacks;
		bool sleeping;
		uint32_t starting_run_time;
		uint8_t active_state_flags;

		/* Location */
		fixed x, y, z_layer; /* Position is relative to map */
		uint32_t _mapid, displaymap;
		void ChangeMap(uint32_t map_id);

		/* Collision Data */
		int32_t _used_collisions;
		CollisionObject _collisions[16];
		std::vector<CollisionResult*> _hits;
		int32_t AddCollision(int32_t rect, int32_t x, int32_t y, int32_t w, int32_t h, int32_t type );
		int32_t CheckCollision(int32_t rect, LuxRect otherRect);
		void SetCollisionCount(int32_t count);
		int32_t GetHits( int32_t type = -1 );
		int32_t GetHitsCount();
		CollisionResult * GetCurrentHit();
		void UpdateMapCollsion();
		void SetMapCollsion();
		void ClearMapCollsion();

		/* Setting Functions */
		void AddSetting(std::string key, std::string value);
		void AddSetting(std::string key, int32_t value);
		void AddSetting(std::string key, uint32_t value);
		std::string GetSetting(std::string key);
		std::string GetSetting( char * key );
		int32_t GetSettingAsInt(std::string key);
		float GetSettingAsFloat(std::string key);

		/* Data Functions */
		bool deleted, firstran, active, loaded, onscreen, closing;
		void * _data;
		void FreeData();

		/* Infomation */
		std::string Infomation();


		/* Event */
		/*std::vector<EventEntity*> _event;*/
};


struct DownloadEntityRequest
{
		Entity * entity;
		std::string url, callback;
};

#endif
