#include <graphics>
#include <entities>
#include <string>
#include <core>

new obj = -1;
new ascale = 120;
new dir = 1
new Fixed:x, Fixed:y, Fixed:z;
new dx,dy;

public Init( ... )
{
	obj = EntityGetNumber("object-id");

	EntityGetPosition(x,y,z);
	dx = fround(x);
	dy = fround(y);

}

public Close()
{
}

main()
{
	ObjectPosition(obj,dx, dy, 2, ascale, ascale);
	ascale += (GameFrame()/10) * dir;
	if ( ascale > 200 || ascale < 32 )
	{
		ascale = clamp(ascale, 32, 200);
		dir *= -1;
	}

}
