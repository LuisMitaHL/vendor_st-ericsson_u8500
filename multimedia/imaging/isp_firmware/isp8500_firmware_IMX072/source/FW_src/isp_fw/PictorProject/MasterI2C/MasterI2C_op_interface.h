/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \file MasterI2C_op_interface.h
 \brief  This file is a part of the MasterI2C module release code and provide an
         interface to the module. All functionalities offered by
         the module are available through this file. The file also declare the page elements
         to be used in virtual register list
 \ingroup MasterI2C
*/
#ifndef _MASTERI2C_OP_INTERFACE_H_
#   define _MASTERI2C_OP_INTERFACE_H_

#   include "MasterI2C.h"

/**
 \enum SendType_te
 \brief Synchronous and Asyncronous Read/Write to slave
 \ingroup MasterI2C
*/
typedef enum
{
    /// Read/Write command is non blocking i.e. function will
    /// return and command will execute in context of interrupt
    SendType_e_Asynchronous,

    /// Read/Write command is blocking i.e. function will be blocked
    /// till the time command is not complete. The function will be blocked
    /// on busy bit polling.
    SendType_e_Synchronous
} SendType_te;

/**
 \enum DeviceAddress_te
 \brief Type of device address
 \ingroup MasterI2C
*/
typedef enum
{
    /// 10 bit i2c slave address
    DeviceAddress_e_10BitDeviceAddress,

    /// 7 bit i2c slave address
    DeviceAddress_e_7BitDeviceAddress
} DeviceAddress_te;

/**
 \enum DeviceIndex_te
 \brief Type of register data index in i2c slave
 \ingroup MasterI2C
*/
typedef enum
{
    /// 16-bits data index
    DeviceIndex_e_16BitDataIndex,

    /// 8-bits data index
    DeviceIndex_e_8BitDataIndex
} DeviceIndex_te;

/**
 \enum HostToMasterI2CRequest_te
 \brief Read/Write command to slave
 \ingroup MasterI2C
*/
typedef enum
{
    /// Read N bytes from slave,
    HostToMasterI2CRequest_e_ReadNBytes,

    /// Write N bytes to slave,
    HostToMasterI2CRequest_e_WriteNBytes
} HostToMasterI2CRequest_te;


/**
 \struct MasterI2CStatus_ts
 \brief Reflects current state of MasterI2C driver and trasaction status
 \ingroup MasterI2C
*/
typedef struct
{
    /// Indicates the Status of MasterI2C driver whether it is free or busy
    uint8_t e_Resource_Status;

    /// Indicates the Status of MasterI2C H/W whether it is free or busy
    uint8_t e_Resource_I2C_TrasactionStatus;

    /// Gives the status of MasterI2C Transaction Status
    uint8_t e_CCI_EERROR_MGMT_Error;

    /// Gives the number of errors observed during transactions on MasterI2C
    /// These could be while writing to or reading from the sensor.
    uint8_t u8_NumberOfTransactionFailures;

    /// Gives the number of times Master I2C Grab has failed in the system.
    uint8_t u8_NumberOfConsecutiveGrabFailures;

    /// Gives the number of times Master I2C has been forced to become free
    uint8_t u8_NumberOfForcedReleases;

    /// Indicates the write FIFO depth used
    uint8_t u8_WriteFifoUseCount;
} MasterI2CStatus_ts;

/**
 \struct MasterI2CClockControl_ts
 \brief Control Page for the MasterI2C clocks
 \ingroup MasterI2C
*/
typedef struct
{
    /// count fall
    uint8_t u8_CountFall;

    /// count rise
    uint8_t u8_CountRise;

    /// count high
    uint8_t u8_CountHigh;

    /// count buffer
    uint8_t u8_CountBuffer;

    /// hold data
    uint8_t u8_CountHoldData;

    /// setup data
    uint8_t u8_CountSetupData;

    /// hold start
    uint8_t u8_CountHoldStart;

    /// setup start
    uint8_t u8_CountSetupStart;

    /// setup stop
    uint8_t u8_CountSetupStop;

    /// Gives the maximum number of grab attempts which should be done for Master I2C before it is forced to become free.
    uint8_t u8_MaximumNumberOfGrabAttempts;
} MasterI2CClockControl_ts;

/**
 \struct HostToMasterI2CControl_ts
 \brief HostToMasterI2C Control structure defines the page elements to be programmed by Host to do MasterI2C Transaction with slave.
 Host can do Read and Write transactions with the help of these page elements
 \ingroup MasterI2C
*/
typedef struct
{
    /// I2C Slave Device ID
    /// NOTE: Only 7 bit device is supported, 10 Bit slave address is not supported
    uint16_t    u16_DeviceID;

    /// Index register in I2C slave to which host would like to read or write
    /// NOTE: Currenlty 16 bit index supported, 8 bit index is not supported
    uint16_t    u16_Index;

    /// Programme Command != Status, After Read/Write access command complete,  Command will become equal to Status
    /// After request completion, module raises event notification to host.
    uint8_t     e_Coin_Command;

    /// The type of Read/Write request to connected I2C slave
    uint8_t     e_HostToMasterI2CRequest_Request;

    /// Number of bytes to be Read/Write on slave
    /// NOTE: Only valid if e_HostToMasterI2CRequest_Request set to e_HostToMasterI2C_ReadNBytes (Read) or e_HostToMasterI2C_WriteNBytes (Write)
    uint8_t     u8_NoBytesReadWrite;

    /// CCI Device address type \n
    /// [DEFAULT]: e_16BitDataIndex(0)
    uint8_t     e_DeviceAddress_Type;

    /// CCI Device ID type \n
    /// [DEFAULT]: e_7BitDeviceAddress(1)
    uint8_t     e_DeviceIndex_Type;
} HostToMasterI2CControl_ts;

/**
 \struct HostToMasterI2CAccessData_ts
 \brief HostToMasterI2C page element defines data page elements to be Read / write. \n
 When read operation is performed, it reflect the data read from slave.\n
 When write operation to be perfomed, host should programme data in the pages.\n
 \ingroup MasterI2C
*/
typedef struct
{
    /// Read / Write data on slave when e_HostToMasterI2CRequest_Request set to e_HostToMasterI2C_ReadNBytes (Read) or e_HostToMasterI2C_WriteNBytes (Write)
    uint8_t u8_arrData[16];
} HostToMasterI2CAccessData_ts;

/**
 \struct HostToMasterI2CAccessStatus_ts
 \brief HostToMasterI2C Status structure: Reflect Host to MasterI2C transaction status
 \ingroup MasterI2C
*/
typedef struct
{
    /// When e_Coin_Command != e_Coin_Status, an attempt is made to perform the required read/write operation
    /// When command is completed, e_Coin_Status is set to e_Coin_Command
    /// and module raises event notification to host.
    uint8_t e_Coin_Status;

    /// Return e_success(1) if there is no error returned by i2c driver
    /// If I2C driver is busy or transaction can not be completed due to error returned by hardware, it will return e_failure(0)
    uint8_t e_Result_Driver;

    /// Counter to keep track on no of errors occured on MasterI2C
    uint8_t u8_HostToMasterI2CAccessErrorCount;
} HostToMasterI2CAccessStatus_ts;

/********************    Intialise API's to be called before BOOT  ************************************/
extern void                             MasterI2C_ModeStaticInitialise (void)TO_EXT_DDR_PRGM_MEM;

/********************************    Read API's   ****************************************************/
extern uint8_t                          MasterI2C_ReadNBytes (
                                        uint16_t    u16_DeviceID,
                                        uint16_t    u16_Address,
                                        uint8_t     *ptru8_Data,
                                        uint8_t     u8_NumberOfBytes,
                                        uint8_t     e_DeviceAddress_Type,
                                        uint8_t     e_DeviceIndex_Type)TO_EXT_PRGM_MEM;

/********************************    Write API's   ****************************************************/
extern uint8_t                          MasterI2C_WriteNByte (
                                        uint16_t    u16_DeviceID,
                                        uint16_t    u16_Address,
                                        uint8_t     *ptru8_Data,
                                        uint8_t     u8_NumberOfBytes,
                                        uint8_t     e_DeviceAddress_Type,
                                        uint8_t     e_DeviceIndex_Type,
                                        uint8_t     e_SendType_Type)TO_EXT_PRGM_MEM;

/********************************   Host to MasterI2C Access  **********************************************/
//extern void                             HostToMasterI2CAccess_Run (void);

extern void                             MasterI2C_Isr (void);

/**** Page elements to be exported *********************/

/// MasterI2C Status page element
// The structure is used in interrupt context, so it should be volatile
volatile extern MasterI2CStatus_ts               g_MasterI2CStatus;

/// MasterI2C clock setup page element. I2C clocks should be programmed in following pages
extern MasterI2CClockControl_ts         g_MasterI2CClockControl;

/// MasterI2C Access control page elements
extern HostToMasterI2CControl_ts        g_HostToMasterI2CAccessControl;

/// MasterI2C Access Data page elements
extern HostToMasterI2CAccessData_ts     g_HostToMasterI2CAccessData;

/// MasterI2C  status page elements
extern HostToMasterI2CAccessStatus_ts   g_HostToMasterI2CAccessStatus;

extern void HostToMasterI2CAccess_Run (void)TO_EXT_DDR_PRGM_MEM;
#endif // _MASTERI2C_OP_INTERFACE_H_

