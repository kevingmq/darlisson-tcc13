#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#define SEPARADOR	'.'
#define EXTENSAO "pgm"
#define CPU_TYPE "CPU"
#define GPU_TYPE "GPU"
#define DEV_TYPE(x) ((x == 1) ? (CPU_TYPE) : (GPU_TYPE))
#define APP_TYPE(x) ((x == 1) ? ("sobel") : ("lowpass"))
#define LOG_NAME "global.log"

typedef struct _imageFile_t
{
    int res;
    int num;
} imageFile_t;


void saveOnLog(imageFile_t* imgFile, double interval){
    FILE *fp;

	if ((fp = fopen(LOG_NAME, "a+")) == NULL)
    {
        fprintf(stderr, "Failed to open file\n");
    }
	 fprintf(fp,"| %d | %d | %d | %f |\n", imgFile->res,imgFile->num,(imgFile->res*imgFile->res), interval);
     fclose(fp);
}

void splitImageFileName(imageFile_t * imgFile, char *in)
{
    int i, j, sep_1, sep_2, total_sep, lenght;
    char *temp, *temp2;
    total_sep = 0;
    sep_1 = 0;
    sep_2 = 0;
    j = 0;

    lenght = strlen(in);

    for (i = 0; i < lenght; i++)
    {
        if (in[i] == SEPARADOR)
        {
            if (total_sep == 0)
            {
                sep_1 = i;
                total_sep++;
            }
            else if (total_sep == 1)
            {
                sep_2 = i;
                break;
            }
        }
    }

    temp = (char *) malloc(sep_1*sizeof(char));
    temp2 = (char *) malloc((sep_2-sep_1)*sizeof(char));

    for (i = 0; i < sep_1; i++)
    {
        temp[i] = in[i];
    }

    imgFile->res = atoi(temp);

    for (i = sep_1 + 1; i < sep_2; i++)
    {
        temp2[j] = in[i];
        j++;
    }
    imgFile->num = atoi(temp2);
    free(temp);
    free(temp2);
}