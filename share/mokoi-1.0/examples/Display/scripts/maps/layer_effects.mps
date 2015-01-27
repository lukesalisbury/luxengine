// To ease development, <map_default> may include a Init Function. 
// If You wish to use your own uncomment the next line 
//#define HASINITFUNCTION 1 
#tryinclude <map_default> 
#include <entities> 
#include <graphics> 
#include <helper> 
 
new timer = 1000; 
new styles = 7; 
new style = 0; 
 
new style_name[42]; 
 
public Init( ... ) 
{ 
	StringFormat( style_name, _, "Style %d", style); 
	LayerSetEffect(5, 1); 
 
} 
 
main() 
{ 
	GraphicsDraw(style_name, TEXT, 0, 0, 6, 0, 0); 
 
	if ( TimerCountdownWithReset(timer, 1000) ) 
	{ 
		if (style >= styles) 
			style = 0; 
		else 
			style++; 
		StringFormat( style_name, _, "Style %d", style); 
		LayerSetEffect(2, style); 
	} 
} 
