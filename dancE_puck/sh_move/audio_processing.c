#include "ch.h"
#include "hal.h"
#include <main.h>
#include <usbcfg.h>
#include <chprintf.h>

#include <motors.h>
#include <audio/microphone.h>
#include <audio_processing.h>
#include <fft.h>
#include <arm_math.h>
#include <audio/play_melody.h>
#include <motor_control.h>
#include <dances.h>


//semaphore
static BSEMAPHORE_DECL(soundPickUpProcess_sem, TRUE);

//2 times FFT_SIZE because these arrays contain complex numbers (real + imaginary)
static float micLeft_cmplx_input[2 * FFT_SIZE];
static float micRight_cmplx_input[2 * FFT_SIZE];
static float micFront_cmplx_input[2 * FFT_SIZE];
static float micBack_cmplx_input[2 * FFT_SIZE];
//Arrays containing the computed magnitude of the complex numbers
static float micLeft_output[FFT_SIZE];
static float micRight_output[FFT_SIZE];
static float micFront_output[FFT_SIZE];
static float micBack_output[FFT_SIZE];

static const float freq_tab[] = {550, 620, 666, 697};
static const float epsilon = 3;
/*
*	Callback called when the demodulation of the four microphones is done.
*	We get 160 samples per mic every 10ms (16kHz)
*	
*	params :
*	int16_t *data			Buffer containing 4 times 160 samples. the samples are sorted by micro
*							so we have [micRight1, micLeft1, micBack1, micFront1, micRight2, etc...]
*	uint16_t num_samples	Tells how many data we get in total (should always be 640)
*/
void processAudioData(int16_t *data, uint16_t num_samples){

	/*
	*
	*	We get 160 samples per mic every 10ms
	*	So we fill the samples buffers to reach
	*	1024 samples, then we compute the FFTs.
	*
	*/
	float* const buffer_left_cmplx_input = get_audio_buffer_ptr(LEFT_CMPLX_INPUT);
	float* const buffer_right_cmplx_input = get_audio_buffer_ptr(RIGHT_CMPLX_INPUT);
	float* const buffer_front_cmplx_input = get_audio_buffer_ptr(FRONT_CMPLX_INPUT);
	float* const buffer_back_cmplx_input = get_audio_buffer_ptr(BACK_CMPLX_INPUT);
	float* const buffer_left_output = get_audio_buffer_ptr(LEFT_OUTPUT);
	float* const buffer_right_output = get_audio_buffer_ptr(RIGHT_OUTPUT);
	float* const buffer_front_output = get_audio_buffer_ptr(FRONT_OUTPUT);
	float* const buffer_back_output = get_audio_buffer_ptr(BACK_OUTPUT);

	static uint8_t  dance_idx = 0;
	static uint8_t cnt_process = 0, cnt_redun = 0;
	static uint16_t nb_sample = 0;

	//fills the input buffers of FFT_SIZE(*2) with 640samples/10ms
	uint16_t i=0;
	while(i < num_samples){
		buffer_right_cmplx_input[nb_sample] = data[i++];
		buffer_left_cmplx_input[nb_sample] = data[i++];
		buffer_back_cmplx_input[nb_sample] = data[i++];
		buffer_front_cmplx_input[nb_sample] = data[i++];
		nb_sample++;

		buffer_right_cmplx_input[nb_sample] = 0;
		buffer_left_cmplx_input[nb_sample] = 0;
		buffer_back_cmplx_input[nb_sample] = 0;
		buffer_front_cmplx_input[nb_sample] = 0;
		nb_sample++;

		if(nb_sample == 2*FFT_SIZE)
			break;
	}
	//FFT full buffers
	if (nb_sample == 2*FFT_SIZE){
		doFFT_optimized(FFT_SIZE, buffer_right_cmplx_input);
		doFFT_optimized(FFT_SIZE, buffer_left_cmplx_input);
		doFFT_optimized(FFT_SIZE, buffer_front_cmplx_input);
		doFFT_optimized(FFT_SIZE, buffer_back_cmplx_input);

		arm_cmplx_mag_f32(buffer_right_cmplx_input, buffer_right_output, FFT_SIZE);
		arm_cmplx_mag_f32(buffer_left_cmplx_input, buffer_left_output, FFT_SIZE);
		arm_cmplx_mag_f32(buffer_front_cmplx_input, buffer_front_output, FFT_SIZE);
		arm_cmplx_mag_f32(buffer_back_cmplx_input, buffer_back_output, FFT_SIZE);
		//readying the next sampling
		nb_sample = 0;
		cnt_process++;
	}



	if (cnt_process >0) {
		const uint16_t peak_pos = get_peak_pos(buffer_front_output, FFT_SIZE);
		const float peak_frequency = get_frequency(peak_pos);
		if(dance_idx == freq2dance(peak_frequency)){
			dance_idx = freq2dance(peak_frequency);
			cnt_redun++;
		}else{
			cnt_redun = 0;
		}
		cnt_process = 0;
	}
	if(cnt_redun >= 50){ //if freq is persistent then it is considered as input
		chBSemSignal(&soundPickUpProcess_sem);
		danceSetSong(dance_idx);
	}
	if(isDancing()){
		waitDanceFinish();
	}
}


void waitSoundPickUp(void){
	chBSemWait(&soundPickUpProcess_sem);
}

float* get_audio_buffer_ptr(BUFFER_NAME_t name){
	if(name == LEFT_CMPLX_INPUT){
		return micLeft_cmplx_input;
	}
	else if (name == RIGHT_CMPLX_INPUT){
		return micRight_cmplx_input;
	}
	else if (name == FRONT_CMPLX_INPUT){
		return micFront_cmplx_input;
	}
	else if (name == BACK_CMPLX_INPUT){
		return micBack_cmplx_input;
	}
	else if (name == LEFT_OUTPUT){
		return micLeft_output;
	}
	else if (name == RIGHT_OUTPUT){
		return micRight_output;
	}
	else if (name == FRONT_OUTPUT){
		return micFront_output;
	}
	else if (name == BACK_OUTPUT){
		return micBack_output;
	}
	else{
		return NULL;
	}
}

uint16_t get_peak_pos(const float *buffer, uint16_t size) {
	float max_value = 0.0f;
	uint16_t index = size / 2;
	for (uint16_t i = size / 2; i < size; ++i) {
		if (buffer[i] > max_value) {
			max_value = buffer[i];
			index = i;
		}
	}
	return index;
}

float get_frequency(uint16_t peak_pos) {
	// 2 measurements to determine the slope and offset
	const float x0 = 513.0f;
	const float y0 = 7800.0f;
	const float x1 = 1004.0f;
	const float y1 = 300.0f;

	// y = slope * (x - x0) + y0
	const float slope = (y1 - y0) / (x1 - x0);
	return slope * (peak_pos - x0) + y0;
}

uint8_t freq2dance(float frequency) {
	for(uint8_t i = 0; i < 4; i++){
		if(frequency > (freq_tab[i]-epsilon) && frequency < (freq_tab[i]+epsilon)){
			return i+1;
		}
	}
	return 0;
}
