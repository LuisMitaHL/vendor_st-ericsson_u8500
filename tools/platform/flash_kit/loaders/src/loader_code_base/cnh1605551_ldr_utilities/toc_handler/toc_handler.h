/*******************************************************************************
 * $Copyright ST-Ericsson 2012 $
 ******************************************************************************/
#ifndef _TOC_HANDLER_H_
#define _TOC_HANDLER_H_
/**
 * @file  toc_handler.h
 * @brief Functions and type declarations for TOC management
 *
 * @addtogroup ldr_toc
 * @{
 *
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "error_codes.h"
#include "r_basicdefinitions.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/
#define TOC_ID_LENGTH             12
#define MAX_ROOT_TOC_ENTRIES      16
#define MAX_ROOT_TOC_ENTRIES_IN_LAYOUT_V3 2

/** Mask for the values present in flags filed. */
#define TOC_FLAG_MASK            0x0000FFFF
/** Sub TOC entry identifier in the 'flags' field is 'ST'. */
#define TOC_FLAG_SUBTOC          0x00005354
/** Partition entry identifier in the 'flags' field is 'PE'. */
#define TOC_FLAG_PARTITION_ENTRY 0x00005045
/** Identifier for sector addressing LBA (512B) in the 'flags' field, 'LB'. */
#define TOC_FLAG_LBA             0x00004C42
/** Supported addressing unit size in bytes. */
#define SUPPORTED_SECTOR_SIZE    512

// Extended flags identifiers for sparse images.
/** Flag that signal if specified image is TOC entry extension. */
#define PARTITION_ENTRY_EXTENSION         (1 << 0)
/** Flag that signal if specified image is last extension in TOC entry. */
#define PARTITION_ENTRY_EXTENSION_LAST    (1 << 1)
/** Flag that signal if specified image is first extension in TOC entry. */
#define PARTITION_ENTRY_EXTENSION_FIRST   (1 << 2)

typedef enum {
    TOC_BAMCOPY_0 = 0,
    TOC_BAMCOPY_1,
    TOC_BAMCOPY_2,
    TOC_BAMCOPY_3,
    BAM_N_BOOT_IMAGE_COPIES,
    TOC_SUBTOC,
} TOC_Type_e;

typedef struct {
    uint32  TOC_Offset;
    uint32  TOC_Size;
    uint32  TOC_Flags;
    uint32  TOC_Align;
    uint32  TOC_LoadAddress;
    char    TOC_Id[TOC_ID_LENGTH];
} TOC_Entry_t;

typedef struct TOC_List_ {
    struct TOC_List_   *next;
    struct TOC_List_   *SubItems;
    struct TOC_List_   *Parent;
    TOC_Entry_t         entry;
} TOC_List_t;



TOC_List_t *Do_TOC_CreateTOCList(uint8 *Data_p, uint32 DataSize, TOC_Type_e BootCopyNr);

boolean Do_TOC_AppendNode(TOC_List_t **ListHead, TOC_Entry_t *NewData);

TOC_List_t *Do_TOC_ConcatenateTOCLists(TOC_List_t *ListHead, TOC_List_t *NewList);

TOC_List_t *Do_TOC_MergeTOCLists(TOC_List_t *Old, TOC_List_t *New);

TOC_List_t *Do_TOC_DestroyTOCList(TOC_List_t *ListHead);

uint8 *Do_TOC_CreateTOCDataFromList(TOC_List_t *ListHead, uint32 *OutputSize);

void UpdateTOCInWriteData(uint8 *WriteData_p, TOC_List_t *ListHead, uint32 BootCopyNr, uint32 AddOffset);

ErrorCode_e Do_TOC_Create_RootList(TOC_List_t **roottoc_pp, uint8 *Data_p, uint32 DataSize, TOC_Type_e TocType);
TOC_List_t *Do_TOC_GetNext_SubTocList(TOC_List_t *List);
TOC_List_t *Do_TOC_GetNext_SubTocNode(TOC_List_t *List);
boolean     Do_TOC_IsEntrySubToc(TOC_Entry_t *Entry_p);
boolean     Do_TOC_IsEntryPartitionEntry(TOC_Entry_t *Entry_p);
boolean     Do_TOC_Add_SubTocList(TOC_List_t *RootList, uint8 *Data_p, uint32 DataSize, char *SubTocNode_ID, uint32 SubToc_Offset);
boolean     Do_TOC_Add_RootList_Entry(TOC_List_t **RootList, TOC_Entry_t *NewData);
boolean     Do_TOC_Add_SubTocList_Entry(TOC_List_t *RootList, char *SubTocNode_ID, uint32 SubToc_Offset, TOC_Entry_t *NewSubTocData);
TOC_List_t *Do_TOC_FindNextEntry(TOC_List_t *ListEntry);
TOC_List_t *Do_TOC_LocateEntryByNodeId(TOC_List_t *List, const char *NodeId);
boolean     Do_TOC_Remove_EntryByNodeId(TOC_List_t **List, const char *NodeId);

uint32 GetSizeForRootToc(uint32 PageSize);

/**
 * @brief Function that fill entry for given TOC with correct addresses and
 *        flags.
 *
 * This function checks if given TOC Offset or Size is bigger than max uint32.
 * If so they will be written in the input TOCEntry_p entry converted to LBA and
 * the LBA flag will be set. Otherwise the input argument are written as it is
 * in TOCEntry_p entry.
 *
 * @param[in,out] TOCEntry_p  Entry pointer that will be set.
 * @param[in]     TOC_Offset  TOC offset entry to be checked.
 * @param[in]     TOC_Size    TOC size entry to be checked.
 * @param[in]     TOC_Flags   Generic TOC entry flags.
 *
 * @return none
 */
void Do_TOC_FillEntry(TOC_Entry_t *TOCEntry_p, uint64 TOC_Offset, uint64 TOC_Size, uint32 TOC_Flags);

/**
 * @brief Get entry offset in bytes.
 *
 * This function checks the entry addressing mode and returns offset of the
 * given entry in bytes.
 *
 * @param[in] TOCEntry_p  Entry to be evaluated.
 *
 * @return    Entry offset in bytes.
 */
uint64 Do_TOC_GetOffset(TOC_Entry_t *TOCEntry_p);

/**
 * @brief Get entry size in bytes.
 *
 * This function checks the entry addressing mode and returns size of the
 * given entry in bytes.
 *
 * @param[in] TOCEntry_p  Entry to be evaluated.
 *
 * @return    Entry size in bytes.
 */
uint64 Do_TOC_GetSize(TOC_Entry_t *TOCEntry_p);

/** @} */
#endif // _TOC_HANDLER_H_
