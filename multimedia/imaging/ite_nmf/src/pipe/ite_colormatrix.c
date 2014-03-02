/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */



#include "ite_colormatrix.h"
#include <stdio.h>
#include <los/api/los_api.h>
#include "ite_main.h"

t_uint32 ColourMatrix850[9] = {0x40cf,0xbfae,0x33d7,0xba8e,0x3fa4,0xbc00,0xb9d7,0xbe14,0x4048};


    struct s_page_element ColourEngine0_ColourMatrixSensor0[] = {
    {CE_ColourMatrixFloat_0_f_RedInRed_Byte0, 0x0, 0},
    {CE_ColourMatrixFloat_0_f_GreenInRed_Byte0, 0x0, 0},
    {CE_ColourMatrixFloat_0_f_BlueInRed_Byte0, 0x0, 0},
    {CE_ColourMatrixFloat_0_f_RedInGreen_Byte0, 0x0, 0},
    {CE_ColourMatrixFloat_0_f_GreenInGreen_Byte0, 0x0, 0},
    {CE_ColourMatrixFloat_0_f_BlueInGreen_Byte0, 0x0, 0},
    {CE_ColourMatrixFloat_0_f_RedInBlue_Byte0, 0x0, 0},
    {CE_ColourMatrixFloat_0_f_GreenInBlue_Byte0, 0x0, 0},
    {CE_ColourMatrixFloat_0_f_BlueInBlue_Byte0, 0x0, 0},
    {0, 0, 0}
    };

    struct s_page_element ColourEngine0_ColourMatrixSensor1[] = {
    {CE_ColourMatrixFloat_0_f_RedInRed_Byte0, 0x0, 0},
    {CE_ColourMatrixFloat_0_f_GreenInRed_Byte0, 0x0, 0},
    {CE_ColourMatrixFloat_0_f_BlueInRed_Byte0, 0x0, 0},
    {CE_ColourMatrixFloat_0_f_RedInGreen_Byte0, 0x0, 0},
    {CE_ColourMatrixFloat_0_f_GreenInGreen_Byte0, 0x0, 0},
    {CE_ColourMatrixFloat_0_f_BlueInGreen_Byte0, 0x0, 0},
    {CE_ColourMatrixFloat_0_f_RedInBlue_Byte0, 0x0, 0},
    {CE_ColourMatrixFloat_0_f_GreenInBlue_Byte0, 0x0, 0},
    {CE_ColourMatrixFloat_0_f_BlueInBlue_Byte0, 0x0, 0},
    {0, 0, 0}
    };

    struct s_page_element ColourEngine1_ColourMatrixSensor0[] = {
    {CE_ColourMatrixFloat_1_f_RedInRed_Byte0, 0x0, 0},
    {CE_ColourMatrixFloat_1_f_GreenInRed_Byte0, 0x0, 0},
    {CE_ColourMatrixFloat_1_f_BlueInRed_Byte0, 0x0, 0},
    {CE_ColourMatrixFloat_1_f_RedInGreen_Byte0, 0x0, 0},
    {CE_ColourMatrixFloat_1_f_GreenInGreen_Byte0, 0x0, 0},
    {CE_ColourMatrixFloat_1_f_BlueInGreen_Byte0, 0x0, 0},
    {CE_ColourMatrixFloat_1_f_RedInBlue_Byte0, 0x0, 0},
    {CE_ColourMatrixFloat_1_f_GreenInBlue_Byte0, 0x0, 0},
    {CE_ColourMatrixFloat_1_f_BlueInBlue_Byte0, 0x0, 0},
    {0, 0, 0}
    };

/* -----------------------------------------------------------------------
   FUNCTION : ITE_SetColorMatrix
   PURPOSE  : Set color matrix according to pipe and sensor
   ------------------------------------------------------------------------ */
void ITE_SetColorMatrix(t_uint16 pipe, t_uint16 sensor){
   pts_page_element  PE_table = NULL;
   t_uint32*         color_table = NULL;
   t_uint32 i = 0, index = 0, value = 0;

   UNUSED(pipe);
   UNUSED(sensor);
   /*
   if(pipe == 0){
      switch(sensor){

      }
   }
   else if(pipe == 1){

   }*/
   PE_table = ColourEngine1_ColourMatrixSensor0;
   color_table = ColourMatrix850;

   /* Send Color Matrix PE */
   for (i=0; PE_table[i].addr != 0; i++) {
      value = color_table[index];
      index++;

      /* LSB */
      ITE_writePE(PE_table[i].addr, value & 0xFF);
      i++;
      /* MSB */
      ITE_writePE(PE_table[i].addr, (value& 0xFF00) >> 8);
   }

}

