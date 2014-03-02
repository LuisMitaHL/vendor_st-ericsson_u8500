/*******************************************************************************
 * $Copyright ST-Ericsson 2012 $
 ******************************************************************************/
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <string.h>
#include "r_debug_macro.h"
#include "r_memory_utils.h"
#include "preflash_tool.h"
#include "preflash_utils.h"

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/
extern const char *ContentTypeBootImageString;
extern const char *ContentTypeImageString;
extern const char *ContentTypeSparseImageString;
extern const char *ContentTypeEmptyPartitionString;
extern const char *ContentTypeFlashLayoutString;

extern const char *BDMUnitString;
extern const char *BAMUnitString;

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/
void SerializeTOCEntry(TOC_Entry_t *const TocEntry_p, uint8 Data[sizeof(TOC_Entry_t)])
{
    uint8 i;
    uint8 count = 0;

    for (i = 0; i < sizeof(TocEntry_p->TOC_Offset); i++) // LE
        //for (int i = sizeof(TocEntry_p->TOC_Offset) - 1; i >= 0; i--) // BE
    {
        Data[count++] = ((TocEntry_p->TOC_Offset >> (i * 8)) & 0xFF);
    }

    for (i = 0; i < sizeof(TocEntry_p->TOC_Size); i++) // LE
        //for (int i = sizeof(TocEntry_p->TOC_Size) - 1; i >= 0; i--) // BE
    {
        Data[count++] = ((TocEntry_p->TOC_Size >> (i * 8)) & 0xFF);
    }

    for (i = 0; i < sizeof(TocEntry_p->TOC_Flags); i++) // LE
        //for (int i = sizeof(TocEntry_p->TOC_Flags) - 1; i >= 0; i--) // BE
    {
        Data[count++] = ((TocEntry_p->TOC_Flags >> (i * 8)) & 0xFF);
    }

    for (i = 0; i < sizeof(TocEntry_p->TOC_Align); i++) // LE
        //for (int i = sizeof(TocEntry_p->TOC_Align) - 1; i >= 0; i--) // BE
    {
        Data[count++] = ((TocEntry_p->TOC_Align >> (i * 8)) & 0xFF);
    }

    for (i = 0; i < sizeof(TocEntry_p->TOC_LoadAddress); i++) // LE
        //for (int i = sizeof(TocEntry_p->TOC_LoadAddress) - 1; i >= 0; i--) // BE
    {
        Data[count++] = ((TocEntry_p->TOC_LoadAddress >> (i * 8)) & 0xFF);
    }

    memcpy(&Data[count], TocEntry_p->TOC_Id, strlen(TocEntry_p->TOC_Id));
}

SourceType_t GetImageType(ManifestParser_t *ManifestParser_p)
{
    if ((0 == strncmp(ContentTypeImageString, ManifestParser_p->ContentType, strlen(ContentTypeImageString))) ||
            (0 == strncmp(ContentTypeSparseImageString, ManifestParser_p->ContentType, strlen(ContentTypeSparseImageString)))) {
        if (0 == strncmp(BAMUnitString, ManifestParser_p->TargetType, strlen(BAMUnitString))) {
            C_(printf("flash_applications_functions.c (%d): DEVICE_BAM\n", __LINE__);)
            return DEVICE_BAM;
        } else if (0 == strncmp(BDMUnitString, ManifestParser_p->TargetType, strlen(BDMUnitString))) {
            C_(printf("flash_applications_functions.c (%d): DEVICE_BDM\n", __LINE__);)
            return DEVICE_BDM;
        } else {
            C_(printf("flash_applications_functions.c (%d): DEVICE_BDM\n", __LINE__);)
            return DEVICE_BDM;
        }
    } else if (0 == strncmp(ContentTypeBootImageString, ManifestParser_p->ContentType, strlen(ContentTypeBootImageString))) {
        C_(printf("flash_applications_functions.c (%d): DEVICE_BAM\n", __LINE__);)
        return DEVICE_BAM;
    } else if (0 == strncmp(ContentTypeEmptyPartitionString, ManifestParser_p->ContentType, strlen(ContentTypeEmptyPartitionString))) {
        C_(printf("flash_applications_functions.c (%d): Empty Partition DEVICE_PARTITION\n", __LINE__);)
        return DEVICE_PARTITION;
    } else {
        C_(printf("flash_applications_functions.c (%d): DEVICE_UNKNOWN\n", __LINE__);)
        return DEVICE_UNKNOWN;
    }
}

void CreateSRecord(const uint32 Address, uint8 *const Data_p, const uint8 DataLengthToCopy, char SRecord[128])
{
    uint8 i;
    uint8 AddressLength;
    uint16 Checksum = 0;
    char AddressFormat[10];
    uint8 DataLength = MAX_DATA_LENGTH_IN_SRECORD;

    if (DataLengthToCopy < DataLength) {
        DataLength = DataLengthToCopy;
    }

    // Create correct srecord depending on address length
    if (Address < 0x10000) { // 2 byte address
        snprintf(&SRecord[0], 128, "%s", "S1");
        AddressLength = 2;
    } else if (Address < 0x1000000) { // 3 byte address
        snprintf(&SRecord[0], 128, "%s", "S2");
        AddressLength = 3;
    } else { // 4 byte address
        snprintf(&SRecord[0], 128, "%s", "S3");
        AddressLength = 4;
    }

    // Copy count to srecord
    snprintf(&SRecord[2], (128 - 2), "%.2X", AddressLength + DataLength + CHECKSUM_LENGTH_IN_SRECORD);

    // Copy address to srecord
    snprintf(AddressFormat, 10, "%%.%uX", AddressLength * 2);
    snprintf(&SRecord[4], (128 - 4), AddressFormat, Address);

    // Copy data to srecord
    for (i = 0; i < DataLength; i++) {
        snprintf(&SRecord[4 + (AddressLength * 2) + (i * 2)], (128 - (4 + (AddressLength * 2) + (i * 2))), "%.2X", Data_p[i]);
    }

    // Calculate checksum
    Checksum += AddressLength + DataLength + CHECKSUM_LENGTH_IN_SRECORD;
    Checksum += ((Address >> 24) & 0xFF);
    Checksum += ((Address >> 16) & 0xFF);
    Checksum += ((Address >> 8) & 0xFF);
    Checksum += Address & 0xFF;

    for (i = 0; i < DataLength; i++) {
        Checksum += Data_p[i];
    }

    Checksum = ~Checksum;
    Checksum &= 0xFF;

    // Copy checksum to srecord
    snprintf(&SRecord[4 + (AddressLength * 2) + (DataLength * 2)], (128 - (4 + (AddressLength * 2) + (DataLength * 2))), "%.2X\n", Checksum);
}

ErrorCode_e WriteSRec0ToImage(const IO_Handle_t OutputIOHandle)
{
    char SRecord[] = "S00600004844521B\n";

    if (Do_IO_PositionedWrite(OutputIOHandle, SRecord, strlen(SRecord), strlen(SRecord), 0) == IO_ERROR) {
        printf("Error when writing SRec0 to image!\n");
        return E_GENERAL_FATAL_ERROR;
    }

    return E_SUCCESS;
}

ErrorCode_e WriteSRec9ToImage(const IO_Handle_t OutputIOHandle)
{
    char SRecord[] = "S9030000FC\n";
    uint64 FileLength = 0;

    FileLength = Do_IO_GetLength(OutputIOHandle);

    if (IO_ERROR == FileLength) {
        printf("Failed to get file length!\n");
        return E_GENERAL_FATAL_ERROR;
    }

    if (Do_IO_PositionedWrite(OutputIOHandle, SRecord, strlen(SRecord), strlen(SRecord), FileLength) == IO_ERROR) {
        printf("Error when writing SRec9 to image!\n");
        return E_GENERAL_FATAL_ERROR;
    }

    return E_SUCCESS;
}
