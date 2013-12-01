#include <nds.h>

void handleVBlank() 
{
	uint16 but=0, x=0, y=0, xpx=0, ypx=0, z1=0, z2=0, batt=0, aux=0;
	int t1=0, t2=0;
	uint32 temp=0;
		
	but = REG_KEYXY;
	if (~but & 0x40) {
		x = touchReadXY().x;
		y = touchReadXY().y;
		xpx = touchReadXY().px;
		ypx =touchReadXY().py;
		z1 = touchRead(TSC_MEASURE_Z1);
		z2 = touchRead(TSC_MEASURE_Z2);
	}
		
	batt = touchRead(TSC_MEASURE_BATTERY);
	aux  = touchRead(TSC_MEASURE_AUX);

	temp = touchReadTemperature(&t1, &t2);

	IPC->buttons   = but;
	IPC->touchX    = x;
	IPC->touchY    = y;
	IPC->touchXpx  = xpx;
	IPC->touchYpx  = ypx;
	IPC->touchZ1   = z1;
	IPC->touchZ2   = z2;
	IPC->battery   = batt;
	IPC->aux       = aux;
	IPC->temperature = temp;
	IPC->tdiode1 = t1;
	IPC->tdiode2 = t2;
}

//---------------------------------------------------------------------------------
int main(int argc, char ** argv) {
//---------------------------------------------------------------------------------

	rtcReset();
		
	irqInit();
	irqSet(IRQ_VBLANK, handleVBlank);
	irqEnable(IRQ_VBLANK);

	// Keep the ARM7 idle
	while (1) swiWaitForVBlank();
}


