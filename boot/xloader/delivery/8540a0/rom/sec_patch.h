
#ifndef SEC_PATCHES_H
#define SEC_PATCHES_H

#include <stdarg.h>

  /* Constants */

/* Patch ID's. Note that these ID's shall fit into one byte */


/** @{ \internal define for patch IDs */
#define SEC_PATCH_DMA_INIT                        1
#define SEC_PATCH_DMA_PROGRAM_TRANSFER            2
#define SEC_PATCH_DMA_TRANSFER_RUNNING            3
#define SEC_PATCH_DMA_FIQ_HANDLER_USER            4
#define SEC_PATCH_DMA_FIQ_HANDLER                 5
#define SEC_PATCH_DMA_COMPUTE_EVEN_PARAM_REG      6
#define SEC_PATCH_DMA_COMPUTE_ODD_PARAM_REG       7

#define SEC_PATCH_L2CC_CLEAN                      8

#define SEC_PATCH_DMA_SECURIZE_EVENT_LINE         9

#define SEC_PATCH_HW_HASH_DMA_INIT               10
#define SEC_PATCH_HW_HASH_DMA_UPDATE             11
#define SEC_PATCH_HW_HASH_DMA_FINAL              12

#define SEC_PATCH_L2CC_ENABLE                    13
#define SEC_PATCH_L2CC_DISABLE                   14

#define SEC_PATCH_PKA_HW_SET_KEY                 15
#define SEC_PATCH_PKA_HW_COMPUTE                 16

#define SEC_PATCH_RESET_REQUEST                  17

#define SEC_PATCH_L2CC_INVALIDATE                18
#define SEC_PATCH_L2CC_CLEAN_AND_INVALIDATE      19

#define SEC_PATCH_HW_RNG_RANDOM_DATA_GET         20
#define SEC_PATCH_RANDOM_DATA_GET                21
#define SEC_PATCH_RANDOM_GEN_INIT                22

#define SEC_PATCH_SPINTRYLOCK                    23
#define SEC_PATCH_SPINLOCK                       24
#define SEC_PATCH_SPINUNLOCK                     25
#define SEC_PATCH_SPINLOCK_ACTIVE                26

#define SEC_PATCH_SHA512_INIT                    27

#define SEC_PATCH_IS_NON_SEC_ADDRESS             28
#define SEC_PATCH_IS_NON_SEC_AREA                29

#define SEC_PATCH_INITIALIZE_COUNTER             30

#define SEC_PATCH_EXECUTE_RPC                    31

#define SEC_PATCH_PROCESS_TIMER_IT               32

#define SEC_PATCH_DEEP_END_DISPATCH              33
#define SEC_PATCH_DEEP_CHECK_CMD                 34
#define SEC_PATCH_DEEP_DISPATCH                  35
#define SEC_PATCH_DEEP_ENTRY                     36
#define SEC_PATCH_DEEP_PERSONAL_CONTEXT_SAVE     37
#define SEC_PATCH_DEEP_COMMON_CONTEXT_SAVE       38
#define SEC_PATCH_DEEP_COMMON_CONTEXT_AND_MEM_SAVE 39

//unused                                         40

#define SEC_PATCH_DEEP_ADDITIONNAL_MEM_SAVE      41
#define SEC_PATCH_DEEP_ADDITIONNAL_SAVE          42
#define SEC_PATCH_DEEP_ADDITIONNAL_RESTORE       43
#define SEC_PATCH_DEEP_SAVE_MEMORY_PAGE          44
#define SEC_PATCH_DEEP_SAVE_MEMORY               45
#define SEC_PATCH_DEEP_EXIT_SLAVE                46
#define SEC_PATCH_DEEP_EXIT_MASTER               47

#define SEC_PATCH_NON_VALID_ADDRESS              48

#define SEC_PATCH_PA2VA                          49
#define SEC_PATCH_ENABLE_SEC_MODE                50
#define SEC_PATCH_DISABLE_SEC_MODE               51

#define SEC_PATCH_EVENT_WAIT                     52
#define SEC_PATCH_EVENT_SYNCHRONIZE              53

#define SEC_PATCH_DMA_INIT_LLI_AREA              54
#define SEC_PATCH_DMA_ALLOCATE_LLI               55
#define SEC_PATCH_DMA_FILL_LLI                   56

// unused                                        57

#define SEC_PATCH_DMA_FREE_LLI_ENTRY             58

/* Used only in assembly file ! */
#define SEC_PATCH_PROCESS_UNKNOWN_IT             59

#define SEC_PATCH_XP70_WRITE_MAILBOX             60
#define SEC_PATCH_XP70_WAIT_MAILBOX              61
#define SEC_PATCH_XP70_CLEAR_IT                  62

#define SEC_PATCH_L2CC_MAINTENANCE_BY_AREA       63

#define SEC_PATCH_DEEP_REGISTER_STACK_AREA       64

#define SEC_PATCH_PROC_MAINTAIN_CACHE			 65

#define SEC_PATCH_IT_WAIT                        66
#define SEC_PATCH_IT_SYNCHRONIZE                 67
#define SEC_PATCH_SLAVE_CPU_WAKE_UP              68
#define SEC_PATCH_IT_SYNC_EXECUTION              69

#define SEC_PATCH_HW_CRYP_INIT                   70
#define SEC_PATCH_HW_CRYP_UPDATE                 71
#define SEC_PATCH_HW_CRYP_UPDATE_BASIC           72
#define SEC_PATCH_HW_CRYP_UPDATE_XTS             73

#define SEC_PATCH_DEEP_ENCRYPT_PAGE              74

#define SEC_PATCH_VA2PA							 75

#define SEC_PATCH_FIQ_SGI_HANDLE                 76

#define SEC_PATCH_DMA_SET_DMA_REGS               77

#define SEC_PATCH_XP70_WRITE_DMA_HASH_ONGOING    78
#define SEC_PATCH_XP70_WRITE_DMA_CRYP_ONGOING    79
#define SEC_PATCH_XP70_WRITE_SEC_MEM_SAVED       80

#define SEC_PATCH_DMA_RESTORE_CONFIG             81

#define SEC_PATCH_ENABLE_SEC_IP_CLK              82
#define SEC_PATCH_DISABLE_SEC_IP_CLK             83

#define SEC_PATCH_EXP_MOD                        84
#define SEC_PATCH_MULTIPLY                       85
#define SEC_PATCH_SUBSTRACT_MOD                  86

#define SEC_PATCH_ARITHM_ADDSELF                 87
#define SEC_PATCH_ARITHM_GETLENGTH               88
#define SEC_PATCH_ARITHM_ADD                     89
#define SEC_PATCH_ARITHM_INCREMENT               90
#define SEC_PATCH_ARITHM_COMPARE                 91
#define SEC_PATCH_ARITHM_SUBSTRACT               92
#define SEC_PATCH_ARITHM_DECREMENT               93
#define SEC_PATCH_ARITHM_SUBSTRACTSELF           94
#define SEC_PATCH_ARITHM_MULTIPLY_SCALAR         95
#define SEC_PATCH_ARITHM_DIVIDE                  96
#define SEC_PATCH_ARITHM_MODULUS                 97
#define SEC_PATCH_ARITHM_MULTIPLY_MONT           98
#define SEC_PATCH_ARITHM_GETBIT                  99
#define SEC_PATCH_ARITHM_INVMOD                  100
#define SEC_PATCH_ARITHM_GEN_RANDOM              101
#define SEC_PATCH_ARITHM_GEN_RANDOM_LESS         102
#define SEC_PATCH_ARITHM_IS_EQUAL                103
#define SEC_PATCH_ARITHM_INVERSE_MODULO          104
#define SEC_PATCH_ARITHM_GCD                     105
#define SEC_PATCH_ARITHM_IS_PRIME                106
#define SEC_PATCH_HW_CRYP_FINALIZE               107

#define SEC_PATCH_HASH_INIT                      109
#define SEC_PATCH_HASH_UPDATE                    110
#define SEC_PATCH_HASH_FINAL                     111

#define SEC_PATCH_ECC_DOUBLE                     112
#define SEC_PATCH_ECC_SCAL_MULT                  113
#define SEC_PATCH_ECC_ADD                        114

//unused                                         115

#define SEC_PATCH_XP70_DACR                      116

/** @} */


/* Max patch ID must be less or equal to 255 */

#endif /* End of sec_patch.h */
