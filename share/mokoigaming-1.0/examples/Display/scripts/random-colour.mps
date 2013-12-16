#include <helper>
#include <core>

new obj = -1;
new acolor = 0;

stock SetColourBits( &v, n, p )
{
	new sn = clamp( n, 0, 255);
	v |= (sn << p);
}

public Init( ... )
{
	obj = EntityGetNumber("object-id");
}

public Close()
{
}

main()
{
	
	acolor = 0;
	SetColourBits( acolor, random(255), 24); // Alpha
	SetColourBits( acolor, random(255), 16); // Blue
	SetColourBits( acolor, random(255), 8); // Green
	SetColourBits( acolor, random(255), 0); // Red

	ObjectEffect( object:obj, acolor );

}
