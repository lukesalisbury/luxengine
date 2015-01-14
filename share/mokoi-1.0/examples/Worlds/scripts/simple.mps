#include <core>

public x, y, z; 

new object:obj1 = OBJECT_NONE; 
new object:obj2 = OBJECT_NONE; 

new object:textObject1 = OBJECT_NONE; 
new object:textObject2 = OBJECT_NONE; 



main() 
{
	new xaxis = 255; 
	new yaxis = -255; 


	xaxis = InputAxis(0)/8;
	yaxis = InputAxis(1)/8;

	if ( xaxis > 0 )
		ObjectPosition(obj1, x, y-5, 5.0, xaxis, 10); 
	else if ( xaxis < 0 )
		ObjectPosition(obj1, x+xaxis, y-5, 5.0, 0-xaxis, 10); 
	else
		ObjectPosition(obj1, x-1, y-5, 5.0, 1, 10); 

	if ( yaxis > 0 )
		ObjectPosition(obj2, x-5, y, 5.0, 10, yaxis); 
	else if ( yaxis < 0 )
		ObjectPosition(obj2, x-5, y+yaxis, 5.0, 10, 0-yaxis); 
	else 
		ObjectPosition(obj2, x-5, y-1, 5.0, 10, 1);


	ConsoleOutput("%d %d", x, y);

} 
 
public Init(...) 
{
	EntityGetPosition2(x, y, z);

	ConsoleLog("%d %d", x, y);

	obj1 = ObjectCreate("", RECTANGLE, 11, 10, 5.0, 16, 64, 0xFF0000FF, true);
	obj2 = ObjectCreate("", RECTANGLE, 100, 100, 5.0, 64, 16, 0xFF0000FF, true);

	textObject1 = ObjectCreate("X Axis: ◀ & ▶", TEXT, x , y, 5.0, 64, 16, 0xFF0000FF ); 
	textObject2 = ObjectCreate("Y Axis: ▲ & ▼", TEXT, x , y +16, 5.00, 0, 0, 0xFF0000FF ); 


	
} 

public Close() 
{ 

} 
