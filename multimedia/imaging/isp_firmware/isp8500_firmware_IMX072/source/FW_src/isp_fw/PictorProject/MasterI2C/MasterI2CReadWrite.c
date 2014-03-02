/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \if INCLUDE_IN_HTML_ONLY
 \file MasterI2CReadWrite.c
 \brief Contains the necessary API for Communication with MasterI2C IP.
 \ingroup MasterI2C
 \endif
 */
#include "MasterI2C.h"

/**
 \fn uint8_t MasterI2C_ReadNBytes (uint16_t u16_DeviceID, uint16_t u16_Address, uint8_t *ptru8_Data, uint8_t u8_NumberOfBytes, uint8_t e_DeviceAddress_Type, uint8_t e_DeviceIndex_Type )
 \brief Read n no bytes from I2C device
 \details Read n no byte from the I2C device, User has to specify Device address, register address in the device and buffer to read in.

 \param u16_DeviceID          : I2C device address, The device address could be 10 bits NOTE: Currently only 7 bit device address is supported
 \param u16_Address           : 16 bit unsigned Register Index from where the read has to be performed
 \param ptru8_Data            : Pointer to a memory location where the read value would be stored
 \param u8_NumberOfBytes      : Number of bytes to be read
 \param e_DeviceAddress_Type  : Device address type (10 bit (0) or 7 bit(1))
 \param e_DeviceIndex_Type    : Devide index type (16 bit(0) or 8 bit(1))
 \return Flag_e_TRUE(Success), Flag_e_FALSE(Failure)
 \ingroup MasterI2C
 \callgraph
 \callergraph
*/
uint8_t
MasterI2C_ReadNBytes(
uint16_t    u16_DeviceID,
uint16_t    u16_Address,
uint8_t     *ptru8_Data,
uint8_t     u8_NumberOfBytes,
uint8_t     e_DeviceAddress_Type,
uint8_t     e_DeviceIndex_Type)
{
    // if the i2c hardware is free then grab it.
    if (MasterI2C_GrabResource())
    {
        // read single byte from location bIndex
        if
        (
            MasterI2C_ReadBytesHW(
                u16_DeviceID,
                u16_Address,
                u8_NumberOfBytes,
                ptru8_Data,
                e_DeviceAddress_Type,
                e_DeviceIndex_Type)
        )
        {
            return (Flag_e_TRUE);
        }
    }

    /// Raise event to signal host that I2C transaction has failed
    MasterI2C_TransactionFailed_Notify();


    //  else the i2c master resource has already been grabbed.
    return (Flag_e_FALSE);
}


/*##########################################   End READ      ##############################################################*/

/*#########################################################################################################################*/

/*##########################################  WRITE API'S    ##############################################################*/

/*#########################################################################################################################*/

/**
 \fn uint8_t MasterI2C_WriteNByte (uint16_t u16_DeviceID, uint16_t u16_Address, uint8_t *ptru8_Data, uint8_t u8_NumberOfBytes,uint8_t e_DeviceAddress_Type, uint8_t e_DeviceIndex_Type, uint8_t e_SendType_Type)
 \brief Write two bytes to I2C device
 \details Write two bytes to the I2C device, User has to specify Device address, register address in the device, data buffer write, Send Type and no of bytes
 \param u16_DeviceID : I2C device address, The device address could be 10 bits NOTE: Currently only 7 bit device address is supported
 \param u16_Address  : 16 bit unsigned Register Index from where the read has to be performed
 \param *ptru8_Data  : Pointer to data buffer
 \param u8_NumberOfBytes: No of bytes to write on I2C slave
 \param e_DeviceAddress_Type: Device address type (10 bit (0) or 7 bit(1))
 \param e_DeviceIndex_Type  : Devide index type (16 bit(0) or 8 bit(1))
 \param e_SendType_Type: e_SendType_Asynchronous(0), e_SendType_Synchronous(1)
 \return Flag_e_TRUE(Success), Flag_e_FALSE(Failure)
 \ingroup MasterI2C
 \callgraph
 \callergraph
*/
uint8_t
MasterI2C_WriteNByte(
uint16_t    u16_DeviceID,
uint16_t    u16_Address,
uint8_t     *ptru8_Data,
uint8_t     u8_NumberOfBytes,
uint8_t     e_DeviceAddress_Type,
uint8_t     e_DeviceIndex_Type,
uint8_t     e_SendType_Type)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    uint8_t u8_Status = Flag_e_FALSE;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    // Passed size should not be greater than the hardware fifo size
    if (u8_NumberOfBytes <= BYTES_IN_HW_FIFO_DEPTH)
    {
        if (MasterI2C_GrabResource())
        {
            u8_Status = MasterI2C_WriteBytesHW(
                u16_DeviceID,
                u16_Address,
                u8_NumberOfBytes,
                ptru8_Data,
                e_DeviceAddress_Type,
                e_DeviceIndex_Type,
                e_SendType_Type);

            if(Flag_e_FALSE == u8_Status)
            {
                /// Raise event to signal host that I2C transaction has failed
            	MasterI2C_TransactionFailed_Notify();
            }

        }//MasterI2C_GrabResource

    }//u8_NUmberOfBytes

    return (u8_Status);
}


/*##########################################   End WRITE     ##############################################################*/

