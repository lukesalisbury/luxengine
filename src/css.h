/****************************
Copyright © 2006-2011 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef _CSS_H_
#define _CSS_H_

class CSSParser;

#include <map>
#include "lux_types.h"
#include "widget.h"
#include "object_effect.h"
#include "display_styles.h"
/*
element:state
	|  key:value;
*/
class CSSElement
{
	public:
		LuxWidget element;
		_WidgetStates state;
		std::map<std::string, std::string> key;
		CSSElement( LuxWidget _element, _WidgetStates _state ) { this->element = _element; this->state = _state; }
		~CSSElement() { this->key.clear(); }
};

class CSSParser
{
	public:
		CSSParser( std::string content );
		~CSSParser();
	private:
		std::vector<CSSElement*> elements;
		void TrimString( std::string * source );
		void KeyValue( std::string str, char delim, std::map<std::string, std::string> * results );
		void ParseElementAndType( LuxWidget & element, _WidgetStates & state, std::string source );
		CSSElement * FindElement( LuxWidget element, _WidgetStates state, bool create = false);
	public:
		void AddContent( std::string content );
		bool HasKey( LuxWidget element, _WidgetStates state, std::string key );
		LuxColour GetColour( LuxWidget element, _WidgetStates state, std::string key );
		ObjectEffect GetEffect( LuxWidget element, _WidgetStates state );
		uint16_t GetSize( LuxWidget element, _WidgetStates state, std::string key );
		std::string GetString( LuxWidget element, _WidgetStates state, std::string key );
		DisplayStyles GetStyle( LuxWidget element, _WidgetStates state, std::string key );
};

#endif /* _CSS_H_ */
