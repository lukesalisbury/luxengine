new player = 0; 
new x = 10, y = 10; 

/* Init function is the call before anything else */
public Init(...) 
{ 
	new Fixed:fx, Fixed:fy, Fixed:fz;
	EntityGetPosition(fx, fy,fz);
	x = fround(fx);
	y = fround(fy)+32;
 
	player =  EntityGetNumber("player");
}

/* Close function when it is deleted' */
public Close()
{
	
}

main()
{
	new str[128];
	strformat(str, _,_, "Position %d %d", InputPointer(0, player), InputPointer(1, player) );
	GraphicsDraw(str, TEXT, x, y, 3, 10, 1); 
	
}
