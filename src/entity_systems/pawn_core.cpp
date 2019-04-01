/*  Core module for the Pawn AMX
 *
 *  Copyright (c) ITB CompuPhase, 1997-2009
 *
 *  This software is provided "as-is", without any express or implied warranty.
 *  In no event will the authors be held liable for any damages arising from
 *  the use of this software.
 *
 *  Permission is granted to anyone to use this software for any purpose,
 *  including commercial applications, and to alter it and redistribute it
 *  freely, subject to the following restrictions:
 *
 *  1.  The origin of this software must not be misrepresented; you must not
 *      claim that you wrote the original software. If you use this software in
 *      a product, an acknowledgment in the product documentation would be
 *      appreciated but is not required.
 *  2.  Altered source versions must be plainly marked as such, and must not be
 *      misrepresented as being the original software.
 *  3.  This notice may not be removed or altered from any source distribution.
 *
 *  Version: $Id: amxcore.c 4057 2009-01-15 08:21:31Z thiadmer $
 */
#if defined _UNICODE || defined __UNICODE__ || defined UNICODE
# if !defined UNICODE   /* for Windows */
#   define UNICODE
# endif
# if !defined _UNICODE  /* for C library */
#   define _UNICODE
# endif
#endif

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include "pawn_helper.h"

#if defined __WIN32__ || defined _WIN32 || defined WIN32 || defined _Windows
  #include <windows.h>
#endif

/* A few compilers do not provide the ANSI C standard "time" functions */
#if !defined SN_TARGET_PS2 && !defined _WIN32_WCE && !defined __ICC430__
  #include <time.h>
#endif

typedef unsigned char   uchar;

static cell core_numargs(AMX *amx,const cell *params)
{
	AMX_HEADER *hdr;
	uchar *data;
	cell bytes;

	(void)params;
	hdr=(AMX_HEADER *)amx->base;
	data=amx->data ? amx->data : amx->base+(int)hdr->dat;
	/* the number of bytes is on the stack, at "frm + 2*cell" */
	bytes= * (cell *)(data+(int)amx->frm+2*sizeof(cell));
	/* the number of arguments is the number of bytes divided
	* by the size of a cell */
	return bytes/sizeof(cell);
}

static cell core_getarg(AMX *amx,const cell *params)
{
	AMX_HEADER *hdr;
	uchar *data;
	cell value;

	hdr=(AMX_HEADER *)amx->base;
	data=amx->data ? amx->data : amx->base+(int)hdr->dat;
	/* get the base value */
	value= * (cell *)(data+(int)amx->frm+((int)params[1]+3)*sizeof(cell));
	/* adjust the address in "value" in case of an array access */
	value+=params[2]*sizeof(cell);
	/* get the value indirectly */
	value= * (cell *)(data+(int)value);
	return value;
}

static cell core_setarg(AMX *amx,const cell *params)
{
	AMX_HEADER *hdr;
	uchar *data;
	cell value;

	hdr=(AMX_HEADER *)amx->base;
	data=amx->data ? amx->data : amx->base+(int)hdr->dat;
	/* get the base value */
	value= * (cell *)(data+(int)amx->frm+((int)params[1]+3)*sizeof(cell));
	/* adjust the address in "value" in case of an array access */
	value+=params[2]*sizeof(cell);
	/* verify the address */
	if ( value<0 || (value>=amx->hea && value<amx->stk) )
		return 0;
	/* set the value indirectly */
	* (cell *)(data+(int)value) = params[3];
	return 1;
}

static cell core_heapspace(AMX *amx,const cell *params)
{
	(void)params;
	return amx->stk - amx->hea;
}


void amx_swapcell(cell *pc)
{
  union {
	cell c;
	#if PAWN_CELL_SIZE==16
	  uchar b[2];
	#elif PAWN_CELL_SIZE==32
	  uchar b[4];
	#elif PAWN_CELL_SIZE==64
	  uchar b[8];
	#else
	  #error Unsupported cell size
	#endif
  } value;
  uchar t;

  assert(pc!=nullptr);
  value.c = *pc;
  #if PAWN_CELL_SIZE==16
	t = value.b[0];
	value.b[0] = value.b[1];
	value.b[1] = t;
  #elif PAWN_CELL_SIZE==32
	t = value.b[0];
	value.b[0] = value.b[3];
	value.b[3] = t;
	t = value.b[1];
	value.b[1] = value.b[2];
	value.b[2] = t;
  #elif PAWN_CELL_SIZE==64
	t = value.b[0];
	value.b[0] = value.b[7];
	value.b[7] = t;
	t = value.b[1];
	value.b[1] = value.b[6];
	value.b[6] = t;
	t = value.b[2];
	value.b[2] = value.b[5];
	value.b[5] = t;
	t = value.b[3];
	value.b[3] = value.b[4];
	value.b[4] = t;
  #else
	#error Unsupported cell size
  #endif
  *pc = value.c;
}

static cell core_swapchars(AMX *amx,const cell *params)
{
  cell c;

  (void)amx;
  assert((size_t)params[0]==sizeof(cell));

  c=params[1];
  amx_swapcell(&c);
  return c;
}

static cell core_tolower(AMX *amx,const cell *params)
{
  (void)amx;
  #if defined __WIN32__ || defined _WIN32 || defined WIN32
	return (cell)CharLower((LPTSTR)params[1]);
  #elif defined _Windows
	return (cell)AnsiLower((LPSTR)params[1]);
  #else
	if ((unsigned)(params[1]-'A')<26u)
	  return params[1]+'a'-'A';
	return params[1];
  #endif
}

static cell core_toupper(AMX *amx,const cell *params)
{
  (void)amx;
  #if defined __WIN32__ || defined _WIN32 || defined WIN32
	return (cell)CharUpper((LPTSTR)params[1]);
  #elif defined _Windows
	return (cell)AnsiUpper((LPSTR)params[1]);
  #else
	if ((unsigned)(params[1]-'a')<26u)
	  return params[1]+'A'-'a';
	return params[1];
  #endif
}

static cell core_min(AMX *amx,const cell *params)
{
  (void)amx;
  return params[1] <= params[2] ? params[1] : params[2];
}

static cell core_max(AMX *amx,const cell *params)
{
  (void)amx;
  return params[1] >= params[2] ? params[1] : params[2];
}

static cell core_clamp(AMX *amx,const cell *params)
{
  cell value = params[1];
  if (params[2] > params[3])  /* minimum value > maximum value ! */
	amx_RaiseError(amx,AMX_ERR_NATIVE);
  if (value < params[2])
	value = params[2];
  else if (value > params[3])
	value = params[3];
  return value;
}


#if !defined AMX_NORANDOM
/* This routine comes from the book "Inner Loops" by Rick Booth, Addison-Wesley
 * (ISBN 0-201-47960-5). This is a "multiplicative congruential random number
 * generator" that has been extended to 31-bits (the standard C version returns
 * only 15-bits).
 */
#define INITIAL_SEED  0xcaa938dbL
static unsigned long IL_StandardRandom_seed = INITIAL_SEED; /* always use a non-zero seed */
#define IL_RMULT 1103515245L
#if defined __BORLANDC__ || defined __WATCOMC__
  #pragma argsused
#endif
static cell core_random(AMX *amx,const cell *params)
{
	unsigned long lo, hi, ll, lh, hh, hl;
	unsigned long result;

	/* one-time initialization (or, mostly one-time) */
	#if !defined SN_TARGET_PS2 && !defined _WIN32_WCE && !defined __ICC430__
		if (IL_StandardRandom_seed == INITIAL_SEED)
			IL_StandardRandom_seed=(unsigned long)time(nullptr);
	#endif

	(void)amx;

	lo = IL_StandardRandom_seed & 0xffff;
	hi = IL_StandardRandom_seed >> 16;
	IL_StandardRandom_seed = IL_StandardRandom_seed * IL_RMULT + 12345;
	ll = lo * (IL_RMULT  & 0xffff);
	lh = lo * (IL_RMULT >> 16    );
	hl = hi * (IL_RMULT  & 0xffff);
	hh = hi * (IL_RMULT >> 16    );
	result = ((ll + 12345) >> 16) + lh + hl + (hh << 16);
	result &= ~LONG_MIN;        /* remove sign bit */
	if (params[1]!=0)
		result %= params[1];
	return (cell)result;
}
#endif


static cell pawnDebugWatch(AMX *amx,const cell *params)
{
	amx->flags |= AMX_FLAG_WATCH;
	return 0;
}

#if defined __cplusplus
  extern "C"
#endif
const AMX_NATIVE_INFO core_Natives[] = {
  { "numargs",       core_numargs },
  { "getarg",        core_getarg },
  { "setarg",        core_setarg },
  { "heapspace",     core_heapspace },
  { "swapchars",     core_swapchars },
  { "tolower",       core_tolower },
  { "toupper",       core_toupper },
  { "min",           core_min },
  { "max",           core_max },
  { "clamp",         core_clamp },
  { "random",        core_random },
  { "DebugWatch",        pawnDebugWatch },
  { nullptr, nullptr }        /* terminator */
};
