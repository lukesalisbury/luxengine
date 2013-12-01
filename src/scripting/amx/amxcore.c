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
#include "amx.h"
#include "amxcons.h"
#if defined __WIN32__ || defined _WIN32 || defined WIN32 || defined _Windows
  #include <windows.h>
#endif

/* A few compilers do not provide the ANSI C standard "time" functions */
#if !defined SN_TARGET_PS2 && !defined _WIN32_WCE && !defined __ICC430__
  #include <time.h>
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

static proplist proproot = { NULL, 0, NULL, 0 };

static proplist *list_additem(proplist *root)
{
  proplist *item;

  assert(root!=NULL);
  if ((item=(proplist *)malloc(sizeof(proplist)))==NULL)
    return NULL;
  item->name=NULL;
  item->id=0;
  item->value=0;
  item->next=root->next;
  root->next=item;
  return item;
}
static void list_delete(proplist *pred,proplist *item)
{
  assert(pred!=NULL);
  assert(item!=NULL);
  pred->next=item->next;
  assert(item->name!=NULL);
  free(item->name);
  free(item);
}
static void list_setitem(proplist *item,cell id,char *name,cell value)
{
  char *ptr;

  assert(item!=NULL);
  if ((ptr=(char *)malloc(strlen(name)+1))==NULL)
    return;
  if (item->name!=NULL)
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
  assert(name!=NULL);
  if (strlen(name)>0) {
    /* find by name */
    while (item!=NULL && (item->id!=id || stricmp(item->name,name)!=0)) {
      prev=item;
      item=item->next;
    } /* while */
  } else {
    /* find by value */
    while (item!=NULL && (item->id!=id || item->value!=value)) {
      prev=item;
      item=item->next;
    } /* while */
  } /* if */
  if (pred!=NULL)
    *pred=prev;
  return item;
}

static char *MakePackedString(cell *cptr)
{
  int len;
  char *dest;

  amx_StrLen(cptr,&len);
  dest=(char *)malloc(len+sizeof(cell));
  amx_GetString(dest,cptr,0,UNLIMITED);
  return dest;
}

static int verify_addr(AMX *amx,cell addr)
{
  int err;
  cell *cdest;

  err=amx_GetAddr(amx,addr,&cdest);
  if (err!=AMX_ERR_NONE)
    amx_RaiseError(amx,err);
  return err;
}

static cell AMX_NATIVE_CALL getproperty(AMX *amx,const cell *params)
{
  cell *cstr;
  char *name;
  proplist *item;

  amx_GetAddr(amx,params[2],&cstr);
  name=MakePackedString(cstr);
  item=list_finditem(&proproot,params[1],name,params[3],NULL);
  /* if list_finditem() found the value, store the name */
  if (item!=NULL && item->value==params[3] && strlen(name)==0) {
    int needed=(strlen(item->name)+sizeof(cell)-1)/sizeof(cell);     /* # of cells needed */
    if (verify_addr(amx,(cell)(params[4]+needed))!=AMX_ERR_NONE) {
      free(name);
      return 0;
    } /* if */
    amx_GetAddr(amx,params[4],&cstr);
    amx_SetString(cstr,item->name,1,0,UNLIMITED);
  } /* if */
  free(name);
  return (item!=NULL) ? item->value : 0;
}

static cell AMX_NATIVE_CALL setproperty(AMX *amx,const cell *params)
{
  cell prev=0;
  cell *cstr;
  char *name;
  proplist *item;

  amx_GetAddr(amx,params[2],&cstr);
  name=MakePackedString(cstr);
  item=list_finditem(&proproot,params[1],name,params[3],NULL);
  if (item==NULL)
    item=list_additem(&proproot);
  if (item==NULL) {
    amx_RaiseError(amx,AMX_ERR_MEMORY);
  } else {
    prev=item->value;
    if (strlen(name)==0) {
      free(name);
      amx_GetAddr(amx,params[4],&cstr);
      name=MakePackedString(cstr);
    } /* if */
    list_setitem(item,params[1],name,params[3]);
  } /* if */
  free(name);
  return prev;
}

static cell AMX_NATIVE_CALL delproperty(AMX *amx,const cell *params)
{
  cell prev=0;
  cell *cstr;
  char *name;
  proplist *item,*pred;

  amx_GetAddr(amx,params[2],&cstr);
  name=MakePackedString(cstr);
  item=list_finditem(&proproot,params[1],name,params[3],&pred);
  if (item!=NULL) {
    prev=item->value;
    list_delete(pred,item);
  } /* if */
  free(name);
  return prev;
}

static cell AMX_NATIVE_CALL existproperty(AMX *amx,const cell *params)
{
  cell *cstr;
  char *name;
  proplist *item;

  amx_GetAddr(amx,params[2],&cstr);
  name=MakePackedString(cstr);
  item=list_finditem(&proproot,params[1],name,params[3],NULL);
  free(name);
  return (item!=NULL);
}

#if defined __cplusplus
  extern "C"
#endif
const AMX_NATIVE_INFO core2_Natives[] = {
  { "getproperty",   getproperty },
  { "setproperty",   setproperty },
  { "deleteproperty",delproperty },
  { "existproperty", existproperty },
  { NULL, NULL }        /* terminator */
};

int AMXEXPORT AMXAPI amx_CoreInit(AMX *amx)
{
  return amx_Register(amx, core2_Natives, -1);
}

int AMXEXPORT AMXAPI amx_CoreCleanup(AMX *amx)
{
  while (proproot.next!=NULL)
    list_delete(&proproot,proproot.next);
  return AMX_ERR_NONE;
}
#endif
