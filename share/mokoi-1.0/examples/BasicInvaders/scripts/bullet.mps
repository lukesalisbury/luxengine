new type = 0;
new Fixed:x, Fixed:y, Fixed:z;
new object:obj;
new Fixed:speed = 50.0;
public Init(...)
{
	EntityGetPosition(x, y, z);
	obj = ObjectCreate("", RECTANGLE, fround(x), fround(y), 4, 2, 4, 0x22EEFFFF  );

}

public Close()
{
	CollisionSet(SELF, 0, 0, 0, 0, 0);
	ObjectDelete(obj);
}

main()
{
	new Fixed:movement = GameFrameSeconds()*speed;

	y -= movement;
	CheckForEnemies();

	if( y < 6.00)
	{
		EntityDelete();
	}
	else
	{

		CollisionSet(SELF, 0, 0, fround(x), fround(y), 2, 4);
		ObjectPosition(obj, fround(x), fround(y), z, 2, 4);
	}
}

CheckForEnemies()
{
	if ( CollisionCalculate() )
	{
		new entity:current;
		new angle;
		new dist;
		new rect;
		new type;
		while ( CollisionGetCurrent(SELF, current, angle, dist, rect, type) )
		{
			EntityPublicFunction(current, "hit");
			EntityDelete();
			return;
		}
	}

}