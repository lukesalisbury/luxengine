/****************************
Copyright © 2006-2012 Luke Salisbury
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

	#if PAWN_VERSION == 4
    #include "../scripting/amx4/amx.h"
    #include "../scripting/amx4/amxcons.h"
    #include "../scripting/amx4/osdefs.h"
	#else
    #include "../scripting/amx/amx.h"
    #include "../scripting/amx/amxcons.h"
    #include "../scripting/amx/osdefs.h"
	#endif

	#include "../entity.h"
	#include "pawn.h"

	#define CHARBITS (8*sizeof(char))
	#define CELLMIN (-1 << (8*sizeof(cell) - 1))
	#define PAWNPARAMCHECK(a,c) (a >= c * sizeof(cell))

	char * Lux_PawnEntity_StrError(unsigned int errnum);
	cell AMX_NATIVE_CALL pawnDeprecatedFunction(AMX *amx, const cell *params);

	Entity * Lux_PawnEntity_GetEntity(AMX * amx, cell text_param);

	uint32_t Lux_PawnEntity_HasString(AMX * amx, cell str_param);
	uint32_t Lux_PawnEntity_SetString(cell * dest, const char * source, size_t size );
	std::string Lux_PawnEntity_GetString( AMX *amx, cell param );

	int32_t Lux_PawnEntity_PublicVariable(AMX * amx, std::string varname, int32_t * new_value );

	#ifndef amx_Address
	inline cell * amx_Address( AMX * amx, cell param )
	{
		cell *cellptr = NULL;
		amx_GetAddr(amx, param, &cellptr);
		return cellptr;
	}
	#endif




#endif
