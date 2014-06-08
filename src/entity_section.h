 /****************************
Copyright © 2006-2011 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef _ENTITY_SECTION_H_
	#define _ENTITY_SECTION_H_
	
	#include "lux_types.h"
	#include "entity.h"
	#include <vector>

	class EntitySection
	{
		public:
			EntitySection(std::string parent_entity, uint32_t id);
			~EntitySection();

		private:
			bool initialised;
			uint8_t _magic[3];
			uint32_t map_id;
			std::map<uint32_t, Entity *> _entities;

		public:
			std::vector<Entity *> children;
			std::vector<Entity *>::iterator iter;
			Entity * parent;

			bool Loop();
			void Init();
			void PreClose();
			void Close();

			void Switch(fixed x, fixed y);
			bool Save( elix::File * current_save_file );
			bool Restore( elix::File * current_save_file );

			void Append( Entity * enw );
			Entity * GetEntity( uint32_t entity_id );
	};

#endif
