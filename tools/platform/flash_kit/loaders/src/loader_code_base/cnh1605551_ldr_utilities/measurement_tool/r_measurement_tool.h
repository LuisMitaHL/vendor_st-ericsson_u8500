/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
#ifndef R_MEASUREMENT_TOOL_H_
#define R_MEASUREMENT_TOOL_H_
/**
 * @addtogroup ldr_communication_module
 * @{
 *    @addtogroup measurement_serv Measurement services
 *    Measurement functions is tool for performance measurement. Can be used
 *    for flashing performance, communication performance and i.t.c.
 *
 *    @{
 *
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "t_basicdefinitions.h"
#include "t_measurement_tool.h"
#include "error_codes.h"

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/

#define MP(x, y, z) Do_Measurement_AddMeasurementData( x, Do_Timer_ReadTime(NULL,x->TimerId), y, z);
#define MFINISH(x) Do_Measurement_CompleteResults( x);
/**
 * Initialization of measurement module.
 *
 * @param [in] MeasurementData_p Pointer to curren measurement data block.
 *
 * @retval  E_SUCCESS If initialization is done.
 */
ErrorCode_e Do_Measurment_Init(Measurement_t *MeasurementData_p);

/**
 * This function create general measurement data block.
 *
 * @param [in]  Communication_p    Communication module context.
 * @param [in]  MeasurementData_pp Pointer to the created measurement data block.
 * @param [in]  Type               Type of measurement.
 *
 * @retval E_SUCCESS         After successful execution.
 * @retval E_ALLOCATE_FAILED If failed to allocate session measurement
 *                           data block.
 */
ErrorCode_e  Do_Measurement_CreateMeasurementData(Communication_t *Communication_p, Measurement_t    **MeasurementData_pp, uint32 Type);

/**
 * This function collect all data from measurement  buffer.
 *
 * @param [in]                      Communication_p    Communication module context.
 * @param [in, out]             Data_pp Point to pointer that should be initialized to point
 *                          on outgoing buffer.
 *
 * @return                 Number of characters successfully readied from measurement
 *                          buffer.
 */
uint32  Do_Measurement_GetMeasurementData(Communication_t *Communication_p, uint8 **Data_pp);

/**
 * This function create measurement data block for bulk session.
 *
 * @param [in]  Communication_p            Communication module context.
 * @param [in]  SessionMeasurement_pp      Pointer to the created session
 *                                         measurement data block.
 * @param [in]  PreviousSessionDataBlock_p Pointer to the previouse session
 *                                         measurement data block.
 * @param [in]  ChunkNr                    Number of chunks in this session.
 * @param [in]  Session                    Session number.
 *
 * @retval E_SUCCESS         After successful execution.
 * @retval E_ALLOCATE_FAILED If failed to allocate session measurement
 *                           data block.
 */
ErrorCode_e  Do_Measurement_CreateSessionMeasurementData(Communication_t *Communication_p,
        SessionMeasurement_t **SessionMeasurement_pp,
        SessionMeasurement_t *PreviousSessionDataBlock_p,
        uint32 ChunkNr, uint32 Session);

/**
 * This function create measurement data block for measure chunks transfer.
 *
 * @param [in]  Communication_p            Communication module context.
 * @param [in]  ChunkMeasurement_pp        Pointer to the created chunk
 *                                         measurement data block.
 * @param [in]  PreviousChunkDataBlock_p   Pointer to the previouse chunk
 *                                         measurement data block.
 * @param [in]  ChunkId                    Chunk ID.
 *
 * @retval E_SUCCESS         After successful execution.
 * @retval E_ALLOCATE_FAILED If failed to allocate session measurement
 *                           data block.
 */
ErrorCode_e  Do_Measurement_CreateChunkMeasurementData(Communication_t *Communication_p,
        ChunkMeasurement_t **ChunkMeasurement_pp,
        ChunkMeasurement_t *PreviousChunkDataBlock_p,
        uint32 ChunkId);
/**
 * This function destroy measurement data block for bulk session.
 *
 * @param [in]  MeasurementData_pp Pointer to the measurement data block.
 *
 * @retval E_SUCCESS         After successful execution.
 */
ErrorCode_e  Do_Measurement_DestroySessionMeasurementData(SessionMeasurement_t    **MeasurementData_pp);

/**
 * This function destroy measurement data block for bulk chunks.
 *
 * @param [in]  MeasurementData_pp Pointer to the main measurement data block.
 *
 * @retval E_SUCCESS         After successful execution.
 */
ErrorCode_e  Do_Measurement_DestroyChunkMeasurementData(ChunkMeasurement_t    **MeasurementData_pp);

/**
 * This function destroy general measurement data block.
 *
 * @param [in]  MeasurementData_pp Pointer to the main measurement data block.
 *
 * @retval E_SUCCESS         After successful execution.
 */
ErrorCode_e  Do_Measurement_DestroyMeasurementData(Measurement_t    **MeasurementData_pp);

/**
 * This function make zero reference for all measured data.
 *
 * @param [in]  MeasurementData_p Pointer to the main measurement data block.
 *
 * @retval E_SUCCESS         After successful execution.
 */
ErrorCode_e  Do_Measurement_ZeroReferenceMeasurementData(Measurement_t    *MeasurementData_p);

/**
 * This function add measured time in specified measurement data block.
 *
 * @param [in]  MeasurementData_p Pointer to the main measurement data block.
 * @param [in]  Time              Measured time.
 * @param [in]  ChunkID           Chunk ID
 * @param [in]  Parameter         Parameter in the measurement data block.
 *
 * @retval E_SUCCESS         After successful execution.
 */
ErrorCode_e  Do_Measurement_AddMeasurementData(Measurement_t *MeasurementData_p, uint32 Time, uint32 ChunkID, MeasurementParameter_t Parameter);

/**
 *
 */
ErrorCode_e  Do_Measurement_CompleteResults(Measurement_t *Measurement_p);

/** @} */
/** @} */
#endif /* R_MEASUREMENT_TOOL_H_ */
