/*******************************************************************************
 * $Copyright ST-Ericsson 2011 $
 ******************************************************************************/
#ifndef CLI_PARSER_H
#define CLI_PARSER_H

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_basicdefinitions.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/
typedef enum {
    E_OK,
    E_EMPTY_PARAM_LINE,
    E_INVAL_PARAM_FORMAT,
    E_INVAL_PARAM_TYPE,
    E_MALLOC_FAILED,
    E_INVALID_LINE,
    E_INVALID_PARAMS,
    E_NULL_INPUT,
    E_MISSING_REQ_PARAMS,
    E_INVALID_PARAM_INDEX,
    E_INVALID_COMMAND_NAME,
    E_COMMAND_DATA_NULL,
    E_COMMAND_NAME_NULL,
    E_COMMAND_NOT_FOUND,
    E_COMMAND_FUNCTION_NULL,
    E_CMD_EXEC_ERROR,
    E_INVALID_SWITCH
} Status_e;

typedef struct Param {
    char *Value;
    struct Param *Next;
} Param_t;

typedef struct Switch {
    char *Value;
    Param_t *Params_p;
    struct Switch *Next;
} Switch_t;

typedef struct Command {
    Switch_t *CurrentSwitch;
    Switch_t *Switches_p;
    char *Name;
    struct Command *Next;
} Command_t;

typedef struct {
    char *Name;
    char *ReqSwitches[32];
    char *OptionalSwitches[32];
    int (*CmdFunction_p)(Command_t *Command_p);
} CommandProperties_t;

#define CLI_ERROR ((uint32) -1)

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/

/**
 * @brief Prints full command data, including command name,
 * command line, switches and parameter values
 *
 * @param [in] Command_p     - Pointer to command data structure
 *
 */
void PrintCommand(const Command_t *const Command_p);

/**
 * @brief Parses command line arguments that are send to preflash tool
 *
 * @param [in] argc     - Standard argument count parameter
 * @param [in] argv     - Standard argument array
 *
 * @return              Pointer to command data structure
 *
 */
Command_t *ParseArguments(int argc, char **argv);

/**
 * @brief Checks if switch indicated by SwitchValue_p is found in the list
 * of inserted switches for specified command
 *
 * @param [in] Command_p  - Pointer to the command data structure
 * @param [in] SwitchValue_p  - Value of the switch to search for
 *
 * @return                Boolean value indicating whether the switch is found
                          in the list or not
 *
 */
boolean IsSwitchInInputList(Command_t *Command_p, char *SwitchValue_p);

/**
 * @brief Frees allocated command data
 *
 * @param [in] Command_p - Pointer to the structure holding
 *                         the command data to be freed
 *
 */
void FreeCommandData(Command_t **Command_p);

/**
 * @brief Checks if the command line input string provides all required switches
 *
 * @param [in] Command_p  - Pointer to the command data structure
 *
 * @return                Execution status (E_OK after successful execution)
 *
 */
Status_e CheckCommandParameters(Command_t *Command_p);

/**
 * @brief Returns parameter value from a command line switch by its index
 *
 * @param [in] Command_p      - Pointer to the command data structure
 * @param [in] SwitchValue_p  - Value of the switch to search for
 * @param [in] ParamIndex     - Index of the parameter
 *
 * @return                    Value of the parameter after successful execution, NULL otherwise
 *
 */
char *GetSwitchParam(Command_t *Command_p, char *SwitchValue_p, uint32 ParamIndex);

/**
 * @brief Prints info for all of the commands found
 * in the command list of the CLI parser
 *
 */
void PrintCmdProperties(void);

/**
 * @brief Returns the execution status of the last method
 * invoked
 *
 * @return                Execution status (E_OK after successful execution)
 */
Status_e GetParseStatus(void);

/**
 * @brief Invokes the command method for the command defined by Command_p
 *
 * @param [in] Command_p  - Pointer to the command data structure
 *
 * @return                Execution status (E_OK after successful execution)
 */
Status_e ExecuteCommand(Command_t *Command_p);

#endif
