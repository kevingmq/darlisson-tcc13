package br.edu.uea.main;

import static br.edu.uea.utils.DataCollectorUtil.arrayListToString;
import static br.edu.uea.utils.DataCollectorUtil.getDataTransferRateATI;
import static br.edu.uea.utils.DataCollectorUtil.getDataTransferRateNVIDIA;
import static br.edu.uea.utils.DataCollectorUtil.getKernelHeader;
import static br.edu.uea.utils.DataCollectorUtil.getKernelTimeExecutionATI;
import static br.edu.uea.utils.DataCollectorUtil.getKernelTimeExecutionNVIDIA;
import static br.edu.uea.utils.FilePathUtil.getKernelFilename;
import static br.edu.uea.utils.FilePathUtil.getMemoryFilename;

import java.io.FileNotFoundException;
import java.io.IOException;

import br.edu.uea.enums.APPLICATION;
import br.edu.uea.enums.GPU_MODEL;
import br.edu.uea.enums.GPU_VENDOR;

public class MainCollectorResults {


	private static final String IMG_TAM_256  = "256";
	private static final String IMG_TAM_512  = "512";
	private static final String IMG_TAM_1024 = "1024";
	private static final String IMG_TAM_2048 = "2048";
	private static final String KERNEL_TIME_EXECUTION = "KERNEL TIME EXECUTION";
	private static final String DATA_TRANFER_RATE = "DATA TRANFER RATE";
		
	private static GPU_MODEL 	gpuModel;
	private static GPU_VENDOR 	gpuVendor;

	private static APPLICATION  appType;
	private static String 		kernelHeader;
	private static final boolean onDebug = false;


	public static void main(String[] args) throws IOException {

		String kernelFileName = "";
		String memoryFileName = "";

		try {

			/* Setup Data Collector */
			/* GPU_VENDOR  = NVIDIA || ATI */ 
			gpuVendor = GPU_VENDOR.ATI;
			
			/* GPU_MODEL   - GT_520,GT_210 || HD_6450*/
			gpuModel = GPU_MODEL.HD_6450;
			
			/* APPLICATION - SOBEL, FFT*/
			appType = APPLICATION.SOBEL;
			
			if(gpuVendor == GPU_VENDOR.ATI && gpuModel != GPU_MODEL.HD_6450){
				System.out.println("**Error: invalid GPU info: [" + gpuVendor + " - " + gpuModel + "]");
				System.exit(0);
			}


			//====================/ KERNEL /====================================
			
			printOnDebug(IMG_TAM_256, KERNEL_TIME_EXECUTION);	

			kernelFileName = getKernelFilename(gpuModel, appType, IMG_TAM_256, 1);

				if(gpuVendor == GPU_VENDOR.NVIDIA){
					kernelHeader=arrayListToString(getKernelHeader(kernelFileName), true,gpuVendor);
				    System.out.println(kernelHeader);
				    }

			

			for(int i = 1; i <=8;i++){
				kernelFileName = getKernelFilename(gpuModel, appType, IMG_TAM_256, i);

				if(gpuVendor == GPU_VENDOR.NVIDIA)				
					System.out.println(getKernelTimeExecutionNVIDIA(getKernelTimeExecutionNVIDIA(kernelFileName)));
				else{
					System.out.println(getKernelTimeExecutionATI(kernelFileName));
				}
			}

			printOnDebug(IMG_TAM_512, KERNEL_TIME_EXECUTION);	

			for(int i = 1; i <=8;i++){
				kernelFileName = getKernelFilename(gpuModel, appType, IMG_TAM_512, i);

				if(gpuVendor == GPU_VENDOR.NVIDIA)				
					System.out.println(getKernelTimeExecutionNVIDIA(getKernelTimeExecutionNVIDIA(kernelFileName)));
				else{
					System.out.println(getKernelTimeExecutionATI(kernelFileName));
				}}

			printOnDebug(IMG_TAM_1024, KERNEL_TIME_EXECUTION);	

			for(int i = 1; i <=8;i++){
				kernelFileName = getKernelFilename(gpuModel, appType, IMG_TAM_1024, i);
				if(gpuVendor == GPU_VENDOR.NVIDIA)				
					System.out.println(getKernelTimeExecutionNVIDIA(getKernelTimeExecutionNVIDIA(kernelFileName)));
				else{
					System.out.println(getKernelTimeExecutionATI(kernelFileName));
				}}

			printOnDebug(IMG_TAM_2048, KERNEL_TIME_EXECUTION);	

			for(int i = 1; i <=8;i++){
				kernelFileName = getKernelFilename(gpuModel, appType, IMG_TAM_2048, i);
				if(gpuVendor == GPU_VENDOR.NVIDIA)				
					System.out.println(getKernelTimeExecutionNVIDIA(getKernelTimeExecutionNVIDIA(kernelFileName)));
				else{
					System.out.println(getKernelTimeExecutionATI(kernelFileName));
				}}

			//====================/ MEMORY /====================================


			printOnDebug(IMG_TAM_256, DATA_TRANFER_RATE);	

			System.out.println("|WRITE|READ|");
			for(int i = 1; i <=8;i++){
				memoryFileName = getMemoryFilename(gpuModel, appType, IMG_TAM_256, i);

				if(gpuVendor == GPU_VENDOR.NVIDIA)				
					System.out.print(arrayListToString(getDataTransferRateNVIDIA(memoryFileName),false,gpuVendor));
				else{
					System.out.println(arrayListToString(getDataTransferRateATI(memoryFileName),false,gpuVendor));
				}
			}

			printOnDebug(IMG_TAM_512, DATA_TRANFER_RATE);	

			for(int i = 1; i <=8;i++){
				memoryFileName = getMemoryFilename(gpuModel, appType, IMG_TAM_512, i);

				if(gpuVendor == GPU_VENDOR.NVIDIA)				
					System.out.print(arrayListToString(getDataTransferRateNVIDIA(memoryFileName),false,gpuVendor));
				else{
					System.out.println(arrayListToString(getDataTransferRateATI(memoryFileName),false,gpuVendor));
				}
			}

			printOnDebug(IMG_TAM_1024, DATA_TRANFER_RATE);	

			for(int i = 1; i <=8;i++){
				memoryFileName = getMemoryFilename(gpuModel, appType, IMG_TAM_1024, i);
				if(gpuVendor == GPU_VENDOR.NVIDIA)				
					System.out.print(arrayListToString(getDataTransferRateNVIDIA(memoryFileName),false,gpuVendor));
				else{
					System.out.println(arrayListToString(getDataTransferRateATI(memoryFileName),false,gpuVendor));
				}
			}

			printOnDebug(IMG_TAM_2048, DATA_TRANFER_RATE);	

			for(int i = 1; i <=8;i++){
				memoryFileName = getMemoryFilename(gpuModel, appType, IMG_TAM_2048, i);
				if(gpuVendor == GPU_VENDOR.NVIDIA)				
					System.out.print(arrayListToString(getDataTransferRateNVIDIA(memoryFileName),false,gpuVendor));
				else{
					System.out.println(arrayListToString(getDataTransferRateATI(memoryFileName),false,gpuVendor));
				}	}						
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		}	
	}


	private static void printOnDebug(String imgTam, String kpi) {
			
		if(onDebug){
			System.out.println("+------------------------------------------------------+");
			System.out.println("|" + gpuVendor + " "+ gpuModel+" - "+ appType +" - " + imgTam + " - " + kpi + "");
			System.out.println("+------------------------------------------------------+");
		}
	}
}