#include <core>
#include <system>
public Fixed:x, Fixed:y, Fixed:z; 

new Fixed:speed = 0.5;

new object:obj1; 
new global =0;

main() 
{
	//Input Axis puts out -255 to 255
	x += InputAxis(0) * speed * GameFrameSeconds();
	y += InputAxis(1) * speed * GameFrameSeconds();

	ObjectPosition(obj1, fround(x), fround(y), z, 0, 0);
	EntitySetPosition(x, y, z);
	MapOffsetSet(x,y); 

	ConsoleOutput(" %q %q", MapOffsetGet('x'), MapOffsetGet('y'));
	ConsoleOutput("global %d", global);
} 
 
public Init(...) 
{
	global = EntityGetNumber("global");

	EntityGetPosition(x, y, z);

	obj1 = ObjectCreate("HornGirl.png:front", SPRITE, fround(x), fround(y), z, 0, 0, 0xFFFFFFFF, global);
	
} 

public UpdatePosition()
{
	EntityGetPosition(x, y, z);
	ObjectPosition(obj1, fround(x), fround(y), 5.0, 0, 0);
	MapOffsetSet(x, y); 

}

public Close() 
{ 
	
} 
