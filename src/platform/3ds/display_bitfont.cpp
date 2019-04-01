#include "display_bitfont.h"
#include "bitfont.h"
#include "core.h"


#include <citro2d.h>
#include <citro3d.h>
/**
 * @brief DisplayFont::DisplayFont
 */
DisplayBitFont::DisplayBitFont( )
{
	uint8_t * font_point = &gfxPrimitivesFontdata[0];
	uint8_t l = 0, q;
	for ( uint8_t c = 0; c < 130; c++)
	{
		uint32_t * charflip = new uint32_t[64];
		//uint16_t * charflip = new uint16_t[64];
		for (l = 0; l < 8; l++)
		{
			for (q = 0; q < 8; q++)
			{
				uint8_t y = (l)*8;
				charflip[y + (7-q)] = !!(font_point[l] & (1 << q)) ? 0xFFFFFFFF : 0x00000000 ;
			}
		}
		C2D_Image i = { nullptr, nullptr };
		C3D_TexInitVRAM(i.tex, 8, 8, GPU_RGBA8);

		C3D_TexUpload(i.tex, charflip);

		C2D_SpriteFromImage(this->textures[c], i);


		font_point += 8;

		delete[] charflip;
	}

}


/**
 * @brief DisplayFont::~DisplayFont
 */
DisplayBitFont::~DisplayBitFont()
{
	for ( uint8_t c = 0; c < 130; c++)
	{
		C3D_TexDelete(&this->textures[c]->tex);
		C2D_Sprite(this->textures[c]);
	}
}

/**
 * @brief DisplayFont::GetTexture
 * @param character
 * @return
 */
C2D_Sprite * DisplayBitFont::GetTexture(uint32_t character)
{

	C2D_Sprite * texture = nullptr;
	if ( character >= 32 && character <= 128 )
	{
		texture = this->textures[character];
	}
	else if ( character  == 0x2026 )//…
	{
		texture = this->textures[129];
	}
	else // �
	{
		texture = this->textures[128];
	}
	return texture;
}
