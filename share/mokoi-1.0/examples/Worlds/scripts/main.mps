/* Main Entity has 3 more Public Functions */
forward public save();
forward public load();
forward public dialogbox(line);


new sections[4];


public Init( ... )
{
	sections[0] = WorldLoad("Section1");
	sections[1] = WorldLoad("Section2");
	sections[2] = MapGetIdent("red", true);
	sections[3] = MapGetIdent("entity", false);
}

main()
{
	if ( InputButton(0) == 1 )
	{
		WorldSet(sections[0], 0, 0);
	}
	else if ( InputButton(1) == 1 )
	{
		WorldSet(sections[1], 0, 0);
	}
	else if ( InputButton(2) == 1 )
	{
		MapSet( sections[2], 0, 0 );
	}

	ConsoleOutput("%s=%x", "Section1", sections[0] );
	ConsoleOutput("%s=%x", "Section2", sections[1] );
	ConsoleOutput("%s=%x", "red", sections[2] );
	ConsoleOutput("%s=%x", "entity", sections[3] );

	displayinfo();

}

public load()
{ 
}

public save()
{ 
}

public dialogbox(line)
{

}

displayinfo()
{
	new string1{128};
	new section_grid_x = 0;
	new section_grid_y = 0;
	new map_id = 0;

	map_id = WorldGetIdent( 0, section_grid_x , section_grid_y );

	StringFormat(string1, _,  "§d%dx%d §4%d",  section_grid_x , section_grid_y, map_id); /* _ means past default argument */
	GraphicsDraw(string1, TEXT, 8, 8, 1, 0, 0 ); 

}
