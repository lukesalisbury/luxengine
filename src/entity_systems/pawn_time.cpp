/*	Date/time module for the Pawn Abstract Machine
 *
 *	Copyright (c) ITB CompuPhase, 2001-2009
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
 *	Version: $Id: amxtime.c 4057 2009-01-15 08:21:31Z thiadmer $
 */
/* Modified for Mokoi Gaming */

#include <time.h>
#include <assert.h>
#include "pawn_helper.h"

#if defined __GNUWIN32__ || defined __WIN32__ || defined _WIN32 || defined _Windows
	#include <windows.h>
	#include <mmsystem.h>
#endif

extern const AMX_NATIVE_INFO Time_Natives[];

#define SECONDS_PER_MINUTE	60
#define SECONDS_PER_HOUR	3600
#define SECONDS_PER_DAY		86400
#define SECONDS_PER_YEAR	31556952	/* based on 365.2425 days per year */

#if !defined CLOCKS_PER_SEC
	#define CLOCKS_PER_SEC CLK_TCK
#endif

static const unsigned char monthdays[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };


void stamp2datetime( time_t sec1970, cell * year, cell * month, cell * day, cell * hour, cell * minute, cell * second)
{
	cell days, seconds;

	/* find the year */
	assert(year!=NULL);
	for (*year = 1970; ; *year += 1) {
		days = 365 + ((*year & 0x03) == 0); /* clumsy "leap-year" routine, fails for 2100 */
		seconds = days * SECONDS_PER_DAY;
		if ((time_t)seconds > sec1970)
			break;
		sec1970 -= seconds;
	} /* if */

	/* find the month */
	assert(month!=NULL);
	for (*month = 1; ; *month += 1) {
		days = monthdays[*month - 1];
		seconds = days * SECONDS_PER_DAY;
		if ((time_t)seconds > sec1970)
			break;
		sec1970 -= seconds;
	} /* if */

	/* find the day */
	assert(day!=NULL);
	for (*day = 1; sec1970 >= SECONDS_PER_DAY; *day += 1)
		sec1970 -= SECONDS_PER_DAY;

	/* find the hour */
	assert(hour!=NULL);
	for (*hour = 0; sec1970 >= SECONDS_PER_HOUR; *hour += 1)
		sec1970 -= SECONDS_PER_HOUR;

	/* find the minute */
	assert(minute!=NULL);
	for (*minute = 0; sec1970 >= SECONDS_PER_MINUTE; *minute += 1)
		sec1970 -= SECONDS_PER_MINUTE;

	/* remainder is the number of seconds */
	assert(second!=NULL);
	*second = (int)sec1970;
}

/* pawnTimeGet
 * Time(&hour, &minute, &second)
 * The return value is the number of seconds since 1 January 1970 (Unix system
 * time).
 */
static cell pawnTimeGet(AMX *amx, const cell *params)
{
	time_t sec1970;
	struct tm gtm;
	cell *cptr;

	ASSERT_PAWN_PARAM( amx, params, 3 );

	time(&sec1970);

	/* on DOS/Windows, the timezone is usually not set for the C run-time
	 * library; in that case gmtime() and localtime() return the same value
	 */
	gtm = *localtime(&sec1970);

	write_amx_address(amx, params[1], gtm.tm_hour );
	write_amx_address(amx, params[2], gtm.tm_min );
	write_amx_address(amx, params[3], gtm.tm_sec );

	/* the time() function returns the number of seconds since January 1 1970
	 * in Universal Coordinated Time (the successor to Greenwich Mean Time)
	 */
	return (cell)sec1970;
}

/* pawnDateGet
 * Date(&year, &month, &day)
 * The return value is the number of days since the start of the year. January
 * 1 is day 1 of the year.
 */
static cell pawnDateGet(AMX *amx, const cell *params)
{
	time_t sec1970;
	struct tm gtm;
	cell *cptr;

	ASSERT_PAWN_PARAM( amx, params, 3 );

	time(&sec1970);

	/* on DOS/Windows, the timezone is usually not set for the C run-time
	 * library; in that case gmtime() and localtime() return the same value
	 */
	gtm = *localtime(&sec1970);

	write_amx_address(amx, params[1], gtm.tm_year + 1900);
	write_amx_address(amx, params[2], gtm.tm_mon + 1);
	write_amx_address(amx, params[3], gtm.tm_mday );


	return gtm.tm_yday+1;
}

/* cvttimestamp(seconds1970, &year, &month, &day, &hour, &minute, &second)
 */
static cell n_cvttimestamp(AMX *amx, const cell *params)
{
	time_t timestamp;
	cell year, month, day, hour, minute, second;
	cell * cptr;

	timestamp = (time_t)params[1];

	stamp2datetime(timestamp, &year, &month, &day, &hour, &minute, &second);

	write_amx_address(amx, params[2], year);
	write_amx_address(amx, params[3], month);
	write_amx_address(amx, params[4], day);
	write_amx_address(amx, params[5], hour);
	write_amx_address(amx, params[6], minute);
	write_amx_address(amx, params[7], second);

	return 0;
}

/** TimestampCurrent
* native TimestampCurrent();
*
*/
static cell pawnTimestampCurrent(AMX *amx, const cell *params)
{
	time_t timestamp;

	time( &timestamp );

	return (cell)timestamp;
}


const AMX_NATIVE_INFO Time_Natives[] = {
	{ "TimestampCurrent", pawnTimestampCurrent },
	{ "Time", pawnTimeGet },
	{ "Date", pawnDateGet },
	{ "TimestampDetails", n_cvttimestamp },
	{ 0, 0 }
};

