/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Header file for MSP SERVICES
* \author  ST-Ericsson
*/
/*****************************************************************************/
#ifndef _SERVICES_MSP
#define _SERVICES_MSP

#include "msp.h"

#if ((defined ST_8500V1)||(defined ST_HREFV1)||(defined __PEPS_8500_V1)||(defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))

#define PRCC_KERNEL_CLK_EN_OFFSET 0x8
#define PRCMU_MSP_CLK_EN_OFFSET   0x1C

#define MSP_CLK_EN_VAL            0x00000122

#define MSP2_AMBA_CLK_EN_VAL      0x00000020
#define MSP2_KERNEL_CLK_EN_VAL    0x00000008

#endif

#if ((defined ST_8500V1)||(defined ST_HREFV1)||(defined __PEPS_8500_V1))
#define MSP0_AMBA_CLK_EN_VAL      0x00000018
#define MSP0_KERNEL_CLK_EN_VAL    0x00000018
#endif

#if ((defined ST_8500V2)||(defined ST_HREFV2)||(defined __PEPS_8500V2))
#define MSP0_AMBA_CLK_EN_VAL      0x00000818
#define MSP0_KERNEL_CLK_EN_VAL    0x00000418
#endif

/*
	Only following protocols are tested:
		1. SER_MSP_I2S_PROTOCOL
		2. SER_MSP_16BIT_PCM_PROTOCOL
*/
typedef enum
{
    SER_MSP_I2S_PROTOCOL,
    SER_MSP_AC97_PROTOCOL,
    SER_MSP_16BIT_PCM_PROTOCOL,
    SER_MSP_PCM_COMPAND_A_LAW_PROTOCOL,
    SER_MSP_PCM_COMPAND_MU_LAW_PROTOCOL,
    SER_MSP_8BIT_PCM_PROTOCOL,
    SER_MSP_MASTER_SPI_PROTOCOL,
    SER_MSP_SLAVE_SPI_PROTOCOL,
    SER_MSP_SPI_BURST_PROTOCOL,
    SER_MSP_USER_DEFINED_PROTOCOL
} t_ser_msp_protocol;

typedef enum
{
    SER_MSP_LINEAR_CODE,
    SER_MSP_A_LAW_CODE,
    SER_MSP_MU_LAW_CODE
} t_ser_msp_compand_mode;

typedef struct
{
    t_msp_irq_status    irq_status;
    t_msp_event         current_event;
    t_msp_error         msp_error;
    t_uint8             padding[3];
} t_ser_msp_param;

PUBLIC void         SER_MSP0_InterruptHandler(IN t_uint32 not_used);
PUBLIC void         SER_MSP1_InterruptHandler(IN t_uint32 not_used);
PUBLIC void         SER_MSP2_InterruptHandler(IN t_uint32 not_used);
#if ((defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
PUBLIC void 		SER_MSP3_InterruptHandler(IN t_uint32 not_used);
#endif


PUBLIC t_bool       SER_MSP_ConfigureCustom
                    (
                        IN t_msp_device_id              msp_device_id,
                        IN t_msp_protocol_descriptor    *p_msp_protocol_desc_user_defined,
                        IN t_uint32                     msp_sample_freq,
                        IN t_uint32                     msp_in_clock_freq,
                        IN t_msp_general_config         *p_msp_general_config
                    );
PUBLIC t_bool       SER_MSP_Configure(IN t_msp_device_id msp_device_id, IN t_ser_msp_protocol ser_msp_protocol, IN t_msp_mode msp_mode, IN t_uint32 msp_sample_freq, IN t_uint32 msp_in_clock_freq);
PUBLIC t_bool       SER_MSP_Transmit(IN t_msp_device_id msp_device_id, IN t_uint8 *p_transmit_buffer, IN t_uint32 buffer_size);
PUBLIC t_bool       SER_MSP_Receive(IN t_msp_device_id msp_device_id, IN t_uint8 *p_receive_buffer, IN t_uint32 buffer_size);
PUBLIC t_bool       SER_MSP_TransmitWaitEnd(IN t_msp_device_id msp_device_id);
PUBLIC t_bool       SER_MSP_ReceiveWaitEnd(IN t_msp_device_id msp_device_id);
PUBLIC t_bool       SER_MSP_TransmitBlocking(IN t_msp_device_id msp_device_id, IN t_uint8 *p_transmit_buffer, IN t_uint32 buffer_size);
PUBLIC t_bool       SER_MSP_ReceiveBlocking(IN t_msp_device_id msp_device_id, IN t_uint8 *p_receive_buffer, IN t_uint32 buffer_size);
PUBLIC t_bool       SER_MSP_Stop(IN t_msp_device_id msp_device_id);
PUBLIC t_bool       SER_MSP_Start(IN t_msp_device_id msp_device_id, IN t_msp_direction msp_direction);
PUBLIC t_bool       SER_MSP_Reset(IN t_msp_device_id msp_device_id);
PUBLIC t_ser_error  SER_MSP_InstallDefaultHandler(IN t_msp_device_id msp_device_id);
PUBLIC t_ser_error  SER_MSP_RegisterCallback(IN t_msp_device_id msp_device_id, IN t_callback_fct callback_fct, IN void *param);
PUBLIC void         SER_MSP_Init(IN t_uint8 mask);
PUBLIC void         SER_MSP_Close(void);
#endif /*End of file */

