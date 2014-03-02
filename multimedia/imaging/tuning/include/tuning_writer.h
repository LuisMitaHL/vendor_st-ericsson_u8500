/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _TUNING_WRITER_H_
#define _TUNING_WRITER_H_

#include "tuning_types.h"
#include "tuning_iqset.h"
#include "tuning_osal.h"

/** writer error code*/
typedef enum
{
    TUNING_WRITER_OK = 0,                   /**< last method called performed as expected */
    TUNING_WRITER_INVALID_ARGUMENT,         /**< bad argument provided to API function */
    TUNING_WRITER_FILE_OPEN_FAILED,         /**< tuning file could not be created */
    TUNING_WRITER_STRING_CONSTRUCT_FAILED,  /**< failed to construct on line of the file */
    TUNING_WRITER_FILE_WRITE_FAILED,        /**< error while writing to file */
    TUNING_WRITER_UNKNOWN_IQSET_ID,         /**< could not recognize an IQSET id */
    TUNING_WRITER_UNKNOWN_IQ_PARAM_TYPE,    /**< could not recognize an PARAM type */
    TUNING_WRITER_BAD_PARAM_SIZE,           /**< PARAM size is invalid */
    TUNING_WRITER_BAD_VALUE_LIST,           /**< Vector/Matrix values list invalid */
    TUNING_WRITER_BUFFER_TOO_SMALL,         /**< insufficient space in destination buffer */
    TUNING_WRITER_INTERNAL_ERROR,           /**< internal error */
} t_tuning_writer_error_code;

/** \brief class implementing the writing of PE configuration into xml tuning files */
class CTuningWriter
{
  public:
      CTuningWriter ();
      ~CTuningWriter();
      t_tuning_writer_error_code DumpToFile(const char* aFileName, t_tuning_data* pTuningData);
      t_tuning_writer_error_code DumpToBuffer(void* pDestBuffer, const int aDestBufferSize, int* pFilledBufferSize, const t_tuning_data* pTuningData);
      static const char*         ErrorCode2String(const t_tuning_writer_error_code);

  private:
      /** private data */
      t_tuning_struct_version    iWriterVersion;
      char                       iCurLineStr[256];
      int                        iCurLineNum;
      int                        iIndentCount;
      char                       iIndentStr[128];
      char                       iValStr[256];
      CTuningOsal                iOsal;
      bool                       iToBuffer;
      char*                      iDestBuffer;
      t_uint32                   iDestBufferSize;
      t_uint32                   iDestBufferFilledSize;

      /** private methods */
      t_tuning_writer_error_code DoMakeXml(const t_tuning_data* pTuningData);
      void                       IncLineNum();
      void                       ResetLineNum();
      void                       ResetIndent();
      void                       Indent();
      void                       UnIndent();
      t_tuning_writer_error_code OpenFile(const char *);
      t_tuning_writer_error_code CloseFile();
      t_tuning_writer_error_code WriteXmlHeaderSection();
      t_tuning_writer_error_code WriteFileIdSection(const t_tuning_data* pTuningData);
      t_tuning_writer_error_code WriteIqSettingsSection(const t_tuning_data* pTuningData);
      t_tuning_writer_error_code WriteConfigurationSection(const t_tuning_data* pTuningData);
      t_tuning_writer_error_code WriteUserSection(const e_iquser_id aUserId, const t_iqset_user* pIqUser);
      t_tuning_writer_error_code WriteLine(const char*);
      t_tuning_writer_error_code WriteLine(const char*, const char*, const char*);
      t_tuning_writer_error_code DoWriteLine();
      const char*                MakeIndent();
      const char*                MakeString(const t_sint16);
      const char*                MakeString(const t_uint16);
      const char*                MakeString(const t_sint32);
      const char*                MakeString(const t_uint32);
      const char*                MakeString(const int);
      const char*                MakeString(const float);
      const char*                MakeString(const t_sint32*, const int);
      const char*                MakeString(const float*, const int);
      const char*                MakeString(const u_iqset_value*, const e_iqset_value_type, const int);
      const char*                MakeHexString(const t_sint16);
      const char*                MakeHexString(const t_uint16);
      const char*                MakeHexString(const t_sint32);
      const char*                MakeHexString(const t_uint32);
      const char*                MakeHex16String(const t_uint32);
      const char*                MakeRowColString(const int aRows, const int aCols);
};

#endif /*_TUNING_WRITER_H_*/
