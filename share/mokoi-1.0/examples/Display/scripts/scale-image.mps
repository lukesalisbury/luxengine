#include <helper>
#include <core>

new object:display_object = object:-1;
new object_scale = 1000;
new scale_direction = 1;

public Init( ... )
{
	display_object = object:EntityGetNumber("object-id");
}

public Close()
{
}

main()
{
	ObjectEffect( display_object, WHITE, 0, object_scale, object_scale );
	object_scale += GameFrame() * scale_direction;

	if ( object_scale < 0500 || object_scale > 5000  )
	{
		scale_direction *= -1;
	}
	object_scale = clamp( object_scale,   0500, 5000 );

	DebugText("Scale Image: %q %q", object_scale, object_scale);

}
