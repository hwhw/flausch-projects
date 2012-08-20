#include "platform_config.h"
#include "uart.h"
#include "spi.h"
#include "diskio.h"
#include "systick.h"

#include <peripheral/int.h>

/* low priority interrupt as a life indicator */
void __attribute__ ((nomips16,interrupt(ipl1),vector(_TIMER_2_VECTOR))) timer2_interrupt(void) {
	static int step = 400;
	static int duty = 20000;

	mLED_2_Toggle();
	if(duty) {
		if(duty == 20000 || duty == 400) step = -step;
		PR2 = duty + step;
		duty = 0;
	} else {
		duty = PR2;
		PR2 = 64000 - duty;
	}

	IFS0bits.T2IF = 0; /* reset interrupt flag */
}

int main (void) {
	mInitAllLEDs();

	/* set up timer 2 for life indicator */
	PR2 = 20000;
	T2CON = 0x8030; // enable, prescaler=1/8
	IPC2bits.T2IP = 1;
	INTEnableSystemMultiVectoredInt();
	IEC0bits.T2IE = 1;

	systick_init();

	uart_init_highspeed();

	spi_sd_init();

	delay100usec(30000);

	unsigned int stat = disk_initialize();
	uart_puthex(stat);
	uart_puts("\r\n");

	while(1) {
	}
}
