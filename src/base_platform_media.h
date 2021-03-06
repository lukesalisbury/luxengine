/****************************
Copyright © 2013-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef BASE_PLATFORM_MEDIA_H
#define BASE_PLATFORM_MEDIA_H

#include "sprite_sheet.h"

class BasePlatformMedia
{
		BasePlatformMedia() {}
		~BasePlatformMedia() {}
	public:
		void Init(GraphicSystem graphic) { }
		void Free() { }
		LuxSprite * GetInputImage( InputDevice device, uint32_t device_number, int32_t symbol ) {
			return NULL;
		}

};



#endif // BASE_PLATFORM_MEDIA_H
