/* NDS Display function setup */
if ( !is_display_setup )
{
	this->InitGraphics = &Lux_NATIVE_Init;
	if ( this->InitGraphics(this->screen_dimension.w, this->screen_dimension.h, bpp, false ) )
	{
		this->DestoryGraphics = &Lux_NATIVE_Destory;
		this->PNGtoSprite = &Lux_NATIVE_PNGtoSprite;
		this->DrawSprite = &Lux_NATIVE_DrawSprite;
		this->DrawRect = &Lux_NATIVE_DrawRect;
		this->DrawCircle = &Lux_NATIVE_DrawCircle;
		this->DrawPolygon = &Lux_NATIVE_DrawPolygon;
		this->DrawLine = &Lux_NATIVE_DrawLine;
		this->DrawText = &Lux_NATIVE_DrawText;
		this->UpdateRect = &Lux_NATIVE_Update;
		this->Show = &Lux_NATIVE_Show;
		this->TextSprites = &Lux_NATIVE_TextSprites;

		this->LoadSpriteSheet = &Lux_NATIVE_LoadSpriteSheet;
		this->FreeSpriteSheet = &Lux_NATIVE_FreeSpriteSheet;
		this->SetBackground = &Lux_NATIVE_BackgroundObject;
		this->Display2Screen = &Lux_NATIVE_Display2Screen;

		is_display_setup = true;
	}
}