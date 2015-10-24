/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "elix/elix_intdef.h"

#if defined ENDIAN_SDL
	#include <SDL/SDL.h>
#elif __GNUWIN32__
	#include <winsock2.h>
	#define ENDIAN_NETWORK 1
#elif __linux__
	#include <arpa/inet.h>
	#define ENDIAN_NETWORK 1
#elif  FLASCC
	#include <arpa/inet.h>
	#define ENDIAN_NETWORK 1
#elif defined (__NDS__)
	#include <sys/socket.h>
	#define ENDIAN_NETWORK 1
#elif defined (__3DS__)
	#include <arpa/inet.h>
	#include <sys/socket.h>
	#define ENDIAN_NETWORK 1
#elif defined (DREAMCAST)
	#include <endian.h>
	#define ENDIAN_NETWORK 1
#elif defined (__GAMECUBE__) || defined (__WII__)
	#include <network.h>
	#define ENDIAN_NETWORK 1
#elif defined __apple__
	#include <SDL.h>
	#define ENDIAN_SDL 1
#else
	#include <sys/socket.h>
	#define ENDIAN_NETWORK 1
#endif


namespace elix {
	namespace endian {
		#ifdef ENDIAN_NETWORK
		uint32_t big32(uint32_t number)
		{
			return htonl(number);
		}

		uint16_t big16(uint16_t number)
		{
			return htons(number);
		}

		uint32_t host32(uint32_t number)
		{
			return ntohl(number);
		}
		uint16_t host16(uint16_t number)
		{
			return ntohs(number);
		}
		#elif ENDIAN_SDL
		uint32_t big32(uint32_t number)
		{
			return SDL_SwapBE32(number);
		}

		uint16_t big16(uint16_t number)
		{
			return SDL_SwapBE16(number);
		}

		uint32_t host32(uint32_t number)
		{
			#if SDL_BYTEORDER == SDL_LIL_ENDIAN
				return SDL_Swap32(number);
			#else
				return number;
			#endif
		}
		uint16_t host16(uint16_t number)
		{
			#if SDL_BYTEORDER == SDL_LIL_ENDIAN
				return SDL_Swap16(number);
			#else
				return number;
			#endif
		}
		#endif

	}
}
