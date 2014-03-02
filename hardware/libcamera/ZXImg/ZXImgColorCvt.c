#include "ZXImgColorCvt.h"

void	s1format_2_yuvnv12(uint8_t *p_src,uint8_t *p_dst,int32_t width,int32_t height)
{
    int32_t     nSrcX;
    int32_t     nSrcY;
    int32_t     nDstY;
    int32_t     k;
    int32_t     nShift      = width  >> 2;
    int32_t     nSize       = height * width;
    int32_t     mb_width    = width  >>3;
    int32_t     mb_height   = height >> 4;
    uint32_t*   pSrcStart;
    uint32_t*   pDstStart;

    /////////////Y/////////////////////////////////
    for(nSrcY = 0; nSrcY < mb_height; ++nSrcY)
    {
        nDstY = nSrcY << 4;
        for(nSrcX = 0; nSrcX < mb_width; ++nSrcX)
        {
            pDstStart = (uint32_t*)(p_dst + nDstY * width + (nSrcX << 3));
            pSrcStart = (uint32_t*)(p_src + ((nSrcY * mb_width + nSrcX) << 7));

            for(k = 0; k < 16; ++k)
            {
                pDstStart[0] = pSrcStart[0];
                pDstStart[1] = pSrcStart[1];
                pSrcStart += 2;
                pDstStart += nShift;
            }
        }
    }

    ////////////UV////////////////////////////////
    p_src += nSize;
    p_dst += nSize;

    for(nSrcY = 0; nSrcY < mb_height; ++nSrcY)
    {
        nDstY = nSrcY << 3;
        for(nSrcX = 0; nSrcX < mb_width; ++nSrcX)
        {
            pDstStart = (uint32_t*)(p_dst + nDstY * width + (nSrcX << 3));
            pSrcStart = (uint32_t*)(p_src + ((nSrcY * mb_width + nSrcX) << 6));

            for(k = 0; k < 8; ++k)
            {
                pDstStart[0] = pSrcStart[0];
                pDstStart[1] = pSrcStart[1];
                pSrcStart += 2;
                pDstStart += nShift;
            }
        }
    }

    return;
}


void	yuvnv12_2_s1format(uint8_t *p_src,uint8_t *p_dst,int32_t width,int32_t height)
{
    int32_t     nSrcX;
    int32_t     nSrcY;
    int32_t     nDstY;
    int32_t     k;
    int32_t     nShift      = width  >> 2;
    int32_t     nSize       = height * width;
    int32_t     mb_width    = width  >>3;
    int32_t     mb_height   = height >> 4;
    uint32_t*   pSrcStart;
    uint32_t*   pDstStart;

    /////////////Y/////////////////////////////////
    for(nSrcY = 0; nSrcY < mb_height; ++nSrcY)
    {
        nDstY = nSrcY << 4;
        for(nSrcX = 0; nSrcX < mb_width; ++nSrcX)
        {
            pDstStart = (uint32_t*)(p_dst + ((nSrcY * mb_width + nSrcX) << 7));
            pSrcStart = (uint32_t*)(p_src + nDstY * width + (nSrcX << 3));

            for(k = 0; k < 16; ++k)
            {
                pDstStart[0] = pSrcStart[0];
                pDstStart[1] = pSrcStart[1];
                pSrcStart += nShift;
                pDstStart += 2;
            }
        }
    }

    ////////////UV////////////////////////////////
    p_src += nSize;
    p_dst += nSize;

    for(nSrcY = 0; nSrcY < mb_height; ++nSrcY)
    {
        nDstY = nSrcY << 3;
        for(nSrcX = 0; nSrcX < mb_width; ++nSrcX)
        {
            pDstStart = (uint32_t*)(p_dst + ((nSrcY * mb_width + nSrcX) << 6));
            pSrcStart = (uint32_t*)(p_src + nDstY * width + (nSrcX << 3));

            for(k = 0; k < 8; ++k)
            {
                pDstStart[0] = pSrcStart[0];
                pDstStart[1] = pSrcStart[1];
                pSrcStart += nShift;
                pDstStart += 2;
            }
        }
    }

    return;
}