#include <graphics> 
#include <entities> 
#include <string> 
#include <mokoi> 
#include <core>
 
new Fixed:CheckPoints[4]; 
new Fixed:_x_, Fixed:_y_, Fixed:_z_  ; 
new dx, dy,dz; 
new Fixed:angle = 0.0; 
new Fixed:_speed_ = 40.0; 
new object:obj = object:-1; 
new changed = false;
new sprites[4]{} = ["player1.png:side", "player1.png:back", "player1.png:side",  "player1.png:front"];  

EntityMove() 
{ 
	new Fixed:speed =  _speed_ * GameFrame2(); 
	_x_ += (speed * fsin(angle, degrees)); 
	_y_ += (speed * fcos(angle, degrees)); 

	EntitySetPosition(_x_, _y_); 
	dx = fround(_x_); 
	dy = fround(_y_); 


	ObjectPosition(obj, dx,dy, -1, 0,0);
	if ( changed )
	{
		new dir = fround(angle/90);
		dir = clamp(0,3);
		ObjectReplace(obj, sprites[dir], SPRITE);
		changed = false;
	} 
} 
 
main() 
{ 
	CollisionSet(SELF, 0, 0, dx, dy, 32, 40); 
	check_collision(); 
	move() 
} 
 
move() 
{ 
	if ( !(0 < dx < 512) ) 
		angle -= 170.00 + random(10000); 
	else if ( !(0 < dy < 384) ) 
		angle -= 170.00 + random(10000); 
	if ( angle < 0 ) 
		angle += 360.00; 
 
	new hits = CollisionCalculate(SELF, 0); 
	ObjectEffect(obj,( hits ? 0xFF0000FF: 0xFFFFFFFF)); 
 
	EntityMove(); 
} 
 
check_collision() 
{ 
	new x_check = 0; 
	new y_check = 0; 
 
	CheckPoints[0] = angle; 
	CheckPoints[1] = angle + 90; 
	CheckPoints[2] = angle + 180; 
	CheckPoints[3] = angle + 270; 
 
	for (new n = 0; n < 1; n++) 
	{ 
		x_check = dx + fround(20.0 * fsin(CheckPoints[n], degrees)) + 16; 
		y_check = dy + fround(20.0 * fcos(CheckPoints[n], degrees)) + 20; 
		if ( MaskGetValue(x_check, y_check) > 40 ) 
		{
			changed = true; 
			angle = CheckPoints[0] - 180; 
		} 
	} 
 
 
} 
 
 
public Init(...) 
{ 
	EntityGetPosition(_x_, _y_, _z_); 
	dz = fround(_z_); 
	angle = random(8) * 33.0; 
	obj = object:EntityGetNumber("object-id"); 

	new dir = fround(angle/90.0);
	dir = clamp(0,3);
	ObjectReplace(obj, sprites[dir], SPRITE); 
} 
 
public Close() 
{ 
 
} 
