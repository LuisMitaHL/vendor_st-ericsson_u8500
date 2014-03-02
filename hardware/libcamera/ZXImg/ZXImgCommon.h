#ifndef	__ZXIMGCOMMON_H__
#define	__ZXIMGCOMMON_H__

#include "ZXImgCoreDef.h"

#define RADIUS  (4) // can be 1, 2, 4, 8, 16, 32 or 64
#define D       ((RADIUS)*2)
#define N       ((D)*(D))
#define AB_SCALING (256)

#define RADIUS_UV  (RADIUS/2) // can be 1, 2, 4, 8, 16, 32 or 64
#define D_UV       ((RADIUS_UV)*2)
#define N_UV       ((D_UV)*(D_UV))

#define HALF_RESOLUTION

void	*malloc_aligned(uint32_t size,  uint8_t alignment);

void	free_aligned(void *mem_ptr);

void box_mean_uint8_row_y_c(uint8_t *p_src, uint16_t *p_dst,int32_t param1, int32_t* param2);

void box_mean_uint8_col_y_c(uint16_t *p_src, uint8_t *p_dst,int32_t param1, int32_t* param2);

void box_mean_uint16_row_y_c(uint16_t *p_src, uint16_t *p_dst,int32_t param1, int32_t* param2);

void box_mean_uint16_col_y_c(uint16_t *p_src, uint16_t *p_dst,int32_t param1, int32_t* param2);

void box_mean_uint8_row_uv_c(uint8_t *p_src, uint16_t *p_dst,int32_t param1, int32_t* param2);

void box_mean_uint8_col_uv_c(uint16_t *p_src, uint8_t *p_dst,int32_t param1, int32_t* param2);

void box_mean_uint16_row_uv_c(uint16_t *p_src, uint16_t *p_dst,int32_t param1, int32_t* param2);

void box_mean_uint16_col_uv_c(uint16_t *p_src, uint16_t *p_dst,int32_t param1, int32_t* param2);

//int32_t	box_mean_uint8_y_c(uint8_t *p_src, uint8_t *p_dst,int32_t param1, int32_t param2);

//int32_t	box_mean_uint16_y_c(uint16_t *p_src, uint16_t *p_dst,int32_t param1, int32_t param2);

//int32_t	box_mean_uint8_uv_c(uint8_t *p_src, uint8_t *p_dst,int32_t param1, int32_t param2);

//int32_t	box_mean_uint16_uv_c(uint16_t *p_src, uint16_t *p_dst,int32_t param1, int32_t param2);

void calc_square_c(uint8_t *p_src,uint16_t *p_dst,int32_t param1,int32_t *param2);

void calc_ab_c(uint16_t *p_mean_II,uint8_t *p_mean_I,uint8_t *p_lookup_a,int32_t *param);

void calc_dst_c(uint8_t *p_mean_a,uint16_t *p_mean_b,uint8_t *p_src,int32_t *param);

void halve_image_uint8(uint8_t *p_large, uint8_t *p_small, int32_t width, int32_t height, int32_t isUV);

void double_image_uint8(uint8_t *p_small, uint8_t *p_large, int32_t width, int32_t height, int32_t isUV);

void double_image_uint16(uint16_t *p_small, uint16_t *p_large, int32_t width, int32_t height, int32_t isUV);


#endif