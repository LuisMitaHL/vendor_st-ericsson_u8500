/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Header file SSP Service initialization
* \author  ST-Ericsson
*/
/*****************************************************************************/
#ifndef _SERVICES_SSP
#define _SERVICES_SSP

#include "ssp.h"
#include "ssp_irq.h"
#include "gic.h"
#if ((!defined (__PEPS_8500)) && (!defined (__PEPS_8500_V1)) && (!defined (__PEPS_8500_V2)))
#include "gpio.h"
#include "gpio_services.h"
#endif
#include "memory_mapping.h"
#include "services.h"

#define SER_TXFIFOEMPTY     0x01
#define SER_TXFIFONOTFULL   0x02
#define SER_RXFIFONOTEMPTY  0x04
#define SER_RXFIFOFULL      0x08
#define SER_SSPBUSY         0x10
/* Defines for instances */
#define SER_NUM_SSP_INSTANCES	2


#ifndef ST_8500_ED
#define SER_SSP_PRCC_KCKEN_OFFSET 0x08
#define SER_SSP_PRCC_ENABLE       0x06
#endif

/* Structure for ssp service context */
typedef struct
{
    t_bool      rcv_flag;
    t_bool      trans_flag;
    t_bool      rcv_timeout;
    t_bool      rcv_overrun;
    t_bool      receive_it_mode;
    t_bool      transmit_it_mode;
    t_uint32    rx_index;
    t_uint32    tx_index;
    t_uint32    rx_trig_level;
    t_uint32    tx_trig_level;
    t_uint32    transmit_size;
    t_uint32    receive_size;
    volatile t_uint32   *p_tx_it_data_flow;
    volatile t_uint32   *p_rx_it_data_flow;
} ser_ssp_context;

typedef struct
{
    t_uint8 irq_src;
}
t_ser_ssp_param, *t_p_ser_ssp_param;

/*Function Prototypes */
void SER_SSP_InterruptHandler(t_uint32);

PUBLIC void SER_SSP_Close(void);
PUBLIC void SER_SSP_Init(t_uint8);
PUBLIC void SER_SSP_RegisterCallback(t_callback_fct, void *);
PUBLIC t_uint32 SER_SSP_WaitEnd(t_ssp_device_id, t_uint32);
PUBLIC t_ssp_error SER_SSP_ReceiveData(t_ssp_device_id,t_logical_address,t_uint32);
PUBLIC t_ssp_error SER_SSP_TransmitData(t_ssp_device_id,t_logical_address,t_uint32);
#endif /*End of file */


