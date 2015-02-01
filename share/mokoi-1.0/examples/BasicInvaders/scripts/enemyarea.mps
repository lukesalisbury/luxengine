/* Usage 
	Draw a rectangle on the map and attach this entity


*/

#define rect [.x, .y, .w, .h]

new ship_size = 20;
new ships_per_line = 16
new lines_of_ships = 5;
new total_ships = 10;
new screen_padding = 8;

new Fixed:x, Fixed:y, Fixed:z;
new dir = 0;
new dir_multiple = -1;
new Fixed:speed = 32.0;
new map_width, map_height;

new entity:ships[512];
new area[rect];
new last_line_y;
new object:obj;


public Init(...)
{
	obj = EntityGetObject();
	EntityGetPosition(x,y,z);

	ObjectInfo(obj, area.w, area.h, area.x, area.y );
	ObjectEffect(obj, 0xFFFFFF00);

	MapCurrentDimension(map_width, map_height);

	CreateEnemies( );
}

CreateEnemies( )
{
	new i = 0;
	new subx,suby;
	
	ships_per_line = area.w / ship_size;
	lines_of_ships = area.h / ship_size;
	total_ships = ships_per_line * lines_of_ships;

	if ( total_ships == 0 )
	{
	}
	else
	{
		total_ships = NumberClamp( total_ships, ships_per_line, 512);

		while ( i < total_ships )
		{
			subx = area.x + ( (i % ships_per_line) * ship_size);
			suby = area.y + ( (i / ships_per_line) * ship_size);
			ships[i] = EntityCreate("invader", "", subx, suby, z + 1, MapCurrentIdent() );
			i++;
		}
	}
}


main()
{
	if ( total_ships == 0 )
	{
		GraphicsDraw("No Invaders", TEXT, 5, 5, 6,0 ,0)
	}
	else
	{
		switch ( dir )
		{
			case 0: // right
			{
				if ( area.x + area.w  > map_width - screen_padding )
				{
					dir_multiple *= -1;
					dir += dir_multiple;
					last_line_y = area.y;
					ChangeDirection();
				}
				else
					x += GameFrameSeconds() * speed;
			} 
			case 1: // down
			{
				if ( area.y > last_line_y + ship_size )
				{
					dir += dir_multiple;
					ChangeDirection();
				}
				else
					y += GameFrameSeconds() * speed;
			} 
			case 2: // left
			{
				if ( area.x < screen_padding )
				{
					dir_multiple *= -1;
					dir += dir_multiple;
					last_line_y = area.y;
					ChangeDirection()
				}
				else
					x -= GameFrameSeconds() * speed;
			}
		}

		area.x = fround(x);
		area.y = fround(y);
		
		EntitySetPosition(x,y,z);
		ObjectPosition(obj, area.x, area.y,z, area.w, area.h);
	}
}

ChangeDirection()
{
	new i = 0;
	while ( i < total_ships )
	{
		EntityPublicFunction(ships[i], "ChangeDirection");
		i++;
	}

}

public Close()
{
	new i = 0;
	while ( i < total_ships )
	{
		EntityDelete(ships[i]);
		i++;
	}
}