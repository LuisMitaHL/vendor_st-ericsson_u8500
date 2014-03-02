/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

/*
 *  @addtogroup ldr_utilities
 *  @{
 *    @addtogroup zip_parser
 *    @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdlib.h>
#include <string.h>

#include "t_basicdefinitions.h"
#include "r_debug.h"
#include "r_debug_macro.h"
#include "r_zip_parser.h"
#include "r_io_subsystem.h"
#include "error_codes.h"
#include "r_memory_utils.h"
#include "r_serialization.h"

/***********************************************************************
 * Types, constants
 **********************************************************************/
/** List of initialized IO vectors */
extern IO_VectorList_t *VectorList;

/* Size of end of central directory record, for zip files, without comments*/
#define SIZE_OF_END_OF_CENTRAL_DIRECTORY_RECORD         22
/* Size of zip64 end of central locator, for zip files, without extensible sector*/
#define SIZE_OF_ZIP64_END_OF_CENTRAL_DIRECTORY_LOCATOR  20
/* Size of zip64 end of central directory record, for zip files*/
#define SIZE_OF_ZIP64_END_OF_CENTRAL_DIRECTORY_RECORD   56
/* Size of digital signature, for zip files, without the signature data*/
#define SIZE_OF_DIGITAL_SIGNATURE                       6

/* Signature used for identifying the start of file header in the central directory, in the zip file*/
#define CENTRAL_FILE_HEADER_SIGNATURE               0x02014b50
/* Signature used for identifying the start of local file header, in the zip file*/
#define LOCAL_HEADER_SIGNATURE                      0x04034b50
/* Signature used for identifying the start of digital signature, in the zip file*/
#define HEADER_SIGNATURE                            0x05054b50
/* Signature used for identifying the start of "end of central directory", in the zip file*/
#define END_OF_CENTRAL_DIR_SIGNATURE                0x06054b50
/* Signature used for identifying the start of "zip64 end of central directory", in the zip file (with zip64 extension)*/
#define ZIP64_END_OF_CENTRAL_DIR_SIGNATURE          0x06064b50
/* Signature used for identifying the start of "zip64 end of central directory locator", in the zip file (with zip64 extension)*/
#define ZIP64_END_OF_CENTRAL_DIR_LOCATOR_SIGNATURE  0x07064b50
/* Signature used for identifying the start of archive extra data, in the zip file*/
#define ARCHIVE_EXTRA_DATA_SIGNATURE                0x08064b50

/* Size of extra data field if archive is not aligned and no extra data is present. */
#define ZIP_NO_EXTRADATA   0
/* Size of extra data field if archive is aligned in old archive not created with zip64. */
#define ZIP_EXTRADATA_PADDING_LENGTHEXTENSION      2
/* Size of extra data field if archive is aligned and created with zip64 */
#define ZIP64_EXTRADATA_PADDING_LENGTHEXTENSION    6
/* Size of extra data field in CDR if file offset is less than 4GB, file size is larger than 4GB and no alignment record present */
#define ZIP64_EXTRADATA_LENGTHEXTENSION                20
/* Size of extra data field in CDR if file offset is larger than 4GB, file size is less than 4GB and no alignment record present */
#define ZIP64_EXTRADATA_LENGTHEXTENSION_1              12
/* Size of extra data field in CDR if file offset is larger than 4GB, file size is larger than 4GB and no alignment record present */
#define ZIP64_EXTRADATA_LENGTHEXTENSION_2              28
/* Size of extra data field in CDR if file offset is less than 4GB, file size is larger than 4GB and if file is aligned */
#define ZIP64_EXTRADATA_AND_PADDING_LENGTHEXTENSION    26
/* Size of extra data field in CDR if file offset is larger than 4GB, file size is less than 4GB and if file is aligned */
#define ZIP64_EXTRADATA_AND_PADDING_LENGTHEXTENSION_1  18
/* Size of extra data field in CDR if file offset is larger than 4GB, file size is larger than 4GB and if file is aligned */
#define ZIP64_EXTRADATA_AND_PADDING_LENGTHEXTENSION_2  34

/* Header Id for extra data. */
#define EXTRAFIELD_HEADER_ID         0x0001
/* Header Id for alignment in the extra data. */
#define EXTRAFIELD_PADDING_HEADER_ID 0x0002
/* Size of header Id ad size fields in extra data. */
#define EXTRAFIELD_HEADERLENGTH      4

#include "c_prepack.h"

/*
 * Structure of file header in zip archive.
 *
 * This is the data that is placed right in front of the file data in the zip archive.
 * Not to be confused with FileHeader_t.
 */
typedef PRE_PACKED struct {
    /*
     * Used for verification of the local header,
     * the correct signature is LOCAL_FILE_HEADER_SIGNATURE
     */
    uint32 LocalFileHeaderSignature;
    /*
     * Zip version needed to extract.
     * The minimum supported ZIP specification.
     * version needed to extract the file.
     */
    uint16 VersionNeededToExtract;
    /* Bit flags with used for enabling/disabling of some options*/
    uint16 GeneralPurposeBitFlag;
    /* Idicates which compression method is used*/
    uint16 CompressionMethod;
    /* Time when the file was last modified, standard MS-DOS format*/
    uint16 LastModFileTime;
    /* Date when the file was last modified, standard MS-DOS format*/
    uint16 LastModFileDate;
    /* CRC for verification of data*/
    uint32 CRC32;
    /* Compressed Size of the file*/
    uint32 CompressedSize;
    /* Uncompressed Size of the file*/
    uint32 UncompressedSize;
    /* Length of the FileName_p*/
    uint16 FileNameLength;
    /* Length of the ExtraField_p*/
    uint16 ExtraFieldLength;
    /* Name of the file, including the directory structure*/
    uint8 *FileName;
    /* Extra field containing additional information, such as size of file for Zip64 archive*/
    uint8 *ExtraField;
} LocalFileHeader_t;

/*
 * File Header in the Central Directory.
 *
 * Structure of the File Header that is placed in
 * the Central Directory. Information that we need
 * for files is extracted from this structure when parsing the file.
 */
typedef PRE_PACKED struct {
    /* Used for verification of the File Header, the correct signature is FILE_HEADER_SIGNATURE*/
    uint32 CentralFileHeaderSignature;
    /* Version of ZIP format used for creating the zip file*/
    uint16 VersionMadeBy;
    /* The minimum supported ZIP specification version needed to extract the file*/
    uint16 VersionNeededToExtract;
    /* Flags for enabling/disabling different options, more details can be find in the Zip specification*/
    uint16 GeneralPurposeBitFlag;
    /* Indicates the compression method used for compressing the file, since we don't use compression this should be zero*/
    uint16 CompressionMethod;
    /* Time when the file was last modified, standard MS-DOS format*/
    uint16 LastModFileTime;
    /* Date when the file was last modified, standard MS-DOS format*/
    uint16 LastModFileDate;
    /* CRC for verification of data*/
    uint32 CRC32;
    /* Compressed Size of the file*/
    uint32 CompressedSize;
    /* Uncompressed Size fo the file*/
    uint32 UncompressedSize;
    /* Length of the FileName_p*/
    uint16 FileNameLength;
    /* Length of the ExtraField_p*/
    uint16 ExtraFieldLength;
    /* Length of file comment should be zero since we don't support comments*/
    uint16 FileCommentLength;
    /* The number of the disk on which this file begins, should be zero, since we don't support multi volume archives*/
    uint16 DiskNumberStart;
    /* The zip parsers doesn't need this. For details see the Zip Specification*/
    uint16 InternalFileAttributes;
    /* The zip parsers doesn't need this. For details see the Zip Specification*/
    uint32 ExternalFileAttributes;
    /*
     * Offset of local header. If this field is 0xFFFFFFFF the
     * offset is in the corresponding 8 byte zip64 extended information extra field.
     */
    uint32 RelativeOffsetOfLocalHeader;
    /* Name of the file, including the directory structure*/
    uint8 *FileName_p;
    /* Extra field containing additional information, such as size of file for Zip64 archive*/
    uint8 *ExtraField_p;
} FileHeader_t;

/*
 * Holds information about a file in the zip archive.
 *
 * Holds the information that are needed for accessing a file
 * in the zip archive.
 */
typedef PRE_PACKED struct {
    /* Size of the file in the zip archive. This is Uncompressed size, we don't use compresion.*/
    uint64 Size;
    /* Length of the file name in bytes, including the path.*/
    uint16 FileNameLength;
    /* Offset of the file starting from the beginning of the archive.*/
    uint64 Offset;
    /* Pointer to the File Name.*/
    char *FileName_p;
} ZipFileHeader_t;

/*
 * Information for all files in the zip archive.
 *
 * Holds information for every file in the zip archive.
 * When accessing file from the zip archive, we search
 * for the file in this structure.
 */
typedef PRE_PACKED struct {
    /* Total number of entries is equal to the total number of files in the archive.*/
    uint64 TotalNrOfEntries;
    /* Pointer to array of ZipFileHeader_t, The number of elements in the array is TotalNrOfEntries.*/
    ZipFileHeader_t *ZipFileHeaders_p;
} CentralDirectory_t;

/*
 * End Of Central Directory.
 *
 * When not using the Zip64 format, all the information
 * needed for the Central Directory is in this structure.
 * The Zip file comment is not supported.
 *
 * When Zip64 is used additional information are placed
 * in Zip64 End of CD Locator and Zip64 End of CD Record.
 * See below structures for detail information.
 */
typedef PRE_PACKED struct {
    /* Used for verification of the EndOfCDRecord_t, the correct signature is END_OF_CENTRAL_DIR_SIGNATURE.*/
    uint32 EndOfCDSignature;
    /* Should be zero, we don't support multi volumes archives.*/
    uint16 NrOfThisDisk;
    /* Should be zero, we don't support multi volumes archives.*/
    uint16 NrOfTheDiskWithTheStartOfTheCD;
    /* Number of entries in the Central Directory on this disk. it should be the same as TotalNrOfEntriesInTheCD, since we don't support multi volume archives.*/
    uint16 TotalNrOfEntriesInTheCDOnThisDisk;
    /* Total number of entries in the Central Directory is equal to the total number of files in the archive.*/
    uint16 TotalNrOfEntriesInTheCD;
    /* Size of the Central Directory.*/
    uint32 SizeOfTheCD;
    /* Offset of start of Central Directory, from the beginning of the Zip Archive.*/
    uint32 OffsetOfStartOfCD;
    /* Length of the Zip Archive Comment, it should be zero, because we don't support zip comments.*/
    uint16 ZIPFileCommentLength;
} EndOfCDRecord_t;

/*
 * Structure of Zip64 end of central directory locator in zip archive.
 */
typedef PRE_PACKED struct {
    /* Used for verification of the Zip64EndOfCDLocator_t, the correct signature is ZIP64_END_OF_CENTRAL_DIR_LOCATOR_SIGNATURE. */
    uint32 Zip64EndOfCDLocatorSignature;
    /* Should be zero, we don't support multi volumes archives.*/
    uint32 Zip64NrOfTheDiskWithTheStartOfTheEndOfCD;
    /* Zip64 Relative Offset Of The End Of The Central Directory. */
    uint64 Zip64RelativeOffsetOfTheEndOfCD;
    /* Should be zero, we don't support multi volumes archives.*/
    uint32 Zip64TotalNrOfDisks;
} Zip64EndOfCDLocator_t;

/*
 * Structure of Zip64 end of central directory record in zip archive.
 */
typedef PRE_PACKED struct {
    /* Used for verification of the Zip64EndOfCDRecord_t, the correct signature is ZIP64_END_OF_CENTRAL_DIR_SIGNATURE. */
    uint32 Zip64EndOfCDSignature;
    /* Size of the ZIP64 End Of Central Directory Record. Should be the size of the remaining
       record and should not include the leading 12 bytes. */
    uint64 Zip64EndOfCDSize;
    /* Version of ZIP format used for creating the zip file. */
    uint16 Zip64VersionMadeBy;
    /* The minimum supported ZIP specification version needed to extract the file. */
    uint16 Zip64VersionNeededToExtract;
    /* Should be zero, we don't support multi volumes archives.*/
    uint32 Zip64NrOfThisDisk;
    /* Should be zero, we don't support multi volumes archives. */
    uint32 Zip64NrOfTheDiskWithTheStartOfTheCD;
    /* Number of entries in the Central Directory on this disk. it should be the same as Zip64TotalNrOfEntriesInTheCD, since we don't support multi volume archives. */
    uint64 Zip64TotalNrOfEntriesInCDOnThisDisk;
    /* Total number of entries in the Central Directory is equal to the total number of files in the archive. */
    uint64 Zip64TotalNrOfEntriesInTheCD;
    /* Size of the CentralDirectory. */
    uint64 Zip64SizeOfTheCD;
    /* Offset of start of Central Directory, with respect to the starting disk number. */
    uint64 Zip64OffsetOfStartOfCD;
    /* Zip64 Extensible Data Sector. */
    uint8 *Zip64ExtensibleDataSector;
} Zip64EndOfCDRecord_t;

#include "c_postpack.h"
//#pragma pack(pop)

/*
 * Holds information about a zip archive.
 *
 * It contains all the necessary information about a zip archive.
 */
typedef struct {
    /* IO_Handle to a zip file.*/
    IO_Handle_t             ReadHandle;
    /* Mode in which files will be opened from zip archive.*/
    ZipOpenFileMode_t       ZipOpenFileMode;
    /* Structure containing information for every file in the archive.*/
    CentralDirectory_t      CentralDirectory;
    /* Used to determine whether zip64 is used or standard zip.*/
    boolean                 ZIP64;
    /* Offset of the Zip64 End of Central Directory Record, from the beginning of the zip archive.*/
    uint64                  OffsetOfCDRecordZip64;
    /* Offset of the Central Directory from the beginning of the zip archive.*/
    uint64                  OffsetOfCDZip64;
    /* Size of the Central directory.*/
    uint64                  SizeOfCDZip64;
    /* Bytes to be read from the zip file.*/
    uint64                  BytesToRead;
    /* Offset from where to read data.*/
    uint64                  Offset;
    /* IO vector used for reading data.*/
    IO_Vector_t             Vector;
    /* VerifyContext to pass to secure wrapper.*/
    VerifyContext_t *VerifyContext_p;
} ZipHandle_t;

/***********************************************************************
 * Declaration of file local functions
 **********************************************************************/
static ErrorCode_e ZipInitEndOfCD_Transfer(IO_Handle_t ReadHandle, ZipHandle_t *ZipHandle_p, ZipState_t *State_p);
static ErrorCode_e ZipProcessEndOfCD(IO_Handle_t ReadHandle, ZipHandle_t *ZipHandle_p, ZipState_t *State_p);
static ErrorCode_e Zip64ProcessEndOfCDL(IO_Handle_t ReadHandle, ZipHandle_t *ZipHandle_p, ZipState_t *State_p);
static ErrorCode_e Zip64ProcessEndOfCDR(IO_Handle_t ReadHandle, ZipHandle_t *ZipHandle_p, ZipState_t *State_p);
static ErrorCode_e ZipInitCD(IO_Handle_t ReadHandle, ZipHandle_t *ZipHandle_p, ZipState_t *State_p);
static ErrorCode_e ZipProcessCD(IO_Handle_t ReadHandle, ZipHandle_t *ZipHandle_p, ZipState_t *State_p);

static sint32 ParseEndOfCDRecord(const EndOfCDRecord_t *const EndOfCDRecord_p);
static sint32 ParseZip64EndOfCDLocator(const Zip64EndOfCDLocator_t *const Zip64EndOfCDLocator_p);
static sint32 ParseZip64EndOfCDRecord(const Zip64EndOfCDRecord_t *const Zip64EndOfCDRecord_p);
static sint32 ParseCentralDirectoryZIP64(const ZipHandle_t *const ZipHandle_p, const uint8 *Data_p, uint64 DataLength);

static void DeserializeEndOfCDRecord(EndOfCDRecord_t *EndOfCDRecord_p, void *Data_p);
static void DeserializeZip64EndOfCDLocator(Zip64EndOfCDLocator_t *Zip64EndOfCDLocator_p, void *Data_p);
static void DeserializeZip64EndOfCDRecord(Zip64EndOfCDRecord_t *Zip64EndOfCDRecord_p, void *Data_p);

#ifdef PRINT_LEVEL_C_
static uint32 PrintEndOfCDRecord(const EndOfCDRecord_t *const EndOfCDRecord_p);
static uint32 PrintZip64EndOfCDLocator(const Zip64EndOfCDLocator_t *const Zip64EndOfCDLocator_p);
static uint32 PrintZip64EndOfCDRecord(const Zip64EndOfCDRecord_t *const Zip64EndOfCDRecord_p);
#endif

/***********************************************************************
 * Definition of internal functions
 **********************************************************************/
static ErrorCode_e ZipInitEndOfCD_Transfer(IO_Handle_t ReadHandle, ZipHandle_t *ZipHandle_p, ZipState_t *State_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_ZIP_ERROR;
    uint64 Length = 0;

    Length = Do_IO_GetLength(ReadHandle);
    VERIFY(IO_ERROR != Length, E_IO_FAILED_TO_GET_LENGTH);

    switch (*State_p) {
    case ZIP_INIT_EOCDR:
        ZipHandle_p->Offset = Length - sizeof(EndOfCDRecord_t);
        ZipHandle_p->BytesToRead = sizeof(EndOfCDRecord_t);
        *State_p = ZIP_PROCESSING_EOCDR;
        break;

    case ZIP64_INIT_EOCD_LOCATOR:
        ZipHandle_p->Offset = Length - (sizeof(EndOfCDRecord_t) + sizeof(Zip64EndOfCDLocator_t));
        ZipHandle_p->BytesToRead = sizeof(EndOfCDRecord_t) + sizeof(Zip64EndOfCDLocator_t);
        *State_p = ZIP64_PROCESSING_EOCD_LOCATOR;
        break;

    case ZIP64_INIT_EOCD_RECORD:
        ZipHandle_p->Offset = ZipHandle_p->OffsetOfCDRecordZip64;
        ZipHandle_p->BytesToRead = Length - ZipHandle_p->OffsetOfCDRecordZip64;
        *State_p = ZIP64_PROCESSING_EOCD_RECORD;
        break;

    default:
        A_(printf("zip_parser.c (%d): Unknown State\n", __LINE__);)
        ReturnValue = E_GENERAL_ZIP_ERROR;
        goto ErrorExit;
    }

    ZipHandle_p->Vector = Do_IO_CreateVector(ReadHandle, ZipHandle_p->BytesToRead, ZipHandle_p->BytesToRead);
    VERIFY(IO_VECTOR_INVALID_HANDLE != ZipHandle_p->Vector, E_VECTOR_CREATE_FAIL);

    ReturnValue = E_SUCCESS;
ErrorExit:
    return ReturnValue;
}

static void DeserializeEndOfCDRecord(EndOfCDRecord_t *EndOfCDRecord_p, void *Data_p)
{
    void *Iter_p = Data_p;

    EndOfCDRecord_p->EndOfCDSignature                  = get_uint32_le(&Iter_p);
    EndOfCDRecord_p->NrOfThisDisk                      = get_uint16_le(&Iter_p);
    EndOfCDRecord_p->NrOfTheDiskWithTheStartOfTheCD    = get_uint16_le(&Iter_p);
    EndOfCDRecord_p->TotalNrOfEntriesInTheCDOnThisDisk = get_uint16_le(&Iter_p);
    EndOfCDRecord_p->TotalNrOfEntriesInTheCD           = get_uint16_le(&Iter_p);
    EndOfCDRecord_p->SizeOfTheCD                       = get_uint32_le(&Iter_p);
    EndOfCDRecord_p->OffsetOfStartOfCD                 = get_uint32_le(&Iter_p);
    EndOfCDRecord_p->ZIPFileCommentLength              = get_uint16_le(&Iter_p);
}

static ErrorCode_e ZipProcessEndOfCD(IO_Handle_t ReadHandle, ZipHandle_t *ZipHandle_p, ZipState_t *State_p)
{
    ErrorCode_e ReturnValue = E_SUCCESS;
    uint64 BytesRead = 0;

    BytesRead = Do_IO_ScatteredPositionedRead(ReadHandle, ZipHandle_p->Vector, ZipHandle_p->BytesToRead, ZipHandle_p->Offset);
    VERIFY(IO_ERROR != BytesRead, IO_ErrorCode);

    if (BytesRead == ZipHandle_p->BytesToRead) {
        EndOfCDRecord_t EndOfCDRecord = {0};

        DeserializeEndOfCDRecord(&EndOfCDRecord, (uint8 *)VectorList[ZipHandle_p->Vector].Entries[0].Buffer_p + VectorList[ZipHandle_p->Vector].IOffset);

        if (E_SUCCESS != ParseEndOfCDRecord(&EndOfCDRecord)) {
            A_(printf("zip_parser.c (%d): Error parsing End of CD Record\n", __LINE__);)
            ReturnValue = (ErrorCode_e)Do_IO_DestroyVector(ZipHandle_p->Vector);
            VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
            return E_GENERAL_ZIP_ERROR;
        }

#ifdef PRINT_LEVEL_C_
        ReturnValue = (ErrorCode_e)PrintEndOfCDRecord(&EndOfCDRecord);
#endif

        ZipHandle_p->CentralDirectory.TotalNrOfEntries = EndOfCDRecord.TotalNrOfEntriesInTheCD;
        ReturnValue = (ErrorCode_e)Do_IO_DestroyVector(ZipHandle_p->Vector);

        if (0xffffffff == EndOfCDRecord.OffsetOfStartOfCD) {
            //The archive is bigger than 4GB, zip64 will be used to parse the fields.
            *State_p = ZIP64_INIT_EOCD_LOCATOR;
        } else {
            ZipHandle_p->OffsetOfCDZip64 = EndOfCDRecord.OffsetOfStartOfCD;
            ZipHandle_p->SizeOfCDZip64 = EndOfCDRecord.SizeOfTheCD;
            *State_p = ZIP_INIT_CD;
        }

    }

ErrorExit:
    return ReturnValue;
}

static void DeserializeZip64EndOfCDLocator(Zip64EndOfCDLocator_t *Zip64EndOfCDLocator_p, void *Data_p)
{
    void *Iter_p = Data_p;

    Zip64EndOfCDLocator_p->Zip64EndOfCDLocatorSignature             = get_uint32_le(&Iter_p);
    Zip64EndOfCDLocator_p->Zip64NrOfTheDiskWithTheStartOfTheEndOfCD = get_uint32_le(&Iter_p);
    Zip64EndOfCDLocator_p->Zip64RelativeOffsetOfTheEndOfCD          = get_uint64_le(&Iter_p);
    Zip64EndOfCDLocator_p->Zip64TotalNrOfDisks                      = get_uint32_le(&Iter_p);
}

static ErrorCode_e Zip64ProcessEndOfCDL(IO_Handle_t ReadHandle, ZipHandle_t *ZipHandle_p, ZipState_t *State_p)
{
    ErrorCode_e ReturnValue = E_SUCCESS;
    uint64 BytesRead = 0;

    BytesRead = Do_IO_ScatteredPositionedRead(ReadHandle, ZipHandle_p->Vector, ZipHandle_p->BytesToRead, ZipHandle_p->Offset);
    VERIFY(IO_ERROR != BytesRead, IO_ErrorCode);

    if (BytesRead == ZipHandle_p->BytesToRead) {
        Zip64EndOfCDLocator_t Zip64EndOfCDLocator = {0};

        DeserializeZip64EndOfCDLocator(&Zip64EndOfCDLocator, (uint8 *)VectorList[ZipHandle_p->Vector].Entries[0].Buffer_p + VectorList[ZipHandle_p->Vector].IOffset);

        if (E_SUCCESS != ParseZip64EndOfCDLocator(&Zip64EndOfCDLocator)) {
            A_(printf("zip_parser.c (%d): Error parsing Zip64 End of CD Locator\n", __LINE__);)
            ReturnValue = (ErrorCode_e)Do_IO_DestroyVector(ZipHandle_p->Vector);
            VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
            return E_GENERAL_ZIP_ERROR;
        }

#ifdef PRINT_LEVEL_C_
        ReturnValue = (ErrorCode_e)PrintZip64EndOfCDLocator(&Zip64EndOfCDLocator);
#endif

        ReturnValue = (ErrorCode_e)Do_IO_DestroyVector(ZipHandle_p->Vector);
        ZipHandle_p->OffsetOfCDRecordZip64 = Zip64EndOfCDLocator.Zip64RelativeOffsetOfTheEndOfCD;
        *State_p = ZIP64_INIT_EOCD_RECORD;
    }

ErrorExit:
    return ReturnValue;
}

static void DeserializeZip64EndOfCDRecord(Zip64EndOfCDRecord_t *Zip64EndOfCDRecord_p, void *Data_p)
{
    void *Iter_p = Data_p;

    Zip64EndOfCDRecord_p->Zip64EndOfCDSignature               = get_uint32_le(&Iter_p);
    Zip64EndOfCDRecord_p->Zip64EndOfCDSize                    = get_uint64_le(&Iter_p);
    Zip64EndOfCDRecord_p->Zip64VersionMadeBy                  = get_uint16_le(&Iter_p);
    Zip64EndOfCDRecord_p->Zip64VersionNeededToExtract         = get_uint16_le(&Iter_p);
    Zip64EndOfCDRecord_p->Zip64NrOfThisDisk                   = get_uint32_le(&Iter_p);
    Zip64EndOfCDRecord_p->Zip64NrOfTheDiskWithTheStartOfTheCD = get_uint32_le(&Iter_p);
    Zip64EndOfCDRecord_p->Zip64TotalNrOfEntriesInCDOnThisDisk = get_uint64_le(&Iter_p);
    Zip64EndOfCDRecord_p->Zip64TotalNrOfEntriesInTheCD        = get_uint64_le(&Iter_p);
    Zip64EndOfCDRecord_p->Zip64SizeOfTheCD                    = get_uint64_le(&Iter_p);
    Zip64EndOfCDRecord_p->Zip64OffsetOfStartOfCD              = get_uint64_le(&Iter_p);
}

static ErrorCode_e Zip64ProcessEndOfCDR(IO_Handle_t ReadHandle, ZipHandle_t *ZipHandle_p, ZipState_t *State_p)
{
    ErrorCode_e ReturnValue = E_SUCCESS;
    uint64 BytesRead = 0;

    BytesRead = Do_IO_ScatteredPositionedRead(ReadHandle, ZipHandle_p->Vector, ZipHandle_p->BytesToRead, ZipHandle_p->Offset);
    VERIFY(IO_ERROR != BytesRead, IO_ErrorCode);

    if (BytesRead == ZipHandle_p->BytesToRead) {
        Zip64EndOfCDRecord_t Zip64EndOfCDRecord = {0};

        DeserializeZip64EndOfCDRecord(&Zip64EndOfCDRecord, (uint8 *)VectorList[ZipHandle_p->Vector].Entries[0].Buffer_p + VectorList[ZipHandle_p->Vector].IOffset);

        if (E_SUCCESS != ParseZip64EndOfCDRecord(&Zip64EndOfCDRecord)) {
            A_(printf("zip_parser.c (%d): Error parsing Zip64 End of CD Record\n", __LINE__);)
            ReturnValue = (ErrorCode_e)Do_IO_DestroyVector(ZipHandle_p->Vector);
            VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
            return E_GENERAL_ZIP_ERROR;
        }

#ifdef PRINT_LEVEL_C_
        ReturnValue = (ErrorCode_e)PrintZip64EndOfCDRecord(&Zip64EndOfCDRecord);
#endif

        ReturnValue = (ErrorCode_e)Do_IO_DestroyVector(ZipHandle_p->Vector);

        ZipHandle_p->OffsetOfCDZip64 = Zip64EndOfCDRecord.Zip64OffsetOfStartOfCD;
        ZipHandle_p->SizeOfCDZip64 = Zip64EndOfCDRecord.Zip64SizeOfTheCD;
        *State_p = ZIP_INIT_CD;
    }

ErrorExit:
    return ReturnValue;
}

static ErrorCode_e ZipInitCD(IO_Handle_t ReadHandle, ZipHandle_t *ZipHandle_p, ZipState_t *State_p)
{
    ErrorCode_e ReturnValue = E_SUCCESS;

    ZipHandle_p->Offset = ZipHandle_p->OffsetOfCDZip64;
    ZipHandle_p->BytesToRead = ZipHandle_p->SizeOfCDZip64;

    ZipHandle_p->Vector = Do_IO_CreateVector(ReadHandle, ZipHandle_p->BytesToRead, ZipHandle_p->BytesToRead);
    VERIFY(IO_VECTOR_INVALID_HANDLE != ZipHandle_p->Vector, E_VECTOR_CREATE_FAIL);

    // Check if the product of multiplying TotalNrOfEntries and sizeof(ZipFileHeader_t) will not exceed limits of uint32
    VERIFY(ZipHandle_p->CentralDirectory.TotalNrOfEntries <= (0xffffffff / sizeof(ZipFileHeader_t)), E_GENERAL_ZIP_ERROR);

    ZipHandle_p->CentralDirectory.ZipFileHeaders_p = (ZipFileHeader_t *)malloc((uint32)ZipHandle_p->CentralDirectory.TotalNrOfEntries * sizeof(ZipFileHeader_t));
    ASSERT(NULL != ZipHandle_p->CentralDirectory.ZipFileHeaders_p);

    *State_p = ZIP_PROCESSING_CD;

ErrorExit:
    return ReturnValue;
}

static ErrorCode_e ZipProcessCD(IO_Handle_t ReadHandle, ZipHandle_t *ZipHandle_p, ZipState_t *State_p)
{
    ErrorCode_e ReturnValue = E_SUCCESS;
    uint64 BytesRead;

    BytesRead = Do_IO_ScatteredPositionedRead(ReadHandle, ZipHandle_p->Vector, ZipHandle_p->BytesToRead, ZipHandle_p->Offset);
    VERIFY(IO_ERROR != BytesRead, IO_ErrorCode);

    if (BytesRead == ZipHandle_p->BytesToRead) {
        uint8 *Data_p;
        Data_p = (uint8 *)((uint8 *)VectorList[ZipHandle_p->Vector].Entries[0].Buffer_p + VectorList[ZipHandle_p->Vector].IOffset);

        if (E_SUCCESS != ParseCentralDirectoryZIP64(ZipHandle_p, Data_p, BytesRead)) {
            A_(printf("zip_parser.c (%d): Error parsing ZIP64 End of CD Record\n", __LINE__);)
            ReturnValue = (ErrorCode_e)Do_IO_DestroyVector(ZipHandle_p->Vector);
            VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
            return E_GENERAL_ZIP_ERROR;
        }

        ReturnValue = (ErrorCode_e)Do_IO_DestroyVector(ZipHandle_p->Vector);
        *State_p = ZIP_FINISHED;
    }

ErrorExit:
    return ReturnValue;
}

static sint32 ParseEndOfCDRecord(const EndOfCDRecord_t *const EndOfCDRecord_p)
{
    if (NULL == EndOfCDRecord_p) {
        A_(printf("zip_parser.c (%d): EndOfCDRecord_p is NULL\n", __LINE__);)
        return ZIP_ERROR;
    }

    if (END_OF_CENTRAL_DIR_SIGNATURE != EndOfCDRecord_p->EndOfCDSignature) {
        A_(printf("zip_parser.c (%d): The signature is not correct.\n", __LINE__);)
        return ZIP_ERROR;
    }

    if (0 != EndOfCDRecord_p->NrOfTheDiskWithTheStartOfTheCD) {
        A_(printf("zip_parser.c (%d): Number of the disk with the start of the CD should be zero\n", __LINE__);)
        return ZIP_ERROR;
    }

    if (0 != EndOfCDRecord_p->NrOfThisDisk) {
        A_(printf("zip_parser.c (%d): Number of this disk should be zero\n", __LINE__);)
        return ZIP_ERROR;
    }

    if (0 != EndOfCDRecord_p->ZIPFileCommentLength) {
        A_(printf("zip_parser.c (%d): The comment length should be zero\n", __LINE__);)
        return ZIP_ERROR;
    }

    return E_SUCCESS;
}

static sint32 ParseZip64EndOfCDLocator(const Zip64EndOfCDLocator_t *const Zip64EndOfCDLocator_p)
{
    if (NULL == Zip64EndOfCDLocator_p) {
        A_(printf("zip_parser.c (%d): Zip64EndOfCDLocator_p is NULL\n", __LINE__);)
        return ZIP_ERROR;
    }

    if (ZIP64_END_OF_CENTRAL_DIR_LOCATOR_SIGNATURE != Zip64EndOfCDLocator_p->Zip64EndOfCDLocatorSignature) {
        A_(printf("zip_parser.c (%d): The signature is not correct\n", __LINE__);)
        return ZIP_ERROR;
    }

    return E_SUCCESS;
}

static sint32 ParseZip64EndOfCDRecord(const Zip64EndOfCDRecord_t *const Zip64EndOfCDRecord_p)
{
    if (NULL == Zip64EndOfCDRecord_p) {
        A_(printf("zip_parser.c (%d): Zip64EndOfCDRecord_p is NULL\n", __LINE__);)
        return ZIP_ERROR;
    }

    if (ZIP64_END_OF_CENTRAL_DIR_SIGNATURE != Zip64EndOfCDRecord_p->Zip64EndOfCDSignature) {
        A_(printf("zip_parser.c (%d): The signature is not correct\n", __LINE__);)
        return ZIP_ERROR;
    }

    return E_SUCCESS;
}

#ifdef PRINT_LEVEL_C_
static uint32 PrintEndOfCDRecord(const EndOfCDRecord_t *const EndOfCDRecord_p)
{
    printf("*******************************************************************\n");
    printf("EndOfCDSignature = 0x%08lX\n", EndOfCDRecord_p->EndOfCDSignature);
    printf("NrOfThisDisk = 0x%04X\n", EndOfCDRecord_p->NrOfThisDisk);
    printf("NrOfTheDiskWithTheStartOfTheCD = 0x%04X\n", EndOfCDRecord_p->NrOfTheDiskWithTheStartOfTheCD);
    printf("TotalNrOfEntriesInTheCDOnThisDisk = 0x%04X\n", EndOfCDRecord_p->TotalNrOfEntriesInTheCDOnThisDisk);
    printf("TotalNrOfEntriesInTheCD = 0x%04X\n", EndOfCDRecord_p->TotalNrOfEntriesInTheCD);
    printf("SizeOfTheCD = 0x%08lX\n", EndOfCDRecord_p->SizeOfTheCD);
    printf("OffsetOfStartOfCD = 0x%08lX\n", EndOfCDRecord_p->OffsetOfStartOfCD);
    printf("ZIPFileCommentLength = 0x%04X\n", EndOfCDRecord_p->ZIPFileCommentLength);
    printf("*******************************************************************\n");
    return 0;
}

static uint32 PrintZip64EndOfCDLocator(const Zip64EndOfCDLocator_t *const Zip64EndOfCDLocator_p)
{
    printf("*******************************************************************\n");
    printf("Zip64EndOfCDLocatorSignature = 0x%08lX\n", Zip64EndOfCDLocator_p->Zip64EndOfCDLocatorSignature);
    printf("Zip64NrOfTheDiskWithTheStartOfTheEndOfCD = 0x%08lX\n", Zip64EndOfCDLocator_p->Zip64NrOfTheDiskWithTheStartOfTheEndOfCD);
    printf("Zip64RelativeOffsetOfTheEndOfCD = 0x%08X%08X\n", (uint32)(Zip64EndOfCDLocator_p->Zip64RelativeOffsetOfTheEndOfCD >> 32), (uint32)(Zip64EndOfCDLocator_p->Zip64RelativeOffsetOfTheEndOfCD));
    printf("Zip64TotalNrOfDisks = 0x%08lX\n", Zip64EndOfCDLocator_p->Zip64TotalNrOfDisks);
    printf("*******************************************************************\n");
    return 0;
}

static uint32 PrintZip64EndOfCDRecord(const Zip64EndOfCDRecord_t *const Zip64EndOfCDRecord_p)
{
    printf("*******************************************************************\n");
    printf("Zip64EndOfCDSignature = 0x%08lX\n", Zip64EndOfCDRecord_p->Zip64EndOfCDSignature);
    printf("Zip64EndOfCDSize = 0x%08X%08X\n", (uint32)(Zip64EndOfCDRecord_p->Zip64EndOfCDSize >> 32), (uint32)(Zip64EndOfCDRecord_p->Zip64EndOfCDSize));
    printf("Zip64VersionMadeBy = 0x%04X\n", Zip64EndOfCDRecord_p->Zip64VersionMadeBy);
    printf("Zip64VersionNeededToExtract = 0x%04X\n", Zip64EndOfCDRecord_p->Zip64VersionNeededToExtract);
    printf("Zip64NrOfThisDisk = 0x%08lX\n", Zip64EndOfCDRecord_p->Zip64NrOfThisDisk);
    printf("Zip64NrOfTheDiskWithTheStartOfTheCD = 0x%08lX\n", Zip64EndOfCDRecord_p->Zip64NrOfTheDiskWithTheStartOfTheCD);
    printf("Zip64TotalNrOfEntriesInCDOnThisDisk = 0x%08X%08X\n", (uint32)(Zip64EndOfCDRecord_p->Zip64TotalNrOfEntriesInCDOnThisDisk >> 32), (uint32)(Zip64EndOfCDRecord_p->Zip64TotalNrOfEntriesInCDOnThisDisk));
    printf("Zip64TotalNrOfEntriesInTheCD = 0x%08X%08X\n", (uint32)(Zip64EndOfCDRecord_p->Zip64TotalNrOfEntriesInTheCD >> 32), (uint32)(Zip64EndOfCDRecord_p->Zip64TotalNrOfEntriesInTheCD));
    printf("Zip64SizeOfTheCD = 0x%08X%08X\n", (uint32)(Zip64EndOfCDRecord_p->Zip64SizeOfTheCD >> 32), (uint32)(Zip64EndOfCDRecord_p->Zip64SizeOfTheCD));
    printf("Zip64OffsetOfStartOfCD = 0x%08X%08X\n", (uint32)(Zip64EndOfCDRecord_p->Zip64OffsetOfStartOfCD >> 32), (uint32)(Zip64EndOfCDRecord_p->Zip64OffsetOfStartOfCD));
    printf("*******************************************************************\n");
    return 0;
}
#endif

static sint32 ParseCentralDirectoryZIP64(const ZipHandle_t *const ZipHandle_p, const uint8 *Data_p, uint64 DataLength)
{
    FileHeader_t *FileHeader_p = NULL;
    uint16 LocalExtraFieldLength = 0;
    uint16 TmpUint16 = 0;
    uint32 i = 0;
    uint32 TmpUint32 = 0;
    uint64 TmpUint64 = 0;

    if (NULL == Data_p) {
        A_(printf("zip_parser.c (%d): Data_p is NULL\n", __LINE__);)
        return ZIP_ERROR;
    }

    if (NULL == ZipHandle_p) {
        A_(printf("zip_parser.c (%d): ZipHandle_p is NULL\n", __LINE__);)
        return ZIP_ERROR;
    }

    if (ZipHandle_p->SizeOfCDZip64 > DataLength) {
        A_(printf("zip_parser.c (%d): The data length is not OK\n", __LINE__);)
        return ZIP_ERROR;
    }

    for (i = 0; i < ZipHandle_p->CentralDirectory.TotalNrOfEntries; i++) {
        FileHeader_p = (FileHeader_t *)Data_p;
        TmpUint32 = FileHeader_p->CentralFileHeaderSignature;

        if (CENTRAL_FILE_HEADER_SIGNATURE != TmpUint32) {
            A_(printf("zip_parser.c (%d): Not valid header signature\n", __LINE__);)
            return ZIP_ERROR;
        }

        if (0x0000 != FileHeader_p->DiskNumberStart) {
            A_(printf("zip_parser.c (%d): DiskNumberStart should be zero\n", __LINE__);)
            return ZIP_ERROR;
        }

        if (0 != FileHeader_p->FileCommentLength) {
            A_(printf("zip_parser.c (%d): File comment length is not zero\n", __LINE__);)
            return ZIP_ERROR;
        }

        ZipHandle_p->CentralDirectory.ZipFileHeaders_p[i].Size = FileHeader_p->UncompressedSize;
        ZipHandle_p->CentralDirectory.ZipFileHeaders_p[i].Offset = FileHeader_p->RelativeOffsetOfLocalHeader + sizeof(LocalFileHeader_t) - sizeof(uint8 *) - sizeof(uint8 *) + FileHeader_p->FileNameLength;
        ZipHandle_p->CentralDirectory.ZipFileHeaders_p[i].FileNameLength = FileHeader_p->FileNameLength;
        ZipHandle_p->CentralDirectory.ZipFileHeaders_p[i].FileName_p = (char *)malloc(FileHeader_p->FileNameLength + 1);
        ASSERT(NULL != ZipHandle_p->CentralDirectory.ZipFileHeaders_p[i].FileName_p);

        Data_p += sizeof(FileHeader_t) - sizeof(FileHeader_p->FileName_p) - sizeof(FileHeader_p->ExtraField_p);
        memcpy(ZipHandle_p->CentralDirectory.ZipFileHeaders_p[i].FileName_p, Data_p, FileHeader_p->FileNameLength);
        ZipHandle_p->CentralDirectory.ZipFileHeaders_p[i].FileName_p[ZipHandle_p->CentralDirectory.ZipFileHeaders_p[i].FileNameLength] = 0;

        C_(printf("zip_parser.c (%d): File = %s\n", __LINE__, ZipHandle_p->CentralDirectory.ZipFileHeaders_p[i].FileName_p);)
        C_(printf("zip_parser.c (%d): Extra Field Length = 0x%08x\n", __LINE__, FileHeader_p->ExtraFieldLength);)

        Data_p += ZipHandle_p->CentralDirectory.ZipFileHeaders_p[i].FileNameLength;

        /*
         * Because of performance reason only the central directory (CD) headers are used and not the local file header.
         * In order to allow different programs and different types of information to be stored in the 'extra' field in
         * .ZIP files, the following structure is used for all programs storing data in this field:
         *
         * header1+data1 + header2+data2 . . .
         *
         * Each header should consist of:
         *
         * Header ID - 2 bytes
         * Data Size - 2 bytes
         *
         *         Value      Size       Description
         *         -----      ----       -----------
         * (ZIP64) 0x0001     2 bytes    Tag for this "extra" block type
         *         Size       2 bytes    Size of this "extra" block
         *         Original
         *         Size       8 bytes    Original uncompressed file size
         *         Compressed
         *         Size       8 bytes    Size of compressed data
         *         Relative Header
         *         Offset     8 bytes    Offset of local header record
         *         Disk Start
         *         Number     4 bytes    Number of the disk on which
         *                               this file starts
         *
         *           Value      Size       Description
         *           -----      ----       -----------
         * (Padding) 0x0002     2 bytes    Tag for this "extra" block type
         *           Size       2 bytes    Size of this "extra" block
         *           Padding
         *           Size       2 bytes    Size of padding of the file due to alignment.
         *
         * This structure is used for support of zip64 where 'extra' field contains information for the extra data if the
         * archive is bigger then 4GB and/or length of the padding between the local header and the file data.
         *
         * For backward compatibility of old archive not created with zip64 there is a check that if the extra field length
         * is 2 bytes then it is used to show how much padding is done.
         */

        switch (FileHeader_p->ExtraFieldLength) {
        case ZIP_NO_EXTRADATA:
            /* Not aligned archive and no extra data present. Do nothing. */
            C_(printf("zip_parser.c (%d): No Extra Data and alignment present.\n", __LINE__);)
            C_(printf("zip_parser.c (%d): Uncompressed File Size = 0x%08x\n", __LINE__, FileHeader_p->UncompressedSize);)
            C_(printf("zip_parser.c (%d): Offset of Local Header Record = 0x%08x\n", __LINE__, ZipHandle_p->CentralDirectory.ZipFileHeaders_p[i].Offset);)
            break;

        case ZIP_EXTRADATA_PADDING_LENGTHEXTENSION:
            /* Aligned archive */
            C_(printf("zip_parser.c (%d): Alignment archive not created with zip64.\n", __LINE__);)

            memcpy(&TmpUint16, Data_p, sizeof(uint16));

            ZipHandle_p->CentralDirectory.ZipFileHeaders_p[i].Offset += TmpUint16;

            Data_p += FileHeader_p->ExtraFieldLength;

            C_(printf("zip_parser.c (%d): Uncompressed File Size = 0x%08x\n", __LINE__, FileHeader_p->UncompressedSize);)
            C_(printf("zip_parser.c (%d): Offset of Local Header Record = 0x%08x\n", __LINE__, ZipHandle_p->CentralDirectory.ZipFileHeaders_p[i].Offset);)
            break;

        case ZIP64_EXTRADATA_PADDING_LENGTHEXTENSION:
            /* Aligned archive created with zip64. */
            C_(printf("zip_parser.c (%d): Only alignment present in extra data.\n", __LINE__);)

            memcpy(&TmpUint16, Data_p, sizeof(uint16));

            if (EXTRAFIELD_PADDING_HEADER_ID != TmpUint16) { //*((uint16 *)Data_p))
                A_(printf("zip_parser.c (%d): Not a valid ID\n", __LINE__);)
                return ZIP_ERROR;
            }

            Data_p += sizeof(uint16);

            //Extra Data Size
            memcpy(&TmpUint16, Data_p, sizeof(uint16));

            if (sizeof(uint16) != TmpUint16) {
                /* The data from this extra field have to be uint16 where the padding value is placed. */
                A_(printf("zip_parser.c (%d): Not a valid size\n", __LINE__);)
                return ZIP_ERROR;
            }

            Data_p += sizeof(uint16);

            //Padding size
            memcpy(&TmpUint16, Data_p, sizeof(uint16));
            ZipHandle_p->CentralDirectory.ZipFileHeaders_p[i].Offset += TmpUint16;//(*((uint16 *)Data_p));
            Data_p += sizeof(uint16);

            C_(printf("zip_parser.c (%d): Uncompressed File Size = 0x%08x\n", __LINE__, FileHeader_p->UncompressedSize);)
            C_(printf("zip_parser.c (%d): Offset of Local Header Record = 0x%08x\n", __LINE__, ZipHandle_p->CentralDirectory.ZipFileHeaders_p[i].Offset);)

            break;

        case ZIP64_EXTRADATA_LENGTHEXTENSION:
        case ZIP64_EXTRADATA_LENGTHEXTENSION_1:
        case ZIP64_EXTRADATA_LENGTHEXTENSION_2:
        case ZIP64_EXTRADATA_AND_PADDING_LENGTHEXTENSION:
        case ZIP64_EXTRADATA_AND_PADDING_LENGTHEXTENSION_1:
        case ZIP64_EXTRADATA_AND_PADDING_LENGTHEXTENSION_2:
            C_(printf("zip_parser.c (%d): Extra data present.\n", __LINE__);)

            LocalExtraFieldLength = 0;

            memcpy(&TmpUint16, Data_p, sizeof(uint16));

            if (EXTRAFIELD_HEADER_ID != TmpUint16) { //*((uint16 *)Data_p))
                A_(printf("zip_parser.c (%d): Not a valid ID\n", __LINE__);)
                return ZIP_ERROR;
            }

            Data_p += sizeof(uint16);
            LocalExtraFieldLength += sizeof(uint16);

            //Extra Data Size
            memcpy(&TmpUint16, Data_p, sizeof(uint16));

            if ((TmpUint16 + EXTRAFIELD_HEADERLENGTH) > FileHeader_p->ExtraFieldLength) {
                A_(printf("zip_parser.c (%d): Inconsistent size of Extra Field.\n", __LINE__);)
                return ZIP_ERROR;
            }

            Data_p += sizeof(uint16);
            LocalExtraFieldLength += sizeof(uint16);

            if (0xffffffff == FileHeader_p->UncompressedSize) {
                /* If file is larger than 4GB its original size is placed in the Zip64 extra data field. */

                //Original Uncompressed File Size
                memcpy(&TmpUint64, Data_p, sizeof(uint64));
                ZipHandle_p->CentralDirectory.ZipFileHeaders_p[i].Size = TmpUint64;//(*((uint64 *)Data_p));

                C_(printf("zip_parser.c (%d): Uncompressed File Size = 0x%08X%08X\n", __LINE__, (uint32)(TmpUint64 >> 32), (uint32)(TmpUint64));)

                Data_p += sizeof(uint64);
                LocalExtraFieldLength += sizeof(uint64);
            } else {
                //File size is lass than 4GB.
                C_(printf("zip_parser.c (%d): Uncompressed File Size = 0x%08x\n", __LINE__, FileHeader_p->UncompressedSize);)
            }

            if (0xffffffff == FileHeader_p->CompressedSize) {
                /* If file is larger than 4GB its compressed size is placed in the Zip64 extra data field.
                   Compressed archive is not supported. The compressed size should be same as uncompressed file size field.
                   Data_p is only moved for correct parsing of next arguments. */

                Data_p += sizeof(uint64);
                LocalExtraFieldLength += sizeof(uint64);
            }

            if (0xffffffff == FileHeader_p->RelativeOffsetOfLocalHeader) {
                /* If file offset is larger than 4GB its offset of local header is placed in the Zip64 extra data field. */

                //Offset Of Local Header Record
                memcpy(&TmpUint64, Data_p, sizeof(uint64));
                ZipHandle_p->CentralDirectory.ZipFileHeaders_p[i].Offset = TmpUint64 + sizeof(LocalFileHeader_t) - sizeof(uint8 *) - sizeof(uint8 *) + FileHeader_p->FileNameLength;

                Data_p += sizeof(uint64);
                LocalExtraFieldLength += sizeof(uint64);
            }

            if (LocalExtraFieldLength == FileHeader_p->ExtraFieldLength) {
                C_(printf("zip_parser.c (%d): No alignment present in the extra data.\n", __LINE__);)

                /* If alignment isn't present in the CDR extra data, the extra data field in local file header will contain ONLY this extra data.
                   Offset is moved for FileHeader_p->ExtraFieldLength. */
                ZipHandle_p->CentralDirectory.ZipFileHeaders_p[i].Offset += FileHeader_p->ExtraFieldLength;
                C_(printf("zip_parser.c (%d): Offset of Local Header Record = 0x%08X%08X\n", __LINE__, (uint32)(ZipHandle_p->CentralDirectory.ZipFileHeaders_p[i].Offset >> 32), (uint32)(ZipHandle_p->CentralDirectory.ZipFileHeaders_p[i].Offset));)
                break;
            } else {
                C_(printf("zip_parser.c (%d): Alignment present in extra data.\n", __LINE__);)
            }

            /* Continue to parse the alignment record from the extra data. */

            memcpy(&TmpUint16, Data_p, sizeof(uint16));

            if (EXTRAFIELD_PADDING_HEADER_ID != TmpUint16) { //*((uint16 *)Data_p))
                A_(printf("zip_parser.c (%d): Not a valid ID\n", __LINE__);)
                return ZIP_ERROR;
            }

            Data_p += sizeof(uint16);
            LocalExtraFieldLength += sizeof(uint16);

            //Extra Data Size
            memcpy(&TmpUint16, Data_p, sizeof(uint16));

            if (sizeof(uint16) != TmpUint16) {
                /* The data from this extra field have to be uint16 where the padding value is placed. */
                A_(printf("zip_parser.c (%d): Not a valid size\n", __LINE__);)
                return ZIP_ERROR;
            }

            Data_p += sizeof(uint16);
            LocalExtraFieldLength += sizeof(uint16);

            //Padding size
            memcpy(&TmpUint16, Data_p, sizeof(uint16));

            /* If alignment is present in the CDR extra data, the extra data field in local file header will contain ONLY padding data.
               Offset is moved for padding size. */
            ZipHandle_p->CentralDirectory.ZipFileHeaders_p[i].Offset += TmpUint16;//(*((uin16 *)Data_p));

            C_(printf("zip_parser.c (%d): Offset of Local Header Record aligned to = 0x%08X%08X\n", __LINE__, (uint32)(ZipHandle_p->CentralDirectory.ZipFileHeaders_p[i].Offset >> 32), (uint32)(ZipHandle_p->CentralDirectory.ZipFileHeaders_p[i].Offset));)
            Data_p += sizeof(uint16);
            LocalExtraFieldLength += sizeof(uint16);

            if (LocalExtraFieldLength != FileHeader_p->ExtraFieldLength) {
                A_(printf("zip_parser.c (%d): Error in parsing of the extra data.\n", __LINE__);)
                return ZIP_ERROR;
            }

            break;

        default:
            A_(printf("zip_parser.c(%d): Not supported ExtraFieldLength\n", __LINE__);)
            return ZIP_ERROR;
        }

    }

    return E_SUCCESS;
}

ErrorCode_e Do_Zip_CreateParserContext(const IO_Handle_t ReadHandle,
                                       void **const ZipHandle_pp,
                                       ZipState_t *const State_p,
                                       ZipOpenFileMode_t  ZipOpenFileMode,
                                       VerifyContext_t *VerifyContext_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_ZIP_ERROR;
    ZipHandle_t **Handle_pp = (ZipHandle_t **)ZipHandle_pp;
    ZipHandle_t *ZipHandle_p;

    VERIFY(NULL != Handle_pp, E_INVALID_INPUT_PARAMETERS);

    if (NULL == *Handle_pp) {
        //if using secure wrap a VerifyContext must be supplied
        if (ZIP_USE_IO_SEC_WRAP == ZipOpenFileMode && NULL == VerifyContext_p) {
            ReturnValue = E_INVALID_INPUT_PARAMETERS;
            goto ErrorExit;
        }

        *Handle_pp = (ZipHandle_t *)malloc(sizeof(ZipHandle_t));
        ASSERT(NULL != *Handle_pp);

        (*Handle_pp)->BytesToRead = 0;
        (*Handle_pp)->Offset = 0;
        (*Handle_pp)->ZipOpenFileMode = ZipOpenFileMode;
        (*Handle_pp)->VerifyContext_p = VerifyContext_p;
        *State_p = ZIP_INIT_EOCDR;
    }

    ZipHandle_p = *Handle_pp;

    switch (*State_p) {
    case ZIP_INIT_EOCDR:
        (*Handle_pp)->ReadHandle = ReadHandle;
        ReturnValue = ZipInitEndOfCD_Transfer((*Handle_pp)->ReadHandle, *Handle_pp, State_p);
        break;

    case ZIP_PROCESSING_EOCDR:
        ReturnValue = ZipProcessEndOfCD((*Handle_pp)->ReadHandle, ZipHandle_p, State_p);
        break;

    case ZIP64_INIT_EOCD_LOCATOR:
        ReturnValue = ZipInitEndOfCD_Transfer((*Handle_pp)->ReadHandle, *Handle_pp, State_p);
        break;

    case ZIP64_PROCESSING_EOCD_LOCATOR:
        ReturnValue = Zip64ProcessEndOfCDL((*Handle_pp)->ReadHandle, ZipHandle_p, State_p);
        break;

    case ZIP64_INIT_EOCD_RECORD:
        ReturnValue = ZipInitEndOfCD_Transfer((*Handle_pp)->ReadHandle, *Handle_pp, State_p);
        break;

    case ZIP64_PROCESSING_EOCD_RECORD:
        ReturnValue = Zip64ProcessEndOfCDR((*Handle_pp)->ReadHandle, ZipHandle_p, State_p);
        break;

    case ZIP_INIT_CD:
        ReturnValue = ZipInitCD((*Handle_pp)->ReadHandle, ZipHandle_p, State_p);
        break;

    case ZIP_PROCESSING_CD:
        ReturnValue = ZipProcessCD((*Handle_pp)->ReadHandle, ZipHandle_p, State_p);
        break;

    default:
        A_(printf("zip_parser.c (%d): Unknown State\n", __LINE__);)
        ReturnValue = E_GENERAL_ZIP_ERROR;
        break;
    }

ErrorExit:
    return ReturnValue;
}

ErrorCode_e Do_Zip_DestroyParserContext(void **const ZipHandle_pp)
{
    ErrorCode_e ReturnValue = E_SUCCESS;
    ZipHandle_t *ZipHandle_p = NULL;
    uint32 i = 0;

    VERIFY(NULL != ZipHandle_pp, E_INVALID_INPUT_PARAMETER);
    ZipHandle_p = *ZipHandle_pp;

    for (i = 0; i < ZipHandle_p->CentralDirectory.TotalNrOfEntries; i++) {
        BUFFER_FREE(ZipHandle_p->CentralDirectory.ZipFileHeaders_p[i].FileName_p);
    }

#ifndef DISABLE_SECURITY

    /* Clear cached SecureWrap data */
    if (ZipHandle_p->ZipOpenFileMode == ZIP_USE_IO_SEC_WRAP) {
        ReturnValue = Do_IOW_SecureWrap_DestroyCachedData();
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
        C_(printf("zip_parser.c (%d): SecureWrap cached data destroyed \n", __LINE__);)

    }

#endif
    BUFFER_FREE(ZipHandle_p->CentralDirectory.ZipFileHeaders_p);
    BUFFER_FREE(*ZipHandle_pp);

ErrorExit:
    return ReturnValue;
}

ErrorCode_e Do_Zip_OpenFile(const void        *const ZipHandle_p,
                            const char        *const FileName_p,
                            const uint32             FileNameLength,
                            IO_Handle_t *const IO_Handle_p)
{
    ErrorCode_e ReturnValue = E_SUCCESS;
    ZipHandle_t *Handle_p = (ZipHandle_t *)ZipHandle_p;
    uint32 i = 0;

    VERIFY(NULL != ZipHandle_p, E_INVALID_INPUT_PARAMETERS);
    VERIFY(NULL != FileName_p, E_INVALID_INPUT_PARAMETERS);

    for (i = 0; i < Handle_p->CentralDirectory.TotalNrOfEntries; i++) {
        if (Handle_p->CentralDirectory.ZipFileHeaders_p[i].FileNameLength == FileNameLength) {
            if (0 == strncmp(Handle_p->CentralDirectory.ZipFileHeaders_p[i].FileName_p, FileName_p, FileNameLength)) {
                if (Handle_p->ZipOpenFileMode == ZIP_USE_IO_SEC_WRAP) {
#ifndef DISABLE_SECURITY
                    *IO_Handle_p = Do_IOW_SecureWrapOpen(Handle_p->ReadHandle, Handle_p->CentralDirectory.ZipFileHeaders_p[i].Size, Handle_p->CentralDirectory.ZipFileHeaders_p[i].Offset, Handle_p->VerifyContext_p);
#endif
                } else {
                    *IO_Handle_p = Do_IOW_WrapOpen(Handle_p->ReadHandle, Handle_p->CentralDirectory.ZipFileHeaders_p[i].Size, Handle_p->CentralDirectory.ZipFileHeaders_p[i].Offset);
                }

                ReturnValue = E_SUCCESS;
                goto ErrorExit;
            }
        }
    }

    VERIFY(i != Handle_p->CentralDirectory.TotalNrOfEntries, E_ZIP_PARSER_FILE_NOT_FOUND);

ErrorExit:
    return ReturnValue;
}

ErrorCode_e Do_Zip_GetTotalFilesSize(const void     *const ZipHandle_p,
                                     uint64   *const TotalLength_p)
{
    ErrorCode_e ReturnValue = E_SUCCESS;
    ZipHandle_t *Handle_p = (ZipHandle_t *)ZipHandle_p;
    uint32 i = 0;

    VERIFY(NULL != ZipHandle_p, E_INVALID_INPUT_PARAMETERS);
    VERIFY(NULL != TotalLength_p, E_INVALID_INPUT_PARAMETERS);

    for (i = 0; i < Handle_p->CentralDirectory.TotalNrOfEntries; i++) {
        *TotalLength_p += Handle_p->CentralDirectory.ZipFileHeaders_p[i].Size;
    }

ErrorExit:
    return ReturnValue;
}

ErrorCode_e Do_Zip_GetTotalNrOfEntries(const void *const ZipHandle_p, uint32 *const NrOfEntries_p)
{
    ErrorCode_e ReturnValue = E_SUCCESS;
    ZipHandle_t *Handle_p = (ZipHandle_t *)ZipHandle_p;

    VERIFY(NULL != ZipHandle_p, E_INVALID_INPUT_PARAMETERS);
    VERIFY(NULL != NrOfEntries_p, E_INVALID_INPUT_PARAMETERS);

    *NrOfEntries_p = (uint32)Handle_p->CentralDirectory.TotalNrOfEntries;

    ReturnValue = E_SUCCESS;
ErrorExit:
    return ReturnValue;
}

/** @} */
/** @} */
