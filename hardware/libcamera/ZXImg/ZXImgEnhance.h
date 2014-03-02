////////////////////////////////////////////////////////////////////////////////
// Author:          Sicong Zhang
// Description:     photography effect enhancement functions
// Create:          2012-05-08
// Update:          2012-07-27
// Copyright:       Zhang Sicong
//                  All Rights Reserved
////////////////////////////////////////////////////////////////////////////////

#ifndef __ZXIMGENHANCE_H__
#define __ZXIMGENHANCE_H__

#ifdef	__cplusplus
extern "C" {
#endif

/////////////////MODE PARAMETERS////////////////////////////////////////
#define		IMGEE_MODE_DISABLE      0x0000  //turn off enhance processing
#define		IMGEE_MODE_FAST			0x1000  //more powerful enhancement

#define		IMGEE_MODE_AUTO         0x0001  //default settings
#define		IMGEE_MODE_HDR          0x0002  //useful for night, HDR photography
#define		IMGEE_MODE_BEAUTY       0x0004  //portrait 
#define		IMGEE_MODE_SHARPEN      0x0008  //increase detail 
#define		IMGEE_MODE_DENOISE      0x0010  //decrease noise
#define		IMGEE_MODE_SHARPEN_EX   0x0020  //increase detail with noise reduction
/////////////////DATA FORMAT/////////////////////////////////////////////
#define		IMGEE_FORMAT_YUVNV21	0x0000	//yuv NV21
#define		IMGEE_FORMAT_S1			0x0001	//yuv MB

/////////////////RETURN VALUE///////////////////////////////////////////
#define		ZXIMGCORE_OK				0	//ok
#define		ZXIMGCORE_PARA_ERR			-1	//parameter is error
#define		ZXIMGCOER_MEM_ERR			-2	//memory space error
#define		ZXIMGCORE_INITCLAHE_ERR		-3	//initiate clahe filter error
#define		ZXIMGCORE_INITGUIDED_ERR	-4	//initiate guided filter error
#define		ZXIMGCORE_FAILED			-5	//usual error

int		ZXImgEnhanceInit(int* p_handle,int format,int nMode);

int		ZXImgEnhanceProcess(int handle, unsigned char* pData,int nImgW,int nImgH);

void	ZXImgEnhanceRelease(int handle);

#ifdef	__cplusplus
}
#endif

#endif//__ZX_IMG_ENHANCE_H__