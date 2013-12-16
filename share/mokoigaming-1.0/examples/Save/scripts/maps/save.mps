#include <time>
#include <mokoi> 
#include <graphics>  
#include <string>

new saveinfo[64]= [5,0]; 
new info1[64]; 
new info2[64]; 
new puttytris[64]; 
new stringinfo[128]; 
new year=1970, month=0, day=0, hour=0, minute=0, second=0; 

public Init( ... ) 
{ 
	TextSprites(true,"alpha02.png");
	GameGetDetails(1, info1); 
	GameGetDetails(2, info2); 
	GameDetails(0x287d9d3a, 1, puttytris); 
}

main()
{
	TimestampDetails(info1[0], year, month, day, hour, minute, second);
	strformat(stringinfo, _, _, "Save 1: Press A\nTime: %d/%d/%d %d:%02d", year, month, day, hour, minute ); 
	GraphicsDraw(stringinfo, TEXT, 36,48,4000,0,0); 
 
	TimestampDetails(info2[0], year, month, day, hour, minute, second);
	strformat(stringinfo, _, _, "Save 2: Press S\nTime: %d/%d/%d %d:%02d", year, month, day, hour, minute ); 
	GraphicsDraw(stringinfo, TEXT, 36,128,4000,0,0); 
 

	strformat(stringinfo, _, _, "Other Game Saves\nPuttytris High Score: %d", puttytris[0] ); 
	GraphicsDraw(stringinfo, TEXT, 36,208,4000,0,0); 

	if ( InputButton(0,0) == 1 ) 
	{
		saveinfo[0] = TimestampCurrent();
		//GameSave(1, saveinfo); 
		GameSave(1, saveinfo);
		GameGetDetails(1, info1);
	} 
	if ( InputButton(1,0) == 1 ) 
	{
		saveinfo[0] = TimestampCurrent();
 		//GameSave(2, saveinfo); 
		GameSave(2, saveinfo);
		GameGetDetails(2, info2);
	}  
} 