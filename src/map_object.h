/****************************
Copyright © 2006-2011 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef _MAP_OBJECT_H_
	#define _MAP_OBJECT_H_

class MapObject;

#include "luxengine.h"
#include "sprite_types.h"
#include "display_types.h"
#include "object_effect.h"

#include "lux_canvas.h"
#include "lux_polygon.h"
#include "lux_virtual_sprite.h"


class MapObject
{
	public:
		MapObject( uint8_t type = 0 );
		~MapObject();

		MapObject * clone() const { return new MapObject( *this ); }

		ObjectEffect effects;
		std::string image;
		std::string id;
		LuxRect position;
		uint8_t layer;
		int16_t offset_x, offset_y;
		uint16_t sprite_width;
		uint16_t sprite_height;
		uint32_t static_map_id;
		uint32_t layer_ref;
		bool hidden;
		bool can_remove;
		bool global;

		void Toggle();
		bool CollisionRectangle( LuxRect rect[7] );

		/* Animations */
		uint8_t timer_mode;
		uint32_t timer;
		int16_t speed;
		bool loop;
		bool reverse;
		bool auto_delete;

		/* Data Function */
		bool has_data;
		uint8_t type;
		uint8_t data_type;

		mem_pointer GetData();
		void SetData(mem_pointer, uint8_t type);
		void FreeData();

		mem_pointer GetImage( ObjectEffect fx );
		LuxSprite * GetCurrentSprite( );
		LuxSprite * GetAnimationFrame( LuxSprite * orig, bool no_increment = false);

		LuxPolygon * GetPolygon();
		void SetPolygon(LuxPolygon * data);

		/* canvas type */
		LuxCanvas * GetCanvas();
		void SetCanvas(LuxCanvas * data);

		/* Virtual Sprite type */
		LuxVirtualSprite * GetVirtual( );
		LuxVirtualSprite * InitialiseVirtual( );

		/* Path */
		std::vector<LuxPath> _path;
		uint16_t path_point;
		fixed path_current_x, path_current_y;

		/* Save */
		void Save( elix::File *current_save_file );
		void Restore(elix::File *current_save_file);

		/* */
		std::string Name();
		std::string Info();
		void SetZPos(int32_t z);

	private:

		/* Data */
		mem_pointer data;
		LuxSprite * PeekSprite();
		LuxSprite * PeekAnimationFrame(LuxSprite * sprite );
		LuxSprite * GetSprite(bool no_increment = false);
		void IncrementAnimation( LuxSprite * sprite );
		SpriteFrame RetieveAnimationFrame( LuxSprite * sprite, uint32_t timer );

};

#endif /* _MAP_OBJECT_H_ */
