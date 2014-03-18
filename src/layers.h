/****************************
Copyright © 2006-2011 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef _LAYER_H_
	#define _LAYER_H_

	#include "map_object.h"
	#include "display_types.h"
	#include <list>

	class DisplaySystem;

	class Layer
	{
		public:
			Layer(DisplaySystem * parent, uint8_t layer_value);
			~Layer();
			bool _cachedynamic;
			LuxColour colour;
			bool static_layer;
			uint8_t wrap_layer;
			int16_t _roll, _pitch, _yaw;
			bool refreshz;

			uint8_t shader;

		private:
			DisplaySystem * _parent;
			std::list<MapObject *> objects_static;
			std::list<MapObject *> objects_dynamic;
			fixed _x, _y, _z, _w, _h;
			uint8_t display_layer;
			fixed _mapx, _mapy, _mapz;
			uint32_t _mapw, _maph, wrap_mapw, wrap_maph;
			fixed _oldx, _oldy, _oldz;
			LuxRect display_dimension;
			LuxRect wrap_dimension;


		public:
			/* Object Management Functions */
			bool AddObject( MapObject * new_object, bool static_object );
			bool RemoveObject( MapObject * new_object );
			bool ClearObjects( bool static_objects );
			bool ClearAll();

			/* Camera Functions */
			void SetOffset( fixed x, fixed y );
			void SetRotation( int16_t roll, int16_t pitch, int16_t yaw);

			/* Caching Functions */
			void GetSurface();

			/* Drawing Functions */
			void Display();

			void SetShader( uint8_t new_shader );

	private:
			LuxRect GetObjectLocation( LuxRect map_location, uint8_t type );
			bool ObjectOnScreen(LuxRect o, uint8_t flipmode, LuxRect s);
			bool ObjectOnVirtualScreen(LuxRect & o, uint8_t flipmode, LuxRect s, uint32_t offx, uint32_t offy );
	};
#endif

