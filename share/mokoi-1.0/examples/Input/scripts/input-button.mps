#include <entities> 
#include <graphics> 
#include <string> 
#include <core> 
 
new object:textObject;
new object:obj;
new player = 0; 
new button = 0; 
new mode = 0; 
new color[3] = [ 0x000000FF, 0xFF000011, 0xFFFFFFFF ]; 
 
new inputButton[]{} = [ "❶", "❷", "❸", "❹", "❺", "❻", "❼", "❽", "❾", "❿", "➀", "➁", "➂", "➃", "➄", "➅" ]; 
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
	new x, y, z; 
	EntityGetPosition2(x, y, z); 
 
	obj = EntityGetObject("object-id"); 
	player = EntityGetNumber("player"); 
	button = EntityGetNumber("button"); 
 
	button = clamp( button, 0, 15 );

	textObject = ObjectCreate( inputButton[button], TEXT, x+8, y+8, 5.000, 0, 0, WHITE ); 
 
} 
 
main() 
{ 
	mode = InputButton( button, player ); 
	mode = clamp( mode, 0, 2); 
  
	ObjectEffect( obj, color[mode] ); 
} 
 
public Close() 
{ 
	ObjectDelete(textObject); 
}