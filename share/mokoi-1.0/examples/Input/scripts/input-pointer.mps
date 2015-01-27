new player = 0; 
new x = 10, y = 10; 
new z;

/* Init function is the call before anything else */
public Init(...) 
{ 
	EntityGetPosition2(x, y,z);
	player =  EntityGetNumber("player");
}

/* Close function when it is deleted' */
public Close()
{
	
}

main()
{
	new str{128};
	StringFormat(str, _, "Position %d %d", InputPointer(0, player), InputPointer(1, player) );
	GraphicsDraw(str, TEXT, x, y+ 10, 3, 10, 1); 
	
}
