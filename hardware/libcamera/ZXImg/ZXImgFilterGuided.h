#ifndef __ZXIMGFILTERGUIDED_H__
#define __ZXIMGFILTERGUIDED_H__

#include "ZXImgCoreDef.h"

///////////the error value of Guided Filter //////////
#define	GUIDED_FILTER_OK				0	//ok
#define	GUIDED_FILTER_MEMERR			-1	//memory error 
#define	GUIDED_FILTER_WHERR				-2	//width height error
#define	GUIDED_FILTER_THSETUPERR		-3	//thread setup error
#define	GUIDED_FILTER_PARAMERR			-4	//parameter error
#define	GUIDED_FILTER_FAILED			-5	//usual error

///////////define the Guided Filter functions interface///////////
int32_t	sndaGuidedFilterInit(uint32_t *pHandle,int32_t eps0_y, int32_t eps1_y,
							int32_t eps0_uv, int32_t eps1_uv,
							float_t lumda0_y, float_t lumda1_y, 
							float_t lumda0_uv, float_t lumda1_uv, 
							int32_t threads_cnt);

int32_t	sndaGuidedFilterProcess(uint32_t Handle,uint8_t *srcData,uint8_t *dstData,
							int width, int height);

void	sndaGuidedFilterRelease(uint32_t Handle);

//////////define the struct for Guided Filter////////////////////
typedef	struct	tagGuidedFilter
{
	int32_t	threads_cnt;
	int32_t	width;
	int32_t	height;
	int32_t	only_cpy[2];
	uint8_t	*p_buf[2];
	uint8_t	*p_lookup_a[2];
	///////////////define these functions pointers
	void (*box_row_func_uint8[2])(uint8_t *p_src, uint16_t *p_dst,int32_t param1, int32_t* param2);
	void (*box_row_func_uint16[2])(uint16_t *p_src, uint16_t *p_dst,int32_t param1, int32_t* param2);
	void (*box_col_func_uint8[2])(uint16_t *p_src, uint8_t *p_dst,int32_t param1, int32_t* param2);
	void (*box_col_func_uint16[2])(uint16_t *p_src, uint16_t *p_dst,int32_t param1, int32_t* param2);
	
	void (*calc_square)(uint8_t *p_src,uint16_t *p_dst,int32_t param1,int32_t *param2);
	void (*calc_ab)(uint16_t *p_mean_II,uint8_t *p_mean_I,uint8_t *p_lookup_a,int32_t *param);
	void (*calc_dst)(uint8_t *p_mean_a,uint16_t *p_mean_b,uint8_t *p_src,int32_t *param);
}GuidedFilter;

#endif