/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _BDM_CABS_H_
#define _BDM_CABS_H_

/**
 * @file  bdm_cabs.h
 * @brief Function and type declarations for CABS block device manager
 *
 * @addtogroup ldr_service_bdm Block Device Manager
 * @{
 *   @addtogroup ldr_service_bdm_cabs CABS Block Device Manager
 *   @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "error_codes.h"
#include "ddb_os_free.h"

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/

/**
* @brief Starts CABS block device manager
*
* @return ErrorCode_e Forwards the response from CABS module.
*
* @remark This function should be called only from BDM module.
*/
ErrorCode_e Do_BDM_CABS_Start(void);

/**
 * @brief Stops CABS block device manager
 *
 * @return ErrorCode_e Forwards the response from CABS module.
 *
 * @remark This function should be called only from BDM module.
 */
ErrorCode_e Do_BDM_CABS_Stop(void);

/**
 * @brief Reads data from CABS block device manager
 *
 * @param[in]     Unit        CABS Unit that data should be read from.
 * @param[in]     Pos         CABS sector where reading should start.
 * @param[in]     Length      Number of sectors to read.
 * @param[in,out] FirstBad_p  See r_cabs.h for more info.
 * @param[out]    Data_p      Output buffer.
 *
 * @return ErrorCode_e Forwards the response from CABS module.
 *
 * @remark This function should be called only from BDM module.
 */
ErrorCode_e Do_BDM_CABS_Read(uint32 Unit, uint32 Pos, uint32 Length, uint32 *FirstBad_p, uint8 *Data_p);

/**
 * @brief Writes data in CABS block device manager
 *
 * @param[in]     Unit        CABS Unit that data should be written to.
 * @param[in]     Pos         CABS sector where writting should start.
 * @param[in]     Length      Number of sectors to write.
 * @param[in,out] FirstBad_p  See r_cabs.h for more info.
 * @param[in]     Data_p      Input data buffer.
 *
 * @return ErrorCode_e Forwards the response from CABS module.
 *
 * @remark This function should be called only from BDM module.
 */
ErrorCode_e Do_BDM_CABS_Write(uint32 Unit, uint32 Pos, uint32 Length, uint32 *FirstBad_p, uint8 *Data_p);

/**
 * @brief Returns CABS configuration
 *
 * @param[in]  Unit             CABS Unit that configuration is read from.
 * @param[out] Configuration_p  Pointer to returned configuration.
 *
 * @return ErrorCode_e Forwards the response from CABS module.
 *
 * @remark This function should be called only from BDM module.
 */
ErrorCode_e Do_BDM_CABS_GetInfo(uint32 Unit, BDM_Unit_Config_t *Configuration_p);

/**
 * @brief Returns function pointers needed for file system operation
 *
 * @param[in]  Unit          CABS Unit.
 * @param[out] Device_p      DDB Handle.
 * @param[out] Functions_pp  Returned function pointers.
 *
 * @return ErrorCode_e Forwards the response from CABS module.
 *
 * @remark This function should be called only from BDM module.
 */
ErrorCode_e Do_BDM_CABS_GetFSInterface(uint32 Unit, DdbHandle *Device_p, DdbFunctions **Functions_pp);

/**
 * @brief Junks consecutive logical sectors.
 *
 * @param[in]      Unit        CABS Unit.
 * @param[in]      Position    Logical sector number.
 * @param[in]      NrOfSectors Number of sectors to junk.
 * @param[in, out] FirstBad_p  Pointer to storage of the index of the first
 *                             sector that was not junked when error occurred.
 *                             If no error has occurred the contents pointed to
 *                             by \c FirstBad_p will be set to
 *                             \c CABS_NO_SECTOR_ERROR. \c NOTE: The logical
 *                             sector referred to by \c Position is at index 0,
 *                             the next sector is at index 1 and so on.
 *
 * @return     E_SUCCESS       Logical sectors was successfully junked.
 *
 * @return ErrorCode_e Forwards the response from CABS module.
 */
ErrorCode_e Do_BDM_CABS_Junk(uint32 Unit, uint32 Position, uint32 NrOfSectors, uint32 *FirstBad_p);

/** @} */
/** @} */
#endif //_BDM_CABS_H_
