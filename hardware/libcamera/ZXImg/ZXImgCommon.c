#include "ZXImgCommon.h"

void	*malloc_aligned(uint32_t size,  uint8_t alignment)
{
	uint8_t *mem_ptr;
	if (!alignment)
	{
		if ((mem_ptr = (uint8_t *) malloc(size + 1)) != NULL) 
		{
			*mem_ptr = (uint8_t)1;
			return ((void*)(mem_ptr+1));
		}
	} 
	else
	{
		uint8_t *tmp;
		if ((tmp = (uint8_t *) malloc(size + alignment)) != NULL) 
		{
			mem_ptr = 	(uint8_t*)((uint32_t)(tmp+alignment-1)&
						(~(uint32_t)(alignment-1)));

			if (mem_ptr == tmp)
				mem_ptr += alignment;

			*(mem_ptr-1) = (uint8_t)(mem_ptr-tmp);
			return ((void *)mem_ptr);
		}
	}
	return(NULL);
}

void free_aligned(void *mem_ptr)
{
	uint8_t *ptr;
	if (mem_ptr == NULL)
		return;
	ptr = (uint8_t*)mem_ptr;
	ptr -= *(ptr - 1);
	free(ptr);
}

void box_mean_uint8_row_y_c(uint8_t *p_src, uint16_t *p_dst, int32_t param1, int32_t* param2)
{
    int32_t i,j,width,height,src_wstride,dst_wstride;
	uint16_t	sum;
	
	width = param1;
	height = param2[0];
	src_wstride = param2[1];
	dst_wstride = param2[2];
	
    // row integral
    for (j =0; j < height; j++, p_src += src_wstride, p_dst += dst_wstride)
	{
		for(i = 0; i < width; i++)
		{
			sum = (((i-4)<0)? p_src[0]:p_src[i-4]);
			sum += (((i-3)<0)? p_src[0]:p_src[i-3]); 
			sum += (((i-2)<0)? p_src[0]:p_src[i-2]); 
			sum += (((i-1)<0)? p_src[0]:p_src[i-1]); 
			sum += p_src[i]; 
			sum += (((i+1)>= width)? p_src[width-1]:p_src[i+1]); 
			sum += (((i+2)>= width)? p_src[width-1]:p_src[i+2]); 
			sum += (((i+3)>= width)? p_src[width-1]:p_src[i+3]);
			p_dst[i] = sum;
		}
    }
	return;
}

void box_mean_uint8_col_y_c(uint16_t *p_src, uint8_t *p_dst, int32_t param1, int32_t* param2)
{
    int32_t	j,i,width,height,src_wstride,dst_wstride;
    int32_t sum;

	width = param1;
	height = param2[0];
	src_wstride = param2[1];
	dst_wstride = param2[2];

    // column integral
    for(i = 0; i < width; i++,p_src++,p_dst++)
	{
        for(j = 0; j < height; j++)
		{
			sum = (((j-4)<0)? p_src[0]:p_src[(j-4)*src_wstride]);
			sum += (((j-3)<0)? p_src[0]:p_src[(j-3)*src_wstride]); 
			sum += (((j-2)<0)? p_src[0]:p_src[(j-2)*src_wstride]); 
			sum += (((j-1)<0)? p_src[0]:p_src[(j-1)*src_wstride]); 
			sum += p_src[j*src_wstride]; 
			sum += (((j+1)>= height)? p_src[(height-1)*src_wstride]:p_src[(j+1)*src_wstride]); 
			sum += (((j+2)>= height)? p_src[(height-1)*src_wstride]:p_src[(j+2)*src_wstride]); 
			sum += (((j+3)>= height)? p_src[(height-1)*src_wstride]:p_src[(j+3)*src_wstride]);
			p_dst[j*dst_wstride] = (uint8_t)((sum + N/2) / N);
		}
    }
	return;
}

void box_mean_uint16_row_y_c(uint16_t *p_src, uint16_t *p_dst, int32_t param1, int32_t* param2)
{
    int32_t i,j,width,height,src_wstride,dst_wstride;
	int32_t	sum;
	
	width = param1;
	height = param2[0];
	src_wstride = param2[1];
	dst_wstride = param2[2];
	
    // row integral
    for (j =0; j < height; j++, p_src += src_wstride, p_dst += dst_wstride)
	{
		for(i = 0; i < width; i++)
		{
			sum = (((i-4)<0)? p_src[0]:p_src[i-4]);
			sum += (((i-3)<0)? p_src[0]:p_src[i-3]); 
			sum += (((i-2)<0)? p_src[0]:p_src[i-2]); 
			sum += (((i-1)<0)? p_src[0]:p_src[i-1]); 
			sum += p_src[i]; 
			sum += (((i+1)>= width)? p_src[width-1]:p_src[i+1]); 
			sum += (((i+2)>= width)? p_src[width-1]:p_src[i+2]); 
			sum += (((i+3)>= width)? p_src[width-1]:p_src[i+3]);
			p_dst[i] = (uint16_t)((sum + RADIUS) / D);
		}
    }
	return;
}

void box_mean_uint16_col_y_c(uint16_t *p_src,uint16_t *p_dst,int32_t param1, int32_t* param2)
{
    int32_t	j,i,width,height,src_wstride,dst_wstride;
    int32_t sum;

	width = param1;
	height = param2[0];
	src_wstride = param2[1];
	dst_wstride = param2[2];

    // column integral
    for(i = 0; i < width; i++,p_src++,p_dst++)
	{
        for(j = 0; j < height; j++)
		{
			sum = (((j-4)<0)? p_src[0]:p_src[(j-4)*src_wstride]);
			sum += (((j-3)<0)? p_src[0]:p_src[(j-3)*src_wstride]); 
			sum += (((j-2)<0)? p_src[0]:p_src[(j-2)*src_wstride]); 
			sum += (((j-1)<0)? p_src[0]:p_src[(j-1)*src_wstride]); 
			sum += p_src[j*src_wstride]; 
			sum += (((j+1)>= height)? p_src[(height-1)*src_wstride]:p_src[(j+1)*src_wstride]); 
			sum += (((j+2)>= height)? p_src[(height-1)*src_wstride]:p_src[(j+2)*src_wstride]); 
			sum += (((j+3)>= height)? p_src[(height-1)*src_wstride]:p_src[(j+3)*src_wstride]);
			p_dst[j*dst_wstride] = (uint16_t)((sum + RADIUS) / D);
		}
    }
	return;
}

void box_mean_uint8_row_uv_c(uint8_t *p_src,uint16_t *p_dst, int32_t param1, int32_t* param2)
{
    int32_t i,j,width,height,src_wstride,dst_wstride;
	uint16_t	sum_v,sum_u;
	
	width = param1;
	height = param2[0];
	src_wstride = param2[1];
	dst_wstride = param2[2];
	
    // row integral
    for (j =0; j < height; j++, p_src += src_wstride, p_dst += dst_wstride)
	{
		for(i = 0; i < width; i+=2)
		{
			sum_v = (((i-4)<0)? p_src[0]:p_src[i-4]);
			sum_u = (((i-3)<0)? p_src[1]:p_src[i-3]); 
			sum_v += (((i-2)<0)? p_src[0]:p_src[i-2]); 
			sum_u += (((i-1)<0)? p_src[1]:p_src[i-1]); 
			sum_v += p_src[i];
			sum_u += p_src[i+1];
			sum_v += (((i+2)>= width)? p_src[width-2]:p_src[i+2]); 
			sum_u += (((i+3)>= width)? p_src[width-1]:p_src[i+3]); 
			p_dst[i] = sum_v;
			p_dst[i+1] = sum_u;
		}
    }
	return;
}

void box_mean_uint8_col_uv_c(uint16_t *p_src,uint8_t *p_dst,int32_t param1,int32_t* param2)
{
    int32_t	j,i,width,height,src_wstride,dst_wstride;
    int32_t sum;

	width = param1;
	height = param2[0];
	src_wstride = param2[1];
	dst_wstride = param2[2];

    // column integral
    for(i = 0; i < width; i++,p_src++,p_dst++)
	{
        for(j = 0; j < height; j++)
		{
			sum = (((j-2)<0)? p_src[0]:p_src[(j-2)*src_wstride]); 
			sum += (((j-1)<0)? p_src[0]:p_src[(j-1)*src_wstride]); 
			sum += p_src[j*src_wstride]; 
			sum += (((j+1)>= height)? p_src[(height-1)*src_wstride]:p_src[(j+1)*src_wstride]);
			p_dst[j*dst_wstride] = (uint8_t)((sum + N_UV/2) / N_UV);
		}
    }
}

void box_mean_uint16_row_uv_c(uint16_t *p_src,uint16_t *p_dst,int32_t param1,int32_t* param2)
{
    int32_t i,j,width,height,src_wstride,dst_wstride;
	int32_t	sum_v,sum_u;
	
	width = param1;
	height = param2[0];
	src_wstride = param2[1];
	dst_wstride = param2[2];
	
    // row integral
    for (j =0; j < height; j++, p_src += src_wstride, p_dst += dst_wstride)
	{
		for(i = 0; i < width; i+=2)
		{
			sum_v = (((i-4)<0)? p_src[0]:p_src[i-4]);
			sum_u = (((i-3)<0)? p_src[1]:p_src[i-3]); 
			sum_v += (((i-2)<0)? p_src[0]:p_src[i-2]); 
			sum_u += (((i-1)<0)? p_src[1]:p_src[i-1]); 
			sum_v += p_src[i];
			sum_u += p_src[i+1];
			sum_v += (((i+2)>= width)? p_src[width-2]:p_src[i+2]); 
			sum_u += (((i+3)>= width)? p_src[width-1]:p_src[i+3]); 
			p_dst[i] = (uint16_t)((sum_v + RADIUS_UV) / D_UV);
			p_dst[i+1] = (uint16_t)((sum_u + RADIUS_UV) / D_UV);
		}
    }
	return;
}

void box_mean_uint16_col_uv_c(uint16_t *p_src,uint16_t *p_dst,int32_t param1,int32_t* param2)
{
    int32_t	j,i,width,height,src_wstride,dst_wstride;
    int32_t sum;

	width = param1;
	height = param2[0];
	src_wstride = param2[1];
	dst_wstride = param2[2];

    // column integral
    for(i = 0; i < width; i++,p_src++,p_dst++)
	{
        for(j = 0; j < height; j++)
		{
			sum = (((j-2)<0)? p_src[0]:p_src[(j-2)*src_wstride]); 
			sum += (((j-1)<0)? p_src[0]:p_src[(j-1)*src_wstride]); 
			sum += p_src[j*src_wstride]; 
			sum += (((j+1)>= height)? p_src[(height-1)*src_wstride]:p_src[(j+1)*src_wstride]);
			p_dst[j*dst_wstride] = (uint16_t)((sum + RADIUS_UV) / D_UV);
		}
    }
}

/*int32_t	box_mean_uint8_y_c(uint8_t *p_src, uint8_t *p_dst,int32_t param1, int32_t param2)
{
	int32_t width,height;
	uint16_t	*p_tmp = NULL;

	width = param1 &0xFFFF;
	height = param2 & 0xFFFF;
	p_tmp = (uint16_t*)malloc_aligned(width*height*sizeof(uint16_t),32);
	if(p_tmp == NULL)
		return -1;
	
	box_mean_uint8_row_y_c(p_src,p_tmp,param1,height);
	box_mean_uint8_col_y_c(p_tmp,p_dst,width,param2);

	free_aligned(p_tmp);
	return 0;
}

int32_t	box_mean_uint16_y_c(uint16_t *p_src, uint16_t *p_dst,int32_t param1, int32_t param2)
{
	int32_t width,height;
	uint16_t	*p_tmp = NULL;

	width = param1 &0xFFFF;
	height = param2 & 0xFFFF;
	p_tmp = (uint16_t*)malloc_aligned(width*height*sizeof(uint16_t),32);
	if(p_tmp == NULL)
		return -1;
	
	box_mean_uint16_row_y_c(p_src,p_tmp,param1,height);
	box_mean_uint16_col_y_c(p_tmp,p_dst,width,param2);

	free_aligned(p_tmp);
	return 0;
}

int32_t	box_mean_uint8_uv_c(uint8_t *p_src, uint8_t *p_dst,int32_t param1, int32_t param2)
{
	int32_t width,height;
	uint16_t	*p_tmp = NULL;

	width = param1 &0xFFFF;
	height = param2 & 0xFFFF;
	p_tmp = (uint16_t*)malloc_aligned(width*height*sizeof(uint16_t),32);
	if(p_tmp == NULL)
		return -1;
	
	box_mean_uint8_row_uv_c(p_src,p_tmp,param1,height);
	box_mean_uint8_col_uv_c(p_tmp,p_dst,width,param2);

	free_aligned(p_tmp);
	return 0;
}

int32_t	box_mean_uint16_uv_c(uint16_t *p_src, uint16_t *p_dst,int32_t param1, int32_t param2)
{
	int32_t width,height;
	uint16_t	*p_tmp = NULL;

	width = param1 &0xFFFF;
	height = param2 & 0xFFFF;
	p_tmp = (uint16_t*)malloc_aligned(width*height*sizeof(uint16_t),32);
	if(p_tmp == NULL)
		return -1;
	
	box_mean_uint16_row_uv_c(p_src,p_tmp,param1,height);
	box_mean_uint16_col_uv_c(p_tmp,p_dst,width,param2);

	free_aligned(p_tmp);
	return 0;
}*/

void calc_square_c(uint8_t *p_src,uint16_t *p_dst,int32_t param1,int32_t *param2)
{
	int32_t i,j,width,height,src_wstride,dst_wstride;
	width = param1;
	height = param2[0];
	src_wstride = param2[1];
	dst_wstride = param2[2];

	for(j = 0; j < height; j++,p_src+=src_wstride,p_dst+=dst_wstride)
		for(i = 0; i < width; i+= 4)
		{
			p_dst[i] = p_src[i]*p_src[i];
			p_dst[i+1] = p_src[i+1]*p_src[i+1];
			p_dst[i+2] = p_src[i+2]*p_src[i+2];
			p_dst[i+3] = p_src[i+3]*p_src[i+3];
		}

	return;
}

void calc_ab_c(uint16_t *p_mean_II,uint8_t *p_mean_I,uint8_t *p_lookup_a,int32_t *param)
{
	uint8_t	*p_a;
	uint16_t *p_b;
	int32_t i,j,temp,mean_II_wstride,mean_I_wstride,a_wstride,b_wstride,width,height;

	p_a = (uint8_t*)param[0];
	p_b = (uint16_t*)param[1];
	width = param[2];
	height = param[3];
	mean_II_wstride = param[4];
	mean_I_wstride = param[5];
	a_wstride = param[6];
	b_wstride = param[7];

	for(j = 0; j < height; j++)
	{
		for(i = 0; i < width; i++)
		{
			temp  = ZXCORE_MAX(p_mean_II[i] - p_mean_I[i] * p_mean_I[i], 0);
			if(temp>=16384)
				temp = 16383;
			temp = p_lookup_a[temp];
			p_b[i] = (uint16_t)(((p_mean_I[i]*(GF_A_SCALE-temp)+GF_B_SHIFT*GF_A_SCALE)*GF_B_SCALE+
					(GF_A_SCALE/2))/GF_A_SCALE);
			p_a[i] = (uint8_t)temp;
		}
		p_a+=a_wstride;
		p_b+=b_wstride;
		p_mean_II += mean_II_wstride;
		p_mean_I += mean_I_wstride;
	}
	return;
}

void calc_dst_c(uint8_t *p_mean_a,uint16_t *p_mean_b,uint8_t *p_src,int32_t *param)
{
	int32_t i,j,temp,width,height,mean_a_wstride,mean_b_wstride,src_wstride,dst_wstride;
	uint8_t *p_dst;

	p_dst = (uint8_t*)param[0];
	width = param[1];
	height = param[2];
	mean_a_wstride = param[3];
	mean_b_wstride = param[4];
	src_wstride = param[5];
	dst_wstride = param[6];

	for(j = 0; j < height; j++)
	{
		for(i = 0; i < width; i++)
		{
			temp = ( p_mean_a[i] * p_src[i] * GF_B_SCALE + p_mean_b[i] * GF_A_SCALE);
			temp = ( temp + GF_A_SCALE*GF_B_SCALE/2)/GF_A_SCALE/GF_B_SCALE - GF_B_SHIFT;
			p_dst[i] = (uint8_t)(ZXCORE_CLIP(temp,0,255));
		}
		p_src+=src_wstride;
		p_dst+=dst_wstride;
		p_mean_a += mean_a_wstride;
		p_mean_b += mean_b_wstride;
    }

	return;
}


void halve_image_uint8(uint8_t *p_large, uint8_t *p_small, int32_t width, int32_t height, int32_t isUV)
{
    int32_t i,j,ii;

    if ( (width&0x01) || (height&0x01) )
    {
        printf("invalid downsampling factor!\n");
        exit(-1);
    }

    if (!isUV) // y channel
    {
	    for(j = 0; j < height; j+=2)
	    {
		    for(i = 0, ii = 0; i < width; i+=2, ii++)
		    {
			    p_small[ii] = p_large[i];
		    }
		    p_large += width * 2;
            p_small += width / 2;
        }
    }
    else       // uv channel
    {
	    for(j = 0; j < height; j+=2)
	    {
		    for(i = 0, ii = 0; i < width; i+=4, ii+=2)
		    {
			    p_small[ii]   = p_large[i];
                p_small[ii+1] = p_large[i+1];
		    }
		    p_large += width * 2;
            p_small += width / 2;
        }
    }

    return;
}

void double_image_uint8(uint8_t *p_small, uint8_t *p_large, int32_t width, int32_t height, int32_t isUV) // w/h are for the large size
{
    int32_t i,j,ii;

    if ( (width&0x01) || (height&0x01) )
    {
        printf("invalid downsampling factor!\n");
        exit(-1);
    }

    if (!isUV)
    {
        // even lines
	    for(j = 0; j < height; j+=2)
	    {
		    for(i = 0, ii = 0; i < width-2; i+=2, ii++)
		    {
			    p_large[i] = p_small[ii];
                p_large[i+1] = (p_small[ii] + p_small[ii+1] + 1) / 2;
		    }
            p_large[width-2] = p_large[width-1] = p_small[width/2-1];

		    p_large += width * 2;
            p_small += width / 2;
        }

        // odd lines
        p_large = p_large - width * (height - 1);
	    for(j = 1; j < height-1; j+=2)
	    {
            for(i = 0; i < width; i++)
		    {
			    p_large[i] = ( p_large[i-width] + p_large[i+width] + 1) / 2;
		    }
		    p_large += width*2;
        }
        memcpy(p_large, p_large-width, width*sizeof(uint8_t));
    }
    else
    {
        // even lines
	    for(j = 0; j < height; j+=2)
	    {
		    for(i = 0, ii = 0; i < width-2; i+=4, ii+=2)
		    {
			    p_large[i]   =  p_small[ii];
			    p_large[i+1] =  p_small[ii+1];
                p_large[i+2] = (p_small[ii]   + p_small[ii+2] + 1) / 2;
                p_large[i+3] = (p_small[ii+1] + p_small[ii+3] + 1) / 2;
		    }
            p_large[width-4] = p_large[width-2] = p_small[width/2-2];
            p_large[width-3] = p_large[width-1] = p_small[width/2-1];

		    p_large += width * 2;
            p_small += width / 2;
        }

        // odd lines
        p_large = p_large - width * (height - 1);
	    for(j = 1; j < height-1; j+=2)
	    {
            for(i = 0; i < width; i++)
		    {
			    p_large[i] = ( p_large[i-width] + p_large[i+width] + 1) / 2;
		    }
		    p_large += width*2;
        }
        memcpy(p_large, p_large-width, width*sizeof(uint8_t));
    }
    return;
}

void double_image_uint16(uint16_t *p_small, uint16_t *p_large, int32_t width, int32_t height, int32_t isUV) // w/h are for the large size
{
    int32_t i,j,ii;

    if ( (width&0x01) || (height&0x01) )
    {
        printf("invalid downsampling factor!\n");
        exit(-1);
    }

    if (!isUV)
    {
        // even lines
	    for(j = 0; j < height; j+=2)
	    {
		    for(i = 0, ii = 0; i < width-2; i+=2, ii++)
		    {
			    p_large[i] = p_small[ii];
                p_large[i+1] = (p_small[ii] + p_small[ii+1] + 1) / 2;
		    }
            p_large[width-2] = p_large[width-1] = p_small[width/2-1];

		    p_large += width * 2;
            p_small += width / 2;
        }

        // odd lines
        p_large = p_large - width * (height - 1);
	    for(j = 1; j < height-1; j+=2)
	    {
            for(i = 0; i < width; i++)
		    {
			    p_large[i] = ( p_large[i-width] + p_large[i+width] + 1) / 2;
		    }
		    p_large += width*2;
        }
        memcpy(p_large, p_large-width, width*sizeof(uint16_t));
    }
    else
    {
        // even lines
	    for(j = 0; j < height; j+=2)
	    {
		    for(i = 0, ii = 0; i < width-2; i+=4, ii+=2)
		    {
			    p_large[i]   =  p_small[ii];
			    p_large[i+1] =  p_small[ii+1];
                p_large[i+2] = (p_small[ii]   + p_small[ii+2] + 1) / 2;
                p_large[i+3] = (p_small[ii+1] + p_small[ii+3] + 1) / 2;
		    }
            p_large[width-4] = p_large[width-2] = p_small[width/2-2];
            p_large[width-3] = p_large[width-1] = p_small[width/2-1];

		    p_large += width * 2;
            p_small += width / 2;
        }

        // odd lines
        p_large = p_large - width * (height - 1);
	    for(j = 1; j < height-1; j+=2)
	    {
            for(i = 0; i < width; i++)
		    {
			    p_large[i] = ( p_large[i-width] + p_large[i+width] + 1) / 2;
		    }
		    p_large += width*2;
        }
        memcpy(p_large, p_large-width, width*sizeof(uint16_t));
    }
    return;
}
