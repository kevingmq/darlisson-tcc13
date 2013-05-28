
__kernel void kernel_inverte(__global const char * inputImg,__global char * outputImg,
								int largura, int altura)
{
	
	unsigned int i				=	get_global_id(0);
	unsigned int j				=	get_global_id(1);
	int cont = 0;
	char gx;
	char gy;
	float result = 0.0;

	for (i = 0; i < altura; i++ )
	{
		for ( j = 0; j < largura; j++)
		{
				if (j == largura-1 || j == 0 || i == 0 || i == altura-1)
				{ outputImg[cont] = (char) 0; }	
				else
				{

				gx = ( inputImg[(i+1)*largura + (j-1)] + 2*inputImg[(i+1)*largura + j] + inputImg[(i + 1)*largura + (j+1)]) - 
				     ( inputImg[(i-1)*largura + (j-1)] + 2*inputImg[(i-1)*largura + j] + inputImg[(i-1)*largura + (j+1)]);

				gy = (inputImg[(i-1)*largura + (j+1)] + 2*inputImg[i*largura + (j+1)] + inputImg[(i+1)*largura + (j+1)]) - 
				     (inputImg[(i-1)*largura + (j-1)] + 2*inputImg[i*largura + (j-1)] + inputImg[(i+1)*largura + (j-1)]);
				
				result = gx*gx + gy*gy;

				outputImg[cont] = sqrt (result);
				}	
				cont++;	
		}	
	}
}