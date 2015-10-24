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

#include "tinyxml2ext.h"
#include <iostream>

namespace tinyxml2
{
	int QueryStringAttribute( XMLElement * element, const char* name, std::string & _value )
	{
		const char * cstr = element->Attribute( name );
		if ( cstr ) {
			_value = std::string( cstr );
			return XML_SUCCESS;
		}
		_value = std::string("");
		return XML_NO_ATTRIBUTE;
	}

	int QueryUint8Attribute( XMLElement * element, const std::string& name, uint8_t & outValue )
	{
		int result = XML_SUCCESS;

		outValue = (uint8_t)element->UnsignedAttribute( name.c_str() );

		return result;
	}

	template<typename T> int QueryUnsignedAttribute( XMLElement * element, const std::string& name,  T & outValue )
	{
		int result = XML_SUCCESS;
		unsigned int value = 0;

		result = element->QueryUnsignedAttribute( name.c_str(), &value );
		if ( result == XML_SUCCESS )
		{
			outValue = (T)value;
		}
		return result;
	}
	template<typename T> int QueryIntAttribute( XMLElement * element, const std::string& name,  T & outValue, int default_value )
	{
		int result = XML_SUCCESS;
		int value = default_value;

		result = element->QueryIntAttribute( name.c_str(), &value );
		if ( result == XML_SUCCESS )
		{
			outValue = (T)value;
		}
		return result;
	}

	template int QueryUnsignedAttribute( XMLElement * element, const std::string& name, uint32_t & outValue );
	template int QueryUnsignedAttribute( XMLElement * element, const std::string& name, uint16_t & outValue );
	template int QueryIntAttribute( XMLElement * element, const std::string& name, int32_t & outValue, int default_value );
	template int QueryIntAttribute( XMLElement * element, const std::string& name, int16_t & outValue, int default_value );
}
