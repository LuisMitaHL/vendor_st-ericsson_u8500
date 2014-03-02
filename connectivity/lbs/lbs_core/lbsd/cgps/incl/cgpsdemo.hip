/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#ifndef __CGPSDEMO_HIP__
#define __CGPSDEMO_HIP__
/**
* \file cgpsdemo.hip
* \date 02/04/2008
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B> This file contain all prototype used by External application 
* to communicate with the CGPSDEMO module.\n
* 
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> 25.01.08 </TD><TD> Y.DESAPHI </TD><TD> Creation </TD>
*     </TR>
* </TABLE>
*/
#ifdef __CGPS3DEMO_C__
#define GLOBAL
#else
#define GLOBAL extern
#endif /*__CGPS3DEMO_C__*/

GLOBAL uint32_t CGPS3_00demoSetReceivePacket(t_RtkObject* p_FsmObject);

GLOBAL uint32_t CGPS3_01demoGetReceivePacket(uint8_t* pp_Data ,uint32_t vp_Length);

#undef GLOBAL

#endif /*__CGPSDEMO_HIP__*/
