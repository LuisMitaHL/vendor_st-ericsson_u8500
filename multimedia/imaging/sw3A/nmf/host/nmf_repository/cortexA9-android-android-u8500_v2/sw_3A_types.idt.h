/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*****************************************************************************/
/**
 * This is a Nomadik Multiprocessing Framework generated file, please do not modify it.
 */

/* Generated sw_3A_types.idt defined type */
#if !defined(_sw_3A_types_idt)
#define _sw_3A_types_idt


#define SW_3A_PELIST_MAX_SIZE 64

#define SW3A_FLASH_MODE_NONE (0x000)

#define SW3A_FLASH_MODE_XENON (0x001)

#define SW3A_FLASH_MODE_XENON_EXTERNAL_STROBE (0x002)

#define SW3A_FLASH_MODE_VIDEO_LED (0x004)

#define SW3A_FLASH_MODE_VIDEO_LED_EXTERNAL_STROBE (0x008)

#define SW3A_FLASH_MODE_STILL_LED (0x010)

#define SW3A_FLASH_MODE_STILL_LED_EXTERNAL_STROBE (0x020)

#define SW3A_FLASH_MODE_AF_ASSISTANT (0x040)

#define SW3A_FLASH_MODE_INDICATOR (0x080)

#define SW3A_FLASH_MODE_STILL_HPLED (0x100)

#define SW3A_FLASH_MODE_STILL_HPLED_EXTERNAL_STROBE (0x200)

typedef enum t_xyuv_t_sw3A_LoopState {
  SW3A_ENQUEUE_LIST,
  SW3A_COMMIT_LIST,
  SW3A_SYNC_LIST,
  SW3A_MODE_SET_NONE,
  SW3A_MODE_SET_STILLVF,
  SW3A_MODE_SET_VIDEOVF,
  SW3A_MODE_SET_STILL,
  SW3A_MODE_SET_VIDEO,
  SW3A_MODE_SET_HDR_CAPTURE,
  SW3A_MODE_SET_TIME_NUDGE,
  SW3A_MODE_SET_STILL_IN_VIDEO} t_sw3A_LoopState;

typedef enum t_xyuv_t_sw3A_StatEvent {
  SW3A_STAT_EVENT_NONE,
  SW3A_STAT_EVENT_AEW,
  SW3A_STAT_EVENT_AF} t_sw3A_StatEvent;

typedef struct t_xyuv_t_sw3A_PageElement {
  t_uint16 pe_addr;
  t_uint32 pe_data;
} t_sw3A_PageElement;

typedef enum t_xyuv_t_sw3A_FlashState {
  SW3A_FLASH_OFF,
  SW3A_FLASH_AFASSIST,
  SW3A_FLASH_PREFLASH,
  SW3A_FLASH_PREFLASH_HPLED,
  SW3A_FLASH_MAINFLASH,
  SW3A_FLASH_MAINFLASH_HPLED,
  SW3A_FLASH_VIDEOLED,
  SW3A_FLASH_INDICATOR} t_sw3A_FlashState;

typedef struct t_xyuv_t_sw3A_FlashDirective {
  t_sw3A_FlashState flashState;
  t_sint16 flashPower;
  t_sint32 flashDurationUs;
  t_sint32 bNeedPreCaptureFlash;
  t_sint32 bNeedPostCaptureFlash;
} t_sw3A_FlashDirective;

typedef struct t_xyuv_t_sw3A_StillSynchro {
  t_sint32 bGrabAllowed;
  t_sint32 bStatsReceived;
} t_sw3A_StillSynchro;

typedef struct t_xyuv_t_sw3A_PageElementList {
  t_uint32 size;
  t_sw3A_PageElement a_list[SW_3A_PELIST_MAX_SIZE];
} t_sw3A_PageElementList;

typedef struct t_xyuv_t_sw3A_Buffer {
  t_uint8* pData;
  t_uint32 size;
} t_sw3A_Buffer;

typedef struct t_xyuv_t_sw3A_ISPSharedBuffer {
  t_uint8* pData;
  t_uint32 ispAddress;
  t_uint32 size;
  t_uint32 hwBufHnd;
} t_sw3A_ISPSharedBuffer;

typedef struct t_xyuv_t_sw3A_EVBracketing {
  t_uint32 evCompensation;
  t_uint8 bBracketing;
} t_sw3A_EVBracketing;

typedef enum t_xyuv_t_sw3A_Msg {
  SW3A_MSG_OK,
  SW3A_MSG_START_COMPLETE,
  SW3A_MSG_STOP_COMPLETE,
  SW3A_MSG_MODE_COMPLETE,
  SW3A_MSG_SETCONFIG_COMPLETE,
  SW3A_MSG_OMXEVENT,
  SW3A_MSG_OPEN_COMPLETE,
  SW3A_MSG_AEC_CONVERGED,
  SW3A_MSG_AEC_TIMEOUT,
  SW3A_MSG_ERROR_RESOURCE=100,
  SW3A_MSG_ERROR_STATE,
  SW3A_MSG_STOP_ACK,
  SW3A_MSG_STAT_STATUS,
  SW3A_MSG_ERROR_BAD_PARAMETER} t_sw3A_Msg;

typedef void* t_sw3A_MsgData;

typedef t_uint32 t_sw3A_FlashModeSet;

typedef struct t_xyuv_t_sw3A_metadata {
  t_sw3A_Buffer captureParametersBuf;
  t_sw3A_Buffer steCaptureParametersBuf;
  t_sw3A_Buffer makernotesBuf;
  t_sw3A_Buffer sw3AextradataBuf;
  t_sw3A_Buffer dampersDataBuf;
  t_sw3A_Buffer sw3AflashstatusBuf;
  t_sw3A_Buffer focusStatusBuf;
  t_sw3A_EVBracketing evBracketing;
  t_uint32 frame_counter;
} t_sw3A_metadata;

typedef struct t_xyuv_t_sw3A_stat_status {
  t_bool bLensMoved;
  t_uint32 afMoveAndExportStatRequested;
  t_uint32 afExportStatRequested;
  t_uint32 glaceStatRequested;
  t_uint32 afStatReceived;
  t_uint32 glaceStatReceived;
} t_sw3A_stat_status;

#endif
