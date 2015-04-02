#include "fft.h"
#include "ipc.h"
#include "audio.h"
#include <stdlib.h>
#include <math.h>
#include <fftw3.h>

double *fft_real_in;
fftw_complex *fft_comp_orig;
fftw_complex *fft_comp_work;
fftw_complex *fft_comp_mod;
double *fft_real_out;
fftw_plan fft_plan_forward;
fftw_plan fft_plan_backward;

void fft_init()
{
	fft_real_in = malloc(sizeof(double) * audio_period_size_frames);
	fft_real_out = malloc(sizeof(double) * audio_period_size_frames);

    fft_comp_orig = fftw_malloc(sizeof(fftw_complex) * audio_period_size_frames);
    fft_comp_work = fftw_malloc(sizeof(fftw_complex) * audio_period_size_frames);
    fft_comp_mod = fftw_malloc(sizeof(fftw_complex) * audio_period_size_frames);

    fft_plan_forward  = fftw_plan_dft_r2c_1d(audio_period_size_frames, fft_real_in, fft_comp_orig, FFTW_ESTIMATE);
    fft_plan_backward = fftw_plan_dft_c2r_1d(audio_period_size_frames, fft_comp_work, fft_real_out, FFTW_ESTIMATE);
}

void fft_perform()
{
    fftw_execute(fft_plan_forward);

    /* do something here */
    int i;

    for (i = 0; i < audio_period_size_frames; i++) {
    	/* copy fft_comp_orig -> fft_comp_work */
    	fft_comp_work[i][0] = fft_comp_orig[i][0];
    	fft_comp_work[i][1] = fft_comp_orig[i][1];

    	/* apply equalizer */
    	if (i < FFT_LEN) {
    		fft_comp_work[i][0] *= ipc_eq[i];
    		fft_comp_work[i][1] *= ipc_eq[i];
    	} else {
    		fft_comp_work[i][0] *= ipc_eq[FFT_LEN - (i - FFT_LEN + 1)];
    		fft_comp_work[i][1] *= ipc_eq[FFT_LEN - (i - FFT_LEN + 1)];
    	}
    }

    /* do a copy of modified FFT data, for fft_data_work will be destroyed during backward FFT transform */
    for (i = 0; i < audio_period_size_frames; i++) {
    	fft_comp_mod[i][0] = fft_comp_work[i][0];
    	fft_comp_mod[i][1] = fft_comp_work[i][1];
    }

    fftw_execute(fft_plan_backward);

    for (i = 0; i < audio_period_size_frames; i++)
    	fft_real_out[i] /= (double)audio_period_size_frames;

    /* compression */
    if (ipc_comp->enable) {
		double max_val = fft_real_out[0];
		for (i = 1; i < audio_period_size_frames; i++)
			if (fabs(fft_real_out[i]) > max_val)
				max_val = fabs(fft_real_out[i]);

		for (i = 0; i < audio_period_size_frames; i++) {
			double diff = fabs(fft_real_out[i]) / ipc_comp->threshold;
			if (diff > 1.0)
					fft_real_out[i] /= 1.0 + (log10(diff) * 2.0);

			fft_real_out[i] *= ipc_comp->gain;
		}
    }
}

void fft_free()
{
    fftw_destroy_plan(fft_plan_forward);
    fftw_destroy_plan(fft_plan_backward);
    fftw_free(fft_comp_orig);
    fftw_free(fft_comp_work);
    fftw_free(fft_comp_mod);

    free(fft_real_in);
    free(fft_real_out);
}

void fft_to_magn(fftw_complex *fft_in, double *magnitude_out, unsigned int len)
{
	unsigned int i;

	for (i = 0; i < len; i++)
		magnitude_out[i] = sqrt((fft_in[i][0] * fft_in[i][0]) + (fft_in[i][1] * fft_in[i][1])) / (double)audio_period_size_frames;
}
