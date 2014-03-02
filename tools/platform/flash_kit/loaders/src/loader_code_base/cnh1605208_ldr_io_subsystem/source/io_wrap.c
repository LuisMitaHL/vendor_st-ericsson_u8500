/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

/**
 *  @addtogroup ldr_IO_subsystem
 *  @{
 *      @addtogroup IO_Wrap
 *      @{
 */
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdlib.h>

#include "r_basicdefinitions.h"
#include "r_debug.h"
#include "r_debug_macro.h"
#include "error_codes.h"
#include "r_io_subsystem.h"
#include "io_subsystem.h"
#include "r_memory_utils.h"

/*******************************************************************************
 * Declaration of file local functions
 ******************************************************************************/
static uint32 WrapClose(void *Context_p);
static uint64 WrapRead(const void *const Context_p, void *const Buffer_p, uint64 Length, const uint64 Offset);
static uint64 WrapWrite(const void *const Context_p, const void *const Buffer_p, uint64 Length, const uint64 Offset);
static uint64 WrapLength(const void *const Context_p);
static uint64 WrapPositionedRead(const void *const Context_p, void *const Buffer_p, uint64 Length, const uint64 Offset);
static uint64 WrapPositionedWrite(const void *const Context_p, const void *const Buffer_p, uint64 Length, const uint64 Offset);
static uint64 WrapScatteredPositionedRead(const void *const Context_p, const uint32 VectorIndex, uint64 Length, const uint64 Offset);
static uint64 WrapGatheredPositionedWrite(const void *const Context_p, const uint32 VectorIndex, uint64 Length, const uint64 Offset);
static uint32 WrapCreateVector(const void *const Context_p, uint64 Length, const uint64 BuffSize);
C_(uint32 PrintWrapContext_tStruct(WrapContext_t *WrapContext_p);)
/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/
/*
 *  This type defines functions for Wrap IO Interface
 */
static const IO_Interface_t WrapInterface = {
    WrapClose,
    WrapRead,
    WrapWrite,
    WrapPositionedRead,
    WrapPositionedWrite,
    WrapScatteredPositionedRead,
    WrapGatheredPositionedWrite,
    WrapLength,
    WrapCreateVector
};

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/
/*
 *  Opens wrap around IO resource and returns Handle to it.
 *
 *  @param [in] Parent          IO handle of Parent.
 *  @param [in] Length          in bytes in Parent resource.
 *  @param [in] Offset          in Parent resource.
 *
 *  @retval IO_Handle_t         Initialized IO Handle to a wrap.
 *  @retval IO_INVALID_HANDLE   if resource can not be opened,
 *                              or no space can be allocated for wrap context,
 *                              or there is no free IO channel available.
 */
IO_Handle_t Do_IOW_WrapOpen(const IO_Handle_t Parent, const uint64 Length, const uint64 Offset)
{
    static WrapContext_t *WrapContext_p = NULL;
    IO_Handle_t Handle;
    uint64 ParentLength;
    uint32 MaxOpenIO = IO_Entries[0].MaxOpenEntries;

    if (Parent >= MaxOpenIO) {
        A_(printf("Error: Handle index out of the range io_wrap.c (%d)\n\n", __LINE__);)
        Handle =  IO_INVALID_HANDLE;
        goto ErrorExit;
    }

    ParentLength = Do_IO_GetLength(Parent);

    if (ParentLength == IO_ERROR) {
        A_(printf("io_wrap.c(%d): IO_GetLenth failed!\n", __LINE__);)
        Handle =  IO_INVALID_HANDLE;
        goto ErrorExit;
    }

    WrapContext_p = malloc(sizeof(WrapContext_t));
    ASSERT(NULL != WrapContext_p);
    WrapContext_p->Parent = Parent;
    WrapContext_p->Offset = Offset;
    WrapContext_p->ReadPosition = 0;
    WrapContext_p->WritePosition = 0;
    WrapContext_p->Length = MIN(Length, ParentLength - Offset);
    C_(PrintWrapContext_tStruct(WrapContext_p);)
    Handle = IO_Create(&WrapInterface, WrapContext_p);

    if (Handle == IO_INVALID_HANDLE) {
        A_(printf("io_wrap.c(%d): IO_Create failed!\n", __LINE__);)
        BUFFER_FREE(WrapContext_p);
    }

ErrorExit:
    return Handle;
}


/*******************************************************************************
 * Definition of internal functions
 ******************************************************************************/
/*
 * Creates IO vector.
 *
 * IO Vector is a space of underlying buffers
 * referenced by unique Vector Handle.
 *
 * @param [in] Context_p        initialized IO Channel context.
 * @param [in] Length           in bytes for Vector space.
 * @param [in] BuffSize         Size of each underlying buffer.
 *
 * @retval   IO_VECTOR_INVALID_HANDLE   if invalid Handle is passed,
 * @retval   VectorHandle               if all Buffers for the Vector
 *                                      are allocated and reserved.
 *
 */
static uint32 WrapCreateVector(const void *const Context_p, uint64 Length, const uint64 BuffSize)
{
    WrapContext_t *WrapContext_p = (WrapContext_t *) Context_p;

    Length = MIN(WrapContext_p->Length, Length);
    return Do_IO_CreateVector(WrapContext_p->Parent, Length, BuffSize);
}

/*
 * Closes IO channel with given Context.
 *
 * @param [in] Context_p        initialized IO channel context.
 *
 * @retval  E_SUCCESS           when IO channel is closed,
 *
 */
static uint32 WrapClose(void *Context_p)
{
    BUFFER_FREE(Context_p);
    return E_SUCCESS;
}

/*
 * Reads Length bytes from IO resource defined by Context to Buffer.
 *
 * @param [in]  Context_p       initialized IO channel context.
 * @param [out] Buffer_p        pointer to Buffer space.
 * @param [in]  Length          bytes to be read in Buffer space.
 * @param [in]  Offset          Offset in buffer space.
 *
 * @return The function returns bytes read if successful, otherwise it returns
 *         IO_ERROR and IO_ErrorCode is set to appropriate error code.
 */
static uint64 WrapRead(const void *const Context_p, void *const Buffer_p, uint64 Length, const uint64 Offset)
{
    WrapContext_t *WrapContext_p = (WrapContext_t *) Context_p;
    uint64 ReadLength = 0;
    void *TempBuffer_p = NULL;
    uint64 TempOffset = 0;

    Length = MIN(WrapContext_p->Length - WrapContext_p->ReadPosition, Length);

    TempBuffer_p = (void *)(Buffer_p + (uint32)(WrapContext_p->ReadPosition));
    TempOffset = WrapContext_p->Offset + WrapContext_p->ReadPosition;

    ReadLength = Do_IO_PositionedRead(WrapContext_p->Parent, TempBuffer_p, Length, Length, TempOffset);

    if (ReadLength != IO_ERROR) {
        WrapContext_p->ReadPosition += ReadLength;
    }

    return ReadLength;
}

/*
 * Writes Length bytes from Buffer to
 *                IO resource defined by Context.
 *
 * @param [in]  Context_p       initialized IO channel context.
 * @param [out] Buffer_p        pointer to Buffer space.
 * @param [in]  Length          bytes to be read in Buffer space.
 * @param [in]  Offset          Offset in buffer space.
 *
 * @return The function returns bytes written if successful, otherwise it returns
 *         IO_ERROR and IO_ErrorCode is set to appropriate error code..
 */
static uint64 WrapWrite(const void *const Context_p, const void *const Buffer_p, uint64 Length, const uint64 Offset)
{
    WrapContext_t *WrapContext_p = (WrapContext_t *) Context_p;
    uint64 WriteLength = 0;
    void *TempBuffer_p = NULL;
    uint64 TempOffset = 0;

    Length = MIN(WrapContext_p->Length - WrapContext_p->WritePosition, Length);

    TempBuffer_p = (void *)(Buffer_p + (uint32)(WrapContext_p->WritePosition));
    TempOffset = WrapContext_p->Offset + WrapContext_p->WritePosition;

    WriteLength = Do_IO_PositionedWrite(WrapContext_p->Parent, TempBuffer_p, Length, Length, TempOffset);

    if (WriteLength != IO_ERROR) {
        WrapContext_p->WritePosition += WriteLength;
    }

    return WriteLength;
}

/*
 * Reads length bytes from IO resource defined by Context
 *              starting at Offset location to Buffer.
 *
 * @param [in]  Context_p       initialized IO channel context.
 * @param [out] Buffer_p        pointer to Buffer space.
 * @param [in]  Length          bytes to be read in Buffer space.
 * @param [in]  Offset          in area to be read.
 *
 * @return The function returns bytes read if successful, otherwise it returns
 *         IO_ERROR and IO_ErrorCode is set to appropriate error code.
 */
static uint64 WrapPositionedRead(const void *const Context_p, void *const Buffer_p, uint64 Length, const uint64 Offset)
{
    uint64 ReadLength = 0;
    uint64 TempOffset = 0;

    WrapContext_t *WrapContext_p = (WrapContext_t *) Context_p;

    if (Offset >= WrapContext_p->Length) {
        ReadLength = 0;
        goto ErrorExit;
    }

    Length = MIN(WrapContext_p->Length - Offset, Length);
    TempOffset = Offset + WrapContext_p->Offset;

    ReadLength = Do_IO_PositionedRead(WrapContext_p->Parent, Buffer_p, Length, Length, TempOffset);

ErrorExit:
    return ReadLength;
}

/*
 * Writes length bytes from Buffer to IO resource
 *              defined by Context starting at Offset location.
 *
 * @param [in]  Context_p       initialized IO channel context.
 * @param [out] Buffer_p        pointer to Buffer space.
 * @param [in]  Length          bytes to be written in IO resource.
 * @param [in]  Offset          of area to be written.
 *
 * @return The function returns bytes written if successful, otherwise it returns
 *         IO_ERROR and IO_ErrorCode is set to appropriate error code
 */
static uint64 WrapPositionedWrite(const void *const Context_p, const void *const Buffer_p, uint64 Length, const uint64 Offset)
{
    uint64 WriteLength = 0;
    uint64 TempOffset = 0;

    WrapContext_t *WrapContext_p = (WrapContext_t *) Context_p;

    if (Offset >= WrapContext_p->Length) {
        WriteLength = 0;
        goto ErrorExit;
    }

    Length = MIN(WrapContext_p->Length - Offset, Length);
    TempOffset = Offset + WrapContext_p->Offset;

    WriteLength = Do_IO_PositionedWrite(WrapContext_p->Parent, Buffer_p, Length, Length, TempOffset);

ErrorExit:
    return WriteLength;
}

/*
 * Reads length bytes from IO resource defined by Context
 *              starting at Offset location to Vector.
 *
 * @param [in]  Context_p       initialized IO channel context.
 * @param [in]  VectorIndex     Index of Vector to use.
 * @param [in]  Length          bytes to be read in Buffer space.
 * @param [in]  Offset          in area that is read.
 *
 * @return The function returns bytes read if successful, otherwise it returns
 *         IO_ERROR and IO_ErrorCode is set to appropriate error code.
 *
 * @note VectorList[VectorHandle].IOffset is the offset of valid data in IO_Vector
 */
static uint64 WrapScatteredPositionedRead(const void *const Context_p, const uint32 VectorIndex, uint64 Length, const uint64 Offset)
{
    uint64 ReadLength = 0;
    uint64 TempOffset = 0;

    WrapContext_t *WrapContext_p = (WrapContext_t *) Context_p;

    if (Offset >= WrapContext_p->Length) {
        ReadLength = 0;
        goto ErrorExit;
    }

    Length = MIN(WrapContext_p->Length - Offset, Length);
    TempOffset = Offset + (WrapContext_p->Offset);

    ReadLength = Do_IO_ScatteredPositionedRead(WrapContext_p->Parent, VectorIndex, Length, TempOffset);

ErrorExit:
    return ReadLength;
}

/*
 * Writes length bytes from Vector to IO resource
 *              defined by Context starting at Offset location.
 *
 * @param [in]  Context_p       initialized IO channel context.
 * @param [in]  VectorIndex     Index of Vector to use.
 * @param [in]  Length          bytes to be read in vector.
 * @param [in]  Offset          of area to be written.
 *
 * @return The function returns bytes written if successful, otherwise it returns
 *         IO_ERROR and IO_ErrorCode is set to appropriate error code
 */
static uint64 WrapGatheredPositionedWrite(const void *const Context_p, const uint32 VectorIndex, uint64 Length, const uint64 Offset)
{
    uint64 WriteLength = 0;
    uint64 TempOffset = 0;

    WrapContext_t *WrapContext_p = (WrapContext_t *) Context_p;

    if (Offset >= WrapContext_p->Length) {
        WriteLength = 0;
        goto ErrorExit;
    }

    Length = MIN(WrapContext_p->Length - Offset, Length);
    TempOffset = Offset + WrapContext_p->Offset;

    WriteLength = Do_IO_GatheredPositionedWrite(WrapContext_p->Parent, VectorIndex, Length, TempOffset);

ErrorExit:
    return WriteLength;
}

/*
 * Returns length in bytes of the area in IO resource
 *              pointed by the Context.
 *
 * @param [in]  Context_p       initialized IO channel context.
 *
 * @retval  Length              if length of IO resource is available
 *
 */
static uint64 WrapLength(const void *const Context_p)
{
    return ((WrapContext_t *)Context_p)->Length;
}

C_(uint32 PrintWrapContext_tStruct(WrapContext_t *WrapContext_p)
{

    printf("WrapContext_p->Offset   = 0x%llx\n", WrapContext_p->Offset);
    printf("WrapContext_p->Length   = 0x%llx\n", WrapContext_p->Length);
    printf("WrapContext_p->Parent   = 0x%llx\n", WrapContext_p->Parent);
    return 0;
})
/**     @} */
/** @} */
