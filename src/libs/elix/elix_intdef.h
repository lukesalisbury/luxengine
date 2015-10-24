#ifndef _INTDEF_H_
#define _INTDEF_H_

	#ifdef __BEOS__
		#include <inttypes.h>
	#elif DREAMCAST
		#include <stdint.h>
/*
		typedef char int8_t;
		typedef unsigned char uint8_t;
		typedef short int16_t;
		typedef unsigned short uint16_t;
		typedef int int32_t;
		typedef unsigned int uint32_t;
		typedef long long int64_t;
		typedef unsigned long long uint64_t;
*/
	#else
		#include <stdint.h>
	#endif

#endif
