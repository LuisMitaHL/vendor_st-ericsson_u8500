/*****************************************************************************/
/**
*  © ST-Ericsson, 2010 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Header file SPI Service initialization for U5500
* \author  ST-Ericsson
*/
/*****************************************************************************/
#ifndef _SERVICES_SPI
#define _SERVICES_SPI

#include "spi.h"
#include "gic.h"
#include "gpio.h"
#include "gpio_services.h"
#include "services.h"
#include "memory_mapping.h"


#define SER_TXFIFOEMPTY     0x01
#define SER_TXFIFONOTFULL   0x02
#define SER_RXFIFONOTEMPTY  0x04
#define SER_RXFIFOFULL      0x08
#define SER_SPIBUSY         0x10


#define SER_SPI_PRCC_KCKEN_OFFSET 0x08
#define SER_SPI_PRCC_ENABLE		  (SER_SPI_3_PRCC_ENABLE|SER_SPI_0_PRCC_ENABLE|SER_SPI_2_PRCC_ENABLE|SER_SPI_1_PRCC_ENABLE)
#define SER_SPI_3_PRCC_ENABLE     0x00004000
#define SER_SPI_0_PRCC_ENABLE     0x00000800
#define SER_SPI_2_PRCC_ENABLE	  0x00002000	
#define SER_SPI_1_PRCC_ENABLE	  0x00001000	



/* Structure for spi service context */
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
    t_callback  callback_spi;
} ser_spi_context;

typedef struct
{
    t_spi_irq_status    irq_status;
    t_spi_event         current_event;
    t_spi_error         spi_error;
}
t_ser_spi_param, *p_ser_spi_param;

/*Function Prototypes */
PUBLIC void SER_SPI_InterruptHandler(t_spi_device_id);
PUBLIC void SER_SPI0_InterruptHandler(IN t_uint32);
PUBLIC void SER_SPI1_InterruptHandler(IN t_uint32);
PUBLIC void SER_SPI2_InterruptHandler(IN t_uint32);
PUBLIC void SER_SPI3_InterruptHandler(IN t_uint32);
PUBLIC t_spi_error SER_SPI_TransmitData(IN t_spi_device_id,IN t_uint8 *,IN t_uint32,IN t_spi_config *);
PUBLIC t_spi_error SER_SPI_ReceiveData(IN t_spi_device_id,IN t_uint8 *,IN t_uint32,IN t_spi_config *);
PUBLIC void SER_SPI_Close(void);
PUBLIC void SER_SPI_Init(t_uint8);
PUBLIC void SER_SPI_RegisterCallback(t_callback_fct, void *);
PUBLIC t_uint32 SER_SPI_WaitEnd(t_spi_device_id, t_uint32);
#endif /*End of file */


