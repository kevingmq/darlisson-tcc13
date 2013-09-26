#ifndef _PGM_H_
#define _PGM_H_

#include <math.h>
#include <string.h>

#define PGM_MAGIC "P5"

#ifdef _WIN32
#define STRTOK_R(ptr, del, saveptr) strtok_s(ptr, del, saveptr)
#else
#define STRTOK_R(ptr, del, saveptr) strtok_r(ptr, del, saveptr)
#endif

typedef struct _pgm_t
{
    int width;
    int height;
    unsigned char *buf;
} pgm_t;

int readPGM(pgm_t *pgm, const char *filename)
{
    char *token, *pc, *saveptr;
    char *buf;
    size_t bufsize;
    char del[] = " \t\n";
    unsigned char *dot;

    long begin, end;
    int filesize;
    int i, w, h, luma, pixs;


    FILE *fp;
    if ((fp = fopen(filename, "rb")) == NULL)
    {
        fprintf(stderr, "Failed to open file\n");
        return -1;
    }

    fseek(fp, 0, SEEK_SET);
    begin = ftell(fp);
    fseek(fp, 0, SEEK_END);
    end = ftell(fp);
    filesize = (int)(end - begin);

    buf = (char *)malloc(filesize * sizeof(char));
    fseek(fp, 0, SEEK_SET);
    bufsize = fread(buf, filesize * sizeof(char), 1, fp);

    fclose(fp);

    token = (char *)STRTOK_R(buf, del, &saveptr);
    if (strncmp(token, PGM_MAGIC, 2) != 0)
    {
        return -1;
    }

    token = (char *)STRTOK_R(NULL, del, &saveptr);
    if (token[0] == '#' )
    {
        token = (char *)STRTOK_R(NULL, "\n", &saveptr);
        token = (char *)STRTOK_R(NULL, del, &saveptr);
    }

    w = strtoul(token, &pc, 10);
    token = (char *)STRTOK_R(NULL, del, &saveptr);
    h = strtoul(token, &pc, 10);
    token = (char *)STRTOK_R(NULL, del, &saveptr);
    luma = strtoul(token, &pc, 10);

    token = pc + 1;
    pixs = w * h;

    pgm->buf = (unsigned char *)malloc(pixs * sizeof(unsigned char));

    dot = pgm->buf;

    for (i = 0; i < pixs; i++, dot++)
    {
        *dot = *token++;
    }

    pgm->width = w;
    pgm->height = h;

    return 0;
}

int writePGM(pgm_t *pgm, const char *filename)
{
    int i, w, h, pixs;
    FILE *fp;
    unsigned char *dot;

    w = pgm->width;
    h = pgm->height;
    pixs = w * h;

    if ((fp = fopen(filename, "wb+")) == NULL)
    {
        fprintf(stderr, "Failed to open file\n");
        return -1;
    }

    fprintf (fp, "%s\n%d %d\n255\n", PGM_MAGIC, w, h);

    dot = pgm->buf;

    for (i = 0; i < pixs; i++, dot++)
    {
        putc((unsigned char)*dot, fp);
    }

    fclose(fp);

    return 0;
}

int writeFLOAT(pgm_t *pgm, float *x)
{
    int i, j, w, h;
    FILE *fp;

    w = pgm->width;
    h = pgm->height;

    if ((fp = fopen("float.dat", "w+")) == NULL)
    {
        fprintf(stderr, "Failed to open file\n");
        return -1;
    }

    for (i = 0; i < h; i++)
    {
        for (j = 0; j < w; j++)
        {
            fprintf(fp, "%.2f ", x[i * w + j]);
        }
        fprintf(fp, "\n");
    }

    fclose(fp);

    return 0;
}

int normalizeF2PGM(pgm_t *pgm, float *x)
{
    int i, j, w, h;
    float min = 0;
    float max = 0;

    w = pgm->width;
    h = pgm->height;

    pgm->buf = (unsigned char *)malloc(w * h * sizeof(unsigned char));


    for (i = 0; i < h; i++)
    {
        for (j = 0; j < w; j++)
        {
            if (max < x[i * w + j])
                max = x[i * w + j];
            if (min > x[i * w + j])
                min = x[i * w + j];
        }
    }

    for (i = 0; i < h; i++)
    {
        for (j = 0; j < w; j++)
        {
            if ((max - min) != 0)
                pgm->buf[i * w + j] = (unsigned char)(255 * (x[i * w + j] - min) / (max - min));
            else
                pgm->buf[i * w + j] = 0;
        }
    }

    return 0;
}

int sobelOnCPU(pgm_t *ipgm, pgm_t* opgm)
{

    int i, j, w, h, sum;
    unsigned char gx, gy;
    w = ipgm->width;
    h = ipgm->height;

	opgm->buf = malloc(w*h*(sizeof(unsigned char)));

    for (i = 0; i < w; i++)
    {
        for (j = 0; j < h; j++)
        {
			gx = 0;

			gx = ( ipgm->buf[(i - 1) * w + (j + 1)] + 2 * ipgm->buf[(i) * w + (j + 1)] + ipgm->buf[(i + 1) * w + (j + 1)])
                 - ( ipgm->buf[(i - 1) * w + (j - 1)] + 2 * ipgm->buf[(i) * w + (j - 1)] + ipgm->buf[(i + 1) * w + (j + 1)]);
			
			gy = 0;
            gy = ( ipgm->buf[(i - 1) * w + (j - 1)] + 2 * ipgm->buf[(i - 1) * w + j] + ipgm->buf[(i - 1) * w + (j + 1)])
                  - ( ipgm->buf[(i + 1) * w + (j - 1)] + 2 * ipgm->buf[(i + 1) * w + (j)] + ipgm->buf[(i + 1) * w + (j + 1)]);

			
			sum = abs(gx)+abs(gy);
           sum=abs(gx)+abs(gx);
			opgm->buf[i*w+j]=(sum>255)?255:sum;
	
        }
    }

    return 0;
}

int sobelOnCPU2(pgm_t *ipgm, pgm_t* opgm)
{
	int dx[3][3] = {{1,0,-1},{2,0,-2},{1,0,-1}};
	int dy[3][3] = {{1,2,1},{0,0,0},{-1,-2,-1}};
	int i, j, step, h,s, sum, sum_x,sum_y, m,n,x;
    step = ipgm->width;
    h = ipgm->height;

	opgm->buf = malloc(step*h*(sizeof(unsigned char)));

    for ( i=1; i < ipgm->height-2; i++)
        for ( j=1; j < ipgm->width-2; j++)
        {
            sum_x=0;
            sum_y=0;
            for(m=-1; m<=1; m++)
                for(n=-1; n<=1; n++)
                {
                    s=ipgm->buf[(i+m)*step+j+n]; 
                    sum_x+=s*dx[m+1][n+1];
                    sum_y+=s*dy[m+1][n+1];
                }
            sum=abs(sum_x)+abs(sum_y);
			opgm->buf[i*step+j]=(sum>255)?255:sum;
        }
}

int destroyPGM(pgm_t *pgm)
{
    if (pgm->buf)
    {
        free(pgm->buf);
    }

    return 0;
}
#endif /* _PGM_H_ */