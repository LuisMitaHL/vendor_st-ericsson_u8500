/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Public Header file of I2C Controller services module
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef _I2C_SERVICES
#define _I2C_SERVICES

#ifndef _HCL_DEFS_H
#include "hcl_defs.h" 
#endif
#include "services.h"
#include "i2c.h"

#if ((defined ST_8500V1)||(defined ST_HREFV1)||(defined __PEPS_8500_V1)||(defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))

#define PRCC_KERNEL_CLK_EN_OFFSET 0x8

#define I2C0_AMBA_CLK_EN_VAL      0x00000008
#define I2C0_KERNEL_CLK_EN_VAL    0x00000008

#define I2C1_AMBA_CLK_EN_VAL      0x00000444
#define I2C1_KERNEL_CLK_EN_VAL    0x00000244

#define I2C3_AMBA_CLK_EN_VAL      0x00000001
#define I2C3_KERNEL_CLK_EN_VAL    0x00000001

#endif


/*********************************************************************************/
/*Following Macros are defined to provide the compatibility to old Services APIs */
/*********************************************************************************/
/* Single Read */

#define SER_I2C_SingleReadBlocking(id,address,p_data)\
             SER_I2C_ReadSingleDataBlocking(id,address,I2C_NO_INDEX,0,p_data)


#define SER_I2C_SingleReadRegister8Blocking(id,address,index, p_data)\
                 SER_I2C_ReadSingleDataBlocking(id,address,I2C_BYTE_INDEX,index,p_data)


#define SER_I2C_SingleReadRegister16Blocking(id,address,index,p_data)\
                 SER_I2C_ReadSingleDataBlocking(id,address,I2C_HALF_WORD_LITTLE_ENDIAN,index,p_data)

/* Single Write */


#define SER_I2C_SingleWriteBlocking(id,address,data)\
                SER_I2C_WriteSingleDataBlocking(id,address,I2C_NO_INDEX,0,data)

#define SER_I2C_SingleWriteRegister8Blocking(id,address,index,data)\
                SER_I2C_WriteSingleDataBlocking(id,address,I2C_BYTE_INDEX,index,data)

#define SER_I2C_SingleWriteRegister16Blocking(id,address,index,data)\
                SER_I2C_WriteSingleDataBlocking(id,address,I2C_HALF_WORD_LITTLE_ENDIAN,index,data)

/* Multiple Read  */


#define SER_I2C_MultiReadBlocking(id,address,p_data,count)\
                 SER_I2C_ReadMultipleDataBlocking(id,address,I2C_NO_INDEX,0,p_data,count)

#define SER_I2C_MultiReadRegister8Blocking(id,address,index,p_data,count)\
                 SER_I2C_ReadMultipleDataBlocking(id,address,I2C_BYTE_INDEX,index,p_data,count) 

#define SER_I2C_MultiReadRegister16Blocking(id,address,index,p_data,count)\
                 SER_I2C_ReadMultipleDataBlocking(id,address,I2C_HALF_WORD_LITTLE_ENDIAN,index,p_data,count)                
/* Multiple Write */



#define SER_I2C_MultiWriteBlocking(id,address,p_data,count)\
             SER_I2C_WriteMultipleDataBlocking(id,address,I2C_NO_INDEX,0,p_data,count)


#define SER_I2C_MultiWriteRegister8Blocking(id,address,index,p_data,count)\
             SER_I2C_WriteMultipleDataBlocking(id,address,I2C_BYTE_INDEX,index,p_data,count)


#define SER_I2C_MultiWriteRegister16Blocking(id,address,index,p_data,count)\
             SER_I2C_WriteMultipleDataBlocking(id,address,I2C_HALF_WORD_LITTLE_ENDIAN,index,p_data,count)               

/*********************************************************************************/
/*End od Macros*/
/*********************************************************************************/


/*private define*/


#define I2C_PRINT PRINT


typedef struct {
    t_i2c_active_event i2c_event;
    } t_ser_i2c_callback;



/*Function Protypes */
/* Basic Services */
PUBLIC void SER_I2C_Init(IN t_uint8);
PUBLIC void SER_I2C_Close(void);
PUBLIC int SER_I2C_RegisterCallback (IN t_i2c_device_id id, IN t_callback_fct callback_fct, IN void* param);
PUBLIC t_i2c_event SER_I2C_WaitEnd(IN t_i2c_device_id id);

/*Extended Services */
PUBLIC void SER_I2C0_InterruptHandler(IN t_uint32 irq);
PUBLIC void SER_I2C1_InterruptHandler(IN t_uint32 irq);
PUBLIC void SER_I2C2_InterruptHandler(IN t_uint32 irq);
PUBLIC void SER_I2C3_InterruptHandler(IN t_uint32 irq);
#if ((defined ST_8500V1)||(defined ST_HREFV1)||(defined __PEPS_8500_V1)||(defined __PEPS_8500_V2)||(defined ST_8500V2)||(ST_HREFV2))
PUBLIC void SER_I2C4_InterruptHandler(IN t_uint32 irq);
#endif
PUBLIC t_i2c_error SER_I2C_ConfigureDefault(void);


/* Blocking Data services */
PUBLIC t_i2c_error  SER_I2C_WriteSingleDataBlocking  (IN t_i2c_device_id id,
                                                      IN t_uint16             slave_address, 
                                                      IN t_i2c_index_format   index_format,
                                                      IN t_uint16             index_value,
                                                      IN t_uint8              data);
PUBLIC t_i2c_error  SER_I2C_WriteMultipleDataBlocking(IN t_i2c_device_id             id, 
                                                      IN t_uint16             slave_address,
                                                      IN t_i2c_index_format   index_format,
                                                      IN t_uint16             index_value,
                                                      IN t_uint8              *p_data, 
                                                      IN t_uint32             count);
PUBLIC t_i2c_error  SER_I2C_ReadSingleDataBlocking   (IN t_i2c_device_id             id, 
                                                      IN t_uint16             slave_address, 
                                                      IN t_i2c_index_format   index_format,
                                                      IN t_uint16             index_value,
                                                      IN t_uint8              *p_data);             
PUBLIC t_i2c_error SER_I2C_ReadMultipleDataBlocking  (IN t_i2c_device_id             id, 
                                                      IN t_uint16             slave_address, 
                                                      IN t_i2c_index_format   index_format,
                                                      IN t_uint16             index_value,
                                                      IN t_uint8              *p_data, 
                                                      IN t_uint32             count);

#endif /*End of File */
