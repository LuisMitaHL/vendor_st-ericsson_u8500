#include "ZXImgCoreDef.h"
#include "ZXImgCommon.h"
#include "ZXImgFilterClahe.h"
#include "ZXImgFilterGuided.h"
#include "ZXImgColorCvt.h"
#include "ZXImgEnhance.h"

#define IMGEE_IS_VALID(x)		((x) & (0xFF))
#define IMGEE_IS_FAST(x)		((x) & (IMGEE_MODE_FAST))
#define IMGEE_GET_MODE(x)		((x) & (0x0FFF))

typedef	struct ZXImgEEFilter_T
{
	uint32_t pCFltHandle;
	uint32_t pGFltHandle;
	int32_t	format;
	uint8_t	*p_src;
	int32_t	size;
}ZXImgEEFilter;

int ZXImgEnhanceInit(int* p_handle,int format,int nMode)
{
    ZXImgEEFilter* pFilter = NULL;
	int32_t	ret = ZXIMGCORE_OK, eps0_y, eps1_y, eps0_uv, eps1_uv;
    float_t         fSaturation, fContrast;
    double_t        lumda0_y, lumda1_y, lumda0_uv, lumda1_uv;

	//#0 check the nMode parameter
	*p_handle = 0;
	if(!IMGEE_IS_VALID(nMode))
		return	ZXIMGCORE_PARA_ERR;
	if(((nMode&IMGEE_MODE_AUTO)==0)&&
		((nMode&IMGEE_MODE_HDR)==0)&&
		((nMode&IMGEE_MODE_BEAUTY)==0)&&
		((nMode&IMGEE_MODE_SHARPEN)==0)&&
		((nMode&IMGEE_MODE_DENOISE)==0)&&
		((nMode&IMGEE_MODE_SHARPEN_EX)==0))
		return ZXIMGCORE_PARA_ERR;
	//#1 malloc memory space 
	pFilter = (ZXImgEEFilter*)malloc_aligned(sizeof(ZXImgEEFilter),4);
	if(pFilter==NULL)
	{
		ret = ZXIMGCOER_MEM_ERR;
		goto _error;
	}
	memset(pFilter,0,sizeof(ZXImgEEFilter));
	//#1.1 malloc memory space for CLAHE
	if((nMode&IMGEE_MODE_AUTO)||(nMode&IMGEE_MODE_HDR)||(nMode&IMGEE_MODE_BEAUTY))
	{
        switch(IMGEE_GET_MODE(nMode))
        {
        case IMGEE_MODE_AUTO:
            fSaturation = 1.6f;
            fContrast   = 1.7f;
            break;
        case IMGEE_MODE_HDR:
            fSaturation = 1.6f;
            fContrast   = 2.5f;
            break;
        case IMGEE_MODE_BEAUTY:
            fSaturation = 1.5f;
            fContrast   = 1.7f;
            break;
        default:
            fSaturation = 1.6f;
            fContrast   = 1.7f;
            break;
        };

        ret = sndaClaheFilterInit(&pFilter->pCFltHandle,4,4,256,
                                fSaturation, fContrast,1);
		if(ret!=CLAHE_FILTER_OK)
		{
			ret = ZXIMGCORE_INITCLAHE_ERR;
			goto _error;
		}
	}
    //1.2 malloc memory for Guided Filter
	if(!(IMGEE_IS_FAST(nMode)))
	{
		switch(IMGEE_GET_MODE(nMode))
		{
			case IMGEE_MODE_AUTO:
            
					eps0_y  = 8;    lumda0_y  = 1.00;
					eps1_y  = 8192; lumda1_y  = 1.65;
					eps0_uv = 64;   lumda0_uv = 0.25;
					eps1_uv = 8192; lumda1_uv = 1.00;
					break;
			case IMGEE_MODE_HDR:
					eps0_y  = 32;   lumda0_y  = 0.50;
					eps1_y  = 8192; lumda1_y  = 1.50;
					eps0_uv = 64;   lumda0_uv = 0.25;
					eps1_uv = 8192; lumda1_uv = 1.00;
					break;
			case IMGEE_MODE_BEAUTY:
					eps0_y  = 128;  lumda0_y  = 0.50;
					eps1_y  = 8192; lumda1_y  = 1.75;  //1.75
					eps0_uv = 128;  lumda0_uv = 0.00;
					eps1_uv = 8192; lumda1_uv = 1.50;  //1.5
					break;
			default: // same as auto
					eps0_y  = 8;    lumda0_y  = 1.00;
					eps1_y  = 8192; lumda1_y  = 1.50;
					eps0_uv = 64;   lumda0_uv = 0.25;
					eps1_uv = 8192; lumda1_uv = 1.00;
					break;
		}
		ret = sndaGuidedFilterInit(&pFilter->pGFltHandle,eps0_y,eps1_y,eps0_uv, eps1_uv,
									(float_t)lumda0_y,(float_t)lumda1_y,(float_t)lumda0_uv,(float_t)lumda1_uv,2);
		if(ret != GUIDED_FILTER_OK)
		{
			ret = ZXIMGCORE_INITGUIDED_ERR;
			goto _error;
		}
	}
	pFilter->format = format;
	*p_handle = (int32_t)pFilter;
    return ZXIMGCORE_OK;

_error:
	if(pFilter)
	{
		if(pFilter->pGFltHandle)
		{
			sndaGuidedFilterRelease(pFilter->pGFltHandle);
			pFilter->pGFltHandle = 0;
		}
		if(pFilter->pCFltHandle)
		{
			sndaClaheFilterRelease(pFilter->pCFltHandle);
			pFilter->pCFltHandle = 0;
		}
		free_aligned(pFilter);
		pFilter = NULL;
	}
	return ret;
}

void	ZXImgEnhanceRelease(int handle)
{    
	ZXImgEEFilter* pFilter = (ZXImgEEFilter*)handle;

	if(pFilter)
	{
		if(pFilter->p_src)
		{
			free_aligned(pFilter->p_src);
			pFilter->p_src = NULL;
		}
		if(pFilter->pGFltHandle)
		{
			sndaGuidedFilterRelease(pFilter->pGFltHandle);
			pFilter->pGFltHandle = 0;
		}
		if(pFilter->pCFltHandle)
		{
			sndaClaheFilterRelease(pFilter->pCFltHandle);
			pFilter->pCFltHandle = 0;
		}
		free_aligned(pFilter);
		pFilter = NULL;
	}
	return;
}

int32_t	ZXImgEnhanceProcess(int32_t Handle,uint8_t* pData,int32_t nImgW,int32_t nImgH)
{
	ZXImgEEFilter* pFilter = (ZXImgEEFilter*)Handle;
	int32_t ret;
	uint8_t	*p_data;
	//#0 check the data safty
	if((nImgW&0xF)||(nImgH&7))
		return ZXIMGCORE_PARA_ERR;
	//#1 color convert
	if(pFilter->format!=IMGEE_FORMAT_YUVNV21)
	{
		if(pFilter->format==IMGEE_FORMAT_S1)
			ret = nImgW * nImgH * 3 / 2;
		if(pFilter->size != ret)
		{
			if(pFilter->p_src)
			{
				free_aligned(pFilter->p_src);
				pFilter->p_src = NULL;
			}
			pFilter->p_src = (uint8_t*)malloc_aligned(ret,32);
			if(!pFilter->p_src)
				goto _error;
		}
		s1format_2_yuvnv12(pData,pFilter->p_src,nImgW,nImgH);
		p_data = pFilter->p_src;
	}
	else
		p_data = pData;

	//#2 Filter
	if(pFilter->pGFltHandle)
	{
		ret = sndaGuidedFilterProcess(pFilter->pGFltHandle,p_data,p_data,nImgW,nImgH);
		if(ret != GUIDED_FILTER_OK)
			return ZXIMGCORE_FAILED;
	}
	
	if(pFilter->pCFltHandle)
	{
		ret = sndaClaheFilterProcess(pFilter->pCFltHandle,p_data,p_data,nImgW,nImgH);
		if(ret != CLAHE_FILTER_OK)
			return ZXIMGCORE_FAILED;
	}
	//#1 color convert
	if(pFilter->format!=IMGEE_FORMAT_YUVNV21)
	{
		yuvnv12_2_s1format(p_data,pData,nImgW,nImgH);
	}

    return ZXIMGCORE_OK;
_error:
	return ZXIMGCORE_FAILED;
}