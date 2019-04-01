/*  Core module for the Pawn AMX
 *
 *  Copyright (c) ITB CompuPhase, 1997-2012
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
 *  Version: $Id: amxcore.c 4708 2012-05-18 12:52:49Z thiadmer $
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
#include "osdefs.h"
#if defined __ECOS__
  /* eCos puts include files in cyg/package_name */
  #include <cyg/pawn/amx.h>
  #define  stricmp(a,b) strcasecmp(a,b)
#else
  #include "amx.h"
#endif
#if defined __WIN32__ || defined _WIN32 || defined WIN32 || defined _Windows
  #include <windows.h>
#endif

/* A few compilers do not provide the ANSI C standard "time" functions */
#if !defined SN_TARGET_PS2 && !defined _WIN32_WCE && !defined __ICC430__
  #include <time.h>
#endif

#if defined _UNICODE
# include <tchar.h>
#elif !defined __T
  typedef char          TCHAR;
# define __T(string)    string
# define _tcschr        strchr
# define _tcscpy        strcpy
# define _tcsdup        strdup
# define _tcslen        strlen
#endif


#define CHARBITS        (8*sizeof(char))
typedef unsigned char   uchar;

#if !defined AMX_NOPROPLIST
typedef struct _property_list {
  struct _property_list *next;
  cell id;
  char *name;
  cell value;
} proplist;

static proplist proproot = { nullptr, 0, nullptr, 0 };

static proplist *list_additem(proplist *root)
{
  proplist *item;

  assert(root!=nullptr);
  if ((item=(proplist *)malloc(sizeof(proplist)))==nullptr)
	return nullptr;
  item->name=nullptr;
  item->id=0;
  item->value=0;
  item->next=root->next;
  root->next=item;
  return item;
}
static void list_delete(proplist *pred,proplist *item)
{
  assert(pred!=nullptr);
  assert(item!=nullptr);
  pred->next=item->next;
  assert(item->name!=nullptr);
  free(item->name);
  free(item);
}
static void list_setitem(proplist *item,cell id,char *name,cell value)
{
  char *ptr;

  assert(item!=nullptr);
  if ((ptr=(char *)malloc(strlen(name)+1))==nullptr)
	return;
  if (item->name!=nullptr)
	free(item->name);
  strcpy(ptr,name);
  item->name=ptr;
  item->id=id;
  item->value=value;
}
static proplist *list_finditem(proplist *root,cell id,char *name,cell value,
							   proplist **pred)
{
  proplist *item=root->next;
  proplist *prev=root;

  /* check whether to find by name or by value */
  assert(name!=nullptr);
  if (strlen(name)>0) {
	/* find by name */
	while (item!=nullptr && (item->id!=id || stricmp(item->name,name)!=0)) {
	  prev=item;
	  item=item->next;
	} /* while */
  } else {
	/* find by value */
	while (item!=nullptr && (item->id!=id || item->value!=value)) {
	  prev=item;
	  item=item->next;
	} /* while */
  } /* if */
  if (pred!=nullptr)
	*pred=prev;
  return item;
}
#endif




#if !defined AMX_NOPROPLIST
static char *MakePackedString(cell *cptr)
{
  int len;
  char *dest;

  amx_StrLen(cptr,&len);
  dest=(char *)malloc(len+sizeof(cell));
  amx_GetString(dest,cptr,0,len+sizeof(cell));
  return dest;
}

/* getproperty(id=0, const name[]="", value=cellmin, string[]="", size=sizeof string) */
static cell AMX_NATIVE_CALL getproperty(AMX *amx,const cell *params)
{
  cell *cstr;
  char *name;
  proplist *item;

  (void)amx;
  cstr=amx_Address(amx,params[2]);
  name=MakePackedString(cstr);
  item=list_finditem(&proproot,params[1],name,params[3],nullptr);
  /* if list_finditem() found the value, store the name */
  if (item!=nullptr && item->value==params[3] && strlen(name)==0) {
	cstr=amx_Address(amx,params[4]);
	amx_SetString(cstr,item->name,1,0,params[5]);
  } /* if */
  free(name);
  return (item!=nullptr) ? item->value : 0;
}

/* setproperty(id=0, const name[]="", value=cellmin, const string[]="") */
static cell AMX_NATIVE_CALL setproperty(AMX *amx,const cell *params)
{
  cell prev=0;
  cell *cstr;
  char *name;
  proplist *item;

  cstr=amx_Address(amx,params[2]);
  name=MakePackedString(cstr);
  item=list_finditem(&proproot,params[1],name,params[3],nullptr);
  if (item==nullptr)
	item=list_additem(&proproot);
  if (item==nullptr) {
	amx_RaiseError(amx,AMX_ERR_MEMORY);
  } else {
	prev=item->value;
	if (strlen(name)==0) {
	  free(name);
	  cstr=amx_Address(amx,params[4]);
	  name=MakePackedString(cstr);
	} /* if */
	list_setitem(item,params[1],name,params[3]);
  } /* if */
  free(name);
  return prev;
}

/* deleteproperty(id=0, const name[]="", value=cellmin) */
static cell AMX_NATIVE_CALL delproperty(AMX *amx,const cell *params)
{
  cell prev=0;
  cell *cstr;
  char *name;
  proplist *item,*pred;

  (void)amx;
  cstr=amx_Address(amx,params[2]);
  name=MakePackedString(cstr);
  item=list_finditem(&proproot,params[1],name,params[3],&pred);
  if (item!=nullptr) {
	prev=item->value;
	list_delete(pred,item);
  } /* if */
  free(name);
  return prev;
}

/* existproperty(id=0, const name[]="", value=cellmin) */
static cell AMX_NATIVE_CALL existproperty(AMX *amx,const cell *params)
{
  cell *cstr;
  char *name;
  proplist *item;

  (void)amx;
  cstr=amx_Address(amx,params[2]);
  name=MakePackedString(cstr);
  item=list_finditem(&proproot,params[1],name,params[3],nullptr);
  free(name);
  return (item!=nullptr);
}
#endif



