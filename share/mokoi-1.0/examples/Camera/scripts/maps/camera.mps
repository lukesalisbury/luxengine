new Fixed:worldX = 0.00; 
new Fixed:worldY = 0.00; 
new Fixed:layer1X = 0.00; 
new Fixed:layer2X = 0.00; 
new cameraX = -32; 

new Fixed:DirectionMultipler = 1.00;

new bool:cameraMode = false;

new str{32};

new map_width;
new map_height;


SetCameraMode( bool:mode )
{

	StringFormat(str, _, "Scroll Mode: %L", mode + 3  );
	CameraSetScroll(mode);

	cameraMode  = mode;
}

public Init(...)
{
	SetCameraMode( false );
	MapCurrentDimension( map_width, map_height );
}

main() 
{ 
	new Fixed:movement = GameFrameSeconds() * 50.00; 

	worldX += (movement) * DirectionMultipler 
	layer1X += (movement*3)* DirectionMultipler;
	layer2X += (movement*2)* DirectionMultipler; 
 	cameraX = fround(worldX) - 32;

	LayerSetOffset(-1, 0.00, worldY);  // All Layers
	LayerSetOffset(1, layer1X, worldY); 
	LayerSetOffset(2, layer2X, worldY); 
	
	MapOffsetSet(worldX, worldY); 

	GraphicsDraw("camera.png:1", SPRITE, cameraX, 184, 5.000, 0, 0);
	GraphicsDraw(str, TEXT, 4,32, 6.0 ,0, 0);
	
	if ( InputButton(0,0) == 1)
	{
		SetCameraMode( !cameraMode );
	}

	//when moved out of map, switch directions
	if ( worldX > map_width)
	{
		DirectionMultipler = -1.00;
	}
	else if ( worldX < 0 )
	{
		DirectionMultipler = 1.00;
	}
}
