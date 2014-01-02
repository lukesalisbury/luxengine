/****************************
Copyright © 2006-2011 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#include "masks.h"
#include "mokoi_game.h"
#include "core.h"
Lux_Mask * Lux_Mask_New(uint16_t width, uint16_t height)
{
	Lux_Mask * mask = new Lux_Mask;
	mask->width = width;
	mask->height = height;
	mask->length = width * height;
	mask->ref = 0;
	if ( mask->length )
	{
		mask->data = new uint8_t[mask->length];
		memset( mask->data, 0, mask->length ); 
		mask->ref++;
	}
	return mask;
}

bool Lux_Mask_Load(std::string filename, Lux_Mask * mask)
{
	if (mask->ref)
	{
		return false;
	}
	std::stringstream mask_file;
	uint32_t count = 0;

	if ( lux::game->GetStream("./masks/" + filename, &mask_file) )
	{
		mask_file.read((char*)&mask->width, sizeof(uint16_t));
		mask_file.read((char*)&mask->height, sizeof(uint16_t));
		mask->length = mask->width * mask->height;
		lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << "Mask Overflow: " << mask->width << " " << mask->height <<" " << mask->length << std::endl;
		mask->data = new uint8_t[mask->length];
		mask->ref++;
		char ch;
		while( mask_file.get(ch) )
		{
			if (count < mask->length)
			{
				mask->data[count] = (uint8_t)ch;
			}
			else
				lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << "Mask Overflow: " << ch << std::endl;
			count++;
		}
		lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << std::endl;
		mask_file.clear();
	}
	return true;
}

bool Lux_Mask_Free(Lux_Mask * mask)
{
	if ( !mask )
		return false;
	mask->ref--;
	if (!mask->ref)
	{
		delete [] mask->data;
		return true;
	}
	return false;
}

uint16_t Lux_Mask_GetValue(Lux_Mask * mask, uint16_t x, uint16_t y)
{
	uint32_t point = x + (mask->width * y);
	if ( x > mask->width )
	{
		return 256;
	}
	if ( mask->length >= point )
	{
		return (uint16_t)mask->data[point];
	}
	return 256;
}

void Lux_Mask_CopyArea(Lux_Mask * src, Lux_Mask * dest, uint16_t x, uint16_t y)
{
	if (x > dest->width || y > dest->height || !src->ref || !dest->ref)
	{
		return;
	}
	uint32_t source_pos = 0;
	uint32_t dest_pos = (y * dest->width) + (uint32_t)x;
	uint32_t copy_length = dest->width - x;
	copy_length = (copy_length > src->width ? src->width : copy_length);
	memcpy ( dest->data + dest_pos, src->data, copy_length );
	while (source_pos <= src->length )
	{
		memcpy ( dest->data + dest_pos, src->data + source_pos, copy_length );
		dest_pos += dest->width;
		source_pos += src->width;
	}
}

void Lux_Mask_FillArea(Lux_Mask * dest, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t value)
{
	if (x > dest->width || y > dest->height || !dest->ref)
	{
		return;
	}

	uint32_t source_pos = 0;
	uint16_t height_count = 0;
	uint32_t dest_pos = (y * dest->width) + (uint32_t)x;
	uint32_t copy_length = dest->width - x;
	uint32_t height_length = height;

	height_length = (height >= dest->height ? dest->height : height);
	copy_length = (copy_length >= width ? width : copy_length);
	height_length = ( height_length + y > dest->height ? height_length - y : height_length);
	while ( height_count < height_length )
	{
		if ( (dest_pos + copy_length) > dest->length )
		{
			lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR) << "Fill Area is larger then Mask" << std::endl;
			break;
		}
		memset( dest->data + dest_pos, value, copy_length );
		dest_pos += dest->width;
		source_pos += width;
		height_count++;
	}
}
/* Semi-Working Version
void Lux_Mask_FillArea(Lux_Mask * dest, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t value)
{
	if (x > dest->width || y > dest->height || !dest->ref)
	{
		return;
	}

	uint32_t source_pos = 0;
	uint16_t height_count = 0;
	uint32_t dest_pos = (y * dest->width) + (uint32_t)x;
	uint32_t copy_length = dest->width - x;
	uint32_t height_length = height;

	height_length = (height >= dest->height ? dest->height : height);
	copy_length = (copy_length >= width ? width : copy_length);
	height_length = ( height_length + y > dest->height ? height_length - y : height_length);
	while ( height_count < height_length )
	{
		if ( (dest_pos + copy_length) > dest->length )
		{
			lux::core->SystemMessage(SYSTEM_MESSAGE_ERROR) << "Fill Area is larger then Mask" << std::endl;
			break;
		}
		memset( dest->data + dest_pos, value, copy_length );
		dest_pos += dest->width;
		source_pos += width;
		height_count++;
	}
}
*/
