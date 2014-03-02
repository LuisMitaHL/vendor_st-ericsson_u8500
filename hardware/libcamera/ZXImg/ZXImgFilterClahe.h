#ifndef __ZXIMGFILTERCLAHE_H__
#define __ZXIMGFILTERCLAHE_H__

#include "ZXImgCoreDef.h"

///////////the error value of Guided Filter //////////
#define	CLAHE_FILTER_OK				0	//ok
#define	CLAHE_FILTER_MEMERR			-1	//memory error 
#define	CLAHE_FILTER_WHERR			-2	//width height error
#define	CLAHE_FILTER_THSETUPERR		-3	//thread setup error
#define	CLAHE_FILTER_PARAMERR		-4	//parameter error
#define	CLAHE_FILTER_FAILED			-5	//usual error
 ///////////define the Guided Filter functions interface///////////
int32_t    sndaClaheFilterInit(uint32_t *pHandle,int32_t nBlockNumW,int32_t nBlockNumH,
							int32_t nHistBins,float_t fSaturation,float_t fContrast,int32_t threads_cnt);

int32_t    sndaClaheFilterProcess(int32_t Handle, uint8_t* pSrc, uint8_t* pDst,
								int32_t	width, int32_t height);

void	sndaClaheFilterRelease(int32_t Handle);

//////////define the struct for Clahe Filter////////////////////
typedef	struct	tagClaheFilter
{
	int32_t			threads_cnt;
	int32_t			width;
	int32_t			height;
	uint8_t			nMin;
    uint8_t			nMax;    

	float_t			fCliplimit;
	int32_t         m_nHistBins;
    int32_t         m_nBlockNumW;
    int32_t         m_nBlockNumH;
    int32_t*        m_pHist;    
    uint16_t*		m_pLUTUV;
}ClaheFilter;
#endif //__ZX_IMG_FILTER_H__