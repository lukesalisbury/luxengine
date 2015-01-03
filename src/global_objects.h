/****************************
Copyright © 2014-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#ifndef GLOBALOBJECTS_HPP
#define GLOBALOBJECTS_HPP
#include <map>

#include "map_object.h"
#include "map.h"
#include "world_section.h"
#include "entity_section.h"

class GlobalObjects
{
public:
	GlobalObjects();
	~GlobalObjects();


	bool Init( );
	void Loop( LuxState engine_state );
	bool Close();

	/* Save */
	bool Save( elix::File * current_save_file );
	bool Restore( elix::File * current_save_file );


	/* Object Handling */
	uint32_t AddObject( MapObject * object, bool is_static );
	MapObject * GetObject( uint32_t ident );
	bool RemoveObject( uint32_t ident );
	void ClearObjects();




	/* Colllision Functions */
	void AddCollision( uint32_t hashname, CollisionObject * rect );
	void CheckCollision( uint32_t hashname, CollisionObject * rect );
	void ClearCollisions();
	void ReturnCollisions( std::vector<CollisionResult *> * hits, uint32_t entity, int16_t count, LuxRect rect );
	void RemoveCollisions( uint32_t hashname );

	void DrawCollisions();

private:


	/* DisplayObject */
	uint32_t object_cache_count;
	std::map<uint32_t, MapObject *> object_cache;

	/* Colllision Functions */
	std::multimap<uint32_t, CollisionObject*> collision_list;

};

#endif // GLOBALOBJECTS_HPP
