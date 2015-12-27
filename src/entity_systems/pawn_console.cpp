/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "display/display.h"
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
	lux::core->SystemMessage(SYSTEM_MESSAGE_DEBUG) << str;
	return 0;
}

static int debugmsg_putchar(void * dest, char ch)
{
	lux::core->SystemMessage(SYSTEM_MESSAGE_DEBUG) << ch;
	return 0;
}

static int debugcons_putstr(void * dest,const char * str)
{
	std::string * a = (std::string *)dest;

	a->append(str);

	return 0;
}

static int debugcons_putchar(void * dest, char ch)
{
	std::string * a = (std::string *)dest;

	a->append(1, ch);

	return 0;
}




/**
 * @brief pawnConsoleLog
 * @param amx
 * @param params (text[], ...)
 * @return
 */
static cell pawnConsoleLog(AMX *amx,const cell *params)
{
	if ( lux::display->show_debug )
	{
		std::string msg = "";
		cell *cstr;
		AMX_FMTINFO info;

		memset( &info, 0, sizeof info );
		info.params = params+2;
		info.numparams = (int)(params[0]/sizeof(cell))-1;
		info.skip = 0;
		info.length = INT_MAX;
		info.user = &msg;

		cstr = amx_Address(amx, params[1]);
		if ( cstr )
		{
		info.f_putstr = debugcons_putstr;
		info.f_putchar = debugcons_putchar;

		amx_printstring(amx,cstr,&info);

		lux::core->SystemMessage( SYSTEM_MESSAGE_VISUAL_WARNING, msg );
		}
	}
	else
	{
		cell *cstr;
		AMX_FMTINFO info;

		memset( &info, 0, sizeof info );
		info.params = params+2;
		info.numparams = (int)(params[0]/sizeof(cell))-1;
		info.skip = 0;
		info.length = INT_MAX;


		cstr = amx_Address(amx, params[1]);
		info.f_putstr = cons_putstr;
		info.f_putchar = cons_putchar;
		cons_putstr(NULL, "Game: ");
		amx_printstring(amx,cstr,&info);
		cons_putstr(NULL, "\n");
		fflush(stdout);
	}
	return 0;
}

/**
 * @brief pawnConsoleOutput
 * @param amx
 * @param params (text[], ...)
 * @return
 */
static cell pawnConsoleOutput(AMX *amx, const cell *params)
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
	lux::core->SystemMessage(SYSTEM_MESSAGE_DEBUG) << "> ";
	amx_printstring(amx,cstr,&info);
	lux::core->SystemMessage(SYSTEM_MESSAGE_DEBUG) << std::endl;

	return 0;
}

/**
 * @brief pawnConsoleInfomation
 * @param amx
 * @param params ()
 * @return
 */
static cell pawnConsoleInfomation(AMX *amx, const cell *params)
{
	Entity * wanted = Lux_PawnEntity_GetParent(amx);
	lux::core->SystemMessage(SYSTEM_MESSAGE_DEBUG) << "> ";
	if ( wanted )
	{
		lux::core->SystemMessage(SYSTEM_MESSAGE_DEBUG) << wanted->Infomation() <<std::endl;
	}


	return 0;
}


const AMX_NATIVE_INFO console_Natives[] = {
	{ "ConsoleOutput", pawnConsoleOutput },
	{ "ConsoleInfomation", pawnConsoleInfomation },
	{ "ConsoleLog", pawnConsoleLog },
	{ NULL, NULL }				/* terminator */
};
