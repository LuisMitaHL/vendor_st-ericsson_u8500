/*****************************************************************************
 *  ST-Ericsson GPS Technology
 *  Copyright (C) ST-Ericsson SA 2009. All rights reserved.
 *  This code is ST-Ericsson proprietary and confidential.
 *  Any use of the code for whatever purpose is subject to
 *  specific written permission of ST-Ericsson.
 *
 *  Author: Balaji Natakala
 *  Email : balaji.natakala@stericsson.com
 *****************************************************************************/

#ifndef SMS_CLIENT_SMS_HANDLING_H
#define SMS_CLIENT_SMS_HANDLING_H

#ifdef AMIL_LCS_FTR
#include "smsClient.h"
#endif /* AMIL_LCS_FTR */

#ifdef SMSCLIENT_SMSHANDLING_C
#define GLOBAL 
#else
#define GLOBAL extern
#endif

#define K_SMSCLIENT_WAP_PUSH_PORT_ID         2948
#define K_SMSCLIENT_SUPL_SMS_PORT_ID        7275   /* MT SMS port number */
#define K_SMSCLIENT_MESSAGE_TIMER_VALUE     30     /*default time in seconds for incomplete message deletion from message list */


/**
 *  @brief Maximum supported SLP initiated SUPL Push body length.
 */
#define K_SMSCLIENT_SUPL_PUSH_BODY_MAX_LENGTH 512

/**
 *  @brief Maximum supported SLP initiated SUPL Push header length.
 */
#define K_SMSCLIENT_SUPL_PUSH_HDR_MAX_LENGTH 256

/**
 *  @brief Length of SUPL Push hash.
 */
#define K_SMSCLIENT_SUPL_PUSH_HASH_OCTET_LENGTH 8


/**
 *  @brief Indicates the Push method used to transport the SUPL trigger message.
 */
typedef enum {
    /**
     *  @brief SUPL trigger message over unspecified Push method.
     */
    K_SMSCLIENT_SUPL_PUSH_METHOD_UNSPECIFIED = 0,
    /**
     *  @brief SUPL trigger message over mobile terminated SMS.
     */
    K_SMSCLIENT_SUPL_PUSH_METHOD_MT_SMS      = 1,
    /**
     *  @brief SUPL trigger message over WAP Push.
     */
    K_SMSCLIENT_SUPL_PUSH_METHOD_WAP_PUSH    = 2,
    /**
     *  @brief SUPL trigger message over SIP Push.
     */
    K_SMSCLIENT_SUPL_PUSH_METHOD_SIP_PUSH    = 3,
    /**
     *  @brief SUPL trigger message over UDP/IP bearer.
     */
    K_SMSCLIENT_SUPL_PUSH_METHOD_UDP_IP      = 4
}t_smsClient_SuplPushMethod;


/**
 *  @brief Indicates parameters related to the SUPL Push message.
 */
typedef struct {
    /**
     *  @brief Contains the Push method used to transport the trigger message.
     */
    t_smsClient_SuplPushMethod v_PushMethod;
    /**
     *  @brief Indicates if Hash code derived from Push message is present.
     */
    bool                       v_PushHashPresent;
    /**
     *  @brief Contains the length of the Push header message.
     *  Set to zero if Push header is not specified.
     */
    uint32_t                   v_PushHeaderLength;
    /**
     *  @brief Contains the length of the Push body message.
     *  A value of zero will result in an decoding error.
     */
    uint32_t v_PushBodyLength;
    /**
     *  @brief Contains the hash code derived from the SUPL Push message. Valid if v_PushHashPresent is true.
     *  Generation of Hash code is defined by OMA SUPL.
     *  Array size defined by @ref K_SMSCLIENT_SUPL_PUSH_HASH_OCTET_LENGTH.
     */
    uint8_t                a_PushHash[ K_SMSCLIENT_SUPL_PUSH_HASH_OCTET_LENGTH ];
    /**
     *  @brief Push Header as defined by OMA SUPL.
     *  Encoded as a null terminated ASCII string.
     *  Array size defined by @ref K_SMSCLIENT_SUPL_PUSH_HDR_MAX_LENGTH.
     */
    uint8_t                a_PushHeader[ K_SMSCLIENT_SUPL_PUSH_HDR_MAX_LENGTH ];
    /**
     *  @brief Push Body as defined by OMA SUPL.
     *  Contains an ASN1 PER encoded SUPL PDU. The length is defined by the first two octets.
     *  Array size defined by @ref K_SMSCLIENT_SUPL_PUSH_BODY_MAX_LENGTH.
     */
    uint8_t                a_PushBody[ K_SMSCLIENT_SUPL_PUSH_BODY_MAX_LENGTH ];
}t_SuplPushParameters;



typedef struct v_smsClient_TpduElementTag {
    struct     v_smsClient_TpduElementTag *p_Next;   /* next concatenated TPDU */
    uint8_t    v_SequenceNo;                           /* concatenated message TPDU sequence number */
    SMS_Slot_t v_SMSSlot;                             /* MTS storage location */
    uint16_t   v_UserDataLength;
    uint8_t*   p_UserData;
} t_smsClient_TpduElement;


typedef struct t_smsClient_MessageElementTag {
    // Message
    struct t_smsClient_MessageElementTag *p_Next;  /* next message */
    SMS_ConcatMessageRefNumber_t v_MessageReference;      /* message identifier */
    // TPDUs
    t_smsClient_TpduElement     *p_Head;     /* current message TPDU list */
    uint16_t v_TotalTpduLength;  /* length of all listed parts */
    uint8_t v_TpduCount;         /* current listed parts */
    uint8_t v_MaxTpdu;           /* max concatenated parts for message */
} t_smsClient_MessageElement;


typedef struct {
    t_smsClient_MessageElement *p_Head;
    uint8_t v_MessageCount;
    bool    v_MessageTimerRunning;
} t_smsClient_MessageList;



bool smsClient2_01RegisterApplicationPort(void);
bool smsClient2_02DeregisterApplicationPort(void);
void smsClient2_03SmsPortRegistered(void);
void smsClient2_04InitialiseMessageList(void);
void smsClient2_05SetSmsPort(const uint16_t vp_SmsPort);
void smsClient2_06SetMessageTimer(const uint16_t vp_NewTimerVal);
static bool smsClient2_09ApplicationPortMatch(const SMS_ApplicationPortRange_t * const ApplicationPortRange_p);
static uint8_t smsClient2_10MessageCount(void);
static bool smsClient2_11StoreMessage(const SMS_ShortMessageReceived_t * const MessageData_p, SMS_ConcatMessageRefNumber_t * const MessageReference_p);
static uint16_t smsClient2_12MessageDataLength(const SMS_ConcatMessageRefNumber_t v_MessageReference);
static bool smsClient2_13MessageComplete(const SMS_ConcatMessageRefNumber_t v_MessageReference);
static void smsClient2_14DestroyMessageList(void);
static bool smsClient2_15ExtractMessage(const SMS_ConcatMessageRefNumber_t v_MessageReference, uint8_t * const CompleteUserData_p);
static bool smsClient2_16AnyMessagesIncomplete(void);
static void smsClient2_17StartMessageTimer(void);
static bool smsClient2_18MessageListTPDUAdd(t_smsClient_MessageElement * const MessageElement_p,
                                             const uint8_t v_SequenceNo, const bool IsReplace, const SMS_Slot_t * const SMS_Slot_p, const uint16_t TPDU_DataLength, const uint8_t * const TPDU_Data_p);
static void smsClient2_19MessageListTPDUDelete(t_smsClient_MessageElement * const MessageElement_p, const uint8_t SequenceNo);
static void smsClient2_20MessageListTPDUDeleteAll(t_smsClient_TpduElement * TPDU_Element_p);
static t_smsClient_TpduElement *smsClient2_21MessageListTPDUFind(const t_smsClient_MessageElement * const MessageElement_p, const uint8_t SequenceNo, t_smsClient_TpduElement * *const PrevTPDU_Element_pp);
static bool smsClient2_22MessageListTPDUUserDataRead(const uint16_t TPDU_DataLength,
                                                      const uint8_t * const TPDU_Data_p, const bool FirstPart, uint16_t * const UserDataLength_p, uint8_t * *const UserData_pp);
static t_smsClient_MessageElement *smsClient2_23MessageListMessageElementAdd(const SMS_ConcatMessageRefNumber_t v_MessageReference, const uint8_t v_MaxTpdu);
static void smsClient2_24MessageListMessageElementDelete(const SMS_ConcatMessageRefNumber_t v_MessageReference);
static t_smsClient_MessageElement *smsClient2_25MessageListMessageElementFind(const SMS_ConcatMessageRefNumber_t v_MessageReference, t_smsClient_MessageElement * *const PrevMessageElement_pp);
static bool smsClient2_26TPDUUserDataHeaderRead(const uint8_t TPDU_UserHeaderDataLength, const uint8_t* const TPDU_UserHeaderData_p,
                                                      uint16_t* const ConcatMessRefNum_p, uint8_t* const MaxNumConcatMsgs_p,  uint8_t* const MsgSeqNum_p);
static bool smsClient2_27TPDUConcatInfoRead(const uint16_t TPDU_DataLength, const uint8_t * const TPDU_Data_p,
                                                  uint16_t * const ConcatMessRefNum_p, uint8_t *const MaxNumConcatMsgs_p, uint8_t* const MsgSeqNum_p);
static void smsClient2_28SendDeliverReport(void);


#endif /*SMS_CLIENT_SMS_HANDLING_H*/

