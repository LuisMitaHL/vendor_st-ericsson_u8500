#ifndef INCLUSION_GUARD_T_CATDEFINITIONS_H
#define INCLUSION_GUARD_T_CATDEFINITIONS_H

/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#define CAT_ID_START_BIT 21
#define CAT_ID_FIRST_VALUE 0x00200000
#define META_DATA_MAX_VALUE 0x0000003F
#define EVENTID_OFFSET_MAX_VALUE 0x0000FFFF

/* ---------- Category Interval for Signal and Event Identitites ---------*/
#define CAT_INTERVAL_DEFINITION(CatId)                                                     \
  enum {                                                                                   \
    SIGID_INTERVAL_START_V2_##CatId = CatId << CAT_ID_START_BIT,                           \
    SIGID_INTERVAL_END_V2_##CatId = ((CatId + 1) << CAT_ID_START_BIT) - 1,                 \
    EVENTID_INTERVAL_START_##CatId = CatId << CAT_ID_START_BIT,                            \
    EVENTID_INTERVAL_END_##CatId = (CatId << CAT_ID_START_BIT) + EVENTID_OFFSET_MAX_VALUE  \
  };

/* --------------------- Signal Identities --------------------------------*/
/******************************************************************************
* Bit pattern
*        Category Id               Sig Id Offset            Meta Info    
*   31                  21 20                           6 5          0      
******************************************************************************/

#define SIGID_INTERVAL_START_V2(Id)                \
  typedef enum {                                   \
    SIGID_INTERVAL_START_V2_UNIQUE_TEST_ON_##Id =  \
    SIGID_INTERVAL_START_V2_##Id - 1,

#define SIGID_INTERVAL_END_V2(Id)                                         \
    SIGID_INTERVAL_END_V2_UNIQUE_TEST_ON_##Id,                            \
    SIGID_INTERVAL_TEST_V2_##Id =                                         \
    1 / (int)( ( SIGID_INTERVAL_END_V2_##Id                      >=       \
                 SIGID_INTERVAL_END_V2_UNIQUE_TEST_ON_##Id - 1      ) &&  \
               ( SIGID_INTERVAL_START_V2_UNIQUE_TEST_ON_##Id + 1 <        \
                 SIGID_INTERVAL_END_V2_UNIQUE_TEST_ON_##Id          )   ) \
  } SIGID_INTERVAL_V2_##Id##_t;                                           \
extern SIGID_INTERVAL_V2_##Id##_t SIGID_VAR_V2_##Id;


#define SIGID_INTERVAL_DEFINITION_V2(Id, Length) \
  SIGID_INTERVAL_START_V2_##Id,                  \
  SIGID_INTERVAL_END_V2_##Id = SIGID_INTERVAL_START_V2_##Id + ((Length) * 64) - 1

#define SIGID_DEFINITION_V2(SigIdName, SignalDataType) \
  SIGID_DEFINITION_WITH_META_INFO(SigIdName, SignalDataType, 0)

#define SIGID_DEFINITION_WITH_META_INFO(SigIdName, SignalDataType, MetaInfo)            \
  SIGID_TEMP_V2_##SigIdName,                                                            \
  SigIdName = (((SIGID_TEMP_V2_##SigIdName - 1) | META_DATA_MAX_VALUE) + 1) | MetaInfo, \
  SIGID_ANALYZER_INFO1_V2_##SigIdName = SigIdName,                                      \
  SIGID_ANALYZER_INFO2_V2_##SignalDataType##_##SigIdName = SigIdName

/******************************************************************************
* Values to use for Meta info
******************************************************************************/

#define SIGID_META_INFO_SIGNAL_HANDLER 0x01
#define SIGID_META_INFO_WAIT_MODE      0x02

/******************************************************************************
* Support for extracting meta info from a signal identity
******************************************************************************/

#define SIGNALID_IS_NOT_LEGACY(Primitive) \
  ((Primitive) > CAT_ID_FIRST_VALUE)

#define SIGNAL_HAS_SIGNAL_HANDLER(Primitive) \
  ((((Primitive) & SIGID_META_INFO_SIGNAL_HANDLER) != 0) && SIGNALID_IS_NOT_LEGACY(Primitive))

#define SIGNAL_IS_WAIT_MODE(Primitive) \
  ((((Primitive) & SIGID_META_INFO_WAIT_MODE)      != 0) && SIGNALID_IS_NOT_LEGACY(Primitive))

/******************************************************************************
* Support for signal casting etc.
******************************************************************************/

#define SIGNAL_CAST(Type, Signal_p) (/*lint --e(826)*/(Type)(Signal_p))

#define SIGNAL_DYNAMIC_CAST(Signal_p, SignalId, SignalType) \
  SIGNAL_CAST(SignalType, (((union SIGNAL*)Signal_p)->Primitive == SIGID_ANALYZER_INFO2_V2_##SignalType##_##SignalId) ? Signal_p : NIL)

#define SIGNAL_ID_TYPE_ASSERT(Signal_p, SignalId, SignalType) \
  B_ASSERT(((union SIGNAL*)Signal_p)->Primitive == SIGID_ANALYZER_INFO2_V2_##SignalType##_##SignalId)

/* ---------------------- Event Identities --------------------------------*/
/******************************************************************************
* Bit pattern
*          Category Id       Reserved       Event Channel Id Offset          
*   31                 21 20         16 15                             0         
******************************************************************************/

#define EVENTID_INTERVAL_START(Id)                 \
  typedef enum {                                   \
    EVENTID_INTERVAL_START_UNIQUE_TEST_ON_##Id =   \
    EVENTID_INTERVAL_START_##Id - 1,

#define EVENTID_DEFINITION(EventIdName) \
  EventIdName,                          \
  EVENTID_ANALYZER_INFO1_##EventIdName = EventIdName

#define EVENTID_INTERVAL_END(Id)                                         \
    EVENTID_INTERVAL_END_UNIQUE_TEST_ON_##Id,                            \
    EVENTID_INTERVAL_TEST_##Id =                                         \
    1 / (int)( ( EVENTID_INTERVAL_END_##Id                      >=       \
                 EVENTID_INTERVAL_END_UNIQUE_TEST_ON_##Id - 1      ) &&  \
               ( EVENTID_INTERVAL_START_UNIQUE_TEST_ON_##Id + 1 <        \
                 EVENTID_INTERVAL_END_UNIQUE_TEST_ON_##Id          )   ) \
  }EVENTID_INTERVAL_##Id##_t;                                            \
extern EVENTID_INTERVAL_##Id##_t EVENTID_VAR_##Id;

/* ---- Macros for signal and event id definitions using explicitly allocated Offset ---- */

/** The maximum offset that can be assigned to a signal */
#define CAT_MAX_SIGNAL_OFFSET 16384

/** The maximum offset that can be assigned to an event id. */
#define CAT_MAX_EVENT_ID_OFFSET 100

/**
 * Defines a new signal within the specified function category.
 *
 * @param CatId          The category identity (has name CAT_ID_<CAT>).
 * @param SigIdName      The name of the signal.
 * @param Offset         The offset of the signal; must be written as a plain decimal number and must
 *                       be unique within the function category. Once allocated it must never be changed!
 *                       Must be a number where Offset >= 0 and Offset < CAT_MAX_SIGNAL_OFFSET.
 * @param SignalDataType The data type that is carried by the signal. Note that the data type is only used
 *                       as a parameter for purpose of source code analysis - the actual type is not used in 
 *                       the macro and the .h-file containing the data type declaration doesn't have to be included.
 */
#define SIGID_DEFINITION_V3(CatId, SigIdName, Offset, SignalDataType)                                   \
enum {                                                                                                  \
  SIGID_DEFINITION_TEST_##CatId##_##Offset = 1 / (int) (Offset >= 0 && Offset < CAT_MAX_SIGNAL_OFFSET), \
  SigIdName = (CatId << CAT_ID_START_BIT) + (META_DATA_MAX_VALUE + 1) * Offset,                         \
  SIGID_ANALYZER_INFO2_V2_##SignalDataType##_##SigIdName = SigIdName                                    \
};

/**
 * Defines a new signal with meta information within the specified function category
 * (used in for instance Unified SwBP style).
 *
 * @param CatId          The category identity (has name CAT_ID_<CAT>).
 * @param SigIdName      The name of the signal.
 * @param Offset         The offset of the signal; must be written as a plain decimal number and must
 *                       be unique within the function category. Once allocated it must never be changed!
 *                       Must be a number where Offset >= 0 and Offset < 8192.
 * @param SignalDataType The data type that is carried by the signal. Note that the data type is only used
 *                       as a parameter for purpose of source code analysis - the actual type is not used in 
 *                       the macro and the .h-file containing the data type declaration doesn't have to be included.
 * @param MetaInfo       Meta info.
 */
#define SIGID_DEFINITION_WITH_META_INFO_V2(CatId, SigIdName, Offset, SignalDataType, MetaInfo)          \
enum {                                                                                                  \
  SIGID_DEFINITION_TEST_##CatId##_##Offset = 1 / (int) (Offset >= 0 && Offset < CAT_MAX_SIGNAL_OFFSET), \
  META_CHECK_##SigIdName = 1 / (int) (MetaInfo >= 0 && MetaInfo <= META_DATA_MAX_VALUE),                \
  SigIdName = (CatId << CAT_ID_START_BIT) + (META_DATA_MAX_VALUE + 1) * Offset + MetaInfo,              \
  SIGID_ANALYZER_INFO2_V2_##SignalDataType##_##SigIdName = SigIdName                                    \
};

/**
 * Specifies that the given signal and offset are not to be used anymore.
 *
 * @param CatId     The category identity (has name CAT_ID_<CAT>)
 * @param SigIdName The name of the signal that is not to be used anymore.
 * @param Offset    The offset of the signal; must be written as a plain decimal number and must
 *                  be unique within the function category. Once allocated it must never be changed!
 *                  This is the same Offset that was specified when the signal was defined.
 */
#define REVOKED_SIGID_DEFINITION(CatId, SigIdName, Offset)                                             \
enum {                                                                                                 \
  SIGID_DEFINITION_TEST_##CatId##_##Offset = 1 / (int) (Offset >= 0 && Offset < CAT_MAX_SIGNAL_OFFSET) \
};

/**
 * Defines a new event id within the specified function category.
 *
 * @param CatId       The category identity (has name CAT_ID_<CAT>).
 * @param EventIdName The name of the event id.
 * @param Offset      The offset of the event id; must be written as a plain decimal number and must
 *                    be unique within the function category. Once allocated it must never be changed!
 *                    Must be a number where Offset >= 0 and Offset < CAT_MAX_EVENT_ID_OFFSET.
 */
#define EVENTID_DEFINITION_V2(CatId, EventIdName, Offset)                                                   \
enum {                                                                                                      \
  EVENTID_DEFINITION_TEST_##CatId##_##Offset = 1 / (int) (Offset >= 0 && Offset < CAT_MAX_EVENT_ID_OFFSET), \
  EventIdName = (CatId << CAT_ID_START_BIT) + Offset                                                        \
};

/**
 * Specifies that the given event id and offset are not to be used anymore.
 *
 * @param CatId       The category identity (has name CAT_ID_<CAT>)
 * @param EventIdName The name of the event id that is not to be used anymore.
 * @param Offset      The offset of the event id; must be written as a plain decimal number and must
 *                    be unique within the function category. Once allocated it must never be changed!
 *                    This is the same Offset that was specified when the event id was defined.
 */
#define REVOKED_EVENTID_DEFINITION(CatId, EventIdName, Offset)                                             \
enum {                                                                                                     \
  EVENTID_DEFINITION_TEST_##CatId##_##Offset = 1 / (int) (Offset >= 0 && Offset < CAT_MAX_EVENT_ID_OFFSET) \
};

#endif                          /* INCLUSION_GUARD_T_CATDEFINITIONS_H */
