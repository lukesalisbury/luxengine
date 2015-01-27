#include <graphics> 
#include <helper>

#define item[.canvas,.child]


new object:effectscanvas;
new object:spritescanvas;
new object:shapescanvas;

new child[8];


//-------------------------------------------------------- 

RandomColour( object:canvas, c )
{
	static timer_colour = 0;
	if ( TimerCountdownWithReset(timer_colour, 1000) )
	{
		new colour = 0;
		BitsSet( colour, random(255), 24, 8); // Alpha
		BitsSet( colour, random(255), 16, 8); // Blue
		BitsSet( colour, random(255), 8, 8); // Green
		BitsSet( colour, random(255), 0, 8); // Red

		CanvasChildSetEffect( canvas, c, colour); 
	}
}

//-------------------------------------------------------- 

StyleChange( object:canvas, c1, c2 )
{
	static timer_style = 0;
	static total_styles = 6;
	static StyleTypes:object_style = STYLE_GLOW;

	CanvasChildSetEffect(canvas, c1, WHITE, .style = object_style, .colour2 = 0x089955FF);
	CanvasChildSetEffect(canvas, c2, WHITE, .style = object_style, .colour2 = 0x089955FF);

	if ( TimerCountdownWithReset(timer_style, 2000) )
	{
		if ( _:object_style >= total_styles)
			object_style = STYLE_NORMAL;
		else
			object_style++;
	}

}

//-------------------------------------------------------- 

ScaleChange( object:canvas, c1 )
{
	static object_scale = 1000;
	static scale_direction = 1;

	CanvasChildSetEffect(canvas, c1, WHITE, .scale_w = object_scale, .scale_h = object_scale );
	object_scale += GameFrame() * scale_direction;

	if ( object_scale < 0500 || object_scale > 3000  )
	{
		scale_direction *= -1;
	}
	object_scale = clamp( object_scale,  0500, 3000 );
}



//-------------------------------------------------------- 

TileChange( object:canvas, c1 )
{
	static object_tile = 32;
	static tile_direction = 1;

	//CanvasChildSetDimension( canvas, c1, object_tile, object_tile);
	object_tile += (GameFrame()/10) * tile_direction;
	if ( object_tile > 200 || object_tile < 32 )
	{
		object_tile = clamp(object_tile, 32, 200);
		tile_direction *= -1;
	}

}

//-------------------------------------------------------- 

RotateChange( object:canvas, c1 )
{
	static object_rotation = 0;

	CanvasChildSetEffect(canvas, c1,  WHITE, object_rotation);
	object_rotation += GameFrame();
	object_rotation %= 360;

}

//--------------------------------------------------------
forward public @SwitchCanvas( Fixed:angle ) ;
public @SwitchCanvas(  Fixed:angle )  
{
	ConsoleOutput("%d", angle);
	if ( angle == 90.0 )
	{
		ObjectFlag(effectscanvas, FLAG_HIDDEN, true )
		ObjectFlag(spritescanvas, FLAG_HIDDEN, false )
		ObjectFlag(shapescanvas, FLAG_HIDDEN, true )
	}
	else if ( angle == 180.0 )
	{
		ObjectFlag(effectscanvas, FLAG_HIDDEN, true )
		ObjectFlag(spritescanvas, FLAG_HIDDEN, true )
		ObjectFlag(shapescanvas, FLAG_HIDDEN, false )
	}
	else 
	{
		ObjectFlag(effectscanvas, FLAG_HIDDEN, false )
		ObjectFlag(spritescanvas, FLAG_HIDDEN, true )
		ObjectFlag(shapescanvas, FLAG_HIDDEN, true )
	}
}


//--------------------------------------------------------

public Init(...)  
{
	new w,h,x,y;
	ObjectInfo( ObjectHash("drawarea", false),  w,h,x,y );

	effectscanvas = ObjectCreate("effects", CANVAS, x, y, 6, 0, 0);  
	spritescanvas = ObjectCreate("sprites", CANVAS, x, y, 6, 0, 0);  
	shapescanvas = ObjectCreate("shapes", CANVAS, x, y, 6, 0, 0);  

	ObjectFlag(spritescanvas, FLAG_HIDDEN, true )
	ObjectFlag(shapescanvas, FLAG_HIDDEN, true )

	child[0] = CanvasChildInfo(effectscanvas, "sprite_tint", w,h,x,y);
	child[1] = CanvasChildInfo(effectscanvas, "rect_effect", w,h,x,y);
	child[2] = CanvasChildInfo(effectscanvas, "sprite_effect", w,h,x,y);
	child[3] = CanvasChildInfo(effectscanvas, "sprite_scale", w,h,x,y);
	child[4] = CanvasChildInfo(effectscanvas, "sprite_tile", w,h,x,y);
	child[5] = CanvasChildInfo(effectscanvas, "sprite_rotate", w,h,x,y);


} 

main()  
{  
	RandomColour( effectscanvas, child[0] );
	StyleChange( effectscanvas, child[1], child[2] );
	ScaleChange( effectscanvas, child[3] );
	TileChange( effectscanvas, child[4] );
	RotateChange( effectscanvas, child[5] );
}  
 

public Close()  
{  
 
} 




