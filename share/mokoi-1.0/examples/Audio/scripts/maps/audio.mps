#include <system>

#define area [.left,.top,.right,.bottom]
#define point [.x,.y]

new musicFiles[12]{32};
new effectsFiles[12]{32};

new textArea[area];

new pointer[point];

PointCollide( px, py )
{
	if(px < textArea.left)	return 0;
	if(px > textArea.right)	return 0;
	if(py > textArea.bottom)	return 0;
	if(py < textArea.top)	return 0;

	return 1;
}


listMusic()
{
	new i = 0;
	while ( musicFiles[i]{0} ) // Check to see if the first character of a string is a valid character
	{
		textArea.left = 20;
		textArea.right = textArea.left + ( StringLength(musicFiles[i]) * 8 );
		textArea.top = 20 + (i*10);
		textArea.bottom = textArea.top + 9;

		if ( PointCollide(pointer.x, pointer.y) )
		{
			GraphicsDraw(musicFiles[i], TEXT, textArea.left, textArea.right, 1, 0, 0, RED );
			if ( InputButton(BUTTON_MOUSELEFT) == 1 )
			{
				AudioPlayMusic(musicFiles[i], 0, 0);
			}
		}
		else
		{
			GraphicsDraw(musicFiles[i], TEXT, textArea.left, textArea.right, 1, 0, 0, BLACK);
		}
		i++;
	}
}

listAudio()
{
	new i = 0;
	while ( effectsFiles[i]{0} )
	{
		textArea.left = 20;
		textArea.right = textArea.left + ( StringLength(effectsFiles[i]) * 8 );
		textArea.top = 20 + (i*10);
		textArea.bottom = textArea.top + 9;

		if ( PointCollide(pointer.x, pointer.y) )
		{
			GraphicsDraw(effectsFiles[i], TEXT, textArea.left, textArea.right, 1, 0, 0, RED );
			if ( InputButton(BUTTON_MOUSELEFT) == 1 )
			{
				AudioPlaySound(effectsFiles[i], 0, 0);
			}
		}
		else
		{
			GraphicsDraw(effectsFiles[i], TEXT, textArea.left, textArea.right, 1, 0, 0, BLACK);
		}
		i++;
	}
}

main()
{
	pointer.x = InputPointer(0);
	pointer.y = InputPointer(1);

	// Show Dialog 
	if ( InputButton(BUTTON_ACTION7) == 1 )
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
}

public Close()
{

}
