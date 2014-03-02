/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _R_IO_SUBSYSTEM_H_
#define _R_IO_SUBSYSTEM_H_

#include "t_loader_sec_lib.h"

/**
 *  @addtogroup ldr_IO_subsystem Loader IO subsystem
 *  @{
 *
 * The I/O layer acts as a multiplexer for different I/O capable devices
 * and data-sources.
 *
 * The module consists of front-end, which exposes
 * the generic operations, and back-end, which exposes
 * architecture for low-lever drivers and modules.
 * The front-end comprises:
 * - Read,
 * - Write,
 * - Positioned Read,
 * - Positioned Write,
 * - Scattered Positioned Read,
 * - Gathered Positioned Write,
 * - Get Length, and
 * - Close
 *
 * Additional front-end functions, used with scattered/gathered
 * positioned read/write, are:
 * - Create Vector
 * - Destroy Vector, and
 * - Find Free Vector
 *
 * Front end uses IO handle, provided from Back-end function,
 * to differentiate between IO capable devices.
 *
 * Back-end function is:
 * - Module Open (device specific function
 * which will initialize the IO device and
 * return IO handle to its caller)
 *
 * All functions using IO subsystem should be non-blocking @n
 * same principle should be used for ReadFileContentsRepeat() @n
 *
 * Front end API should be as follows:
 * @code
 *
 * ErrorCode_e ReadFileContentsRepeat(IO_Handle_t* Handle_p,
 *                                    uint32* State_p,
 *                                    uint64 Length,
 *                                    struct Param_t param)
 * {
 *    switch (*State_p)
 *    {
 *      case Idle:
 *      {
 *        TargetBuffer_p = malloc(Length);
 *        Dest_p = TargetBuffer_p;
 *        *Handle_p = Do_Module_Open(param);
 *        *State_p = Reading;
 *        int RequestedLength = Length;
 *        break;
 *      }
 *      case Reading:
 *      {
 *        ReadLength = Do_IO_Read(*Handle_p, Dest_p, RequestedLength);
 *        if (IO_Error == ReadLength)
 *        {
 *          // break with error
 *        }
 *        RequestedLength -= ReadLength;
 *        Dest_p += ReadLength;
 *        if (0 >= RequestedLength)
 *          *State_p = Do_Something_with_data;
 *        break;
 *      }
 *      case Do_Something_with_data:
 *      {
 *        // Do something with data read
 *        *State_p = Done_with_data;
 *        break;
 *      }
 *      case Done_with_data:
 *      {
 *        Do_IO_Close(*Handle_p);
 *        break;
 *      }
 *    }
 *  }
 * @endcode
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_io_subsystem.h"
#include "flash_manager.h"

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/
/**
 * Initializes IO and sets maximal number of concurrent IO channels.
 *
 * This function must be called before any use of IO layer.
 *
 * @param [in] MaxOpenIO        Number of available IO channels.
 *
 * @retval  E_SUCCESS           Layer initialized
 *
 */
ErrorCode_e Do_IO_Init(uint32 MaxOpenIO);

/**
 * Closes IO channel with given Handle.
 *
 * @param [in] Handle           of initialized IO Channel.
 *
 * @retval  0                   if IO channel closed,
 * @retval  IO_INVALID_HANDLE   if invalid Handle is passed to function,
 *                              or resource is already closed.
 */
uint32 Do_IO_Close(const IO_Handle_t Handle);

/**
 * Returns length in bytes of the area in IO resource
 *              pointed by the Handle.
 *
 * @param [in] Handle           of initialized IO Channel.
 *
 * @retval  IO_ERROR            if invalid Handle is passed,
 *                              or there is no defined Length function,
 * @retval  Length              if length of IO resource is available
 *
 */
uint64 Do_IO_GetLength(const IO_Handle_t Handle);

/**
 * Reads Length bytes from IO resource defined by Handle to Buffer.
 *
 * @param [in]  Handle          of initialized IO Channel.
 * @param [out] Buffer_p        pointer to Buffer space.
 * @param [in]  BufferLength    length in bytes of
 *                              allocated space for passed Buffer.
 *                              This value is added only to protect
 *                              function from buffer overflow.
 * @param [in]  Length          bytes to be read in Buffer space.
 *
 * @retval  IO_ERROR            if invalid Handle is passed,
 *                              or there is no defined Read function,
 * @retval  bytes read          if "bytes read" bytes could be
 *                              brought into the Buffer.
 */
uint64 Do_IO_Read(const IO_Handle_t Handle,
                  void *const Buffer_p,
                  const uint64      BufferLength,
                  uint64      Length);

/**
 * Writes Length bytes from Buffer to
 *                IO resource defined by Handle.
 *
 * @param [in]  Handle          of initialized IO Channel.
 * @param [out] Buffer_p        pointer to Buffer space.
 * @param [in]  BufferLength    length in bytes of
 *                              allocated space for passed Buffer.
 *                              This value is added only to protect
 *                              function from buffer overflow.
 * @param [in]  Length          bytes to be read in Buffer space.
 *
 * @retval  IO_ERROR            if invalid Handle is passed,
 *                              or there is no defined Write function,
 * @retval  bytes written       if "bytes written" bytes could be
 *                              brought into the IO resource
 */
uint64 Do_IO_Write(const IO_Handle_t Handle,
                   const void *const Buffer_p,
                   const uint64      BufferLength,
                   uint64      Length);

/**
 * Reads length bytes from IO resource defined by Handle
 *              starting at Offset location to Buffer.
 *
 * @param [in]  Handle          of initialized IO Channel.
 * @param [out] Buffer_p        pointer to Buffer space.
 * @param [in]  BufferLength    length in bytes of
 *                              allocated space for passed Buffer.
 *                              This value is added only to protect
 *                              function from buffer overflow.
 * @param [in]  Length          bytes to be read in Buffer space.
 * @param [in]  Offset          of area to be read.
 *
 * @retval  IO_ERROR            if invalid Handle is passed,
 *                              or there is no defined PositionedRead function,
 * @retval  bytes read          if "bytes read" bytes could be
 *                              brought into the Buffer.
 */
uint64 Do_IO_PositionedRead(const IO_Handle_t Handle,
                            void *const Buffer_p,
                            const uint64      BufferLength,
                            uint64      Length,
                            const uint64      Offset);

/**
 * Writes length bytes from Buffer to IO resource
 *              defined by Handle starting at Offset location.
 *
 * @param [in]  Handle          of initialized IO Channel.
 * @param [out] Buffer_p        pointer to Buffer space.
 * @param [in]  BufferLength    length in bytes of
 *                              allocated space for passed Buffer.
 *                              This value is added only to protect
 *                              function from buffer overflow.
 * @param [in]  Length          bytes to be written in IO resource.
 * @param [in]  Offset          of area to be written.
 *
 * @retval  IO_ERROR            if invalid Handle is passed,
 *                              or there is no defined PositionedWrite function,
 * @retval  bytes written       if "bytes written" bytes could be
 *                              brought into the IO resource
 */
uint64 Do_IO_PositionedWrite(const IO_Handle_t Handle,
                             const void *const Buffer_p,
                             const uint64      BufferLength,
                             uint64      Length,
                             const uint64      Offset);

/**
 * Reads length bytes from IO resource defined by Handle
 *              starting at Offset location to Vector.
 *
 * @param [in]  Handle          of initialized IO Channel.
 * @param [in]  VectorHandle    Handle of Vector space to use.
 * @param [in]  Length          bytes to be read in Buffer space.
 * @param [in]  Offset          in area that is read.
 *
 * @retval  IO_ERROR            if invalid Handle is passed,
 *                              or there is no defined
 *                              ScatteredPositionedRead function,
 * @retval  bytes read          if "bytes read" bytes could be
 *                              brought into the Buffer.
 *
 * @note VectorList[VectorHandle].IOffset is the offset of valid data in IO_Vector
 */
uint64 Do_IO_ScatteredPositionedRead(const IO_Handle_t Handle,
                                     const uint32      VectorHandle,
                                     uint64      Length,
                                     const uint64      Offset);

/**
 * Writes length bytes from Vector to IO resource
 *              defined by Handle starting at Offset location.
 *
 * @param [in]  Handle          of initialized IO Channel.
 * @param [in]  VectorHandle    Handle of Vector space to use.
 * @param [in]  Length          bytes to be read in Buffer space.
 * @param [in]  Offset          of area to be written.
 *
 * @retval  IO_ERROR            if invalid Handle is passed,
 *                              or there is no defined
 *                              GatheredPositionedWrite function,
 * @retval  bytes written       if "bytes written" bytes could be
 *                              brought into the IO resource
 */
uint64 Do_IO_GatheredPositionedWrite(const IO_Handle_t Handle,
                                     const uint32      VectorHandle,
                                     uint64      Length,
                                     const uint64      Offset);

/**
 * Creates IO vector.
 *
 * IO Vector is a space of underlying buffers
 * referenced by unique Vector Handle.
 *
 * @param [in] Handle           of initialized IO Channel.
 * @param [in] Length           in bytes for Vector space.
 * @param [in] BuffSize         Size of each underlying buffer.
 *
 * @retval   IO_VECTOR_INVALID_HANDLE   if invalid Handle is passed,
 * @retval   E_INVALID_INPUT_PARAMETERS if input parameters are
 *                                      out of set limits,
 * @retval   E_VECTOR_CREATE_FAIL       if there is no defined
 *                                      CreateVector function,
 * @retval   VectorHandle               if all Buffers for the Vector
 *                                      are allocated and reserved.
 */
uint32 Do_IO_CreateVector(const IO_Handle_t Handle,
                          uint64      Length,
                          const uint64      BuffSize);

/**
 * Destroys IO vector.
 *
 * @param [in] VectorHandle           of initialized IO Channel.
 *
 * @retval   E_VECTOR_DESTROY_FAIL      if there is no defined
 *                                      DestroyVector function,
 * @retval   E_SUCCESS                  if all Buffers for the Vector
 *                                      are freed.
 */
uint32 Do_IO_DestroyVector(const uint32 VectorHandle);

/*
 * Calculates best values for IO Vector: VectorLength and ChunkSize
 *
 * @param [out] VectorLength_p          address of VectorLength
 * @param [out] ChunkSize_p             address of ChunkSize
 * @param [in]  PageSize                Align values to page size
 * @param [in]  RequestedVectorLength   data space to be allocated in vector
 * @param [in]  IsProcess               determine if transfer is ME->PC or PC->ME
 */
void Do_IO_CalculateVectorParameters(uint32 *const VectorLength_p,
                                     uint32 *const ChunkSize_p,
                                     uint32 PageSize,
                                     const uint64 RequestedVectorLength,
                                     boolean IsProcess);

/**
 * Copies Buffer to Vector.
 *
 * @param [in] VectorIndex   Handle of initialized empty IO Vector
 *                           with at least Buffers in it.
 * @param [in] Buffers       Number of buffers in vector that will provide
 *                           enough space to copy all data from buffer.
 * @param [in] Buffer_p      Pointer to buffer filed with data.
 */
void Do_IO_CopyBufferToVector(const uint32      VectorIndex,
                              const void *const Buffer_p,
                              const uint32      Buffers);

/**
 * Copies Vector to Buffer.
 *
 * @param [in] Buffer_p      Pointer to buffer that is big enough to accept
 *                           all data.
 * @param [in] VectorIndex   Handle of initialized IO Vector
 *                           with at least Buffers in it, containing all data.
 * @param [in] Length        Data Length that needs to be coppied
 *
 * @retval   E_INVALID_INPUT_PARAMETERS  if invalid parameters are passed to
 *                                       function
 * @retval   E_SUCCESS                   if all bytes are successfuly transfered
 *                                       from Vector to bufer.
 */
ErrorCode_e Do_IO_CopyVectorToBuffer(void  *const Buffer_p,
                                     const uint32       VectorIndex,
                                     uint64       Length);

/*
 * Get number of max open entries in same time.
 *
 * @param [in] none
 *
 * @return     Number of maximum allowed entries to ne opened in one time.
 */
uint32 Do_IO_GetNrOfMaxOpenEntry(void);

/*******************************************************************************
 *  Macros definition
 ******************************************************************************/
/**
 * This macro destroy IO vector with sanity check of input parameters.
 *
 * @param [in] VectorIndex    IO Vector to destroy.
 *
 */
#define DESTROY_VECTOR(VectorIndex) \
if (IO_VECTOR_INVALID_HANDLE != (VectorIndex)) \
{ \
  if (E_SUCCESS != (ErrorCode_e)Do_IO_DestroyVector(VectorIndex)) \
  { \
    ReturnValue = E_VECTOR_DESTROY_FAIL; \
    (VectorIndex) = IO_VECTOR_INVALID_HANDLE; \
  } \
  else \
  { \
    ReturnValue = E_SUCCESS; \
    (VectorIndex) = IO_VECTOR_INVALID_HANDLE; \
  } \
}

/**
 * This macro destroy IO vector with sanity check of input parameters.
 * Should be called in case when ReturnValue doesn't need to be update
 * in case when vector is successfully destroyed.
 *
 * @param [in] VectorIndex    IO Vector to destroy.
 *
 */
#define DESTROY_VECTOR_ON_EXIT(VectorIndex) \
if (IO_VECTOR_INVALID_HANDLE != (VectorIndex)) \
{ \
  if (E_SUCCESS != (ErrorCode_e)Do_IO_DestroyVector(VectorIndex)) \
  { \
    ReturnValue = E_VECTOR_DESTROY_FAIL; \
    (VectorIndex) = IO_VECTOR_INVALID_HANDLE; \
    A_(printf("%s(%d): Failed to destroy IO Vector!!!\n", __FILE__, __LINE__);) \
  } \
}

/**
 * This macro close IO handle with sanity check of input parameter.
 *
 * @param [in] Handle         Handle of initialized IO Channel.
 *
 */
#define CLOSE_HANDLE(Handle) \
if (IO_INVALID_HANDLE != (Handle)) \
{ \
  if(E_SUCCESS != (ErrorCode_e)Do_IO_Close(Handle)) \
  { \
    ReturnValue = E_IO_FAILED_TO_CLOSE; \
    (Handle) = IO_INVALID_HANDLE; \
  } \
  else \
  { \
    ReturnValue = E_SUCCESS; \
    (Handle) = IO_INVALID_HANDLE; \
  } \
}

/**
 * This macro close IO handle with sanity check of input parameter.
 * Should be called in case when ReturnValue doesn't need to be update
 * in case when IO channel is successfully closed.
 *
 * @param [in] Handle         Handle of initialized IO Channel.
 *
 */
#define CLOSE_HANDLE_ON_EXIT(Handle) \
if (IO_INVALID_HANDLE != (Handle)) \
{ \
  if(E_SUCCESS != (ErrorCode_e)Do_IO_Close(Handle)) \
  { \
    ReturnValue = E_IO_FAILED_TO_CLOSE; \
    (Handle) = IO_INVALID_HANDLE; \
    A_(printf("%s(%d): Failed to close IO chanel!!!\n", __FILE__, __LINE__);) \
  } \
}

/**
 * @addtogroup IO_Bulk IO subsystem for bulk transfer
 * @{
 *
 * Implements IO subsystem functionalities to transfer data over bulk.
 */

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/
/**
 * Opens Bulk Channel and returns Handle to it.
 *
 * @param [in] SessionId        Id of the Bulk Session
 *                              that needs to be opened.
 * @param [in] Mode             of Bulk Session to be opened
 *                              (BULK_RECEIVE(1), BULK_SEND(2)
 *                              and BULK_RS(3) for both directions).
 * @param [in] Length           Length of the data for bulk transfer in bytes.
 *
 * @retval IO_Handle_t          Initialized IO Handle to a Bulk.
 * @retval IO_INVALID_HANDLE    if invalid length is passed to function,
 *                         or bulk session with these parameters can't be open,
 *                         or no space can be allocated for bulk context,
 *                         or there is no free IO channel available.
 */
IO_Handle_t Do_IOW_BulkOpen(const uint32           SessionId,
                            const TL_SessionMode_t Mode,
                            const uint64           Length);

/**     @} */

/**
 * @addtogroup IO_File IO subsystem to access files in file system
 * @{
 *
 * Implements IO subsystem functionalities to access files in file system.
 */

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/
/**
 *  Opens File and returns Handle to it.
 *
 *  @param [in] Name_p  Pointer to zero terminated string
 *                      containing the complete path of the file.
 *  @param [in] Omode   Open Mode (defined in fcntl.h)
 *  @param [in] Amode   Access Mode (attributes are defined in sys/stat.h)
 *
 *  @retval IO_Handle_t Initialized IO Handle to a File.
 *  @retval IO_INVALID_HANDLE   if invalid file name is passed to function,
 *                              or resource can not be opened,
 *                              or no space can be allocated for file context,
 *                              or there is no free IO channel available.
 */
/*IO_Handle_t Do_IOW_FileOpen(const char*  Name_p,
                           const uint32 Omode,
                           const uint32 Amode);*/

/**     @} */

/**
 * @addtogroup IO_Wrap IO subsystem wrap around IO channel
 * @{
 *
 * Implements IO subsystem functionalities to make wrap around resource.
 */

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/
/**
 *  Opens Wrap and returns Handle to it.
 *
 *  @param [in] Parent  IO Handle of the Parent
 *  @param [in] Offset  Offset of Wrap in the Parent
 *  @param [in] Length  Wrap length in bytes
 *
 *  @retval IO_Handle_t Initialized IO Handle to a Wrap.
 *  @retval IO_INVALID_HANDLE   if invalid Handle is passed to function,
 *                              or resource can not be opened,
 *                              or no space can be allocated for wrap context,
 *                              or there is no free IO channel available.
 */
IO_Handle_t Do_IOW_WrapOpen(const IO_Handle_t Parent,
                            const uint64      Length,
                            const uint64      Offset);

/**     @} */

/**
 * @addtogroup IO_Secure_Wrap IO subsystem secure wrap around IO channel
 * @{
 *
 * Implements IO subsystem functionalities to make secure wrap around resource.
 */

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/
/**
 *  Opens Secure Wrap and returns Handle to it.
 *
 *  @param [in] Parent  IO Handle of the Parent
 *  @param [in] Offset  Offset of Wrap in the Parent
 *  @param [in] Length  Wrap length in bytes
 *  @param [in] VerifyContext_p Verification Context to use when
 *                              calling security functions
*
 *  @retval IO_Handle_t Initialized IO Handle to a Secure Wrap.
 *  @retval IO_INVALID_HANDLE   if invalid Handle is passed to function,
 *                         or resource can not be opened,
 *                         or no space can be allocated for secure wrap context,
 *                         or there is no free IO channel available.
 */
IO_Handle_t Do_IOW_SecureWrapOpen(const IO_Handle_t     Parent,
                                  const uint64           Length,
                                  const uint64           Offset,
                                  VerifyContext_t       *VerifyContext_p);

/*
 * Initialize data cachiong.
 *
 * @return   none
 *
 * @note This function is usable only in Flash Process File
 *       function.
 */
void Do_IOW_SecureWrap_InitializeDataCaching(void);

/*
 * Destroy cached data.
 *
 * @return   IO_CACHED_DATA_DESTROY_FAIL - in case when parent vector cannot
 *                                          be destroyed.
 *
 * @return   E_SUCCESS - in case when cached data are successfuly destroyed
 *
 */
uint32 Do_IOW_SecureWrap_DestroyCachedData(void);

/**
 * Update internal parameter in Secure Wrap that hold information
 * about current (relative) read possition in file that is transfered.
 * Function is called in case when progress data are found during
 * fail save reflashing in process file command.
 *
 * @param [in] NewReadPosition        of file that should be transfered.
 *
 * @return none.
 */
void Do_IOW_SecureWrap_UpdateInternalParameters(uint32 NewReadPosition);

/**     @} */

/**
 * @addtogroup IO_BDM IO subsystem to access block device management
 * @{
 *
 * Implements IO subsystem functionalities to access block devices.
 */

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/
/**
 *  Opens BDM Unit and returns Handle to it.
 *
 *  @param [in] FPDConfig_p    Flash configuration of BDM to be opened
 *
 *  @retval IO_Handle_t Initialized IO Handle to a BDM Unit.
 *  @retval IO_INVALID_HANDLE   if invalid flash configuration is passed to function,
 *                              or resource can not be opened,
 *                              or no space can be allocated for BDM context,
 *                              or there is no free IO channel available.
 */
IO_Handle_t Do_IOW_BDMOpen(FPD_Config_t *const FPDConfig_p);

/**     @} */

/**
 *      @addtogroup IO_BAM IO subsystem to access boot area management
 *      @{
 *      Implements IO subsystem functionalities to access boot area.
 */

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/
/**
 *  Opens BAM and returns Handle to it.
 *
 *  @param [in] FPDConfig_p    Flash configuration of BAM to be opened
 *
 *  @retval IO_Handle_t Initialized IO Handle to a BAM.
 *  @retval IO_INVALID_HANDLE   if resource can not be opened,
 *                              or no space can be allocated for BAM context,
 *                              or there is no free IO channel available.
 */
IO_Handle_t Do_IOW_BAMOpen(FPD_Config_t *const FPDConfig_p);

/**     @} */

/**
 *      @addtogroup IO_FPD IO subsystem to access phisical driver for flash
 *      @{
 *      Implements IO subsystem functionalities to access flash.
 */

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/
/**
 *  Opens FPD and returns Handle to it.
 *
 *  @param [in] RedundantArea   If set to 0 redundant area will be included.
 *  @param [in] FlashDevice     Flash Device to be opened
 *
 *  @retval IO_Handle_t         Initialized IO Handle to a FPD.
 *  @retval IO_INVALID_HANDLE   if resource can not be opened,
 *                              or no space can be allocated for FPD context,
 *                              or there is no free IO channel available.
 */
IO_Handle_t Do_IOW_FPDOpen(const uint32 RedundantArea, const uint8 FlashDevice);

/**     @} */

/** @} */

#endif /*_R_IO_SUBSYSTEM_H_*/
