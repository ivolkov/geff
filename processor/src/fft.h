#ifndef FFT_H_
#define FFT_H_

#include <fftw3.h>
#include "audio.h"

#define FFT_LEN (audio_period_size_frames / 2)

extern double *fft_real_in;
extern fftw_complex *fft_comp_orig;
extern fftw_complex *fft_comp_mod;
extern double *fft_real_out;

void fft_init();
void fft_perform();
void fft_free();

void fft_to_magn(fftw_complex *fft_in, double *magnitude_out, unsigned int len);

#endif /* FFT_H_ */
