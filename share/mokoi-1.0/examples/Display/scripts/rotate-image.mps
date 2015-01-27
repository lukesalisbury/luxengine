new object:display_object = OBJECT_NONE;
new ascale = 0;

public Init( ... )
{
	display_object = EntityGetObject();
}

public Close()
{
}

main()
{
	ObjectEffect(display_object, WHITE, ascale);
	ascale += GameFrame();
	ascale %= 360;

}
