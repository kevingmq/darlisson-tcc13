#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include "pgm.h"
#include "utils.h"

int main(int argc, char *argv[])
{
    int dx[3][3] = { {1, 0, -1}, {2, 0, -2}, {1, 0, -1} };
    int dy[3][3] = { {1, 2, 1}, {0, 0, 0}, {-1, -2, -1} };

    int i, j, s, sum, sum_x, sum_y, m, n;
    int image_width, image_height;
    LARGE_INTEGER frequency;
    LARGE_INTEGER start;
    LARGE_INTEGER end;

    char* output_filename;
    double interval;
    size_t image_size;
    pgm_t ipgm, opgm;
    imageFile_t* image;

    if (argc < 2)
    {
        printf("**Erro: parametros de entrada invalidos");
        exit(EXIT_FAILURE);
    }

    image = (imageFile_t *) malloc(sizeof(imageFile_t));
    splitImageFileName(image, argv[1]);
    output_filename = (char *) malloc(40*sizeof(char));
    sprintf(output_filename, "%d.%d.%s.%s.%s", image->res, image->num, APP_TYPE(argv[2]), DEV_TYPE(argv[3]), EXTENSAO);

    readPGM(&ipgm, argv[1]);
    image_width = ipgm.width;
    image_height = ipgm.height;
    image_size = image_width * image_height * sizeof(unsigned char);
    opgm.width = ipgm.width;
    opgm.height = ipgm.height;
    opgm.buf = (unsigned char *) malloc(image_size);

    //====== Performance Test - start =======================================
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start);

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

    QueryPerformanceCounter(&end);

    interval = (double) (end.QuadPart - start.QuadPart) / frequency.QuadPart;

    //====== Performance Test - end ============================================

	saveOnLog(image, interval);

    writePGM(&opgm, output_filename);
    destroyPGM(&ipgm);
    destroyPGM(&opgm);
    free(image);
    free(output_filename);

    return 0;
}