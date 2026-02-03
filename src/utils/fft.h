#ifndef FFT_H
#define FFT_H

#include <complex.h>

typedef float complex complex32_t;

#ifndef PI
#define PI 3.14159265358979
#endif

// FFT functions
void fft_1d_c32(complex32_t *x, int n);
void fft2_real(float *input_real, complex32_t *output_freq, complex32_t *buf, int n);
void ifft2_complex(complex32_t *input_freq, float *output_real, complex32_t *buf, int n);


#endif