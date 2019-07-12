/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "elix/elix_intdef.h"



namespace elix {
	namespace endian {
	#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
		uint32_t big32(uint32_t number)
		{
			return number;
		}

		uint16_t big16(uint16_t number)
		{
			return number;
		}

		uint32_t host32(uint32_t number)
		{
			return number;
		}
		uint16_t host16(uint16_t number)
		{
			return number;
		}
	#else
		uint32_t big32(uint32_t number) {
			return __builtin_bswap32(number);
		}
		uint16_t big16(uint16_t number) {
			return __builtin_bswap16(number);
		}
		uint32_t host32(uint32_t number) {
			return __builtin_bswap32(number);
		}
		uint16_t host16(uint16_t number) {
			return __builtin_bswap16(number);
		}
	#endif


	}
}
