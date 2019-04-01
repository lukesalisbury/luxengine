
/**
 * @brief Lux_NATIVE_LoadSpriteSheet
 * @param name
 * @param children
 * @return
 */
bool Lux_NATIVE_LoadSpriteSheet( std::string name, std::map<uint32_t, LuxSprite *> * children )
{
	/* PNG Image */
	uint8_t * data = nullptr;
	uint32_t size;
	elix::Image * png = new elix::Image;
	if ( lux::game_data )
	{
		size = lux::game_data->GetFile("./sprites/" + name, &data, false);
		if ( size )
		{
			png->LoadFile(data, size);
		}
	}
	delete data;
	/*  ^ PNG Image */

	if ( png->HasContent() )
	{
		std::map<uint32_t, LuxSprite *>::iterator p = children->begin();
		for( ; p != children->end(); p++ )
		{
			if ( !p->second->animated )
			{
				Lux_NATIVE_CreateSprite( p->second, p->second->sheet_area, png );
			}
		}
		delete png;
		return true;
	}
	else
	{
		return false;
	}
}

/**
 * @brief Lux_NATIVE_LoadSpriteSheetImage
 * @param image
 * @param children
 * @return
 */
bool Lux_NATIVE_LoadSpriteSheetImage(  elix::Image * image, std::map<uint32_t, LuxSprite *> * children )
{
	if ( !image  )
	{
		return false;
	}

	if (  !children )
	{
		return false;
	}


	if ( image->HasContent() )
	{
		std::map<uint32_t, LuxSprite *>::iterator p = children->begin();
		for( ; p != children->end(); p++ )
		{
			if ( !p->second->animated )
			{
				Lux_NATIVE_CreateSprite( p->second, p->second->sheet_area, image );
			}
		}
		return true;
	}
	else
	{
		return false;
	}
}

/**
 * @brief Lux_NATIVE_RefreshSpriteSheet
 * @param name
 * @param children
 * @return
 */
bool Lux_NATIVE_RefreshSpriteSheet( std::string name, std::map<uint32_t, LuxSprite *> * children )
{
	if ( Lux_NATIVE_FreeSpriteSheet( children ) )
		Lux_NATIVE_LoadSpriteSheet( name, children );
	return true;
}

/**
 * @brief Lux_NATIVE_FreeSpriteSheet
 * @param children
 * @return
 */
bool Lux_NATIVE_FreeSpriteSheet( std::map<uint32_t, LuxSprite *> * children )
{
	std::map<uint32_t, LuxSprite *>::iterator p;
	for( p = children->begin(); p != children->end(); p++ )
	{
		if ( !p->second->animated )
		{
			Lux_NATIVE_FreeSprite( p->second );
		}
	}
	return true;
}

/**
 * @brief Lux_NATIVE_PNGtoSprite
 * @param data
 * @param size
 * @return
 */
LuxSprite * Lux_NATIVE_PNGtoSprite( uint8_t * data, uint32_t size )
{
	LuxSprite * sprite = nullptr;
	elix::Image * png = new elix::Image(data, size);

	if ( png->HasContent() )
	{
		sprite = new LuxSprite( GraphicsNative );
		sprite->sheet_area.x = sprite->sheet_area.y = 0;
		sprite->sheet_area.w = png->Width();
		sprite->sheet_area.h = png->Height();

		Lux_NATIVE_CreateSprite( sprite, sprite->sheet_area, png );
	}
	delete png;
	return sprite;
}
