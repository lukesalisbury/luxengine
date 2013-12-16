#include <graphics>
#include <string>

/* Main Entity has 3 more Public Functions */
forward public save();
forward public load();
forward public dialogbox(line);

new dialog_obj = -1;
new dialog_line = -1;
new dialog_text[1024];

main()
{
	if ( dialog_line != -1 )
	{
		if (  InputButton(6,0) == 1)
		{
			ObjectDelete(dialog_obj);
			dialog_line = -1;
		}
		dialog();
	}
}

dialog()
{
	GraphicsDraw(dialog_text, TEXT, 104,104,6,0,0);
	GraphicsDraw("press enter to close", TEXT, 104,124,6,0,0);
}

public save()
{ 
}

public dialogbox(line)
{
	DialogGetString(line, dialog_text);
	DialogPlayAudio(line);
	dialog_line = line;
	dialog_obj = ObjectCreate("", 'r', 100, 100, 6, 300, 40);
	ObjectEffect(dialog_obj,0x00000077,_,_,_,_,STYLE_HGRADIENT,0x00000000);
}
