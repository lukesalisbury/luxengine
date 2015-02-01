#include <default>
#include <graphics>
#include <string>

/* Main Entity has 3 more Public Functions */
forward public save();
forward public load();
forward public dialogbox(line);

new DayNight[24] = [
	0x4E5CAFFF,\
	0x5964A9FF,\
	0x606BAEFF,\
	0x6A76BAFF,\
	0x6E76BAFF,\
	0x7883C6FF,\
	0x98A3E8FF,\
	0xB6BEEEFF,\
	0xD6DAF3FF,\
	0xF4F5FBFF,\
	0xFFFFFFFF,\
	0xFFFFFFFF,\
	0xF7F8ECFF,\
	0xF7F8E7FF,\
	0xEFF2E1FF,\
	0xEDEDD2FF,\
	0xE8ECC8FF,\
	0xE8E1BEFF,\
	0xE4C47EFF,\
	0xE5A67EFF,\
	0xE59E96FF,\
	0x7D7CC8FF,\
	0x766FD8FF,\
	0x645CBEFF\
];
new hour = 20;
new minute = 0;
new Fixed:seconds = 0.0;
new Fixed:timemod = 2000.00;
new str[10];
main()
{
	seconds += GameFrameSeconds() * timemod;
	if ( seconds >= 60.0 )
	{
		minute++;
		seconds -= 60.0;
	}
	if ( minute >= 60 )
	{
		hour++;
		minute -= 60;
	}

	hour %= 24;

	LayerColour(0, DayNight[hour]);
	LayerColour(1, DayNight[hour]);
	LayerColour(2, DayNight[hour]);
	LayerColour(3, DayNight[hour]);
	LayerColour(4, DayNight[hour]);
	LayerColour(5, DayNight[hour]);

	strformat(str, _,true, "Time: %02d:%02d", hour, minute);
	GraphicsDraw(str, TEXT, 4,20,6,0, 0);
}
