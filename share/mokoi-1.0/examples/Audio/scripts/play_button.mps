new x = 0;
new y = 0;
new w = 0;
new h = 0;

new object:obj = OBJECT_NONE;
new action{64};

/* Init function is the call before anything else */
public Init(...)
{
	obj = EntityGetObject(); 
	EntityGetSetting( "action", action );

	ObjectInfo( obj, w, h, x, y);
}

/* Close function when it is deleted' */
public Close()
{
	
}

main()
{
	ObjectEffect(obj, 0xFFFFFFF);
	if ( x < InputPointer(0) < (x+w) )
	{
		if ( y < InputPointer(1) < (y+h) )
		{
			ObjectEffect(obj, 0xFF0000FF);
			if ( InputButton(BUTTON_MOUSELEFT) == 1 )
			{
				EntityPublicFunction( MAP_ENTITY, action );
			}
		}
	}
}
