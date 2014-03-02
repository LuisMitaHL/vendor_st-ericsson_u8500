#ifndef	__ZXIMGCOLORCVT_H__
#define	__ZXIMGCOLORCVT_H__

#include "ZXImgCoreDef.h"

void	s1format_2_yuvnv12(uint8_t *p_src,uint8_t *p_dst,int32_t width,int32_t height);

void	yuvnv12_2_s1format(uint8_t *p_src,uint8_t *p_dst,int32_t width,int32_t height);
#endif