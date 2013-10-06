package br.edu.uea.ati;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;

import org.apache.commons.lang3.StringUtils;

public class AtiResultsImprover {


	private static final String FILTER_KERNEL_FILE_NAME = "ContextSummary.html";
	private static final String FILTER_DATA_TRANSFER_FILE_NAME = "Top10DataTransferSummary.html"; 
	private static final String KERNEL_HTML_EXT 	= "_kernel.html";
	private static final String MEMORY_HTML_EXT 	= "_memory.html";
	private static final String IMG_SHORT_NAME		= "_img";

	private static final String IMG_TAM_256  = "256";
	private static final String IMG_TAM_512  = "512";
	private static final String IMG_TAM_1024 = "1024";
	private static final String IMG_TAM_2048 = "2048";

	private static final String FULL_SOBEL_PATH = "C:/PROJETOS/darlisson-tcc13/RESULTS/ATI/SOBEL/";
	private static final String FULL_FFT_PATH = "C:/PROJETOS/darlisson-tcc13/RESULTS/ATI/FFT/";
	private static final String ATI_HTML_DATA_PATH = "data_html2";
	private static final String IMG_BASE_PATH = "image";
	private static File fileAti;
	private static File newKernelFile;
	private static File newMemoryFile;

	public static void main(String[] args) throws IOException {
		String filesPath = "";
		
		System.out.println("Removing and renaming files..." );
		
		filesPath = FULL_SOBEL_PATH + ATI_HTML_DATA_PATH ;
		File file = new File(filesPath);
		file.mkdir();
		
		String filesPath2 = FULL_SOBEL_PATH + IMG_TAM_256 +"/" + IMG_TAM_256 + "_" + IMG_BASE_PATH + 2;
		
		File file2 = new File(filesPath2);
		File[] files = file2.listFiles();
		
		newKernelFile = new File(filesPath + "/" + 256 + IMG_SHORT_NAME+ 1 + KERNEL_HTML_EXT);
		/* modificacao*/
		copia(file2, newKernelFile);
		
		for(int i = 1; i <=8; i++){
			filesPath = FULL_SOBEL_PATH + IMG_TAM_256 +"/" + IMG_TAM_256 + "_" + IMG_BASE_PATH + i;
			deleteAllFiles(filesPath);
			renameFiles(filesPath, IMG_TAM_256, i);
		}
		
		for(int i = 1; i <=8; i++){
			filesPath = FULL_SOBEL_PATH + IMG_TAM_512 +"/" + IMG_TAM_512 + "_" + IMG_BASE_PATH + i;
			deleteAllFiles(filesPath);
			renameFiles(filesPath,IMG_TAM_512,i);
		}

		for(int i = 1; i <=8; i++){
			filesPath = FULL_SOBEL_PATH + IMG_TAM_1024+"/" +   IMG_TAM_1024 + "_" + IMG_BASE_PATH + i;
			deleteAllFiles(filesPath);
			renameFiles(filesPath, IMG_TAM_1024, i);
		}

		for(int i = 1; i <=8; i++){
			filesPath = FULL_SOBEL_PATH + IMG_TAM_2048 + "/" + IMG_TAM_2048 + "_" + IMG_BASE_PATH + i;
			deleteAllFiles(filesPath);
			renameFiles(filesPath, IMG_TAM_2048, i);
		}
		//================== FFT ===========================


		for(int i = 1; i <=8; i++){
			filesPath = FULL_FFT_PATH + IMG_TAM_256 +"/" + IMG_BASE_PATH + i;
			deleteAllFiles(filesPath);
			renameFiles(filesPath, IMG_TAM_256, i);

		}

		for(int i = 1; i <=8; i++){
			filesPath = FULL_FFT_PATH + IMG_TAM_512 +"/" + IMG_BASE_PATH + i;
			deleteAllFiles(filesPath);
			renameFiles(filesPath, IMG_TAM_512, i);
		}

		for(int i = 1; i <=8; i++){
			filesPath = FULL_FFT_PATH + IMG_TAM_1024 +"/" + IMG_BASE_PATH + i;
			deleteAllFiles(filesPath);
			renameFiles(filesPath, IMG_TAM_1024, i);
		}

		for(int i = 1; i <=8; i++){
			filesPath = FULL_FFT_PATH + IMG_TAM_2048 +"/" + IMG_BASE_PATH + i;
			deleteAllFiles(filesPath);
			renameFiles(filesPath, IMG_TAM_2048, i);
		}
		
		System.out.println("The process was successfully completed!" );
	}

	public static void renameFiles(String filesPath, String imgTam, int imgID){

		fileAti = new File(filesPath);
		newKernelFile = new File(filesPath + "/" + imgTam + IMG_SHORT_NAME+imgID + KERNEL_HTML_EXT);
		newMemoryFile = new File(filesPath + "/" + imgTam + IMG_SHORT_NAME+imgID + MEMORY_HTML_EXT);
		
		File[] files = fileAti.listFiles();
		
		if(StringUtils.substringAfter(files[0].getName(), ".").equals(FILTER_KERNEL_FILE_NAME)){
			files[0].renameTo(newKernelFile);
			files[1].renameTo(newMemoryFile);
		} else {
			files[0].renameTo(newMemoryFile);
			files[1].renameTo(newKernelFile);
		} 			
	}	

	public static void copyFiles(String filesPath, String imgTam, int imgID){

		fileAti = new File(filesPath);
		newKernelFile = new File(filesPath + "/" + 256 + IMG_SHORT_NAME+ 1 + KERNEL_HTML_EXT);
		/* temp */
		copia(fileAti, newKernelFile);
			
		newKernelFile = new File(filesPath + "/" + imgTam + IMG_SHORT_NAME+imgID + KERNEL_HTML_EXT);
		newMemoryFile = new File(filesPath + "/" + imgTam + IMG_SHORT_NAME+imgID + MEMORY_HTML_EXT);
		
		File[] files = fileAti.listFiles();
		
		if(StringUtils.substringAfter(files[0].getName(), ".").equals(FILTER_KERNEL_FILE_NAME)){
			files[0].renameTo(newKernelFile);
			files[1].renameTo(newMemoryFile);
		} else {
			files[0].renameTo(newMemoryFile);
			files[1].renameTo(newKernelFile);
		} 			
	}	
		
	public static void deleteAllFiles(String filesPath) {
		fileAti = new File(filesPath);
	//	deleteFile(filterFilesToDelete(fileAti.listFiles()));  
	}

	public static void printFileName(ArrayList<File> listFiles){
		for (File f: listFiles) {
			System.out.println(f.getName());
		}
	}

	public static void printFileName(File[] listFiles){
		for (File f: listFiles) {
			System.out.println(f.getName());
		}
	}

	public static ArrayList<File> filterFilesToCopy(File[] files){

		ArrayList<File> filesToCopy = new ArrayList<File>();

		for (File f : files) {
			if((StringUtils.substringAfter(f.getName(), ".").equals(FILTER_KERNEL_FILE_NAME)
					|| StringUtils.substringAfter(f.getName(), ".").equals(FILTER_DATA_TRANSFER_FILE_NAME)))
				filesToCopy.add(f);
		}
		return filesToCopy;
	}

	public static void deleteFile(ArrayList<File> files){

		if(!files.isEmpty()){
			try {
				for (File file : files) {
					file.delete();
				}	
			} catch (Exception e) {
				e.printStackTrace();
			}
		} else{
			System.out.println("The folder is empty!");
		}

	}
	
	public static boolean copia( File srcDir, File dstDir ){
	    
        try{

            if( srcDir.isDirectory() ){

                if( !dstDir.exists() ){

                    dstDir.mkdir();
                }

                String[] children = srcDir.list();

                for (int i=0; i<children.length; i++){

                    copia( new File( srcDir, children[i] ), new File( dstDir, children[i] ) );
                }
            } 
            else{

                InputStream in = new FileInputStream( srcDir );
                OutputStream out = new FileOutputStream( dstDir );

                byte[] buf = new byte[1024];
                int len;

                while( (len = in.read( buf ) ) > 0 ) {

                    out.write( buf, 0, len );
                }

                in.close();
                out.close();
            }
        }
        catch( IOException ioex ){

            ioex.printStackTrace();
            return false;
        }

        return true;
    }

}
