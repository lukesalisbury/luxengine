/* OpenGL Display function setup */
if ( !is_display_setup )
{
	this->InitGraphics = &Lux_D3D_Init;
	if ( this->InitGraphics(this->screen_dimension.w, this->screen_dimension.h, bpp, false ) )
	{
		this->DestoryGraphics = &Lux_D3D_Destory;
		this->DrawSprite = &Lux_D3D_DrawSprite;
		this->DrawRect = &Lux_D3D_DrawRect;
		this->DrawCircle = &Lux_D3D_DrawCircle;
		this->DrawPolygon = &Lux_D3D_DrawPolygon;
		this->DrawLine = &Lux_D3D_DrawLine;
		this->DrawText = &Lux_D3D_DrawText;
		this->UpdateRect = &Lux_D3D_Update;
		this->Show = &Lux_D3D_Show;
		this->LoadSpriteSheet = &Lux_D3D_LoadSpriteSheet;
		this->FreeSpriteSheet = &Lux_D3D_FreeSpriteSheet;
		this->SetBackgroundColour = &Lux_D3D_Background;
		is_display_setup = true;
	}
}
