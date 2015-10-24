#ifndef _ELIX_PNG_H_
#define _ELIX_PNG_H_

#include "elix/elix_intdef.h"
#include "elix/elix_file.hpp"

namespace elix
{
	class Image
	{
		private:
			int32_t width;
			int32_t height;
			int32_t length;
			int32_t bpp;
			int32_t original_bpp;
			uint8_t * pixels;

			uint8_t * GetPixelPointer( int32_t x, int32_t y );

		public:
			Image() : pixels(NULL) {}
			Image( uint8_t * data, int32_t size )
			{
				this->LoadFile( data, size );
			}
			~Image();
			bool LoadFile( File *png_file );
			bool LoadFile( uint8_t * data, int32_t size );
			void SaveFile( const char * filename );
			uint16_t GetPixel16( int32_t x, int32_t y );
			uint32_t GetPixel( int32_t x, int32_t y );
			bool GetPixel( int32_t x, int32_t y, uint8_t &r, uint8_t &g, uint8_t &b, uint8_t &a );
			//bool GetPixel16( int32_t x, int32_t y, uint8_t &r, uint8_t &g, uint8_t &b, uint8_t &a );

			bool HasContent();
			char* getError();
			int32_t Width() { return width; }
			int32_t Height() { return height; }
			int32_t BytesPerPixel() { return bpp; }


			uint8_t * GetPixels() { return pixels; }
			bool SetPixels( int32_t w, int32_t h, uint8_t * data )
			{
				this->width = w;
				this->height = h;

				this->pixels = data;
				this->bpp = 4;



				return true;
			}



			void ConvertTo16BPP();
			void ConvertToPOT( int32_t w, int32_t h );


	};
}


#endif
