/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 


/*F(***************************************************************************
 * File name : lsimupagps.hip
 * Project :   LMS
 * Module :    LSIMUP
 * Date :      06/11/06
 * Version :   1.0
 *----------------------------------------------------------------------------*
 *----------------------------------------------------------------------------*
 *                                    DESCRIPTION
 *
 * This file contains all internal functions required by LSIMUP
 *
 *
 *----------------------------------------------------------------------------*
 *----------------------------------------------------------------------------*
 *
 *----------------------------------------------------------------------------*
 *----------------------------------------------------------------------------*
 *                                    EVOLUTION
 *----------------------------------------------------------------------------*
 * Date     | Author     | Arnb    | Description
 *----------------------------------------------------------------------------*
 * 15/11/06 |P. Meignen              | Creation
 * 31/01/07 |P.Meignen               | CR LMSqb06635
 * 14/02/07 |P.Meignen               |lsimup5_03ALClose CR LMSqb06635
 *----------------------------------------------------------------------------*
 **************************************************************************)F*/

#ifndef LSIMUP_HIP
#define LSIMUP_HIP

#ifndef LSIM1UP_C
#define GLOBAL extern
#else
#define GLOBAL
#endif

/*======================= Init/Deinit functions ================================*/

GLOBAL void lsim1_01InitReq(t_RtkObject* p_FsmObject);
GLOBAL void lsim1_02DeInitReq(t_RtkObject* p_FsmObject);
GLOBAL void lsim1_03MobileInfoReq(t_RtkObject* p_FsmObject);

GLOBAL void lsim1_04EstablishBearerReq(t_RtkObject* p_FsmObject);
GLOBAL void lsim1_05TcpIpConnectReq(t_RtkObject* p_FsmObject);
GLOBAL void lsim1_06DisconnectReq(t_RtkObject* p_FsmObject);
GLOBAL void lsim1_07CloseBearerReq(t_RtkObject* p_FsmObject);
GLOBAL void lsim1_08SendDataReq(t_RtkObject* p_FsmObject);
GLOBAL void lsim1_09ReceiveDataCnf(t_RtkObject* p_FsmObject);

#undef GLOBAL
#ifndef LSIM3UPGNS_C
#define GLOBAL extern
#else
#define GLOBAL
#endif
void lsim3_00MobileInfoReq( e_lsimup_ValidityField v_RequestedData );
void lsim3_01TcpIpConnectReq( t_lsimup_ConnectionHandle v_ConnectionHandle  , t_lsimup_TcpIpConnectReqM *p_Req );
void lsim3_02TcpIpDisconnectReq( t_lsimup_ConnectionHandle v_ConnectionHandle );
void lsim3_03SendDataReq( t_lsimup_ConnectionHandle v_ConnectionHandle , uint8_t* p_Data , uint32_t v_DataLen);
void lsim3_04ReceiveDataCnf( t_lsimup_ConnectionHandle v_ConnectionHandle );
void lsim3_15EstablishBearerReq();
void lsim3_18CloseBearerReq();


#undef GLOBAL

#endif  /* LSIMUP_HIP */



