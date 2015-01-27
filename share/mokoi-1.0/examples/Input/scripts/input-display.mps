#include <default> 
#include <graphics> 
#include <entities> 
#include <string> 
 
new player = 0; 
new object:obj1;
new object:obj2; 
new object:obj3;


new object:textObject1; 
new object:textObject2;
new object:textObject3;

new x = 10, y = 10; 
 
main() 
{ 
	new xaxis = 255; 
	new yaxis = -255; 
	new zaxis = 128; 

	xaxis = InputAxis(0, player)/8;
	yaxis = InputAxis(1, player)/8;
	zaxis = InputAxis(2, player)/8;

	if ( xaxis > 0 )
		ObjectPosition(obj1, x, y-5, 3, xaxis, 10); 
	else if ( xaxis < 0 )
		ObjectPosition(obj1, x+xaxis, y-5, 3, 0-xaxis, 10); 
	else
		ObjectPosition(obj1, x-1, y-5, 3, 1, 10); 

	if ( yaxis > 0 )
		ObjectPosition(obj2, x-5, y, 3, 10, yaxis); 
	else if ( yaxis < 0 )
		ObjectPosition(obj2, x-5, y+yaxis, 3, 10, 0-yaxis); 
	else 
		ObjectPosition(obj2, x-5, y-1, 3, 10, 1);

	if ( zaxis > 0 )
		ObjectPosition(obj3, x+44, y, 3, 10, zaxis); 
	else if ( zaxis < 0 )
		ObjectPosition(obj3, x+44, y+zaxis, 3, 10, 0-zaxis); 
	else 
		ObjectPosition(obj3, x+44, y-1, 3, 10, 1); 




} 

/*
❶ ❷ ❸ ❹ ❺ ❻ ❼ ❽ ❾ ❿ ➀ ➁ ➂ ➃ ➄ ➅
Player Controller Axis
Axis  1	2
y-	▲	△
y+	▼	▽
x-	◀	◁
x+	▶	▷
z-	◐	◒
z+	◑	◓ 
*/

public Init(...) 
{ 
	new Fixed:fx, Fixed:fy, Fixed:fz;
	EntityGetPosition(fx, fy,fz);
	x = fround(fx)+32;
	y = fround(fy)+32;
 
	player =  EntityGetNumber("player");

	obj1 = ObjectCreate("", RECTANGLE, 11, 10, 5, 16, 64, 0xFF0000FF);
	obj2 = ObjectCreate("", RECTANGLE, 100, 100, 5, 64, 16, 0xFF0000FF);
	obj3 = ObjectCreate("", RECTANGLE, 100, 10, 5, 64, 16, 0xFF0000FF);

	textObject1 = ObjectCreate("X Axis: ◀ & ▶", TEXT, x + 64 , y -32, 5.0, 0, 0, 0xFFFFFFFF); 
	textObject2 = ObjectCreate("Y Axis: ▲ & ▼", TEXT, x + 64 , y -8, 5.0, 0, 0, 0xFFFFFFFF); 
	textObject3 = ObjectCreate("Z Axis: ◐ & ◑", TEXT, x + 64 , y +16, 5.0, 0, 0, 0xFFFFFFFF); 

} 
 
public Close() 
{ 
	ObjectDelete(obj1); 
	ObjectDelete(obj2); 
	ObjectDelete(obj3); 
	ObjectDelete(textObject1); 
	ObjectDelete(textObject2); 
	ObjectDelete(textObject3); 
} 
