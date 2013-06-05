#define _CRT_SECURE_NO_DEPRECATE
#define CL_VERSION_1_1
#include <stdio.h>   
#include <stdlib.h>  
#include <CL/cl.h>  

#include "cl_utils.h"
#include "pgm.h"
  
#define MAX_SOURCE_SIZE 		(0x100000) 
#define C_NOME_ARQ_KERNEL		"sobel_kernel.cl"
#define C_NOME_ARQ_IMAGEM_IN	"2048_image8.pgm"
#define C_NOME_ARQ_IMAGEM_OUT	"2048_image8.sobel.pgm"
#define MAX_PLATFORM_ID			2
int main(void){
	
	/* Variaveis obrigatorias do openCL*/
	
	cl_platform_id platform_ids[3];
	cl_device_id device_id;
	cl_context context;
	cl_command_queue queue;
	cl_program program;
	cl_kernel sobel_kernel;
	
	cl_int ret_code;
	cl_uint ret_num_devices;   
	cl_uint ret_num_platforms;
	
	/* objetos que serao armazenados na memoria da GPU */
	cl_mem imageIn_cl_mem, imageOut_cl_mem;	
	
	/* objetos que serao armazenados na memoria local (host) */
	
	unsigned char *imageIn_host, *imageOut_host;
	int image_width, image_height;
	size_t image_size;
	
	/*IMPORTANTE: dimensionamento dos compute units para exec do kernel*/
	size_t WorkTotal[C_NUM_DIMENSOES] = {C_NUM_WORK_GROUPS_DIM_0}; 
	
	/*Setup dos nomes de arquivos*/
	
	char *kernel_file_name = C_NOME_ARQ_KERNEL;
	char *imageIn_file_name = C_NOME_ARQ_IMAGEM_IN;
	char *imageOut_file_name = C_NOME_ARQ_IMAGEM_OUT;
	pgm_t ipgm;
	pgm_t opgm;
	
	/* Codigo fonte do kernel dever ser aberto como uma cadeia de caracteres*/
	
	FILE *fp;  
	size_t source_size;
	char *source_str;  
	fp = fopen(kernel_file_name, "r"); 
	if (!fp) { 
		fprintf(stderr, "Failed to load kernel.\n");   
		exit(1);   
	}  
	source_str = (char *)malloc(MAX_SOURCE_SIZE);  
	source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);   
	fclose( fp );  
	
	/* Abrindo imagem do arquivo para objeto de memoria local*/
	
	readPGM(&ipgm, imageIn_file_name);
	imageIn_host = ipgm.buf;
	image_width = ipgm.width;
	image_height = ipgm.height;

	image_size = image_width * image_height * sizeof(unsigned char);
	
	/* Alocando memoria para a imagem de saida apos o processamento*/
	imageOut_host = malloc(image_size);
		
	/* Recebe um vetor de platform_id e retorna sucesso
	 * se encontrar plataformas OpenCL no sistema, inseridos
	 * essas plataformas no vetor com no maximo MAX_PLATFORM_ID
	 * entradas, caso contrario retorna codigo de erro.  
	 * CL_CHECK é um macro para retornar o titulo do erro
	 * a partir de uma funcao que retorne um codigo de erro
	 ***************************************************/	
	 
	CL_CHECK(clGetPlatformIDs(MAX_PLATFORM_ID,platform_ids, &ret_num_platforms));
	
	if( ret_num_platforms == 0 ){
		fprintf(stderr,"[Erro] Não existem plataformas OpenCL\n");
		exit(2);
	}
	
	/* Recebe uma platform_id e retorna sucesso
	 * se obter um device do tipo GPU dessa plataforma OpenCL
	 * caso contrario retorna codigo de erro.  
	 ***************************************************/	
	 
	CL_CHECK(clGetDeviceIDs(platform_ids[0], CL_DEVICE_TYPE_GPU, 1, &device_id, &ret_num_devices));
	
	/* Retorna sucesso se consegui criar um contexto para
	 * o device id escolhido, caso contrario retorna codigo de erro.  
	 ***************************************************/	
	context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret_code);
	
	queue = clCreateCommandQueue(context, device_id, 0, &ret_code);
	
	program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, NULL);
	
	ret_code = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
	
	if( ret_code != CL_SUCCESS){
		char Build_S[4096];
		fprintf(stderr, "[ERRO] clBuildProgram '%s' (code: %d)\n", 
			ErroCLtoStr(ret_code), ret_code );
		// solicita log da compilação
		clGetProgramBuildInfo( program, device_id, 
			CL_PROGRAM_BUILD_LOG, sizeof(Build_S), Build_S, NULL);
		fprintf(stderr, "[ERRO] log: '%s'\n", Build_S);
		
		exit(4);
	}
	
	sobel_kernel = clCreateKernel(program,"sobel_kernel",NULL);
	
	imageIn_cl_mem = clCreateBuffer(context,CL_MEM_READ_ONLY, image_size, NULL,NULL);
	
	imageOut_cl_mem = clCreateBuffer(context, CL_MEM_WRITE_ONLY, image_size ,NULL,NULL);
	
	CL_CHECK(clEnqueueWriteBuffer(queue, imageIn_cl_mem, CL_TRUE, 0, image_size, imageIn_host, 0, NULL, NULL));
	
	CL_CHECK( clSetKernelArg(sobel_kernel, 0, sizeof(cl_mem), &imageIn_cl_mem));
	
	CL_CHECK( clSetKernelArg(sobel_kernel, 1, sizeof(cl_mem), &imageOut_cl_mem));
	
	CL_CHECK( clSetKernelArg(sobel_kernel, 2, sizeof(cl_int), &image_width));
	
	clEnqueueNDRangeKernel(queue, sobel_kernel, 1, NULL, WorkTotal, NULL, 0, NULL, NULL);
	
	CL_CHECK( clFinish(queue));
	
	clEnqueueReadBuffer(queue, imageOut_cl_mem, CL_TRUE, 0, image_size, imageOut_host, 0, NULL, NULL);
	
	opgm.width = image_width;
	opgm.height = image_height; 
	opgm.buf = imageOut_host;
	
	writePGM(&opgm, imageOut_file_name); 
	
	CL_CHECK( clReleaseMemObject(imageIn_cl_mem) );
	CL_CHECK( clReleaseMemObject(imageOut_cl_mem) );
	CL_CHECK( clReleaseProgram(program) );
	CL_CHECK( clReleaseKernel(sobel_kernel) );
	CL_CHECK( clReleaseCommandQueue(queue) );
	CL_CHECK( clReleaseContext(context) );
	free(source_str);
	free(imageIn_host);
	free(imageOut_host);
	
	return 0;
}