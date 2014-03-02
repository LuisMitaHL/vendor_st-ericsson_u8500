#ifndef _FLASH_DUMP_AREA_REPEAT_H_
#define _FLASH_DUMP_AREA_REPEAT_H_
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

/*******************************************************************************
 * Types, constants
 ******************************************************************************/
typedef struct {
    DumpArea_t  State;
    IO_Handle_t ReadHandle;
    IO_Handle_t WriteHandle;
    IO_Handle_t VectorIndex;
    IO_Handle_t NextVectorIndex;
    uint64      VectorLength;
    uint64      NextVectorLength;
    uint64      SourceDataLength;
    uint64      BytesWritten;
    uint64      WritePosition;
    uint64      ReadPosition;
    boolean     VectorPending;
} DumpAreaParameters_t;

typedef struct {
    uint64 Start;
    uint64 Length;
    uint32 RedundantArea;
    void  *Source_p;
    void  *Target_p;
} DumpInputParameters_t;

/** @} */
#endif /*_FLASH_DUMP_AREA_REPEAT_H_*/
