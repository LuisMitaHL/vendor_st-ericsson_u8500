/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "imgTest_draw.h"

#undef MIN
#define MIN(a,b) (((a)>(b))?(b):(a))

#undef MAX
#define MAX(a,b) (((a)<(b))?(b):(a))

#define Y(_R, _G, _B) ( 0.299   * (_R) + 0.587   * (_G) + 0.114   * (_B))
#define U(_R, _G, _B) (-0.14713 * (_R) - 0.28886 * (_G) + 0.436   * (_B))
#define V(_R, _G, _B) ( 0.615   * (_R) - 0.51498 * (_G) - 0.10001 * (_B))


#include <string.h>
long imgtest_drawRel2Abs(long pos, long reference) {
    return reference * pos / 100 / 1000;
}

void imgtest_drawPixel(unsigned char *pu8_buffer, long x, long y, long stride, long height, e_imgTestC_colorFormat colorFmt, unsigned long RGBColor) {
    unsigned char R = (RGBColor >> 16) & 0xFF;
    unsigned char G = (RGBColor >> 8)  & 0xFF;
    unsigned char B = (RGBColor >> 0)  & 0xFF;
    switch (colorFmt) {
        case  IMGTEST_DRAWCOLORFMT_RGB565:
        {
            unsigned short *pu16_pixel = (unsigned short *) (&pu8_buffer[x * sizeof(unsigned short) + y * stride]);
            pu16_pixel[0] = ((R & 0x1F) << 11) | ((G & 0x3F) << 6) | (B & 0x1F);
            break;
        }
        case IMGTEST_DRAWCOLORFMT_RGB888:
        {
            long pixPos = x * 3 + y * stride;
            pu8_buffer[pixPos]     = R;
            pu8_buffer[pixPos + 1] = G;
            pu8_buffer[pixPos + 2] = B;
            break;
        }
        case IMGTEST_DRAWCOLORFMT_CbYCrY422:
        {
            long pixPos = (x&~1) * 2 + (y&~1) * stride; // Write 2 pixels; even column and line numbers
            pu8_buffer[pixPos]     = (unsigned char) V(R, G, B);
            pu8_buffer[pixPos + 1] = pu8_buffer[pixPos + 3] = (unsigned char) Y(R, G, B);
            pu8_buffer[pixPos + 2] = (unsigned char) U(R, G, B);
            break;
        }
        default:
            ;// printf("Error: Unsupported color format\n");
    }
}


void imgtest_drawLine(unsigned char *pu8_buffer, long width, long height, long stride, e_imgTestC_colorFormat colorFmt, imgTest_drawObject *pObject) {
    bool bDrawn = false;
    if (pObject->coords.line.y0 == pObject->coords.line.y1) { // Horizontal line
        long y = imgtest_drawRel2Abs(pObject->coords.line.y0, height);
        for (long x = imgtest_drawRel2Abs(pObject->coords.line.x0, width); x < imgtest_drawRel2Abs(pObject->coords.line.x1, width); x++) {
            imgtest_drawPixel(pu8_buffer, x, y, stride, height, colorFmt, pObject->RGBColor);
        }
        bDrawn = true;
    }

    if (pObject->coords.line.x0 == pObject->coords.line.x1) { // Vertical line
        long x = imgtest_drawRel2Abs(pObject->coords.line.x0, width);
        for (long y = imgtest_drawRel2Abs(pObject->coords.line.y0, height); y < imgtest_drawRel2Abs(pObject->coords.line.y1, height); y++) {
            imgtest_drawPixel(pu8_buffer, x, y, stride, height, colorFmt, pObject->RGBColor);
        }
        bDrawn = true;
    }
    if (! bDrawn) {
        ;// printf("Error: Not a vertical or horizontal line.\n"); // Error
    }
}

void imgTest_draw(unsigned char *pu8_buffer, long width, long height, long stride, e_imgTestC_colorFormat colorFmt, imgTest_drawObject *aObjects, int objCount) {
    for (int obj = 0; obj < objCount; obj++) {
        imgTest_drawObject *pObj = &aObjects[obj];
        imgTest_drawObject tmpObject;
        // Drawing the object in the pixel buffer
        switch(pObj->type) {
            case IMGTEST_DRAWOBJECT_SPOT:
                tmpObject.type = IMGTEST_DRAWOBJECT_LINE;
                tmpObject.RGBColor = pObj->RGBColor;
                tmpObject.coords.line.x0 = MAX(0, pObj->coords.spot.x - 2000);
                tmpObject.coords.line.x1 = MIN(100000, pObj->coords.spot.x + 2000);
                tmpObject.coords.line.y0 = pObj->coords.spot.y;
                tmpObject.coords.line.y1 = pObj->coords.spot.y;
                imgtest_drawLine(pu8_buffer, width, height, stride, colorFmt, &tmpObject);
                tmpObject.coords.line.x0 = pObj->coords.spot.x;
                tmpObject.coords.line.x1 = pObj->coords.spot.x;
                tmpObject.coords.line.y0 = MAX(0, pObj->coords.spot.y - 2000);
                tmpObject.coords.line.y1 = MIN(100000, pObj->coords.spot.y + 2000);
                imgtest_drawLine(pu8_buffer, width, height, stride, colorFmt, &tmpObject);
                break;
            case IMGTEST_DRAWOBJECT_LINE:
                imgtest_drawLine(pu8_buffer, width, height, stride, colorFmt, pObj);
                break;
            case IMGTEST_DRAWOBJECT_RECT:
                tmpObject.type = IMGTEST_DRAWOBJECT_LINE;
                tmpObject.RGBColor = pObj->RGBColor;

                tmpObject.coords.line.x0 = pObj->coords.rect.x0;
                tmpObject.coords.line.x1 = pObj->coords.rect.x1;
                tmpObject.coords.line.y0 = pObj->coords.rect.y0;
                tmpObject.coords.line.y1 = pObj->coords.rect.y0;
                imgtest_drawLine(pu8_buffer, width, height, stride, colorFmt, &tmpObject);

                tmpObject.coords.line.x0 = pObj->coords.rect.x0;
                tmpObject.coords.line.x1 = pObj->coords.rect.x1;
                tmpObject.coords.line.y0 = pObj->coords.rect.y1;
                tmpObject.coords.line.y1 = pObj->coords.rect.y1;
                imgtest_drawLine(pu8_buffer, width, height, stride, colorFmt, &tmpObject);

                tmpObject.coords.line.x0 = pObj->coords.rect.x0;
                tmpObject.coords.line.x1 = pObj->coords.rect.x0;
                tmpObject.coords.line.y0 = pObj->coords.rect.y0;
                tmpObject.coords.line.y1 = pObj->coords.rect.y1;
                imgtest_drawLine(pu8_buffer, width, height, stride, colorFmt, &tmpObject);

                tmpObject.coords.line.x0 = pObj->coords.rect.x1;
                tmpObject.coords.line.x1 = pObj->coords.rect.x1;
                tmpObject.coords.line.y0 = pObj->coords.rect.y0;
                tmpObject.coords.line.y1 = pObj->coords.rect.y1;
                imgtest_drawLine(pu8_buffer, width, height, stride, colorFmt, &tmpObject);
                break;
            default:
                ; // printf("Error: Unsupported object type.\n"); // Error
        }
    }
}

