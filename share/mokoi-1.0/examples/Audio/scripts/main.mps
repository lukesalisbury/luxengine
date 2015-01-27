#define NO_DIALOG -1
#define rect [.x,.y,.w,.h]

forward public dialogbox(line)

new dialogArea[rect];
new dialogString{300};
new dialogLine = NO_DIALOG;

new object:dialogBackground = OBJECT_NONE;
new object:dialogBox = OBJECT_NONE;

public Init(...)
{
	dialogBackground = ObjectCreate("", RECTANGLE, 0, 0, 6, 640, 480, 0x00000066, true);
	dialogBox = ObjectCreate("sprite-border.png:5", SPRITE, 160, 160, 6, 320, 160, 0xFFFFFFFF, true);
	
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
		GraphicsDraw(dialogString, TEXT, dialogArea.x, dialogArea.y, 6.002, 0, 0, 0x421234FF);
		CheckForClose();
	}
}

CheckForClose()
{
	if ( InputButton(BUTTON_MENU) == 1 )
	{
		dialogLine = NO_DIALOG;
		ObjectFlag(dialogBackground, FLAG_HIDDEN, true );
		ObjectFlag(dialogBox, FLAG_HIDDEN, true );
	}
}

SetRectFromText( string{}, area[rect], text_width, text_height )
{
	new max_length = 1;
	new length_count = 0;
	new lines = 1;
	new watch_for_color = false;
	new count = 0;

	while ( string{count} )
	{
		if ( string{count} == '\n' || string{count} == '\r' )
		{
			lines++;
			max_length = max(length_count, max_length);
			length_count = 0;
		}
		else if ( watch_for_color )
		{
			count++;
			watch_for_color = false;
		}
		else if ( string{count} == 0xA7 )
		{
			count++;
			watch_for_color = true;
		}
		else if ( string{count} <= 128 )
		{
			length_count++;
		}
		else if ( string{count} < 224 )
		{
			count++;

			length_count++;
		}
		else if ( string{count} < 240 )
		{
			count++;
			count++;

			length_count++;
		}
		else if ( string{count} < 245 )
		{
			count++;
			count++;
			count++;

			length_count++;
		}
		count++;
	}
	max_length = max(length_count, max_length);

	area.w = text_width * max_length;
	area.h = text_height * lines;

	area.w = (area.w /16) * 16; // Sprites border is 16px width

	area.x = 320 - (area.w/2);
	area.y = 240 - (area.h/2);

}


public dialogbox( line )
{
	if ( dialogLine == NO_DIALOG ) // We don't have a dialog box display
	{
		dialogLine = line;
		DialogGetString( line, dialogString );
		AudioPlayDialog(dialogLine);
		
		SetRectFromText( dialogString, dialogArea, 7, 16 );

		ObjectFlag(dialogBackground, FLAG_HIDDEN, false );
		ObjectFlag(dialogBox, FLAG_HIDDEN, false );
		ObjectPosition( dialogBox, dialogArea.x, dialogArea.y, 6,dialogArea.w, dialogArea.h );		

	}
}