/*
NOKIA                                                             CONFIDENTIAL
NOKIA SOUTHWODD R&D
WGMODEM
TIM NORRIS



                  ISIHDR_TITLE
                  --------------------------------
                  SW Include Document - ANSI C/C++



Continuus Ref:
--------------
database:        ou1gif

project path:    global_interface/ISI

name:            modem_audio_isi.h

version:         001.000

type:            incl

instance:        CM_INSTANCE_NAME


ISI header file for Modem Audio Interface

Current   ISI Version : 001.000
Supported ISI Versions: 001.000

Generated with autogen version 08_w25 on 01-Aug-2008 12:22:51
DO NOT EDIT - changes will be lost at next autogeneration

Copyright (c) Nokia Corporation. All rights reserved.


ISI version change history

Version     : 001.000    APPROVED    30-Jul-2008    Tim Norris
Reason      : Initial version
Reference   : MSW-2.5-17, TN08062533989 in ActionDB for Wireless Modem
Description : First version of the interface

------------------------------------------------------------------------------

Description

This interface is used by the modem to setup audio codecs and transfer audio
data.
NOTE: There is no actual ISA server for the audio codecs within the modem,
this interface defines the message requirements from an external audio
server.
Details about the WGmodem audio concept and messaging details can be seen in
refs /6/ and /7/.

*/

#ifndef _MODEM_AUDIO_ISI_H
#define _MODEM_AUDIO_ISI_H

/* #ifndef SIMU_OF_SIMU */
/* #include "isi_conf.h" */
/* #else */
typedef  unsigned char   uint8;
typedef  unsigned short  uint16;
typedef  unsigned short  bits16;
typedef  signed short    int16;
typedef  unsigned long   uint32;
/* #endif */

#ifndef MODEM_AUDIO_ISI_VERSION
#define MODEM_AUDIO_ISI_VERSION
#define MODEM_AUDIO_ISI_VERSION_Z   1
#define MODEM_AUDIO_ISI_VERSION_Y   0
#endif

#define MODEM_AUDIO_ISI_MIN_VERSION(z,y) \
 ((MODEM_AUDIO_ISI_VERSION_Z == (z) && MODEM_AUDIO_ISI_VERSION_Y >= (y)) || \
  (MODEM_AUDIO_ISI_VERSION_Z > (z)))

#if !MODEM_AUDIO_ISI_MIN_VERSION(1,0)
#error The specified interface version is not supported by this header file
#elif MODEM_AUDIO_ISI_MIN_VERSION(1,1)
#error The specified interface version is not supported by this header file
#endif

/* Definition of PhoNet type */
typedef uint8 PN_DSP_AUDIO_TYPE;


/* ----------------------------------------------------------------------- */
/* Constant Table: MODEM_AUDIO_SPC_USE                                     */
/* ----------------------------------------------------------------------- */
typedef uint16 MODEM_AUDIO_SPC_USE_CONST;

/* Speech codec not used. This symbol can be used when disabling previously
   activated codec.
*/
#define SPC_USE_NO                               0x0000
/* Speech codec used in WGModem circuit switched GSM speech call,
   synchronisation time provided in uS
*/
#define SPC_USE_GSM_MODEM                        0x0008
/* Speech codec used in WGModem circuit switched WCDMA speech call,
   synchronisation time provided in uS
*/
#define SPC_USE_WCDMA_MODEM                      0x0009

/* ----------------------------------------------------------------------- */
/* Constant Table: MODEM_AUDIO_SPC                                         */
/* ----------------------------------------------------------------------- */
typedef uint16 MODEM_AUDIO_SPC_CONST;

/* No speech codec in use. */
#define SPC_NO                                   0x0000
/* GSM full rate speech codec (unified format) used. */
#define SPC_UNI_GSM_FR                           0x0012
/* GSM enhanced full rate speech codec (unified format) used. */
#define SPC_UNI_GSM_HR                           0x0013
/* GSM half rate speech codec (unified format) used. */
#define SPC_UNI_GSM_EFR                          0x0014
/* NB AMR adaptive multi rate speech codec (unified format) used. */
#define SPC_UNI_AMR                              0x0015
/* WB AMR adaptive multirate speech codec (unified format) used. */
#define SPC_UNI_WB_AMR                           0x0016

/* ----------------------------------------------------------------------- */
/* Constant Table: MODEM_AUDIO_SPC_RESERVED5                               */
/* ----------------------------------------------------------------------- */
/* Reserved value for backwards compatibility. Not to be used. */
#define MODEM_AUDIO_SPC_RESERVED                 0x00

/* ----------------------------------------------------------------------- */
/* Constant Table: MODEM_AUDIO_SPC_RESERVED4                               */
/* ----------------------------------------------------------------------- */
/* #define MODEM_AUDIO_SPC_RESERVED              0x00 (already defined) */

/* ----------------------------------------------------------------------- */
/* Constant Table: MODEM_AUDIO_SPC_RESERVED1                               */
/* ----------------------------------------------------------------------- */
/* #define MODEM_AUDIO_SPC_RESERVED              0x00 (already defined) */

/* ----------------------------------------------------------------------- */
/* Constant Table: DSP_AUDIO_SPC_AAC_FLAG                                  */
/* ----------------------------------------------------------------------- */
#define DSP_AUDIO_SPC_AAC_OFF                    0x00  /* off */
#define DSP_AUDIO_SPC_AAC_ON                     0x01  /* on */

/* ----------------------------------------------------------------------- */
/* Constant Table: MODEM_AUDIO_SPC_CONF_NSYNC                              */
/* ----------------------------------------------------------------------- */
#define SPC_CONF_NSYNC_OFF                       0x00
#define SPC_CONF_NSYNC_ON                        0x01

/* ----------------------------------------------------------------------- */
/* Constant Table: MODEM_AUDIO_SPC_CONF_DTX                                */
/* ----------------------------------------------------------------------- */
/* DTX not used in encoder. */
#define SPC_CONF_DTX_OFF                         0x00
/* DTX used in encoder. */
#define SPC_CONF_DTX_ON                          0x01

/* ----------------------------------------------------------------------- */
/* Constant Table: MODEM_AUDIO_SPC_CONF_INIT                               */
/* ----------------------------------------------------------------------- */
typedef uint16 MODEM_AUDIO_SPC_CONF_INIT_CONST;

/* No initialization */
#define SPC_CONF_INIT_NO                         0x0000
/* Initialization */
#define SPC_CONF_INIT_YES                        0x0001

/* ----------------------------------------------------------------------- */
/* Constant Table: MODEM_AUDIO_SPC_REASON                                  */
/* ----------------------------------------------------------------------- */
typedef uint16 MODEM_AUDIO_SPC_REASON_CONST;

#define SPC_RESP_OK                              0x0000
#define SPC_RESP_FAIL_NO_RESOURCES               0x0001

/* ----------------------------------------------------------------------- */
/* Constant Table: MODEM_AUDIO_SPC_TIMING_REASON                           */
/* ----------------------------------------------------------------------- */
typedef uint16 MODEM_AUDIO_SPC_TIMING_REASON_CONST;

#define SPC_TIMING_OK                            0x0000
#define SPC_TIMING_FAIL                          0x0001

/* ----------------------------------------------------------------------- */
/* Constant Table: MODEM_AUDIO_SPC_AMR_RX_TYPE                             */
/* ----------------------------------------------------------------------- */
/* GSM AMR RX type is sent downlink together with other frame parameters in
   MODEM_AUDIO_SPC_DECODER_DATA_REQ message.
*/
/* Speech frame with CRC OK, Channel Decoder soft values also OK */
#define AMR_RX_SPEECH_GOOD                       0x00
/* Speech frame with CRC OK, but 1B bits and class2 bits may be corrupted */
#define AMR_RX_SPEECH_PROBABLY_DEGRADED          0x01
#define AMR_RX_SPARE                             0x02  /* Spare */
/* (likely) speech frame, bad CRC (or very bad Channel Decoder measures) */
#define AMR_RX_SPEECH_BAD                        0x03
/* first SID marks the beginning of a comfort noise period */
#define AMR_RX_SID_FIRST                         0x04
/* SID update frame (with correct CRC) */
#define AMR_RX_SID_UPDATE                        0x05
/* Corrupt SID update frame (bad CRC; applicable only for SID_UPDATE frames)
*/
#define AMR_RX_SID_BAD                           0x06
/* Nothing usable was received */
#define AMR_RX_NO_DATA                           0x07

/* ----------------------------------------------------------------------- */
/* Constant Table: MODEM_AUDIO_SPC_AMR_TX_TYPE                             */
/* ----------------------------------------------------------------------- */
/* GSM AMR TX type is sent uplink together with other frame parameters in
   MODEM_AUDIO_SPC_ENCODER_DATA_NTF message.
*/
#define AMR_TX_SPEECH                            0x00  /* Speech frame */
/* First SID marks the beginning of a comfort noise period */
#define AMR_TX_SID_FIRST                         0x01
/* SID update frame */
#define AMR_TX_SID_UPDATE                        0x02
#define AMR_TX_NO_DATA                           0x03  /* No usable data */

/* ----------------------------------------------------------------------- */
/* Constant Table: MODEM_AUDIO_SPC_AMR_FRAME_TYPE                          */
/* ----------------------------------------------------------------------- */
/* AMR frame types. Only one type at a time can be used in the frametype
   field of the speech codec data.
*/
#define MODEM_AUDIO_AMR_475                      0x00  /* AMR 4.75 frame */
#define MODEM_AUDIO_AMR_515                      0x01  /* AMR 5.15 frame */
#define MODEM_AUDIO_AMR_590                      0x02  /* AMR 5.90 frame */
#define MODEM_AUDIO_AMR_670                      0x03  /* AMR 6.70 frame */
#define MODEM_AUDIO_AMR_740                      0x04  /* AMR 7.40 frame */
#define MODEM_AUDIO_AMR_795                      0x05  /* AMR 7.95 frame */
#define MODEM_AUDIO_AMR_102                      0x06  /* AMR 10.2 frame */
#define MODEM_AUDIO_AMR_122                      0x07  /* AMR 12.2 frame */
/* AMR comfort noise frame */
#define MODEM_AUDIO_AMR_CN                       0x08
#define MODEM_AUDIO_NO_RX_TX                     0x0F  /* No data */

/* ----------------------------------------------------------------------- */
/* Constant Table: MODEM_AUDIO_SPC_WB_AMR_FRAME_TYPE                       */
/* ----------------------------------------------------------------------- */
/* AMR-WB frame types. Only one type at a time can be used in the frametype
   field of the speech codec data.
*/
/* AMR-WB 6.60 frame */
#define MODEM_AUDIO_WB_AMR_660                   0x00
/* AMR-WB 8.85 frame */
#define MODEM_AUDIO_WB_AMR_885                   0x01
/* AMR-WB 12.65 frame */
#define MODEM_AUDIO_WB_AMR_1265                  0x02
/* AMR-WB 14.25 frame */
#define MODEM_AUDIO_WB_AMR_1425                  0x03
/* AMR-WB 15.85 frame */
#define MODEM_AUDIO_WB_AMR_1585                  0x04
/* AMR-WB 18.25 frame */
#define MODEM_AUDIO_WB_AMR_1825                  0x05
/* AMR-WB 19.85 frame */
#define MODEM_AUDIO_WB_AMR_1985                  0x06
/* AMR-WB 23.05 frame */
#define MODEM_AUDIO_WB_AMR_2305                  0x07
/* AMR-WB 23.85 frame */
#define MODEM_AUDIO_WB_AMR_2385                  0x08
/* AMR-WB SID (comfort noise frame) */
#define MODEM_AUDIO_WB_AMR_SID                   0x09
/* AMR-WB speech lost */
#define MODEM_AUDIO_WB_AMR_SPEECH_LOST           0x0E
/* No data (No Rx/Tx) */
#define MODEM_AUDIO_WB_AMR_NO_RX_TX              0x0F

/* ----------------------------------------------------------------------- */
/* Constant Table: MODEM_AUDIO_SPC_AMR_QUALITY_TYPE                        */
/* ----------------------------------------------------------------------- */
/* Quality of class A bits in AMR data messages. */
/* Errors in class A bits */
#define MODEM_AUDIO_SPC_AMR_QUALITY_NOT_OK       0x00
/* No errors in class A bits */
#define MODEM_AUDIO_SPC_AMR_QUALITY_OK           0x01

/* ----------------------------------------------------------------------- */
/* Constant Table: MODEM_AUDIO_SPC_TAF_TYPE                                */
/* ----------------------------------------------------------------------- */
/* Timing Alignment Flag */
/* Timing not aligned */
#define MODEM_AUDIO_SPC_TAF_NOT_OK               0x00
#define MODEM_AUDIO_SPC_TAF_OK                   0x01  /* Timing aligned */

/* ----------------------------------------------------------------------- */
/* Constant Table: MODEM_AUDIO_SPC_BFI_TYPE                                */
/* ----------------------------------------------------------------------- */
/* Bad Frame Indicator */
/* Bad Frame indicator off */
#define MODEM_AUDIO_SPC_BFI_OFF                  0x00
#define MODEM_AUDIO_SPC_BFI_ON                   0x01  /* Bad Frame */

/* ----------------------------------------------------------------------- */
/* Constant Table: MODEM_AUDIO_SPC_UFI_TYPE                                */
/* ----------------------------------------------------------------------- */
/* Unreliable Frame Indicator */
/* Unreliable Frame Indicator off */
#define MODEM_AUDIO_SPC_UFI_OFF                  0x00
/* Unreliable Frame */
#define MODEM_AUDIO_SPC_UFI_ON                   0x01

/* ----------------------------------------------------------------------- */
/* Constant Table: MODEM_AUDIO_SPC_SP_FLAG_TYPE                            */
/* ----------------------------------------------------------------------- */
/* SP Flag */
/* SID Frame present */
#define MODEM_AUDIO_SPC_SP_OFF                   0x00
/* Speech Frame present */
#define MODEM_AUDIO_SPC_SP_ON                    0x01

/* ----------------------------------------------------------------------- */
/* Constant Table: MODEM_AUDIO_SPC_SID_TYPE                                */
/* ----------------------------------------------------------------------- */
/* SID Descriptor field */
#define MODEM_AUDIO_SPC_SID_0                    0x00  /* SID = 0, n < 2 */
/* SID = 1, 2 <= n < 16 */
#define MODEM_AUDIO_SPC_SID_1                    0x01
/* SID = 2, n >= 16 */
#define MODEM_AUDIO_SPC_SID_2                    0x02

/* ----------------------------------------------------------------------- */
/* Message ID's                                                            */
/* ----------------------------------------------------------------------- */

#define MODEM_AUDIO_SPEECH_CODEC_REQ             0x03
#define MODEM_AUDIO_SPEECH_CODEC_RESP            0x04
#define MODEM_AUDIO_SPC_TIMING_REQ               0x0B
#define MODEM_AUDIO_SPC_TIMING_RESP              0x0C
#define MODEM_AUDIO_SPC_TIMING_NEEDED_NTF        0x2F
#define MODEM_AUDIO_SPC_DECODER_DATA_REQ         0xB0
#define MODEM_AUDIO_SPC_ENCODER_DATA_NTF         0xB2

/* ----------------------------------------------------------------------- */
/* Subblock ID's                                                           */
/* ----------------------------------------------------------------------- */

#define MODEM_AUDIO_SB_SPC_CODEC_FRAME           0x007B

/* ----------------------------------------------------------------------- */
/* Sequence: MODEM_AUDIO_SPC_DATA_UNI_GSM_FR                               */
/* ----------------------------------------------------------------------- */

typedef struct
    {
    /* reserved for future use. Field should be set to zero in this version
       of the specification.
    */
    uint16  reserved;
    /* xxxxxxxxxx------  Filler
       ----------x-----  Reserved1
       -----------x----  SP
       ------------xx--  SID
       --------------x-  TAF
       ---------------x  BFI
    */
    uint16  header;
    /* Start of audio data, length of data is variable */
    } MODEM_AUDIO_SPC_DATA_UNI_GSM_FR_STR;

#define SIZE_MODEM_AUDIO_SPC_DATA_UNI_GSM_FR_STR   (2*sizeof(uint16))

/* Merge mask constants */
#define MODEM_AUDIO_SPC_DATA_UNI_GSM_FR_reserved1_OFFSET             0x5u  /*        5 */
#define MODEM_AUDIO_SPC_DATA_UNI_GSM_FR_reserved1_WIDTH              0x1u  /*        1 */
#define MODEM_AUDIO_SPC_DATA_UNI_GSM_FR_reserved1_MASK              0x20u  /*       32 */
#define MODEM_AUDIO_SPC_DATA_UNI_GSM_FR_sp_OFFSET                    0x4u  /*        4 */
#define MODEM_AUDIO_SPC_DATA_UNI_GSM_FR_sp_WIDTH                     0x1u  /*        1 */
#define MODEM_AUDIO_SPC_DATA_UNI_GSM_FR_sp_MASK                     0x10u  /*       16 */
#define MODEM_AUDIO_SPC_DATA_UNI_GSM_FR_sid_OFFSET                   0x2u  /*        2 */
#define MODEM_AUDIO_SPC_DATA_UNI_GSM_FR_sid_WIDTH                    0x2u  /*        2 */
#define MODEM_AUDIO_SPC_DATA_UNI_GSM_FR_sid_MASK                     0xCu  /*       12 */
#define MODEM_AUDIO_SPC_DATA_UNI_GSM_FR_taf_OFFSET                   0x1u  /*        1 */
#define MODEM_AUDIO_SPC_DATA_UNI_GSM_FR_taf_WIDTH                    0x1u  /*        1 */
#define MODEM_AUDIO_SPC_DATA_UNI_GSM_FR_taf_MASK                     0x2u  /*        2 */
#define MODEM_AUDIO_SPC_DATA_UNI_GSM_FR_bfi_OFFSET                   0x0u  /*        0 */
#define MODEM_AUDIO_SPC_DATA_UNI_GSM_FR_bfi_WIDTH                    0x1u  /*        1 */
#define MODEM_AUDIO_SPC_DATA_UNI_GSM_FR_bfi_MASK                     0x1u  /*        1 */


/* ----------------------------------------------------------------------- */
/* Sequence: MODEM_AUDIO_SPC_DATA_UNI_GSM_HR                               */
/* ----------------------------------------------------------------------- */

typedef struct
    {
    /* reserved for future use. Field should be set to zero in this version
       of the specification.
    */
    uint16  reserved;
    /* xxxxxxxxx-------  Filler
       ---------x------  Reserved1
       ----------x-----  SP
       -----------xx---  SID
       -------------x--  TAF
       --------------x-  UFI
       ---------------x  BFI
    */
    uint16  header;
    /* Start of audio data, length of data is variable */
    } MODEM_AUDIO_SPC_DATA_UNI_GSM_HR_STR;

#define SIZE_MODEM_AUDIO_SPC_DATA_UNI_GSM_HR_STR   (2*sizeof(uint16))

/* Merge mask constants */
#define MODEM_AUDIO_SPC_DATA_UNI_GSM_HR_reserved1_OFFSET             0x6u  /*        6 */
#define MODEM_AUDIO_SPC_DATA_UNI_GSM_HR_reserved1_WIDTH              0x1u  /*        1 */
#define MODEM_AUDIO_SPC_DATA_UNI_GSM_HR_reserved1_MASK              0x40u  /*       64 */
#define MODEM_AUDIO_SPC_DATA_UNI_GSM_HR_sp_OFFSET                    0x5u  /*        5 */
#define MODEM_AUDIO_SPC_DATA_UNI_GSM_HR_sp_WIDTH                     0x1u  /*        1 */
#define MODEM_AUDIO_SPC_DATA_UNI_GSM_HR_sp_MASK                     0x20u  /*       32 */
#define MODEM_AUDIO_SPC_DATA_UNI_GSM_HR_sid_OFFSET                   0x3u  /*        3 */
#define MODEM_AUDIO_SPC_DATA_UNI_GSM_HR_sid_WIDTH                    0x2u  /*        2 */
#define MODEM_AUDIO_SPC_DATA_UNI_GSM_HR_sid_MASK                    0x18u  /*       24 */
#define MODEM_AUDIO_SPC_DATA_UNI_GSM_HR_taf_OFFSET                   0x2u  /*        2 */
#define MODEM_AUDIO_SPC_DATA_UNI_GSM_HR_taf_WIDTH                    0x1u  /*        1 */
#define MODEM_AUDIO_SPC_DATA_UNI_GSM_HR_taf_MASK                     0x4u  /*        4 */
#define MODEM_AUDIO_SPC_DATA_UNI_GSM_HR_ufi_OFFSET                   0x1u  /*        1 */
#define MODEM_AUDIO_SPC_DATA_UNI_GSM_HR_ufi_WIDTH                    0x1u  /*        1 */
#define MODEM_AUDIO_SPC_DATA_UNI_GSM_HR_ufi_MASK                     0x2u  /*        2 */
#define MODEM_AUDIO_SPC_DATA_UNI_GSM_HR_bfi_OFFSET                   0x0u  /*        0 */
#define MODEM_AUDIO_SPC_DATA_UNI_GSM_HR_bfi_WIDTH                    0x1u  /*        1 */
#define MODEM_AUDIO_SPC_DATA_UNI_GSM_HR_bfi_MASK                     0x1u  /*        1 */


/* ----------------------------------------------------------------------- */
/* Sequence: MODEM_AUDIO_SPC_DATA_UNI_GSM_EFR                              */
/* ----------------------------------------------------------------------- */

typedef struct
    {
    /* reserved for future use. Field should be set to zero in this version
       of the specification.
    */
    uint16  reserved;
    /* xxxxxxxxxxx-----  Filler
       -----------x----  SP
       ------------xx--  SID
       --------------x-  TAF
       ---------------x  BFI
    */
    uint16  header;
    /* Start of audio data, length of data is variable */
    } MODEM_AUDIO_SPC_DATA_UNI_GSM_EFR_STR;

#define SIZE_MODEM_AUDIO_SPC_DATA_UNI_GSM_EFR_STR   (2*sizeof(uint16))

/* Merge mask constants */
#define MODEM_AUDIO_SPC_DATA_UNI_GSM_EFR_sp_OFFSET                   0x4u  /*        4 */
#define MODEM_AUDIO_SPC_DATA_UNI_GSM_EFR_sp_WIDTH                    0x1u  /*        1 */
#define MODEM_AUDIO_SPC_DATA_UNI_GSM_EFR_sp_MASK                    0x10u  /*       16 */
#define MODEM_AUDIO_SPC_DATA_UNI_GSM_EFR_sid_OFFSET                  0x2u  /*        2 */
#define MODEM_AUDIO_SPC_DATA_UNI_GSM_EFR_sid_WIDTH                   0x2u  /*        2 */
#define MODEM_AUDIO_SPC_DATA_UNI_GSM_EFR_sid_MASK                    0xCu  /*       12 */
#define MODEM_AUDIO_SPC_DATA_UNI_GSM_EFR_taf_OFFSET                  0x1u  /*        1 */
#define MODEM_AUDIO_SPC_DATA_UNI_GSM_EFR_taf_WIDTH                   0x1u  /*        1 */
#define MODEM_AUDIO_SPC_DATA_UNI_GSM_EFR_taf_MASK                    0x2u  /*        2 */
#define MODEM_AUDIO_SPC_DATA_UNI_GSM_EFR_bfi_OFFSET                  0x0u  /*        0 */
#define MODEM_AUDIO_SPC_DATA_UNI_GSM_EFR_bfi_WIDTH                   0x1u  /*        1 */
#define MODEM_AUDIO_SPC_DATA_UNI_GSM_EFR_bfi_MASK                    0x1u  /*        1 */


/* ----------------------------------------------------------------------- */
/* Sequence: MODEM_AUDIO_SPC_DATA_UNI_AMR                                  */
/* ----------------------------------------------------------------------- */

typedef struct
    {
    /* reserved for future use. Field should be set to zero in this version
       of the specification.
    */
    uint16  reserved;
    /* xxx-------------  RX Type
       ---xxx----------  SFN
       ------xx--------  TX Type
       --------x-------  Padding1
       ---------xxxx---  Frame Type
       -------------x--  Quality
       --------------x-  Padding2
       ---------------x  Padding3
    */
    uint16  header;
    /* Start of audio data, length of data is variable */
    } MODEM_AUDIO_SPC_DATA_UNI_AMR_STR;

#define SIZE_MODEM_AUDIO_SPC_DATA_UNI_AMR_STR   (2*sizeof(uint16))

/* Merge mask constants */
#define MODEM_AUDIO_SPC_DATA_UNI_AMR_rx_type_OFFSET                  0xDu  /*       13 */
#define MODEM_AUDIO_SPC_DATA_UNI_AMR_rx_type_WIDTH                   0x3u  /*        3 */
#define MODEM_AUDIO_SPC_DATA_UNI_AMR_rx_type_MASK                 0xE000u  /*    57344 */
#define MODEM_AUDIO_SPC_DATA_UNI_AMR_sfn_OFFSET                      0xAu  /*       10 */
#define MODEM_AUDIO_SPC_DATA_UNI_AMR_sfn_WIDTH                       0x3u  /*        3 */
#define MODEM_AUDIO_SPC_DATA_UNI_AMR_sfn_MASK                     0x1C00u  /*     7168 */
#define MODEM_AUDIO_SPC_DATA_UNI_AMR_tx_type_OFFSET                  0x8u  /*        8 */
#define MODEM_AUDIO_SPC_DATA_UNI_AMR_tx_type_WIDTH                   0x2u  /*        2 */
#define MODEM_AUDIO_SPC_DATA_UNI_AMR_tx_type_MASK                  0x300u  /*      768 */
#define MODEM_AUDIO_SPC_DATA_UNI_AMR_padding1_OFFSET                 0x7u  /*        7 */
#define MODEM_AUDIO_SPC_DATA_UNI_AMR_padding1_WIDTH                  0x1u  /*        1 */
#define MODEM_AUDIO_SPC_DATA_UNI_AMR_padding1_MASK                  0x80u  /*      128 */
#define MODEM_AUDIO_SPC_DATA_UNI_AMR_frame_type_OFFSET               0x3u  /*        3 */
#define MODEM_AUDIO_SPC_DATA_UNI_AMR_frame_type_WIDTH                0x4u  /*        4 */
#define MODEM_AUDIO_SPC_DATA_UNI_AMR_frame_type_MASK                0x78u  /*      120 */
#define MODEM_AUDIO_SPC_DATA_UNI_AMR_quality_OFFSET                  0x2u  /*        2 */
#define MODEM_AUDIO_SPC_DATA_UNI_AMR_quality_WIDTH                   0x1u  /*        1 */
#define MODEM_AUDIO_SPC_DATA_UNI_AMR_quality_MASK                    0x4u  /*        4 */
#define MODEM_AUDIO_SPC_DATA_UNI_AMR_padding2_OFFSET                 0x1u  /*        1 */
#define MODEM_AUDIO_SPC_DATA_UNI_AMR_padding2_WIDTH                  0x1u  /*        1 */
#define MODEM_AUDIO_SPC_DATA_UNI_AMR_padding2_MASK                   0x2u  /*        2 */
#define MODEM_AUDIO_SPC_DATA_UNI_AMR_padding3_OFFSET                 0x0u  /*        0 */
#define MODEM_AUDIO_SPC_DATA_UNI_AMR_padding3_WIDTH                  0x1u  /*        1 */
#define MODEM_AUDIO_SPC_DATA_UNI_AMR_padding3_MASK                   0x1u  /*        1 */

/* Value constants */
#define MODEM_AUDIO_SPC_DATA_UNI_AMR_sfn_MIN                         0x0u  /*        0 */
#define MODEM_AUDIO_SPC_DATA_UNI_AMR_sfn_MAX                         0x7u  /*        7 */


/* ----------------------------------------------------------------------- */
/* Sequence: MODEM_AUDIO_SPC_DATA_UNI_WB_AMR                               */
/* ----------------------------------------------------------------------- */

typedef struct
    {
    /* reserved for future use. Field should be set to zero in this version
       of the specification.
    */
    uint16  reserved;
    /* xxx-------------  RX Type
       ---xxx----------  SFN
       ------xx--------  TX Type
       --------x-------  Padding1
       ---------xxxx---  Frame Type
       -------------x--  Quality
       --------------x-  Padding2
       ---------------x  Padding3
    */
    uint16  header;
    /* Start of audio data, length of data is variable */
    } MODEM_AUDIO_SPC_DATA_UNI_WB_AMR_STR;

#define SIZE_MODEM_AUDIO_SPC_DATA_UNI_WB_AMR_STR   (2*sizeof(uint16))

/* Merge mask constants */
#define MODEM_AUDIO_SPC_DATA_UNI_WB_AMR_rx_type_OFFSET               0xDu  /*       13 */
#define MODEM_AUDIO_SPC_DATA_UNI_WB_AMR_rx_type_WIDTH                0x3u  /*        3 */
#define MODEM_AUDIO_SPC_DATA_UNI_WB_AMR_rx_type_MASK              0xE000u  /*    57344 */
#define MODEM_AUDIO_SPC_DATA_UNI_WB_AMR_sfn_OFFSET                   0xAu  /*       10 */
#define MODEM_AUDIO_SPC_DATA_UNI_WB_AMR_sfn_WIDTH                    0x3u  /*        3 */
#define MODEM_AUDIO_SPC_DATA_UNI_WB_AMR_sfn_MASK                  0x1C00u  /*     7168 */
#define MODEM_AUDIO_SPC_DATA_UNI_WB_AMR_tx_type_OFFSET               0x8u  /*        8 */
#define MODEM_AUDIO_SPC_DATA_UNI_WB_AMR_tx_type_WIDTH                0x2u  /*        2 */
#define MODEM_AUDIO_SPC_DATA_UNI_WB_AMR_tx_type_MASK               0x300u  /*      768 */
#define MODEM_AUDIO_SPC_DATA_UNI_WB_AMR_padding1_OFFSET              0x7u  /*        7 */
#define MODEM_AUDIO_SPC_DATA_UNI_WB_AMR_padding1_WIDTH               0x1u  /*        1 */
#define MODEM_AUDIO_SPC_DATA_UNI_WB_AMR_padding1_MASK               0x80u  /*      128 */
#define MODEM_AUDIO_SPC_DATA_UNI_WB_AMR_frame_type_OFFSET            0x3u  /*        3 */
#define MODEM_AUDIO_SPC_DATA_UNI_WB_AMR_frame_type_WIDTH             0x4u  /*        4 */
#define MODEM_AUDIO_SPC_DATA_UNI_WB_AMR_frame_type_MASK             0x78u  /*      120 */
#define MODEM_AUDIO_SPC_DATA_UNI_WB_AMR_quality_OFFSET               0x2u  /*        2 */
#define MODEM_AUDIO_SPC_DATA_UNI_WB_AMR_quality_WIDTH                0x1u  /*        1 */
#define MODEM_AUDIO_SPC_DATA_UNI_WB_AMR_quality_MASK                 0x4u  /*        4 */
#define MODEM_AUDIO_SPC_DATA_UNI_WB_AMR_padding2_OFFSET              0x1u  /*        1 */
#define MODEM_AUDIO_SPC_DATA_UNI_WB_AMR_padding2_WIDTH               0x1u  /*        1 */
#define MODEM_AUDIO_SPC_DATA_UNI_WB_AMR_padding2_MASK                0x2u  /*        2 */
#define MODEM_AUDIO_SPC_DATA_UNI_WB_AMR_padding3_OFFSET              0x0u  /*        0 */
#define MODEM_AUDIO_SPC_DATA_UNI_WB_AMR_padding3_WIDTH               0x1u  /*        1 */
#define MODEM_AUDIO_SPC_DATA_UNI_WB_AMR_padding3_MASK                0x1u  /*        1 */

/* Value constants */
#define MODEM_AUDIO_SPC_DATA_UNI_WB_AMR_sfn_MIN                      0x0u  /*        0 */
#define MODEM_AUDIO_SPC_DATA_UNI_WB_AMR_sfn_MAX                      0x7u  /*        7 */


/* ----------------------------------------------------------------------- */
/* Subblock: MODEM_AUDIO_SB_SPC_CODEC_FRAME                                */
/* ----------------------------------------------------------------------- */
/* This sub block includes speech codec frame data. The audio frame starts
   after the subblock header. Its length depends on the used codec. 
*/

typedef struct
    {
    uint16  sb_id;
    uint16  sb_len;
    } MODEM_AUDIO_SB_SPC_CODEC_FRAME_STR;


/* ----------------------------------------------------------------------- */
/* Message: MODEM_AUDIO_SPEECH_CODEC_REQ                                   */
/* ----------------------------------------------------------------------- */
/* This message is used by the modem to request  the setup, modification or
   release of the audio codecs. The message specifies the codec to be used,
   and related parameters.
*/

typedef struct
    {
/* PhoNet header */
    uint8   media;
    uint8   receiver_dev;
    uint8   sender_dev;
    uint8   res;
    uint16  length;
    uint8   receiver_obj;
    uint8   sender_obj;
/* Message data */
    uint8   TransactionId;
    uint8   MessageId;
    int16   spare;
    /* Values from the constant table MODEM_AUDIO_SPC_USE */
    uint16  SpeechCodecUse;
    uint16  SpeechCodec; /* Values from the constant table MODEM_AUDIO_SPC */
    /* xxxxx-----------  Reserved1
       -----x----------  Audio Activity Control Flag
       ------x---------  NSYNC control
       -------xxx------  Speech frame number
       ----------xxxx--  AMR encoder
       --------------x-  Reserved2
       ---------------x  DTX use
    */
    bits16  SpeechEncoderConfiguration;
    /* xxxxxx----------  Filler
       ------xxxx------  Reserved1
       ----------xxxx--  AMR decoder
       --------------x-  Reserved2
       ---------------x  Filler
    */
    bits16  SpeechDecoderConfiguration;
    /* Values from the constant table MODEM_AUDIO_SPC_CONF_INIT */
    bits16  SpeechCodecInit;
    } ModemAudioSpeechCodecReq;

typedef ModemAudioSpeechCodecReq tISI_ModemAudioSpeechCodecReq;

/* Merge mask constants */
#define MODEM_AUDIO_SPEECH_CODEC_REQ_reserved1_OFFSET                0xBu  /*       11 */
#define MODEM_AUDIO_SPEECH_CODEC_REQ_reserved1_WIDTH                 0x5u  /*        5 */
#define MODEM_AUDIO_SPEECH_CODEC_REQ_reserved1_MASK               0xF800u  /*    63488 */
#define MODEM_AUDIO_SPEECH_CODEC_REQ_audio_activity_control_OFFSET          0xAu  /*       10 */
#define MODEM_AUDIO_SPEECH_CODEC_REQ_audio_activity_control_WIDTH          0x1u  /*        1 */
#define MODEM_AUDIO_SPEECH_CODEC_REQ_audio_activity_control_MASK        0x400u  /*     1024 */
#define MODEM_AUDIO_SPEECH_CODEC_REQ_nsync_OFFSET                    0x9u  /*        9 */
#define MODEM_AUDIO_SPEECH_CODEC_REQ_nsync_WIDTH                     0x1u  /*        1 */
#define MODEM_AUDIO_SPEECH_CODEC_REQ_nsync_MASK                    0x200u  /*      512 */
#define MODEM_AUDIO_SPEECH_CODEC_REQ_SpeechFrameNumber_OFFSET          0x6u  /*        6 */
#define MODEM_AUDIO_SPEECH_CODEC_REQ_SpeechFrameNumber_WIDTH          0x3u  /*        3 */
#define MODEM_AUDIO_SPEECH_CODEC_REQ_SpeechFrameNumber_MASK        0x1C0u  /*      448 */
#define MODEM_AUDIO_SPEECH_CODEC_REQ_AMR_encoder_OFFSET              0x2u  /*        2 */
#define MODEM_AUDIO_SPEECH_CODEC_REQ_AMR_encoder_WIDTH               0x4u  /*        4 */
#define MODEM_AUDIO_SPEECH_CODEC_REQ_AMR_encoder_MASK               0x3Cu  /*       60 */
#define MODEM_AUDIO_SPEECH_CODEC_REQ_reserved2_OFFSET                0x1u  /*        1 */
#define MODEM_AUDIO_SPEECH_CODEC_REQ_reserved2_WIDTH                 0x1u  /*        1 */
#define MODEM_AUDIO_SPEECH_CODEC_REQ_reserved2_MASK                  0x2u  /*        2 */
#define MODEM_AUDIO_SPEECH_CODEC_REQ_DTX_use_OFFSET                  0x0u  /*        0 */
#define MODEM_AUDIO_SPEECH_CODEC_REQ_DTX_use_WIDTH                   0x1u  /*        1 */
#define MODEM_AUDIO_SPEECH_CODEC_REQ_DTX_use_MASK                    0x1u  /*        1 */
//#define MODEM_AUDIO_SPEECH_CODEC_REQ_reserved1_OFFSET                0x6u  /*        6 */
//#define MODEM_AUDIO_SPEECH_CODEC_REQ_reserved1_WIDTH                 0x4u  /*        4 */
//#define MODEM_AUDIO_SPEECH_CODEC_REQ_reserved1_MASK                0x3C0u  /*      960 */
#define MODEM_AUDIO_SPEECH_CODEC_REQ_AMR_decoder_OFFSET              0x2u  /*        2 */
#define MODEM_AUDIO_SPEECH_CODEC_REQ_AMR_decoder_WIDTH               0x4u  /*        4 */
#define MODEM_AUDIO_SPEECH_CODEC_REQ_AMR_decoder_MASK               0x3Cu  /*       60 */
#define MODEM_AUDIO_SPEECH_CODEC_REQ_reserved2_OFFSET                0x1u  /*        1 */
#define MODEM_AUDIO_SPEECH_CODEC_REQ_reserved2_WIDTH                 0x1u  /*        1 */
#define MODEM_AUDIO_SPEECH_CODEC_REQ_reserved2_MASK                  0x2u  /*        2 */

/* Value constants */
#define MODEM_AUDIO_SPEECH_CODEC_REQ_SpeechFrameNumber_MIN           0x0u  /*        0 */
#define MODEM_AUDIO_SPEECH_CODEC_REQ_SpeechFrameNumber_MAX           0x7u  /*        7 */


/* ----------------------------------------------------------------------- */
/* Message: MODEM_AUDIO_SPEECH_CODEC_RESP                                  */
/* ----------------------------------------------------------------------- */
/* This message is used for informing the modem of whether the speech call
   can be started successfully or not.
*/

typedef struct
    {
/* PhoNet header */
    uint8   media;
    uint8   receiver_dev;
    uint8   sender_dev;
    uint8   res;
    uint16  length;
    uint8   receiver_obj;
    uint8   sender_obj;
/* Message data */
    uint8   TransactionId;
    uint8   MessageId;
    int16   spare;
    /* Values from the constant table MODEM_AUDIO_SPC_REASON */
    uint16  Reason;
    } ModemAudioSpeechCodecResp;

typedef ModemAudioSpeechCodecResp tISI_ModemAudioSpeechCodecResp;


/* ----------------------------------------------------------------------- */
/* Message: MODEM_AUDIO_SPC_TIMING_REQ                                     */
/* ----------------------------------------------------------------------- */
/* 3GPP TS 43.050 specifies the maximum speech delay in the uplink direction.
   Audio subsystem needs to know the latest timing  when the encoded speech
   frame must be sent to the cellular subsystem and also the processing time
   reserved for transmission by the cellular subsystem.
   This message is used for controlling the timing of encoded uplink speech
   data transmitted from the audio sub system to external interface of the
   modem. Timing field contains information about when the data has to, at
   the latest, be on the upper interface of the modem.The timing request will
   only be sent when the speech encoding is started or when the timing needs
   to be corrected. The timing change request need not be synchronized with
   the natural timing of audio sub system, and may be sent at any time. After
   starting, the data is sent at the natural rate of system ie. every 20 ms
   in GSM and WCDMA.
*/

typedef struct
    {
/* PhoNet header */
    uint8   media;
    uint8   receiver_dev;
    uint8   sender_dev;
    uint8   res;
    uint16  length;
    uint8   receiver_obj;
    uint8   sender_obj;
/* Message data */
    uint8   TransactionId;
    uint8   MessageId;
    /* Unsigned value which tells the number of microsends from when the
       timing message was sent until the time the data has to be on the upper
       interface of the modem 
    */
    uint16  timing_high;
    uint16  timing_low;
    /* Maximum time in microseconds needed for processing by GSM cellular
       subsystem, defined as Tencode + Trftx where
       - Tencode: Time required for channel encoder to perform channel
         encoding
       - Trftx: Time required for transmission of a TCH radio interface frame
         over the air due to interleaving and de-interleaving
    */
    uint16  processing_time;
    /* This parameter may be used for audio delay calculations in audio
       component if cellular subsystems have different audio delay
       requirements.
       A subset of values from the constant table MODEM_AUDIO_SPC_USE
    */
    uint16  subsystem;
    uint8   fill1[2];
    } ModemAudioSpcTimingReq;

typedef ModemAudioSpcTimingReq tISI_ModemAudioSpcTimingReq;

/* Value constants */
#define MODEM_AUDIO_SPC_TIMING_REQ_timing_high_MIN                   0x0u  /*        0 */
#define MODEM_AUDIO_SPC_TIMING_REQ_timing_high_MAX                0xFFFFu  /*    65535 */
#define MODEM_AUDIO_SPC_TIMING_REQ_timing_low_MIN                    0x0u  /*        0 */
#define MODEM_AUDIO_SPC_TIMING_REQ_timing_low_MAX                 0xFFFFu  /*    65535 */


/* ----------------------------------------------------------------------- */
/* Message: MODEM_AUDIO_SPC_TIMING_RESP                                    */
/* ----------------------------------------------------------------------- */
/* This message is sent as a response to a request for setting the timing of
   uplink speech data.
*/

typedef struct
    {
/* PhoNet header */
    uint8   media;
    uint8   receiver_dev;
    uint8   sender_dev;
    uint8   res;
    uint16  length;
    uint8   receiver_obj;
    uint8   sender_obj;
/* Message data */
    uint8   TransactionId;
    uint8   MessageId;
    /* Values from the constant table MODEM_AUDIO_SPC_TIMING_REASON */
    bits16  status;
    } ModemAudioSpcTimingResp;

typedef ModemAudioSpcTimingResp tISI_ModemAudioSpcTimingResp;


/* ----------------------------------------------------------------------- */
/* Message: MODEM_AUDIO_SPC_TIMING_NEEDED_NTF                              */
/* ----------------------------------------------------------------------- */
/* This message is sent to cellular subsystem when new uplink timing is
   needed. Cellular subsystem will send a new MODEM_AUDIO_SPC_TIMING_REQ
   message with updated timing information when it receives this
   notification.
*/

typedef struct
    {
/* PhoNet header */
    uint8   media;
    uint8   receiver_dev;
    uint8   sender_dev;
    uint8   res;
    uint16  length;
    uint8   receiver_obj;
    uint8   sender_obj;
/* Message data */
    uint8   TransactionId;
    uint8   MessageId;
    uint8   fill1[2];
    } ModemAudioSpcTimingNeededNtf;

typedef ModemAudioSpcTimingNeededNtf tISI_ModemAudioSpcTimingNeededNtf;


/* ----------------------------------------------------------------------- */
/* Message: MODEM_AUDIO_SPC_DECODER_DATA_REQ                               */
/* ----------------------------------------------------------------------- */
/* This decoder message contains downlink speech data. The amount of actual
   data depends on codec.
   Only one subblock of audio data should be included in this version of the
   specification.
   See /6/ for the data format descriptions. 
*/

typedef struct
    {
/* PhoNet header */
    uint8   media;
    uint8   receiver_dev;
    uint8   sender_dev;
    uint8   res;
    uint16  length;
    uint8   receiver_obj;
    uint8   sender_obj;
/* Message data */
    uint8   TransactionId;
    uint8   MessageId;
    bits16  data_format; /* Values from the constant table MODEM_AUDIO_SPC */
    uint8   fill1[2];
    uint16  num_sb;
    /* Subblock list:
       MODEM_AUDIO_SB_SPC_CODEC_FRAME
    */
    } ModemAudioSpcDecoderDataReq;

typedef ModemAudioSpcDecoderDataReq tISI_ModemAudioSpcDecoderDataReq;


/* ----------------------------------------------------------------------- */
/* Message: MODEM_AUDIO_SPC_ENCODER_DATA_NTF                               */
/* ----------------------------------------------------------------------- */
/* This message contains encoded uplink speech data. It should be sent to the
   ISA entity which sent the MODEM_AUDIO_SPEECH_CODEC_REQ. The amount of
   actual data in the message can vary depending on codec frame used. 
   See /6/ for the data format descriptions 
*/

typedef struct
    {
/* PhoNet header */
    uint8   media;
    uint8   receiver_dev;
    uint8   sender_dev;
    uint8   res;
    uint16  length;
    uint8   receiver_obj;
    uint8   sender_obj;
/* Message data */
    uint8   TransactionId;
    uint8   MessageId;
    bits16  data_format; /* Values from the constant table MODEM_AUDIO_SPC */
    uint8   fill1[2];
    uint16  num_sb;
    /* Subblock list:
       MODEM_AUDIO_SB_SPC_CODEC_FRAME
    */
    } ModemAudioSpcEncoderDataNtf;

typedef ModemAudioSpcEncoderDataNtf tISI_ModemAudioSpcEncoderDataNtf;




/* ----------------------------------------------------------------------- */
/* Notes                                                                   */
/* ----------------------------------------------------------------------- */
/*
   /1/   autogen documentation
         Intranet
         http://rdsuite.europe.nokia.com/products/autogen/documentation.asp
         Link: autogen documentation

   /2/   ISI Server Specification Documentation Guidelines
         Document code: XXXL7714-EN
         DocMan location: eslns42/SRV/Nokia
         DocMan: TP / WP Front End DocMan
         Filename: TP/WP/tpwpfendm.nsf

   /3/   ISI General Information
         Document code: XXXL7715-EN
         DocMan location: eslns42/SRV/Nokia
         DocMan: TP / WP Front End DocMan
         Filename: TP/WP/tpwpfendm.nsf

   /4/   ISI Common Message Descriptions
         Continuus Database: ou1gif
         Continuus Project: GIF_SPECS-ou1gif#browse
         Directory Path: GIF_SPECS/CoreSW
         Filename: i_comm_m.html

   /5/   ISI Common Feature Descriptions
         Continuus Database: ou1gif
         Continuus Project: GIF_SPECS-ou1gif#browse
         Directory Path: GIF_SPECS/CoreSW
         Filename: i_comm_fp.doc

   /6/   WGModem Audio Upper Interface Specification
         Continuus Database: fa1prot
         DoCMan ID: MSW-2.5-17 DSD07069-EN
         Filename:  ModemIP_Audio_Upper_interface_specification.doc

   /7/   WGModem Audio Concept
         Continuus Database: fa1prot
         DoCMan ID: ASD-2.5-15 DCSA21092-EN
         Filename:  modemIP_audio_concept.doc

   /8/   Mandatory Speech Codec speech processing functions;
         AMR Speech Codec Frame Structure, TS 26.101

   /9/   Full rate speech; Discontinuous Transmission (DTX) for full rate
         speech traffic channels (GSM 46.31) 

   /10/  Adaptive Multi-Rate (AMR) speech transcoding (GSM 46.90) 

   /11/  Discontinuous Transmission (DTX) for Adaptive Multi-Rate (AMR)
         speech traffic channels (3GPP TS 46.93 

   /12/  AMR Wideband Speech Codec; Frame Structure (Release 5) 
         3GPP TS 26.201
*/


#endif /* _MODEM_AUDIO_ISI_H */
