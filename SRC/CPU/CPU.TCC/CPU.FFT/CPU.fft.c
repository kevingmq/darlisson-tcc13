#define _CRT_SECURE_NO_DEPRECATE

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "pgm.h"
#include "utils.h"
#include "fft.h"

#define EXTENSAO                "pgm"
#define APP_TYPE                "FFT"
#define LOG_NAME                "global.log"
#define ENV_TYPE                "CPU"

#define AMP(a, b) (sqrt((a)*(a)+(b)*(b)))

void lowpass_filter(float *image, int n);

int main(int argc, char *argv[])
{
    int i, j, m, n;
	double			tempo_kernel;
    double			tempo_total;
    char			*output_filename;
	float			*image_amplitudes;
	float           (*x)[2];  
	float           (*X)[2]; 
	
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
	x = malloc(2 * n * n * sizeof(float));
	X = malloc(2 * n * n * sizeof(float));
		
	opgm.width = n;
    opgm.height = n;
    
	for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            x[i*n + j][0] = (float) ipgm.buf[i*n + j];
			x[i*n + j][1] = (float) 0;
        }
    }

	/* Check that n = 2^m for some integer m >= 1. */
	if (n >= 2) {
		i = n;
		while(i==2*(i/2)) i = i/2;  /* While i is even, factor out a 2. */
	}  /* For n >=2, we now have N = 2^n iff i = 1. */
	if (n < 2 || i != 1) {
		  printf(" %d deve ser um inteiro tal que n = 2^m , para m >= 1", n);
		  exit(EXIT_FAILURE);
	}
	
	timer_stop();
    tempo_total = get_elapsed_time();

    //====== Performance Test - start =======================================
    timer_reset();
    timer_start();
	j = 0;
	j = n*n;
	
    //fft direta
	fft(j, x, X);
	// filtro passa baixa
	lowpass_filter(X, n);
	//fft inversa
	for(i=0; i<j; i++) x[i][0] = x[i][1] = 0;
	ifft(j, x, X);
   
	timer_stop();
    tempo_kernel = get_elapsed_time();

    tempo_total += tempo_kernel;
    //====== Performance Test - end ============================================

    save_log_cpu(image_filename, tempo_kernel, tempo_total, LOG_NAME);
	
	image_amplitudes = (float*)malloc(n*n*sizeof(float));
	 for (i=0; i < n; i++) {
        for (j=0; j < n; j++) {
            image_amplitudes[i*n + j] = (float) (AMP(x[i*n + j][0], x[i*n + j][1]));
        }
    }
	
	normalizar_pgm(&opgm, image_amplitudes);
    escrever_pgm(&opgm, output_filename);
    free(x);
	free(X);
	destruir_pgm(&ipgm);
    destruir_pgm(&opgm);
    free(image_filename);
    free(output_filename);
    free(image_amplitudes); 
   _CrtDumpMemoryLeaks();
    return 0;
}

void lowpass_filter( float (*image)[2], int n)
{
	int u,v;
	int p_2 = n/2;
	int q_2 = n/2;
	float diff_u, diff_v;
	float dist = 0;
	int r = n/8;	
	
	for (u = 0; u < n; u++) {
		for (v = 0; v < n; v++) {
			dist = 0;
			diff_u = (u < p_2) ? u : u - n;
			diff_v = (v < q_2) ? v : v - n;
			dist = (float) (diff_u*diff_u  + diff_v*diff_v); 
			
			if (dist > r*r) {
				image[u*n + v][0] = 0;	
				image[u*n + v][1] = 0;	
			}
		}	    
	}
}