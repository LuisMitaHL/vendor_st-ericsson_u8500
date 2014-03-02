/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _R_PATH_UTILITIES_H_
#define _R_PATH_UTILITIES_H_

/**
 * @addtogroup ldr_utilities
 * @{
 *   @addtogroup path_utilities
 *   @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_basicdefinitions.h"

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/
/**
 * @brief Check if path is in one of secured folders.
 *
 * @param [in]   PathName_p - File system path.
 * @return TRUE  If folder is secured.
 * @return FALSE If folder is not secured.
 */
boolean CheckIfSecuredPath(const char *PathName_p);

/**
 * @brief Check if incoming file is properly signed
 *
 * @param [in]   PathName_p - File system path.
 * @return TRUE  If file is signed properly.
 * @return FALSE If file is not signed properly.
 */
boolean VerifyIncomingFile(const char *PathName_p);

/**
 * @brief Extracting path from general path
 *
 * @param [in]  GeneralPath_p - File system path.
 * @return      Pointer to nul terminated copy of path string.
 * @retval      on error NULL pointer.
 *
 * @note       User must free allocated space pointed by return pointer
 */
char *ExtractPath(const char *GeneralPath_p);

/**
 * @brief Extracting path from general path
 *
 * @param [in] GeneralPath_p  File system path.
 * @return     Pointer to nul terminated copy of path string.
 * @retval     on error NULL pointer.
 *
 * @note       User must free allocated space pointed by return pointer
 */
char *ExtractDevice(const char *GeneralPath_p);

/**
 * @brief Serching if path represent bulk path
 *
 * @param [in]  Path_p - File system or bulk id path.
 * @return TRUE If path is bulk and FALSE otherwise.
 */
boolean SearchBulkPath(const char *Path_p);

/**
 * @brief Get last word from device path
 *        Counting from last back slash till the end of device string
 *
 * @param [in] Device_p  Device path.
 * @return     Pointer to nul terminated copy of path string.
 * @retval     on error NULL pointer.
 *
 * @note       User must free allocated space pointed by return pointer
 */
char *GetLastWord(void *Device_p);

/**
 * @brief Get last word from device path with a known length
 *        Counting from last back slash till the end of device string
 *
 * @param [in] Device_p  Device path.
 * @param [in] DeviceLength  Length of device path string.
 * @return     Pointer in Device_p to last word
 *
 * @note       This doesn't copy the string but only give a pointer
 *             into Device_p, also Device_p doesn't need to be nul
 *             terminated.
 */
char *GetLastWordn(char *Device_p, uint32 DeviceLength, uint32 *LastWordLength_p);

/**
 * @brief Gets the file name from a given path
 *
 * @param [in] Path_p  Input path.
 * @return     Pointer to nul terminated file name string.
 * @retval     on error NULL pointer.
 *
 * @note       User must free allocated space pointed by return pointer
 */
char *GetFileName(const char *Path_p);

/**
 * @brief Gets the device number.
 *
 * @param [in] Device_p  Device.
 *
 * @return     The number of the device.
 */
uint32 GetDeviceNumber(void *Device_p);

/** @} */
/** @} */
#endif /*_R_ATOMIC_H_*/
