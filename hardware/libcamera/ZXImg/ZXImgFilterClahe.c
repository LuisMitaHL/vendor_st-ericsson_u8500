//////////////////////////////////////////////////////////////////////////
//                  CLAHE contrast enhancement
//////////////////////////////////////////////////////////////////////////

/*  
* ANSI C code from the article  
* "Contrast Limited Adaptive Histogram Equalization"  
* by Karel Zuiderveld, karel@cv.ruu.nl  
* in "Graphics Gems IV", Academic Press, 1994  
*  
*  
*  These functions implement Contrast Limited Adaptive Histogram Equalization.  
*  The main routine (CLAHE) expects an input image that is stored contiguously in  
*  memory;  the CLAHE output image overwrites the original input image and has the  
*  same minimum and maximum values (which must be provided by the user).  
*  This implementation assumes that the X- and Y image resolutions are an integer  
*  multiple of the X- and Y sizes of the contextual regions. A check on various other  
*  error conditions is performed.  
*  *  
*  The code is ANSI-C and is also C++ compliant.  
*/   
 
#include <math.h>
#include "ZXImgCommon.h"
#include "ZXImgFilterClahe.h"
//#include "arm.h"
const int32_t    uiMAX_REG_X = 32;      // max. # contextual regions in x-direction
const int32_t    uiMAX_REG_Y = 32;      // max. # contextual regions in y-direction
//////////////////////////////////////////////////////////////////////////
int32_t    sndaClaheFilterInit(uint32_t *pHandle,int32_t nBlockNumW,int32_t nBlockNumH,
							int32_t nHistBins,float_t fSaturation,float_t fContrast,int32_t threads_cnt)
{
    int32_t         i, j, nU, nV, nIdx;
    float_t         fU, fV;
    float_t         fNewS, fScale;
    int32_t         ret = CLAHE_FILTER_OK;
    ClaheFilter*    pFilter;

    *pHandle = 0;
    //#0 check parameters
    if((nBlockNumW>uiMAX_REG_X)||(nBlockNumH>uiMAX_REG_Y)||(nBlockNumW<2||nBlockNumH<2))
    {
        ret = CLAHE_FILTER_PARAMERR;
        goto    _error;
    }

    //#1 set some parameters
    if(nHistBins == 0)
        nHistBins = 256;

    //#2 malloc some space
    pFilter = (ClaheFilter*)malloc_aligned(sizeof(ClaheFilter),4);
    if(pFilter==NULL)
    {
        ret = CLAHE_FILTER_MEMERR;
        goto _error;
    }
    memset(pFilter,0,sizeof(ClaheFilter));
    pFilter->m_pHist = (int32_t*)malloc_aligned(nBlockNumW*nBlockNumH*nHistBins*sizeof(int32_t),16);
    if(pFilter->m_pHist==NULL)
    {
        ret = CLAHE_FILTER_MEMERR;
        goto _error;
    }
    memset(pFilter->m_pHist,0,nBlockNumW*nBlockNumH*nHistBins*sizeof(int32_t));
    pFilter->m_pLUTUV = (uint16_t*)malloc_aligned(65536*sizeof(uint16_t),8);
    if(pFilter->m_pLUTUV==NULL)
    {
        ret = CLAHE_FILTER_MEMERR;
        goto _error;
    }
    memset(pFilter->m_pLUTUV,0,65536*sizeof(uint16_t));

    fSaturation -= 1.0f;
    for(i = 0; i < 256; ++i)
    {
        for(j = 0; j < 256; ++j)
        {                        
            fU = (float_t)(i - 128.0f);
            fV = (float_t)(j - 128.0f);

            fNewS = (float_t)sqrt(fU * fU + fV * fV);
                       
            if(fNewS > 128.0f)
            {
                fScale = 128.0f * (1.0f - (fNewS - 128.0f) / 128.0f) / fNewS;
            }
            else
            {
                if(fNewS < 2.0f)
                {
                    fScale = 0;
                }
                else
                {
                    fScale = (fNewS * (1.0f + fSaturation)) >= 128.0f ? 
                        ((128.0f + fNewS) * 0.5f / fNewS) :  
                        (1.0f + (1.0f - fNewS / 128.0f) * fSaturation); 
                    
                    if(fNewS * fScale > 128.0f)
                    {
                        fScale = (256.0f - fNewS * fScale) / fNewS;
                    }
                }
            }

            fU *= fScale;
            fV *= fScale;

            nIdx = (i << 8) + j;

            nU = (int32_t)(fU + 128.0f);
            nU = (uint8_t)ZXCORE_CLIP(nU, 0, 255);
            nV = (int32_t)(fV + 128.0f);
            nV = (uint8_t)ZXCORE_CLIP(nV, 0, 255);
            pFilter->m_pLUTUV[nIdx] = (nU << 8) + nV;
        }
    }
    //#server some variables
    pFilter->m_nBlockNumW   = nBlockNumW;
    pFilter->m_nBlockNumH   = nBlockNumH;
    pFilter->m_nHistBins    = nHistBins;
    pFilter->fCliplimit     = fContrast;
	pFilter->threads_cnt    = threads_cnt;
    *pHandle                = (uint32_t)pFilter;
    return    CLAHE_FILTER_OK;

_error:
    if(pFilter)
    {
        if(pFilter->m_pLUTUV)
        {
            free_aligned(pFilter->m_pLUTUV);
            pFilter->m_pLUTUV = 0;
        }
        if(pFilter->m_pHist)
        {
            free_aligned(pFilter->m_pHist);
            pFilter->m_pHist = 0;
        }
        free_aligned(pFilter);
    }
    return ret;
}

void    sndaClaheFilterRelease(int32_t Handle)
{
    ClaheFilter    *pFilter = (ClaheFilter*)(Handle);

    if(pFilter)
    {
        if(pFilter->m_pLUTUV)
        {
            free_aligned(pFilter->m_pLUTUV);
            pFilter->m_pLUTUV = 0;
        }
        if(pFilter->m_pHist)
        {
            free_aligned(pFilter->m_pHist);
            pFilter->m_pHist = 0;
        }
        free_aligned(pFilter);
    }
    return ;
}

//This function performs clipping of the histogram and redistribution of bins.  
//The histogram is clipped and the number of excess pixels is counted. Afterwards  
//the excess pixels are equally redistributed across the whole histogram (providing  
//the bin count is smaller than the cliplimit)   
static void ZXCLAHEClipHistogram(int32_t* pHist, int32_t nHistBins, int32_t nClipLimit)
{   
    int32_t* pBinPointer;
    int32_t* pEndPointer;
    int32_t* pHisto;   
    int32_t nSumExcessLmt, nUpper, nBinIncr, i;   
    int32_t nBinExcess;   

    nSumExcessLmt = 0;  
    for (i = 0; i < nHistBins; i++)    
    { /* calculate total number of excess pixels */   
        nBinExcess = pHist[i] - nClipLimit;   
        if (nBinExcess > 0) 
            nSumExcessLmt += nBinExcess;      /* excess in current bin */   
    };   
       
    /* Second part: clip histogram and redistribute excess pixels in each bin */   
    nBinIncr = nSumExcessLmt / nHistBins;          /* average binincrement */   
    nUpper =  nClipLimit - nBinIncr;  /* Bins larger than nUpper set to cliplimit */   
       
    pHisto = pHist;
    for(i = 0; i < nHistBins; ++i)    
    {   
        if (*pHisto > nClipLimit) 
        {
            *pHisto = nClipLimit; /* clip bin */  
        }
        else    
        {   
            if (*pHisto > nUpper)    
            {   /* high bin count */    
                nSumExcessLmt -= nClipLimit - *pHisto; 
                *pHisto = nClipLimit;  
            }   
            else    
            {   /* low bin count */   
                nSumExcessLmt -= nBinIncr; 
                *pHisto += nBinIncr;   
            }   
        }  
        ++pHisto;
    }   

    pHisto = pHist;   
    pEndPointer = pHist + nHistBins;
       
    /* Redistribute remaining excess  */ 
    while (nSumExcessLmt)    
    {   
        for (pBinPointer = pHisto; pBinPointer < pEndPointer && nSumExcessLmt;   
            ++pBinPointer)    
        {   
            if (*pBinPointer < nClipLimit)    
            {   
                (*pBinPointer)++;  nSumExcessLmt--;    /* reduce excess */   
            }   
        }   
    }     
}   

// This function classifies the greylevels present in the array image into  
//a greylevel histogram. The pLookupTable specifies the relationship  
//between the greyvalue of the pixel (typically between 0 and 4095) and  
//the corresponding bin in the histogram (usually containing only 128 bins).   
static void ZXCLAHEMakeHistogram(uint8_t* pImg, uint32_t nImgW, int32_t nBlockW, int32_t nBlockH,   
                          int32_t* pHist, int32_t nHistBins)
{   
    int32_t  i, j;          
    int32_t  nWidthStep = ZXCORE_ALIGN_4(nImgW);
    uint8_t* pImgRow = pImg;   

    for(i = 0; i < nHistBins; ++i)    
        pHist[i] = 0; /* clear histogram */   
       
    for(i = 0; i < nBlockH; ++i)    
    {   
        for(j = 0; j < nBlockW; ++j)   
        {
            ++pHist[pImgRow[j]];
        }
        pImgRow += nWidthStep;   
    }  
}  

//This function calculates the equalized lookup table (mapping) by  
//cumulating the input histogram. Note: lookup table is rescaled in range [Min..Max].
static void ZXCLAHEMapHistogram (int32_t* pHist, uint8_t nMin, uint8_t nMax, int32_t nHistBins, int32_t nBlockPxlNum)   
                     
{      
    int32_t i;  
    int32_t nSum = 0;   
    float_t fScale = ((float_t)(nMax - nMin)) / nBlockPxlNum;  

    for (i = 0; i < nHistBins; i++)    
    {   
        nSum += *pHist;    
        *pHist=(int32_t)(nMin + nSum * fScale);   
        (*pHist > nMax) ? *pHist = nMax : 0; 
        ++pHist;
    } 
}  

static	void ZXCLAHEInterploateEntry(int *p_LU,int *p_LB,int *p_RU,int *param)
{
	int *p_RB = (int*)param[0];
	uint8_t*	pSrcRow = (uint8_t*)param[1]; 
	uint8_t*	pDstRow = (uint8_t*)param[2];
	int32_t	nBlockW = param[3];
	int32_t	nBlockH = param[4];
	int32_t	nSrcWidthStep = param[5];
	int32_t	nDstWidthStep = param[6];
	int32_t	nDivScalar = param[7];
	int32_t	nDivShift = param[8];

	int32_t  nNum = nBlockW * nBlockH; // Normalization factor 
	int32_t  nXCoef, nYCoef, nXInvCoef, nYInvCoef, nShift = 0; 
	uint8_t  nGray;

	nYInvCoef = nBlockH - 1;
	for (nYCoef = 0; nYCoef < nBlockH; ++nYCoef)    
	{   
		nXInvCoef = nBlockW - 1;
		for (nXCoef = 0; nXCoef < nBlockW; ++nXCoef)    
		{    
			nGray = pSrcRow[nXCoef];  
			pDstRow[nXCoef] = (uint8_t)(
					(	nYInvCoef * (nXInvCoef * p_LU[nGray] + nXCoef * p_RU[nGray])  
						+
						nYCoef * (nXInvCoef * p_LB[nGray] + nXCoef * p_RB[nGray])
					) * nDivScalar >> nDivShift);
			--nXInvCoef;
		}   
		--nYInvCoef;
		pSrcRow += nSrcWidthStep;
		pDstRow += nDstWidthStep;
	}
	return;
}

//pImg      - pointer to input/output image  
//nImgW      - resolution of image in x-direction  
//pulMap*     - mappings of greylevels from histograms  
//nBlockW     - nBlockW of image submatrix  
//nBlockH     - nBlockH of image submatrix  
//pLUT           - lookup table containing mapping greyvalues to bins  
//This function calculates the new greylevel assignments of pixels within a submatrix  
//of the image with size nBlockW and nBlockH. This is done by a bilinear interpolation  
//between four different mappings in order to eliminate boundary artifacts.  
//It uses a division; since division is often an expensive operation, I added code to  
//perform a logical shift instead when feasible.  
static void ZXCLAHEInterpolate (uint8_t * pSrc, uint8_t * pDst, int32_t nImgW, 
                               int32_t* pHistLU, int32_t* pHistRU, 
                               int32_t* pHistLB,  int32_t* pHistRB,   
                               int32_t nBlockW, int32_t nBlockH) 
{   
    int32_t  nWidthStep = ZXCORE_ALIGN_4(nImgW); 
    int32_t  nNum = nBlockW * nBlockH; // Normalization factor 
    int32_t  nXCoef, nYCoef, nXInvCoef, nYInvCoef, nShift = 0; 
    uint8_t  nGray;   
    uint8_t* pSrcRow = pSrc; 
    uint8_t* pDstRow = pDst;
    uint32_t nDivShift = 24;
    uint32_t nDivScalar = (uint32_t)((float)(1 << nDivShift) / (nNum + 0.0f));
    //static FILE	*fp=NULL;
	//static	int old_idx = 0;

    if (nNum & (nNum - 1))   // If nNum is not a power of two, use division  
    {
#if 0
		int32_t param[9];
		param[0] = (int)pHistRB;
		param[1] = (int)pSrcRow;
		param[2] = (int)pDstRow;
		param[3] = (int)nBlockW;
		param[4] = (int)nBlockH;
		param[5] = (int)nWidthStep;
		param[6] = (int)nWidthStep;
		param[7] = (int)nDivScalar;
		param[8] = (int)nDivShift;
#if	1
		_ZXCLAHEInterploateEntry_neon(pHistLU,pHistLB,pHistRU,param);
#else
		ZXCLAHEInterploateEntry(pHistLU,pHistLB,pHistRU,param);
#endif
#else
		nYInvCoef = nBlockH - 1;
        for (nYCoef = 0; nYCoef < nBlockH; ++nYCoef)    
        {   
            nXInvCoef = nBlockW - 1;
            for (nXCoef = 0; nXCoef < nBlockW; ++nXCoef)    
            {    
                nGray = pSrcRow[nXCoef];  
                pDstRow[nXCoef] = (uint8_t) 
                    ((nYInvCoef * (nXInvCoef * pHistLU[nGray] + nXCoef * pHistRU[nGray])  
                    + nYCoef    * (nXInvCoef * pHistLB[nGray] + nXCoef * pHistRB[nGray])) * nDivScalar >> nDivShift);   
                
                --nXInvCoef;
            }   
            --nYInvCoef;
            pSrcRow += nWidthStep;
            pDstRow += nWidthStep;
        }
#endif
    }
    else 
    {             // avoid the division and use a right shift instead  
        while (nNum >>= 1) ++nShift;           // Calculate 2log of nNum   
        nYInvCoef = nBlockH;
        for (nYCoef = 0; nYCoef < nBlockH; ++nYCoef)  
        {   
            nXInvCoef = nBlockW;
            for (nXCoef = 0; nXCoef < nBlockW; ++nXCoef) 
            {   
                nGray = pSrcRow[nXCoef]; 
                pDstRow[nXCoef] = (uint8_t) 
                    ((nYInvCoef * (nXInvCoef * pHistLU[nGray] + nXCoef * pHistRU[nGray])   
                      + nYCoef  * (nXInvCoef * pHistLB[nGray] + nXCoef * pHistRB[nGray])) >> nShift);    
                --nXInvCoef;
            }   
            --nYInvCoef;
            pSrcRow += nWidthStep;
            pDstRow += nWidthStep;
        }   
    }
}  

//pImg - Pointer to the input/output image  
//nImgW - Image resolution in the X direction  
//nImgH - Image resolution in the Y direction  
//Min - Minimum greyvalue of input image (also becomes minimum of output image)  
//Max - Maximum greyvalue of input image (also becomes maximum of output image)  
//nBlockNumW - Number of contextial regions in the X direction (min 2, max uiMAX_REG_X)  
//nBlockNumH - Number of contextial regions in the Y direction (min 2, max uiMAX_REG_Y)  
//nHistBins - Number of greybins for histogram ("dynamic range")  
//float fCliplimit - Normalized cliplimit (higher values give more contrast)  
//The number of "effective" greylevels in the output image is set by nHistBins; selecting  
// a small value (eg. 128) speeds up processing and still produce an output image of  
//good quality. The output image will have the same minimum and maximum value as the input 
//image. A clip limit smaller than 1 results in standard (non-contrast limited) AHE. 
static int32_t single_thread_entry(ClaheFilter* pFilter, uint8_t* pSrc, uint8_t* pDst,
								int32_t nImgW, int32_t nImgH,
								float fCliplimit, uint8_t nMin, uint8_t nMax)
{   
    int32_t     nX, nY;  
    int32_t     i;                                              /* counters */   
    int32_t     nBlockNumW   = pFilter->m_nBlockNumW;
    int32_t     nBlockNumH   = pFilter->m_nBlockNumH;
    int32_t     nBlockW      = (int32_t)((float_t)(nImgW) / nBlockNumW); /* Actual size of contextual regions */   
    int32_t     nBlockH      = (int32_t)((float_t)(nImgH) / nBlockNumH);
    int32_t     nSubX, nSubY;                                  /* size of context. reg. and subimages */   
    int32_t     nXL, nXR, nYU, nYB;                            /* auxiliary variables interpolation routine */   
    int32_t     nBlockPxlNum = nBlockW * nBlockH;              /* clip limit and region pixel count */   
    int32_t     nWidthStep   = ZXCORE_ALIGN_4(nImgW);
    int32_t     nUndoW       = nWidthStep;
    int32_t     nUndoH       = nImgH;
    int32_t     nHistIdxYU;
    int32_t     nHistIdxYB;
    int32_t     nHistBins    = pFilter->m_nHistBins;
    int32_t     nImgRowStep  = (nBlockH - 1) * nWidthStep + (nWidthStep % nBlockW);
    int32_t     nClipLimit   = (int32_t)(fCliplimit * nBlockW * nBlockH * 2.5f * 256 / (nHistBins * (256 + nHistBins)));
    int32_t*    pHist;                                         /* pointer to histogram and mappings*/ 
    int32_t*    pLU;                                           /* auxiliary pointers interpolation */  
    int32_t*    pLB;
    int32_t*    pRU;
    int32_t*    pRB;  
    uint8_t*    pSrcRow;                                       /* pointer to image */    
    uint8_t*    pDstRow;                                       /* pointer to image */ 
    float       fCoefInt, fCoefSrc;
    float       fNorm = 1.0f / 255.0f;
    int32_t     nUVSize;
    uint16_t*   pSrcDataUV;
    uint16_t*   pDstDataUV;
    
    if ((ZXCORE_ABS(fCliplimit - 1.0) < ZXCORE_EPS_16) && (pSrc != pDst))   
    {
        memcpy(pDst, pSrc, nWidthStep * nImgH);
        return    CLAHE_FILTER_OK;
    }

    /* Calculate greylevel mappings for each contextual region */ 
    pSrcRow = pSrc;
    pHist = pFilter->m_pHist; 
    for (nY = 0; nY < nBlockNumH; ++nY)    
    {   
        for (nX = 0; nX < nBlockNumW; ++nX)    
        {  
            ZXCLAHEMakeHistogram(pSrcRow, nImgW, nBlockW, nBlockH, pHist, nHistBins);
            ZXCLAHEClipHistogram(pHist, nHistBins, nClipLimit); 
            ZXCLAHEMapHistogram(pHist, 0, 255, nHistBins, nBlockPxlNum);

            for(i = 0; i < nHistBins; ++i)    
            {
                fCoefSrc = i * fNorm;
                fCoefInt = 1.0f - fCoefSrc;

                pHist[i] = i * fCoefSrc + pHist[i] * fCoefInt;
            }

            pHist += nHistBins;
            pSrcRow += nBlockW;
        }    
        pSrcRow += nImgRowStep;
    }   
        
    /* ZXCLAHEInterpolate greylevel mappings to get CLAHE image */   
    pSrcRow = pSrc; 
    pDstRow = pDst;
    pHist = pFilter->m_pHist;
    for(nY = 0; nY <= nBlockNumH; ++nY)    
    {   
        if (nY == 0)    
        {                     
            /* special case: top row */   
            nSubY   = nBlockH >> 1;  
            nYU     = 0; 
            nYB     = 0;   
            nUndoH -= nSubY;
        }   
        else    
        {   
            if (nY == nBlockNumH) 
            {               /* special case: bottom row */   
                nSubY = nUndoH;  
                nYU   = nBlockNumH - 1;  
                nYB   = nYU;  
            }   
            else    
            {                     /* default values */   
                nSubY   = nBlockH; 
                nYU     = nY - 1; 
                nYB     = nY;   
                nUndoH -= nSubY;
            }   
        }   

        nUndoW = nWidthStep;
        nHistIdxYU = nYU * nBlockNumW;
        nHistIdxYB = nYB * nBlockNumW;
        for (nX = 0; nX <= nBlockNumW; ++nX)    
        {   
            if (nX == 0) 
            {                  
                /* special case: left column */
                nSubX   = nBlockW >> 1; 
                nXL     = 0; 
                nXR     = 0; 
                nUndoW -= nSubX;
            }
            else 
            {
                if(nX == nBlockNumW) 
                {              
                    /* special case: right column */
                    nSubX = nUndoW;  
                    nXL   = nBlockNumW - 1; 
                    nXR   = nXL; 
                }
                else 
                {                      
                    /* default values */
                    nSubX   = nBlockW; 
                    nXL     = nX - 1; 
                    nXR     = nX; 
                    nUndoW -= nSubX;
                }
            } 

            pLU = &pHist[nHistBins * (nHistIdxYU + nXL)];   
            pRU = &pHist[nHistBins * (nHistIdxYU + nXR)];   
            pLB = &pHist[nHistBins * (nHistIdxYB + nXL)];   
            pRB = &pHist[nHistBins * (nHistIdxYB + nXR)];
            ZXCLAHEInterpolate(pSrcRow, pDstRow, nImgW, pLU, pRU, pLB, pRB, nSubX, nSubY); 

            pSrcRow += nSubX;             /* set pointer on next matrix */     
            pDstRow += nSubX;             /* set pointer on next matrix */  
        } 

        nX       = (nSubY - 1) * nWidthStep; 
        pSrcRow += nX;
        pDstRow += nX;
    }

	/////////////////////////NV21//////////////
    nUVSize = (nImgW >> 1) * (nImgH >> 1);
    pSrcDataUV = (uint16_t*)(pSrc + nImgW * nImgH);
    pDstDataUV = (uint16_t*)(pDst + nImgW * nImgH);

    for(i = 0; i < nUVSize; ++i)
    {
        pDstDataUV[i] = pFilter->m_pLUTUV[pSrcDataUV[i]];
    }

    return CLAHE_FILTER_OK; 
}

//static void double_thread_entry(void *p_param)
//{
//	ThreadParam* p_th = (ThreadParam *)p_param;
//	ClaheFilter *pFilter = (ClaheFilter *)(p_th->pFilter);
//	int32_t     nX, nY;
//    int32_t     nYStart, nYEnd;
//    int32_t     i;                                              /* counters */  
//    int32_t     nBlockNumW   = pFilter->m_nBlockNumW;
//    int32_t     nBlockNumH   = pFilter->m_nBlockNumH;
//	int32_t     nBlockW      = (int32_t)((float_t)(pFilter->width) / nBlockNumW); /* Actual size of contextual regions */   
//	int32_t     nBlockH      = (int32_t)((float_t)(pFilter->height) / nBlockNumH);
//    int32_t     nSubX, nSubY;                                  /* size of context. reg. and subimages */   
//    int32_t     nXL, nXR, nYU, nYB;                            /* auxiliary variables interpolation routine */   
//    int32_t     nBlockPxlNum = nBlockW * nBlockH;              /* clip limit and region pixel count */   
//    int32_t     nWidthStep   = ZXCORE_ALIGN_4(pFilter->width);
//    int32_t     nUndoW       = nWidthStep;
//    int32_t     nUndoH       = pFilter->height - nBlockH - (nBlockH >> 1);
//    int32_t     nHistIdxYU;
//    int32_t     nHistIdxYB;
//    int32_t     nHistBins    = pFilter->m_nHistBins;
//    int32_t     nImgRowStep  = (nBlockH - 1) * nWidthStep + (nWidthStep % nBlockW);
//    int32_t     nClipLimit   = (int32_t)(pFilter->fCliplimit * nBlockW * nBlockH * 3.0f * 256 / (nHistBins * (256 + nHistBins)));
//    int32_t*    pHist;                                         /* pointer to histogram and mappings*/ 
//    int32_t*    pLU;                                           /* auxiliary pointers interpolation */  
//    int32_t*    pLB;
//    int32_t*    pRU;
//    int32_t*    pRB;  
//    uint8_t*    pImgMinThd = &(pFilter->nSrcMin);
//    uint8_t*    pImgMaxThd = &(pFilter->nSrcMax); 
//    uint8_t*    pSrcRow;                                       /* pointer to image */    
//    uint8_t*    pDstRow;                                       /* pointer to image */  
//    float       fCoefInt, fCoefSrc;
//    float       fNorm = 1.0f;
//    float       fTmp;
//    int32_t      nUVSize;
//    uint16_t*    pSrcDataUV,*pDstDataUV;
//	
//	if (ZXCORE_ABS(pFilter->fCliplimit - 1.0) < ZXCORE_EPS_16) 
//    {
//		if(p_th->thread_id == 1)//used one thread to copy
//			memcpy(p_th->p_dst,p_th->p_src, nWidthStep * pFilter->height);
//        return ;
//    }
//
//    /* Calculate greylevel mappings for each contextual region */     
//    pSrcRow = p_th->p_src + p_th->thread_id * (pFilter->height>>1) * nWidthStep;
//    pHist = pFilter->m_pHist + p_th->thread_id * (nBlockNumH>>1) * nBlockNumW * nHistBins;
//	
//    nYStart = (nBlockNumH >> 1) *  p_th->thread_id;
//    nYEnd   = (nBlockNumH >> 1) << p_th->thread_id;
//
//    for (nY = nYStart; nY < nYEnd; ++nY)    
//    {   
//        for (nX = 0; nX < nBlockNumW; ++nX)    
//        {  
//            ZXCLAHEMakeHistogram(pSrcRow, pFilter->width, nBlockW, nBlockH, pHist, nHistBins);
//                        
//            if(*pImgMinThd != 0)
//            {
//                for(i = 0; i < nHistBins; i++)    
//                {
//                    if(pHist[i])
//                    {
//                        i < *pImgMinThd ? *pImgMinThd = i : 0;
//                        break;
//                    }
//                }
//            }
//
//            if(*pImgMaxThd != 255)
//            {
//                for(i = nHistBins - 1; i >= 0; --i)    
//                {
//                    if(pHist[i])
//                    {
//                        i > *pImgMaxThd ? *pImgMaxThd = i : 0;
//                        break;
//                    }
//                }
//            }
//
//            pHist += nHistBins;
//            pSrcRow += nBlockW;
//        }    
//        pSrcRow += nImgRowStep;
//    }
//
//    p_th->thread_flag |= 4;
//    while((p_th->p_others_flag[0]&4)==0)
//    {
//        SLEEP(1);
//    }
//
//    pHist = pFilter->m_pHist + (nBlockNumH >> 1) * nBlockNumW * nHistBins * p_th->thread_id;
//	
//    *pImgMinThd = 0;
//    *pImgMaxThd = 255;
//    //pHist = pFilter->m_pHist;   
//  //  fTmp = (nImgMax + nImgMin) * 0.35f;
//    fTmp = *pImgMinThd;
//    fNorm = 1.0f / (*pImgMaxThd - fTmp);
//    for (nY = nYStart; nY < nYEnd; ++nY) 
//    {   
//        for (nX = 0; nX < nBlockNumW; ++nX)    
//        {  
//            ZXCLAHEClipHistogram(pHist, nHistBins, nClipLimit); 
//            ZXCLAHEMapHistogram(pHist, 0, 255, nHistBins, nBlockPxlNum);
//            
//            for(i = fTmp; i <= *pImgMaxThd; ++i)    
//            {
//                fCoefSrc = (i - fTmp) * fNorm;
//                fCoefInt = 1.0f - fCoefSrc;
//
//                pHist[i] = i * fCoefSrc + pHist[i] * fCoefInt;
//            }
//
//            pHist += nHistBins;
//        }    
//    }   
//
//	p_th->thread_flag |= 2;
//	while((p_th->p_others_flag[0]&2)==0)
//    {
//        SLEEP(1);
//	}
//    /* ZXCLAHEInterpolate greylevel mappings to get CLAHE image */
//    if(!p_th->thread_id)
//    {
//        nYStart = 0;
//        nYEnd   = (nBlockNumH >> 1) - 1;
//    }
//    else
//    {
//        nYStart = (nBlockNumH >> 1);
//        nYEnd   = nBlockNumH;
//    }
//
//    nY = p_th->thread_id * (nBlockH + (nBlockH >> 1)) * nWidthStep;
//    pSrcRow = p_th->p_src + nY;
//    pDstRow = p_th->p_dst + nY;
//
//	pHist = pFilter->m_pHist;
//
//    for(nY = nYStart; nY <= nYEnd; ++nY) 
//    {   
//        if (nY == 0)    
//        {                     
//            /* special case: top row */   
//            nSubY   = nBlockH >> 1;  
//            nYU     = 0; 
//            nYB     = 0;   
//            nUndoH -= nSubY;
//        }   
//        else    
//        {   
//            if (nY == nBlockNumH) 
//            {               /* special case: bottom row */   
//                nSubY = nUndoH;  
//                nYU   = nBlockNumH - 1;  
//                nYB   = nYU;  
//            }   
//            else    
//            {                     /* default values */   
//                nSubY   = nBlockH; 
//                nYU     = nY - 1; 
//                nYB     = nY;   
//                nUndoH -= nSubY;
//            }   
//        }   
//
//        nUndoW = nWidthStep;
//        nHistIdxYU = nYU * nBlockNumW;
//        nHistIdxYB = nYB * nBlockNumW;
//        for (nX = 0; nX <= nBlockNumW; ++nX)    
//        {   
//            if (nX == 0) 
//            {                  
//                /* special case: left column */
//                nSubX   = nBlockW >> 1; 
//                nXL     = 0; 
//                nXR     = 0; 
//                nUndoW -= nSubX;
//            }
//            else 
//            {
//                if(nX == nBlockNumW) 
//                {              
//                    /* special case: right column */
//                    nSubX = nUndoW;  
//                    nXL   = nBlockNumW - 1; 
//                    nXR   = nXL; 
//                }
//                else 
//                {                      
//                    /* default values */
//                    nSubX   = nBlockW; 
//                    nXL     = nX - 1; 
//                    nXR     = nX; 
//                    nUndoW -= nSubX;
//                }
//            } 
//
//            pLU = &pHist[nHistBins * (nHistIdxYU + nXL)];   
//            pRU = &pHist[nHistBins * (nHistIdxYU + nXR)];   
//            pLB = &pHist[nHistBins * (nHistIdxYB + nXL)];   
//            pRB = &pHist[nHistBins * (nHistIdxYB + nXR)];
//			ZXCLAHEInterpolate(pSrcRow, pDstRow, pFilter->width, pLU, pRU, pLB, pRB, nSubX, nSubY); 
//
//            pSrcRow += nSubX;             /* set pointer on next matrix */     
//            pDstRow += nSubX;             /* set pointer on next matrix */  
//        } 
//
//        nX       = (nSubY - 1) * nWidthStep; 
//        pSrcRow += nX;
//        pDstRow += nX;
//    }
//
/////////////////////////uv//////////////
//    nUVSize = (pFilter->width >> 1) * (pFilter->height >> 1) >> 1;
//    pSrcDataUV = (uint16_t*)(p_th->p_src + pFilter->width * pFilter->height);
//    pDstDataUV = (uint16_t*)(p_th->p_dst + pFilter->width * pFilter->height);
//    if(p_th->thread_id)
//    {
//        pSrcDataUV = pSrcDataUV + nUVSize;
//        pDstDataUV = pDstDataUV + nUVSize;
//    }
//
//    for(i = 0; i < nUVSize; ++i)
//    {
//        pDstDataUV[i] = pFilter->m_pLUTUV[pSrcDataUV[i]];
//    }
//
//   /* if(!p_th->thread_id)
//    {
//        nUVSize = (pFilter->width >> 1) * (pFilter->height >> 1);
//        pSrcDataUV = (uint16_t*)(p_th->p_src + pFilter->width * pFilter->height);
//        pDstDataUV = (uint16_t*)(p_th->p_dst + pFilter->width * pFilter->height);
//
//        for(i = 0; i < nUVSize; ++i)
//        {
//            pDstDataUV[i] = pFilter->m_pLUTUV[pSrcDataUV[i]];
//        }
//    }*/
//
//	p_th->thread_flag |= 1;
//}

static void double_thread_entry(void *p_param)
{
	ThreadParam* p_th = (ThreadParam *)p_param;
	ClaheFilter *pFilter = (ClaheFilter *)(p_th->pFilter);
	int32_t     nX, nY;
    int32_t     nYStart, nYEnd;
    int32_t     i;                                              /* counters */  
    int32_t     nBlockNumW   = pFilter->m_nBlockNumW;
    int32_t     nBlockNumH   = pFilter->m_nBlockNumH;
	int32_t     nBlockW      = (int32_t)((float_t)(pFilter->width) / nBlockNumW); /* Actual size of contextual regions */   
	int32_t     nBlockH      = (int32_t)((float_t)(pFilter->height) / nBlockNumH);
    int32_t     nSubX, nSubY;                                  /* size of context. reg. and subimages */   
    int32_t     nXL, nXR, nYU, nYB;                            /* auxiliary variables interpolation routine */   
    int32_t     nBlockPxlNum = nBlockW * nBlockH;              /* clip limit and region pixel count */   
    int32_t     nWidthStep   = ZXCORE_ALIGN_4(pFilter->width);
    int32_t     nUndoW       = nWidthStep;
    int32_t     nUndoH       = pFilter->height - nBlockH - (nBlockH >> 1);
    int32_t     nHistIdxYU;
    int32_t     nHistIdxYB;
    int32_t     nHistBins    = pFilter->m_nHistBins;
    int32_t     nImgRowStep  = (nBlockH - 1) * nWidthStep + (nWidthStep % nBlockW);
    int32_t     nClipLimit   = (int32_t)(pFilter->fCliplimit * nBlockW * nBlockH * 2.5f * 256 / (nHistBins * (256 + nHistBins)));
    int32_t*    pHist;                                         /* pointer to histogram and mappings*/ 
    int32_t*    pLU;                                           /* auxiliary pointers interpolation */  
    int32_t*    pLB;
    int32_t*    pRU;
    int32_t*    pRB;  
    uint8_t*    pSrcRow;                                       /* pointer to image */    
    uint8_t*    pDstRow;                                       /* pointer to image */  
    float       fCoefInt, fCoefSrc;
    float       fNorm = 1.0f / 255.0f;
    int32_t     nUVSize;
    uint16_t*   pSrcDataUV;
    uint16_t*   pDstDataUV;
	
	if (ZXCORE_ABS(pFilter->fCliplimit - 1.0) < ZXCORE_EPS_16) 
    {
		if(p_th->thread_id == 1)//used one thread to copy
			memcpy(p_th->p_dst,p_th->p_src, nWidthStep * pFilter->height);
        return ;
    }

    /* Calculate greylevel mappings for each contextual region */     
    pSrcRow = p_th->p_src + p_th->thread_id * (pFilter->height>>1) * nWidthStep;
    pHist = pFilter->m_pHist + p_th->thread_id * (nBlockNumH>>1) * nBlockNumW * nHistBins;
	
    nYStart = (nBlockNumH >> 1) *  p_th->thread_id;
    nYEnd   = (nBlockNumH >> 1) << p_th->thread_id;

    for (nY = nYStart; nY < nYEnd; ++nY)    
    {   
        for (nX = 0; nX < nBlockNumW; ++nX)    
        {  
            ZXCLAHEMakeHistogram(pSrcRow, pFilter->width, nBlockW, nBlockH, pHist, nHistBins);
            ZXCLAHEClipHistogram(pHist, nHistBins, nClipLimit); 
            ZXCLAHEMapHistogram(pHist, 0, 255, nHistBins, nBlockPxlNum);
                
            for(i = 0; i < nHistBins; ++i)    
            {
                fCoefSrc = i * fNorm;
                fCoefInt = 1.0f - fCoefSrc;
    
                pHist[i] = i * fCoefSrc + pHist[i] * fCoefInt;
            }
    
            pHist += nHistBins;
            pSrcRow += nBlockW;
        }    
        pSrcRow += nImgRowStep;
    }   

	p_th->thread_flag |= 2;
	while((p_th->p_others_flag[0]&2)==0)
    {
        SLEEP(1);
	}
    /* ZXCLAHEInterpolate greylevel mappings to get CLAHE image */
    if(!p_th->thread_id)
    {
        nYStart = 0;
        nYEnd   = (nBlockNumH >> 1) - 1;
    }
    else
    {
        nYStart = (nBlockNumH >> 1);
        nYEnd   = nBlockNumH;
    }

    nY = p_th->thread_id * (nBlockH + (nBlockH >> 1)) * nWidthStep;
    pSrcRow = p_th->p_src + nY;
    pDstRow = p_th->p_dst + nY;

	pHist = pFilter->m_pHist;

    for(nY = nYStart; nY <= nYEnd; ++nY) 
    {   
        if (nY == 0)    
        {                     
            /* special case: top row */   
            nSubY   = nBlockH >> 1;  
            nYU     = 0; 
            nYB     = 0;   
            nUndoH -= nSubY;
        }   
        else    
        {   
            if (nY == nBlockNumH) 
            {               /* special case: bottom row */   
                nSubY = nUndoH;  
                nYU   = nBlockNumH - 1;  
                nYB   = nYU;  
            }   
            else    
            {                     /* default values */   
                nSubY   = nBlockH; 
                nYU     = nY - 1; 
                nYB     = nY;   
                nUndoH -= nSubY;
            }   
        }   

        nUndoW = nWidthStep;
        nHistIdxYU = nYU * nBlockNumW;
        nHistIdxYB = nYB * nBlockNumW;
        for (nX = 0; nX <= nBlockNumW; ++nX)    
        {   
            if (nX == 0) 
            {                  
                /* special case: left column */
                nSubX   = nBlockW >> 1; 
                nXL     = 0; 
                nXR     = 0; 
                nUndoW -= nSubX;
            }
            else 
            {
                if(nX == nBlockNumW) 
                {              
                    /* special case: right column */
                    nSubX = nUndoW;  
                    nXL   = nBlockNumW - 1; 
                    nXR   = nXL; 
                }
                else 
                {                      
                    /* default values */
                    nSubX   = nBlockW; 
                    nXL     = nX - 1; 
                    nXR     = nX; 
                    nUndoW -= nSubX;
                }
            } 

            pLU = &pHist[nHistBins * (nHistIdxYU + nXL)];   
            pRU = &pHist[nHistBins * (nHistIdxYU + nXR)];   
            pLB = &pHist[nHistBins * (nHistIdxYB + nXL)];   
            pRB = &pHist[nHistBins * (nHistIdxYB + nXR)];
			ZXCLAHEInterpolate(pSrcRow, pDstRow, pFilter->width, pLU, pRU, pLB, pRB, nSubX, nSubY); 

            pSrcRow += nSubX;             /* set pointer on next matrix */     
            pDstRow += nSubX;             /* set pointer on next matrix */  
        } 

        nX       = (nSubY - 1) * nWidthStep; 
        pSrcRow += nX;
        pDstRow += nX;
    }

///////////////////////uv//////////////
    nUVSize = (pFilter->width >> 1) * (pFilter->height >> 1) >> 1;
    pSrcDataUV = (uint16_t*)(p_th->p_src + pFilter->width * pFilter->height);
    pDstDataUV = (uint16_t*)(p_th->p_dst + pFilter->width * pFilter->height);
    if(p_th->thread_id)
    {
        pSrcDataUV = pSrcDataUV + nUVSize;
        pDstDataUV = pDstDataUV + nUVSize;
    }

    for(i = 0; i < nUVSize; ++i)
    {
        pDstDataUV[i] = pFilter->m_pLUTUV[pSrcDataUV[i]];
    }

	p_th->thread_flag |= 1;
}

int32_t    sndaClaheFilterProcess(int32_t Handle, uint8_t* pSrc, uint8_t* pDst,
								int32_t	width, int32_t height)
{
    ClaheFilter*pFilter =(ClaheFilter*)Handle;
	int32_t	ret;

    //uint32_t nNum = (width / pFilter->m_nBlockNumW) * (height * pFilter->m_nBlockNumH);
    //uint32_t nDivShift = 3;//accuracy min is 2
    //uint32_t nDivScalar = 1;

    //while(nDivScalar < nNum)
    //{
    //    nDivScalar <<= 1;
    //    ++nDivShift;
    //}

    ////too large input image
    //if(nDivShift > 24)
    //    return CLAHE_FILTER_WHERR;

	if(pFilter->threads_cnt == 1)
    	ret = single_thread_entry(pFilter, pSrc,pDst, width, height,pFilter->fCliplimit, 0, 255);
	else
	{
		ThreadParam th_p[2];
		pthread_t	th_h[2];

		pFilter->width = width;
		pFilter->height = height;
		pFilter->nMin = 0;
        pFilter->nMax = 255;

		th_p[0].threads_cnt = 2;
		th_p[0].thread_id = 0;
		th_p[0].pFilter = pFilter;
		th_p[0].p_src = pSrc;
		th_p[0].p_dst = pDst;
		th_p[0].thread_flag = 0;
		th_p[0].p_others_flag = &th_p[1].thread_flag;
		th_p[1].threads_cnt = 2;
		th_p[1].thread_id = 1;
		th_p[1].pFilter = pFilter;
		th_p[1].p_src = pSrc;
		th_p[1].p_dst = pDst;
		th_p[1].thread_flag = 0;
		th_p[1].p_others_flag = &th_p[0].thread_flag;
		//setup the threads
		CREATE_THREAD(th_h[0],double_thread_entry,&th_p[0]);
		CREATE_THREAD(th_h[1],double_thread_entry,&th_p[1]);
		if((th_h[0]==0)||(th_h[1]==0))
		{
			LOGE("setup threads are error\n");
			ret = CLAHE_FILTER_THSETUPERR;
			goto _error;
		}
		WAIT_THREAD(th_h[0],th_h[1],th_p[0].thread_flag,th_p[1].thread_flag,3);
	}
        
    return CLAHE_FILTER_OK;

_error:
    return ret;
}
