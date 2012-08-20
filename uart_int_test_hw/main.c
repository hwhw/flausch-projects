#include "platform_config.h"
#include "fifo.h"
#include "uart.h"

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

	uart_init_highspeed();

//	char c = '0';
	while(1) {
		volatile int i;
		uart_puts("0123456789a");
		for(i=0; i< 500; i++);
		uart_puts("bcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ\r\n");
		for(i=0; i< 5000000; i++);
//		uart_putc(c++);
//		if(c > '9') { uart_puts("\r\n"); c = '0'; }
	}
}
