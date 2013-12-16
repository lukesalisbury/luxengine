#include <entities>
#include <graphics>
#include <string>
new id = -1;
new x, y;

public Init( ... )
{
	id = EntityGetNumber("object-id");
}

main()
{
	new p = ObjectFollowPath(object:id, 60.0, x, y, true);
	if ( p >= 0  )
		DebugText("To:%d %dx%d",  p+1, x, y);
	else
		DebugText("ObjectFollowPath Error");

}
