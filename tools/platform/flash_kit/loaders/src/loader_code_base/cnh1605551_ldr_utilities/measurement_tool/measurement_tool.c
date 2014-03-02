/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
/**
 * @addtogroup ldr_communication_serv
 * @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <string.h>
#include "t_basicdefinitions.h"
#include "r_communication_service.h"
#include "r_measurement_tool.h"
#include "r_time_utilities.h"
#include "r_serialization.h"
#include "r_debug_macro.h"
#include "r_memory_utils.h"

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/

Measurement_t *Measurement_p;

static ChunkMeasurement_t *GetLastChunkDataBlock(ChunkMeasurement_t *ChunkMeasurement_p);
static SessionMeasurement_t *GetLastSessionDataBlock(SessionMeasurement_t *SessionMeasurement_p);
static ChunkMeasurement_t *FindChunkDataBlock(ChunkMeasurement_t *Measurement_p, uint32 ChunkID);
static uint32 CalculateSizeOfResults(SessionMeasurement_t *DataMeasurement_p, uint32 Type, uint32 *SessionNr_pp);
static void SessionDataSerialize(uint8 **Data_pp, SessionMeasurement_t *SessData_p, uint32 *Length_p);
static void ChunkDataSerialize(uint8 **Data_pp, ChunkMeasurement_t *ChunkData_p, uint32 *Length_p);
static ErrorCode_e CompleteBulkFlashResults(Measurement_t *Measurement_p);
static void TimerMeasurmentHandler(void *Data_p, void *Timer_p, void *Param_p);

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/
/*
 * Initialization of measurement module.
 *
 * @param [in] Measurement_p Pointer to curren measurement data block.
 *
 * @retval  E_SUCCESS If initialization is done.
 */
ErrorCode_e Do_Measurement_Init(Measurement_t *Measurement_p)
{
    return E_SUCCESS;
}

/*
 * This function create general measurement data block.
 *
 * @param [in]  Communication_p    Communication module context.
 * @param [in]  Measurement_pp Pointer to the created measurement data block.
 * @param [in]  Type               Type of measurement.
 *
 * @retval E_SUCCESS         After successful execution.
 * @retval E_ALLOCATE_FAILED If failed to allocate session measurement
 *                           data block.
 */
ErrorCode_e  Do_Measurement_CreateMeasurementData(Communication_t *Communication_p, Measurement_t **Measurement_pp, uint32 Type)
{
    ErrorCode_e ReturnValue = E_ALLOCATE_FAILED;

    if (NULL != *Measurement_pp) {
        ReturnValue = Do_Measurement_DestroyMeasurementData(Measurement_pp);

        if (E_SUCCESS != ReturnValue) {
            return ReturnValue;
        }
    }

    *Measurement_pp = (Measurement_t *)malloc(sizeof(Measurement_t));

    if (NULL == *Measurement_pp) {
        return E_ALLOCATE_FAILED;
    }

    (*Measurement_pp)->MeasurementData_p = malloc(sizeof(MeasurementDataContext_t));

    if (NULL == (*Measurement_pp)->MeasurementData_p) {
        return E_ALLOCATE_FAILED;
    }

    (*Measurement_pp)->MeasurementData_p->SessionNr = 0;
    (*Measurement_pp)->MeasurementData_p->SessionMeasurement_p = NULL;
    (*Measurement_pp)->MeasurementData_p->ProcesFileMeasurement_p = NULL;
    (*Measurement_pp)->Timer.Time = 3600000;
    (*Measurement_pp)->Timer.Time = 3600000;
    (*Measurement_pp)->Timer.PeriodicalTime = 0;
    (*Measurement_pp)->Timer.HandleFunction_p = TimerMeasurmentHandler;
    (*Measurement_pp)->TimerId = Do_Timer_TimerGet(NULL, &(*Measurement_pp)->Timer);
    (*Measurement_pp)->StartCommandTime = 0;
    (*Measurement_pp)->EndCommandTime = 0;
    (*Measurement_pp)->Type = Type;
    (*Measurement_pp)->ResultLength = 0;
    (*Measurement_pp)->Result_p = NULL;
    return E_SUCCESS;
}

/*
 * This function create general measurement data block.
 *
 * @param [in]  Communication_p    Communication module context.
 * @param [in]  Data_pp                      Pointer to the data block.
 * @param [in]  DataLength_p       Pointer to the data block length.
 *
 * @retval E_SUCCESS         After successful execution.
 * @retval E_ALLOCATE_FAILED If failed to allocate session measurement
 *                           data block.
 */
uint32  Do_Measurement_GetMeasurementData(Communication_t *Communication_p, uint8 **Data_pp)
{
    if (NULL == Measurement_p || 0 == Measurement_p->ResultLength || NULL == Measurement_p->Result_p) {
        return 0;
    }

    *Data_pp = (uint8 *)malloc(Measurement_p->ResultLength);
    ASSERT(NULL != *Data_pp);
    memcpy((uint8 *)*Data_pp, Measurement_p->Result_p, Measurement_p->ResultLength);
    return Measurement_p->ResultLength;
}

/*
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
        uint32 ChunkNr, uint32 Session)
{
    ChunkMeasurement_t *ChunkDataBlock_p = NULL;
    SessionMeasurement_t **NewSessionDataBlock_pp = NULL;
    SessionMeasurement_t *LastSessionMeasurement_p = NULL;
    uint32 i;
    ErrorCode_e ReturnValue = E_ALLOCATE_FAILED;

    if (*SessionMeasurement_pp != NULL) {
        LastSessionMeasurement_p = GetLastSessionDataBlock(*SessionMeasurement_pp);
        NewSessionDataBlock_pp = &LastSessionMeasurement_p->NextDataBlock_p;
    } else {
        NewSessionDataBlock_pp = SessionMeasurement_pp;
    }

    *NewSessionDataBlock_pp = (SessionMeasurement_t *)malloc(sizeof(SessionMeasurement_t));

    if (NULL == *SessionMeasurement_pp) {
        return ReturnValue;
    }

    (*NewSessionDataBlock_pp)->PreviousDataBlock_p = LastSessionMeasurement_p;
    (*NewSessionDataBlock_pp)->SessionId = Session;
    (*NewSessionDataBlock_pp)->ChunkNr = ChunkNr;
    (*NewSessionDataBlock_pp)->OpenSessionTime = 0;
    (*NewSessionDataBlock_pp)->CloseSessionTime = 0;
    (*NewSessionDataBlock_pp)->ChunkMeasurement_p = NULL;
    (*NewSessionDataBlock_pp)->NextDataBlock_p = NULL;

    for (i = 0; i < ChunkNr; i++) {
        ReturnValue = Do_Measurement_CreateChunkMeasurementData(Communication_p, &(*NewSessionDataBlock_pp)->ChunkMeasurement_p, ChunkDataBlock_p, i);
    }

    return ReturnValue;
}

/*
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
        uint32 ChunkId)
{
    ChunkMeasurement_t **NewChunkDataBlock_pp = NULL;
    ChunkMeasurement_t *LastChunkMeasurement_p = NULL;

    if (*ChunkMeasurement_pp != NULL) {
        LastChunkMeasurement_p = GetLastChunkDataBlock(*ChunkMeasurement_pp);
        NewChunkDataBlock_pp = &(LastChunkMeasurement_p)->NextDataBlock_p;
    } else {
        NewChunkDataBlock_pp = ChunkMeasurement_pp;
    }

    *NewChunkDataBlock_pp = (ChunkMeasurement_t *)malloc(sizeof(ChunkMeasurement_t));

    if (NULL == *NewChunkDataBlock_pp) {
        return E_ALLOCATE_FAILED;
    }

    (*NewChunkDataBlock_pp)->PreviousDataBlock_p = LastChunkMeasurement_p;
    (*NewChunkDataBlock_pp)->ChunkId = ChunkId;
    (*NewChunkDataBlock_pp)->ReceivedChunkTime = 0;
    (*NewChunkDataBlock_pp)->StartFlashChunkTime = 0;
    (*NewChunkDataBlock_pp)->EndFlashChunkTime = 0;
    (*NewChunkDataBlock_pp)->NextDataBlock_p = NULL;
    return E_SUCCESS;
}

/*
 * This function destroy measurement data block for bulk session.
 *
 * @param [in]  Measurement_pp Pointer to the measurement data block.
 *
 * @retval E_SUCCESS         After successful execution.
 */
ErrorCode_e  Do_Measurement_DestroySessionMeasurementData(SessionMeasurement_t **SessionMeasurement_pp)
{
    ErrorCode_e ReturnValue;
    SessionMeasurement_t *LastSessionDataBlock_p = NULL;

    if (*SessionMeasurement_pp != NULL) {
        LastSessionDataBlock_p = GetLastSessionDataBlock(*SessionMeasurement_pp);

        do {
            *SessionMeasurement_pp = LastSessionDataBlock_p->PreviousDataBlock_p;
            /* free the all chunks in the session data block */
            ReturnValue = Do_Measurement_DestroyChunkMeasurementData(&LastSessionDataBlock_p->ChunkMeasurement_p);

            if (ReturnValue != E_SUCCESS) {
                return ReturnValue;
            }

            /* free the session data block */
            free(LastSessionDataBlock_p);
            LastSessionDataBlock_p = *SessionMeasurement_pp;
        } while (LastSessionDataBlock_p->PreviousDataBlock_p != NULL);

        /* free the all chunks in the last session data block */
        ReturnValue = Do_Measurement_DestroyChunkMeasurementData(&LastSessionDataBlock_p->ChunkMeasurement_p);

        if (ReturnValue != E_SUCCESS) {
            return ReturnValue;
        }

        /* free the last session data block */
        free(LastSessionDataBlock_p);
    }

    return E_SUCCESS;
}

/*
 * This function destroy measurement data block for bulk chunks.
 *
 * @param [in]  Measurement_pp Pointer to the main measurement data block.
 *
 * @retval E_SUCCESS         After successful execution.
 */
ErrorCode_e  Do_Measurement_DestroyChunkMeasurementData(ChunkMeasurement_t **ChunkMeasurement_pp)
{
    ChunkMeasurement_t *LastChunkDataBlock_p = NULL;
    ChunkMeasurement_t *TmpLastChunkDataBlock_p = NULL;

    if (*ChunkMeasurement_pp != NULL) {
        LastChunkDataBlock_p = GetLastChunkDataBlock(*ChunkMeasurement_pp);
        TmpLastChunkDataBlock_p = LastChunkDataBlock_p->PreviousDataBlock_p;
        free(LastChunkDataBlock_p);

        while (TmpLastChunkDataBlock_p != NULL) {
            /* free the chunk data block */
            LastChunkDataBlock_p      = TmpLastChunkDataBlock_p;
            TmpLastChunkDataBlock_p = LastChunkDataBlock_p->PreviousDataBlock_p;
            free(LastChunkDataBlock_p);
        }//while(TmpLastChunkDataBlock_p != NULL);

        //    while(LastChunkDataBlock_p->PreviousDataBlock_p != NULL)
        //    {
        //      *ChunkMeasurement_pp = LastChunkDataBlock_p->PreviousDataBlock_p;
        //      /* free the chunk data block */
        //      free(LastChunkDataBlock_p);
        //      LastChunkDataBlock_p = *ChunkMeasurement_pp;
        //    }
        /* free the last chunk data block */
        //free(*ChunkMeasurement_pp);
    }

    return E_SUCCESS;
}

/*
 * This function destroy general measurement data block.
 *
 * @param [in]  Measurement_pp Pointer to the main measurement data block.
 *
 * @retval E_SUCCESS         After successful execution.
 */
ErrorCode_e  Do_Measurement_DestroyMeasurementData(Measurement_t **Measurement_pp)
{
    ErrorCode_e ReturnValue = E_INVALID_INPUT_PARAMETERS;

    if (NULL == *Measurement_pp) {
        return ReturnValue;
    }

    switch ((*Measurement_pp)->Type) {
    case FLASH_VIA_BULK_TRANSFER:

        /* free the all created session data blocks */
        if (NULL != (*Measurement_pp)->MeasurementData_p) {
            if (NULL != (*Measurement_pp)->MeasurementData_p->SessionMeasurement_p) {
                ReturnValue = Do_Measurement_DestroySessionMeasurementData(&(*Measurement_pp)->MeasurementData_p->SessionMeasurement_p);
                (*Measurement_pp)->MeasurementData_p->SessionMeasurement_p = NULL;
            }

            ReturnValue = E_SUCCESS;
            //free((*Measurement_pp)->MeasurementData_p->ProcesFileMeasurement_p);
            //free((*Measurement_pp)->MeasurementData_p);
        }

        if (E_SUCCESS != ReturnValue) {
            return ReturnValue;
        }

        break;

    default:
        //...
        break;
    }

    //free(*Measurement_pp);
    return ReturnValue;
}

/*
 * This function make zero reference for all measured data.
 *
 * @param [in]  Measurement_p Pointer to the main measurement data block.
 *
 * @retval E_SUCCESS         After successful execution.
 */
ErrorCode_e  Do_Measurement_ZeroReferenceMeasurementData(Measurement_t *Measurement_p)
{
    return E_SUCCESS;
}

/*
 * This function add measured time in specified measurement data block.
 *
 * @param [in]  Measurement_p Pointer to the main measurement data block.
 * @param [in]  Time              Measured time.
 * @param [in]  ChunkID           Chunk ID
 * @param [in]  Parameter         Parameter in the measurement data block.
 *
 * @retval E_SUCCESS         After successful execution.
 */
ErrorCode_e  Do_Measurement_AddMeasurementData(Measurement_t *Measurement_p, uint32 Time, uint32 ChunkID, MeasurementParameter_t Parameter)
{
    ErrorCode_e ReturnValue = E_FAILED_TO_FIND_CHUNK_DATA_BLOCK;
    uint32  NewTime;
    SessionMeasurement_t *SessionMeasurement_p = GetLastSessionDataBlock(Measurement_p->MeasurementData_p->SessionMeasurement_p);
    ChunkMeasurement_t *ChankDataBlock_p = NULL;
    NewTime = Measurement_p->Timer.Time - Time;

    switch (Parameter) {
    case START_SESSION_TIME:
        SessionMeasurement_p->OpenSessionTime = NewTime;
        ReturnValue = E_SUCCESS;
        break;

    case END_SESSION_TIME:
        SessionMeasurement_p->CloseSessionTime = NewTime;
        ReturnValue = E_SUCCESS;
        break;

    case RECEIVED_CHUNK_TIME :
        ChankDataBlock_p = FindChunkDataBlock(SessionMeasurement_p->ChunkMeasurement_p, ChunkID);

        if (ChankDataBlock_p != NULL) {
            ChankDataBlock_p->ReceivedChunkTime = NewTime;
            ReturnValue = E_SUCCESS;
        }

        break;

    case START_FLASH_CHUNK_TIME:
        ChankDataBlock_p = FindChunkDataBlock(SessionMeasurement_p->ChunkMeasurement_p, ChunkID);

        if (ChankDataBlock_p != NULL) {
            ChankDataBlock_p->StartFlashChunkTime = NewTime;
            ReturnValue = E_SUCCESS;
        }

        break;

    case END_FLASH_CHUNK_TIME:
        ChankDataBlock_p = FindChunkDataBlock(SessionMeasurement_p->ChunkMeasurement_p, ChunkID);

        if (ChankDataBlock_p != NULL) {
            ChankDataBlock_p->EndFlashChunkTime = NewTime;
            ReturnValue = E_SUCCESS;
        }

        break;

    case START_COMMAND_TIME:
        Measurement_p->StartCommandTime = NewTime;
        ReturnValue = E_SUCCESS;
        break;

    case END_COMMAND_TIME:
        Measurement_p->EndCommandTime = NewTime;
        ReturnValue = E_SUCCESS;
        break;

    default:
        //�
        break;
    }

    return ReturnValue;
}

/*
 *
 */
ErrorCode_e  Do_Measurement_CompleteResults(Measurement_t *Measurement_p)
{
    ErrorCode_e ReturnValue = E_FAILED_TO_FIND_CHUNK_DATA_BLOCK;
    //SessionMeasurement_t * TmpSessData_p = NULL;
    //uint8* Data_p = NULL;
    uint32 Size = 0;

    switch (Measurement_p->Type) {
    case FLASH_VIA_BULK_TRANSFER:
        Size = CalculateSizeOfResults(Measurement_p->MeasurementData_p->SessionMeasurement_p, Measurement_p->Type, (uint32 *)&Measurement_p->MeasurementData_p->SessionNr);

        if (Size != 0) {
            Size += 2 * sizeof(uint32);
            BUFFER_FREE(Measurement_p->Result_p);

            Measurement_p->Result_p = (uint8 *)malloc(Size);
            //    ASSERT(NULL != Measurement_p->Result_p);
            Measurement_p->ResultLength = Size;

            /* get result from the list of the session data blocks */
            ReturnValue = CompleteBulkFlashResults(Measurement_p);
        } else {
            Measurement_p->ResultLength = Size;
            BUFFER_FREE(Measurement_p->Result_p);

            Measurement_p->Result_p = NULL;
            ReturnValue = E_SUCCESS;
        }

        break;

    default:
        //...
        break;
    }

    return ReturnValue;
}

/*******************************************************************************
 * Definition of internal functions
 ******************************************************************************/
/*
 * This function create Timer callback.
 *
 * @param [in]  Data_p    Pointer to data.
 * @param [in]  Timer_p   Pointer to timer.
 * @param [in]  Param_p   Pointer to parameter.
 *
 */
void TimerMeasurmentHandler(void *Data_p, void *Timer_p, void *Param_p)
{
    ASSERT(FALSE);
}

/*
 * This function search the requested chunk data block in the list of the chunk
 * data blocks.
 *
 * @param [in]  Measurement_p  Pointer to the main measurement data block.
 * @param [in]  ChunkID        Chunk ID for requested chunk data block.
 *
 * @retval pointer Pointer to the requested chunk data block.
 * @retval NULL If the list of the chunk data blocks is empty.
 */
static ChunkMeasurement_t *FindChunkDataBlock(ChunkMeasurement_t *Measurement_p, uint32 ChunkID)
{
    ChunkMeasurement_t *TmpData_p = Measurement_p;

    if (TmpData_p != NULL) {
        while ((TmpData_p->NextDataBlock_p != NULL) && (TmpData_p->ChunkId != ChunkID)) {
            TmpData_p = TmpData_p->NextDataBlock_p;
        }
    }

    return TmpData_p;
}

/*
 * Get last chunk data block from the list of the chunk data blocks.
 *
 * @param [in]  ChunkMeasurement_p  Pointer to the main measurement data block.
 *
 * @retval pointer Pointer to the last chunk data block.
 * @retval NULL If the list of the chunk data blocks is empty.
 */
static ChunkMeasurement_t *GetLastChunkDataBlock(ChunkMeasurement_t *ChunkMeasurement_p)
{
    ChunkMeasurement_t *TmpData_p = ChunkMeasurement_p;

    if (TmpData_p != NULL) {
        while (TmpData_p->NextDataBlock_p != NULL) {
            TmpData_p = TmpData_p->NextDataBlock_p;
        }
    }

    return TmpData_p;
}

/*
 * Get last session data block from the list of the chunk data blocks.
 *
 * @param [in]  ChunkMeasurement_p  Pointer to the main measurement data block.
 *
 * @retval pointer Pointer to the last chunk data block.
 * @retval NULL If the list of the chunk data blocks is empty.
 */
static SessionMeasurement_t *GetLastSessionDataBlock(SessionMeasurement_t *SessionMeasurement_p)
{
    SessionMeasurement_t *TmpData_p = SessionMeasurement_p;

    if (TmpData_p != NULL) {
        while (TmpData_p->NextDataBlock_p != NULL) {
            TmpData_p = TmpData_p->NextDataBlock_p;
        }
    }

    return TmpData_p;
}


static ErrorCode_e CompleteBulkFlashResults(Measurement_t *Measurement_p)
{
    ErrorCode_e ReturnValue = E_SUCCESS;
    SessionMeasurement_t *TmpSessData_p = Measurement_p->MeasurementData_p->SessionMeasurement_p;
    ChunkMeasurement_t *TmpChunkData_p = NULL;
    uint8 *Data_p = Measurement_p->Result_p;

    if (TmpSessData_p != NULL) {
        memcpy(Data_p                               , (uint8 *)&Measurement_p->Type                                              , sizeof(uint32));
        memcpy(Data_p + sizeof(uint32), (uint8 *)&Measurement_p->MeasurementData_p->SessionNr, sizeof(uint32));
        Data_p += 2 * sizeof(uint32);
        //Measurement_p->ResultLength ++;
        //SessionDataSerialize(&Data_p, TmpSessData_p, &Measurement_p->ResultLength);
        TmpChunkData_p = TmpSessData_p->ChunkMeasurement_p;

        //while(TmpSessData_p->NextDataBlock_p != NULL)
        while (TmpSessData_p != NULL) {
            SessionDataSerialize(&Data_p, TmpSessData_p, &Measurement_p->ResultLength);
            TmpChunkData_p = TmpSessData_p->ChunkMeasurement_p;

            if (TmpChunkData_p != NULL) {
                ChunkDataSerialize(&Data_p, TmpChunkData_p, &Measurement_p->ResultLength);

                while (TmpChunkData_p->NextDataBlock_p != NULL) {
                    TmpChunkData_p = TmpChunkData_p->NextDataBlock_p;
                    ChunkDataSerialize(&Data_p, TmpChunkData_p, &Measurement_p->ResultLength);
                }
            }

            TmpSessData_p = TmpSessData_p->NextDataBlock_p;
        }
    }

    return ReturnValue;
}


static void SessionDataSerialize(uint8 **Data_pp, SessionMeasurement_t *SessData_p, uint32 *Length_p)
{
    put_uint32_le((void **)Data_pp, SessData_p->SessionId);
    put_uint32_le((void **)Data_pp, SessData_p->OpenSessionTime);
    put_uint32_le((void **)Data_pp, SessData_p->CloseSessionTime);
    put_uint32_le((void **)Data_pp, SessData_p->ChunkNr);
}


static void ChunkDataSerialize(uint8 **Data_pp, ChunkMeasurement_t *ChunkData_p, uint32 *Length_p)
{
    put_uint32_le((void **)Data_pp, ChunkData_p->ChunkId);
    put_uint32_le((void **)Data_pp, ChunkData_p->ReceivedChunkTime);
    put_uint32_le((void **)Data_pp, ChunkData_p->StartFlashChunkTime);
    put_uint32_le((void **)Data_pp, ChunkData_p->EndFlashChunkTime);
}

/*
 * Calculate size of the results.
 *
 * @param [in]  DataMeasurement_p  Pointer to the main measurement data block.
 * @param [in]  Type               Type of measurement data block.
 *
 * @retval Size Size of the results.
 * @retval 0 If the list of the chunk data blocks is empty.
 */
static uint32 CalculateSizeOfResults(SessionMeasurement_t *DataMeasurement_p, uint32 Type, uint32 *SessionNr_p)
{
    SessionMeasurement_t *TmpData_p = DataMeasurement_p;
    uint32 Size = 0;

    switch (Type) {
    case  FLASH_VIA_BULK_TRANSFER:

        if (TmpData_p != NULL) {
            *SessionNr_p = 0;

            //(SessionMeasurement_t*)TmpData_p = ((SessionMeasurement_t*)TmpData_p)->NextDataBlock_p;
            //while(((SessionMeasurement_t*)TmpData_p)->NextDataBlock_p != NULL)
            while (TmpData_p != NULL) {
                Size = Size + (((SessionMeasurement_t *)TmpData_p)->ChunkNr * 4 * sizeof(uint32));
                Size += (4 * sizeof(uint32));
                //(SessionMeasurement_t*)TmpData_p = ((SessionMeasurement_t*)TmpData_p)->NextDataBlock_p;
                TmpData_p = (SessionMeasurement_t *)TmpData_p->NextDataBlock_p;
                (*SessionNr_p)++;
            }
        }

        break;

    default:
        Size = 0;
        break;

    }

    return Size;
}

/** @} */
