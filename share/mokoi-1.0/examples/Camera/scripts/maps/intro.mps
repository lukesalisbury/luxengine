#tryinclude <map_default>
#include <helper>

new time = 2000;
new target = 0;

public Init(...)
{
	TextSprites(true,"alpha02.png");
}

public Close()
{
	TextSprites(false,"alpha02.png");
}


main()
{
	if ( TimerCountdown(time) )
	{

	}
}
