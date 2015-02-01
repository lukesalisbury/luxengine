new global =0;

/* Init function is the call before anything else */
public Init(...)
{
	global = EntityGetNumber("global");
	
}

/* Close function when it is deleted' */
public Close()
{
	
}


main()
{
	ConsoleOutput("global %d", global);	
}
