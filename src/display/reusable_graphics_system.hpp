#ifndef REUSABLE_GRAPHICS_SYSTEM_HPP
#define REUSABLE_GRAPHICS_SYSTEM_HPP


LuxSprite * Lux_NATIVE_PNGtoSprite( uint8_t * data, uint32_t size );
bool Lux_NATIVE_LoadSpriteSheet( std::string name, std::map<uint32_t, LuxSprite *> * children );
bool Lux_NATIVE_LoadSpriteSheetImage( elix::Image * image, std::map<uint32_t, LuxSprite *> * child );
bool Lux_NATIVE_RefreshSpriteSheet( std::string name, std::map<uint32_t, LuxSprite *> * children );
bool Lux_NATIVE_FreeSpriteSheet( std::map<uint32_t, LuxSprite *> * children );



#endif // REUSABLE_GRAPHICS_SYSTEM_HPP
