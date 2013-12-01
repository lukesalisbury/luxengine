if ( !is_display_setup )
{
	this->InitGraphics = &Lux_NATIVE_Init;
	if ( this->InitGraphics(this->screen_dimension.w, this->screen_dimension.h, bpp, false ) )
	{
		this->DestoryGraphics = &Lux_NATIVE_Destory;
		this->UpdateRect = &Lux_NATIVE_UpdateRect;
		this->Show = &Lux_NATIVE_Show;
		this->SetBackground = &Lux_NATIVE_BackgroundObject;

		this->DrawSprite = &Lux_GLES_DrawSprite;
		this->DrawRect = &Lux_GLES_DrawRect;
		this->DrawLine = &Lux_GLES_DrawLine;
		this->DrawText = &Lux_GLES_DrawText;
		this->DrawCircle = &Lux_GLES_DrawCircle;
		this->DrawPolygon = &Lux_GLES_DrawPolygon;

		this->LoadSpriteSheet = &Lux_NATIVE_LoadSpriteSheet;
		this->FreeSpriteSheet = &Lux_NATIVE_FreeSpriteSheet;
		this->PNGtoSprite = &Lux_NATIVE_PNGtoSprite;
		is_display_setup = true;
	}
}
