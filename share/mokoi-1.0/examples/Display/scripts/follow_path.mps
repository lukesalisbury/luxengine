#include <helper>

new object:id;
new x, y;
new lx, ly;
new timer;
new Fixed:angle;

public Init( ... )
{
	id = EntityGetObject();
}

main()
{
	new p = PathMoveObject(id, 60.0, x, y, 1);

	if ( p >= 0  )
		ConsoleOutput("To:%d %dx%d %q",  p+1, x, y,  angle);
	else
		ConsoleOutput("ObjectFollowPath Error");

	if ( TimerCountdownWithReset(timer, 1000) )
	{
		angle = fatan2(x-lx,y-ly);
		EntityPublicFunction( MAP_ENTITY, "@SwitchCanvas", [ ARG_NUMBER, ARG_END ], _, angle);
	}

	lx = x;
	ly = y;
	
}
