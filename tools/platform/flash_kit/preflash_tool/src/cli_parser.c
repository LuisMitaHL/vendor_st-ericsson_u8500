/*******************************************************************************
 * $Copyright ST-Ericsson 2011 $
 ******************************************************************************/
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "cli_parser.h"
#include "preflash_tool.h"

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/
#ifndef MAX
#define MAX(a,b) (a) > (b) ? (a) : (b)
#endif

static Status_e ParseStatus = E_OK;

static CommandProperties_t CmdProperties[] = {

    {
        "preflash", /* command Name */
        {"-a", "\0"}, /* requireed parameters list */
        {"-gd", "-s", "--skipfirstblock", "-b", "\0"}, /* optional parameters list */
        CommandPreFlash
    },

    {
        "tocgen", /* command Name */
        {"-a", "\0"}, /* requireed parameters list */
        {"-b", "-f", "--splitoutputfile", "\0"}, /* optional parameters list */
        CommandTocGen
    },

    {
        "--help",
        {"\0"},
        {"\0"},
        CommandHelp
    },

    {
        "\0",
        {"\0"},
        {"\0"},
        NULL
    }
};

/*******************************************************************************
 * Declaration of internal functions
 ******************************************************************************/
static boolean IsSwitch(const char *LineParam_p);
static Switch_t *FindSwitch(Switch_t *Switches_p, char *Value_p);
static Status_e AddSwitch(Command_t *Command_p, char *Value_p);
static Status_e AddArgument(Param_t **ArgumentList_pp, char *Value_p);
static boolean AreParamsInList(Switch_t *SwitchIter_p, uint32 Index);
static boolean IsReqSwitchInList(Switch_t *SwitchIter_p, char *SwitchValue_p);
static boolean IsSwitchInParamList(const char *SwitchValue_p, uint32 Index);

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/
Status_e GetParseStatus()
{
    return ParseStatus;
}

void PrintCmdProperties(void)
{
    printf("Usage: preflash_tool <COMMAND_NAME> <REQUIRED_ARGUMENTS> [OPTIONAL_ARGUMENTS]...\n\n");

    printf("Command Name: preflash\n");
    printf("\t Description: Creates preflash image\n");
    printf("\t Required arguments:\n");
    printf("\t\t -a [FLASH_ARCHIVE] \t Flash archive that need to be parsed\n");
    printf("\t Optional arguments:\n");
    printf("\t\t -gd [GDF_FILE] \t Optional gdf file that need to be written\n");
    printf("\t\t -s, --skipfirstblock \t Output image will start from second boot block\n");
    printf("\t\t -b \t\t\t Create preflash image in binary format (default is S record)\n");
    printf("\n");

    printf("Command Name: tocgen\n");
    printf("\t Description: Creates toc boot image\n");
    printf("\t Required arguments:\n");
    printf("\t\t -a [FLASH_ARCHIVE] \t Flash archive that need to be parsed\n");
    printf("\t Optional arguments:\n");
    printf("\t\t -f, --splitoutputfile \t Split output image in two files\n");
    printf("\t\t -b, \t\t\t Create boot area in binary format (default is S record)\n");
    printf("\n");
}

Status_e ExecuteCommand(Command_t *Command_p)
{
    if (NULL != Command_p) {
        if (NULL != Command_p->Name) {
            uint32 Index = 0;

            while (0 != strcmp("\0", CmdProperties[Index].Name)) {
                size_t Count = MAX(strlen(CmdProperties[Index].Name), strlen(Command_p->Name));

                if (0 == strncmp(CmdProperties[Index].Name, Command_p->Name, Count)) {
                    if (NULL != CmdProperties[Index].CmdFunction_p) {
                        if (0 != CmdProperties[Index].CmdFunction_p(Command_p)) {
                            return E_CMD_EXEC_ERROR;
                        }
                    } else {
                        return E_COMMAND_FUNCTION_NULL;
                    }

                    return E_OK;
                }

                Index++;
            }

            return E_COMMAND_NOT_FOUND;
        } else {
            return E_COMMAND_NAME_NULL;
        }
    } else {
        return E_COMMAND_DATA_NULL;
    }
}

Status_e CheckCommandParameters(Command_t *Command_p)
{
    uint32 i = 0;
    boolean MissmatchFound = FALSE;
    uint32 SwitchIndex = 0;

    if (NULL != Command_p) {
        while ((0 != strcmp("\0", CmdProperties[i].Name)) && (FALSE == MissmatchFound)) {
            if (0 == strcmp(CmdProperties[i].Name, Command_p->Name)) {
                SwitchIndex = 0;

                /* check if some of the requred parameters is missing */
                while (0 != strcmp("\0", CmdProperties[i].ReqSwitches[SwitchIndex])) {
                    MissmatchFound = !IsReqSwitchInList(Command_p->Switches_p, CmdProperties[i].ReqSwitches[SwitchIndex]);

                    if (MissmatchFound) {
                        break;
                    }

                    SwitchIndex++;
                }

                break;
            }

            i++;
        }

        if (TRUE == MissmatchFound) {
            printf("Missing required switch '%s'\n", CmdProperties[i].ReqSwitches[SwitchIndex]);
            return E_MISSING_REQ_PARAMS;
        } else {
            if (0 == strcmp("\0", CmdProperties[i].Name)) {
                return E_COMMAND_NOT_FOUND;
            } else {
                /* check if some of the parameters is not in the parameter list*/
                if (FALSE == AreParamsInList(Command_p->Switches_p, i)) {
                    return E_INVALID_SWITCH;
                }

                return E_OK;
            }
        }
    } else {
        return E_COMMAND_DATA_NULL;
    }
}

char *GetSwitchParam(Command_t *Command_p, char *SwitchValue_p, uint32 ParamIndex)
{
    if (NULL != Command_p) {
        Switch_t *Switch_p = FindSwitch(Command_p->Switches_p, SwitchValue_p);

        if (NULL != Switch_p) {
            if (NULL != Switch_p->Params_p) {
                Param_t *Params_p = Switch_p->Params_p;

                while (ParamIndex--) {
                    Params_p = Params_p->Next;

                    if (NULL == Params_p) {
                        ParseStatus = E_INVALID_PARAM_INDEX;
                        return NULL;
                    }
                }

                return Params_p->Value;
            }
        }
    }

    ParseStatus = E_NULL_INPUT;
    return NULL;
}

boolean IsSwitchInInputList(Command_t *Command_p, char *SwitchValue_p)
{
    if ((NULL == Command_p) || (NULL == SwitchValue_p)) {
        ParseStatus = E_NULL_INPUT;
        return FALSE;
    }

    if (NULL != FindSwitch(Command_p->Switches_p, SwitchValue_p)) {
        return TRUE;
    }

    return FALSE;
}

void FreeCommandData(Command_t **Command_p)
{
    if (NULL != Command_p) {
        if (NULL != *Command_p) {
            Switch_t *SwitchIter_p = (*Command_p)->Switches_p;

            while (NULL != SwitchIter_p) {
                Switch_t *switch_pos_p = SwitchIter_p;
                Param_t *ParamIter_p = SwitchIter_p->Params_p;

                while (NULL != ParamIter_p) {
                    Param_t *pos_p = ParamIter_p;
                    ParamIter_p = ParamIter_p->Next;
                    free(pos_p);
                }

                SwitchIter_p = SwitchIter_p->Next;
                free(switch_pos_p);
            }

            if (NULL != (*Command_p)->Name) {
                free((*Command_p)->Name);
            }

            free(*Command_p);
            *Command_p = NULL;
        }
    }
}

Command_t *ParseArguments(int argc, char **argv)
{
    int i;
    char *Param_p;
    char *ParamValue_p = NULL;

    Command_t *Command_p = (Command_t *)malloc(sizeof(Command_t));

    if (NULL == Command_p) {
        ParseStatus = E_MALLOC_FAILED;
        return NULL;
    }

    if (argc < 2) {
        ParseStatus = E_NULL_INPUT;
        return NULL;
    }

    Command_p->Switches_p = NULL;
    Command_p->Next = NULL;
    Command_p->CurrentSwitch = NULL;
    Command_p->Name = (char *)malloc((strlen(argv[1]) + 1) * sizeof(char));

    if (NULL == Command_p->Name) {
        free(Command_p);
        ParseStatus = E_MALLOC_FAILED;
        return NULL;
    } else {
        strcpy(Command_p->Name, argv[1]);
    }

    for (i = 2; i < argc; ++i) {
        if (IsSwitch(argv[i])) {
            Param_p = argv[i];
            ParamValue_p = "\0";

            if (argc > i + 1) {
                if (!IsSwitch(argv[i + 1])) {
                    ParamValue_p = argv[i + 1];
                    i++;
                }
            }

            AddSwitch(Command_p, Param_p);

            if ((NULL != ParamValue_p) && (0 != strcmp("\0", ParamValue_p))) {
                AddArgument(&Command_p->CurrentSwitch->Params_p, ParamValue_p);
            }
        } else {
            Switch_t *Switch_p;

            if (2 == i) {
                ParseStatus = E_INVALID_LINE;
                FreeCommandData(&Command_p);
                return NULL;
            }

            Switch_p = FindSwitch(Command_p->Switches_p, Param_p);
            AddArgument(&Switch_p->Params_p, argv[i]);
        }
    }

    return Command_p;
}

void PrintCommand(const Command_t *const Command_p)
{
    if (NULL != Command_p) {
        Switch_t *SwitchIter_p = Command_p->Switches_p;

        while (NULL != SwitchIter_p) {
            Param_t *ParamIter_p = SwitchIter_p->Params_p;
            printf("%s ", SwitchIter_p->Value);

            while (NULL != ParamIter_p) {
                printf("%s ", ParamIter_p->Value);
                ParamIter_p = ParamIter_p->Next;
            }

            SwitchIter_p = SwitchIter_p->Next;

            printf("\n");
        }
    }
}

/*******************************************************************************
 * Definition of internal functions
 ******************************************************************************/

/**
 * @brief Checks if input substring from command line is switch
 *
 * @param [in] LineParam_p - Substring parsed from the command line input
 *
 * @return                 TRUE if the LineParam_p points to switch data, FALSE otherwise
 *
 */
static boolean IsSwitch(const char *LineParam_p)
{
    if (2 > strlen(LineParam_p)) {
        return FALSE;
    } else {
        return ('-' == LineParam_p[0]);
    }
}

/**
 * @brief Searches for command line switch by its value
 *
 * @param [in] Switches_p  - Pointer to the list of switches from the command that is parsed
 * @param [in] Value_p     - Value of the switch to search for including the '-' character
                             at the beginning of it
 *
 * @return                 Pointer to the switch if it is found in the list, NULL otherwise
 *
 */
static Switch_t *FindSwitch(Switch_t *Switches_p, char *Value_p)
{
    while (NULL != Switches_p) {
        if (0 == strcmp(Value_p, Switches_p->Value)) {
            return Switches_p;
        }

        Switches_p = Switches_p->Next;
    }

    return NULL;
}

/**
 * @brief Adds new command line switch in the command data structure
 *
 * @param [in] Command_p  - Pointer to the comand data structure
 * @param [in] Value_p    - Value of the switch to add in the command structure
 *
 * @return                Execution status (E_OK after successful execution)
 *
 */
static Status_e AddSwitch(Command_t *Command_p, char *Value_p)
{
    Switch_t **ArgumentList_pp = &Command_p->Switches_p;
    Switch_t *NewSwitch_p = (Switch_t *)malloc(sizeof(Switch_t));

    if (NULL == NewSwitch_p) {
        return E_MALLOC_FAILED;
    } else {
        NewSwitch_p->Next = NULL;
        NewSwitch_p->Params_p = NULL;
        NewSwitch_p->Value = (char *)malloc(strlen(Value_p) + sizeof(char));

        if (NULL == NewSwitch_p->Value) {
            free(NewSwitch_p);
            return E_MALLOC_FAILED;
        } else {
            strcpy(NewSwitch_p->Value, Value_p);

            if (NULL == *ArgumentList_pp) {
                *ArgumentList_pp = NewSwitch_p;
            } else {
                Switch_t *It_p = *ArgumentList_pp;

                while (NULL != It_p->Next) {
                    It_p = It_p->Next;
                }

                It_p->Next = NewSwitch_p;
            }

            Command_p->CurrentSwitch = NewSwitch_p;
        }
    }

    return E_OK;
}

/**
 * @brief Adds new command line argument in the command data structure
 *
 * @param [in] Command_p  - Pointer to the comand data structure
 * @param [in] Value_p    - Value of the argument to add in the command structure
 *
 * @return                Execution status (E_OK after successful execution)
 *
 */
static Status_e AddArgument(Param_t **ArgumentList_pp, char *Value_p)
{
    Param_t *NewArgument_p = (Param_t *)malloc(sizeof(Param_t));

    if (NULL == NewArgument_p) {
        return E_MALLOC_FAILED;
    } else {
        NewArgument_p->Next = NULL;
        NewArgument_p->Value = (char *)malloc(strlen(Value_p) + sizeof(char));

        if (NULL == NewArgument_p->Value) {
            free(NewArgument_p);
            return E_MALLOC_FAILED;
        } else {
            strcpy(NewArgument_p->Value, Value_p);

            if (NULL == *ArgumentList_pp) {
                *ArgumentList_pp = NewArgument_p;
            } else {
                Param_t *It_p = *ArgumentList_pp;

                while (NULL != It_p->Next) {
                    It_p = It_p->Next;
                }

                It_p->Next = NewArgument_p;
            }
        }
    }

    return E_OK;
}

/**
 * @brief Checks if switch indicated by SwitchValue_p is found in switch list of
 * the command with index equal to the value of Index parameter
 *
 * @param [in] SwitchValue_p  - Pointer to the switch list data
 * @param [in] Index          - Index of the command
 *
 * @return                Boolean value indicating whether the switch is found
                          in the list or not
 *
 */
static boolean IsSwitchInParamList(const char *SwitchValue_p, uint32 Index)
{
    uint32 SwitchIndex = 0;

    while (0 != strcmp("\0", CmdProperties[Index].OptionalSwitches[SwitchIndex])) {
        if (0 == strcmp(SwitchValue_p, CmdProperties[Index].OptionalSwitches[SwitchIndex])) {
            return TRUE;
        }

        SwitchIndex++;
    }

    SwitchIndex = 0;

    while (0 != strcmp("\0", CmdProperties[Index].ReqSwitches[SwitchIndex])) {
        if (0 == strcmp(SwitchValue_p, CmdProperties[Index].ReqSwitches[SwitchIndex])) {
            return TRUE;
        }

        SwitchIndex++;
    }

    printf("Invalid command switch '%s'\n", SwitchValue_p);
    return FALSE;
}

/**
 * @brief Checks if switch indicated by SwitchValue_p is found in switch list SwitchIter_p
 *
 * @param [in] SwitchIter_p  - Pointer to the switch list data
 * @param [in] SwitchValue_p - Value of the switch
 *
 * @return                Boolean value indicating whether the switch is found
 *                        in the list or not
 *
 */
static boolean IsReqSwitchInList(Switch_t *SwitchIter_p, char *SwitchValue_p)
{
    while (NULL != SwitchIter_p) {
        if (0 == strcmp(SwitchIter_p->Value, SwitchValue_p)) {
            return TRUE;
        }

        SwitchIter_p = SwitchIter_p->Next;
    }

    return FALSE;
}

/**
 * @brief Checks if all the switches from SwitchIter_p are found in
 * the switch list of the command with index value equal to Index
 *
 * @param [in] SwitchIter_p  - Pointer to the switch list data
 * @param [in] Index         - Index of the command
 *
 * @return                Boolean value indicating whether the switches are
 *                        in the list or not
 *
 */
static boolean AreParamsInList(Switch_t *SwitchIter_p, uint32 Index)
{
    while (NULL != SwitchIter_p) {
        if (!IsSwitchInParamList(SwitchIter_p->Value, Index)) {
            return FALSE;
        }

        SwitchIter_p = SwitchIter_p->Next;
    }

    return TRUE;
}
