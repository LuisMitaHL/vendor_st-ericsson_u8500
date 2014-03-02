/*******************************************************************************
* $Copyright Ericsson AB 2009 $
********************************************************************************
*
* DESCRIPTION:
* NAND device configuration file.
*
* To support a new NAND device add the manufacturer to PD_NAND_CONF_ManufacturerId_t and
* the device id to the manufacturers device id list. Then add the device to
* PD_NAND_CONF_DeviceConfigurationTable.
*
********************************************************************************
*
* REVISION HISTORY
*
* $Log: \LD_PlatformSec_Pub\cnh1605207_ldr_emulator\modules\cnh1601476_nand_pd\public\c_pd_nand.h $
 *
 *    Version: ...\cnh1605207\cnh1605207_r1\4 12 Feb 2009 15:13 (GMT) XVSVLPI
 * lint warnings fixed
 *
 *    Version: ...\cnh1605207\cnh1605207_r1\3 11 Nov 2008 16:44 (GMT) XVSVLPI
 * update merge
 *
 *    Version: ...\cnh1601476\cnh1601476_r5\1 22 Apr 2008 07:57 (GMT) qmarlun
 * Intermediat merge from iso branch.
 *
 *    Version: ...\cnh1601476\1 07 Feb 2008 07:07 (GMT) qmarlun
 * Prepared for new release branch.
 *
 *    Version: ...\cnh1601476\cnh1601476_r4\3 20 Sep 2007 11:37 (GMT) edalann
 * Added support for Spansion ORNAND MirrorBit NAND flash
 *
 *    Version: ...\cnh1601476\cnh1601476_r4\2 18 Sep 2007 05:22 (GMT) qmarlun
 * Added 2G Samsung NAND.
 *
 *    Version: ...\cnh1601476\cnh1601476_r4\1 07 Mar 2007 07:18 (GMT) qmarlun
 * NAND device configuration file.
*
*******************************************************************************/

// =============================================================================
#ifndef INCLUSION_GUARD_NAND_DEV_CFG_H
#define INCLUSION_GUARD_NAND_DEV_CFG_H
// =============================================================================

// =============================================================================
//  Include Header Files
// =============================================================================

#include "c_system_v2.h"
#include "t_basicdefinitions.h"

// =============================================================================
//  Defines
// =============================================================================

#define D_PD_NAND_CONF_PAGE_SIZE_SMALL        512
#define D_PD_NAND_CONF_PAGE_SIZE_LARGE        2048

#define D_PD_NAND_CONF_REDUNDANT_SIZE_SMALL   16
#define D_PD_NAND_CONF_REDUNDANT_SIZE_LARGE   64

// =============================================================================
//  Types
// =============================================================================

/**
 * PD_NAND_CONF_ManufacturerId_t represents different NAND manufacturer ID's.
 *
 * @param T_PD_NAND_CONF_MAN_ID_UNDEF     Manufacturer ID undefined.
 * @param T_PD_NAND_CONF_MAN_ID_SAMSUNG   Samsung.
 * @param T_PD_NAND_CONF_MAN_ID_ST        ST.
 * @param T_PD_NAND_CONF_MAN_ID_TOSHIBA   Toshiba.
 * @param T_PD_NAND_CONF_MAN_ID_AMD       AMD.
 */
typedef enum {
    T_PD_NAND_CONF_MAN_ID_UNDEF             = 0x00,
    // - - -
    T_PD_NAND_CONF_MAN_ID_SAMSUNG           = 0xEC,
    T_PD_NAND_CONF_MAN_ID_ST                = 0x20,
    T_PD_NAND_CONF_MAN_ID_TOSHIBA           = 0x98,
    T_PD_NAND_CONF_MAN_ID_AMD               = 0x01,
    T_PD_NAND_CONF_MAN_ID_MICRON            = 0x2C,
} PD_NAND_CONF_ManufacturerId_t;

// -----------------------------------------------------------------------------

/**
 * PD_NAND_CONF_DeviceId_t represents different NAND device ID's.
 *
 * @param T_NAND_DEV_ID_128       {?;        128MBit; 1.8V; 512B Page}.
 */
typedef enum {
    T_PD_NAND_CONF_DEV_ID_UNDEF             = 0x00,
    // - - -
    T_PD_NAND_CONF_DEV_ID_128               = 0x73,
} PD_NAND_CONF_DeviceId_t;

// -----------------------------------------------------------------------------

/**
 * PD_NAND_CONF_DeviceId_ST_t represents different Samsung NAND device ID's.
 *
 * @param T_PD_NAND_CONF_DEV_ID_SAMSUNG_K9F5608D0D       { 256MBit; 1.8V; 512B Page}.
 * @param T_PD_NAND_CONF_DEV_ID_SAMSUNG_K9F5608R0D       { 256MBit; 1.8V; 512B Page}.
 * @param T_PD_NAND_CONF_DEV_ID_SAMSUNG_K9F1208R0B       { 512MBit; 1.8V; 512B Page}.
 * @param T_PD_NAND_CONF_DEV_ID_SAMSUNG_K9F1208B0B       { 512MBit; 1.8V; 512B Page}.
 * @param T_PD_NAND_CONF_DEV_ID_SAMSUNG_K9K1G08R0B       {1024MBit; 1.8V; 512B Page}.
 * @param T_PD_NAND_CONF_DEV_ID_SAMSUNG_K9K1G08B0B       {1024MBit; 1.8V; 512B Page}.
 * @param T_PD_NAND_CONF_DEV_ID_SAMSUNG_K5D1G12ACA       {   1GBit; 1.8V;  2kB Page}.
 */
typedef enum {
    T_PD_NAND_CONF_DEV_ID_SAMSUNG_K9F5608D0D           = 0x75,
    T_PD_NAND_CONF_DEV_ID_SAMSUNG_K9F5608R0D           = 0x35,
    T_PD_NAND_CONF_DEV_ID_SAMSUNG_K9F1208R0B           = 0x36,
    T_PD_NAND_CONF_DEV_ID_SAMSUNG_K9F1208B0B           = 0x76,
    T_PD_NAND_CONF_DEV_ID_SAMSUNG_K9K1G08R0B           = 0x78,
    T_PD_NAND_CONF_DEV_ID_SAMSUNG_K9K1G08B0B           = 0x79,
    T_PD_NAND_CONF_DEV_ID_SAMSUNG_K5D1G12ACA           = 0xA1,
    T_PD_NAND_CONF_DEV_ID_SAMSUNG_K9F2G08R0A           = 0xAA,
} PD_NAND_CONF_DeviceId_SAMSUNG_t;

// -----------------------------------------------------------------------------

/**
 * PD_NAND_CONF_DeviceId_ST_t represents different ST NAND device ID's.
 *
 * @param T_PD_NAND_CONF_DEV_ID_ST_NAND128W3A       { 128MBit; 1.8V; 512B Page}
 * @param T_PD_NAND_CONF_DEV_ID_ST_NAND256R3A       { 256MBit; 1.8V; 512B Page}
 * @param T_PD_NAND_CONF_DEV_ID_ST_NAND256W3A       { 256MBit; 1.8V; 512B Page}
 * @param T_PD_NAND_CONF_DEV_ID_ST_NAND512R3A       { 512MBit; 1.8V; 512B Page}
 * @param T_PD_NAND_CONF_DEV_ID_ST_NAND512W3A       { 512MBit; 1.8V; 512B Page}
 * @param T_PD_NAND_CONF_DEV_ID_ST_NAND01GR3A       {1024MBit; 1.8V; 512B Page}
 * @param T_PD_NAND_CONF_DEV_ID_ST_NAND01GW3A       {1024MBit; 1.8V; 512B Page}
 * @param T_PD_NAND_CONF_DEV_ID_ST_NAND01GR3B       {   1GBit; 1.8V;  2kB Page}.
 * @param T_PD_NAND_CONF_DEV_ID_ST_NAND02GR3B       {   2GBit; 1.8V;  2kB Page}.
 */
typedef enum {
    T_PD_NAND_CONF_DEV_ID_ST_NAND128W3A               = 0x73,
    T_PD_NAND_CONF_DEV_ID_ST_NAND256R3A               = 0x35,
    T_PD_NAND_CONF_DEV_ID_ST_NAND256W3A               = 0x75,
    T_PD_NAND_CONF_DEV_ID_ST_NAND512R3A               = 0x36,
    T_PD_NAND_CONF_DEV_ID_ST_NAND512W3A               = 0x76,
    T_PD_NAND_CONF_DEV_ID_ST_NAND01GR3A               = 0x39,
    T_PD_NAND_CONF_DEV_ID_ST_NAND01GW3A               = 0x79,
    T_PD_NAND_CONF_DEV_ID_ST_NAND01GR3B2B             = 0xA1,
    T_PD_NAND_CONF_DEV_ID_ST_NAND01GW3B2B             = 0xF1,
    T_PD_NAND_CONF_DEV_ID_ST_NAND01GR4B2B             = 0xB1,
    T_PD_NAND_CONF_DEV_ID_ST_NAND01GW4B2B             = 0xC1,
    T_PD_NAND_CONF_DEV_ID_ST_NAND02GR3B2B             = 0xAA,
    T_PD_NAND_CONF_DEV_ID_ST_NAND02GW3B2C             = 0xDA,
    T_PD_NAND_CONF_DEV_ID_ST_NAND02GR4B2C             = 0xBA,
    T_PD_NAND_CONF_DEV_ID_ST_NAND02GW4B2C             = 0xCA,
} PD_NAND_CONF_DeviceId_ST_t;

// -----------------------------------------------------------------------------

/**
 * NAND_DeviceId_AMD_t represents different AMD (Spansion) NAND device ID's.
 *
 * @param T_PD_NAND_CONF_DEV_ID_AMD_NAND512MS        { 512MBit; 1.8V;  8Bit; 2kB Page}.
 * @param T_PD_NAND_CONF_DEV_ID_AMD_NAND512MW        { 512MBit; 1.8V; 16Bit; 2kB Page}.
 * @param T_PD_NAND_CONF_DEV_ID_AMD_NAND001GS        {   1GBit; 1.8V;  8Bit; 2kB Page}.
 * @param T_PD_NAND_CONF_DEV_ID_AMD_NAND001GW        {   1GBit; 1.8V; 16Bit; 2kB Page}.
 */
typedef enum {
    T_PD_NAND_CONF_DEV_ID_AMD_NAND512MS                = 0x81,
    T_PD_NAND_CONF_DEV_ID_AMD_NAND512MW                = 0x91,
    T_PD_NAND_CONF_DEV_ID_AMD_NAND001GS                = 0xA1,
    T_PD_NAND_CONF_DEV_ID_AMD_NAND001GW                = 0xB1,
} NAND_DeviceId_AMD_t;
typedef enum {
    T_PD_NAND_CONF_DEV_ID_MICRON_MT29F1G08ABB          = 0xA1,
    T_PD_NAND_CONF_DEV_ID_MICRON_MT29F1G16ABB          = 0xB1,
    T_PD_NAND_CONF_DEV_ID_MICRON_MT29F2G08AAC          = 0xDA,
    T_PD_NAND_CONF_DEV_ID_MICRON_MT29F2G08ABC          = 0xAA,
    T_PD_NAND_CONF_DEV_ID_MICRON_MT29F2G16AAC          = 0xCA,
    T_PD_NAND_CONF_DEV_ID_MICRON_MT29F2G16ABC          = 0xBA,
    T_PD_NAND_CONF_DEV_ID_MICRON_MT29F4G08BAC          = 0xDC,
} PD_NAND_CONF_DeviceId_MICRON_t;

// -----------------------------------------------------------------------------

/**
 * PD_NAND_CONF_DeviceInfo_t represents common NAND device type settings.
 *
 * @param DeviceSize            NAND device size in [MB] (!).
 * @param EraseBlockSize        NAND erase block size in [B].
 * @param PageSize              NAND page size in [B].
 * @param PageExtraSize         NAND page redundant area size in [B].
 */
typedef struct {
    uint32 DeviceSize;
    uint32 EraseBlockSize;
    uint32 PageSize;
    uint32 PageExtraSize;
    uint32 BusWidth;
} PD_NAND_CONF_DeviceInfo_t;

/**
 * PD_NAND_CONF_DeviceType_t represents common NAND device type settings.
 *
 * @param T_NAND_DEVICE_TYPE_SMALL_PAGE     Represents common settings for small page NAND's.
 * @param T_NAND_DEVICE_TYPE_LARGE_PAGE     Represents common settings for large page NAND's.
 * @param T_NAND_DEVICE_TYPE_128_MBIT       Represents common settings for 128MBit 512B page NAND's.
 * @param T_NAND_DEVICE_TYPE_256_MBIT       Represents common settings for 256MBit 512B page NAND's.
 * @param T_NAND_DEVICE_TYPE_512_MBIT       Represents common settings for 512MBit 512B page NAND's.
 * @param T_NAND_DEVICE_TYPE_1024_MBIT      Represents common settings for 1024MBit 512B page NAND's.
 * @param T_NAND_DEVICE_TYPE_1_GBIT         Represents common settings for 1GBit 2kB page NAND's.
 * @param T_NAND_DEVICE_TYPE_2_GBIT         Represents common settings for 2GBit 2kB page NAND's.
 * @param T_NAND_DEVICE_TYPE_4_BBIT         Represents common settings for 4GBit 2kB page NAND's.
 * @param T_NAND_DEVICE_TYPE_8_GBIT         Represents common settings for 8GBit 2kB page NAND's.
 */
typedef enum {
    T_PD_NAND_CONF_DEVICE_TYPE_SMALL_PAGE = 0,
    T_PD_NAND_CONF_DEVICE_TYPE_128_MBIT,
    T_PD_NAND_CONF_DEVICE_TYPE_256_MBIT,
    T_PD_NAND_CONF_DEVICE_TYPE_512_MBIT,
    T_PD_NAND_CONF_DEVICE_TYPE_1024_MBIT,
    T_PD_NAND_CONF_DEVICE_TYPE_1_GBIT_BW8,
    T_PD_NAND_CONF_DEVICE_TYPE_1_GBIT_BW16,
    T_PD_NAND_CONF_DEVICE_TYPE_2_GBIT_BW8,
    T_PD_NAND_CONF_DEVICE_TYPE_2_GBIT_BW16,
    T_PD_NAND_CONF_DEVICE_TYPE_4_GBIT,
    T_PD_NAND_CONF_DEVICE_TYPE_8_GBIT,
    // - - -
    T_PD_NAND_CONF_DEVICE_TYPE_NBR_OF_ITEMS,
    T_PD_NAND_CONF_DEVICE_TYPE_UNDEFINED,
} PD_NAND_CONF_DeviceType_t;

// -----------------------------------------------------------------------------

/**
 * Represents a row in a NAND device configuration table.
 *
 * @param DeviceId        NAND device ID.
 * @param DeviceType      NAND device type.
 *
 * @see PD_NAND_CONF_DeviceType_t
 */
typedef struct {
    uint8 ManufacturerId;
    uint8 DeviceId;
    PD_NAND_CONF_DeviceType_t DeviceType;
} PD_NAND_CONF_DeviceConfiguration_t;

// =============================================================================
//  NAND Device Configuration Table
// =============================================================================

#ifdef CFG_CNH1601476_INSTANTIATE_DEVICE_CONFIGURATION_TABLE

const PD_NAND_CONF_DeviceInfo_t PD_NAND_CONF_DeviceInfoTable[T_PD_NAND_CONF_DEVICE_TYPE_NBR_OF_ITEMS] = {
    //   Device Size [MB], Erase Block Size [B], Page Size [B], Redundant Size [B] }
    {         0,            16 * 1024,            D_PD_NAND_CONF_PAGE_SIZE_SMALL, D_PD_NAND_CONF_REDUNDANT_SIZE_SMALL,      8     }, // - T_PD_NAND_CONF_DEVICE_TYPE_SMALL_PAGE
    {         16,           16 * 1024,            D_PD_NAND_CONF_PAGE_SIZE_SMALL, D_PD_NAND_CONF_REDUNDANT_SIZE_SMALL,      8     }, // - T_PD_NAND_CONF_DEVICE_TYPE_128_MBIT
    {         32,           16 * 1024,            D_PD_NAND_CONF_PAGE_SIZE_SMALL, D_PD_NAND_CONF_REDUNDANT_SIZE_SMALL,      8     }, // - T_PD_NAND_CONF_DEVICE_TYPE_256_MBIT
    {         64,           16 * 1024,            D_PD_NAND_CONF_PAGE_SIZE_SMALL, D_PD_NAND_CONF_REDUNDANT_SIZE_SMALL,      8     }, // - T_PD_NAND_CONF_DEVICE_TYPE_512_MBIT
    {        128,           16 * 1024,            D_PD_NAND_CONF_PAGE_SIZE_SMALL, D_PD_NAND_CONF_REDUNDANT_SIZE_SMALL,      8     }, // - T_PD_NAND_CONF_DEVICE_TYPE_1024_MBIT
    {        128,          128 * 1024,            D_PD_NAND_CONF_PAGE_SIZE_LARGE, D_PD_NAND_CONF_REDUNDANT_SIZE_LARGE,      8     }, // - T_PD_NAND_CONF_DEVICE_TYPE_1_GBIT_BW8
    {        128,          128 * 1024,            D_PD_NAND_CONF_PAGE_SIZE_LARGE, D_PD_NAND_CONF_REDUNDANT_SIZE_LARGE,     16     }, // - T_PD_NAND_CONF_DEVICE_TYPE_1_GBIT_BW16
    {        256,          128 * 1024,            D_PD_NAND_CONF_PAGE_SIZE_LARGE, D_PD_NAND_CONF_REDUNDANT_SIZE_LARGE,      8     }, // - T_PD_NAND_CONF_DEVICE_TYPE_2_GBIT_BW8
    {        256,          128 * 1024,            D_PD_NAND_CONF_PAGE_SIZE_LARGE, D_PD_NAND_CONF_REDUNDANT_SIZE_LARGE,     16     }, // - T_PD_NAND_CONF_DEVICE_TYPE_2_GBIT_BW16
    {        512,          128 * 1024,            D_PD_NAND_CONF_PAGE_SIZE_LARGE, D_PD_NAND_CONF_REDUNDANT_SIZE_LARGE,      8     }, // - T_PD_NAND_CONF_DEVICE_TYPE_4_GBIT
    {       1024,          128 * 1024,            D_PD_NAND_CONF_PAGE_SIZE_LARGE, D_PD_NAND_CONF_REDUNDANT_SIZE_LARGE,      8     }, // - T_PD_NAND_CONF_DEVICE_TYPE_8_GBIT
};

const PD_NAND_CONF_DeviceConfiguration_t PD_NAND_CONF_DeviceConfigurationTable[] = {
    // Small-Page-NAND (512byte) devices are listed here
    {T_PD_NAND_CONF_MAN_ID_SAMSUNG, T_PD_NAND_CONF_DEV_ID_128,                T_PD_NAND_CONF_DEVICE_TYPE_128_MBIT  },
    {T_PD_NAND_CONF_MAN_ID_SAMSUNG, T_PD_NAND_CONF_DEV_ID_SAMSUNG_K9F5608D0D, T_PD_NAND_CONF_DEVICE_TYPE_256_MBIT  },
    {T_PD_NAND_CONF_MAN_ID_SAMSUNG, T_PD_NAND_CONF_DEV_ID_SAMSUNG_K9F5608R0D, T_PD_NAND_CONF_DEVICE_TYPE_256_MBIT  },
    {T_PD_NAND_CONF_MAN_ID_SAMSUNG, T_PD_NAND_CONF_DEV_ID_SAMSUNG_K9F1208B0B, T_PD_NAND_CONF_DEVICE_TYPE_512_MBIT  },
    {T_PD_NAND_CONF_MAN_ID_SAMSUNG, T_PD_NAND_CONF_DEV_ID_SAMSUNG_K9F1208R0B, T_PD_NAND_CONF_DEVICE_TYPE_512_MBIT  },
    {T_PD_NAND_CONF_MAN_ID_SAMSUNG, T_PD_NAND_CONF_DEV_ID_SAMSUNG_K9K1G08R0B, T_PD_NAND_CONF_DEVICE_TYPE_1024_MBIT },
    {T_PD_NAND_CONF_MAN_ID_SAMSUNG, T_PD_NAND_CONF_DEV_ID_SAMSUNG_K9K1G08B0B, T_PD_NAND_CONF_DEVICE_TYPE_1024_MBIT },
    {T_PD_NAND_CONF_MAN_ID_ST,      T_PD_NAND_CONF_DEV_ID_ST_NAND128W3A,      T_PD_NAND_CONF_DEVICE_TYPE_128_MBIT  },
    {T_PD_NAND_CONF_MAN_ID_ST,      T_PD_NAND_CONF_DEV_ID_ST_NAND256R3A,      T_PD_NAND_CONF_DEVICE_TYPE_256_MBIT  },
    {T_PD_NAND_CONF_MAN_ID_ST,      T_PD_NAND_CONF_DEV_ID_ST_NAND256W3A,      T_PD_NAND_CONF_DEVICE_TYPE_256_MBIT  },
    {T_PD_NAND_CONF_MAN_ID_ST,      T_PD_NAND_CONF_DEV_ID_ST_NAND512R3A,      T_PD_NAND_CONF_DEVICE_TYPE_512_MBIT  },
    {T_PD_NAND_CONF_MAN_ID_ST,      T_PD_NAND_CONF_DEV_ID_ST_NAND512W3A,      T_PD_NAND_CONF_DEVICE_TYPE_512_MBIT  },
    {T_PD_NAND_CONF_MAN_ID_ST,      T_PD_NAND_CONF_DEV_ID_ST_NAND01GR3A,      T_PD_NAND_CONF_DEVICE_TYPE_1024_MBIT },
    {T_PD_NAND_CONF_MAN_ID_ST,      T_PD_NAND_CONF_DEV_ID_ST_NAND01GW3A,      T_PD_NAND_CONF_DEVICE_TYPE_1024_MBIT },

    // Large-Page-NAND (2048byte) devices are listed here
    {T_PD_NAND_CONF_MAN_ID_SAMSUNG, T_PD_NAND_CONF_DEV_ID_SAMSUNG_K5D1G12ACA,   T_PD_NAND_CONF_DEVICE_TYPE_1_GBIT_BW8   },
    {T_PD_NAND_CONF_MAN_ID_SAMSUNG, T_PD_NAND_CONF_DEV_ID_SAMSUNG_K9F2G08R0A,   T_PD_NAND_CONF_DEVICE_TYPE_2_GBIT_BW8   },
    {T_PD_NAND_CONF_MAN_ID_ST,      T_PD_NAND_CONF_DEV_ID_ST_NAND01GR3B2B,      T_PD_NAND_CONF_DEVICE_TYPE_1_GBIT_BW8   },
    {T_PD_NAND_CONF_MAN_ID_ST,      T_PD_NAND_CONF_DEV_ID_ST_NAND02GR3B2B,      T_PD_NAND_CONF_DEVICE_TYPE_2_GBIT_BW8   },
    {T_PD_NAND_CONF_MAN_ID_AMD,     T_PD_NAND_CONF_DEV_ID_AMD_NAND512MS,        T_PD_NAND_CONF_DEVICE_TYPE_512_MBIT     },
    {T_PD_NAND_CONF_MAN_ID_AMD,     T_PD_NAND_CONF_DEV_ID_AMD_NAND512MW,        T_PD_NAND_CONF_DEVICE_TYPE_512_MBIT     },
    {T_PD_NAND_CONF_MAN_ID_AMD,     T_PD_NAND_CONF_DEV_ID_AMD_NAND001GS,        T_PD_NAND_CONF_DEVICE_TYPE_1_GBIT_BW8   },
    {T_PD_NAND_CONF_MAN_ID_AMD,     T_PD_NAND_CONF_DEV_ID_AMD_NAND001GW,        T_PD_NAND_CONF_DEVICE_TYPE_1_GBIT_BW8   },
    {T_PD_NAND_CONF_MAN_ID_MICRON,  T_PD_NAND_CONF_DEV_ID_MICRON_MT29F1G08ABB,  T_PD_NAND_CONF_DEVICE_TYPE_1_GBIT_BW8   },
    {T_PD_NAND_CONF_MAN_ID_MICRON,  T_PD_NAND_CONF_DEV_ID_MICRON_MT29F1G16ABB,  T_PD_NAND_CONF_DEVICE_TYPE_1_GBIT_BW16  },
    {T_PD_NAND_CONF_MAN_ID_MICRON,  T_PD_NAND_CONF_DEV_ID_MICRON_MT29F2G08AAC,  T_PD_NAND_CONF_DEVICE_TYPE_2_GBIT_BW8   },
    {T_PD_NAND_CONF_MAN_ID_MICRON,  T_PD_NAND_CONF_DEV_ID_MICRON_MT29F2G08ABC,  T_PD_NAND_CONF_DEVICE_TYPE_2_GBIT_BW8   },
    {T_PD_NAND_CONF_MAN_ID_MICRON,  T_PD_NAND_CONF_DEV_ID_MICRON_MT29F2G16AAC,  T_PD_NAND_CONF_DEVICE_TYPE_2_GBIT_BW16  },
    {T_PD_NAND_CONF_MAN_ID_MICRON,  T_PD_NAND_CONF_DEV_ID_MICRON_MT29F2G16ABC,  T_PD_NAND_CONF_DEVICE_TYPE_2_GBIT_BW16  },
    {T_PD_NAND_CONF_MAN_ID_MICRON,  T_PD_NAND_CONF_DEV_ID_MICRON_MT29F4G08BAC,  T_PD_NAND_CONF_DEVICE_TYPE_4_GBIT       },
};

#else

extern const PD_NAND_CONF_DeviceInfo_t PD_NAND_CONF_DeviceInfoTable[];
extern const PD_NAND_CONF_DeviceConfiguration_t PD_NAND_CONF_DeviceConfigurationTable[];

#endif // CFG_CNH1601476_INSTANTIATE_DEVICE_CONFIGURATION_TABLE

// =============================================================================
//  Global Function Declarations
// =============================================================================

#define T_PD_NAND_CONF_CYCLES_T_RR  6     // t_rr,  busy to re_n (min 0)
#define T_PD_NAND_CONF_CYCLES_T_AR  2     // t_ar,  ID read time (min 0)
#define T_PD_NAND_CONF_CYCLES_T_CLR 2     // t_clr, status read time (min 0)
#define T_PD_NAND_CONF_CYCLES_T_WP  5     // t_wp,  we_n de-assertion delay (min 1)
#define T_PD_NAND_CONF_CYCLES_T_REA 3     // t_rea, re_n assertion (min 1)
#define T_PD_NAND_CONF_CYCLES_T_WC  12    // t_wc,  write cycle time (min 2)
#define T_PD_NAND_CONF_CYCLES_T_RC  12    // t_rc,  read cycle time (min 2)
// =============================================================================
#endif // INCLUSION_GUARD_NAND_DEV_CFG_H
// =============================================================================


