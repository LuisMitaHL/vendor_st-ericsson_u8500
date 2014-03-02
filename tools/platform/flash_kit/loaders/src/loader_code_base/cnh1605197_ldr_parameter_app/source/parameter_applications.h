/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _PARAMETER_APPLICATIONS_H_
#define _PARAMETER_APPLICATIONS_H_

/*
 * @addtogroup ldr_parameter_app
 * @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "error_codes.h"
#include "r_io_subsystem.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/
/** Size of the data that describing single unit. */
#define UNIT_DESCRIPTOR_SIZE (sizeof(uint16) + sizeof(uint32) + sizeof(uint16))

typedef enum {
    GD_READ_SET_COLLECT_DATA = 0,    /**< Collect data from Trim area partition to buffer. >*/
    GD_READ_SET_INIT_DATA_TRANSFER,  /**< Start bulk session. >*/
    GD_READ_SET_TRANSFER_DATA,       /**< Write the data and close session. >*/
    GD_READ_SET_FINISH               /**< Finalize command. >*/
} GD_ReadSet_t;

typedef enum {
    GD_WRITE_SET_INITIALIZE = 0,  /**< Start bulk session. >*/
    GD_WRITE_SET_GET_DATA,        /**< Receive data via bulk protocol. >*/
    GD_WRITE_SET_PROCESS_DATA     /**< Write received data in Trim Area. >*/
} GD_WriteSet_t;

typedef struct {
    GD_ReadSet_t State;
    IO_Handle_t BulkHandle;
    uint8 *GD_ReadDataSet_p;
    uint32 GD_ReadDataSize;
} GD_ReadSetParameters_t;

typedef struct {
    GD_WriteSet_t State;
    IO_Handle_t BulkHandle;
    uint8 *GD_WriteDataSet_p;
    uint64 GD_WriteDataSize;
} GD_WriteSetParameters_t;


/* @} */
#endif /*_PARAMETER_APPLICATIONS_H_*/
