/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#ifndef _PAWNHELPER_H_
	#define _PAWNHELPER_H_
	#if defined apple || defined IOS || defined FLASCC
		#include <stdlib.h>
	#else
		#include <malloc.h>
	#endif


	#include "../stdheader.h"

    #include "../scripting/amx4/amx.h"
    #include "../scripting/amx4/amxcons.h"
    #include "../scripting/amx4/osdefs.h"


	#include "../entity.h"
	#include "pawn.h"

	//#define CELLMIN (-1 << (8*sizeof(cell) - 1))
	#if PLATFORMBITS == 64
	//INT64_MIN
	#define CELLMIN (-9223372036854775807LL - 1)
	#else
	//INT32_MIN
	#define CELLMIN (-2147483647 - 1)
	#endif
	#define CHARBITS 8


	#define ASSERT_PAWN_PARAM(a,p,m) if ( m > p[0] / sizeof(cell) ) { amx_RaiseError(a,AMX_ERR_PARAMS); return 0x7FFFFFFF; }

	char * Lux_PawnEntity_StrError(unsigned int errnum);
	cell pawnDeprecatedFunction(AMX *amx, const cell *params);

	Entity * Lux_PawnEntity_GetEntity(AMX * amx, cell text_param);
	uint32_t Lux_PawnEntity_GetEntityHash(AMX *amx);

	uint32_t Lux_PawnEntity_HasString(AMX * amx, cell str_param);
	size_t Lux_PawnEntity_SetString(cell * dest, const char * source, size_t dest_size );
	std::string Lux_PawnEntity_GetString( AMX *amx, cell param );

	int32_t Lux_PawnEntity_PublicVariable(AMX * amx, std::string varname, int32_t * new_value );

	#ifndef amx_Address
	inline cell * amx_Address( AMX * amx, cell param )
	{
		cell *cellptr = nullptr;
		amx_GetAddr(amx, param, &cellptr);
		return cellptr;
	}
	#endif

	inline void write_amx_address( AMX * amx, cell param, cell value )
	{
		if ( param != CELLMIN )
		{
			cell * cptr = amx_Address(amx, param);
			if ( cptr )
				*cptr = value;
		}
	}

	inline uint8_t amx_get_next_packed( cell * & cstr, ucell &c , uint32_t & i )
	{
		uint8_t char8 = 1;

		if ( i == sizeof(ucell)-1)
			c = *cstr++;

		char8 = (uint8_t)(c >> i*CHARBITS);
		if ( char8 != '\0')
		{
			i = (i+sizeof(ucell)-1) % sizeof(ucell);
		}

		return char8;
	}

#endif
