/* Main Entity has 3 more Public Functions */
forward public save();
forward public load();
forward public dialogbox(line);



main()
{
	if ( InputButton(0) == 1 )
	{
		SectionSet("Section1", 0, 0);
	}
	else if ( InputButton(1) == 1 )
	{
		SectionSet("Section2", 0, 0);
	}
	else if ( InputButton(2) == 1 )
	{
		MapChange( MapID("entity") );
	}

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
