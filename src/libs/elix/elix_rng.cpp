/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

/* Just a wrapper */
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include "elix/elix_intdef.h"
#include "elix/elix_string.hpp"

namespace elix
{
	namespace rng
	{
		time_t seed = 0;

		inline void SetSeed()
		{
			if ( seed == 0 )
			{
				seed = time(NULL);
				std::srand( seed );
			}
		}

		int32_t GetRandomNumber()
		{
			SetSeed();
			return static_cast<int32_t>( rand() );
		}
		std::string GetRandomString()
		{
			SetSeed();
			return string::FromInt( GetRandomNumber() );
		}

	}
}
