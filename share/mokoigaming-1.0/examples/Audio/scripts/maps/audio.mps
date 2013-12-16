
#define rect [.x,.y,.w,.h]

new menu[3]{} = { \
	"❶ Play Music", \
	"❷ Play Effect", \
	"❸ Play Dialog" \
};

new files[12][64];
new effects[12][64];

new r[rect];

PointCollide( px, py )
{
	if(px < r.x)	return 0;
	if(px > r.x + r.w)	return 0;
	if(py > r.y + r.h)	return 0;
	if(py < r.y)	return 0;

	return 1;
}
main()
{
	new px = InputPointer(0);
	new py = InputPointer(1);

	if ( InputButton(2) == 1 )
	{
		DialogShow(0);
	}

	new i = 0;
	while ( files[i][0] )
	{
		r.w = strlen(files[i]) * 8;
		r.y = 20 + (i*10);
		r.h = 9;
		r.x = 20;

		GraphicsDraw(files[i], TEXT, r.x, r.y, 0, 0, 0, PointCollide(px,py) ? RED : BLACK);
		if ( PointCollide(px,py) && InputButton(11) == 1 )
		{
			AudioPlayMusic(files[i], 0, 0);
		}
		i++;
	}

 	i = 0;
	while ( effects[i][0] )
	{
		r.w = strlen(files[i]) * 8;
		r.y = 170 + (i*10);
		r.h = 9;
		r.x = 20;

		GraphicsDraw(effects[i], TEXT, r.x, r.y, 0, 0, 0, PointCollide(px,py) ? RED : BLACK);
		if ( PointCollide(px,py) && InputButton(11) == 1 )
		{
			AudioPlaySound(effects[i]);
		}
		i++;
	}

}

public Init(...)
{
	AudioVolume(SOUNDFX, 128);
	AudioVolume(MUSIC, 128);

	FileGetList(files, "music");
	FileGetList(effects, "soundfx");
}

public Close()
{

}
