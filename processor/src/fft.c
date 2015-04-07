#include "fft.h"
#include "ipc.h"
#include "audio.h"
#include "compression.h"
#include <stdlib.h>
#include <math.h>
#include <fftw3.h>

double *fft_real_in;
double *fft_real_compressed;
fftw_complex *fft_comp_in;
fftw_complex *fft_comp_compressed;
fftw_complex *fft_comp_equalized;
fftw_complex *fft_comp_out;
double *fft_real_out;
fftw_plan fft_plan_forward;
fftw_plan fft_plan_forward_compressed;
fftw_plan fft_plan_backward;

void fft_init()
{
	fft_real_in = malloc(sizeof(double) * audio_period_size_frames);
	fft_real_compressed = malloc(sizeof(double) * audio_period_size_frames);
	fft_real_out = malloc(sizeof(double) * audio_period_size_frames);

    fft_comp_in = fftw_malloc(sizeof(fftw_complex) * audio_period_size_frames);
    fft_comp_compressed = fftw_malloc(sizeof(fftw_complex) * audio_period_size_frames);
    fft_comp_equalized = fftw_malloc(sizeof(fftw_complex) * audio_period_size_frames);
    fft_comp_out = fftw_malloc(sizeof(fftw_complex) * audio_period_size_frames);

    fft_plan_forward  = fftw_plan_dft_r2c_1d(audio_period_size_frames, fft_real_in, fft_comp_in, FFTW_ESTIMATE);
    fft_plan_forward_compressed  = fftw_plan_dft_r2c_1d(audio_period_size_frames, fft_real_compressed, fft_comp_compressed, FFTW_ESTIMATE);
    fft_plan_backward = fftw_plan_dft_c2r_1d(audio_period_size_frames, fft_comp_equalized, fft_real_out, FFTW_ESTIMATE);
}

void fft_perform()
{
	/* copy fft_real_in buffer to fft_real_compressed */
	memcpy(fft_real_compressed, fft_real_in, sizeof(double) * audio_period_size_frames);

	/* apply compression */
	compression(fft_real_compressed);

	/* execute FFT */
    fftw_execute(fft_plan_forward);
    fftw_execute(fft_plan_forward_compressed);

    int i;

    for (i = 0; i < audio_period_size_frames; i++) {
    	/* copy fft_comp_compressed -> fft_comp_work */
    	fft_comp_equalized[i][0] = fft_comp_compressed[i][0];
    	fft_comp_equalized[i][1] = fft_comp_compressed[i][1];

    	/* apply equalizer */
    	if (i < FFT_LEN) {
    		fft_comp_equalized[i][0] *= ipc_eq[i];
    		fft_comp_equalized[i][1] *= ipc_eq[i];
    	} else {
    		fft_comp_equalized[i][0] *= ipc_eq[FFT_LEN - (i - FFT_LEN + 1)];
    		fft_comp_equalized[i][1] *= ipc_eq[FFT_LEN - (i - FFT_LEN + 1)];
    	}
    }

    /* do a copy of modified FFT data, for fft_data_work will be destroyed during backward FFT transform */
    for (i = 0; i < audio_period_size_frames; i++) {
    	fft_comp_out[i][0] = fft_comp_equalized[i][0];
    	fft_comp_out[i][1] = fft_comp_equalized[i][1];
    }

    fftw_execute(fft_plan_backward);

    for (i = 0; i < audio_period_size_frames; i++)
    	fft_real_out[i] /= (double)audio_period_size_frames;
}

void fft_free()
{
    fftw_destroy_plan(fft_plan_forward);
    fftw_destroy_plan(fft_plan_forward_compressed);
    fftw_destroy_plan(fft_plan_backward);

    fftw_free(fft_comp_in);
    fftw_free(fft_comp_compressed);
    fftw_free(fft_comp_equalized);
    fftw_free(fft_comp_out);

    free(fft_real_in);
    free(fft_real_compressed);
    free(fft_real_out);
}

void fft_to_magn(fftw_complex *fft_in, double *magnitude_out, unsigned int len)
{
	unsigned int i;

	for (i = 0; i < len; i++)
		magnitude_out[i] = sqrt((fft_in[i][0] * fft_in[i][0]) + (fft_in[i][1] * fft_in[i][1])) / (double)audio_period_size_frames;
}
