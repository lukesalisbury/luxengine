/****************************
Copyright © 2006-2011 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/
#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <sstream>
#include <map>
#include "lux_types.h"
#include "display_types.h"
#include "layers.h"
#include "lux_sprite.h"
#include "sprite_sheet.h"
#include "core.h"

typedef std::map<std::string, LuxSheet *>::iterator LuxSheetIter;
typedef std::map<uint32_t, LuxSprite *>::iterator LuxSpriteIter;


class DisplaySystem
{
public:
	DisplaySystem();
	DisplaySystem( uint16_t width, uint16_t height, uint8_t bpp, bool fullscreen );
	~DisplaySystem();

private:
	uint8_t bpp;
	bool fullscreen;
	MapObject background_object;
	bool cache_sprites;

	/* Layers */
	std::vector<Layer *> _layers;
	Layer * overlay_layer;

	/* Resource Management */
	bool sprite_quick_access;
	std::map<std::string, LuxSheet *> _sheets;
	std::map<uint32_t, LuxSprite *> _sprites;

public:
	bool Init();
	void Loop(LuxState engine_state);
	void DrawGameStatic();
	bool Close();

	GraphicSystem graphics;

	/* Settings and Misc Variable */
	std::stringstream debug_msg;
	std::string sprite_font;
	bool show_debug;
	bool show_mask;
	bool show_collisions;
	bool show_spriteinfo;
	bool show_3d;
	bool show_cursor;
	bool show_layers[7];
	bool SetCursor( bool able );
	void InitialSetup();
	void DrawCursor();
	void SetRectFromText(LuxRect & area, std::string text, uint8_t text_width, uint8_t text_height );
	void ShowMessages();

	void SetTextFont( bool enable, std::string font_sheet );

	/* Resource Management */
	void ReloadSheets();
	void LoadSheet( LuxSheet * sheet );
	void LoadSheet( std::string name, bool preload = false );
	void RefSheet( std::string name );
	void UnrefSheet( std::string name );
	void UnloadSheet( std::string name );

	LuxSheet * GetSheet( std::string name );
	LuxSprite * GetSprite( std::string parent, std::string name );
	LuxSprite * GetSprite( std::string parent, uint32_t name_hash );
	LuxSprite * GetSprite(std::string sprite_name );
	uint32_t isAnimation( std::string parent, std::string name );

	LuxSprite * GetInputSprite(uint32_t player_id, int8_t axis, int8_t key, int8_t pointer );

	/* Frame Management */
	uint16_t frame_skip;

	/* Screen */
	LuxRect screen_dimension;
	LuxRect display_dimension;

	/* Layer System */
	bool AddObjectToLayer( uint32_t layer, MapObject * new_object, bool static_objects );
	bool AddObjects( std::list<MapObject*> * objects );
	bool RemoveObject( uint32_t layer, MapObject * new_object );
	bool ClearLayer( uint32_t layer, bool static_objects );
	bool ClearLayers( bool skip_static );
	void CacheLayers( bool able );
	void ResortLayer( uint32_t layer );
	void SetLayerColour( uint32_t layer, LuxColour colour );
	void DisplayOverlay();

	/* Map Objects */
	void DrawMapObjectBorder(uint8_t position, SpriteBorder sprite_border, LuxRect object, ObjectEffect new_effects );
	void DrawMapObject( MapObject * object, LuxRect new_position, ObjectEffect new_effects );

	/* Camera Functions */
	void SetCameraView( fixed x, fixed y );
	void SetCameraOffset( fixed x, fixed y );
	void SetCameraFocus( fixed x, fixed z );
	void SetCameraPosition( fixed x, fixed z );
	void ChangeLayerRotation( int16_t roll, int16_t pitch, int16_t yaw );

	void SetCameraView( uint32_t layer, fixed x, fixed y );
	void SetCameraOffset( uint32_t layer, fixed x, fixed y );
	void SetCameraFocus( uint32_t layer, fixed x, fixed z );
	void SetCameraPosition( uint32_t layer, fixed x, fixed z );
	void ChangeLayerRotation( uint32_t layer, int16_t roll, int16_t pitch, int16_t yaw );



	void SetBackgroundColour( LuxColour fillcolor );
	void SetBackgroundEffect( ObjectEffect effect );
	void SetBackgroundObject( MapObject background );
	void ResetBackgroundObject( );
};



namespace lux {
	extern DisplaySystem * display;
}

#endif
