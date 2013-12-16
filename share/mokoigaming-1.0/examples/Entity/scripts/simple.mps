#include <core>

forward public ChangeID();
public Fixed:_x_, Fixed:_y_, Fixed:_z_; 
public x, y, z; 
new string[4];
public id = 0;

public Init(...) 
{
	EntityGetPosition(_x_, _y_, _z_);
	x =  fround(_x_);
	y = fround(_y_);
	z = fround(_z_);
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
	strformat(string, _, true, "%d", id);
}

public Close() 
{ 

} 
