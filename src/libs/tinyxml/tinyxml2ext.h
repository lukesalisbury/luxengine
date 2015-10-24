/*
Original code by Lee Thomason (www.grinninglizard.com)

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any
damages arising from the use of this software.

Permission is granted to anyone to use this software for any
purpose, including commercial applications, and to alter it and
redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must
not claim that you wrote the original software. If you use this
software in a product, an acknowledgment in the product documentation
would be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and
must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source
distribution.
*/
#ifndef TINYXML2EXT_INCLUDED
#define TINYXML2EXT_INCLUDED
#include "tinyxml2.h"
#include <string>
#include <stdint.h>

namespace tinyxml2
{
	int QueryStringAttribute( XMLElement * element, const char* name, std::string & _value );
	int QueryUint8Attribute( XMLElement * element, const std::string& name, uint8_t & outValue );

	template<typename T> int QueryUnsignedAttribute( XMLElement * element, const std::string& name, T & outValue );
	template<typename T> int QueryIntAttribute( XMLElement * element, const std::string& name,  T & outValue, int default_value = 0 );
}


#endif
