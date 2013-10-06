#define _CRT_SECURE_NO_DEPRECATE
/*#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
*/
#include <CL/cl.h>
#include "cl_utils.h"
#include "utils.h"
#include "pgm.h"

#define USE_NVIDIA_CARD         1
#define MAX_PLATFORM_ID         2
#define MAX_SOURCE_SIZE         (0x100000)

#define EXTENSAO                "pgm"
#define APP_TYPE                "sobel"
#define LOG_NAME                "global.log"
#define C_NOME_ARQ_KERNEL       "../../../utils/kernels/sobel_kernel.cl"
#define ENV_TYPE                "GPU"

int main(int argc, char *argv[])
{
    /* Variaveis obrigatorias do openCL pdccpk*/
    cl_platform_id          platform_ids[2];
    cl_device_id            device_id;
    cl_context              context;
    cl_command_queue        commands;
    cl_program              program;
    cl_kernel               kernel_sobel;

    cl_int                  ret_code;
    cl_uint                 ret_num_devices;
    cl_uint                 ret_num_platforms;
    //
    cl_event                kernel_event;
    cl_ulong                kernel_start_time   = (cl_ulong) 0;
    cl_ulong                kernel_end_time     = (cl_ulong) 0;
    cl_ulong                kernel_run_time     = (cl_ulong) 0;

    cl_event                write_host_dev_event;
    cl_ulong                write_host_dev_start_time   = (cl_ulong) 0;
    cl_ulong                write_host_dev_end_time     = (cl_ulong) 0;
    cl_ulong                write_host_dev_run_time     = (cl_ulong) 0;

    cl_event                read_dev_host_event;
    cl_ulong                read_dev_host_start_time    = (cl_ulong) 0;
    cl_ulong                read_dev_host_end_time      = (cl_ulong) 0;
    cl_ulong                read_dev_host_run_time      = (cl_ulong) 0;

    unsigned __int64        image_tam;
    const unsigned __int64  MEGA_BYTES   =  1048576; // 1024*1024
    double                  image_tam_MB;
    double                  tempo_total;

    /* objetos que serao armazenados na memoria da GPU */
    cl_mem                  image_in_mem, image_out_mem;
    /* objetos que serao armazenados na memoria local (host) */
    unsigned char           *image_in_host, *image_out_host;
    unsigned  int           image_width, image_height;
    size_t                  image_size;
    /*IMPORTANTE: dimensionamento dos compute units para exec do kernel*/
    size_t                  work_global[C_NUM_DIMENSOES];
    size_t                  work_local[C_NUM_DIMENSOES];
    /*Setup dos nomes de arquivos*/
    const char              *kernel_filename = C_NOME_ARQ_KERNEL;
    pgm_t                   ipgm, opgm;
    /* Codigo fonte do kernel dever ser aberto como uma cadeia de caracteres*/
    image_file_t* image_filename;
    char* output_filename;
    FILE                    *fp;
    size_t                  source_size;
    char                    *source_str;

    /* Timer count start */
    timer_reset();
    timer_start();

    if (argc < 2) {
        printf("**Erro: A imagem de entrada é necessaria.\n");
        exit(EXIT_FAILURE);
    }

    //===================================================================================================
    image_filename = (image_file_t *) malloc(sizeof(image_file_t));
    split_image_filename(image_filename, argv[1]);
    output_filename = (char *) malloc(40*sizeof(char));
    sprintf(output_filename, "%d.%d.%s.%s.%s", image_filename->res, image_filename->num, ENV_TYPE, APP_TYPE, EXTENSAO);
    //===================================================================================================

    fp = fopen(kernel_filename, "r");
    if (!fp) {
        fprintf(stderr, "Failed to load kernel.\n");
        exit(1);
    }

    source_str  = (char *) malloc(MAX_SOURCE_SIZE);
    source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose(fp);

    //===================================================================================================
    // Abrindo imagem do arquivo para objeto de memoria local
    if( ler_pgm(&ipgm, argv[1]) == -1)
        exit(EXIT_FAILURE);

    image_in_host = ipgm.buf;
    image_width  = ipgm.width;
    image_height = ipgm.height;

    image_size =  (int) (image_width * image_height) * sizeof(unsigned char);

    image_tam = image_size;

    /* Alocando memoria para a imagem de saida apos o processamento*/
    image_out_host = (unsigned char *) malloc(image_size);
    //===================================================================================================

    /* Recebe um vetor de platform_id e retorna sucesso
     * se encontrar plataformas OpenCL no sistema, inseridos
     * essas plataformas no vetor com no maximo MAX_PLATFORM_ID
     * entradas, caso contrario retorna codigo de erro.
     * CL_CHECK é um macro para retornar o titulo do erro
     * a partir de uma funcao que retorne um codigo de erro
     ***************************************************/

    CL_CHECK(clGetPlatformIDs(MAX_PLATFORM_ID, platform_ids, &ret_num_platforms));

    if (ret_num_platforms == 0) {
        fprintf(stderr, "[Erro] Não existem plataformas OpenCL\n");
        exit(2);
    }
    //===================================================================================================

    /* Recebe uma platform_id e retorna sucesso
     * se obter um device do tipo GPU dessa plataforma OpenCL
     * caso contrario retorna codigo de erro.
     ***************************************************/

    CL_CHECK(clGetDeviceIDs(platform_ids[1], CL_DEVICE_TYPE_GPU, 1, &device_id, &ret_num_devices));
    //print_platform_info(&platform_ids[0]);
    //system("pause");
    //exit(0);
    //===================================================================================================

    /* Retorna sucesso se consegui criar um contexto para
     * o device id escolhido, caso contrario retorna codigo de erro.
     ***************************************************/
    context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret_code);
    //===================================================================================================

    commands = clCreateCommandQueue(context, device_id, CL_QUEUE_PROFILING_ENABLE, &ret_code);
    //===================================================================================================

    program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, NULL);
    //===================================================================================================

    ret_code = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);

    if (ret_code != CL_SUCCESS) {
        char build_str[4096];
        fprintf(stderr, "[ERRO] clBuildProgram '%s' (code: %d)\n",
                error_cl_str(ret_code), ret_code );
        clGetProgramBuildInfo( program, device_id,
                               CL_PROGRAM_BUILD_LOG, sizeof(build_str), build_str, NULL);
        fprintf(stderr, "[ERRO] log: '%s'\n", build_str);
        system("pause");
        exit(4);
    }
    //===================================================================================================

    kernel_sobel = clCreateKernel(program, "sobel_kernel", NULL);
    image_in_mem  = clCreateBuffer(context, CL_MEM_READ_ONLY, image_size, NULL, NULL);
    image_out_mem = clCreateBuffer(context, CL_MEM_WRITE_ONLY, image_size , NULL, NULL);
    //===================================================================================================

    CL_CHECK(clEnqueueWriteBuffer(commands, image_in_mem, CL_TRUE, 0, image_size, image_in_host, 0, NULL, &write_host_dev_event));
    CL_CHECK(clSetKernelArg(kernel_sobel, 0, sizeof(cl_mem), &image_in_mem));
    CL_CHECK(clSetKernelArg(kernel_sobel, 1, sizeof(cl_mem), &image_out_mem));
    //===================================================================================================

    work_global[0] = image_width;
    work_global[1] = image_height;
    work_local[0] = 32;
    work_local[1] = 32;
    //===================================================================================================

    CL_CHECK(clEnqueueNDRangeKernel(commands, kernel_sobel, 2, NULL, work_global, work_local, 0, NULL,  &kernel_event) );
    // CL_CHECK(clFinish(commands));
    // CL_CHECK( clWaitForEvents(1 , &kernel_event) );
    //===================================================================================================

    CL_CHECK(clEnqueueReadBuffer(commands, image_out_mem, CL_TRUE, 0, image_size, image_out_host, 0, NULL, &read_dev_host_event));

    //== Total time elapsed =============================================================================
    timer_stop();
    tempo_total = get_elapsed_time();
    //===================================================================================================
    //====== Get time of Profile Info ===================================================================
    // kernel sobel time
    CL_CHECK(clGetEventProfilingInfo(kernel_event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &kernel_start_time, NULL));
    CL_CHECK(clGetEventProfilingInfo(kernel_event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &kernel_end_time, NULL));
    // Write data time
    CL_CHECK(clGetEventProfilingInfo(write_host_dev_event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &write_host_dev_start_time, NULL));
    CL_CHECK(clGetEventProfilingInfo(write_host_dev_event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &write_host_dev_end_time, NULL));
    // Read data time
    CL_CHECK(clGetEventProfilingInfo(read_dev_host_event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &read_dev_host_start_time, NULL));
    CL_CHECK(clGetEventProfilingInfo(read_dev_host_event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &read_dev_host_end_time, NULL));
    //===================================================================================================

    write_host_dev_run_time = write_host_dev_end_time - write_host_dev_start_time;
    read_dev_host_run_time =  read_dev_host_end_time -  read_dev_host_start_time;
    kernel_run_time = kernel_end_time - kernel_start_time;

    image_tam_MB = (double) (((double) image_tam)/(double) MEGA_BYTES);

    //===================================================================================================
    save_log_gpu(image_filename, kernel_run_time, (double) (image_tam_MB/( (double) read_dev_host_run_time/(double) NANOSECONDS)),
        (double) (image_tam_MB/ ((double) write_host_dev_run_time/ (double) NANOSECONDS)), tempo_total, LOG_NAME);
    //===================================================================================================

    opgm.width  = image_width;
    opgm.height = image_height;
    opgm.buf    = image_out_host;

    escrever_pgm(&opgm, output_filename);

    //===================================================================================================
    CL_CHECK(clReleaseMemObject(image_in_mem));
    CL_CHECK(clReleaseMemObject(image_out_mem));
    CL_CHECK(clReleaseProgram(program));
    CL_CHECK(clReleaseKernel(kernel_sobel));
    CL_CHECK(clReleaseCommandQueue(commands));
    CL_CHECK(clReleaseContext(context));
    destruir_pgm(&ipgm);
    destruir_pgm(&opgm);
    free(source_str);
    free(image_filename);
    free(output_filename);

    //_CrtDumpMemoryLeaks();

    return 0;
}