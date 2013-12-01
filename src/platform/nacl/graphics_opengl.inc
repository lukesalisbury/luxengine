/* OpenGL Display function setup */
if ( !is_display_setup )
{
	this->InitGraphics = &Lux_OGL_Init;
	if ( this->InitGraphics(this->screen_dimension.w, this->screen_dimension.h, bpp, this->_fullscreen ) )
	{
		this->DestoryGraphics = &Lux_OGL_Destory;
		this->PNGtoSprite = &Lux_OGL_PNGtoSprite;
		this->DrawSprite = &Lux_OGL_DrawSprite;
		this->DrawRect = &Lux_OGL_DrawRect;
		this->DrawCircle = &Lux_OGL_DrawCircle;
		this->DrawPolygon = &Lux_OGL_DrawPolygon;
		this->DrawLine = &Lux_OGL_DrawLine;
		this->DrawText = &Lux_OGL_DrawText;
		this->UpdateRect = &Lux_OGL_Update;
		this->Show = &Lux_OGL_Show;
		this->TextSprites = &Lux_OGL_TextSprites;
		this->SetRotation = &Lux_OGL_SetRotation;
		this->LoadSpriteSheet = &Lux_OGL_LoadSpriteSheet;
		this->FreeSpriteSheet = &Lux_OGL_FreeSpriteSheet;
		this->SetBackground = &Lux_OGL_BackgroundObject;
		this->Display2Screen = &Lux_OGL_Display2Screen;
		this->FreeSprite = &Lux_OGL_FreeSprite;
		is_display_setup = true;
	}
}
