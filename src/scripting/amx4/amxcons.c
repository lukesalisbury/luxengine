/* Console output module (terminal I/O) for the Pawn AMX
 *
 *  Since some of these routines go further than those of standard C, they
 *  cannot always be implemented with portable C functions. In other words,
 *  these routines must be ported to other environments.
 *
 *  Copyright (c) ITB CompuPhase, 1997-2015
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may not
 *  use this file except in compliance with the License. You may obtain a copy
 *  of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 *  License for the specific language governing permissions and limitations
 *  under the License.
 *
 *  Version: $Id: amxcons.c 5181 2015-01-21 09:44:28Z thiadmer $
 */

#if defined _UNICODE || defined __UNICODE__ || defined UNICODE
# if !defined UNICODE   /* for Windows */
#   define UNICODE
# endif
# if !defined _UNICODE  /* for C library */
#   define _UNICODE
# endif
#endif

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#if defined __WIN32__ || defined _WIN32 || defined WIN32 || defined __MSDOS__
  #define HAVE_CONIO
  #include <conio.h>
  #include <malloc.h>
#endif
#if defined USE_CURSES || defined HAVE_CURSES_H
  #include <curses.h>
  #if !defined CURSES
	#define CURSES  1
  #endif
#endif
#include "osdefs.h"
#if defined __ECOS__
  /* eCos puts include files in cyg/package_name */
  #include <cyg/hal/hal_if.h>
  #include <cyg/infra/diag.h>
  #include <cyg/hal/hal_diag.h>
  #include <cyg/pawn/amx.h>
#else
  #include "amx.h"
#endif
#if defined __WIN32__ || defined _WIN32 || defined WIN32
  #include <windows.h>
#endif

#if defined _UNICODE
# include <tchar.h>
#elif !defined __T
  typedef char          TCHAR;
# define __T(string)    string
# define _fgetts        fgets
# define _puttchar      putchar
# define _stprintf      sprintf
# define _tcschr        strchr
# define _tcscpy        strcpy
# define _tcsdup        strdup
# define _tcslen        strlen
# define _tprintf       printf
#endif
#include "amxcons.h"

#if defined AMX_TERMINAL
  #define EOL_CHAR       '\r'
#endif
#if defined __WIN32__ || defined _WIN32 || defined WIN32 || defined __MSDOS__
  #define EOL_CHAR       '\r'
#endif
#if !defined EOL_CHAR
  /* if not a "known" operating system, assume Linux/Unix */
  #define EOL_CHAR     '\n'
#endif

enum {
  SV_DECIMAL,
  SV_HEX
};

static TCHAR *reverse(TCHAR *string,int stop)
{
	int start=0;
	TCHAR temp;

	/* swap the string */
	stop--;				/* avoid swapping the '\0' byte to the first position */
	while (stop - start > 0) {
		temp = string[start];
		string[start] = string[stop];
		string[stop] = temp;
		start++;
		stop--;
	} /* while */
	return string;
}

/* Converts an integral value to a string, with optional padding with spaces or
 * zeros.
 * The "format" must be decimal or hexadecimal
 * The number is right-aligned in the field with the size of the absolute value
 * of the "width" parameter.
 * If the width value is positive, the string is padded with spaces; if it is
 * negative, it is padded with zeros.
 */
static TCHAR *amx_strval(TCHAR buffer[], long value, int format, int width)
{
	int start, stop;
	TCHAR temp;

	start = stop = 0;
	if (format == SV_DECIMAL) {
		if (value < 0) {
			buffer[0] = __T('-');
			start = stop = 1;
			value = -value;
		} /* if */
		do {
			buffer[stop++] = (TCHAR)((value % 10) + __T('0'));
			value /= 10;
		} while (value > 0);
	} else {
		/* hexadecimal */
		unsigned long v = (unsigned long)value;	/* copy to unsigned value for shifting */
		do {
			buffer[stop] = (TCHAR)((v & 0x0f) + __T('0'));
			if (buffer[stop] > __T('9'))
				buffer[stop] += (TCHAR)(__T('A') - __T('0') - 10);
			v >>= 4;
			stop++;
		} while (v != 0);
	} /* if */

	/* pad to given width */
	if (width < 0) {
		temp = __T('0');
		width = -width;
	} else {
		temp = __T(' ');
	} /* if */
	while (stop < width)
		buffer[stop++] = temp;

	buffer[stop] = __T('\0');

	/* swap the string, and we are done */
	reverse(buffer+start,stop-start);
	return buffer;
}

#if defined FIXEDPOINT
  #define FIXEDMULT     1000
  #define FIXEDDIGITS   3

static TCHAR *formatfixed(TCHAR *string,cell value,TCHAR align,int width,TCHAR decpoint,int digits,TCHAR filler)
{
  int i, len;
  cell ipart,v;
  TCHAR vsign=__T('\0');

  /* make the value positive (but keep the sign) */
  if (value<0) {
	value=-value;
	vsign=__T('-');
  } /* if */

  /* "prepare" the value so that when it is truncated to the requested
   * number of digits, the result is rounded towards the dropped digits
   */
  assert(digits<INT_MAX);
  v=FIXEDMULT/2;
  for (i=0; i<digits; i++)
	v/=10;
  value+=v;

  /* get the integer part and remove it from the value */
  ipart=value/FIXEDMULT;
  value-=FIXEDMULT*ipart;
  assert(ipart>=0);
  assert(value>=0);

  /* truncate the fractional part to the requested number of digits */
  for (i=FIXEDDIGITS; i>digits; i--)
	value/=10;

  string[0]=__T('\0');

  /* add sign */
  i=(int)_tcslen(string);
  string[i]=vsign;
  string[i+1]=__T('\0');

  /* add integer part */
  amx_strval(string+_tcslen(string),(long)ipart,SV_DECIMAL,0);

  /* add fractional part */
  if (digits>0) {
    i=(int)_tcslen(string);
	string[i]=decpoint;
	amx_strval(string+i+1,(long)value,SV_DECIMAL,-digits);
  } /* if */

  len=(int)_tcslen(string);
  if (len<width) {
	/* pad to the requested width */
	for (i=len; i<width; i++)
	  string[i]=filler;
	string[i]=__T('\0');
	/* optionally move the padding to the beginning of the string, using the handwaving algorithm */
	if (align!=__T('-')) {
	  assert(i==(int)_tcslen(string));
	  assert(i>=len);
	  reverse(string,len);
	  reverse(string+len,i-len);
	  reverse(string,i);
	} /* if */
  } /* if */

  return string;
}
#endif


static int dochar(AMX *amx,TCHAR ch,cell param,TCHAR sign,TCHAR decpoint,int width,int digits,TCHAR filler,
				  int (*f_putstr)(void*,const TCHAR *),int (*f_putchar)(void*,TCHAR),void *user)
{
  cell *cptr;
  TCHAR buffer[40];
  #if defined FLOATPOINT
	TCHAR formatstring[40];
  #endif

  #if !defined FIXEDPOINT && !defined FLOATPOINT
	(void)decpoint;
  #endif
  assert(f_putstr!=NULL);
  assert(f_putchar!=NULL);

  switch (ch) {
  case __T('c'):
	cptr=amx_Address(amx,param);
	width--;            /* single character itself has a with of 1 */
	if (sign!=__T('-'))
	  while (width-->0)
		f_putchar(user,filler);
	f_putchar(user,(TCHAR)*cptr);
	while (width-->0)
	  f_putchar(user,filler);
	return 1;

  case __T('d'): {
	cell value;
	int length=1;
	cptr=amx_Address(amx,param);
	value=*cptr;
	if (value<0 || sign==__T('+'))
	  length++;
	if (value<0)
	  value=-value;
	while (value>=10) {
	  length++;
	  value/=10;
	} /* while */
	width-=length;
	if (sign!=__T('-'))
	  while (width-->0)
		f_putchar(user,filler);
	amx_strval(buffer,*cptr,SV_DECIMAL,0);
	if (sign==__T('+') && *cptr>=0)
	  f_putchar(user,sign);
	f_putstr(user,buffer);
	while (width-->0)
	  f_putchar(user,filler);
	return 1;
  } /* case */

#if defined FLOATPOINT
  case __T('f'): /* 32-bit floating point number */
  case __T('r'): /* if floating point is enabled, %r == %f */
	/* build a format string */
	if (digits==INT_MAX)
	  digits=5;
	else if (digits>25)
	  digits=25;
	_tcscpy(formatstring,__T("%"));
	if (sign!=__T('\0'))
	  _stprintf(formatstring+_tcslen(formatstring),__T("%c"),sign);
	if (width>0)
	  _stprintf(formatstring+_tcslen(formatstring),__T("%d"),width);
	_stprintf(formatstring+_tcslen(formatstring),__T(".%df"),digits);
	cptr=amx_Address(amx,param);
	#if PAWN_CELL_SIZE == 64
	  _stprintf(buffer,formatstring,*(double*)cptr);
	#else
	  _stprintf(buffer,formatstring,*(float*)cptr);
	#endif
	if (decpoint==__T(',')) {
	  TCHAR *ptr=_tcschr(buffer,__T('.'));
	  if (ptr!=NULL)
		*ptr=__T(',');
	} /* if */
	f_putstr(user,buffer);
	return 1;
#endif

#if defined FIXEDPOINT
  #define FIXEDMULT 1000
  case __T('q'): /* 32-bit fixed point number */
#if !defined FLOATPOINT
  case __T('r'): /* if fixed point is enabled, and floating point is not, %r == %q */
#endif
	cptr=amx_Address(amx,param);
	/* format the number */
	if (digits==INT_MAX)
	  digits=3;
	else if (digits>25)
	  digits=25;
	formatfixed(buffer,*cptr,sign,width,decpoint,digits,filler);
	assert(_tcslen(buffer)<sizeof buffer);
	f_putstr(user,buffer);
	return 1;
#endif

#if !defined FLOATPOINT && !defined FIXEDPOINT
  case __T('f'):
  case __T('q'):
  case __T('r'):
	f_putstr(user,__T("(no rational number support)"));
	return 0; /* flag this as an error */
#endif

  case __T('s'): {
	AMX_FMTINFO info;
	memset(&info,0,sizeof info);
	info.length=digits;
	info.f_putstr=f_putstr;
	info.f_putchar=f_putchar;
	info.user=user;
	cptr=amx_Address(amx,param);
	amx_printstring(amx,cptr,&info);
	return 1;
  } /* case */

  case __T('x'): {
	ucell value;
	int length=1;
	cptr=amx_Address(amx,param);
	value=*(ucell*)cptr;
	while (value>=0x10) {
	  length++;
	  value>>=4;
	} /* while */
	width-=length;
	if (sign!=__T('-'))
	  while (width-->0)
		f_putchar(user,filler);
	amx_strval(buffer,(long)*cptr,SV_HEX,0);
	f_putstr(user,buffer);
	while (width-->0)
	  f_putchar(user,filler);
	return 1;
  } /* case */

  } /* switch */
  /* error in the string format, try to repair */
  f_putchar(user,ch);
  return 0;
}

enum {
  FMT_NONE,   /* not in format state; accept '%' */
  FMT_START,  /* found '%', accept '+', '-' (START), '0' (filler; START), digit (WIDTH), '.' (DECIM), or '%' or format letter (done) */
  FMT_WIDTH,  /* found digit after '%' or sign, accept digit (WIDTH), '.' (DECIM) or format letter (done) */
  FMT_DECIM,  /* found digit after '.', accept accept digit (DECIM) or format letter (done) */
};

static int formatstate(TCHAR c,int *state,TCHAR *sign,TCHAR *decpoint,int *width,int *digits,TCHAR *filler)
{
  assert(state!=NULL && sign!=NULL && decpoint!=NULL && width!=NULL && digits!=NULL && filler!=NULL);
  switch (*state) {
  case FMT_NONE:
	if (c==__T('%')) {
	  *state=FMT_START;
	  *sign=__T('\0');
	  *decpoint=__T('.');
	  *width=0;
	  *digits=INT_MAX;
	  *filler=__T(' ');
	} else {
	  return -1;  /* print a single character */
	} /* if */
	break;
  case FMT_START:
	if (c==__T('+') || c==__T('-')) {
	  *sign=c;
	} else if (c==__T('0')) {
	  *filler=c;
	} else if (c>=__T('1') && c<=__T('9')) {
	  *width=(int)(c-__T('0'));
	  *state=FMT_WIDTH;
	} else if (c==__T('.') || c==__T(',')) {
	  *decpoint=c;
	  *digits=0;
	  *state=FMT_DECIM;
	} else if (c==__T('%')) {
	  *state=FMT_NONE;
	  return -1;  /* print literal '%' */
	} else {
	  return 1;   /* print formatted character */
	} /* if */
	break;
  case FMT_WIDTH:
	if (c>=__T('0') && c<=__T('9')) {
	  *width=*width*10+(int)(c-__T('0'));
	} else if (c==__T('.') || c==__T(',')) {
	  *decpoint=c;
	  *digits=0;
	  *state=FMT_DECIM;
	} else {
	  return 1;   /* print formatted character */
	} /* if */
	break;
  case FMT_DECIM:
	if (c>=__T('0') && c<=__T('9')) {
	  *digits=*digits*10+(int)(c-__T('0'));
	} else {
	  return 1;   /* print formatted character */
	} /* if */
	break;
  } /* switch */

  return 0;
}

int amx_printstring(AMX *amx,cell *cstr,AMX_FMTINFO *info)
{
  int i,paramidx=0;
  int fmtstate=FMT_NONE,width,digits;
  TCHAR sign,decpoint,filler;
  int (*f_putstr)(void*,const TCHAR *);
  int (*f_putchar)(void*,TCHAR);
  void *user;
  int skip,length;

  if (info!=NULL) {
	f_putstr=info->f_putstr;
	f_putchar=info->f_putchar;
	user=info->user;
	skip=info->skip;
	length=info->length;
  } else {
	f_putstr=NULL;
	f_putchar=NULL;
	user=NULL;
	skip=0;
	length=INT_MAX;
  } /* if */
  assert(f_putstr!=NULL && f_putchar!=NULL);


  /* if no placeholders appear, we can use a quicker routine */
  if (info==NULL || info->params==NULL) {

	TCHAR cache[100];
	int idx=0;

	if ((ucell)*cstr>UNPACKEDMAX) {
	  int j=sizeof(cell)-sizeof(char);
	  char c;
	  /* the string is packed */
	  i=0;
	  for ( ; ; ) {
		c=(char)((ucell)cstr[i] >> 8*j);
		if (c==0)
		  break;
		if (skip>0) {
		  skip--;               /* skip a number of characters */
		} else {
		  if (length--<=0)
			break;              /* print up to a certain length */
		  assert(idx<sizeof cache);
		  cache[idx++]=c;
		  if (idx==sizeof cache - 1) {
			cache[idx]=__T('\0');
			f_putstr(user,cache);
			idx=0;
		  } /* if */
		} /* if */
		if (j==0)
		  i++;
		j=(j+sizeof(cell)-sizeof(char)) % sizeof(cell);
	  } /* for */
	} else {
	  /* unpacked string */
	  for (i=0; cstr[i]!=0; i++) {
		if (skip-->0)
		  continue;
		assert(idx<sizeof cache);
		cache[idx++]=(TCHAR)cstr[i];
		if (idx==sizeof cache - 1) {
		  cache[idx]=__T('\0');
		  f_putstr(user,cache);
		  idx=0;
		} /* if */
	  } /* for */
	} /* if */
	if (idx>0) {
	  cache[idx]=__T('\0');
	  f_putstr(user,cache);
	} /* if */

  } else {

	/* check whether this is a packed string */
	if ((ucell)*cstr>UNPACKEDMAX) {
	  int j=sizeof(cell)-sizeof(char);
	  char c;
	  /* the string is packed */
	  i=0;
	  for ( ; ; ) {
		c=(char)((ucell)cstr[i] >> 8*j);
		if (c==0)
		  break;
		switch (formatstate(c,&fmtstate,&sign,&decpoint,&width,&digits,&filler)) {
		case -1:
		  f_putchar(user,c);
		  break;
		case 0:
		  break;
		case 1:
		  assert(info!=NULL && info->params!=NULL);
		  if (paramidx>=info->numparams)  /* insufficient parameters passed */
			amx_RaiseError(amx, AMX_ERR_NATIVE);
		  else
			paramidx+=dochar(amx,c,info->params[paramidx],sign,decpoint,width,digits,filler,
							 f_putstr,f_putchar,user);
		  fmtstate=FMT_NONE;
		  break;
		default:
		  assert(0);
		} /* switch */
		if (j==0)
		  i++;
		j=(j+sizeof(cell)-sizeof(char)) % sizeof(cell);
	  } /* for */
	} else {
	  /* the string is unpacked */
	  for (i=0; cstr[i]!=0; i++) {
		switch (formatstate((TCHAR)cstr[i],&fmtstate,&sign,&decpoint,&width,&digits,&filler)) {
		case -1:
		  f_putchar(user,(TCHAR)cstr[i]);
		  break;
		case 0:
		  break;
		case 1:
		  assert(info!=NULL && info->params!=NULL);
		  if (paramidx>=info->numparams)  /* insufficient parameters passed */
			amx_RaiseError(amx, AMX_ERR_NATIVE);
		  else
			paramidx+=dochar(amx,(TCHAR)cstr[i],info->params[paramidx],sign,decpoint,width,digits,filler,
							 f_putstr,f_putchar,user);
		  fmtstate=FMT_NONE;
		  break;
		default:
		  assert(0);
		} /* switch */
	  } /* for */
	} /* if */

  } /* if (info==NULL || info->params==NULL) */

  return paramidx;
}

