#include "platform_config.h"
#include "uart.h"

#include <peripheral/int.h>

#define mInitSoundOutput() LATCCLR=(1<<7);TRISCCLR=(1<<7);RPC7R=5;
#define mSound1() LATCSET=(1<<7);
#define mSound0() LATCCLR=(1<<7);

#define SOUND_HALF_BUFSIZE 192
static unsigned char samples[2][SOUND_HALF_BUFSIZE];
static volatile unsigned char samplebank_active=0;
static unsigned char sample_play=0;

/* sound interrupt @100kHz (currently) */
void __attribute__ ((nomips16,interrupt(ipl5),vector(_TIMER_2_VECTOR))) timer2_interrupt(void) {
	OC1RS=samples[samplebank_active][sample_play]<<3;
	sample_play++;
	if(sample_play == SOUND_HALF_BUFSIZE) {
		sample_play=0;
		samplebank_active=1-samplebank_active;
		mLED_2_Toggle();
	}
	IFS0bits.T2IF = 0; /* reset interrupt flag */
}

int main (void) {
	unsigned char i, s;

	mInitAllLEDs();
	mInitSoundOutput();

	uart_init_highspeed();

	PR2 = 2048; // 2048 @ 40MHz = ca 19531 Hz
	T2CON = 0x8000; // enable, prescaler=1/1

	IPC2bits.T2IP = 5;
	INTEnableSystemMultiVectoredInt();

	OC1CON = 0xA006; /* on, SIDL=1, OC32=0, OCFLT=0, OCTSEL=0 (Timer 2), OCM=PWM, fault bit disabled */
	OC1RS = 0;

	s=1;

	IEC0bits.T2IE = 1;
	while(1) {
		if(s != samplebank_active) {
			mLED_1_On();
			uart_putc('A'+i);
			for(i=0; i<SOUND_HALF_BUFSIZE; i++) {
				samples[s][i]=(unsigned char) uart_getc();
			}
			s = samplebank_active;
			mLED_1_Off();
		}
	}
}
