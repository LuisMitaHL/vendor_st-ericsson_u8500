/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef INCLUSION_GUARD_COPS_HAL_SECURITY_H
#define INCLUSION_GUARD_COPS_HAL_SECURITY_H

/**
 * @file  t_cops_extint_security.h
 * @brief Type and constant declarations for Security
 *
 * @addtogroup ldr_service_management
 * @{
 *    @addtogroup ldr_service_cops COPS Data Manager
 *    @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "cops_common.h"
#include "cops_storage.h"
#include "t_hal_security.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/

/*TYPEDEF_ENUM { //xvsvlpi: temprary comment
    COPS_EXTINT_SECURITY_DATA_INTEGRITY_DOMAIN,
    COPS_EXTINT_SECURITY_DATA_INTEGRITY_STATIC_DATA,
    COPS_EXTINT_SECURITY_DATA_INTEGRITY_DYNAMIC_DATA_1,
    COPS_EXTINT_SECURITY_DATA_INTEGRITY_DYNAMIC_DATA_2,
    COPS_EXTINT_SECURITY_DATA_INTEGRITY_DYNAMIC_DATA_3,
    COPS_EXTINT_SECURITY_DATA_INTEGRITY_SECURE_SETTINGS,
}ENUM32(COPS_ExtInt_Security_DataIntegrity_t);
*/
/**
 * COPS_ExtInt_Security_SHA256_Init_t
 *
 * Initializes a message digesting with the SHA-256 algorithm.
 *
 * @param [in]  Context_pp          Context pointer pointer *Context_pp has to be NULL
 * @param [in]  Data_p              Data to calculate hash on.
 * @param [in]  Length              Size of data
 *
 */
typedef COPS_RC_t(*COPS_ExtInt_Security_SHA256_Init_t)
(
    void                    **Context_pp
);


/**
 * COPS_ExtInt_Security_SHA256_Update_t
 *
 * Initializes a message digesting with the SHA-256 algorithm.
 *
 * @param [in]  Context_pp          Context pointer pointer
 * @param [in]  Data_p              Data to calculate hash on.
 * @param [in]  Length              Size of data
 *
 */
typedef COPS_RC_t(*COPS_ExtInt_Security_SHA256_Update_t)
(
    void                    **Context_pp,
    const uint8              *const Data_p,
    const uint32                    Length
);


/**
 * COPS_ExtInt_Security_SHA256_Final_t
 *
 * Initializes a message digesting with the SHA-256 algorithm.
 *
 * @param [in]  Context_pp          Context pointer pointer
 * @param [out] Calculated_Hash     Calculated hash
 *
 */
typedef COPS_RC_t(*COPS_ExtInt_Security_SHA256_Final_t)
(
    void                    **Context_pp,
    COPS_Storage_MAC_t *const Calculated_Hash_p
);
/**
 * COPS_ExtInt_Security_GetRNG_t
 *
 * Generate a random number
 *
 * @param [out] Data_p     Random number
 * @param [in]  Length     Requested length
 *
 */

typedef COPS_RC_t(*COPS_ExtInt_Security_GetRNG_t)
(
    uint8              *const Data_p,
    const uint32                    Length
);



// Prototype to verify SDD or SDD data MAC
/*typedef COPS_RC_t(*COPS_ExtInt_Security_VerifyMAC_t) //xvsvlpi : removed
(
  const COPS_ExtInt_Security_DataIntegrity_t        MAC_Type,
  const COPS_Storage_MAC_t*                   const Reference_MAC_p,
  const uint8*                                const Data_p,
        uint32                                      Length,
        boolean*                              const VerifyOK_p
);
*/
// Prototype to calculate SDD or SDD data MAC/
/*typedef COPS_RC_t(*COPS_ExtInt_Security_CalculateMAC_t) //xvsvlpi : removed
(
  const COPS_ExtInt_Security_DataIntegrity_t        MAC_Type,
        COPS_Storage_MAC_t*                   const Calculated_MAC_p,
  const uint8*                                const Data_p,
  const uint32                                      Length
);
*/
// Prototype to verify a footprint
typedef COPS_RC_t(*COPS_ExtInt_Security_VerifyFootprint_t)
(
    const COPS_Storage_MAC_t *const HashOfSecret_p,
    const COPS_Storage_MAC_t *const Refference_Footprint,
    const uint8              *const Data_p,
    const uint32                    Length,
    boolean            *const VerifyOK_p
);

// Prototype to calculate a footprint
/*typedef COPS_RC_t(*COPS_ExtInt_Security_CalculateFootprint_t) //xvsvlpi : removed
(
  const COPS_Storage_MAC_t* const HashOfSecret_p,
        COPS_Storage_MAC_t* const Calculated_Footprint_p,
  const uint8*              const Data_p,
  const uint32                    Length
);
*/
// TODO: Update
// Prototype to verify a Certificate
typedef COPS_RC_t(*COPS_ExtInt_Security_VerifySignature_t)
(
    const uint8    *const SignedObject_p,
    const uint32          ObjectLength,
    uint32   *const PayloadOffset_p,
    uint32   *const CertificatePurpose_p,
    boolean  *const VerifyOK_p
);

typedef COPS_RC_t(*COPS_ExtInt_Security_ReadOTP_t)
(
    COPS_OTP_t *const OTP_p
);

typedef COPS_RC_t(*COPS_ExtInt_Security_WriteOTP_t)
(
    const uint8                  *const OTP_Data_p,
    const uint32                        OTP_DataLength,
    const COPS_OTP_Overrides_t   *const OTP_Overrides_p,
    const boolean                       LockCheck
);

typedef COPS_RC_t(*COPS_ExtInt_Security_ReadDomain_t)
(
    HAL_Security_Domain_t      *const Domain_p
);

typedef COPS_RC_t(*COPS_ExtInt_Security_WriteDomain_t)
(
    HAL_Security_Domain_t   Domain
);


/*typedef struct //xvsvlpi: removed
{
  COPS_ExtInt_Security_SHA256_Init_t        Do_SHA256_Init_p;
  COPS_ExtInt_Security_SHA256_Update_t      Do_SHA256_Update_p;
  COPS_ExtInt_Security_SHA256_Final_t       Do_SHA256_Final_p;
  COPS_ExtInt_Security_GetRNG_t             Do_GetRNG_p;
  COPS_ExtInt_Security_VerifyMAC_t          Do_VerifyMAC_p;
  COPS_ExtInt_Security_CalculateMAC_t       Do_CalculateMAC_p;
  COPS_ExtInt_Security_VerifyFootprint_t    Do_VerifyFootprint_p;
  COPS_ExtInt_Security_CalculateFootprint_t Do_CalculateFootprint_p;
  COPS_ExtInt_Security_VerifySignature_t    Do_VerifySignature_p;
  COPS_ExtInt_Security_ReadOTP_t            Do_ReadOTP_p;
  COPS_ExtInt_Security_WriteOTP_t           Do_WriteOTP_p;
  COPS_ExtInt_Security_ReadDomain_t         Do_ReadDomain_p;
  COPS_ExtInt_Security_WriteDomain_t        Do_WriteDomain_p;
}COPS_ExtInt_Security_t;
*/
/** @} */
/** @} */
#endif /*INCLUSION_GUARD_COPS_HAL_SECURITY_H*/
