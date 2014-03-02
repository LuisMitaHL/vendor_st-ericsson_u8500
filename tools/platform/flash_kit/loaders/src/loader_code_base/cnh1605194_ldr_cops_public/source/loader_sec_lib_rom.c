/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
/**
 *  @addtogroup ldr_security_library
 *  @{
 *      @addtogroup rom_functions
 *      @{
 */
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_esbrom.h"

#ifdef WIN32
#include "t_basicdefinitions.h"
#include <stdlib.h>
#include <string.h>

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/
const uint32 rred[16] = {
    0,
    (0x1 << 16) ^(0x1021 << 0), \
    (0x2 << 16) ^(0x1021 << 1), \
    (0x3 << 16) ^(0x1021 << 1) ^(0x1021 << 0), \
    (0x4 << 16) ^(0x1021 << 2), \
    (0x5 << 16) ^(0x1021 << 2) ^(0x1021 << 0), \
    (0x6 << 16) ^(0x1021 << 2) ^(0x1021 << 1), \
    (0x7 << 16) ^(0x1021 << 2) ^(0x1021 << 1) ^(0x1021 << 0), \
    (0x8 << 16) ^(0x1021 << 3), \
    (0x9 << 16) ^(0x1021 << 3) ^(0x1021 << 0), \
    (0xa << 16) ^(0x1021 << 3) ^(0x1021 << 1), \
    (0xb << 16) ^(0x1021 << 3) ^(0x1021 << 1) ^(0x1021 << 0), \
    (0xc << 16) ^(0x1021 << 3) ^(0x1021 << 2), \
    (0xd << 16) ^(0x1021 << 3) ^(0x1021 << 2) ^(0x1021 << 0), \
    (0xe << 16) ^(0x1021 << 3) ^(0x1021 << 2) ^(0x1021 << 1), \
    (0xf << 16) ^(0x1021 << 3) ^(0x1021 << 2) ^(0x1021 << 1) ^(0x1021 << 0)
};

/*******************************************************************************
 * Declaration of file local functions
 ******************************************************************************/
static ESBROM_Return_t SHA256(uint8 *p, uint32 count, uint8 *Hash_p);

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/

/*
 * Initiates a MAC generation operation.
 *
 * @param[in,out] MacSessionCtx_p                     Pointer to a Mac Session context.
 * @param[in]     MacType                             The type of Mac to use with this session.
 * @param[in]     MacKey_p                            Pointer to a Mac key.  If the MacType indicates that an external key is to be used,
 *                                                    this argument should point to the key. In other cases this parameter is ignored.
 * @param[in]     MacKeySize                          The size of the key, in bytes. Only used in external key mode. Ignored otherwise.
 *
 * @retval ESBROM_RETURN_OK                           Operation succeeded.
 * @retval ESBROM_RETURN_ARGUMENT_ERROR               The MacType argument is out of bounds.
 * @retval ESBROM_RETURN_ERROR_CHECKSUM               The integrity of the MacSessionCtx is broken.
 * @retval ESBROM_RETURN_ILLEGAL_STATE_CHANGE         The session is in the wrong state for doing an init.
 * @retval ESBROM_RETURN_MAC_ERROR_LOCKED             Operation not possible because the MAC has been locked down.
 * @retval ESBROM_RETURN_NOT_IMPLEMENTED              This return code means that this interface is not implemented.  See the c_esbrom.h file for configuration.
 * @retval ESBROM_RETURN_NOT_SUPPORTED                This return code means that this interface does not support a certain mode of operation.  See the c_esbrom.h file for configuration.
 *
 * @see    ESBROM_MAC_Update, ESBROM_MAC_Cancel, ESBROM_MAC_Final_Gen, ESBROM_MAC_Context_Get, ESBROM_MAC_Context_Set
 */
ESBROM_Return_t   R_Do_ESBROM_MAC_Init_Gen(
    ESBROM_MacSessionCtx_t             *const MacSessionCtx_p,
    const ESBROM_MacType_t                          MacType,
    const ESBROM_KeyUnknownSize_t            *const MacKey_p,
    const ESBROM_KeySize_t                          MacKeySize)
{
    return ESBROM_RETURN_OK;
}

/*
 * Initiates a MAC compare operation.
 *
 * @param[in,out] MacSessionCtx_p                     Pointer to a Mac Session context.
 * @param[in]     MacType                             The type of Mac to use with this session.
 * @param[in]     MacKey_p                            Pointer to a Mac key.  If the MacType indicates that an external key is to be used,
 *                                                    this argument should point to the key. In other cases this parameter is ignored.
 * @param[in]     MacKeySize                          The size of the key, in bytes. Only used in external key mode. Ignored otherwise.
 *
 * @retval ESBROM_RETURN_OK                           Operation succeeded.
 * @retval ESBROM_RETURN_ARGUMENT_ERROR               The MacType argument is out of bounds.
 * @retval ESBROM_RETURN_ERROR_CHECKSUM               The integrity of the MacSessionCtx is broken.
 * @retval ESBROM_RETURN_ILLEGAL_STATE_CHANGE         The session is in the wrong state for doing an init.
 * @retval ESBROM_RETURN_NOT_IMPLEMENTED              This return code means that this interface is not implemented.  See the c_esbrom.h file for configuration.
 * @retval ESBROM_RETURN_NOT_SUPPORTED                This return code means that this interface does not support a certain mode of operation.  See the c_esbrom.h file for configuration.
 *
 * @see   ESBROM_MAC_Update, ESBROM_MAC_Cancel, ESBROM_MAC_Final_Cmp, ESBROM_MAC_Context_Get, ESBROM_MAC_Context_Set
 */
ESBROM_Return_t   R_Do_ESBROM_MAC_Init_Cmp(
    ESBROM_MacSessionCtx_t             *const MacSessionCtx_p,
    const ESBROM_MacType_t                          MacType,
    const ESBROM_KeyUnknownSize_t            *const MacKey_p,
    const ESBROM_KeySize_t                          MacKeySize)
{
    return  ESBROM_RETURN_OK;
}

/*
 * Continues a MAC operation, feeding it with data.
 *
 * @param[in,out] MacSessionCtx_p                     Pointer to a Mac Session context.
 * @param[in]     Data_p                              Pointer to data.
 * @param[in]     DataLength                          Length of the data.
 *
 * @retval ESBROM_RETURN_OK                           Operation succeeded.
 * @retval ESBROM_RETURN_ILLEGAL_STATE_CHANGE         The session is in the wrong state for doing an update.
 * @retval ESBROM_RETURN_ERROR_CHECKSUM               The integrity of the supplied data structure is compromised.
 * @retval ESBROM_RETURN_NOT_IMPLEMENTED              This return code means that this interface is not implemented.  See the c_esbrom.h file for configuration.
 *
 * @see    ESBROM_MAC_Init_Gen, ESBROM_MAC_Init_Cmp, ESBROM_MAC_Cancel, ESBROM_MAC_Final_Gen,
 *         ESBROM_MAC_Final_Cmp, ESBROM_MAC_Context_Get, ESBROM_MAC_Context_Set
 */
ESBROM_Return_t   R_Do_ESBROM_MAC_Update(
    ESBROM_MacSessionCtx_t             *const MacSessionCtx_p,
    const uint8                                    *Data_p,
    const uint32                                    DataLength)
{
    ESBROM_Return_t RetValue = ESBROM_RETURN_ERROR_CHECKSUM;

    RetValue = SHA256((uint8 *)Data_p, (uint32)DataLength, (uint8 *) & (MacSessionCtx_p->MacBuffer.Buffer[0]));

    return  RetValue;
}

/*
 * Ends a MAC generation operation.
 *
 * @param[in,out] MacSessionCtx_p                     Pointer to a Mac Session context.
 * @param[out]    Mac_p                               Pointer to the produced MAC.
 *
 * @retval ESBROM_RETURN_OK                           Operation succeeded.
 * @retval ESBROM_RETURN_ILLEGAL_STATE_CHANGE         The session is in the wrong state for doing a final.
 * @retval ESBROM_RETURN_ERROR_CHECKSUM               The integrity of the supplied data structure is compromised.
 * @retval ESBROM_RETURN_ERROR_MAC_EXPECTED_FINAL_CMP Expected the final generation operation.
 * @retval ESBROM_RETURN_NOT_IMPLEMENTED              This return code means that this interface is not implemented.  See the c_esbrom.h file for configuration.
 *
 * @see    ESBROM_MAC_Init_Gen, ESBROM_MAC_Update, ESBROM_MAC_Cancel, ESBROM_MAC_Context_Get, ESBROM_MAC_Context_Set
 */
ESBROM_Return_t   R_Do_ESBROM_MAC_Final_Gen(
    ESBROM_MacSessionCtx_t             *const MacSessionCtx_p,
    ESBROM_Mac_t                             *Mac_p)
{

    memcpy(&Mac_p->Mac[0], (uint8 *) & (MacSessionCtx_p->MacBuffer.Buffer[0]), 32);
    return ESBROM_RETURN_OK;
}

/*
 * Ends a MAC compare operation.
 *
 * @param[in,out] MacSessionCtx_p                     Pointer to a Mac Session context.
 * @param[in]     Mac_p                               Pointer to the MAC to compare against.
 * @param[out]    CompareResult_p                     Pointer to the produced MAC.
 *
 * @retval ESBROM_RETURN_OK                           Operation succeeded.
 * @retval ESBROM_RETURN_ILLEGAL_STATE_CHANGE         The session is in the wrong state for doing a final.
 * @retval ESBROM_RETURN_ERROR_CHECKSUM               The integrity of the supplied data structure is compromised.
 * @retval ESBROM_RETURN_ERROR_MAC_EXPECTED_FINAL_GEN Expected the final compare operation.
 * @retval ESBROM_RETURN_NOT_IMPLEMENTED              This return code means that this interface is not implemented.  See the c_esbrom.h file for configuration.
 *
 * @see    ESBROM_MAC_Init_Cmp, ESBROM_MAC_Update, ESBROM_MAC_Cancel, ESBROM_MAC_Context_Get, ESBROM_MAC_Context_Set
 */
ESBROM_Return_t   R_Do_ESBROM_MAC_Final_Cmp(
    ESBROM_MacSessionCtx_t             *const MacSessionCtx_p,
    const ESBROM_Mac_t                 *Mac_p,
    ESBROM_CompareResult_t             *const CompareResult_p)
{
    return ESBROM_RETURN_OK;
}


/*
 * Makes a complete MAC generation operation with the specified data.
 *
 * @param[in]     MacType                             The type of Mac to use with this session.
 * @param[in]     MacKey_p                            Pointer to a Mac key.  If the MacType indicates that an external key is to be used,
 *                                                    this argument should point to the key. In other cases this parameter is ignored.
 * @param[in]     MacKeySize                          The size of the key, in bytes. Only used in external key mode. Ignored otherwise.
 * @param[in]     Data_p                              Pointer to data to process.
 * @param[in]     DataLength                          The length of the data in bytes.
 * @param[out]    Mac_p                               Pointer to the resulting MAC.
 *
 * @retval ESBROM_RETURN_OK                           Operation succeeded.
 * @retval ESBROM_RETURN_ARGUMENT_ERROR               The MacType argument is out of bounds.
 * @retval ESBROM_RETURN_MAC_ERROR_LOCKED             Operation not possible because the MAC has been locked down.
 * @retval ESBROM_RETURN_NOT_IMPLEMENTED              This return code means that this interface is not implemented.  See the c_esbrom.h file for configuration.
 * @retval ESBROM_RETURN_NOT_SUPPORTED                This return code means that this interface does not support a certain mode of operation.  See the c_esbrom.h file for configuration.
 *
 * @see    ESBROM_MAC_Cmp
 */
ESBROM_Return_t   R_Do_ESBROM_MAC_Gen(
    const ESBROM_MacType_t                          MacType,
    const ESBROM_KeyUnknownSize_t                  *MacKey_p,
    const ESBROM_KeySize_t                          MacKeySize,
    const uint8                                    *Data_p,
    const uint32                                    DataLength,
    ESBROM_Mac_t                       *const Mac_p)
{

    return ESBROM_RETURN_OK;
}


/*
 * Makes a complete MAC compare operation with the specified data.
 *
 * @param[in]     MacType                             The type of Mac to use with this session.
 * @param[in]     MacKey_p                            Pointer to a Mac key.  If the MacType indicates that an external key is to be used,
 *                                                    this argument should point to the key. In other cases this parameter is ignored.
 * @param[in]     Data_p                              Pointer to data to process.
 * @param[in]     MacKeySize                          The size of the key, in bytes. Only used in external key mode. Ignored otherwise.
 * @param[in]     DataLength                          The length of the data in bytes.
 * @param[in]     Mac_p                               Pointer to the MAC to compare against.
 * @param[out]    CompareResult_p                     Pointer to the result of the compare.
 *
 * @retval ESBROM_RETURN_OK                           Operation succeeded.
 * @retval ESBROM_RETURN_ARGUMENT_ERROR               The MacType argument is out of bounds.
 * @retval ESBROM_RETURN_NOT_IMPLEMENTED              This return code means that this interface is not implemented.  See the c_esbrom.h file for configuration.
 * @retval ESBROM_RETURN_NOT_SUPPORTED                This return code means that this interface does not support a certain mode of operation.  See the c_esbrom.h file for configuration.
 *
 * @see    ESBROM_MAC_Gen
 */
ESBROM_Return_t   R_Do_ESBROM_MAC_Cmp(
    const ESBROM_MacType_t                          MacType,
    const ESBROM_KeyUnknownSize_t            *const MacKey_p,
    const ESBROM_KeySize_t                          MacKeySize,
    const uint8                                    *Data_p,
    const uint32                                    DataLength,
    const ESBROM_Mac_t                       *const Mac_p,
    ESBROM_CompareResult_t             *const CompareResult_p)
{

    return ESBROM_RETURN_OK;
}


static ESBROM_Return_t SHA256(uint8 *p, uint32 count, uint8 *Hash_p)
{
    register uint32 crc = 1;
    uint32 Var;
    int i;

    while (--(sint32)count >= 0) {
        crc = crc ^(uint8) * p++ << 8;
        crc ^= rred[crc >> 16];
        crc = (crc << 4);
        crc ^= rred[crc >> 16];
        crc = (crc << 4);
        crc ^= rred[crc >> 16];
    }

    Var = crc;

    for (i = 0; i < 8; i++) {
        memcpy((uint8 *)(Hash_p + i * sizeof(uint32)), (uint8 *)&Var, sizeof(uint32));
    }

    return ESBROM_RETURN_OK;
}

#endif      //#ifndef WIN32

/*******************************************************************************
 * Definition of internal functions
 ******************************************************************************/

/** @} */
/** @} */
