 /****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef _ENTITY_MANAGER_H_
	#define _ENTITY_MANAGER_H_

	#include "luxengine.h"
	#include "entity_system.h"
	#include "entity.h"
	#include "entity_section.h"


	class EntityManager
	{
		friend class EntitySection;
		public:
			EntityManager();
			~EntityManager();

			uint32_t GetRandomID( std::string * id );
			void SetSaveMode(uint8_t mode);
		private:

			/* Entity Manager */
			uint32_t _count;
			std::map<uint32_t, Entity *> _entities;


		public:
			Entity * _global, * _keyboard;
			Entity * NewEntity( std::string id, std::string base, uint32_t map_id );
			Entity * GetEntity( uint32_t entity_id );
			bool AddEntity( Entity * enw );
	};

namespace lux
{
	extern EntityManager * entities;
}
#endif
