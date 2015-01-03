#include <core>

forward public ChangeID();
public Fixed:_x_, Fixed:_y_, Fixed:_z_; 
 
/*
native EntityCreate(parententity[], id[], x, y, z, map_id = CURRENT_MAP, args[]="", ...);
native EntityDelete(id[] = SELF, length=sizeof id);
native EntityGetSetting(key[], string[], id[] = SELF, length=sizeof id );
native EntityGetNumber(key[], id[] = SELF, length=sizeof id );
native EntityFunction(function);
native EntityPublicFunction(id[], function[], args[]="", ...);
native EntityPublicVariable(id[], variable[]);
native EntityPublicVariableSet(id[], variable[], value);
*/

new string[4];
public id = 0;

main() 
{

 	GraphicsDraw("", RECTANGLE, _:_x_,_:_y_,_:_z_, 16, 16, 0x99999999);
	GraphicsDraw(string, TEXT, _:_x_,_:_y_,_:_z_, 32, 32, 0xDD1111FF);
} 
 
public Init(...) 
{
	EntityGetPosition(_x_, _y_, _z_);
	ChangeID();
	
} 

public ChangeID()
{
	id = random(999)+1;
	strformat(string, _, true, "%d", id);
}

public Close() 
{ 

} 
