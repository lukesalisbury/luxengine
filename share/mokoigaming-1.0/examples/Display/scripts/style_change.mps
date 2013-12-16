#include <entities>
#include <graphics>
#include <helper>

new obj;
new timer = 4000;
new styles = 6;
new style = STYLE_GLOW;
new Fixed:x, Fixed:y, Fixed:z;

new style_name[42];

public Init( ... )
{
	obj = EntityGetNumber("object-id");
	EntityGetPosition(x, y, z);
	LanguageString(1+style, style_name);
}

main()
{
	ObjectEffect(object:obj, WHITE, .style = style, .colour2 = 0x089955FF);
	GraphicsDraw(style_name, TEXT, fround(x), fround(y)+74, 6, 0, 0);

	if ( TimerCountdownWithReset(timer, 4000) )
	{
		if (style >= styles)
			style = 0;
		else
			style++;
		LanguageString(1+style, style_name); // Refresh string
	}
}
