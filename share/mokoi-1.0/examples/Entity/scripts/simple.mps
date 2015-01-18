#include <core>

forward public ChangeID();
public x, y, z; 
new string[4];
public id = 0;

public Init(...) 
{
	EntityGetPosition2(x, y, z);
	ChangeID();
} 

main() 
{
 	GraphicsDraw("", RECTANGLE, x, y, z, 16, 16, 0x99999999);
	GraphicsDraw(string, TEXT, x, y, z, 32, 32, 0xDD1111FF);
} 

public ChangeID()
{
	id = random(99)+1;
	StringFormat(string, _, true, "%d", id);
}

public Close() 
{ 

} 
