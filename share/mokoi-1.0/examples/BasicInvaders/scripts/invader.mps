new Fixed:x, Fixed:y, Fixed:z;
new dead = 0;
new object:obj;
new dir = 0;
new dir_multiple =1;
new Fixed:speed = 32.0;
forward public Init(...);
forward public Close();
forward public hit();

public Init(...)
{
	EntityGetPosition(x, y,z);
	obj = ObjectCreate("enemy.png:1", SPRITE, fround(x), fround(y), z, 0, 0);


}

public Close()
{
	CollisionSet( SELF, 0, 0, 0, 0,0);
	ObjectDelete(obj);
}

main()
{
	if (!dead)
	{
		move();
	}
}

move()
{
	switch ( dir )
	{
		case 0: // right
		{
			x += GameFrameSeconds() * speed
		} 
		case 1: // down
		{
			y += GameFrameSeconds() * speed
		} 
		case 2: // left
		{
			x -= GameFrameSeconds() * speed
		}
	}

	EntitySetPosition( x,y,z);
	ObjectPosition(obj, fround(x), fround(y), z, 0, 0);
	CollisionSet(SELF, 0, 1, fround(x), fround(y), 12, 12);
}

public ChangeDirection()
{
	dir += dir_multiple;

	if ( dir != 1 )
		dir_multiple *= -1;
}

public hit()
{
	//EntityPublicFunction(0, "addScore");
	dead = 1;
	AudioPlaySound("invader_death.wav");
	EntityDelete();
}