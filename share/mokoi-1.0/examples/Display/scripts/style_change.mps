#include <helper>

new object:obj;
new timer = 4000;
new styles = 6;
new style = STYLE_GLOW;
new x, y, z;

new style_name[42];

public Init( ... )
{
	obj = EntityGetObject();
	EntityGetPosition2(x, y, z);
	LanguageString(1+style, style_name);
}

main()
{
	ObjectEffect(obj, WHITE, .style = style, .colour2 = 0x089955FF);
	GraphicsDraw(style_name, TEXT, x, y+74, 6.0, 0, 0);

	if ( TimerCountdownWithReset(timer, 4000) )
	{
		if (style >= styles)
			style = STYLE_NORMAL;
		else
			style++;
		LanguageString(1+style, style_name); // Refresh string
	}
}
