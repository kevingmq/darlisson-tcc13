#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#define SEPARADOR_CHAR   '.'
#define BARRA_CHAR   '/'
#define MILISECONDS 1000
#define MICROSECONDS 1000000
#define NANOSECONDS 1000000000

__int64 freq;
__int64 start;
double iclock = 0;

typedef struct _image_file_t
{
    int res;
    int num;
} image_file_t;


void save_log_cpu(image_file_t* img_file_t, double kernel_time, double total_time, char* log_filename){

    FILE *fp;

    if (( fp = fopen(log_filename, "a+" )) == NULL) {
        fprintf(stderr, "Failed to open file\n");
    }
     fprintf(fp,"| %d | %d | %d | %f | %f |\n", img_file_t->res, img_file_t->num, (img_file_t->res*img_file_t->res), (double) (kernel_time*MILISECONDS), total_time);

     fclose(fp);
}

void save_log_gpu(image_file_t* img_file_t, __int64 kernel_time, double rate_write, double rate_read, double total_time, char* log_filename){

    FILE *fp;

    if ((fp = fopen(log_filename, "a+")) == NULL) {
        fprintf(stderr, "Failed to open file\n");
    }
     fprintf(fp,"| %d | %d | %d | %f | %f | %f | %f |\n",
     img_file_t->res, img_file_t->num, (img_file_t->res*img_file_t->res),
     (double) kernel_time/(double) MICROSECONDS, (double) rate_write,(double) rate_read, total_time);
     fclose(fp);
}

void save_log_debug(__int64 kernel_time, FILE *fp){

    if ((fp = fopen("debug.log", "a+")) == NULL) {
        fprintf(stderr, "Failed to open file\n");
    }
     fprintf(fp,"| %d |\n", kernel_time);
}

void close_log_debug(FILE *fp){
    fclose(fp);
}

void split_image_filename(image_file_t * img_file_t, char *in)
{
    int i, j, bar, sep_1, sep_2, total_sep, lenght;
    char *temp, *temp2;
    total_sep = 0;
    sep_1 = 0;
    sep_2 = 0;
    j = 0;
    bar = 0;

    lenght = strlen(in);

    for (i = lenght; i > 0; i--)
    {
        if (in[i] == BARRA_CHAR)
        {
            bar = i;
            break;
        }
    }

    for (i = bar; i < lenght; i++)
    {
        if (in[i] == SEPARADOR_CHAR)
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

    temp = (char *) malloc((sep_1-bar)*sizeof(char));
    temp2 = (char *) malloc((sep_2-sep_1)*sizeof(char));

    for (i = (bar == 0) ? (0) : (bar + 1) ; i < sep_1; i++)
    {
        temp[j] = in[i];
        j++;
    }

    img_file_t->res = atoi(temp);
    j = 0;
    for (i = sep_1 + 1; i < sep_2; i++)
    {
        temp2[j] = in[i];
        j++;
    }
    img_file_t->num = atoi(temp2);
    free(temp);
    free(temp2);
}

void timer_start(void) {
#ifdef _WIN32
    QueryPerformanceCounter((LARGE_INTEGER * ) & start);
    QueryPerformanceFrequency((LARGE_INTEGER * ) & freq);
    #else
        struct timespec s;
    assert(clock_gettime(CLOCK_REALTIME, & s) == CL_SUCCESS);
    start = (i64) s.tv_sec * 1e9 + (i64) s.tv_nsec;
    freq = 1000000000;
    #endif
}

void timer_reset(void) {
    iclock = 0;
}

void timer_stop(void) {
    __int64 end;
    #ifdef _WIN32
    QueryPerformanceCounter((LARGE_INTEGER * ) & end);
    #else
        struct timespec s;
    assert(clock_gettime(CLOCK_REALTIME, & s) == CL_SUCCESS);
    n = (i64) s.tv_sec * 1e9 + (i64) s.tv_nsec;
    #endif
    end -= start;
    start = 0;
    iclock += end;
}

double get_elapsed_time(void) {
    return (double) iclock / (double) freq;
}