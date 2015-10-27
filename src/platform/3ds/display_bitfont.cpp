#include "display_bitfont.h"
#include "bitfont.h"
#include "core.h"

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
		this->textures[c] = sf2d_create_texture( 8, 8, TEXFMT_RGBA8, SF2D_PLACE_RAM );
		memcpy(this->textures[c]->data, charflip, 256);
		sf2d_texture_tile32(this->textures[c]);
		font_point += 8;

		delete charflip;
	}

}


/**
 * @brief DisplayFont::~DisplayFont
 */
DisplayBitFont::~DisplayBitFont()
{
	for ( uint8_t c = 0; c < 130; c++)
	{
		sf2d_free_texture(this->textures[c]);
	}
}

/**
 * @brief DisplayFont::GetTexture
 * @param character
 * @return
 */
sf2d_texture *DisplayBitFont::GetTexture(uint32_t character)
{

	sf2d_texture * texture = NULL;
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
