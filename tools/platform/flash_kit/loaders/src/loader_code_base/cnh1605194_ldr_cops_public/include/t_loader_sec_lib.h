/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _INCLUSIONGUARD_T_LOADER_SEC_LIB_H
#define _INCLUSIONGUARD_T_LOADER_SEC_LIB_H

/**
 *  @addtogroup ldr_security_library
 *  @{
 */
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_basicdefinitions.h"
#ifndef MACH_TYPE_STN8500
#include "t_copsrom.h"
#include "t_pd_esb.h"
#endif // MACH_TYPE_STN8500

/*******************************************************************************
 * Types, constants
 ******************************************************************************/
#define POINTER_NOT_USED    ((void*)-1)
#define PARAMETER_NOT_USED   1

/** Number of control keys types */
#define LOADER_SEC_LIB_CONTROL_KEY_TYPE         2
/** Number fo control keys */
#define LOADER_SEC_LIB_NO_OF_CONTROL_KEYS       5

/** Length of the each control key */
#define LOADER_SEC_LIB_CONTROL_KEY_LENGTH       16

/** Length of whole control key set*/
#define LOADER_SEC_LIB_CONTROL_KEYS_LENGTH   192

/** Define full permission production mode level */
#define LOADER_SEC_LIB_PRODUCTION_MODE_LEVEL    127

/** Define flash mode permission level */
#define LOADER_SEC_LIB_FLASH_MODE_LEVEL 1

/** Define low permission service mode level */
#define LOADER_SEC_LIB_SERVICE_MODE_LEVEL   0

/** Max number of A1 authentication */
#define MAX_NO_OF_A1_AUTHENTICATION 3

/** Max number of CA authentication */
#define MAX_NO_OF_CA_AUTHENTICATION 3

/** The length of the hash buffer for SHA256*/
#define LOADER_SEC_LIB_SHA256_HASH_LENGTH   32

/** The length of the hash buffer for SHA1*/
#define LOADER_SEC_LIB_SHA1_HASH_LENGTH 20

/** Mask for authentication purpose functionality(Key_Usage in Header)*/
#define MASK_AUTHENTICATION_PURPOSE 0x1000

/**  The control key length. */
#define LOADER_SEC_LIB_CID_IMEI_LENGTH     8

/** Packed IMEI size in bytes */
#define PACKED_IMEI_SIZE  6

/** Unpacked IMEI size in bytes */
#define UNPACKED_IMEI_SIZE  15

/**  Length of the IMEI, in bytes. */
#define LOADER_SEC_LIB_IMEI_LENGTH  14

/** Packed CID size in bytes */
#define PACKED_CID_BIT_SIZE 14

/** Packed CID write mask */
#define OTP_PACKED_CID_MASK                          0x00003FFF

/** Packed CID lock mask */
#define OTP_PACKED_CID_LOCK_MASK                     0x00000001

/** Packed IMEI_CHANGEABLE_FLAG write mask */
#define OTP_PACKED_IMEI_CHANGEABLE_FLAG_MASK         0x00004000

/** Packed IMEI_CHANGEABLE_FLAG lock mask */
#define OTP_PACKED_IMEI_CHANGEABLE_FLAG_LOCK_MASK    0x00000002

/** Packed PACKED_PAF write mask */
#define OTP_PACKED_PAF_MASK                          0x00008000

/** Packed PACKED_PAF lock mask */
#define OTP_PACKED_PAF_LOCK_MASK                     0x00000004

/** Packed PACKED_IMEI_CHANGEABLE lock mask */
#define OTP_PACKED_IMEI_LOCK_MASK                    0x00000008

/** Size of OTP data in words */
#define RAW_OTP_DATA_SIZE 2

/** Size of the domain data. */
#define LOADER_SEC_LIB_DOMAIN_DATA_SIZE 60

/** This is the total size of the static data. */
#define LOADER_SEC_LIB_STATIC_DATA_SIZE 300

/** Offset to operator_name_size field */
#define OPERATOR_NAME_SIZE_OFFSET          14

/** Size of UDB data without size of the operator name. */
#define UDB_SIZE_WITHOUT_OPERATOR_NAME_SIZE_DATA  222

/** Id of domain data object boot record.*/
#define LOADER_SEC_LIB_DOMAIN_DATA_ID    0x0001000E

/** Random data length used in challenge data block. */
#define LOADER_SEC_LIB_RANDOM_DATA_LENGTH 32

/** Additional data length used in challenge data block. */
#define LOADER_SEC_LIB_ADDITIONAL_DATA_LENGTH 16

/** Offset to the Random data in challenge data block. */
#define LOADER_SEC_LIB_RANDOM_DATA_OFFSET 8
/*
 * Platform Property id defines
 */
/** Platform Property id: IMEI property index */
#define PROP_IMEI                               0x01
/** Platform Property id: Notification that simlock is set*/
#define PROP_SIMLOCK_SET                        0x10
/** Platform Property id: Default data.*/
#define DEFAULT_DATA                            0x100
/** Platform Property id: Subdomain.*/
#define PROP_SUBDOMAIN                          0x40
/** Platform Property id: Third Party ID.*/
#define PROP_TPID                               0x41
/**  Length of the Subdomain, in bytes. */
#define PROP_SUBDOMAIN_LENGTH                   1
/**  Length of the Third Party ID, in bytes. */
#define PROP_TPID_LENGTH                        2

/*
 *  Authentication purposes
 */
/** define for all allowed change operation. */
#define ALL_ALLOWED_CHANGE_OPERATION  0x0000001F
/** mask for allowed damains. */
#define ALL_ALLOWED_DOMAINS           0x0000000F
/** mask for reading allowed change operations. */
#define MASK_ALLOWED_OPERATION  0x0000001F

/** Status flag for boot block existing.*/
#define BOOT_BLOCK_EXIST         0x01
/** Status flag for existing domain data record.*/
#define DOMAIN_DATA_RECORD_EXIST 0x02
/** Status flag for domain data record.*/
#define VALID_DOMAIN_DATA_RECORD 0x04
/** Status flag for existing COPS data file.*/
#define COPS_DATA_EXIST          0x08
/** Status flag for existing IMEI data record in COPS file.*/
#define COPS_IMEI_DATA_EXIST     0x10

/** Mask for selecting status of boot block. */
#define BOOT_BLOCK_EXIST_MASK     0x01
/** Mask for selecting status of domain data record. */
#define DOMAIN_DATA_EXIST_MASK    0x02
/** Mask for selecting status of domain data record validity. */
#define VALID_DOMAIN_DATA_MASK    0x04
/** Mask for selecting status of existing COPS data file. */
#define COPS_DATA_EXIST_MASK      0x08
/** Mask for selecting status of exisitng IMEI data record in COPS file. */
#define COPS_IMEI_DATA_EXIST_MASK 0x10
/** Mask for selecting first 3 bits from 32 bits word.*/
#define MASK_FIRST_3_BITS         0x07
/** clear first 3 bits in the 32 bits word.*/
#define CLEAR_FIRST_3_BITS        0xfffffff8

/**
 *  This enum contain the three different colours that a certificate can have.
 */
typedef enum {
    /** Enum member indicating factory domain*/
    LOADER_SEC_LIB_DOMAIN_FACTORY  = 0x00000001UL,
    /** Enum member indicating R&D domain*/
    LOADER_SEC_LIB_DOMAIN_RnD      = 0x00000002UL,
    /** Enum member indicating prouduct domain.*/
    LOADER_SEC_LIB_DOMAIN_PRODUCT  = 0x00000004UL,
    /** Enum member indicating service domain.*/
    LOADER_SEC_LIB_DOMAIN_SERVICE  = 0x00000008UL
} LOADER_SEC_LIB_Domain_t;

/**
 * Identification of Internal data blocks
 */
typedef enum {
    /** Authentication data block ID.*/
    INTERNAL_DATA_BLOCK_AUTHENTICATION = 1,
    /** Domaind data block ID.*/
    INTERNAL_DATA_BLOCK_DOMAINS        = 2,
    /** OTP data block ID.*/
    INTERNAL_DATA_BLOCK_OTP            = 3,
    /** Security data block ID.*/
    INTERNAL_DATA_BLOCK_SECURITY_DATA  = 4,
} Internal_Data_Block_Id_t;

/**
 *  Typedef for authentication states/status
 */
typedef enum {
    /** Enum member indicating that authenthication has not been made.*/
    LOADER_SEC_LIB_LOADER_IS_NOT_AUTHENTICATED      = 0,
    /** Enum member indicating that authenthication has been made.*/
    LOADER_SEC_LIB_LOADER_AUTHENTICATED              = 1,
    /** Authentication is not needed. */
    LOADER_SEC_LIB_LOADER_NOT_NEEDED_AUTHENTICATION  = 2,
} Auth_Status_t;

/**
 *  Typedef for authentication type
 */
typedef enum {
    /** Enum member indicating that authenthication type is with Control keys.*/
    LOADER_SEC_LIB_CONTROL_KEYS_AUTHENTICATION  = 0,
    /** Enum member indicating that authenthication type is with CA certificate.*/
    LOADER_SEC_LIB_CERTIFICATE_AUTHENTICATION   = 1,
    /** R&D authentication. Allowd full access. */
    LOADER_SEC_LIB_R_AND_D_AUTHENTICATION   = 2,
} Authentication_Type_t;

/** Authentication Challenge Data Block. */
typedef struct {
    uint32 CB_Version;    /** Challendge Data Block (CDB) ID */
    uint32 Device_Id;     /** ASIC chip ID */
    /** This is a value based on random data. */
    uint8 Random[LOADER_SEC_LIB_RANDOM_DATA_LENGTH];
    uint32 AddDataLenght; /** States the length of the following additional
                            data field in bytes.*/
    /** Contains additional data if used. This field is mostly a preparation for
        future versions of the Challenge Block. */
    uint8  AddData[LOADER_SEC_LIB_ADDITIONAL_DATA_LENGTH];
} AuthenticationChallengeDataBlock_t;

/** Authentication structure. */
typedef struct {
    /** Authentication counter for A1 authentication type. */
    uint32 A1AuthCounter;
    /** Authentication counter for CA authentication type. */
    uint32 CAAuthCounter;
    /** Current permision Level. */
    uint32 PLevel;
    /** Allowed operation received from CA certificate. */
    uint32 AllowedOperation;
    /** Allowed domains received from CA certificate. */
    uint32 AllowedDomains;
} Authentication_Control_Data_t;

/** Domain data structure. */
typedef struct {
    uint32 EffectiveDomain;     /** Domain received from boot ROM. */
    uint32 WrittenDomain;       /** Domain set by user. */
} DomainsData_t;

// TODO Temp fix
#ifdef MACH_TYPE_STN8500
#define PD_ESB_MAX_MAC_BYTE_SIZE 32
#endif

/** Domain Data for Domain boot record. */
typedef struct {
    /** MAC of the domain data.*/
    uint8          MAC[PD_ESB_MAX_MAC_BYTE_SIZE];
    /** Length of the domain data. */
    uint32         Length;
    /** current domain from flash.*/
    uint32         Domain;
    /** Identification for security config fuses present. */
    uint8          SecurityConfigFusesPresent;
    /** Identification for customer trust anchor present. */
    uint8          CustTrustAnchorPresent;
    /** Security configuration fuses from OTP.*/
#ifdef MACH_TYPE_STN8500
    uint32 SecurityConfigFuses; // TODO TempFix
#else
    COPSROM_OTP_t  SecurityConfigFuses;
#endif
    /** Customer trust anchor data block.*/
    uint8         *CustTrustAnchor;
} Domain_Data_t;

/** Domain boot record structure. */
typedef struct {
    /** Domain data record ID. */
    uint32 DomainID;
    /** Domain data recor size. */
    uint32 DomainSize;
    /** reserved field.*/
    uint32 Reserved;
    /** Domain data structure.*/
    Domain_Data_t DomainData;
} LDR_SEC_LIB_Domain_BootRecord_t;

/** OTP data structure. */
typedef struct {
    /** PAF flag.*/
    boolean  PAF;
    /** Rewritable IMEI flag.*/
    boolean  RewritableIMEI;
    /** CID value.*/
    uint16   CID;
    /** IMEI data.*/
    uint8    IMEI[UNPACKED_IMEI_SIZE];
    /** OTP lock status.*/
    uint32   OTP_Lock_Status;
} LOADER_SEC_LIB_OTP_t;

/* Verification */
// TODO Temp fix
#ifdef MACH_TYPE_STN8500
#define SHA1_HASH_SIZE    20
#define SHA256_HASH_SIZE  32
#endif
typedef uint8 SHA1_Hash_t[SHA1_HASH_SIZE];
typedef uint8 SHA256_Hash_t[SHA256_HASH_SIZE];

/** Payload block info. */
typedef struct {
    /** Size of the payload block. */
    uint32 Size;
    /** Destination address. */
    uint32 DestinationAddress;
} PayloadBlockInfo_t;

/** Hash list info block.*/
typedef struct {
    /** Payload offset. */
    uint64 PayloadOffset;
    /** Payload block info. */
    PayloadBlockInfo_t BlockInfo;
    /** Hash (SHA256) calculated for the specified payload with
     * payload offset.*/
    SHA256_Hash_t Hash;
} HashListEntry_t;

/** Status of the verification.*/
typedef enum {
    /** Initialized for verification of data block.*/
    INITIALIZED = 1,
    /** Header in the received data packet is verified.*/
    HEADER_VERIFIED,
    /** Block mode.*/
    BLOCK_MODE,
    /** Hash mode.*/
    HASH_MODE,
    /** Shut down of verification. */
    SHUTDOWN,
    /** Error in the verification.*/
    ERROR
} VerifyState_t;

/** Context used in the verification of the packet.*/
typedef struct {
    /** Verification state.*/
    VerifyState_t State;
    /** Payload offset.*/
    uint64 PayloadOffset;
    /** Payload size.*/
    uint64 PayloadSize;
    /** Max block size.*/
    uint32 MaxBlockSize;
    /** Number of blocks for verification.*/
    uint32 NumberOfBlocks;
    /** Hash list hash.*/
    SHA256_Hash_t HashListHash;
    /** Payload hash.*/
    SHA256_Hash_t PayloadHash;
    /** pointer to the hash list.*/
    HashListEntry_t *HashList_p;
    /** Pointer to the payload hash. */
    HashListEntry_t *PayloadHash_p;
} VerifyContext_t;

/** Signed header type in the received packet.*/
typedef enum {
    /** Signed software header. */
    LOADER_SEC_LIB_SIGNED_SOFTWARE = 0,
    /** Signed loader header.*/
    LOADER_SEC_LIB_SIGNED_LOADER,
    /** Signed generic header. */
    LOADER_SEC_LIB_SIGNED_GENERIC,
    /** Signed second boot stage header.*/
    LOADER_SEC_LIB_SIGNED_2SBC,
    /** Signed third boot stage header.*/
    LOADER_SEC_LIB_SIGNED_3SBC,
    /** Signed second prologue header.*/
    LOADER_SEC_LIB_SIGNED_PROLOG
} HeaderType_t;

/** Used hash algorithms.*/
typedef enum {
    /** SHA1 hash type of algorithm. */
    SHA_1 = 1,
    /** SHA256 hash type of algorithm. */
    SHA_256  = 2
} HashAlgorithm_t;

/** Header info for header verification.*/
typedef struct {
    /** Signed header type of recived packet.*/
    HeaderType_t HeaderType;
    /** Hash algorithm used.*/
    HashAlgorithm_t HashAlgorithm;
    /** Hash size of the used hash algorithm.*/
    uint32 HashSize;
    /** Max block size.*/
    uint32 MaxBlockSize;
    /** Numner of hashed blocks. */
    uint32 NumberOfBlocks;
    /** Payload offset.*/
    uint32 PayloadOffset;
} HeaderInfo_t;

#include "c_prepack.h"

/** Row OTP data structure. */
typedef PRE_PACKED struct {
    uint8    EMP_RootKeyRevocation     : 3;
    uint8    TrustedRootSource         : 2;
    boolean  IMEI_Changeable           : 1;
    boolean  PAF                       : 1;
    boolean  MC_EnableFlag             : 1;
    uint8    Spare                     : 8;
    uint8    IMEI[PACKED_IMEI_SIZE];

} Raw_OTP_t POST_PACKED;

#include "c_postpack.h"

/** @} */
#endif /*_INCLUSIONGUARD_T_LOADER_SEC_LIB_H*/
