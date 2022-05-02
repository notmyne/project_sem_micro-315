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
#include <audio/play_melody.h>

#include <audio_processing.h>
#include <fft.h>
#include <arm_math.h>


//static void serial_start(void)
//{
//	static SerialConfig ser_cfg = {
//	    115200,
//	    0,
//	    0,
//	    0,
//	};
//
//	sdStart(&SD3, &ser_cfg); // UART3.
//}

//static void timer12_start(void){
//    //General Purpose Timer configuration
//    //timer 12 is a 16 bit timer so we can measure time
//    //to about 65ms with a 1Mhz counter
//    static const GPTConfig gpt12cfg = {
//        1000000,        /* 1MHz timer clock in order to measure uS.*/
//        NULL,           /* Timer callback.*/
//        0,
//        0
//    };
//
//    gptStart(&GPTD12, &gpt12cfg);
//    //let the timer count to max value
//    gptStartContinuous(&GPTD12, 0xFFFF);
//}

int main(void)
{

    halInit();
    chSysInit();
    mpu_init();

    //starts the serial communication
    //serial_start();
    //starts the USB communication
    //usb_start();
    //starts timer 12
    //timer12_start();

    //inits the motors
    motors_init();
    //initts the melody player
    playMelodyStart();
    //intialize audio processing
    mic_start(&processAudioData);


    /* Infinite loop. */
    while (1) {

//        //waits until a result must be sent to the computer
//        wait_send_to_computer();
//
//        SendFloatToComputer((BaseSequentialStream *) &SD3, get_audio_buffer_ptr(LEFT_OUTPUT), FFT_SIZE);


    }
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
