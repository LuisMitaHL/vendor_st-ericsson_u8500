/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

/*
 * @addtogroup ldr_utilities
 * @{
 *   @addtogroup versions_parser
 *   @{
 */


/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <string.h>
#include "versions_parser.h"
#include "flash_process_file_info.h"
#include "r_debug_macro.h"

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/
#define MAX_MODULE_ID_LENGTH (12 + 1)
#define MAX_VERSION_LENGTH 256
#define MAX_RECORDS 16

static const char *IDError_p = "ID ERROR";
static const char *VersionError_p = "VERSION STRING FORMAT ERROR";

typedef struct {
    char ModuleId[MAX_MODULE_ID_LENGTH];
    char Version[MAX_VERSION_LENGTH];
} VersionsEntry_t;

static VersionsEntry_t VersionsParser[MAX_RECORDS];
static uint32 RecordCount = 0;


/*******************************************************************************
 * Declaration of file local functions
 ******************************************************************************/
static boolean IsEmptyChar(char Ch);
static void StrTrim(char **Str_pp, uint32 Size, boolean Left, boolean Right);

void Do_ReadVersionsInfo(char *Data_p, sint32 Size)
{
    char *Pos_p = Data_p;
    char *Head_p = Data_p;
    char *Tail_p = Data_p + Size;
    uint32 Length = 0;
    char Term = ';';
    uint32 Guard = 0;

    memset(VersionsParser, 0, sizeof(VersionsParser));
    RecordCount = 0;

    while (Size > 0) {
        StrTrim(&Pos_p, strlen(Pos_p), TRUE, FALSE);

        if (*Pos_p == 0) {
            break;
        }

        Head_p = Pos_p;
        Length = 0;

        Guard = 0;

        while (*Pos_p != ':' && Guard < MAX_MODULE_ID_LENGTH) {
            Pos_p++;
            Length++;
            Guard++;
        }

        if (MAX_MODULE_ID_LENGTH == Guard) {
            strncpy(VersionsParser[RecordCount].ModuleId, IDError_p, MIN(Length, MAX_MODULE_ID_LENGTH - 1));
            RecordCount++;
            break;
        } else {
            strncpy(VersionsParser[RecordCount].ModuleId, Head_p, MIN(Length, MAX_MODULE_ID_LENGTH - 1));
        }

        Pos_p++;
        StrTrim(&Pos_p, strlen(Pos_p), TRUE, FALSE);

        Head_p = Pos_p;
        Length = 0;

        Guard = 0;

        while (*Pos_p != Term && Guard < MAX_VERSION_LENGTH) {
            Pos_p++;
            Length++;
            Guard++;
        }

        if (MAX_VERSION_LENGTH == Guard) {
            strncpy(VersionsParser[RecordCount].Version, VersionError_p, MIN(Length, MAX_VERSION_LENGTH - 1));
            RecordCount++;
            break;
        } else {
            Pos_p++;
            strncpy(VersionsParser[RecordCount].Version, Head_p, MIN(Length, MAX_VERSION_LENGTH - 1));
        }

        Size = (uint32)Tail_p - (uint32)Head_p - (strlen(VersionsParser[RecordCount].Version) + 1);
        RecordCount++;
    }
}

void Do_PrintVersionsInfo(void)
{
    uint32 i;

    if (0 != RecordCount) {
        AddFlashInfoRecord("\nDisplaying version information\n");
        AddFlashInfoRecord("-----------------------------------\n");

        for (i = 0;  i < RecordCount; ++i) {
            AddFlashInfoRecord("%-12s: %s\n", VersionsParser[i].ModuleId, VersionsParser[i].Version);
        }

        AddFlashInfoRecord("-----------------------------------\n\n");
    }
}

/*******************************************************************************
 * Definition of internal functions
 ******************************************************************************/
static boolean IsEmptyChar(char Ch)
{
    return (Ch == ' ' || Ch == 0x0A || Ch == 0x0D || Ch == 0x09);
}

static void StrTrim(char **Str_pp, uint32 Size, boolean Left, boolean Right)
{
    char *End_p = NULL;

    if (!Str_pp) {
        return;
    }

    if (*Str_pp) {
        if (Left) {
            while (IsEmptyChar(**Str_pp)) {
                (*Str_pp)++;
            }
        }

        if (Right) {
            End_p = *Str_pp + strlen(*Str_pp) - 1;

            while (IsEmptyChar(*End_p)) {
                *End_p = 0;
                End_p--;
            }
        }
    }
}
/** @} */
