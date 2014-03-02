/*******************************************************************************
 * $Copyright Ericsson AB 2009 $
 ******************************************************************************/
#ifndef T_ADBG_H_
#define T_ADBG_H_

/**
 * @addtogroup ADbg
 * @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_basicdefinitions.h"
#include "error_codes.h"

/*******************************************************************************
 * Constants
 ******************************************************************************/
/** Module, case, interface group and interface function maximum name length in
    char. */
#define ADBG_STRING_LENGTH_MAX (32)
/** Maximum number of cases in one module. */
#define ADBG_NUMBER_OF_CASES_MAX (256)
/** Maximum number of modules. */
#define ADBG_NUMBER_OF_MODULES_MAX (24)
/** Maximum number of interface groups in one module. */
#define ADBG_NUMBER_OF_INT_GROUPS_MAX (24)
/** Maximum number of interface functions in one interface group. */
#define ADBG_NUMBER_OF_INT_FUNCTIONS_MAX (24)
/** Average number of parameters for cases and internal functions. */
#define ADBG_AVERAGE_NUMBER_OF_PARAMETERS (15)

/** Test case result is OK. */
#define ASSERT_OK     (0)
/** Test case result is not OK. */
#define ASSERT_FAILED (1)

/** For types where the number of bytes is 8. */
#define DOUBLEWORD  8
/** For types where the number of bytes is 4. */
#define WORD        4
/** For types where the number of bytes is 2. */
#define SHORT       2
/** For types where the number of bytes is 1. */
#define BYTE        1
/** Pointer to void */
#define VOIDPTR       0xF0
/** Pointer to char */
#define CHARPTR       0xFF
/** Pointer to uint8 */
#define BYTEPTR       0xF1
/** Pointer to short */
#define SHORTPTR      0xF2
/** Pointer to WORD */
#define WORDPTR       0xF4
/** Pointer to double */
#define DOUBLEWORDPTR 0xF8

/*******************************************************************************
 * Types, constants
 ******************************************************************************/

/** Information for recovery condition. */
typedef struct {
    boolean SetPrecondition; /**< Is TRUE when precondition is already set. */
    uint8  *Backup_p;        /**< Pointer to value of condition before setting. */
} ADbg_Recovery_t;

/** Structure used to prepare general response. */
typedef struct {
    uint32   Size;        /**< Size of general response payload. */
    uint8   *Data_p;      /**< Pointer to the next free byte in payload buffer. */
    uint8   *StartData_p;     /**< Pointer to general response payload. */
    ErrorCode_e   ErrorValue; /**< General response error value. */
} ADbg_Result_t;

/** Information about received command. */
typedef struct {
    uint8   Command;        /**< Command number. */
    uint32   ModuleId;       /**< Module Id. */
    uint32   CaseId;         /**< Case Id. */
    uint32   IntGroupId;     /**< Interface group Id. */
    uint32   IntFunctionId;  /**< Interface function Id. */
    uint32   RecoveryFlag;   /**< If TRUE then condition should be saved. */
    uint32  Size;           /**< Size of buffer for parameters. */
    uint8  *Data_p;         /**< Pointer to buffer for parameters. */
} ADbg_Command_t;

/*******************************************************************************
 * UNIT TEST TESTCASE
 ******************************************************************************/

/**
* @param Label A null terminated label.
*/
/**< Pointer to function that creates structure. */
typedef void*(*ADbg_Create_t)(void);
/**< Pointer to function that destroys structure. */
typedef void (*ADbg_Destroy_t)(void *ADbg_p);
/**< Pointer to test case function. */
typedef void (*ADbg_CaseRun_t)(void *ADbg_p);
/**< Pointer to function for entering module. */
typedef void (*ADbg_ModuleRun_t)(void *ADbg_p);

/** Interface functions for test case. */
typedef struct {
    ADbg_Create_t   Create;  /**< Create function. */
    ADbg_CaseRun_t  Run;     /**< Run function. */
    ADbg_Destroy_t  Destroy; /**< Destroy function. */
} ADbg_CaseInterface_t;

/** Interface functions for module. */
typedef struct {
    ADbg_Create_t     Create;  /**< Create function. */
    ADbg_ModuleRun_t  Run;     /**< Run function. */
    ADbg_Destroy_t    Destroy; /**< Destroy function. */
} ADbg_ModuleInterface_t;

/** Information for every single test case. */
typedef struct {
    char   *Name;         /**< Test case name. */
    uint8   CaseId;       /**< Test case Id. */
    uint8   ModuleId;     /**< Module Id. */
    uint8  *Parameters_p; /**< Pointer to buffer which holds the sizes of the
                             input parameters. */
    const ADbg_CaseInterface_t Interface;/**<Interface functions for test case. */
} ADbg_CaseDefinition_t;

/** Information for every single module. */
typedef struct {
    char   *Name;     /**< Module name. */
    uint8   ModuleId; /**< Module Id. */
    const ADbg_ModuleInterface_t Interface;/**< Interface functions for module. */
} ADbg_ModuleDefinition_t;

/*******************************************************************************
 * INTERFACE
 ******************************************************************************/
/**< Pointer to interface function. */
typedef void (*ADbg_IntFunctionRun_t)(void *ADbg_p);
/**< Pointer to function for entering interface group. */
typedef void (*ADbg_IntGroupRun_t)(void *ADbg_p);

/** Interface functions for interface function. */
typedef struct {
    ADbg_Create_t          Create;  /**< Create function. */
    ADbg_IntFunctionRun_t  Run;     /**< Run function. */
    ADbg_Destroy_t         Destroy; /**< Destroy function. */
} ADbg_IntFunctionInterface_t;

/** Interface functions for interface group. */
typedef struct {
    ADbg_Create_t       Create;  /**< Create function. */
    ADbg_IntGroupRun_t  Run;     /**< Run function. */
    ADbg_Destroy_t      Destroy; /**< Destroy function. */
} ADbg_IntGroupInterface_t;

/** Information for every single interface function. */
typedef struct {
    char   *Name;             /**< Interface function name. */
    uint32  IntFunctionId;    /**< Interface function Id. */
    uint32  IntGroupId;       /**< Interface group Id. */
    uint32  ModuleId;         /**< Module Id. */
    uint8  *Parameters_p;     /**< Pointer to buffer which holds the sizes of the
                                 input parameters. */
    ADbg_Recovery_t Recovery; /**< Information for recovery condition. */
    const   ADbg_IntFunctionInterface_t Interface; /**< Interface functions for
                                                      interface function. */
} ADbg_IntFunctionDefinition_t;

/** Information for every single interface group. */
typedef struct {
    char   *Name;       /**< Interface group name. */
    uint8   IntGroupId; /**< Interface group Id. */
    uint8   ModuleId;   /**< Module Id. */
    const   ADbg_IntGroupInterface_t Interface; /**< Interface functions for
                                                   interface group. */
} ADbg_IntGroupDefinition_t;

/*******************************************************************************
 *
 * UNIT TEST RUNNER
 *
 ******************************************************************************/

/** Information about one interface function. */
typedef struct {
    ADbg_Command_t   *Command_p;  /**< Pointer to information about received
                                     command. */
    ADbg_Result_t    *Result_p;   /**< Pointer to structure used to prepare
                                     general response. */
    ADbg_Recovery_t  *Recovery_p; /**< Pointer to information for recovery
                                     condition. */
} ADbg_IntFunction_t;

/** Information about one interface group. */
typedef struct {
    uint8   IntFunctionCounter;  /**< Number of interface functions in one
                                    interface group. */
    ADbg_IntFunctionDefinition_t **TestIntFunction_pp;/**< Pointer to array of all
                                                         interface functions
                                                         definitions. */
    ADbg_Command_t   *Command_p; /**< Pointer to information about received
                                    command. */
    ADbg_Result_t    *Result_p;  /**< Pointer to structure used to prepare general
                                    response. */
} ADbg_IntGroup_t;

/** Information about one test case. */
typedef struct {
    ADbg_Command_t   *Command_p; /**< Pointer to information about received
                                    command. */
    ADbg_Result_t    *Result_p;  /**< Pointer to structure used to prepare general
                                    response. */
} ADbg_Case_t;

/** Information about one module. */
typedef struct {
    uint8   CaseCounter;     /**< Number of test cases in one module. */
    uint8   IntGroupCounter; /**< Number of interface groups in one module. */
    const ADbg_CaseDefinition_t **TestCase_pp;     /**< Pointer to array of all
                                                      test cases definitions. */

    const ADbg_IntGroupDefinition_t **TestIntGroup_pp;/**< Pointer to array of all
                                                         interface groups
                                                         definitions. */
    ADbg_Command_t   *Command_p; /**< Pointer to information about received
                                    command. */
    ADbg_Result_t    *Result_p;  /**< Pointer to structure used to prepare general
                                    response. */
} ADbg_Module_t;

/** Information about all modules. */
typedef struct {
    uint8   ModuleCounter;                           /**< Number of modules. */
    const ADbg_ModuleDefinition_t   **TestModule_pp; /**< Pointer to array of all
                                                        modules definitions. */
    ADbg_Command_t   *Command_p; /**< Pointer to information about received
                                    command. */
    ADbg_Result_t    *Result_p;  /**< Pointer to structure used to prepare general
                                    response. */
} ADbg_MainModule_t;

/** @} */
#endif /*T_ADBG_H_*/
