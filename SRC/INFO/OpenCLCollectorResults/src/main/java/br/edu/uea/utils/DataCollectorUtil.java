package br.edu.uea.utils;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;

import org.apache.commons.lang3.StringUtils;
import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;

import br.edu.uea.enums.GPU_VENDOR;

import au.com.bytecode.opencsv.CSVReader;

public class DataCollectorUtil {
	
	private static final int HTML_COLUMN_KERNEL_TIME = 4;
	private static final int HTML_ROW_KERNEL_TIME = 1;
	private static final int HTML_COLUMN_TRANSFER_RATE = 5;
	private static final int HTML_ROW_TRANSFER_RATE_WRITE = 1;
	private static final int HTML_ROW_TRANSFER_RATE_READ = 2;

	private static final String KERNEL_COLUMN_FILTER_NAME 	= "Device Time\n(μs)";
	private static final int 	KERNEL_COLUMN_FILTER_INDEX 	= 7;
	private static final String KERNEL_COLUMN_HEADER_NAME 	= "Kernel Name";
	private static final int 	KERNEL_COLUMN_HEADER_INDEX 	= 3;

	private static final String MEMORY_COLUMN_FILTER_NAME 	= "Rate\n(MB/s)";
	private static final int 	MEMORY_COLUMN_FILTER_INDEX 	= 6;
	private static CSVReader kernelReader;
	private static CSVReader memoryReader;
	private static CSVReader headerReader;
	private static Document htmlDoc;
	private static File htmlFile;
	

	public static ArrayList<String> getKernelTimeExecutionNVIDIA(String kernelFileName)
			throws IOException {

		kernelReader = new CSVReader(new FileReader(kernelFileName));
		String[] nextLine;
		ArrayList<String> kernelValues = new ArrayList<String>();
		try {
			while ((nextLine = kernelReader.readNext()) != null) {
				kernelValues.add(StringUtils.remove(nextLine[KERNEL_COLUMN_FILTER_INDEX],KERNEL_COLUMN_FILTER_NAME));			
			}
			kernelReader.close();		
		} catch (IOException e) {
			e.printStackTrace();
		} 
		return kernelValues;
	}
	
	public static double getKernelTimeExecutionNVIDIA(ArrayList<String> timesStr)
			throws IOException {

		double kernelTime = 0.0f;
		String tempValue;
		for (String strValue : timesStr) {
			
			tempValue = StringUtils.remove(strValue, ',');
			
			if (!tempValue.isEmpty()) {

				kernelTime += Double.parseDouble(tempValue);
			}
			
		}
		return kernelTime;
	}

	public static ArrayList<String> getKernelHeader(String kernelFileName)
			throws IOException {

		headerReader = new CSVReader(new FileReader(kernelFileName));
		String[] nextLine;
		ArrayList<String> kernelHeaders = new ArrayList<String>();

		try {
			while ((nextLine = headerReader.readNext()) != null) {
				kernelHeaders.add(StringUtils.remove(nextLine[KERNEL_COLUMN_HEADER_INDEX],KERNEL_COLUMN_HEADER_NAME));			
			}	
			headerReader.close();			
		} catch (IOException e) {
			e.printStackTrace();
		} 
		return kernelHeaders;
	}

	public static ArrayList<String> getDataTransferRateNVIDIA(String memoryFileName) throws FileNotFoundException
	{
		memoryReader = new CSVReader(new FileReader(memoryFileName));
		String [] nextLine;
		ArrayList<String> memoryValues = new ArrayList<String>();
		try {
			while ((nextLine = memoryReader.readNext()) != null) {
				memoryValues.add(StringUtils.remove(nextLine[MEMORY_COLUMN_FILTER_INDEX],MEMORY_COLUMN_FILTER_NAME));			
			}
			memoryReader.close();
		} catch (IOException e) {
			e.printStackTrace();
		} 
		return memoryValues;
	}

	public static String arrayListToString(ArrayList<String> stringList ,boolean isKernel,GPU_VENDOR gpuVendor) {

		StringBuilder sb = new StringBuilder();
		char charAppend;
		
		if(!isKernel){
			charAppend = '|';
		} else {
			charAppend = ' ';
		}
		
		for (String s : stringList) {
			sb.append(s).append(charAppend);
		}
		if(!isKernel && gpuVendor == GPU_VENDOR.NVIDIA)
			sb.append('\n');	
		return sb.toString();	
	}

	public static String getKernelTimeExecutionATI(String filePath){

		String kernelTimeExecution = "";

		htmlFile = new File(filePath);
		try {
			htmlDoc = Jsoup.parse(htmlFile, "UTF-8");
		} catch (IOException e) {
			e.printStackTrace();
		}
		Element table = htmlDoc.select("table[class=draggable sortable]").get(0);
		Element tr = table.select("tr").get(HTML_ROW_KERNEL_TIME);
		Element td = tr.select("td").get(HTML_COLUMN_KERNEL_TIME);
		kernelTimeExecution = td.text();

		return kernelTimeExecution;
	}

	public static ArrayList<String> getDataTransferRateATI(String filePath){

		ArrayList<String> dataTransferRates = new ArrayList<String>();

		htmlFile = new File(filePath);
		try {
			htmlDoc = Jsoup.parse(htmlFile, "UTF-8");
		} catch (IOException e) {
			e.printStackTrace();
		}

		Element table = htmlDoc.select("table[class=draggable sortable]").get(0);
		Element tr1 = table.select("tr").get(HTML_ROW_TRANSFER_RATE_WRITE);
		Element td1 = tr1.select("td").get(HTML_COLUMN_TRANSFER_RATE);
		dataTransferRates.add(td1.text());
		Element tr2 = table.select("tr").get(HTML_ROW_TRANSFER_RATE_READ);
		Element td2 = tr2.select("td").get(HTML_COLUMN_TRANSFER_RATE);

		dataTransferRates.add(td2.text());

		return dataTransferRates;
	}
}
