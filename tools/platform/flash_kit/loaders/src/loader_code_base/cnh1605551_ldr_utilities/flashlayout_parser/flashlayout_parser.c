/*******************************************************************************
 * $Copyright ST-Ericsson 2012 $
 ******************************************************************************/

/*
 *  @addtogroup ldr_utilities
 *  @{
 *    @addtogroup flashlayout_parser
 *    @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "c_system.h"
#include <string.h>
#ifdef __STRICT_ANSI__
#undef __STRICT_ANSI__
#include <stdlib.h>
#define __STRICT_ANSI__
#else
#include <stdlib.h>
#endif //__STRICT_ANSI__
#include "r_basicdefinitions.h"
#include "error_codes.h"
#include "r_debug.h"
#include "r_debug_macro.h"
#include "r_flashlayout_parser.h"
#include "r_memory_utils.h"


/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/
/** Maxium number of parsed units. */
#define MAX_NUMBER_OF_PARSED_UNITS  128
/** Maximum size of description field. */
#define MAX_SIZE_OF_DESCRIPTION  100
/** Maximum size of each token, including file name with path. */
#define MAX_SIZE_OF_TOKEN             MAX_PATH_SIZE

/** Revision 2.0 is legacy version for flash layout. */
#define FLASHLAYOUT_PARSER_VERSION_2_0 "2.0"
/** Revision 3.0 is generic version for flash layout. */
#define FLASHLAYOUT_PARSER_VERSION_3_0 "3.0"

static FlashLayoutRevision_t FlashLayoutRevision = {0};

/*******************************************************************************
 * Declaration of file local functions
 ******************************************************************************/
static void Reset_All_Flashlayout_Parameters(FlashLayoutParser_t *FlashLayoutEntry_p);

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/
ErrorCode_e Do_FlashLayoutParser_Create(char *FileData_p, uint32 FileSize, FlashLayoutParser_t **const FlashLayoutParser_pp, uint32 *NoOfFiles_p)
{
    ErrorCode_e          ReturnValue         = E_SUCCESS;
    FlashLayoutParser_t *FlashlayoutHandle_p = NULL;
    char                *LinkData_p          = NULL;
    char                *Token_p             = NULL;
    char                *TempToken_p         = NULL;
    uint32               Index;
    uint32               TokenLength;

    VERIFY(NULL != FileData_p, E_INVALID_INPUT_PARAMETER);

    LinkData_p = FileData_p;
    FlashlayoutHandle_p = (FlashLayoutParser_t *)malloc(sizeof(FlashLayoutParser_t) * MAX_NUMBER_OF_PARSED_UNITS);
    ASSERT(NULL != FlashlayoutHandle_p);

    *FlashLayoutParser_pp = FlashlayoutHandle_p;

    Reset_All_Flashlayout_Parameters(FlashlayoutHandle_p);

    Token_p = LinkData_p;
    TempToken_p = (char *)malloc(MAX_SIZE_OF_TOKEN);
    ASSERT(NULL != TempToken_p);
    *NoOfFiles_p = 0;
    Index = FileSize;

    while (Index) {
        if ((Index > strlen("Flash-Layout-Version: ")) && (0 == strncmp("Flash-Layout-Version: ", LinkData_p, strlen("Flash-Layout-Version: ")))) {
            LinkData_p += strlen("Flash-Layout-Version: ");
            Index -= strlen("Flash-Layout-Version: ");

            Token_p = LinkData_p;

            while (((*LinkData_p != 0x0A) && (*LinkData_p != 0x0D)) && (LinkData_p < (FileData_p + FileSize))) {
                LinkData_p++;
            }

            TokenLength = LinkData_p - Token_p;
            VERIFY(TokenLength < MAX_SIZE_OF_TOKEN , E_NAME_TOO_LONG);
            Index -= TokenLength;

            if (((Index > strlen(FLASHLAYOUT_PARSER_VERSION_2_0)) && (0 != strncmp(FLASHLAYOUT_PARSER_VERSION_2_0, Token_p, strlen(FLASHLAYOUT_PARSER_VERSION_2_0)))) &&
                    ((Index > strlen(FLASHLAYOUT_PARSER_VERSION_3_0)) && (0 != strncmp(FLASHLAYOUT_PARSER_VERSION_3_0, Token_p, strlen(FLASHLAYOUT_PARSER_VERSION_3_0))))) {
                strncpy(TempToken_p, Token_p, TokenLength);
                TempToken_p[TokenLength] = '\0';
                A_(printf("flashlayout_parser.c: Error: Can not parse this version: %s\n", TempToken_p);)
                free(TempToken_p);
                return E_GENERAL_FATAL_ERROR;
            } else {
                if (0 == strncmp(FLASHLAYOUT_PARSER_VERSION_2_0, Token_p, strlen(FLASHLAYOUT_PARSER_VERSION_2_0))) {
                    FlashLayoutRevision.RevNrMajor = 2;
                    FlashLayoutRevision.RevNrMinor = 0;
                } else if (0 == strncmp(FLASHLAYOUT_PARSER_VERSION_3_0, Token_p, strlen(FLASHLAYOUT_PARSER_VERSION_3_0))) {
                    FlashLayoutRevision.RevNrMajor = 3;
                    FlashLayoutRevision.RevNrMinor = 0;
                }

                strncpy(TempToken_p, Token_p, TokenLength);
                TempToken_p[TokenLength] = '\0';
                C_(printf("flashlayout_parser.c: Flashlayout Version = %s\n", TempToken_p);)
            }
        }

        else if ((Index > strlen("Path: ")) && (0 == strncmp("Path: ", LinkData_p, strlen("Path: ")))) {
            (*NoOfFiles_p)++;

            if (*NoOfFiles_p != 1) {
                FlashlayoutHandle_p++;

                if (MAX_NUMBER_OF_PARSED_UNITS < *NoOfFiles_p) {
                    (*NoOfFiles_p)--;
                    break;
                }
            }

            LinkData_p += strlen("Path: ");
            Index      -= strlen("Path: ");
            Token_p     = LinkData_p;

            while (((*LinkData_p != 0x0A) && (*LinkData_p != 0x0D)) && (LinkData_p < (FileData_p + FileSize))) {
                LinkData_p++;
            }

            TokenLength = LinkData_p - Token_p;
            VERIFY(TokenLength < MAX_SIZE_OF_TOKEN , E_FS_NAME_TOO_LONG);
            Index -= TokenLength;
            strncpy(TempToken_p, Token_p, TokenLength);
            TempToken_p[TokenLength] = '\0';
            strncpy(FlashlayoutHandle_p->Path, TempToken_p, TokenLength + 1);

            /* Search for TOC ID and SUB TOC */
            {
                char *TOC_p    = NULL;
                char *SubTOC_p = NULL;

                TOC_p = strstr(TempToken_p, "TOC/");

                if (TOC_p != NULL) {
                    /* TOC found in Path */
                    TOC_p += strlen("TOC/");
                    SubTOC_p = strrchr(TOC_p, '/');

                    if (SubTOC_p != NULL) {
                        /* SubTOC found */
                        strncpy(FlashlayoutHandle_p->TOC_ID, SubTOC_p + 1, MAX_TOC_ID_SIZE - 1);
                        FlashlayoutHandle_p->TOC_ID[MAX_TOC_ID_SIZE - 1] = '\0';
                        TOC_p[SubTOC_p - TOC_p] = '\0';

                        strncpy(FlashlayoutHandle_p->SubTOC_Of, TOC_p, MAX_TOC_ID_SIZE);
                        FlashlayoutHandle_p->SubTOC_Of[MAX_TOC_ID_SIZE - 1] = '\0';
                    } else {
                        /* Only TOC found in Path */
                        strncpy(FlashlayoutHandle_p->TOC_ID, TOC_p, MAX_TOC_ID_SIZE - 1);
                        FlashlayoutHandle_p->TOC_ID[MAX_TOC_ID_SIZE - 1] = '\0';
                    }
                } else {
                    // Image in boot area other than BootImages should get a TOC_ID
                    char *BootAreaEntry_p = NULL;
                    BootAreaEntry_p = strstr(TempToken_p, "bootarea/");

                    if (BootAreaEntry_p != NULL) {
                        BootAreaEntry_p += strlen("bootarea/");
                        strncpy(FlashlayoutHandle_p->TOC_ID, BootAreaEntry_p, MAX_TOC_ID_SIZE - 1);
                        FlashlayoutHandle_p->TOC_ID[MAX_TOC_ID_SIZE - 1] = '\0';
                    }
                }
            }
        }

        else if ((Index > strlen("Content-Type: ")) && (0 == strncmp("Content-Type: ", LinkData_p, strlen("Content-Type: ")))) {
            LinkData_p += strlen("Content-Type: ");
            Index -= strlen("Content-Type: ");
            Token_p = LinkData_p;

            while (((*LinkData_p != 0x0A) && (*LinkData_p != 0x0D)) && (LinkData_p < (FileData_p + FileSize))) {
                LinkData_p++;
            }

            TokenLength = LinkData_p - Token_p;
            VERIFY(TokenLength < MAX_SIZE_OF_DESCRIPTION , E_NAME_TOO_LONG);
            Index -= TokenLength;
            strncpy(TempToken_p, Token_p, TokenLength);
            TempToken_p[TokenLength] = '\0';
            strncpy(FlashlayoutHandle_p->ContentType, TempToken_p, TokenLength + 1);
        }

        else if ((Index > strlen("Description: ")) && (0 == strncmp("Description: ", LinkData_p, strlen("Description: ")))) {
            LinkData_p += strlen("Description: ");
            Index -= strlen("Description: ");
            Token_p = LinkData_p;

            while (((*LinkData_p != 0x0A) && (*LinkData_p != 0x0D)) && (LinkData_p < (FileData_p + FileSize))) {
                LinkData_p++;
            }

            TokenLength = LinkData_p - Token_p;
            VERIFY(TokenLength < MAX_SIZE_OF_DESCRIPTION , E_DESCRIPTION_TOO_LONG);
            Index -= TokenLength;
            strncpy(TempToken_p, Token_p, TokenLength);
            TempToken_p[TokenLength] = '\0';
            strncpy(FlashlayoutHandle_p->Description, TempToken_p, TokenLength + 1);
        }

        else if ((Index > strlen("Start: ")) && (0 == strncmp("Start: ", LinkData_p, strlen("Start: ")))) {
            LinkData_p += strlen("Start: ");
            Index -= strlen("Start: ");
            Token_p = LinkData_p;

            while (((*LinkData_p != 0x0A) && (*LinkData_p != 0x0D)) && (LinkData_p < (FileData_p + FileSize))) {
                LinkData_p++;
            }

            TokenLength = LinkData_p - Token_p;
            VERIFY(TokenLength < MAX_SIZE_OF_TOKEN , E_NAME_TOO_LONG);
            Index -= TokenLength;
            strncpy(TempToken_p, Token_p, TokenLength);
            TempToken_p[TokenLength] = '\0';

            FlashlayoutHandle_p->StartAddress = strtoull(TempToken_p, NULL, 16);

            if (2 == FlashLayoutRevision.RevNrMajor) {
                VERIFY(0x00000000FFFFFFFFULL >= FlashlayoutHandle_p->StartAddress, E_UNSUPPORTED_START_ADDRESS);
            }
        }

        else if ((Index > strlen("End: ")) && (0 == strncmp("End: ", LinkData_p, strlen("End: ")))) {
            LinkData_p += strlen("End: ");
            Index -= strlen("End: ");
            Token_p = LinkData_p;

            while (((*LinkData_p != 0x0A) && (*LinkData_p != 0x0D)) && (LinkData_p < (FileData_p + FileSize))) {
                LinkData_p++;
            }

            TokenLength = LinkData_p - Token_p;
            VERIFY(TokenLength < MAX_SIZE_OF_TOKEN , E_NAME_TOO_LONG);
            Index -= TokenLength;
            strncpy(TempToken_p, Token_p, TokenLength);
            TempToken_p[TokenLength] = '\0';

            FlashlayoutHandle_p->EndAddress = strtoull(TempToken_p, NULL, 16);
        }

        else {
            LinkData_p++;
            Index--;
        }

        while (((*LinkData_p == 0x0A) || (*LinkData_p == 0x0D)) && Index) {
            LinkData_p++;
            Index--;
        }
    }

ErrorExit:
    free(TempToken_p);

    return ReturnValue;
}

ErrorCode_e Do_FlashLayoutParser_Destroy(FlashLayoutParser_t **const FlashLayoutParser_pp)
{
    ErrorCode_e ReturnValue = E_SUCCESS;

    VERIFY(NULL != FlashLayoutParser_pp, E_INVALID_INPUT_PARAMETERS);

    ASSERT(NULL != *FlashLayoutParser_pp);

    BUFFER_FREE(*FlashLayoutParser_pp);

ErrorExit:
    return ReturnValue;
}

/*******************************************************************************
 * Definition of internal functions
 ******************************************************************************/
static void Reset_All_Flashlayout_Parameters(FlashLayoutParser_t *FlashLayoutEntry_p)
{
    uint32 Counter = 0;

    for (Counter = 0; Counter < MAX_NUMBER_OF_PARSED_UNITS; Counter++) {
        FlashLayoutEntry_p->Path[0]        = '\0';
        FlashLayoutEntry_p->ContentType[0] = '\0';
        FlashLayoutEntry_p->Description[0] = '\0';
        FlashLayoutEntry_p->TOC_ID[0]      = '\0';
        FlashLayoutEntry_p->SubTOC_Of[0]   = '\0';
        FlashLayoutEntry_p->StartAddress   = 0xFFFFFFFFFFFFFFFFULL;
        FlashLayoutEntry_p->EndAddress     = 0xFFFFFFFFFFFFFFFFULL;

        FlashLayoutEntry_p++;
    }
}

void Do_FlashLayoutParser_GetRevision(FlashLayoutRevision_t *FlashLayoutRevision_p)
{
    FlashLayoutRevision_p->RevNrMajor = FlashLayoutRevision.RevNrMajor;
    FlashLayoutRevision_p->RevNrMinor = FlashLayoutRevision.RevNrMinor;
}

/*@}*/
/*@}*/
