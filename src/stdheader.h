/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef PROGRAM_NAME
#define PROGRAM_NAME "Mokoi"
#endif

#ifndef PROGRAM_VERSION
#define PROGRAM_VERSION "Nightly " __DATE__ " " __TIME__
#endif

#ifndef PROGRAM_VERSION_STABLE
#define PROGRAM_VERSION_STABLE "1.0"
#endif

#ifndef PROGRAM_DOCUMENTS
#define PROGRAM_DOCUMENTS "mokoi-games"
#endif

#ifndef PACKAGE_GET_URL
#define PACKAGE_GET_URL "http://localhost/get/"
#endif

#ifndef STDHEADER_H
#define STDHEADER_H

#define LUX_INIT_BASE		0x00000001
#define LUX_INIT_AUDIO		0x00000010
#define LUX_INIT_VIDEO		0x00000020
#define LUX_INIT_NETWORK	0x00100000

#define LUX_SAVE_DATA_TYPE 1
#define LUX_SAVE_HIBERNATE_TYPE 2
#define LUX_SAVE_READ_COOKIE_TYPE 3
#define LUX_SAVE_COOKIE_TYPE 4

#define OBJECT_GLOBAL_VALUE 0x80000000
#define OBJECT_LOCAL_VALUE 0x7FFFFFFF

#include <iostream>
#include <string>
#include <cstring>
#include <cstdarg>
#include <ctime>
#include "libs/elix/elix_intdef.h"

typedef intptr_t native_type;
typedef void * native_pointer;
typedef void * mem_pointer;
typedef int32_t fixed;
typedef uint32_t int_hash;

#include "libs/elix/elix_file.hpp"


#define UNUSEDARG __attribute__((unused))

#define TIMER_START(function) 
#define TIMER_END(function)

#define NULLIFY(x) if (x != nullptr) { delete(x); x = nullptr; }
#define NULLIFY_ARRAY(x) if (x != nullptr) { delete [] x; x = nullptr; }

#define IS_BIT_SET(value, bit) (value) & ((1<<(bit)))

#define FRACT_BITS 1000

#define MAKE_INT_FIXED(x) ((x) * FRACT_BITS)
#define MAKE_FIXED_INT(x) ((x) / FRACT_BITS)

#define MAKE_FLOAT_FIXED(x) ((fixed)((x) * FRACT_BITS))
#define MAKE_FIXED_FLOAT(x) (((float)(x)) / FRACT_BITS)

#define FIXED_MULT(x, y) ((x*y) / 1000000)
#define FIXED_DIV(x, y) (((x)<<FRACT_BITS) / (y))

inline int32_t clamp(int32_t x, int32_t a, int32_t b)
{
	return (x < a ? a : (x > b ? b : x));
}

#endif
