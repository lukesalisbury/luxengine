
#tryinclude <map_default>
#include <helper>

new time = 2000;
new target = 0;

public Init(...)
{
	TextSprites(true,"alpha02.png");
	target = MapGetIdent("save", true);
}

public Close()
{
	TextSprites(false,"alpha02.png");
}


main()
{
	if ( TimerCountdown(time) )
	{
		MapSet(target, 0 , 0);
	}
}
