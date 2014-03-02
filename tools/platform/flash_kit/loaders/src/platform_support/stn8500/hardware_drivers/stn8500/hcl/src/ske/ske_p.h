/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
*  Private Header file of SKE
*  AUTHOR :  ST-Ericsson
*/
/*****************************************************************************/

#ifndef __SKE_PRIVATE_HEADER
#define __SKE_PRIVATE_HEADER

#include "ske_hwp.h"

/* SKE Peripheral IDs and Primecell ID */
#define SKE_PERIPH_ID_0 0x70
#define SKE_PERIPH_ID_1 0x00
#define SKE_PERIPH_ID_2 0x18
#define SKE_PERIPH_ID_3 0x00

#define SKE_P_CELL_ID_0 0x0D
#define SKE_P_CELL_ID_1 0xF0
#define SKE_P_CELL_ID_2 0x05
#define SKE_P_CELL_ID_3 0xB1

typedef t_uint32    t_ske_device_context[5];

typedef struct
{
    t_ske_register          *p_ske_register;    /* Pointer which stores the SKE base address */
    t_uint8                 ske_keypad_max_columns;
    t_uint8                 ske_keypad_max_rows;
    t_ske_device_context    ske_device_context;
} t_ske_system_context;
#endif /* __SKE_PRIVATE_HEADER */

