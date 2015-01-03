#include <system>

#define area [.left,.top,.right,.bottom]



new menu[3]{} = { \
	"❶ Stop Music", \
	"❷ Stop Effect", \
	"❸ Play Dialog" \
};

new musicFiles[12]{32};
new effectsFiles[12]{32};

new textArea[area];

PointCollide( px, py )
{
	if(px < textArea.left)	return 0;
	if(px > textArea.right)	return 0;
	if(py > textArea.bottom)	return 0;
	if(py < textArea.top)	return 0;

	return 1;
}

main()
{
	new px = InputPointer(0);
	new py = InputPointer(1);

	if ( InputButton(BUTTON_ACTION3) == 1 )
	{
		DialogShow(0);
	}

	new i = 0;
	while ( musicFiles[i]{0} )
	{

		textArea.left = 20;
		textArea.right = textArea.left + ( StringLength(musicFiles[i]) * 8 );
		textArea.top = 20 + (i*10);
		textArea.bottom = textArea.top + 9;

		if ( PointCollide(px,py) )
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

 	i = 0;
	while ( effectsFiles[i]{0} )
	{
		textArea.left = 20;
		textArea.right = textArea.left + ( StringLength(effectsFiles[i]) * 8 );
		textArea.top = 20 + (i*10);
		textArea.bottom = textArea.top + 9;

		if ( PointCollide(px,py) )
		{
			GraphicsDraw(effectsFiles[i], TEXT, textArea.left, textArea.right, 1, 0, 0, RED );
			if ( InputButton(BUTTON_MOUSELEFT) == 1 )
			{
				AudioPlayMusic(musicFiles[i], 0, 0);
			}
		}
		else
		{
			GraphicsDraw(effectsFiles[i], TEXT, textArea.left, textArea.right, 1, 0, 0, BLACK);
		}
		i++;
	}

}

public Init(...)
{
	SystemAudioVolume(SOUNDFX, 128);
	SystemAudioVolume(MUSIC, 128);

	FileGetList(musicFiles, "music");
	FileGetList(effectsFiles, "soundfx");
}

public Close()
{

}
