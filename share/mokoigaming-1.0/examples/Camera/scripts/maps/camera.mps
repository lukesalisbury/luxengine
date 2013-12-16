#include <default>
#include <graphics> 

 
new Fixed:worldx = 0.00; 
new Fixed:worldy = 0.00; 
new Fixed:layer1 = 0.00; 
new Fixed:layer2 = 0.00; 

new bool:b = false;
new str[32];

new map_width;

public Init(...)
{
	CameraSetScroll(b);
	map_width = MiscGetWidth("map");
}

main() 
{ 
	new Fixed:q = GameFrame2() * 10.00; 
	worldx += q*8 
	layer1 -= q/16.0; 
	layer2 += (q*3); 
 

	LayerSetOffset(-1, 0.00, worldy); 
	LayerSetOffset(1, layer1, worldy); 
	LayerSetOffset(2, layer2, worldy); 
	MapSetOffset(worldx, worldy); 
	GraphicsDraw("camera.png:1", SPRITE, fround(worldx)-16, 184, 5000,0, 0);

	strformat(str, _,true, "Scroll Mode: %d", b);
	GraphicsDraw(str, TEXT, 4,32,6000,0, 0);
	
	if ( InputButton(0,0) == 1)
	{
		b = !b;
		CameraSetScroll(b);
	}

	//Reset be to 0 when moved out of map
	if ( worldx > map_width)
	{

		LayerSetOffset(-1, 0.00, 0); 
		LayerSetOffset(1, 0, 0); 
		LayerSetOffset(2, 0, 0); 
		MapSetOffset(0, 0); 

	}

}
