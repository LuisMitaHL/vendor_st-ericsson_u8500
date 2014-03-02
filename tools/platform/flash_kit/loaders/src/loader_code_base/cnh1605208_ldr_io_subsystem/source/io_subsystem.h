/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _IO_SUBSYSTEM_H_
#define _IO_SUBSYSTEM_H_

/**
 *  @addtogroup ldr_IO_subsystem
 *  @{
 *
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "error_codes.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/
/** The number of retries before returning error in communication */
#define MAX_CYCLES 3

/** The starting session number when wrap over bulk is used */
#define MIN_WRAP_SESSION_ID 3

/** Number of redundant bytes in bulk buffer */
#define BULK_BUFFER_REDUNDANT_DATA (ALIGNED_HEADER_LENGTH + ALIGNED_BULK_EXTENDED_HEADER_LENGTH)

/** Wait for the transport to complete */
#define IO_WAIT (uint64)0

/**
 *  Defines the different states of IO Layer
 */
typedef enum {
    IOS_CLOSED = 0,  /**< IO Layer is closed */
    IOS_OPEN         /**< IO Layer is open */
} IO_State_t;

/**
 *  Defines IO Interface parameters
 */
typedef struct {
    /** Function to close IO handle */
    uint32(* Close_fn)(void *Context_p);
    /** Function to read data from device pointed by IO handle */
    uint64(* Read_fn)(const void *const Context_p,
                      void *const Buffer_p,
                      uint64      Length,
                      const uint64      Offset);
    /** Function to write data to device pointed by IO handle */
    uint64(* Write_fn)(const void *const Context_p,
                       const void *const Buffer_p,
                       uint64      Length,
                       const uint64      Offset);
    /** Function to read data from offset location on device pointed by IO handle */
    uint64(* PRead_fn)(const void *const Context_p,
                       void *const Buffer_p,
                       uint64      Length,
                       const uint64      Offset);
    /** Function to write data to offset location on device pointed by IO handle */
    uint64(* PWrite_fn)(const void *const Context_p,
                        const void *const Buffer_p,
                        uint64      Length,
                        const uint64      Offset);
    /** Function to read data from offset location on device pointed by IO handle */
    uint64(* PReadS_fn)(const void *const Context_p,
                        const uint32      VectorIndex,
                        uint64      Length,
                        const uint64      Offset);
    /** Function to write data to offset location on device pointed by IO handle */
    uint64(* PWriteG_fn)(const void *const Context_p,
                         const uint32      VectorIndex,
                         uint64      Length,
                         const uint64      Offset);
    /** Function to get Length of data available at device pointed by IO handle */
    uint64(* Length_fn)(const void *const Context_p);
    /** Function to create IO vector with specified Length and BufferSize */
    uint32(* FCreateV_fn)(const void *const Context_p,
                          uint64      Length,
                          const uint64      BufferSize);
} IO_Interface_t;


/**
 *  Defines IO Instance parameters
 */
typedef struct {
    uint32 MaxOpenEntries;      /**< Total number of available IO channels */
    IO_State_t State;           /**< State of the IO channel */
    IO_Interface_t Interface;   /**< Available functions for the IO channel */
    void *Context_p;            /**< IO channel context */
    uint64 ReadPosition;        /**< Current read position in IO resource */
    uint64 WritePosition;       /**< Current write position in IO resource */
} IO_Entry_t;

/**
 *      @addtogroup IO_Bulk
 *      @{
 */

/*******************************************************************************
 * Types, constants
 ******************************************************************************/
/**
 *  Describes Bulk Session
 */
typedef struct {
    /**< Whether it is receiving or sending */
    TL_SessionMode_t SessionMode;
    /**< Length of data in bytes that should be transfered over bulk */
    uint64 Length;
} BulkContext_t;

/**     @} */

/**
 *      @addtogroup IO_File
 *      @{
 */

/*******************************************************************************
 * Types, constants
 ******************************************************************************/
/**
 *  Describes file context
 */
typedef struct {
    uint32 Descriptor; /**< File Descriptor */
} FileContext_t;

/**     @} */

/**
 *      @addtogroup IO_Wrap
 *      @{
 */

/*******************************************************************************
 * Types, constants
 ******************************************************************************/
/**
 *  Describes wrap context
 */
typedef struct {
    IO_Handle_t Parent;   /**< File descriptor of parent stream */
    uint64 Length;        /**< Length of current Wrap */
    uint64 Offset;        /**< Offset in parent stream */
    uint64 ReadPosition;  /**< Current read position */
    uint64 WritePosition; /**< Current write position */
} WrapContext_t;

/**     @} */

/**
 *      @addtogroup IO_Secure_Wrap
 *      @{
 */

/*******************************************************************************
 * Types, constants
 ******************************************************************************/
/**
 *  Defines the different states of Bulk Session
 */
typedef enum {
    SECURE_WRAP_IDLE = 0,
    SECURE_WRAP_PROCESSING,
    SECURE_WRAP_FINISHED
} IO_SecureWrapState_t;

/**
 *  Describes secure wrap context
 */
typedef struct {
    /**< File descriptor of parent stream */
    IO_Handle_t Parent;
    /**< Secure wrap state */
    uint32 State;
    /**< Secure wrap vector index */
    uint32 VectorId;
    /**< Secure wrap Offset */
    uint64 OffsetInParent;
    /**< Length of current Wrap */
    uint64 Length;
    /**< Current read position */
    uint64 ReadPosition;
    /**< Current write position */
    uint64 WritePosition;
    /**< Size of the hashblocks */
    uint64 HashedBlockSize;
    /**< VerifyContext to use when calling verification functions */
    VerifyContext_t *VerifyContext;
} SecureWrapContext_t;

/**     @} */

/**
 *      @addtogroup IO_BDM
 *      @{
 */

/*******************************************************************************
 * Types, constants
 ******************************************************************************/
/**
 *  Describes BDM context
 */
typedef struct {
    uint32 Unit;          /**< BDM Unit number */
    uint32 PageSizeInBytes; /**< Page size in BDM Unit */
    uint32 PageSizeShiftLeft; /**< Page size in BDM is 2^PageSizeShiftLeft */
    uint64 Length;        /**< Length of BDM Unit */
    uint32 LogicalSectors;/**< Number of logical sectors in BDM */
    uint32 *FirstBad_p;   /**< First bad block address */
    uint64 ReadPosition;  /**< Current read position */
    uint64 WritePosition; /**< Current write position */
    FPD_Config_t *FPDConfig_p; /**< Flash device configuration */
} BDMContext_t;

/**     @} */

/**
 *      @addtogroup IO_BAM
 *      @{
 */

/*******************************************************************************
 * Types, constants
 ******************************************************************************/
/**
 *  Describes BAM context
 */
typedef struct {
    uint64 Length;        /**< Length of BAM Unit */
    uint64 ReadPosition;  /**< Current read position */
    uint64 WritePosition; /**< Current write position */
    uint32 Unit;          /**< Cabs Unit */
    FPD_Config_t *FPDConfig_p; /**< Flash device configuration */
} BAMContext_t;

/**     @} */

/**
 *      @addtogroup IO_FPD
 *      @{
 */

/*******************************************************************************
 * Types, constants
 ******************************************************************************/
/**
 *  Describes FPD context
 */
typedef struct {
    uint64 Length;        /**< Length of FPD Unit */
    uint64 ReadPosition;  /**< Current read position */
    uint64 WritePosition; /**< Current write position */
    uint32 PageSizeInBytes; /**< Page size in FPD */
    uint32 PageSizeShiftLeft; /**< Page size in FPD is 2^PageSizeShiftLeft */
    uint32 PageExtraSize; /**< Extra memory for every page */
    boolean RedundantAreaIncluded; /**< Set to TRUE if redundant area is included. */
    uint8 FlashDevice;  /**< Flash device number */
} FPDContext_t;

/**     @} */

/*******************************************************************************
 * Types, constants
 ******************************************************************************/
/** List of initialized IO instances */
extern IO_Entry_t *IO_Entries;

/*******************************************************************************
 * Declaration of module internal functions
 ******************************************************************************/
/*
 * Returns IO vector index of the first fit
 *
 * @param [in] BuffersNeeded    for the IO Channel.
 *
 * @retval  VectorIndex    if free IO vector with defined resources is available
 * @retval  IO_VECTOR_INVALID_HANDLE     if there is no free IO vector,
 *
 */
uint32 IO_FindFreeVector(const uint32 BuffersNeeded);

/**
 * Returns initialized Handle to IO channel if there is
 * at least one available.
 *
 * Before calling this function you have to initialize IO interface
 * and allocate space for IO context.
 *
 * This function is used by data source Open functions
 * to open IO Channel with defined Interface functions
 * and data context specific for the data source. This
 * way we abstract the uniqueness of every data source to
 * the generality of IO Layer.
 *
 * @param [in] Interface_p     Pointer to initialized IO Interface.
 * @param [in] Context_p       Pointer to initialized IO Context.
 *
 * @retval  IO_Handle          Initialized IO handle if IO channel is available,
 * @retval  IO_ERROR           if there are no more IO channels.
 *
 */
IO_Handle_t IO_Create(const IO_Interface_t *Interface_p, void *Context_p);

/** @} */

#endif /* _IO_SUBSYSTEM_H_*/
