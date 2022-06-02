#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ch.h"
#include "hal.h"
#include "memory_protection.h"
#include <main.h>
#include <chprintf.h>
#include <motors.h>
#include <audio/microphone.h>

#include <audio_processing.h>
#include <fft.h>
#include <arm_math.h>
#include <audio/play_melody.h>
#include <audio/audio_thread.h>
#include <obstacles.h>
#include <dances.h>
#include <sensors/proximity.h>
#include <motor_control.h>


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



int main(void)
{

    halInit();
    chSysInit();
    mpu_init();

    //starts the serial communication
    serial_start();
    //inits the motors
    motors_init();
    //start AUDIO MODULE, AMP ETC
    dac_start();
    //init audio module
    playMelodyStart();
    mic_start(&processAudioData);

    danceThd_start();

    obstacles_start();
    //chprintf((BaseSequentialStream*) &SD3," seq stest" );




    /* Infinite loop. */
    while (1) {



     chThdSleepMilliseconds(1000);

    }

}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
