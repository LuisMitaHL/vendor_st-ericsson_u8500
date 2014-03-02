/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <fcntl.h> 
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>
#include <sys/types.h>

/*Local header*/
#include "i2c_lib.h"
#ifdef DEBUG
#define DEBUG_LOG(fmt, args...) printf("i2c-lib:%s:"fmt,__FUNCTION__, ## args)
#else
#define DEBUG_LOG(...)
#endif

#define I2C_DO_IOCTL(_fd,_ioctl,_arg) do {						\
	error = ioctl(_fd,_ioctl,_arg);								\
	if(error < 0){												\
		DEBUG_LOG("IOCTL %x failed, error %d\n",_ioctl,errno);	\
		error = errno;											\
		goto out;												\
	}															\
}while(0)

#define I2C_CREATE_MSG(_msg,_slave,_flags,_buf,_len) do {		\
	memset((_msg),0,sizeof(*_msg));								\
	(_msg)->addr = _slave ;										\
	(_msg)->flags = _flags;										\
	(_msg)->buf  = _buf;										\
	(_msg)->len  = _len;										\
}while(0)	

#define I2C_DEV_BASE_NAME	"/dev/i2c"

typedef struct i2c_priv{
	int32_t fd;	
	enum i2c_adaptor_no adapter;
	u_int16_t slave_addr;
	u_int16_t time_out;
}i2c_priv_t;

/* Do conversion of error numbers if required 
 * currently we do'nt need any conversion 
 */
u_int32_t convert_error(u_int32_t error)
{
	return error;
}

u_int32_t i2c_init(i2c_init_params_t *init_param,i2clib_handle_t *handle)
{
	char dev_name[20];
	int32_t error=0;
	i2c_priv_t *i2c_priv_p=NULL;
	
	if(!init_param || !handle){
		DEBUG_LOG("Invalid: init_param %p, handle %p\n",init_param,
			handle);
		error = EINVAL;
		goto out;
	}

	if(init_param->adapter >= I2C_ADAP_MAX){
		DEBUG_LOG("Invalid adapter %d\n",init_param->adapter);
		error = EINVAL;
		goto out;
	}
	sprintf(&dev_name[0],"%s-%d",I2C_DEV_BASE_NAME,init_param->adapter);
	DEBUG_LOG("device file %s\n",dev_name);

	i2c_priv_p = (i2c_priv_t *)malloc(sizeof(i2c_priv_t));
	if(!i2c_priv_p){
		DEBUG_LOG("No mem for i2c_priv_p\n");
		error = ENOMEM;
		goto out;
	}
	DEBUG_LOG("Adap %d, slave %d\n",init_param->adapter,init_param->slave_addr);
	i2c_priv_p->fd = open(dev_name,O_RDWR);	
	if(i2c_priv_p->fd <0){
		DEBUG_LOG("Failed to open %s, error %d\n",dev_name,errno);
		error = errno;
		goto out;
	}
	i2c_priv_p->adapter = init_param->adapter;
	i2c_priv_p->slave_addr = init_param->slave_addr;
	
	/*Linux I2C faramework expects address to be in lower 7 bits of address byte
	 */	
	i2c_priv_p->slave_addr = ((i2c_priv_p->slave_addr & SLAVE_ADDR_MASK) >> 1);
	
        // Set Timeout through i2c_set_timeout. Do not set any default timeout.
	//i2c_priv_p->time_out = I2C_DEFAULT_TIMEOUT;
	//I2C_DO_IOCTL(i2c_priv_p->fd,I2C_TIMEOUT,(i2c_priv_p->time_out/10));

    //I2C_DO_IOCTL(i2c_priv_p->fd,I2C_SLAVE,init_param->slave_addr);
	I2C_DO_IOCTL(i2c_priv_p->fd,I2C_SLAVE,i2c_priv_p->slave_addr);
		
	DEBUG_LOG("I2C init done. Adap %d, slave %d",
		init_param->adapter,init_param->slave_addr);
	*handle = i2c_priv_p;

	return convert_error(error);
out:
	if(i2c_priv_p)
        {
            if(i2c_priv_p->fd > 0)
                close(i2c_priv_p->fd);

		free(i2c_priv_p);
        }
	return convert_error(error);
}

u_int32_t i2c_deinit(i2clib_handle_t handle){
	i2c_priv_t *i2c_priv_p= (i2c_priv_t *)handle;
	if(!i2c_priv_p){
		DEBUG_LOG("NULL handle\n");
		return EINVAL;
	}
	if(close(i2c_priv_p->fd) <0){
		DEBUG_LOG("Failed to close: fd %d, adap %d\n",i2c_priv_p->fd,
			i2c_priv_p->adapter);
		/* Anyways we will free stuff, so no return from here*/
	}
	DEBUG_LOG("I2c Deinit done, adap %d\n",i2c_priv_p->adapter);
	free(i2c_priv_p);
	return 0;
}

static u_int32_t i2c_xfer_msgs(i2c_priv_t *i2c_priv_p,struct i2c_msg *msgs,u_int32_t num_mgs)
{
	int32_t error=0;
	struct i2c_rdwr_ioctl_data ioctl_arg;
	ioctl_arg.msgs = msgs;
	ioctl_arg.nmsgs = num_mgs;
	DEBUG_LOG("Adap %d, Slave %d, msgs %p, num %d, ioctl arg %p\n",i2c_priv_p->adapter,
		i2c_priv_p->slave_addr,msgs,num_mgs,&ioctl_arg);
	error = ioctl(i2c_priv_p->fd,I2C_RDWR,&ioctl_arg);
	if(error<0){
		DEBUG_LOG("RdWr failed, err %d\n",errno);
		error = errno;
	}else{
		DEBUG_LOG("RdWr Done\n");
		error = 0;
	}
	return error;
}

u_int32_t i2c_read(i2clib_handle_t handle, u_int8_t *buff,
			u_int32_t max_length,u_int32_t *actual_length)
{
	int32_t error=0;
	struct i2c_msg msg;
	i2c_priv_t *i2c_priv_p= (i2c_priv_t *)handle;
	if(!i2c_priv_p || !buff){
		DEBUG_LOG("Invalid param, priv %x, buf %x\n",i2c_priv_p,buff);
		error = EINVAL;
		goto out;
	}

	I2C_CREATE_MSG(&msg,i2c_priv_p->slave_addr,I2C_M_RD,buff,max_length);
	error = i2c_xfer_msgs(i2c_priv_p,&msg,1);
	if(error)
        {
		DEBUG_LOG("i2c_xfer_msgs failed %d\n",error);
        }
	/*i2c-dev interface doesn't return actual lenght*/
	if(actual_length)
		*actual_length = max_length;
	return convert_error(error);
out:
	if(actual_length)
		*actual_length = 0;
	
	return convert_error(error);
}
u_int32_t i2c_write(i2clib_handle_t handle, u_int8_t *buff,
			u_int32_t length, u_int32_t *length_written,
			u_int8_t no_stop)
{
	int error=0;;
	struct i2c_msg msg;
	i2c_priv_t *i2c_priv_p= (i2c_priv_t *)handle;
	if(!i2c_priv_p || !buff){
		DEBUG_LOG("Invalid param, priv %x, buf %x\n",i2c_priv_p,buff);
		error = EINVAL;
		goto out;
	}

	I2C_CREATE_MSG(&msg,i2c_priv_p->slave_addr,0,buff,length);
	error = i2c_xfer_msgs(i2c_priv_p,&msg,1);
	if(error)
        {
		DEBUG_LOG("i2c_xfer_msgs failed %d\n",error);
        }
	/*i2c-dev interface doesn't return actual length*/
	if(length_written)
		*length_written = length;
	return convert_error(error);
out:	
	if(length_written)
		*length_written = 0;
	return convert_error(error);

}

u_int32_t i2c_read_reg8_bytes(i2clib_handle_t handle, u_int8_t reg,
			u_int8_t *buff,u_int32_t len)
{
	int error =0;
	struct i2c_msg msg[2];
	u_int8_t reg_l=reg;
	i2c_priv_t *i2c_priv_p= (i2c_priv_t *)handle;
	if(!i2c_priv_p || !buff){
		DEBUG_LOG("Invalid param, priv %x, buf %x\n",i2c_priv_p,buff);
		error = EINVAL;
		goto out;
	}
	if(len > I2C_MAX_BUFF_LEN){	
		DEBUG_LOG("Too long buff (%d), truncated to %d\n",len,I2C_MAX_BUFF_LEN);
		len = I2C_MAX_BUFF_LEN;
	}

	I2C_CREATE_MSG(&msg[0],i2c_priv_p->slave_addr,0,&reg_l,1);
	I2C_CREATE_MSG(&msg[1],i2c_priv_p->slave_addr,I2C_M_RD,buff,len);
	error = i2c_xfer_msgs(i2c_priv_p,&msg[0],2);
	if(error)
        {
		DEBUG_LOG("i2c_xfer_msgs failed %d\n",error);
        }
out:
	return convert_error(error);
}

u_int32_t i2c_read_reg16_bytes(i2clib_handle_t handle, u_int16_t reg,
			u_int8_t *buff,u_int32_t len)
{
	int error =0;
	struct i2c_msg msg[2];
	u_int8_t reg_l[2];
	i2c_priv_t *i2c_priv_p= (i2c_priv_t *)handle;
	if(!i2c_priv_p || !buff){
		DEBUG_LOG("Invalid param, priv %x, buf %x\n",i2c_priv_p,buff);
		error = EINVAL;
		goto out;
	}
	if(len > I2C_MAX_BUFF_LEN){	
		DEBUG_LOG("Too long buff (%d), truncated to %d\n",len,I2C_MAX_BUFF_LEN);
		len = I2C_MAX_BUFF_LEN;
	}

	reg_l[0] = (reg >> 8);
	reg_l[1] = (reg & 0xff);
	I2C_CREATE_MSG(&msg[0],i2c_priv_p->slave_addr,0,&reg_l[0],2);
	I2C_CREATE_MSG(&msg[1],i2c_priv_p->slave_addr,I2C_M_RD,buff,len);
	error = i2c_xfer_msgs(i2c_priv_p,&msg[0],2);
	if(error)
        {
		DEBUG_LOG("i2c_xfer_msgs failed %d\n",error);
        }

out:
	return convert_error(error);
}


u_int32_t i2c_write_reg8_bytes(i2clib_handle_t handle, u_int8_t reg,
			u_int8_t *buff,u_int32_t len)
{
	u_int8_t *lbuff_p;
	u_int32_t error;
	if(len > I2C_MAX_BUFF_LEN){	
		DEBUG_LOG("Too long msg (%d), truncated to %d\n",len,I2C_MAX_BUFF_LEN);
		len = I2C_MAX_BUFF_LEN;
	}
	lbuff_p = malloc(len + 1);
	if(!lbuff_p){
		error = ENOMEM;
		DEBUG_LOG("Failed to allocate mem\n");
		goto out;
	}
	lbuff_p[0] = reg;
	memcpy(&lbuff_p[1],buff,len);
	
	error = i2c_write(handle,&lbuff_p[0],(len + 1),NULL,0);	
	if(error)
        {
		DEBUG_LOG("Failed to write reg %x ",reg);
        }
out:
	return convert_error(error);
}

u_int32_t i2c_write_reg16_bytes(i2clib_handle_t handle, u_int16_t reg,
			u_int8_t *buff,u_int32_t len)
{
	u_int8_t *lbuff_p;
	u_int32_t error;
	if(len > I2C_MAX_BUFF_LEN){	
		DEBUG_LOG("Too long msg (%d), truncated to %d\n",len,I2C_MAX_BUFF_LEN);
		len = I2C_MAX_BUFF_LEN;
	}
	lbuff_p = malloc(len + 2);
	if(!lbuff_p){
		error = ENOMEM;
		DEBUG_LOG("Failed to allocate mem\n");
		goto out;
	}
	lbuff_p[0] = (reg >> 8);
	lbuff_p[1] = (reg);
	memcpy(&lbuff_p[2],buff,len);
	
	error = i2c_write(handle,&lbuff_p[0],(len + 2),NULL,0);	
	if(error)
        {
		DEBUG_LOG("Failed to write reg %x",reg);
        }
out:
	return convert_error(error);
}

u_int32_t i2c_write_reg16_byte(i2clib_handle_t handle, u_int16_t reg, u_int8_t val)
{
	int error;	
	u_int8_t buf[3];
	buf[0] = reg >> 8;
	buf[1] = reg & 0xff;
	buf[2] = val;
	error = i2c_write(handle,&buf[0],3,NULL,0);	
	if(error)
        {
		DEBUG_LOG("Failed to write reg %x, val %x",reg,val);
        }
	return convert_error(error);
}

u_int32_t i2c_write_reg8_byte(i2clib_handle_t handle, u_int8_t reg, u_int8_t val)
{
	int error;
	u_int8_t buf[2];
	buf[0] = reg ;
	buf[1] = val ;
	error = i2c_write(handle,&buf[0],2,NULL,0);	
	if(error)
        {
		DEBUG_LOG("Failed to write reg %x, val %x",reg,val);
        }
	return convert_error(error);
}

u_int32_t  i2c_set_timeout (i2clib_handle_t handle, u_int32_t time_out)
{
    int32_t error;
    i2c_priv_t *i2c_priv_p= (i2c_priv_t *)handle;
    i2c_priv_p->time_out = time_out;
    I2C_DO_IOCTL(i2c_priv_p->fd,I2C_TIMEOUT,(i2c_priv_p->time_out/10));

    return convert_error(error);
out:
	if(i2c_priv_p)
    {
        if(i2c_priv_p->fd > 0)
            close(i2c_priv_p->fd);
	    free(i2c_priv_p);
    }
	return convert_error(error);
}
