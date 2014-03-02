/*-----------------------------------------------------------------------------
 * ST Ericsson
 * Reproduction and Communication of this document is strictly prohibited
 * unless specifically authorized in writing by ST Ericsson.
 *---------------------------------------------------------------------------*/
/** \addtogroup UART
 * @{
 */ 
/*---------------------------------------------------------------------------*/
/** \file uart.h
 *  \author ST Ericsson
 *  \brief This module provides function to implement the Boot uart driver
 */
/*---------------------------------------------------------------------------*/
#ifndef __INC_BOOT_UART_H
#define __INC_BOOT_UART_H


/*------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------*/

#include "boot_types.h"
#include "boot_error.h"


/*------------------------------------------------------------------------
 * Defines
 *----------------------------------------------------------------------*/
/* Give the number of UART interface on which it is possible to boot
   only UART0 */
#define UART_BOOT_NB 3

#define BR_CMD 				(0xBD)
#define BR_ACK_CMD			(0xBD0000AC)
#define MAX_SUPPORTED_BR	(0x5F5E10)

#define UARTCLK_49_5625M		(49562500ULL) // 50MHz

/** Parity definition */
typedef enum {
    NOPARITY_BIT    ,  /**<  (0) */
    EVEN_PARITY_BIT ,  /**<  (1) */
    ODD_PARITY_BIT  ,  /**<  (2) */
    PARITY_0_BIT    ,  /**<  (3) stick parity mode */
    PARITY_1_BIT       /**<  (4) stick parity mode */
} t_uart_parity_bit;

/** Data bits number */
typedef enum {
    DATABITS_5 = 0x0,   /**<  (0x0) */
    DATABITS_6 = 0x1,   /**<  (0x1) */
    DATABITS_7 = 0x2,   /**<  (0x2) */
    DATABITS_8 = 0x3    /**<  (0x3) */
} t_uart_data_bits;

/** Stop bits number*/
typedef enum {
    ONE_STOPBIT = 0x0,   /**<  (0x0) */
    TWO_STOPBITS = 0x1   /**<  (0x1) */
} t_uart_stop_bits;


/* Baud Rate for UART_CLK=48MHz (old)
typedef enum {
BR110BAUD   =   0x2F6A88,  
BR1200BAUD  =   0x0009C4,  
BR2400BAUD  =   0x0004E2,  
BR9600BAUD  =   0x200138, 
BR38400BAUD =   0x08004E,  
BR115200BAUD=   0x03001A,   
BR230400BAUD=   0x02000D,   
BR460800BAUD=   0x210006,   
BR921600BAUD=   0x110003,   
BR1843200BAUD=  0x290001,  
BR3000000BAUD=  0x000001   
} t_uart_baudrate;

Baud Rate for UART_CLK=38.4MHz (old)
typedef enum {
BR110BAUD   =   0x0C553A,   
BR1200BAUD  =   0x0007D0,  
BR2400BAUD  =   0x0003E8,   
BR9600BAUD  =   0x0000FA,  
BR38400BAUD =   0x20003E,  
BR115200BAUD=   0x350014, 
BR230400BAUD=   0x1B000A, 
BR460800BAUD=   0x0D0005, 
BR921600BAUD=   0x270002, 
BR1843200BAUD=  0x130001,  
BR3000000BAUD=  0x330000 
} t_uart_baudrate;
*/

/** Baud Rate enumeration for UART_CLK=26MHz. \n
    BRDI + BRDF = UartClk/(16*Baudrate) 
    DIVFRAC= integer(BRDF*64+0.5)*/
typedef enum {
BR110BAUD   =   0x2F39B4,   /**<  (0x2F39B4) UARTCLK=26MHz, BRDI=14772(0x39b4), DIVFRAC=47 (BRDF=0.727)*/
BR1200BAUD  =   0x0B054A,   /**<  (0x0B054A) UARTCLK=26MHz, BRDI=1354(0x54a), DIVFRAC=11 (0xb) (BRDF=0.167)*/
BR2400BAUD  =   0x0502A5,   /**<  (0x0502A5) UARTCLK=26MHz, BRDI=677(0x2a5), DIVFRAC=5 (BRDF=0.083)*/
BR9600BAUD  =   0x1100A9,   /**<  (0x1100A9) UARTCLK=26MHz, BRDI=169(0xA9), DIVFRAC=17 (0x11) (BRDF=0.271)*/
BR38400BAUD =   0x14002A,   /**<  (0x14002A) UARTCLK=26MHz, BRDI=42(0x2A), DIVFRAC=20 (0x14) (BRDF=0.318)*/
BR115200BAUD=   0x07000E,   /**<  (0x070014) UARTCLK=26MHz, BRDI=14 (0xE), DIVFRAC=7 (BRDF=0.106)*/
BR230400BAUD=   0x030007,   /**<  (0x030007) UARTCLK=26MHz, BRDI=7, DIVFRAC=3 (BRDF=0.053)*/
BR460800BAUD=   0x220003,   /**<  (0x220003) UARTCLK=26MHz, BRDI=3, DIVFRAC=34(0x22) (BRDF=0.526)*/
BR921600BAUD=   0x310001,   /**<  (0x310001) UARTCLK=26MHz, BRDI=1, DIVFRAC=49(0x31) (BRDF=0.763)*/
BR1843200BAUD=  0x380000,   /**<  (0x380000) UARTCLK=26MHz, BRDI=0, DIVFRAC=56(0x38) (BRDF=0.882)*/
BR3000000BAUD=  0x230000    /**<  (0x230000) UARTCLK=26MHz, BRDI=0, DIVFRAC=35(0x23) (BRDF=0.542) */
} t_uart_baudrate;

/** Uart structure for global UART information */
typedef struct{
    t_uart_number UartNb;
    t_uart_baudrate Baudrate;
    t_uart_parity_bit ParityBit;
    t_uart_data_bits  DataBits;
    t_uart_stop_bits  StopBits;    
}t_uart_type;
/*------------------------------------------------------------------------
 * Global Variables
 *----------------------------------------------------------------------*/


/*------------------------------------------------------------------------
 * Function definition
 *----------------------------------------------------------------------*/

PUBLIC void UART_Init (t_uart_number,t_uart_baudrate,t_uart_parity_bit,t_uart_stop_bits,t_uart_data_bits);
PUBLIC void UART_Close (t_uart_number);
PUBLIC void UART_SendData(t_uart_number,t_address,t_size);
PUBLIC void UART_StopTx(t_uart_number);
PUBLIC void UART_ReInit (t_uart_number,t_uint32,t_uint32);

PUBLIC t_boot_error UART_GetData(t_uart_number,t_address,t_size);
PUBLIC t_boot_error UART_WaitForData(t_uart_number,t_address,t_size);
PUBLIC t_boot_error UART_GetNbBytesReceived(t_uart_number,t_uint32 *);
PUBLIC t_boot_error UART_GetTransmitFinish(t_uart_number,t_bool *);
PUBLIC t_bool UART_CableDetect(t_uart_number);

PUBLIC void UART_ProcessUart0It(void); 
PUBLIC void UART_ProcessUart1It(void);
PUBLIC void UART_ProcessUart2It(void);

PUBLIC t_bool  UART_IsTxfifoEmpty (t_uart_number );

PUBLIC t_boot_error UART_TransmitxCharacters(t_uart_number,t_address, t_size);
PUBLIC t_boot_error UART_GetNbBytesTransmitted(t_uart_number,t_uint32 *);

#endif /* __INC_BOOT_UART_H */
/* end of uart.h */
/** @} */
