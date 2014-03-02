/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Services file of AB8500 Core module for Read/Write & Interrupt handling
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include "hcl_defs.h"
#include "services.h"
#include "gic.h"
#include "ab8500_core_irq.h"
#include "ab8500_core_services.h"

#ifdef __PRCM_HWI2C
#include "xp70_memmap.h"
#else
#include "ssp.h"
#endif


#include "gpio.h"
#include "gpio_services.h"


t_callback                  g_callback_ab8500_core = { 0, 0 };


/**********************************************************************************************/
/* NAME:    ab8500_set_default_config()                                                       */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is used to read from ab8500 registers.                           */
/*                                                                                            */
/* PARAMETERS:                                                                                */
/* IN :          None                                                                         */
/* OUT:          None                                                                         */
/*                                                                                            */
/* RETURN:    t_ser_ab8500_core_error :   AB8500 error code                                   */
/*                                                                                            */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant                                                                      */
/**********************************************************************************************/
PRIVATE t_ser_ab8500_core_error ab8500_set_default_config()
{
    t_ser_ab8500_core_error error_ab8500 = SER_AB8500_CORE_OK;
#ifndef __PRCM_HWI2C
    t_ssp_device_id ssp_device_id = SSP_DEVICE_ID_0;
    t_ssp_config config_ssp;
    t_ssp_error error_ssp;
    t_gpio_error gpio_error;

    
        
    /* set the configuration parameters */
    config_ssp.iface = SSP_INTERFACE_MOTOROLA_SPI;
    config_ssp.hierarchy = SSP_MASTER;
    config_ssp.endian = SSP_RX_MSB_TX_MSB;
    config_ssp.data_size = SSP_DATA_BITS_24;   
    config_ssp.txcom_mode = SSP_POLLING_MODE;
    config_ssp.rxcom_mode = SSP_POLLING_MODE;
    config_ssp.rx_lev_trig = SSP_RX_1_OR_MORE_ELEM;
    config_ssp.tx_lev_trig = SSP_TX_1_OR_MORE_EMPTY_LOC;
    config_ssp.clk_phase = SSP_CLK_RISING_EDGE;
    config_ssp.clk_pol = SSP_CLK_POL_IDLE_HIGH;
    config_ssp.clk_freq.cpsdvsr = 2;
    config_ssp.clk_freq.scr = 1;
    config_ssp.duplex = SSP_MICROWIRE_CHANNEL_FULL_DUPLEX; 
    
    /* SetConfiguration is invoked */
    PRINT("Initialization of the SSP configuration\n");
    /*coverity[uninit_use_in_call]*/
    error_ssp = SSP_SetConfiguration(ssp_device_id, &config_ssp);
    if(SSP_OK == error_ssp)
    {
        PRINT("Initialization of the SSP configuration PASSED");
    }
       else
    {
        PRINT("Initialization of the SSP configuration FAILED!!!");
        error_ab8500 = SER_AB8500_CORE_TRANSACTION_ON_SPI_FAILED;
    }

//    gpio_error = GPIO_EnableAltFunction(GPIO_ALT_SSP0);
    if (GPIO_OK != gpio_error)
    {
        return(SER_AB8500_CORE_ERROR);
    }


#endif 
    return(error_ab8500);
}

/**********************************************************************************************/
/* NAME:void ser_ab8500_core_FlushRecieveFifo()                                               */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is used to Flush the SSP FIFO                                    */
/*                                                                                            */
/* PARAMETERS:                                                                                */
/* IN :         None                                                                          */
/* OUT:         None                                                                          */
/*                                                                                            */
/* RETURN:     void                                                                           */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant                                                                      */
/**********************************************************************************************/
#ifndef __PRCM_HWI2C
PRIVATE void ser_ab8500_core_FlushRecieveFifo(void)
{
    t_uint32 fifo_status = 0x00, rx_element=0x00;
    t_ssp_error error_status=SSP_OK;

    do
    {
        SSP_GetFIFOStatus(SSP_DEVICE_ID_0, &fifo_status);
        
    if (fifo_status & SER_AB8500_CORE_RX_FIFONOTEMPTY)
        {
            error_status = SSP_GetData(SSP_DEVICE_ID_0, &rx_element);
        }
    } while (fifo_status & SER_AB8500_CORE_RX_FIFONOTEMPTY);

    /*coverity[self_assign]*/
    error_status=error_status;
    return;
}
#endif

/**********************************************************************************************/
/* NAME:     SER_AB8500_CORE_WriteThruHWI2C()                                                 */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is used to write onto ab8500 registers through HWI2C             */
/*                                                                                            */
/* PARAMETERS:                                                                                */
/* IN :          t_uint8  block_add         The block Address where data has to be written,   */
/*               t_uint8  register_offset   The offset within the block is specified,         */
/*               t_uint32 count             The number of bytes that have to be written,      */
/*               t_uint8  *p_data           The value to be written                           */
/* OUT:         none                                                                          */
/*                                                                                            */
/* RETURN:        t_ser_ab8500_core_error: AB8500 error code                                  */
/*                                                                                            */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant                                                                      */
/**********************************************************************************************/
#ifdef __PRCM_HWI2C
PRIVATE t_ser_ab8500_core_error SER_AB8500_CORE_WriteThruHWI2C
(
IN t_uint8 block_add,
IN t_uint8 register_offset,
IN t_uint8 count,
IN t_uint8 *p_data
)
{
    t_uint32 index=0;
    volatile struct req_mb_5 temp_reqmb5 = {0};
    volatile struct ack_mb_5 ackmb5;
        //volatile t_ReqMb5 temp_reqmb5;
    //volatile t_AckMb5 ackmb5;  
    //t_uint8 status = 0xFF;  

    while(index<count)
    {    
        //temp_reqmb5.HwGCEn = 0;
        //temp_reqmb5.StopEn = 1;
        temp_reqmb5.reg_addr = register_offset;
        temp_reqmb5.slave_addr = block_add;
    	temp_reqmb5.data_len = 1; /*length can take only upto 4 bytes */
        //temp_reqmb5.un.I2CopType = I2CWrite;
        temp_reqmb5.data[0] = *p_data;
		(*(volatile t_uint8 *)0x80168FED) = 0x1;

        /* Wait till I2C transactions are over (Bit5) */
        if ( (*(volatile t_uint32 *)SER_AB8500_CORE_PRCM_MBOX_CPU_VAL) & SER_AB8500_CORE_HWI2C_BIT )
        {
            /* Clear the Interrupt status generated (prcm_arm_it1_val[5])*/
            *(volatile t_uint32 *)SER_AB8500_CORE_PRCM_ARM_IT1_CLR |= SER_AB8500_CORE_HWI2C_BIT;
        }
            
        /* Now write to mailbox and trigger IT */
        //*(volatile struct req_mb_5 *)SER_AB8500_CORE_REQMB5 = RMB5H_I2C_WRITE | temp_reqmb5;
        *(volatile struct req_mb_5 *)SER_AB8500_CORE_REQMB5 = temp_reqmb5;
        *(volatile t_uint32 *)SER_AB8500_CORE_PRCM_MBOX_CPU_SET |= SER_AB8500_CORE_HWI2C_BIT;
        
        /* Now wait for Interrupt */
        while ( !((*(volatile t_uint32 *)SER_AB8500_CORE_PRCM_MBOX_CPU_VAL) & SER_AB8500_CORE_HWI2C_BIT )); 
        
        /* Clear the Interrupt status generated (prcm_arm_it1_val[5])*/
        *(volatile t_uint32 *)SER_AB8500_CORE_PRCM_ARM_IT1_CLR |= SER_AB8500_CORE_HWI2C_BIT; 

       ackmb5 = *(volatile struct ack_mb_5 *)SER_AB8500_CORE_ACKMB5;

  		//status =(*(volatile t_uint8 *)0x80168FF5);
  
       if(RC_SUCCESS != (t_uint8)(ackmb5.return_code))
        {
            return SER_AB8500_CORE_HWI2C_OPERATION_FAILED;
        }

        p_data++;
        register_offset++;
        index++;
    }
    return SER_AB8500_CORE_OK;
}
#endif





/**********************************************************************************************/
/* NAME:     SER_AB8500_CORE_Write()                                                          */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is used to write onto ab8500 registrs.                           */
/*                                                                                            */
/* PARAMETERS:                                                                                */
/* IN :          t_uint8  block_add         The block Address where data has to be written,   */
/*               t_uint8  register_offset   The offset within the block is specified,         */
/*               t_uint32 count             The number of bytes that have to be written,      */
/*               t_uint8  *p_data           The value to be written                           */
/* OUT:         none                                                                          */
/*                                                                                            */
/* RETURN:        t_ser_ab8500_core_error: AB8500 error code                                  */
/*                                                                                            */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant                                                                      */
/**********************************************************************************************/
PUBLIC t_ser_ab8500_core_error SER_AB8500_CORE_Write
(
IN t_uint8 block_add,
IN t_uint8 register_offset,
IN t_uint8 count,
IN t_uint8 *p_data
)
{
    t_ser_ab8500_core_error error_ab8500 = SER_AB8500_CORE_OK;
#ifndef __PRCM_HWI2C
    t_uint32 delay = 0;
    t_uint32 data_packet = 0;
    t_ssp_error error_ssp = SSP_OK;
    t_uint32 index;
    t_ssp_device_id ssp_device_id = SSP_DEVICE_ID_0;
    error_ssp = SSP_Enable(ssp_device_id, SSP_DISABLE_RX_TX);
    
    if(SSP_OK != error_ssp)
    {
        error_ab8500 = SER_AB8500_CORE_TRANSACTION_ON_SPI_FAILED;
        return(error_ab8500);
    }
    
    /* flush receive fifo */
    ser_ab8500_core_FlushRecieveFifo();

    
    /* Fill the Tx FIFO */
    index = 0;       
    if(count <= SER_AB8500_CORE_SSPFIFO)
    {
        while ((index < count) && (SSP_OK == error_ssp))
        {        
            /* Data transmitted is write command and block address */
            block_add = (t_uint8)((~MASK_BIT5) & block_add);
            data_packet = ((block_add << SHIFT_BYTE1) | register_offset);
            
            /* Tx FIFO is filled before enabling SSP */
            data_packet = ((data_packet << SHIFT_AB8500_DATA) | *p_data);
        
            error_ssp = SSP_SetData(ssp_device_id,data_packet);
            if(SSP_OK != error_ssp)
            {
                PRINT("SSP0 Error %d", error_ssp);
                error_ab8500 = SER_AB8500_CORE_TRANSACTION_ON_SPI_FAILED;
                return(error_ab8500);
            }

            p_data++;
            register_offset++;
            index++;
            for(delay=0; delay<AB8500_CORE_MAX_SSPWRITE_DELAY; delay++);   
        }

        /* SSP is enabled */
        error_ssp = SSP_Enable(ssp_device_id, SSP_ENABLE_RX_TX);
        
        if(SSP_OK != error_ssp)
        {
            error_ab8500 = SER_AB8500_CORE_TRANSACTION_ON_SPI_FAILED;
            PRINT("SSP0 Error %d", error_ssp);
            return(error_ab8500);
        }
    }
    
    else
    {
        while ((index < SER_AB8500_CORE_SSPFIFO) && (SSP_OK == error_ssp))
        {        
            /* Data transmitted is write command and block address */
            block_add = (t_uint8)((~MASK_BIT5) & block_add);
            data_packet = ((block_add << SHIFT_BYTE1) | register_offset);
            
            /* Tx FIFO is filled before enabling SSP */
            data_packet = ((data_packet << SHIFT_AB8500_DATA) | *p_data);
            error_ssp = SSP_SetData(ssp_device_id,data_packet);
            
            p_data++;
            register_offset++;
            index++;                
        }

        count = count - SER_AB8500_CORE_SSPFIFO;

        if(SSP_OK != error_ssp)
        {
            PRINT("SSP0 Error %d", error_ssp);
            error_ab8500 = SER_AB8500_CORE_TRANSACTION_ON_SPI_FAILED;
            return(error_ab8500);
        }
        
        /* SSP is enabled */
        error_ssp = SSP_Enable(ssp_device_id, SSP_ENABLE_RX_TX);
        
        if(SSP_OK != error_ssp)
        {
            error_ab8500 = SER_AB8500_CORE_TRANSACTION_ON_SPI_FAILED;
            PRINT("SSP0 Error %d", error_ssp);
            return(error_ab8500);
        }
        
        while(count)
        {
            /* Data transmitted is write command and block address */
            block_add = (t_uint8)((~MASK_BIT5) & block_add);
            data_packet = ((block_add << SHIFT_BYTE1) | register_offset);
            
             /* Tx FIFO is filled before enabling SSP */
             data_packet = ((data_packet << SHIFT_AB8500_DATA) | *p_data);
             error_ssp = SSP_SetData(ssp_device_id,data_packet);
             
             p_data++;
             register_offset++;
             count--;
        }
      
        /* SSP is enabled */
        error_ssp = SSP_Enable(ssp_device_id, SSP_ENABLE_RX_TX);
        
        if(SSP_OK != error_ssp)
        {
            error_ab8500 = SER_AB8500_CORE_TRANSACTION_ON_SPI_FAILED;
            PRINT("SSP0 Error %d", error_ssp);
            return(error_ab8500);
        }
    } 

    /* flush receive fifo */
    ser_ab8500_core_FlushRecieveFifo();
       
    for(delay=0; delay<AB8500_CORE_MAX_SSPWRITE_DELAY; delay++);   
    
    if(SER_AB8500_CORE_OK == error_ab8500)
    {
        error_ssp = SSP_Enable(ssp_device_id, SSP_DISABLE_RX_TX);
        
        if(SSP_OK != error_ssp)
        {
            error_ab8500 = SER_AB8500_CORE_TRANSACTION_ON_SPI_FAILED;
            return(error_ab8500);
        }
    }

#else             
    error_ab8500 = SER_AB8500_CORE_WriteThruHWI2C(block_add, register_offset, count, p_data); 
#endif

    return(error_ab8500);
    
}


/**********************************************************************************************/
/* NAME:void ser_ab8500_ReadSingleFifoTransaction()                                           */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is used to read from SSP less than 1 FIFO full data              */
/*                                                                                            */
/* PARAMETERS:                                                                                */
/* IN :          t_uint8  block_add         The block Address from where data has to be read, */
/*               t_uint8  register_offset   The offset within the block is specified,         */
/*               t_uint32 count             The number of bytes that have to be read,         */
/*               t_uint8  *p_data_in         Dummy Data                                       */
/* OUT:          t_uint8  *p_data_out        Data read from the specified address             */
/*                                                                                            */
/* RETURN:         t_ser_ab8500_core_error                                                    */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant                                                                      */
/**********************************************************************************************/
#ifndef __PRCM_HWI2C
PRIVATE t_ser_ab8500_core_error ser_ab8500_ReadSingleFifoTransaction
(
IN t_uint8 block_add,
IN t_uint8 register_offset,
IN t_uint32 count,
IN t_uint8 *p_data_in,
OUT t_uint8 *p_data_out
)
{
    t_uint32 index=0x00, rx_element=0x0;
    t_uint32 data_packet=0x00, delay=0x00;
    t_ssp_error error_ssp = SSP_OK;

    while ((index < count) && (SSP_OK == error_ssp))
    {
        /* Data transmitted is write command and block address */
        block_add =(t_uint8) (MASK_BIT5 | block_add);
        data_packet = ((block_add << SHIFT_BYTE1) | register_offset);
         
        /* Tx FIFO is filled before enabling SSP */ 
        data_packet = ((data_packet << SHIFT_AB8500_DATA) | *p_data_in);

        /* Dummy Data */
        error_ssp = SSP_SetData(SSP_DEVICE_ID_0,data_packet);
         
        p_data_in++;
        register_offset++;
        index++;
        for(delay=0; delay<AB8500_CORE_MAX_SSPWRITE_DELAY; delay++);   
    }
    
    
    for(delay=0; delay<AB8500_CORE_MAX_SSPWRITE_DELAY; delay++);   
         
    /* SSP is enabled */
    if(SSP_OK != SSP_Enable(SSP_DEVICE_ID_0, SSP_ENABLE_RX_TX))
    {
        PRINT("SSP0 Error SER_AB8500_CORE_TRANSACTION_ON_SPI_FAILED");
        return(SER_AB8500_CORE_TRANSACTION_ON_SPI_FAILED);
    }
  
    for(delay=0; delay<AB8500_CORE_MAX_SSPWRITE_DELAY; delay++);   
        
    /* Data recieved */
    index = 0;
    while ((index < count) && (SSP_OK == error_ssp))
    {
        error_ssp = SSP_GetData(SSP_DEVICE_ID_0,&rx_element);
        *p_data_out = (rx_element & MASK_ALL8);
        p_data_out++;
        index++; 
         
        if(SSP_OK != error_ssp)
        {
             PRINT("SSP0 Error %d", error_ssp);
             return(SER_AB8500_CORE_TRANSACTION_ON_SPI_FAILED);
        }
        for(delay=0; delay<AB8500_CORE_MAX_SSPWRITE_DELAY; delay++);   
    }
    return(SER_AB8500_CORE_OK);
}
#else 

/**********************************************************************************************/
/* NAME:                             SER_AB8500_CORE_ReadThruHWI2C()                          */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is used to read from ab8500 registers through HWI2C.             */
/*                                                                                            */
/* PARAMETERS:                                                                                */
/* IN :          t_uint8  block_add         The block Address from where data has to be read, */
/*               t_uint8  register_offset   The offset within the block is specified,         */
/*               t_uint32 count             The number of bytes that have to be read,         */
/*               t_uint8  *p_data_in         Dummy Data                                       */
/* OUT:          t_uint8  *p_data_out        Data read from the specified address             */
/*                                                                                            */
/* RETURN:        t_ser_ab8500_core_error :                        AB8500 error code          */
/*                                                                                            */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant                                                                      */
/**********************************************************************************************/
PRIVATE t_ser_ab8500_core_error SER_AB8500_CORE_ReadThruHWI2C
(
IN t_uint8 block_add,
IN t_uint8 register_offset,
IN t_uint8 count,
IN t_uint8 *p_data_in,
OUT t_uint8 *p_data_out
)
{
    volatile struct req_mb_5 temp_reqmb5;
    volatile struct ack_mb_5 ackmb5;    
    t_uint32 index = 0x00;
    //t_uint8 status= 0xF;
    
    while(index<count)
    {
        //temp_reqmb5.un.HwGCEn = 0;
      //  temp_reqmb5.un.StopEn = 1; 
        temp_reqmb5.reg_addr = register_offset;
        temp_reqmb5.slave_addr = block_add;
        temp_reqmb5.data_len = 1;	/*Data Lenghth can't take a value more than 4 bytes */
       // temp_reqmb5.un.I2CopType = I2CRead;
        temp_reqmb5.data[0] = *p_data_in;
        
        	(*(volatile t_uint8 *)0x80168FED) = 2;
        
        /* Wait till I2C transactions are over (Bit5) */
        if ( (*(volatile t_uint32 *)SER_AB8500_CORE_PRCM_MBOX_CPU_VAL) & SER_AB8500_CORE_HWI2C_BIT )
        {
            /* Clear the Interrupt status generated (prcm_arm_it1_val[5])*/
            *(volatile t_uint32 *)SER_AB8500_CORE_PRCM_ARM_IT1_CLR |= SER_AB8500_CORE_HWI2C_BIT;
        }
        
        /* Now write to mailbox and trigger IT */
       // *(volatile struct ack_mb_5 *)SER_AB8500_CORE_REQMB5 = RMB5H_I2C_READ | temp_reqmb5;
       *(volatile struct req_mb_5 *)SER_AB8500_CORE_REQMB5 =  temp_reqmb5;
        *(volatile t_uint32 *)SER_AB8500_CORE_PRCM_MBOX_CPU_SET |= SER_AB8500_CORE_HWI2C_BIT;
        
        
        /* Now wait for Interrupt */
        while (!( (*(volatile t_uint32 *)SER_AB8500_CORE_PRCM_MBOX_CPU_VAL) & SER_AB8500_CORE_HWI2C_BIT )); 
        
        ackmb5 = *(volatile struct ack_mb_5 *)SER_AB8500_CORE_ACKMB5;
        
        //status = 	(*(volatile t_uint8 *)0x80168FF5);

        /* Clear the Interrupt status generated (prcm_arm_it1_val[5])*/
        *(volatile t_uint32 *)SER_AB8500_CORE_PRCM_ARM_IT1_CLR |= SER_AB8500_CORE_HWI2C_BIT; 

        *p_data_out  = (t_uint8)ackmb5.data[0] & MASK_BYTE;
    
        if(RC_SUCCESS != (t_uint8)(ackmb5.return_code))
        {
            return SER_AB8500_CORE_HWI2C_OPERATION_FAILED;
        }

        p_data_in++;
        p_data_out++;
        register_offset++;
        index++;  
    }

    return SER_AB8500_CORE_OK;

}
#endif

/**********************************************************************************************/
/* NAME:                             SER_AB8500_CORE_Read()                                   */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is used to read from ab8500 registers.                           */
/*                                                                                            */
/* PARAMETERS:                                                                                */
/* IN :          t_uint8  block_add         The block Address from where data has to be read, */
/*               t_uint8  register_offset   The offset within the block is specified,         */
/*               t_uint32 count             The number of bytes that have to be read,         */
/*               t_uint8  *p_data_in         Dummy Data                                       */
/* OUT:          t_uint8  *p_data_out        Data read from the specified address             */
/*                                                                                            */
/* RETURN:        t_ser_ab8500_core_error :                        AB8500 error code          */
/*                                                                                            */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant                                                                      */
/**********************************************************************************************/
PUBLIC t_ser_ab8500_core_error SER_AB8500_CORE_Read
(
IN t_uint8 block_add,
IN t_uint8 register_offset,
IN t_uint8 count,
IN t_uint8 *p_data_in,
OUT t_uint8 *p_data_out
)
{
    t_ser_ab8500_core_error error_ab8500 = SER_AB8500_CORE_OK;
#ifndef __PRCM_HWI2C
    t_ssp_error error_ssp = SSP_OK;
    t_ssp_device_id ssp_device_id = SSP_DEVICE_ID_0;
    t_uint32 rx_element,index = 0x00;
    t_uint32 data_packet=0x00;
    
    if(SSP_OK != SSP_Enable(ssp_device_id, SSP_DISABLE_RX_TX))
    {
        return(SER_AB8500_CORE_TRANSACTION_ON_SPI_FAILED);
    }
    
    /* flush receive fifo */
    ser_ab8500_core_FlushRecieveFifo();

    if(count <= SER_AB8500_CORE_SSPFIFO)
    {
        /* Perform Dummy read for first 0x00 data read with SSP */
        error_ab8500 = ser_ab8500_ReadSingleFifoTransaction(0x01, 0x00, 1, p_data_in, p_data_out);
        if(SER_AB8500_CORE_OK != error_ab8500)
        {
             return(error_ab8500);   
        } 

        /* Now perform proper read */
        error_ab8500 = ser_ab8500_ReadSingleFifoTransaction(block_add, register_offset, count, p_data_in, p_data_out);
        if(SER_AB8500_CORE_OK != error_ab8500)
        {
             return(error_ab8500);   
        }
    }
    else
    {
        /* Perform Dummy read for first 0x00 data read with SSP */
        error_ab8500 = ser_ab8500_ReadSingleFifoTransaction(0x01, 0x00, 1, p_data_in, p_data_out);
        if(SER_AB8500_CORE_OK != error_ab8500)
        {
             return(error_ab8500);   
        } 
     
        /* Now perform proper read */
        while ((index < SER_AB8500_CORE_SSPFIFO) && (SSP_OK == error_ssp))
        {        
            /* Data transmitted is write command and block address */
            block_add = (t_uint8)((MASK_BIT5) & block_add);
            data_packet = ((block_add << SHIFT_BYTE1) | register_offset);
            
            /* Tx FIFO is filled before enabling SSP */
            data_packet = ((data_packet << SHIFT_AB8500_DATA) | *p_data_in);
            error_ssp = SSP_SetData(ssp_device_id,data_packet);
            
            p_data_in++;
            register_offset++;
            index++;                
        }
        
        count = count - SER_AB8500_CORE_SSPFIFO;

        if(SSP_OK != error_ssp)
        {
            PRINT("SSP0 Error %d", error_ssp);
            return(SER_AB8500_CORE_TRANSACTION_ON_SPI_FAILED);
        }
        
        /* SSP is enabled */
        if(SSP_OK != SSP_Enable(ssp_device_id, SSP_ENABLE_RX_TX))
        {
            PRINT("SSP0 Error SER_AB8500_CORE_TRANSACTION_ON_SPI_FAILED");
            return(SER_AB8500_CORE_TRANSACTION_ON_SPI_FAILED);
        }

        /* Data recieved */
        index = 0;
        while (index < SER_AB8500_CORE_SSPFIFO)
        {
            if(SSP_OK != SSP_GetData(ssp_device_id,&rx_element))
            {
                 PRINT("SSP0 Error SER_AB8500_CORE_TRANSACTION_ON_SPI_FAILED");
                 return(SER_AB8500_CORE_TRANSACTION_ON_SPI_FAILED);
            }

            *p_data_out = (rx_element & MASK_ALL8);

            p_data_out++;
            index++; 
        }
        
        while(count)
        {
            /* Data transmitted is write command and block address */
            block_add = (t_uint8)((MASK_BIT5) & block_add);
            data_packet = ((block_add << SHIFT_BYTE1) | register_offset);
            
            /* Tx FIFO is filled before enabling SSP */
            data_packet = ((data_packet << SHIFT_AB8500_DATA) | *p_data_in);
            if(SSP_OK != SSP_SetData(ssp_device_id,data_packet))
            {
                 PRINT("SSP0 Error SER_AB8500_CORE_TRANSACTION_ON_SPI_FAILED");
                 return(SER_AB8500_CORE_TRANSACTION_ON_SPI_FAILED);
            }

            p_data_in++;
            count--;
            if(SSP_OK != SSP_GetData(ssp_device_id,&rx_element))
            {
                 PRINT("SSP0 Error SER_AB8500_CORE_TRANSACTION_ON_SPI_FAILED");
                 return(SER_AB8500_CORE_TRANSACTION_ON_SPI_FAILED);
            }

            *p_data_out = (rx_element & MASK_ALL8);
            p_data_out++;
            register_offset++;
        }
    } 
     
    if(SSP_OK != SSP_Enable(ssp_device_id, SSP_DISABLE_RX_TX))
    {
        return(SER_AB8500_CORE_TRANSACTION_ON_SPI_FAILED);
    }
    
#else
    error_ab8500 = SER_AB8500_CORE_ReadThruHWI2C(block_add, register_offset, count, p_data_in, p_data_out); 
#endif 
    return(error_ab8500); 
    
}

/************************************************************************************/
/* NAME:SER_AB8500_CORE_RegisterCallback                                            */
/*----------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine binds an call back for extended interrupt processing   */
/*                                                                                  */
/* PARAMETERS:  t_callback_fct fct : Address of the call back                       */
/*                                                                                  */
/* RETURN: void                                                                     */
/*                                                                                  */
/*----------------------------------------------------------------------------------*/
/* REENTRANCY: Non Reentrant                                                        */

/************************************************************************************/
PUBLIC void SER_AB8500_CORE_RegisterCallback(t_callback_fct fct, void *param)
{
    g_callback_ab8500_core.fct = fct;
    g_callback_ab8500_core.param = param;
}

/****************************************************************************/
/* NAME:    SER_AB8500_CORE_InterruptHandler                                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This is interrupt service routine for AB8500 INTn Interrupts*/
/*                                                                          */
/*                                                                          */
/* PARAMETERS:         t_uint32 irq                                         */
/* RETURN: None                                                             */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Non-Reentrant                                                */
/****************************************************************************/
void SER_AB8500_CORE_InterruptHandler(t_uint32 irq) 
{
    t_uint8 interrupt_source[22];
    t_uint8 dummy_source[22];
    t_ser_ab8500_core_param ab8500_param;
    t_gic_error gic_error;
    t_uint32 i;

    gic_error = GIC_DisableItLine(GIC_IRQ_N_LINE);
    if (gic_error != GIC_OK)
    {
        PRINT("GIC Disabling Genepi Line error - %d\n", gic_error);
        return;
    }

    for(i=0;i<22;i++)
    {
        interrupt_source[i]=0x00;
    }
    
    /*coverity[uninit_use_in_call]*/
    if(SER_AB8500_CORE_OK !=SER_AB8500_CORE_Read(AB8500_CORE_INTERRUPT_BLOCK, AB8500_CORE_IT_LATCH_1_REGISTER, 22, dummy_source, interrupt_source))
    {
        PRINT("SER_AB8500_CORE_Read error - \n");
        return;
    }	    
     
    if (g_callback_ab8500_core.fct != 0)
    {
            
        if(SER_AB8500_CORE_TEST_BIT(interrupt_source[0],AB8500_CORE_IT_USBSUS_DETECT))
        {
             ab8500_param.irq_src_register = AB8500_CORE_IT_LATCH_9_REGISTER;
             ab8500_param.irq_src = interrupt_source[0];
             g_callback_ab8500_core.fct(g_callback_ab8500_core.param, &ab8500_param);
        }
     
        if(SER_AB8500_CORE_TEST_BIT(interrupt_source[0],AB8500_CORE_IT_USBCHARGER_DETECT))
        {
             ab8500_param.irq_src_register = AB8500_CORE_IT_LATCH_9_REGISTER;
             ab8500_param.irq_src = interrupt_source[0];
             g_callback_ab8500_core.fct(g_callback_ab8500_core.param, &ab8500_param);
        }
     
        if(SER_AB8500_CORE_TEST_BIT(interrupt_source[1],AB8500_CORE_IT_VBUS_DETECTR))
        {
             ab8500_param.irq_src_register = AB8500_CORE_IT_LATCH_18_REGISTER;
             ab8500_param.irq_src = interrupt_source[1];
             g_callback_ab8500_core.fct(g_callback_ab8500_core.param, &ab8500_param);
        }
    
        if(SER_AB8500_CORE_TEST_BIT(interrupt_source[1],AB8500_CORE_IT_VBUS_DETECTF))
        {
             ab8500_param.irq_src_register = AB8500_CORE_IT_LATCH_18_REGISTER;
             ab8500_param.irq_src = interrupt_source[1];
             g_callback_ab8500_core.fct(g_callback_ab8500_core.param, &ab8500_param);
        }
          
    }

    gic_error = GIC_EnableItLine(GIC_IRQ_N_LINE);
    if (gic_error != GIC_OK)
    {
        PRINT("GIC Enabling AB8500 Line error - %d\n", gic_error);
    }
    return; 
}

/*******************************************************************************/
/* NAME:    SER_AB8500_CORE_InterruptHandler                                   */
/*-----------------------------------------------------------------------------*/
/* DESCRIPTION: This is interrupt service routine for PRCMU Mailbox Interrupts */
/*                                                                             */
/*                                                                             */
/* PARAMETERS:         t_uint32 irq                                            */
/* RETURN: None                                                                */
/*                                                                             */
/*-----------------------------------------------------------------------------*/
/* REENTRANCY: Non-Reentrant                                                   */
/*******************************************************************************/
PUBLIC void SER_AB8500_CORE_PRCMUInterruptHandler(t_uint32 irq)
{
    t_gic_error gic_error;
    
    gic_error = GIC_DisableItLine(GIC_PRCMU_NON_SECURE_LINE);
    if (gic_error != GIC_OK)
    {
        PRINT("GIC Disabling PRCMU Line error - %d\n", gic_error);
    }
    
    /* Clear the Interrupt status generated */
    *(volatile t_uint32 *)SER_AB8500_CORE_PRCM_ARM_IT1_CLR |= SER_AB8500_CORE_HWI2C_BIT;
    
    gic_error = GIC_EnableItLine(GIC_PRCMU_NON_SECURE_LINE);
    if (gic_error != GIC_OK)
    {
        PRINT("GIC Enabling PRCMU Line error - %d\n", gic_error);
    }
    
}

/**********************************************************************************************/
/* NAME:void SER_AB8500_CORE_Init()                                                           */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is used to initialize ab8500                                     */
/*                                                                                            */
/* PARAMETERS:                                                                                */
/* IN :          None                                                                         */
/* OUT:          None                                                                         */
/*                                                                                            */
/* RETURN:         Void                                                                       */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant                                                                      */
/**********************************************************************************************/
void SER_AB8500_CORE_Init()
{
    t_gic_error gic_error;
    t_uint32 old_datum,i;
    t_ser_ab8500_core_error ab8500_error;
    t_uint8 data_out[24], data_in[24];
    t_gic_config_cntrl  gic_config;

    (gic_config.prio_level)  = GIC_PR_LEVEL_0;
    (gic_config.it_sec) 	 = GIC_IT_NON_SEC;
    (gic_config.it_config)   = GIC_IT_EDGE_SENSITIVE;
    (gic_config.cpu_num)  	 = GIC_CPU_CORE_0;
    

    gic_error = GIC_DisableItLine(GIC_IRQ_N_LINE);
    if (gic_error != GIC_OK)
    {
        PRINT("GIC Disabling AB8500 Line error - %d\n", gic_error);
    }
    
    gic_error = GIC_DisableItLine(GIC_PRCMU_NON_SECURE_LINE);
    if (gic_error != GIC_OK)
    {
        PRINT("GIC Disabling PRCMU Line error - %d\n", gic_error);
    }
    
    gic_error = GIC_ConfigureIrqLine(GIC_IRQ_N_LINE, (t_gic_func_ptr) SER_AB8500_CORE_InterruptHandler, &gic_config);
    if (gic_error != GIC_OK)
    {
        PRINT("GIC Interrupt Handler Binding error - %d\n", gic_error);
    }
    
    gic_error = GIC_ChangeDatum(GIC_PRCMU_NON_SECURE_LINE, (t_gic_func_ptr) SER_AB8500_CORE_PRCMUInterruptHandler, (t_gic_func_ptr *)&old_datum);
    if (gic_error != GIC_OK)
    {
        PRINT("GIC Interrupt Handler Binding error - %d\n", gic_error);
    }

    for(i=0;i<0x17;i++)
        data_out[i]=0xFF; 

    /* Disable all masks */
    if(SER_AB8500_CORE_OK != SER_AB8500_CORE_Write(AB8500_CORE_INTERRUPT_BLOCK, AB8500_CORE_IT_MASK_1_REGISTER, 22,  data_out))
    {
        PRINT("SER_AB8500_CORE_Write(AB8500_CORE_INTERRUPT_BLOCK, AB8500_CORE_IT_MASK_1_REGISTER, 10,  data_out) FAILED!!!.");
        return;
    }

      /* Read to clear all latches */
    /*coverity[uninit_use_in_call]*/
    if(SER_AB8500_CORE_OK != SER_AB8500_CORE_Read(AB8500_CORE_INTERRUPT_BLOCK, AB8500_CORE_IT_LATCH_1_REGISTER, 22,  data_in, data_out))
    {
        PRINT("SER_AB8500_CORE_Read(AB8500_CORE_INTERRUPT_BLOCK, AB8500_CORE_IT_LATCH_1_REGISTER, 24,  data_in, data_out) FAILED!!!.");
        return;
    } 


#if 0
    /* For USB */
    /* Enable all reqd USB masks */
    data_out[0]=0x00;
    if(SER_AB8500_CORE_OK != SER_AB8500_CORE_Write(0x0E, 0x69, 0x1,  data_out))
    {
        PRINT("SER_AB8500_CORE_Write(0x0E, 0x40, 0x17, data_out) FAILED!!!.");
        return;
    } 

    data_out[0]=0x00;
    if(SER_AB8500_CORE_OK != SER_AB8500_CORE_Write(0x0E, 0x6F, 0x1,  data_out))
    {
        PRINT("SER_AB8500_CORE_Write(0x0E, 0x40, 0x17, data_out) FAILED!!!.");
        return;
    }

    data_out[0]=0x00;
    if(SER_AB8500_CORE_OK != SER_AB8500_CORE_Write(0x0E, 0x72, 0x1,  data_out))
    {
        PRINT("SER_AB8500_CORE_Write(0x0E, 0x40, 0x17, data_out) FAILED!!!.");
        return;
    }

    /* Watchdog settings: Enable*/
      /* Watchdog settings: Enable */
    data_out = 0x00;
    if(SER_AB8500_CORE_OK != SER_AB8500_CORE_Write(0x03, 0x2F, 0x01, &data_out))
    {
        PRINT("SER_AB8500_CORE_Write(0x02, 0x01, 0x01, data_out) FAILED for data_out 0x01!!!");
        return;
    }    

 
    /* Watchdog settings: Kick */
   data_out = 0x04;
    if(SER_AB8500_CORE_OK != SER_AB8500_CORE_Write(0x02, 0x2F, 0x01,  &data_out))
    {
        PRINT("SER_AB8500_CORE_Write(0x02, 0x01, 0x01, data_out) FAILED for data_out 0x03!!!.");
        return;
    }  

    /* Watchdog settings: Disable */
   data_out = 0x02;
    if(SER_AB8500_CORE_OK != SER_AB8500_CORE_Write(0x03, 0x2F, 0x01,  &data_out))
    {
        PRINT("SER_AB8500_CORE_Write(0x02, 0x01, 0x01, data_out) FAILED for data_out 0x00!!!.");
        return;
     } 


    /* Set UsbDeviceModeEna in USBPhyCtrl Register */
    data_out[0] = 0x01;                              /* Make this 2 for Host */
    if(SER_AB8500_CORE_OK != SER_AB8500_CORE_Write(0x05, 0x8A, 0x01,  data_out))
    {
        PRINT("SER_AB8500_CORE_Write(0x05, 0x8A, 0x01, data_out) FAILED for data_out 0x02!!!.");
        return;
    }
#endif

    gic_error = GIC_EnableItLine(GIC_IRQ_N_LINE);
    if (gic_error != GIC_OK)
    {
        PRINT("GIC Enabling AB8500 Line error - %d\n", gic_error);
    }
    
   // gic_error = GIC_EnableItLine(GIC_PRCMU_NON_SECURE_LINE);
  //  if (gic_error != GIC_OK)
   // {
   //     PRINT("GIC Enabling PRCMU Line error - %d\n", gic_error);
   // }


    ab8500_error = ab8500_set_default_config();
    if(SER_AB8500_CORE_OK!=ab8500_error)
    {
        PRINT("\r\nSSP Configurations not set properly. Returned error %d\r\n", ab8500_error);
    }

}


