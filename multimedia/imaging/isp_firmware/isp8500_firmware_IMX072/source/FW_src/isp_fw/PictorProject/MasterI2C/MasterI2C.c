/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \if INCLUDE_IN_HTML_ONLY
 \file  MasterI2C.c
 \brief Contains the necessary API for Communication with HOST and CCI hardware.
 \ingroup MasterI2C
 \endif
*/
#include "MasterI2C.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "PictorProject_MasterI2C_MasterI2CTraces.h"
#endif

MasterI2CClockControl_ts    g_MasterI2CClockControl =
{
    MASTER_I2C_DEFAULT_COUNT_FALL,
    MASTER_I2C_DEFAULT_COUNT_RISE,
    MASTER_I2C_DEFAULT_COUNT_HIGH,
    MASTER_I2C_DEFAULT_COUNT_BUFFER,
    MASTER_I2C_DEFAULT_COUNT_HOLD_DATA,
    MASTER_I2C_DEFAULT_COUNT_SETUP_DATA,
    MASTER_I2C_DEFAULT_COUNT_HOLD_START,
    MASTER_I2C_DEFAULT_COUNT_SETUP_START,
    MASTER_I2C_DEFAULT_COUNT_SETUP_STOP,
    MASTER_I2C_DEFAULT_MAX_NUMBER_MASTERI2C_GRAB_ATTEMPTS
};

// The structure is used in interrupt context, so it should be volatile
volatile MasterI2CStatus_ts          g_MasterI2CStatus =
{
    Resource_e_ResourceAvailable,
    Resource_e_ResourceAvailable,
    Flag_e_FALSE,
    0,
    0,
    0,
    MASTER_I2C_NO_OF_BYTES_FOR_HOST_TO_MASTERI2C_ACCESS
};

/**
 \fn void MasterI2C_ModeStaticInitialise(void)
 \brief Initialize Master I2C hardware registers
 \details Initialize Master I2C hardware registers and should be invoked
          whenever there is change MasterI2C configuration.
          The procedure must be invoked before BOOT command from host as
          it intializes the MasterI2C IP.
 \return void
 \ingroup MasterI2C
 \callgraph
 \callergraph
*/
void
MasterI2C_ModeStaticInitialise(void)
{
    // Enable MasterI2C IP: enable,soft_reset
    I2C_ENABLE(1,0);

    I2C_COUNT_FALL(g_MasterI2CClockControl.u8_CountFall);
    I2C_COUNT_RISE(g_MasterI2CClockControl.u8_CountRise);
    I2C_COUNT_HIGH(g_MasterI2CClockControl.u8_CountHigh);
    I2C_COUNT_BUFFER(g_MasterI2CClockControl.u8_CountBuffer);

    I2C_COUNT_HOLD_DATA(g_MasterI2CClockControl.u8_CountHoldData);
    I2C_COUNT_SETUP_DATA(g_MasterI2CClockControl.u8_CountSetupData);

    I2C_COUNT_HOLD_START(g_MasterI2CClockControl.u8_CountHoldStart);
    I2C_COUNT_SETUP_START(g_MasterI2CClockControl.u8_CountSetupStart);
    I2C_COUNT_SETUP_STOP(g_MasterI2CClockControl.u8_CountSetupStop);

    return;
}


/**
 \if INCLUDE_IN_HTML_ONLY
 \fn uint8_t MasterI2C_GrabResource (void)

 \brief This function is called to gain access to the
             MasterI2C channel. The caller must ensure that
             this function returns Flag_e_TRUE before performing
             a read or a write operation
 \details
 \return uint8_t
 Flag_e_TRUE:   Grap resource is successful
 Flag_e_FALSE;  Grap resource is unsuccessful
 \ingroup MasterI2C

 \callgraph
 \callergraph
 \endif
*/
uint8_t
MasterI2C_GrabResource(void)
{
    uint8_t e_Flag_Result;
    // block all resource grab attempts atomically
    // Disable all the interrupts
    MasterI2C_BlockResourceGrabAttempts();

    if (Resource_e_ResourceAvailable == g_MasterI2CStatus.e_Resource_Status)
    {
        g_MasterI2CStatus.e_Resource_Status = Resource_e_ResourceBusy;
        g_MasterI2CStatus.u8_NumberOfConsecutiveGrabFailures = 0;

        // allow grab attempts
        // Enable all the interrupts
        MasterI2C_AllowResourceGrabAttempts();

        e_Flag_Result = Flag_e_TRUE;
    }
    else
    {
        // allow resource grab attempts
        // Enable all the interrupts
        MasterI2C_AllowResourceGrabAttempts();

        g_MasterI2CStatus.u8_NumberOfConsecutiveGrabFailures++;

        if
        (
            g_MasterI2CStatus.u8_NumberOfConsecutiveGrabFailures >= g_MasterI2CClockControl.
                u8_MaximumNumberOfGrabAttempts
        )
        {
            // if the number of consecutive grab attempts exceeds the maximum number given by the control
            // page, we should force the resource to be available so that the next attempt is
            // successful...
            // Resource Status variable is directly assigned to Resource_Available
            g_MasterI2CStatus.e_Resource_Status = Resource_e_ResourceAvailable;

            g_MasterI2CStatus.u8_NumberOfConsecutiveGrabFailures = 0;
            g_MasterI2CStatus.u8_NumberOfForcedReleases++;

            // code should never come there as this leg will be hit only when there is some error in ip
            OstTraceInt0(TRACE_ERROR, "<ERROR>!!MasterI2C_GrabResource() - I2C grab resource failure!!");
            ASSERT_XP70();
        }


        e_Flag_Result = Flag_e_FALSE;
    }

    return (e_Flag_Result);
}


/**
 \if INCLUDE_IN_HTML_ONLY
 \fn void MasterI2C_FreeResource (void)
 \brief This function is called to free the MasterI2C
             channel
 \details
 \return void
 \ingroup MasterI2C
 \callgraph
 \callergraph
 \endif
*/
void
MasterI2C_FreeResource(void)
{
    // ensure that the resource flag is cleared as part of an
    // atomic operation by disabling all interrupts (effectively a MUTEX)
    MasterI2C_BlockResourceGrabAttempts();

    g_MasterI2CStatus.e_Resource_Status = Resource_e_ResourceAvailable;

    // enable the interrupts
    MasterI2C_AllowResourceGrabAttempts();

    return;
}


/**
 \if INCLUDE_IN_HTML_ONLY
 \fn uint8_t MasterI2C_ReadBytesHW ( uint16_t u16_DeviceID, uint16_t u16_RegisterAddress, uint8_t u8_NoOfBytes, uint8_t *ptru8_ReadData, uint8_t e_DeviceAddress_Type, uint8_t e_DeviceIndex_Type)
 \brief Read u8_NoOfBytes from MasterI2C slave syncronously
 \details Read u8_NoOfBytes from MasterI2C slave syncronously. The function just copy data from H/W registers. Endianess related issues are handled in upper layers
 \param u16_DeviceID        : 16 bit slave address, currently only 7 bit slave address supported
 \param u16_RegisterAddress : 16 bit index, (No support for 8 bit index)
 \param u8_NoOfBytes        : No of bytes read from slave
 \param *ptru8_ReadData     : Buffer to copy data
 \param e_DeviceAddress_Type: Device address type (10 bit (0) or 7 bit(1))
 \param e_DeviceIndex_Type  : Devide index type (16 bit(0) or 8 bit(1))
 \return uint8_t
 Flag_e_TRUE:   If the read is successful
 Flag_e_FALSE;  If the read is not successful due to error returned by MasterI2C block
 \ingroup MasterI2C
 \callgraph
 \callergraph
 \endif
*/

// The function will be called only iff resource is free
uint8_t
MasterI2C_ReadBytesHW(
uint16_t    u16_DeviceID,
uint16_t    u16_RegisterAddress,
uint8_t     u8_NoOfBytes,
uint8_t     *ptru8_ReadData,
uint8_t     e_DeviceAddress_Type,
uint8_t     e_DeviceIndex_Type)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    uint8_t     u8_Status = Flag_e_FALSE;
    uint8_t     u8_count = 0;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    MasterI2C_ResetTransactionError();

    // Supporting only 7 bit device address
    MasterI2C_ProgramDeviceID(u16_DeviceID);

    // Send device id : 0x01
    MasterI2C_Set_Device_ID_Valid();

    MasterI2C_ProgramIndex(u16_RegisterAddress);

    // TODO: tobe removed
    MasterI2C_DisableInterrupt();

    //    MasterI2C_Program_Read_RepeatedStart_NoOfBytes_16BitIndex_8BitDeviceID(u8_NoOfBytes);
    MasterI2C_Program_Read_RepeatedStart_NoOfBytes_Index_DeviceID(u8_NoOfBytes, e_DeviceAddress_Type, e_DeviceIndex_Type);

    // set hardware resource buzy
    MasterI2C_SetI2CTransactionBusy();

    MasterI2C_Start_Trasaction();

    // The Resource will be available only after i2c trasaction complete interrupt in MasterI2C_Isr
    while (MasterI2C_IsI2C_TransactionBusy())
        ;

    // if Hardware return error
    if (Flag_e_TRUE == MasterI2C_GetTransactionError())
    {
        // reusing u8_count for status
        // return error
        u8_Status = Flag_e_FALSE;
    }
    else
    {
        for (u8_count = 0; u8_count < u8_NoOfBytes; u8_count++)
        {
            ptru8_ReadData[u8_count] = *(( uint8_t * ) &MasterI2C_Read_u32datastore0() + u8_count);
        }


        u8_Status = Flag_e_TRUE;
    }


    // make i2c available for other modules
    MasterI2C_FreeResource();

    return (u8_Status);
}


/**
 \if INCLUDE_IN_HTML_ONLY
 \fn uint8_t MasterI2C_WriteBytesHW ( uint16_t u16_DeviceID, uint16_t u16_RegisterAddress, uint8_t u8_NoOfBytes, uint8_t *ptru8_ReadData, uint8_t e_DeviceAddress_Type,uint8_t e_DeviceIndex_Type, uint8_t e_SendType_Type)
 \brief Read u8_NoOfBytes from MasterI2C slave syncronously
 \details Read u8_NoOfBytes from MasterI2C slave syncronously. The function just copy data from H/W registers. Endianess related issues are hangled in upper layers
 \param u16_DeviceID        : 16 bit slave address, currently only 7 bit slave address supported
 \param u16_RegisterAddress : 16 bit index, (No support for 8 bit index)
 \param u8_NoOfBytes        : No of bytes read from slave
 \param *ptru8_ReadData     : Buffer to copy data
 \param e_DeviceAddress_Type: Device address type (10 bit (0) or 7 bit(1))
 \param e_DeviceIndex_Type  : Devide index type (16 bit(0) or 8 bit(1))
 \param e_SendType_Type     : e_SendType_Asynchronous(0), e_SendType_Synchronous(1)
 \return uint8_t
 Flag_e_TRUE:   If the read is successful
 Flag_e_FALSE;  If the read is not successful due to error returned by MasterI2C block
 \ingroup MasterI2C
 \callgraph
 \callergraph
 \endif
*/

// The function will be called only iff resource is free
uint8_t
MasterI2C_WriteBytesHW(
uint16_t    u16_DeviceID,
uint16_t    u16_RegisterAddress,
uint8_t     u8_NoOfBytes,
uint8_t     *ptru8_ReadData,
uint8_t     e_DeviceAddress_Type,
uint8_t     e_DeviceIndex_Type,
uint8_t     e_SendType_Type)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    uint8_t     u8_Status = Flag_e_FALSE;
    uint8_t     u8_count = 0;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    MasterI2C_ResetTransactionError();

    // Supporting only 7 bit device address
    MasterI2C_ProgramDeviceID(u16_DeviceID);

    // Send device id : 0x01
    MasterI2C_Set_Device_ID_Valid();

    // TODO: tobe removed
    MasterI2C_DisableInterrupt();

    //    MasterI2C_Program_Write_NoOfBytes_16BitIndex_8BitDeviceID(u8_NoOfBytes);
    MasterI2C_Program_Write_NoOfBytes_Index_DeviceID(u8_NoOfBytes, e_DeviceAddress_Type, e_DeviceIndex_Type);

    MasterI2C_ProgramIndex(u16_RegisterAddress);


    for (u8_count = 0; u8_count < u8_NoOfBytes; u8_count++)
    {
        // Directly using pointer to avoid complex logic
        // *(( uint8_t * ) &(p_Master_CCI_IP->MSTR_CCI_WRITE_DATA0.word) + u8_count) = ptru8_ReadData[u8_count] ;
    	*((uint8_t *)(MasterI2C_Get_MASTER_CCI_WRITE_DATA0_BASE_ADDRESS() + u8_count)) = ptru8_ReadData[u8_count];
    }

    // Bug fix due to alignment error
    // ptru32_ReadData was typed casted from ptru8_ReadData
    // If ptru8_ReadData is not 32 bit aligned, xp70 crashes
    // above code is added
#if 0
    MasterI2C_Write_u32datastore0(ptru32_ReadData[0]);

    MasterI2C_Write_u32datastore1(ptru32_ReadData[1]);

    MasterI2C_Write_u32datastore2(ptru32_ReadData[2]);

    MasterI2C_Write_u32datastore3(ptru32_ReadData[3]);
#endif

    // load fifo
    MasterI2C_Transfer_Data_To_Fifo();

    if (SendType_e_Asynchronous == e_SendType_Type)
    {
        MasterI2C_Start_Trasaction();

        // Async func will always return success
        u8_Status = Flag_e_TRUE;
    }
    else
    {
        // set hardware resource buzy
        MasterI2C_SetI2CTransactionBusy();

        MasterI2C_Start_Trasaction();

        // The Resource will be available only after i2c trasaction complete interrupt in MasterI2C_Isr
        while (MasterI2C_IsI2C_TransactionBusy())
            ;

        if (Flag_e_TRUE == MasterI2C_GetTransactionError())
        {
            // reusing u8_count for status
            // return error
            u8_Status = Flag_e_FALSE;
        }
        else
        {
            u8_Status = Flag_e_TRUE;
        }


        // make i2c available for other modules
        MasterI2C_FreeResource();
    }


    return (u8_Status);
}


/**
 \if INCLUDE_IN_HTML_ONLY
 \fn   void MasterI2C_Isr(void)
 \brief When Read / Write transaction is completed, MasterI2C block
        generate interrupt and MasterI2C_Isr is invoked. For Syncronous mode, two flags are set to buzy\n
        1. Resource flag and trasaction flag. Resouce flag signify driver context and its allocation to
        the calling entity. Trasaction flag will be buzy for syncrounous case when resource flag is already busy.
 \details
 \return void
 \ingroup MasterI2C
 \callgraph
 \callergraph
 \endif
*/
void
MasterI2C_Isr(void)
{
    if (MasterI2C_HardwareError())  //if status shows error
    {
        //  error has been asserted by master i2c
        MasterI2C_SetTransactionError(MasterI2C_GetErrorType());

        // clear the error status
        MasterI2C_SetErrorStatus(0x1);

        // Error bit is not auto clear, so reset the error bit
        MasterI2C_SetErrorStatus(0x0);

        // increment error count
        g_MasterI2CStatus.u8_NumberOfTransactionFailures++;

        OstTraceInt1(TRACE_ERROR, "<ERROR>!!MasterI2C_Isr() - Hardware !!ERROR!! in I2C: %d ",g_MasterI2CStatus.u8_NumberOfTransactionFailures);
    }


    // Make resource available only if it was busy. As syncrounous write will make h/w busy.\n
    // In case of asyncronous write only resource is busy and but transaction is free
    if ((MasterI2C_IsI2C_TransactionBusy()) && MasterI2C_IsI2C_ResourceBusy())
    {
        // Hardware has completed the transaction, Hardware resource is free
        MasterI2C_SetI2CTransactionAvailable();
    }// case of async write only
    else if ((MasterI2C_IsI2C_TransactionAvailable()) && (MasterI2C_IsI2C_ResourceBusy()))
    {
        // For Asyncronous Write free the Software resources, Hardware resource is free
        MasterI2C_FreeResource();
    }
    else
    {
        // Should never come here why interrupt fired when there is no transaction
        OstTraceInt0(TRACE_ERROR, "<ERROR>!!MasterI2C_Isr() - FALSE I2C interrupt!!");
        ASSERT_XP70();
    }


    return;
}

