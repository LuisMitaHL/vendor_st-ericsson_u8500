/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

/*
 *  @addtogroup ldr_utilities
 *  @{
 *    @addtogroup manifest_parser
 *    @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "c_system.h"
#include <stdio.h>
#include <string.h>
#ifdef __STRICT_ANSI__
#undef __STRICT_ANSI__
#include <stdlib.h>
#define __STRICT_ANSI__
#else
#include <stdlib.h>
#endif //__STRICT_ANSI__
#include <ctype.h>
#include "r_basicdefinitions.h"
#include "error_codes.h"
#include "r_debug.h"
#include "r_debug_macro.h"
#include "r_manifest_parser.h"
#include "r_path_utilities.h"
#include "r_memory_utils.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/
/** Current version of the Manifest */

#define VERSIONNUMBER_STRING "2.0"

/** Max length of String in tokenentry struct below, should only be used there */
#define MAX_ID_NAME_LENGTH 30

typedef enum {
    IDUnknown,
    IDVersion,
    IDName,
    IDContent,
    IDTarget,
    IDTocID,
    IDTocLoadAddress,
    IDTocEntryPoint,
    IDTocFlags,
    IDFill,
    IDEnhanced,
    IDSizeMode
} tokenid;

typedef struct {
    tokenid entryID;
    char entryText[MAX_ID_NAME_LENGTH];
} tokenentry;

tokenentry ManifestTokens[] = {
    {IDVersion,        "EmpFlash-Manifest-Version:"},
    {IDVersion,        "STEFlash-Manifest-Version:"},
    {IDName,           "Name:"},
    {IDContent,        "Content-Type:"},
    {IDTarget,         "Target:"},
    {IDTocID,          "TOC-ID:"},
    {IDTocLoadAddress, "TOC-LoadAddress:"},
    {IDTocEntryPoint,  "TOC-EntryPoint:"},
    {IDTocFlags,       "TOC-Flags:"},
    {IDFill,           "Fill:"},
    {IDEnhanced,       "Enhanced:"},
    {IDSizeMode,       "SIZE-MODE:"},
    {IDUnknown, "\0"} /* this must be placed last as it is also a end-of-array marker*/
};

ManifestRevision_t Revision = {0};

extern const char *BAMUnitString;

/***********************************************************************
 * Declaration of file local functions
 **********************************************************************/
static char *Do_FindToken(const char *Buffer_p, const  uint32 Bufferlength, uint32 *const Length);
static tokenid Do_LookUpTokens(const char *const Token_p, const uint32 TokenLength);
static uint32 FindId(const char *Buffer_p, const uint32 Length);
C_(void TestFoundText(char *Start, uint32 Length);)
B_(void PrintManiFestEntries(ManifestParser_t *ManifestHandle_pp, uint32 NoOfFiles);)

ErrorCode_e
Do_ManifestParser_Create(char                     *FileData_p,
                         uint32                    FileSize,
                         ManifestParser_t **const ManifestParser_pp,
                         uint32                   *NoOfFiles_p,
                         uint32                   MaxNrOfEntries)
{
    ErrorCode_e ReturnValue = E_SUCCESS;
    ManifestParser_t *ManifestHandle_p = NULL;
    char *FileDataEnd_p = FileData_p + FileSize;
    char *Current_p = FileData_p;
    uint32 TextLength = 0;
    char *Token_p = NULL;
    uint32 TokenLength = 0;
    tokenid TokenId = IDUnknown;

    VERIFY(NULL != FileData_p, E_INVALID_INPUT_PARAMETER);

    C_(printf("manifest_parser.c: FileData: 0x%x FileSize: 0x%x FileDataEnd: 0x%x\n", FileData_p, FileSize, FileDataEnd_p);)

    ManifestHandle_p = (ManifestParser_t *)malloc(sizeof(ManifestParser_t) * MaxNrOfEntries);
    ASSERT(NULL != ManifestHandle_p);

    memset(ManifestHandle_p, 0, sizeof(ManifestParser_t)*MaxNrOfEntries);

    *ManifestParser_pp = ManifestHandle_p;
    *NoOfFiles_p = 0;

    while (Current_p != NULL && Current_p < FileDataEnd_p) {
        Current_p = Do_FindToken(Current_p, FileDataEnd_p - Current_p, &TextLength);

        /*Save the Token and the length to do lookup later*/
        Token_p = Current_p;
        TokenLength = TextLength;

        if (Current_p != NULL) {
            Current_p = Current_p + TextLength;
            Current_p = Do_FindToken(Current_p, FileDataEnd_p - Current_p, &TextLength);
        }

        C_(printf("manifest_parser.c: Token_p: 0x%x TokenLength: 0x%x Current_p: 0x%x TextLength: 0x%x\n", Token_p, TokenLength, Current_p, TextLength);)

        if ((NULL == Token_p) || (NULL == Current_p)) {
            /* no need to continue if id or data is not found */
            break;
        }

        TokenId = Do_LookUpTokens(Token_p, TokenLength);

        switch (TokenId) {
        case IDVersion: {
            if (0 == strncmp(VERSIONNUMBER_STRING, Current_p, TextLength)) {
                C_(printf("manifest_parser.c: Manifest Version ok\n");)
                Revision.RevNrMajor = 2;
                Revision.RevNrMinor = 0;
            } else {
                A_(printf("manifest_parser.c: Error: Can not parse this version\n");)
                return E_GENERAL_FATAL_ERROR;
            }

            break;
        }
        case IDName: {
            (*NoOfFiles_p)++;

            if (*NoOfFiles_p != 1) {
                ManifestHandle_p++;

                if (MaxNrOfEntries < *NoOfFiles_p) {
                    (*NoOfFiles_p)--;
                    break;
                }
            }

            VERIFY(TextLength < MAX_NAME_SIZE, E_FS_NAME_TOO_LONG);
            strncpy(ManifestHandle_p->Name, Current_p, TextLength);
            ManifestHandle_p->Name[TextLength] = '\0';
            break;
        }
        case IDContent: {
            VERIFY(TextLength < MAX_CONTENT_TYPE, E_CONTENT_TYPE);
            strncpy(ManifestHandle_p->ContentType, Current_p, TextLength);
            ManifestHandle_p->ContentType[TextLength] = '\0';
            break;
        }
        case IDTarget: {
            char  *TypeStart  = 0;
            char  *TOC_p      = NULL;
            char  *SubTOC_p   = NULL;
            char  *Comma_p    = NULL;
            uint32 TypeLength = 0;
            uint32 Counter    = 0;

            while ((*(Current_p + Counter) != 0x0a) && (*(Current_p + Counter) != 0x0d)) {
                Counter++;
            }

            *(Current_p + Counter) = '\0';

            TOC_p = strstr(Current_p, "TOC/");

            if (TOC_p != NULL) {
                /* TOC ID found in Target */
                /* Read Target type and Id first */
                TypeStart = GetLastWordn(Current_p, TOC_p - Current_p - 2, &TypeLength);
                VERIFY(TypeLength < MAX_TARGET_TYPE, E_DEVICE_TYPE);
                strncpy(ManifestHandle_p->TargetType, TypeStart, TypeLength);
                ManifestHandle_p->TargetType[TypeLength] = '\0';
                ManifestHandle_p->TargetId = FindId(TypeStart, TypeLength + 1);

                /* Read TOC and SubTOC */
                TOC_p += strlen("TOC/");
                SubTOC_p = strrchr(TOC_p, '/');
                Comma_p  = strrchr(TOC_p, ',');
                VERIFY(Comma_p != NULL, E_GENERAL_FATAL_ERROR);

                if (SubTOC_p != NULL) {
                    /* SubTOC found */
                    SubTOC_p[Comma_p - SubTOC_p] = '\0';
                    strncpy(ManifestHandle_p->TocID, SubTOC_p + 1, MAX_TOCID_SIZE - 1);
                    ManifestHandle_p->TocID[MAX_TOCID_SIZE - 1] = '\0';

                    TOC_p[SubTOC_p - TOC_p] = '\0';
                    strncpy(ManifestHandle_p->SubTOC_Of, TOC_p, MAX_TOCID_SIZE - 1);
                    ManifestHandle_p->SubTOC_Of[MAX_TOCID_SIZE - 1] = '\0';
                } else {
                    /* Only TOC found in Path */
                    TOC_p[Comma_p - TOC_p] = '\0';
                    strncpy(ManifestHandle_p->TocID, TOC_p, MAX_TOCID_SIZE - 1);
                    ManifestHandle_p->TocID[MAX_TOCID_SIZE - 1] = '\0';
                }

                /* Address should be ignored in this case!? */
                Current_p = Current_p + TextLength;
                Current_p = Do_FindToken(Current_p, FileDataEnd_p - Current_p, &TextLength);
                ManifestHandle_p->TargetStartAddress = strtoull(Current_p, 0, 16);
                //    ManifestHandle_p->TargetStartAddress = 0xFFFFFFFF;
            } else {
                char *BootAreaEntry_p = NULL;
                BootAreaEntry_p = strstr(Current_p, BAMUnitString);

                // Image to be written in boot area other than BootImages
                // The following will make sure the image is treated as a BAM image
                if (BootAreaEntry_p != NULL && BootAreaEntry_p - Current_p + strlen(BAMUnitString) + 1 < TextLength) {
                    TypeStart = BootAreaEntry_p;
                    TypeLength = strlen(BAMUnitString) + 1;
                } else {
                    TypeStart = GetLastWordn(Current_p, TextLength, &TypeLength);
                }

                VERIFY(TypeLength < MAX_TARGET_TYPE, E_DEVICE_TYPE);
                strncpy(ManifestHandle_p->TargetType, TypeStart, TypeLength);
                ManifestHandle_p->TargetType[TypeLength] = '\0';

                ManifestHandle_p->TargetId = FindId(Current_p, TextLength); // This will get the flash device id and not the boot device id as previously done

                /* Here Do_FindToken must be called again as this is special and have two
                 * data-items on one line */
                Current_p = Current_p + TextLength;
                Current_p = Do_FindToken(Current_p, FileDataEnd_p - Current_p, &TextLength);

                if ((Current_p + TextLength) == FileDataEnd_p) {
                    char *Temp_p = NULL;

                    Temp_p = (char *)malloc(TextLength + 1);
                    ASSERT(NULL != Temp_p);

                    memcpy(Temp_p, Current_p, TextLength);
                    Temp_p[TextLength] = '\n';

                    ManifestHandle_p->TargetStartAddress = strtoull(Temp_p, 0, 16);

                    BUFFER_FREE(Temp_p);
                } else {
                    ManifestHandle_p->TargetStartAddress = strtoull(Current_p, 0, 16);
                }
            }

            break;
        }
        case IDTocID: {
            VERIFY(TextLength < MAX_TOCID_SIZE, E_CONTENT_TYPE);
            strncpy(ManifestHandle_p->TocID, Current_p, TextLength);
            ManifestHandle_p->TocID[TextLength] = '\0';
            break;
        }
        case IDTocLoadAddress: {
            ManifestHandle_p->TocLoadAddress = strtoul(Current_p, 0, 16);
            break;
        }
        case IDTocEntryPoint: {
            ManifestHandle_p->TocEntryPoint = strtoul(Current_p, 0, 16);
            break;
        }
        case IDTocFlags: {
            ManifestHandle_p->TocFlags = strtoul(Current_p, 0, 16);
            break;
        }
        case IDFill: {
            ManifestHandle_p->Fill.Set = TRUE;
            ManifestHandle_p->Fill.Pattern = strtoul(Current_p, 0, 16);
            break;
        }
        case IDEnhanced: {
            VERIFY(TextLength < MAX_NAME_SIZE, E_FS_NAME_TOO_LONG);

            if (0 == strncmp("TRUE", Current_p, TextLength)) {
                ManifestHandle_p->Enhanced = TRUE;
            }

            break;
        }
        case IDSizeMode: {
            VERIFY(TextLength < MAX_CONTENT_TYPE, E_SIZEMODE_UNKNOWN);
            strncpy(ManifestHandle_p->SizeMode, Current_p, TextLength);
            ManifestHandle_p->SizeMode[TextLength] = '\0';

            break;
        }

        //      case IDSize:
        //      {
        //        ManifestHandle_p->JunkedAreaSize = strtoul(Current_p, 0, 16);
        //        break;
        //      }

        case IDUnknown:
        default: {
            B_(printf("manifest_parser.c: Id not found or not understood\n");)
            break;
        }
        }

        Current_p = Current_p + TextLength;
    }

    B_(PrintManiFestEntries(*ManifestParser_pp, *NoOfFiles_p);)

ErrorExit:
    return ReturnValue;
}

ErrorCode_e
Do_ManifestParser_Destroy(ManifestParser_t **const ManifestParser_pp)
{
    ErrorCode_e ReturnValue = E_SUCCESS;

    VERIFY(NULL != ManifestParser_pp, E_INVALID_INPUT_PARAMETER);
    ASSERT(NULL != *ManifestParser_pp);

    BUFFER_FREE(*ManifestParser_pp);

ErrorExit:
    return ReturnValue;
}

void GetManifestRevision(ManifestRevision_t *ManifestRevision_p)
{
    ManifestRevision_p->RevNrMajor = Revision.RevNrMajor;
    ManifestRevision_p->RevNrMinor = Revision.RevNrMinor;
}

/***********************************************************************
 * Definition of internal functions
 **********************************************************************/
static char *Do_FindToken(const char *Buffer_p, const  uint32 Bufferlength, uint32 *const Length)
{
    const char *Data_p = Buffer_p;
    const char *Start = NULL;
    char *ReturnValue = NULL;

    *Length = 0;

    while (Data_p < (Buffer_p + Bufferlength)) {
        if ((*(Data_p) != 0x0A) && (*(Data_p) != 0x0D) && (*(Data_p) != ' ') && (*(Data_p) != ',') && (*(Data_p) != 0)) {
            Start = Data_p;
            break;
        }

        Data_p++;
    }

    if (Start != NULL) {
        while (Data_p < (Buffer_p + Bufferlength)) {
            if ((*(Data_p) == 0x0A) || (*(Data_p) == 0x0D) || (*(Data_p) == ' ') || (*(Data_p) == ',') || (*(Data_p) == 0)) {
                break;
            }

            Data_p++;
        }

        *Length = Data_p - Start;
        ReturnValue = (char *)Start;
    }

    C_(TestFoundText(ReturnValue, *Length);)

    return ReturnValue;
}

static tokenid Do_LookUpTokens(const char *const Token_p, const uint32 TokenLength)
{
    tokenid ReturnValue = IDUnknown;
    uint32 index = 0;

    if (NULL == Token_p || 0 == TokenLength) {
        return ReturnValue;
    }

    while (ManifestTokens[index].entryID != IDUnknown) {
        if (0 == strncmp(ManifestTokens[index].entryText, Token_p, TokenLength)) {
            ReturnValue = ManifestTokens[index].entryID;
            break;
        }

        index++;
    }

    return ReturnValue;
}

static uint32 FindId(const char *Buffer_p, const uint32 Length)
{
    const char *Temp_p = Buffer_p;

    while ((!isdigit(*Temp_p)) && (Temp_p <= (Buffer_p + Length))) {
        Temp_p++;
    }

    return(uint32) atoi(Temp_p);
}

C_(void TestFoundText(char *Start, uint32 Length)
{
    char temp[50];

    printf("TestFoundText: Start: 0x%x, Length: 0x%x\n", Start, Length);

    if (Start != NULL) {
        strncpy(temp, Start, Length);
    }

    temp[Length] = 0;

    printf("*%s*\n", temp);
})

B_(
    void PrintManiFestEntries(ManifestParser_t *ManifestHandle_p, uint32 NoOfFiles)
{
    uint32 index = 0;

    printf("PrintManiFestEntries %d\n", NoOfFiles - 1);
    printf("----------------------------------------------------------\n", NoOfFiles - 1);

    while (index < NoOfFiles) {
        printf("PrintManiFestEntry %d (of %d)\n", index, NoOfFiles - 1);

        printf("Name:               %s\n", ManifestHandle_p->Name);
        printf("ContentType:        %s\n", ManifestHandle_p->ContentType);
        printf("TargetType:         %s\n", ManifestHandle_p->TargetType);
        printf("SizeMode:           %s\n", ManifestHandle_p->SizeMode);
        printf("TargetId:           %d\n", ManifestHandle_p->TargetId);
        printf("TargetStartAddress: 0x%08X%08X\n", (uint32)(ManifestHandle_p->TargetStartAddress >> 32), (uint32)(ManifestHandle_p->TargetStartAddress));
        printf("TocID:              %s\n", ManifestHandle_p->TocID);
        printf("TocLoadAddress:     0x%08X\n", ManifestHandle_p->TocLoadAddress);
        printf("TocEntryPoint:      0x%08X\n", ManifestHandle_p->TocEntryPoint);
        printf("TocFlags:           0x%08X\n", ManifestHandle_p->TocFlags);
        printf("Fill:               0x%08X\n", ManifestHandle_p->Fill);


        printf("----------------------------------------------------------\n", NoOfFiles - 1);
        index++;
        ManifestHandle_p++;
    }
}
)

/* @} */
/* @} */
