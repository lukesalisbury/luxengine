#define NO_DIALOG -1

forward public dialogbox(line)

new dialogString{256};
new dialogLine = NO_DIALOG;

new object:dialogBackground = OBJECT_NONE;
new object:dialogBox = OBJECT_NONE;

public Init(...)
{
	dialogBackground = ObjectCreate("", RECTANGLE, 0, 0, 6.000, 640, 480, 0x00000066);
	dialogBox = ObjectCreate("sprite-border.png:5", SPRITE, 160, 160, 6.001, 320, 160, 0xFFFFFFFF);

	ObjectFlag(dialogBackground, FLAG_HIDDEN, true );
	ObjectFlag(dialogBox, FLAG_HIDDEN, true );
}

main()
{
	HandleDialog();
}

HandleDialog()
{
	if (dialogLine != NO_DIALOG)
	{
		ObjectFlag(dialogBackground, FLAG_HIDDEN, false );
		ObjectFlag(dialogBox, FLAG_HIDDEN, false );
		GraphicsDraw(dialogString, TEXT, 160, 160, 6.002, 0, 0, 0x421234FF);
		CheckForClose();
	}
	else
	{
		ObjectFlag(dialogBackground, FLAG_HIDDEN, true );
		ObjectFlag(dialogBox, FLAG_HIDDEN, true );
	}
}

CheckForClose()
{
	if ( InputButton(BUTTON_MENU) )
		dialogLine = NO_DIALOG;
}

public dialogbox( line )
{
	if ( dialogLine == NO_DIALOG ) // We don't have a dialog box display
	{
		dialogLine = line;
		DialogGetString( line, dialogString );
		//AudioPlayDialog(dialogLine);
		
	}
}