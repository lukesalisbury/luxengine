/*	Fixed-point arithmetic for the Pawn Abstract Machine
 *
 *	Fixed point numbers compromise range versus number of decimals. This
 *	library decimal fixed point numbers with an configurable number of
 *	decimals. The current setting is 3 decimals.
 *
 *	Copyright (c) ITB CompuPhase, 1998-2009
 *
 *	This software is provided "as-is", without any express or implied warranty.
 *	In no event will the authors be held liable for any damages arising from
 *	the use of this software.
 *
 *	Permission is granted to anyone to use this software for any purpose,
 *	including commercial applications, and to alter it and redistribute it
 *	freely, subject to the following restrictions:
 *
 *	1.	The origin of this software must not be misrepresented; you must not
 *			claim that you wrote the original software. If you use this software in
 *			a product, an acknowledgment in the product documentation would be
 *			appreciated but is not required.
 *	2.	Altered source versions must be plainly marked as such, and must not be
 *			misrepresented as being the original software.
 *	3.	This notice may not be removed or altered from any source distribution.
 *
 *	Version: $Id: fixed.c 4057 2009-01-15 08:21:31Z thiadmer $
 */
 /* Modified for Mokoi Gaming */

#include <assert.h>
#include <stdio.h>
#include "pawn_helper.h"
#include "stdheader.h"
#include <math.h>

#define FIXEDPI  3.1415926535897932384626433832795

#if !defined isdigit
	#define isdigit(c)		((unsigned)((c)-'0')<10u)
#endif

#define MULTIPLIER		1000L	/* 10^decimals */


static cell n_fixedixed(AMX *amx,const cell *params)
{
	(void)amx;
	return params[1] * MULTIPLIER;
}

static cell n_strfixed(AMX *amx,const cell *params)
{
	char str[50],*ptr;
	cell *cstr,intpart,decimals;
	long multiplier,divisor;
	int len,sign=1;

	cstr=amx_Address(amx,params[1]);
	amx_StrLen(cstr,&len);
	if (len>=50) {
		amx_RaiseError(amx,AMX_ERR_NATIVE);
		return 0;
	} /* if */
	amx_GetString(str,cstr,0,UNLIMITED);
	ptr=str;
	intpart=0;
	decimals=0;
	multiplier=MULTIPLIER;
	divisor=1;
	while (*ptr!='\0' && *ptr<=' ')
		ptr++;							/* skip whitespace */
	if (*ptr=='-') {			/* handle sign */
		sign=-1;
		ptr++;
	} else if (*ptr=='+') {
		ptr++;
	} /* if */
	while (isdigit(*ptr)) {
		intpart=intpart*10 + (*ptr-'0');
		ptr++;
	} /* while */
	if (*ptr=='.') {
		ptr++;
		len=0;
		while (isdigit(*ptr) && len<8) {
			decimals=decimals*10 + (*ptr-'0');
			if (multiplier>1)
				multiplier/=10;
			else
				divisor*=10;
			ptr++;
			len++;
		} /* while */
	} /* if */
	return ((intpart*MULTIPLIER) + (decimals*multiplier+(divisor/2))/divisor) * sign;
}

/* Some C/C++ compilers have problems with long lists of definitions, so
 * I create another macro to fix this.
 */
#if PAWN_CELL_SIZE!=32
  /* the assembler and compiler-supported optimizations are only implemented
	 * for 32-bit cells
	 */
	#define USE_ANSI_C		1

#elif defined __WATCOMC__	&& defined __386__					 /* Watcom C/C++ */
	/* ANSI 64-bit division routine not needed for Watcom C/C++ because
	 * it uses inline assembler.
	 */
	#define USE_ANSI_C		0

#elif defined _MSC_VER && _MSC_VER>=9 && defined _WIN32 /* Microsoft C/C++ */
	/* ANSI 64-bit division routine not needed for Microsoft Visual C/C++
	 * because it supports 64-bit integers
	 */
	#define USE_ANSI_C		0

#elif defined __BORLANDC__	&& __BORLANDC__ >= 0x500 && defined __32BIT__ /* Borland C++ v.5 */
	/* ANSI 64-bit division routine not needed for Borland C++ because it
	 * supports 64-bit integers
	 */
	#define USE_ANSI_C		0

#elif defined __GNUC__																	/* GNU GCC */
	/* ANSI 64-bit division routine not needed for GNU GCC because it
	 * supports 64-bit integers
	 */
	#define USE_ANSI_C		0

#else

	#define USE_ANSI_C		1

#endif

#if USE_ANSI_C
	#define WORDSHIFT	 (PAWN_CELL_SIZE/2)

	#if PAWN_CELL_SIZE==32
		typedef unsigned short word_t;
		#define LOWORD(v)	 (word_t)((v) & 0xffffu)
		#define HIWORD(v)	 (word_t)(((v) >> 16) & 0xffffu)
	#elif PAWN_CELL_SIZE==64
		typedef uint32_t		word_t;
		#define LOWORD(v)	 (word_t)((v) & 0xffffffffu)
		#define HIWORD(v)	 (word_t)(((v) >> 32) & 0xffffffffu)
	#else
		#error Unsupported cell size
	#endif

	static ucell div64_32(ucell t[2], ucell divisor)
	{
		/* This function was adapted from source code that appeared in
		 * Dr. Dobb's Journal, August 1992, page 117.
		 */
		ucell u, v;
		word_t rHigh, rLow, dHigh, dLow;

		assert(divisor!=0);
		/* if the divisor is smaller than t[1], the result will not fit in a cell */
		assert(divisor>=t[1]);

		dHigh=HIWORD(divisor);
		dLow=LOWORD(divisor);

		/* Underestimate high half of quotient and subtract product
		 * of estimate and divisor from dividend.
		 */
		rHigh = (word_t)(t[1] / (dHigh + 1));
		u = (ucell)rHigh * (ucell)dLow;
		v = (ucell)rHigh * (ucell)dHigh;
		if ((t[0] -= (u << WORDSHIFT)) > ((ucell)-1L - (u << WORDSHIFT)))
			t[1]--;
		t[1] -= HIWORD(u);
		t[1] -= v;

		/* Correct estimate. */
		while ((t[1] > (ucell)dHigh) || ((t[1] == (ucell)dHigh) && (t[0] >= ((ucell)dLow << WORDSHIFT)))) {
			if ((t[0] -= ((ucell)dLow << WORDSHIFT)) > (ucell)-1L - ((ucell)dLow << WORDSHIFT))
				t[1]--;
			t[1] -= dHigh;
			rHigh++;
		} /* while */
		/* Underestimate low half of quotient and subtract product of
		 * estimate and divisor from what remains of dividend.
		 */
		rLow = (word_t) ((ucell)((t[1] << WORDSHIFT) + HIWORD(t[0])) / (dHigh + 1));
		u = (ucell)rLow * (ucell)dLow;
		v = (ucell)rLow * (ucell)dHigh;
		if ((t[0] -= u) > ((ucell)-1L - u))
			t[1]--;
		if ((t[0] -= (v << WORDSHIFT)) > ((ucell)-1L - (v << WORDSHIFT)))
			t[1]--;
		t[1] -= HIWORD(v);

		/* Correct estimate. */
		while ((t[1] > 0) || ((t[1] == 0) && t[0] >= divisor)) {
			if ((t[0] -= divisor) > ((ucell)-1L - divisor))
				t[1]--;
			rLow++;
		} /* while */

		return ((ucell)rHigh << WORDSHIFT) + rLow;
	}
#endif

static cell n_fixedmul(AMX *amx,const cell *params)
{
#if !USE_ANSI_C
#if defined __WATCOMC__ && defined __386__

	cell __fmul(void);
	cell a=params[1];
	cell b=params[2];
	#if MULTIPLIER != 1000
		#error Assembler chunks must be modified for a different base
	#endif
	#pragma aux __fmul =		\
		"mov		eax, [a]"		 \
		"mov		ebx, 1000"		\
		"imul	 [b]"					\
		"add		eax, 500"		 \
		"adc		edx, 0"			 \
		"idiv	 ebx"					\
		"mov		[a], eax"		 \
		modify [eax ebx edx];
	__fmul();
	(void)amx;
	return a;

#elif _MSC_VER>=9 && defined _WIN32

	__int64 a=(__int64)params[1] * (__int64)params[2];
	a=(a+MULTIPLIER/2) / MULTIPLIER;
	(void)amx;
	return (cell)a;

#elif defined __BORLANDC__	&& __BORLANDC__ >= 0x500 && (defined __32BIT__ || defined __WIN32__)

	__int64 a=(__int64)params[1] * (__int64)params[2];
	a=(a+MULTIPLIER/2) / MULTIPLIER;
	(void)amx;
	return (cell)a;

#elif defined __GNUC__

	long long a=(long long)params[1] * (long long)params[2];
	a=(a+MULTIPLIER/2) / MULTIPLIER;
	(void)amx;
	return (cell)a;

#else
  #error Unsupported compiler configuration, but USE_ANSI_C is false
#endif

#else // USE_ANSI_C

	/* (Xs * Ys) == (X*Y)ss, where "s" stands for scaled.
	 * The desired result is (X*Y)s, so we must unscale once.
	 * but we cannot do this before multiplication, because of loss
	 * of precision, and we cannot do it after the multiplication
	 * because of the possible overflow.
	 * The technique used here is to cut the multiplicands into
	 * components and to multiply these components separately:
	 *
	 * Assume Xs == (A << 16) + B and Ys == (C << 16) + D, where A, B,
	 * C and D are 16 bit numbers.
	 *
	 *		A B
	 *		C D
	 *		--- *
	 *		D*B + (D*A << 16) + (C*B << 16) + (C*A << (16+16))
	 *
	 * Thus we have built a 64-bit number, which can now be scaled back
	 * to 32-bit by dividing by the scale factor.
	 */
	#define ADD_WRAP(var,carry,expr)	(((var)+=(expr)), ((carry)+=((var)<(expr)) ? 1 : 0))
	ucell a,b,c,d;
	ucell v[2];
	cell sign=1;

	(void)amx;
	assert(MULTIPLIER<=(1L<<WORDSHIFT));

	/* make both operands positive values, but keep the sign of the result */
	if (params[1]<0) {
		((cell*)params)[1]=-params[1];
		sign=-sign;		 /* negate result */
	} /* if */
	if (params[2]<0) {
		((cell*)params)[2]=-params[2];
		sign=-sign;		 /* negate result */
	} /* if */

	a = HIWORD(params[1]);
	b = LOWORD(params[1]);
	c = HIWORD(params[2]);
	d = LOWORD(params[2]);

	/* store the intermediate into a 64-bit/128-bit number */
	v[1]=c*a;
	v[0]=d*b;
	ADD_WRAP(v[0],v[1],d*a << WORDSHIFT);
	ADD_WRAP(v[0],v[1],c*b << WORDSHIFT);

	/* add half of the divisor, to round the data */
	ADD_WRAP(v[0],v[1],MULTIPLIER/2);

	/* if the divisor is smaller than v[1], the result will not fit in a cell */
	if (MULTIPLIER<v[1]) {
		amx_RaiseError(amx,AMX_ERR_DOMAIN);
		return 0;
	} /* if */

	return (cell)div64_32(v,MULTIPLIER) * sign;
#endif
}

static cell n_fixeddiv(AMX *amx,const cell *params)
{
#if !USE_ANSI_C
#if defined __WATCOMC__ && defined __386__

	cell __fdiv(void);
	cell a=params[1];
	cell b=params[2];
	#if MULTIPLIER != 1000
		#error Assembler chunks must be modified for a different base
	#endif

	if (b==0) {
		amx_RaiseError(amx,AMX_ERR_DIVIDE);
		return 0;
	} /* if */

	#pragma aux __fdiv =		\
		"mov		eax, [a]"		 \
		"mov		ecx, [b]"		 \
		"cdq"								 \
		"mov		ebx, 1000"		\
		"imul	 ebx"					\
		"mov		ebx, ecx"		 \
		"shr		ecx, 1"			 \
		"add		eax, ecx"		 \
		"adc		edx, 0"			 \
		"idiv	 ebx"					\
		"mov		[a], eax"		 \
		modify [eax ebx ecx edx];
	__fdiv();
	return a;

#elif _MSC_VER>=9 && defined _WIN32

	__int64 a;
	cell divisor=params[2];
	if (divisor==0) {
		amx_RaiseError(amx,AMX_ERR_DIVIDE);
		return 0;
	} /* if */
	a=((__int64)params[1] * (__int64)MULTIPLIER + (__int64)(divisor/2)) / (__int64)divisor;
	return (cell)a;

#elif defined __BORLANDC__	&& __BORLANDC__ >= 0x500	&& (defined __32BIT__ || defined __WIN32__)

	__int64 a;
	cell divisor=params[2];
	if (divisor==0) {
		amx_RaiseError(amx,AMX_ERR_DIVIDE);
		return 0;
	} /* if */
	a=((__int64)params[1] * (__int64)MULTIPLIER + (__int64)(divisor/2)) / (__int64)divisor;
	return (cell)a;

#elif defined __GNUC__

	long long a;
	cell divisor=params[2];
	if (divisor==0) {
		amx_RaiseError(amx,AMX_ERR_DIVIDE);
		return 0;
	} /* if */
	a=((long long)params[1] * (long long)MULTIPLIER + (long long)(divisor/2)) / (long long)divisor;
	return (cell)a;

#else
  #error Unsupported compiler configuration, but USE_ANSI_C is false
#endif

#else // USE_ANSI_C

	/* The dividend must be scaled prior to division. The dividend
	 * is a 32-bit number, however, so when shifted, it will become
	 * a value that no longer fits in a 32-bit variable. This routine
	 * does the division by using only 16-bit and 32-bit values, but
	 * with considerable effort.
   * If your compiler supports 64-bit integers, modify this routine
	 * to use them. If your processor can do a simple 64-bit by 32-bit
	 * division in assembler, write assembler chunks.
	 * In other words: the straight C routine that follows is correct
	 * and portable, but use it only as a last resort.
	 *
	 * This function was adapted from source code that appeared in
	 * Dr. Dobb's Journal, August 1992, page 117.
	 */

	cell dividend=params[1];
	cell divisor=params[2];
	cell sign=1;
	ucell b[2];

	if (divisor==0) {
		amx_RaiseError(amx,AMX_ERR_NATIVE);
		return 0;
	} /* if */

	/* make both operands positive values, but keep the sign of the result */
	if (dividend<0) {
		dividend=-dividend;
		sign=-sign;		 /* negate result */
	} /* if */
	if (divisor<0) {
		divisor=-divisor;
		sign=-sign;		 /* negate result */
	} /* if */

	/* pre-scale the dividend into a 64-bit/128-bit number */
	b[0]=dividend*MULTIPLIER;
	b[1]=(HIWORD(dividend)*MULTIPLIER) >> WORDSHIFT;

	/* add half of the divisor, to round the data */
	b[0]+=(ucell)divisor/2;
	if (b[0]<(ucell)divisor/2)
		b[1]+=1;	/* wrap-around ocurred */

	/* if the divisor is smaller than b[1], the result will not fit in a cell */
	if ((ucell)divisor<b[1]) {
		amx_RaiseError(amx,AMX_ERR_DOMAIN);
		return 0;
	} /* if */

	return (cell)div64_32(b,(ucell)divisor) * sign;
#endif
}

static cell n_fixedmuldiv(AMX *amx,const cell *params)
{
#if !USE_ANSI_C
#if defined __WATCOMC__ && defined __386__

	cell __fmuldiv(void);
	cell a=params[1];
	cell b=params[2];
	cell c=params[3];

	if (c==0) {
		amx_RaiseError(amx,AMX_ERR_DIVIDE);
		return 0;
	} /* if */

	#pragma aux __fmuldiv = \
		"mov		eax, [a]"		 \
		"mov		ecx, [c]"		 \
		"imul	 [b]"					\
		"mov		ebx, ecx"		 \
		"shr		ecx, 1"			 \
		"add		eax, ecx"		 \
		"adc		edx, 0"			 \
		"idiv	 ebx"					\
		"mov		[a], eax"		 \
		modify [eax ebx ecx edx];
	__fmuldiv();
	return a;

#elif _MSC_VER>=9 && defined _WIN32

	__int64 a;
	cell divisor=params[3];
	if (divisor==0) {
		amx_RaiseError(amx,AMX_ERR_DIVIDE);
		return 0;
	} /* if */
	a=((__int64)params[1] * (__int64)params[2] + (__int64)(divisor/2)) / (__int64)divisor;
	return (cell)a;

#elif defined __BORLANDC__	&& __BORLANDC__ >= 0x500	&& (defined __32BIT__ || defined __WIN32__)

	__int64 a;
	cell divisor=params[3];
	if (divisor==0) {
		amx_RaiseError(amx,AMX_ERR_DIVIDE);
		return 0;
	} /* if */
	a=((__int64)params[1] * (__int64)params[2] + (__int64)(divisor/2)) / (__int64)divisor;
	return (cell)a;

#elif defined __GNUC__

	long long a;
	cell divisor=params[3];
	if (divisor==0) {
		amx_RaiseError(amx,AMX_ERR_DIVIDE);
		return 0;
	} /* if */
	a=((long long)params[1] * (long long)params[2] + (long long)(divisor/2)) / (long long)divisor;
	return (cell)a;

#else
  #error Unsupported compiler configuration, but USE_ANSI_C is false
#endif

#else // USE_ANSI_C

	ucell a,b,c,d;
	ucell v[2];
	cell sign=1;
	cell divisor=params[3];

	assert(MULTIPLIER<=(1L<<16));

	if (divisor==0) {
		amx_RaiseError(amx,AMX_ERR_DIVIDE);
		return 0;
	} /* if */

	/* make all three operands positive values, but keep the sign of the result */
	if (params[1]<0) {
		((cell*)params)[1]=-params[1];
		sign=-sign;		 /* negate result */
	} /* if */
	if (params[2]<0) {
		((cell*)params)[2]=-params[2];
		sign=-sign;		 /* negate result */
	} /* if */
	if (divisor<0) {
		divisor=-divisor;
		sign=-sign;		 /* negate result */
	} /* if */

	a = HIWORD(params[1]);
	b = LOWORD(params[1]);
	c = HIWORD(params[2]);
	d = LOWORD(params[2]);

	/* store the intermediate into a 64-bit/128-bit number */
	v[1]=c*a;
	v[0]=d*b;
	ADD_WRAP(v[0],v[1],d*a << WORDSHIFT);
	ADD_WRAP(v[0],v[1],c*b << WORDSHIFT);

	/* add half of the divisor, to round the data */
	ADD_WRAP(v[0],v[1],(ucell)divisor/2);

	/* if the divisor is smaller than v[1], the result will not fit in a cell */
	if ((ucell)divisor<v[1]) {
		amx_RaiseError(amx,AMX_ERR_DOMAIN);
		return 0;
	} /* if */

	return (cell)div64_32(v,(ucell)divisor) * sign;
#endif
}

static cell n_fixedfract(AMX *amx,const cell *params)
{
	(void)amx;
	return params[1] % MULTIPLIER;
}

static cell n_fixedround(AMX *amx,const cell *params)
{
	cell value;

	(void)amx;
	switch (params[2]) {
	case 1:			 /* round downwards (truncate) */
		value=params[1] / MULTIPLIER;
		if (params[1]<0 && (params[1] % MULTIPLIER)!=0)
			value--;
		break;
	case 2:			 /* round upwards */
		value=params[1] / MULTIPLIER;
		if (params[1]>=0 && (params[1] % MULTIPLIER)!=0)
			value++;
		break;
	case 3:			 /* truncate: round down when > 0, round up when < 0 */
		value=params[1] / MULTIPLIER;
		break;
	case 4:			 /* round to even number when fractional part is exactly 0.5 */
		value=(params[1] + MULTIPLIER/2) / MULTIPLIER;
		if ((params[1] % MULTIPLIER)==MULTIPLIER/2 && (value & 1)==1)
			value--;
		break;
	default:			/* standard (fractional part of 0.5 is rounded up */
		value=(params[1] + MULTIPLIER/2) / MULTIPLIER;
	} /* switch */
	return value;
}

/* Fixed:fsqroot(Fixed:value) */
static cell n_fixedsqroot(AMX *amx,const cell *params)
{
	cell value=params[1];
	cell low=0;
	cell high=value;
	cell mid[3]={8,0,0};

	if (value<0) {
		amx_RaiseError(amx, AMX_ERR_DOMAIN);
		return 0;
	} /* if */

	while (high-low > 1) {
		mid[1]=mid[2]=(low+high)/2;
		if (n_fixedmul(amx,mid) < value)
			low=mid[1];
		else
			high=mid[1];
	} /* while */

	/* check whether low or high mark comes closest */
	if (low!=high) {
		cell deltalow, deltahigh;
		mid[1]=mid[2]=low;
		deltalow=value-n_fixedmul(amx,mid);
		assert(deltalow>=0);
		mid[1]=mid[2]=high;
		deltahigh=n_fixedmul(amx,mid)-value;
		assert(deltahigh>=0);
		if (deltahigh<=deltalow)
			low=high; /* return "high" mark (comes closer to the answer) */
	} /* if */

	return low;
}

/* Fixed: fpower(Fixed:value, exponent)
 * note: x^n = exp(n*ln(x)), see http://yacas.sourceforge.net/Algochapter5.html
 */
static cell n_fixedpower(AMX *amx,const cell *params)
{
	#define LIMIT32	 146542L /* top value to calculate with extra digit, when a cell is 32-bit */
	cell result[3] = {8,0,0};
	int power=(int)params[2];
	int iter=1;
	int reciprocal=0;
	int isscaled=0;

	if (power<0) {
		reciprocal=1;
		power=-power;
	} /* if */
	if (power==0)
		return MULTIPLIER;

	/* quick squaring, to nearest power of 2 */
	result[1]=params[1];
	/* first try to do this with an extra digit of precision */
	if (result[1]<LIMIT32) {
		assert(sizeof(cell)>=4);
		result[1]*=10;											/* scale to have an extra digit */
		isscaled=1;
		while (2*iter<=power && result[1]<LIMIT32*10) {
			iter*=2;
			result[2]=result[1];
			result[1]=(n_fixedmul(amx,result)+5)/10;
		} /* while */
	} /* if */
	assert(2*iter>power || result[1]>=LIMIT32*10);
	if (result[1]>=LIMIT32*10) {
		result[1]=(result[1]+5)/10;				 /* undo scaling */
		isscaled=0;
	} /* if */
	while (2*iter<=power) {
		iter*=2;
		result[2]=result[1];
		result[1]=n_fixedmul(amx,result);
	} /* while */

	/* multilply with the remainder */
	if (iter<power && (isscaled || result[1]<LIMIT32)) {
		if (!isscaled) {
			result[1]*=10;										/* scale to have an extra digit */
			isscaled=1;
		} /* if */
		while (iter<power && result[1]<LIMIT32*10) {
			iter++;
			result[2]=params[1];
			result[1]=n_fixedmul(amx,result);
		} /* while */
	} /* if */
	if (isscaled) {
		result[1]=(result[1]+5)/10;				 /* undo scaling */
		/* isscaled=0; */
	} /* if */
	while (iter<power) {
		iter++;
		result[2]=params[1];
		result[1]=n_fixedmul(amx,result);
	} /* while */

	if (reciprocal) {
		result[2]=result[1];
		result[1]=MULTIPLIER;
		result[1]=n_fixeddiv(amx,result);			 /* result[1] = 1 / result[1] */
	} /* if */

	return result[1];
}

/* Fixed: fabs(Fixed:value)
 */
static cell n_fixedabs(AMX *amx,const cell *params)
{
	cell result=params[1];
	(void)amx;
	return (result>=0) ? result : -result;
}


/******************************************************************/
/** n_fixedlog
* native fixelog(Fixed:y, Fixed:x);
*/
static cell n_fixedlog(AMX *amx,const cell *params)
{
	/*
	*   params[0] = number of bytes
	*   params[1] = float operand 1 (value)
	*   params[2] = float operand 2 (base)
	*/
	float fValue = MAKE_FIXED_FLOAT(params[1]);
	float fBase = MAKE_FIXED_FLOAT(params[2]);
	(void)amx;
	if (fValue <= 0.0 || fBase <= 0)
		return amx_RaiseError(amx, AMX_ERR_DOMAIN);
	if (fBase == 10.0) // ??? epsilon
		fValue = (float)log10(fValue);
	else
		fValue = (float)(log(fValue) / log(fBase));
	return MAKE_FLOAT_FIXED(fValue);
}

static float n_toRadians(float angle, int radix)
{
	switch (radix)
	{
		case 1:         /* degrees, sexagesimal system (technically: degrees/minutes/seconds) */
			return (angle * FIXEDPI / 180.0);
		case 2:         /* grades, centesimal system */
			return (angle * FIXEDPI / 200.0);
		default:        /* assume already radian */
			return angle;
	} /* switch */
}


/** n_fixedsin
* native fsin(Fixed:angle, mode);
*/
static cell n_fixedsin(AMX *amx,const cell *params)
{
	(void)amx;
	float fA = n_toRadians( MAKE_FIXED_FLOAT(params[1]), params[2]);
	return MAKE_FLOAT_FIXED( sin(fA) );
}

/** n_fixedcos
* native fcos(Fixed:angle, mode);
*/
static cell n_fixedcos(AMX *amx,const cell *params)
{
	(void)amx;
	float fA = n_toRadians( MAKE_FIXED_FLOAT(params[1]), params[2]);
	return MAKE_FLOAT_FIXED(cos(fA));
}
/** n_fixedtan
* native ftan(Fixed:angle, mode);
*/
static cell n_fixedtan(AMX *amx,const cell *params)
{
	(void)amx;
	float fA = n_toRadians( MAKE_FIXED_FLOAT(params[1]), params[2]);
	return MAKE_FLOAT_FIXED(tan(fA));
}


/** n_fixedatan2
* native fixedatan2(Fixed:x, Fixed:y);
*/
static cell n_fixedatan2(AMX *amx, const cell *params)
{
	float x, y, result;
	x = MAKE_FIXED_FLOAT(params[1]);
	y = MAKE_FIXED_FLOAT(params[2]);
	result = atan2(y,x) * 180 / FIXEDPI;
	return MAKE_FLOAT_FIXED(result);
}

/******************************************************************/
#if defined __cplusplus
	extern "C"
#endif
const AMX_NATIVE_INFO fixed_Natives[] = {
	{ "fixed",		n_fixedixed },
	{ "strfixed", n_strfixed },
	{ "fmul",		 n_fixedmul },
	{ "fdiv",		 n_fixeddiv },
	{ "ffract",	 n_fixedfract },
	{ "fround",	 n_fixedround },
	{ "fmuldiv",	n_fixedmuldiv },
	{ "fsqroot",	n_fixedsqroot },
	{ "fpower",	 n_fixedpower },
	{ "fabs",		 n_fixedabs },

	{ "flog",	n_fixedlog },
	{ "fsin",	n_fixedsin },
	{ "fcos",	n_fixedcos },
	{ "ftan",	n_fixedtan },
	{ "fatan2",	n_fixedatan2 },


	{ NULL, NULL }				/* terminator */
};

int AMXEXPORT AMXAPI amx_FixedInit(AMX *amx)
{
	return amx_Register(amx,fixed_Natives,-1);
}

int AMXEXPORT AMXAPI amx_FixedCleanup(AMX *amx)
{
	(void)amx;
	return AMX_ERR_NONE;
}
