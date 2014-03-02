/*******************************************************************************
 * $Copyright Ericsson AB 2009 $
 ******************************************************************************/
#ifndef __R_ADBG_INT_GROUP_H__
#define __R_ADBG_INT_GROUP_H__

/**
 * @addtogroup ADbg
 * @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_adbg.h"

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/

/**
 * @brief Function that allocates memory space for requested interface group.
 *
 * @param [in] TestIntFunction_pp is pointer to array of all interface functions
 *             definitions.
 * @param [in] Command_p is pointer to information about received command.
 * @param [in] Result_p is pointer to structure used to prepare general response
 * @return     Pointer to information about one interface group.
 *
 * @remark     It dosen’t check the input parameters.
 */
ADbg_IntGroup_t *Do_ADbg_IntGroup_Create(ADbg_IntFunctionDefinition_t **TestIntFunction_pp,
        ADbg_Command_t                *Command_p,
        ADbg_Result_t                 *Result_p);

/**
 * @brief Function that frees the reserved memory space.
 *
 * @param [in] IntGroup_p is pointer to information about one interface group.
 * @return     None.
 *
 * @remark     None.
 */
void Do_ADbg_IntGroup_Destroy(ADbg_IntGroup_t *IntGroup_p);

/**
 * @brief Function that finds interface function specified in command.
 *
 * @param [in] IntGroup_p is pointer to information about interface group.
 * @return     Pointer to definition of interface function specified in command.
 *
 * @remark     It dosen’t check the input parameters.
 */
ADbg_IntFunctionDefinition_t *Do_ADbg_FindIntFunction(ADbg_IntGroup_t *IntGroup_p);

/**
 * Function that writes names and IDs all interface functions in one module in
 * buffer, that holds information to be sent through general response.
 *
 * @param [in/out] IntGroup_p is pointer to information about one interface
 *                 group.
 * @return         None.
 *
 * @remark         It dosen’t check the input parameters.
 */
void Do_ADbg_InIntGroup_ListInterface(ADbg_IntGroup_t *IntGroup_p);

/**
 * @brief Function that creates, runs and destroys information about interface
 *        function.
 *
 * @param [in] TestIntFunction_p is pointer to interface function definition.
 * @param [in] IntGroup_p is pointer to information about one interface group.
 * @return     None.
 *
 * @remark     It dosen’t check the input parameters.
 */
void Do_ADbg_RunIntFunction(ADbg_IntFunctionDefinition_t *TestIntFunction_p,
                            ADbg_IntGroup_t              *IntGroup_p);

/** @} */
#endif /*__R_ADBG_INT_GROUP_H__*/
