/*-----------------------------------------------------------------------------
 * ST-Ericsson
 * Reproduction and Communication of this document is strictly prohibited
 * unless specifically authorized in writing by ST-Ericsson.
 *---------------------------------------------------------------------------*/
/** \addtogroup API_REUSE
 * @{
 */ 
/*---------------------------------------------------------------------------*/
/** \file boot_api.h
 * \author ST-Ericsson
 * \brief This module provides Boot API reuse definition for ISSW/X_Loader.c file 
 *       implementation.\n
 *       see \ref BOOT_Reuse[] table
 */
/*---------------------------------------------------------------------------*/

/** \mainpage AP8540 Public ROM API documentation 
 *  \n\n\n
    \image html ste_jpeg_rgb.jpg
    \n\n\n 
 * This document stands for BOOT ROM code API documentation. \n
 * It describes all types ( structures, enum, union...) and all functions exported by the ROM code.\n
 * It also includes \ref faqs and \ref releasenote.
 * \n
 * \n
 * \section list_api BOOT API REUSE MACRO
 * \n
 *  This list defines all the Boot Rom functions that can be called from the Xloader. \n
 *  The first parameters of the macro are the same as the associated function, the last parameter
 *  is the jump table \ref BOOT_Reuse[] address.
 *  \n
 * \subsection timer_api TIMER BOOT API REUSE MACROS 
 * - ROM_SHARED_FUNC_TIMER_INIT(jmp_table) Macro to call TIMER_Init() function. \n
 * - ROM_SHARED_FUNC_TIMER_START_TIMER(a,jmp_table) Macro to call TIMER_Start() function. \n
 * - ROM_SHARED_FUNC_TIMER_STOP_TIMER(jmp_table) Macro to call TIMER_Stop() function. \n
 * - ROM_SHARED_FUNC_TIMER_HAS_TIMER_REACHED_ZERO(a,jmp_table) Macro to call TIMER_HasTimerReachedZero() function. \n
 *\n
 * \subsection mmc_api MMC BOOT API REUSE MACROS  
 * - ROM_SHARED_MMC_DETECT_AND_INIT (a,b,jmp_table) Macro to call MMC_DetectAndInit() function. \n
 * - ROM_SHARED_MMC_TOC_SEARCH (a,b,jmp_table) Macro to call MMC_TocSearch() function. \n
 * - ROM_SHARED_MMC_READ (a,b,c,d,e,jmp_table) Macro to call MMC_Read() function. \n
 * - ROM_SHARED_MMC_SET_WIDE_BUS(a,b,jmp_table) Macro to call MMC_SetMmcWideBus() function. \n
 *\n
 * \subsection usb_api USB BOOT API REUSE MACROS  
 * - ROM_SHARED_USB_INIT(a,b,jmp_table) Macro to call USB_Init() function. \n
 * - ROM_SHARED_USB_GET_NB_BYTES_REC(a,jmp_table) Macro to call USB_GetNbBytesReceived() function. \n
 * - ROM_SHARED_USB_SET_CONFIG(jmp_table) Macro to call USB_SetConfiguration() function. \n
 * - ROM_SHARED_USB_RECEIVE_DATA(a,b,c,d,jmp_table) Macro to call USB_ReceiveData() function. \n
 * - ROM_SHARED_USB_TRANSMIT_DATA(a,b,c,jmp_table) Macro to call USB_TransmitData() function. \n
 * - ROM_SHARED_USB_IS_CONFIG_RECEIVED(a,jmp_table) Macro to call USB_IsConfigReceived() function. \n
 * - ROM_SHARED_USB_IS_SESSION_VALID(jmp_table)Macro to call USB_IsSessionValid() function. \n
 * - ROM_SHARED_USB_IT_PROCESS(jmp_table) Macro to call USB_ItProcess() function. \n
 * - ROM_SHARED_USB_CUST_DISCONNECT(jmp_table) Macro to call USB_CustDisconnect() function. \n
 * - ROM_SHARED_USB_GET_STATUS(a,jmp_table) Macro to call USB_GetStatus() function. \n
 * - ROM_SHARED_USB_INIT_EP(a,jmp_table) Macro to call USB_InitEndpoint() function. \n
 * - ROM_SHARED_USB_CUST_INITIALIZE(jmp_table) Macro to call USB_CustInitialize() function. \n
 * - ROM_SHARED_USB_CUST_CONNECT(a,b,jmp_table) Macro to call USB_CustConnect() function. \n
 * - ROM_SHARED_USB_CUST_CLOSE(jmp_table) Macro to call USB_CustClose() function. \n
 * - ROM_SHARED_USB_CUST_WRITE_DATA(a,b,c,jmp_table) Macro to call USB_CustWriteData() function. \n
 * - ROM_SHARED_USB_GET_NB_BYTES_SENT(a,jmp_table) Macro to call USB_GetNbBytesSent() function. \n
 * - ROM_SHARED_USB_READ_LLI_CONF(a,b,jmp_table) Macro to call USB_ReadLliConf() function. \n
 * - ROM_SHARED_USB_CHANGE_LLI_CONF(a,b,jmp_table) Macro to call USB_ChangeLliConf() function. \n
 * - ROM_SHARED_USB_CUST_READ_DATA(a,b,c,jmp_table) Macro to call USB_CustReadData() function. \n
 * - ROM_SHARED_USB_CUST_SET_TRANS_MODE(a,b,jmp_table) Macro to call USB_CustSetTransferMode() function. \n
 * - ROM_SHARED_USB_CUST_SET_EXT_EVENT_HANDLER(a,jmp_table) Macro to call USB_CustSetExtEventHandler() function. \n
 * \n
 * \subsection uart_api UART BOOT API REUSE MACROS 
 * - ROM_SHARED_FUNC_UART_INIT(a,b,c,d,e,jmp_table)  Macro to call UART_Init() function. \n
 * - ROM_SHARED_FUNC_UART_SEND_DATA(a,b,c,jmp_table) Macro to call UART_SendData() function. \n
 * - ROM_SHARED_FUNC_UART_GET_DATA(a,b,c,jmp_table)  Macro to call UART_GetData() function. \n
 * - ROM_SHARED_FUNC_UART_WAIT_FOR_DATA(a,b,c,jmp_table)  Macro to call UART_WaitForData() function. \n
 * - ROM_SHARED_FUNC_UART_GET_NB_BYTES_REC(a,b,jmp_table)  Macro to call UART_GetNbBytesReceived()function. \n
 * - ROM_SHARED_FUNC_UART_STOP(a,jmp_table) Macro to call UART_StopTx() function. \n
 * - ROM_SHARED_FUNC_UART_TRANSMIT_TX_CHAR(a,b,c,jmp_table) Macro to call UART_TransmitxCharacters() function. \n
 * - ROM_SHARED_IT_PROCESS_UART0(jmp_table)  Unused. \n
 * - ROM_SHARED_IT_PROCESS_UART1 (jmp_table) Macro to call UART_ProcessUart1It() function. \n
 * - ROM_SHARED_IT_PROCESS_UART2 (jmp_table) Macro to call UART_ProcessUart2It() function. \n
 *\n
 * \subsection secure_api SECURE BOOT API REUSE MACROS  
 * - ROM_SHARED_PUBLIC_DISPACHER(a,b,jmp_table) Macro to call hw_sec_rom_pub_bridge(). 
 * \n
 * \subsection loader_api LOADER BOOT API REUSE MACROS 
 * - ROM_SHARED_LOAD_IMAGE(a,b,c,d,e,f,g,jmp_table) Macro to call PUB_LoadImage() function.\n
 * \n
 * \subsection public_api PUBLIC BOOT API REUSE MACROS 
 * - ROM_SHARED_FUNC_WAKEUP_CPU1(a,jmp_table) Macro to call PUB_SlaveProcWakeUp().\n
 * \n
 * \subsection dma_api DMA BOOT API REUSE MACROS 
 * - ROM_SHARED_DMA_INIT Macro to call DMA_Init() function.\n
 * - ROM_SHARED_DMA_IT_PROCESS Macro to call DMA_ItProcess() function.\n
 *
 */

#ifndef __INC_BOOT_API_H
#define __INC_BOOT_API_H

// #include "boot_secure_types.h"
#include "uart.h"
#include "usb.h"
#include "public_rom_api.h"

/* When adding an API Reuse function, Please update the previous list 
 to update Boot API documentation generated by doxygen. */

/* TIMER BOOT API REUSE DEFINE */
#define BOOT_API_TIMER_INIT			        	0 
#define BOOT_API_TIMER_START_TIMER			    1
#define BOOT_API_TIMER_STOP_TIMER			    2
#define BOOT_API_TIMER_HAS_TIMER_REACHED_ZERO	3
 
/* MMC API function */
#define BOOT_API_MMC_DETECT_AND_INIT            4
#define BOOT_API_MMC_TOC_SEARCH                 5
#define BOOT_API_MMC_READ                       6

/* USB-OTG API function */                      
#define BOOT_API_USB_INIT                       7
#define BOOT_API_USB_GET_NB_BYTES_RECEIVED      8
#define BOOT_API_USB_SET_CONFIGURATION          9
#define BOOT_API_USB_RECEIVE_DATA               10
#define BOOT_API_USB_TRANSMIT_DATA              11
#define BOOT_API_USB_IS_CONFIG_RECEIVED         12
#define BOOT_API_USB_IS_SESSION_VALID           13
#define BOOT_API_USB_IT_PROCESS                 14
#define BOOT_API_USB_GET_STATUS                 15
#define BOOT_API_USB_CUST_DISCONNECT            16
#define BOOT_API_USB_INIT_EP                    17
#define BOOT_API_USB_CUST_INITIALIZE            18
#define BOOT_API_USB_CUST_CONNECT               18
#define BOOT_API_USB_CUST_CLOSE                 20

/* UART BOOT API REUSE DEFINE */
#define BOOT_API_UART_INIT			        	21
#define BOOT_API_UART_SEND_DATA		    		22
#define BOOT_API_UART_GET_DATA		        	23
#define BOOT_API_UART_WAIT_FOR_DATA	    		24
#define BOOT_API_UART_GET_NB_BYTES_REC	    	25
#define BOOT_API_UART_STOP						26
#define BOOT_API_UART_TRANSMIT_TX_CHAR			27
#define BOOT_API_UART0_IT_PROCESS               28
#define BOOT_API_UART1_IT_PROCESS          		29
#define BOOT_API_UART2_IT_PROCESS               30 

/* SECURE API REUSE DEFINE */
#define BOOT_API_PUBLIC_DISPACHER	            31

/* LOADER API REUSE DEFINE */
#define BOOT_API_PUB_LOAD_IMAGE 			    32

/* PUBLIC API REUSE DEFINE */
#define BOOT_API_WAKEUP_CPU1     			    33

/*additionnal USB API REUSE DEFINE*/
#define BOOT_API_USB_CUST_WRITE_DATA            34
#define BOOT_API_USB_GET_NB_BYTES_SENT          35
#define BOOT_API_USB_READ_LLI_CONF              36
#define BOOT_API_USB_CHANGE_LLI_CONF            37

/*additionnal MMC API REUSE DEFINE*/
#define BOOT_API_MMC_SET_WIDE_BUS               38

/*DMA API REUSE DEFINE*/
#define BOOT_API_DMA_INIT                       39
#define BOOT_API_DMA_IT_PROCESS                 40

/*additionnal USB API REUSE DEFINE*/
#define BOOT_API_USB_CUST_READ_DATA             41
#define BOOT_API_USB_CUST_SET_TRANS_MODE        42
#define BOOT_API_USB_CUST_SET_EXT_EVENT_HANDLER 43

/* Macros */

/* TIMER BOOT API REUSE MACROS */
#define ROM_SHARED_FUNC_TIMER_INIT(jmp_table) \
((void (*)()) jmp_table [BOOT_API_TIMER_INIT])()

#define ROM_SHARED_FUNC_TIMER_START_TIMER(a,jmp_table) \
((void (*)(t_uint32)) jmp_table [BOOT_API_TIMER_START_TIMER])(a)

#define ROM_SHARED_FUNC_TIMER_STOP_TIMER(jmp_table) \
((void (*)()) jmp_table [BOOT_API_TIMER_STOP_TIMER])()

#define ROM_SHARED_FUNC_TIMER_HAS_TIMER_REACHED_ZERO(a,jmp_table) \
((void (*)(t_bool*)) jmp_table [BOOT_API_TIMER_HAS_TIMER_REACHED_ZERO])(a)


/* MMC BOOT API REUSE DEFINE */
#define ROM_SHARED_MMC_DETECT_AND_INIT(a,b,jmp_table) \
((t_boot_error (*)(const t_mmc_init_conf*,t_MmcType*))jmp_table[BOOT_API_MMC_DETECT_AND_INIT])(a,b)

#define ROM_SHARED_MMC_TOC_SEARCH(a,b,jmp_table) \
((t_boot_error (*)(const t_MmcType*,t_address*))jmp_table[BOOT_API_MMC_TOC_SEARCH])(a,b)

#define ROM_SHARED_MMC_READ(a,b,c,d,e,jmp_table) \
((t_boot_error (*)(const t_MmcType*,const t_address,t_uint8*,const t_size, t_dma_use))jmp_table[BOOT_API_MMC_READ])(a,b,c,d,e)

#define ROM_SHARED_MMC_SET_WIDE_BUS(a,b,jmp_table) \
((t_boot_error(*)(t_MmcType*, const t_mmc_init_conf*)) jmp_table[BOOT_API_MMC_SET_WIDE_BUS])(a,b)


/* USB BOOT API REUSE DEFINE */
#define ROM_SHARED_USB_INIT(a,b,jmp_table) \
((void (*)(t_usb_type *,t_usb_descriptor_list*))jmp_table[BOOT_API_USB_INIT])(a,b)

#define ROM_SHARED_USB_GET_NB_BYTES_REC(a,jmp_table) \
((void (*)(t_uint32*))jmp_table[BOOT_API_USB_GET_NB_BYTES_RECEIVED])(a)

#define ROM_SHARED_USB_SET_CONFIG(jmp_table) \
((void (*)())jmp_table[BOOT_API_USB_SET_CONFIGURATION])()

#define ROM_SHARED_USB_RECEIVE_DATA(a,b,c,d,jmp_table) \
((void(*)(t_size,t_address,t_dma_use,t_pUsbCallBack))jmp_table[BOOT_API_USB_RECEIVE_DATA])(a,b,c,d)

#define ROM_SHARED_USB_TRANSMIT_DATA(a,b,c,jmp_table) \
((void (*)(t_endp_num,t_address,t_size))jmp_table[BOOT_API_USB_TRANSMIT_DATA])(a,b,c)

#define ROM_SHARED_USB_IS_CONFIG_RECEIVED(a,jmp_table) \
((void (*)(t_bool*))jmp_table[BOOT_API_USB_IS_CONFIG_RECEIVED])(a)

#define ROM_SHARED_USB_IS_SESSION_VALID(jmp_table) \
((t_bool (*)())jmp_table[BOOT_API_USB_IS_SESSION_VALID])()

#define ROM_SHARED_USB_IT_PROCESS(jmp_table) \
((t_usb_sys_it (*)())jmp_table[BOOT_API_USB_IT_PROCESS])()

#define ROM_SHARED_USB_CUST_DISCONNECT(jmp_table) \
((t_uint32 (*)())jmp_table[BOOT_API_USB_CUST_DISCONNECT])()

#define ROM_SHARED_USB_GET_STATUS(a,jmp_table) \
((void (*)(t_usb_status*))jmp_table[BOOT_API_USB_GET_STATUS])(a)

#define ROM_SHARED_USB_INIT_EP(a,jmp_table) \
((void (*)(t_endpoint_config*))jmp_table[BOOT_API_USB_INIT_EP])(a) 

#define ROM_SHARED_USB_CUST_INITIALIZE(jmp_table) \
((t_uint32(*)())jmp_table[BOOT_API_USB_CUST_INITIALIZE])()

#define ROM_SHARED_USB_CUST_CONNECT(a,b,jmp_table) \
((t_uint32 (*)(t_usb_descriptor_list*,t_pUsbCallBack))jmp_table[BOOT_API_USB_CUST_CONNECT])(a,b)

#define ROM_SHARED_USB_CUST_CLOSE(jmp_table) \
((t_uint32 (*)())jmp_table[BOOT_API_USB_CUST_CLOSE])()

#define ROM_SHARED_USB_CUST_WRITE_DATA(a,b,c,jmp_table) \
((t_uint32 (*)(t_size, t_address, t_pUsbCallBack)) jmp_table[BOOT_API_USB_CUST_WRITE_DATA])(a,b,c)

#define ROM_SHARED_USB_GET_NB_BYTES_SENT(a,jmp_table) \
((void (*)(t_uint32*)) jmp_table[BOOT_API_USB_GET_NB_BYTES_SENT])(a)

#define ROM_SHARED_USB_READ_LLI_CONF(a,b,jmp_table) \
((void (*)( t_address*, t_uint32*))jmp_table[BOOT_API_USB_READ_LLI_CONF])(a,b)

#define ROM_SHARED_USB_CHANGE_LLI_CONF(a,b,jmp_table) \
((t_boot_error (*)( t_address, t_uint32))jmp_table[BOOT_API_USB_CHANGE_LLI_CONF])(a,b)

#define ROM_SHARED_USB_CUST_READ_DATA(a,b,c,jmp_table) \
((t_uint32 (*)( t_size, t_address, t_pUsbCallBack))jmp_table[BOOT_API_USB_CUST_READ_DATA])(a,b,c)

#define ROM_SHARED_USB_CUST_SET_TRANS_MODE(a,b,jmp_table) \
((t_uint32 (*)(t_usb_mode, t_usb_dir))jmp_table[BOOT_API_USB_CUST_SET_TRANS_MODE])(a,b)

#define ROM_SHARED_USB_CUST_SET_EXT_EVENT_HANDLER(a,jmp_table) \
((t_uint32 (*)(t_pUsbCallBack))jmp_table[BOOT_API_USB_CUST_SET_EXT_EVENT_HANDLER])(a)

/* UART BOOT API REUSE MACROS */
#define ROM_SHARED_FUNC_UART_INIT(a,b,c,d,e,jmp_table) \
((void (*) (t_uart_number,t_uart_baudrate,t_uart_parity_bit,t_uart_stop_bits,t_uart_data_bits)) jmp_table [BOOT_API_UART_INIT])(a,b,c,d,e)

#define ROM_SHARED_FUNC_UART_SEND_DATA(a,b,c,jmp_table) \
((void (*)	(t_uart_number,t_address,t_size)) jmp_table [BOOT_API_UART_SEND_DATA])(a,b,c)

#define ROM_SHARED_FUNC_UART_GET_DATA(a,b,c,jmp_table) \
((t_boot_error (*)	(t_uart_number,t_address,t_size)) jmp_table [BOOT_API_UART_GET_DATA])(a,b,c)

#define ROM_SHARED_FUNC_UART_WAIT_FOR_DATA(a,b,c,jmp_table) \
((t_boot_error (*)	(t_uart_number,t_address,t_size)) jmp_table [BOOT_API_UART_WAIT_FOR_DATA])(a,b,c)

#define ROM_SHARED_FUNC_UART_GET_NB_BYTES_REC(a,b,jmp_table) \
((t_boot_error (*)	(t_uart_number,t_uint32*)) jmp_table [BOOT_API_UART_GET_NB_BYTES_REC])(a,b)

#define ROM_SHARED_FUNC_UART_STOP(a,jmp_table) \
((void (*)(t_uart_number)) jmp_table [BOOT_API_UART_STOP])(a)

#define ROM_SHARED_FUNC_UART_TRANSMIT_TX_CHAR(a,b,c,jmp_table) \
((t_boot_error (*)	(t_uart_number,t_address,t_size)) jmp_table [BOOT_API_UART_TRANSMIT_TX_CHAR])(a,b,c)

#define ROM_SHARED_IT_PROCESS_UART0(jmp_table) \
((void (*)())jmp_table[BOOT_API_UART0_IT_PROCESS])()

#define ROM_SHARED_IT_PROCESS_UART1(jmp_table) \
((void (*)()) jmp_table [BOOT_API_UART1_IT_PROCESS])()

#define ROM_SHARED_IT_PROCESS_UART2(jmp_table) \
((void (*)())jmp_table[BOOT_API_UART2_IT_PROCESS])()

/* PUBLIC DISPATCHER API_REUSE */
#define ROM_SHARED_PUBLIC_DISPACHER(a,b,jmp_table) \
((void (*)(const t_sec_rom_appli_id * const, ...))jmp_table[BOOT_API_PUBLIC_DISPACHER])(a,b)

/* LOADER API REUSE MACROS */
#define ROM_SHARED_LOAD_IMAGE(a,b,c,d,e,f,g,jmp_table) \
((t_boot_error (*)	(const t_address, const t_size, t_uint32*, t_load_if, t_uint32*, t_rpc_feature_flags, t_uint32 ))jmp_table [BOOT_API_PUB_LOAD_IMAGE])(a,b,c,d,e,f,g)

#define ROM_SHARED_FUNC_WAKEUP_CPU1(a,jmp_table) \
((void (*)(const t_address)) jmp_table [BOOT_API_WAKEUP_CPU1])(a)

/* DMA API REUSE MACROS */
#define ROM_SHARED_DMA_INIT(jmp_table) \
((void (*)()) jmp_table [BOOT_API_DMA_INIT])()

#define ROM_SHARED_DMA_IT_PROCESS(jmp_table) \
((t_boot_error (*)()) jmp_table [BOOT_API_DMA_IT_PROCESS])()


#endif /*__INC_BOOT_API_H*/

/** @} */
