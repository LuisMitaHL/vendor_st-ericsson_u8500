/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdarg.h>
#include <teec_hcl.h>

#define U8500_BOOT_ROM_BASE 0x90000000
#define BOOT_CONVERSION_FUNC (U8500_BOOT_ROM_BASE + 0x18588)
#define BOOT_BRIDGE_FUNC (U8500_BOOT_ROM_BASE + 0x18300)

PRIVATE const t_bass_app_sec_rom_cut_desc NomadikCutDescTab[] = {
    /* Checksum addr, Id value, Conversion func, Bridge func, Cut name */
    { 0x9001DBF4, 0x008500C0, 0x90017588, 0x90017300, 0x8500C0 },
    { 0x9001DBF4, 0x008520B2, 0x90017588, 0x90017300, 0x8520B2 },
    { 0x9001DBF4, 0x008500B2, 0x90017588, 0x90017300, 0x8500B2 },
    { 0x9001DBF4, 0x008500B1, 0x90017588, 0x90017300, 0x8500B1 },
    { 0x9001DBF4, 0x008500B0, 0x90017588, 0x90017300, 0x8500B0 },
    { 0x9001FFF4, 0x00850001, 0x90018588, 0x90018300, 0x850001 },
    { 0x9001FFF4, 0x008500A0, 0x90018588, 0x90018300, 0x8500A0 },
    { 0x9001FFF4, 0x008500A1, 0x90018588, 0x90018300, 0x8500A1 }
};

typedef t_uint32(*t_boot_rom_bridge_func)(t_bass_app_sec_rom_service_id ,
                                          t_uint32, va_list);

PRIVATE t_boot_rom_bridge_func hw_sec_rom_pub_bridge;

PRIVATE t_uint8 TEEC_Initialized;

typedef t_uint32(*t_boot_rom_phys_addr_func)(t_uint32);
PRIVATE t_boot_rom_phys_addr_func hw_mmu_physical_address_get;

PUBLIC void TEEC_Init(void)
{
    t_uint8 CutNb = 0; /* Used to the NomadikCutDescTab array. */

    /*
     * Reads checksum information contained in the ROM code and compare to known
     * values for each existing code.
     */
    while ((CutNb < sizeof(NomadikCutDescTab) /
            sizeof(t_bass_app_sec_rom_cut_desc)) &&
            (NomadikCutDescTab[CutNb].ChecksumValue !=
             *(t_uint32 *)(NomadikCutDescTab[CutNb].ChecksumAddr -
                           BOOT_PHYSICAL_BASE_ADDR + ROM_BASE_ADDR))) {
        CutNb++;
    }

    /*
     * Assignes addresses of ROM-located functions required by the HCL if the
     * ROM code version has been recognized.
     */
    if (CutNb < sizeof(NomadikCutDescTab) /
        sizeof(t_bass_app_sec_rom_cut_desc)) {
        hw_sec_rom_pub_bridge =
            (t_boot_rom_bridge_func)(NomadikCutDescTab[CutNb].BridgeFunc -
                                     BOOT_PHYSICAL_BASE_ADDR + ROM_BASE_ADDR);
        hw_mmu_physical_address_get =
            (t_boot_rom_phys_addr_func)(NomadikCutDescTab[CutNb].ConversionFunc -
                                    BOOT_PHYSICAL_BASE_ADDR + ROM_BASE_ADDR);
    }

    TEEC_Initialized = 1;
}

PUBLIC t_bass_app_sec_rom_response SECURITY_CallSecureService(
                                    t_bass_app_sec_rom_service_id serviceId,
                                    t_bass_app_sec_rom_exec_config secureConfig,
                                    ...)
{
    va_list ap;
    t_bass_app_sec_rom_response Returnedvalue;

    va_start(ap, secureConfig);

    if (!TEEC_Initialized) {
        TEEC_Init();
    }

    Returnedvalue =
        (t_bass_app_sec_rom_response)hw_sec_rom_pub_bridge(serviceId,
                                                           secureConfig, ap);

    va_end(ap);
    return Returnedvalue;
}

PUBLIC t_bass_app_physical_address SECURITY_GetPhysicalAddress(
                                   t_bass_app_logical_address logicalAddress)
{
    t_bass_app_physical_address returnValue = 0;

    if (!TEEC_Initialized) {
        TEEC_Init();
    }

    returnValue =
        (t_bass_app_physical_address)hw_mmu_physical_address_get(
                                                             logicalAddress);
    return returnValue;
}
