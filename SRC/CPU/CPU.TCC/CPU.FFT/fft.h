#ifndef FFT_H
#define FFT_H

#define TWO_PI (6.2831853071795864769252867665590057683943L)

void fft(int N, float (*x)[2], float (*X)[2]);

void fft_rec(int N, int offset, int delta,
             float (*x)[2], float (*X)[2], float (*XX)[2]);

void ifft(int N, float (*x)[2], float (*X)[2]);

#endif