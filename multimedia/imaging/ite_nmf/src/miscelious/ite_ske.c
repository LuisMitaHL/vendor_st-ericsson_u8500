/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*

#include "ske.h"
#include "ske_services.h"

t_uint32 volatile g_keypad_still = 0;
t_uint8 volatile g_line = 0;
t_uint8 volatile g_column = 0;

void keypad_callback_func(void *ske_callback_param, void *custom_param)
{

   if (SKE_DEVICE_KEYPAD == ((t_ske_callback_param *) ske_callback_param)->ske_device)
   {
      g_line = (((t_ske_callback_param *) ske_callback_param)->ske_keypressed_row);
      g_column = (((t_ske_callback_param *) ske_callback_param)->ske_keypressed_column);

      if(g_line == 0 && g_column == 4){
         g_keypad_still = 1;
      }
    }
}

void ITE_SKEInit(void){

   //SER_SKE_Init(DEFAULT_INIT_MASK);
   SER_SKE_RegisterCallback(SKE_DEVICE_KEYPAD, keypad_callback_func, 0); 
   SKE_SetKeypadDebounce(0x14);
   // Select Key Pad  
   SER_SKE_SelectKeyPad();
   SER_SKE_SetKeypadMatrixSize(5, 5);
   // Setting Autoscan columns...
   SKE_SetAutoScanColumns(SKE_SCAN_COLUMN_0_TO_4);

   SKE_EnableIRQSrc((t_ske_irq_src) SKE_IRQ_KEYPAD_AUTOSCAN);
   SKE_EnableKeypadAutoScan();

}
*/
