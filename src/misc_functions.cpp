/****************************
Copyright © 2006-2015 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "lux_types.h"
#include "tinyxml/tinyxml2ext.h"
#include "mokoi_game.h"
#include "core.h"

bool is_whitespace(char ch)
{
	return (ch == '\v' || ch == '\r' || ch == '\n');
}

bool Lux_Util_RectCollide(LuxRect a, LuxRect b)
{
	if(b.x + b.w < a.x)	return 0;
	if(b.x > a.x + a.w)	return 0;
	if(b.y + b.h < a.y)	return 0;
	if(b.y > a.y + a.h)	return 0;

	return 1;
}

bool Lux_Util_PointCollide(LuxRect a, int32_t x, int32_t y)
{
	if(x < a.x)	return 0;
	if(x > a.x + a.w)	return 0;
	if(y > a.y + a.h)	return 0;
	if(y < a.y)	return 0;

	return 1;
}

/* Lux_Util_SetRectFromText
 * Update Width and Height of a LuxRect base on a string of text
 @ area:
 @ text:
 @ text_width:
 @ text_height:
 */
void Lux_Util_SetRectFromText( LuxRect & area, std::string & text, uint8_t text_width, uint8_t text_height, uint16_t wrap_length )
{
	uint16_t max_length = 1;
	uint16_t length_count = 0;
	uint16_t lines = 1;
	std::string::iterator object;

	for ( object = text.begin(); object != text.end(); object++ )
	{
		bool watch_for_color = false;
		uint8_t utfchar = *object;
		uint32_t cchar = utfchar;
		if ( cchar == '\n' || cchar == '\r' )
		{
			lines++;
			max_length = std::max(length_count, max_length);
			length_count = 0;
		}
		else if ( watch_for_color )
		{
			object++;
			watch_for_color = false;
		}
		else if ( cchar == 0xA7 )
		{
			object++;
			watch_for_color = true;
		}
		else if ( cchar <= 128 )
		{
			length_count++;
		}
		else if ( cchar < 224 )
		{
			object++;

			length_count++;
		}
		else if ( cchar < 240 )
		{
			object++;
			object++;

			length_count++;
		}
		else if ( cchar < 245 )
		{
			object++;
			object++;
			object++;

			length_count++;
		}

		if (wrap_length == length_count + 2)
		{
			text.insert(object, '\n');
		}

	}
	max_length = std::max(length_count, max_length);

	area.w = text_width * max_length;
	area.h = text_height * lines;

}

/* Lux_Util_CheckTextColour
 * Checks to see if character determines the text colour
 @ cchar:
 @ font_color:
 @ watch_for_color:
 */
bool Lux_Util_CheckTextColour( uint32_t cchar, LuxColour & font_color, bool & watch_for_color )
{
	bool is_colour_char = false; //True = don't draw

	if ( cchar == 0xA7 )
	{
		watch_for_color = true;
		is_colour_char = true;
	}
	else if ( watch_for_color == true )
	{
		watch_for_color = false;
		is_colour_char = true;

		switch ( cchar )
		{
			case 0xA7: // §
			{
				is_colour_char = false;
				break;
			}

			case '0': // Black
			{
				font_color.r = font_color.g = font_color.b = 0;
				break;
			}
			case '1': // Dark Blue
			{
				font_color.r = font_color.g = 0;
				font_color.b = 170;
				break;
			}
			case '2': // Dark Green
			{
				font_color.r = font_color.b = 0;
				font_color.g = 170;
				break;
			}
			case '3': // Dark Aqua
			{
				font_color.r = 0;
				font_color.g = font_color.b = 170;
				break;
			}
			case '4': // Dark Red
			{
				font_color.g = font_color.b = 0;
				font_color.r = 170;
				break;
			}
			case '5': // Dark Purple
			{
				font_color.g = 0;
				font_color.r = font_color.b = 170;
				break;
			}
			case '6': // Gold
			{
				font_color.b = 0;
				font_color.r = 255;
				font_color.g = 170;
				break;
			}
			case '7': // Gray
			{
				font_color.r = font_color.g = font_color.b = 170;
				break;
			}
			case '8': // Dark Gray
			{
				font_color.r = font_color.g = font_color.b = 85;
				break;
			}
			case '9': // Blue
			{
				font_color.r = font_color.g = 85;
				font_color.b = 255;
				break;
			}
			case 'a': // Green
			{
				font_color.r = font_color.b = 85;
				font_color.g = 255;
				break;
			}
			case 'b': // Aqua
			{
				font_color.r = 85;
				font_color.g = font_color.b = 255;
				break;
			}
			case 'c': // Red
			{
				font_color.b = font_color.g = 85;
				font_color.r = 255;
				break;
			}
			case 'd': // Light Purple
			{
				font_color.g = 85;
				font_color.r = font_color.b = 255;
				break;
			}
			case 'e': // Yellow
			{
				font_color.b = 85;
				font_color.r = font_color.g = 255;
				break;
			}
			default: // White
				font_color.r = font_color.g = font_color.b = 255;
				break;
		}


	}

	return is_colour_char;

}



/*
Special unicode
Buttons
U+2776	❶
U+2777

U+2778	?
U+2779	?
U+277A	?
U+277B	?
U+277C	?
U+277D	?
U+277E	?
U+277F	?
U+2780	?
U+2781	?
U+2782	?
U+2783	?
U+2784	?
U+2785	➅

axis 1
U+25B2	▲	y-
U+25B6	▶	x+
U+25BC	?	y+
U+25C0	?	x+
U+25D0	?	z-
U+25D1	?	z+

axis 2
U+25B3	△
U+25B7	?
U+25BD	?
U+25C1	?
U+25D2	?
U+25D3	◓

pointer
U+24CD Ⓧ
U+24CE Ⓨ
*/

void UnicodeToInput( int32_t cchar, int8_t * axis, int8_t * button, int8_t * pointer )
{
	if ( cchar >= 0x2776 && cchar <= 0x27BF ) // Button
	{
		*button = cchar - 0x2776;
	}
	else
	{
		*button = -1;
	}

	*axis = -1;
	if ( cchar >= 0x25B2 && cchar <= 0x25D3 )
	{
		if ( cchar == 0x25C0 ) // U+25C0	◀	x-
			*axis = 0;
		else if ( cchar == 0x25B6 ) // U+25B6	▶	x+
			*axis = 1;
		else if ( cchar == 0x25B2 ) // U+25B2	▲	y-
			*axis = 2;
		else if ( cchar == 0x25BC ) // U+25BC	▼	y+
			*axis = 3;
		else if ( cchar == 0x25D0 ) // U+25D0	◐	z-
			*axis = 4;
		else if ( cchar == 0x25D1 ) // U+25D1	◑	z+
			*axis = 5;
		else if ( cchar == 0x25C1 ) // U+25C1	◀	x-
			*axis = 6;
		else if ( cchar == 0x25B7 ) // U+25B7	▶	x+
			*axis = 7;
		else if ( cchar == 0x25B3 ) // U+25B3	▲	y-
			*axis = 8;
		else if ( cchar == 0x25BD ) // U+25BD	▼	y+
			*axis = 9;
		else if ( cchar == 0x25D2 ) // U+25D2	◐	z-
			*axis = 10;
		else if ( cchar == 0x25D3 ) // U+25D3	◓	z+
			*axis = 11;

	}

	if ( cchar >= 0x24CD && cchar <= 0x24CE )
	{
		*pointer = cchar - 0x24CD;
	}
	else
	{
		*pointer = -1;
	}

}

tinyxml2::XMLDocument * MokoiGame_GetXML( std::string file )
{
	tinyxml2::XMLDocument * xml_doc = new tinyxml2::XMLDocument;
	uint8_t * data = NULL;
	if ( lux::game_data->GetFile(file, &data, true) )
	{
		xml_doc->Parse( (char *)data );

	}
	delete[] data;
	return xml_doc;
}
