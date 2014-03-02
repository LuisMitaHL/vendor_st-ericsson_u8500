/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file	   painfo.h
* \brief   this module declares HATS RF service related to PA INFO
* \author  ST-Ericsson
*/
/*****************************************************************************/

#if !defined(HATS_RF_PA_INFO_H__)
#define HATS_RF_PA_INFO_H__

/* Type: PA_INFO_TYPE */
enum PA_INFO_TYPE {
    PA_INFO_TYPE_NONE           = 0,
    PA_INFO_TYPE_PA_INFO        = 0x0001,
    PA_INFO_TYPE_PA_BIAS_INFO   = 0x0002,
    PA_INFO_TYPE_PA_ALL         = 0xFFFF
};

/** save or get information of PA's in phone.
 * If the operation succeeds then the PA info and/or PA bias info are available
 * after the call. painfo_infoAvailable() tells which ones are available.
 * \param pp_type [in/out] non null pointer to a combination of PA_TYPE_INFO.
 * \param vp_system [in] system information: a SYSTEM value
 * \param vp_saveOrUpdate [in] C_TEST_FALSE: get PA type from DSP.
 * C_TEST_TRUE: ask DSP to save PA type to Permanent Memory.
 * \constraint do not call while using tatl17isi facilities. It would result in
 * a buffer collision.
 * \return a TAT_ERROR code
 */
int painfo_saveOrUpdatePAInfo(int *pp_type, uint16 vp_system,
    uint16 vp_saveOrUpdate);

/** return which types of PA's information is available
 * \return a combination of values from PA_INFO_TYPE
 */
int painfo_infoAvailable();

/** get the PA's version and vendor information, if available.
 * Must have read the PA's information before calling this function.
 * \param pp_info [out] non null pointer to receive PA info data.
 * \retval 0 if success then pp_info is filled with PA information.
 * \retval -1 if no information available
 * \retval -2 under-sized, would overflow. pp_info is unchanged.
 */
int painfo_getPAInfo(C_TEST_SB_PA_INFO_STR *pp_info, size_t vp_size);

/** get the PA's bias information, it retrieved.
 * \param pp_info [out] non null pointer to receive PA bias info.
 * \return 0 if PA info is available else -1
 */
int painfo_getPABiasInfo(C_TEST_SB_PA_BIAS_INFO_STR *pp_info);

#endif /* !defined(HATS_RF_PA_INFO_H__) */
