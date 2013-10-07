#define _CRT_SECURE_NO_DEPRECATE
/*
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
*/
#include "pgm.h"
#include "utils.h"
#include "fft.c"

#define TWO_PI (6.2831853071795864769252867665590057683943L)
#define EXTENSAO                "pgm"
#define APP_TYPE                "FFT"
#define LOG_NAME                "global.log"
#define ENV_TYPE                "CPU"

#define AMP(a, b) (sqrt((a)*(a)+(b)*(b)))

void lowpass_filter( float *image, int n, int raio);

int main(int argc, char *argv[])
{
    int i, j, m, n,raio;
	double			tempo_kernel;
    double			tempo_total;
    char			*output_filename;
	float			*image_in_host;
    float			*image_out_host;
	float			*image_amplitudes;
    pgm_t			ipgm, opgm;   
	image_file_t	*image_filename;

    timer_reset();
    timer_start();

    if (argc < 2)
    {
        printf("**Erro: parametros de entrada invalidos");
        exit(EXIT_FAILURE);
    }

    image_filename = (image_file_t *) malloc(sizeof(image_file_t));
    split_image_filename(image_filename, argv[1]);
    output_filename = (char *) malloc(40*sizeof(char));
    sprintf(output_filename, "%d.%d.%s.%s.%s", image_filename->res, image_filename->num, ENV_TYPE, APP_TYPE, EXTENSAO);

    if( ler_pgm(&ipgm, argv[1]) == -1)
        exit(EXIT_FAILURE);

	n = ipgm.width;
	m = (int)(log((double)n)/log(2.0));
	
	raio = n/8;
	
	image_in_host = (float *)malloc(n * n * sizeof(float));
    image_out_host = (float *)malloc(n * n * sizeof(float));

    opgm.width = n;
    opgm.height = n;
    
	for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            image_in_host[i*n + j] = (float) ipgm.buf[i*n + j];
			image_out_host[i*n + j] = (float) 0;
        }
    }

	timer_stop();
    tempo_total = get_elapsed_time();

    //====== Performance Test - start =======================================
    timer_reset();
    timer_start();
	
    //fft direta
	
    // filtro passa baixa
	//lowpass_filter(image_in_host, n, raio);
	//fft inversa
	
   
	timer_stop();
    tempo_kernel = get_elapsed_time();

    tempo_total += tempo_kernel;

    //====== Performance Test - end ============================================

    save_log_cpu(image_filename, tempo_kernel, tempo_total, LOG_NAME);
	
	image_amplitudes = (float*)malloc(n*n*sizeof(float));
    for (i=0; i < n; i++) {
        for (j=0; j < n; j++) {
            image_amplitudes[i*n + j] = (float) (AMP(((float*)image_in_host)[i*n + j], ((float*)image_out_host)[i*n + j]));
        }
    }

	normalizar_pgm(&opgm, image_in_host);
    escrever_pgm(&opgm, output_filename);
    
	destruir_pgm(&ipgm);
    destruir_pgm(&opgm);
    free(image_filename);
    free(output_filename);
    free(image_amplitudes);
    free(image_in_host);
    free(image_out_host);
   
   //_CrtDumpMemoryLeaks();
    return 0;
}

void lowpass_filter( float *image, int n, int raio)
{
	int u,v;
	int p_2 = n/2;
	int q_2 = n/2;
	int diff_u, diff_v;
	int dist = 0;
		
	for (u = 0; u < n; u++) {
		for (v = 0; v < n; v++) {
			dist = 0;
			diff_u = (u - p_2);
			diff_v = (v - q_2);
			dist = (diff_u*diff_u) + (diff_v*diff_v); 
			
			if (dist < raio*raio) {
				image[u*n + v] = (float) image[u*n + v];	
			} else {   
				image[u*n + v] = (float) 0;	
			}	
		}	    
	}
}



/* function prototypes */
void fft(int N, double (*x)[2], double (*X)[2]);
void fft_rec(int N, int offset, int delta,
             double (*x)[2], double (*X)[2], double (*XX)[2]);
void ifft(int N, double (*x)[2], double (*X)[2]);

/* FFT */
void fft(int N, double (*x)[2], double (*X)[2])
{
  /* Declare a pointer to scratch space. */
  double (*XX)[2] = malloc(2 * N * sizeof(double));

  /* Calculate FFT by a recursion. */
  fft_rec(N, 0, 1, x, X, XX);

  /* Free memory. */
  free(XX);
}

/* FFT recursion */
void fft_rec(int N, int offset, int delta,
             double (*x)[2], double (*X)[2], double (*XX)[2])
{
  int N2 = N/2;            /* half the number of points in FFT */
  int k;                   /* generic index */
  double cs, sn;           /* cosine and sine */
  int k00, k01, k10, k11;  /* indices for butterflies */
  double tmp0, tmp1;       /* temporary storage */

  if(N != 2)  /* Perform recursive step. */
    {
      /* Calculate two (N/2)-point DFT's. */
      fft_rec(N2, offset, 2*delta, x, XX, X);
      fft_rec(N2, offset+delta, 2*delta, x, XX, X);

      /* Combine the two (N/2)-point DFT's into one N-point DFT. */
      for(k=0; k<N2; k++)
        {
          k00 = offset + k*delta;    k01 = k00 + N2*delta;
          k10 = offset + 2*k*delta;  k11 = k10 + delta;
          cs = cos(TWO_PI*k/(double)N); sn = sin(TWO_PI*k/(double)N);
          tmp0 = cs * XX[k11][0] + sn * XX[k11][1];
          tmp1 = cs * XX[k11][1] - sn * XX[k11][0];
          X[k01][0] = XX[k10][0] - tmp0;
          X[k01][1] = XX[k10][1] - tmp1;
          X[k00][0] = XX[k10][0] + tmp0;
          X[k00][1] = XX[k10][1] + tmp1;
        }
    }
  else  /* Perform 2-point DFT. */
    {
      k00 = offset; k01 = k00 + delta;
      X[k01][0] = x[k00][0] - x[k01][0];
      X[k01][1] = x[k00][1] - x[k01][1];
      X[k00][0] = x[k00][0] + x[k01][0];
      X[k00][1] = x[k00][1] + x[k01][1];
    }
}

/* IFFT */
void ifft(int N, double (*x)[2], double (*X)[2])
{
  int N2 = N/2;       /* half the number of points in IFFT */
  int i;              /* generic index */
  double tmp0, tmp1;  /* temporary storage */

  /* Calculate IFFT via reciprocity property of DFT. */
  fft(N, X, x);
  x[0][0] = x[0][0]/N;    x[0][1] = x[0][1]/N;
  x[N2][0] = x[N2][0]/N;  x[N2][1] = x[N2][1]/N;
  for(i=1; i<N2; i++)
    {
      tmp0 = x[i][0]/N;       tmp1 = x[i][1]/N;
      x[i][0] = x[N-i][0]/N;  x[i][1] = x[N-i][1]/N;
      x[N-i][0] = tmp0;       x[N-i][1] = tmp1;
    }
}