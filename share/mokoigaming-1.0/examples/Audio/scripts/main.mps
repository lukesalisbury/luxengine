forward public dialogbox(line)

new dialog[256];
new dline = -1;

main()
{
	drawdialog();
}

drawdialog()
{
	if (dline == -1)
		return;
	GraphicsDraw("", RECTANGLE, 0, 0, 6000, 640, 640, 0x00000066);
	GraphicsDraw("", RECTANGLE, 80, 80, 6001, 348, 200, 0xFFFFFFFF);
	GraphicsDraw(dialog, TEXT, 84, 84, 6002, 0, 0, 0x421234FF);

	if ( InputButton(6) == 1 )
		dline = -1;
}

public dialogbox(line)
{
	if (dline != -1)
		return;
	DialogGetString(line, dialog);
	DialogPlayAudio(line);
	dline = line;
}