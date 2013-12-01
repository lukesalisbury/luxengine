/* NDS Display function setup */
if ( !is_display_setup )
{
	this->InitGraphics = &Lux_NDS_Init;
	if ( this->InitGraphics(this->screen_dimension.w, this->screen_dimension.h, bpp, false ) )
	{
		this->DestoryGraphics = &Lux_NDS_Destory;
		this->PNGtoSprite = &Lux_NDS_PNGtoSprite;
		this->DrawSprite = &Lux_NDS_DrawSprite;
		this->DrawRect = &Lux_NDS_DrawRect;
		this->DrawCircle = &Lux_NDS_DrawCircle;
		this->DrawPolygon = &Lux_NDS_DrawPolygon;
		this->DrawLine = &Lux_NDS_DrawLine;
		this->DrawText = &Lux_NDS_DrawText;
		this->UpdateRect = &Lux_NDS_Update;
		this->Show = &Lux_NDS_Show;
		this->TextSprites = &Lux_NDS_TextSprites;

		this->LoadSpriteSheet = &Lux_NDS_LoadSpriteSheet;
		this->FreeSpriteSheet = &Lux_NDS_FreeSpriteSheet;
		this->SetBackground = &Lux_NDS_BackgroundObject;
		this->Display2Screen = &Lux_NDS_Display2Screen;
/*
		this->CacheSprite = &Lux_NATIVE_CacheSprite;
		this->FreeSprite = &Lux_NATIVE_FreeSprite;
*/
		is_display_setup = true;
	}
}