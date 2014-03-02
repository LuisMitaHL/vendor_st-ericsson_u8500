/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
/**
 \file MasterI2C_ip_interface.h
 \brief This file is NOT a part of the module release code.
            All inputs needed by the MaterI2C module that can only
            be resolved at the project level (at integration time)
            are met through this file.
            It is the responsibility of the integrator to generate
            this file at integration time and meet all the input
            dependencies.


 \note      The following sections define the dependencies that must be
           met by the system integrator. The way these dependencies
           have been resolved here are just for example. These dependencies
           must be appropriately resolved based on the platform being used.
 \ingroup MasterI2C
*/
#ifndef _MASTERI2C_IP_INTERFACE_H_
#   define _MASTERI2C_IP_INTERFACE_H_

#   include "Platform.h"
#   include "PictorhwReg.h"
#   include "EventManager.h"

/// Total NO of bytes in FIFO implemented in MasterI2C IP
#   define BYTES_IN_HW_FIFO_DEPTH  16

/// Enable MasterI2C IP
#   define I2C_ENABLE(enable, soft_reset)  Set_MASTER_CCI_MASTER_CCI_ENABLE(enable, soft_reset)

/* TODO: <AG>: The device address should be 7 bits, check for compatibility on 10 bit addresses
*/

/// 16 bit I2C device address \n
/// NOTE: Currently MasterI2C driver support only 7 bit address
#   define MasterI2C_ProgramDeviceID(x)    Set_MASTER_CCI_MASTER_CCI_DEVICE_ID_device_id_reg(x)

/// 16 bit Index register into I2C device \n
/// NOTE: Currently MasterI2C driver support only 16 bit address, 8 bit index is not supported
#   define MasterI2C_ProgramIndex(x)   Set_MASTER_CCI_MASTER_CCI_INDEX_index_reg(x)

/// No of bytes to be Read from I2C device \n
/// Write:0, Read:1,Repeated start:1 No of bytes, 16 bit device index, 7 bit device id
//#   define MasterI2C_Program_Read_RepeatedStart_NoOfBytes_16BitIndex_8BitDeviceID(x)    Set_Master_CCI_MSTR_CCI_CONTROL(0,1,1,x,single_byte_index_B_0x0,Single_byte_device_id_B_0x1)

/// Write:0, Read:1, Repeated start:1, No of bytes,
/// DeviceAddressType: 0: 16 bit device index, 1: 8 bit device index
/// DeviceIDType:      0: 10 bit device ID,    1: 7 bit device index
#   define MasterI2C_Program_Read_RepeatedStart_NoOfBytes_Index_DeviceID(x, DeviceIDType, DeviceIndexType)                               \
        Set_MASTER_CCI_MASTER_CCI_CONTROL(0, 1, 1, x, DeviceIndexType, DeviceIDType)

/// No of bytes to write on I2C device \n
/// Write:1, Read:0, Repeated start:0, No of bytes, 16 bit device index, 7 bit device id
//#   define MasterI2C_Program_Write_NoOfBytes_16BitIndex_8BitDeviceID(x)   Set_Master_CCI_MSTR_CCI_CONTROL(1,0,0,x,single_byte_index_B_0x0,Single_byte_device_id_B_0x1)

/// Write:1, Read:0, Repeated start:1, No of bytes,
/// DeviceAddressType: 0: 16 bit device index, 1: 8 bit device index
/// DeviceIDType:      0: 10 bit device ID,    1: 7 bit device index
#   define MasterI2C_Program_Write_NoOfBytes_Index_DeviceID(x, DeviceIDType, DeviceIndexType)                  \
        Set_MASTER_CCI_MASTER_CCI_CONTROL(1, 0, 0, x, DeviceIndexType, DeviceIDType)

/// Send device id to the MasterI2C IP
#   define MasterI2C_Set_Device_ID_Valid()     Set_MASTER_CCI_MASTER_CCI_TRANSFER(1, 0, 0)    // device_id_valid

    /// Start I2C transaction on Bus
#   define MasterI2C_Start_Trasaction()        Set_MASTER_CCI_MASTER_CCI_TRANSFER(0, 1, 0)    // start_transfer

    /// Transfer data from write register bank0-3 to internal fifo of IP
#   define MasterI2C_Transfer_Data_To_Fifo()   Set_MASTER_CCI_MASTER_CCI_TRANSFER(0, 0, 1)    // fifo_wr_fill

    /// Error Status of MasterI2C \n
    /// Value: 1 => Error occured during transfer \n
    /// Value: 0 => No Error \n
#   define MasterI2C_HardwareError()           Get_MASTER_CCI_MASTER_CCI_STATUS_error()

/// Clear error in MasterI2C
/// Value: 1 => Clear error \n
#   define MasterI2C_SetErrorStatus(x) Set_MASTER_CCI_MASTER_CCI_STATUS(x)

/// Read 4 byte from Read Register Bank 0
#   define MasterI2C_Read_u32datastore0()  Get_MASTER_CCI_MASTER_CCI_READ_DATA0()

/// Write 4 byte from Write Register Bank 0
#   define MasterI2C_Write_u32datastore0(x)    Set_MASTER_CCI_MASTER_CCI_WRITE_DATA0_word(x)

/// Write 4 byte from Write Register Bank 1
#   define MasterI2C_Write_u32datastore1(x)    Set_MASTER_CCI_MASTER_CCI_WRITE_DATA1_word(x)

/// Write 4 byte from Write Register Bank 2
#   define MasterI2C_Write_u32datastore2(x)    Set_MASTER_CCI_MASTER_CCI_WRITE_DATA2_word(x)

/// Write 4 byte from Write Register Bank 3
#   define MasterI2C_Write_u32datastore3(x)    Set_MASTER_CCI_MASTER_CCI_WRITE_DATA3_word(x)

/// Program Fall time
#   define I2C_COUNT_FALL(x)   Set_MASTER_CCI_MASTER_CCI_TIMING1_time_fall(x)

/// Program Rise time
#   define I2C_COUNT_RISE(x)   Set_MASTER_CCI_MASTER_CCI_TIMING1_time_rise(x)

/// Program high count
#   define I2C_COUNT_HIGH(x)   Set_MASTER_CCI_MASTER_CCI_TIMING1_t_high(x)

/// Program buffer count
#   define I2C_COUNT_BUFFER(x) Set_MASTER_CCI_MASTER_CCI_TIMING1_t_buffer(x)

/// Program Hold count
#   define I2C_COUNT_HOLD_DATA(x)  Set_MASTER_CCI_MASTER_CCI_TIMING2_t_hd_dat(x)

/// Program setup count
#   define I2C_COUNT_SETUP_DATA(x) Set_MASTER_CCI_MASTER_CCI_TIMING2_t_su_dat(x)

/// Program hold start
#   define I2C_COUNT_HOLD_START(x) Set_MASTER_CCI_MASTER_CCI_TIMING3_t_hd_sta(x)

/// Program start setup
#   define I2C_COUNT_SETUP_START(x)    Set_MASTER_CCI_MASTER_CCI_TIMING3_t_su_sta(x)

/// Program stop setup
#   define I2C_COUNT_SETUP_STOP(x) Set_MASTER_CCI_MASTER_CCI_TIMING3_t_su_sto(x)

/// In case of error, The macro will return error no
#   define MasterI2C_GetErrorType()    Get_MASTER_CCI_MASTER_CCI_ERR_MGMT_error_mgmt_value()

/// Gets the base address of the Master i2c write data0 register
#   define MasterI2C_Get_MASTER_CCI_WRITE_DATA0_BASE_ADDRESS() \
        (( uint8_t * ) &(p_MASTER_CCI_IP->MASTER_CCI_WRITE_DATA0.word))

/// Macro to raise MasterI2Cs event notification when MasterI2C_access transaction is completed.
#   define MasterI2C_HostRequestComplete_Notify()  EventManager_HostToMasterI2CAccess_Notify()

/// Macro to raise MasterI2Cs event notification when MasterI2C_access transaction has failed.
#   define MasterI2C_TransactionFailed_Notify()    EventManager_MasterI2CAccessFailed_Notify()


/* TODO: <AG>: Currently Not supporting interrupt mode
*/
#   ifdef USE_TLM
#      define MasterI2C_BlockResourceGrabAttempts()
#      define MasterI2C_AllowResourceGrabAttempts()
#   else
#      define MasterI2C_BlockResourceGrabAttempts()
#      define MasterI2C_AllowResourceGrabAttempts()
#   endif

/* TODO : <AG> Interrupt is not clear
*/
#   if 0
#      define MasterI2C_DisableInterrupt()    SI_INT_EN = 0
#   else
#      define MasterI2C_DisableInterrupt()
#   endif
#endif // _MASTERI2C_IP_INTERFACE_H_

