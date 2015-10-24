/****************************
Modified LodePNG version 20111210

Copyright © 2005-2011 Lode Vandevenne
Copyright © 2011-2012 Luke Salisbury

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#define STBI_NO_STDIO 1
#define STBI_NO_HDR 1

#include <iostream>
#include "elix/elix_png.hpp"
#include "elix/elix_file.hpp"
#include "stb_image.c"

#define STB_IMAGE_WRITE_IMPLEMENTATION 1
#include "stb_image_write.h"

/* Little Endian  ARRRRRGGGGGBBBBB */
//#define ARGB16(a, r, g, b) (  0x8000 | (r)| ((b)<<5)|((g)<<10))
#define TRUNCATE(x)  ((x) >> 3)
#define ARGB16(a,r,g,b) ((a << 15) | (TRUNCATE(r) << 10) | (TRUNCATE(g) << 5) | TRUNCATE(b))
/* Big Endian  ARRRRRGGGGGBBBBB*/
//#define ARGB16(a, r, g, b) ( ((a) << 15) | (r)|((g)<<5)|((b)<<10))


/* Little Endian  AABBGGRR*/
#define RGBA32(r, g, b, a) ( (r) | (a << 24) | ((b)<<16)|((g)<<8) )
/* Big Endian  RRGGBBAA*/
//#define RGBA32(r, g, b, a) ( (a) | (r << 24) | ((g)<<16)|((b)<<8) )

namespace elix {
	Image::~Image()
	{
		stbi_image_free(this->pixels);
	}

	bool Image::LoadFile( elix::File * png_file )
	{
		uint8_t * png_data = NULL;
		uint32_t png_file_size = 0;
		bool results = false;

		if ( png_file->Exist() )
		{
			png_file_size = png_file->Length();
			if ( png_file_size )
			{
				png_data = new uint8_t[png_file_size];
				png_file->ReadAll( (void**)&png_data );
				results = this->LoadFile( png_data,  png_file_size );
				delete[] png_data;
			}
		}
		return results;
	}
	bool Image::LoadFile( uint8_t * data, int32_t size )
	{
		this->bpp = 4;
		stbi_convert_iphone_png_to_rgb(1);
		this->pixels = stbi_load_from_memory(data, (int)size, (int*)&this->width, (int*)&this->height, (int*)&this->original_bpp, this->bpp);
		if ( this->pixels == NULL )
		{
			std::cout << "elix::Image:" << stbi_failure_reason() << std::endl;
			this->length = 0;
			return false;
		}
		if ( this->bpp != 4 )
		{
			std::cout << "this->bpp:" << this->bpp << "-" << stbi_failure_reason() << std::endl;
		}
		this->length = this->width * this->height * this->bpp;
		return true;
	}

	char* Image::getError()
	{
		return (char*)stbi_failure_reason();
	}


	void Image::SaveFile(const char *filename )
	{
		stbi_write_png(filename, this->width, this->height, 4, this->pixels, this->width * this->bpp);
	}


	uint8_t * Image::GetPixelPointer( int32_t x, int32_t y )
	{
		if ( x > this->width )
			return NULL;
		if ( y > this->height )
			return NULL;
		int32_t p = (x + (y * this->width))*this->bpp;
		if (p > length)
			return NULL;

		uint8_t * pix = this->pixels;
		pix += p;
		return pix;
	}

	uint16_t Image::GetPixel16( int32_t x, int32_t y )
	{
		if ( this->pixels )
		{
			uint8_t * pix = this->GetPixelPointer( x, y );
			if ( pix )
				return (uint16_t)ARGB16(pix[3]/128, pix[0], pix[1], pix[2]);
		}
		return 0;
	}
	uint32_t Image::GetPixel( int32_t x, int32_t y )
	{
		if ( this->pixels )
		{
			uint8_t * pix = this->GetPixelPointer( x, y );
			if (pix)
			{
				return (uint32_t)RGBA32(pix[0], pix[1], pix[2], pix[3]);
			}
		}
		return 0;
	}

	bool Image::GetPixel( int32_t x, int32_t y, uint8_t &r, uint8_t &g, uint8_t &b, uint8_t &a )
	{
		if ( this->pixels )
		{
			uint8_t * pix = this->GetPixelPointer( x, y );
			if ( pix )
			{
				r = pix[0];
				g = pix[1];
				b = pix[2];
				a = pix[3];
				return 1;
			}
		}
		return 0;
	}

	bool Image::HasContent()
	{
		return (this->pixels != NULL);
	}

	void Image::ConvertToPOT( int32_t w, int32_t h)
	{
		if ( w < this->width || h < this->height )
		{
			return;
		}

		uint32_t size = w * h * 4;
		uint8_t * new_data = (uint8_t *)malloc(size);
		uint8_t * p = new_data;
		uint8_t * s = this->pixels;
		for ( int32_t qy = 0; qy < this->height; qy++ )
		{
			//( void * destination, const void * source, size_t num );
			memcpy( p, s, this->width * 4);
			p += (w*4);
			s += (this->width * 4);
		}

		stbi_image_free(this->pixels);
		this->pixels = new_data;
		this->width = w;
		this->height = h;

	}


	void Image::ConvertTo16BPP()
	{
		int32_t size = this->width * this->height * 2;
		if ( size == this->length/2 )
		{
			uint8_t * new_data = (uint8_t *)malloc(size);
			uint16_t *p = (uint16_t *)new_data;
			for ( int32_t qy = 0; qy < this->height; qy++ )
			{
				for ( int32_t qx = 0; qx < this->width; qx++ )
				{
					*p = GetPixel16(qx, qy);
					p++;
				}
			}
			stbi_image_free(this->pixels);
			this->pixels = new_data;
			this->bpp = 2;
		}

	}


}

