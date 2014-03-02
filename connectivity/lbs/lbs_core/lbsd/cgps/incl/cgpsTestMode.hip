/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#ifndef __CGPSTESTMODE_HIP__
#define __CGPSTESTMODE_HIP__
/**
* \file cgpsTestMode.hip
* \date 19/11/2009
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B> This file contains all function prototypes used in cgps9TestMode.c\n
*
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> 19.11.09</TD><TD> BALAJI NATAKALA </TD><TD> Creation </TD>
*     </TR>
* </TABLE>
*/

#ifdef ATT_LOGGING_ENABLE
uint8_t CGPS9_01GenerateCheckSum(char* vp_NmeaBuffer, uint16_t vp_BufferLen);
#endif /*ATT_LOGGING_ENABLE*/

#ifdef CGPS_CNO_VALUE_FTR
uint16_t CGPS9_03GetCnoValue( void );

const t_OperationDescriptor* CGPS9_04HandleGetCnoMessage(t_RtkObject* p_FsmObject);
#endif /* CGPS_CNO_VALUE_FTR */



void CGPS9_23LogPosition (uint8_t vl_index, uint32_t Status_Code, s_GN_GPS_Nav_Data *pl_nav_data_to_use);


/**
* \fn t_OperationDescriptor* CGPS9_30ProductionTestStartReq(t_RtkObject* p_FsmObject)
* \brief 
*\param p_FsmObject parameter use with fsm table
*/
const t_OperationDescriptor* CGPS9_30ProductionTestStartReq(t_RtkObject* p_FsmObject);

/**
* \fn t_OperationDescriptor* CGPS9_31ProductionTestStopReq(t_RtkObject* p_FsmObject)
* \brief 
*\param p_FsmObject parameter use with fsm table
*/
const t_OperationDescriptor* CGPS9_31ProductionTestStopReq(t_RtkObject* p_FsmObject);

/**
* \fn uint32_t CGPS9_32CheckROMVersion
* \brief check to CG2900 ROM version from the #STAT message.
* \return value is ROM version number
*/
uint32_t CGPS9_32CheckROMVersion(void);

/**
* \fn void CGPS9_35ProductionTREQ_InputSignalSNR(t_CGPS_TestRequest *p_cgpsTREQ_Req)
* \brief Required TBD Production Test kind of InputSignalSNR test function.
*/
void CGPS9_35ProductionTREQ_InputSignalSNR(t_CGPS_TestRequest *p_cgpsTREQ_Req);

/**
* \fn void CGPS9_36ProductionTREQ_ME(t_CGPS_TestRequest *p_cgpsTREQ_Req)
* \brief Required TBD Production Test kind of ME test function.
*/
void CGPS9_36ProductionTREQ_ME(t_CGPS_TestRequest *p_cgpsTREQ_Req);

/**
* \fn void CGPS9_37ProductionTREQ_RTC(t_CGPS_TestRequest *p_cgpsTREQ_Req)
* \brief Required TBD Production Test kind of RTC test function.
*/
void CGPS9_37ProductionTREQ_RTC(t_CGPS_TestRequest *p_cgpsTREQ_Req);

/**
* \fn void CGPS9_38ProductionTREQ_InputPin(t_CGPS_TestRequest *p_cgpsTREQ_Req)
* \brief Required TBD Production Test kind of InputPin test function.
*/
void CGPS9_38ProductionTREQ_InputPin(t_CGPS_TestRequest *p_cgpsTREQ_Req);

/**
* \fn void CGPS9_39ProductionTREQ_Outputpin(t_CGPS_TestRequest *p_cgpsTREQ_Req)
* \brief Required TBD Production Test kind of Outputpin test function.
*/
void CGPS9_39ProductionTREQ_Outputpin(t_CGPS_TestRequest *p_cgpsTREQ_Req);

/**
* \fn void CGPS9_40ProductionTREQ_ROMChecksum(t_CGPS_TestRequest *p_cgpsTREQ_Req)
* \brief Required TBD Production Test kind of ROMChecksum test function.
*/
void CGPS9_40ProductionTREQ_ROMChecksum(t_CGPS_TestRequest *p_cgpsTREQ_Req);

/**
* \fn void CGPS9_41ProductionTREQ_PatchChecksum(t_CGPS_TestRequest *p_cgpsTREQ_Req)
* \brief  Required TBD Production Test kind of PatchChecksum test function. 
*/
void CGPS9_41ProductionTREQ_PatchChecksum(t_CGPS_TestRequest *p_cgpsTREQ_Req);

/**
* \fn void CGPS9_42ProductionTREQ_BIST(t_CGPS_TestRequest *p_cgpsTREQ_Req)
* \brief Required TBD Production Test kind of BIST test function. 
*/
void CGPS9_42ProductionTREQ_BIST(t_CGPS_TestRequest *p_cgpsTREQ_Req);

/**
* \fn void CGPS9_43ProductionTREQ_SpectrumAnalyzer(t_CGPS_TestRequest *p_cgpsTREQ_Req)
* \brief Spectrum Analyzer test function. in this funtion made TREQ 9 message from the Test Request paramater
* \param t_CGPS_TestRequest 
*/
void CGPS9_43ProductionTREQ_SpectrumAnalyzer(t_CGPS_TestRequest *p_cgpsTREQ_Req);

/**
* \fn const t_OperationDescriptor* CGPS9_44TREQTimerExpired(t_RtkObject* p_FsmObject)
* \brief Production test using to Timer funtion for the patch download, Request command, Read Test Result.
*/
const t_OperationDescriptor* CGPS9_44TREQTimerExpired(t_RtkObject* p_FsmObject);

/**
* \fn uint32_t CGPS9_45Get_ProductionTestResult()
* \brief Read from Chipset data buffer the Production test Result.
* \Now using only Spectrum Analyzer test result.
*/
uint32_t CGPS9_45Get_ProductionTestResult();



#endif /* __CGPSTESTMODE_HIP__ */

