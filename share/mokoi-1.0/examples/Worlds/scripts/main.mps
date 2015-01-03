/* Main Entity has 3 more Public Functions */
forward public save();
forward public load();
forward public dialogbox(line);



main()
{
	if ( InputButton(0) == 1 )
	{
		WorldSet(EntityHash("Section1"), 0, 0);
	}
	else if ( InputButton(1) == 1 )
	{
		WorldSet(EntityHash("Section2"), 0, 0);
	}
	else if ( InputButton(2) == 1 )
	{
		MapSet( MapGetIdent("entity", false), 0, 0 );
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
