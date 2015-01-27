#include <helper>

new object:obj;

public Init( ... )
{
	obj = EntityGetObject();
}

public Close()
{
}

main()
{
	RandomColour( obj );
}

RandomColour( object:item )
{

	new colour = 0;
	BitsSet( colour, random(255), 24, 8); // Alpha
	BitsSet( colour, random(255), 16, 8); // Blue
	BitsSet( colour, random(255), 8, 8); // Green
	BitsSet( colour, random(255), 0, 8); // Red

	ObjectEffect( item, colour );

}

