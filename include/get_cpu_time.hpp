

#ifndef _GET_CPU_TIME_HPP_
	#define _GET_CPU_TIME_HPP_

	/*
	 * Author:  David Robert Nadeau
	 * Site:    http://NadeauSoftware.com/
	 * License: Creative Commons Attribution 3.0 Unported License
	 *          http://creativecommons.org/licenses/by/3.0/deed.en_US
	 */

	#include <unistd.h>	/* POSIX flags */
	#include <time.h>	/* clock_gettime(), time() */
	#include <sys/time.h>	/* gethrtime(), gettimeofday() */
#ifndef getCPUTime
	inline uint64_t get_cpu_time( )
	{

	#if defined(_POSIX_TIMERS) && (_POSIX_TIMERS > 0)
		{
			struct timespec ts;
		#if defined(CLOCK_MONOTONIC_PRECISE)
			/* BSD. --------------------------------------------- */
			const clockid_t id = CLOCK_MONOTONIC_PRECISE;
		#elif defined(CLOCK_MONOTONIC_RAW)
			/* Linux. ------------------------------------------- */
			const clockid_t id = CLOCK_MONOTONIC_RAW;
		#elif defined(CLOCK_HIGHRES)
			/* Solaris. ----------------------------------------- */
			const clockid_t id = CLOCK_HIGHRES;
		#elif defined(CLOCK_MONOTONIC)
			/* AIX, BSD, Linux, POSIX, Solaris. ----------------- */
			const clockid_t id = CLOCK_MONOTONIC;
		#elif defined(CLOCK_REALTIME)
			/* AIX, BSD, HP-UX, Linux, POSIX. ------------------- */
			const clockid_t id = CLOCK_REALTIME;
		#else
			const clockid_t id = (clockid_t)-1;	/* Unknown. */
		#endif /* CLOCK_* */
			if ( id != (clockid_t)-1 && clock_gettime( id, &ts ) != -1 )
				return (ts.tv_sec * 1000000000) + ts.tv_nsec;
			/* Fall thru. */
		}
	#endif /* _POSIX_TIMERS */
		struct timeval tm;
		gettimeofday( &tm, NULL );
		return (tm.tv_sec * 1000000000) + (tm.tv_usec * 1000);

	}
#endif
#endif //_GET_CPU_TIME_HPP_
