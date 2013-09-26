
__kernel void sobel_kernel( __global const char* inputImg,
                            __global char* outputImg)
    {
        int i   =   get_global_id(0);
        int j   =   get_global_id(1);
        int width = get_global_size(0);
        int height = get_global_size(1);
        char gx, gy;
        char sum = 0;

        outputImg[i*width + j] = (char) 0;

        if (i > 0 && i < width - 1 && j > 0 && j < height - 1) {

        gx = ( inputImg[(i-1)*width + (j+1)] + 2*inputImg[(i)*width + (j+1)] + inputImg[(i+1)*width + (j+1)])
                - ( inputImg[(i-1)*width + (j-1)] + 2*inputImg[(i)*width + (j-1)] + inputImg[(i+1)*width + (j+1)]);

        gy = ( inputImg[(i-1)*width + (j-1)] + 2*inputImg[(i-1)*width + j] + inputImg[(i-1)*width + (j+1)])
             - ( inputImg[(i+1)*width + (j-1)] + 2*inputImg[(i+1)*width + (j)] + inputImg[(i+1)*width + (j+1)]);

         sum = (char) sqrt( (float) gx*gx + gy*gy);
         outputImg[i*width + j] = (sum>255)?255:sum;
    }
}