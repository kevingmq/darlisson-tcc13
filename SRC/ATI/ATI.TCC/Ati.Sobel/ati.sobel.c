#include "cl_utils.h"


int main(void){
	cl_context Contexto;
	cl_context_properties Props_V[3];
	cl_kernel kernel;
	cl_command_queue FilaComandos;
	cl_program Programa;

#define MAX_PLATFORMS (5)

	cl_platform_id Pltformas_V[MAX_PLATFORMS];
	cl_uint MaxPltformas = MAX_PLATFORMS;
	cl_uint NumPltformas=0;
	cl_device_id DeviceID;
	cl_uint NumDevices=0;
	cl_mem input, output;	
	unsigned char *ImageIn_h, *ImageOut_h;
	int Largura, Altura;
	size_t TamImagem;
	cl_int RetCode;
	unsigned int i;
	int NumLinhas;
	char **KernelStr;
	size_t WorkTotal[C_NUM_DIMENSOES] = {C_NUM_WORK_GROUPS_DIM_0}; 

	char *KernelNomeFich = C_NOME_ARQ_KERNEL;
	char *InputFName = C_NOME_ARQ_IMAGEM_IN;
	char *OutputFName = C_NOME_ARQ_IMAGEM_OUT;

	// Carrega a imagem para o vetor ImageIn_h
	ImageIn_h = LerPGM(InputFName, &Largura, &Altura);


	// Alloca espaço em memória para guardar imagem invertida
	ImageOut_h = MallocImage(Largura,Altura);

	// Obtém lista de plataformas OpenCL
	CL_CHECK(clGetPlatformIDs(MaxPltformas,Pltformas_V,&NumPltformas));
	if( NumPltformas == 0 ){
		fprintf(stderr,"[Erro] Não existem plataformas OpenCL\n");
		exit(2);
	}

	// Mostra informação a respeito das plataformas OpenCL encontradas
	printf("=== %d plataformas OpenCL ===\n", NumPltformas);
	for (i=0; i<NumPltformas; i++){
		char S[8192];
		printf("  -- %d --\n", i);
		CL_CHECK(clGetPlatformInfo(Pltformas_V[i], 
			CL_PLATFORM_PROFILE, sizeof(S), S, NULL));
		printf("  PROFILE = %s\n", S);
		CL_CHECK(clGetPlatformInfo(Pltformas_V[i], 
			CL_PLATFORM_VERSION, sizeof(S), S, NULL));
		printf("  VERSION = %s\n", S);
		CL_CHECK(clGetPlatformInfo(Pltformas_V[i], 
			CL_PLATFORM_NAME, sizeof(S), S, NULL));
		printf("  NAME = %s\n", S);
		CL_CHECK(clGetPlatformInfo(Pltformas_V[i], 
			CL_PLATFORM_VENDOR, sizeof(S), S, NULL));
		printf("  VENDOR = %s\n", S);
		CL_CHECK(clGetPlatformInfo(Pltformas_V[i], 
			CL_PLATFORM_EXTENSIONS, sizeof(S), S, NULL));
		printf("  EXTENSIONS = %s\n", S);
	}
	printf("====================================\n");
	// Procura dispositivo OpenCL do tipo GPU
	CL_CHECK(clGetDeviceIDs(Pltformas_V[0], CL_DEVICE_TYPE_GPU, 1, 
		&DeviceID, &NumDevices));

	// Vetor com propriedades para criação de contexto 
	// (último elemento deve obrigatoriamente conter 0)
	Props_V[0]= CL_CONTEXT_PLATFORM;
	Props_V[1]= (cl_context_properties) Pltformas_V[1];
	Props_V[2]= 0;

	// Cria um contexto com o dispositivo GPU
	Contexto = clCreateContext(Props_V,1,&DeviceID,NULL,NULL,NULL);

	// Cria fila de espera  
	// create command queue using the context and device
	FilaComandos = clCreateCommandQueue(Contexto, DeviceID, 0, NULL);
	// Cria vetor de string KernelStr contendo o ficheiro do kernel // FIXME - melhorar isso
	KernelStr = FichParaVetorStr(KernelNomeFich,&NumLinhas);

	// Cria o programa a partir do código fonte
	Programa =  clCreateProgramWithSource(Contexto, NumLinhas, (const char**)KernelStr, NULL, NULL);

	// Compila o programa
	RetCode = clBuildProgram(Programa, 0, NULL, NULL, NULL, NULL);
	if( RetCode != CL_SUCCESS){
		char Build_S[4096];
		fprintf(stderr, "[ERRO] clBuildProgram '%s' (code: %d)\n", 
			ErroCLtoStr(RetCode), RetCode );
		// solicita log da compilação
		clGetProgramBuildInfo( Programa, DeviceID, 
			CL_PROGRAM_BUILD_LOG, sizeof(Build_S), Build_S, NULL);
		fprintf(stderr, "[ERRO] log: '%s'\n", Build_S);
		exit(4);
	}

	// Especifica o kernel que deve ser executado
	kernel = clCreateKernel(Programa,"sobel_kernel",NULL);

	// Cria os vetores input (imagem a inverter) e 
	// output (imagem invertida) na memória do dispositivo 
	TamImagem = Largura * Altura * sizeof(unsigned char);
	input = clCreateBuffer(Contexto,CL_MEM_READ_ONLY,TamImagem,NULL,NULL);
	output = clCreateBuffer(Contexto, CL_MEM_WRITE_ONLY,TamImagem,NULL,NULL);

	// Carrega imagem a inverter na memória input
	CL_CHECK(clEnqueueWriteBuffer(FilaComandos, input, 
		CL_TRUE, 0, TamImagem, ImageIn_h, 0, NULL, NULL));

	// Especifica os parâmetros a serem passados ao kernel
	CL_CHECK( clSetKernelArg(kernel, 0, sizeof(cl_mem), &input) );
	CL_CHECK( clSetKernelArg(kernel, 1, sizeof(cl_mem), &output) );
	CL_CHECK( clSetKernelArg(kernel, 2, sizeof(cl_int), &Largura) );
	CL_CHECK( clSetKernelArg(kernel, 3, sizeof(cl_int), &Altura) );

	CL_CHECK( clEnqueueNDRangeKernel(FilaComandos, kernel, 1, NULL, WorkTotal, NULL, 0, NULL, NULL) );

	CL_CHECK( clFinish(FilaComandos) );

	// Copia a imagem invertida da memória do dispositivo para 
	// a memória do programa hospedeiro
	CL_CHECK( clEnqueueReadBuffer(FilaComandos, output, CL_TRUE, 0, TamImagem, ImageOut_h, 0, NULL, NULL) );
	// Grava a imagem invertida para o respetivo ficheiro
	GravarPGM(OutputFName, Largura, Altura, ImageOut_h);
	//system("pause");


	// Liberta os recursos OpenCL
	CL_CHECK( clReleaseMemObject(input) );
	CL_CHECK( clReleaseMemObject(output) );
	CL_CHECK( clReleaseProgram(Programa) );
	CL_CHECK( clReleaseKernel(kernel) );
	CL_CHECK( clReleaseCommandQueue(FilaComandos) );
	CL_CHECK( clReleaseContext(Contexto) );

	// Liberta o vetor de string que continha o código fonte do kernel
	free(KernelStr[0]);
	free(KernelStr);

	// Liberta as zonas de memória 
	free(ImageIn_h);
	free(ImageOut_h);
	system("pause");
	return 0;
}