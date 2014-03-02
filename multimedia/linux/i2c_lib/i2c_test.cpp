/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "MMIO_Camera.h"
#include "mmio_linux_api.h"
#include "i2c_lib.h"

#define DEBUG
#ifdef DEBUG
#define DEBUG_LOG(fmt, args...) printf("i2c-test:%s:"fmt,__FUNCTION__, ## args)
#else
#define DEBUG_LOG(...)
#endif

#define I2C_TEST_CALL(func, ...) do{					\
	int error = func(__VA_ARGS__);							\
	if (error) {											\
		DEBUG_LOG("Error returned by " #func ":err:%d",error);	\
		goto out;											\
	}														\
} while(0)

#define TEST_DEV_PRI_SENSOR 

#ifdef TEST_DEV_PRI_SENSOR 
#define TEST_ADAPTER		(I2C_ADAP_2)
#define SLAVE_ADDR 			(0x20) /*Sensor slave addr*/
/*Reg addresses are randomly picked for test, and they are for toshiba
 *8MP sensor. there is no significance of these Read/write
 *operations, they are just used for testing I2C API
 */
#define READ_REG_ADDR_8 	(0x01)  /*Expected value: 0x3E */
#define READ_REG_ADDR_16	(0x1120)/*Expected value: 00 01 0C 43 96 00 00 44 2A */ 	
#define WRITE_REG_ADDR_8	 (0x80)
#define WRITE_REG_ADDR_16	 (0x0112)
#define WRITE_VAL_8			 (0x0A)
#else
#define TEST_ADAPTER		(I2C_ADAP_0)
#define SLAVE_ADDR 			(0x0)			
#define READ_REG_ADDR_8 	(0x0)
#define READ_REG_ADDR_16	(0x0)
#define WRITE_REG_ADDR_8	(0x0)
#define WRITE_REG_ADDR_16	(0x0)
#define WRITE_VAL_8			(0x0)
#endif

#define BUF_MAX_LEN			 (10)

int test_read(i2clib_handle_t handle){
	int error=0,i;
	unsigned char buf[BUF_MAX_LEN];
	unsigned char *val_8;

	memset(&buf,0,BUF_MAX_LEN);
#if 0	
	I2C_TEST_CALL(i2c_read,handle,&buf[0],BUF_MAX_LEN,&actual_len,20);
	DEBUG_LOG("Read Block:curr, no offset:\n");
	for(i=0;i<BUF_MAX_LEN;i++){
		DEBUG_LOG(" %x\n",buf[i]);
	}
	val_8 = &buf[0];
	*val_8=0;
	*val_8 = i2c_read_reg8_byte(handle,READ_REG_ADDR_8);	
	DEBUG_LOG("Read_8_8: [%x]:%x\n",READ_REG_ADDR_8,*val_8);
#endif	
	DEBUG_LOG("Read block @ 16bit address\n");
	memset(&buf,0,BUF_MAX_LEN);
	I2C_TEST_CALL(i2c_read_reg16_bytes,handle,READ_REG_ADDR_16,&buf[0],BUF_MAX_LEN);
	for(i=0;i<BUF_MAX_LEN;i++){
		DEBUG_LOG(" [%x]:0x%x\n",(READ_REG_ADDR_16 +i),buf[i]);
	}

	val_8 = &buf[0];
	*val_8 = 0;
	*val_8 = i2c_read_reg16_byte(handle,READ_REG_ADDR_16);	
	DEBUG_LOG("Read_16_8: [%x]:0x%x\n",READ_REG_ADDR_16,*val_8);
out: 
	return error;
}

int test_write(i2clib_handle_t handle){
	int error=0,i;
	unsigned char buf[BUF_MAX_LEN];
	unsigned char buf_rb[BUF_MAX_LEN];
	
	buf[0] = 0x08;
	buf[1] = 0x0A;
	DEBUG_LOG("Write block:\n");
	I2C_TEST_CALL(i2c_write_reg16_bytes,handle,WRITE_REG_ADDR_16,&buf[0],2);
	memset(&buf_rb,0,BUF_MAX_LEN);
	I2C_TEST_CALL(i2c_read_reg16_bytes,handle,WRITE_REG_ADDR_16,&buf_rb[0],2);
	DEBUG_LOG("Read bak:\n");
	for(i=0;i< 2;i++){
		DEBUG_LOG("[%x] wr - 0x%x  rb - 0x%x\n",(WRITE_REG_ADDR_16 +i),
			buf[i],buf_rb[i]);
	}
	DEBUG_LOG("Write_16_8 :\n");
	I2C_TEST_CALL(i2c_write_reg16_byte,handle,WRITE_REG_ADDR_16,WRITE_VAL_8);
	buf_rb[0] = 0;
	buf_rb[0] = i2c_read_reg16_byte(handle,WRITE_REG_ADDR_16);	
	DEBUG_LOG("Rd back:[%x]:wr 0x%x rb 0x%x\n",WRITE_REG_ADDR_16,
			WRITE_VAL_8,buf_rb[0]);
out: 
	return error;
}
int main()
{
	i2clib_handle_t i2c_handle;
	i2c_init_params_t init_param;
	int error;

	/*USE mmio API to enable i2c2. By default MMIO camera driver
	 *configures GPIO 8,9 (used as SDA and SDL) in IPI2C mode (Alt A), in which they 
	 *are driven by f/w, In order to drive drive these GPIOs by I2C adapter 2
	 *the need to be configured in I2C2 mode (Alt B)
	 */
	MMIO_Camera::initBoard();
	MMIO_Camera::powerSensor(OMX_TRUE);
	enable_xshutdown_from_host(OMX_TRUE,OMX_TRUE);
	MMIO_Camera::setExtClk(OMX_TRUE);
	MMIO_Camera::setPrimaryCameraHWInterface();
	activate_i2c2(OMX_TRUE);
   	
	init_param.adapter = TEST_ADAPTER; 
   	init_param.slave_addr = SLAVE_ADDR;
	I2C_TEST_CALL(i2c_init,&init_param,&i2c_handle);
#if 0
	error = test_write(i2c_handle);
	if(error)
		goto out;
#endif		
	error = test_read(i2c_handle);
	if(error)
		goto out;
	DEBUG_LOG("All Passed\n");
	i2c_deinit(i2c_handle);
	return 0;
out:
	DEBUG_LOG("Test Failed\n");
	i2c_deinit(i2c_handle);
    return 0;
}
