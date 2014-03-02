#ifndef __ZXIMGFILTERWAVELET_H__
#define __ZXIMGFILTERWAVELET_H__

#include "ZXImgCoreDef.h"

///////////the error value of Wavelet Filter //////////
#define	WAVELET_FILTER_OK				 0	//ok
#define	WAVELET_FILTER_MEMERR			-1	//memory error 
#define	WAVELET_FILTER_WHERR			-2	//width height error
#define	WAVELET_FILTER_THSETUPERR		-3	//thread setup error
#define	WAVELET_FILTER_PARAMERR		    -4	//parameter error
#define	WAVELET_FILTER_FAILED			-5	//usual error


///////////define the Wavelet Filter functions interface///////////
int32_t     sndaWaveletFilterInit(int32_t* pHandle, uint32_t width, uint32_t height, float_t threshold, float_t low, uint32_t threads_cnt);

int32_t	sndaWaveletFilterProcess(int32_t Handle, uint8_t* pSrc, uint8_t* pDst);

void	sndaWaveletFilterRelease(int32_t Handle);

extern unsigned int EdgeStopFuncTab[256];

//////////define the struct for Wavelet Filter////////////////////
typedef	struct	tagWaveletFilter
{
	int32_t	threads_cnt;
	int32_t	width;
	int32_t	height;
	uint8_t	*buf;

	uint8_t tholds[5]; //constants
	float_t threshold; //preset constant and control the smoothness
	float_t low;       //preset constant and control the suppression degree
	float_t one_low_thr; //(1-low) * threshold
	float_t stdev[5];  //computed in process, and then updated by 

	///////////////define these functions pointers
	void (*func_hat_transform)(uint8_t* p_src, uint8_t* p_dst, uint16_t* p_buf, int32_t param1, int32_t param2, int32_t param3);
	void (*func_calc_stdev)(int8_t* p_h, uint8_t* p_l, int32_t param1, int32_t param2, uint8_t param3, float_t* param4);
	void (*func_calc_threshold)(uint8_t* img[3], int32_t param1, int32_t param2, uint32_t param3, uint32_t param4, uint8_t param5, float_t* param6);
}WaveletFilter;


// For test: may be moved to ZXImgCommon
void FilterWaveletC1(int Handle, uint8_t* p_src, uint8_t* p_dst, int offset, int cn, int whScale);
void hat_transform_row(uint8_t *p_src, uint16_t *p_dst, int32_t width, int sc);
void hat_transform_col(uint16_t *current, uint16_t *top, uint16_t* bottom, uint8_t* output, int32_t width);
void hat_transform(uint8_t* p_src, uint8_t* p_dst, uint16_t* p_buf, int width, int height, int sc);
void calc_stdev(int8_t* p_h, uint8_t* p_l, int width, int height, uint8_t thold, float stdev[5]);
void calc_threshold(uint8_t* fimg[3], int32_t width, int32_t height, uint32_t lpass, uint32_t hpass, uint8_t thold, float stdev[5]);

void hat_transform_row_ea(uint8_t *p_src, uint16_t *p_dst, int32_t width, int sc);
void hat_transform_ea(uint8_t* p_src, uint8_t* p_dst, uint16_t* p_buf, int width, int height, int sc);


#endif //__ZXIMGFILTERWAVELET_H__