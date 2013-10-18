#define CL_VERSION_1_1
#define _CRT_SECURE_NO_DEPRECATE
/*#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
*/
#include <CL/cl.h>
#include <math.h>
#include "../../../utils/cl_utils.h"
#include "../../../utils/utils.h"
#include "../../../utils/pgm.h"

#define PI 3.14159265358979
#define MAX_PLATFORM_ID         2
#define MAX_SOURCE_SIZE (0x100000)

#define MAX_WORK_GROUP_ITEM_SIZE_DIM_1 16
#define MAX_WORK_GROUP_ITEM_SIZE_DIM_2 16

#define AMP(a, b) (sqrt((a)*(a)+(b)*(b)))

#define EXTENSAO                "pgm"
#define APP_TYPE                "FFT"
#define LOG_NAME                "global.log"
#define C_NOME_ARQ_KERNEL       "../../../utils/kernels/fft_kernel.cl"
#define ENV_TYPE                "GPU"
#define MAX_CALL_FFT            4

cl_device_id        device_id  = NULL;
cl_context          context    = NULL;
cl_command_queue    cmd_queue  = NULL;
cl_program          program    = NULL;
cl_event            *kernel_butter_events;
int                 butter_event_it = 0;
enum Tipo {
    direta = 0,
    inversa = 1
};

struct event_in_fft_t {
    cl_event  kernel_bitsrev;
    cl_event  kernel_normalize;
};

int config_workgroup_size(size_t* global_wg, size_t* local_wg, cl_int dim_1, cl_int dim_2)
{
    switch(dim_2) {
    case 1:
        global_wg[0] = dim_1;
        global_wg[1] = 1;
        local_wg[0]  = 1;
        local_wg[1]  = 1;
        break;
    default:
        global_wg[0] = dim_1;
        global_wg[1] = dim_2;
        local_wg[0]  = MAX_WORK_GROUP_ITEM_SIZE_DIM_1;
        local_wg[1]  = MAX_WORK_GROUP_ITEM_SIZE_DIM_2;
        break;
    }

    return 0;
}

int fft_main(cl_mem dst, cl_mem src, cl_mem twiddles, cl_int m, enum Tipo direcao, struct event_in_fft_t *fft_event)
{
    cl_int ret_code;

    cl_int iter;
    cl_uint flag;

    size_t global_wg[2];
    size_t local_wg[2];

    cl_int n = 1 << m;

    cl_kernel kernel_bits_rev = NULL;
    cl_kernel kernel_butterfly_op = NULL;
    cl_kernel kernel_normalize = NULL;

    kernel_bits_rev = clCreateKernel(program, "bits_reverse", &ret_code);
    kernel_butterfly_op = clCreateKernel(program, "butterfly_operation", &ret_code);
    kernel_normalize = clCreateKernel(program, "normalizar", &ret_code);

    switch (direcao) {
        case direta:flag = 0x00000000; break;
        case inversa:flag = 0x80000000; break;
    }

    CL_CHECK(clSetKernelArg(kernel_bits_rev, 0, sizeof(cl_mem), (void *)&dst));
    CL_CHECK(clSetKernelArg(kernel_bits_rev, 1, sizeof(cl_mem), (void *)&src));
    CL_CHECK(clSetKernelArg(kernel_bits_rev, 2, sizeof(cl_int), (void *)&m));
    CL_CHECK(clSetKernelArg(kernel_bits_rev, 3, sizeof(cl_int), (void *)&n));

    CL_CHECK(clSetKernelArg(kernel_butterfly_op, 0, sizeof(cl_mem), (void *)&dst));
    CL_CHECK(clSetKernelArg(kernel_butterfly_op, 1, sizeof(cl_mem), (void *)&twiddles));
    CL_CHECK(clSetKernelArg(kernel_butterfly_op, 2, sizeof(cl_int), (void *)&m));
    CL_CHECK(clSetKernelArg(kernel_butterfly_op, 3, sizeof(cl_int), (void *)&n));
    CL_CHECK(clSetKernelArg(kernel_butterfly_op, 5, sizeof(cl_uint), (void *)&flag));

    CL_CHECK(clSetKernelArg(kernel_normalize, 0, sizeof(cl_mem), (void *)&dst));
    CL_CHECK(clSetKernelArg(kernel_normalize, 1, sizeof(cl_int), (void *)&n));
    config_workgroup_size(global_wg, local_wg, n, n);

    CL_CHECK(clEnqueueNDRangeKernel(cmd_queue, kernel_bits_rev, 2, NULL, global_wg, local_wg, 0, NULL, &fft_event->kernel_bitsrev));

    config_workgroup_size(global_wg, local_wg, n/2, n);

    for (iter = 1; iter <= m; iter++) {
         CL_CHECK(clSetKernelArg(kernel_butterfly_op, 4, sizeof(cl_int), (void *)&iter));
         CL_CHECK(clEnqueueNDRangeKernel(cmd_queue, kernel_butterfly_op, 2, NULL, global_wg, local_wg, 0, NULL, &kernel_butter_events[butter_event_it]));
         butter_event_it++;
    }

    fft_event->kernel_normalize = NULL;

    if (direcao == inversa) {
        config_workgroup_size(global_wg, local_wg, n, n);
         CL_CHECK(clEnqueueNDRangeKernel(cmd_queue, kernel_normalize, 2, NULL, global_wg, local_wg, 0, NULL, &fft_event->kernel_normalize));
    }

    clReleaseKernel(kernel_bits_rev);
    clReleaseKernel(kernel_butterfly_op);
    clReleaseKernel(kernel_normalize);

    return 0;
}

int main(int argc, char *argv[])
{
    //FILE *fp;

    cl_platform_id      platform_id[2];
    cl_uint             ret_num_devices;
    cl_uint             ret_num_platforms;
    cl_int              ret_code;

    cl_mem              image_in_mem = NULL;
    cl_mem              image_out_mem = NULL;
    cl_mem              twiddle_factors_mem = NULL;
    cl_float2           *image_in_host;
    cl_float2           *twiddle_factors_host;

    cl_kernel           kernel_twiddle_factors;
    cl_kernel           kernel_matriz_transpose;
    cl_kernel           kernel_lowpass_filter;

    pgm_t ipgm;
    pgm_t opgm;

    image_file_t        *image_filename;
    char                *output_filename;
    FILE                *fp;
    const char          *kernel_filename = C_NOME_ARQ_KERNEL;
    size_t              source_size;
    char                *source_str;
    cl_int              i, j,n ,m;
    cl_int              raio = 0;
    size_t              global_wg[2];
    size_t              local_wg[2];
    float               *image_amplitudes;
    size_t              log_size;
    char                *log_file;

    cl_event            kernels_events_out_fft[4];

    cl_ulong            kernel_runtime              = (cl_ulong) 0;
    cl_ulong            kernel_start_time           = (cl_ulong) 0;
    cl_ulong            kernel_end_time             = (cl_ulong) 0;

    cl_event            write_host_dev_event;
    cl_ulong            write_host_dev_start_time   = (cl_ulong) 0;
    cl_ulong            write_host_dev_end_time     = (cl_ulong) 0;
    cl_ulong            write_host_dev_run_time     = (cl_ulong) 0;

    cl_event            read_dev_host_event;
    cl_ulong            read_dev_host_start_time    = (cl_ulong) 0;
    cl_ulong            read_dev_host_end_time      = (cl_ulong) 0;
    cl_ulong            read_dev_host_run_time      = (cl_ulong) 0;

    unsigned __int64    image_tam;
    unsigned __int64    MEGA_BYTES   =  1048576; // 1024*1024
    double              image_tam_MB;
    double              tempo_total;

    struct event_in_fft_t *fft_events;


   //=== Timer count start ==============================================================================
    timer_reset();
    timer_start();
    //===================================================================================================

    if (argc < 2) {
        printf("**Erro: O arquivo de entrada eh necessario.\n");
        exit(EXIT_FAILURE);
    }

    image_filename = (image_file_t *) malloc(sizeof(image_file_t));
    split_image_filename(image_filename, argv[1]);
    output_filename = (char *) malloc(40*sizeof(char));
    sprintf(output_filename, "%d.%d.%s.%s.%s", image_filename->res, image_filename->num, ENV_TYPE, APP_TYPE, EXTENSAO);

    fp = fopen(kernel_filename, "r");
    if (!fp) {
        fprintf(stderr, "Failed to load kernel.\n");
        exit(EXIT_FAILURE);
    }

    source_str = (char *)malloc(MAX_SOURCE_SIZE);
    source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose( fp );

    //===================================================================================================
     /* Abrindo imagem do arquivo para objeto de memoria local*/
    if( ler_pgm(&ipgm, argv[1]) == -1)
        exit(EXIT_FAILURE);

    n = ipgm.width;
    raio = n/8;
    m = (cl_int)(log((double)n)/log(2.0));

    image_in_host = (cl_float2 *)malloc(n * n * sizeof(cl_float2));
    twiddle_factors_host = (cl_float2 *)malloc(n / 2 * sizeof(cl_float2));

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            image_in_host[n*i + j].s[0] = (float)ipgm.buf[n*i + j];
            image_in_host[n*i + j].s[1] = (float)0;
        }
    }

    fft_events = (struct event_in_fft_t *)malloc(MAX_CALL_FFT*sizeof(struct event_in_fft_t));

    kernel_butter_events = (cl_event *)malloc(MAX_CALL_FFT*m*sizeof(cl_event));

    //===================================================================================================
    CL_CHECK(clGetPlatformIDs(MAX_PLATFORM_ID, platform_id, &ret_num_platforms));

    if (ret_num_platforms == 0 ) {
        fprintf(stderr,"[Erro] Não existem plataformas OpenCL\n");
        exit(2);
    }

    //===================================================================================================

    CL_CHECK(clGetDeviceIDs( platform_id[1], CL_DEVICE_TYPE_GPU, 1, &device_id, &ret_num_devices));
    //print_platform_info(&platform_id[1]);

    //===================================================================================================
    context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret_code);
    //===================================================================================================

    cmd_queue = clCreateCommandQueue(context, device_id, CL_QUEUE_PROFILING_ENABLE, &ret_code);
    //===================================================================================================

    image_in_mem = clCreateBuffer(context, CL_MEM_READ_WRITE, n*n*sizeof(cl_float2), NULL, &ret_code);
    image_out_mem = clCreateBuffer(context, CL_MEM_READ_WRITE, n*n*sizeof(cl_float2), NULL, &ret_code);
    twiddle_factors_mem = clCreateBuffer(context, CL_MEM_READ_WRITE, (n/2)*sizeof(cl_float2), NULL, &ret_code);
    //===================================================================================================

    /* Transfer data to memory buffer */
    CL_CHECK(clEnqueueWriteBuffer(cmd_queue, image_in_mem, CL_TRUE, 0, n*n*sizeof(cl_float2), image_in_host, 0, NULL, &write_host_dev_event));

    image_tam = n*n*sizeof(cl_float2);

    //===================================================================================================
    program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret_code);
    //===================================================================================================
    ret_code = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
    //===================================================================================================
    if (ret_code != CL_SUCCESS) {
    // Determine the size of the log
    clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
    //===================================================================================================

    // Allocate memory for the log
    log_file = (char *) malloc(log_size);

    // Get the log
    clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, log_size, log_file, NULL);
    printf("%s\n", log_file);
    system("pause");
    exit(0);
}
    kernel_twiddle_factors = clCreateKernel(program, "twiddle_factors", &ret_code);
    kernel_matriz_transpose = clCreateKernel(program, "matrix_trasponse", &ret_code);
    kernel_lowpass_filter  = clCreateKernel(program, "lowpass_filter", &ret_code);

    /* Processa os fatores Wn*/
    //===================================================================================================
    CL_CHECK(clSetKernelArg(kernel_twiddle_factors, 0, sizeof(cl_mem), (void *)&twiddle_factors_mem));
    CL_CHECK(clSetKernelArg(kernel_twiddle_factors, 1, sizeof(cl_int), (void *)&n));
    config_workgroup_size(global_wg, local_wg, n/2, 1);
    CL_CHECK(clEnqueueNDRangeKernel(cmd_queue, kernel_twiddle_factors, 1, NULL, global_wg, local_wg, 0, NULL, &kernels_events_out_fft[0]));

    //===================================================================================================
    /* Executa a FFT em N/2 */
    fft_main(image_out_mem, image_in_mem, twiddle_factors_mem, m, direta, &fft_events[0]);

    //===================================================================================================
    /* Realiza a transposta da Matriz (imagem) */
    CL_CHECK(clSetKernelArg(kernel_matriz_transpose, 0, sizeof(cl_mem), (void *)&image_in_mem));
    CL_CHECK(clSetKernelArg(kernel_matriz_transpose, 1, sizeof(cl_mem), (void *)&image_out_mem));
    CL_CHECK(clSetKernelArg(kernel_matriz_transpose, 2, sizeof(cl_int), (void *)&n));
    config_workgroup_size(global_wg, local_wg, n, n);
    CL_CHECK(clEnqueueNDRangeKernel(cmd_queue, kernel_matriz_transpose, 2, NULL, global_wg, local_wg, 0, NULL, &kernels_events_out_fft[1]));

    //===================================================================================================
    /* Executa a FFT N/2 */
    fft_main(image_out_mem, image_in_mem, twiddle_factors_mem, m, direta, &fft_events[1]);

    //===================================================================================================
    /* Processa o filtro passa baixa */
    CL_CHECK(clSetKernelArg(kernel_lowpass_filter, 0, sizeof(cl_mem), (void *)&image_out_mem));
    CL_CHECK(clSetKernelArg(kernel_lowpass_filter, 1, sizeof(cl_int), (void *)&n));
    CL_CHECK(clSetKernelArg(kernel_lowpass_filter, 2, sizeof(cl_int), (void *)&raio));
    config_workgroup_size(global_wg, local_wg, n, n);
    CL_CHECK(clEnqueueNDRangeKernel(cmd_queue, kernel_lowpass_filter, 2, NULL, global_wg, local_wg, 0, NULL, &kernels_events_out_fft[2]));

    //===================================================================================================
    /* Obtem a FFT inversa*/
    fft_main(image_in_mem, image_out_mem, twiddle_factors_mem, m, inversa, &fft_events[2]);
    //===================================================================================================

    /* Realiza a transposta da Matriz (imagem) */
    CL_CHECK(clSetKernelArg(kernel_matriz_transpose, 0, sizeof(cl_mem), (void *)&image_out_mem));
    CL_CHECK(clSetKernelArg(kernel_matriz_transpose, 1, sizeof(cl_mem), (void *)&image_in_mem));
    CL_CHECK(clSetKernelArg(kernel_matriz_transpose, 2, sizeof(cl_int), (void *)&n));
    config_workgroup_size(global_wg, local_wg, n, n);
    CL_CHECK(clEnqueueNDRangeKernel(cmd_queue, kernel_matriz_transpose, 2, NULL, global_wg, local_wg, 0, NULL, &kernels_events_out_fft[3]));

    //===================================================================================================
    fft_main(image_in_mem, image_out_mem, twiddle_factors_mem, m, inversa, &fft_events[3]);
    //===================================================================================================

    CL_CHECK(clEnqueueReadBuffer(cmd_queue, image_in_mem, CL_TRUE, 0, n*n*sizeof(cl_float2), image_in_host, 0, NULL, &read_dev_host_event));
    //===================================================================================================

    //== Total time elapsed ============================================================================
    timer_stop();
    tempo_total = get_elapsed_time();
    //==================================================================================================

    //====== Get time of Profile Info ==================================================================
    // Write data time
    CL_CHECK(clGetEventProfilingInfo(write_host_dev_event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &write_host_dev_start_time, NULL));
    CL_CHECK(clGetEventProfilingInfo(write_host_dev_event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &write_host_dev_end_time, NULL));
    // Read data time
    CL_CHECK(clGetEventProfilingInfo(read_dev_host_event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &read_dev_host_start_time, NULL));
    CL_CHECK(clGetEventProfilingInfo(read_dev_host_event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &read_dev_host_end_time, NULL));

    for (i = 0; i < MAX_CALL_FFT; i++) {

        kernel_start_time = (cl_long) 0;
        kernel_end_time = (cl_long) 0;
        CL_CHECK(clGetEventProfilingInfo(kernels_events_out_fft[i], CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &kernel_start_time, NULL));
        CL_CHECK(clGetEventProfilingInfo(kernels_events_out_fft[i], CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &kernel_end_time, NULL));
        kernel_runtime += (kernel_end_time - kernel_start_time);

        kernel_start_time = (cl_long) 0;
        kernel_end_time = (cl_long) 0;
        CL_CHECK(clGetEventProfilingInfo(fft_events[i].kernel_bitsrev, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &kernel_start_time, NULL));
        CL_CHECK(clGetEventProfilingInfo(fft_events[i].kernel_bitsrev, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &kernel_end_time, NULL));
        kernel_runtime += (kernel_end_time - kernel_start_time);

        kernel_start_time = (cl_long) 0;
        kernel_end_time = (cl_long) 0;

        if (fft_events[i].kernel_normalize != NULL) {
            CL_CHECK(clGetEventProfilingInfo(fft_events[i].kernel_normalize, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &kernel_start_time, NULL));
            CL_CHECK(clGetEventProfilingInfo(fft_events[i].kernel_normalize, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &kernel_end_time, NULL));
            kernel_runtime += (kernel_end_time - kernel_start_time);
        }
   }

    for (j=0; j < MAX_CALL_FFT*m; j++){
        kernel_start_time = (cl_long) 0;
        kernel_end_time = (cl_long) 0;

        CL_CHECK(clGetEventProfilingInfo(kernel_butter_events[j], CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &kernel_start_time, NULL));
        CL_CHECK(clGetEventProfilingInfo(kernel_butter_events[j], CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &kernel_end_time, NULL));
        kernel_runtime += (kernel_end_time - kernel_start_time);
    }

    write_host_dev_run_time = write_host_dev_end_time - write_host_dev_start_time;
    read_dev_host_run_time =  read_dev_host_end_time -  read_dev_host_start_time;

/* save_log_debug(write_host_dev_run_time,fp);
    save_log_debug(read_dev_host_run_time,fp);
    close_log_debug(fp); */

    image_tam_MB = (double) (((double) image_tam)/(double) MEGA_BYTES);

    //==================================================================================================
    save_log_gpu(image_filename, kernel_runtime, (double) (image_tam_MB/( (double) read_dev_host_run_time/(double) NANOSECONDS)),
    (double) (image_tam_MB/ ((double) write_host_dev_run_time/ (double) NANOSECONDS)), tempo_total, LOG_NAME);

    //===================================================================================================
    image_amplitudes = (float*)malloc(n*n*sizeof(float));
    for (i=0; i < n; i++) {
        for (j=0; j < n; j++) {
            image_amplitudes[n*j + i] = (float) (AMP(((float*)image_in_host)[(2*n*j)+2*i], ((float*)image_in_host)[(2*n*j)+2*i+1]));
        }
    }

    //clFlush(cmd_queue);
    //clFinish(cmd_queue);
    opgm.width = n;
    opgm.height = n;

    normalizar_pgm(&opgm, image_amplitudes);
    escrever_pgm(&opgm, output_filename);

    //===================================================================================================
	clFinish(cmd_queue);
    clReleaseKernel(kernel_twiddle_factors);
    clReleaseKernel(kernel_matriz_transpose);
    clReleaseKernel(kernel_lowpass_filter);
    clReleaseProgram(program);
    clReleaseMemObject(image_in_mem);
    clReleaseMemObject(image_out_mem);
    clReleaseMemObject(twiddle_factors_mem);
    clReleaseCommandQueue(cmd_queue);
    clReleaseContext(context);
	clReleaseEvent(read_dev_host_event);
	clReleaseEvent(write_host_dev_event);
	clReleaseEvent(kernels_events_out_fft[0]);
	clReleaseEvent(kernels_events_out_fft[1]);
	clReleaseEvent(kernels_events_out_fft[2]);
	clReleaseEvent(kernels_events_out_fft[3]);
    destruir_pgm(&ipgm);
    destruir_pgm(&opgm);
    free(image_amplitudes);
    free(source_str);
    free(image_in_host);
    free(image_filename);
    free(twiddle_factors_host);
    free(output_filename);
    free(fft_events);
    free(kernel_butter_events);

    //_CrtDumpMemoryLeaks();

    return 0;
}
