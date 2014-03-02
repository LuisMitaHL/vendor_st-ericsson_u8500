/*=============================================================================
 * Copyright (C) ST-Ericsson SA 2010
 * Author: wlan_humac.com
 * License terms: GNU General Public License (GPL), version 2.
 *===========================================================================*/
/**
* \file
* \ingroup HIF
*
* \brief Host Interface API
* This module provides the definitions of common host/device message interface,
* it is shared by both the host driver and the firmware.
*/

#ifndef HI_API_H
#define HI_API_H

/*--------------------------------------------------------------------------*
 * Includes								 *
 *--------------------------------------------------------------------------*/

/*Use your own platform's stddefs.h or from mod_global*/
/*#include "stddefs.h"*/

/*--------------------------------------------------------------------------*
 * Define								   *
 *--------------------------------------------------------------------------*/

#define HI_API_VER		  0x0400	/* This should be ORed with FW API version */
#define HI_API_VER_MASK	     0xFF00	/* Mask for HI Api Ver */

typedef struct HI_MSG_HDR_S {
	uint16 MsgLen;		/* length in bytes from MsgLen to end of payload */
	uint16 MsgId;
	/* Payload is followed */
} HI_MSG_HDR;

#define HI_STATUS_SUCCESS	   0
#define HI_STATUS_INVALID_DATA      1

#define HI_MAX_MSG_SIZE	     8188	/* (8192-4) */

#define HI_IND_BASE		 0x0800

#define HI_EXCEPTION_IND_ID	 (HI_IND_BASE+0x00)	/* HI exception id */
#define HI_STARTUP_IND_ID	   (HI_IND_BASE+0x01)	/* HI config (1st)id */
#define HI_TRACE_IND_ID	     (HI_IND_BASE+0x02)	/* HI trace id */
#define HI_GENERIC_IND_ID	   (HI_IND_BASE+0x03)	/* HI gereric indication id */

#define HI_APPL_IND_ID_MIN	  (HI_IND_BASE+0x04)
#define HI_APPL_IND_ID_MAX	  (HI_IND_BASE+0x3FF)

/* Message bases */
#define HI_REQ_BASE		 0x0000
#define HI_CNF_BASE		 0x0400

/* Message IDs */
#define HI_MEM_READ_REQ_ID	  (HI_REQ_BASE+0x00)
#define HI_MEM_READ_CNF_ID	  (HI_CNF_BASE | HI_MEM_READ_REQ_ID)
#define HI_MEM_WRITE_REQ_ID	 (HI_REQ_BASE+0x01)
#define HI_MEM_WRITE_CNF_ID	 (HI_CNF_BASE | HI_MEM_WRITE_REQ_ID)

/* Use projects are using different names for this*/
#define HI_READ_MEM_REQ_ID		    HI_MEM_READ_REQ_ID
#define HI_READ_MEM_CNF_ID		    HI_MEM_READ_CNF_ID
#define HI_WRITE_MEM_REQ_ID		    HI_MEM_WRITE_REQ_ID
#define HI_WRITE_MEM_CNF_ID		    HI_MEM_WRITE_CNF_ID

#define HI_CONFIG_REQ_ID	    (HI_REQ_BASE+0x02)
#define HI_CONFIG_CNF_ID	    (HI_CNF_BASE | HI_CONFIG_REQ_ID)
#define HI_GENERIC_REQ_ID	   (HI_REQ_BASE+0x03)
#define HI_GENERIC_CNF_ID	   (HI_CNF_BASE | HI_GENERIC_REQ_ID)

/* Application Message ID limits */
#define HI_APPL_REQ_ID_MIN	  (HI_REQ_BASE+0x04)
#define HI_APPL_CNF_ID_MIN	  (HI_CNF_BASE | HI_APPL_REQ_ID_MIN)
#define HI_APPL_REQ_ID_MAX	  (HI_REQ_BASE+0x3FF)
#define HI_APPL_CNF_ID_MAX	  (HI_CNF_BASE+0x3FF)

/* Temporary messsage ID */
#define HI_SOFT_BOOT_REQ_ID	 0x03FF

#define HI_GET_MSG_SEQ(_id)	 ((_id) >> 13)
#define HI_PUT_MSG_SEQ(_id, _seq)    ((_id) | (uint16)((_seq) << 13))
#define HI_MSG_ID_MASK	      0x1FFF
#define HI_MSG_SEQ_RANGE	    0x0007
#define HI_MSG_SLEEP_BIT	    0x1000

/* Generic Message Request Ids */
#define HI_GEN_REQ_TRACE_CONTROL_REQ_ID    0
#define HI_GEN_REQ_HI_LOOPBACK_TEST_REQ_ID 1
#define HI_GEN_REQ_HI_CLI_REQ_ID	   2
#define HI_GEN_REQ_CONTROL_REQ_ID	  3

/* Generic Message Design Verification Test Request Ids */
#define HI_GEN_REQ_HI_DVT_LOOPBACK_ID	100
#define HI_GEN_REQ_HI_DVT_PARAMETERS_ID      101
#define HI_GEN_REQ_HI_DVT_AIRPCAP_REQ_ID     102

#define HI_GEN_IND_HI_DVT_DATA_IND_ID	103

#define HI_CONFIG_MASK_DPD	  0x0001

typedef struct HI_CONFIG_REQ_S {
	uint16 MsgLen;
	uint16 MsgId;
	uint16 OperationFlags;	/* FW config options */
	uint16 ConfigMask;	/* what config struct contained */
	uint32 ConfigData[1];	/* 1st 4 bytes of the config struct */
} HI_CONFIG_REQ;

typedef struct HI_CONFIG_CNF_S {
	uint16 MsgLen;
	uint16 MsgId;
	uint32 Result;		/* 0 means success */
} HI_CONFIG_CNF;

/* This is used for generic request that every firmware shall support */
typedef struct HI_GENERIC_REQ_S {
	uint16 MsgLen;
	uint16 MsgId;
	uint32 RequestId;
	uint32 RequestData[1];
} HI_GENERIC_REQ;

typedef struct HI_GENERIC_CNF_S {
	uint16 MsgLen;
	uint16 MsgId;
	uint32 RequestId;
	uint32 Result;		/* 0 means success */
	uint32 ConfirmationData[1];
} HI_GENERIC_CNF;

typedef struct HI_GENERIC_LOOPBACK_TEST_REQ_S {
	uint16 MsgLen;
	uint16 MsgId;
	uint32 RequestId;
	uint32 Pad;
	uint32 RequestData[1];
} HI_GENERIC_LOOPBACK_TEST_REQ;

typedef struct HI_GEN_REQ_CONTROL_REQ_S {
	HI_GENERIC_REQ sHiGenericReq;
	uint32 Iterations;
	uint32 DataLength;
} HI_GEN_REQ_CONTROL_REQ;

#define HI_EXCEP_FILENAME_SIZE      48

/* Exception Indication message */
typedef struct HI_EXCEPTION_IND_S {
	uint16 MsgLen;
	uint16 MsgId;
	uint32 Reason;
	uint32 R0;
	uint32 R1;
	uint32 R2;
	uint32 R3;
	uint32 R4;
	uint32 R5;
	uint32 R6;
	uint32 R7;
	uint32 R8;
	uint32 R9;
	uint32 R10;
	uint32 R11;
	uint32 R12;
	uint32 SP;
	uint32 LR;
	uint32 PC;
	uint32 CPSR;
	uint32 SPSR;
	uint8 FileName[HI_EXCEP_FILENAME_SIZE];
} HI_EXCEPTION_IND;

/* Exception indication reason */
#define DBG_UNDEF_INST      0
#define DBG_PREFETCH_ABORT  1
#define DBG_DATA_ABORT      2
#define DBG_UNKNOWN_ERROR   3
#define DBG_ASSERT	  4

#define HI_MAX_CONFIG_TABLES	4
#define HI_SW_LABEL_MAX	     128

#define FW_TYPE_ETF		 0
#define FW_TYPE_WFM		 1
#define FW_TYPE_WSM		 2
#define FW_TYPE_HI_TEST	     3
#define FW_TYPE_PLATFORM_TEST       4

/* Trace Indication message */
#define HI_TRACE_ASCII	      0x00	/* trace types */
#define HI_TRACE_OVERFLOW	   0x01
#define HI_TRACE_PRINTF	     0x02
#define HI_TRACE_BYTES	      0x03
#define HI_TRACE_WORDS	      0x04
#define HI_TRACE_DWORDS	     0x05
#define HI_TRACE_MACADDR	    0x06
#define HI_TRACE_STRING	     0x07

typedef struct HI_TRACE_RECORD_S {
	uint16 Length;		/* of the trace struct, 32-bit alligned */
	uint8 Type;		/* one of the above trace types */
	uint8 OffTail;
	uint32 TimeStamp;
	uint32 TraceData[1];	/* 1st 4 bytes of the trace data */
	/* rest of the trace data */
} HI_TRACE_RECORD;

typedef struct HI_TRACE_IND_S {
	uint16 MsgLen;
	uint16 MsgId;
	uint32 Flags;
	HI_TRACE_RECORD TraceRecords[1];	/* 1st record */
	/* other trace records */
} HI_TRACE_IND;

/* Startup Indication message */
typedef struct HI_STARTUP_IND_S {
	uint16 MsgLen;
	uint16 MsgId;
	uint16 NumInpChBufs;
	uint16 SizeInpChBuf;
	uint16 HardwareId;
	uint16 HardwareSubId;
	uint16 InitStatus;
	uint16 FirmwareCap;
	uint16 FirmwareType;
	uint16 FirmwareApiVer;
	uint16 FirmwareBuildNumber;
	uint16 FirmwareVersion;
	uint8 FirmwareLabel[HI_SW_LABEL_MAX];
	uint32 Configuration[HI_MAX_CONFIG_TABLES];
} HI_STARTUP_IND;

/* This is used for generic indication that every firmware shall support */
typedef struct HI_GENERIC_IND_S {
	uint16 MsgLen;
	uint16 MsgId;
	uint32 IndicationId;
	uint32 IndicationData[1];
} HI_GENERIC_IND;

#define HI_RW_F_FIQ_PROTECT	 0x40
#define HI_RW_F_IRQ_PROTECT	 0x80
#define HI_RW_F_PROTECT_MASK	(HI_RW_F_FIQ_PROTECT|HI_RW_F_IRQ_PROTECT)

#define HI_MEM_BLK_DWORDS	   (1024/4)

typedef struct HI_MEM_READ_REQ_S {
	uint16 MsgLen;
	uint16 MsgId;
	uint32 Address;
	uint16 Length;
	uint16 Flags;
} HI_MEM_READ_REQ;

typedef struct HI_MEM_READ_CNF_S {
	uint16 MsgLen;
	uint16 MsgId;
	uint32 Result;
	uint32 Data[HI_MEM_BLK_DWORDS];
} HI_MEM_READ_CNF;

typedef struct HI_MEM_WRITE_REQ_S {
	uint16 MsgLen;
	uint16 MsgId;
	uint32 Address;
	uint16 Length;
	uint16 Flags;
	uint32 Data[HI_MEM_BLK_DWORDS];
} HI_MEM_WRITE_REQ;

typedef struct HI_MEM_WRITE_CNF_S {
	uint16 MsgLen;
	uint16 MsgId;
	uint32 Result;
} HI_MEM_WRITE_CNF;

/* some projects use different names*/
typedef HI_MEM_READ_REQ HI_READ_MEM_REQ;
typedef HI_MEM_READ_CNF HI_READ_MEM_CNF;
typedef HI_MEM_WRITE_REQ HI_WRITE_MEM_REQ;
typedef HI_MEM_WRITE_CNF HI_WRITE_MEM_CNF;

/* DPD Flags definitions */
#define HI_DPD_PHY_F__FIRST_ELEMENT    (1 << 0)
#define HI_DPD_PHY_F__INTERIM_ELEMENT  (1 << 1)
#define HI_DPD_PHY_F__LAST_ELEMENT     (1 << 2)

typedef struct HI_DPD_CONFIG_S {
	uint16 Length;		/* length in bytes of the DPD struct */
	uint16 Version;		/* version of the DPD record */
	uint8 MacAddress[6];	/* MAC address of the device */
	uint16 Flags;		/* record options */
	uint32 SddData[1];	/* 1st 4 bytes of the Static & Dynamic Data */
} HI_DPD_CONFIG;

#endif				/* HI_API_H */
