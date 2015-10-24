#include "display_bitfont.h"
#include "bitfont.h"
#include "core.h"

/**
 * @brief DisplayFont::DisplayFont
 */
DisplayBitFont::DisplayBitFont( )
{
	uint8_t * font_point = &gfxPrimitivesFontdata[0];
	uint8_t i = 0, q;
	for ( uint8_t c = 0; c < 130; c++)
	{
		uint16_t * charflip = new uint16_t[64];
		for (i = 0; i < 8; i++)
		{
			for (q = 0; q < 8; q++)
			{
				charflip[(i*8) + q] =  (!!(font_point[i] & (1 << (8-q))) ? 0xFFFF : 0x000F) ;
			}
		}
		//this->textures[c] = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB1555, SDL_TEXTUREACCESS_STATIC, 8, 8);
		//if ( !this->textures[c] )
		//	lux::core->SystemMessage(SYSTEM_MESSAGE_LOG) << "DisplayFont: " << SDL_GetError() << std::endl;
		//SDL_SetTextureBlendMode( this->textures[c], SDL_BLENDMODE_BLEND);
		//SDL_SetTextureBlendMode( this->textures[c], SDL_BLENDMODE_NONE);

		//SDL_UpdateTexture( this->textures[c], NULL, charflip, 16);

		font_point += 8;
	}

}


/**
 * @brief DisplayFont::~DisplayFont
 */
DisplayBitFont::~DisplayBitFont()
{
	for ( uint8_t c = 0; c < 130; c++)
	{
		//SDL_DestroyTexture(this->textures[c]);
	}
}

/**
 * @brief DisplayFont::GetTexture
 * @param character
 * @return
 */
void *DisplayBitFont::GetTexture(uint32_t character)
{
	/*
	SDL_Texture * texture = NULL;
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
	*/
	return NULL;
}
