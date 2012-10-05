#include "platform_config.h"
#include "uart.h"
#include "spi.h"
#include "diskio.h"
#include "systick.h"

#include <peripheral/int.h>

#define mInitSoundOutput() LATCCLR=(1<<7);TRISCCLR=(1<<7);RPC7R=5;
#define mSound1() LATCSET=(1<<7);
#define mSound0() LATCCLR=(1<<7);

#define SOUND_HALF_BUFSIZE 512
static unsigned char samples[2][SOUND_HALF_BUFSIZE];
static volatile unsigned char samplebank_active=0;
static unsigned int sample_play=0;

// sample offset on SD card (in 512 byte blocks)
#define SAMPLE_OFFSET 8388608

/* sound interrupt @100kHz (currently) */
void __attribute__ ((nomips16,interrupt(ipl4),vector(_TIMER_2_VECTOR))) timer2_interrupt(void) {
	OC1RS = 302 - (((unsigned int)samples[samplebank_active][sample_play]));
	sample_play++;
	if(sample_play == SOUND_HALF_BUFSIZE) {
		sample_play = 0;
		samplebank_active = 1 - samplebank_active;
	}
	IFS0bits.T2IF = 0; /* reset interrupt flag */
}

int main (void) {
	unsigned int sample_count=0;
	unsigned char s;

	mInitAllLEDs();
	mInitSoundOutput();

	uart_init_highspeed();

	PR2 = 301; // 302 @ 40MHz = ca 132300 Hz (=44100*3)
	T2CON = 0x8000; // enable, prescaler=1/1

	IPC2bits.T2IP = 4;
	INTEnableSystemMultiVectoredInt();

	OC1CON = 0xA006; /* on, SIDL=1, OC32=0, OCFLT=0, OCTSEL=0 (Timer 2), OCM=PWM, fault bit disabled */
	OC1RS = 0;

	s=1;

	systick_init();
	uart_init_highspeed();

	spi_sd_init();

	while(disk_initialize() != 0) {
		delay100usec(30000);
	}
	uart_puts("SD card initialized\r\n");

	IEC0bits.T2IE = 1;
	while(1) {
		if(s != samplebank_active) {
			mLED_1_On();
			if(disk_read(0, samples[s], SAMPLE_OFFSET+sample_count, 1)) {
				uart_puts("Reading from SD failed :-(\r\n");
				goto error;
			}
			sample_count++;
			s = samplebank_active;
			mLED_1_Off();
		}
	}

error:
	OC1CON = 0;
	IEC0bits.T2IE = 0;
	while(1) {};
}
