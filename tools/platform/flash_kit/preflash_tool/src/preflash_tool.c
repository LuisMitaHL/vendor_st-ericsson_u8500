/*******************************************************************************
 * $Copyright ST-Ericsson 2010 $
 ******************************************************************************/
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <string.h>
#include <stdio.h>
#include "r_zip_parser.h"
#include "r_debug_macro.h"
#include "r_memory_utils.h"
#include "file_handler.h"
#include "gdf_converter.h"
#include "preflash_tool.h"
#include "preflash_utils.h"

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/
/* Number of TOC entries in ROOT TOC for flash layout version 3.0. */
#define ROOT_TOC_ENTRIES 2
/* Number of copies of the Root TOC for flash layout version 3.0. */
#define ROOT_TOC_COPIES_NUMBER 2
/* Number of copies of the Boot Block for flash layout version 3.0. */
#define BOOT_BLOCK_COPIES_NUMBER 2
/* Major revision number for flash layout version 3.0. */
#define MAJOR_REVISION_FLASHLAYOUT_3_0  3
/* Minor revision number for flash layout version 3.0. */
#define MINOR_REVISION_FLASHLAYOUT_3_0  0
/*File 2 contains data from original bin file from start address [4MB -0x20000]*/
#define START_ADDRESS_SPLIT_IMAGE_FILE_2  0x3E0000
/* File 1 contains data from original bin file from address 0x0 to 0x40000*/
#define SIZE_SPLIT_IMAGE_FILE_1  0x40000
/* File 2 contains 0.5MB data from original bin file from address START_ADDRESS_SPLIT_IMAGE_FILE_2*/
#define SIZE_SPLIT_IMAGE_FILE_2  0x80000

const char *ArchiveString       = "archive.zip";
const char *ContentsString      = "contents.zip";
const char *ManifestFileName    = "manifest.txt";
const char *FlashlayoutFileName = "flashlayout.txt";

const char *ContentTypeBootImageString       = "x-empflash/bootimage";
const char *ContentTypeImageString           = "x-empflash/image";
const char *ContentTypeSparseImageString     = "x-steflash/sparse-image";
const char *ContentTypeEmptyPartitionString  = "x-empflash/initilized-area";
const char *ContentTypeFlashLayoutString     = "x-empflash/flash-layout";

const char *BDMUnitString = "bdm";
const char *BAMUnitString = "boot";
const char *PartSizeString = "PartitionSize";

static const char *SkipTypes[] = {
    "x-empflash/flash-layout",
    NULL // array terminator
};

Communication_t GlobalCommunication;

/*******************************************************************************
 * Declaration of file local functions
 ******************************************************************************/
static LocalParameters_t *CreatePreflashParameters(PreflashType_t PreflashType);
static ErrorCode_e OpenAndParseFlashArchive(char *const ArchiveName_p, LocalParameters_t *const Params_p);
static ErrorCode_e OpenGDFAndCreateCSPSA(char *const FileName_p, LocalParameters_t *const Params_p);
static ErrorCode_e CreatePreflashImage(LocalParameters_t *const Params_p);
static void Cleanup(LocalParameters_t *const Params_p);

static ErrorCode_e ParseZip(const IO_Handle_t IOHandle, void **const ZipHandle_pp);
static ErrorCode_e GetManifestParser(void *const ZipHandle_p, ManifestParser_t **const ManifestParser_pp, uint32 *const NoOfImages_p);
static ErrorCode_e GetFlashLayoutParser(void *const ZipHandle_p, FlashLayoutParser_t **const FlashLayout_pp, uint32 *const NoOfImages_p, FlashLayoutRevision_t *FlashLayoutRevision_p);
static void CheckForSubTOC(FlashLayoutParser_t *const FlashLayoutParser_p, const uint32 ImagesInFlashLayout, char *const SubTOCID_p, uint32 *const SubTOCStartAddress_p, uint32 *const SubTOCSize_p, uint8 *SubTOCFlash_p);
static ErrorCode_e CompatibilityCheck(ManifestParser_t *const ManifestParser_p, const uint32 ImagesInManifest, FlashLayoutParser_t *const FlashLayoutParser_p, const uint32 ImagesInFlashLayout, uint8 *const BAMFlash_p);
static boolean IsContentTypeSkippable(const char *Type_p);

static uint8 GetDestinationFlashes(ManifestParser_t *const ManifestParser_p, const uint32 ImagesInManifest);
static void GetNoOfTOCEntries(ManifestParser_t *const ManifestParser_p, const uint32 ImagesInManifest, uint8 *const NoOfTOCEntries_p, uint8 *const NoOfSubTOCEntries_p);
static ErrorCode_e WriteBAM(LocalParameters_t *const Params_p, const uint8 TOCEntries, const uint8 SubTOCEntries, const IO_Handle_t OutputIOHandle_1, const IO_Handle_t OutputIOHandle_2);
static ErrorCode_e WriteBAM_Layout_2_0(LocalParameters_t *const Params_p, const uint8 TOCEntries, const uint8 SubTOCEntries, const IO_Handle_t OutputIOHandle);
static ErrorCode_e WriteBAM_Layout_3_0(LocalParameters_t *const Params_p, const uint8 TOCEntries, const uint8 SubTOCEntries, const IO_Handle_t OutputIOHandle_1, const IO_Handle_t OutputIOHandle_2);
static ErrorCode_e WriteBDM(LocalParameters_t *const Params_p, const uint8 SubTOCEntries, const IO_Handle_t OutputIOHandle, const uint8 CurrentFlash);
static void BuildTOC_Layout_2_0(LocalParameters_t *const Params_p, const uint32 TOCStartAddress, const uint8 NoOfSubTOCEntries, uint8 *const TOCData_p);
static void BuildSubTOC_Layout_2_0(LocalParameters_t *const Params_p, uint8 *const SubTOCData_p);
static void BuildTOCandSubTOC_Layout_3_0(LocalParameters_t *const Params_p, const uint32 TOCStartAddress, const uint8 NoOfSubTOCEntries, uint32 BootBlockOffset, uint8 *const TOCData_p, uint8 *const SubTOCData_p);
static uint64 GetLengthOfFileInArchive(void *const ZipHandle_p, char *const Name_p);
static ErrorCode_e WriteToImageSrecord(const uint32 StartAddress, const IO_Handle_t OutputIOHandle, uint8 *const Data_p, const uint64 DataLength);
static ErrorCode_e WriteToImageBinary(uint32 StartAddress, const IO_Handle_t OutputIOHandle, uint8 *const Data_p, const uint64 DataLength);
static ErrorCode_e WriteBinToImage(LocalParameters_t *const Params_p, char *const Name_p, const uint32 Address, const IO_Handle_t OutputIOHandle);
static ErrorCode_e SplitBinaryFile(char *FileName_1, char *FileName_2);
static ErrorCode_e UpdateSparseImagesParameters(ManifestParser_t *const ManifestParser_p, const uint32 ImagesInManifest, FlashLayoutParser_t *const FlashLayoutParser_p, const uint32 ImagesInFlashLayout);
static boolean IsFirstEntryInTOC_Partition(ManifestParser_t *const ManifestParser_p, const uint32 ImagesInManifest, char *TocID, uint32 TargetStartAddress);
static boolean IsLastEntryInTOC_Partition(ManifestParser_t *const ManifestParser_p, const uint32 ImagesInManifest, char *TocID, uint32 TargetStartAddress);

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/
int main(int argc, char *argv[])
{
    Command_t *Command_p = NULL;
    int RetVal = -1;

    if (1 == argc) {
        printf("Input parameters missing\n");
        PrintCmdProperties();
        return 0;
    }

    Command_p = ParseArguments(argc, argv);

    if (NULL == Command_p) {
        printf("Error parsing arguments (Status: %d)\n", GetParseStatus());
        return RetVal;
    }

    if (E_OK != CheckCommandParameters(Command_p)) {
        printf("Input parameters error!\n");
        PrintCmdProperties();
        FreeCommandData(&Command_p);
        return RetVal;
    }

    RetVal = (int)ExecuteCommand(Command_p);

    if (0 != RetVal) {
        printf("Failed to execute command! ERROR: %d\n", RetVal);
        PrintCmdProperties();
    }

    FreeCommandData(&Command_p);
    return RetVal;
}

int CommandPreFlash(Command_t *Command_p)
{
    int Result = -1;
    LocalParameters_t *Params_p = NULL;
    char *Gdfs_File_Path = NULL;
    char *Zip_Archive_Path = NULL;

    Params_p = CreatePreflashParameters(PREFLASH_IMAGE);

    if (NULL == Params_p) {
        printf("Cannot create preflash initialization parameters!\n");
        goto ErrorExit;
    }

    (void)Do_IO_Init(16);

    Zip_Archive_Path = GetSwitchParam(Command_p, "-a", 0);

    if (NULL == Zip_Archive_Path) {
        printf("Path to Flash archive not specified!\n");
        goto ErrorExit;
    }

    if (E_SUCCESS != OpenAndParseFlashArchive(Zip_Archive_Path, Params_p)) {
        printf("OpenAndParseFlashArchive FAILED!\n");
        goto ErrorExit;
    }

    if (IsSwitchInInputList(Command_p, "-s") || IsSwitchInInputList(Command_p, "--skipfirstblock")) {
        printf("Output image wouldn't contain data for first boot block!\n");
        Params_p->SkipFirstBlock = TRUE;
    }

    if (TRUE == IsSwitchInInputList(Command_p, "-b")) {
        printf("Output image will be generated in binary format!\n");
        Params_p->GenerateBinary = TRUE;
    }

    Gdfs_File_Path = GetSwitchParam(Command_p, "-gd", 0);

    if (NULL != Gdfs_File_Path) {
        if (E_SUCCESS != OpenGDFAndCreateCSPSA(Gdfs_File_Path, Params_p)) {
            printf("OpenGDFAndCreateCSPSA FAILED!\n");
            goto ErrorExit;
        }
    } else {
        printf("GDFS file not specified!\n");
    }

    if (E_SUCCESS != CreatePreflashImage(Params_p)) {
        printf("CreatePreflashImage FAILED!\n");
        goto ErrorExit;
    }

    Result = 0;
ErrorExit:
    Cleanup(Params_p);
    BUFFER_FREE(Params_p);

    return Result;
}

int CommandTocGen(Command_t *Command_p)
{
    int Result = -1;
    LocalParameters_t *Params_p = NULL;
    char *Zip_Archive_Path = NULL;

    Params_p = CreatePreflashParameters(TOC_IMAGE);

    if (NULL == Params_p) {
        printf("Cannot create preflash initialization parameters!\n");
        goto ErrorExit;
    }

    (void)Do_IO_Init(16);

    Zip_Archive_Path = GetSwitchParam(Command_p, "-a", 0);

    if (NULL == Zip_Archive_Path) {
        printf("Path to Flash archive not specified!\n");
        goto ErrorExit;
    }

    if (E_SUCCESS != OpenAndParseFlashArchive(Zip_Archive_Path, Params_p)) {
        printf("OpenAndParseFlashArchive FAILED!\n");
        goto ErrorExit;
    }

    //First block in boot area should be always skipped in case of tocgen image for flashlayout version 2.0
    Params_p->SkipFirstBlock = TRUE;

    if (TRUE == IsSwitchInInputList(Command_p, "-b")) {
        printf("Output image will be generated in binary format!\n");
        Params_p->GenerateBinary = TRUE;
    }

    if (IsSwitchInInputList(Command_p, "-f") || IsSwitchInInputList(Command_p, "--splitoutputfile")) {
        printf("Output image will be split in two files!\n");
        Params_p->SplitOutputImage = TRUE;
    }

    if (E_SUCCESS != CreatePreflashImage(Params_p)) {
        printf("CreatePreflashImage FAILED!\n");
        goto ErrorExit;
    }

    Result = 0;
ErrorExit:
    Cleanup(Params_p);
    BUFFER_FREE(Params_p);

    return Result;
}

int CommandHelp(Command_t *Command_p)
{
    PrintCmdProperties();
    return 0;
}

/*******************************************************************************
 * Definition of internal functions
 ******************************************************************************/
static LocalParameters_t *CreatePreflashParameters(PreflashType_t PreflashType)
{
    LocalParameters_t *Param_p = NULL;

    Param_p = (LocalParameters_t *)malloc(sizeof(LocalParameters_t));
    ASSERT(NULL != Param_p);

    Param_p->ZipIOHandle = IO_INVALID_HANDLE;
    Param_p->ZipInZipIOHandle = IO_INVALID_HANDLE;
    Param_p->ZipHandle_p = NULL;
    Param_p->ZipInZipHandle_p = NULL;
    Param_p->ManifestParser_p = NULL;
    Param_p->ImagesInManifest = 0;
    Param_p->FlashLayoutParser_p = NULL;
    Param_p->ImagesInFlashLayout = 0;
    Param_p->SubTOCID[0] = '\0';
    Param_p->SubTOCStartAddress = 0;
    Param_p->SubTOCSize = 0;
    Param_p->CSPSAHandle = NULL;
    Param_p->BAMFlash = 0;
    Param_p->SubTOCFlash = 0;
    Param_p->GenerateBinary = FALSE;
    Param_p->SkipFirstBlock = FALSE;
    Param_p->PreflashType = PreflashType;
    Param_p->Flashlayout_3_0 = FALSE;

    return Param_p;
}

static ErrorCode_e OpenAndParseFlashArchive(char *const ArchiveName_p, LocalParameters_t *const Params_p)
{
    FlashLayoutRevision_t FlashLayoutRevision = {0};

    printf("Parsing input file: '%s'...\n", ArchiveName_p);

    // Open zip(flash archive)
    if (FileOpen(ArchiveName_p, "rb", &Params_p->ZipIOHandle) != E_SUCCESS) {
        printf("Open flash archive FAILED!\n");
        return E_GENERAL_FATAL_ERROR;
    }

    // Parse zip
    if (ParseZip(Params_p->ZipIOHandle, &Params_p->ZipHandle_p) != E_SUCCESS) {
        printf("Parse zip FAILED!\n");
        return E_GENERAL_FATAL_ERROR;
    }

    // Open zip in zip
    if (Do_Zip_OpenFile(Params_p->ZipHandle_p, ContentsString, strlen(ContentsString), &Params_p->ZipInZipIOHandle) != E_SUCCESS) {
        printf("Do_Zip_OpenFile:'%s' FAILED!\n", ContentsString);

        if (Do_Zip_OpenFile(Params_p->ZipHandle_p, ArchiveString, strlen(ArchiveString), &Params_p->ZipInZipIOHandle) != E_SUCCESS) {
            printf("Do_Zip_OpenFile:'%s' FAILED!\n", ArchiveString);
            return E_GENERAL_FATAL_ERROR;
        }
    }

    // Parse zip in zip
    if (ParseZip(Params_p->ZipInZipIOHandle, &Params_p->ZipInZipHandle_p) != E_SUCCESS) {
        printf("Parse zip in zip FAILED!\n");
        return E_GENERAL_FATAL_ERROR;
    }

    // Create manifest parser
    if (GetManifestParser(Params_p->ZipInZipHandle_p, &Params_p->ManifestParser_p, &Params_p->ImagesInManifest) != E_SUCCESS) {
        printf("GetManifestParser FAILED!\n");
        return E_GENERAL_FATAL_ERROR;
    }

    // Create flash layout parser
    if (GetFlashLayoutParser(Params_p->ZipInZipHandle_p, &Params_p->FlashLayoutParser_p, &Params_p->ImagesInFlashLayout, &FlashLayoutRevision) != E_SUCCESS) {
        printf("GetFlashLayoutParser FAILED!\n");
        return E_GENERAL_FATAL_ERROR;
    }

    // Compatibility check
    if (CompatibilityCheck(Params_p->ManifestParser_p, Params_p->ImagesInManifest, Params_p->FlashLayoutParser_p, Params_p->ImagesInFlashLayout, &Params_p->BAMFlash) != E_SUCCESS) {
        printf("CompatibilityCheck FAILED!\n");
        return E_GENERAL_FATAL_ERROR;
    }

    if ((MAJOR_REVISION_FLASHLAYOUT_3_0 == FlashLayoutRevision.RevNrMajor) && (MINOR_REVISION_FLASHLAYOUT_3_0 == FlashLayoutRevision.RevNrMinor)) {
        Params_p->Flashlayout_3_0 = TRUE;
    }

    // FIXED TOC is generated in both tools TOC-gen and Pre-flash for flash
    // layout version 3.0 because SUBTOC is needed to generate FIXED TOC image.
    if ((PREFLASH_IMAGE == Params_p->PreflashType) || (TRUE == Params_p->Flashlayout_3_0)) {
        CheckForSubTOC(Params_p->FlashLayoutParser_p, Params_p->ImagesInFlashLayout, &Params_p->SubTOCID[0], &Params_p->SubTOCStartAddress, &Params_p->SubTOCSize, &Params_p->SubTOCFlash);
    }

    return E_SUCCESS;
}

static ErrorCode_e OpenGDFAndCreateCSPSA(char *const FileName_p, LocalParameters_t *const Params_p)
{
    uint32 i;
    uint32 Size = 0;
    ErrorCode_e Result = E_SUCCESS;
    IO_Handle_t IOHandle = IO_INVALID_HANDLE;
    GDDataNode_t *Root_p = NULL;
    ManifestParser_t *ManifestTemp_p;

    // First check that CSPSA exists in manifest
    for (i = 0, ManifestTemp_p = Params_p->ManifestParser_p; i < Params_p->ImagesInManifest; i++, ManifestTemp_p++) {
        if (strncmp(ManifestTemp_p->TocID, "CSPSA", 5) == 0) {
            Size = ManifestTemp_p->PartitionContentSize;
            break;
        }
    }

    if (0 == Size) {
        printf("CSPSA is not included in manifest!\n");
        return E_GENERAL_FATAL_ERROR;
    }

    printf("Parsing GDF file: '%s'...\n", FileName_p);

    if (FileOpen(FileName_p, "r", &IOHandle) != E_SUCCESS) {
        printf("Open GDF FAILED!\n");
        return E_GENERAL_FATAL_ERROR;
    }


    Result = GetGDFList(IOHandle, &Root_p);

    if (E_SUCCESS == Result) {
        GDDataNode_t *Iter_p = Root_p;

        Result = InitializeCSPSA(Size, &Params_p->CSPSAHandle);

        printf("Writing parameters to CSPSA...\n");

        while (Iter_p && E_SUCCESS == Result) {
            GDData_t *GDData_p = Iter_p->GDData_p;

            Result = WriteCSPSA(Params_p->CSPSAHandle, GDData_p->Index, GDData_p->DataSize, GDData_p->Data);

            Iter_p = Iter_p->Next_p;
        }

        if (E_SUCCESS == Result) {
            Result = FlushCSPSA(Params_p->CSPSAHandle);
        }

        DestroyGDFList(&Root_p);
    }

    (void)Do_IO_Close(IOHandle);

    return Result;
}

static ErrorCode_e CreatePreflashImage(LocalParameters_t *const Params_p)
{
    ErrorCode_e Result = E_SUCCESS;
    uint8 CurrentFlash = 0;
    uint8 FlashMask = GetDestinationFlashes(Params_p->ManifestParser_p, Params_p->ImagesInManifest);

    for (CurrentFlash = 0; FlashMask; CurrentFlash++) {
        uint8 TOCEntries = 0;
        uint8 SubTOCEntries = 0;

        if (!((FlashMask >> CurrentFlash) & 0x01)) {
            continue;
        }

        FlashMask &= (FlashMask - 1);

        GetNoOfTOCEntries(Params_p->ManifestParser_p, Params_p->ImagesInManifest, &TOCEntries, &SubTOCEntries);

        if (0 < TOCEntries) {
            char FileName_1[20];
            IO_Handle_t OutputIOHandle_1 = IO_INVALID_HANDLE;
            char FileName_2[20];
            IO_Handle_t OutputIOHandle_2 = IO_INVALID_HANDLE;

            // Create output file
            if (PREFLASH_IMAGE == Params_p->PreflashType) {
                snprintf(FileName_1, 20, "preflash%u.bin", CurrentFlash);
            } else {
                if (FALSE == Params_p->SplitOutputImage) {
                    snprintf(FileName_1, 20, "bootblock%u.bin", CurrentFlash);
                } else {
                    snprintf(FileName_1, 20, "bootblock%u_1.bin", CurrentFlash);
                    snprintf(FileName_2, 20, "bootblock%u_2.bin", CurrentFlash);
                }
            }

            if (E_SUCCESS != FileOpen(FileName_1, "wb", &OutputIOHandle_1)) {
                printf("Error creating output file!\n");
                return E_GENERAL_FATAL_ERROR;
            }

            printf("Created '%s' for flash%u\n", FileName_1, CurrentFlash);

            if (Params_p->SplitOutputImage) {
                if (E_SUCCESS != FileOpen(FileName_2, "wb", &OutputIOHandle_2)) {
                    printf("Error creating output file!\n");
                    return E_GENERAL_FATAL_ERROR;
                }

                printf("Created '%s' for flash%u\n", FileName_2, CurrentFlash);
            }

            // Write S0 srec
            if (FALSE == Params_p->GenerateBinary) {
                Result = WriteSRec0ToImage(OutputIOHandle_1);

                if (E_SUCCESS == Result && Params_p->SplitOutputImage) {
                    Result = WriteSRec0ToImage(OutputIOHandle_2);
                }
            }

            // Write BAM
            if (E_SUCCESS == Result && Params_p->BAMFlash == CurrentFlash) {
                if (TRUE == Params_p->SplitOutputImage && FALSE == Params_p->GenerateBinary) {
                    Result = WriteBAM(Params_p, TOCEntries, SubTOCEntries, OutputIOHandle_1, OutputIOHandle_2);
                } else {
                    Result = WriteBAM(Params_p, TOCEntries, SubTOCEntries, OutputIOHandle_1, OutputIOHandle_1);
                }
            }

            // Write BDM
            if ((E_SUCCESS == Result) && (PREFLASH_IMAGE == Params_p->PreflashType)) {
                Result = WriteBDM(Params_p, SubTOCEntries, OutputIOHandle_1, CurrentFlash);
            }

            // Write S9 srec
            if ((E_SUCCESS == Result) && (FALSE == Params_p->GenerateBinary)) {
                Result = WriteSRec9ToImage(OutputIOHandle_1);

                if (E_SUCCESS == Result && Params_p->SplitOutputImage) {
                    Result = WriteSRec9ToImage(OutputIOHandle_2);
                }
            }

            (void)Do_IO_Close(OutputIOHandle_1);

            if (Params_p->SplitOutputImage) {
                (void)Do_IO_Close(OutputIOHandle_2);
            }

            //Split binary file
            if (TRUE == Params_p->GenerateBinary && TRUE == Params_p->SplitOutputImage) {
                Result = SplitBinaryFile(FileName_1, FileName_2);
            }
        }
    }

    return Result;
}

static void Cleanup(LocalParameters_t *const Params_p)
{
    if (Params_p->FlashLayoutParser_p != NULL) {
        (void)Do_FlashLayoutParser_Destroy(&Params_p->FlashLayoutParser_p);
    }

    if (Params_p->ManifestParser_p != NULL) {
        (void)Do_ManifestParser_Destroy(&Params_p->ManifestParser_p);
    }

    if (Params_p->ZipInZipHandle_p != NULL) {
        (void)Do_Zip_DestroyParserContext(&Params_p->ZipInZipHandle_p);
    }

    if (Params_p->ZipInZipIOHandle != IO_INVALID_HANDLE) {
        (void)Do_IO_Close(Params_p->ZipInZipIOHandle);
    }

    if (Params_p->ZipHandle_p != NULL) {
        (void)Do_Zip_DestroyParserContext(&Params_p->ZipHandle_p);
    }

    if (Params_p->ZipIOHandle != IO_INVALID_HANDLE) {
        (void)Do_IO_Close(Params_p->ZipIOHandle);
    }

    if (Params_p->CSPSAHandle != NULL) {
        (void)DestroyCSPSA(&Params_p->CSPSAHandle);
    }
}

static ErrorCode_e ParseZip(const IO_Handle_t IOHandle, void **const ZipHandle_pp)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    ZipState_t ZipState = ZIP_INIT_EOCDR;

    if (IOHandle != IO_INVALID_HANDLE) {
        do {
            ReturnValue = Do_Zip_CreateParserContext(IOHandle, ZipHandle_pp, &ZipState, ZIP_USE_IO_WRAP, NULL);

        } while (E_SUCCESS == ReturnValue && ZipState < ZIP_FINISHED);
    }

    return ReturnValue;
}

static ErrorCode_e GetManifestParser(void *const ZipHandle_p, ManifestParser_t **const ManifestParser_pp, uint32 *const NoOfImages_p)
{
    IO_Handle_t IOHandle;
    uint64 FileLength;
    char *FileData_p = NULL;
    ErrorCode_e Result = E_SUCCESS;
    uint32 NrOfEntries = 0;

    // Open manifest file in zip
    if (Do_Zip_OpenFile(ZipHandle_p, ManifestFileName, strlen(ManifestFileName), &IOHandle) != E_SUCCESS) {
        printf("Could not open manifest file in zip!\n");
        return E_GENERAL_FATAL_ERROR;
    }

    // Create buffer to store file
    FileLength = Do_IO_GetLength(IOHandle);

    if (FileLength == IO_ERROR) {
        Do_IO_Close(IOHandle);
        return E_GENERAL_FATAL_ERROR;
    }

    FileData_p = (char *)malloc(FileLength);
    ASSERT(NULL != FileData_p);

    // Read manifest to buffer
    if (Do_IO_PositionedRead(IOHandle, FileData_p, FileLength, FileLength, 0) == IO_ERROR) {
        printf("Could not read manifest file!\n");
        Result =  E_GENERAL_FATAL_ERROR;
    }

    // Get total number of entries in inner zip
    if (Do_Zip_GetTotalNrOfEntries(ZipHandle_p, &NrOfEntries) != E_SUCCESS) {
        printf("Could not get total number of entries in inner zip!\n");
        Result = E_GENERAL_FATAL_ERROR;
    }

    // Create manifest parser
    if (Result == E_SUCCESS && Do_ManifestParser_Create(FileData_p, (uint32)FileLength, ManifestParser_pp, NoOfImages_p, NrOfEntries) != E_SUCCESS) {
        printf("Could not create manifest parser!\n");
        Result = E_GENERAL_FATAL_ERROR;
    }

    BUFFER_FREE(FileData_p);
    Do_IO_Close(IOHandle);

    return Result;
}

static ErrorCode_e GetFlashLayoutParser(void *const ZipHandle_p, FlashLayoutParser_t **const FlashLayout_pp, uint32 *const NoOfImages_p, FlashLayoutRevision_t *FlashLayoutRevision_p)
{
    IO_Handle_t IOHandle;
    uint64 FileLength;
    char *FileData_p = NULL;
    ErrorCode_e Result = E_SUCCESS;

    // Open flash layout file in zip
    if (Do_Zip_OpenFile(ZipHandle_p, FlashlayoutFileName, strlen(FlashlayoutFileName), &IOHandle) != E_SUCCESS) {
        printf("Could not open flash layout file in zip!\n");
        return E_GENERAL_FATAL_ERROR;
    }

    // Create buffer to store file
    FileLength = Do_IO_GetLength(IOHandle);

    if (FileLength == IO_ERROR) {
        Do_IO_Close(IOHandle);
        return E_GENERAL_FATAL_ERROR;
    }

    FileData_p = (char *)malloc(FileLength);
    ASSERT(NULL != FileData_p);

    // Read flash layout to buffer
    if (Do_IO_PositionedRead(IOHandle, FileData_p, FileLength, FileLength, 0) == IO_ERROR) {
        printf("Could not read flash layout file!\n");
        Result =  E_GENERAL_FATAL_ERROR;
    }

    // Create flash layout parser
    if (Result == E_SUCCESS && Do_FlashLayoutParser_Create(FileData_p, (uint32)FileLength, FlashLayout_pp, NoOfImages_p) != E_SUCCESS) {
        printf("Could not create flash layout parser!\n");
        Result = E_GENERAL_FATAL_ERROR;
    }

    // Get flash layout revision must be called after Do_FlashLayoutParser_Create() func.
    Do_FlashLayoutParser_GetRevision(FlashLayoutRevision_p);

    BUFFER_FREE(FileData_p);
    Do_IO_Close(IOHandle);

    return Result;
}

static void CheckForSubTOC(FlashLayoutParser_t *const FlashLayoutParser_p, const uint32 ImagesInFlashLayout, char *const SubTOCID_p, uint32 *const SubTOCStartAddress_p, uint32 *const SubTOCSize_p, uint8 *SubTOCFlash_p)
{
    uint8 i;
    FlashLayoutParser_t *FlashLayoutTemp_p = FlashLayoutParser_p;

    for (i = 0; i < ImagesInFlashLayout; i++) {
        if (FlashLayoutTemp_p->SubTOC_Of[0] != '\0') {
            printf("preflash_tool.c(%d): SubTOC ID: %s found in flashlayout!\n", __LINE__, FlashLayoutTemp_p->SubTOC_Of);
            memcpy(SubTOCID_p, FlashLayoutTemp_p->SubTOC_Of, TOC_ID_LENGTH);
            break;
        }

        FlashLayoutTemp_p++;
    }

    if (SubTOCID_p[0] != '\0') {
        FlashLayoutTemp_p = FlashLayoutParser_p;

        for (i = 0; i < ImagesInFlashLayout; i++) {
            if (0 == strncmp(FlashLayoutTemp_p->TOC_ID, SubTOCID_p, TOC_ID_LENGTH)) {
                printf("preflash_tool.c(%d): TOC ID: %s found in flashlayout! Offset 0x%llX\n", __LINE__, SubTOCID_p, FlashLayoutTemp_p->StartAddress);
                *SubTOCStartAddress_p = FlashLayoutTemp_p->StartAddress;
                *SubTOCSize_p = FlashLayoutTemp_p->EndAddress - FlashLayoutTemp_p->StartAddress;
                *SubTOCFlash_p = strtoul(FlashLayoutTemp_p->Path + 6, NULL, 10);
                break;
            }

            FlashLayoutTemp_p++;
        }
    }
}

static ErrorCode_e CompatibilityCheck(ManifestParser_t *const ManifestParser_p, const uint32 ImagesInManifest, FlashLayoutParser_t *const FlashLayoutParser_p, const uint32 ImagesInFlashLayout, uint8 *const BAMFlash_p)
{
    ErrorCode_e          ReturnValue        = E_GENERAL_FATAL_ERROR;
    FlashLayoutParser_t *Flashlayout_Temp_p = NULL;
    ManifestParser_t    *ManifestTemp_p     = NULL;
    boolean              EntryFound         = FALSE;
    uint32               Counter_1          = 0;
    uint32               Counter_2          = 0;

    ManifestTemp_p = ManifestParser_p;

    for (Counter_1 = 1; Counter_1 <= ImagesInManifest; Counter_1++) {
        Flashlayout_Temp_p = FlashLayoutParser_p;
        EntryFound = FALSE;
        Counter_2 = 0;

        if (IsContentTypeSkippable(ManifestTemp_p->ContentType)) {
            ManifestTemp_p++;
            continue;
        }

        do {
            if (0 == strncmp(ManifestTemp_p->TocID, Flashlayout_Temp_p->TOC_ID, MAX_TOC_ID_SIZE)) {
                EntryFound = TRUE;
            } else {
                Flashlayout_Temp_p++;
                Counter_2++;
            }
        } while ((!EntryFound) && (Counter_2 < ImagesInFlashLayout));

        if (!EntryFound) {
            if (GetImageType(ManifestTemp_p) == DEVICE_BAM) {
                ManifestTemp_p++;
                continue;
            }

            printf("preflash_tool.c(%d): TOC_ID %s not found in flashlayout!\n", __LINE__, ManifestTemp_p->TocID);
            ReturnValue = E_ENTRY_NOT_FOUND_IN_FLASHLAYOUT;
            goto ErrorExit;
        }

        if (ManifestTemp_p->TargetStartAddress == 0) {
            if (0 == strncmp(ManifestTemp_p->ContentType, ContentTypeSparseImageString, strlen(ContentTypeSparseImageString))) {
                if (IsLastEntryInTOC_Partition(ManifestParser_p, ImagesInManifest, ManifestTemp_p->TocID, ManifestTemp_p->TargetStartAddress)) {
                    ManifestTemp_p->FlashingFlags |= PARTITION_ENTRY_EXTENSION_LAST;
                } else {
                    ManifestTemp_p->FlashingFlags |= PARTITION_ENTRY_EXTENSION_FIRST;
                }
            } else {
                ManifestTemp_p->TargetStartAddress = Flashlayout_Temp_p->StartAddress;
            }
        } else {
            // Make sure that images start at very beginning of TOC partition (except when they are TOC partition extensions)
            VERIFY(0 == strncmp(ManifestTemp_p->ContentType, ContentTypeSparseImageString, strlen(ContentTypeSparseImageString)), E_MISMATCH_MANIFEST_FLASHLAYOUT);
            VERIFY(!IsFirstEntryInTOC_Partition(ManifestParser_p, ImagesInManifest, ManifestTemp_p->TocID, ManifestTemp_p->TargetStartAddress), E_MISMATCH_MANIFEST_FLASHLAYOUT);

            if (IsLastEntryInTOC_Partition(ManifestParser_p, ImagesInManifest, ManifestTemp_p->TocID, ManifestTemp_p->TargetStartAddress)) {
                ManifestTemp_p->FlashingFlags |= PARTITION_ENTRY_EXTENSION_LAST;
            } else {
                ManifestTemp_p->FlashingFlags |= PARTITION_ENTRY_EXTENSION;
            }
        }

        if (Flashlayout_Temp_p->EndAddress > Flashlayout_Temp_p->StartAddress) {
            ManifestTemp_p->PartitionContentSize = Flashlayout_Temp_p->EndAddress - Flashlayout_Temp_p->StartAddress;
        } else {
            printf("preflash_tool.c(%d): Invalid Start and End address in flashlayout!!\n", __LINE__);
        }

        /* Check some other parameters? */
        ManifestTemp_p++;
    }

    ReturnValue = UpdateSparseImagesParameters(ManifestParser_p, ImagesInManifest, FlashLayoutParser_p, ImagesInFlashLayout);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

ErrorExit:
    return ReturnValue;
}

static ErrorCode_e UpdateSparseImagesParameters(ManifestParser_t *const ManifestParser_p, const uint32 ImagesInManifest, FlashLayoutParser_t *const FlashLayoutParser_p, const uint32 ImagesInFlashLayout)
{
    ErrorCode_e          ReturnValue        = E_GENERAL_FATAL_ERROR;
    FlashLayoutParser_t *Flashlayout_Temp_p = NULL;
    ManifestParser_t    *ManifestTemp_p     = NULL;
    boolean              EntryFound         = FALSE;
    uint32 i = 1;
    uint32 j = 1;

    ManifestTemp_p = ManifestParser_p;

    for (i = 1; i <= ImagesInManifest; i++) {
        Flashlayout_Temp_p = FlashLayoutParser_p;
        j = 1;

        if (IsContentTypeSkippable(ManifestTemp_p->ContentType)) {
            ManifestTemp_p++;
            continue;
        }

        if ((ManifestTemp_p->FlashingFlags & PARTITION_ENTRY_EXTENSION_FIRST) ||
                (ManifestTemp_p->FlashingFlags & PARTITION_ENTRY_EXTENSION_LAST) ||
                (ManifestTemp_p->FlashingFlags & PARTITION_ENTRY_EXTENSION)) {
            do {
                if (0 == strncmp(ManifestTemp_p->TocID, Flashlayout_Temp_p->TOC_ID, MAX_TOC_ID_SIZE)) {
                    ManifestTemp_p->TargetStartAddress += Flashlayout_Temp_p->StartAddress;
                    EntryFound = TRUE;
                    break;
                } else {
                    Flashlayout_Temp_p++;
                    j++;
                }
            } while (j <= ImagesInFlashLayout);

            VERIFY(TRUE == EntryFound, E_MISMATCH_MANIFEST_FLASHLAYOUT);
        }

        ManifestTemp_p++;
    }

    ReturnValue = E_SUCCESS;

ErrorExit:
    return ReturnValue;
}

static boolean IsFirstEntryInTOC_Partition(ManifestParser_t *const ManifestParser_p, const uint32 ImagesInManifest, char *TocID, uint32 TargetStartAddress)
{
    ManifestParser_t *ManifestTemp_p = NULL;
    uint32 i = 1;

    ManifestTemp_p = ManifestParser_p;

    for (i = 1; i <= ImagesInManifest; i++) {
        if (0 == strncmp(ManifestTemp_p->TocID, TocID, MAX_TOC_ID_SIZE)) {
            if (ManifestTemp_p->TargetStartAddress < TargetStartAddress) {
                return FALSE;
            }
        }

        ManifestTemp_p++;
    }

    return TRUE;
}

static boolean IsLastEntryInTOC_Partition(ManifestParser_t *const ManifestParser_p, const uint32 ImagesInManifest, char *TocID, uint32 TargetStartAddress)
{
    ManifestParser_t *ManifestTemp_p = NULL;
    uint32 i = 1;

    ManifestTemp_p = ManifestParser_p;

    for (i = 1; i <= ImagesInManifest; i++) {
        if (0 == strncmp(ManifestTemp_p->TocID, TocID, MAX_TOC_ID_SIZE)) {
            if (ManifestTemp_p->TargetStartAddress > TargetStartAddress) {
                return FALSE;
            }
        }

        ManifestTemp_p++;
    }

    return TRUE;
}

static boolean IsContentTypeSkippable(const char *Type_p)
{
    uint8 i;

    for (i = 0; NULL != SkipTypes[i]; ++i) {
        if (0 == strcmp(Type_p, SkipTypes[i])) {
            return TRUE;
        }
    }

    return FALSE;
}

static uint8 GetDestinationFlashes(ManifestParser_t *const ManifestParser_p, const uint32 ImagesInManifest)
{
    uint32 i;
    uint8 FlashMask = 0;
    ManifestParser_t *ManifestTemp_p = ManifestParser_p;

    for (i = 0; i < ImagesInManifest; i++) {
        FlashMask |= (1 << ManifestTemp_p->TargetId);
        ManifestTemp_p++;
    }

    return FlashMask;
}

static void GetNoOfTOCEntries(ManifestParser_t *const ManifestParser_p, const uint32 ImagesInManifest, uint8 *const NoOfTOCEntries_p, uint8 *const NoOfSubTOCEntries_p)
{
    uint32 i;
    ManifestParser_t *ManifestTemp_p = ManifestParser_p;

    for (i = 0; i < ImagesInManifest; i++) {
        if ((ManifestTemp_p->TocID[0] != '\0') && !((ManifestTemp_p->FlashingFlags & PARTITION_ENTRY_EXTENSION) || (ManifestTemp_p->FlashingFlags & PARTITION_ENTRY_EXTENSION_FIRST))) {
            if (ManifestTemp_p->SubTOC_Of[0] != '\0') {
                (*NoOfSubTOCEntries_p)++;
            } else {
                (*NoOfTOCEntries_p)++;
            }
        }

        ManifestTemp_p++;
    }

    if (*NoOfSubTOCEntries_p > 0) {
        (*NoOfTOCEntries_p)++;
    }
}

static ErrorCode_e WriteBAM(LocalParameters_t *const Params_p, const uint8 TOCEntries, const uint8 SubTOCEntries, const IO_Handle_t OutputIOHandle_1, const IO_Handle_t OutputIOHandle_2)
{
    /*
     * For flash layout version 2.0:
     * -----------------------------
     * FIXEDTOC always contains all entries for boot images.
     * For TOC-gen tool Root TOC has equal contents with FIXEDTOC.
     * For Pre-flash tool Root TOC has equal contents with FIXEDTOC plus one
     * additional entry for SUBTOC.
     * SUBTOC contains entries for images that are specified as SUBTOC entries
     * in manifest.txt file.
     *
     * For flash layout version 3.0:
     * -----------------------------
     * Root TOC always contains entries only for ISSW and SUBTOC.
     * FIXEDTOC always contains all entries for boot images, without ISSW.
     * SUBTOC has equal contents with FIXEDTOC, and additionally contains entries for images that
     * are not part of the boot area (meaning all entries found in the manifest).
     */

    if (FALSE == Params_p->Flashlayout_3_0) {
        return WriteBAM_Layout_2_0(Params_p, TOCEntries, SubTOCEntries, OutputIOHandle_1);
    } else {
        if (FALSE == Params_p->SplitOutputImage) {
            return WriteBAM_Layout_3_0(Params_p, TOCEntries, SubTOCEntries, OutputIOHandle_1, OutputIOHandle_1);
        } else {
            return WriteBAM_Layout_3_0(Params_p, TOCEntries, SubTOCEntries, OutputIOHandle_1, OutputIOHandle_2);
        }
    }
}

/*
 * For flash layout version 2.0.
 */
static ErrorCode_e WriteBAM_Layout_2_0(LocalParameters_t *const Params_p, const uint8 TOCEntries, const uint8 SubTOCEntries, const IO_Handle_t OutputIOHandle)
{
    uint32 i, j = 0;
    uint32 TOCStartAddress = 0;
    uint8 DuplicatesOfTOC = 4;
    uint8 *TOCData_p = NULL;
    ErrorCode_e Result = E_SUCCESS;
    ManifestParser_t *ManifestTemp_p = NULL;
    SourceType_t ImageType = DEVICE_UNKNOWN;

    // Allocate memory for complete TOC
    TOCData_p = (uint8 *)malloc(TOCEntries * sizeof(TOC_Entry_t));
    ASSERT(NULL != TOCData_p);
    memset(TOCData_p, 0, TOCEntries * sizeof(TOC_Entry_t));

    // Create and write main TOC and boot images
    for (i = 0; i < DuplicatesOfTOC && E_SUCCESS == Result; i++) {
        if ((i == 0) && (TRUE == Params_p->SkipFirstBlock)) {
            continue;
        }

        TOCStartAddress = i * BOOT_IMAGE_SIZE;

        printf("Building TOC[%u]...\n", i);
        BuildTOC_Layout_2_0(Params_p, TOCStartAddress, SubTOCEntries, TOCData_p);

        // Write TOC to image
        printf("Writing TOC[%u] to image, start address:%X...\n", i, TOCStartAddress);

        if (TRUE == Params_p->GenerateBinary) {
            Result = WriteToImageBinary(TOCStartAddress, OutputIOHandle, TOCData_p, TOCEntries * sizeof(TOC_Entry_t));
        } else {
            Result = WriteToImageSrecord(TOCStartAddress, OutputIOHandle, TOCData_p, TOCEntries * sizeof(TOC_Entry_t));
        }

        // Write ISSW and X-LOADER to image
        for (j = 0, ManifestTemp_p = Params_p->ManifestParser_p; j < Params_p->ImagesInManifest && E_SUCCESS == Result; j++, ManifestTemp_p++) {
            if ('\0' == ManifestTemp_p->TocID[0]) {
                continue;
            }

            // Get Image type of next file
            ImageType = GetImageType(ManifestTemp_p);

            if (ImageType == DEVICE_BAM) {
                printf("Writing '%s' to image, start address:%llX...\n", ManifestTemp_p->Name, ManifestTemp_p->TargetStartAddress + TOCStartAddress);
                Result = WriteBinToImage(Params_p, ManifestTemp_p->Name, ManifestTemp_p->TargetStartAddress + TOCStartAddress, OutputIOHandle);
            }
        }
    }

    BUFFER_FREE(TOCData_p);

    return Result;
}

/*
 * For flash layout version 3.0.
 */
static ErrorCode_e WriteBAM_Layout_3_0(LocalParameters_t *const Params_p, const uint8 TOCEntries, const uint8 SubTOCEntries, const IO_Handle_t OutputIOHandle_1, const IO_Handle_t OutputIOHandle_2)
{
    ErrorCode_e Result = E_SUCCESS;
    uint32 i = 0;
    uint32 j = 0;
    uint32 TOCStartAddress = 0;
    uint32 BootBlockOffset = 0;
    uint8 *TOCData_p = NULL;
    uint8 *SubTOCData_p = NULL;
    ManifestParser_t *ManifestTemp_p = NULL;
    SourceType_t ImageType = DEVICE_UNKNOWN;

    /*
     * Root TOC always contains entries only for ISSW and SUBTOC.
     * FIXEDTOC always contains all entries for boot images, without ISSW.
     * SUBTOC has equal contents with FIXEDTOC and additionally contains images that
     * are not part of the boot area (meaning all entries found in the manifest).
     */

    // Allocate memory for complete TOC
    TOCData_p = (uint8 *)malloc(TOCEntries * sizeof(TOC_Entry_t));
    ASSERT(NULL != TOCData_p);
    memset(TOCData_p, 0, TOCEntries * sizeof(TOC_Entry_t));

    // Allocate memory for complete SUBTOC
    SubTOCData_p = (uint8 *)malloc(SubTOCEntries * sizeof(TOC_Entry_t));
    ASSERT(NULL != SubTOCData_p);
    memset(SubTOCData_p, 0, SubTOCEntries * sizeof(TOC_Entry_t));

    BootBlockOffset = 0;

    for (i = 1; i <= ROOT_TOC_COPIES_NUMBER && E_SUCCESS == Result; i++) {

        TOCStartAddress = i * BOOT_IMAGE_SIZE;

        printf("Building TOC[%u]...\n", i);
        printf("Building SUBTOC[%u]...\n", i);

        BuildTOCandSubTOC_Layout_3_0(Params_p, TOCStartAddress, SubTOCEntries, BootBlockOffset, TOCData_p, SubTOCData_p);

        // Write TOC to image
        printf("Writing TOC[%u] to image, start address:%X...\n", i, TOCStartAddress);

        if (TRUE == Params_p->GenerateBinary) {
            Result = WriteToImageBinary(TOCStartAddress, OutputIOHandle_1, TOCData_p, TOCEntries * sizeof(TOC_Entry_t));
        } else {
            Result = WriteToImageSrecord(TOCStartAddress, OutputIOHandle_1, TOCData_p, TOCEntries * sizeof(TOC_Entry_t));
        }

        // Write SubTOC to image
        printf("Writing SubTOC to image, start address:%X...\n", Params_p->SubTOCStartAddress + BootBlockOffset);

        if (Params_p->GenerateBinary) {
            Result = WriteToImageBinary(Params_p->SubTOCStartAddress + BootBlockOffset, OutputIOHandle_1, SubTOCData_p, SubTOCEntries * sizeof(TOC_Entry_t));
        } else {
            if (Params_p->SplitOutputImage) {
                Result = WriteToImageSrecord(Params_p->SubTOCStartAddress + BootBlockOffset, OutputIOHandle_2, SubTOCData_p, SubTOCEntries * sizeof(TOC_Entry_t));
            } else {
                Result = WriteToImageSrecord(Params_p->SubTOCStartAddress + BootBlockOffset, OutputIOHandle_1, SubTOCData_p, SubTOCEntries * sizeof(TOC_Entry_t));
            }
        }

        //Write Boot images
        for (j = 0, ManifestTemp_p = Params_p->ManifestParser_p; j < Params_p->ImagesInManifest && E_SUCCESS == Result; j++, ManifestTemp_p++) {
            if ('\0' == ManifestTemp_p->TocID[0]) {
                continue;
            }

            // Get Image type of next file
            ImageType = GetImageType(ManifestTemp_p);

            if (ImageType == DEVICE_BAM) {
                printf("Writing '%s' to image, start address:%llX...\n", ManifestTemp_p->Name, ManifestTemp_p->TargetStartAddress + BootBlockOffset);

                if (FALSE == Params_p->GenerateBinary && TRUE == Params_p->SplitOutputImage) {
                    Result = WriteBinToImage(Params_p, ManifestTemp_p->Name, ManifestTemp_p->TargetStartAddress + BootBlockOffset, OutputIOHandle_2);
                } else {
                    Result = WriteBinToImage(Params_p, ManifestTemp_p->Name, ManifestTemp_p->TargetStartAddress + BootBlockOffset, OutputIOHandle_1);
                }

            }
        }

        BootBlockOffset += BOOT_BLOCK_SIZE;
    }

    BUFFER_FREE(TOCData_p);
    BUFFER_FREE(SubTOCData_p);

    return Result;
}

static ErrorCode_e WriteBDM(LocalParameters_t *const Params_p, const uint8 SubTOCEntries, const IO_Handle_t OutputIOHandle, const uint8 CurrentFlash)
{
    uint32 i;
    uint32 CSPSAStartAddress = 0;
    uint32 CSPSAPartitionSize = 0;
    ErrorCode_e Result = E_SUCCESS;
    ManifestParser_t *ManifestTemp_p = NULL;
    SourceType_t ImageType = DEVICE_UNKNOWN;

    // Write rest of bins to image
    for (i = 0, ManifestTemp_p = Params_p->ManifestParser_p; i < Params_p->ImagesInManifest && E_SUCCESS == Result; i++, ManifestTemp_p++) {
        if ('\0' == ManifestTemp_p->TocID[0]) {
            continue;
        }

        // Get Image type of next file
        ImageType = GetImageType(ManifestTemp_p);

        if ((ImageType != DEVICE_BAM) && (ManifestTemp_p->TargetId == CurrentFlash)) {

            if (0 == strncmp(ContentTypeEmptyPartitionString, ManifestTemp_p->ContentType, strlen(ContentTypeEmptyPartitionString))) {
                printf("Empty partition  %s found. Skip.\n", ManifestTemp_p->Name);
                continue;
            }

            printf("Writing '%s' to image, start address:%llX...\n", ManifestTemp_p->Name, ManifestTemp_p->TargetStartAddress);
            Result = WriteBinToImage(Params_p, ManifestTemp_p->Name, ManifestTemp_p->TargetStartAddress, OutputIOHandle);

            if (0 == strncmp(ManifestTemp_p->TocID, "CSPSA", 5)) {
                CSPSAStartAddress = ManifestTemp_p->TargetStartAddress;
                CSPSAPartitionSize = ManifestTemp_p->PartitionContentSize;
            }
        }
    }

    // Write CSPSA to image
    if (E_SUCCESS == Result && NULL != Params_p->CSPSAHandle && 0 != CSPSAStartAddress && 0 != CSPSAPartitionSize) {
        uint32 Count;
        uint8 Data[MAX_DATA_LENGTH_IN_SRECORD];
        printf("Dumping raw CSPSA to image, start address:%X...\n", CSPSAStartAddress);

        for (Count = 0; Count < CSPSAPartitionSize && E_SUCCESS == Result; Count += MAX_DATA_LENGTH_IN_SRECORD) {
            (void)ReadRawCSPSA(Params_p->CSPSAHandle, Count, Data, MAX_DATA_LENGTH_IN_SRECORD);

            if (Params_p->GenerateBinary) {
                Result = WriteToImageBinary(CSPSAStartAddress + Count, OutputIOHandle, Data, MAX_DATA_LENGTH_IN_SRECORD);
            } else {
                Result = WriteToImageSrecord(CSPSAStartAddress + Count, OutputIOHandle, Data, MAX_DATA_LENGTH_IN_SRECORD);
            }
        }
    }

    /*
     * SUBTOC image should be written on flash only for flashlayout version 2.0
     * in this phase.
     * For flashlayout version 3.0 SUBTOC is written immediately after the
     * Root TOC is written and is part of the boot area.
     */
    if ((E_SUCCESS == Result) && (Params_p->Flashlayout_3_0 == FALSE)) {
        // Write Sub TOC to image
        if (E_SUCCESS == Result && 0 < SubTOCEntries && Params_p->SubTOCFlash == CurrentFlash) {
            // Allocate memory for SubTOC
            uint8 *SubTOCData_p = (uint8 *)malloc(SubTOCEntries * sizeof(TOC_Entry_t));
            ASSERT(NULL != SubTOCData_p);
            memset(SubTOCData_p, 0, SubTOCEntries * sizeof(TOC_Entry_t));

            printf("Building SubTOC...\n");
            BuildSubTOC_Layout_2_0(Params_p, SubTOCData_p);

            // Write SubTOC to image
            printf("Writing SubTOC to image, start address:%X...\n", Params_p->SubTOCStartAddress);

            if (Params_p->GenerateBinary) {
                Result = WriteToImageBinary(Params_p->SubTOCStartAddress, OutputIOHandle, SubTOCData_p, SubTOCEntries * sizeof(TOC_Entry_t));
            } else {
                Result = WriteToImageSrecord(Params_p->SubTOCStartAddress, OutputIOHandle, SubTOCData_p, SubTOCEntries * sizeof(TOC_Entry_t));
            }

            BUFFER_FREE(SubTOCData_p);
        }
    }

    return Result;
}

static void BuildTOC_Layout_2_0(LocalParameters_t *const Params_p, const uint32 TOCStartAddress, const uint8 NoOfSubTOCEntries, uint8 *const TOCData_p)
{
    uint32 i;
    uint32 flash_layout_img_count = 1;
    uint8 count = 0;
    TOC_Entry_t TOCEntry;
    uint8 TempData[sizeof(TOC_Entry_t)];
    ManifestParser_t *ManifestTemp_p = Params_p->ManifestParser_p;
    FlashLayoutParser_t *Flashlayout_Temp_p = Params_p->FlashLayoutParser_p;
    SourceType_t ImageType = DEVICE_UNKNOWN;
    uint64 TOC_Size = 0;
    uint64 TOC_Offset = 0;

    for (i = 0; i < Params_p->ImagesInManifest; i++) {
        memset(&TOCEntry, 0, sizeof(TOC_Entry_t));

        if ((ManifestTemp_p->FlashingFlags & PARTITION_ENTRY_EXTENSION) || (ManifestTemp_p->FlashingFlags & PARTITION_ENTRY_EXTENSION_FIRST)) {
            ManifestTemp_p++;
            continue;
        }

        if (ManifestTemp_p->TocID[0] != '\0' && ManifestTemp_p->SubTOC_Of[0] == '\0') {
            // Get Image type of next file
            ImageType = GetImageType(ManifestTemp_p);

            if ((0 == strncmp(ManifestTemp_p->SizeMode, PartSizeString, strlen(PartSizeString)))) { // The entry is marked as PartitionSize
                flash_layout_img_count = 1;
                Flashlayout_Temp_p = Params_p->FlashLayoutParser_p;

                // Iterate trough the flashlayout file and Retrieve partition range info
                do {
                    if (0 == strncmp(ManifestTemp_p->TocID, Flashlayout_Temp_p->TOC_ID, MAX_TOC_ID_SIZE)) {
                        TOC_Size = Flashlayout_Temp_p->EndAddress - Flashlayout_Temp_p->StartAddress;
                        break;
                    } else {
                        Flashlayout_Temp_p++;
                        flash_layout_img_count++;
                    }
                } while (flash_layout_img_count <= Params_p->ImagesInFlashLayout);
            } else {
                if (strcmp(ManifestTemp_p->ContentType, "x-empflash/image") == 0) {
                    TOC_Size = GetLengthOfFileInArchive(Params_p->ZipInZipHandle_p, ManifestTemp_p->Name);
                } else {
                    TOC_Size = ManifestTemp_p->PartitionContentSize;
                }

                if (ImageType == DEVICE_BAM) {
                    TOC_Offset               = ManifestTemp_p->TargetStartAddress;
                    TOCEntry.TOC_Align       = 0xFFFFFFFF;
                    TOCEntry.TOC_LoadAddress = 0xFFFFFFFF;
                } else {
                    TOC_Offset               = ManifestTemp_p->TargetStartAddress - TOCStartAddress;
                    TOCEntry.TOC_Align       = ManifestTemp_p->TocEntryPoint;
                    TOCEntry.TOC_LoadAddress = ManifestTemp_p->TocLoadAddress;
                }
            }

            Do_TOC_FillEntry(&TOCEntry, TOC_Offset, TOC_Size, ManifestTemp_p->TocFlags);

            memcpy(TOCEntry.TOC_Id, ManifestTemp_p->TocID, TOC_ID_LENGTH);

            if (ManifestTemp_p->FlashingFlags & PARTITION_ENTRY_EXTENSION_LAST) {
                Flashlayout_Temp_p = Params_p->FlashLayoutParser_p;
                flash_layout_img_count = 1;

                do {
                    if (0 == strncmp(ManifestTemp_p->TocID, Flashlayout_Temp_p->TOC_ID, MAX_TOC_ID_SIZE)) {
                        TOC_Offset = Flashlayout_Temp_p->StartAddress;
                        TOC_Size = Flashlayout_Temp_p->EndAddress - Flashlayout_Temp_p->StartAddress;
                        Do_TOC_FillEntry(&TOCEntry, TOC_Offset, TOC_Size, ManifestTemp_p->TocFlags);
                        break;
                    } else {
                        Flashlayout_Temp_p++;
                        flash_layout_img_count++;
                    }
                } while (flash_layout_img_count <= Params_p->ImagesInFlashLayout);
            }

            memset(TempData, 0, sizeof(TOC_Entry_t));
            SerializeTOCEntry(&TOCEntry, TempData);
            memcpy(&TOCData_p[count * sizeof(TOC_Entry_t)], TempData, sizeof(TOC_Entry_t));
            count++;
        }

        ManifestTemp_p++;
    }

    // Generate SUBTOC
    if ((NoOfSubTOCEntries > 0) && (TOC_IMAGE != Params_p->PreflashType)) {
        memset(&TOCEntry, 0, sizeof(TOC_Entry_t));
        TOCEntry.TOC_Offset = Params_p->SubTOCStartAddress - TOCStartAddress;
        TOCEntry.TOC_Size = NoOfSubTOCEntries * sizeof(TOC_Entry_t);
        TOCEntry.TOC_Flags = TOC_FLAG_SUBTOC;
        TOCEntry.TOC_Align = 0xFFFFFFFF;
        TOCEntry.TOC_LoadAddress = 0;
        memcpy(TOCEntry.TOC_Id, Params_p->SubTOCID, TOC_ID_LENGTH);

        memset(TempData, 0, sizeof(TOC_Entry_t));
        SerializeTOCEntry(&TOCEntry, TempData);
        memcpy(&TOCData_p[count * sizeof(TOC_Entry_t)], TempData, sizeof(TOC_Entry_t));
    }
}

static void BuildSubTOC_Layout_2_0(LocalParameters_t *const Params_p, uint8 *const SubTOCData_p)
{
    uint32 i;
    uint32 flash_layout_img_count = 1;
    uint8 count = 0;
    TOC_Entry_t TOCEntry;
    uint8 TempData[sizeof(TOC_Entry_t)];
    ManifestParser_t *ManifestTemp_p = Params_p->ManifestParser_p;
    FlashLayoutParser_t *Flashlayout_Temp_p = Params_p->FlashLayoutParser_p;
    uint64 TOC_Size = 0;
    uint64 TOC_Offset = 0;

    for (i = 0; i < Params_p->ImagesInManifest; i++) {
        memset(&TOCEntry, 0, sizeof(TOC_Entry_t));

        if ((ManifestTemp_p->FlashingFlags & PARTITION_ENTRY_EXTENSION) || (ManifestTemp_p->FlashingFlags & PARTITION_ENTRY_EXTENSION_FIRST)) {
            ManifestTemp_p++;
            continue;
        }

        if (ManifestTemp_p->TocID[0] != '\0' && ManifestTemp_p->SubTOC_Of[0] != '\0') {


            if ((0 == strncmp(ManifestTemp_p->SizeMode, PartSizeString, strlen(PartSizeString)))) { // The entry is marked as PartitionSize

                Flashlayout_Temp_p = Params_p->FlashLayoutParser_p;
                flash_layout_img_count = 1;

                do {
                    if (0 == strncmp(ManifestTemp_p->TocID, Flashlayout_Temp_p->TOC_ID, MAX_TOC_ID_SIZE)) {
                        TOC_Offset = Flashlayout_Temp_p->StartAddress;
                        TOC_Size = Flashlayout_Temp_p->EndAddress - Flashlayout_Temp_p->StartAddress;
                        break;
                    } else {
                        Flashlayout_Temp_p++;
                        flash_layout_img_count++;
                    }
                } while (flash_layout_img_count <= Params_p->ImagesInFlashLayout);
            } else {
                if (strcmp(ManifestTemp_p->ContentType, "x-empflash/image") == 0) {
                    TOC_Size = GetLengthOfFileInArchive(Params_p->ZipInZipHandle_p, ManifestTemp_p->Name);
                } else {
                    TOC_Size = ManifestTemp_p->PartitionContentSize;
                }

                TOC_Offset = ManifestTemp_p->TargetStartAddress;
                TOCEntry.TOC_Align = ManifestTemp_p->TocEntryPoint;
                TOCEntry.TOC_LoadAddress = ManifestTemp_p->TocLoadAddress;
            }

            Do_TOC_FillEntry(&TOCEntry, TOC_Offset, TOC_Size, ManifestTemp_p->TocFlags);
            memcpy(TOCEntry.TOC_Id, ManifestTemp_p->TocID, TOC_ID_LENGTH);

            memset(TempData, 0, sizeof(TOC_Entry_t));
            SerializeTOCEntry(&TOCEntry, TempData);
            memcpy(&SubTOCData_p[count * sizeof(TOC_Entry_t)], TempData, sizeof(TOC_Entry_t));
            count++;
        }

        ManifestTemp_p++;
    }
}

/*
 * @brief This function generates TOC and SUB TOC entries for flashlayout
 *        version 3.0. It is used by both tools TOC-gen and Pre-flash.
 *
 * For flash layout version 3.0:
 * -----------------------------
 * Root TOC always contains entries only for ISSW and SUBTOC.
 * FIXEDTOC always contains all entries for boot images, without ISSW.
 * SUBTOC has equal contents with FIXEDTOC and additionally contains entries for
 * images that are not part of the boot area (meaning all entries found in the manifest).
 *
 * For flash layout version 2.0:
 * -----------------------------
 * FIXEDTOC always contains all entries for boot images.
 * For TOC-gen tool Root TOC has equal contents with FIXEDTOC.
 * For Pre-flash tool Root TOC has equal contents with FIXEDTOC plus one
 * additional entry for SUBTOC.
 * SUBTOC contains entries for images that are specified as SUBTOC entries in
 * manifest.txt file.
 *
 * @param [in,out] Params_p          Contains parameters needed in both tools.
 * @param [in]     TOCStartAddress   TOC start address. It is used for
 *                                   calculating of the offset for TOC entries
 *                                   relative to the Root TOC copy.
 * @param [in]     NoOfSubTOCEntries Number of SUBTOC entries
 * @param [in]     BootBlockOffset   Offset of the boot block copy.
 * @param [out]    TOCData_p         Output buffer.
 * @param [out]    SubTOCData_p      Output buffer.
 *
 * @return none
 */
static void BuildTOCandSubTOC_Layout_3_0(LocalParameters_t *const Params_p, const uint32 TOCStartAddress, const uint8 NoOfSubTOCEntries, uint32 BootBlockOffset, uint8 *const TOCData_p, uint8 *const SubTOCData_p)
{
    uint32 i;
    uint32 flash_layout_img_count = 1;
    uint8 count = 0;
    TOC_Entry_t TOCEntry;
    uint8 TempData[sizeof(TOC_Entry_t)];
    ManifestParser_t *ManifestTemp_p = Params_p->ManifestParser_p;
    FlashLayoutParser_t *Flashlayout_Temp_p = Params_p->FlashLayoutParser_p;
    SourceType_t ImageType = DEVICE_UNKNOWN;
    uint64 TOC_Size = 0;
    uint64 TOC_Offset = 0;

    for (i = 0; i < Params_p->ImagesInManifest; i++) {
        memset(&TOCEntry, 0, sizeof(TOC_Entry_t));

        if ((ManifestTemp_p->FlashingFlags & PARTITION_ENTRY_EXTENSION) || (ManifestTemp_p->FlashingFlags & PARTITION_ENTRY_EXTENSION_FIRST)) {
            ManifestTemp_p++;
            continue;
        }

        if (ManifestTemp_p->TocID[0] != '\0' && ManifestTemp_p->SubTOC_Of[0] == '\0') {
            // Get Image type of next file
            ImageType = GetImageType(ManifestTemp_p);

            if ((0 == strncmp(ManifestTemp_p->SizeMode, PartSizeString, strlen(PartSizeString)))) { // The entry is marked as PartitionSize
                flash_layout_img_count = 1;
                Flashlayout_Temp_p = Params_p->FlashLayoutParser_p;

                // Iterate trough the flashlayout file and Retrieve partition range info
                do {
                    if (0 == strncmp(ManifestTemp_p->TocID, Flashlayout_Temp_p->TOC_ID, MAX_TOC_ID_SIZE)) {
                        TOC_Size = Flashlayout_Temp_p->EndAddress - Flashlayout_Temp_p->StartAddress;
                        break;
                    } else {
                        Flashlayout_Temp_p++;
                        flash_layout_img_count++;
                    }
                } while (flash_layout_img_count <= Params_p->ImagesInFlashLayout);
            } else {

                if (ImageType == DEVICE_BAM) {
                    TOC_Offset               = ManifestTemp_p->TargetStartAddress - Params_p->SubTOCStartAddress;
                    TOCEntry.TOC_Align       = 0xFFFFFFFF;
                    TOCEntry.TOC_LoadAddress = 0xFFFFFFFF;
                } else {
                    TOC_Offset               = ManifestTemp_p->TargetStartAddress;
                    TOCEntry.TOC_Align       = ManifestTemp_p->TocEntryPoint;
                    TOCEntry.TOC_LoadAddress = ManifestTemp_p->TocLoadAddress;
                }

                if (strcmp(ManifestTemp_p->ContentType, "x-empflash/image") == 0) {
                    TOC_Size = GetLengthOfFileInArchive(Params_p->ZipInZipHandle_p, ManifestTemp_p->Name);
                } else {
                    TOC_Size = ManifestTemp_p->PartitionContentSize;
                }
            }

            Do_TOC_FillEntry(&TOCEntry, TOC_Offset, TOC_Size, ManifestTemp_p->TocFlags);

            memcpy(TOCEntry.TOC_Id, ManifestTemp_p->TocID, TOC_ID_LENGTH);
            memset(TempData, 0, sizeof(TOC_Entry_t));
            SerializeTOCEntry(&TOCEntry, TempData);
            memcpy(&TOCData_p[count * sizeof(TOC_Entry_t)], TempData, sizeof(TOC_Entry_t));
            count++;
        }

        ManifestTemp_p++;
    }

    // Generate SUBTOC
    if (NoOfSubTOCEntries > 0) {
        memset(&TOCEntry, 0, sizeof(TOC_Entry_t));
        TOCEntry.TOC_Offset = Params_p->SubTOCStartAddress - TOCStartAddress + BootBlockOffset;
        TOCEntry.TOC_Size = NoOfSubTOCEntries * sizeof(TOC_Entry_t);
        TOCEntry.TOC_Flags = TOC_FLAG_SUBTOC;
        TOCEntry.TOC_Align = 0xFFFFFFFF;
        TOCEntry.TOC_LoadAddress = 0;
        memcpy(TOCEntry.TOC_Id, Params_p->SubTOCID, TOC_ID_LENGTH);

        memset(TempData, 0, sizeof(TOC_Entry_t));
        SerializeTOCEntry(&TOCEntry, TempData);
        memcpy(&TOCData_p[count * sizeof(TOC_Entry_t)], TempData, sizeof(TOC_Entry_t));
    }

    ManifestTemp_p = Params_p->ManifestParser_p;
    Flashlayout_Temp_p = Params_p->FlashLayoutParser_p;
    count = 0;

    for (i = 0; i < Params_p->ImagesInManifest; i++) {
        memset(&TOCEntry, 0, sizeof(TOC_Entry_t));

        if ((ManifestTemp_p->FlashingFlags & PARTITION_ENTRY_EXTENSION) || (ManifestTemp_p->FlashingFlags & PARTITION_ENTRY_EXTENSION_FIRST)) {
            ManifestTemp_p++;
            continue;
        }

        if (ManifestTemp_p->TocID[0] != '\0' && ManifestTemp_p->SubTOC_Of[0] != '\0') {
            // Get Image type of next file
            ImageType = GetImageType(ManifestTemp_p);

            if ((0 == strncmp(ManifestTemp_p->SizeMode, PartSizeString, strlen(PartSizeString)))) { // The entry is marked as PartitionSize

                Flashlayout_Temp_p = Params_p->FlashLayoutParser_p;
                flash_layout_img_count = 1;

                do {
                    if (0 == strncmp(ManifestTemp_p->TocID, Flashlayout_Temp_p->TOC_ID, MAX_TOC_ID_SIZE)) {
                        TOC_Size = Flashlayout_Temp_p->EndAddress - Flashlayout_Temp_p->StartAddress;
                        break;
                    } else {
                        Flashlayout_Temp_p++;
                        flash_layout_img_count++;
                    }
                } while (flash_layout_img_count <= Params_p->ImagesInFlashLayout);
            } else {

                if (ImageType == DEVICE_BAM) {
                    TOC_Offset               = ManifestTemp_p->TargetStartAddress - Params_p->SubTOCStartAddress;
                    TOCEntry.TOC_Align       = 0xFFFFFFFF;
                    TOCEntry.TOC_LoadAddress = 0xFFFFFFFF;
                } else {
                    TOC_Offset               = ManifestTemp_p->TargetStartAddress;
                    TOCEntry.TOC_Align       = ManifestTemp_p->TocEntryPoint;
                    TOCEntry.TOC_LoadAddress = ManifestTemp_p->TocLoadAddress;
                }

                if (strcmp(ManifestTemp_p->ContentType, "x-empflash/image") == 0) {
                    TOC_Size = GetLengthOfFileInArchive(Params_p->ZipInZipHandle_p, ManifestTemp_p->Name);
                } else {
                    TOC_Size = ManifestTemp_p->PartitionContentSize;
                }
            }

            Do_TOC_FillEntry(&TOCEntry, TOC_Offset, TOC_Size, ManifestTemp_p->TocFlags);
            memcpy(TOCEntry.TOC_Id, ManifestTemp_p->TocID, TOC_ID_LENGTH);
            memset(TempData, 0, sizeof(TOC_Entry_t));
            SerializeTOCEntry(&TOCEntry, TempData);
            memcpy(&SubTOCData_p[count * sizeof(TOC_Entry_t)], TempData, sizeof(TOC_Entry_t));
            count++;
        }

        ManifestTemp_p++;
    }
}

static uint64 GetLengthOfFileInArchive(void *const ZipHandle_p, char *const Name_p)
{
    IO_Handle_t IOHandle;
    uint64 FileLength;

    if (Do_Zip_OpenFile(ZipHandle_p, Name_p, strlen(Name_p), &IOHandle) != E_SUCCESS) {
        printf("Could not open file:%s in archive!\n", Name_p);
        return 0;
    }

    FileLength = Do_IO_GetLength(IOHandle);

    if (FileLength == IO_ERROR) {
        printf("Do_IO_GetLength returned IO_ERROR!\n");
        Do_IO_Close(IOHandle);
        return 0;
    }

    Do_IO_Close(IOHandle);

    return FileLength;
}

static ErrorCode_e WriteToImageSrecord(const uint32 StartAddress, const IO_Handle_t OutputIOHandle, uint8 *const Data_p, const uint64 DataLength)
{
    char SRecord[128];
    uint64 DataUsed = 0;
    uint32 Address = StartAddress;
    uint8 *DataToWrite_p = Data_p;
    uint64 FileLength = 0;

    FileLength = Do_IO_GetLength(OutputIOHandle);

    if (IO_ERROR == FileLength) {
        printf("Failed to get file length!\n");
        return E_GENERAL_FATAL_ERROR;
    }

    while (DataUsed < DataLength) {
        uint8 DataLengthInSRecord = MAX_DATA_LENGTH_IN_SRECORD;

        if ((DataLength - DataUsed) < MAX_DATA_LENGTH_IN_SRECORD) {
            DataLengthInSRecord = (DataLength - DataUsed);
        }

        CreateSRecord(Address, DataToWrite_p, DataLengthInSRecord, SRecord);

        if (Do_IO_PositionedWrite(OutputIOHandle, SRecord, strlen(SRecord), strlen(SRecord), FileLength) == IO_ERROR) {
            printf("Error when writing to image!\n");
            return E_GENERAL_FATAL_ERROR;
        }

        DataToWrite_p += DataLengthInSRecord;
        DataUsed += DataLengthInSRecord;
        Address += DataLengthInSRecord;
        FileLength += strlen(SRecord);
    }

    return E_SUCCESS;
}

static ErrorCode_e WriteToImageBinary(uint32 StartOffset, const IO_Handle_t OutputIOHandle, uint8 *const Data_p, const uint64 DataLength)
{
    uint8 *PadBuffer_p;
    uint32 PadLength;
    uint64 FileLength = 0;
    static uint32 DestinationOffset = 0;

    FileLength = Do_IO_GetLength(OutputIOHandle);

    if (IO_ERROR == FileLength) {
        printf("Failed to get file length!\n");
        return E_GENERAL_FATAL_ERROR;
    }

    if ((FileLength == 0) && (StartOffset > 0)) {
        DestinationOffset = StartOffset;
    }

    //
    // StartOffset represent absolute offset on target device.
    // We will start preflash image relatively from lowest
    // StartOffset address.
    //
    StartOffset -= DestinationOffset;

    if (StartOffset > (uint32)FileLength) {
        PadBuffer_p = (uint8 *)calloc(1, PAD_BUFFER_SIZE);

        if (PadBuffer_p == NULL) {
            printf("Memory allocation failure!\n");
            return E_GENERAL_FATAL_ERROR;
        }

        while ((uint32)FileLength < StartOffset) {
            PadLength = StartOffset - (uint32)FileLength;

            if (PadLength > PAD_BUFFER_SIZE) {
                PadLength = PAD_BUFFER_SIZE;
            }

            if (Do_IO_PositionedWrite(OutputIOHandle, PadBuffer_p, PadLength, PadLength, FileLength) == IO_ERROR) {
                printf("Error when writing to image!\n");
                return E_GENERAL_FATAL_ERROR;
            }

            FileLength += PadLength;
        }

        free(PadBuffer_p);
    } else {
        // Change current offset position according to requested offset
        FileLength = StartOffset;
    }

    if (Do_IO_PositionedWrite(OutputIOHandle, Data_p, DataLength, DataLength, FileLength) == IO_ERROR) {
        printf("Error when writing to image!\n");
        return E_GENERAL_FATAL_ERROR;
    }

    return E_SUCCESS;
}

static ErrorCode_e WriteBinToImage(LocalParameters_t *const Params_p, char *const Name_p, const uint32 Address, const IO_Handle_t OutputIOHandle)
{
    IO_Handle_t IOHandle;
    uint64 FileLength;
    uint64 BytesRead;
    uint64 TotalBytesRead = 0;
    uint16 BufferLength = 2048;
    uint8 *Buffer_p = NULL;
    ErrorCode_e Result = E_SUCCESS;

    // Open bin in zip
    if (Do_Zip_OpenFile(Params_p->ZipInZipHandle_p, Name_p, strlen(Name_p), &IOHandle) != E_SUCCESS) {
        printf("Could not open bin:%s in zip!\n", Name_p);
        return E_GENERAL_FATAL_ERROR;
    }

    // Create buffer to store bin
    FileLength = Do_IO_GetLength(IOHandle);

    if (FileLength == IO_ERROR) {
        Do_IO_Close(IOHandle);
        return E_GENERAL_FATAL_ERROR;
    }

    if (FileLength < BufferLength) {
        BufferLength = FileLength;
    }

    Buffer_p = (uint8 *)malloc(BufferLength);
    ASSERT(NULL != Buffer_p);

    // Write bin to image
    while (TotalBytesRead < FileLength) {
        // Read file to buffer
        BytesRead = Do_IO_PositionedRead(IOHandle, Buffer_p, BufferLength, (FileLength - TotalBytesRead) < BufferLength ? (FileLength - TotalBytesRead) : BufferLength, TotalBytesRead);

        if (BytesRead == IO_ERROR) {
            printf("Error during read!\n");
            Result =  E_GENERAL_FATAL_ERROR;
            break;
        }

        if (TRUE == Params_p->GenerateBinary) {
            Result = WriteToImageBinary(Address + TotalBytesRead, OutputIOHandle, Buffer_p, BytesRead);
        } else {
            Result = WriteToImageSrecord(Address + TotalBytesRead, OutputIOHandle, Buffer_p, BytesRead);
        }

        if (Result != E_SUCCESS) {
            printf("Error during write!\n");
            break;
        }

        TotalBytesRead += BytesRead;
    }

    BUFFER_FREE(Buffer_p);
    Do_IO_Close(IOHandle);

    return Result;
}

/*
 * This function splits the output image into two files.
 * At the beginning FileName_1 contains the all data.
 * Data from FileName_1 from address START_ADDRESS_SPLIT_IMAGE_FILE_2 (4MB-0x20000) is copied to
 * FileName_2 in total size of  SIZE_SPLIT_IMAGE_FILE_2 (0.5 MB).
 * Than FileName_1 is changed to contain only data from address 0x20000 to 0x60000.
 */
static ErrorCode_e SplitBinaryFile(char *FileName_1, char *FileName_2)
{
    ErrorCode_e Result = E_SUCCESS;
    uint64 LengthFile1 = 0;
    FILE *file_handle1;
    FILE *file_handle2;
    char *buf1 = NULL;
    char *buf2 = NULL;

    file_handle1 = fopen(FileName_1, "rb");

    if (NULL == file_handle1) {
        return E_GENERAL_FATAL_ERROR;
    }

    file_handle2 = fopen(FileName_2, "wb");

    if (NULL == file_handle2) {
        return E_GENERAL_FATAL_ERROR;
    }


    buf1 = (char *)malloc(SIZE_SPLIT_IMAGE_FILE_1);

    if (NULL == buf1) {
        return E_GENERAL_FATAL_ERROR;
    }

    if (fseek(file_handle1 , 0, SEEK_END)) {
        return E_GENERAL_FATAL_ERROR;
    }

    LengthFile1 = ftell(file_handle1);

    buf2 = (char *)malloc(SIZE_SPLIT_IMAGE_FILE_2);

    if (NULL == buf2) {
        return E_GENERAL_FATAL_ERROR;
    }

    // set the file_handle1 to address of 4MB-0x20000
    if (fseek(file_handle1 , START_ADDRESS_SPLIT_IMAGE_FILE_2, SEEK_SET)) {
        return E_GENERAL_FATAL_ERROR;
    }

    if (LengthFile1 >= START_ADDRESS_SPLIT_IMAGE_FILE_2) {
        if (fread(buf2, 1, SIZE_SPLIT_IMAGE_FILE_2, file_handle1) != SIZE_SPLIT_IMAGE_FILE_2) {
            return E_GENERAL_FATAL_ERROR;
        }

        if (fwrite(buf2, 1, SIZE_SPLIT_IMAGE_FILE_2, file_handle2) != SIZE_SPLIT_IMAGE_FILE_2) {
            return E_GENERAL_FATAL_ERROR;
        }
    } else {
        return E_GENERAL_FATAL_ERROR;
    }

    if (fseek(file_handle1 , 0, SEEK_SET)) {
        return E_GENERAL_FATAL_ERROR;
    }

    if (fread(buf1, 1, SIZE_SPLIT_IMAGE_FILE_1, file_handle1) != SIZE_SPLIT_IMAGE_FILE_1) {
        return E_GENERAL_FATAL_ERROR;
    }

    if (fclose(file_handle1) != 0) {
        return E_GENERAL_FATAL_ERROR;
    }

    file_handle1 = fopen(FileName_1, "wb");

    if (NULL == file_handle1) {
        return E_GENERAL_FATAL_ERROR;
    }

    if (fwrite(buf1, 1, SIZE_SPLIT_IMAGE_FILE_1, file_handle1) != SIZE_SPLIT_IMAGE_FILE_1) {
        return E_GENERAL_FATAL_ERROR;
    }

    free(buf1);
    free(buf2);

    if (fclose(file_handle1)) {
        return E_GENERAL_FATAL_ERROR;
    }

    if (fclose(file_handle2)) {
        return E_GENERAL_FATAL_ERROR;
    }

    printf("Image was successfully split into two files\n");
    return Result;
}
