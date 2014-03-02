/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/* --- prevents recursive inclusion --------------------------------------- */
#ifndef __STSSC_H
#define __STSSC_H


/* --- includes ----------------------------------------------------------- */
#include "inc/type.h"
// #define PUBLIC
#define  FALSE  0
#define  TRUE  1




/* --- defines ------------------------------------------------------------ */
// these are defines imported from stddefs.h
typedef volatile unsigned int       DU32        ;

typedef const char *            ST_Revision_t   ;
typedef t_uint32                     ST_ErrorCode_t  ;

#define TraceI2C(x)

enum
{
    ST_NO_ERROR                 = 0,
    ST_ERROR_BAD_PARAMETER,             // Bad parameter passed
    ST_ERROR_NO_MEMORY,                 // Memory allocation failed
    ST_ERROR_TIMEOUT                    // Timeout occured
};


/* --- variables ---------------------------------------------------------- */

/* --- enumerations ------------------------------------------------------- */
/* =======================================================================
   Definition of a handle
   ======================================================================== */
typedef     t_uint32  STSSC_Handle_t;

/* =======================================================================
   MAIN structure to initialize a handle
   ======================================================================== */
typedef struct
{
    t_uint32                             SDAPIOBase      ;   /* HW PIO base      */
    t_uint8                              SDAPIOBit       ;   /* SDA bit number   */
    t_uint32                             SCKPIOBase      ;   /* HW PIO base      */
    t_uint8                              SCKPIOBit       ;   /* SCK bit number   */
    t_uint32                             BaudRate        ;   /* baudrate         */
}   STSSC_InitParams_t;


#ifdef __cplusplus
extern "C" {
#endif

/* --- prototypes of functions -------------------------------------------- */
/* =======================================================================
   MAIN functions in this driver
   ======================================================================== */
/* --- registration, destruction, revision -------------------------------- */
ST_Revision_t  STSSC_GetRevision    ( void );
ST_ErrorCode_t STSSC_Init           ( STSSC_InitParams_t   *InitParams_p    ,STSSC_Handle_t *Handle );
ST_ErrorCode_t STSSC_Term           ( STSSC_Handle_t        Handle );

/* --- standard transactions ---------------------------------------------- */
ST_ErrorCode_t STSSC_Read           ( STSSC_Handle_t        Handle         ,
                                      t_uint8                    SlaveAddress   ,
                                      t_uint8                   *Buffer         ,
                                      t_uint32                   MaxLength      ,
                                      t_uint32                  *ActualLength   ,
                                      t_uint32                   TimeOut         );
ST_ErrorCode_t STSSC_Write          ( STSSC_Handle_t        Handle         ,
                                      t_uint8                    SlaveAddress   ,
                                      t_uint8                   *Buffer         ,
                                      t_uint32                   Length         ,
                                      t_uint32                  *NumWritten     ,
                                      t_uint32                   TimeOut        ,
                                      t_bool                  NoStop         );
/* --- modifications on the protocol -------------------------------------- */
ST_ErrorCode_t STSSC_SetBaudRate    ( STSSC_Handle_t        Handle, t_uint32 NewBaudRate  );
PUBLIC void i2c_read_index16 (t_uint8 controller_number, t_uint32 device_addr, t_uint32 index1, t_uint32 index2, t_uint32 number_of_data, t_uint8 *pread_buff);
PUBLIC void i2c_write_p(t_uint8 controller_number, t_uint32 device_addr, t_uint32 number_of_data, t_uint8 *pwrite_buff);
t_uint8 Read_8_8(t_uint16 i2cnum, t_uint16 i2c_addr, t_uint8 i2c_reg);
void Write_16_8(t_uint16 i2cnum,t_uint16 i2c_ad,t_uint16 i2c_sla,t_uint8 data);
t_uint8 Read_16_8(t_uint16 i2cnum,t_uint16 i2c_ad,t_uint16 i2c_sla);
void Write_16_16  (t_uint16 i2cnum,t_uint16 i2c_ad,t_uint16 i2c_sla,t_uint16 data);


#ifdef __cplusplus
}
#endif


#endif /* #ifndef __STSSC_H */



