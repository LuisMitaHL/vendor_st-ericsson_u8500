#ifndef _FLASH_PROCESS_FILE_REPEAT_H_
#define _FLASH_PROCESS_FILE_REPEAT_H_
/*******************************************************************************
* $Copyright ST-Ericsson 2009 $
******************************************************************************/

/**
*  @addtogroup ldr_flash_app
*  @{
*/
/*******************************************************************************
* Includes
******************************************************************************/
#ifdef FAILSAFE_FLASHING
#define MAX_FILENAME_SIZE 256
/* structure for preserving the current ProcessFile state */
typedef struct {
    uint64 Magic;
    uint64 FileOffset;
    uint32 ParsedImageNum;
    char ImageName[MAX_FILENAME_SIZE];
    uint8 Hash_SF;
    uint8 HashProgressInfo;
} FlashingProgressInfo_t;
#endif

#ifdef NONSIGNED_ARCHIVE_ENABLED
typedef enum {
    SIGNED_FLASH_ARCHIVE,
    NON_SIGNED_FLASH_ARCHIVE
} ArchiveType_t;
#endif

typedef struct ProcessFileParameters_st {
    ProcessFile_t           State;
#ifdef NONSIGNED_ARCHIVE_ENABLED
    ArchiveType_t           ArchiveType;
#endif
    IO_Handle_t             ZipReadHandle;
    IO_Handle_t             ZipReadHandleIn;
    IO_Handle_t             ReadHandle;
    IO_Handle_t             WriteHandle;
    FlashCommandPayLoad_t   CommandParameters;
    VerifyContext_t         VerifyContext;
    void                   *ZipHandle_p;
    void                   *ZipHandleIn_p;
    SourceType_t            ImageType;
    ExecutionContext_t     *ExeContext_p;
    uint64                  ReadSize;
    uint64                  NextReadSize;
    uint64                  FileSize;
    char                   *FileData_p;
    ManifestParser_t       *ManifestParser_p;
    uint32                  NoOfParsedImages;
    uint32                  NoOfImages;
    uint32                  VectorLength;
    uint32                  VectorIndex;
    uint32                  NextVectorIndex;
    uint64                  BytesReadPosition;
    uint64                  BytesWritePosition;
    uint64                  BytesWritten;
    uint32                  FlashingSameFile;
    uint64                  MaxVectorLength;
    uint64                  HashBlockSize;
    uint64                  TotalLength;
    uint64                  TotalBytesWritten;
    uint32                  DeviceNumber;
    FPD_Config_t            Configuration;
    FPD_Config_t            BootAreaConfig;
    uint32                 *BootBuffer_p;
    uint32                  BootBufferSize;
    boolean                 TheTransportIsBulk;
#ifdef FAILSAFE_FLASHING
    FlashingProgressInfo_t  *FS_ProgressData_p;
    boolean                 ReflashInProgress;
    uint64                  FailSafeSize;
#endif
    FlashLayoutParser_t    *Flashlayout_p;
    uint32                  FlashlayoutImages;
    TOC_List_t             *TOC_List;
    TOC_List_t             *SubTOC_List;
    char                    SubTOC_List_ID[MAX_TOC_ID_SIZE + 1];
    uint32                  SubTOC_Offset;
    uint32                  SubTOC_Size;
} ProcessFileParameters_t;


/*******************************************************************************
* Declaration of functions
******************************************************************************/



/* @}*/
#endif /*_FLASH_PROCESS_FILE_REPEAT_H_*/
