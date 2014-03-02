/*****************************************************************************
 *  ST-Ericsson GPS Technology
 *  Copyright (C) 2009 ST-Ericsson.
 *  All rights reserved
 *  Date: 03-12-2009
 *  Author: Rahul Ranade
 *  Email : rahul.ranade@stericsson.com
 *****************************************************************************/
#define LSIM1UP_C

#ifdef AGPS_UP_FTR
#define ACCESS_RIGHTS_LSIMUP
#include "lsimup.h"
#undef ACCESS_RIGHTS_LSIMUP

#undef  FILE_NUMBER
#define FILE_NUMBER     1


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 1
void lsim1_01InitReq(t_RtkObject* p_FsmObject)
{
    t_lsimup_InitCnfM* p_MsgInitCnf = NULL;

    p_FsmObject = p_FsmObject;

    p_MsgInitCnf = (t_lsimup_InitCnfM *)MC_RTK_GET_MEMORY(sizeof(*p_MsgInitCnf));

    /* Request for the following information */
    lsim3_00MobileInfoReq( K_LSIMUP_MCC_FIELD | K_LSIMUP_MNC_FIELD | K_LSIMUP_LAC_FIELD | K_LSIMUP_IMSI_FIELD | K_LSIMUP_MSISDN_FIELD );
    /*LSIMUP Init_CnfM to send to C-GPS */
    p_MsgInitCnf->v_SessionHandle = 0;
    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_LSIMUP,0,PROCESS_CGPS,0,CGPS_LSIMUP_INIT_CNF,(t_MsgHeader*) p_MsgInitCnf);
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 2

void lsim1_02DeInitReq(t_RtkObject* p_FsmObject)
{
    t_lsimup_DeInitReqM* p_MsgDeInitReq = NULL;
    t_lsimup_DeInitCnfM* p_MsgDeInitCnf = NULL;

    p_MsgDeInitReq = (t_lsimup_DeInitReqM*)p_FsmObject->u_ReceivedObj.p_MessageAddress;
    p_MsgDeInitCnf = (t_lsimup_DeInitCnfM *)MC_RTK_GET_MEMORY(sizeof(*p_MsgDeInitCnf));

    /*Prepare the data to send*/
    p_MsgDeInitCnf->v_SessionHandle = p_MsgDeInitReq->v_SessionHandle;

    lsim3_00MobileInfoReq( 0 );
    /*Send the K_LSIMUP_DEINIT_CNF to C-GPS*/
    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_LSIMUP,0,PROCESS_CGPS,0,CGPS_LSIMUP_DEINIT_CNF,(t_MsgHeader*) p_MsgDeInitCnf);

    // @todo. Handle disconnects for existing sessions here
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 3

void lsim1_03MobileInfoReq(t_RtkObject* p_FsmObject)
{
    t_lsimup_MobileInfoReqM* p_MsgMobileInfoReq = NULL;

    p_MsgMobileInfoReq = (t_lsimup_MobileInfoReqM*)p_FsmObject->u_ReceivedObj.p_MessageAddress;

    lsim3_00MobileInfoReq(p_MsgMobileInfoReq->v_RequiredField);
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 4
void lsim1_04EstablishBearerReq(t_RtkObject* p_FsmObject)
{
    p_FsmObject = p_FsmObject;

    lsim3_15EstablishBearerReq();
}
/*- CR LMSqb06635*/


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 5
void lsim1_05TcpIpConnectReq(t_RtkObject* p_FsmObject)
{
    v_ConnectionHandleRunningCount++;
    lsim3_01TcpIpConnectReq(v_ConnectionHandleRunningCount , (t_lsimup_TcpIpConnectReqM*)p_FsmObject->u_ReceivedObj.p_MessageAddress )  ;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 3
void lsim1_06DisconnectReq(t_RtkObject* p_FsmObject)
{
    t_lsimup_TcpIpDisconnectReqM* p_MsgDisconnectReq = NULL;

    p_MsgDisconnectReq = (t_lsimup_TcpIpDisconnectReqM*)p_FsmObject->u_ReceivedObj.p_MessageAddress;

    lsim3_02TcpIpDisconnectReq(p_MsgDisconnectReq->v_ConnectionHandle);
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 4
void lsim1_07CloseBearerReq(t_RtkObject* p_FsmObject)
{
    p_FsmObject = p_FsmObject;

    lsim3_18CloseBearerReq();
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 5
void lsim1_08SendDataReq(t_RtkObject* p_FsmObject)
{
    t_lsimup_SendDataReqM* p_MsgSendDataReq = NULL;

    p_MsgSendDataReq = (t_lsimup_SendDataReqM*)p_FsmObject->u_ReceivedObj.p_MessageAddress;

    lsim3_03SendDataReq(p_MsgSendDataReq->v_ConnectionHandle,p_MsgSendDataReq->v_DataPointer,p_MsgSendDataReq->v_DataLength);
}
#endif /* AGPS_UP_FTR */

