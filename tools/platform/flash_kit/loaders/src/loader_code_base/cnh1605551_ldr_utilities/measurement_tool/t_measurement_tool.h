/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
#ifndef T_MEASUREMENT_TOOL_H_
#define T_MEASUREMENT_TOOL_H_

/**
 * @addtogroup ldr_communication_serv
 * @{
 *    @addtogroup measurement_serv
 *    @{
 *
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "t_basicdefinitions.h"
#include "t_time_utilities.h"

/** */
typedef enum {
    FLASH_VIA_BULK_TRANSFER,
} MeasurementType_t;

/** Type of parameters for measurement. */
typedef enum {
    START_SESSION_TIME,
    END_SESSION_TIME,
    RECEIVED_CHUNK_TIME ,
    START_FLASH_CHUNK_TIME,
    END_FLASH_CHUNK_TIME,
    START_COMMAND_TIME,
    END_COMMAND_TIME,
} MeasurementParameter_t;

/** Structure for Chunk Data measurement */
typedef struct ChunkMeasurement_s {
    struct ChunkMeasurement_s *PreviousDataBlock_p;
    uint32 ChunkId;
    uint32 ReceivedChunkTime;
    uint32 StartFlashChunkTime;
    uint32 EndFlashChunkTime;
    struct ChunkMeasurement_s *NextDataBlock_p;
} ChunkMeasurement_t;

/** structure for Session measurement */
typedef struct SessionMeasurement_s {
    struct SessionMeasurement_s *PreviousDataBlock_p;
    uint32 SessionId;
    uint32 OpenSessionTime;
    uint32 CloseSessionTime;
    uint32 ChunkNr;
    ChunkMeasurement_t *ChunkMeasurement_p;
    struct SessionMeasurement_s *NextDataBlock_p;
} SessionMeasurement_t;

/** structure for fllash file procedure measurement */
typedef struct ProcesFileMeasurement_s {
    struct ProcesFileMeasurement_s *PreviousBlock_p;
    uint32 OpenTime;
    uint32 CloseTime;
    struct ProcesFileMeasurement_s *NextBlock_p;
} ProcesFileMeasurement_t;

/** Measurement data context structure*/
typedef struct {
    uint32                                    SessionNr;
    SessionMeasurement_t        *SessionMeasurement_p;
    ProcesFileMeasurement_t *ProcesFileMeasurement_p;
} MeasurementDataContext_t;

/** Structure for global data measurement */
typedef struct {
    uint32 Type;
    uint32 TimerId;
    uint32 StartCommandTime;
    uint32 EndCommandTime;
    MeasurementDataContext_t *MeasurementData_p;
    Timer_t Timer;
    uint32 ResultLength;
    uint8 *Result_p;
} Measurement_t;

/** @} */
/** @} */
#endif /* T_MEASUREMENT_TOOL_H_ */
