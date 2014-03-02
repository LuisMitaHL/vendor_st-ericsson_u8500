/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * Trace identifiers
 */
#define OMXCOMPONENT "TUNING"
#define OMX_TRACE_UID 0x10

/* +CR337836 */
#define DECLARE_AUTOVAR
/* -CR337836 */

/*
 * Includes
 */
#include "tuning_types.h"
#include "tuning.h"
#include "tuning_iqset.h"
#include "tuning_user.h"
#include "tuning_writer.h"
#include "tuning_macros.h"
#include "tuning_osal.h"
#include <string.h>

/*
 * Local definitions
 */

#define HASH_ENTRY(id) {id,#id}

typedef struct
{
    t_tuning_error_code id;
    const char* name;
} t_tuning_error_entry;

static const t_tuning_error_entry KTuningErrorCodesList[] =
{
    HASH_ENTRY(TUNING_OK),
    HASH_ENTRY(TUNING_ALREADY_CONSTRUCTED),
    HASH_ENTRY(TUNING_INVALID_ARGUMENT),
    HASH_ENTRY(TUNING_MALLOC_ERROR),
    HASH_ENTRY(TUNING_NOT_CONSTRUCTED),
    HASH_ENTRY(TUNING_FILE_NOT_FOUND),
    HASH_ENTRY(TUNING_FILE_READ_ERROR),
    HASH_ENTRY(TUNING_FOUND_NO_ENTRIES),
    HASH_ENTRY(TUNING_TOO_MANY_ENTRIES),
    HASH_ENTRY(TUNING_PARAM_SIZE_MISMATCH),
    HASH_ENTRY(TUNING_VERSION_MISMATCH),
    HASH_ENTRY(TUNING_XML_ERROR),
    HASH_ENTRY(TUNING_DUPLICATE_PARAM),
    HASH_ENTRY(TUNING_PARAM_NOT_FOUND),
    HASH_ENTRY(TUNING_PARAM_TYPE_MISMATCH),
    HASH_ENTRY(TUNING_ARRAY_TOO_SMALL),
    HASH_ENTRY(TUNING_INVALID_IQSET_ID),
    HASH_ENTRY(TUNING_INVALID_USER_ID),
    HASH_ENTRY(TUNING_PARAMS_ARRAY_FULL),
    HASH_ENTRY(TUNING_COMMENTS_ARRAY_FULL),
    HASH_ENTRY(TUNING_FILE_CREATION_FAILED),
    HASH_ENTRY(TUNING_FILE_WRITE_ERROR),
    HASH_ENTRY(TUNING_BUFFER_TOO_SMALL),
    HASH_ENTRY(TUNING_SENSOR_ID_MISMATCH),
    HASH_ENTRY(TUNING_INTERNAL_ERROR)
};

#define KTuningErrorCodesListSize (sizeof(KTuningErrorCodesList)/sizeof(KTuningErrorCodesList[0]))

#define FlatIndex2Row(i,cols)                   ((i)/(cols))
#define FlatIndex2Col(i,cols)                   ((i)%(cols))
#define RowCol2FlatIndex(row,col,ContainerCols) (((row)*(ContainerCols))+(col))

// Prevent export of symbols
#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(CTuning);
#endif

/****************************************************************************/
/**
 * \brief     standard constructor
 * \author    vollejm*
 * Initialize properties to default values but does not perform any memory allocation
 **/
/****************************************************************************/
void CTuning::Reset()
{
   // Init private variables
   for(int i=0; i<IQSET_COUNT; i++) {
       for(int u=0; u<IQSET_USER_COUNT; u++) {
          iTuningData.iIqSet[i].user[u].pParamList = NULL;
          iTuningData.iIqSet[i].user[u].pLastParam = NULL;
          iTuningData.iIqSet[i].user[u].iParamCount = 0;
       }
   }
   iTuningData.iParamsPool.pParam = NULL;
   iTuningData.iParamsPool.iTotal = 0;
   iTuningData.iParamsPool.iUsed = 0;
   iTuningData.iValuesPool.pValue = NULL;
   iTuningData.iValuesPool.iTotal = 0;
   iTuningData.iValuesPool.iUsed = 0;
   iTuningData.iOperatingMode.name[0] = '\0';
   iTuningData.iStructVersion.structure = 0;
   iTuningData.iStructVersion.entries = 0;
   iTuningData.iConfigVersion.params = 0;
   iTuningData.iConfigVersion.values = 0;
   iTuningData.iSensorId.name[0] = '\0';
   iTuningData.iSensorId.manuf = 0;
   iTuningData.iSensorId.model = 0;
   iTuningData.iSensorId.rev = 0;
   iTuningData.iFirmwareVersion.major = 0;
   iTuningData.iFirmwareVersion.minor = 0;
   iTuningData.iFirmwareVersion.patch = 0;
   iTuningData.iSw3aVersion.major = 0;
   iTuningData.iSw3aVersion.minor = 0;
   iTuningData.iSw3aVersion.build = 0;
   iTuningData.iComments.count = 0;
   iConstructed = false;
   iInstanceName = "Anonymous";
   return;
}

/****************************************************************************/
/**
 * \brief     standard constructor
 * \author    vollejm*
 * Initialize properties to default values but does not perform any memory allocation
 **/
/****************************************************************************/
CTuning::CTuning()
{
   TUNING_FUNC_IN0();
   Reset();
   TUNING_INFO_MSG1("[%s] Instantiated\n", iInstanceName);
   TUNING_FUNC_OUT0();
   return;
}

/****************************************************************************/
/**
 * \brief     standard constructor
 * \author    vollejm*
 * Initialize properties to default values but does not perform any memory allocation
 **/
/****************************************************************************/
CTuning::CTuning(const char* aInstanceName)
{
    /* +CR337836 */
    GET_AND_SET_TRACE_LEVEL(tuning);
    /* -CR337836 */

   TUNING_FUNC_IN0();
   Reset();
   if(aInstanceName!=NULL) {
       iInstanceName = aInstanceName;
   }
   TUNING_INFO_MSG1("[%s] Instantiated\n", iInstanceName);
   TUNING_FUNC_OUT0();
   return;
}

/****************************************************************************/
/**
 * \brief   2nd stage constructor for the Tuning object.
 * \author  vollejm
 * \warning Must be called after instanciation of tuning object.
 *
 * This constructor does memory allocation and reports an error if the allocation fails.
 * As a standard constructor cannot report any error code, a 2 stage construction scheme is used.
 * Allocation is done dynamically but with fixed size: MAX_PES, and MAX_PAGES
 *
 **/
/****************************************************************************/
t_tuning_error_code CTuning::Construct(unsigned int aMaxParams)
{
   TUNING_FUNC_IN0();

   // Check if already constructed
   if(iConstructed==true)
   {
      TUNING_INFO_MSG0("Instance already constructed\n");
      TUNING_ASSERT(0);
      TUNING_FUNC_OUTR(TUNING_ALREADY_CONSTRUCTED);
      return TUNING_ALREADY_CONSTRUCTED;
   }

   // Check argumenst
   if(aMaxParams == 0)
   {
      TUNING_INFO_MSG1("Invalid argument: aParamCount=%u\n", aMaxParams);
      TUNING_ASSERT(0);
      TUNING_FUNC_OUTR(TUNING_INVALID_ARGUMENT);
      return TUNING_INVALID_ARGUMENT;
   }

   // Allocate Paramaters pool
   iTuningData.iParamsPool.pParam = new t_iqset_param[aMaxParams];
   if(iTuningData.iParamsPool.pParam == NULL)
   {
      TUNING_INFO_MSG1("Could not allocate Param pool list (%u entries)\n", aMaxParams);
      TUNING_ASSERT(0);
      TUNING_FUNC_OUTR(TUNING_MALLOC_ERROR);
      return TUNING_MALLOC_ERROR;
   }
   TUNING_INFO_MSG3("Allocated %u entries in Param pool: pParam=0x%p, size=%d\n", aMaxParams, iTuningData.iParamsPool.pParam, aMaxParams*sizeof(t_iqset_param));
   iTuningData.iParamsPool.iTotal = aMaxParams;

   // Param pool default init
   for(t_uint32 i=0; i<iTuningData.iParamsPool.iTotal; i++)
   {
#if defined(TUNING_PARAM_NAMES_ENABLED)
      iTuningData.iParamsPool.pParam[i].name[0] = '\0';
#endif
      iTuningData.iParamsPool.pParam[i].addr  = 0;
      iTuningData.iParamsPool.pParam[i].rows  = 0;
      iTuningData.iParamsPool.pParam[i].cols  = 0;
      iTuningData.iParamsPool.pParam[i].type  = IQSET_VT_UNKNOWN;
      iTuningData.iParamsPool.pParam[i].value = NULL;
      iTuningData.iParamsPool.pParam[i].next  = NULL;
   }
   iTuningData.iParamsPool.iUsed = 0;

   // Allocate Values pool
   const unsigned int iMaxValues = 16*iTuningData.iParamsPool.iTotal;
   iTuningData.iValuesPool.pValue = new u_iqset_value[iMaxValues];
   if(iTuningData.iValuesPool.pValue == NULL)
   {
      TUNING_INFO_MSG1("Could not allocate Value pool (%u entries)\n", iMaxValues);
      TUNING_ASSERT(0);
      TUNING_FUNC_OUTR(TUNING_MALLOC_ERROR);
      return TUNING_MALLOC_ERROR;
   }
   TUNING_INFO_MSG3("Allocated %d entries in Values pool: pValue=0x%p, size=%d\n", iMaxValues, iTuningData.iValuesPool.pValue, iMaxValues*sizeof(u_iqset_value));
   iTuningData.iValuesPool.iTotal = iMaxValues;

   // Init comments
   for(int i=0; i<COMMENTS_MAX_COUNT; i++)
   {
      iTuningData.iComments.pComment[i] = NULL;
   }

   // Construction done
   iConstructed = true;
   TUNING_FUNC_OUTR(TUNING_OK);
   return TUNING_OK;
}

/****************************************************************************/
/**
 * \brief  Tuning class destructor: free memory allocated for storing PE and pages
 * \author vollejm
 *
 **/
/****************************************************************************/
CTuning::~CTuning()
{
    TUNING_FUNC_IN0();
    Destroy();
    TUNING_FUNC_OUT0();
    return;
}

/****************************************************************************/
/**
 * \brief   free memory allocated by #Construct call
 * \author  vollejm
 **/
/****************************************************************************/
void CTuning::Destroy(void)
{
   TUNING_FUNC_IN0();

   // Delete Parameters pool
   if(iTuningData.iParamsPool.pParam != NULL)
   {
      TUNING_INFO_MSG1("Deleting Param pool: %p\n", iTuningData.iParamsPool.pParam);
      delete [] iTuningData.iParamsPool.pParam;
      iTuningData.iParamsPool.pParam = NULL;
      iTuningData.iParamsPool.iTotal = 0;
      iTuningData.iParamsPool.iUsed = 0;
   }

   // Delete Values pool
   if(iTuningData.iValuesPool.pValue != NULL)
   {
      TUNING_INFO_MSG1("Deleting Values pool: %p\n", iTuningData.iValuesPool.pValue);
      delete [] iTuningData.iValuesPool.pValue;
      iTuningData.iValuesPool.pValue = NULL;
      iTuningData.iValuesPool.iTotal = 0;
      iTuningData.iValuesPool.iUsed = 0;
   }

   // Delete comments if any
   for(int i=0; i<iTuningData.iComments.count; i++)
   {
      delete [] iTuningData.iComments.pComment[i];
      iTuningData.iComments.pComment[i] = NULL;
   }
   iTuningData.iComments.count = 0;

   // Reset members
   Reset();

   // Done
   iConstructed = false;
   TUNING_FUNC_OUT0();
   return;
}

/****************************************************************************/
/**
 * \brief   Parse a xml tuning file given as argument. Parsing is actually performed by #CTuningParser class
 * \author  vollejm
 * \in      aFileName: name of the file to parse. Should not include the absolute path
 *
 **/
/****************************************************************************/
const char* CTuning::GetInstanceName()
{
   return iInstanceName;
}
/****************************************************************************/
/**
 * \brief   Parse a xml tuning file given as argument. Parsing is actually performed by #CTuningParser class
 * \author  vollejm
 * \in      aFileName: name of the file to parse. Should not include the absolute path
 *
 **/
/****************************************************************************/
t_tuning_error_code CTuning::FetchTuningConfiguration(const char *aFileName)
{
   TUNING_FUNC_IN0();
   t_tuning_parser_error_code parserErr = TUNING_PARSER_OK;
   t_tuning_error_code ret = TUNING_OK;
   CTuningParser* pTuningParser = NULL;

   // Sanity check
   if( IsInstanceConstructed() == false) {
       TUNING_ASSERT(0);
       TUNING_FUNC_OUTR(TUNING_NOT_CONSTRUCTED);
       return TUNING_NOT_CONSTRUCTED;
   }
   if(aFileName==NULL) {
       TUNING_ASSERT(0);
       TUNING_FUNC_OUTR(TUNING_INVALID_ARGUMENT);
       return TUNING_INVALID_ARGUMENT;
   }

   // Instantiate a Parser
   pTuningParser = new CTuningParser;
   if(pTuningParser==NULL) {
      TUNING_ASSERT(0);
      TUNING_FUNC_OUTR(TUNING_MALLOC_ERROR);
      return TUNING_MALLOC_ERROR;
   }

   // Parse the file
   TUNING_PARAM_MSG2("[%s] Fetching tuning confiruration: %s\n", iInstanceName, aFileName);
   parserErr = pTuningParser->ParseFileContent( aFileName, &iTuningData);
   if(parserErr!=TUNING_PARSER_OK) {
       // Translate into appropriate error code
       TUNING_ASSERT(0);
       ret = ConvertErrorCode(parserErr);
   }
   else {
       ret = TUNING_OK;
   }

   // Deinstantiate the Parser
   delete pTuningParser;
   pTuningParser = NULL;

   TUNING_INFO_MSG1("Parsed %ld Parameters\n",GetParamCount());
   TUNING_FUNC_OUTR(ret);
   return ret;
}

/****************************************************************************/
/**
 * \brief  Parse a xml tuning buffer given as argument. Parsing is actually performed by #CTuningParser class
 * \author vollejm
 * \in     aFileBuffer: buffer to parse.
 *
 **/
/****************************************************************************/
t_tuning_error_code CTuning::FetchTuningConfiguration(char* aBuffer, const int aBufferSize)
{
   TUNING_FUNC_IN0();
   t_tuning_parser_error_code parserErr = TUNING_PARSER_OK;
   t_tuning_error_code ret = TUNING_OK;
   CTuningParser* pTuningParser = NULL;

   // Sanity check
   if( IsInstanceConstructed() == false) {
       TUNING_ASSERT(0);
       TUNING_FUNC_OUTR(TUNING_NOT_CONSTRUCTED);
       return TUNING_NOT_CONSTRUCTED;
   }
   if(aBuffer==NULL || aBufferSize==0) {
      TUNING_ASSERT(0);
      TUNING_FUNC_OUTR(TUNING_INVALID_ARGUMENT);
      return TUNING_INVALID_ARGUMENT;
   }

   // Instantiate a Parser
   pTuningParser = new CTuningParser;
   if(pTuningParser==NULL) {
      TUNING_ASSERT(0);
      TUNING_FUNC_OUTR(TUNING_MALLOC_ERROR);
      return TUNING_MALLOC_ERROR;
   }

   // Parse the buffer
   TUNING_PARAM_MSG3("[%s] Fetching tuning confiruration: buffer=%p, size=%d\n", iInstanceName, aBuffer, aBufferSize);
   parserErr = pTuningParser->ParseBufferContent( aBuffer, aBufferSize, &iTuningData);
   if(parserErr!=TUNING_PARSER_OK) {
       // Translate into appropriate error code
       TUNING_ASSERT(0);
       ret = ConvertErrorCode(parserErr);
   }
   else {
       ret = TUNING_OK;
   }

   // Deinstantiate the Parser
   delete pTuningParser;
   pTuningParser = NULL;

   TUNING_INFO_MSG1("Parsed %ld Parameters\n",GetParamCount());
   TUNING_FUNC_OUTR(ret);
   return ret;
}

/****************************************************************************/
/**
 * \brief   report the number of page elements in the tuning storage object
 * \author  vollejm
 * \return  number of pages elements (#iStdNbElements)
 *
 **/
/****************************************************************************/
t_sint32 CTuning::GetParamCount()
{
   t_sint32 count = 0;

   // Sanity check
   if( IsInstanceConstructed() == false) {
       TUNING_ASSERT(0);
       return 0;
   }

   for(int iqsetIndex=0; iqsetIndex<IQSET_COUNT; iqsetIndex++)
   {
       for(int userIndex=0; userIndex<IQSET_USER_COUNT; userIndex++)
       {
           count += iTuningData.iIqSet[iqsetIndex].user[userIndex].iParamCount;
       }
   }
   return count;
}

/****************************************************************************/
/**
 * \brief   report the number of page elements in the tuning storage object
 * \author  vollejm
 * \return  number of pages elements (#iStdNbElements)
 *
 **/
/****************************************************************************/
t_sint32 CTuning::GetParamCount(const e_iqset_id aIqSetId, const e_iquser_id aUserId)
{
   // Sanity check
   if( IsInstanceConstructed() == false) {
       TUNING_ASSERT(0);
       return 0;
   }

   // Check parameters
   if(   CTuningIQSet::IsValidIQSetId(aIqSetId) == false
      || CTuningUser::IsValidUserId(aUserId) == false)
   {
       // One param is invalid
       TUNING_ASSERT(0);
       return 0;
   }

   return iTuningData.iIqSet[aIqSetId].user[aUserId].iParamCount;
}

/****************************************************************************/
/**
 * \brief   report the stringyfied name of an IqSet Id
 * \author  vollejm
 * \return  string
 *
 **/
/****************************************************************************/
const char* CTuning::GetIqSetName(const e_iqset_id aIqSetId)
{
   return CTuningIQSet::FindIQSetNameFromId(aIqSetId);
}

/****************************************************************************/
/**
 * \brief   report the stringyfied name of User Id
 * \author  vollejm
 * \return  string
 *
 **/
/****************************************************************************/
const char* CTuning::GetUserName(const e_iquser_id aUserId)
{
   return CTuningUser::FindUserNameFromId(aUserId);
}

/****************************************************************************/
/**
 * \brief   report the File version retrieved from the xml file
 * \author  vollejm
 * \out     aVersion: pointer on struct used to report the version
 * \return  #t_tuning_error_code error code
 *
 **/
/****************************************************************************/
t_tuning_error_code CTuning::GetFileStructureVersion(t_tuning_struct_version* pVersion)
{
   TUNING_FUNC_IN0();
   // Sanity check
   if( IsInstanceConstructed() == false) {
       TUNING_ASSERT(0);
       TUNING_FUNC_OUTR(TUNING_NOT_CONSTRUCTED);
       return TUNING_NOT_CONSTRUCTED;
   }
   if(pVersion==NULL) {
       TUNING_ASSERT(0);
       TUNING_FUNC_OUTR(TUNING_INVALID_ARGUMENT);
       return TUNING_INVALID_ARGUMENT;
   }
   // Return version
   pVersion->structure = iTuningData.iStructVersion.structure;
   pVersion->entries   = iTuningData.iStructVersion.entries;
   TUNING_FUNC_OUTR(TUNING_OK);
   return TUNING_OK;
}

/****************************************************************************/
/**
 * \brief   report the File version retrieved from the xml file
 * \author  vollejm
 * \out     pVersion: pointer on struct used to report the version
 * \return  #t_tuning_error_code error code
 *
 **/
/****************************************************************************/
t_tuning_error_code CTuning::GetConfigurationVersion(t_tuning_config_version* pVersion)
{
   TUNING_FUNC_IN0();
   // Sanity check
   if( IsInstanceConstructed() == false) {
       TUNING_ASSERT(0);
       TUNING_FUNC_OUTR(TUNING_NOT_CONSTRUCTED);
       return TUNING_NOT_CONSTRUCTED;
   }
   if(pVersion==NULL) {
       TUNING_ASSERT(0);
       TUNING_FUNC_OUTR(TUNING_INVALID_ARGUMENT);
       return TUNING_INVALID_ARGUMENT;
   }

   // Return version
   pVersion->params = iTuningData.iConfigVersion.params;
   pVersion->values = iTuningData.iConfigVersion.values;
   TUNING_FUNC_OUTR(TUNING_OK);
   return TUNING_OK;
}

/****************************************************************************/
/**
 * \brief   report the File version retrieved from the xml file
 * \author  vollejm
 * \out     pVersion: pointer on struct used to report the version
 * \return  #t_tuning_error_code error code
 *
 **/
/****************************************************************************/
t_tuning_error_code CTuning::GetFirmwareVersion(t_tuning_fw_version* pVersion)
{
   TUNING_FUNC_IN0();
   // Sanity check
   if( IsInstanceConstructed() == false) {
       TUNING_ASSERT(0);
       TUNING_FUNC_OUTR(TUNING_NOT_CONSTRUCTED);
       return TUNING_NOT_CONSTRUCTED;
   }
   if(pVersion==NULL) {
       TUNING_ASSERT(0);
       TUNING_FUNC_OUTR(TUNING_INVALID_ARGUMENT);
       return TUNING_INVALID_ARGUMENT;
   }
   // Return version
   pVersion->major = iTuningData.iFirmwareVersion.major;
   pVersion->minor = iTuningData.iFirmwareVersion.minor;
   pVersion->patch = iTuningData.iFirmwareVersion.patch;
   TUNING_FUNC_OUTR(TUNING_OK);
   return TUNING_OK;
}

/****************************************************************************/
/**
 * \brief   report the File version retrieved from the xml file
 * \author  vollejm
 * \out     pVersion: pointer on struct used to report the version
 * \return  #t_tuning_error_code error code
 *
 **/
/****************************************************************************/
t_tuning_error_code CTuning::GetSw3aVersion(t_tuning_sw3a_version* pVersion)
{
   TUNING_FUNC_IN0();
   // Sanity check
   if( IsInstanceConstructed() == false) {
       TUNING_ASSERT(0);
       TUNING_FUNC_OUTR(TUNING_NOT_CONSTRUCTED);
       return TUNING_NOT_CONSTRUCTED;
   }
   if(pVersion==NULL) {
       TUNING_ASSERT(0);
       TUNING_FUNC_OUTR(TUNING_INVALID_ARGUMENT);
       return TUNING_INVALID_ARGUMENT;
   }
   // Return version
   pVersion->major = iTuningData.iSw3aVersion.major;
   pVersion->minor = iTuningData.iSw3aVersion.minor;
   pVersion->build = iTuningData.iSw3aVersion.build;
   TUNING_FUNC_OUTR(TUNING_OK);
   return TUNING_OK;
}

/****************************************************************************/
/**
 * \brief   Retrieve sensor ID
 * \author  vollejm
 * \out     aSensorId: pointer on sensor ID to be retrieved
 * \return  #TUNING_FILE_NOT_FOUND if parsing not done or failed, #TUNING_OK otherwise
 *
 **/
/****************************************************************************/
t_tuning_error_code CTuning::GetSensorId(t_tuning_sensor_id* pSensorId)
{
   TUNING_FUNC_IN0();
   // Sanity check
   if( IsInstanceConstructed() == false) {
       TUNING_ASSERT(0);
       TUNING_FUNC_OUTR(TUNING_NOT_CONSTRUCTED);
       return TUNING_NOT_CONSTRUCTED;
   }
   if(pSensorId==NULL) {
       TUNING_ASSERT(0);
       TUNING_FUNC_OUTR(TUNING_INVALID_ARGUMENT);
       return TUNING_INVALID_ARGUMENT;
   }
   // Return sensor ID
   *pSensorId = iTuningData.iSensorId;
   TUNING_FUNC_OUTR(TUNING_OK);
   return TUNING_OK;
}

/****************************************************************************/
/**
 * \brief   retrieve name of the sensor for which tuning applies
 * \author  vollejm
 * \return  pointer on string containing the sensor name, NULL is parsing failed.
 *
 **/
/****************************************************************************/
const char* CTuning::GetSensorName()
{
   TUNING_FUNC_IN0();
   // Sanity check
   if( IsInstanceConstructed() == false) {
       TUNING_ASSERT(0);
       TUNING_FUNC_OUTR("");
       return "";
   }
   // Return sensor name
   TUNING_FUNC_OUTR(iTuningData.iSensorId.name);
   return iTuningData.iSensorId.name;
}

/****************************************************************************/
/**
 * \brief   retrieve name of the operative mode for which tuning applies
 * \author  vollejm
 * \return  pointer on string containing the operative mode name, NULL if parsing failed.
 *
 **/
/****************************************************************************/
const char* CTuning::GetOperativeModeName()
{
   TUNING_FUNC_IN0();
   // Sanity check
   if( IsInstanceConstructed() == false) {
       TUNING_ASSERT(0);
       TUNING_FUNC_OUTR("");
       return "";
   }
   // Return operative mode
   TUNING_FUNC_OUTR(iTuningData.iOperatingMode.name);
   return (const char*)iTuningData.iOperatingMode.name;
}

/****************************************************************************/
/**
 * \brief   report the value of a parameter from a given Iqset and from its address
 * \author  vollejm
 * \in      aAddr: address of the parameter who's value is requested
 * \in      aIqSetId name of the iqset the param belongs to
 * \return  value of the parameter
 * \warning this functions asserts no parameter is found or if the parameter type is not int
 * \see     #GetFloatDriverParamA for requesting float parameters instead of int
 **/
/****************************************************************************/
t_sint32 CTuning::GetIntParam(e_iqset_id aIqSet, const e_iquser_id aUserId, t_uint32 aAddr)
{
   TUNING_FUNC_IN0();
   t_sint32 value = 0;
   t_tuning_error_code err = TUNING_OK;
   err = GetParam( aIqSet, aUserId, aAddr, &value);
   if(err!=TUNING_OK) {
       TUNING_INFO_MSG3("Failed to get param Addr=0x%X Err=%d (%s)", (unsigned int)aAddr, err, CTuning::ErrorCode2String(err));
       TUNING_ASSERT(0);
       TUNING_FUNC_OUTR(0);
       return 0;
   }
   TUNING_FUNC_OUTR(value);
   return value;
}

/****************************************************************************/
/**
 * \brief   report the value of a parameter from a given Iqset and from its address
 * \author  vollejm
 * \in      aAddr: address of the parameter who's value is requested
 * \in      aIqSetId name of the iqset the param belongs to
 * \return  value of the parameter
 * \warning this functions asserts no parameter is found or if the parameter type is not float
 * \see     #GetIntDriverParamA for requesting int parameters instead of float
 **/
/****************************************************************************/
float CTuning::GetFloatParam(e_iqset_id aIqSet, const e_iquser_id aUserId, t_uint32 aAddr)
{
   float value = 0.0;
   t_tuning_error_code err = TUNING_OK;
   err = GetParam( aIqSet, aUserId, aAddr, &value);
   if(err!=TUNING_OK) {
       TUNING_ERROR_MSG3("Failed to get param Addr=0x%X Err=%d (%s)", (unsigned int)aAddr, err, CTuning::ErrorCode2String(err));
       TUNING_ASSERT(0);
       return 0;
   }
   return value;
}

/****************************************************************************/
/**
 * \brief   find a given parameter from its address in the list of param of an Iqset
 * \author  vollejm
 * \in      aFirstParam: pointer on the first param of the list
 * \in      aParamCount: number of params in the list
 * \in      aAddr: address of the param to look for
 * \return  pointer on the param, null if no param found
 *
 **/
/****************************************************************************/
t_iqset_param* CTuning::FindIqParam(t_iqset_param* aParamList, const t_uint32 aAddr)
{
   t_iqset_param* pParam = aParamList;
   while(pParam != NULL) {
      if (pParam->addr == aAddr)
          return pParam;
      pParam = pParam->next;
   }
   return NULL;
}

/****************************************************************************/
/**
 * \brief   retrieve a specific driver parameter from its IQSET id and its "virtual" address
 * \author  vollejm
 * \in      aIqsetId: identifier of the IQSET to which this param belongs to
 * \out     out
 * \return  return
 *
 **/
/****************************************************************************/
t_tuning_error_code CTuning::GetParam( const e_iqset_id  aIqSetId,
                                       const e_iquser_id aUserId,
                                       const t_uint32    aAddr,
                                       t_sint32*         pValue)
{
   int rows=1, cols=1;
   return DoGetParam(aIqSetId, aUserId, aAddr, IQSET_VT_INT, TUNING_STRICT_TYPE_MATCH, pValue, &rows, &cols);
}

/****************************************************************************/
/**
 * \brief   retrieve a specific driver parameter from its IQSET id and its "virtual" address
 * \author  vollejm
 * \in      aIqsetId: identifier of the IQSET to which this param belongs to
 * \out     out
 * \return  return
 *
 **/
/****************************************************************************/
t_tuning_error_code CTuning::GetParam( const e_iqset_id  aIqSetId,
                                       const e_iquser_id aUserId,
                                       const t_uint32    aAddr,
                                       int*              pValue)
{
   int rows=1, cols=1;
   return DoGetParam(aIqSetId, aUserId, aAddr, IQSET_VT_INT, TUNING_STRICT_TYPE_MATCH, pValue, &rows, &cols);
}

/****************************************************************************/
/**
 * \brief   retrieve a specific driver parameter from its IQSET id and its "virtual" address
 * \author  vollejm
 * \in      aIqsetId: identifier of the IQSET to which this param belongs to
 * \out     out
 * \return  return
 *
 **/
/****************************************************************************/
t_tuning_error_code CTuning::GetParam( const e_iqset_id  aIqSetId,
                                       const e_iquser_id aUserId,
                                       const t_uint32    aAddr,
                                       float*            pValue)
{
   int rows=1, cols=1;
   return DoGetParam(aIqSetId, aUserId, aAddr, IQSET_VT_FLOAT, TUNING_STRICT_TYPE_MATCH, pValue, &rows, &cols);
}

/****************************************************************************/
/**
 * \brief   retrieve a specific driver parameter from its IQSET id and its "virtual" address
 * \author  vollejm
 * \in      aIqsetId: identifier of the IQSET to which this param belongs to
 * \out     out
 * \return  return
 *
 **/
/****************************************************************************/
t_tuning_error_code CTuning::GetMatrix( const e_iqset_id  aIqSetId,
                                        const e_iquser_id aUserId,
                                        const t_uint32    aAddr,
                                        t_sint32*         pValue,
                                        int*              pNumRows,
                                        int*              pNumCols)
{
   return DoGetParam(aIqSetId, aUserId, aAddr, IQSET_VT_INT, TUNING_STRICT_TYPE_MATCH, pValue, pNumRows, pNumCols);
}

/****************************************************************************/
/**
 * \brief   retrieve a specific driver parameter from its IQSET id and its "virtual" address
 * \author  vollejm
 * \in      aIqsetId: identifier of the IQSET to which this param belongs to
 * \out     out
 * \return  return
 *
 **/
/****************************************************************************/
t_tuning_error_code CTuning::GetMatrixAsInt( const e_iqset_id  aIqSetId,
                                             const e_iquser_id aUserId,
                                             const t_uint32    aAddr,
                                             t_sint32*         pValue,
                                             int*              pNumRows,
                                             int*              pNumCols)
{
   return DoGetParam(aIqSetId, aUserId, aAddr, IQSET_VT_INT, TUNING_CONVERT_IF_TYPE_MISMATCH, pValue, pNumRows, pNumCols);
}

/****************************************************************************/
/**
 * \brief   retrieve a specific driver parameter from its IQSET id and its "virtual" address
 * \author  vollejm
 * \in      aIqsetId: identifier of the IQSET to which this param belongs to
 * \out     out
 * \return  return
 *
 **/
/****************************************************************************/
t_tuning_error_code CTuning::GetMatrix( const e_iqset_id  aIqSetId,
                                        const e_iquser_id aUserId,
                                        const t_uint32    aAddr,
                                        float*            pValue,
                                        int*              pNumRows,
                                        int*              pNumCols)
{
   return DoGetParam(aIqSetId, aUserId, aAddr, IQSET_VT_FLOAT, TUNING_STRICT_TYPE_MATCH, pValue, pNumRows, pNumCols);
}

/****************************************************************************/
/**
 * \brief   retrieve a specific driver parameter from its IQSET id and its "virtual" address
 * \author  vollejm
 * \in      aIqsetId: identifier of the IQSET to which this param belongs to
 * \out     out
 * \return  return
 *
 **/
/****************************************************************************/
t_tuning_error_code CTuning::GetMatrixAsFloat( const e_iqset_id  aIqSetId,
                                               const e_iquser_id aUserId,
                                               const t_uint32    aAddr,
                                               float*            pValue,
                                               int*              pNumRows,
                                               int*              pNumCols)
{
   return DoGetParam(aIqSetId, aUserId, aAddr, IQSET_VT_FLOAT, TUNING_CONVERT_IF_TYPE_MISMATCH, pValue, pNumRows, pNumCols);
}

/****************************************************************************/
/**
 * \brief   retrieve a specific driver parameter from its IQSET id and its "virtual" address
 * \author  vollejm
 * \in      aIqsetId: identifier of the IQSET to which this param belongs to
 * \out     out
 * \return  return
 *
 **/
/****************************************************************************/
t_tuning_error_code CTuning::GetVector( const e_iqset_id  aIqSetId,
                                        const e_iquser_id aUserId,
                                        const t_uint32    aAddr,
                                        t_sint32*         pValue,
                                        int*              pNumElements)
{
   int rows=1;
   return DoGetParam(aIqSetId, aUserId, aAddr, IQSET_VT_INT, TUNING_STRICT_TYPE_MATCH, pValue, &rows, pNumElements);
}

/****************************************************************************/
/**
 * \brief   retrieve a specific driver parameter from its IQSET id and its "virtual" address
 * \author  vollejm
 * \in      aIqsetId: identifier of the IQSET to which this param belongs to
 * \out     out
 * \return  return
 *
 **/
/****************************************************************************/
t_tuning_error_code CTuning::GetVectorAsInt( const e_iqset_id  aIqSetId,
                                             const e_iquser_id aUserId,
                                             const t_uint32    aAddr,
                                             t_sint32*         pValue,
                                             int*              pNumElements)
{
   int rows=1;
   return DoGetParam(aIqSetId, aUserId, aAddr, IQSET_VT_INT, TUNING_CONVERT_IF_TYPE_MISMATCH, pValue, &rows, pNumElements);
}

/****************************************************************************/
/**
 * \brief   retrieve a specific driver parameter from its IQSET id and its "virtual" address
 * \author  vollejm
 * \in      aIqsetId: identifier of the IQSET to which this param belongs to
 * \out     out
 * \return  return
 *
 **/
/****************************************************************************/
t_tuning_error_code CTuning::GetVector( const e_iqset_id  aIqSetId,
                                        const e_iquser_id aUserId,
                                        const t_uint32    aAddr,
                                        float*            pValue,
                                        int*              pNumElements)
{
   int rows=1;
   return DoGetParam(aIqSetId, aUserId, aAddr, IQSET_VT_FLOAT, TUNING_STRICT_TYPE_MATCH, pValue, &rows, pNumElements);
}

/****************************************************************************/
/**
 * \brief   retrieve a specific driver parameter from its IQSET id and its "virtual" address
 * \author  vollejm
 * \in      aIqsetId: identifier of the IQSET to which this param belongs to
 * \out     out
 * \return  return
 *
 **/
/****************************************************************************/
t_tuning_error_code CTuning::GetVectorAsFloat( const e_iqset_id  aIqSetId,
                                               const e_iquser_id aUserId,
                                               const t_uint32    aAddr,
                                               float*            pValue,
                                               int*              pNumElements)
{
   int rows=1;
   return DoGetParam(aIqSetId, aUserId, aAddr, IQSET_VT_FLOAT, TUNING_CONVERT_IF_TYPE_MISMATCH, pValue, &rows, pNumElements);
}

/****************************************************************************/
/**
 * \brief   retrieve a specific driver parameter from its IQSET id and its "virtual" address
 * \author  vollejm
 * \in      aIqsetId: identifier of the IQSET to which this param belongs to
 * \out     out
 * \return  return
 *
 **/
/****************************************************************************/
t_tuning_error_code CTuning::DoGetParam( const e_iqset_id         aIqSetId,
                                         const e_iquser_id        aUserId,
                                         const t_uint32           aAddr,
                                         const e_iqset_value_type aExpectedType,
                                         const e_type_check       aTypeCheck,
                                         void*                    pValue,
                                         int*                     pNumRows,
                                         int*                     pNumCols)
{
   TUNING_FUNC_IN0();
   t_iqset* pIqSet = NULL;
   t_iqset_param* pParam = NULL;
   t_iqset_user* pUser = NULL;

   /* Sanity checks */
   if( IsInstanceConstructed() == false) {
       TUNING_ASSERT(0);
       TUNING_FUNC_OUTR(TUNING_NOT_CONSTRUCTED);
       return TUNING_NOT_CONSTRUCTED;
   }
   if( CTuningIQSet::IsValidIQSetId(aIqSetId) == false) {
       TUNING_ASSERT(0);
       TUNING_FUNC_OUTR(TUNING_INVALID_IQSET_ID);
       return TUNING_INVALID_IQSET_ID;
   }
   if( CTuningUser::IsValidUserId(aUserId) == false) {
       TUNING_ASSERT(0);
       TUNING_FUNC_OUTR(TUNING_INVALID_USER_ID);
       return TUNING_INVALID_USER_ID;
   }
   if(pValue==NULL) {
       TUNING_ASSERT(0);
       TUNING_FUNC_OUTR(TUNING_INVALID_ARGUMENT);
       return TUNING_INVALID_ARGUMENT;
   }
   if(aExpectedType!=IQSET_VT_INT && aExpectedType!=IQSET_VT_FLOAT)
   {
       TUNING_ASSERT(0);
       TUNING_FUNC_OUTR(TUNING_INVALID_ARGUMENT);
       return TUNING_INVALID_ARGUMENT;
   }

   /* Find the IQ Set entry */
   pIqSet = &iTuningData.iIqSet[aIqSetId];

   /* Find the parameter entry for this User */
   pUser = &pIqSet->user[aUserId];
   pParam = FindIqParam(pUser->pParamList, aAddr);
   if (pParam==NULL) {
       TUNING_ERROR_MSG1("Parameter 0x%lX not found\n", aAddr);
       TUNING_ASSERT(0);
       TUNING_FUNC_OUTR(TUNING_PARAM_NOT_FOUND);
       return TUNING_PARAM_NOT_FOUND;
   }

   /* Check size match */
   if(((pParam->rows)*(pParam->cols)) > ((*pNumRows)*(*pNumCols))) {
       TUNING_ERROR_MSG4("Destination too small: dest size = %dx%d whereas param size = %dx%d \n", *pNumRows, *pNumCols, pParam->rows, pParam->cols);
       TUNING_ASSERT(0);
       TUNING_FUNC_OUTR(TUNING_PARAM_SIZE_MISMATCH);
       return TUNING_PARAM_SIZE_MISMATCH;
   }

   /* Check if the data type match */
   if(aTypeCheck == TUNING_STRICT_TYPE_MATCH && pParam->type != aExpectedType) {
       TUNING_ASSERT(0);
       TUNING_FUNC_OUTR(TUNING_PARAM_TYPE_MISMATCH);
       return TUNING_PARAM_TYPE_MISMATCH;
   }

   // Check parameter sanity
   if(pParam->value == NULL) {
       TUNING_INFO_MSG1("Param 0x%lX: unexpected value=NULL\n", pParam->addr);
       TUNING_FUNC_OUTR(TUNING_INTERNAL_ERROR);
       return TUNING_INTERNAL_ERROR;
   }

   // Copy parameter (Note: the copy is optimized in terms of CPU usage)
   TUNING_INFO_MSG4("Param 0x%lX: value=%p, rows=%d, cols=%d\n", pParam->addr, pParam->value, pParam->rows, pParam->cols);
   if (aExpectedType ==  IQSET_VT_INT)
   {
       // Integer expected
       if(pParam->type == IQSET_VT_INT)
       {
          // Source value is integer: copy as is
          for(int i=0; i<(pParam->rows*pParam->cols); i++)
          {
             ((t_sint32*)pValue)[i] = pParam->value[i].i32;

             TUNING_PARAM_MSG8("[R] [%s][%s][Addr=0x%lX][Name=%s][%d][%d] [Val=%ld] (Val=0x%lX)\n",
                               CTuningIQSet::FindIQSetNameFromId(aIqSetId),
                               CTuningUser::FindUserNameFromId(aUserId),
                               pParam->addr, pParam->name,
                               FlatIndex2Row(i,pParam->cols),
                               FlatIndex2Col(i,pParam->cols),
                               ((t_sint32*)pValue)[i], ((t_sint32*)pValue)[i]);
          }
       }
       else
       {
          // Source value is a float: convert to integer
          for(int i=0; i<(pParam->rows*pParam->cols); i++)
          {
             ((t_sint32*)pValue)[i] = (t_sint32)pParam->value[i].f32;

             TUNING_PARAM_MSG8("[R] [%s][%s][Addr=0x%lX][Name=%s][%d][%d] [Val=%ld] (Val=0x%lX)\n",
                               CTuningIQSet::FindIQSetNameFromId(aIqSetId),
                               CTuningUser::FindUserNameFromId(aUserId),
                               pParam->addr, pParam->name,
                               FlatIndex2Row(i,pParam->cols),
                               FlatIndex2Col(i,pParam->cols),
                               ((t_sint32*)pValue)[i], ((t_sint32*)pValue)[i]);
          }
       }
   }
   else
   {
       // Floating point expected
       if(pParam->type == IQSET_VT_INT)
       {
           // Source value is integer: convert to float
           for(int i=0; i<(pParam->rows*pParam->cols); i++)
           {
              ((float*)pValue)[i] = (float)pParam->value[i].i32;

              TUNING_PARAM_MSG8("[R] [%s][%s][Addr=0x%lX][Name=%s][%d][%d] [Val=%f] (Val=0x%lX)\n",
                                CTuningIQSet::FindIQSetNameFromId(aIqSetId),
                                CTuningUser::FindUserNameFromId(aUserId),
                                pParam->addr, pParam->name,
                                FlatIndex2Row(i,pParam->cols),
                                FlatIndex2Col(i,pParam->cols),
                                ((float*)pValue)[i], ((t_sint32*)pValue)[i]);
           }
       }
       else
       {
           // Source value is float: copy as is
           for(int i=0; i<(pParam->rows*pParam->cols); i++)
           {
              ((float*)pValue)[i] = pParam->value[i].f32;

              TUNING_PARAM_MSG8("[R] [%s][%s][Addr=0x%lX][Name=%s][%d][%d] [Val=%f] (Val=0x%lX)\n",
                                CTuningIQSet::FindIQSetNameFromId(aIqSetId),
                                CTuningUser::FindUserNameFromId(aUserId),
                                pParam->addr, pParam->name,
                                FlatIndex2Row(i,pParam->cols),
                                FlatIndex2Col(i,pParam->cols),
                                ((float*)pValue)[i], ((t_sint32*)pValue)[i]);
           }
       }
   }

   // Update actual param dimensions
   *pNumRows = pParam->rows;
   *pNumCols = pParam->cols;
   TUNING_PARAM_MSG6("[R] [%s][%s][Addr=0x%lX] [Name=%s] [Size=%dx%d]\n",
                     CTuningIQSet::FindIQSetNameFromId(aIqSetId),
                     CTuningUser::FindUserNameFromId(aUserId),
                     pParam->addr, pParam->name, *pNumRows, *pNumCols);

   // Return value
   TUNING_FUNC_OUTR(TUNING_OK);
   return TUNING_OK;
}

/****************************************************************************/
/**
 * \brief   retrieve a specific driver parameter from its IQSET id and its "virtual" address
 * \author  vollejm
 * \in      aIqsetId: identifier of the IQSET to which this param belongs to
 * \out     out
 * \return  return
 *
 **/
/****************************************************************************/
t_tuning_error_code CTuning::GetParamList( const e_iqset_id  aIqSetId,
                                           const e_iquser_id aUserId,
                                           t_tuning_param*   pParamInfoList,
                                           t_sint32*         pNbElements)
{
   TUNING_FUNC_IN0();
   t_iqset* pIqSet = NULL;
   t_iqset_param* pParam = NULL;
   t_sint32 destIndex = 0; // Index of element in destination array

   /* Sanity checks */
   if( IsInstanceConstructed() == false) {
       TUNING_ASSERT(0);
       TUNING_FUNC_OUTR(TUNING_NOT_CONSTRUCTED);
       return TUNING_NOT_CONSTRUCTED;
   }
   if( CTuningIQSet::IsValidIQSetId(aIqSetId) == false) {
       TUNING_ASSERT(0);
       TUNING_FUNC_OUTR(TUNING_INVALID_IQSET_ID);
       return TUNING_INVALID_IQSET_ID;
   }
   if( CTuningUser::IsValidUserId(aUserId) == false) {
       TUNING_ASSERT(0);
       TUNING_FUNC_OUTR(TUNING_INVALID_USER_ID);
       return TUNING_INVALID_USER_ID;
   }
   if(pParamInfoList==NULL || pNbElements==NULL || *pNbElements<=0) {
       TUNING_ASSERT(0);
       TUNING_FUNC_OUTR(TUNING_INVALID_ARGUMENT);
       return TUNING_INVALID_ARGUMENT;
   }

   /* Find the IQ Set entry */
   pIqSet = &iTuningData.iIqSet[aIqSetId];

   /* Find the parameter entry for this User */
   pParam = pIqSet->user[aUserId].pParamList;

   /* Copy param to caller */
   TUNING_PARAM_MSG2("[R] [%s] [%s]\n", CTuningIQSet::FindIQSetNameFromId(aIqSetId), CTuningUser::FindUserNameFromId(aUserId));
   while( pParam != NULL )
   {
       t_uint32      addr = pParam->addr;

       for(int i=0; i<(pParam->rows*pParam->cols); i++)
       {
           /* Check if there is enough space in destination array */
           if( destIndex >= *pNbElements) {
               TUNING_ASSERT(0);
               TUNING_FUNC_OUTR(TUNING_ARRAY_TOO_SMALL);
               return TUNING_ARRAY_TOO_SMALL;
           }

           // Reconstruct one addr per matrix element and copy to destination array
           pParamInfoList[destIndex].addr   = addr + i*sizeof(t_uint32);
           pParamInfoList[destIndex].value  = pParam->value[i].r32;
           pParamInfoList[destIndex].f32dbg = pParam->value[i].f32;
           TUNING_PARAM_MSG5("[R] [%s][%s][Addr=0x%lX] [Name=%s] [Val=0x%lX]\n",
                             CTuningIQSet::FindIQSetNameFromId(aIqSetId),
                             CTuningUser::FindUserNameFromId(aUserId),
                             pParamInfoList[destIndex].addr, pParam->name,
                             pParamInfoList[destIndex].value);
           destIndex++;
       }
       // Next param
       pParam = pParam->next;
   }
   *pNbElements = destIndex;
   TUNING_FUNC_OUTR(TUNING_OK);
   return TUNING_OK;
}

/****************************************************************************/
/**
 * \brief   retrieve a specific driver parameter from its IQSET id and its "virtual" address
 * \author  vollejm
 * \in      aIqsetId: identifier of the IQSET to which this param belongs to
 * \out     out
 * \return  return
 *
 **/
/****************************************************************************/
t_tuning_error_code CTuning::GetParamInfoList( const e_iqset_id     aIqSetId,
                                               const e_iquser_id    aUserId,
                                               t_tuning_param_info* pParamInfoList,
                                               t_sint32*            pNbElements)
{
   TUNING_FUNC_IN0();
   t_iqset* pIqSet = NULL;
   t_iqset_param* pParam = NULL;
   t_sint32 paramIndex = 0;

   /* Sanity checks */
   if( IsInstanceConstructed() == false) {
       TUNING_ASSERT(0);
       TUNING_FUNC_OUTR(TUNING_NOT_CONSTRUCTED);
       return TUNING_NOT_CONSTRUCTED;
   }
   if( CTuningIQSet::IsValidIQSetId(aIqSetId) == false) {
       TUNING_ASSERT(0);
       TUNING_FUNC_OUTR(TUNING_INVALID_IQSET_ID);
       return TUNING_INVALID_IQSET_ID;
   }
   if( CTuningUser::IsValidUserId(aUserId) == false) {
       TUNING_ASSERT(0);
       TUNING_FUNC_OUTR(TUNING_INVALID_USER_ID);
       return TUNING_INVALID_USER_ID;
   }
   if(pParamInfoList==NULL || pNbElements==NULL || *pNbElements<=0) {
       TUNING_ASSERT(0);
       TUNING_FUNC_OUTR(TUNING_INVALID_ARGUMENT);
       return TUNING_INVALID_ARGUMENT;
   }

   /* Find the IQ Set entry */
   pIqSet = &iTuningData.iIqSet[aIqSetId];

   /* Find the parameters entry for this User */
   pParam = pIqSet->user[aUserId].pParamList;

   /* Copy param info to caller */
   while( pParam != NULL )
   {
       TUNING_PARAM_MSG4("[R] [%s][%s][Addr=0x%lX] [Name=%s]\n",
                         CTuningIQSet::FindIQSetNameFromId(aIqSetId),
                         CTuningUser::FindUserNameFromId(aUserId),
                         pParam->addr, pParam->name);
       pParamInfoList[paramIndex].addr = pParam->addr;
       pParamInfoList[paramIndex].size = pParam->rows*pParam->cols;
       pParam = pParam->next;
       paramIndex++;
   }
   *pNbElements = paramIndex;
   TUNING_FUNC_OUTR(TUNING_OK);
   return TUNING_OK;
}

/****************************************************************************/
/**
 * \brief   get a list of page element from the Id of the IQSET they belong to
 * \author  vollejm
 * \in      aIqSetId: Id of the set
 * \out     aPeList Page element list result. enough memory should be allocated. size of allocation is checked using pNbElements value.
 * \return  number of PE found for the requested iqset
 * \warning this function asserts in case of failure. use #GetIqSet for error checking
 *
 **/
/****************************************************************************/
t_sint32 CTuning::GetParamListA(const e_iqset_id aIqSetId, const e_iquser_id aUserId, t_tuning_param* pParamList, int aListSize)
{
   TUNING_FUNC_IN0();
   t_tuning_error_code err = TUNING_OK;
   t_sint32 paramCount = aListSize;
   err = GetParamList( aIqSetId, aUserId, pParamList, &paramCount);
   if(err!=TUNING_OK) {
       TUNING_ERROR_MSG2("IQSET [%s] [%s]", CTuningIQSet::FindIQSetNameFromId(aIqSetId), CTuningUser::FindUserNameFromId(aUserId));
       TUNING_ASSERT(0);
       TUNING_FUNC_OUTR(0);
       return 0;
   }
   TUNING_FUNC_OUTR(paramCount);
   return paramCount;
}

/****************************************************************************/
/**
 * \brief   report the File version retrieved from the xml file
 * \author  vollejm
 * \out     aVersion: pointer on struct used to report the version
 * \return  #t_tuning_error_code error code
 *
 **/
/****************************************************************************/
t_tuning_error_code CTuning::SetConfigurationVersion(const t_tuning_config_version* pVersion)
{
   TUNING_FUNC_IN0();
   // Check parameter
   if( pVersion == NULL)
   {
       TUNING_ASSERT(0);
       TUNING_FUNC_OUTR(TUNING_INVALID_ARGUMENT);
       return TUNING_INVALID_ARGUMENT;
   }

   // Check whether instance has been constructed
   if( IsInstanceConstructed() == false)
   {
       TUNING_ASSERT(0);
       TUNING_FUNC_OUTR(TUNING_NOT_CONSTRUCTED);
       return TUNING_NOT_CONSTRUCTED;
   }

   // Store version
   iTuningData.iConfigVersion = *pVersion;
   TUNING_FUNC_OUTR(TUNING_OK);
   return TUNING_OK;
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
t_tuning_error_code CTuning::SetOperativeModeName(const char* aName)
{
   TUNING_FUNC_IN0();
   // Check parameter
   if( aName == NULL || strnlen(aName,OPERATIVE_MODE_MAX_SIZE) == 0)
   {
       // Invalid string
       TUNING_ASSERT(0);
       TUNING_FUNC_OUTR(TUNING_INVALID_ARGUMENT);
       return TUNING_INVALID_ARGUMENT;
   }

   // Check whether instance has been constructed
   if( IsInstanceConstructed() == false)
   {
      // Instance not constructed
       TUNING_ASSERT(0);
       TUNING_FUNC_OUTR(TUNING_NOT_CONSTRUCTED);
       return TUNING_NOT_CONSTRUCTED;
   }

   // Write Operative Mode
   int res = snprintf(iTuningData.iOperatingMode.name, sizeof(iTuningData.iOperatingMode.name), aName);
   if(res >= (int)sizeof(iTuningData.iOperatingMode.name))
   {
       // String does not fit in buffer
       TUNING_ASSERT(0);
       TUNING_FUNC_OUTR(TUNING_INVALID_ARGUMENT);
       return TUNING_INVALID_ARGUMENT;
   }
   else if( res < 0 )
   {
       // Unknown error
       TUNING_ASSERT(0);
       TUNING_FUNC_OUTR(TUNING_INTERNAL_ERROR);
       return TUNING_INTERNAL_ERROR;
   }

   TUNING_FUNC_OUTR(TUNING_OK);
   return TUNING_OK;
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
t_tuning_error_code CTuning::SetFirmwareVersion(const t_tuning_fw_version* pVersion)
{
   TUNING_FUNC_IN0();
   // Check parameter
   if( pVersion == NULL)
   {
       // Bad parameter
       TUNING_ASSERT(0);
       TUNING_FUNC_OUTR(TUNING_INVALID_ARGUMENT);
       return TUNING_INVALID_ARGUMENT;
   }

   // Check whether instance has been constructed
   if( IsInstanceConstructed() == false)
   {
       // Instance not constructed
       TUNING_ASSERT(0);
       TUNING_FUNC_OUTR(TUNING_NOT_CONSTRUCTED);
       return TUNING_NOT_CONSTRUCTED;
   }

   // Store FW version
   iTuningData.iFirmwareVersion = *pVersion;
   TUNING_INFO_MSG3("Firmware version: %d.%d.%d\n", pVersion->major,pVersion->minor,pVersion->patch);
   TUNING_FUNC_OUTR(TUNING_OK);
   return TUNING_OK;
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
t_tuning_error_code CTuning::SetSensorId(const t_tuning_sensor_id* pSensorId)
{
   TUNING_FUNC_IN0();
   // Check whether instance has been constructed
   if( IsInstanceConstructed() == false)
   {
      // Instance not constructed
      TUNING_ASSERT(0);
      TUNING_FUNC_OUTR(TUNING_NOT_CONSTRUCTED);
      return TUNING_NOT_CONSTRUCTED;
   }

   // Store Sensor ID
   iTuningData.iSensorId = *pSensorId;
   TUNING_FUNC_OUTR(TUNING_OK);
   return TUNING_OK;
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
t_tuning_error_code CTuning::SetSensorName(const char* aSensorName)
{
   TUNING_FUNC_IN0();
   // Check whether instance has been constructed
   if( IsInstanceConstructed() == false)
   {
      // Instance not constructed
      TUNING_ASSERT(0);
      TUNING_FUNC_OUTR(TUNING_NOT_CONSTRUCTED);
      return TUNING_NOT_CONSTRUCTED;
   }

   // Check parameter
   if( aSensorName == 0 || strnlen(aSensorName,SENSOR_NAME_MAX_SIZE) == 0)
   {
       // Invalid string
       TUNING_ASSERT(0);
       TUNING_FUNC_OUTR(TUNING_INVALID_ARGUMENT);
       return TUNING_INVALID_ARGUMENT;
   }

   // Store sensor name
   int res = snprintf(iTuningData.iSensorId.name, sizeof(iTuningData.iSensorId.name), aSensorName);
   if ( res >= (int)sizeof(iTuningData.iSensorId.name))
   {
       // String does not fit in buffer
       TUNING_ASSERT(0);
       TUNING_FUNC_OUTR(TUNING_INVALID_ARGUMENT);
       return TUNING_INVALID_ARGUMENT;
   }
   else if( res < 0 )
   {
       // Unknown error
       TUNING_ASSERT(0);
       TUNING_FUNC_OUTR(TUNING_INTERNAL_ERROR);
       return TUNING_INTERNAL_ERROR;
   }

   TUNING_FUNC_OUTR(TUNING_OK);
   return TUNING_OK;
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
t_tuning_error_code CTuning::AddParam( const e_iqset_id  aIqSetId,
                                       const e_iquser_id aUserId,
                                       const t_uint32    aParamAddr,
                                       const char*       pParamName,
                                       const t_sint32    aParamValue)
{
   return DoAddParam(aIqSetId, aUserId, aParamAddr, pParamName, &aParamValue, IQSET_VT_INT, 1, 1, 1);
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
t_tuning_error_code CTuning::AddParam( const e_iqset_id  aIqSetId,
                                       const e_iquser_id aUserId,
                                       const t_uint32    aParamAddr,
                                       const char*       pParamName,
                                       const int         aParamValue)
{
   return DoAddParam(aIqSetId, aUserId, aParamAddr, pParamName, &aParamValue, IQSET_VT_INT, 1, 1, 1);
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
t_tuning_error_code CTuning::AddParam( const e_iqset_id  aIqSetId,
                                       const e_iquser_id aUserId,
                                       const t_uint32    aParamAddr,
                                       const char*       pParamName,
                                       const t_uint32    aParamValue)
{
   return AddParam(aIqSetId, aUserId, aParamAddr, pParamName, (t_sint32)aParamValue);
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
t_tuning_error_code CTuning::AddParam( const e_iqset_id  aIqSetId,
                                       const e_iquser_id aUserId,
                                       const t_uint32    aParamAddr,
                                       const char*       pParamName,
                                       const uint32_t    aParamValue)
{
   return AddParam(aIqSetId, aUserId, aParamAddr, pParamName, (t_sint32)aParamValue);
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
t_tuning_error_code CTuning::AddParam( const e_iqset_id  aIqSetId,
                                       const e_iquser_id aUserId,
                                       const t_uint32    aParamAddr,
                                       const char*       pParamName,
                                       const float       aParamValue)
{
   return DoAddParam(aIqSetId, aUserId, aParamAddr, pParamName, &aParamValue, IQSET_VT_FLOAT, 1, 1, 1);
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
t_tuning_error_code CTuning::AddMatrix( const e_iqset_id  aIqSetId,
                                        const e_iquser_id aUserId,
                                        const t_uint32    aParamAddr,
                                        const char*       pParamName,
                                        const t_sint32*   pMatrix,
                                        const int         aNumRows,
                                        const int         aNumCols)
{
   return DoAddParam(aIqSetId, aUserId, aParamAddr, pParamName, pMatrix, IQSET_VT_INT, aNumRows, aNumCols, aNumCols);
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
t_tuning_error_code CTuning::AddMatrix( const e_iqset_id  aIqSetId,
                                        const e_iquser_id aUserId,
                                        const t_uint32    aParamAddr,
                                        const char*       pParamName,
                                        const t_uint32*   pMatrix,
                                        const int         aNumRows,
                                        const int         aNumCols)
{
   return DoAddParam(aIqSetId, aUserId, aParamAddr, pParamName, pMatrix, IQSET_VT_INT, aNumRows, aNumCols, aNumCols);
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
t_tuning_error_code CTuning::AddMatrix( const e_iqset_id  aIqSetId,
                                        const e_iquser_id aUserId,
                                        const t_uint32    aParamAddr,
                                        const char*       pParamName,
                                        const uint32_t*   pMatrix,
                                        const int         aNumRows,
                                        const int         aNumCols)
{
   return DoAddParam(aIqSetId, aUserId, aParamAddr, pParamName, pMatrix, IQSET_VT_INT, aNumRows, aNumCols, aNumCols);
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
t_tuning_error_code CTuning::AddMatrix( const e_iqset_id  aIqSetId,
                                        const e_iquser_id aUserId,
                                        const t_uint32    aParamAddr,
                                        const char*       pParamName,
                                        const int*        pMatrix,
                                        const int         aNumRows,
                                        const int         aNumCols)
{
   return DoAddParam(aIqSetId, aUserId, aParamAddr, pParamName, pMatrix, IQSET_VT_INT, aNumRows, aNumCols, aNumCols);
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
t_tuning_error_code CTuning::AddMatrix( const e_iqset_id  aIqSetId,
                                        const e_iquser_id aUserId,
                                        const t_uint32    aParamAddr,
                                        const char*       pParamName,
                                        const float*      pMatrix,
                                        const int         aNumRows,
                                        const int         aNumCols)
{
   return DoAddParam(aIqSetId, aUserId, aParamAddr, pParamName, pMatrix, IQSET_VT_FLOAT, aNumRows, aNumCols, aNumCols);
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
t_tuning_error_code CTuning::AddMatrix( const e_iqset_id  aIqSetId,
                                        const e_iquser_id aUserId,
                                        const t_uint32    aParamAddr,
                                        const char*       pParamName,
                                        const t_sint32*   pMatrix,
                                        const int         aNumRows,
                                        const int         aNumCols,
                                        const int         aContainerNumCols)
{
   return DoAddParam(aIqSetId, aUserId, aParamAddr, pParamName, pMatrix, IQSET_VT_INT, aNumRows, aNumCols, aContainerNumCols);
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
t_tuning_error_code CTuning::AddMatrix( const e_iqset_id  aIqSetId,
                                        const e_iquser_id aUserId,
                                        const t_uint32    aParamAddr,
                                        const char*       pParamName,
                                        const t_uint32*   pMatrix,
                                        const int         aNumRows,
                                        const int         aNumCols,
                                        const int         aContainerNumCols)
{
   return DoAddParam(aIqSetId, aUserId, aParamAddr, pParamName, pMatrix, IQSET_VT_INT, aNumRows, aNumCols, aContainerNumCols);
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
t_tuning_error_code CTuning::AddMatrix( const e_iqset_id  aIqSetId,
                                        const e_iquser_id aUserId,
                                        const t_uint32    aParamAddr,
                                        const char*       pParamName,
                                        const uint32_t*   pMatrix,
                                        const int         aNumRows,
                                        const int         aNumCols,
                                        const int         aContainerNumCols)
{
   return DoAddParam(aIqSetId, aUserId, aParamAddr, pParamName, pMatrix, IQSET_VT_INT, aNumRows, aNumCols, aContainerNumCols);
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
t_tuning_error_code CTuning::AddMatrix( const e_iqset_id  aIqSetId,
                                        const e_iquser_id aUserId,
                                        const t_uint32    aParamAddr,
                                        const char*       pParamName,
                                        const int*        pMatrix,
                                        const int         aNumRows,
                                        const int         aNumCols,
                                        const int         aContainerNumCols)
{
   return DoAddParam(aIqSetId, aUserId, aParamAddr, pParamName, pMatrix, IQSET_VT_INT, aNumRows, aNumCols, aContainerNumCols);
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
t_tuning_error_code CTuning::AddMatrix( const e_iqset_id  aIqSetId,
                                        const e_iquser_id aUserId,
                                        const t_uint32    aParamAddr,
                                        const char*       pParamName,
                                        const float*      pMatrix,
                                        const int         aNumRows,
                                        const int         aNumCols,
                                        const int         aContainerNumCols)
{
   return DoAddParam(aIqSetId, aUserId, aParamAddr, pParamName, pMatrix, IQSET_VT_FLOAT, aNumRows, aNumCols, aContainerNumCols);
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
t_tuning_error_code CTuning::AddVector( const e_iqset_id  aIqSetId,
                                        const e_iquser_id aUserId,
                                        const t_uint32    aParamAddr,
                                        const char*       pParamName,
                                        const t_sint32*   pVector,
                                        const int         aNumCols)
{
   return DoAddParam(aIqSetId, aUserId, aParamAddr, pParamName, pVector, IQSET_VT_INT, 1, aNumCols, aNumCols);
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
t_tuning_error_code CTuning::AddVector( const e_iqset_id  aIqSetId,
                                        const e_iquser_id aUserId,
                                        const t_uint32    aParamAddr,
                                        const char*       pParamName,
                                        const t_uint32*   pVector,
                                        const int         aNumCols)
{
   return DoAddParam(aIqSetId, aUserId, aParamAddr, pParamName, pVector, IQSET_VT_INT, 1, aNumCols, aNumCols);
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
t_tuning_error_code CTuning::AddVector( const e_iqset_id  aIqSetId,
                                        const e_iquser_id aUserId,
                                        const t_uint32    aParamAddr,
                                        const char*       pParamName,
                                        const uint32_t*   pVector,
                                        const int         aNumCols)
{
   return DoAddParam(aIqSetId, aUserId, aParamAddr, pParamName, pVector, IQSET_VT_INT, 1, aNumCols, aNumCols);
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
t_tuning_error_code CTuning::AddVector( const e_iqset_id  aIqSetId,
                                        const e_iquser_id aUserId,
                                        const t_uint32    aParamAddr,
                                        const char*       pParamName,
                                        const int*        pVector,
                                        const int         aNumCols)
{
   return DoAddParam(aIqSetId, aUserId, aParamAddr, pParamName, pVector, IQSET_VT_INT, 1, aNumCols, aNumCols);
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
t_tuning_error_code CTuning::AddVector( const e_iqset_id  aIqSetId,
                                        const e_iquser_id aUserId,
                                        const t_uint32    aParamAddr,
                                        const char*       pParamName,
                                        const float*      pVector,
                                        const int         aNumCols)
{
   return DoAddParam(aIqSetId, aUserId, aParamAddr, pParamName, pVector, IQSET_VT_FLOAT, 1, aNumCols, aNumCols);
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
t_tuning_error_code CTuning::DoAddParam( const e_iqset_id          aIqSetId,
                                         const e_iquser_id         aUserId,
                                         const t_uint32            aAddr,
                                         const char*               aName,
                                         const void*               pValue,
                                         const e_iqset_value_type  aType,
                                         const int                 aNumRows,
                                         const int                 aNumCols,
                                         const int                 aContainerNumCols)
{
   TUNING_FUNC_IN0();
   t_iqset* pIqSet = NULL;
   t_iqset_user* pUser = NULL;
   t_iqset_param* pParam = NULL;

   // Check whether instance has been constructed
   if( IsInstanceConstructed() == false)
   {
      // Instance not constructed
      TUNING_ASSERT(0);
      TUNING_FUNC_OUTR(TUNING_NOT_CONSTRUCTED);
      return TUNING_NOT_CONSTRUCTED;
   }

   // Check IQSet Id
   if( CTuningIQSet::IsValidIQSetId(aIqSetId) == false )
   {
      // Bad IQSet ID
      TUNING_ERROR_MSG1("Invalid IqSet %d\n", aIqSetId);
      TUNING_ASSERT(0);
      TUNING_FUNC_OUTR(TUNING_INVALID_IQSET_ID);
      return TUNING_INVALID_IQSET_ID;
   }

   // Check User Id
   if( CTuningUser::IsValidUserId(aUserId) == false )
   {
       // Bad IQSet ID
       TUNING_ERROR_MSG1("Invalid User %d\n", aUserId);
       TUNING_ASSERT(0);
       TUNING_FUNC_OUTR(TUNING_INVALID_USER_ID);
       return TUNING_INVALID_USER_ID;
   }

   // Check arguments
   if( aName == NULL || pValue == NULL || aType == IQSET_VT_UNKNOWN || aContainerNumCols<aNumCols)
   {
       // Bad name
       TUNING_ASSERT(0);
       TUNING_FUNC_OUTR(TUNING_INVALID_ARGUMENT);
       return TUNING_INVALID_ARGUMENT;
   }

   // Find IQSET entry
   pIqSet = &iTuningData.iIqSet[aIqSetId];

   // Select User list
   pUser = &pIqSet->user[aUserId];

   // Check whether Parameter is already present
   pParam = FindIqParam(pUser->pParamList, aAddr);
   if(pParam != NULL)
   {
       // Param already exist
       TUNING_ERROR_MSG3("[%s][%s] Parameter 0x%lX already exist\n", CTuningIQSet::FindIQSetNameFromId(aIqSetId), CTuningUser::FindUserNameFromId(aUserId), aAddr);
       TUNING_ERROR_MSG2("Parameter 0x%lX = %s\n", aAddr, aName);
       TUNING_ASSERT(0);
       TUNING_FUNC_OUTR(TUNING_DUPLICATE_PARAM);
       return TUNING_DUPLICATE_PARAM;
   }

   // Check if there is a free param in the pool
   if(iTuningData.iParamsPool.iUsed >= iTuningData.iParamsPool.iTotal)
   {
       // No more space in Params array
       TUNING_ASSERT(0);
       TUNING_FUNC_OUTR(TUNING_PARAMS_ARRAY_FULL);
       return TUNING_PARAMS_ARRAY_FULL;
   }

   // Get a free parameter from the pool
   pParam = &iTuningData.iParamsPool.pParam[iTuningData.iParamsPool.iUsed];
   iTuningData.iParamsPool.iUsed++;

   // Link parameter to the current user list
   if(pUser->pParamList==NULL) {
      // No parameters yet
      pUser->pParamList = pParam;
   }
   else {
      pUser->pLastParam->next = pParam;
   }
   pUser->pLastParam = pParam;

   // Init this param
   pParam->addr = aAddr;
   pParam->next = NULL;

   // Check if enough values entries are available in the pool
   if( iTuningData.iValuesPool.iUsed + (aNumRows*aNumCols) > iTuningData.iValuesPool.iTotal )
   {
       TUNING_ERROR_MSG2("Too many value entries (required:%ld, max:%ld)\n", iTuningData.iValuesPool.iUsed+(aNumRows*aNumCols), iTuningData.iValuesPool.iTotal);
       TUNING_ASSERT(0);
       TUNING_FUNC_OUTR(TUNING_TOO_MANY_ENTRIES);
       return TUNING_TOO_MANY_ENTRIES;
   }

   // Attach values slot to parameter
   pParam->value = &iTuningData.iValuesPool.pValue[iTuningData.iValuesPool.iUsed];
   pParam->rows = aNumRows;
   pParam->cols = aNumCols;
   pParam->type = IQSET_VT_UNKNOWN;
   iTuningData.iValuesPool.iUsed += aNumRows*aNumCols;
   TUNING_INFO_MSG4("Param 0x%lX: value=%p, numRows=%d, numCols=%d\n", pParam->addr, pParam->value, pParam->rows, pParam->cols);

   TUNING_PARAM_MSG2("[W] [%s] [%s]\n", CTuningIQSet::FindIQSetNameFromId(aIqSetId), CTuningUser::FindUserNameFromId(aUserId));
#if defined(TUNING_PARAM_NAMES_ENABLED)
   snprintf(pParam->name, sizeof(pParam->name), "%s", aName);
#endif

   // Copy parameter (Note: the copy is optimized in terms of CPU usage)
   if( aType == IQSET_VT_INT )
   {
      int srcRowStart  = 0;
      int destRowStart = 0;

      for(int row=0; row<aNumRows; row++)
      {
         for(int col=0; col<aNumCols; col++)
         {
            int src  = srcRowStart + col;
            int dest = destRowStart + col;
            pParam->value[dest].i32 = ((t_sint32*)pValue)[src];

            TUNING_PARAM_MSG8("[W] [%s][%s][Addr=0x%lX][Name=%s][%d][%d] [Val=%ld] (Val=0x%lX)\n",
                              CTuningIQSet::FindIQSetNameFromId(aIqSetId),
                              CTuningUser::FindUserNameFromId(aUserId),
                              pParam->addr, pParam->name,
                              row, col,
                              pParam->value[dest].i32,
                              pParam->value[dest].i32);
         }
         srcRowStart  += aContainerNumCols;
         destRowStart += aNumCols;
      }
   }
   else
   {
      int srcRowStart  = 0;
      int destRowStart = 0;

      for(int row=0; row<aNumRows; row++)
      {
         for(int col=0; col<aNumCols; col++)
         {
            int src  = srcRowStart + col;
            int dest = destRowStart + col;
            pParam->value[dest].f32 = ((float*)pValue)[src];

            TUNING_PARAM_MSG8("[W] [%s][%s][Addr=0x%lX][Name=%s][%d][%d] [Val=%f] (Val=0x%lX)\n",
                              CTuningIQSet::FindIQSetNameFromId(aIqSetId),
                              CTuningUser::FindUserNameFromId(aUserId),
                              pParam->addr, pParam->name,
                              row, col,
                              pParam->value[dest].f32,
                              pParam->value[dest].r32);
         }
         srcRowStart  += aContainerNumCols;
         destRowStart += aNumCols;
      }
   }

   // Paramater added
   pParam->type = aType;
   pUser->iParamCount++;

   // Param added
   TUNING_FUNC_OUTR(TUNING_OK);
   return TUNING_OK;
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
t_tuning_error_code CTuning::AddComment(const char* aComment)
{
   TUNING_FUNC_IN0();
   if( aComment == NULL)
   {
      TUNING_ASSERT(0);
      TUNING_FUNC_OUTR(TUNING_INVALID_ARGUMENT);
      return TUNING_INVALID_ARGUMENT;
   }
   if(iTuningData.iComments.count>=COMMENTS_MAX_COUNT)
   {
      TUNING_ASSERT(0);
      TUNING_FUNC_OUTR(TUNING_COMMENTS_ARRAY_FULL);
      return TUNING_COMMENTS_ARRAY_FULL;
   }
   char* pComment = new char[COMMENTS_MAX_LENGTH];
   if(pComment==NULL)
   {
      TUNING_ASSERT(0);
      TUNING_FUNC_OUTR(TUNING_MALLOC_ERROR);
      return TUNING_MALLOC_ERROR;
   }
   int res = snprintf(pComment, COMMENTS_MAX_LENGTH-1, "%s", aComment);
   if(res<0)
   {
      TUNING_ERROR_MSG1("Failed to string copy comment '%s'\n", aComment);
      TUNING_FUNC_OUTR(TUNING_INTERNAL_ERROR);
      return TUNING_INTERNAL_ERROR;
   }
   iTuningData.iComments.pComment[iTuningData.iComments.count] = pComment;
   TUNING_INFO_MSG1("Added comment: %s\n", iTuningData.iComments.pComment[iTuningData.iComments.count]);
   iTuningData.iComments.count++;
   TUNING_INFO_MSG1("Number of comments: %d\n", iTuningData.iComments.count);
   TUNING_FUNC_OUTR(TUNING_OK);
   return TUNING_OK;
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
t_tuning_error_code CTuning::DumpTuningConfiguration(const char* aFileName)
{
   TUNING_FUNC_IN0();
   t_tuning_writer_error_code writerErr = TUNING_WRITER_OK;
   t_tuning_error_code ret = TUNING_OK;
   CTuningWriter* pTuningWriter = NULL;

   // Check whether instance has been constructed
   if( IsInstanceConstructed() == false)
   {
      TUNING_FUNC_OUTR(TUNING_NOT_CONSTRUCTED);
      return TUNING_NOT_CONSTRUCTED;
   }

   // Check parameter
   if(aFileName==NULL)
   {
      TUNING_ASSERT(0);
      TUNING_FUNC_OUTR(TUNING_INVALID_ARGUMENT);
      return TUNING_INVALID_ARGUMENT;
   }

   // Instantiate a Writer
   pTuningWriter = new CTuningWriter;
   if(pTuningWriter==NULL)
   {
      TUNING_ASSERT(0);
      TUNING_FUNC_OUTR(TUNING_MALLOC_ERROR);
      return TUNING_MALLOC_ERROR;
   }

   // Populate file
   TUNING_INFO_MSG1("Dumping tuning configuration to file: %s\n", aFileName);
   writerErr = pTuningWriter->DumpToFile(aFileName, &iTuningData);
   if( writerErr != TUNING_WRITER_OK) {
      // Translate into appropriate error code
      TUNING_ASSERT(0);
      ret = ConvertErrorCode(writerErr);
   }
   else {
      // Dump done
      TUNING_INFO_MSG0("Dumped tuning configuration to file\n");
      ret = TUNING_OK;
   }

   // Deinstantiate the Writer
   delete pTuningWriter;
   pTuningWriter = NULL;
   TUNING_FUNC_OUTR(ret);
   return ret;
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
t_tuning_error_code CTuning::DumpTuningConfiguration(void* aBuffer, const int aBufferSize, int* pFilledBufferSize)
{
   TUNING_FUNC_IN0();
   t_tuning_writer_error_code writerErr = TUNING_WRITER_OK;
   t_tuning_error_code ret = TUNING_OK;
   CTuningWriter* pTuningWriter = NULL;

   // Check whether instance has been constructed
   if( IsInstanceConstructed() == false)
   {
      // Instance not constructed
      TUNING_ASSERT(0);
      TUNING_FUNC_OUTR(TUNING_NOT_CONSTRUCTED);
      return TUNING_NOT_CONSTRUCTED;
   }

   // Check parameter
   if(aBuffer==NULL || aBufferSize<=0 || pFilledBufferSize==NULL)
   {
      TUNING_ASSERT(0);
      TUNING_FUNC_OUTR(TUNING_INVALID_ARGUMENT);
      return TUNING_INVALID_ARGUMENT;
   }

   // Instantiate a Writer
   pTuningWriter = new CTuningWriter;
   if(pTuningWriter==NULL)
   {
       TUNING_ASSERT(0);
       TUNING_FUNC_OUTR(TUNING_MALLOC_ERROR);
       return TUNING_MALLOC_ERROR;
   }

   // Populate buffer
   TUNING_INFO_MSG2("Dumping tuning configuration to buffer: %p, size = %d\n", aBuffer, *pFilledBufferSize);
   writerErr = pTuningWriter->DumpToBuffer( aBuffer, aBufferSize, pFilledBufferSize, &iTuningData);
   if( writerErr != TUNING_WRITER_OK) {
      // Translate into appropriate error code
      TUNING_ERROR_MSG2("Failed to dump tuning configuration: err=%d (%s)\n", writerErr, CTuningWriter::ErrorCode2String(writerErr));
      TUNING_ASSERT(0);
      ret = ConvertErrorCode(writerErr);
   }
   else {
      // Dump done
      TUNING_INFO_MSG1("Dumped tuning configuration: %d bytes written\n", *pFilledBufferSize);
      ret = TUNING_OK;
   }

   // Deinstantiate the Writer
   delete pTuningWriter;
   pTuningWriter = NULL;
   TUNING_FUNC_OUTR(ret);
   return ret;
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
t_tuning_error_code CTuning::MergeWith(const CTuning* pTuningSource, const e_merge_option aMergeOption)
{
   TUNING_FUNC_IN0();

   // Check whether instance has been constructed
   if( IsInstanceConstructed() == false)
   {
      // Instance not constructed
      TUNING_ASSERT(0);
      TUNING_FUNC_OUTR(TUNING_NOT_CONSTRUCTED);
      return TUNING_NOT_CONSTRUCTED;
   }

   // Check parameter
   if(pTuningSource == NULL)
   {
      TUNING_ERROR_MSG1("Invalid argument: pSourceTuning=%p\n",pTuningSource);
      TUNING_ASSERT(0);
      TUNING_FUNC_OUTR(TUNING_INVALID_ARGUMENT);
      return TUNING_INVALID_ARGUMENT;
   }
   if(pTuningSource->iConstructed == false)
   {
      TUNING_ERROR_MSG0("Tuning object to merge not constructed\n");
      TUNING_ASSERT(0);
      TUNING_FUNC_OUTR(TUNING_NOT_CONSTRUCTED);
      return TUNING_NOT_CONSTRUCTED;
   }

   // Check whether sensor IDs match
   if(   iTuningData.iSensorId.manuf != pTuningSource->iTuningData.iSensorId.manuf
      || iTuningData.iSensorId.model != pTuningSource->iTuningData.iSensorId.model)
   {
      TUNING_ERROR_MSG2("Sensor IDs mismatch: Manufacturer ID: this:0x%lX, to-be-merged:0x%lX\n", iTuningData.iSensorId.manuf, pTuningSource->iTuningData.iSensorId.manuf);
      TUNING_ERROR_MSG2("Sensor IDs mismatch: Model ID: this:0x%lX, to-be-merged:0x%lX\n", iTuningData.iSensorId.model, pTuningSource->iTuningData.iSensorId.model);
      TUNING_ASSERT(0);
      TUNING_FUNC_OUTR(TUNING_SENSOR_ID_MISMATCH);
      return TUNING_SENSOR_ID_MISMATCH;
   }

   TUNING_PARAM_MSG2("[%s] Merging with [%s]\n", this->iInstanceName, pTuningSource->iInstanceName);

   // Merge source into destination ('this')
   const t_tuning_data* pSrcData = &pTuningSource->iTuningData;

   // Parse all IQSets of source
   for(int IqSetIndex=0; IqSetIndex < IQSET_COUNT; IqSetIndex++)
   {
      // IQSet to be merged
      const t_iqset* pSrcIqSet = &pSrcData->iIqSet[IqSetIndex];
      e_iqset_id     IqSetId = (e_iqset_id)IqSetIndex;

      // Find IQSET entry in destintation
      t_iqset* pDestIqSet = &iTuningData.iIqSet[IqSetId];

      // Parse all Users of source
      for(int UserId=0; UserId < IQSET_USER_COUNT; UserId++)
      {
          // Param shortcuts
          const t_iqset_user*  pSrcUser  = &pSrcIqSet->user[UserId];
          const t_iqset_param* pSrcParam = pSrcUser->pParamList;
          t_iqset_user*        pDestUser = &pDestIqSet->user[UserId];
          t_iqset_param*       pDestParam = NULL;

          if(pSrcUser->iParamCount!=0) {
              TUNING_PARAM_MSG2("[W] [%s] [%s]\n",
                                 CTuningIQSet::FindIQSetNameFromId(IqSetId),
                                 CTuningUser::FindUserNameFromId((e_iquser_id)UserId));
          }

          // Parse all parameters of this user
          while( pSrcParam != NULL)
          {
             t_uint32 ParamAddr = pSrcParam->addr;

             // Find whether parameter exist in destintation
             pDestParam = FindIqParam( pDestUser->pParamList, ParamAddr);
             if(pDestParam != NULL)
             {
                 // Param already exist
                 if(aMergeOption==TUNING_IGNORE_DUPLICATE_ENTRIES)
                 {
                    // Duplicate entries to be ignored silently
                    // The "this" param entry is kept
                    TUNING_INFO_MSG3("[%s][%s] Param 0x%lX already exist => Ignored\n",
                                     CTuningIQSet::FindIQSetNameFromId(IqSetId),
                                     CTuningUser::FindUserNameFromId((e_iquser_id)UserId),
                                     ParamAddr);
                    pSrcParam = pSrcParam->next;
                    continue;
                 }
                 else
                 {
                    // Duplicate entry to be handled as an error
                    TUNING_ERROR_MSG3("[%s][%s] Param 0x%lX already exist\n",
                                      CTuningIQSet::FindIQSetNameFromId(IqSetId),
                                      CTuningUser::FindUserNameFromId((e_iquser_id)UserId),
                                      ParamAddr);
                    TUNING_ASSERT(0);
                    TUNING_FUNC_OUTR(TUNING_DUPLICATE_PARAM);
                    return TUNING_DUPLICATE_PARAM;
                 }
             }

             // Check if there is a free param in the pool
             if(iTuningData.iParamsPool.iUsed >= iTuningData.iParamsPool.iTotal)
             {
                // No more space in Params array
                TUNING_ASSERT(0);
                TUNING_FUNC_OUTR(TUNING_PARAMS_ARRAY_FULL);
                return TUNING_PARAMS_ARRAY_FULL;
             }

             // Get a free parameter from the pool
             pDestParam = &iTuningData.iParamsPool.pParam[iTuningData.iParamsPool.iUsed];
             iTuningData.iParamsPool.iUsed++;

             // Link parameter to the current user list
             if(pDestUser->pParamList==NULL) {
                // No parameters yet
                pDestUser->pParamList = pDestParam;
             }
             else {
                pDestUser->pLastParam->next = pDestParam;
             }
             pDestUser->pLastParam = pDestParam;

             // New param entry in destination
             pDestUser->iParamCount++;

             // Raw copy of source param to destination
             *pDestParam = *pSrcParam;
             pDestParam->value = NULL;
             pDestParam->next = NULL;

             // Check if enough values entries are available in the pool
             if( iTuningData.iValuesPool.iUsed + (pDestParam->rows*pDestParam->cols) > iTuningData.iValuesPool.iTotal )
             {
                 TUNING_ERROR_MSG2("Too many value entries (required:%ld, max:%ld)\n", iTuningData.iValuesPool.iUsed+(pSrcParam->rows*pSrcParam->cols), iTuningData.iValuesPool.iTotal);
                 TUNING_ASSERT(0);
                 TUNING_FUNC_OUTR(TUNING_TOO_MANY_ENTRIES);
                 return TUNING_TOO_MANY_ENTRIES;
             }

             // Attach values slot to destination parameter
             pDestParam->value = &iTuningData.iValuesPool.pValue[iTuningData.iValuesPool.iUsed];
             iTuningData.iValuesPool.iUsed += pDestParam->rows*pDestParam->cols;
             TUNING_INFO_MSG4("Param 0x%lX: value=%p, numRows=%d, numCols=%d\n", pDestParam->addr, pDestParam->value, pDestParam->rows, pDestParam->cols);

             // Actual values need to be "manually" copied
             TUNING_PARAM_MSG6("[W] [%s][%s][Addr=0x%lX][Name=%s][Size=%dx%d]\n",
                               CTuningIQSet::FindIQSetNameFromId(IqSetId),
                               CTuningUser::FindUserNameFromId((e_iquser_id)UserId),
                               pDestParam->addr, pDestParam->name,
                               pDestParam->rows,
                               pDestParam->cols);
             for(int i=0; i<(pDestParam->rows*pDestParam->cols); i++)
             {
                 pDestParam->value[i] = pSrcParam->value[i];
                 if( pDestParam->type == IQSET_VT_INT )
                 {
                     TUNING_PARAM_MSG8("[W] [%s][%s][Addr=0x%lX][Name=%s][%d][%d] [Val=%ld] (Val=0x%lX)\n",
                                       CTuningIQSet::FindIQSetNameFromId(IqSetId),
                                       CTuningUser::FindUserNameFromId((e_iquser_id)UserId),
                                       pDestParam->addr, pDestParam->name,
                                       FlatIndex2Row(i,pDestParam->cols),
                                       FlatIndex2Col(i,pDestParam->cols),
                                       pDestParam->value[i].i32,
                                       pDestParam->value[i].i32);
                 }
                 else
                 {
                     TUNING_PARAM_MSG8("[W] [%s][%s][Addr=0x%lX][Name=%s][%d][%d] [Val=%f] (Val=0x%lX)\n",
                                       CTuningIQSet::FindIQSetNameFromId(IqSetId),
                                       CTuningUser::FindUserNameFromId((e_iquser_id)UserId),
                                       pDestParam->addr, pDestParam->name,
                                       FlatIndex2Row(i,pDestParam->cols),
                                       FlatIndex2Col(i,pDestParam->cols),
                                       pDestParam->value[i].f32,
                                       pDestParam->value[i].r32);
                 }
             }

             // Next Param
             pSrcParam = pSrcParam->next;
         }
      }
   }

   // Merge done
   TUNING_FUNC_OUTR(TUNING_OK);
   return TUNING_OK;
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 **/
/****************************************************************************/
t_tuning_error_code CTuning::ConvertErrorCode(t_tuning_parser_error_code aErr)
{
    switch(aErr)
    {
        case TUNING_PARSER_OK:                       return TUNING_OK;
        case TUNING_PARSER_MALLOC_ERROR:             return TUNING_MALLOC_ERROR;
        case TUNING_PARSER_FILE_OPEN_FAILED:         return TUNING_FILE_NOT_FOUND;
        case TUNING_PARSER_FILE_READ_ERROR:          return TUNING_FILE_READ_ERROR;
        case TUNING_PARSER_FOUND_NO_DATA:            return TUNING_FOUND_NO_ENTRIES;
        case TUNING_PARSER_TOO_MANY_ENTRIES:         return TUNING_TOO_MANY_ENTRIES;
        case TUNING_PARSER_PARAM_SIZE_MISMATCH:      return TUNING_PARAM_SIZE_MISMATCH;
        case TUNING_PARSER_PARAM_TYPE_MISMATCH:      return TUNING_PARAM_TYPE_MISMATCH;
        case TUNING_PARSER_VERSION_ENTRIES_MISMATCH: return TUNING_VERSION_MISMATCH;
        case TUNING_PARSER_VERSION_STRUCT_MISMATCH:  return TUNING_VERSION_MISMATCH;
        case TUNING_PARSER_XML_ERROR:                return TUNING_XML_ERROR;
        case TUNING_PARSER_DUPLICATE_PARAM:          return TUNING_DUPLICATE_PARAM;
        case TUNING_PARSER_INTERNAL_ERROR:           return TUNING_INTERNAL_ERROR;
        default:                                     return TUNING_INTERNAL_ERROR;
    }
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 **/
/****************************************************************************/
t_tuning_error_code CTuning::ConvertErrorCode(t_tuning_writer_error_code aErr)
{
    switch(aErr)
    {
        case TUNING_WRITER_FILE_OPEN_FAILED:  return TUNING_FILE_CREATION_FAILED;
        case TUNING_WRITER_FILE_WRITE_FAILED: return TUNING_FILE_WRITE_ERROR;
        case TUNING_WRITER_BUFFER_TOO_SMALL:  return TUNING_BUFFER_TOO_SMALL;
        default:                              return TUNING_INTERNAL_ERROR;
    }
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
const char* CTuning::ErrorCode2String(t_tuning_error_code aErr)
{
    for(unsigned int i=0; i<KTuningErrorCodesListSize; i++) {
        if(KTuningErrorCodesList[i].id == aErr)
            return KTuningErrorCodesList[i].name;
    }
    return "*** ERROR CODE NOT FOUND ***";
}
