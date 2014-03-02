/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#include "ite_nmf_Rx_Testpattern.h"




void ITE_SetRxTestPattern(RxTestPattern_te ap_pattern)
{

    LOS_Log("Rx Test Pattern Command is %u \n", ap_pattern);

    // Write Requested Test Pattern into the Virtual Pattern Register.
    ITE_writePE(SystemSetup_e_RxTestPattern_Byte0, ap_pattern);
        
    LOS_Log("EXIT Rx Test Pattern Command");
				
}


void ITE_SetSolidColorData(t_uint16 Red, t_uint16 GR, t_uint16 Blue, t_uint16 BG)
{

    LOS_Log("SolidColorData are %u %u %u %u \n", Red, GR, Blue, BG);
    
    ITE_writePE(testpattern_SolidColor_data_u16_SolidColor_data_red_Byte0, Red);
    ITE_writePE(testpattern_SolidColor_data_u16_SolidColor_data_gir_Byte0, GR);
    ITE_writePE(testpattern_SolidColor_data_u16_SolidColor_data_blu_Byte0, Blue);
    ITE_writePE(testpattern_SolidColor_data_u16_SolidColor_data_gib_Byte0, BG);
    
    //ITE_CheckSolidColorData(Red, GR, Blue, BG);
}




void ITE_SetRxCursorPosition(t_uint8 x_pos, t_uint8 x_width, t_uint8 y_pos, t_uint8 y_width)
{
    
    LOS_Log("CursorPositionData are %u %u %u %u \n", x_pos, x_width, y_pos, y_width);
    
    ITE_writePE(testpattern_Cursors_values_u8_hcur_posn_per_Byte0, x_pos);
    ITE_writePE(testpattern_Cursors_values_u8_hcur_width_Byte0, x_width);
    ITE_writePE(testpattern_Cursors_values_u8_vcur_posn_per_Byte0, y_pos);
    ITE_writePE(testpattern_Cursors_values_u8_vcur_width_Byte0, y_width);
	
}


void ITE_CheckRxPattern(RxTestPattern_te ap_pattern)
{

	t_uint32 rx_testpattern;
	rx_testpattern = ITE_readPE(SystemSetup_e_RxTestPattern_Byte0);
	if(rx_testpattern == ap_pattern){
		LOS_Log("RxPattern is written Sucessfull %u", rx_testpattern);
		mmte_testResult(TEST_PASSED);}
	else {
		LOS_Log("RxPattern is written Fail %u", rx_testpattern);
		mmte_testResult(TEST_FAILED);}
		
}



void ITE_CheckSolidColorData(t_uint16 Red, t_uint16 GR, t_uint16 Blue, t_uint16 BG)
{

	if((Red != ITE_readPE(testpattern_SolidColor_data_u16_SolidColor_data_red_Byte0))
		|| (GR != ITE_readPE(testpattern_SolidColor_data_u16_SolidColor_data_gir_Byte0))
		|| (Blue != ITE_readPE(testpattern_SolidColor_data_u16_SolidColor_data_blu_Byte0))
		|| (BG != ITE_readPE(testpattern_SolidColor_data_u16_SolidColor_data_gib_Byte0)))
	{
		LOS_Log("SolidColorData written Fail");
		mmte_testResult(TEST_FAILED);
	}
	else
	{
		LOS_Log("SolidColorData written Pass");
		mmte_testResult(TEST_PASSED);
	}

	LOS_Log("\n RED Value %d", ITE_readPE(testpattern_SolidColor_data_u16_SolidColor_data_red_Byte0));
		
	LOS_Log("\n GIR Value %d", ITE_readPE(testpattern_SolidColor_data_u16_SolidColor_data_gir_Byte0));

	LOS_Log("\n Blue Value %d", ITE_readPE(testpattern_SolidColor_data_u16_SolidColor_data_blu_Byte0));

	LOS_Log("\n GIB Value %d", ITE_readPE(testpattern_SolidColor_data_u16_SolidColor_data_gib_Byte0));

}




