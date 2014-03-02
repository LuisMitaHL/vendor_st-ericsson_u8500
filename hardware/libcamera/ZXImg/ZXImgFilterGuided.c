#include <math.h>
#include "ZXImgCommon.h"
#include "ZXImgFilterGuided.h"
#if	USE_ANDROID_ASM
#include "arm.h"
#endif

#define	LOOKUP_TABLE_SIZE	16384
#define GF_LUMDA_SCALE		4

void build_lookup_table(GuidedFilter *pFilter, int32_t eps0, int32_t eps1, float_t lumda0, float_t lumda1, int32_t isUV) 
{
	uint32_t sigma2, a0, a1;
	int32_t temp;
	uint32_t LUMDA0 = (uint32_t)(lumda0*GF_LUMDA_SCALE+0.5);
	uint32_t LUMDA1 = (uint32_t)(lumda1*GF_LUMDA_SCALE+0.5);

    uint8_t *table = isUV ? (pFilter->p_lookup_a[1]) : (pFilter->p_lookup_a[0]);
    if(eps0 == 0)//sharpen only
	{
		a0 = GF_A_SCALE;
		for (sigma2=0; sigma2<16384; sigma2++)
		{
			a1 = ( sigma2 * GF_A_SCALE + (sigma2+eps1) / 2 ) / (sigma2 + eps1);
			temp = ( GF_A_SCALE * LUMDA0 + a0 * (LUMDA1 - LUMDA0) + a1 * (GF_LUMDA_SCALE-LUMDA1) + GF_LUMDA_SCALE/2 ) / GF_LUMDA_SCALE;
			table[sigma2] = (uint8_t)temp;
		}
	}
	else 
		if(eps0 == eps1)//denoise only
		{ 
			for (sigma2=0; sigma2<16384; sigma2++) 
			{
				a0 = ( sigma2 * GF_A_SCALE + (sigma2+eps0) / 2 ) / (sigma2 + eps0);
				a1 = a0;
				temp = ( GF_A_SCALE * LUMDA0 + a0 * (LUMDA1 - LUMDA0) + a1 * (GF_LUMDA_SCALE-LUMDA1) + GF_LUMDA_SCALE/2 ) / GF_LUMDA_SCALE;
				table[sigma2] = (uint8_t)temp;
			}
		}
		else
		{
			for (sigma2=0; sigma2<16384; sigma2++) 
			{
				a0 = ( sigma2 * GF_A_SCALE + (sigma2+eps0) / 2 ) / (sigma2 + eps0);
				a1 = ( sigma2 * GF_A_SCALE + (sigma2+eps1) / 2 ) / (sigma2 + eps1);
				temp = ( GF_A_SCALE*LUMDA0 + a0*(LUMDA1-LUMDA0) + a1*(GF_LUMDA_SCALE-LUMDA1) + GF_LUMDA_SCALE/2 ) / GF_LUMDA_SCALE;
				table[sigma2] = (uint8_t)temp;
			}
		}
}

static	int32_t	sndaGuidedFilterMalloc(GuidedFilter	*pFilter, int width, int height, int threads_cnt)
{
	//#free the memory space
	if(pFilter->p_buf[0])
	{
		free_aligned(pFilter->p_buf[0]);
		pFilter->p_buf[0] = NULL;
	}
	if(pFilter->p_buf[1])
	{
		free_aligned(pFilter->p_buf[1]);
		pFilter->p_buf[1] = NULL;
	}
	//#malloc the memory space
	if(threads_cnt == 1)
		pFilter->p_buf[0] = (uint8_t*)malloc_aligned(height*width*5,32);
	else
	{
		pFilter->p_buf[0] = (uint8_t*)malloc_aligned((height/2+8)*width*5,32);
		pFilter->p_buf[1] = (uint8_t*)malloc_aligned((height/2+8)*width*5,32);
	}
	if(
		((threads_cnt==1)&&(pFilter->p_buf[0]==NULL))||
		((threads_cnt==2)&&((pFilter->p_buf[0]==NULL)||(pFilter->p_buf[1]==NULL))))
	{
		LOGE("sndaGuidedFilterMalloc error: pFilter->buf malloc!\n");
		goto	_error;
	}

	return GUIDED_FILTER_OK;
_error:
	if(pFilter->p_buf[0])
	{
		free_aligned(pFilter->p_buf[0]);
		pFilter->p_buf[0] = NULL;
	}
	if(pFilter->p_buf[1])
	{
		free_aligned(pFilter->p_buf[1]);
		pFilter->p_buf[1] = NULL;
	}
	return	GUIDED_FILTER_MEMERR;
}

int32_t	sndaGuidedFilterInit(uint32_t *pHandle,int32_t eps0_y, int32_t eps1_y,
							int32_t eps0_uv, int32_t eps1_uv,
							float_t lumda0_y, float_t lumda1_y, 
							float_t lumda0_uv, float_t lumda1_uv, 
							int32_t threads_cnt) 
{
    GuidedFilter	*pFilter = NULL;
	int32_t	ret = GUIDED_FILTER_OK;

	//#0 check the width|height availiable
	if((threads_cnt>2)||(threads_cnt<0))
	{
		LOGE("sndaGuidedFilterInit error:threads_cnt!\n");
		ret = GUIDED_FILTER_PARAMERR;
		goto	_error;
	}
	if(eps0_y > eps1_y || eps0_uv > eps1_uv)
	{
		LOGE("sndaGuidedFilterInit error: eps0 > eps1!\n");
		ret = GUIDED_FILTER_PARAMERR;
		goto	_error;
	}

    //#1 allocate handle memory
    pFilter = (GuidedFilter *)malloc_aligned(sizeof(GuidedFilter), 16);
	if(pFilter==NULL)
	{
		LOGE("sndaGuidedFilterInit error: pFilter malloc!\n");
		ret = GUIDED_FILTER_WHERR;
		goto	_error;
	}
	memset(pFilter,0,sizeof(GuidedFilter));
	if(eps0_y == 0 && eps1_y == 0)// no operation needed
		pFilter->only_cpy[0] = 1;
	if(eps0_uv == 0 && eps1_uv == 0)// no operation needed
		pFilter->only_cpy[1] = 1;
    
    if(!pFilter->only_cpy[0])
    {
	    pFilter->p_lookup_a[0] = (uint8_t*)malloc_aligned(LOOKUP_TABLE_SIZE,32);
	    if(pFilter->p_lookup_a[0]==NULL)
	    {
		    LOGE("sndaGuidedFilterInit error: pFilter->p_lookup_a_y malloc!\n");
		    ret = GUIDED_FILTER_MEMERR;
		    goto	_error;
	    }
        build_lookup_table(pFilter, eps0_y,  eps1_y,  lumda0_y,  lumda1_y,  0);
    }
    if(!pFilter->only_cpy[1])
    {
	    pFilter->p_lookup_a[1] = (uint8_t*)malloc_aligned(LOOKUP_TABLE_SIZE,32);
	    if(pFilter->p_lookup_a[1]==NULL)
	    {
		    LOGE("sndaGuidedFilterInit error: pFilter->p_lookup_a_uv malloc!\n");
		    ret = GUIDED_FILTER_MEMERR;
		    goto	_error;
	    }
        build_lookup_table(pFilter, eps0_uv, eps1_uv, lumda0_uv, lumda1_uv, 1);
    }//if(!pFilter->only_cpy)
	//#2 reserve the width&heigh and return
	pFilter->threads_cnt = threads_cnt;
	//#3 set the functions pointers
#if	USE_ANDROID_ASM
	pFilter->box_row_func_uint8[0] = _box_mean_uint8_row_y_neon;
	pFilter->box_row_func_uint8[1] = _box_mean_uint8_row_uv_neon;
	pFilter->box_row_func_uint16[0] = _box_mean_uint16_row_y_neon;
	pFilter->box_row_func_uint16[1] = _box_mean_uint16_row_uv_neon;
	pFilter->box_col_func_uint8[0] = _box_mean_uint8_col_y_neon;
	pFilter->box_col_func_uint8[1] = _box_mean_uint8_col_uv_neon;
	pFilter->box_col_func_uint16[0] = _box_mean_uint16_col_y_neon;
	pFilter->box_col_func_uint16[1] = _box_mean_uint16_col_uv_neon;

	pFilter->calc_square = _calc_square_neon;
	pFilter->calc_ab = _calc_ab_neon;
	pFilter->calc_dst = _calc_dst_neon;
#else
	pFilter->box_row_func_uint8[0] = box_mean_uint8_row_y_c;
	pFilter->box_row_func_uint8[1] = box_mean_uint8_row_uv_c;
	pFilter->box_row_func_uint16[0] = box_mean_uint16_row_y_c;
	pFilter->box_row_func_uint16[1] = box_mean_uint16_row_uv_c;
	pFilter->box_col_func_uint8[0] = box_mean_uint8_col_y_c;
	pFilter->box_col_func_uint8[1] = box_mean_uint8_col_uv_c;
	pFilter->box_col_func_uint16[0] = box_mean_uint16_col_y_c;
	pFilter->box_col_func_uint16[1] = box_mean_uint16_col_uv_c;
	pFilter->calc_square = calc_square_c;
	pFilter->calc_ab = calc_ab_c;
	pFilter->calc_dst = calc_dst_c;
#endif
	*pHandle = (uint32_t)pFilter;
	return ret;

_error:
	if(pFilter)
	{
		if(pFilter->p_lookup_a[0])
		{
			free_aligned(pFilter->p_lookup_a[0]);
			pFilter->p_lookup_a[0] = NULL;
		}
		if(pFilter->p_lookup_a[1])
		{
			free_aligned(pFilter->p_lookup_a[1]);
			pFilter->p_lookup_a[1] = NULL;
		}
		if(pFilter->p_buf[0])
		{
			free_aligned(pFilter->p_buf[0]);
			pFilter->p_buf[0] = NULL;
		}
		if(pFilter->p_buf[1])
		{
			free_aligned(pFilter->p_buf[1]);
			pFilter->p_buf[1] = NULL;
		}
		free_aligned(pFilter);
	}

    return ret;
}

void	sndaGuidedFilterRelease(uint32_t Handle)
{
	GuidedFilter	*pFilter = (GuidedFilter*)Handle;

    if(pFilter)
	{
		if(pFilter->p_lookup_a[0])
		{
			free_aligned(pFilter->p_lookup_a[0]);
			pFilter->p_lookup_a[0] = NULL;
		}
		if(pFilter->p_lookup_a[1])
		{
			free_aligned(pFilter->p_lookup_a[1]);
			pFilter->p_lookup_a[1] = NULL;
		}
		if(pFilter->p_buf[0])
		{
			free_aligned(pFilter->p_buf[0]);
			pFilter->p_buf[0] = NULL;
		}
		if(pFilter->p_buf[1])
		{
			free_aligned(pFilter->p_buf[1]);
			pFilter->p_buf[1] = NULL;
		}
		free_aligned(pFilter);
	}
	return;
}

static int32_t	single_thread_entry(GuidedFilter *pFilter,uint8_t *srcData,uint8_t *dstData)
{
    uint8_t  *mean_I, *a,  *mean_a, *p_src, *p_dst;
    uint16_t *mean_II, *buf1, *buf2,*b, *mean_b;
	int32_t	cpt,width,height,total_num,param[8];

	p_src = srcData;
	p_dst = dstData;
	width = pFilter->width;
	height = pFilter->height;
	total_num = width*height;
	for(cpt = 0; cpt < 2; cpt++,p_src+=total_num,p_dst+=total_num,height>>=1,total_num>>=1)
	{
		if(pFilter->only_cpy[cpt])
		{
			memcpy(p_dst,p_src,width*height);
			continue;
		}
		//#0 calculate the mean(I)
		mean_I	=	(uint8_t *)pFilter->p_buf[0];
		buf1	=	(uint16_t *)(pFilter->p_buf[0] + total_num);
		param[0] = height;
		param[1] = width;
		param[2] = width;
		pFilter->box_row_func_uint8[cpt](p_src,buf1,width,param);
		pFilter->box_col_func_uint8[cpt](buf1,mean_I,width,param);
		//#1 calculate the srcData*srcData;
		mean_II =	buf1;
		buf1	=	buf1 + width * 5;
		buf2	=	buf1 + width;
		pFilter->calc_square(p_src,buf2,width,param);
		//#2 calculate the mean(I.*I)
		pFilter->box_row_func_uint16[cpt](buf2,buf1,width,param);
		pFilter->box_col_func_uint16[cpt](buf1,mean_II,width,param);
		//#3 calculate a & b
		a = pFilter->p_buf[0];
		b = (uint16_t*)(a + total_num);
		param[0] = (int)a;
		param[1] = (int)b;
		param[2] = width;
		param[3] = height;
		param[4] = width;
		param[5] = width;
		param[6] = width;
		param[7] = width;
		pFilter->calc_ab(mean_II,mean_I,pFilter->p_lookup_a[cpt],param);
		//#4 calculate  mean_a mean_b
		buf1  = (uint16_t *)(pFilter->p_buf[0] + total_num*3);
		mean_a = a;
		mean_b = b;
		param[0] = height;
		param[1] = width;
		param[2] = width;
		pFilter->box_row_func_uint8[cpt](a,buf1,width,param);
		pFilter->box_col_func_uint8[cpt](buf1, mean_a,width,param);
		pFilter->box_row_func_uint16[cpt](b,buf1,width,param);
		pFilter->box_col_func_uint16[cpt](buf1, mean_b,width,param);
		//#5 calcualte dst
		param[0] = (int)p_dst;
		param[1] = width;
		param[2] = height;
		param[3] = width;
		param[4] = width;
		param[5] = width;
		param[6] = width;
		pFilter->calc_dst(mean_a,mean_b,p_src,param);
	}

	return GUIDED_FILTER_OK;
}

static void double_thread_entry(void *p_param)
{
	ThreadParam* p_th = (ThreadParam *)p_param;
	GuidedFilter *pFilter = (GuidedFilter *)(p_th->pFilter);
	uint8_t  *mean_I,*a,*mean_a,*p_src,*p_dst;
    uint16_t *mean_II, *buf1, *buf2,*b, *mean_b;
	int32_t	cpt,width,height,total_num,param[8];

	if(!p_th->thread_id)//0
	{
		p_src = p_th->p_src;
		p_dst = p_th->p_dst;
	}
	else//1
	{
		p_src = p_th->p_src + pFilter->width*(pFilter->height/2-8);
		p_dst = p_th->p_dst + pFilter->width*(pFilter->height/2);
	}

	width = pFilter->width;
	height = (pFilter->height>>1)+8;
	total_num = width * height;

	for(cpt = 0; cpt < 1; cpt++)
	{
		if((p_th->thread_id==0)&&pFilter->only_cpy[cpt])
		{
			int32_t tmp = pFilter->width*pFilter->height;
			if(!cpt)
				memcpy(p_th->p_dst,p_th->p_src,tmp);
			else
				memcpy(p_th->p_dst+tmp,p_th->p_src+tmp,tmp>>1);
			continue;
		}
		//#0 calculate the mean(I)
		mean_I	=	(uint8_t *)pFilter->p_buf[p_th->thread_id];
		buf1	=	(uint16_t *)(mean_I + total_num);
		param[0] = height;
		param[1] = width;
		param[2] = width;
		pFilter->box_row_func_uint8[cpt](p_src,buf1,width,param);
		pFilter->box_col_func_uint8[cpt](buf1,mean_I,width,param);
		//#1 calculate the srcData*srcData;
		mean_II =	buf1;
		buf1	=	buf1 + width * 5;
		buf2	=	buf1 + width;
		pFilter->calc_square(p_src,buf2,width,param);
		//#2 calculate the mean(I.*I)
		pFilter->box_row_func_uint16[cpt](buf2,buf1,width,param);
		pFilter->box_col_func_uint16[cpt](buf1,mean_II,width,param);
		//#3 calculate a & b
		a = pFilter->p_buf[p_th->thread_id];
		b = (uint16_t*)(a + total_num);
		param[0] = (int)a;
		param[1] = (int)b;
		param[2] = width;
		param[3] = height;
		param[4] = width;
		param[5] = width;
		param[6] = width;
		param[7] = width;
		pFilter->calc_ab(mean_II,mean_I,pFilter->p_lookup_a[cpt],param);
		//#4 calculate  mean_a mean_b
		buf1  = (uint16_t *)(pFilter->p_buf[p_th->thread_id] + total_num*3);
		mean_a = a;
		mean_b = b;
		param[0] = height;
		param[1] = width;
		param[2] = width;
		pFilter->box_row_func_uint8[cpt](a,buf1,width,param);
		pFilter->box_col_func_uint8[cpt](buf1, mean_a,width,param);
		pFilter->box_row_func_uint16[cpt](b,buf1,width,param);
		pFilter->box_col_func_uint16[cpt](buf1, mean_b,width,param);
		//#5 calcualte dst
		param[0] = (int)p_dst;
		param[1] = width;
		param[2] = height-(8>>cpt);
		param[3] = width;
		param[4] = width;
		param[5] = width;
		param[6] = width;
		total_num = (width<<(3-cpt))*p_th->thread_id;
		pFilter->calc_dst(mean_a+total_num,mean_b+total_num,p_src+total_num,param);
		if(!p_th->thread_id)
		{
			p_src += (width*pFilter->height);
			p_dst += (width*pFilter->height);
		}
		else
		{
			total_num = width*(pFilter->height>>2);
			p_src = p_th->p_src + total_num*5 - width*4;
			p_dst = p_th->p_dst + total_num*5;
		}
		height = pFilter->height/4 + 4;
		total_num = width * height;
	}
	p_th->thread_flag |= 1;
}

int32_t	sndaGuidedFilterProcess(uint32_t Handle,uint8_t *srcData,uint8_t *dstData,
								int width, int height)
{
	GuidedFilter	*pFilter = (GuidedFilter*)Handle;
	int32_t	ret;

	//#0 check if malloc space
	if((width&0xF)||(height&0x7))
	{
		LOGE("sndaGuidedFilterProcess error:width|height!\n");
		ret = GUIDED_FILTER_PARAMERR;
		goto	_error;
	}
	if(!pFilter->only_cpy[0] || !pFilter->only_cpy[1])
	{
		if((pFilter->width!=width)||(pFilter->height!=height))
		{
			ret = sndaGuidedFilterMalloc(pFilter,width,height,pFilter->threads_cnt);
			if(ret != GUIDED_FILTER_OK)
			{
				pFilter->width = pFilter->height =0;
				goto _error;
			}
		}
		pFilter->width = width;
		pFilter->height = height;
	}

	if(pFilter->threads_cnt==1)
		ret = single_thread_entry(pFilter,srcData,dstData);
	else
	{
		ThreadParam th_p[2];
		pthread_t	th_h[2];
		//init parameters of threads
		th_p[0].threads_cnt = 2;
		th_p[0].thread_id = 0;
		th_p[0].pFilter = pFilter;
		th_p[0].p_src = srcData;
		th_p[0].p_dst = dstData;
		th_p[0].thread_flag = 0;
		th_p[1].threads_cnt = 2;
		th_p[1].thread_id = 1;
		th_p[1].pFilter = pFilter;
		th_p[1].p_src = srcData;
		th_p[1].p_dst = dstData;
		th_p[1].thread_flag = 0;
		//setup the threads
		CREATE_THREAD(th_h[0],double_thread_entry,&th_p[0]);
		CREATE_THREAD(th_h[1],double_thread_entry,&th_p[1]);
		if((th_h[0]==0)||(th_h[1]==0))
		{
			LOGE("setup threads are error\n");
			ret = GUIDED_FILTER_THSETUPERR;
			goto _error;
		}
		WAIT_THREAD(th_h[0],th_h[1],th_p[0].thread_flag,th_p[1].thread_flag,1);
	}
	return	GUIDED_FILTER_OK;
_error:
	return ret;
}