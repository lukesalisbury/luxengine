#tryinclude <map_default>
#include <system>

new Fixed:worldx = 0.00; 
new Fixed:worldy = 0.00; 
new Fixed:sin=0.00;

public Init( ... )
{
	ConfigSetString("map.centerview", "0");
}


main()
{
	new Fixed:q = GameFrameSeconds() * 26.00; 

	
	sin += q;
	worldx += q;
 	worldy += fsin(sin, degrees);

	MapOffsetSet(worldx, worldy); 
}
