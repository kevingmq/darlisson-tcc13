#define PI 3.14159265358979323846
#define PI_2 1.57079632679489661923

__kernel void twiddle_factors(__global float2* wn, int n)
{
    unsigned int i = get_global_id(0);

    float2 angle = (float2)(2*i*PI/(float)n,(2*i*PI/(float)n) + PI_2);
    // Com o deslocamento de PI/2 basta calcular o cosseno
    wn[i] = cos(angle);
}

__kernel void bits_reverse(__global float2 *output, __global float2 *input, int m, int n)
{
    unsigned int gid = get_global_id(0);
    unsigned int nid = get_global_id(1);

    unsigned int j = gid;

    j = (j & 0x55555555) << 1 | (j & 0xAAAAAAAA) >> 1;
    j = (j & 0x33333333) << 2 | (j & 0xCCCCCCCC) >> 2;
    j = (j & 0x0F0F0F0F) << 4 | (j & 0xF0F0F0F0) >> 4;
    j = (j & 0x00FF00FF) << 8 | (j & 0xFF00FF00) >> 8;
    j = (j & 0x0000FFFF) << 16 | (j & 0xFFFF0000) >> 16;

    j >>= (32-m);

    output[nid*n+j] = input[nid*n+gid];
}

__kernel void normalizar(__global float2 *input, int n)
{
    unsigned int gid = get_global_id(0);
    unsigned int nid = get_global_id(1);

    input[nid*n+gid] = input[nid*n+gid] / (float2)((float)n, (float)n);
}

__kernel void butterfly_operation(__global float2 *x, __global float2* w, int m, int n, int iter, uint flag)
{
    unsigned int gid = get_global_id(0);
    unsigned int nid = get_global_id(1);

    int butterflySize = 1 << (iter-1);
    int butterflyGrpDist = 1 << iter;
    int butterflyGrpNum = n >> iter;
    int butterflyGrpBase = (gid >> (iter-1))*(butterflyGrpDist);
    int butterflyGrpOffset = gid & (butterflySize-1);

    int a = nid * n + butterflyGrpBase + butterflyGrpOffset;
    int b = a + butterflySize;

    int l = butterflyGrpNum * butterflyGrpOffset;

    float2 xa, xb, xbxx, xbyy, wab, wayx, wbyx, resa, resb;

    xa = x[a];
    xb = x[b];
    xbxx = xb.xx;
    xbyy = xb.yy;

    wab = as_float2(as_uint2(w[l]) ^ (uint2)(0x0, flag));
    wayx = as_float2(as_uint2(wab.yx) ^ (uint2)(0x80000000, 0x0));
    wbyx = as_float2(as_uint2(wab.yx) ^ (uint2)(0x0, 0x80000000));

    resa = xa + xbxx*wab + xbyy*wayx;
    resb = xa - xbxx*wab + xbyy*wbyx;

    x[a] = resa;
    x[b] = resb;
}

__kernel void matrix_trasponse(__global float2 *output, __global float2* input, int n)
{
    unsigned int i_gid = get_global_id(0);
    unsigned int j_gid = get_global_id(1);

    output[i_gid*n + j_gid] = input[j_gid*n + i_gid];
}

__kernel void lowpass_filter(__global float2* image, int n, int raio)
{
    unsigned int xid = get_global_id(0);
    unsigned int yid = get_global_id(1);

    int2 n_2 = (int2)(n/2, n/21);
    int2 mask = (int2)(n-1, n-1);

    int2 gid = ((int2)(xid, yid) + n_2) & mask;

    int2 diferenca = n_2 - gid;
    int2 diferenca2 = diferenca * diferenca;
    int dist2 = diferenca2.x + diferenca2.y;

    int2 janela;

    /*passa alta*/

    if (dist2 < raio*raio) {
        //janela = (int2)(0L, 0L);  /*passa alta*/
        janela = (int2)(-1L, -1L);
    } else {
        //janela = (int2)(-1L, -1L); /*passa alta*/
        janela = (int2)(0L, 0L);
    }

    image[yid*n+xid] = as_float2(as_int2(image[yid*n+xid]) &janela);
}

