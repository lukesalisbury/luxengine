/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include <sstream>
#include "css.h"
#include "core.h"
#include "elix_string.hpp"
#include "display_functions.h"
/*
element:state
	|  key:value;
*/

CSSParser::CSSParser( std::string content )
{
	this->AddContent(content);


//	for ( std::vector<CSSElement*>::iterator it = this->elements.begin(); it != this->elements.end(); it++ )
//	{
//		lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << (*it)->element << ":" << (*it)->state << "" << std::endl;
//		for ( std::map<std::string, std::string>::iterator key = (*it)->key.begin(); key != (*it)->key.end(); key++ )
//			lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << "\t - '" << (*key).first << "' : '" << (*key).second << "'" << std::endl;
//	}

}

CSSParser::~CSSParser() {}

void CSSParser::AddContent( std::string content )
{
	std::vector<std::string> first_split;
	std::vector<std::string> element_and_content;
	std::vector<std::string> temp_key_array;
	std::vector<std::string> multi_elements;
	std::map<std::string, std::string> keys_scan;

	elix::string::Split( content, "}", &first_split );

	std::string elements, element_contents, key_line;
	LuxWidget new_element;
	_WidgetStates element_state;

	if ( first_split.size()  )
	{
		for( uint32_t i = 0; i < first_split.size(); i++ )
		{
			if ( first_split.at(i).length() > 10 ) // Ignore empty strings
			{
				elix::string::Split( first_split.at(i), "{", &element_and_content );
				if ( element_and_content.size() >= 2 )
				{
					elements = element_and_content.at(0);

					element_contents = element_and_content.at(1);
					TrimString(&elements);
					TrimString(&element_contents);

					elix::string::Split( element_contents, ";", &temp_key_array );
					if ( temp_key_array.size()  )
					{
						keys_scan.clear();
						for( uint32_t q = 0; q < temp_key_array.size(); q++ )
							KeyValue( temp_key_array.at(q), ':', &keys_scan );
					}

					// TODO elements.find(':', 0) and elements.find(',', 0)
					if ( elements.find(',', 0) != std::string::npos )
					{
						multi_elements.clear();
						elix::string::Split( elements, ",", &multi_elements );
						for ( std::vector<std::string>::iterator it2 = multi_elements.begin(); it2 != multi_elements.end(); it2++ )
						{
							this->ParseElementAndType(new_element, element_state, (*it2));
							CSSElement * temp_element = this->FindElement(new_element, element_state, true);
							for ( std::map<std::string, std::string>::iterator key = keys_scan.begin(); key != keys_scan.end(); key++ )
								temp_element->key[(*key).first] = (*key).second;
						}
					}
					else
					{
						this->ParseElementAndType(new_element, element_state, elements);
						CSSElement * temp_element = this->FindElement(new_element, element_state, true);
						for ( std::map<std::string, std::string>::iterator key = keys_scan.begin(); key != keys_scan.end(); key++ )
							temp_element->key[(*key).first] = (*key).second;
//						temp_element->key.insert( keys_scan.begin(), keys_scan.end() );
					}

					temp_key_array.clear();
				}

				element_and_content.clear();
			}
		}
	}

/*
	for ( std::vector<CSSElement*>::iterator it = this->elements.begin(); it != this->elements.end(); it++ )
	{
		lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << (*it)->element << ":" << (*it)->state << "" << std::endl;
		for ( std::map<std::string, std::string>::iterator key = (*it)->key.begin(); key != (*it)->key.end(); key++ )
			lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << "\t - '" << (*key).first << "' : '" << (*key).second << "'" << std::endl;
	}
*/
}


CSSElement * CSSParser::FindElement( LuxWidget element, _WidgetStates state, bool create )
{
	if ( this->elements.size() )
	{
		std::vector<CSSElement*>::iterator iter = this->elements.begin();
		while( iter != this->elements.end() )
		{
			if ( element == (*iter)->element && state == (*iter)->state )
			{
				return (*iter);
			}
			iter++;
		}
	}
	if (create)
	{
		CSSElement * temp_element = new CSSElement(element, state);
		this->elements.push_back(temp_element);
		return temp_element;
	}
	else
		return NULL;
}
extern ObjectEffect default_fx;

ObjectEffect CSSParser::GetEffect( LuxWidget element, _WidgetStates state )
{
	CSSElement * css = this->FindElement(element, state);
	ObjectEffect new_effect = default_fx;
	if ( css != NULL )
	{
		for ( std::map<std::string, std::string>::iterator keys = css->key.begin(); keys != css->key.end(); keys++ )
		{
			if ( !(*keys).first.compare("background-color") )
			{
				new_effect.SetColour( (*keys).second );
			}
			else if ( !(*keys).first.compare("background-color-secondary") )
			{
				new_effect.SetSecondaryColour( (*keys).second );
			}
			else if ( !(*keys).first.compare("background-style") )
			{
				if ( !(*keys).second.compare("vertical-gradient") )
					new_effect.style = STYLE_VGRADIENT;
				else if ( !(*keys).second.compare("horizontal-gradient") )
					new_effect.style = STYLE_HGRADIENT;
			}
		}
	}

	return new_effect;
}

LuxColour CSSParser::GetColour( LuxWidget element, _WidgetStates state, std::string key )
{
	CSSElement * css = this->FindElement(element, state);
	if ( css != NULL )
	{
		for ( std::map<std::string, std::string>::iterator keys = css->key.begin(); keys != css->key.end(); keys++ )
		{
			if ( !(*keys).first.compare(key) )
			{
				return Lux_Hex2Colour( (*keys).second );
			}
		}
	}
	return (LuxColour){255,255,255,255};
}

uint8_t CSSParser::GetAlign( LuxWidget element, _WidgetStates state, std::string key )
{
	CSSElement * css = this->FindElement(element, state);
	if ( css )
	{
		for ( std::map<std::string, std::string>::iterator keys = css->key.begin(); keys != css->key.end(); keys++ )
		{
			if ( !(*keys).first.compare(key) )
			{

				/*	0 left
					1 right
					2 center
				*/
				uint8_t value = (*keys).second.at(0);
				value = ( value == 'r' ? 1 : ( value == 'c' ? 2 : 0));

				return value;
			}
		}

	}
	return 0;
}
uint16_t CSSParser::GetSize( LuxWidget element, _WidgetStates state, std::string key )
{
	CSSElement * css = this->FindElement(element, state);
	if ( css )
	{
		for ( std::map<std::string, std::string>::iterator keys = css->key.begin(); keys != css->key.end(); keys++ )
		{
			if ( !(*keys).first.compare(key) )
			{
				uint16_t value = 0;
				if ( !key.compare("border-style") )
				{
					/*	0 solid
						1 inset
						2 outset
					*/
					value = (*keys).second.at(0);
					value = ( value == 105 ? 1 : ( value == 111 ? 2 : 0));
				}
				else
				{
					std::stringstream stream((*keys).second);
					stream >> value;
				}
				return value;
			}
		}

	}
	return 0;
}

std::string CSSParser::GetString( LuxWidget element, _WidgetStates state, std::string key )
{
	CSSElement * css = this->FindElement(element, state);
	if ( css )
	{
		for ( std::map<std::string, std::string>::iterator keys = css->key.begin(); keys != css->key.end(); keys++ )
		{
			if ( !(*keys).first.compare(key) )
			{
				return (*keys).second;
			}
		}
	}
	return "";
}

DisplayStyles CSSParser::GetStyle( LuxWidget element, _WidgetStates state, std::string key )
{
	CSSElement * css = this->FindElement(element, state);
	if ( css )
	{
		for ( std::map<std::string, std::string>::iterator keys = css->key.begin(); keys != css->key.end(); keys++ )
		{
			if ( !(*keys).first.compare(key) )
			{
				if ( !(*keys).second.compare("vertical-gradient") )
					return STYLE_VGRADIENT;
				else if ( !(*keys).second.compare("horizontal-gradient") )
					return STYLE_HGRADIENT;
				else if ( !(*keys).second.compare("glow") )
					return STYLE_GLOW;
				else if ( !(*keys).second.compare("outline") )
					return STYLE_OUTLINE;
				else if ( !(*keys).second.compare("replace-colour") )
					return STYLE_REPCOLOUR;
				else if ( !(*keys).second.compare("negative") )
					return STYLE_NEGATIVE;
				else
					return STYLE_NORMAL;
			}
		}
	}
	return STYLE_NORMAL;
}

bool CSSParser::HasKey( LuxWidget element, _WidgetStates state, std::string key )
{
	CSSElement * css = this->FindElement(element, state);
	if ( css )
	{
		for ( std::map<std::string, std::string>::iterator keys = css->key.begin(); keys != css->key.end(); keys++ )
		{
			if ( !(*keys).first.compare(key) )
			{
				return 1;
			}
		}
	}

	return 0;
}

void CSSParser::ParseElementAndType( LuxWidget & element, _WidgetStates & state, std::string source )
{
	std::string::size_type position = source.find(':', 0);
	element = EMPTYWINDOW;
	if ( position == std::string::npos )
		state = ENABLED;
	else
	{
		std::string sstate = source.substr(position+1);
		this->TrimString(&sstate);
		if ( !sstate.compare("enabled") )
			state = ENABLED;
		else if ( !sstate.compare("pressed") )
			state = PRESSED;
		else if ( !sstate.compare("hover") )
			state = HOVER;
		else if ( !sstate.compare("clicked") )
			state = CLICKED;
		else if ( !sstate.compare("active") )
			state = ACTIVE;
		else if ( !sstate.compare("disabled") )
			state = DISABLED;
		source = source.substr(0, position);
	}
	this->TrimString(&source);

	if ( !source.compare("guibackground") )
		element = GUIBACKGROUND;
	else if ( !source.compare("emptywindow") )
		element = EMPTYWINDOW;
	else if ( !source.compare("dialog") )
		element = DIALOG;
	else if ( !source.compare("throbber") )
		element = THROBBER;
	else if ( !source.compare("checkbox") )
		element = CHECKBOX;
	else if ( !source.compare("text") )
		element = TEXT;
	else if ( !source.compare("inputtext") )
		element = INPUTTEXT;
	else if ( !source.compare("dialogtext") )
		element = DIALOGTEXT;
	else if ( !source.compare("button") )
		element = BUTTON;
	else if ( !source.compare("imagebutton") )
		element = IMAGEBUTTON;
	else if ( !source.compare("box") )
		element = BOX;

}

void CSSParser::TrimString( std::string * source )
{
	if ( source->size() )
	{
		source->erase(source->find_last_not_of("\r\n \t")+1);
		source->erase(0, source->find_first_not_of("\r\n \t"));
	}
}

void CSSParser::KeyValue( std::string str, char delim, std::map<std::string, std::string> * results )
{
	if ( str.size() )
	{
		std::string key;
		std::string value;
		std::string::size_type lastPos = str.find_first_not_of(delim, 0);
		std::string::size_type pos = str.find_first_of(delim, lastPos);
		if ( std::string::npos != pos || std::string::npos != lastPos )
		{
			key = str.substr(lastPos, pos - lastPos);
			lastPos = str.find_first_not_of(delim, pos);
			pos = str.find_first_of(delim, lastPos);
			if ( std::string::npos != pos || std::string::npos != lastPos )
			{
				value = str.substr(lastPos, pos - lastPos) ;
				this->TrimString(&value);
				this->TrimString(&key);
				results->insert( std::pair<std::string,std::string>(key,value) );
			}
		}
	}
}

