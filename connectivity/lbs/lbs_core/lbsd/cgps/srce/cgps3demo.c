/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

/**
* \file cgps3demo.c
* \date 02/04/2008
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B>  This file contains demo interface between GPS library and demoagps module
*
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> 14.03.08 </TD><TD> M.BELOU </TD><TD> Creation </TD>
*     </TR>
*     <TR>
*             <TD> 02.04.08 </TD><TD> Y.DESAPHI </TD><TD> CGPS adaptation </TD>
*     </TR>
* </TABLE>
*/

#define __CGPS3DEMO_C__

#include "cgpsdemo.h"

#undef  MODULE_NUMBER
#define MODULE_NUMBER   MODULE_CGPS

#undef  PROCESS_NUMBER
#define PROCESS_NUMBER  PROCESS_CGPS

#undef  FILE_NUMBER
#define FILE_NUMBER     3



#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 0

uint32_t CGPS3_00demoSetReceivePacket(t_RtkObject* p_FsmObject)
{
    t_CGPS_NmeaTestControlInd * pl_CGPS_NmeaTestControl = (t_CGPS_NmeaTestControlInd *)MC_CGPS_GET_MSG_POINTER(p_FsmObject);
    uint32_t vl_EmptyDataIndex = 0;

    if (((vg_demoBufferLenght + pl_CGPS_NmeaTestControl->v_Length) >= CGPS_DEMO_BUFFER_SIZE) || (pl_CGPS_NmeaTestControl->v_Length ==0))
    {
        /*not enough place in the buffer*/
        return 0;
    }

    vl_EmptyDataIndex = CGPS_DEMO_BUFFER_SIZE - vg_demoCurrentWriteIndex;

    if ( pl_CGPS_NmeaTestControl->v_Length > vl_EmptyDataIndex)
    {
        if (vl_EmptyDataIndex)
        {
            memcpy((uint8_t*)&pg_CGPS_DemoReceiveTrameBuffer[vg_demoCurrentWriteIndex],pl_CGPS_NmeaTestControl->p_Data,vl_EmptyDataIndex);
        }
        if ((pl_CGPS_NmeaTestControl->v_Length-vl_EmptyDataIndex)>0)
        {
            memcpy((uint8_t*)&pg_CGPS_DemoReceiveTrameBuffer[0],pl_CGPS_NmeaTestControl->p_Data+vl_EmptyDataIndex,pl_CGPS_NmeaTestControl->v_Length-vl_EmptyDataIndex);
        }

        vg_demoCurrentWriteIndex = pl_CGPS_NmeaTestControl->v_Length-vl_EmptyDataIndex;
    }
    else
    {
        memcpy((uint8_t*)&pg_CGPS_DemoReceiveTrameBuffer[vg_demoCurrentWriteIndex],pl_CGPS_NmeaTestControl->p_Data,pl_CGPS_NmeaTestControl->v_Length);
        vg_demoCurrentWriteIndex+=pl_CGPS_NmeaTestControl->v_Length;
    }

    MC_RTK_FREE_MEMORY(pl_CGPS_NmeaTestControl->p_Data);

    vg_demoBufferLenght += pl_CGPS_NmeaTestControl->v_Length;
    return pl_CGPS_NmeaTestControl->v_Length;
}



#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 1
uint32_t CGPS3_01demoGetReceivePacket(uint8_t* pp_Data ,uint32_t vp_Length)
{
    uint32_t vl_length=0;
    uint32_t vl_DataIndex;


    if (vg_demoBufferLenght<0)
    {
        return 0;
    }
    if (vg_demoBufferLenght==0)
    {
        return 0;
    }
    vl_length = vg_demoBufferLenght;

    if (vl_length>=vp_Length)
    {
        vl_length = vp_Length;
    }


    if ((vg_demoCurrentReadIndex + vl_length) > CGPS_DEMO_BUFFER_SIZE)
    {
        vl_DataIndex = CGPS_DEMO_BUFFER_SIZE -vg_demoCurrentReadIndex;
        if (vl_DataIndex!=0)
        {
            memcpy((uint8_t*)pp_Data,(uint8_t*)&pg_CGPS_DemoReceiveTrameBuffer[vg_demoCurrentReadIndex],vl_DataIndex);
        }
        else
        {
           memcpy((uint8_t*)pp_Data,(uint8_t*)&pg_CGPS_DemoReceiveTrameBuffer[0],vl_length-vl_DataIndex);
        }
        vg_demoBufferLenght -=vl_length;
        vg_demoCurrentReadIndex=vl_length-vl_DataIndex;
    }
    else
    {
        memcpy((uint8_t*)pp_Data,(uint8_t*)&pg_CGPS_DemoReceiveTrameBuffer[vg_demoCurrentReadIndex],vl_length);

        vg_demoCurrentReadIndex+=vl_length;
        vg_demoBufferLenght -=vl_length;
    }
    return vl_length;
}




#undef __CGPS3DEMO_C__
