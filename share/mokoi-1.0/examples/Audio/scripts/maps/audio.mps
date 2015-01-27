#include <system>
#include <controller>

#define area [.left,.top,.right,.bottom]
#define point [.x,.y]

new musicFiles[12]{32};
new effectsFiles[12]{32};

new musicArea[area];
new effectsArea[area];
new textArea[area];

new pointer[point];

PointCollide( px, py, Area[area] )
{
	if(px < Area.left)	return 0;
	if(px > Area.right)	return 0;
	if(py > Area.bottom)	return 0;
	if(py < Area.top)	return 0;

	return 1;
}


listMusic()
{
	new i = 0;
	while ( musicFiles[i]{0} ) // Check to see if the first character of a string is a valid character
	{
		textArea = musicArea;
		textArea.right = textArea.left + ( StringLength(musicFiles[i]) * 8 );
		textArea.top = musicArea.top + (i*10);
		textArea.bottom = textArea.top + 9;

		if ( PointCollide(pointer.x, pointer.y, textArea) )
		{
			GraphicsDraw(musicFiles[i], TEXT, textArea.left, textArea.top, 1, 0, 0, RED );
			if ( InputButton(BUTTON_MOUSELEFT) == 1 )
			{
				AudioPlayMusic(musicFiles[i], 10, 0);
			}
		}
		else
		{
			GraphicsDraw(musicFiles[i], TEXT, textArea.left, textArea.top, 1, 0, 0, BLACK);
		}
		i++;
	}
}

listAudio()
{
	new i = 0;
	while ( effectsFiles[i]{0} )
	{
		textArea = effectsArea;
		textArea.right = textArea.left + ( StringLength(effectsFiles[i]) * 8 );
		textArea.top +=  (i*10);
		textArea.bottom = textArea.top + 9;

		if ( PointCollide(pointer.x, pointer.y, textArea) )
		{
			GraphicsDraw(effectsFiles[i], TEXT, textArea.left, textArea.top, 1, 0, 0, RED );
			if ( InputButton(BUTTON_MOUSELEFT) == 1 )
			{
				AudioPlaySound(effectsFiles[i], 0, 0);
			}
		}
		else
		{
			GraphicsDraw(effectsFiles[i], TEXT, textArea.left, textArea.top, 1, 0, 0, BLACK);
		}
		
		ConsoleOutput("%d %d", textArea.left, textArea.top );
		i++;
	}
}

main()
{
	pointer.x = InputPointer(0);
	pointer.y = InputPointer(1);

	// Show Dialog 
	if ( InputButton(BUTTON_ACTION3) == 1 )
	{
		DialogShow(0);
	}

	listMusic();
	listAudio();



}

public Init(...)
{
	SystemAudioVolume(SOUNDFX, 128);
	SystemAudioVolume(MUSIC, 128);

	/* Scan Directories for Files */
	FileGetList(musicFiles, "music");
	FileGetList(effectsFiles, "soundfx");

	/* */
	ObjectInfo( ObjectHash("musicbox",false), musicArea.right, musicArea.bottom, musicArea.left, musicArea.top ); 
	ObjectInfo( ObjectHash("soundbox",false), effectsArea.right, effectsArea.bottom, effectsArea.left, effectsArea.top ); 
}

public Close()
{

}
