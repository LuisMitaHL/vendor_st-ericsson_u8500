/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*------------------------------------------------------------------------
 *                               TPA / MPU
 *                   MultiMedia Technology Center (MMTC)
 *------------------------------------------------------------------------
 * This module stands for the secure ROM API.
 * It represents the only way to accede secure resources apart from the
 * boot phase.
 *
 *------------------------------------------------------------------------
 *
 ****************************************************************************/


/*--------------------------------------------------------------------------*
 * Includes                                                                 *
 *--------------------------------------------------------------------------*/
#include <stdarg.h>

#include "securityp.h"

/*--------------------------------------------------------------------------*
 *  Global variables                                                        *
 *--------------------------------------------------------------------------*/
// Pointer to functions as secure mode entry points by the upper layer.
// hw_sec_rom_pub_bridge not delcared as PRIVATE due to a link error (?).
typedef t_uint32 (*t_boot_rom_bridge_func)( t_sec_rom_service_id , t_uint32, va_list);
PUBLIC t_boot_rom_bridge_func hw_sec_rom_pub_bridge = (void *)NOT_INITIALIZED;
PRIVATE t_uint8 CutNb = NOT_INITIALIZED;

typedef t_uint32 (*t_boot_rom_phys_addr_func) ( t_uint32 );
PRIVATE t_boot_rom_phys_addr_func hw_mmu_physical_address_get = (void *)NOT_INITIALIZED;


/*--------------------------------------------------------------------------*
 *  Private data                                                            *
 *--------------------------------------------------------------------------*/

PRIVATE const t_sec_rom_cut_desc NomadikCutDescTab[] =
{
    // Checksum addr,   Id value,       Conversion func,    Bridge func,    Cut name
    { 0xFFFFDBF4,       0x009540A0,     0xFFFF7588,         0xFFFF7300,     0x9540A0 },
    { 0x9001DBF4,       0x008500B2,     0x90017588,         0x90017300,     0x8500B2 },
    { 0x9001DBF4,       0x008500B1,     0x90017588,         0x90017300,     0x8500B1 },
    { 0x9001DBF4,       0x008500B0,     0x90017588,         0x90017300,     0x8500B0 },
    { 0x9001FFF4,       0x00850001,     0x90018588,         0x90018300,     0x850001 },
    { 0x9001FFF4,       0x008500A0,     0x90018588,         0x90018300,     0x8500A0 },
    { 0x9001FFF4,       0x008500A1,     0x90018588,         0x90018300,     0x8500A1 },
    { 0x9001FFF4,       0x005500A0,     0x90018588,         0x90018300,     0x5500A0 },
    { 0x9001FFF4,       0x005500B0,     0x90018588,         0x90018300,     0x5500B0 }
};



/*--------------------------------------------------------------------------*
 *  Public functions                                                        *
 *--------------------------------------------------------------------------*/
/****************************************************************************/
/* NAME:        SECURITY_Init(t_logical_address BaseAddress)                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine initializes addresses.                         */
/*                                                                          */
/* PARAMETERS:                                                              */
/* IN :         logBaseAddress: current logical base address.               */
/* OUT :        None                                                        */
/* RETURN:                                                                  */
/*              None                                                        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/
PUBLIC void SECURITY_Init( void )
{
    CutNb = 0;      // Used to the NomadikCutDescTab array

    // Reads checksum information contained in the ROM code and compare to known values for each existing code
    while(  (CutNb < sizeof(NomadikCutDescTab)/sizeof(t_sec_rom_cut_desc)) &&
            (NomadikCutDescTab[CutNb].ChecksumValue != * (t_uint32 *)(NomadikCutDescTab[CutNb].ChecksumAddr))
         )
    {
        CutNb++;
    }

    // Assignes addresses of ROM-located functions required by the HCL if the ROM code version has been recognized
    if( CutNb < sizeof(NomadikCutDescTab)/sizeof(t_sec_rom_cut_desc) )
    {
        hw_sec_rom_pub_bridge       = (t_boot_rom_bridge_func)   (NomadikCutDescTab[CutNb].BridgeFunc);
        hw_mmu_physical_address_get = (t_boot_rom_phys_addr_func)(NomadikCutDescTab[CutNb].ConversionFunc);
    }
}


/****************************************************************************/
/* NAME:        SECURITY_GetVersion()                                       */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine returns the HCL current development version.   */
/*                                                                          */
/* PARAMETERS:                                                              */
/* IN :         None                                                        */
/* OUT :        p_version:  Pointer at the current HCL version.             */
/* RETURN:                                                                  */
/*              None                                                        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/
PUBLIC void SECURITY_GetVersion( t_version * p_version )
{
    p_version->version = SECURE_HCL_VERSION_ID;
    p_version->major = SECURE_HCL_MAJOR_ID;
    p_version->minor = SECURE_HCL_MINOR_ID;
}


/****************************************************************************/
/* NAME:        SECURITY_GetRomVersion()                                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine returns the ROM code current version.          */
/*                                                                          */
/* PARAMETERS:                                                              */
/* IN :         None                                                        */
/* OUT :        p_romVersion:   Pointer to the current ROM code version.    */
/* RETURN:                                                                  */
/*              None                                                        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/
PUBLIC void SECURITY_GetRomVersion( t_uint32 *p_romVersion)
{
    if( CutNb != NOT_INITIALIZED )
    {
        *p_romVersion =  NomadikCutDescTab[CutNb].CutName;
    }
    else
    {
        *p_romVersion = NOT_INITIALIZED;
    }
}


/****************************************************************************/
/* NAME:        SECURITY_CallSecureService()                                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine calls the secure service in parameter with the */
/*              defined configuration.                                      */
/*              Note that this function stands for the only way to access   */
/*              any secure resources.                                       */
/*                                                                          */
/* PARAMETERS:                                                              */
/* IN :         serviceId:      Identification of the secure service to be  */
/*                              executed.                                   */
/*              secureConfig:   Secure environment configuration (caches,   */
/*                              exceptions, MMU).                           */
/*              ...:            Variable parameter list (could be needed by */
/*                              some secure services).                      */
/* OUT :        None                                                        */
/*                                                                          */
/* RETURN:                                                                  */
/*              t_sec_rom_response                                          */
/*                              Error management for the secure environment.*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/


/*
 * To call ROM code compiled with RVCTX.X (ie STn8820 and STn8500) from
 * Xloader compiled with RVCT or GNU compiler
 */
#if ( ( defined(__ARMCC_VERSION) && (defined(__STN_8820) || defined(__STN_8500) || defined(__STN_5500)) )  || \
      ( defined(__GNUC__) )                                                         || \
      ( defined(__ARMV4I_WE) ) )

#if ( defined(__ARMV4I_WE) )
    #warning "Security HCL: Code not yet tested with this compiler"
#endif

PUBLIC t_sec_rom_response SECURITY_CallSecureService(   t_sec_rom_service_id serviceId,
                                                        t_sec_rom_exec_config secureConfig,
                                                        ...
                                                    )
{
    va_list ap;
    t_sec_rom_response Returnedvalue;

    va_start( ap, secureConfig );

    Returnedvalue = (t_sec_rom_response)hw_sec_rom_pub_bridge(serviceId, secureConfig, ap);

    va_end( ap );
    return Returnedvalue;
}


/*
 *  Unknown compiler, function only returns an error code
 */
#else
PUBLIC t_sec_rom_response SECURITY_CallSecureService( t_sec_rom_service_id serviceId,
                                                      t_sec_rom_exec_config secureConfig,
                                                      ...  )
{
    return COMPILER_NOT_SUPPORTED;
}

#endif


/****************************************************************************/
/* NAME:        SECURITY_GetPhysicalAddress()                               */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine calls the hw_mmu_physical_address_get function */
/*              from public ROM.                                            */
/*              It reads the public mmu table and returns the physical      */
/*              address corresponding to virtual one given in parameter.    */
/*                                                                          */
/* PARAMETERS:                                                              */
/* IN :         virtualAddress: Virtual public address to be converted      */
/* OUT :        None                                                        */
/*                                                                          */
/* RETURN:                                                                  */
/*              t_uint32: physical public address corresponding to          */
/*                        virtuaAddress                                     */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/
PUBLIC t_physical_address SECURITY_GetPhysicalAddress( t_logical_address logicalAddress )
{
    t_physical_address returnValue = 0;

    returnValue = (t_physical_address)hw_mmu_physical_address_get(logicalAddress);

    return returnValue;
}


/*--------------------------------------------------------------------------*
 *  Private functions                                                       *
 *--------------------------------------------------------------------------*/


// End of security.c file
