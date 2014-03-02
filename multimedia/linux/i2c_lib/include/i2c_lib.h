/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __I2C_LIB_H__
#define __I2C_LIB_H__
#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/*Linux specific header*/

#include <sys/types.h>

/* Timeout in ms */

#define REG_VAL_INVAL (~0)
#define I2C_MAX_BUFF_LEN (64)	/* This lenght is can be changed, driver does not limit the lenght,
								 * It is just for buffer validation on user side
								 */
#define SLAVE_ADDR_MASK (0xFE)

enum i2c_adaptor_no{
	I2C_ADAP_0=0,
	I2C_ADAP_1,
	I2C_ADAP_2,
	I2C_ADAP_3,
	I2C_ADAP_4,
	I2C_ADAP_MAX
};

/*Slave address : compete 8 bits of address byte including rd/write bit at bit 0
 * bit 1-7 : slave address, bit 0 : dont care
 */
typedef struct i2c_init_params{
	enum i2c_adaptor_no adapter;
	u_int16_t slave_addr;
}i2c_init_params_t;

typedef void * i2clib_handle_t;
extern u_int32_t i2c_init(i2c_init_params_t *init_param, i2clib_handle_t *handle);
extern u_int32_t i2c_deinit(i2clib_handle_t handle);

/*Plain read/write operations without register/offset address */
extern u_int32_t i2c_read(i2clib_handle_t handle, u_int8_t *buff,
			u_int32_t max_length,u_int32_t *actual_length);
			
extern u_int32_t i2c_write(i2clib_handle_t handle, u_int8_t *buff,
			u_int32_t length, u_int32_t *length_written,
			u_int8_t no_stop);


/*Read multiple bytes specified by 'len' from 8 bit Address */
extern u_int32_t i2c_read_reg8_bytes(i2clib_handle_t handle, u_int8_t reg,
			u_int8_t *buff,u_int32_t len); 	
/*Read multiple bytes specified by 'len' from 16 bit register address */
extern u_int32_t i2c_read_reg16_bytes(i2clib_handle_t handle, u_int16_t reg,
			u_int8_t *buff,u_int32_t len); 

/*Read single byte from 8 bit register Address*/
static inline u_int8_t i2c_read_reg8_byte(i2clib_handle_t handle, u_int8_t reg){
	u_int8_t val;
	u_int32_t err;
	err = i2c_read_reg8_bytes(handle,reg,&val,1);
	if(!err)
		return val;
	else
		return REG_VAL_INVAL; 
}

/*Read single byte from 16 bit register Address*/
static inline u_int8_t i2c_read_reg16_byte(i2clib_handle_t handle, u_int16_t reg){
	u_int8_t val;
	u_int32_t err;
	err = i2c_read_reg16_bytes(handle,reg,&val,1);
	if(!err)
		return val;
	else
		return REG_VAL_INVAL; 
}

/*Write multiple bytes specified by 'len' at 8 bit register Address*/
extern u_int32_t i2c_write_reg8_bytes(i2clib_handle_t handle, u_int8_t reg,
			u_int8_t *buff,u_int32_t len); 	

/*Write multiple bytes specified by 'len' at 16 bit register Address*/
extern u_int32_t i2c_write_reg16_bytes(i2clib_handle_t handle, u_int16_t reg,
			u_int8_t *buff,u_int32_t len); 

/*Write single byte at 8/16 bit register Address*/
u_int32_t i2c_write_reg16_byte(i2clib_handle_t handle, u_int16_t reg, u_int8_t val);
u_int32_t i2c_write_reg8_byte(i2clib_handle_t handle, u_int8_t reg, u_int8_t val);
static inline u_int32_t i2c_write_p(i2clib_handle_t handle, u_int8_t *buff,
			u_int32_t length)
{
return i2c_write(handle, buff,	length, NULL, 0);
}

static inline u_int32_t i2c_read_index16(i2clib_handle_t handle, u_int32_t index1, u_int32_t index2, u_int32_t len, u_int8_t *buff)
{
return i2c_read_reg16_bytes(handle, ((index1&0xff)<<8)|(index2&0xff), buff, len); 
}

extern u_int32_t  i2c_set_timeout (i2clib_handle_t handle, u_int32_t time_out);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif
