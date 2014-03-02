/*====================================================================
 *
 *Linux Driver for CW1200 series
 *
 *
 *Copyright(c) ST-Ericsson SA, 2010
 *
 *This program is free software; you can redistribute it and/or modify it
 *under the terms of the GNU General Public License version 2 as published
 *by the Free Software Foundation.
 *
 *======================================================================*/

/**
* DOC: SBUS.C
*
* PROJECT	:	CW1200_LINUX_DRIVER
*
* FILE		:	sbus.c
*
* This module interfaces with the Linux Kernel MMC/SDIO stack.
*
* date 25/02/2010
*/


/**************************************************************************
*INCLUDE FILES
***************************************************************************/

#ifndef USE_SPI
#include <linux/mmc/sdio_func.h>
#else
#include <linux/spi/spi.h>
#endif
#include <linux/delay.h>
#include <linux/mmc/pm.h>
#include <linux/stddef.h>
#include <asm/atomic.h>
#include <linux/gpio.h>
#include <linux/mmc/sdio.h>
#include <asm/setup.h>
#include <mach/cw1200_plat.h>
#include "cw1200_common.h"
#include "sbus.h"
#include "sbus_wrapper.h"
#include "eil.h"
#include <linux/interrupt.h>
/******************************************************************************
*Local functions
*******************************************************************************/
static uint32_t SBUS_Sleep_Device(struct CW1200_priv *priv);
/*static uint32_t SBUS_Device_Status(struct CW1200_priv *priv);*/
static uint32_t SBUS_Wakeup_Device(struct CW1200_priv *priv);
void SBUS_Sleep_Work(struct work_struct *work);
void cw1200_stay_awake(struct CW1200_priv *, unsigned long );
static inline CW1200_STATUS_E enqueue_rx_data(struct CW1200_priv *,
					struct sk_buff *, uint8_t *);
static void rx_list_bh(struct work_struct *);

/******************************************************************************
*Global Variables
*******************************************************************************/
#ifdef WORKAROUND
extern void irq_poll_init(struct CW1200_priv *priv);
extern void irq_poll_destroy(struct CW1200_priv *l_priv);
#endif

uint32_t hif_dbg_flag = FALSE;
extern struct meminfo meminfo;

#ifdef GPIO_BASED_IRQ
static irqreturn_t cw1200_gpio_irq(int irq, void *dev_id)
{
	struct CW1200_priv *priv = dev_id;

	DEBUG(DBG_SBUS, "GPIO:cw1200_sbus_interrupt Called \n");

	atomic_xchg(&(priv->Interrupt_From_Device), TRUE);
	queue_work(priv->sbus_WQ, &priv->sbus_work);
	/* Do not put device to sleep*/
	__cancel_delayed_work(&priv->sbus_sleep_work);

	return IRQ_HANDLED;
}
#endif


/**
* Increment_Wr_Buffer_Number
*
* This function increements the WRITE buffer no.
* Please see HIF MAD doc for details.
*
* @priv: pointer to device private structure.
*/
inline void Increment_Wr_Buffer_Number(struct CW1200_priv *priv)
{

	if (priv->sdio_wr_buf_num_qmode == 31)
		priv->sdio_wr_buf_num_qmode = 0 ;/* make it 0 - 31*/
	else
		priv->sdio_wr_buf_num_qmode++ ;
}


/**
* Increment_Rd_Buffer_Number
*
* This function increements the READ buffer no.
* Please see HIF MAD doc for details.
*
* @priv: pointer to device private structure.
*/
inline void Increment_Rd_Buffer_Number(struct CW1200_priv *priv)
{

	if (priv->sdio_rd_buf_num_qmode == 4)
		priv->sdio_rd_buf_num_qmode = 1 ;  /* make it 0 - 4 */
	else
		priv->sdio_rd_buf_num_qmode++ ;
}


/**
* SBUS_SDIO_RW_Reg
*
* This function reads/writes a register from the device.
* returns the status of the operation
*
* @priv: pointer to device private structure.
* @addr: Register address to read/write.
* @rw_buf: Pointer to buffer containing the write data and the buffer to store
* the data read.
* @length: Length of the buffer to read/write.
* @read_write: Operation to perform - READ/WRITE.
*/
CW1200_STATUS_E SBUS_SDIO_RW_Reg(struct CW1200_priv *priv,
		uint32_t addr, uint8_t *rw_buf,
		uint16_t length, int32_t read_write)
{

	uint32_t  sdio_reg_addr_17bit ;
	int32_t   retval;
	uint32_t  retry = 0;

	/*Check if buffer is aligned to 4 byte boundary */
	if ((((unsigned)rw_buf) & 3) != 0) {
		DEBUG(DBG_SBUS , "FATAL Error:Buffer UnAligned \n");
		return ERROR_BUF_UNALIGNED;
	}

	/*Convert to SDIO Register Address */
	addr = SPI_REG_ADDR_TO_SDIO(addr);
	sdio_reg_addr_17bit = SDIO_ADDR17BIT(0, 0, 0, addr) ;
	sdio_claim_host(priv->func);
	if (SDIO_READ == read_write) {
		/*READ*/
		//printk(KERN_ERR"read start---------------\n");
		while (retry < MAX_RETRY) {
			retval = sbus_memcpy_fromio(priv->func, rw_buf,
						sdio_reg_addr_17bit, length);
			if (unlikely(retval)) {
				retry++;
				mdelay(1);
			} else {
				break;
			}
		}
		//printk(KERN_ERR"read end---------------------\n");
	} else {
		/*WRITE*/
		while (retry < MAX_RETRY) {
			retval = sbus_memcpy_toio(priv->func,
						sdio_reg_addr_17bit,
						rw_buf, length);
			if (unlikely(retval)) {
				retry++;
				mdelay(1);
			} else {
				break;
			}
		}
	}
	sdio_release_host(priv->func);

	if (unlikely(retval)) {
		DEBUG(DBG_ERROR, "SDIO Error [%d] \n", retval);
		return ERROR_SDIO;
	} else
		return SUCCESS;

}


/**
* SBUS_SramWrite_AHB
*
* This function writes directly to AHB RAM.
* returns the status of the operation
*
* @priv: pointer to device private structure.
* @base_addr: Base address in the RAM from where to upload firmware
* @buffer: Pointer to firmware chunk
* @byte_count: Length of the buffer to read/write.
*/
CW1200_STATUS_E SBUS_SramWrite_AHB(struct CW1200_priv *priv, uint32_t base_addr,
				uint8_t *buffer, uint32_t byte_count)
{
	int32_t retval;

	if ((byte_count/2) > 0xfff) {
		DEBUG(DBG_SBUS, "SBUS_SramWrite_AHB: ERROR:"
			"Cannot write more than 0xfff words(requested = %d)\n",
		((byte_count/2)));
		return ERROR_INVALID_PARAMETERS;
	}

	retval = SBUS_SDIO_RW_Reg(priv, ST90TDS_SRAM_BASE_ADDR_REG_ID,
				(uint8_t *)&base_addr,
				BIT_32_REG, SDIO_WRITE);
	if (retval) {
		DEBUG(DBG_SBUS, "SBUS_SramWrite_AHB:"
				"sbus write failed, error=%d \n", retval);
		return ERROR_SDIO;
	}


	if (byte_count < (DOWNLOAD_BLOCK_SIZE - 1)) {
		retval = SBUS_SDIO_RW_Reg(priv, ST90TDS_AHB_DPORT_REG_ID,
					buffer, byte_count, SDIO_WRITE);
		if (retval) {
			DEBUG(DBG_SBUS, "SBUS_SramWrite_AHB:"
				"sbus write failed,error=%d \n", retval);
			return ERROR_SDIO;
		}
	} else {
		mdelay(1);
		/*Right now using Multi Byte */
		retval = SBUS_SDIO_RW_Reg(priv, ST90TDS_AHB_DPORT_REG_ID,
					buffer, byte_count, SDIO_WRITE);
		if (retval) {
			DEBUG(DBG_SBUS, "SBUS_SramWrite_AHB:write"
			"failed, error=%d \n", retval);
			return ERROR_SDIO;
		}
	}
	return SUCCESS;
}


/**
* SBUS_Set_Prefetch
*
* This function sets the prefect bit and waits for it to get cleared.
*
* @priv: pointer to device private structure.
*/
CW1200_STATUS_E SBUS_Set_Prefetch(struct CW1200_priv *priv)
{
	uint32_t config_reg_val = 0;
	uint32_t count = 0;

	/*Read CONFIG Register Value - We will read 32 bits*/
	if (SBUS_SDIO_RW_Reg(priv, ST90TDS_CONFIG_REG_ID,
				(uint8_t *)&config_reg_val,
				BIT_32_REG, SDIO_READ)) {
		return ERROR_SDIO;
	}

	config_reg_val = config_reg_val | ST90TDS_CONFIG_PFETCH_BIT;
	if (SBUS_SDIO_RW_Reg(priv, ST90TDS_CONFIG_REG_ID,
				(uint8_t *)&config_reg_val,
				BIT_32_REG, SDIO_WRITE)) {
		return ERROR_SDIO;
	}
	/*Check for PRE-FETCH bit to be cleared */
	for (count = 0; count < 20; count++) {
		mdelay(1);
	/*Read CONFIG Register Value - We will read 32 bits*/
		if (SBUS_SDIO_RW_Reg(priv, ST90TDS_CONFIG_REG_ID,
					(uint8_t *)&config_reg_val,
					BIT_32_REG, SDIO_READ)) {
			return ERROR_SDIO;
		}

		if (!(config_reg_val & ST90TDS_CONFIG_PFETCH_BIT))
			break;
	}

	if (count >= 20) {
		DEBUG(DBG_ERROR, "Prefetch bit not cleared \n");
		return ERROR;
	}

	return SUCCESS;
}


/**
* SBUS_Set_Prefetch_AHB
*
* This function sets the prefect bit and waits for it to get cleared.
*
* @priv: pointer to device private structure.
*/
CW1200_STATUS_E SBUS_Set_Prefetch_AHB (struct CW1200_priv *priv)
{
	uint32_t config_reg_val = 0;
	uint32_t count = 0;

	/*Read CONFIG Register Value - We will read 32 bits*/
	if (SBUS_SDIO_RW_Reg(priv, ST90TDS_CONFIG_REG_ID,
			(uint8_t *)&config_reg_val, BIT_32_REG, SDIO_READ)) {

		return ERROR_SDIO;
	}

	config_reg_val = config_reg_val | ST90TDS_CONFIG_AHB_PFETCH_BIT ;
	if (SBUS_SDIO_RW_Reg(priv, ST90TDS_CONFIG_REG_ID,
			(uint8_t *)&config_reg_val, BIT_32_REG, SDIO_WRITE)) {

		return ERROR_SDIO;
	}

	/*Check for PRE-FETCH bit to be cleared */
	for (count = 0; count < 20; count++) {
		mdelay (1);
	/*Read CONFIG Register Value - We will read 32 bits*/
		if (SBUS_SDIO_RW_Reg (priv, ST90TDS_CONFIG_REG_ID,
			(uint8_t *)&config_reg_val, BIT_32_REG, SDIO_READ)) {

			return ERROR_SDIO;
		}

		if (!(config_reg_val & ST90TDS_CONFIG_AHB_PFETCH_BIT))
			break;
	}

	if (count >= 20) {
		DEBUG (DBG_ERROR, "AHB Prefetch bit not cleared \n");
		return ERROR;
	}

	return SUCCESS;
}


/**
* SBUS_SramWrite_APB
*
*  This function writes directly to SRAM.
* returns the Status of the operation
*
* @priv: pointer to device private structure.
* @base_addr: Base address in the RAM from where to write to.
* @buffer	: Pointer to the buffer.
* @byte_count: Length of the buffer to read/write
*/
CW1200_STATUS_E SBUS_SramWrite_APB(struct CW1200_priv *priv, uint32_t base_addr,
		uint8_t *buffer, uint32_t byte_count)
{
	int32_t retval;

	if ((byte_count/2) > 0xfff) {
		DEBUG(DBG_SBUS, "SBUSSramWriteAPB:ERROR:Cannot write more than"
				"0xfff words(requested = %d)\n",
				((byte_count/2)));
		return ERROR_INVALID_PARAMETERS;
	}
#ifdef MOP_WORKAROUND
	if ((byte_count > 4) && (byte_count <= DOWNLOAD_BLOCK_SIZE))
		byte_count = DOWNLOAD_BLOCK_SIZE;
#endif
	if ((byte_count > 4) && (byte_count <= DOWNLOAD_BLOCK_SIZE))
		byte_count = DOWNLOAD_BLOCK_SIZE;

	retval = SBUS_SDIO_RW_Reg(priv, ST90TDS_SRAM_BASE_ADDR_REG_ID,
				(uint8_t *)&base_addr, BIT_32_REG, SDIO_WRITE);
	if (retval) {
		DEBUG(DBG_SBUS, "SBUS_SramWrite_APB:"
				"sbus write failed, error=%d \n", retval);
		return ERROR_SDIO;
	}

	retval = SBUS_SDIO_RW_Reg(priv, ST90TDS_SRAM_DPORT_REG_ID,
					buffer, byte_count, SDIO_WRITE);
	if (retval) {
		DEBUG(DBG_SBUS, "SBUS_SramWrite_APB: sbus"
				"write failed, error=%d \n", retval);
		return ERROR_SDIO;
	}

	return SUCCESS;
}


/**
* SBUS_SramRead_APB
*
* This function from APB-SRAM.
* returns the Status of the operation
*
* @priv: pointer to device private structure.
* @base_addr: Base address in the RAM from where to read from.
* @buffer	: Pointer to the buffer.
* @byte_count: Length of the buffer to read/write
*/
CW1200_STATUS_E SBUS_SramRead_APB(struct CW1200_priv *priv,
		uint32_t base_addr,
		uint8_t *buffer, uint32_t byte_count)
{
	int32_t retval;

	if ((byte_count/2) > 0xfff) {
		DEBUG(DBG_SBUS, "SBUS_SramWrite_APB: ERROR: Cannot write"
		"more than 0xfff words(requested = %d)\n", ((byte_count/2)));
		return ERROR_INVALID_PARAMETERS;
	}

	retval = SBUS_SDIO_RW_Reg(priv, ST90TDS_SRAM_BASE_ADDR_REG_ID,
				(uint8_t *)&base_addr, BIT_32_REG, SDIO_WRITE);
	if (retval) {
		DEBUG(DBG_SBUS, "SBUS_SramWrite_APB: sbus"
				"write failed, error=%d \n", retval);
		return ERROR_SDIO;
	}

	if (SBUS_Set_Prefetch(priv))
		return ERROR;

	retval = SBUS_SDIO_RW_Reg(priv, ST90TDS_SRAM_DPORT_REG_ID,
				buffer, byte_count, SDIO_READ);
	if (retval) {
		DEBUG(DBG_SBUS, "SBUS_SramWrite_APB:"
				"sbus write failed, error=%d \n", retval);
		return ERROR_SDIO;
	}

	return SUCCESS;
}


/**
* SBUS_SramRead_AHB
*
* This function from AHB-SRAM.
* returns the Status of the operation
*
* @priv: pointer to device private structure.
* @base_addr: Base address in the RAM from where to read from.
* @buffer	: Pointer to the buffer.
* @byte_count: Length of the buffer to read/write
*/
CW1200_STATUS_E SBUS_SramRead_AHB (struct CW1200_priv *priv, uint32_t base_addr,
				uint8_t *buffer, uint32_t byte_count)
{
	int32_t retval;

	if ((byte_count/2) > 0xfff) {
		DEBUG (DBG_SBUS, "SBUS_SramRead_AHB: ERROR: Cannot write more "
				"than 0xfff words (requested = %d)\n", ((byte_count/2)));
		return ERROR_INVALID_PARAMETERS;
	}

	retval = SBUS_SDIO_RW_Reg (priv, ST90TDS_SRAM_BASE_ADDR_REG_ID, (uint8_t *)&base_addr, BIT_32_REG, SDIO_WRITE) ;
	if (retval) {
		DEBUG (DBG_SBUS, "SBUS_SramRead_AHB: sbus write failed, error=%d \n", retval);
		return ERROR_SDIO;
	}

	if (SBUS_Set_Prefetch_AHB (priv))
		return ERROR;

	retval = SBUS_SDIO_RW_Reg (priv, ST90TDS_AHB_DPORT_REG_ID, buffer, byte_count, SDIO_READ);

	if (retval) {
		DEBUG (DBG_SBUS, "SBUS_SramRead_AHB: sbus write failed, error=%d \n", retval);
		return ERROR_SDIO;
	}

	return SUCCESS;
}


/**
* SBUS_SDIOReadWrite_QUEUE_Reg
*
* This function reads/writes device QUEUE register.
* returns the Status of the operation
*
* @priv: pointer to device private structure.
* @rw_buf: pointer to buffer containing the write data or the buffer to copy
* the data read.
* @length: Data read/write length
* @read_write: READ or WRITE operation.
*/
CW1200_STATUS_E SBUS_SDIOReadWrite_QUEUE_Reg(struct CW1200_priv *priv,
			uint8_t *rw_buf, uint16_t length, int32_t read_write)
{
	uint32_t  sdio_reg_addr_17bit ;
	int32_t	retval;
	int32_t  addr = 0;
	uint32_t retry = 0;
	uint8_t i = 0;
	uint32_t next_start = 0;
	uint32_t end = 0;

	/*Convert to SDIO Register Address */
	addr = SPI_REG_ADDR_TO_SDIO(ST90TDS_IN_OUT_QUEUE_REG_ID);

	sdio_claim_host(priv->func);
	if (read_write == SDIO_READ) {
		/*READ*/
		sdio_reg_addr_17bit = SDIO_ADDR17BIT(
						priv->sdio_rd_buf_num_qmode,
						0, 0, addr);
		while (retry < MAX_RETRY) {
			retval = sbus_memcpy_fromio(priv->func, rw_buf,
						sdio_reg_addr_17bit, length);
			if (unlikely(retval)) {
				DEBUG(DBG_ERROR, "sbus_memcpy_fromio failed,"
					"error#%d, Retry#%d\n", retval, retry);
				retry++;
				mdelay(1);
			} else {
				break;
			}
		}
	} else {
		/*WRITE*/
		sdio_reg_addr_17bit = SDIO_ADDR17BIT(
						priv->sdio_wr_buf_num_qmode,
						0, 0, addr) ;
		for(i = 0; i < meminfo.nr_banks; i++) {
			end = (phys_to_virt((&meminfo.bank[i])->start))
					+ (&meminfo.bank[i])->size;
			if(i == (meminfo.nr_banks - 1)) {
				if((end - 1) < (rw_buf + length - 1)) {
					DEBUG(DBG_ERROR, "Wrong Memory read\n");
					sdio_release_host(priv->func);
					return ERROR;
				}
			} else {
				next_start = phys_to_virt((&meminfo.bank[i+1])->start);
				if((end - 1) < (rw_buf + length - 1)
					&& (rw_buf + length) < next_start) {
					DEBUG(DBG_ERROR, "Wrong Memory read\n");
					sdio_release_host(priv->func);
					return ERROR;
				}
			}
		}
		while (retry < MAX_RETRY) {
			retval = sbus_memcpy_toio(
					priv->func, sdio_reg_addr_17bit,
					rw_buf, length);
			if (unlikely(retval)) {
				DEBUG(DBG_ERROR, "sbus_memcpy_toio failed,"
					"error#%d, Retry#%d\n", retval, retry);
				retry++;
				mdelay(1);
			} else {
				Increment_Wr_Buffer_Number(priv);
				break;
			}
		}
	}

	sdio_release_host(priv->func);
	/*Retries failed return ERROR to Caller */
	if (unlikely(retval))
		return ERROR_SDIO;
	else
		return SUCCESS;

}


/**
* CW1100_UploadFirmware
*
* This function uploads firmware to the device.
* returns the Status of the operation
*
* @priv: pointer to device private structure.
* @firmware:  Pointer to the firmware image
* @length: Length of the firmware image.
* @read_write: READ or WRITE operation.
*/
CW1200_STATUS_E CW1100_UploadFirmware(struct CW1200_priv *priv,
				void *firmware, uint32_t fw_length)
{

	uint32_t	i;
	uint32_t	status = SUCCESS;
	uint8_t 	*buffer_loc;
	uint32_t	num_blocks;
	uint32_t	length = 0, remain_length = 0;


	num_blocks = fw_length/DOWNLOAD_BLOCK_SIZE;

	if ((fw_length  - (num_blocks * DOWNLOAD_BLOCK_SIZE)) > 0)
		num_blocks = num_blocks + 1;

	buffer_loc = (uint8_t *)firmware;

	for (i = 0 ; i < num_blocks ; i++) {

#ifndef MOP_WORKAROUND
		remain_length = fw_length - (i*DOWNLOAD_BLOCK_SIZE);
		if (remain_length >= DOWNLOAD_BLOCK_SIZE)
			length = DOWNLOAD_BLOCK_SIZE;
		else
			length = remain_length;
#else
			length = DOWNLOAD_BLOCK_SIZE;
#endif
		if (SBUS_SramWrite_AHB(priv,
				(FIRMWARE_DLOAD_ADDR + i*DOWNLOAD_BLOCK_SIZE),
				buffer_loc, length) != SUCCESS) {
				status = ERROR;
				break;
		}
			buffer_loc += length;
	}

	if (status != ERROR)
		status = SUCCESS;

	return status;
}


/**
* CW1200_UploadFirmware
*
* This function uploads firmware to the device.
* returns the Status of the operation
*
* @priv: pointer to device private structure.
* @firmware:  Pointer to the firmware image
* @length: Length of the firmware image.
* @read_write: READ or WRITE operation.
*/
CW1200_STATUS_E CW1200_UploadFirmware(struct CW1200_priv *priv,
			uint8_t *firmware, uint32_t fw_length)
{
	uint32_t reg_value = 0;
	download_cntl_t download = {0};
	uint32_t config_reg_val = 0;
	uint32_t count = 0;
	uint32_t num_blocks = 0;
	uint32_t block_size = 0;
	uint8_t *buffer_loc = NULL;
	uint8_t *fw_buf = NULL;
	uint32_t  i = 0;

	/* Initializing the download control Area with bootloader Signature */
	reg_value = DOWNLOAD_ARE_YOU_HERE;

	fw_buf = kzalloc(DOWNLOAD_BLOCK_SIZE, GFP_KERNEL | GFP_DMA);
	if (NULL == fw_buf) {
		DEBUG(DBG_ERROR, "Memory Allocation Failed\n");
		return ERROR;
	}

	if (SBUS_SramWrite_APB(priv, PAC_SHARED_MEMORY_SILICON
				+ DOWNLOAD_IMAGE_SIZE_REG,
				(uint8_t *)&reg_value, BIT_32_REG)) {
		DEBUG(DBG_ERROR, "%s:SBUS_SramWrite_APB()"
				"returned error \n", __func__);
		return ERROR_SDIO;
	}

	download.Flags = 0;
	/*BIT 0 should be set to 1 if the device UART is to be used */
	download.Put = 0;
	download.Get = 0;
	download.Status = DOWNLOAD_PENDING;

	reg_value = download.Put;
	if (SBUS_SramWrite_APB(priv, PAC_SHARED_MEMORY_SILICON
				+ DOWNLOAD_PUT_REG,(uint8_t *)&reg_value,
				sizeof(uint32_t))) {
		DEBUG(DBG_ERROR, "%s:ERROR: Download"
				"Crtl Put failed \n", __func__);
		return ERROR_SDIO;
	}

	reg_value = download.Get;
	if (SBUS_SramWrite_APB(priv, PAC_SHARED_MEMORY_SILICON +
				DOWNLOAD_GET_REG,(uint8_t *)&reg_value,
				sizeof(uint32_t))) {
		DEBUG(DBG_ERROR, "%s:ERROR: Download Crtl"
				"Get failed \n", __func__);
		return ERROR_SDIO;
	}

	reg_value = download.Status;
	if (SBUS_SramWrite_APB(priv, PAC_SHARED_MEMORY_SILICON
				+ DOWNLOAD_STATUS_REG,(uint8_t *)&reg_value,
				sizeof(uint32_t))) {
		DEBUG(DBG_ERROR, "%s:ERROR: Download Crtl"
				"Status Reg failed \n", __func__);
		return ERROR_SDIO;
	}

	reg_value = download.Flags;
	if (SBUS_SramWrite_APB(priv, PAC_SHARED_MEMORY_SILICON
				+ DOWNLOAD_FLAGS_REG,
				(uint8_t *)&reg_value, sizeof(uint32_t))) {
		DEBUG(DBG_ERROR, "%s:ERROR: Download Crtl"
				"Flag Reg failed \n", __func__);
		return ERROR_SDIO;
	}

	/*Write the NOP Instruction */
	reg_value = 0xFFF20000;
	if (SBUS_SDIO_RW_Reg(priv, ST90TDS_SRAM_BASE_ADDR_REG_ID,
				(uint8_t *)&reg_value, BIT_32_REG, SDIO_WRITE)) {

		DEBUG(DBG_ERROR, "%s:ERROR:SRAM Base"
				"Address Reg Failed \n", __func__);
		return ERROR_SDIO;
	}

	reg_value = 0xEAFFFFFE;
	if (SBUS_SDIO_RW_Reg(priv, ST90TDS_AHB_DPORT_REG_ID,
			(uint8_t *)&reg_value, BIT_32_REG, SDIO_WRITE)) {

		DEBUG(DBG_ERROR, "%s:ERROR:NOP Write Failed \n", __func__);
		return ERROR_SDIO;
	}

	/*Release CPU from RESET */
	if (SBUS_SDIO_RW_Reg(priv, ST90TDS_CONFIG_REG_ID,
			(char *)&config_reg_val, BIT_32_REG, SDIO_READ)) {
		DEBUG(DBG_ERROR, "%s:ERROR:Read Config"
				"Reg Failed \n", __func__);
		return ERROR_SDIO;
	}

	config_reg_val = config_reg_val & ST90TDS_CONFIG_CPU_RESET_MASK;
	if (SBUS_SDIO_RW_Reg(priv, ST90TDS_CONFIG_REG_ID,
			(char *)&config_reg_val, BIT_32_REG, SDIO_WRITE)) {
		DEBUG(DBG_ERROR, "%s:ERROR:Write"
				"Config Reg Failed \n", __func__);
		return ERROR_SDIO;
	}

	/*Enable Clock */
	config_reg_val = config_reg_val & ST90TDS_CONFIG_CPU_CLK_DIS_MASK;
	if (SBUS_SDIO_RW_Reg(priv, ST90TDS_CONFIG_REG_ID,
			(char *)&config_reg_val, BIT_32_REG , SDIO_WRITE)) {
		DEBUG(DBG_ERROR, "%s:ERROR:Write Config"
			"Reg Failed \n", __func__);
		return ERROR_SDIO;
	}

	DEBUG(DBG_SBUS, "SBUS:CW1200_UploadFirmware:Waitingi"
		"for Bootloader to be ready \n");
	/*Check if the bootloader is ready */
	for (count = 0; count < 100; count++) {
		mdelay(1);
		if (SBUS_SramRead_APB(priv, PAC_SHARED_MEMORY_SILICON
					+ DOWNLOAD_IMAGE_SIZE_REG,
					(uint8_t *)&reg_value, BIT_32_REG)) {
			return ERROR;
		}

		if (reg_value == DOWNLOAD_I_AM_HERE) {
			DEBUG(DBG_SBUS, "BootLoader Ready \n");
			break;
		}
	} /*End of for loop */

	if (count >= 100) {
		DEBUG(DBG_ERROR, "Bootloader not ready:Timeout \n");
		return ERROR;
	}

	/*Calculcate number of download blocks */
	num_blocks = fw_length/DOWNLOAD_BLOCK_SIZE;

	if ((fw_length  - (num_blocks * DOWNLOAD_BLOCK_SIZE)) > 0)
		num_blocks = num_blocks + 1;

	/*Updating the length in Download Ctrl Area */
	download.ImageSize = fw_length;
	reg_value = download.ImageSize;

	if (SBUS_SramWrite_APB(priv, PAC_SHARED_MEMORY_SILICON
			+ DOWNLOAD_IMAGE_SIZE_REG, (uint8_t *)&reg_value, BIT_32_REG)) {
		DEBUG(DBG_ERROR, "%s:SBUS_SramWrite_APB()"
				"returned error \n", __func__);
		return ERROR_SDIO;
	}

	/* Firmware downloading loop */
	for (i = 0; i < num_blocks ; i++) {
		/* check the download status */
		reg_value = 0;
		if (SBUS_SramRead_APB(priv, PAC_SHARED_MEMORY_SILICON
			+ DOWNLOAD_STATUS_REG, (uint8_t *)&reg_value, BIT_32_REG)) {
			DEBUG(DBG_ERROR, "%s:SBUS_SramRead_APB()"
					"returned error \n", __func__);
			return ERROR;
		}

		download.Status = reg_value;
		DEBUG(DBG_SBUS, "SBUS:CW1200_UploadFirmware()"
				":download status=%lu \n", download.Status);
		if (download.Status != DOWNLOAD_PENDING) {
			/* FW loader reporting an error status*/
			if (download.Status == DOWNLOAD_EXCEPTION) {
				/*read exception data*/
				DEBUG(DBG_SBUS, "SBUS:CW1200_UploadFirmware():"
				"ERROR: Loader DOWNLOAD_EXCEPTION \n");
				SBUS_SramRead_APB(priv,
				PAC_SHARED_MEMORY_SILICON
				+ DOWNLOAD_DEBUG_DATA_REG,
				(uint8_t *)&download.DebugData[0],
				DOWNLOAD_DEBUG_DATA_LEN);
			}
			return ERROR;
		}

		/* loop until put - get <= 24K */
		for (count = 0; count < 200; count++) {
			reg_value = 0;
			if (SBUS_SramRead_APB(priv, PAC_SHARED_MEMORY_SILICON
						+ DOWNLOAD_GET_REG,
						(uint8_t *)&reg_value, BIT_32_REG)) {
				DEBUG(DBG_SBUS, "Sram Read get failed \n");
				return ERROR_SDIO;
			}
			download.Get = reg_value;
			if ((download.Put - download.Get) <=
			(DOWNLOAD_FIFO_SIZE - DOWNLOAD_BLOCK_SIZE))
				/*OK we can put*/
				break;
			mdelay(1);/* 1 Mil sec*/
		}

		if (count >= 200) {
			DEBUG(DBG_ERROR, "SBUS:CW1200_Uploadfirmware()"
			":PUT-GET timedout \n");
			return ERROR;
		}
		/* get the block size */
		if ((download.ImageSize - download.Put) >= DOWNLOAD_BLOCK_SIZE)
			block_size = DOWNLOAD_BLOCK_SIZE;
		else
/*bytes */		block_size = download.ImageSize - download.Put;

		memcpy(fw_buf, (firmware + download.Put), block_size);
		if (DOWNLOAD_BLOCK_SIZE > block_size)
			memset(&fw_buf[block_size], 0,
				DOWNLOAD_BLOCK_SIZE-block_size);
		buffer_loc = fw_buf;

	/*send the block to sram */
		if (SBUS_SramWrite_APB(priv, PAC_SHARED_MEMORY_SILICON
				+ DOWNLOAD_FIFO_OFFSET
				+ (download.Put & (DOWNLOAD_FIFO_SIZE - 1)),
				buffer_loc, block_size)) {
			DEBUG(DBG_ERROR, "SBUS:CW1200_Uploadfirmware()"
				":SRAM Write Error \n");
			kfree(fw_buf);
			return ERROR;
	}

		DEBUG(DBG_SBUS, "WDEV_Start_CW1200: Block %d loaded \n", i);
		/* update the put register*/
		download.Put = download.Put + block_size;
		reg_value = download.Put;
		if (SBUS_SramWrite_APB(priv, PAC_SHARED_MEMORY_SILICON
				+ DOWNLOAD_PUT_REG, (uint8_t *)&reg_value, BIT_32_REG)) {
			DEBUG(DBG_SBUS, "SBUS:CW1200_Uploadfirmware()"
				"Sram Write update put failed \n");
			kfree(fw_buf);
			return ERROR;
		}

	} /*End of firmware download loop*/
		/* wait for the download completion*/
	kfree(fw_buf);
	count = 0;
	reg_value = 0;
	if (SBUS_SramRead_APB(priv, PAC_SHARED_MEMORY_SILICON
			+ DOWNLOAD_STATUS_REG, (uint8_t *)&reg_value, BIT_32_REG)) {
			DEBUG(DBG_SBUS, "SBUS:CW1200_Uploadfirmware()"
			"Sram Read failed \n");
		return ERROR;
	}
	download.Status = reg_value;
	while (download.Status == DOWNLOAD_PENDING) {
		mdelay(5);
		reg_value = 0;
		if (SBUS_SramRead_APB(priv, PAC_SHARED_MEMORY_SILICON
			+ DOWNLOAD_STATUS_REG, (uint8_t *)&reg_value, BIT_32_REG)) {
			DEBUG(DBG_SBUS, "SBUS:"
			"CW1200_Uploadfirmware() Sram Read failed \n");
			return ERROR;
		}
		download.Status = reg_value;
		count++;
		if (count >= 100) {
			DEBUG(DBG_SBUS, "SBUS:CW1200_Uploadfirmware()"
			":Status not changed after firmware upload \n");
			break;
		}
	}
	if (download.Status != DOWNLOAD_SUCCESS) {
		DEBUG(DBG_SBUS, "SBUS:CW1200: download failed\n");
		return ERROR;
	}
	return SUCCESS;
}


/**
* SBUS_bh
*
* This function implements the SBUS bottom half handler
*
* @work: pointer to the Work Queue work_struct
* @firmware:  Pointer to the firmware image
* @length: Length of the firmware image.
* @read_write: READ or WRITE operation.
*/
static void SBUS_bh(struct work_struct *work)
{

	uint32_t	cont_reg_val = 0 ;
	uint32_t	read_len = 0;
	uint32_t	rx_count = 0;
	uint32_t	alloc_len = 0;
	hif_rw_msg_hdr_payload_t *payload;
	HI_STARTUP_IND	*startup_ind;
	HI_EXCEPTION_IND   *exception_ind;
	char buf[HI_SW_LABEL_MAX + 1] ;
	uint16_t	msgid = 0;
	int32_t retval = SUCCESS;
	UMI_GET_TX_DATA *pTxData = NULL;
	struct sk_buff *skb = NULL;
	struct sk_buff *skb_tx = NULL;
	uint32_t tx_len = 0;
	struct CW1200_priv *priv =
			container_of(work, struct CW1200_priv, sbus_work);
	uint32_t control_reg = 0;
	uint8_t *data = NULL;
	uint32_t buffer_type = BUF_TYPE_MANG;
	uint32_t config_reg_val = 0;
	int32_t num_tx_conf=0;
#ifdef MOP_WORKAROUND
	int tmp, bufflen = 0;
	char *buff;
#endif
	cw1200_stay_awake(priv, HZ);

	DEBUG(DBG_SBUS, "SBUS:SBUS_bh Called\n");
#ifdef WORKAROUND
	down(&priv->sem);
#endif
	if (priv->suspend == TRUE) {
		printk(KERN_ERR"wlan is suspend, need delay 100ms\n");
		msleep(100);
	}

/**********************Receive ******************************/
RX_INT:
	/*If the device raised an Interrupt enter the RX code */
	if (atomic_xchg(&(priv->Interrupt_From_Device), 0)) {
		/*Read Control register to retrieve the buffer len */
		retval = SBUS_SDIO_RW_Reg(priv, ST90TDS_CONTROL_REG_ID,
					(char *)&cont_reg_val, BIT_16_REG,
					SDIO_READ);
		if (retval) {
			DEBUG(DBG_SBUS, "SDIO Read/Write Error \n");
			goto err_out;
		}
		control_reg = cont_reg_val;
		cont_reg_val =
			(uint16_t)(cont_reg_val & ST90TDS_SRAM_BASE_LSB_MASK);
		/*Extract next message length(in words) from CONTROL register */
		read_len = SPI_CONT_NEXT_LENGTH(cont_reg_val);
		/*Convert length to length in BYTES */
		read_len = read_len * 2;
		/* Check device sleep status and update driver status */
		if (cont_reg_val & ST90TDS_CONT_RDY_BIT) {
			if (DEVICE_DOWN == priv->device_sleep_status) {
				priv->device_sleep_status = DEVICE_UP;
			}
		} else {
			priv->device_sleep_status = DEVICE_DOWN;
		}
		DEBUG(DBG_SBUS, "SBUS_bh():DEV SLEEP STATUS:[%d] \n",
				priv->device_sleep_status);

		/*For restarting Piggyback Jump here */
RX:
		rx_count = 0;

		/*Repeat the RX Loop 2 times.Imperical Value. Can be changed*/
		while ((rx_count < 1) && (read_len > 0)) {
			buffer_type = BUF_TYPE_MANG;
			if (control_reg & 0x00004000) {
				buffer_type = BUF_TYPE_DATA;
				DEBUG(DBG_SBUS, "SBUS_bh():FRAME TYPE INFO -"
					"Data is set in Control Reg \n");
			}
			if ((read_len < sizeof(HI_MSG_HDR)) || read_len >
				(MAX_SZ_RD_WR_BUFFERS-PIGGYBACK_CTRL_REG)) {
				DEBUG(DBG_ERROR, "SBUS:SBUS_bh():"
					"read buffer length not correct\n");
				goto err_out;
			}
			/*Add SIZE of PIGGYBACK reg(CONTROL Reg) to
			* the NEXT Message length + 2 Bytes for SKB */
			read_len = read_len +  2;
#ifdef MOP_WORKAROUND
			tmp = read_len%RWBLK_SIZE ?
			(read_len+RWBLK_SIZE-read_len%RWBLK_SIZE) : read_len;
			alloc_len = (tmp < SDIO_BLOCK_SIZE) ? SDIO_BLOCK_SIZE : tmp;
#else
			alloc_len = sdio_align_size(priv->func, read_len);
#endif
			if (BUF_TYPE_DATA == buffer_type) {
				/*Allocate SKB  */
				skb = dev_alloc_skb(alloc_len + HEADER_FOR_FWDING);
				if (!skb) {
					DEBUG(DBG_ERROR, "SKB alloc"
						"failed. Out of memory.\n");
					goto err_out;
				}
				/* Allocate room for frame forwarding */
				skb_reserve(skb, HEADER_FOR_FWDING);
				/*Create room for data in the SKB */
				skb_put(skb, (read_len - 2));
				data = skb->data ;
			} else {
				data = (uint8_t *)
					kmalloc(alloc_len, GFP_KERNEL);
				if (!data) {
					DEBUG(DBG_ERROR, "SBUS:SBUS_bh():"
							"kmalloc failed \n");
					goto err_out;
				}
			} /*Read DATA from the DEVICE */
#ifdef MOP_WORKAROUND
			retval = SBUS_SDIOReadWrite_QUEUE_Reg(priv, data,
							alloc_len, SDIO_READ);
			if (retval) {
				DEBUG(DBG_ERROR, "SDIO  Queue "
						"Read/Write Error\n");
				goto err_out;
			}
#else
			retval = SBUS_SDIOReadWrite_QUEUE_Reg(priv,
						data, alloc_len, SDIO_READ);
			if (retval) {
				DEBUG(DBG_ERROR, "SDIO  Queue Read"
						"/Write Error\n");
				goto err_out;
			}
#endif
			/*Update CONTROL Reg value from the
			* DATA read and update NEXT Message Length */
			payload = (hif_rw_msg_hdr_payload_t *) data;

			cont_reg_val =
			*((uint16_t *)((uint8_t *)data + alloc_len - 2));
			/*Extract Message ID from header */

			msgid = (uint16_t)(payload->hdr.MsgId);

			/*If Message ID is EXCEPTION then STOP HIF
			Communication and Report ERROR to WDEV */
			if (unlikely((msgid & MSG_ID_MASK) == HI_EXCEPTION_IND_ID)) {
				exception_ind = (HI_EXCEPTION_IND *)payload;
				DEBUG(DBG_ERROR, "SBUS_bh Exception Reason %x",
						exception_ind->Reason);
				memset(buf, 0, sizeof(buf));
				memcpy(buf, exception_ind->FileName,
					HI_EXCEP_FILENAME_SIZE);

				DEBUG(DBG_ERROR , "SBUS_bh():File Name:[%s],"
					"Line No [%x], R2 [%x]\n", buf,
					exception_ind->R1, exception_ind->R2);


#if 1
				{
					int i, msglen;
					uint8_t *ptr; 
					ptr = (uint8_t *)payload;
					msglen = (uint16_t)(payload->hdr.MsgLen); 
					for (i = 0; i < msglen; i++ ) {
						if ((i%16) == 0)
							DEBUG(DBG_ERROR, "\n");
						DEBUG(DBG_ERROR, "%02x ",ptr[i]);
					}
					DEBUG(DBG_ERROR, "\n");
				}
#endif
				goto err_out;
			}

			/*If Message ID Is STARTUP then read the
			initialisation info from the Payload */
			if (unlikely((msgid & MSG_ID_MASK) == HI_STARTUP_IND_ID)) {
				startup_ind = (HI_STARTUP_IND *)payload;
				memset(buf, 0, sizeof(buf));
				memcpy(buf, startup_ind->FirmwareLabel,
						HI_SW_LABEL_MAX);
				PRINT("Firmware Label : [%s] \n", buf);
				PRINT("InitStatus : [%d]\n",
				startup_ind->InitStatus);
				priv->max_size_supp = startup_ind->SizeInpChBuf;
				priv->max_num_buffs_supp =
				startup_ind->NumInpChBufs;
			}
			/*Extract SEQUENCE Number from the Message ID and
				check whether it is the same as expected */
			/*If SEQUENCE Number mismatches then Shutdown */
			if (SBUS_GET_MSG_SEQ(payload->hdr.MsgId) !=
				(priv->in_seq_num & HI_MSG_SEQ_RANGE)) {
				DEBUG(DBG_ERROR, "FATAL Error:SEQUENCE Number"
				" Mismatchin Received Packet [%x],[%x] \n",
				SBUS_GET_MSG_SEQ(payload->hdr.MsgId),
					priv->in_seq_num & HI_MSG_SEQ_RANGE);
				goto err_out;
			}

			priv->in_seq_num++ ;
			/*Increement READ Queue Register Number */
			Increment_Rd_Buffer_Number(priv);

			/*If MESSAGE ID contains CONFIRMATION ID then
			Increement TX_CURRENT_BUFFER_COUNT */
			if (payload->hdr.MsgId & HI_CNF_BASE) {
				/* Check for multiTx conf */
				if ( (payload->hdr.MsgId & 0xff) == 0x1e) {
					num_tx_conf = payload->payload[0];
				} else {
					num_tx_conf = 1;
				}
				if ( (atomic_read(&(priv->num_unprocessed_buffs_in_device)) - num_tx_conf) >= 0)
					atomic_sub(num_tx_conf, &(priv->num_unprocessed_buffs_in_device));
				else
					DEBUG(DBG_ERROR, "HIF Lost sync RX CONF"
					"but there is no REQUEST message\n");
			}
			DEBUG(DBG_SBUS, "SBUS_bh():SKB Pointer passed:%p,%d\n",
					skb, payload->hdr.MsgLen);
					payload->hdr.MsgId &= MSG_ID_MASK;

			/* HIF debug prints */
			if (unlikely(1 == hif_dbg_flag)) {
				if(MINI_HIF < payload->hdr.MsgLen ) {
					print_hex_dump_bytes ( "PACKET_RECEIVED:",DUMP_PREFIX_NONE ,
							data , MINI_HIF);
				} else {
					print_hex_dump_bytes ( "PACKET_RECEIVED:",DUMP_PREFIX_NONE ,
							data , payload->hdr.MsgLen);
				}
				printk ( KERN_DEBUG "**************************************************\n");
			}

			if (BUF_TYPE_DATA == buffer_type) {
				/*Adjust skb->len as per MsgLen in HIHeader */
				skb->len = payload->hdr.MsgLen;
				/* Pass the packet to UMAC */
				DEBUG(DBG_SBUS, "SBUS_bh():Pass Data Buff\n");
				/* Acquire spinlock as SBUS
				bottom half can int CIL_Set/Get()*/
				retval = enqueue_rx_data(priv, skb, data);
			} else {
				/*Pass the packet to UMAC */
				DEBUG(DBG_SBUS, "SBUS_bh Pass MngtBuffer\n");
				/* Acquire spinlock as SBUS bottom
				half can interrupt CIL_Set/Get */
				retval = enqueue_rx_data(priv, NULL, data);
			}
			rx_count++;
			if (retval != UMI_STATUS_SUCCESS)
				DEBUG(DBG_SBUS, "UMIReceiveFrame returnerr\n");
#ifdef WORKAROUND
			/*Update the piggyback length */
			retval = SBUS_SDIO_RW_Reg(priv, ST90TDS_CONTROL_REG_ID,
				(char *)&cont_reg_val, BIT_16_REG, SDIO_READ);
			if (retval) {
				DEBUG(DBG_SBUS, "SDIO-Read/Write Error \n");
				goto err_out;
			}
#endif
			control_reg = cont_reg_val;
			cont_reg_val = (uint16_t)(cont_reg_val &
						ST90TDS_SRAM_BASE_LSB_MASK);
			/*If NEXT Message Length is greater
			than ZERO then goto step 3 else EXIT BottomHalf*/
			read_len = (SPI_CONT_NEXT_LENGTH(cont_reg_val));
			read_len = read_len * 2;
		}

	} /*End of RX Code */


/**********************Transmit ******************************/
TX:
	/*Check if TX is possible */
	if (atomic_read(&(priv->num_unprocessed_buffs_in_device)) < priv->max_num_buffs_supp) {
		/*Check if UMAC has indicated that it has a packet to send */
		if (atomic_xchg(&(priv->Umac_Tx), 0)) {
			if (DEVICE_DOWN == priv->device_sleep_status) {
				retval = SBUS_Wakeup_Device(priv);
				if (retval) {
					DEBUG(DBG_SBUS, "SBUS_bh():Unable"
						" to wakeup device: Wait for interrupt \n");
#ifdef WORKAROUND
					up(&priv->sem);
#endif
					atomic_xchg(&(priv->Umac_Tx), TRUE);
					return;
				}
			}
			retval = UMI_GetTxFrame(priv->lower_handle, &pTxData);
			if (unlikely(UMI_STATUS_SUCCESS != retval)) {
				if (UMI_STATUS_NO_MORE_DATA == retval) {
					/*If Piggyback is activethen restartRX*/
					if (read_len)
						goto RX;
					else {
						/*We dont having anything todo,
						hence exit bottom half */
#ifdef WORKAROUND
						up(&priv->sem);
#endif
						/*Read CONFIG Register Value- HW BUG - We will read
						only 1 byte*/
						retval = SBUS_SDIO_RW_Reg (priv, ST90TDS_CONFIG_REG_ID,
						(char *)&config_reg_val, 1, SDIO_READ);

						if (retval)
							goto err_out;

						/* Nothing to do. Put device in sleep mode */
				if (DEVICE_UP == priv->device_sleep_status
					&& priv->eil_shutdown == SHUTDOWN_NOT_STARTED) {
							queue_delayed_work(priv->sbus_WQ,
								&priv->sbus_sleep_work, HZ);
						}
						return;
					}
				} else {  /*Supposedly a FATAL Error*/
					DEBUG(DBG_ERROR, "SBUS_bh();"
					"UMI_GetTxFrame returned error \n");
					goto err_out;
				}
			} else {
				/* Has more data to send */
				atomic_xchg(&(priv->Umac_Tx), TRUE);
			} /*If -else check for retval ends here */
			skb_tx = (struct sk_buff *)(pTxData->pDriverInfo);
			if (NULL != skb_tx) {
				tx_len = skb_tx->len;
				/*Memove data if UMAC has added a 2"
				"bytes hole in case of QOS*/
				if ((pTxData->pTxDesc + pTxData->bufferLength)
					!= pTxData->pDot11Frame) {
					printk("Memmove data \n");
					memmove((pTxData->pTxDesc
					+ pTxData->bufferLength)
					, pTxData->pDot11Frame, tx_len);
				}
			}
			/*Increement TX Buffer Count */
			atomic_inc(&(priv->num_unprocessed_buffs_in_device));
			/*Write Buffer to the device */
			payload = (hif_rw_msg_hdr_payload_t *)
				(pTxData->pTxDesc);
			/*Update HI Message Seq Number */
			HI_PUT_MSG_SEQ(payload->hdr.MsgId,
					(priv->out_seq_num & HI_MSG_SEQ_RANGE));
			priv->out_seq_num++;
			tx_len = 0;
			/* HIF debug prints */
			if (unlikely(1 == hif_dbg_flag)) {
				if(MINI_HIF < payload->hdr.MsgLen ) {
					print_hex_dump_bytes ( "PACKET_SENT:", DUMP_PREFIX_NONE ,
							pTxData->pTxDesc ,MINI_HIF );
				} else {
					print_hex_dump_bytes ( "PACKET_SENT:", DUMP_PREFIX_NONE ,
							pTxData->pTxDesc , payload->hdr.MsgLen );
				}
				printk ( KERN_DEBUG "**************************************************\n");
			}
#ifdef MOP_WORKAROUND
			tmp = payload->hdr.MsgLen;
			bufflen = tmp%RWBLK_SIZE ?
			(tmp + RWBLK_SIZE - tmp % RWBLK_SIZE) : tmp;
			retval = SBUS_SDIOReadWrite_QUEUE_Reg(
					priv, pTxData->pTxDesc,
					bufflen, SDIO_WRITE);
			if (retval) {
				DEBUG(DBG_ERROR, "SBUS_bh():Device"
					"Data Queue Read/Write Error \n");
				goto err_out;
			}
#else
			retval = SBUS_SDIOReadWrite_QUEUE_Reg(
					priv, pTxData->pTxDesc,
					sdio_align_size(priv->func, payload->hdr.MsgLen),
					 SDIO_WRITE);
			if (retval) {
				DEBUG(DBG_ERROR, "SBUS_bh():Device"
					"Data Queue Read/Write Error \n");
				goto err_out;
			}
#endif
		} /*If - else check for Umac_Tx flag ends here */
	}
	/*Check is Piggyback is active and needs to be restarted */
	if (read_len) {
		goto RX;
	} else if (TRUE == atomic_read(&(priv->Interrupt_From_Device))) {
			/*If interrupt was raised restart
			RX Interrupt Handling */
			goto RX_INT;
	} else if (TRUE == atomic_read(&(priv->Umac_Tx))) {
		/*if UMAC has packet to send restart TX */
		if (atomic_read(&(priv->num_unprocessed_buffs_in_device)) <
			priv->max_num_buffs_supp)
			goto TX;
	}

	/*Read CONFIG Register Value- HW BUG - We will read only 1 byte*/
	retval = SBUS_SDIO_RW_Reg (priv, ST90TDS_CONFIG_REG_ID,
				(char *)&config_reg_val, 1, SDIO_READ);

	if (retval)
		goto err_out;

	/* Nothing to do. Put device in sleep mode */
	if (DEVICE_UP == priv->device_sleep_status &&
			priv->eil_shutdown == SHUTDOWN_NOT_STARTED) {
		queue_delayed_work(priv->sbus_WQ, &priv->sbus_sleep_work, 1*HZ);
	}
#ifdef WORKAROUND
	up(&priv->sem);
#endif
	return;


err_out:
	DEBUG(DBG_SBUS, "SBUS_bh():FATAL Error:Shutdown driver \n");
#ifdef WORKAROUND
	up(&priv->sem);
#endif
	atomic_xchg(&(priv->cw1200_unresponsive), TRUE);
	flush_workqueue(priv->rx_WQ);
	schedule_delayed_work(&sdio_cw_priv.unload_work, 0);
	return;

}

#ifndef USE_SPI
#ifndef GPIO_BASED_IRQ
void cw1200_sbus_interrupt(CW1200_bus_device_t *func)
{
	struct CW1200_priv *priv = NULL;

	DEBUG(DBG_SBUS, "************cw1200_sbus_interrupt Called \n");
	priv = sdio_cw_priv.driver_priv;

	atomic_xchg(&(priv->Interrupt_From_Device), TRUE);
	queue_work(priv->sbus_WQ, &priv->sbus_work);
	/* Do not put device to sleep*/
	cancel_delayed_work(&priv->sbus_sleep_work);
}
#endif
#else
irqreturn_t cw1200_sbus_interrupt(int irq, void *dev_id)
{
	struct CW1200_priv *priv = dev_id;

	atomic_xchg(&(priv->Interrupt_From_Device), TRUE);
	queue_work(priv->sbus_WQ, &priv->sbus_work);
	return IRQ_HANDLED;
}
#endif

/*******************************************************************
*UMAC Callback Functions
********************************************************************/

/**
* UMI_CB_ScheduleTx
*
* This function writes the buffer onto the device using the SDIO Host
* Controller Driver. Returns the Status Code
*
* @LowerHandle: Pointer to the lower driver. instance for UMI callback.
*/
uint8_t  UMI_CB_ScheduleTx(LL_HANDLE LowerHandle)
{
	struct CW1200_priv *priv = (struct CW1200_priv *)LowerHandle;


	atomic_xchg(&(priv->Umac_Tx), TRUE);

	/*Check if TX is possible */
	if (atomic_read(&(priv->num_unprocessed_buffs_in_device)) <
		priv->max_num_buffs_supp) {
		queue_work(priv->sbus_WQ, &priv->sbus_work);
		/* Do not put device to sleep*/
		cancel_delayed_work(&priv->sbus_sleep_work);
	} else {
		DEBUG(DBG_SBUS, "UMI_CB_ScheduleTx: priv->num_unprocessed_buffs_in_device = %d max_buffer in device %d\n",
			atomic_read(&(priv->num_unprocessed_buffs_in_device)), priv->max_num_buffs_supp);
	}
	return UMI_STATUS_SUCCESS;
}


/**
* UMI_CB_RxComplete
*
* This callback function will tell lower layer driver that receive has been
* finished.
*
* @LowerHandle: Pointer to the lower driver. instance for UMI callback.
* @Frame: Pointer to the received frame.
*/
void UMI_CB_RxComplete(LL_HANDLE LowerHandle, void *pFrame, void *pDriverInfo)
{
	if (NULL != pDriverInfo)
		dev_kfree_skb_any(pDriverInfo);
	else if (NULL != pFrame)
		kfree(pFrame);
}


/**
* UMI_CB_Create
*
* This callback function will tell lower layer driver that receive has been
* finished. And returns the Handle to Lower Layer.
*
* @UMACHandle: Handle to  UMAC instance.
* @ulHandle: Upper layer driver handle.
*/
LL_HANDLE  UMI_CB_Create(UMI_HANDLE UMACHandle, UL_HANDLE ulHandle)
{
	int32_t retval = SUCCESS;
	int32_t CUT1X = FALSE;
	struct CW1200_priv *priv;
	uint32_t configmask_cw1200 = 0;
	uint32_t configmask_silicon_vers = 0;
	uint32_t buffer = 0;
	uint32_t buffer_ar;
	uint32_t buffer_ar1;
	uint32_t buffer_ar2;
	uint32_t dpll_buff = 0;
	uint32_t dpll_buff_read = 0;
	uint32_t i = 0;
	uint32_t cont_reg_val = 0  ;
	uint32_t config_reg_val = 0 ;

	DEBUG(DBG_SBUS, "%s: called\n", __func__);

	priv = (struct CW1200_priv *)ulHandle;
	priv->lower_handle = UMACHandle;
	spin_lock_init(&(priv->pm_lock));

	/*Create WORK Queue */
	priv->sbus_WQ = create_singlethread_workqueue("sbus_work");
	if (NULL == priv->sbus_WQ) {
		DEBUG(DBG_ERROR, "%s Failed to allocate memory for rx_WQ\n",
								__func__);
		return NULL;
	}
	/*1. Register BH */
	INIT_WORK(&priv->sbus_work, SBUS_bh);
	INIT_DELAYED_WORK(&priv->sbus_sleep_work, SBUS_Sleep_Work);
	INIT_LIST_HEAD(&priv->rx_list);
	priv->rx_WQ = create_singlethread_workqueue("rx_work");

	if (NULL == priv->rx_WQ) {
		DEBUG(DBG_ERROR, "%s Failed to allocate memory for rx_WQ\n",
								__func__);
		destroy_workqueue(priv->sbus_WQ);
		return NULL;
	}
	INIT_WORK(&priv->rx_list_work, rx_list_bh);
	mutex_init(&priv->rx_list_lock);
	/*Register Wake Lock*/
	wake_lock_init(&priv->wake_lock, WAKE_LOCK_SUSPEND, "cw1200_wlan");
	wake_lock_init(&priv->wake_lock_rx, WAKE_LOCK_SUSPEND, "cw1200_wlan_rx");
#ifdef WORKAROUND
	sema_init(&priv->sem, 1);
#endif

	/*Init device buffer numbers */
	priv->sdio_wr_buf_num_qmode = 0;
	priv->sdio_rd_buf_num_qmode = 1;

	/*Initialise Interrupt from device flag to FALSE */
	atomic_set(&(priv->Interrupt_From_Device), FALSE);
	atomic_set(&(priv->Umac_Tx), FALSE);
	atomic_set(&(priv->cw1200_unresponsive), FALSE);

	priv->suspend_block = false;
#ifndef USE_SPI
	sdio_claim_host(priv->func);
	DEBUG(DBG_SBUS, "SDIO - VENDORID [%x], DEVICEID [%x] \n",
	priv->func->vendor, priv->func->device);

	retval = sdio_enable_func(priv->func);
	if (retval) {
		DEBUG(DBG_ERROR, "%s, Error :[%d] \n", __func__ , retval);
		destroy_workqueue(priv->sbus_WQ);
		destroy_workqueue(priv->rx_WQ);
		sdio_release_host(priv->func);
		return NULL;
	}
	sdio_release_host(priv->func);

	sdio_cw_priv.driver_priv = priv;
	sdio_set_drvdata(priv->func, &sdio_cw_priv);
#else
	retval = request_irq(priv->func->irq, cw1200_sbus_interrupt,
				IRQF_TRIGGER_FALLING, "CW1200_spi", priv);
	if (retval) {
		DEBUG(DBG_ERROR, "%s, request_irq() return error :[%d] \n",
			__func__ , retval);
		return NULL;
	}

	spi_set_drvdata(priv->func, priv);
#endif

	/*Read CONFIG Register Value - We will read 32 bits*/
	retval = SBUS_SDIO_RW_Reg(priv, ST90TDS_CONFIG_REG_ID,
			(char *)&config_reg_val, BIT_32_REG, SDIO_READ);
	if (retval)
		return NULL;

	DEBUG(DBG_MESSAGE, "Initial 32 Bit CONFIG Register Value: [%x] \n",
			config_reg_val);

	configmask_cw1200 = (config_reg_val >> 24) & 0x3;
	configmask_silicon_vers = (config_reg_val >> 31) & 0x1;

	/*Check if we have CW1200 or STLC9000 */
	if (configmask_cw1200 == 0x1) {
		PRINT("CW1200 Silicon Detected CUT1.X \n");
		if (configmask_silicon_vers)
			priv->hw_type = HIF_8601_VERSATILE;
		else
			priv->hw_type = HIF_8601_SILICON;
		dpll_buff = DPLL_INIT_VAL_CW1200;
		CUT1X=TRUE;
	} else if (configmask_cw1200 == 0x2) {
		PRINT("CW1200 CUT2.X silicon deteced \n");

		priv->hw_type = HIF_8601_SILICON;
		dpll_buff = DPLL_INIT_VAL_CW1200;
	} else {
		PRINT("STLC 9000 Silicon Detected \n");
		priv->hw_type = HIF_9000_SILICON_VERSTAILE;
		dpll_buff = DPLL_INIT_VAL_9000;
	}

	retval = SBUS_SDIO_RW_Reg(priv, ST90TDS_TSET_GEN_R_W_REG_ID,
				(uint8_t *)&dpll_buff, BIT_32_REG, SDIO_WRITE);
	if (retval)
		return NULL;

	msleep(20);

	/*Read DPLL Reg value and compare with value written */
	retval = SBUS_SDIO_RW_Reg(priv, ST90TDS_TSET_GEN_R_W_REG_ID,
			(uint8_t *)&dpll_buff_read, BIT_32_REG, SDIO_READ);
	if (retval)
		return NULL;

	if (dpll_buff_read != dpll_buff) {
		DEBUG(DBG_ERROR, "Unable to initialise DPLL register."
			"Value Read is : [%x] \n", dpll_buff_read);
	}

	/*Set Wakeup bit in device */
	retval = SBUS_SDIO_RW_Reg(priv, ST90TDS_CONTROL_REG_ID,
				(char *)&cont_reg_val, BIT_16_REG, SDIO_READ);
	if (retval)
		return NULL;

	cont_reg_val = cpu_to_le16(cont_reg_val | ST90TDS_CONT_WUP_BIT) ;

	retval = SBUS_SDIO_RW_Reg(priv, ST90TDS_CONTROL_REG_ID,
			(char *)&cont_reg_val, BIT_16_REG, SDIO_WRITE);
	if (retval)
		return NULL;

	for (i = 0 ; i < 300 ; i++) {
		msleep(1);
		retval = SBUS_SDIO_RW_Reg(priv, ST90TDS_CONTROL_REG_ID,
				(char *)&cont_reg_val, BIT_16_REG, SDIO_READ);
		if (retval)
			return NULL;

		if (cont_reg_val & ST90TDS_CONT_RDY_BIT) {
			DEBUG(DBG_SBUS, "WLAN is ready"
					"[%x], [%x]\n", i, cont_reg_val);
			break;
		}
	}

	if (i >= 300) {
		DEBUG(DBG_SBUS, "WLAN READY Bit not set \n");
		return NULL;
	}

	priv->device_sleep_status = DEVICE_DOWN;

	/* CW1200 Hardware detection logic : Check for CUT1 subtypes if not
	CUT2.0 */
	if (CUT1X == TRUE) {
		retval = SBUS_SramRead_AHB(priv, CUT_ID_ADDR ,
						(uint8_t *)&buffer, 4);
		if (retval) {
			DEBUG (DBG_ERROR, "%s:SBUS_SramRead_AHB()"
				"returned error :[%d] \n",__FUNCTION__, retval);
			return NULL;
		}
		DEBUG(DBG_SBUS , " Identification Buffer: [%x] \n", buffer);
		if (CUT11_ID_STR == buffer) {
			PRINT("SBUS_Create():CUT1.1 Silicon Detected \n");
			priv->cw1200_cut_no = CUT11;
		} else {
			PRINT("SBUS_Create():CUT1.0 Silicon Detected \n");
			priv->cw1200_cut_no = CUT10;
		}
	} else {
		SBUS_SramRead_AHB(priv, CUT2_ID_ADDR,
						(uint8_t *)&buffer_ar,4);
		SBUS_SramRead_AHB(priv,CUT2_ID_ADDR + 4,
						(uint8_t *)&buffer_ar1,4);
		SBUS_SramRead_AHB(priv,CUT2_ID_ADDR + 8,
						(uint8_t *)&buffer_ar2,4);
		PRINT("\n %x\t%x\t%x \n", buffer_ar,buffer_ar1,buffer_ar2);
		if ((buffer_ar == CUT22_ID_STR1) &&
				(buffer_ar1 == CUT22_ID_STR2) &&
				((uint16_t)buffer_ar2 == CUT22_ID_STR3)) {
			PRINT("CW1200 CUT2.2 silicon deteced \n");
			priv->cw1200_cut_no = CUT22;
		} else {
			PRINT("CW1200 CUT2.0 silicon deteced \n");
			priv->cw1200_cut_no = CUT2;
		}
	}
	/*The Lower layer handle is the same as the upper
	layer handle - device priv */
	return (LL_HANDLE *)priv;
}


/**
* UMI_CB_Start
*
* This callback function will start the firmware downloading.
* returns the UMI Status Code.
*
* @LowerHandle: Handle to lower driver instance.
* @FmLength: Length of firmware image.
* @FirmwareImage: Path to the Firmware Image.
*/
UMI_STATUS_CODE  UMI_CB_Start(LL_HANDLE	LowerHandle,
		uint32 FmLength, void *FirmwareImage)
{

	struct CW1200_priv *priv = (struct CW1200_priv *)LowerHandle;
	UMI_STATUS_CODE retval = UMI_STATUS_SUCCESS;
	uint32_t config_reg_val = 0 ;
	uint32_t cont_reg_val = 0  ;
	uint32_t config_reg_len = BIT_32_REG;
#ifdef GPIO_BASED_IRQ
	int32_t err_val=0;
	uint8_t cccr=0;
	u32 func_num;
#endif

	if (HIF_8601_SILICON != priv->hw_type) {
		config_reg_len = BIT_16_REG;
	}
	/*Checking for access mode */
	retval = SBUS_SDIO_RW_Reg(priv, ST90TDS_CONFIG_REG_ID,
			(char *)&config_reg_val, config_reg_len, SDIO_READ);
	if (retval)
		return UMI_STATUS_FAILURE;

	if (config_reg_val & ST90TDS_CONFIG_ACCESS_MODE_BIT) {
		DEBUG(DBG_SBUS, "We are in DIRECT Mode\n");
	} else {
		DEBUG(DBG_ERROR, " ERROR: We are in QUEUE Mode\n");
		return UMI_STATUS_FAILURE;
	}
	/*5. Call function to download firmware */
	/*If there was some error.Report ERROR to WDEV */
	if (HIF_8601_SILICON  == priv->hw_type) {
		retval = CW1200_UploadFirmware(priv, FirmwareImage, FmLength);
		if (retval)
			return retval;
	} else {
		retval = CW1100_UploadFirmware(priv, FirmwareImage, FmLength);
		if (retval)
			return retval;
	}

	/*If the device is STLC9000 the device is
	removed from RESET after firmware download */
	if (HIF_9000_SILICON_VERSTAILE  == priv->hw_type) {
		/*7. Enable CLOCK and Remove device from RESET */
		retval = SBUS_SDIO_RW_Reg(priv, ST90TDS_CONFIG_REG_ID,
			(char *)&config_reg_val, config_reg_len, SDIO_READ);
		if (retval)
			return retval;
		config_reg_val = config_reg_val & ST90TDS_CONFIG_CPU_RESET_MASK;
		SBUS_SDIO_RW_Reg(priv, ST90TDS_CONFIG_REG_ID,
			(char *)&config_reg_val, config_reg_len, SDIO_WRITE);

		config_reg_val = config_reg_val &
			ST90TDS_CONFIG_CPU_CLK_DIS_MASK; /*Enable Clock */
		SBUS_SDIO_RW_Reg(priv, ST90TDS_CONFIG_REG_ID,
			(char *)&config_reg_val, config_reg_len, SDIO_WRITE);
		mdelay(100);
	}


#ifdef GPIO_BASED_IRQ
	retval = request_any_context_irq(wpd->irq->start,
				cw1200_gpio_irq,IRQF_TRIGGER_RISING,
				wpd->irq->name, priv);

	if ((retval == 0) || (retval == 1)) {
		DEBUG(DBG_SBUS, "%s,WLAN_IRQ registered on GPIO\n", __func__);
	} else
		DEBUG(DBG_SBUS, "%s,Unable to request IRQ on GPIO\n", __func__);

	retval = enable_irq_wake(wpd->irq->start);
	if(retval) {
		DEBUG(DBG_SBUS, "%s Unable to set enable irq wake", __func__);
	}

	/* Hack to access Fuction-0 */
	func_num = priv->func->num;
	priv->func->num = 0;
	sdio_claim_host(priv->func);
	cccr = sdio_readb(priv->func, SDIO_CCCR_IENx,&err_val);
	if (err_val) {
		sdio_release_host(priv->func);
		return UMI_STATUS_FAILURE;
	}

	/* We are function number 1 */
	cccr |= BIT(func_num);
	cccr |= BIT(0); /* Master interrupt enable */

	sdio_writeb(priv->func, cccr, SDIO_CCCR_IENx,&err_val);
	sdio_release_host(priv->func);
	/* Restore the WLAN function number */
	priv->func->num = func_num;

	if (err_val) {
		DEBUG(DBG_SBUS, "%s,F0 write failed\n", __func__);
		return UMI_STATUS_FAILURE;
	}

#else /* Register SDIO based interrupts */

	DEBUG(DBG_MESSAGE, "Going to register SDIO ISR\n");
	sdio_claim_host(priv->func);
	/*Register Interrupt Handler */
	retval = sdio_claim_irq(priv->func, cw1200_sbus_interrupt);
	sdio_release_host(priv->func);
	if (retval) {
		DEBUG(DBG_ERROR, "%s, sdio_claim_irq() return error :[%d]\n",
				__func__ , retval);
		return UMI_STATUS_FAILURE;
	}
#endif /*GPIO_BASED_IRQ*/

	sdio_claim_host(priv->func);
	/* Set better block size for SDIO */
	DEBUG(DBG_MESSAGE, "Setting SDIO block size %d\n", SDIO_BLOCK_SIZE);
	retval = sdio_set_block_size(priv->func, SDIO_BLOCK_SIZE);
	if (retval) {
		sdio_release_host(priv->func);
		DEBUG(DBG_ERROR, "%s: sdio_set_block_size() error :[%d]\n",
				__func__ , retval);
		return UMI_STATUS_FAILURE;
	}
	/* Notify SDIO that WLAN chip will remain powered during suspend */
	retval = sdio_set_host_pm_flags(priv->func, MMC_PM_KEEP_POWER);
	sdio_release_host(priv->func);
	if (retval) {
		DEBUG(DBG_ERROR, "Error while setting SDIO pm options: %i\n",
				retval);
		return UMI_STATUS_FAILURE;
	}
	/*If device is CW1200 IRQ enable/disable bits are in CONFIG register */
	if (HIF_8601_SILICON  == priv->hw_type) {
		retval = SBUS_SDIO_RW_Reg(priv, ST90TDS_CONFIG_REG_ID,
						(char *)&config_reg_val,
						config_reg_len, SDIO_READ);
		if (retval)
			return retval;

		config_reg_val = config_reg_val | ST90TDS_CONF_IRQ_RDY_ENABLE;
		DEBUG(DBG_MESSAGE, "SBUS_SDIO_RW_Reg called\n");
		SBUS_SDIO_RW_Reg(priv, ST90TDS_CONFIG_REG_ID,
				(char *)&config_reg_val,
				config_reg_len, SDIO_WRITE);
	} else {
		/*If device is STLC9000 the IRQ enable/disable bits
		are in CONTROL register */
		/*Enable device interrupts - Both DATA_RDY and WLAN_RDY */

		retval = SBUS_SDIO_RW_Reg(priv, ST90TDS_CONTROL_REG_ID,
					(char *)&cont_reg_val,
					config_reg_len, SDIO_READ);
		if (retval)
			return retval;
		/*Enable SDIO Host Interrupt */
		/*Enable device interrupts - Both DATA_RDY and WLAN_RDY */
		cont_reg_val =
		cpu_to_le16(cont_reg_val | ST90TDS_CONT_IRQ_RDY_ENABLE);

		SBUS_SDIO_RW_Reg(priv, ST90TDS_CONTROL_REG_ID,
				(char *)&cont_reg_val,
				config_reg_len, SDIO_WRITE);
	}

	/*Configure device for MESSSAGE MODE */

	config_reg_val = config_reg_val & ST90TDS_CONFIG_ACCESS_MODE_MASK;
	SBUS_SDIO_RW_Reg(priv, ST90TDS_CONFIG_REG_ID,
			(char *)&config_reg_val,
			config_reg_len, SDIO_WRITE);

/*Unless we read the CONFIG Register we are not able to get an interrupt */
	mdelay(10);
	SBUS_SDIO_RW_Reg(priv, ST90TDS_CONFIG_REG_ID,
			(char *)&config_reg_val,
			config_reg_len, SDIO_READ);

#ifdef WORKAROUND
	irq_poll_init(priv);
#endif

	return retval;
}


/**
* UMI_CB_Stop
*
* This callback function will stop the  lower layer.
* returns the UMI Status Code.
*
* @LowerHandle: Handle to lower driver instance.
*/
UMI_STATUS_CODE  UMI_CB_Stop(LL_HANDLE  	LowerHandle)
{
	struct CW1200_priv *priv = (struct CW1200_priv *)LowerHandle;
	uint32_t lock_status = FALSE;

	DEBUG(DBG_SBUS, "UMI_CB_Stop Called \n");

#ifndef USE_SPI
	if (spin_is_locked(&(priv->cil_lock)))
		lock_status = TRUE;

	if (lock_status)
		spin_unlock_bh(&(priv->cil_lock));

#ifndef GPIO_BASED_IRQ
	sdio_claim_host(priv->func);
	sdio_release_irq(priv->func);
	sdio_release_host(priv->func);
#endif
#ifdef WORKAROUND
	irq_poll_destroy(priv);
#endif

	if (lock_status)
		spin_lock_bh(&(priv->cil_lock));
#endif
	return UMI_STATUS_SUCCESS;
}


/**
* UMI_CB_Destroy
*
* This callback function will destroy lower layer.
* returns the UMI Status Code.
*
* @LowerHandle: Handle to lower driver instance.
*/
UMI_STATUS_CODE  UMI_CB_Destroy(LL_HANDLE	LowerHandle)
{
	struct CW1200_priv *priv = (struct CW1200_priv *)LowerHandle;

	DEBUG(DBG_SBUS, "UMI_CB_Destroy Called\n");

	cancel_delayed_work_sync(&priv->sbus_sleep_work);
	flush_workqueue(priv->sbus_WQ);
	destroy_workqueue(priv->sbus_WQ);
	wake_lock_destroy(&priv->wake_lock);
	wake_lock_destroy(&priv->wake_lock_rx);

	/*New change*/
	flush_workqueue(priv->rx_WQ);
	destroy_workqueue(priv->rx_WQ);

#ifdef GPIO_BASED_IRQ
	disable_irq_wake(wpd->irq->start);
	free_irq(wpd->irq->start, priv);
#endif

#ifndef USE_SPI
	sdio_claim_host(priv->func);
	sdio_disable_func(priv->func);
	sdio_release_host(priv->func);
#else
	free_irq(priv->func->irq, priv);
#endif

	return UMI_STATUS_SUCCESS;
}


static uint32_t SBUS_Sleep_Device(struct CW1200_priv *priv)
{
	uint32_t cont_reg_val = 0;
	uint32_t retval = SUCCESS;

	/*Set Wakeup bit in device */
	retval = SBUS_SDIO_RW_Reg(priv, ST90TDS_CONTROL_REG_ID,
				(char *)&cont_reg_val, BIT_16_REG, SDIO_READ);
	if (retval)
		return ERROR;

	/* Clear wakeup bit in device */
	cont_reg_val = cpu_to_le16(cont_reg_val & ST90TDS_CONT_WUP_BIT_MASK) ;

	retval = SBUS_SDIO_RW_Reg(priv, ST90TDS_CONTROL_REG_ID,
			(char *)&cont_reg_val, BIT_16_REG, SDIO_WRITE);
	if (retval)
		return ERROR;

	return SUCCESS;
}

#if 0
static uint32_t SBUS_Device_Status(struct CW1200_priv *priv)
{
	uint32_t cont_reg_val = 0;
	uint32_t retval;

	retval = SBUS_SDIO_RW_Reg(priv, ST90TDS_CONTROL_REG_ID,
			(char *)&cont_reg_val, BIT_16_REG, SDIO_READ);

	if (retval) {
		DEBUG(DBG_ERROR, "%s,Error in SDIO:%d \n", __func__, retval);
		return ERROR;
	}

	if (cont_reg_val & ST90TDS_CONT_RDY_BIT) {
		return DEVICE_UP;
	} else {
		return DEVICE_DOWN;
	}
}
#endif
static uint32_t SBUS_Wakeup_Device(struct CW1200_priv *priv)
{
	uint32_t cont_reg_val = 0;
	uint32_t retval = SUCCESS;
	uint8_t i = 0;

	/*Set Wake Lock*/
	if(!priv->suspend_block) {
		wake_lock(&priv->wake_lock);
		priv->suspend_block = true;
	}

	cont_reg_val = cpu_to_le16(cont_reg_val | ST90TDS_CONT_WUP_BIT) ;
	retval = SBUS_SDIO_RW_Reg(priv, ST90TDS_CONTROL_REG_ID,
			(char *)&cont_reg_val, BIT_16_REG, SDIO_WRITE);

	/* Check if device woke up*/
	for(i = 0; i < 10; i++) {
		retval = SBUS_SDIO_RW_Reg(priv, ST90TDS_CONTROL_REG_ID,
			(char *)&cont_reg_val, BIT_16_REG, SDIO_READ);
		if (retval)
			return ERROR;

		if (cont_reg_val & ST90TDS_CONT_RDY_BIT)
			priv->device_sleep_status = DEVICE_UP;
		else
			return ERROR;
	}
	DEBUG(DBG_SBUS, "WLAN woke up"
			"[%x]\n", cont_reg_val);
	return SUCCESS;
}

void cw1200_stay_awake(struct CW1200_priv *priv,
                          unsigned long tmo)
{
	long cur_tmo;
	spin_lock_bh(&priv->pm_lock);
	cur_tmo = priv->wake_lock_rx.expires - jiffies;
	if (!wake_lock_active(&priv->wake_lock_rx) ||
		cur_tmo < (long)tmo)
	wake_lock_timeout(&priv->wake_lock_rx, tmo);
	spin_unlock_bh(&priv->pm_lock);
}

void SBUS_Sleep_Work(struct work_struct *work)
{
	struct CW1200_priv *priv =
		container_of(work, struct CW1200_priv, sbus_sleep_work.work);
	uint32_t retval = SUCCESS;

	/*Release the Wake Lock*/
	if(priv->suspend_block) {
		wake_unlock(&priv->wake_lock);
		priv->suspend_block = false;
	}

	retval = SBUS_Sleep_Device(priv);

	if (SUCCESS == retval) {
		DEBUG(DBG_SBUS, "%s, Device put in sleep mode \n", __func__);
		priv->device_sleep_status = DEVICE_DOWN;
	}
}

static inline CW1200_STATUS_E enqueue_rx_data(struct CW1200_priv *priv,
					struct sk_buff *skb, uint8_t *data)
{
	struct cw1200_rx_buf *rx_buf = NULL;

	rx_buf = kzalloc(sizeof(struct cw1200_rx_buf), GFP_KERNEL);

	if (!rx_buf)
		return ERROR;

	rx_buf->skb = skb;
	rx_buf->data = data;

	mutex_lock(&priv->rx_list_lock);
	list_add_tail(&rx_buf->list, &priv->rx_list);
	mutex_unlock(&priv->rx_list_lock);

	queue_work(priv->rx_WQ, &priv->rx_list_work);
	return SUCCESS;
}

static void rx_list_bh(struct work_struct *work)
{
	struct cw1200_rx_buf *rx_buf = NULL, *next = NULL;
	struct CW1200_priv *priv =
			container_of(work, struct CW1200_priv, rx_list_work);

	DEBUG(DBG_SBUS, "%s,called\n", __func__);

	mutex_lock(&priv->rx_list_lock);
	if (!list_empty(&priv->rx_list)) {
		list_for_each_entry_safe(rx_buf, next, &priv->rx_list, list) {
			spin_lock_bh(&(priv->cil_lock));
			UMI_ReceiveFrame(priv->lower_handle, rx_buf->data,
					rx_buf->skb);
			spin_unlock_bh(&(priv->cil_lock));
			list_del(&rx_buf->list);
			kfree(rx_buf);
		}
	}
	mutex_unlock(&priv->rx_list_lock);
}

