/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _T_CONFIGURATION_PARSER_H_
#define _T_CONFIGURATION_PARSER_H_

/**
 *  @addtogroup ldr_utilities
 *  @{
 *    @addtogroup configuration_parser
 *    @{
 */
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_basicdefinitions.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/
/** Defines maximum number for file system specific volume*/
#define PARAMETERS_LEN_IN_BYTES  (50)
/** Defines maximum length in bytes for volume config param names*/
#define MAX_NAME_LEN_IN_BYTES    (20)

typedef struct {
    /**< Volume name */
    char VolumeName[MAX_NAME_LEN_IN_BYTES];
    /**< File system type */
    char TypeName[MAX_NAME_LEN_IN_BYTES];
    /**< Parameters of specified volume */
    char FSMountingParam[PARAMETERS_LEN_IN_BYTES];
    /**< Device on which specified volume is mounted */
    char DeviceName[MAX_NAME_LEN_IN_BYTES];
    /**< Device start address */
    int LoBlkNo;
    /**< Device end address */
    int HiBlkNo;
    /**< Device end address */
    boolean IsSet;
} ConfigurationParser_t;

/** @} */
/** @} */
#endif /*_T_CONFIGURATION_PARSER_H_*/
