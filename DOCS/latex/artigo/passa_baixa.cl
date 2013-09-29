__kernel void passa_baixa_kernel(
	__global float2* imagem,
	int imgTam,
	int raio_d)
	{
	unsigned int u = get_global_id(0);  
	unsigned int v = get_global_id(1);  
     
	int2 imgTam_2 = (int2)(imgTam/2, imgTam/2);
	/* P/2 e Q/2 */
	int2 mask = (int2)(imgTam-1, imgTam-1); /* mask pra limitar os pixels 0-255*/ 
	int2 uv = ((int2)(u, v) + imgTam_2) & mask;  
    int2 diff = uv - imgTam_2; /* (u - P/2)  e (v - Q/2)     */
	int2 diff2 = diff * diff;  	/* (u - P/2 )^2  e (v - Q/2 )^2  */
	int dist2 = diff2.x + diff2.y;  /* D(u,v) = [(u - P/2 )^2 + (v - Q/2 )^2] */
	int2 H_uv; /* H(u,v) */   
  
	if (dist2 < raio_d) {
		H_uv = (int2)(-1L, -1L);  /* 1 se D(u,v) < Do */
	} else {   
		H_uv = (int2)(0L, 0L);   /* 1 se D(u,v) > Do*/
	}
	imagem[v*imgTam+u] = as_float2(as_int2(imagem[v*imgTam+u]) & H_uv); 
}