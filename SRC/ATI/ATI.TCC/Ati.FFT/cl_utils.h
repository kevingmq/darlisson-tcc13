#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <CL/cl.h>
#include <math.h>

#define C_NOME_ARQ_KERNEL	"sobel_kernel.cl"
#define C_NOME_ARQ_IMAGEM_IN	"lena.pgm"
#define C_NOME_ARQ_IMAGEM_OUT	"lena.sobel.pgm"

#define C_NUM_DIMENSOES	2
#define C_NUM_WORK_GROUPS_DIM_0	64
#define C_NUM_WORK_GROUPS_DIM_1	64
#define C_NUM_WORK_ITEMS_DIM_0	32
#define C_NUM_WORK_ITEMS_DIM_1	32

#define CL_CHECK(_expr)\
	do {\
	cl_int _err = _expr;\
	if (_err == CL_SUCCESS)\
	break;\
	fprintf(stderr, "[%d@%s] OpenCL Error: '%s' returned %d(%s)!\n", \
	__LINE__,__FILE__,#_expr, (int)_err,\
	ErroCLtoStr((int)_err));\
	exit(1);\
	} while (0)

char *ErroCLtoStr(cl_int errorCode) {
	switch(errorCode){
	case CL_SUCCESS:
		return "CL_SUCCESS";
	case CL_DEVICE_NOT_FOUND:
		return "CL_DEVICE_NOT_FOUND";
	case CL_DEVICE_NOT_AVAILABLE:
		return "CL_DEVICE_NOT_AVAILABLE";
	case CL_COMPILER_NOT_AVAILABLE:
		return "CL_COMPILER_NOT_AVAILABLE";
	case CL_MEM_OBJECT_ALLOCATION_FAILURE:
		return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
	case CL_OUT_OF_RESOURCES:
		return "CL_OUT_OF_RESOURCES";
	case CL_OUT_OF_HOST_MEMORY:
		return "CL_OUT_OF_HOST_MEMORY";
	case CL_PROFILING_INFO_NOT_AVAILABLE:
		return "CL_PROFILING_INFO_NOT_AVAILABLE";
	case CL_MEM_COPY_OVERLAP:
		return "CL_MEM_COPY_OVERLAP";
	case CL_IMAGE_FORMAT_MISMATCH:
		return "CL_IMAGE_FORMAT_MISMATCH";
	case CL_IMAGE_FORMAT_NOT_SUPPORTED:
		return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
	case CL_BUILD_PROGRAM_FAILURE:
		return "CL_BUILD_PROGRAM_FAILURE";
	case CL_MAP_FAILURE:
		return "CL_MAP_FAILURE";
#ifdef CL_VERSION_1_1
	case CL_MISALIGNED_SUB_BUFFER_OFFSET:
		return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
	case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:
		return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
#endif
	case CL_INVALID_VALUE:
		return "CL_INVALID_VALUE";
	case CL_INVALID_DEVICE_TYPE:
		return "CL_INVALID_DEVICE_TYPE";
	case CL_INVALID_PLATFORM:
		return "CL_INVALID_PLATFORM";
	case CL_INVALID_DEVICE:
		return "CL_INVALID_DEVICE";
	case CL_INVALID_CONTEXT:
		return "CL_INVALID_CONTEXT";
	case CL_INVALID_QUEUE_PROPERTIES:
		return "CL_INVALID_QUEUE_PROPERTIES";
	case CL_INVALID_COMMAND_QUEUE:
		return "CL_INVALID_COMMAND_QUEUE";
	case CL_INVALID_HOST_PTR:
		return "CL_INVALID_HOST_PTR";
	case CL_INVALID_MEM_OBJECT:
		return "CL_INVALID_MEM_OBJECT";
	case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
		return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
	case CL_INVALID_IMAGE_SIZE:
		return "CL_INVALID_IMAGE_SIZE";
	case CL_INVALID_SAMPLER:
		return "CL_INVALID_SAMPLER";
	case CL_INVALID_BINARY:
		return "CL_INVALID_BINARY";
	case CL_INVALID_BUILD_OPTIONS:
		return "CL_INVALID_BUILD_OPTIONS";
	case CL_INVALID_PROGRAM:
		return "CL_INVALID_PROGRAM";
	case CL_INVALID_PROGRAM_EXECUTABLE:
		return "CL_INVALID_PROGRAM_EXECUTABLE";
	case CL_INVALID_KERNEL_NAME:
		return "CL_INVALID_KERNEL_NAME";
	case CL_INVALID_KERNEL_DEFINITION:
		return "CL_INVALID_KERNEL_DEFINITION";
	case CL_INVALID_KERNEL:
		return "CL_INVALID_KERNEL";
	case CL_INVALID_ARG_INDEX:
		return "CL_INVALID_ARG_INDEX";
	case CL_INVALID_ARG_VALUE:
		return "CL_INVALID_ARG_VALUE";
	case CL_INVALID_ARG_SIZE:
		return "CL_INVALID_ARG_SIZE";
	case CL_INVALID_KERNEL_ARGS:
		return "CL_INVALID_KERNEL_ARGS";
	case CL_INVALID_WORK_DIMENSION:
		return "CL_INVALID_WORK_DIMENSION";
	case CL_INVALID_WORK_GROUP_SIZE:
		return "CL_INVALID_WORK_GROUP_SIZE";
	case CL_INVALID_WORK_ITEM_SIZE:
		return "CL_INVALID_WORK_ITEM_SIZE";
	case CL_INVALID_GLOBAL_OFFSET:
		return "CL_INVALID_GLOBAL_OFFSET";
	case CL_INVALID_EVENT_WAIT_LIST:
		return "CL_INVALID_EVENT_WAIT_LIST";
	case CL_INVALID_EVENT:
		return "CL_INVALID_EVENT";
	case CL_INVALID_OPERATION:
		return "CL_INVALID_OPERATION";
	case CL_INVALID_GL_OBJECT:
		return "CL_INVALID_GL_OBJECT";
	case CL_INVALID_BUFFER_SIZE:
		return "CL_INVALID_BUFFER_SIZE";
	case CL_INVALID_MIP_LEVEL:
		return "CL_INVALID_MIP_LEVEL";
	case CL_INVALID_GLOBAL_WORK_SIZE:
		return "CL_INVALID_GLOBAL_WORK_SIZE";
	default:
		return "Unknow code";
	}
}

unsigned char* LerPGM(char* nomeFich, int* largura, int* altura){
	char Linha[4096];
	int x, y, Idx;
	int w, h, Value;
	size_t TamImagem;
	unsigned char *ImagemPtr;
	FILE *f;

	// Abre o arquivo PGM
	printf("[Info] Abrindo o ficheiro PGM '%s'\n", nomeFich);
	f = fopen(nomeFich, "rt");
	if(f == NULL){
		fprintf(stderr,"[Erro]:impossivel abrir '%s'\n", nomeFich);
		exit(1);
	}

	// Leitura das 4 linhas do cabecalho. Apenas são considerados
	// a largura e a altura
	fscanf(f, "%[^\n]\n", Linha);
	fscanf(f, "%[^\n]\n", Linha);
	fscanf(f, "%d %d\n", &w, &h);
	fscanf(f, "%[^\n]\n", Linha);

	printf("[Info] Leitura de (%dx%d) do ficheiro '%s'\n", w, h, nomeFich);

	// Aloca a memória para a imagem
	TamImagem = w*h*sizeof(unsigned char);
	ImagemPtr = malloc(TamImagem);
	if(ImagemPtr == NULL){
		fprintf(stderr,"[Erro]:impossivel alocar memória"
			"(%dx%d)=%zu bytes\n",w,h,TamImagem);
		exit(2);
	}

	Idx = 0;
	y = 0;
	while(y < h){
		x = 0;
		while(x < w){
			fscanf(f, "%d", &Value);
			ImagemPtr[Idx] = (unsigned char)Value;
			Idx++;
			x = x + 1;
		}
		y = y + 1;
	}

	printf("[Info] %d elementos lidos do ficheiro '%s'\n", Idx, nomeFich);
	fclose(f);

	*largura  = w;
	*altura = h;

	return ImagemPtr;
}

void GravarPGM(char* nomeFich, int largura, int altura, 
	unsigned char imagemPtr[]){
		FILE *f;
		int x, y, Idx;

		// abre arquivo para escrita
		f = fopen(nomeFich, "wt");
		if(f == NULL){
			fprintf(stderr,"[Erro]:impossivel criar ficheiro '%s'\n", nomeFich);
			exit(1);
		}

		// escreve cabecalho do arquiovo
		fprintf(f, "P2\n");
		fprintf(f, "# My PGM file\n");
		fprintf(f, "%d %d\n", largura, altura);
		fprintf(f, "255\n");

		// Escreve píxeis para o arquivo
		Idx = 0;
		y = 0;
		while(y < altura){
			x = 0;
			while(x < largura) {
				fprintf(f, "%d ", (int)imagemPtr[Idx]);
				Idx++;
				x = x + 1;
			}
			fprintf(f, "\n");
			y = y + 1;
		}

		// Close file
		printf("[Info] %d elementos escritos para o ficheiro '%s'\n",Idx,nomeFich);
		fclose(f);
}


void* MallocImage(int largura, int altura){
	size_t TamImagem = largura*altura;
	unsigned char* ImagePtr = malloc(TamImagem);
	if(ImagePtr == NULL){
		fprintf(stderr,"[Erro]:impossivel alocar memoria para imagem "
			"(%dx%d)=%zu bytes\n",largura,altura,TamImagem);
		exit(2);
	}
	return ImagePtr;
}

char **FichParaVetorStr(const char* nomeFich, int *numElms){
#define MAX_SIZE	(1024)	
	char Linha_S[1024];
	char *TodasStrs, **VecStr;
	FILE *F;
	int NumLinhas, LinhaCorrente;
	F = fopen(nomeFich,"rt");
	if(F==NULL){
		fprintf(stderr,"Impossivel abrir '%s'.\n",nomeFich);
		exit(3);
	}
	NumLinhas=0;
	while(!feof(F)){
		if( fgets(Linha_S, sizeof(Linha_S), F) == NULL){
			break;
		}
		NumLinhas++;
	}

	/* DEBUG */
	printf("[Info] ficheiro kernel '%s' (%d linhas)\n",nomeFich,NumLinhas);

	/* Aloca memória para o vetor de strings */
	VecStr = (char**) malloc(sizeof(char*)*NumLinhas);
	/* Alloca bloco de memória para todas as strings */
	TodasStrs = (char*) malloc(sizeof(char)*MAX_SIZE*NumLinhas);
	rewind(F);
	LinhaCorrente = 0;
	while(!feof(F)){
		if( fgets(Linha_S, sizeof(Linha_S), F) == NULL){
			break;
		}

		VecStr[LinhaCorrente] = TodasStrs+LinhaCorrente*MAX_SIZE;
		strncpy(VecStr[LinhaCorrente],Linha_S,MAX_SIZE);
		LinhaCorrente++;
	}
	fclose(F);

	*numElms = NumLinhas;
	return VecStr;
}