#include <time>

new stringinfo[3]{64} = [ "asdasdasd", "asdasdasd", "asdasdasd" ]; 
new saveinfo[64]= [5,0];
new info1[64];
new info2[64];
new puttytris[64];

new year=1970, month=0, day=0, hour=0, minute=0, second=0; 

public Init( ... )
{
	TextSprites(true,"alpha02.png");
	GameGetDetails(1, info1); 
	GameGetDetails(2, info2); 
	GameDetails(0x7a172be3, 1, puttytris); 
}

main()
{
	TimestampDetails(info1[0], year, month, day, hour, minute, second);
	StringFormat(stringinfo[0], _, "%L %d/%d/%d %d:%02d\nRandom Number:%d", 2, day, month, year, hour, minute, info1[1] );
	GraphicsDraw(stringinfo[0], TEXT, 36,48,4.000,0,0, 0xDDDDDDFF);

	TimestampDetails(info2[0], year, month, day, hour, minute, second);
	StringFormat(stringinfo[1],  _, "%L %d/%d/%d %d:%02d\nRandom Number:%d", 3, day, month, year, hour, minute, info2[1] );
	GraphicsDraw(stringinfo[1], TEXT, 36,128,4.000,0,0, 0xDDDDDDFF);

	StringFormat(stringinfo[2], _, "Game Saves\nPuttytris High Score: %d", puttytris[0] ); 
	GraphicsDraw(stringinfo[2], TEXT, 36, 208,4.000,0,0, 0xDDDDDDFF);

	if ( InputButton(0,0) == 1 ) 
	{
		saveinfo[0] = TimestampCurrent();
		saveinfo[1]  = NumberRandom(2000)
		GameSave(1, saveinfo);
		GameGetDetails(1, info1);
	} 
	if ( InputButton(1,0) == 1 ) 
	{
		saveinfo[0] = TimestampCurrent();
		saveinfo[1]  = NumberRandom(2000)
		GameSave(2, saveinfo);
		GameGetDetails(2, info2);
	}
} 