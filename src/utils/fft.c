#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <complex.h>

#include "fft.h"


void fft_1d_c32(complex32_t *x, int n) {
    // Note: n must be a power of 2.

    int index, bit_reversed_index;
    for (index = 1, bit_reversed_index = 0; index < n; index++) {
        int bit_mask = n >> 1;
        while (bit_reversed_index & bit_mask) {
            bit_reversed_index &= ~bit_mask;
            bit_mask >>= 1;
        }

        bit_reversed_index |= bit_mask;
        if (index < bit_reversed_index) {
            complex32_t temp = x[index];
            x[index] = x[bit_reversed_index];
            x[bit_reversed_index] = temp;
        }
    }

    int stage_size, half_size, k;
    complex32_t wlen, w, rotated_value;
    for (stage_size = 2; stage_size <= n; stage_size <<= 1) {
    // Iterate over FFT stages, sub-FFT sizes doubling each time
        half_size = stage_size >> 1;
        wlen = cexpf(-2.0f * I * PI / stage_size);

        for (int block_start = 0; block_start < n; block_start += stage_size) {
        // Iterate over independent butterfly blocks within this stage
            w = 1.0f;
            for (k = 0; k < half_size; k++) {
            // Individual butterfly operations within a block
                int lower_index = block_start + k;
                int upper_index = lower_index + half_size;
                rotated_value = w * x[upper_index];
                x[upper_index] = x[lower_index] - rotated_value;
                x[lower_index] = x[lower_index] + rotated_value;
                w *= wlen;
            }
        }
    }
}

void fft2_real(float *input_real, complex32_t *output_freq, complex32_t *buf, int n) {
    int x, y;
    for (y = 0; y < n; y++) { // Row FFTs
        int row_offset = y * n;
        for (x = 0; x < n; x++) buf[x] = input_real[row_offset + x];
        fft_1d_c32(buf, n);
        for (x = 0; x < n; x++) output_freq[row_offset + x] = buf[x];
    }
    for (x = 0; x < n; x++) { // Column FFTs
        for (y = 0; y < n; y++) buf[y] = output_freq[y * n + x];
        fft_1d_c32(buf, n);
        for (y = 0; y < n; y++) output_freq[y * n + x] = buf[y];
    }
}

void ifft2_complex(complex32_t *input_freq, float *output_real, complex32_t *buf, int n) {
    int x, y;
    for (y = 0; y < n; y++) { // Row inverse FFTs
        int row_offset = y * n;
        for (x = 0; x < n; x++) buf[x] = conjf(input_freq[row_offset + x]);
        fft_1d_c32(buf, n);
        for (x = 0; x < n; x++) input_freq[row_offset + x] = conjf(buf[x]);
    }

    for (x = 0; x < n; x++) { // Column inverse FFTs
        for (y = 0; y < n; y++) buf[y] = conjf(input_freq[y * n + x]);
        fft_1d_c32(buf, n);
        for (y = 0; y < n; y++) output_real[y * n + x] = crealf(buf[y]) / (n * n);
    }
}