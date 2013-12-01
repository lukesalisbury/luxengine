/****************************
Copyright © 2006-2011 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "display.h"
#include "engine.h"
#include "entity_manager.h"

#include "pawn_helper.h"

extern const AMX_NATIVE_INFO console_Natives[];
static int cons_putstr(void * dest, const char * str)
{
	return fputs( str, stdout );
}

static int cons_putchar(void * dest, char ch)
{
	return putchar(ch);
}

static int debugmsg_putstr(void * dest,const char * str)
{
	lux::display->debug_msg << str;
	return 0;
}

static int debugmsg_putchar(void * dest, char ch)
{
	lux::display->debug_msg << ch;
	return 0;
}

/** n_printf
* native n_printf(text[], ...);
*/
static cell AMX_NATIVE_CALL n_printf(AMX *amx,const cell *params)
{
	cell *cstr;
	AMX_FMTINFO info;

	memset(&info,0,sizeof info);
	info.params=params+2;
	info.numparams=(int)(params[0]/sizeof(cell))-1;
	info.skip=0;
	info.length=INT_MAX;
	info.f_putstr = cons_putstr;
	info.f_putchar = cons_putchar;

	cstr = amx_Address(amx, params[1]);
	cons_putstr(NULL, "Game: ");
	amx_printstring(amx,cstr,&info);
	cons_putstr(NULL, "\n");
	fflush(stdout);
	return 0;
}

/** pawnDebugText
* native DebugText(text[], ...);
*/
static cell AMX_NATIVE_CALL pawnDebugText(AMX *amx, const cell *params)
{
	cell *cstr;
	AMX_FMTINFO info;

	memset(&info,0,sizeof info);
	info.params=params+2;
	info.numparams=(int)(params[0]/sizeof(cell))-1;
	info.skip=0;
	info.length=INT_MAX;
	info.f_putstr=debugmsg_putstr;
	info.f_putchar=debugmsg_putchar;

	cstr = amx_Address(amx, params[1]);
	lux::display->debug_msg << "> ";
	amx_printstring(amx,cstr,&info);
	lux::display->debug_msg << '\n';

	return 0;
}
/** pawnInfomationEntity
* native InfomationEntity();
*/
static cell AMX_NATIVE_CALL pawnInfomationEntity(AMX *amx, const cell *params)
{
	Entity * wanted = Lux_PawnEntity_GetParent(amx);


	lux::display->debug_msg << "> ";
	if ( wanted )
	{
		lux::display->debug_msg << wanted->Infomation() <<std::endl;
	}


	return 0;
}


const AMX_NATIVE_INFO console_Natives[] = {
	{ "DebugText", pawnDebugText },
	{ "InfomationEntity", pawnInfomationEntity },
	{ "printf", n_printf },
	{ NULL, NULL }				/* terminator */
};
