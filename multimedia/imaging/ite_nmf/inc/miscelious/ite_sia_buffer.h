/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef ITE_SIA_BUFFER_H_
#define ITE_SIA_BUFFER_H_

#if !( defined(__ARM_LINUX) || defined(__ARM_SYMBIAN))
#include "hcl_defs.h"

#if !(defined(__PEPS8500_SIA) || defined(_SVP_))
#ifdef __STN_8500
#include "mmte_mcde_driver_8500.h"
#include "mmte_b2r2_driver_8500.h"
#else
#include "mmte_mcde_driver.h"
#include "mmte_b2r2_driver.h"
#endif
#endif

#else
#include <inc/type.h>
#endif

#include <los/api/los_api.h>
#include "VhcElementDefs.h"
#include <grab_types.idt.h>



#undef EXTERN_DEF
#ifdef ITE_SIA_BUFFER_C_
#define EXTERN_DEF
#else
#define EXTERN_DEF extern
#endif

#ifdef __STN_8500
typedef enum
{

 SRC_FORMAT_1BPP_PALLETIZED            =    0,
 SRC_FORMAT_2BPP_PALLETIZED            =    1,
 SRC_FORMAT_4BPP_PALLETIZED            =    2,
 SRC_FORMAT_8BPP_PALLETIZED            =    3,
 SRC_FORMAT_12BPP_RGB_444              =    4,
 SRC_FORMAT_16BPP_ARGB_4444         =   5,
 SRC_FORMAT_16BPP_IRGB_1555         =   6,
 SRC_FORMAT_16BPP_RGB_565              =    7,
 SRC_FORMAT_24BPP_PACKED_RGB_888       =    8,
 SRC_FORMAT_24BPP_UNPACKED_RGB_888  =   9,
 SRC_FORMAT_32BPP_ARGB_8888         =   10,
 SRC_FORMAT_YCbCr_422                  =    11
} t_mcde_src_image_format;
#endif

typedef enum
{
    BUF    = 0,
    BUF_LR = 1,
    BUF_HR = 2,
    BUF_BMS= 3
} t_siabuffer_pipe;

#define    NO_OF_LUTBINS                  128
#define    GAMMA_LUT_TABLE_SIZE    512       //Each table has NO_OF_LUTBINS values and each value is of 4 bytes
#define    NUMBER_OF_LUT_TABLES           12        //6 for ce0 and 6 for ce1. IN each CE, 2 for each red, green and blue (sharp and unsharp values)

typedef struct _siapicturebuffer_ {
   t_los_memory_handle      handle;
   t_uint32             address;
   t_los_logical_address        logAddress;
   t_uint32             buffersizeByte;
   enum e_grabFormat        Grb_colorformat;

   OutputFormat_te          PipeOutputformat;


#if !( defined(__ARM_LINUX) || defined(__ARM_SYMBIAN) || defined(__PEPS8500_SIA) || defined(_SVP_))
   t_b2r2_color_form        B2R2_colorformat;
   t_mcde_src_image_format  MCDE_colorformat;
#endif
   t_bool           is420_not422;
   t_uint32             framesizeX;
   t_uint32             framesizeY;
   t_uint32             windowsizeX;
   t_uint32             windowsizeY;
   t_uint32         offsetX;
   t_uint32         offsetY;
   t_siabuffer_pipe     pipe;
   t_uint32         pelorder;
   t_uint32             nonactivelines;
}ts_siapicturebuffer, *tps_siapicturebuffer;

typedef struct _siasensortunningbuffer_ {
   t_los_memory_handle      handle;
   t_uint32             phyaddress;
   t_los_logical_address        logAddress;
   t_uint32             ISPBufferBaseAddress;
   t_uint32              size;

}ts_siasensortunningbuffer, *tps_siasensortunningbuffer;


typedef struct _siaMetaDatabuffer_ {
   t_los_memory_handle      handle;
   t_uint32             phyaddress;
   t_los_logical_address        logAddress;
   t_uint32             ISPBufferBaseAddress;
   t_uint32              size;

}ts_siaMetaDatabuffer, *tps_siaMetaDatabuffer;

typedef struct _siaFocusStatbuffer_ {
   t_los_memory_handle      handle;
   t_uint32             phyaddress;
   t_los_logical_address        logAddress;
   t_uint32             ISPBufferBaseAddress;
   t_uint32              size;

}ts_siaFocusStatbuffer, *tps_siaFocusStatbuffer;

typedef struct _siaNVMbuffer_ {
   t_los_memory_handle      handle;
   t_uint32             phyaddress;
   t_los_logical_address        logAddress;
   t_uint32             ISPBufferBaseAddress;
   t_uint32              size;

}ts_siaNVMbuffer, *tps_siaNVMbuffer;

//gamma buffer
typedef struct _siaGammaLUTbuffer
{
   t_los_memory_handle      handle;
   t_uint32             phyaddress;
   t_los_logical_address        logAddress;
   t_uint32             ISPBufferBaseAddress;
   t_uint32              size;
}ts_siaGammaLUTbuffer, *tps_siaGammaLUTbuffer;



typedef struct {
    t_uint8 *pGridR;              // Virtual memory address
    t_uint8 *pGridG;              // Virtual memory address
    t_uint8 *pGridB;              // Virtual memory address
    t_uint8 *pGridS;              // Virtual memory address
    t_uint32 apHistR[256];
    t_uint32 apHistG[256];
    t_uint32 apHistB[256];
    Glace_Statistics_ts glaceStats;  // All included addresses are expressed in ISP address space
    t_uint32 ispHistAddr_R;       // In Isp memory space address
    t_uint32 ispHistAddr_G;       // In Isp memory space address
    t_uint32 ispHistAddr_B;       // In Isp memory space address
    t_uint32 ispGlaceStatsAddr;   // In Isp memory space address
} StatsBuffersStorage_t;


typedef struct {
   
    t_uint32 ispNVMAddr;   // In Isp memory space address
} NVMBuffersStorage_t;

typedef struct {
   
    t_uint32 ispStreamConfigDataAddr;   // In Isp memory space address
} StreamConfigDataBuffersStorage_t;



typedef struct {
  
    t_uint32 ispFocusStatsAddr;   // In Isp memory space address
} FocusStatsBuffersStorage_t;

//ER 427679,
/* Log messages select */
typedef enum {
       TraceMsgOutput_e_NOMSGS,  /** No log messages */
       TraceMsgOutput_e_XTI,          /** XTI */
       TraceMsgOutput_e_MEMORY_DUMP   /** Memory dumping */
} TraceMsgOutput_te;

// Structure for trace parameters
typedef struct traceProp
{
    TraceMsgOutput_te traceMechanism;
    t_uint16 traceLogLevel;
    t_uint8 traceEnable;
}TraceProp_ts;


EXTERN_DEF ts_siapicturebuffer GrabBufferLR[2]
#if !( defined(__ARM_LINUX) || defined(__ARM_SYMBIAN) || defined(__PEPS8500_SIA) || defined(_SVP_))
    #ifdef ITE_SIA_BUFFER_C_
            = {{
               GrabBufferLR[0].handle  = 0,
               GrabBufferLR[0].address = 0,
               GrabBufferLR[0].logAddress = 0,
               GrabBufferLR[0].buffersizeByte = 0,
               GrabBufferLR[0].Grb_colorformat = GRBFMT_R5G6B5,
               GrabBufferLR[0].PipeOutputformat = OutputFormat_e_YUV,
               GrabBufferLR[0].B2R2_colorformat = RGB565,
               GrabBufferLR[0].MCDE_colorformat = SRC_FORMAT_16BPP_RGB_565,
               GrabBufferLR[0].is420_not422 = 0,
               GrabBufferLR[0].framesizeX = 0,
               GrabBufferLR[0].framesizeY = 0,
               GrabBufferLR[0].windowsizeX = 0,
               GrabBufferLR[0].windowsizeY = 0,
               GrabBufferLR[0].offsetX = 0,
               GrabBufferLR[0].offsetY = 0,
               GrabBufferLR[0].pipe = (t_siabuffer_pipe)0,
               GrabBufferLR[0].pelorder = 0,
               GrabBufferLR[0].nonactivelines = 0,
               },
               {
               GrabBufferLR[1].handle  = 0,
               GrabBufferLR[1].address = 0,
               GrabBufferLR[1].logAddress = 0,
               GrabBufferLR[1].buffersizeByte = 0,
               GrabBufferLR[1].Grb_colorformat = GRBFMT_R5G6B5,
               GrabBufferLR[1].PipeOutputformat = OutputFormat_e_YUV,
               GrabBufferLR[1].B2R2_colorformat = RGB565,
               GrabBufferLR[1].MCDE_colorformat = SRC_FORMAT_16BPP_RGB_565,
               GrabBufferLR[1].is420_not422 = 0,
               GrabBufferLR[1].framesizeX = 0,
               GrabBufferLR[1].framesizeY = 0,
               GrabBufferLR[1].windowsizeX = 0,
               GrabBufferLR[1].windowsizeY = 0,
               GrabBufferLR[1].offsetX = 0,
               GrabBufferLR[1].offsetY = 0,
               GrabBufferLR[1].pipe = (t_siabuffer_pipe)0,
               GrabBufferLR[1].pelorder = 0,
               GrabBufferLR[1].nonactivelines = 0,
               }};

    #else
    ;
    #endif
#else
    #ifdef ITE_SIA_BUFFER_C_
            = {{
               GrabBufferLR[0].handle  = 0,
               GrabBufferLR[0].address = 0,
               GrabBufferLR[0].logAddress = 0,
               GrabBufferLR[0].buffersizeByte = 0,
               GrabBufferLR[0].Grb_colorformat = GRBFMT_R5G6B5,
               GrabBufferLR[0].PipeOutputformat = OutputFormat_e_YUV,
               GrabBufferLR[0].is420_not422 = 0,
               GrabBufferLR[0].framesizeX = 0,
               GrabBufferLR[0].framesizeY = 0,
               GrabBufferLR[0].windowsizeX = 0,
               GrabBufferLR[0].windowsizeY = 0,
               GrabBufferLR[0].offsetX = 0,
               GrabBufferLR[0].offsetY = 0,
               GrabBufferLR[0].pipe = (t_siabuffer_pipe)0,
               GrabBufferLR[0].pelorder = 0,
               GrabBufferLR[0].nonactivelines = 0,
               },
               {
               GrabBufferLR[1].handle  = 0,
               GrabBufferLR[1].address = 0,
               GrabBufferLR[1].logAddress = 0,
               GrabBufferLR[1].buffersizeByte = 0,
               GrabBufferLR[1].Grb_colorformat = GRBFMT_R5G6B5,
               GrabBufferLR[1].PipeOutputformat = OutputFormat_e_YUV,
               GrabBufferLR[1].is420_not422 = 0,
               GrabBufferLR[1].framesizeX = 0,
               GrabBufferLR[1].framesizeY = 0,
               GrabBufferLR[1].windowsizeX = 0,
               GrabBufferLR[1].windowsizeY = 0,
               GrabBufferLR[1].offsetX = 0,
               GrabBufferLR[1].offsetY = 0,
               GrabBufferLR[1].pipe = (t_siabuffer_pipe)0,
               GrabBufferLR[1].pelorder = 0,
               GrabBufferLR[1].nonactivelines = 0,
               }};
    #else
    ;
    #endif
#endif

EXTERN_DEF ts_siapicturebuffer GrabBufferHR[2]
#if !( defined(__ARM_LINUX) || defined(__ARM_SYMBIAN) || defined(__PEPS8500_SIA) || defined(_SVP_))
    #ifdef ITE_SIA_BUFFER_C_
            = {{
               GrabBufferHR[0].handle  = 0,
               GrabBufferHR[0].address = 0,
               GrabBufferHR[0].logAddress = 0,
               GrabBufferHR[0].buffersizeByte = 0,
               GrabBufferHR[0].Grb_colorformat = GRBFMT_YUV420_MB_SEMIPLANAR_DECIMATED,
               GrabBufferHR[0].PipeOutputformat = OutputFormat_e_YUV,
               GrabBufferHR[0].B2R2_colorformat = RGB565,
               GrabBufferHR[0].MCDE_colorformat = SRC_FORMAT_16BPP_RGB_565,
               GrabBufferHR[0].is420_not422 = 0,
               GrabBufferHR[0].framesizeX = 0,
               GrabBufferHR[0].framesizeY = 0,
               GrabBufferHR[0].windowsizeX = 0,
               GrabBufferHR[0].windowsizeY = 0,
               GrabBufferHR[0].offsetX = 0,
               GrabBufferHR[0].offsetY = 0,
               GrabBufferHR[0].pipe = (t_siabuffer_pipe)0s,
               GrabBufferHR[0].pelorder = 0,
               GrabBufferHR[0].nonactivelines = 0,
               },
               {
               GrabBufferHR[1].handle  = 0,
               GrabBufferHR[1].address = 0,
               GrabBufferHR[1].logAddress = 0,
               GrabBufferHR[1].buffersizeByte = 0,
               GrabBufferHR[1].Grb_colorformat = GRBFMT_YUV420_MB_SEMIPLANAR_DECIMATED,
               GrabBufferHR[1].PipeOutputformat = OutputFormat_e_YUV,
               GrabBufferHR[1].B2R2_colorformat = RGB565,
               GrabBufferHR[1].MCDE_colorformat = SRC_FORMAT_16BPP_RGB_565,
               GrabBufferHR[1].is420_not422 = 0,
               GrabBufferHR[1].framesizeX = 0,
               GrabBufferHR[1].framesizeY = 0,
               GrabBufferHR[1].windowsizeX = 0,
               GrabBufferHR[1].windowsizeY = 0,
               GrabBufferHR[1].offsetX = 0,
               GrabBufferHR[1].offsetY = 0,
               GrabBufferHR[1].pipe = (t_siabuffer_pipe)0,
               GrabBufferHR[1].pelorder = 0,
               GrabBufferHR[1].nonactivelines = 0,
               }};

    #else
    ;
    #endif
#else
    #ifdef ITE_SIA_BUFFER_C_
            = {{
               GrabBufferHR[0].handle  = 0,
               GrabBufferHR[0].address = 0,
               GrabBufferHR[0].logAddress = 0,
               GrabBufferHR[0].buffersizeByte = 0,
               GrabBufferHR[0].Grb_colorformat = GRBFMT_YUV420_MB_SEMIPLANAR_DECIMATED,
               GrabBufferHR[0].PipeOutputformat = OutputFormat_e_YUV,
               GrabBufferHR[0].is420_not422 = 0,
               GrabBufferHR[0].framesizeX = 0,
               GrabBufferHR[0].framesizeY = 0,
               GrabBufferHR[0].windowsizeX = 0,
               GrabBufferHR[0].windowsizeY = 0,
               GrabBufferHR[0].offsetX = 0,
               GrabBufferHR[0].offsetY = 0,
               GrabBufferHR[0].pipe = (t_siabuffer_pipe)0,
               GrabBufferHR[0].pelorder = 0,
               GrabBufferHR[0].nonactivelines = 0,
               },
               {
               GrabBufferHR[1].handle  = 0,
               GrabBufferHR[1].address = 0,
               GrabBufferHR[1].logAddress = 0,
               GrabBufferHR[1].buffersizeByte = 0,
               GrabBufferHR[1].Grb_colorformat = GRBFMT_YUV420_MB_SEMIPLANAR_DECIMATED,
               GrabBufferHR[1].PipeOutputformat = OutputFormat_e_YUV,
               GrabBufferHR[1].is420_not422 = 0,
               GrabBufferHR[1].framesizeX = 0,
               GrabBufferHR[1].framesizeY = 0,
               GrabBufferHR[1].windowsizeX = 0,
               GrabBufferHR[1].windowsizeY = 0,
               GrabBufferHR[1].offsetX = 0,
               GrabBufferHR[1].offsetY = 0,
               GrabBufferHR[1].pipe = (t_siabuffer_pipe)0,
               GrabBufferHR[1].pelorder = 0,
               GrabBufferHR[1].nonactivelines = 0,
               }};
    #else
    ;
    #endif
#endif

EXTERN_DEF ts_siapicturebuffer GrabBufferBMS[2]
#if !( defined(__ARM_LINUX) || defined(__ARM_SYMBIAN) || defined(__PEPS8500_SIA) || defined(_SVP_))
    #ifdef ITE_SIA_BUFFER_C_
            = {{
               GrabBufferBMS[0].handle  = 0,
               GrabBufferBMS[0].address = 0,
               GrabBufferBMS[0].logAddress = 0,
               GrabBufferBMS[0].buffersizeByte = 0,
               GrabBufferBMS[0].Grb_colorformat = GRBFMT_RAW8,
               GrabBufferBMS[0].PipeOutputformat = (OutputFormat_te)0,
               GrabBufferBMS[0].B2R2_colorformat = 0,
               GrabBufferBMS[0].MCDE_colorformat = 0,
               GrabBufferBMS[0].is420_not422 = 0,
               GrabBufferBMS[0].framesizeX = 0,
               GrabBufferBMS[0].framesizeY = 0,
               GrabBufferBMS[0].windowsizeX = 0,
               GrabBufferBMS[0].windowsizeY = 0,
               GrabBufferBMS[0].offsetX = 0,
               GrabBufferBMS[0].offsetY = 0,
               GrabBufferBMS[0].pipe = (t_siabuffer_pipe)0,
               GrabBufferBMS[0].pelorder = 0,
               GrabBufferBMS[0].nonactivelines = 0,
               },
               {
               GrabBufferBMS[1].handle  = 0,
               GrabBufferBMS[1].address = 0,
               GrabBufferBMS[1].logAddress = 0,
               GrabBufferBMS[1].buffersizeByte = 0,
               GrabBufferBMS[1].Grb_colorformat = GRBFMT_RAW8,
               GrabBufferBMS[1].PipeOutputformat = (OutputFormat_te)0,
               GrabBufferBMS[1].B2R2_colorformat = 0,
               GrabBufferBMS[1].MCDE_colorformat = 0,
               GrabBufferBMS[1].is420_not422 = 0,
               GrabBufferBMS[1].framesizeX = 0,
               GrabBufferBMS[1].framesizeY = 0,
               GrabBufferBMS[1].windowsizeX = 0,
               GrabBufferBMS[1].windowsizeY = 0,
               GrabBufferBMS[1].offsetX = 0,
               GrabBufferBMS[1].offsetY = 0,
               GrabBufferBMS[1].pipe = (t_siabuffer_pipe)0,
               GrabBufferBMS[1].pelorder = 0,
               GrabBufferBMS[1].nonactivelines = 0,
               }};

    #else
    ;
    #endif
#else
    #ifdef ITE_SIA_BUFFER_C_
            = {{
               GrabBufferBMS[0].handle  = 0,
               GrabBufferBMS[0].address = 0,
               GrabBufferBMS[0].logAddress = 0,
               GrabBufferBMS[0].buffersizeByte = 0,
               GrabBufferBMS[0].Grb_colorformat = GRBFMT_RAW8,
               GrabBufferBMS[0].PipeOutputformat = (OutputFormat_te)0,
               GrabBufferBMS[0].is420_not422 = 0,
               GrabBufferBMS[0].framesizeX = 0,
               GrabBufferBMS[0].framesizeY = 0,
               GrabBufferBMS[0].windowsizeX = 0,
               GrabBufferBMS[0].windowsizeY = 0,
               GrabBufferBMS[0].offsetX = 0,
               GrabBufferBMS[0].offsetY = 0,
               GrabBufferBMS[0].pipe = (t_siabuffer_pipe)0,
               GrabBufferBMS[0].pelorder = 0,
               GrabBufferBMS[0].nonactivelines = 0,
               },
               {
               GrabBufferBMS[1].handle  = 0,
               GrabBufferBMS[1].address = 0,
               GrabBufferBMS[1].logAddress = 0,
               GrabBufferBMS[1].buffersizeByte = 0,
               GrabBufferBMS[1].Grb_colorformat = GRBFMT_RAW8,
               GrabBufferBMS[1].PipeOutputformat = (OutputFormat_te)0,
               GrabBufferBMS[1].is420_not422 = 0,
               GrabBufferBMS[1].framesizeX = 0,
               GrabBufferBMS[1].framesizeY = 0,
               GrabBufferBMS[1].windowsizeX = 0,
               GrabBufferBMS[1].windowsizeY = 0,
               GrabBufferBMS[1].offsetX = 0,
               GrabBufferBMS[1].offsetY = 0,
               GrabBufferBMS[1].pipe = (t_siabuffer_pipe)0,
               GrabBufferBMS[1].pelorder = 0,
               GrabBufferBMS[1].nonactivelines = 0,
               }};
    #else
    ;
    #endif
#endif


EXTERN_DEF ts_siapicturebuffer LCDBuffer
#if !( defined(__ARM_LINUX) || defined(__ARM_SYMBIAN) || defined(__PEPS8500_SIA) || defined(_SVP_))
    #ifdef ITE_SIA_BUFFER_C_
            = {
               LCDBuffer.handle  = 0,
               LCDBuffer.address = 0,
               LCDBuffer.logAddress = 0,
               LCDBuffer.buffersizeByte = 0,
               LCDBuffer.Grb_colorformat = GRBFMT_R5G6B5,
               LCDBuffer.PipeOutputformat = OutputFormat_e_YUV,
               LCDBuffer.B2R2_colorformat = RGB565,
               LCDBuffer.MCDE_colorformat = SRC_FORMAT_16BPP_RGB_565,
               LCDBuffer.is420_not422 = 0,
               LCDBuffer.framesizeX = 0,
               LCDBuffer.framesizeY = 0,
               LCDBuffer.windowsizeX = 0,
               LCDBuffer.windowsizeY = 0,
               LCDBuffer.offsetX = 0,
               LCDBuffer.offsetY = 0,
               LCDBuffer.pipe = (t_siabuffer_pipe)0,
               LCDBuffer.pelorder = 0,
               LCDBuffer.nonactivelines = 0,
               };

    #else
    ;
    #endif
#else
    #ifdef ITE_SIA_BUFFER_C_
            = {
               LCDBuffer.handle  = 0,
               LCDBuffer.address = 0,
               LCDBuffer.logAddress = 0,
               LCDBuffer.buffersizeByte = 0,
               LCDBuffer.Grb_colorformat = GRBFMT_R5G6B5,
               LCDBuffer.PipeOutputformat = OutputFormat_e_YUV,
               LCDBuffer.is420_not422 = 0,
               LCDBuffer.framesizeX = 0,
               LCDBuffer.framesizeY = 0,
               LCDBuffer.windowsizeX = 0,
               LCDBuffer.windowsizeY = 0,
               LCDBuffer.offsetX = 0,
               LCDBuffer.offsetY = 0,
               LCDBuffer.pipe =(t_siabuffer_pipe)0,
               LCDBuffer.pelorder = 0,
               LCDBuffer.nonactivelines = 0,
               };
    #else
    ;
    #endif
#endif

EXTERN_DEF ts_siapicturebuffer GamBuffer
#if !( defined(__ARM_LINUX) || defined(__ARM_SYMBIAN) || defined(__PEPS8500_SIA) || defined(_SVP_))
    #ifdef ITE_SIA_BUFFER_C_
            = {
               GamBuffer.handle  = 0,
               GamBuffer.address = 0,
               GamBuffer.logAddress = 0,
               GamBuffer.buffersizeByte = 0,
               GamBuffer.Grb_colorformat = GRBFMT_R5G6B5,
               GamBuffer.PipeOutputformat = OutputFormat_e_YUV,
               GamBuffer.B2R2_colorformat = RGB565,
               GamBuffer.MCDE_colorformat = (t_mcde_src_image_format)0,
               GamBuffer.is420_not422 = 0,
               GamBuffer.framesizeX = 0,
               GamBuffer.framesizeY = 0,
               GamBuffer.windowsizeX = 0,
               GamBuffer.windowsizeY = 0,
               GamBuffer.offsetX = 0,
               GamBuffer.offsetY = 0,
               GamBuffer.pipe = (t_siabuffer_pipe)0,
               GamBuffer.pelorder = 0,
               GamBuffer.nonactivelines = 0,
               };

    #else
    ;
    #endif
#else
    #ifdef ITE_SIA_BUFFER_C_
            = {
               GamBuffer.handle  = 0,
               GamBuffer.address = 0,
               GamBuffer.logAddress = 0,
               GamBuffer.buffersizeByte = 0,
               GamBuffer.Grb_colorformat = GRBFMT_R5G6B5,
               GamBuffer.PipeOutputformat = OutputFormat_e_YUV,
               GamBuffer.is420_not422 = 0,
               GamBuffer.framesizeX = 0,
               GamBuffer.framesizeY = 0,
               GamBuffer.windowsizeX = 0,
               GamBuffer.windowsizeY = 0,
               GamBuffer.offsetX = 0,
               GamBuffer.offsetY = 0,
               GamBuffer.pipe = BUF,
               GamBuffer.pelorder = 0,
               GamBuffer.nonactivelines = 0,
               };
    #else
    ;
    #endif
#endif


#ifdef __cplusplus
extern "C"
{
#endif



int ITE_createSiaPictureBuffer(tps_siapicturebuffer     p_siapictbuffer,
                   t_uint32         PictureSizeX,
                   t_uint32         PictureSizeY,
                   enum e_grabFormat        ColorFormat,
                   t_siabuffer_pipe pipe);

void ITE_useSiaPictureBuffer(tps_siapicturebuffer p_siapictbuffer,
                   t_uint32 PictureSizeX,
                   t_uint32 PictureSizeY,
                   enum e_grabFormat ColorFormat,
                   t_uint32 PhysicalAdress,
                   t_los_logical_address    LogicalAdress);

void ITE_FreeSiaPictureBuffer(tps_siapicturebuffer p_siapictbuffer);
void ITE_unuseSiaPictureBuffer(tps_siapicturebuffer p_siapictbuffer);
void ITE_Initialize_SiaPictureBuffer(tps_siapicturebuffer p_siapictbuffer);
void ITE_createLCDBuffer(t_uint16 sizex, t_uint16 sizey,enum e_grabFormat format);

void ITE_NMF_createStatsBuffer(tps_siaMetaDatabuffer pBufferMetaData);
void ITE_NMF_freeStatsBuffer(tps_siaMetaDatabuffer pBufferMetaData);
void ITE_NMF_freeFocusStatsBuffer(tps_siaFocusStatbuffer pFocusBufferMetaData);

void ITE_NMF_createFocusStatsBuffer(tps_siaFocusStatbuffer pFocusBufferMetaData, t_uint32 bufSize);
void ITE_NMF_createNVMBuffer(tps_siaNVMbuffer pNVMBufferMetaData);
void ITE_NMF_freeNVMBuffer(tps_siaNVMbuffer pNVMBufferMetaData);
StatsBuffersStorage_t* ITE_NMF_createTraceBuffer(tps_siaMetaDatabuffer pBufferMetaData);
void ITE_NMF_freeTraceBuffer(tps_siaMetaDatabuffer pBufferMetaData);

void ITE_NMF_freeFocusStatsBuffer(tps_siaFocusStatbuffer pFocusBufferMetaData);
void* ITE_NMF_create_Streaming_Config_Data_Buffer(tps_siaMetaDatabuffer pBufferMetaData);

void ITE_NMF_freeSensorOutputModeBuffer(tps_siaMetaDatabuffer pBufferMetaData);

void ITE_NMF_free_SENSOR_TUNNING_Buffer(tps_siasensortunningbuffer psensortunningData);
void* ITE_NMF_create_SENSOR_TUNNING_Buffer(tps_siasensortunningbuffer psensortunningData);

void ITE_NMF_createGammaBuffer(tps_siaGammaLUTbuffer psiaGammaLUTbuffer);
void ITE_NMF_freeGammaBuffer(tps_siaGammaLUTbuffer pGammaLUTMetaData);


#ifdef __cplusplus
}
#endif

#endif /* ITE_SIA_BOOTCMD_H_ */
