#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "audio.h"
#include "fft.h"
#include "ipc.h"

int main(int argc, const char *argv[]) {
	char *audio_raw;
	double *audio_in_real;
	snd_pcm_sframes_t frames_num;
	double *plot_real_x;
	double *plot_fft_x;

    if (!audio_init())
    	return EXIT_FAILURE;

    audio_raw = malloc(AUDIO_SOFT_BUF_SIZE_BYTES);
    audio_in_real = malloc(sizeof(double) * audio_period_size_frames);
    plot_real_x = malloc(sizeof(double) * audio_period_size_frames);
    plot_fft_x = malloc(sizeof(double) * FFT_LEN);

	int i;
	for (i = 0; i < audio_period_size_frames; i++)
		plot_real_x[i] = (double)i * AUDIO_PERIOD_SEC;

	for (i = 0; i < FFT_LEN; i++)
		plot_fft_x[i] = (double)i * (double)audio_samp_rate / (double)audio_period_size_frames;

	/* initialization */
	fft_init();

    if (!ipc_init(plot_real_x))
    	return EXIT_FAILURE;

    /* re-initialization needed to start audio streams */
    audio_reinit(audio_raw);

    /* main loop */
    while (1) {
    	/* read audio buffer */
    	if (!audio_read(audio_raw, &frames_num)) {
    		audio_reinit(audio_raw);
    		continue;
    	}

    	/* raw audio input -> real numbers audio */
    	char *ptr = audio_raw;
    	int ptr_increment = AUDIO_FRAME_SIZE_BYTES * AUDIO_CHANNELS;

    	int16_t ivalue;
    	for (i = 0; i < audio_period_size_frames; i++) {
    		memcpy(&ivalue, ptr, 2);
    		audio_in_real[i] = (double)ivalue;
    		ptr += ptr_increment;
    	}

    	/* real numbers audio -> fft_real input */
    	memcpy(fft_real_in, audio_in_real, sizeof(double) * audio_period_size_frames);

    	/* perform fft */
    	fft_perform();

    	/* fft output -> raw audio output */
		ptr = audio_raw;
		for (i = 0; i < audio_period_size_frames; i++) {
			ivalue = (int16_t)fft_real_out[i];
			memcpy(ptr, &ivalue, 2);
			ptr += ptr_increment;
		}

    	/* write audio buffer */
    	if (!audio_write(audio_raw, frames_num)) {
    		audio_reinit(audio_raw);
    		continue;
    	}

    	/* fill IPC buffers */
    	ipc_write_plot_real(0, plot_real_x, audio_in_real, audio_period_size_frames);
    	ipc_write_plot_real(1, plot_real_x, fft_real_out, audio_period_size_frames);
    	ipc_write_plot_fft(2, plot_fft_x, fft_comp_orig, FFT_LEN);
    	ipc_write_plot_fft(3, plot_fft_x, fft_comp_mod, FFT_LEN);
    }

    audio_free();
    fft_free();

    free(audio_raw);
    free(audio_in_real);
    free(plot_real_x);
    free(plot_fft_x);

    return EXIT_SUCCESS;
}
