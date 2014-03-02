/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef OST_TRACE_H_
#define OST_TRACE_H_

#include "DeviceParameter.h"


#define TRACEMSGID_TYPE                           uint32_t
#define MASTER_TRACE_ID                          (0x08)       /* for OST */
#define VERSION_BYTE                                 (0x05)        /* we've been told that!*/
#define PROTOCOL_ID_ASCII                        (0x02)        /* for bare ASCII messages data*/
#define PROTOCOL_ID_BINARY                     (0x03 )        /* OST is a binary protocol*/

/*
// Pictor v2 Functional Specification v1.1: Page No 112
00: USER I/F
01: Reserved         : Glace Stats Export
02: MASTERCCI
05: SMIARX
06: VIDEOPIPE0
07: VIDEOPIPE1
08: STAT0             : 256-bins histogram (AEC)
10: STAT2             : Auto Focus
16: STAT8             : Glace (AWB)
17: GPIO
20: Reserved         : Histogram Stats Export
*/

//----------------------------------------Memory Dump related-----------------------------------------------
//Trace logs control
typedef struct
{
	uint32_t  u32_BufferAddr; //Address of trace buffer
	uint32_t  u32_BufferSize; //Size of the trace buffer allocated
       uint32_t  u32_LogLevels;  // This is to select the levels to be enabled for traces. User should make a 16 bit word using the OST_TraceTypes_te enum's bit positions. Writing 1/0 will enable/disable the corresponding level...
	uint8_t    u8_LogEnable;    //Enable Traces
       uint8_t    u8_TraceMechanismSelect; //User should select value for this PE from TraceMsgOutput_te enum, depending upon mechanism to be selected.
}TraceLogsControl_ts;

extern volatile TraceLogsControl_ts g_TraceLogsControl;

/* Log messages select */
typedef enum {
	TraceMsgOutput_e_NOMSGS,  /** No log messages */
	TraceMsgOutput_e_XTI,          /** XTI */
	TraceMsgOutput_e_MEMORY_DUMP   /** Memory dumping */
} TraceMsgOutput_te;
/**
 * \enum        OST_TraceTypes_te
 * \brief          Enum to give the definition to various trace types ..
 * \ingroup     OstTrace
*/
typedef enum
{
TRACE_ERROR,
TRACE_WARNING,
TRACE_FLOW,
TRACE_DEBUG,
TRACE_API,
TRACE_OMX_API,
TRACE_OMX_BUFFER,
TRACE_RESERVED,
TRACE_USER1,
TRACE_USER2,
TRACE_USER3,
TRACE_USER4,
TRACE_USER5,
TRACE_USER6,
TRACE_USER7,
TRACE_USER8,
} OST_TraceTypes_te;

#if ((USE_OST_TRACES == PROTOCOL_ID_ASCII) || (USE_OST_TRACES == PROTOCOL_ID_BINARY))
    extern unsigned char st_timestamp[8];
    extern uint8_t   g_Channel;
#endif
/*######################### Open System Trace ste.h STARTS#######################################*/
/**
BTrace category ID for traces
*/
//const unsigned int KBTraceCategoryOpenSystemTrace = 0xA2u;
#define KBTraceCategoryOpenSystemTrace 0xA2u

#define GET_LINE					       __LINE__
#define GEN2(GET_FILE, GET_LINE)	FILE_##GET_FILE##_##GET_LINE
#define GEN1(GET_FILE, GET_LINE)	GEN2(GET_FILE, GET_LINE)
#define GEN_ID						GEN1(GET_FILE, GET_LINE)


// Macros

/** Preprocessor category for all traces off. This should not be used from traces */
#define OST_TRACE_CATEGORY_NONE 0x00000000

/** Preprocessor category for production traces */
#define OST_TRACE_CATEGORY_PRODUCTION 0x00000001

/** Preprocessor category for RnD traces */
#define OST_TRACE_CATEGORY_RND 0x00000002

/** Preprocessor category for performance measurement traces */
#define OST_TRACE_CATEGORY_PERFORMANCE_MEASUREMENT 0x00000004


// Case when Binary traces are selected at compile time.. User still can choose memory dump for traces
#if (USE_OST_TRACES == PROTOCOL_ID_BINARY)

/**
The default preprocessor category is RND
Component may override this by defining
OST_TRACE_LEVEL before including this file
*/
#if !defined (OST_TRACE_CATEGORY)
#define OST_TRACE_CATEGORY OST_TRACE_CATEGORY_RND
#endif

#if !defined (OST_TRACE_CHANNEL)
#define OST_TRACE_CHANNEL 99
#endif

#if !defined (OST_TRACE_ENTITY)
#define OST_TRACE_ENTITY 0x01
#endif

#if !defined (OST_TRACE_GROUP)
#define OST_TRACE_GROUP TRACE_FLOW
#endif

#define OSTTrace0_wrp(a, b, str, c, d, e) 							OSTTrace0(a, b, c, d, e)
#define OSTTrace1_wrp(a, b, str, c, d, e, f) 						OSTTrace1(a, b, c, d, e, f)
#define OSTTrace2_wrp(a, b, str, c, d, e, f, g) 				OSTTrace2(a, b, c, d, e, f, g)
#define OSTTrace3_wrp(a, b, str, c, d, e, f, g, h) 			OSTTrace3(a, b, c, d, e, f, g, h)
#define OSTTrace4_wrp(a, b, str, c, d, e, f, g, h, i) 	OSTTrace4(a, b, c, d, e, f, g, h, i)
#define OSTTraceData_wrp(a, b, str, c, d, e, f, g)  		OSTTraceData(a, b, c, d, e, f, g)



/**
Trace with no parameters

@param aCategory Preprocessor category for the trace
@param aGroupName Name of the trace group. The name is not left into code
@param aTraceName Name of the trace. The name is mapped to a 32-bit identifier and thus must be unique
@param aTraceText The trace text, which is parsed by the trace compiler.
                  The text is not left into the code
*/
	#define OstTraceDef0(aCategory, aGroupName, aTraceName, aTraceText) \
   		if ((aCategory) & OST_TRACE_CATEGORY)  \
     	OSTTrace0_wrp(KOstTraceComponentID, (aTraceName), aTraceText, 0, 0, 0)

/**
Trace with one 32-bit parameter

@param aCategory Preprocessor category for the trace
@param aGroupName Name of the trace group. The name is not left into code
@param aTraceName Name of the trace. The name is mapped to a 32-bit identifier and thus must be unique
@param aTraceText The trace text, which is parsed by the trace compiler.
                  The text is not left into the code
@param aParam The 32-bit value to be traced
*/
	#define OstTraceDef1(aCategory, aGroupName, aTraceName, aTraceText, aParam) \
   		if ((aCategory) & OST_TRACE_CATEGORY) \
    		OSTTrace1_wrp(KOstTraceComponentID, (aTraceName), aTraceText, (aParam), 0, 0, 0)

/**
Trace with 2 32-bit parameter

@param aCategory Preprocessor category for the trace
@param aGroupName Name of the trace group. The name is not left into code
@param aTraceName Name of the trace. The name is mapped to a 32-bit identifier and thus must be unique
@param aTraceText The trace text, which is parsed by the trace compiler.
                  The text is not left into the code
@param aParam1 The 32-bit value to be traced
@param aParam2 The 32-bit value to be traced
*/
	#define OstTraceDef2(aCategory, aGroupName, aTraceName, aTraceText, aParam1, aParam2) \
   		if ((aCategory) & OST_TRACE_CATEGORY) \
    		OSTTrace2_wrp(KOstTraceComponentID, (aTraceName), aTraceText, (aParam1), (aParam2), 0, 0, 0)
/**
Trace with 2 32-bit parameter

@param aCategory Preprocessor category for the trace
@param aGroupName Name of the trace group. The name is not left into code
@param aTraceName Name of the trace. The name is mapped to a 32-bit identifier and thus must be unique
@param aTraceText The trace text, which is parsed by the trace compiler.
                  The text is not left into the code
@param aParam1 The 32-bit value to be traced
@param aParam2 The 32-bit value to be traced
*/
	#define OstTraceDef3(aCategory, aGroupName, aTraceName, aTraceText, aParam1, aParam2, aParam3) \
   		if ((aCategory) & OST_TRACE_CATEGORY) \
    		OSTTrace3_wrp(KOstTraceComponentID, (aTraceName), aTraceText, (aParam1), (aParam2), (aParam3), 0, 0, 0)
/**
Trace with 2 32-bit parameter

@param aCategory Preprocessor category for the trace
@param aGroupName Name of the trace group. The name is not left into code
@param aTraceName Name of the trace. The name is mapped to a 32-bit identifier and thus must be unique
@param aTraceText The trace text, which is parsed by the trace compiler.
                  The text is not left into the code
@param aParam1 The 32-bit value to be traced
@param aParam2 The 32-bit value to be traced
@param aParam3 The 32-bit value to be traced
@param aParam4 The 32-bit value to be traced
*/
	#define OstTraceDef4(aCategory, aGroupName, aTraceName, aTraceText, aParam1, aParam2, aParam3, aParam4) \
   		if ((aCategory) & OST_TRACE_CATEGORY) \
    		OSTTrace4_wrp(KOstTraceComponentID, (aTraceName), aTraceText, (aParam1), (aParam2), (aParam3), (aParam4), 0, 0, 0)
/**
Trace with more than 32 bits of data

@param aCategory Preprocessor category for the trace
@param aGroupName Name of the trace group. The name is not left into code
@param aTraceName Name of the trace. The name is mapped to a 32-bit identifier and thus must be unique
@param aTraceText The trace text, which is parsed by the trace compiler.
                  The text is not left into the code
@param aPtr Pointer to the data to be traced
@param aLength Length of the data to be traced
*/
	#define OstTraceDefData(aCategory, aGroupName, aTraceName, aTraceText, aPtr, aLength) \
  		if ((aCategory) & OST_TRACE_CATEGORY) \
    		OSTTraceData_wrp(KOstTraceComponentID, (aTraceName), aTraceText, (aPtr), (aLength), 0, 0, 0)
/**
*************** RnD trace macros ***************
*/

/**
RnD trace with no parameters

@param aGroupName Name of the trace group. The name is not left into code
@param aTraceName Name of the trace. The name is mapped to a 32-bit identifier and thus must be unique
@param aTraceText The trace text, which is parsed by the trace compiler.
                  The text is not left into the code
*/
#if (TRACE_BUFF_ENABLE)
#define OstTraceInt0(aGroupName, aTraceText) \
        do{ \
        if(g_TraceLogsControl.u8_TraceMechanismSelect == TraceMsgOutput_e_MEMORY_DUMP) \
                {TraceMemoryDump(aTraceText, 0,0,0,0,aGroupName);} \
        else if(g_TraceLogsControl.u8_TraceMechanismSelect == TraceMsgOutput_e_XTI)\
                   {OstTraceDef0 (OST_TRACE_CATEGORY_RND, (aGroupName), GEN_ID, (aTraceText));} \
        else{}  /*Do Nothing, This also Includes case if(g_TraceLogsControl.u8_TraceMechanismSelect == TraceMsgOutput_e_NOMSGS)*/\
        } while(0)
#else
#define OstTraceInt0(aGroupName, aTraceText) \
                {OstTraceDef0 (OST_TRACE_CATEGORY_RND, (aGroupName), GEN_ID, (aTraceText));}

#endif
/**
RnD trace with one 32-bit parameter

@param aGroupName Name of the trace group. The name is not left into code
@param aTraceName Name of the trace. The name is mapped to a 32-bit identifier and thus must be unique
@param aTraceText The trace text, which is parsed by the trace compiler.
                  The text is not left into the code
@param aParam The 32-bit value to be traced
*/

#if (TRACE_BUFF_ENABLE)
#define OstTraceInt1(aGroupName, aTraceText, aParam) \
      do{ \
        if(g_TraceLogsControl.u8_TraceMechanismSelect == TraceMsgOutput_e_MEMORY_DUMP) \
                {TraceMemoryDump(aTraceText, aParam,0,0,0,aGroupName); }\
        else if(g_TraceLogsControl.u8_TraceMechanismSelect == TraceMsgOutput_e_XTI)\
                {OstTraceDef1 (OST_TRACE_CATEGORY_RND, (aGroupName), GEN_ID, (aTraceText), (aParam));}\
        else{} /*Do Nothing, This also Includes case if(g_TraceLogsControl.u8_TraceMechanismSelect == TraceMsgOutput_e_NOMSGS)*/\
        } while(0)
#else
#define OstTraceInt1(aGroupName, aTraceText, aParam) \
    {OstTraceDef1 (OST_TRACE_CATEGORY_RND, (aGroupName), GEN_ID, (aTraceText), (aParam));}
#endif

/**
RnD trace with more than 32 bits of data

@param aGroupName Name of the trace group. The name is not left into code
@param aTraceName Name of the trace. The name is mapped to a 32-bit identifier and thus must be unique
@param aTraceText The trace text, which is parsed by the trace compiler.
                  The text is not left into the code
@param aPtr Pointer to the data to be traced
@param aLength Length of the data to be traced
*/
#define OstTraceArray(aGroupName, aTraceText, aPtr, aLength) \
  OstTraceDefData (OST_TRACE_CATEGORY_RND, (aGroupName), GEN_ID, (aTraceText), (aPtr), (aLength))



/**
RnD trace with two parameters.
@param aGroupName Name of the trace group. The name is not left into code
@param aTraceName Name of the trace. The name is mapped to a 32-bit identifier and thus must be unique
@param aTraceText The trace text, which is parsed by the trace compiler.
                  The text is not left into the code
@param aParam1 The first parameter to be traced
@param aParam2 The second parameter to be traced
*/
#if (TRACE_BUFF_ENABLE)
#define OstTraceInt2( aGroupName, aTraceText, aParam1, aParam2 ) \
       do{ \
        if(g_TraceLogsControl.u8_TraceMechanismSelect == TraceMsgOutput_e_MEMORY_DUMP) \
                {TraceMemoryDump(aTraceText, aParam1,aParam2,0,0,aGroupName); }\
        else if(g_TraceLogsControl.u8_TraceMechanismSelect == TraceMsgOutput_e_XTI)\
               {OstTraceDef2( OST_TRACE_CATEGORY_RND, aGroupName, GEN_ID, aTraceText, aParam1, aParam2);}\
               else{} /*Do Nothing, This also Includes case if(g_TraceLogsControl.u8_TraceMechanismSelect == TraceMsgOutput_e_NOMSGS)*/\
        } while(0)
#else
#define OstTraceInt2(aGroupName, aTraceText, aParam1, aParam2) \
    {OstTraceDef2( OST_TRACE_CATEGORY_RND, aGroupName, GEN_ID, aTraceText, aParam1, aParam2);}
#endif
/**
RnD trace with three parameters.
@param aGroupName Name of the trace group. The name is not left into code
@param aTraceName Name of the trace. The name is mapped to a 32-bit identifier and thus must be unique
@param aTraceText The trace text, which is parsed by the trace compiler.
                  The text is not left into the code
@param aParam1 The first parameter to be traced
@param aParam2 The second parameter to be traced
@param aParam3 The third parameter to be traced
*/
#if (TRACE_BUFF_ENABLE)
#define OstTraceInt3( aGroupName, aTraceText, aParam1, aParam2, aParam3 ) \
      do{ \
        if(g_TraceLogsControl.u8_TraceMechanismSelect == TraceMsgOutput_e_MEMORY_DUMP) \
                {TraceMemoryDump(aTraceText, aParam1,aParam2,aParam3,0,aGroupName); }\
        else if(g_TraceLogsControl.u8_TraceMechanismSelect == TraceMsgOutput_e_XTI)\
                {OstTraceDef3( OST_TRACE_CATEGORY_RND, aGroupName, GEN_ID, aTraceText, aParam1, aParam2, aParam3);}\
        else{} /*Do Nothing, This also Includes case if(g_TraceLogsControl.u8_TraceMechanismSelect == TraceMsgOutput_e_NOMSGS)*/\
        } while(0)
#else
#define OstTraceInt3(aGroupName, aTraceText, aParam1, aParam2, aParam3) \
	{OstTraceDef3( OST_TRACE_CATEGORY_RND, aGroupName, GEN_ID, aTraceText, aParam1, aParam2, aParam3);}
#endif
/**
RnD trace with four parameters.
@param aGroupName Name of the trace group. The name is not left into code
@param aTraceName Name of the trace. The name is mapped to a 32-bit identifier and thus must be unique
@param aTraceText The trace text, which is parsed by the trace compiler.
                  The text itself is not left into the code
@param aParam1 The first parameter to be traced
@param aParam2 The second parameter to be traced
@param aParam3 The third parameter to be traced
@param aParam4 The fourth parameter to be traced
*/
#if (TRACE_BUFF_ENABLE)
#define OstTraceInt4( aGroupName, aTraceText, aParam1, aParam2, aParam3, aParam4 ) \
        do{ \
        if(g_TraceLogsControl.u8_TraceMechanismSelect == TraceMsgOutput_e_MEMORY_DUMP) \
                {TraceMemoryDump(aTraceText, aParam1,aParam2,aParam3,aParam4,aGroupName); }\
        else if(g_TraceLogsControl.u8_TraceMechanismSelect == TraceMsgOutput_e_XTI)\
                {OstTraceDef4( OST_TRACE_CATEGORY_RND, aGroupName, GEN_ID, aTraceText, aParam1, aParam2, aParam3, aParam4);}\
        else{} /*Do Nothing, This also Includes case if(g_TraceLogsControl.u8_TraceMechanismSelect == TraceMsgOutput_e_NOMSGS)*/\
        } while(0)
#else
#define OstTraceInt4(aGroupName, aTraceText, aParam1, aParam2, aParam3, aParam4) \
                {OstTraceDef4( OST_TRACE_CATEGORY_RND, aGroupName, GEN_ID, aTraceText, aParam1, aParam2, aParam3, aParam4);}
#endif

// Case when ASCII traces are selected at compile time.. User still can choose memory dump for traces
#elif (USE_OST_TRACES == PROTOCOL_ID_ASCII)

#define OstTraceInt0(Level, pString) \
        do{ \
        if(g_TraceLogsControl.u8_TraceMechanismSelect == TraceMsgOutput_e_MEMORY_DUMP) \
                {TraceMemoryDump(pString, 0,0,0,0,Level); }\
        else if(g_TraceLogsControl.u8_TraceMechanismSelect == TraceMsgOutput_e_XTI)\
                 {OST_Printf(Level, pString, 0,0,0,0);}\
        else{} /*Do Nothing, This also Includes case if(g_TraceLogsControl.u8_TraceMechanismSelect == TraceMsgOutput_e_NOMSGS)*/\
        } while(0)

#define OstTraceInt1(Level, pString, f_Param1) \
       do{ \
        if(g_TraceLogsControl.u8_TraceMechanismSelect == TraceMsgOutput_e_MEMORY_DUMP) \
                {TraceMemoryDump(pString, f_Param1,0,0,0,Level);} \
        else if(g_TraceLogsControl.u8_TraceMechanismSelect == TraceMsgOutput_e_XTI)\
                 {OST_Printf(Level,pString, f_Param1,0,0,0);}\
         else{} /*Do Nothing, This also Includes case if(g_TraceLogsControl.u8_TraceMechanismSelect == TraceMsgOutput_e_NOMSGS)*/\
        } while(0)

#define OstTraceInt2(Level, pString, f_Param1, f_Param2) \
       do{ \
        if(g_TraceLogsControl.u8_TraceMechanismSelect == TraceMsgOutput_e_MEMORY_DUMP) \
                {TraceMemoryDump(pString, f_Param1,f_Param2,0,0,Level); }\
        else if(g_TraceLogsControl.u8_TraceMechanismSelect == TraceMsgOutput_e_XTI)\
                 {OST_Printf(Level,pString, f_Param1,f_Param2,0,0);}\
        else{} /*Do Nothing, This also Includes case if(g_TraceLogsControl.u8_TraceMechanismSelect == TraceMsgOutput_e_NOMSGS)*/\
        } while(0)

#define OstTraceInt3(Level, pString, f_Param1, f_Param2, f_Param3) \
       do{ \
        if(g_TraceLogsControl.u8_TraceMechanismSelect == TraceMsgOutput_e_MEMORY_DUMP) \
                {TraceMemoryDump(pString, f_Param1,f_Param2,f_Param3,0,Level); }\
        else if(g_TraceLogsControl.u8_TraceMechanismSelect == TraceMsgOutput_e_XTI)\
                 {OST_Printf(Level,pString, f_Param1,f_Param2,f_Param3,0);}\
        else{} /*Do Nothing, This also Includes case if(g_TraceLogsControl.u8_TraceMechanismSelect == TraceMsgOutput_e_NOMSGS)*/\
        } while(0)

#define OstTraceInt4(Level, pString, f_Param1, f_Param2, f_Param3, f_Param4)  \
       do{ \
        if(g_TraceLogsControl.u8_TraceMechanismSelect == TraceMsgOutput_e_MEMORY_DUMP) \
                {TraceMemoryDump(pString, f_Param1,f_Param2,f_Param3,f_Param4,Level); }\
        else if(g_TraceLogsControl.u8_TraceMechanismSelect == TraceMsgOutput_e_XTI)\
                 {OST_Printf(Level,pString, f_Param1,f_Param2,f_Param3,f_Param4); }\
        else{} /*Do Nothing, This also Includes case if(g_TraceLogsControl.u8_TraceMechanismSelect == TraceMsgOutput_e_NOMSGS)*/\
        } while(0)

#else // Case when Neither Binary Traces Nor ASCII traces are selected at compile time.. User still got the flexibility to choose memory dump of traces

#define OstTraceInt0(Level, pString) \
       do{ \
        if(g_TraceLogsControl.u8_TraceMechanismSelect == TraceMsgOutput_e_MEMORY_DUMP) \
                {TraceMemoryDump(pString, 0,0,0,0,Level);}\
        else{} /*Do Nothing, This also Includes case if(g_TraceLogsControl.u8_TraceMechanismSelect == TraceMsgOutput_e_NOMSGS)*/\
        } while(0)

#define OstTraceInt1(Level, pString, f_Param1) \
       do{ \
        if(g_TraceLogsControl.u8_TraceMechanismSelect == TraceMsgOutput_e_MEMORY_DUMP) \
                {TraceMemoryDump(pString, f_Param1,0,0,0,Level);}\
         else{} /*Do Nothing, This also Includes case if(g_TraceLogsControl.u8_TraceMechanismSelect == TraceMsgOutput_e_NOMSGS)*/\
        } while(0)

#define OstTraceInt2(Level, pString, f_Param1, f_Param2) \
       do{ \
        if(g_TraceLogsControl.u8_TraceMechanismSelect == TraceMsgOutput_e_MEMORY_DUMP) \
                {TraceMemoryDump(pString, f_Param1,f_Param2,0,0,Level);}\
        else{} /*Do Nothing, This also Includes case if(g_TraceLogsControl.u8_TraceMechanismSelect == TraceMsgOutput_e_NOMSGS)*/\
        } while(0)

#define OstTraceInt3(Level, pString, f_Param1, f_Param2, f_Param3) \
        do{ \
        if(g_TraceLogsControl.u8_TraceMechanismSelect == TraceMsgOutput_e_MEMORY_DUMP) \
                {TraceMemoryDump(pString, f_Param1,f_Param2,f_Param3,0,Level);}\
        else{} /*Do Nothing, This also Includes case if(g_TraceLogsControl.u8_TraceMechanismSelect == TraceMsgOutput_e_NOMSGS)*/\
        } while(0)

#define OstTraceInt4(Level, pString, f_Param1, f_Param2, f_Param3, f_Param4)  \
       do{ \
        if(g_TraceLogsControl.u8_TraceMechanismSelect == TraceMsgOutput_e_MEMORY_DUMP) \
                {TraceMemoryDump(pString, f_Param1,f_Param2,f_Param3,f_Param4,Level) };\
        else{} /*Do Nothing, This also Includes case if(g_TraceLogsControl.u8_TraceMechanismSelect == TraceMsgOutput_e_NOMSGS)*/\
        } while(0)

#endif //#if (USE_OST_TRACES == PROTOCOL_ID_BINARY)

/*########################## Exported Function Declaration #####################################*/
/*-------------------- Exported Function Declaration Common to ASCII and BINARY Trace implementation------------------*/
void Map_STM_Registers(void)TO_EXT_DDR_PRGM_MEM;
uint16_t Write_Payload(uint8_t *pu8_Payload, uint8_t *pBuffer, float_t  f_Param1, float_t f_Param2, float_t f_Param3, float_t f_Param4, uint16_t u16_MaxSizeToHandle)TO_EXT_DDR_PRGM_MEM;


void stm_trace_8(unsigned char   channel, unsigned char  data);
void stm_tracet_8(unsigned char   channel, unsigned char  data);
void stm_trace_16(unsigned char   channel, unsigned short  data);
void stm_tracet_16(unsigned char   channel, unsigned short  data);
void stm_trace_32(unsigned char   channel, unsigned int data);
void stm_tracet_32(unsigned char   channel, unsigned int data);
void stm_trace_64(unsigned char   channel, unsigned long long data);
void stm_tracet_64(unsigned char   channel, unsigned long long data);

/*-------------------- Exported Function Declaration for ASCII Trace implementation------------------*/
void OST_Printf(uint8_t Level, uint8_t *pString, float_t f_Param1, float_t f_Param2, float_t f_Param3, float_t f_Param4);

/*-------------------- Exported Function Declaration for BINARY Trace implementation------------------*/
void OSTTrace0 (TRACEMSGID_TYPE componentId, TRACEMSGID_TYPE tracemsgId, unsigned short has_handle, unsigned int aComponentHandle, TRACEMSGID_TYPE aParentHandle);
void OSTTrace1 (TRACEMSGID_TYPE componentId, TRACEMSGID_TYPE tracemsgId, unsigned int param, unsigned short has_handle, unsigned int aComponentHandle, TRACEMSGID_TYPE aParentHandle);
void OSTTrace2 (TRACEMSGID_TYPE componentId, TRACEMSGID_TYPE tracemsgId, unsigned int param1, unsigned int param2, unsigned short has_handle, unsigned int aComponentHandle, TRACEMSGID_TYPE aParentHandle);
void OSTTrace3 (TRACEMSGID_TYPE componentId, TRACEMSGID_TYPE tracemsgId, unsigned int param1, unsigned int param2, unsigned int param3, unsigned short has_handle, unsigned int aComponentHandle, TRACEMSGID_TYPE aParentHandle);
void OSTTrace4 (TRACEMSGID_TYPE componentId, TRACEMSGID_TYPE tracemsgId, unsigned int param1, unsigned int param2, unsigned int param3, unsigned int param4, unsigned short has_handle, unsigned int aComponentHandle, TRACEMSGID_TYPE aParentHandle);

/*-------------------- Exported Function Declaration for Memory Dump Trace implementation------------------*/
int32_t TraceLog_Init(void)TO_EXT_DDR_PRGM_MEM;  //Inintialization of Trace buffer structure
int32_t TraceMemoryDump(int8_t *pString, float_t f_Param1, float_t f_Param2, float_t f_Param3, float_t f_Param4, OST_TraceTypes_te level)TO_EXT_DDR_PRGM_MEM;


#endif // #ifndef OST_TRACE_H_

