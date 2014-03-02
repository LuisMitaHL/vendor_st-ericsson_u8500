/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _COMMAND_EXECUTION_CONTROL_H_
#define _COMMAND_EXECUTION_CONTROL_H_

/*
 * \addtogroup ldr_command_exec_service
 * @{
 *  \addtogroup  command_execution_control
 *
 * The Command Execution Control is a central sub module in the CES module. It
 * consists of polling functions that support loader command manipulating
 * functionalities as verification, initiation, execution, monitored. These
 * noted functionalities are implanted as functions contained in the same or
 * other sub modules in the CES framework. As well, it provides allocation of
 * the application register structures and command register structures.
 *
 * There are several purposes of Command Execution Control in the CES module.
 * It provides command polling and allocates memory for application and command
 * register structures defined in compile time.
 *
 *  @{
 *
 */
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "running_commands.h"
#include "t_command_exec_service.h"

/*******************************************************************************
 * Types, constants and external variables
 ******************************************************************************/

/*
 * Data structure used for storing
 * command groups information defined at compile time.
 */
typedef struct ApplicationInfo_s {
    unsigned int                 ApplicationNumber;  /**< Number indicating
                                                        to an application
                                                        group */
    unsigned int                 ApplicationLength;  /**< The length, in entries,
                                                        of CommandInformation_t
                                                        area for each Command
                                                        group. */
    CommandInformation_t        *ListOfCommands_p;   /**< Pointer to command
                                                        register structures.
                                                        Must be correlated with
                                                        ApplicationNr. */
} ApplicationInfo_t;

/* @} */
/* @} */
#endif /*_COMMAND_EXECUTION_CONTROL_H_*/
