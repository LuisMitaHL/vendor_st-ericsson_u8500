/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#ifndef __CGPSSUPL_HIP__
#define __CGPSSUPL_HIP__
/**
* \file cgpssupl.hip
* \date 16/05/2008
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B> This file contain all prototype used by cgps5supl.c.\n
* 
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> 16.05.08 </TD><TD> Y.DESAPHI </TD><TD> Creation </TD>
*     </TR>
* </TABLE>
*/

/* + LMSQC16384 : LMSQC16386 SUPL WAP Push */
/**
* \fn bool CGPS5_00SuplPushInd(uint32_t vp_SuplHandle, t_cgps_SuplPush* pp_SuplPush)
* \brief This function generates verification field using GSECURITY and deliver PDU to SUPL library
* \return Indicates if the service has been correctly executed or not
* \param vp_SuplHandle [in] Handle dedicated to the application which has pushed the SMS/WAP push
* \param pp_SuplPush parameter containing SUPL INIT
*/
bool CGPS5_00SuplPushInd(uint32_t vp_SuplHandle, t_cgps_SuplPush* pp_SuplPush);
/* - LMSQC16384 : LMSQC16386 SUPL WAP Push */

/**
* \fn bool CGPS5_01EstablishBearerSession()
* \brief Establish a bearer session
* \return Indicates if the session has been correctly established or not
*/
bool CGPS5_01EstablishBearerSession(void);

/**
* \fn const t_OperationDescriptor* CGPS5_02EstablishBearerCnf(t_RtkObject* p_FsmObject)
* \brief Bearer establishment confirmation
* \param p_FsmObject information sent with the message
*/
const t_OperationDescriptor* CGPS5_02EstablishBearerCnf(t_RtkObject* p_FsmObject);

/**
* \fn const t_OperationDescriptor* CGPS5_03EstablishBearerErr(t_RtkObject* p_FsmObject)
* \brief Bearer establishment error
* \param p_FsmObject information sent with the message
*/
const t_OperationDescriptor* CGPS5_03EstablishBearerErr(t_RtkObject* p_FsmObject);

/**
* \fn const t_OperationDescriptor* CGPS5_04TcpIpConnectCnf(t_RtkObject* p_FsmObject)
* \brief TCPIP connection confirmation
* \param p_FsmObject information sent with the message
*/
const t_OperationDescriptor* CGPS5_04TcpIpConnectCnf(t_RtkObject* p_FsmObject);

/**
* \fn const t_OperationDescriptor* CGPS5_05AdpMobileInfo(t_RtkObject* p_FsmObject)
* \brief Mobile information receipt
* \param p_FsmObject information sent with the message
*/
const t_OperationDescriptor* CGPS5_05AdpMobileInfo(t_RtkObject* p_FsmObject);

/**
* \fn const t_OperationDescriptor* CGPS5_06CloseBearerCnf(t_RtkObject* p_FsmObject)
* \brief Bearer closure confirmation
* \param p_FsmObject information sent with the message
*/
const t_OperationDescriptor* CGPS5_06CloseBearerCnf(t_RtkObject* p_FsmObject);

/**
* \fn const t_OperationDescriptor* CGPS5_07CloseBearerInd(t_RtkObject* p_FsmObject)
* \brief Bearer closure indication
* \param p_FsmObject information sent with the message
*/
const t_OperationDescriptor* CGPS5_07CloseBearerInd(t_RtkObject* p_FsmObject);

/**
* \fn const t_OperationDescriptor* CGPS5_08ReceiveDataInd(t_RtkObject* p_FsmObject)
* \brief Receive data indication
* \param p_FsmObject information sent with the message
*/
const t_OperationDescriptor* CGPS5_08ReceiveDataInd(t_RtkObject* p_FsmObject);

/**
* \fn const t_OperationDescriptor* CGPS5_09TcpIpConnectErr(t_RtkObject* p_FsmObject)
* \brief TCPIP connect error
* \param p_FsmObject information sent with the message
*/
const t_OperationDescriptor* CGPS5_09TcpIpConnectErr(t_RtkObject* p_FsmObject);

/**
* \fn bool CGPS5_10InitSuplLibrary(void)
* \brief get server information from internal file
* \return TRUE if the action has been performed successfully
*/
bool CGPS5_10InitSuplLibrary(void);

/**
* \fn const t_OperationDescriptor* CGPS5_11TcpIpDisconnectCnf(t_RtkObject* p_FsmObject)
* \brief reset SUPL appli context
* \param p_FsmObject information sent with the message
*/
const t_OperationDescriptor* CGPS5_11TcpIpDisconnectCnf(t_RtkObject* p_FsmObject);

/**
* \fn const t_OperationDescriptor* CGPS5_12TcpIpDisconnectInd(t_RtkObject* p_FsmObject)
* \brief reset SUPL appli context
* \param p_FsmObject information sent with the message
*/
const t_OperationDescriptor* CGPS5_12TcpIpDisconnectInd(t_RtkObject* p_FsmObject);


bool CGPS5_13StartSUPL( t_CgpsNaf* pp_Handle, t_cgps_ThirdPartyInfo *tp_info );

bool CGPS5_47ThirdPartyStartSUPL( t_CgpsNaf* pp_Handle, t_cgps_ThirdPartyInfo *p_Third_party_info );

/**
* \fn const t_OperationDescriptor* CGPS5_14TcpIpDisconnectErr(t_RtkObject* p_FsmObject)
* \brief reset SUPL appli context
* \param p_FsmObject information sent with the message
*/
const t_OperationDescriptor* CGPS5_14TcpIpDisconnectErr(t_RtkObject* p_FsmObject);

/**
* \fn const t_OperationDescriptor* CGPS5_15CloseBearerErr(t_RtkObject* p_FsmObject)
* \brief Bearer closure error
* \param p_FsmObject information sent with the message
*/
const t_OperationDescriptor* CGPS5_15CloseBearerErr(t_RtkObject* p_FsmObject);

/**
* \fn void CGPS5_16SuplFreeMemory()
* \brief Free memory used by SUPL
*/
void CGPS5_16SuplFreeMemory(void);

void CGPS5_18ConnectSUPLSession(int8_t vp_Index);

void CGPS5_19InitSuplBearer(void);

void CGPS5_20ConvertQoP( t_CgpsNaf* pp_Handle , s_GN_SUPL_QoP * pp_QoP );

void CGPS5_21SuplConfig( void );

void CGPS5_48Delete_LocationID_Store( void );

/**
* \fn CGPS5_25EstablishTcpIpSession( int8_t vp_SessionIndex )
* \brief Requests to establish TCP/IP session
* \param vp_SessionIndex which gives the Index of SUPL sessions DB which has the details
* \like TcpIpAddress, Port, TcpIpAddressType that are required to setup a TCP/IP for that session.
*/
void CGPS5_25EstablishTcpIpSession( int8_t vp_SessionIndex );

/**
* \fn CGPS5_26LsimupMobileInfoReq( void )
* \brief Requests for Mobile Infor through LSIMUP
*/
void CGPS5_26LsimupMobileInfoReq( void );

/* +LMSqc18414 */
void CGPS5_28HandleAuthenticationFailure( void );
/* -LMSqc18414 */

/* +LMSqc20169 */
uint8_t CGPS5_29GetLengthOfMnc(uint16_t v_MCC);
/* -LMSqc20169 */

/* +LMSqc38060 */
void CGPS5_30VerifySuplActivity(void);
void CGPS5_31SendTcpDisconnectReq(uint32_t v_SessionHandle, uint32_t v_ConnectionHandle);
void CGPS5_32HandleTcpConnectionFailure( void );
void CGPS5_33HandleTimerExpiry(void);

/* -LMSqc38060 */

/* +SUPLv2.0 Modifications */
#ifdef AGPS_UP_FTR
void CGPS5_35TriggeredIntervalTime(uint8_t vl_Index);
void CGPS5_36TriggeredStartTime(uint8_t vl_Index);
void CGPS5_37SuplProcessEventTimer(uint8_t v_Index);
void CGPS5_38StartTimer(uint8_t timerId,uint32_t v_Millisec );
void CGPS5_39HandleEventSuplTimer0(t_RtkObject* p_FsmObject);
void CGPS5_40HandleEventSuplTimer1(t_RtkObject* p_FsmObject);
void CGPS5_41HandleEventSuplTimer2(t_RtkObject* p_FsmObject);
void CGPS5_42HandleEventSuplTimer3(t_RtkObject* p_FsmObject);
void CGPS5_43HandleEventSuplTimer4(t_RtkObject* p_FsmObject);
void CGPS5_45SuplConfigExt( void );
void CGPS5_46AreaEventStopTime(uint8_t vl_Index);
void CGPS5_47ThirdParty_Location_Transfer();
#endif /* AGPS_UP_FTR */
/* -SUPLv2.0 Modifications */

#endif /* __CGPSSUPL_HIP__ */
