/*******************************************************************************
 * $Copyright Ericsson AB 2009 $
 ******************************************************************************/
#ifndef _BDM_EMMC_H_
#define _BDM_EMMC_H_

/**
 * @file  bdm_emmc.h
 * @brief Function and type declarations for EMMC block device manager
 *
 * @addtogroup ldr_service_bdm Block Device Manager
 * @{
 *   @addtogroup ldr_service_bdm_emmc EMMC Block Device Manager
 *   @{
 */

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/
/**
 * @brief Starts EMMC block device manager
 *
 * @return ErrorCode_e Forwards the response from EMMC module.
 *
 * @remark This function should be called only from BDM module.
 */
ErrorCode_e Do_BDM_EMMC_Start(void);

/**
 * @brief Stops EMMC block device manager
 *
 * @return ErrorCode_e Forwards the response from EMMC module.
 *
 * @remark This function should be called only from BDM module.
 */
ErrorCode_e Do_BDM_EMMC_Stop(void);

/**
 * @brief Reads data from EMMC block device manager
 *
 * @param[in]     FPDConfig_p Flash device configuration of BDM
 * @param[in]     Pos         EMMC sector where reading should start.
 * @param[in]     Length      Number of sectors to read.
 * @param[in,out] FirstBad_p  See r_EMMC.h for more info.
 * @param[out]    Data_p      Output buffer.
 *
 * @return ErrorCode_e Forwards the response from EMMC module.
 *
 * @remark This function should be called only from BDM module.
 */
ErrorCode_e Do_BDM_EMMC_Read(FPD_Config_t *const FPDConfig_p, uint32 Pos, uint32 Length, uint32 *FirstBad_p, uint8 *Data_p);

/**
 * @brief Writes data in EMMC block device manager
 *
 * @param[in]     FPDConfig_p Flash device configuration of BDM
 * @param[in]     Pos         EMMC sector where writting should start.
 * @param[in]     Length      Number of sectors to write.
 * @param[in,out] FirstBad_p  See r_EMMC.h for more info.
 * @param[in]    Data_p      Input data buffer.
 *
 * @return ErrorCode_e Forwards the response from EMMC module.
 *
 * @remark This function should be called only from BDM module.
 */
ErrorCode_e Do_BDM_EMMC_Write(FPD_Config_t *const FPDConfig_p, uint32 Pos, uint32 Length, uint32 *FirstBad_p, uint8 *Data_p);

/**
 * @brief Returns EMMC configuration
 *
 * @param[in]  FPDConfig_p      Flash device configuration of BDM
 * @param[out] Configuration_p  Pointer to returned configuration.
 *
 * @return ErrorCode_e Forwards the response from EMMC module.
 *
 * @remark This function should be called only from BDM module.
 */
ErrorCode_e Do_BDM_EMMC_GetInfo(FPD_Config_t *const FPDConfig_p, BDM_Unit_Config_t *Configuration_p);


/**
 * @brief Junks consecutive logical sectors.
 *
 * @param[in]      FPDConfig_p   Flash device configuration of BDM
 * @param[in]      Position      Logical sector number.
 * @param[in]      NrOfSectors   Number of sectors to junk.
 * @param[in, out] FirstBad_p    Not used for eMMC.
 * @return         E_SUCCESS     Logical sectors was successfully junked.
 *
 * @return ErrorCode_e Forwards the response from CABS module.
 */
ErrorCode_e Do_BDM_EMMC_Junk(FPD_Config_t *const FPDConfig_p, uint32 Position, uint32 NrOfSectors, uint32 *FirstBad_p);

/** @} */
/** @} */
#endif /*_BDM_EMMC_H_*/
