/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef INCLUSION_GUARD_T_MM_PROBE_H
#define INCLUSION_GUARD_T_MM_PROBE_H

#include <stdint.h>

/********************************************/
/* Defines                                  */
/********************************************/
#define MM_PROBE_PROBING_VERSION          "2.0"
#define MM_PROBE_MAX_NUMBER_PROBES        128
#define MM_PROBE_MAX_NUMBER_ADM_PROBES    25
#define MM_PROBE_MAX_NUMBER_SUBSCRIBERS   64
#define MM_PROBE_MAX_DATA_SEND_SIZE       20480
#define MM_PROBE_MAX_DATA_RECEIVE_SIZE    4096
#define MM_PROBE_MAX_PATH_SIZE            150

/********************************************/
/* TypeDefs                                 */
/********************************************/
typedef unsigned char byte;

#define TYPEDEF_ENUM enum
#define STE_ENUM8(t);typedef byte t
#define STE_ENUM32(t);typedef uint32_t t

/********************************************/
/* Enums                                    */
/********************************************/
TYPEDEF_ENUM MM_ProbeMessage_e__
{
  MM_PROBE_REQUEST_ME_VERSION         = 0xA0,
  MM_PROBE_SET_PROBE_STATUS           = 0xA1,
  MM_PROBE_SEND_DATA                  = 0xA2,
  MM_PROBE_SET_LOGGING_LOCATION       = 0xA3,
  MM_PROBE_SUSPEND                    = 0xA4,
  MM_PROBE_SET_PATH                   = 0xA5,
  MM_PROBE_REQUEST_MMPROBE_VERSION    = 0xA6,
  MM_PROBE_SET_PROBE_STATUS_V2        = 0xA7
} STE_ENUM8(MM_ProbeMessage_t);

// This enum is identical to t_sample_freq in U85xx
TYPEDEF_ENUM
{
  MM_PROBE_SAMPLE_RATE_UNKNOWN = 0,
  MM_PROBE_SAMPLE_RATE_192KHZ,
  MM_PROBE_SAMPLE_RATE_176_4KHZ,
  MM_PROBE_SAMPLE_RATE_128KHZ,
  MM_PROBE_SAMPLE_RATE_96KHZ,
  MM_PROBE_SAMPLE_RATE_88_2KHZ,
  MM_PROBE_SAMPLE_RATE_64KHZ,
  MM_PROBE_SAMPLE_RATE_48KHZ,
  MM_PROBE_SAMPLE_RATE_44_1KHZ,
  MM_PROBE_SAMPLE_RATE_32KHZ,
  MM_PROBE_SAMPLE_RATE_24KHZ,
  MM_PROBE_SAMPLE_RATE_22_05KHZ,
  MM_PROBE_SAMPLE_RATE_16KHZ,
  MM_PROBE_SAMPLE_RATE_12KHZ,
  MM_PROBE_SAMPLE_RATE_11_025KHZ,
  MM_PROBE_SAMPLE_RATE_8KHZ,
  MM_PROBE_SAMPLE_RATE_7_2KHZ,
  MM_PROBE_SAMPLE_RATE_LAST_IN_LIST
} STE_ENUM8(MM_ProbeSampleRate_t);


TYPEDEF_ENUM
{
  /** Unknown */
  MM_PROBE_PCM_FORMAT_UNKNOWN = 0,
  /** Signed 16 bit Little Endian */
  MM_PROBE_FORMAT_S16_LE,
  /** Signed 16 bit Big Endian */
  MM_PROBE_FORMAT_S16_BE,
    /** Unsigned 16 bit Little Endian */
  MM_PROBE_FORMAT_U16_LE,
  /** Unsigned 16 bit Big Endian */
  MM_PROBE_FORMAT_U16_BE,
  /** Signed 24 bit Little Endian using low three bytes in 32-bit word */
  MM_PROBE_PCM_FORMAT_S24_LE,
  /** Signed 24 bit Big Endian using low three bytes in 32-bit word */
  MM_PROBE_PCM_FORMAT_S24_BE,
    /** Signed 32 bit Little Endian */
  MM_PROBE_FORMAT_S32_LE,
  /** Signed 32 bit Big Endian */
  MM_PROBE_FORMAT_S32_BE,
  /** Unsigned 32 bit Little Endian */
  MM_PROBE_FORMAT_U32_LE,
  /** Unsigned 32 bit Big Endian */
  MM_PROBE_FORMAT_U32_BE
} STE_ENUM8(MM_ProbeResolution_t);

TYPEDEF_ENUM
{
  MM_PROBE_INTERLEAVED_NONE = 0,
  MM_PROBE_INTERLEAVED      = 1
} STE_ENUM8(MM_ProbeInterleave_t);


TYPEDEF_ENUM MM_ProbeID_e__
{
  MM_PROBE_INVALID    = 0,
  MM_PROBE_PLAIN_TEXT = 1,
  MM_PROBE_RESERVED_1 = 2,
  MM_PROBE_RESERVED_2 = 3,
  MM_PROBE_RESERVED_3 = 4,
  MM_PROBE_RESERVED_4 = 5,
  MM_PROBE_RESERVED_5 = 6,
  MM_PROBE_RESERVED_6 = 7,
  MM_PROBE_RESERVED_7 = 8,
  MM_PROBE_RESERVED_8 = 9,

  /* VCS Probes */
  MM_PROBE_EC_MIC_L_INPUT             = 10,
  MM_PROBE_EC_MIC_R_INPUT             = 11,
  MM_PROBE_EC_REF_L_INPUT             = 12,
  MM_PROBE_EC_REF_R_INPUT             = 13,
  MM_PROBE_NR_TX_REF_INPUT            = 14,
  MM_PROBE_EC_LIN_OUTPUT_L            = 15,
  MM_PROBE_EC_LIN_OUTPUT_R            = 16,
  MM_PROBE_SE_TX_OUTPUT_L             = 17,
  MM_PROBE_SE_TX_OUTPUT_R             = 18,
  MM_PROBE_SE_TX_OUTPUT               = 19,
  MM_PROBE_TX_TREQ_OUTPUT             = 20,
  MM_PROBE_TX_CTM_OUTPUT              = 21,
  MM_PROBE_TX_DRC_OUTPUT              = 22,
  MM_PROBE_TX_SPE_INPUT               = 23,
  MM_PROBE_TX_SPE_OUTPUT              = 24,
  MM_PROBE_RX_SPD_INPUT               = 25,
  MM_PROBE_RX_SPD_OUTPUT              = 26,
  MM_PROBE_RX_SPEECH_PROC_INPUT       = 27,
  MM_PROBE_RX_CTM_OUTPUT              = 28,
  MM_PROBE_SE_RX_ADEQ_REF_INPUT       = 29,
  MM_PROBE_SE_RX_OUTPUT               = 30,
  MM_PROBE_RX_DRC_OUTPUT              = 31,
  MM_PROBE_RX_TREQ_OUTPUT             = 32,
  MM_PROBE_RX_PLIM_OUTPUT             = 33,
  MM_PROBE_VCS_PARAMS                 = 34,
  MM_PROBE_EC_FILTER_L                = 35,
  MM_PROBE_EC_FILTER_R                = 36,
  MM_PROBE_ADEQ_TX_CALIBRATION_DATA   = 37,
  MM_PROBE_ADEQ_RX_CALIBRATION_DATA   = 38,
  MM_PROBE_TX_CS_CALL_INPUT           = 39,
  MM_PROBE_RX_CS_CALL_OUTPUT          = 40,
  MM_PROBE_TX_COMF_NOISE_OUTPUT       = 41,
  MM_PROBE_RX_COMF_NOISE_OUTPUT       = 42,
  MM_PROBE_TX_CUSTOMER_EFFECT_OUTPUT  = 43,
  MM_PROBE_RX_CUSTOMER_EFFECT_OUTPUT  = 44,
  MM_PROBE_NUMBER_OF_SPEECH_PROBES    = 127,

  /* ADM/OMX Probes */
  MM_PROBE_EARPIECE_SNK                     = 200, // Earpiece sink, port 0. Earpiece sink, port 1 = MM_PROBE_EARPIECE_SNK + 1
  MM_PROBE_EARPIECE_COMMON_FIRST_EFFECT     = 210, // First common effect, port 1. Second common effect, port 1 = MM_PROBE_EARPIECE_COMMON_FIRST_EFFECT + 1 etc
  MM_PROBE_EARPIECE_COMMON_MIXER            = 220, // Common mixer, port 0. Common mixer, port 1 = MM_PROBE_EARPIECE_COMMON_MIXER + 1 etc
  MM_PROBE_EARPIECE_VOICE_FIRST_EFFECT      = 230, // First voice effect, port 1. Second voice effect, port 1 = MM_PROBE_EARPIECE_VOICE_FIRST_EFFECT + 1 etc
  MM_PROBE_EARPIECE_APP_FIRST_EFFECT        = 240, // First app effect, port 1. Second app effect, port 1 = MM_PROBE_EARPIECE_APP_FIRST_EFFECT +1 etc
  MM_PROBE_EARPIECE_APP_MIXER               = 250, // App mixer, port 0. App mixer, port 1 = MM_PROBE_EARPIECE_APP_MIXER + 1 etc

  MM_PROBE_SPEAKER_SNK                      = 300, // Speaker sink, port 0. Speaker sink, port 1 = MM_PROBE_SPEAKER_SNK + 1
  MM_PROBE_SPEAKER_COMMON_FIRST_EFFECT      = 310, // First common effect, port 1. Second common effect, port 1 = MM_PROBE_SPEAKER_COMMON_FIRST_EFFECT + 1 etc
  MM_PROBE_SPEAKER_COMMON_MIXER             = 320, // Common mixer, port 0. Common mixer, port 1 = MM_PROBE_SPEAKER_COMMON_MIXER + 1 etc
  MM_PROBE_SPEAKER_VOICE_FIRST_EFFECT       = 330, // First voice effect, port 1. Second voice effect, port 1 = MM_PROBE_SPEAKER_VOICE_FIRST_EFFECT + 1 etc
  MM_PROBE_SPEAKER_APP_FIRST_EFFECT         = 340, // First app effect, port 1. Second app effect, port 1 = MM_PROBE_SPEAKER_APP_FIRST_EFFECT +1 etc
  MM_PROBE_SPEAKER_APP_MIXER                = 350, // App mixer, port 0. App mixer, port 1 = MM_PROBE_SPEAKER_APP_MIXER + 1 etc

  MM_PROBE_MIC_SRC                          = 400, // Mic source, port 0.
  MM_PROBE_MIC_COMMON_FIRST_EFFECT          = 410, // First common effect, port 1. Second common effect, port 1 = MM_PROBE_MIC_COMMON_FIRST_EFFECT + 1 etc
  MM_PROBE_MIC_COMMON_SPLITTER              = 420, // Common splitter, port 0. Common splitter, port 1 = MM_PROBE_MIC_COMMON_SPLITTER + 1 etc
  MM_PROBE_MIC_VOICE_FIRST_EFFECT           = 430, // First voice effect, port 1. Second voice effect, port 1 = MM_PROBE_MIC_VOICE_FIRST_EFFECT + 1 etc
  MM_PROBE_MIC_APP_FIRST_EFFECT             = 440, // First app effect, port 1. Second app effect, port 1 = MM_PROBE_MIC_APP_FIRST_EFFECT + 1 etc
  MM_PROBE_MIC_APP_SPLITTER                 = 450, // App splitter, port 0. App splitter, port 1 = MM_PROBE_MIC_APP_SPLITTER + 1 etc

  MM_PROBE_HSETOUT_SNK                      = 500, // HSetOut sink, port 0. HSetOut sink, port 1 = MM_PROBE_HSETOUT_SNK + 1
  MM_PROBE_HSETOUT_COMMON_FIRST_EFFECT      = 510, // First common effect, port 1. Second common effect, port 1 = MM_PROBE_HSETOUT_COMMON_FIRST_EFFECT + 1 etc
  MM_PROBE_HSETOUT_COMMON_MIXER             = 520, // Common mixer, port 0. Common mixer, port 1 = MM_PROBE_HSETOUT_COMMON_MIXER + 1 etc
  MM_PROBE_HSETOUT_VOICE_FIRST_EFFECT       = 530, // First voice effect, port 1. Second voice effect, port 1 = MM_PROBE_HSETOUT_VOICE_FIRST_EFFECT + 1 etc
  MM_PROBE_HSETOUT_APP_FIRST_EFFECT         = 540, // First app effect, port 1. Second app effect, port 1 = MM_PROBE_HSETOUT_APP_FIRST_EFFECT +1 etc
  MM_PROBE_HSETOUT_APP_MIXER                = 550, // App mixer, port 0. App mixer, port 1 = MM_PROBE_HSETOUT_APP_MIXER + 1 etc

  MM_PROBE_HSETIN_SRC                       = 600, // HSetIn source, port 0.
  MM_PROBE_HSETIN_COMMON_FIRST_EFFECT       = 610, // First common effect, port 1. Second common effect, port 1 = MM_PROBE_HSETIN_COMMON_FIRST_EFFECT + 1 etc
  MM_PROBE_HSETIN_COMMON_SPLITTER           = 620, // Common splitter, port 0. Common splitter, port 1 = MM_PROBE_HSETIN_COMMON_SPLITTER + 1 etc
  MM_PROBE_HSETIN_VOICE_FIRST_EFFECT        = 630, // First voice effect, port 1. Second voice effect, port 1 = MM_PROBE_HSETIN_VOICE_FIRST_EFFECT + 1 etc
  MM_PROBE_HSETIN_APP_FIRST_EFFECT          = 640, // First app effect, port 1. Second app effect, port 1 = MM_PROBE_HSETIN_APP_FIRST_EFFECT + 1 etc
  MM_PROBE_HSETIN_APP_SPLITTER              = 650, // App splitter, port 0. App splitter, port 1 = MM_PROBE_HSETIN_APP_SPLITTER + 1 etc

  MM_PROBE_USBOUT_SNK                       = 700, // USBOut sink, port 0. USBOut sink, port 1 = MM_PROBE_USBOUT_SNK + 1
  MM_PROBE_USBOUT_COMMON_FIRST_EFFECT       = 710, // First common effect, port 1. Second common effect, port 1 = MM_PROBE_USBOUT_COMMON_FIRST_EFFECT + 1 etc
  MM_PROBE_USBOUT_COMMON_MIXER              = 720, // Common mixer, port 0. Common mixer, port 1 = MM_PROBE_USBOUT_COMMON_MIXER + 1 etc
  MM_PROBE_USBOUT_VOICE_FIRST_EFFECT        = 730, // First voice effect, port 1. Second voice effect, port 1 = MM_PROBE_USBOUT_VOICE_FIRST_EFFECT + 1 etc
  MM_PROBE_USBOUT_APP_FIRST_EFFECT          = 740, // First app effect, port 1. Second app effect, port 1 = MM_PROBE_USBOUT_APP_FIRST_EFFECT + 1 etc
  MM_PROBE_USBOUT_APP_MIXER                 = 750, // App mixer, port 0. App mixer, port 1 = MM_PROBE_USBOUT_APP_MIXER + 1 etc

  MM_PROBE_USBIN_SRC                        = 800, // USBIn source, port 0.
  MM_PROBE_USBIN_COMMON_FIRST_EFFECT        = 810, // First common effect, port 1. Second common effect, port 1 = MM_PROBE_USBIN_COMMON_FIRST_EFFECT + 1 etc
  MM_PROBE_USBIN_COMMON_SPLITTER            = 820, // Common splitter, port 0. Common splitter, port 1 = MM_PROBE_USBIN_COMMON_SPLITTER + 1 etc
  MM_PROBE_USBIN_VOICE_FIRST_EFFECT         = 830, // First voice effect, port 1. Second voice effect, port 1 = MM_PROBE_USBIN_VOICE_FIRST_EFFECT + 1 etc
  MM_PROBE_USBIN_APP_FIRST_EFFECT           = 840, // First app effect, port 1. Second app effect, port 1 = MM_PROBE_USBIN_APP_FIRST_EFFECT + 1 etc
  MM_PROBE_USBIN_APP_SPLITTER               = 850, // App splitter, port 0. App splitter, port 1 = MM_PROBE_USBIN_APP_SPLITTER + 1 etc

  MM_PROBE_FMTX_SNK                         = 900, // FMTX sink, port 0.
  MM_PROBE_FMTX_COMMON_FIRST_EFFECT         = 910, // First common effect, port 1. Second common effect, port 1 = MM_PROBE_FMTX_COMMON_FIRST_EFFECT + 1 etc
  MM_PROBE_FMTX_COMMON_MIXER                = 920, // Common mixer, port 0. Common mixer, port 1 = MM_PROBE_FMTX_COMMON_MIXER + 1 etc
  MM_PROBE_FMTX_VOICE_FIRST_EFFECT          = 930, // First voice effect, port 1. Second voice effect, port 1 = MM_PROBE_FMTX_VOICE_FIRST_EFFECT + 1 etc
  MM_PROBE_FMTX_APP_FIRST_EFFECT            = 940, // First app effect, port 1. Second app effect, port 1 = MM_PROBE_FMTX_APP_FIRST_EFFECT + 1 etc
  MM_PROBE_FMTX_APP_MIXER                   = 950, // App mixer, port 0. App mixer, port 1 = MM_PROBE_FMTX_APP_MIXER + 1 etc

  MM_PROBE_FMRX_SRC                         = 1000, // FMRX source, port 0.
  MM_PROBE_FMRX_COMMON_FIRST_EFFECT         = 1010, // First common effect, port 1. Second common effect, port 1 = MM_PROBE_FMRX_COMMON_FIRST_EFFECT + 1 etc
  MM_PROBE_FMRX_COMMON_SPLITTER             = 1020, // Common splitter, port 0. Common splitter, port 1 = MM_PROBE_FMRX_COMMON_SPLITTER + 1 etc
  MM_PROBE_FMRX_VOICE_FIRST_EFFECT          = 1030, // First voice effect, port 1. Second voice effect, port 1 = MM_PROBE_FMRX_VOICE_FIRST_EFFECT + 1 etc
  MM_PROBE_FMRX_APP_FIRST_EFFECT            = 1040, // First app effect, port 1. Second app effect, port 1 = MM_PROBE_FMRX_APP_FIRST_EFFECT + 1 etc
  MM_PROBE_FMRX_APP_SPLITTER                = 1050, // App splitter, port 0. App splitter, port 1 = MM_PROBE_FMRX_APP_SPLITTER + 1 etc

  MM_PROBE_BTOUT_SNK                        = 1100, // BTOut sink, port 0. BTOut sink, port 1 = MM_PROBE_BTOUT_SNK + 1
  MM_PROBE_BTOUT_COMMON_FIRST_EFFECT        = 1100, // First common effect, port 1. Second common effect, port 1 = MM_PROBE_BTOUT_COMMON_FIRST_EFFECT + 1 etc
  MM_PROBE_BTOUT_COMMON_MIXER               = 1120, // Common mixer, port 0. Common mixer, port 1 = MM_PROBE_BTOUT_COMMON_MIXER + 1 etc
  MM_PROBE_BTOUT_VOICE_FIRST_EFFECT         = 1130, // First voice effect, port 1. Second voice effect, port 1 = MM_PROBE_BTOUT_VOICE_FIRST_EFFECT + 1 etc
  MM_PROBE_BTOUT_APP_FIRST_EFFECT           = 1140, // First app effect, port 1. Second app effect, port 1 = MM_PROBE_BTOUT_APP_FIRST_EFFECT + 1 etc
  MM_PROBE_BTOUT_APP_MIXER                  = 1150, // App mixer, port 0. App mixer, port 1 = MM_PROBE_BTOUT_APP_MIXER + 1 etc

  MM_PROBE_BTIN_SRC                         = 1200, // BTIn source, port 0.
  MM_PROBE_BTIN_COMMON_FIRST_EFFECT         = 1210, // First common effect, port 1. Second common effect, port 1 = MM_PROBE_BTIN_COMMON_FIRST_EFFECT + 1 etc
  MM_PROBE_BTIN_COMMON_SPLITTER             = 1220, // Common splitter, port 0. Common splitter, port 1 = MM_PROBE_BTIN_COMMON_SPLITTER + 1 etc
  MM_PROBE_BTIN_VOICE_FIRST_EFFECT          = 1230, // First voice effect, port 1. Second voice effect, port 1 = MM_PROBE_BTIN_VOICE_FIRST_EFFECT + 1 etc
  MM_PROBE_BTIN_APP_FIRST_EFFECT            = 1240, // First app effect, port 1. Second app effect, port 1 = MM_PROBE_BTIN_APP_FIRST_EFFECT + 1 etc
  MM_PROBE_BTIN_APP_SPLITTER                = 1250, // App splitter, port 0. App splitter, port 1 = MM_PROBE_BTIN_APP_SPLITTER + 1 etc

  MM_PROBE_BTWBOUT_SNK                      = 1300, // BTWBOut sink, port 0. BTWBOut sink, port 1 = MM_PROBE_BTWBOUT_SNK + 1
  MM_PROBE_BTWBOUT_COMMON_FIRST_EFFECT      = 1310, // First common effect, port 1. Second common effect, port 1 = MM_PROBE_BTWBOUT_COMMON_FIRST_EFFECT + 1 etc
  MM_PROBE_BTWBOUT_COMMON_MIXER             = 1320, // Common mixer, port 0. Common mixer, port 1 = MM_PROBE_BTWBOUT_COMMON_MIXER + 1 etc
  MM_PROBE_BTWBOUT_VOICE_FIRST_EFFECT       = 1330, // First voice effect, port 1. Second voice effect, port 1 = MM_PROBE_BTWBOUT_VOICE_FIRST_EFFECT + 1 etc
  MM_PROBE_BTWBOUT_APP_FIRST_EFFECT         = 1340, // First app effect, port 1. Second app effect, port 1 = MM_PROBE_BTWBOUT_APP_FIRST_EFFECT + 1 etc
  MM_PROBE_BTWBOUT_APP_MIXER                = 1350, // App mixer, port 0. App mixer, port 1 = MM_PROBE_BTWBOUT_APP_MIXER + 1 etc

  MM_PROBE_BTWBIN_SRC                       = 1400, // BTWBIn source, port 0.
  MM_PROBE_BTWBIN_COMMON_FIRST_EFFECT       = 1410, // First common effect, port 1. Second common effect, port 1 = MM_PROBE_BTWBIN_COMMON_FIRST_EFFECT + 1 etc
  MM_PROBE_BTWBIN_COMMON_SPLITTER           = 1420, // Common splitter, port 0. Common splitter, port 1 = MM_PROBE_BTWBIN_COMMON_SPLITTER + 1 etc
  MM_PROBE_BTWBIN_VOICE_FIRST_EFFECT        = 1430, // First voice effect, port 1. Second voice effect, port 1 = MM_PROBE_BTWBIN_VOICE_FIRST_EFFECT + 1 etc
  MM_PROBE_BTWBIN_APP_FIRST_EFFECT          = 1440, // First app effect, port 1. Second app effect, port 1 = MM_PROBE_BTWBIN_APP_FIRST_EFFECT + 1 etc
  MM_PROBE_BTWBIN_APP_SPLITTER              = 1450, // App splitter, port 0. App splitter, port 1 = MM_PROBE_BTWBIN_APP_SPLITTER + 1 etc

  MM_PROBE_A2DP_SNK                         = 1500, // A2DP sink, port 0.
  MM_PROBE_A2DP_COMMON_FIRST_EFFECT         = 1510, // First common effect, port 1. Second common effect, port 1 = MM_PROBE_A2DP_COMMON_FIRST_EFFECT + 1 etc
  MM_PROBE_A2DP_COMMON_MIXER                = 1520, // Common mixer, port 0. Common mixer, port 1 = MM_PROBE_A2DP_COMMON_MIXER + 1 etc
  MM_PROBE_A2DP_VOICE_FIRST_EFFECT          = 1530, // First voice effect, port 1. Second voice effect, port 1 = MM_PROBE_A2DP_VOICE_FIRST_EFFECT + 1 etc
  MM_PROBE_A2DP_APP_FIRST_EFFECT            = 1540, // First app effect, port 1. Second app effect, port 1 = MM_PROBE_A2DP_APP_FIRST_EFFECT + 1 etc
  MM_PROBE_A2DP_APP_MIXER                   = 1550, // App mixer, port 0. App mixer, port 1 = MM_PROBE_A2DP_APP_MIXER + 1 etc

  MM_PROBE_HDMI_SNK                         = 1600, // HDMI sink, port 0.
  MM_PROBE_HDMI_COMMON_FIRST_EFFECT         = 1610, // First common effect, port 1. Second common effect, port 1 = MM_PROBE_HDMI_COMMON_FIRST_EFFECT + 1 etc
  MM_PROBE_HDMI_COMMON_MIXER                = 1620, // Common mixer, port 0. Common mixer, port 1 = MM_PROBE_HDMI_COMMON_MIXER + 1 etc
  MM_PROBE_HDMI_VOICE_FIRST_EFFECT          = 1630, // First voice effect, port 1. Second voice effect, port 1 = MM_PROBE_HDMI_VOICE_FIRST_EFFECT + 1 etc
  MM_PROBE_HDMI_APP_FIRST_EFFECT            = 1640, // First app effect, port 1. Second app effect, port 1 = MM_PROBE_HDMI_APP_FIRST_EFFECT + 1 etc
  MM_PROBE_HDMI_APP_MIXER                   = 1650, // App mixer, port 0. App mixer, port 1 = MM_PROBE_HDMI_APP_MIXER + 1 etc

} STE_ENUM32(MM_ProbeID_t);

TYPEDEF_ENUM MM_ProbeSubscriberID_e__
{
  MM_PROBE_SUBSCRIBER_INVALID   = 0,
  MM_PROBE_SUBSCRIBER_TEST      = 1,
  MM_PROBE_SUBSCRIBER_ADM       = 2,
  MM_PROBE_NUMBER_OF_SUBSCRIBERS
} STE_ENUM8(MM_ProbeSubscriberID_t);

TYPEDEF_ENUM MM_ProbeStatus_e__
{
  MM_PROBE_STATUS_DISABLED = 0,
  MM_PROBE_STATUS_ENABLED  = 1
} STE_ENUM8(MM_ProbeStatus_t);

TYPEDEF_ENUM MM_AlgorithmStatus_e__
{
  MM_ALGORITHM_STATUS_DISABLED = 0,
  MM_ALGORITHM_STATUS_ENABLED  = 1
} STE_ENUM8(MM_AlgorithmStatus_t);

TYPEDEF_ENUM MM_SetLoggingLocation_e__
{
  MM_LOGGING_LOCATION_PC    = 0,
  MM_LOGGING_LOCATION_PHONE = 1
} STE_ENUM8(MM_SetLoggingLocation_t);

/********************************************/
/* Data  structs                            */
/********************************************/
/* PLEASE NOTE: keep this aligned with 32 */
typedef struct
{
  MM_ProbeSampleRate_t    SampleRate;
  uint8_t                 NoChannels;
  MM_ProbeInterleave_t    Interleave;
  MM_ProbeResolution_t    Resolution;
} DataFormat_t;

/********************************************/
/* Message structs                          */
/********************************************/
typedef struct
{
  MM_ProbeMessage_t MessageId;
  uint8_t           ProbeId;
  MM_ProbeStatus_t  ProbeStatus;
} MM_ProbeSetStatusCmd_t;

typedef struct
{
  MM_ProbeMessage_t MessageId;
  MM_ProbeID_t      ProbeId;
  MM_ProbeStatus_t  ProbeStatus;
} MM_ProbeSetStatusCmd_V2_t;

typedef struct
{
  MM_ProbeMessage_t MessageId;
} MM_ProbeGetME_VersionCmd_t;

typedef struct
{
  MM_ProbeMessage_t MessageId;
} MM_ProbeGetMM_ProbeVersionCmd_t;

typedef struct
{
  MM_ProbeMessage_t MessageId;
} MM_ProbeSuspendCmd_t;

typedef struct
{
  MM_ProbeMessage_t       MessageId;
  MM_ProbeSubscriberID_t  SubscriberId;
  uint16_t                DataSize;
  uint8_t                 Data[MM_PROBE_MAX_DATA_RECEIVE_SIZE];
} MM_ProbeSendDataCmd_t;

typedef struct
{
  MM_ProbeMessage_t         MessageId;
  MM_SetLoggingLocation_t   Location;
} MM_ProbeSetLoggingLocation_t;

/* Path must be null terminated */
typedef struct
{
  MM_ProbeMessage_t   MessageId;
  char                Path[MM_PROBE_MAX_PATH_SIZE];
} MM_ProbeSetPath_t;

/* MM Probe Header definition */
typedef struct
{
  uint32_t ProbeId;
  uint32_t SeqNo;
	uint32_t Size;
  uint32_t TimeStamp;
} MM_ProbeHeader_t;

typedef struct
{
  uint32_t           ProbeId;
  uint32_t           SeqNo;
  uint32_t           Size;
  uint32_t           TimeStamp;
  DataFormat_t       DataFormat;
} MM_ProbeHeader_V2_t;

/* MM Probe Callback struct */
typedef struct
{
  MM_ProbeHeader_t  Header;
  uint8_t           *Data_p;
} MM_ProbeDataCB_t;

/* MM Probe Callback struct, V2 */
typedef struct
{
  MM_ProbeHeader_V2_t  Header;
  uint8_t              *Data_p;
} MM_ProbeDataCB_V2_t;

#endif /* INCLUSION_GUARD_T_MM_PROBE_H */
