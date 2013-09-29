package br.edu.uea.utils;

import br.edu.uea.enums.APPLICATION;
import br.edu.uea.enums.GPU_MODEL;

public class FilePathUtil {

	private static final String BASE_PATH 			= "C:/PROJETOS/darlisson-tcc13/RESULTS/";
	private static final String NVIDIA_520_PATH 	= "NVIDIA/";
	private static final String NVIDIA_210_PATH 	= "NVIDIA_2/";
	private static final String ATI_HD_6450_PATH 	= "ATI/";

	private static final String SOBEL_PATH 			= "SOBEL/";
	private static final String FFT_PATH			= "FFT/";

	private static final String NVIDIA_520_CSV_PATH = "NVIDIA_GT_520_csv/"; 
	private static final String NVIDIA_210_CSV_PATH = "NVIDIA_GeForce_210_csv/"; 


	private static final String ATI_IMG_BASE_PATH 	= "image"; 
	private static final String DADOS_CSV_NAME 		= "dados_"; 
	private static final String IMG_SHORT_NAME 		= "_img"; 

	private static final String KERNEL_HTML_EXT 	= "_kernel.html"; 
	private static final String MEMORY_HTML_EXT 	= "_memory.html"; 

	private static final String KERNEL_CSV_EXT 		= "_kernel.csv"; 
	private static final String MEMORY_CSV_EXT 		= "_memory.csv"; 

	public static String getKernelFilename(GPU_MODEL gpuModel,APPLICATION appType,String imgTam, int imgID){

		String kernelFilename = null;

		switch(appType){

		case SOBEL:

			switch(gpuModel){

			case GT_520:

				kernelFilename = BASE_PATH + NVIDIA_520_PATH + SOBEL_PATH + NVIDIA_520_CSV_PATH +
				DADOS_CSV_NAME + imgTam + "/" + imgTam + IMG_SHORT_NAME + imgID + KERNEL_CSV_EXT;
				break;

			case GT_210:
				kernelFilename = BASE_PATH + NVIDIA_210_PATH + SOBEL_PATH + NVIDIA_210_CSV_PATH +
				DADOS_CSV_NAME + imgTam + "/" + imgTam + IMG_SHORT_NAME + imgID + KERNEL_CSV_EXT;
				break;

			case HD_6450:
				kernelFilename = BASE_PATH + ATI_HD_6450_PATH + SOBEL_PATH + imgTam +"/"+ imgTam + "_" + ATI_IMG_BASE_PATH +
				imgID +"/"+ imgTam +IMG_SHORT_NAME +imgID + KERNEL_HTML_EXT;
				break;
			}
			break;

		case FFT:

			switch(gpuModel){

			case GT_520:			

				kernelFilename = BASE_PATH + NVIDIA_520_PATH + FFT_PATH + NVIDIA_520_CSV_PATH +
				DADOS_CSV_NAME + imgTam + "/" + imgTam + IMG_SHORT_NAME + imgID + KERNEL_CSV_EXT;
				break;

			case GT_210:

				kernelFilename = BASE_PATH + NVIDIA_210_PATH + FFT_PATH + NVIDIA_210_CSV_PATH +
				DADOS_CSV_NAME + imgTam + "/" + imgTam + IMG_SHORT_NAME + imgID + KERNEL_CSV_EXT;
				break;

			case HD_6450:
				kernelFilename = BASE_PATH + ATI_HD_6450_PATH + FFT_PATH + imgTam +"/"+ ATI_IMG_BASE_PATH +
				imgID +"/"+ imgTam +IMG_SHORT_NAME +imgID + KERNEL_HTML_EXT;
				
				break;
			}
			break;
		}
		return kernelFilename;
	} 

	public static String getMemoryFilename(GPU_MODEL gpuModel, APPLICATION appType,String imgTam, int imgID){

		String memoryFilename = null;

		switch(appType){

		case SOBEL:

			switch(gpuModel){

			case GT_520:
				memoryFilename = BASE_PATH + NVIDIA_520_PATH + SOBEL_PATH + NVIDIA_520_CSV_PATH +
				DADOS_CSV_NAME + imgTam + "/" + imgTam + IMG_SHORT_NAME + imgID + MEMORY_CSV_EXT;
				break;

			case GT_210:
				memoryFilename = BASE_PATH + NVIDIA_210_PATH + SOBEL_PATH + NVIDIA_210_CSV_PATH +
				DADOS_CSV_NAME + imgTam + "/" + imgTam + IMG_SHORT_NAME + imgID + MEMORY_CSV_EXT;
				break;

			case HD_6450:
				memoryFilename = BASE_PATH + ATI_HD_6450_PATH + SOBEL_PATH + imgTam +"/"+ imgTam + "_" + ATI_IMG_BASE_PATH +
				imgID +"/"+ imgTam +IMG_SHORT_NAME +imgID + MEMORY_HTML_EXT;
				break;
			}
			break;

		case FFT:

			switch(gpuModel){

			case GT_520:
				memoryFilename = BASE_PATH + NVIDIA_520_PATH + FFT_PATH + NVIDIA_520_CSV_PATH +
				DADOS_CSV_NAME + imgTam + "/" + imgTam + IMG_SHORT_NAME + imgID + MEMORY_CSV_EXT;
				break;

			case GT_210:
				memoryFilename = BASE_PATH + NVIDIA_210_PATH + FFT_PATH + NVIDIA_210_CSV_PATH +
				DADOS_CSV_NAME + imgTam + "/" + imgTam + IMG_SHORT_NAME + imgID + MEMORY_CSV_EXT;
				break;

			case HD_6450:
				memoryFilename = BASE_PATH + ATI_HD_6450_PATH + FFT_PATH + imgTam +"/"+ ATI_IMG_BASE_PATH +
						imgID +"/"+ imgTam +IMG_SHORT_NAME +imgID + MEMORY_HTML_EXT;
				break;
			}
			break;
		}
		return memoryFilename;
	}
}
