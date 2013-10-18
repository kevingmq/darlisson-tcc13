#define _CRT_SECURE_NO_DEPRECATE
/*#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
*/
#include "../../../utils/pgm.h"
#include "../../../utils/utils.h"

#define EXTENSAO "pgm"
#define APP_TYPE "sobel"
#define LOG_NAME "global.log"
#define ENV_TYPE "CPU"

int main(int argc, char *argv[])
{
    int dx[3][3] = { {1, 0, -1}, {2, 0, -2}, {1, 0, -1} };
    int dy[3][3] = { {1, 2, 1}, {0, 0, 0}, {-1, -2, -1} };

    int i, j, s, sum, sum_x, sum_y, m, n;
    int image_width, image_height;

    double tempo_kernel;
    double tempo_total;
    char* output_filename;

    size_t image_size;
    pgm_t ipgm, opgm;
    image_file_t *image_filename;

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

    image_width = ipgm.width;
    image_height = ipgm.height;
    image_size = image_width * image_height * sizeof(unsigned char);
    opgm.width = ipgm.width;
    opgm.height = ipgm.height;
    opgm.buf = (unsigned char *) malloc(image_size);

    timer_stop();
    tempo_total = get_elapsed_time();

    //====== Performance Test - start =======================================
    timer_reset();
    timer_start();

    for (i = 1; i < image_height - 1; i++)
        for (j = 1; j < image_width - 1; j++)
        {
            sum_x = 0;
            sum_y = 0;
            for (m = -1; m <= 1; m++)
                for (n = -1; n <= 1; n++)
                {
                    s = ipgm.buf[(i + m) * image_width + j + n];
                    sum_x += s * dx[m + 1][n + 1];
                    sum_y += s * dy[m + 1][n + 1];
                }
            sum = abs(sum_x) + abs(sum_y);
            opgm.buf[i * image_width + j] = (sum > 255) ? 255 : sum;
        }

    timer_stop();
    tempo_kernel = get_elapsed_time();

    tempo_total += tempo_kernel;

    //====== Performance Test - end ============================================

    save_log_cpu(image_filename, tempo_kernel, tempo_total, LOG_NAME);

    escrever_pgm(&opgm, output_filename);
    destruir_pgm(&ipgm);
    destruir_pgm(&opgm);
    free(image_filename);
    free(output_filename);
    //_CrtDumpMemoryLeaks();
    return 0;
}