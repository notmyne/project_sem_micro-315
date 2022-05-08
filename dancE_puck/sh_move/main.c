#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ch.h"
#include "hal.h"
#include "memory_protection.h"
#include <usbcfg.h>
#include <main.h>
#include <chprintf.h>
#include <motors.h>
#include <audio/microphone.h>

#include <audio_processing.h>
#include <fft.h>
#include <communications.h>
#include <arm_math.h>
#include <audio/play_melody.h>
#include <audio/audio_thread.h>
#include <obstacles.h>
#include <dances.h>


//uncomment to send the FFTs results from the real microphones
//#define SEND_FROM_MIC

//uncomment to use double buffering to send the FFT to the computer
//#define DOUBLE_BUFFERING


//static complex_float altBufferCmplxInput[FFT_SIZE];

static void serial_start(void)
{
	static SerialConfig ser_cfg = {
	    115200,
	    0,
	    0,
	    0,
	};

	sdStart(&SD3, &ser_cfg); // UART3.
}

static void timer12_start(void){
    //General Purpose Timer configuration   
    //timer 12 is a 16 bit timer so we can measure time
    //to about 65ms with a 1Mhz counter
    static const GPTConfig gpt12cfg = {
        1000000,        /* 1MHz timer clock in order to measure uS.*/
        NULL,           /* Timer callback.*/
        0,
        0
    };

    gptStart(&GPTD12, &gpt12cfg);
    //let the timer count to max value
    gptStartContinuous(&GPTD12, 0xFFFF);
}

static THD_WORKING_AREA(waTestThd, 256);
static THD_FUNCTION(TestThd, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

    //systime_t time;
    move(-500);

    while(1){
        //time = chVTGetSystemTime();


        //100Hz
        //chThdSleepUntilWindowed(time, time + MS2ST(2000));
    }
}

void testThd_start(void){
	chThdCreateStatic(waTestThd, sizeof(waTestThd), NORMALPRIO, TestThd, NULL);
}

int main(void)
{

    halInit();
    chSysInit();
    mpu_init();

    //starts the serial communication
    serial_start();
    //starts the USB communication
    usb_start();
    //starts timer 12
    timer12_start();
    //inits the motors
    motors_init();
    //start AUDIO MODULE, AMP ETC
    dac_start();
    //init audio module
    playMelodyStart();
    //send_tab is used to save the state of the buffer to send (double buffering)
    testThd_start();
#ifdef DOUBLE_BUFFERING
    static float send_tab[FFT_SIZE];
#endif  /* DOUBLE_BUFFERING */

#ifdef SEND_FROM_MIC
//    starts the microphones processing thread.
//    it calls the callback given in parameter when samples are ready
    mic_start(&processAudioData);
#endif  /* SEND_FROM_MIC */

    /* Infinite loop. */
    while (1) {

#ifdef SEND_FROM_MIC
        //waits until a result must be sent to the computer
        //wait_send_to_computer();
#ifdef DOUBLE_BUFFERING
        //we copy the buffer to avoid conflicts
        arm_copy_f32(get_audio_buffer_ptr(LEFT_OUTPUT), send_tab, FFT_SIZE);
#endif  /* DOUBLE_BUFFERING */
#endif /*SNED FROM MIC*/
//        playMelody(SANDSTORMS, ML_SIMPLE_PLAY, NULL);
//        waitMelodyHasFinished();
        //move(500);


     chThdSleepMilliseconds(1000);

    }

}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
