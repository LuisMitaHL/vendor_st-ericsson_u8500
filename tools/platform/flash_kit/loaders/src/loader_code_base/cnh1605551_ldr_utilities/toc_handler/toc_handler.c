/*******************************************************************************
 * $Copyright ST-Ericsson 2012 $
 ******************************************************************************/

/**
 * @file  toc_handler.c
 * @brief Functions and type declarations for TOC management
 *
 * @addtogroup ldr_toc
 * @{
 *
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "r_debug.h"
#include "r_debug_macro.h"
#include "boot_area_management.h"
#ifdef MACH_TYPE_STN8500
#include "bam_emmc.h"
#endif
#include "toc_handler.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/
const char *CSPSAString = "CSPSA";
const char *CRKCString = "CRKC";
const char *ARB_TBL_String = "ARB_TBL";
const char *ISSWString = "ISSW";
const char *PartSizeString = "PartitionSize";

/*******************************************************************************
 * Declaration of internal functions
 ******************************************************************************/

static TOC_List_t *CreateTOCList(uint8 *Data_p, uint32 DataSize, TOC_Type_e TocType, TOC_List_t *Parent);
static boolean Do_TOC_Add_Entry(TOC_List_t **List_pp, TOC_Entry_t *NewData_p, TOC_List_t *Parent_p);
static TOC_List_t *LocateEntryByOffset(TOC_List_t *List, uint32 Offset);
static ErrorCode_e CheckTOCBufferContents(uint8 *Data_p, uint32 DataSize);
static ErrorCode_e TocIdStringCheck(uint8 *String_p);
#ifdef PRINT_B_
static void PrintTocList(TOC_List_t *List);
#endif

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/

/**
 * Creates Root Toc list from binary data.
 *
 * @param[out] roottoc_pp  Will be set to point on resulting TOC list.
 * @param[in]  Data_p      Raw data containing a TOC
 * @param[in]  DataSize    Size of raw data
 * @param[in]  TocType     Indicates what type the new list will be.
 *
 * @return       @ref E_SUCCESS - Success ending.
 * @return       @ref E_INVALID_TOC_TYPE - If wrong TOC type is inserted.
 * @return       @ref E_TOC_HANDLER_INPUT_DATA_EMPTY - Supplied buffer doesn't contain any usable data.
 * @return       @ref E_TOC_HANDLER_INPUT_DATA_CORRUPTED - Supplied buffer contains corrupted data.
 * @return       @ref E_TOC_LIST_CORRUPTED - TOC list cannot be created because of some malicious state.
 *
 * @remark       see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_TOC_Create_RootList(TOC_List_t **roottoc_pp, uint8 *Data_p, uint32 DataSize, TOC_Type_e TocType)
{
    ErrorCode_e ReturnValue = E_TOC_LIST_CORRUPTED;
    TOC_Entry_t entry;
    uint32 Size = MIN(MAX_ROOT_TOC_ENTRIES * sizeof(TOC_Entry_t), DataSize);
    uint8 i = 0;

    if (TocType != TOC_BAMCOPY_0 && TocType != TOC_BAMCOPY_1 && TocType != TOC_BAMCOPY_2 && TocType != TOC_BAMCOPY_3) {
        A_(printf("toc_handler.c (%d): Do_TOC_CreateRootTOCList with invalid TOCType \n", __LINE__);)
        ReturnValue = E_INVALID_TOC_TYPE;
        goto ErrorExit;
    }

    // Check if input buffer is empty or corrupted
    ReturnValue = CheckTOCBufferContents(Data_p, Size);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    // Sanity check, read 1 TOC entry. It should be ISSW or CSPSA if valid root toc
    memcpy(&entry.TOC_Offset, &Data_p[i], sizeof(entry.TOC_Offset));
    i += sizeof(entry.TOC_Offset);
    memcpy(&entry.TOC_Size, &Data_p[i], sizeof(entry.TOC_Size));
    i += sizeof(entry.TOC_Size);
    memcpy(&entry.TOC_Flags, &Data_p[i], sizeof(entry.TOC_Flags));
    i += sizeof(entry.TOC_Flags);
    memcpy(&entry.TOC_Align, &Data_p[i], sizeof(entry.TOC_Align));
    i += sizeof(entry.TOC_Align);
    memcpy(&entry.TOC_LoadAddress, &Data_p[i], sizeof(entry.TOC_LoadAddress));
    i += sizeof(entry.TOC_LoadAddress);
    memcpy(&entry.TOC_Id, &Data_p[i], TOC_ID_LENGTH);
    i += TOC_ID_LENGTH;

    /*
     * First Entry of the TOC table MUST HAVE one of the following TOC_ID's:
     *
     *  "ISSW"
     *  "CSPSA"
     *  "ARB_TBL1"
     *  "ARB_TBL2"
     *
     *  otherwise we proclaim this TOC as invalid with corrupted data
     */

    if (E_SUCCESS == TocIdStringCheck(entry.TOC_Id)) {
        *roottoc_pp = CreateTOCList(Data_p, Size, TocType, NULL);
    } else {
        A_(printf("toc_handler.c (%d): Corrupted data in boot area.\n", __LINE__);)
        ReturnValue = E_TOC_LIST_CORRUPTED;
        goto ErrorExit;
    }

    ReturnValue = E_SUCCESS;
ErrorExit:
    return ReturnValue;
}

/**
 * Append node to list
 *
 *  @param[in] **ListHead_pp  List to append data to. If *ListhHead_pp is NULL, it will contain
 *                            the new listhead on return
 *  @param[in] NewData_p      Contains data for new node
 *
 *  @return boolean success.
 */
boolean Do_TOC_AppendNode(TOC_List_t **ListHead_pp, TOC_Entry_t *NewData_p)
{
    TOC_List_t *iter;
    TOC_List_t *new_pos;

    iter    = *ListHead_pp;
    new_pos = NULL;

    if (!NewData_p) {
        return FALSE;
    }

    while (iter) {
        new_pos = iter;
        iter    = iter->next;
    }

    if (new_pos) {
        new_pos->next = malloc(sizeof(TOC_List_t));
        new_pos       = new_pos->next;
    } else {
        *ListHead_pp = malloc(sizeof(TOC_List_t));
        new_pos      = *ListHead_pp;
    }

    if (new_pos) {
        new_pos->entry.TOC_Offset       = NewData_p->TOC_Offset;
        new_pos->entry.TOC_Size         = NewData_p->TOC_Size;
        new_pos->entry.TOC_Flags        = NewData_p->TOC_Flags;
        new_pos->entry.TOC_Align        = NewData_p->TOC_Align;
        new_pos->entry.TOC_LoadAddress  = NewData_p->TOC_LoadAddress;
        memcpy(new_pos->entry.TOC_Id, NewData_p->TOC_Id, TOC_ID_LENGTH);

        new_pos->SubItems = NULL;
        new_pos->Parent = NULL;
        new_pos->next = NULL;

        return TRUE;
    } else {
        A_(printf("toc_handler.c (%d): ERROR! AppendNode\n", __LINE__);)
        return FALSE;
    }
}

/**
 * Concatenate two lists
 *
 * @param[in] ListHead_p  List to append data to
 * @param[in] NewList_p   List to append
 *
 * @return TOC_List_t* Head of list after new list appended
 */
TOC_List_t *Do_TOC_ConcatenateTOCLists(TOC_List_t *ListHead_p, TOC_List_t *NewList_p)
{
    TOC_List_t *iter;
    TOC_List_t *new_pos;

    if (!ListHead_p) {
        A_(printf("toc_handler.c(%d): ListHead is NULL!\n", __LINE__);)
        return NewList_p;
    }

    iter    = ListHead_p;
    new_pos = NULL;

    while (iter) {
        new_pos = iter;
        iter = iter->next;
    }

    if (new_pos) {
        new_pos->next = NewList_p;
    }

#ifdef PRINT_LEVEL_B_

    if (ListHead_p != NULL) {
        printf("\nResult List:\n");
        PrintTocList(ListHead_p);
    }

#endif

    return ListHead_p;
}


/**
 * Destroy linked list of TOC entries.
 *
 * @param[in] ListHead  List to destroy
 *
 * @return TOC_List_t* Head of list after destruction (i.e. NULL).
 */
#if 1

/* @TODO: Take care of Subtocs !! */
TOC_List_t *Do_TOC_DestroyTOCList(TOC_List_t *ListHead)
{
    TOC_List_t *head;
    TOC_List_t *iter;

    head = ListHead;

    while (NULL != head) {
        iter = head->next;
        free(head);
        head = iter;
    }

    return head;
}
#endif


/**
 * Create data vector suitable for flashing from linked list of TOC entries.
 *
 * @param[in]   ListHead    List to create data from
 * @param[out]  OutputSize  Will be updated with size of allocated data
 *
 * @return uint8* Pointer to data. Caller must deallocate pointer. NULL if failed.
 */
uint8 *Do_TOC_CreateTOCDataFromList(TOC_List_t *ListHead, uint32 *OutputSize)
{
    TOC_List_t *iter     = NULL;
    uint32      nRecords = 0;
    uint32      ii       = 0;
    uint8       *Data_p  = NULL;

    // Count records
    iter = ListHead;

    while (iter != NULL) {
        nRecords++;
        iter = iter->next;
    }

    nRecords *= sizeof(TOC_Entry_t);

    // Create data
    if (nRecords > 0) {
        Data_p = malloc(nRecords);

        if (NULL == Data_p) {
            nRecords = 0;
            goto ErrorExit;
        }

        // Fill with 0xff
        memset(Data_p, 0xff, (nRecords));

        // Populate data
        iter = ListHead;
        ii = 0;

        while (iter) {
            memcpy(&Data_p[ii], &iter->entry.TOC_Offset, sizeof(iter->entry.TOC_Offset));
            ii += sizeof(iter->entry.TOC_Offset);

            memcpy(&Data_p[ii], &iter->entry.TOC_Size, sizeof(iter->entry.TOC_Size));
            ii += sizeof(iter->entry.TOC_Size);

            memcpy(&Data_p[ii], &iter->entry.TOC_Flags, sizeof(iter->entry.TOC_Flags));
            ii += sizeof(iter->entry.TOC_Flags);

            memcpy(&Data_p[ii], &iter->entry.TOC_Align, sizeof(iter->entry.TOC_Align));
            ii += sizeof(iter->entry.TOC_Align);

            memcpy(&Data_p[ii], &iter->entry.TOC_LoadAddress, sizeof(iter->entry.TOC_LoadAddress));
            ii += sizeof(iter->entry.TOC_LoadAddress);

            memcpy(&Data_p[ii], &iter->entry.TOC_Id, TOC_ID_LENGTH);
            ii += TOC_ID_LENGTH;

            iter = iter->next;
        }
    }

ErrorExit:

    *OutputSize = nRecords;
    return Data_p;
}

/**
 * Update TOC offset for all entries ouside bootimage.
 *
 * @param[out]    WriteData_p   Vector to receive updated TOC binary. Assumed to be large enough. TODO: Add size param?
 * @param[in]     ListHead      Original TOC list
 * @param[in]     BootCopyNr    Boot image copy, max BAM_N_BOOT_IMAGE_COPIES-1.
 *
 * @return void
 */
void UpdateTOCInWriteData(uint8 *WriteData_p, TOC_List_t *ListHead, uint32 BootCopyNr, uint32 AddOffset)
{
    TOC_List_t *iter;
    uint32      ii;
    uint32      Offset;

    iter = ListHead;
    ii = 0;

    if (!WriteData_p) {
        return;
    }

    while (iter) {
        if (iter->entry.TOC_Offset >= BAM_BOOT_IMAGE_COPY_SIZE) {
            // Offset is outside boot image, update Offset
            Offset = iter->entry.TOC_Offset - (BootCopyNr * BAM_BOOT_IMAGE_COPY_SIZE) + AddOffset;
        } else {
            // Offset is within boot image, keep Offset
            Offset = iter->entry.TOC_Offset;
        }

        memcpy(&WriteData_p[ii], &Offset, sizeof(Offset));
        ii += sizeof(Offset);

        memcpy(&WriteData_p[ii], &iter->entry.TOC_Size, sizeof(iter->entry.TOC_Size));
        ii += sizeof(iter->entry.TOC_Size);

        memcpy(&WriteData_p[ii], &iter->entry.TOC_Flags, sizeof(iter->entry.TOC_Flags));
        ii += sizeof(iter->entry.TOC_Flags);

        memcpy(&WriteData_p[ii], &iter->entry.TOC_Align, sizeof(iter->entry.TOC_Align));
        ii += sizeof(iter->entry.TOC_Align);

        memcpy(&WriteData_p[ii], &iter->entry.TOC_LoadAddress, sizeof(iter->entry.TOC_LoadAddress));
        ii += sizeof(iter->entry.TOC_LoadAddress);

        memcpy(&WriteData_p[ii], &iter->entry.TOC_Id, TOC_ID_LENGTH);
        ii += TOC_ID_LENGTH;

        iter = iter->next;
    }
}

/**
 * Assigns a subtoc list on a node entry in root-toclist specified by Node Id and Node Offset.
 * Both Node Id and Node Offset must match for the entry we are searching.
 *
 * @param[in]     RootList_p    Root toc list where subtoc list will be assigned.
 * @param[in]     Data_p        Raw data containing subtoc.
 * @param[in]     DataSize      Data size residing in Data_p
 * @param[in]     SubTocNode_ID Id of TOC entry where subtoc list will be assigned.
 * @param[in]     SubToc_Offset Offset of the TOC entry where subtoc list will be assigned.
 *
 * @return boolean TRUE if the assignment is successful, FALSE otherwise.
 *
 */
boolean Do_TOC_Add_SubTocList(TOC_List_t *RootList_p, uint8 *Data_p, uint32 DataSize, char *SubTocNode_ID, uint32 SubToc_Offset)
{
    boolean result = FALSE;
    TOC_List_t *SubTocEntry = LocateEntryByOffset(RootList_p, SubToc_Offset);

    if (SubTocEntry) {
        if (0 != strncmp(SubTocEntry->entry.TOC_Id, SubTocNode_ID, TOC_ID_LENGTH)) {
            A_(printf("toc_handler.c(%d): *ERR Inconsistency found in the Root List and SubToc List **\n", __LINE__);)
            return FALSE;
        }
    } else {
        SubTocEntry = Do_TOC_LocateEntryByNodeId(RootList_p, SubTocNode_ID);

        if (SubTocEntry) {
            if (SubTocEntry->entry.TOC_Offset != SubToc_Offset) {
                A_(printf("toc_handler.c(%d): *ERR Inconsistency found in the Root List and SubToc List **\n", __LINE__);)
                return FALSE;
            }
        }
    }

    if (NULL != SubTocEntry) {
        TOC_List_t *SubTocList = NULL;

        SubTocList = CreateTOCList(Data_p, DataSize, TOC_SUBTOC, SubTocEntry);

        SubTocEntry->SubItems = SubTocList;

        result = TRUE;
    }

    return result;
}

/**
 * Adds an Entry in a root toc list. If Entry with identical TOC_Offset already exists then it is being replaced.
 *
 * @param[in]     RootList_pp   Pointer to a root toc linked list.
 * @param[in]     NewData_p     Pointer to an entry element that is going to be added in the list.

 *
 * @return boolean TRUE if operation is successful. FALSE otherwise.
 *
 */
boolean Do_TOC_Add_RootList_Entry(TOC_List_t **RootList_pp, TOC_Entry_t *NewData_p)
{
    return Do_TOC_Add_Entry(RootList_pp, NewData_p, NULL);
}

/**
 * Adds an Entry in a sub toc list. If Entry with identical TOC_Offset already exists then it is being replaced.
 * Note: Both Node ID and Offset must match the subtoc list entry.
 *
 * @param[in]     RootList_pp   Root toc linked list whose subtoc list is going to be updated with new entry.
 * @param[in]     SubTocNode_ID Node Id of the root toc list entry which contains the subtoc list.
 * @param[in]     SubToc_Offset Offset of the root toc list entry which contains the subtoc list.
 * @param[in]     NewSubTocData Pointer to an entry element that is going to be added in the list.
 *
 * @return boolean TRUE if operation is successful. FALSE otherwise.
 *
 */
boolean Do_TOC_Add_SubTocList_Entry(TOC_List_t *RootList, char *SubTocNode_ID, uint32 SubToc_Offset, TOC_Entry_t *NewSubTocData)
{
    TOC_List_t *iter;
    TOC_List_t *SubTocPoint;
    TOC_List_t *SubTocList;
    int ii = 0;

    boolean Done = FALSE;

    if (NULL == RootList || NULL == NewSubTocData) {
        A_(printf("toc_handler.c(%d): Invalid input parameters!\n", __LINE__);)
        return FALSE;
    }

    do {
        iter = RootList;

        while (iter) {
            if (iter->entry.TOC_Offset == SubToc_Offset) {
                if (memcmp(iter->entry.TOC_Id, SubTocNode_ID, sizeof(iter->entry.TOC_Id))) {
                    A_(printf("toc_handler.c(%d): *ERR Inconsistency found in the Root List and SubToc List **\n", __LINE__);)
                    return FALSE;
                }

                Done = TRUE;
                break;
            }

            if (!memcmp(iter->entry.TOC_Id, SubTocNode_ID, sizeof(iter->entry.TOC_Id))) {
                if (iter->entry.TOC_Offset != SubToc_Offset) {
                    A_(printf("toc_handler.c(%d): *ERR Inconsistency found in the Root List and SubToc List **\n", __LINE__);)
                    return FALSE;
                }

                Done = TRUE;
                break;
            }

            iter = iter->next;
        }

        if (NULL == iter) {
            TOC_Entry_t SubTocEntry = {0};

            SubTocEntry.TOC_Align = 0xffffffff;
            SubTocEntry.TOC_Flags = NewSubTocData->TOC_Flags | TOC_FLAG_SUBTOC;
            memcpy(&SubTocEntry.TOC_Id, SubTocNode_ID, sizeof(SubTocEntry.TOC_Id));
            SubTocEntry.TOC_Offset = SubToc_Offset;
            SubTocEntry.TOC_Size = 0xffffffff;  // Will be calculated later on

            if (FALSE == Do_TOC_Add_RootList_Entry(&RootList, &SubTocEntry)) {
                A_(printf("toc_handler.c (%d): **ERR Adding entry to the root list **\n", __LINE__);)
                return FALSE;
            }
        }

    } while (!Done);

    SubTocPoint = iter;
    SubTocList  = SubTocPoint->SubItems;

    // Now iter points to the correct subtoc entry

    if (FALSE == Do_TOC_Add_Entry(&SubTocPoint->SubItems, NewSubTocData, SubTocPoint)) {
        return FALSE;
    }

    // Now update the size of SubTocPoint accordingly

    iter = SubTocPoint->SubItems;

    while (iter) {
        iter = iter->next;
        ii++;
    }

    SubTocPoint->entry.TOC_Size = ii * sizeof(TOC_Entry_t);

    return TRUE;
}

/**
 * Returns the fist node that contains sub toc entry.
 * Note: This function walks through sub entry lists also!
 *
 * @param[in]     List_p   Linked list node from where the search starts.
 *
 * @return TOC_List_t* Pointer to the next node entry. NULL if there is no suitable node.
 *
 */
TOC_List_t *Do_TOC_GetNext_SubTocList(TOC_List_t *List_p)
{
    TOC_List_t *iter = List_p;

    while (iter) {
        if ((iter->entry.TOC_Flags & TOC_FLAG_MASK) == TOC_FLAG_SUBTOC) {
            return iter->SubItems;
        }

        iter = Do_TOC_FindNextEntry(iter);
    }

    return NULL;
}

TOC_List_t *Do_TOC_GetNext_SubTocNode(TOC_List_t *List)
{
    TOC_List_t *iter = List;

    while (iter) {
        if ((iter->entry.TOC_Flags & TOC_FLAG_MASK) == TOC_FLAG_SUBTOC) {
            return iter;
        }

        iter = Do_TOC_FindNextEntry(iter);
    }

    return NULL;
}

/**
 * Returns the next node in a linked list. As an input parameter can be used either head of linked list, or previous
 * result from this function.
 * Note: This function walks through sub entry lists also!
 *
 * @param[in]     ListEntry_p   Linked list node from where the search starts.
 *
 * @return TOC_List_t* Pointer to the next node entry. NULL if there is no next entry.
 *
 */
TOC_List_t *Do_TOC_FindNextEntry(TOC_List_t *ListEntry_p)
{
    if (NULL == ListEntry_p) {
        return NULL;
    }

    if (NULL != ListEntry_p->SubItems) {
        return ListEntry_p->SubItems;
    }

    if (NULL != ListEntry_p->next) {
        return ListEntry_p->next;
    }

    if (NULL != ListEntry_p->Parent) {
        return ListEntry_p->Parent->next;
    }

    return NULL;
}

/*******************************************************************************
 * Definition of internal functions
 ******************************************************************************/
/**
 * Adds a node in a linked list. If a node entry with identical TOC_Offset already
 * exists in the list, then entry members are updated with input node.
 * Note: This function does not search through sub entry lists!
 *
 * @param[in]     List_pp       Linked list where new entry will be put.
 * @param[in]     NewData_p     New TOC entry that is to be added in the linked list.
 * @param[in]     Parent_p      Parent of the newly added node entry.
 *
 * @return boolean TRUE if node is added successfully, FALSE otherwise.
 *
 */
static boolean Do_TOC_Add_Entry(TOC_List_t **List_pp, TOC_Entry_t *NewData_p, TOC_List_t *Parent_p)
{
    TOC_List_t *iter = NULL;
    TOC_List_t *new_pos = NULL;
    boolean bEntryExists = FALSE;

    if (!NewData_p) {
        return FALSE;
    }

    if (NULL == *List_pp) {
        *List_pp = (TOC_List_t *)malloc(sizeof(TOC_List_t));

        if (NULL == *List_pp) {
            A_(printf("toc_handler.c(%d): **ERR Memory allocation fail **\n", __LINE__);)
            return FALSE;
        }

        new_pos = *List_pp;

        new_pos->entry.TOC_Offset       = NewData_p->TOC_Offset;
        new_pos->entry.TOC_Size         = NewData_p->TOC_Size;
        new_pos->entry.TOC_Flags        = NewData_p->TOC_Flags;
        new_pos->entry.TOC_Align        = NewData_p->TOC_Align;
        new_pos->entry.TOC_LoadAddress  = NewData_p->TOC_LoadAddress;
        memcpy(new_pos->entry.TOC_Id, NewData_p->TOC_Id, TOC_ID_LENGTH);

        new_pos->SubItems = NULL;
        new_pos->Parent = Parent_p;
        new_pos->next = NULL;

        return TRUE;
    }

    //
    // ISSW TOC must be placed on top of the List head
    //
    if ((0 == strncmp(NewData_p->TOC_Id, ISSWString, strlen(ISSWString))) &&
            (0 != strncmp((*List_pp)->entry.TOC_Id, ISSWString, strlen(ISSWString)))) {
        new_pos = malloc(sizeof(TOC_List_t));

        if (NULL == new_pos) {
            A_(printf("toc_handler.c(%d): **ERR Memory allocation fail **\n", __LINE__);)
            return FALSE;
        }

        new_pos->entry.TOC_Offset       = NewData_p->TOC_Offset;
        new_pos->entry.TOC_Size         = NewData_p->TOC_Size;
        new_pos->entry.TOC_Flags        = NewData_p->TOC_Flags;
        new_pos->entry.TOC_Align        = NewData_p->TOC_Align;
        new_pos->entry.TOC_LoadAddress  = NewData_p->TOC_LoadAddress;
        memcpy(new_pos->entry.TOC_Id, NewData_p->TOC_Id, TOC_ID_LENGTH);

        new_pos->SubItems = NULL;
        new_pos->Parent = Parent_p;
        new_pos->next = NULL;

        *List_pp = Do_TOC_ConcatenateTOCLists(new_pos, *List_pp);
        B_(PrintTocList(*List_pp);)

        return TRUE;
    }

    //
    // CRKC TOC must be placed after ISSW entry
    //
    if ((0 == strncmp(NewData_p->TOC_Id, CRKCString, strlen(CRKCString))) &&
            (NULL == Do_TOC_LocateEntryByNodeId(*List_pp, CRKCString))) {
        new_pos = malloc(sizeof(TOC_List_t));

        if (NULL == new_pos) {
            A_(printf("toc handler.c(%d): **ERR Memory allocation fail **\n", __LINE__);)
            return FALSE;
        }

        new_pos->entry.TOC_Offset       = NewData_p->TOC_Offset;
        new_pos->entry.TOC_Size         = NewData_p->TOC_Size;
        new_pos->entry.TOC_Flags        = NewData_p->TOC_Flags;
        new_pos->entry.TOC_Align        = NewData_p->TOC_Align;
        new_pos->entry.TOC_LoadAddress  = NewData_p->TOC_LoadAddress;
        memcpy(new_pos->entry.TOC_Id, NewData_p->TOC_Id, TOC_ID_LENGTH);

        new_pos->SubItems = NULL;
        new_pos->Parent = Parent_p;
        new_pos->next = NULL;

        if (0 == strncmp((*List_pp)->entry.TOC_Id, ISSWString, strlen(ISSWString))) {
            TOC_List_t *old_next = (*List_pp)->next;
            (*List_pp)->next = new_pos;
            new_pos->next = old_next;
        } else {
            *List_pp = Do_TOC_ConcatenateTOCLists(new_pos, *List_pp);
        }

        B_(PrintTocList(*List_pp);)

        return TRUE;
    }

    iter = *List_pp;
    new_pos = NULL;

    /* Search Item by Offset */
    while (iter) {
        new_pos = iter;

        if (0 == strncmp(iter->entry.TOC_Id, NewData_p->TOC_Id, TOC_ID_LENGTH) && iter->entry.TOC_Flags == NewData_p->TOC_Flags) {
            bEntryExists = TRUE;
            break;
        }

        iter = iter->next;
    }

    if (!bEntryExists) {
        new_pos->next = malloc(sizeof(TOC_List_t));
        new_pos = new_pos->next;

        if (NULL == new_pos) {
            A_(printf("toc_handler.c(%d): **ERR Memory allocation fail **\n", __LINE__);)
            return FALSE;
        }
    }

    new_pos->entry.TOC_Offset       = NewData_p->TOC_Offset;
    new_pos->entry.TOC_Size         = NewData_p->TOC_Size;
    new_pos->entry.TOC_Flags        = NewData_p->TOC_Flags;
    new_pos->entry.TOC_Align        = NewData_p->TOC_Align;
    new_pos->entry.TOC_LoadAddress  = NewData_p->TOC_LoadAddress;
    memcpy(new_pos->entry.TOC_Id, NewData_p->TOC_Id, TOC_ID_LENGTH);
    new_pos->Parent = Parent_p;

    if (!bEntryExists) {
        new_pos->SubItems = NULL;
        new_pos->next = NULL;
    }

    B_(PrintTocList(*List_pp);)

    return TRUE;
}

/**
 * Seeks for a entry node by Offset.
 * Note: This function performs a search through sub entry lists also.
 *
 * @param[in]     List_p        Linked list (either root toc list or subtoc) where this function will search in.
 * @param[in]     Offset        Key used as a search criteria.
 *
 * @return TOC_List_t* Pointer to a list node entry element matching the search criteria. NULL means no entry is found.
 *
 */
TOC_List_t *LocateEntryByOffset(TOC_List_t *List_p, uint32 Offset)
{
    TOC_List_t *ReturnEntry = NULL;
    TOC_List_t *Iter = List_p;

    while (Iter) {
        if (Offset == Iter->entry.TOC_Offset) {
            ReturnEntry = Iter;
            break;
        }

        Iter = Do_TOC_FindNextEntry(Iter);
    }

    return ReturnEntry;
}

/**
 * Seeks for a entry node by Node Id.
 * Note: This function performs a search through sub entry lists also.
 *
 * @param[in]     List_p        Linked list (either root toc list or subtoc) where this function will search in.
 * @param[in]     NodeId        Key used as a search criteria.
 *
 * @return TOC_List_t* Pointer to a list node entry element matching the search criteria. NULL means no entry is found.
 *
 */
TOC_List_t *Do_TOC_LocateEntryByNodeId(TOC_List_t *List, const char *NodeId)
{
    TOC_List_t *Iter = List;
    TOC_List_t *ReturnEntry = NULL;

    while (Iter) {
        if (0 == strncmp(NodeId, Iter->entry.TOC_Id, TOC_ID_LENGTH)) {
            ReturnEntry = Iter;
            break;
        }

        Iter = Do_TOC_FindNextEntry(Iter);
    }

    return ReturnEntry;
}

/**
 * Checks whether input entry is of type Sub Toc.
 *
 * @param[in]     Entry_p       Pointer to an Entry that is to be checked.
 *
 * @return boolean TRUE if input entry is of type Sub Toc. FALSE otherwise.
 *
 */
boolean Do_TOC_IsEntrySubToc(TOC_Entry_t *Entry_p)
{
    boolean returnvalue = FALSE;

    if (Entry_p != NULL) {
        if ((Entry_p->TOC_Flags & TOC_FLAG_MASK) == TOC_FLAG_SUBTOC) {
            returnvalue = TRUE;
        }
    }

    return returnvalue;
}

/**
 * Checks whether input entry is of type Partition Entry.
 *
 * @param[in]     Entry_p       Pointer to an Entry that is to be checked.
 *
 * @return boolean TRUE if input entry is of type Partition Entry. FALSE otherwise.
 *
 */
boolean Do_TOC_IsEntryPartitionEntry(TOC_Entry_t *Entry_p)
{
    boolean returnvalue = FALSE;

    if (Entry_p != NULL) {
        if ((Entry_p->TOC_Flags & TOC_FLAG_MASK) == TOC_FLAG_PARTITION_ENTRY) {
            returnvalue = TRUE;
        }
    }

    return returnvalue;
}

/**
 * Creates linked list of Tocs from binary data.
 *
 * @param[in] Data_p      Raw data containing a TOC
 * @param[in] DataSize    Size of raw data
 * @param[in] TocType     Indicaties what type the new list will be.
 * @param[in] Parent      Parent of each node entry in the new list.
 *
 * @return TOC_List_t* Header of newly created list. NULL on error.
 */
static TOC_List_t *CreateTOCList(uint8 *Data_p, uint32 DataSize, TOC_Type_e TocType, TOC_List_t *Parent_p)
{
    TOC_List_t *head;
    TOC_List_t *iter;
    uint32      ii;
    uint32      i = 0;
    uint32      tmpData;
    boolean     Done = FALSE;
    boolean     ErasedFlash = TRUE;

    head = NULL;

    // Sanity check. At least one record
    if ((DataSize < sizeof(TOC_Entry_t)) || (Data_p == NULL)) {
        goto ErrorExit;
    }

    // Check 1 toc entry to see if flash is erased
    while (i < sizeof(TOC_Entry_t)) {
        memcpy(&tmpData, &Data_p[i], sizeof(uint32));

        if (tmpData != 0xFFFFFFFF && tmpData != 0x0) {
            ErasedFlash = FALSE;
            break;
        }

        i += sizeof(uint32);
    }

    // Sanity check. Data read from erased eMMC can be either 0xff or 0x00
    if (FALSE == ErasedFlash) {
        // Create list
        head = malloc(sizeof(TOC_List_t));
    }

    if (head) {
        iter = head;
        iter->next = NULL;
        ii = 0;

        do {
            // Populate list members
            memcpy(&iter->entry.TOC_Offset, &Data_p[ii], sizeof(iter->entry.TOC_Offset));
            ii += sizeof(iter->entry.TOC_Offset);

            memcpy(&iter->entry.TOC_Size, &Data_p[ii], sizeof(iter->entry.TOC_Size));
            ii += sizeof(iter->entry.TOC_Size);

            memcpy(&iter->entry.TOC_Flags, &Data_p[ii], sizeof(iter->entry.TOC_Flags));
            ii += sizeof(iter->entry.TOC_Flags);

            memcpy(&iter->entry.TOC_Align, &Data_p[ii], sizeof(iter->entry.TOC_Align));
            ii += sizeof(iter->entry.TOC_Align);

            memcpy(&iter->entry.TOC_LoadAddress, &Data_p[ii], sizeof(iter->entry.TOC_LoadAddress));
            ii += sizeof(iter->entry.TOC_LoadAddress);

            memcpy(&iter->entry.TOC_Id, &Data_p[ii], TOC_ID_LENGTH);
            ii += TOC_ID_LENGTH;

            iter->SubItems = NULL;
            iter->Parent = Parent_p;

            if (TocType != TOC_SUBTOC) {
                if (iter->entry.TOC_Offset >= BAM_BOOT_IMAGE_COPY_SIZE) {
                    // Offset is outside boot image, update Offset
                    B_(printf("Offset recalc %d : 0x%08x -> ", TocType, iter->entry.TOC_Offset);)
                    iter->entry.TOC_Offset = iter->entry.TOC_Offset + (TocType * BAM_BOOT_IMAGE_COPY_SIZE);
                    B_(printf("0x%08x\n", iter->entry.TOC_Offset);)
                }
            }

            // Enough data left for additional node?
            if ((ii + sizeof(TOC_Entry_t) <= DataSize)) {
                // Check 1 toc entry to see if flash is erased
                i = 0;
                ErasedFlash = TRUE;

                while (i < sizeof(TOC_Entry_t)) {
                    memcpy(&tmpData, &Data_p[ii + i], sizeof(uint32));

                    if (tmpData != 0xFFFFFFFF && tmpData != 0x0) {
                        ErasedFlash = FALSE;
                        break;
                    }

                    i += sizeof(uint32);
                }

                if (ErasedFlash) {
                    // Invalid offset, we're done
                    Done = TRUE;
                } else {
                    iter->next = malloc(sizeof(TOC_List_t));

                    if (iter->next) {
                        iter = iter->next;
                        iter->next = NULL;
                    } else {
                        head = Do_TOC_DestroyTOCList(head);
                        Done = TRUE;
                    }
                }
            } else {
                Done = TRUE;
            }
        } while (!Done);
    }

    B_(
    else {
        printf("Do_TOC_CreateTOCList: *** No head ***\n");
    }
    )

#ifdef PRINT_LEVEL_B_

    if (head != NULL) {
        PrintTocList(head);
    }

#endif

ErrorExit:
    return head;
}

/**
 * TOC_Id should contain only: letters, numbers, "_", "-".
 * If TOC_Id contains other character than these, it's not valid TOC_Id.
 */
static ErrorCode_e TocIdStringCheck(uint8 *String_p)
{
    ErrorCode_e ReturnValue = E_TOC_LIST_CORRUPTED;
    uint32 i = 0;

    do {
        if ((String_p[i] >= 'A' && String_p[i] <= 'Z') || (String_p[i] >= 'a' && String_p[i] <= 'z') ||
                (String_p[i] >= '0' && String_p[i] <= '9') || (String_p[i] == '-') || (String_p[i] == '_')) {
            i++;
            ReturnValue = E_SUCCESS;
        } else {
            ReturnValue = E_TOC_LIST_CORRUPTED;
            break;
        }
    } while (String_p[i] != '\0' && i < TOC_ID_LENGTH);

    return ReturnValue;
}

/**
 * Check input data buffer in "create root list" functionality.
 *
 * @param[in] Data_p      Raw data containing a TOC
 * @param[in] DataSize    Size of raw data
 *
 * @return       @ref E_SUCCESS - Input buffer contains possibly valid data.
 * @return       @ref E_TOC_HANDLER_INPUT_DATA_EMPTY - Supplied buffer doesn't contain any usable data.
 * @return       @ref E_TOC_HANDLER_INPUT_DATA_CORRUPTED - Supplied buffer contains corrupted data.
 *
 * @remark       see \c ErrorCode_e for detail description of the error codes.
 */
static ErrorCode_e CheckTOCBufferContents(uint8 *Data_p, uint32 DataSize)
{
    ErrorCode_e ReturnValue = E_TOC_HANDLER_INPUT_DATA_CORRUPTED;
    uint32 NextTOCOffset;
    uint32 i = 0;

    memcpy(&NextTOCOffset, &Data_p[i], sizeof(uint32));

    if (0x00 == NextTOCOffset) {
        for (i = 0; i < DataSize; i++) {
            VERIFY(0x00 == Data_p[i], E_TOC_HANDLER_INPUT_DATA_CORRUPTED);
        }

        A_(printf("toc_handler.c (%d): Supplied data buffer doesn't contain any usable data. TOC list cannot be generated.\n", __LINE__);)
        ReturnValue = E_TOC_HANDLER_INPUT_DATA_EMPTY;

    } else if (0xffffffff == NextTOCOffset) {
        for (i = 0; i < DataSize; i++) {
            VERIFY(0xff == Data_p[i], E_TOC_HANDLER_INPUT_DATA_CORRUPTED);
        }

        A_(printf("toc_handler.c (%d): Supplied data buffer doesn't contain any usable data. TOC list cannot be generated.\n", __LINE__);)
        ReturnValue = E_TOC_HANDLER_INPUT_DATA_EMPTY;

    } else {
        ReturnValue = E_SUCCESS;
        goto ErrorExit;
    }

ErrorExit:
    return ReturnValue;
}

#ifdef PRINT_B_
static void PrintTocList(TOC_List_t *List)
{
    TOC_List_t *iter = List;
    char buf[TOC_ID_LENGTH + 1];

    buf[TOC_ID_LENGTH] = 0;

    printf("Offset     Size       Flags      Align      LoadAddr   ID\n");

    while (iter) {
        printf("0x%08x ", iter->entry.TOC_Offset);
        printf("0x%08x ", iter->entry.TOC_Size);
        printf("0x%08x ", iter->entry.TOC_Flags);
        printf("0x%08x ", iter->entry.TOC_Align);
        printf("0x%08x ", iter->entry.TOC_LoadAddress);
        memcpy(buf, &iter->entry.TOC_Id, TOC_ID_LENGTH);
        printf("\"%s\"\n", buf);

        iter = iter->next;
    }

    printf("\n");
}
#endif

/**
 * Removes the entry identified by TOC ID from the TOC list.
 *
 * @param[out] List       Pointer to the head of the TOC list. In the
 *                        case when head node is removed the List will
 *                        be updated with the new head.
 * @param[in] NodeId      ID of the node to be removed.
 *
 * @return boolean TRUE if entry was removed, otherwise FALSE.
 */
boolean Do_TOC_Remove_EntryByNodeId(TOC_List_t **List, const char *NodeId)
{
    TOC_List_t *Node_p = NULL;
    TOC_List_t *TmpNode_p = NULL;

    if (List == NULL || *List == NULL || NodeId == NULL) {
        A_(printf("toc_handler.c (%d): NULL parameter\n", __LINE__);)
        return FALSE;
    }

    Node_p = *List;

    // Handle the removal of the first node
    if (0 == strncmp(NodeId, Node_p->entry.TOC_Id, TOC_ID_LENGTH)) {
        *List = Node_p->next;
        free(Node_p);
        B_(printf("toc_handler.c (%d): head node with ID %s removed\n", __LINE__, NodeId);)
        return TRUE;
    }

    TmpNode_p = Node_p;

    while ((Node_p = Do_TOC_FindNextEntry(Node_p)) != NULL) {
        if (0 == strncmp(NodeId, Node_p->entry.TOC_Id, TOC_ID_LENGTH)) {
            if (TmpNode_p->next == Node_p) {
                TmpNode_p->next = Node_p->next;
            } else if (TmpNode_p->SubItems == Node_p) {
                TmpNode_p->SubItems = Node_p->next;
            } else if (TmpNode_p->Parent->next == Node_p) {
                TmpNode_p->Parent->next = Node_p->next;
            } else {
                A_(printf("toc_handler.c (%d): inconsistency detected, can't relink the nodes!\n", __LINE__);)
                return FALSE;
            }

            free(Node_p);
            B_(printf("toc_handler.c (%d): node with ID %s removed\n", __LINE__, NodeId);)
            return TRUE;
        }

        TmpNode_p = Node_p;
    }

    A_(printf("toc_handler.c (%d): node with ID %s not found\n", __LINE__, NodeId);)
    return FALSE;
}

/**
 * Returns the amount of space needed for the RootTOC.
 * This is the larger of the space needed to hold a
 * complete RootToc or the Pagesize.
 *
 * @param[in] PageSize    PageSize of the emmc to read from
 *
 * @return uint32 number of bytes to allocate for the buffer
 */
uint32 GetSizeForRootToc(uint32 PageSize)
{
    return MAX(MAX_ROOT_TOC_ENTRIES * sizeof(TOC_Entry_t), PageSize);
}

/** @} */
