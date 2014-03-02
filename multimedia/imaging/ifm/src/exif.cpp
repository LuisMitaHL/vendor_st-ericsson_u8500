/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#define OMXCOMPONENT "EXIF"
#define OMX_TRACE_UID 0x10
#include "osi_trace.h"
#include <string.h>
#include <math.h>
#include <stdio.h>
#include "exif.h"
#include "ImgConfig.h"
#include "ENS_DBC.h"
/* +CR370700 */
#include "ExifReader.h"
/* -CR370700 */


#if defined(__SYMBIAN32__) || defined(ANDROID)
// In Symbian and Android, log2 not in libc
#define M_LOG2_E  0.693147180559945309417
#define log2f(x)  (logf(x) / (float)M_LOG2_E)
#endif

/* ------------------------------------- */
/*             APP1 header               */
/* ------------------------------------- */

CApp1Header::CApp1Header()
{
   iHeaderData.iMarkerPrefix = 0xFF;
   iHeaderData.iApp1Marker   = 0xE1;
   iHeaderData.iApp1Length   = 0;
   iHeaderData.iExifId[0]    = 'E';
   iHeaderData.iExifId[1]    = 'x';
   iHeaderData.iExifId[2]    = 'i';
   iHeaderData.iExifId[3]    = 'f';
   iHeaderData.iExifId[4]    = 0;
   iHeaderData.iPadding      = 0;
}

void CApp1Header::setLenght(OMX_U16 aLength)
{
   iHeaderData.iApp1Length = aLength;
}

OMX_U32 CApp1Header::size()
{
   return sizeof(App1Header_t);
}

OMX_U32 CApp1Header::serialize(OMX_U8* pDest)
{
   /* Raw copy of APP1 header data to destination */
   memcpy(pDest, &iHeaderData, sizeof(App1Header_t));
   return sizeof(App1Header_t);
}

/* ------------------------------------- */
/*             TIFF header               */
/* ------------------------------------- */

CTiffHeader::CTiffHeader()
{
   /* CPU endianness retrieval */
#if defined(LINUX) && defined(WORKSTATION)
   const bool bigEndian = false;
#else
   const OMX_U16 val=0xAABB;
   const bool bigEndian = ((char*)&val)[0]==0xAA;
#endif

   if(bigEndian) {
      iHeaderData.iByteOrder[0] = 'M';
      iHeaderData.iByteOrder[1] = 'M';
   }
   else {
      iHeaderData.iByteOrder[0] = 'I';
      iHeaderData.iByteOrder[1] = 'I';
   }
   iHeaderData.iFourtyTwo = 0x002A;
   iHeaderData.iIfdOffset = 0;
}

OMX_U32 CTiffHeader::size()
{
   return sizeof(TiffHeader_t);
}

void CTiffHeader::setIfdOffset(OMX_U32 aOffset)
{
   iHeaderData.iIfdOffset = aOffset;
}

OMX_U32 CTiffHeader::serialize(OMX_U8* pDest)
{
   /* Raw copy of TIFF header data to destination */
   memcpy(pDest, &iHeaderData, sizeof(TiffHeader_t));
   return sizeof(TiffHeader_t);
}


/* ------------------------------------- */
/*   Main/EXIF syntax descriptors        */
/* ------------------------------------- */

/* Note: 0 length denote variable-length fields. */
static const IFDfieldDesc_t IFDsyntax[] =
{
   // TIFF attributes
   {IFDtag_Orientation,              IFD_SHORT,     1},
   {IFDtag_YCbCrPositioning,         IFD_SHORT,     1},
   {IFDtag_XResolution,              IFD_RATIONAL,  1},
   {IFDtag_YResolution,              IFD_RATIONAL,  1},
   {IFDtag_ResolutionUnit,           IFD_SHORT,     1},
   {IFDtag_Make,                     IFD_ASCII,     0},
   {IFDtag_Model,                    IFD_ASCII,     0},
   {IFDtag_ExifIfdPointer,           IFD_LONG,      1},
   {IFDtag_GpsInfoIfdPointer,        IFD_LONG,      1},

   // EXIF attributes
   {IFDtag_ExifVersion,              IFD_UNDEFINED, 4},
   {IFDtag_FlashpixVersion,          IFD_UNDEFINED, 4},
   {IFDtag_ColorSpace,               IFD_SHORT,     1},
   {IFDtag_ComponentsConfiguration,  IFD_UNDEFINED, 4},
   {IFDtag_PixelXDimension,          IFD_SHORT,     1},
   {IFDtag_PixelYDimension,          IFD_SHORT,     1},
   {IFDtag_DateTimeOriginal,         IFD_ASCII,     20},
   {IFDtag_DateTimeDigitized,        IFD_ASCII,     20},
   {IFDtag_DateTime,		         IFD_ASCII,     20},
   {IFDtag_Software,                 IFD_ASCII,     0},

   // Picture-taking specifics
   {IFDtag_ExposureTime,             IFD_RATIONAL,  1},
   {IFDtag_FNumber,                  IFD_RATIONAL,  1},
   {IFDtag_ISOSpeedRatings,          IFD_SHORT,     1},
   {IFDtag_ShutterSpeedValue,        IFD_SRATIONAL, 1},
   {IFDtag_ApertureValue,            IFD_RATIONAL,  1},
   {IFDtag_BrightnessValue,          IFD_SRATIONAL, 1},
   {IFDtag_SubjectDistance,          IFD_RATIONAL,  1},
   {IFDtag_MeteringMode,             IFD_SHORT,     1},
   {IFDtag_LightSource,              IFD_SHORT,     1},
   {IFDtag_Flash,                    IFD_SHORT,     1},
   {IFDtag_FocalLength,              IFD_RATIONAL,  1},
   {IFDtag_SubjectArea,              IFD_SHORT,     4},
   {IFDtag_MakerNote,                IFD_UNDEFINED, 0},
   {IFDtag_FlashEnergy,              IFD_RATIONAL,  1},
   {IFDtag_ExposureIndex,            IFD_RATIONAL,  1},
   {IFDtag_CustomRendered,           IFD_SHORT,     1},
   {IFDtag_ExposureMode,             IFD_SHORT,     1},
   {IFDtag_WhiteBalance,             IFD_SHORT,     1},
   {IFDtag_DigitalZoomRatio,         IFD_RATIONAL,  1},
   {IFDtag_SceneCaptureType,         IFD_SHORT,     1},
   {IFDtag_GainControl,              IFD_SHORT,     1},
   {IFDtag_Contrast,                 IFD_SHORT,     1},
   {IFDtag_ExposureBiasValue,  IFD_SRATIONAL, 1},
   {IFDtag_SubjectDistanceRange, IFD_SHORT,1},

   {IFDtag_InteroperabilityIFDPointer, IFD_LONG,1},
   {IFDtag_Index, IFD_ASCII,0},
   {IFDtag_Version, IFD_UNDEFINED,4}
   	
};

/* ------------------------------------- */
/*            CIfd base class            */
/* ------------------------------------- */

CIfd::CIfd( IFDfield_t* aFields,
            OMX_U16     aNumberOfFields,
            OMX_U32     aFieldsSize,
            OMX_U8*     aLargeValuesArea,
            OMX_U32     aLargeValuesAreaSize)
{
   pFields              = aFields;
   iNumberOfFields      = aNumberOfFields;
   iFieldsSize          = aFieldsSize;
   pLargeValuesArea     = aLargeValuesArea;
   iLargeValuesAreaSize = aLargeValuesAreaSize;
   iNextIfdOffset       = 0;
   iLargeDataSizeActual = 0;
}

void CIfd::setNextIfdOffset(OMX_U16 aNextIfdOffset)
{
   iNextIfdOffset = aNextIfdOffset;
}

OMX_U32 CIfd::size()
{
   // Return the byte size of the IFD segment to be binary copied
   return   sizeof(iNumberOfFields)
          + iFieldsSize
          + sizeof(iNextIfdOffset)
          + iLargeDataSizeActual;
}

OMX_U32 CIfd::serialize(OMX_U8* pData, OMX_U32 aOffsetFromTiffHeader)
{
   OMX_U8* pNumberOfFields  = pData;
   OMX_U8* pExifFields      = pData+sizeof(iNumberOfFields);
   OMX_U8* pNextIfdOffset   = pData+sizeof(iNumberOfFields)+iFieldsSize;
   OMX_U8* pExifLargeValues = pData+sizeof(iNumberOfFields)+iFieldsSize+sizeof(iNextIfdOffset);

   MSG2("pNumberOfFields = %p, pNextIdOffset = %p\n", pNumberOfFields, pNextIfdOffset);
   memcpy(pNumberOfFields, &iNumberOfFields, sizeof(OMX_U16));
   patchLargeValuesOffsets(aOffsetFromTiffHeader);
   memcpy(pExifFields, pFields, iFieldsSize);
   memcpy(pNextIfdOffset, &iNextIfdOffset, sizeof(OMX_U32));
   memcpy(pExifLargeValues, pLargeValuesArea, iLargeDataSizeActual);

   return size();
}

OMX_ERRORTYPE CIfd::initField(IFDfield_t* pField, IFDtag_e aTag)
{
   // Sanity check
   if (pField == NULL) {
      MSG0("Invalid field entry: pField==NULL\n");
      return OMX_ErrorBadParameter;
   }

   // Retrieve field desctiption
   IFDfieldDesc_t const* pFieldDesc = getFieldDescFromTag(aTag);
   if(pFieldDesc == NULL) {
      MSG1("Could not find description for tag %d\n",aTag);
      return OMX_ErrorUnsupportedIndex;
   }

   // Initialize field
   pField->tag         = aTag;
   pField->type        = pFieldDesc->type;
   pField->count       = pFieldDesc->count;
   pField->valueOffset = 0;

   // Done
   return OMX_ErrorNone;
}

int CIfd::sizeOfType(IFDtype_e aType)
{
   int size = 0;
   switch(aType)
   {
      case IFD_BYTE:
      case IFD_ASCII:
      case IFD_UNDEFINED:
         size = 1;
         break;
      case IFD_SHORT:
         size = 2;
         break;
      case IFD_LONG:
      case IFD_SLONG:
         size = 4;
         break;
      case IFD_RATIONAL:
      case IFD_SRATIONAL:
         size = 8;
         break;
      default:
         MSG1("Type %d not recognized\n", aType);
         size = 0;
   }
   return size;
}


/**
 ******************************************************************************
 * @brief   Set the tag-selected IFD field to the passed array.
 * @par Abstract:
 *          <>
 * @param   tag      Unique identifier of the selected field.
 * @param   pData    Reference to the array containing values to set the IFD field to.
 * @param   bSetType If set, additionnally initialize the 'type' and 'count' of
 *                   the selected IFD field.
 * @return  - OMX_ErrorUnsupportedIndex   Field does not exist in this object.
 *          - OMX_ErrorFormatNotDetected  Encountered a corrupted field description.
 *          - OMX_ErrorUnsupportedSetting Field type/count combination is not supported.
 ******************************************************************************
 */

OMX_ERRORTYPE CIfd::setFieldFromTag(IFDtag_e aTag, const void* pData, IFDtype_e aType, int aCount)
{
   // Sanity check
   if(aCount <= 0) {
      MSG2("Tag %d: invalid atgument: aCount = %d\n", aTag, aCount);
      return OMX_ErrorBadParameter;
   }

   // Get field storage address in object data.
   IFDfield_t* pField = getFieldFromTag(aTag);
   if (pField == NULL) {
      // Could not find the field having tag==aTag
      // Possible causes:
      //   - the field is really missing (i.e. not there in the IFD fields array)
      //   - or the field is there but is not initialized
      MSG1("Could not find IFD field having tag==%d\n", aTag);
      return OMX_ErrorUnsupportedIndex;
   }

   // Check that field type matches the definition
   if(pField->type != aType) {
      MSG3("Tag %d: Argument type does not match field definition (expected type: %u, argument type: %d)\n", aTag, pField->type, aType);
      return OMX_ErrorBadParameter;
   }

   // In case of variable-length, use the passed parameter count
   // Else, the count argument must match the field definition
   if (pField->count == 0) {
      pField->count = aCount;
   }
   else if(pField->count != (OMX_U32)aCount) {
      MSG3("Tag %d: Argument count does not match field definition (expected count: %lu, argument count: %d)\n", aTag, pField->count, aCount);
      return OMX_ErrorBadParameter;
   }

   // Get the storage size for this type
   int size = sizeOfType((IFDtype_e)pField->type);
   if(size==0) {
      MSG1("Could not determine size for field tag %d\n", aTag);
      return OMX_ErrorFormatNotDetected;
   }

   // Do store value of this field
   OMX_ERRORTYPE err = storeField(pData, pField->count*size, &pField->valueOffset);
   if(err != OMX_ErrorNone) {
      MSG1("Failed to store value for tag %d\n", aTag);
      return err;
   }

   // Done
   return OMX_ErrorNone;
}

OMX_ERRORTYPE CIfd::setFieldFromTag(IFDtag_e aTag, const char* pData, int aLength)
{
   return setFieldFromTag( aTag, pData, IFD_ASCII, aLength);
}

OMX_ERRORTYPE CIfd::setFieldFromTag(IFDtag_e aTag, OMX_U8 aData)
{
   return setFieldFromTag( aTag, &aData, IFD_BYTE, 1);
}

OMX_ERRORTYPE CIfd::setFieldFromTag(IFDtag_e aTag, const exif_undefined_t* pData, int aCount)
{
   return setFieldFromTag( aTag, pData, IFD_UNDEFINED, aCount);
}

OMX_ERRORTYPE CIfd::setFieldFromTag(IFDtag_e aTag, OMX_U16 aData)
{
   return setFieldFromTag( aTag, &aData, IFD_SHORT, 1);
}

OMX_ERRORTYPE CIfd::setFieldFromTag(IFDtag_e aTag, const OMX_U16* pData, int aCount)
{
   return setFieldFromTag( aTag, pData, IFD_SHORT, aCount);
}

OMX_ERRORTYPE CIfd::setFieldFromTag(IFDtag_e aTag, OMX_S32 aData)
{
   return setFieldFromTag( aTag, &aData, IFD_SLONG, 1);
}

OMX_ERRORTYPE CIfd::setFieldFromTag(IFDtag_e aTag, OMX_U32 aData)
{
   return setFieldFromTag( aTag, &aData, IFD_LONG, 1);
}

OMX_ERRORTYPE CIfd::setFieldFromTag(IFDtag_e aTag, const exif_srational_t* pData)
{
   return setFieldFromTag( aTag, pData, IFD_SRATIONAL, 1);
}

OMX_ERRORTYPE CIfd::setFieldFromTag(IFDtag_e aTag, const exif_urational_t* pData)
{
   return setFieldFromTag( aTag, pData, IFD_RATIONAL, 1);
}


/**
 ******************************************************************************
 * @brief   Get the tag-selected IFD field format.
 * @param   tag      Unique identifier of the selected field.
 * @return  - IFDfieldDesc_t * : ref. to a structure describing the field type and size.
 *          - NULL             if no field descriptor matches the passed tag.
 ******************************************************************************
 */
const IFDfieldDesc_t* CIfd::getFieldDescFromTag(IFDtag_e aTag)
{
   const IFDfieldDesc_t* pFieldDesc = NULL;
   // Lookup the field descriptor in the syntax EXIF structure.
   for (OMX_U32 i = 0; i < sizeof(IFDsyntax) / sizeof(IFDfieldDesc_t); i++)
   {
      if (IFDsyntax[i].tag == aTag) {
         pFieldDesc = &IFDsyntax[i];
         break;
      }
   }
   return pFieldDesc;
}

/**
 ******************************************************************************
 * @brief   Get the tag-selected IFD field.
 * @param   tag      Unique identifier of the selected field.
 * @return  - IFDfield_t * : ref to the selected field in the class IFD storage.
 *          - NULL         if no field matches the passed tag..
 ******************************************************************************
 */
IFDfield_t* CIfd::getFieldFromTag(IFDtag_e aTag)
{
   IFDfield_t* pRet = NULL;
   // Lookup the field structure in the class IFD (fields are stored in tag order).

   for (OMX_U16 i = 0; i < iNumberOfFields; i++)
   {
      if (pFields[i].tag > aTag)
      {  // Tag scan failed: Error: The IFD has not been created in tag order -> abort request.
         break;
      }
      if (pFields[i].tag == aTag)
      {  // Tag found.
         pRet = &(pFields[i]);
         break;
      }
   }
   return pRet;
}

/**
 ******************************************************************************
 * @brief   Set a value of an IFD field.
 * @par Abstract:
 *      Store the data passed as reference to a ValueOffset field, or to the
 *      extended storage area of the IFD buffer (iLargeValuesArea), depending on its
 *      size.
 *      pValueOffset is updated with either the stored value, or with the offset
 *      from the IFD start.
 * @param   in: p_data        Reference to the data to store.
 * @param   in: size          Byte size of the data to store.
 * @param   out: pValueOffset Field of the IFD field where the offset to the passed data will be updated.
 * @return  - OMX_ErrorNone
 ******************************************************************************
 */
OMX_ERRORTYPE CIfd::storeField(const void* pData, OMX_U32 aSize, OMX_U32* pValueOffset)
{
   OMX_U8*       pu8_out = (OMX_U8*)pValueOffset;
   const OMX_U8* pu8_in = (OMX_U8*)pData;
   OMX_U32       storeSize = aSize;

   if (storeSize > sizeof(OMX_U32))
   {
      // Write to extended storage
      if (*pValueOffset == 0)
      {
         // No extended storage reserved yet
         storeSize = (storeSize + 3) & ~3;   // Aligned on word boundaries

         // If enough space in the Large Value area then compute destination offset
         if (storeSize + iLargeDataSizeActual < iLargeValuesAreaSize)
         {
            // Offset of the value from the start of the Large Values area
            *pValueOffset = iLargeDataSizeActual;
            iLargeDataSizeActual += storeSize;
         }
         else {
            MSG1("Extended storage area is full (size=%lu)\n", iLargeValuesAreaSize);
            return OMX_ErrorInsufficientResources;
         }
      }
      pu8_out = &pLargeValuesArea[*pValueOffset];
   }

   /* Does not worth a memcpy */
   for (OMX_U32 i = 0; i < storeSize; i++) {
      pu8_out[i] = pu8_in[i];
   }
   return OMX_ErrorNone;
}

void CIfd::patchLargeValuesOffsets(OMX_U32 aOffsetFromTiffHeader)
{
   for(OMX_U16 i=0; i < iNumberOfFields; i++)
   {
      if( (pFields[i].count * sizeOfType((IFDtype_e)pFields[i].type)) > sizeof(OMX_U32))
      {
         // 'valueOffset' is the offset from the start of buffer 'pLargeValuesArea'
         // => actual offset value must be from the start of the TIFF header
         pFields[i].valueOffset +=   aOffsetFromTiffHeader
                                   + sizeof(iNumberOfFields)
                                   + iFieldsSize
                                   + sizeof(iNextIfdOffset);
      }
   }
}

/**
 ******************************************************************************
 * @brief   Check whether all EXIF fields were initialized.
 * @par Abstract:
 *      Store the data passed as reference to a ValueOffset field, or to the
 *      extended storage area of the IFD buffer (iLargeValuesArea), depending on its
 *      size.
 *      pValueOffset is updated with either the stored value, or with the offset
 *      from the IFD start.
 * @param   in: p_data        Reference to the data to store.
 * @param   in: size          Byte size of the data to store.
 * @param   out: pValueOffset Field of the IFD field where the offset to the passed data will be updated.
 * @return  - OMX_ErrorNone
 ******************************************************************************
 */
bool CIfd::initialized()
{
   // Check whether all IFD entries were set
   for(unsigned int i=0; i < iNumberOfFields; i++)
   {
      if(   pFields[i].tag == 0
         || pFields[i].count == 0
         || pFields[i].type == IFD_TYPE_UNKNOWN)
      {
        MSG1("Field[%d] not initialized\n",i);
        return false;
      }
   }
   return true;
}

/* ------------------------------------- */
/*  Field initialization helper macros   */
/* ------------------------------------- */

/* Macro for initializing fields */
#define INIT_FIELD(_ifd_,_field_)                            \
   do {                                                      \
      OMX_ERRORTYPE err = OMX_ErrorNone;                     \
      err = initField( &_ifd_._field_, IFDtag_##_field_);    \
      if(err!=OMX_ErrorNone) {                               \
         MSG1("Failed to initialize field '%s'\n", #_field_);\
         DBC_ASSERT(0);                                      \
      }                                                      \
   } while(0)

/* ------------------------------------- */
/*           MainIfd class               */
/* ------------------------------------- */

CMainIfd::CMainIfd():CIfd( (IFDfield_t*)&iMainIfdFields,
                            sizeof(iMainIfdFields)/sizeof(IFDfield_t),
                            sizeof(iMainIfdFields),
                            iMainIfdLargeValuesArea,
                            sizeof(iMainIfdLargeValuesArea))
{
   // Init the Main Ifd entries
   INIT_FIELD(iMainIfdFields, Make);
   INIT_FIELD(iMainIfdFields, Model);
   INIT_FIELD(iMainIfdFields, Orientation);
   INIT_FIELD(iMainIfdFields, XResolution);
   INIT_FIELD(iMainIfdFields, YResolution);
   INIT_FIELD(iMainIfdFields, ResolutionUnit);
   INIT_FIELD(iMainIfdFields, Software);
   INIT_FIELD(iMainIfdFields, DateTime);
   INIT_FIELD(iMainIfdFields, YCbCrPositioning);
   INIT_FIELD(iMainIfdFields, ExifIfdPointer);
   INIT_FIELD(iMainIfdFields, GpsInfoIfdPointer);
}

CInterOpIfd::CInterOpIfd():CIfd( (IFDfield_t*)&iInterOpIfdFields,
                            sizeof(iInterOpIfdFields)/sizeof(IFDfield_t),
                            sizeof(iInterOpIfdFields),
                            iInterOpIfdLargeValuesArea,
                            sizeof(iInterOpIfdLargeValuesArea))
{
   // Init the Main Ifd entries
   INIT_FIELD(iInterOpIfdFields, Index);
   INIT_FIELD(iInterOpIfdFields, Version);
}
	  
/* ------------------------------------- */
/*           ExifIfd class               */
/* ------------------------------------- */

CExifIfd::CExifIfd():CIfd( (IFDfield_t*)&iExifIfdFields,
                           sizeof(iExifIfdFields)/sizeof(IFDfield_t),
                           sizeof(iExifIfdFields),
                           iExifIfdLargeValuesArea,
                           sizeof(iExifIfdLargeValuesArea))
{
   OMX_ERRORTYPE err = OMX_ErrorNone;

   // Init the EXIF Ifd entries
   INIT_FIELD(iExifIfdFields, ExposureTime);
   INIT_FIELD(iExifIfdFields, FNumber);
   INIT_FIELD(iExifIfdFields, ISOSpeedRatings);
   INIT_FIELD(iExifIfdFields, ExifVersion);
   INIT_FIELD(iExifIfdFields, DateTimeOriginal);
   INIT_FIELD(iExifIfdFields, DateTimeDigitized);
   INIT_FIELD(iExifIfdFields, ComponentsConfiguration);
   INIT_FIELD(iExifIfdFields, ShutterSpeedValue);
#ifndef CUSTOMER_EXIF
   INIT_FIELD(iExifIfdFields, ApertureValue);
   INIT_FIELD(iExifIfdFields, BrightnessValue);
   INIT_FIELD(iExifIfdFields, SubjectDistance);
   INIT_FIELD(iExifIfdFields, GainControl);
   INIT_FIELD(iExifIfdFields, Contrast);
   INIT_FIELD(iExifIfdFields, SubjectArea);
#endif
 
   INIT_FIELD(iExifIfdFields, MeteringMode);
   INIT_FIELD(iExifIfdFields, LightSource);
   INIT_FIELD(iExifIfdFields, Flash);
   INIT_FIELD(iExifIfdFields, FocalLength);
#if (IMG_CONFIG_EXIF_MAKER_NOTES == IMG_CONFIG_TRUE)
   INIT_FIELD(iExifIfdFields, MakerNote);
#endif
   INIT_FIELD(iExifIfdFields, FlashpixVersion);
   INIT_FIELD(iExifIfdFields, ColorSpace);
   INIT_FIELD(iExifIfdFields, PixelXDimension);
   INIT_FIELD(iExifIfdFields, PixelYDimension);
#ifndef CUSTOMER_EXIF
   INIT_FIELD(iExifIfdFields, ExposureIndex);
#endif
   INIT_FIELD(iExifIfdFields, CustomRendered);
   INIT_FIELD(iExifIfdFields, ExposureMode);
   INIT_FIELD(iExifIfdFields, WhiteBalance);
   INIT_FIELD(iExifIfdFields, DigitalZoomRatio);
   INIT_FIELD(iExifIfdFields, SceneCaptureType);


   INIT_FIELD(iExifIfdFields, ExposureBiasValue);
   INIT_FIELD(iExifIfdFields, SubjectDistanceRange); 
   INIT_FIELD(iExifIfdFields, InteroperabilityIFDPointer);

   // Hardcode ExifVersion to "v2.2"
   const char* pExifVersion = "0220";
   MSG1("Setting ExifVersion = '%s'\n", pExifVersion);
   err = setFieldFromTag( IFDtag_ExifVersion, (const exif_undefined_t*)pExifVersion, 4);
   if(err!=OMX_ErrorNone) {
      MSG0("Failed to set field ExifVersion\n");
      DBC_ASSERT(0);
   }

   // Hardcode FlashpixVersion to "1.0"
   const char* pFlashpixVersion = "0100";
   MSG1("Setting FlashpixVersion = '%s'\n", pFlashpixVersion);
   err = setFieldFromTag( IFDtag_FlashpixVersion, (const exif_undefined_t*)pFlashpixVersion, 4);
   if(err!=OMX_ErrorNone) {
      MSG0("Failed to set field FlashpixVersion\n");
      DBC_ASSERT(0);
   }
}

/* ------------------------------------- */
/*     EXIF Metadata Management class    */
/* ------------------------------------- */

CExifMetadata::CExifMetadata()
{
}

OMX_U32 CExifMetadata::size()
{
   return iApp1Header.size() + iTiffHeader.size() + iMainIFD.size() + iExifIFD.size() + iInterOpIFD.size();
}

OMX_ERRORTYPE CExifMetadata::setMake(const char* aManufacturer)
{
   /*
    * EXIF Make values
    * The manufacturer of the recording equipment. This is the manufacturer of the DSC, scanner,
    * video digitizer or other equipment that generated the image.
    * When the field is left blank,it is treated as unknown.
    */
   MSG1("Setting Make = '%s'\n", aManufacturer);
   return iMainIFD.setFieldFromTag(IFDtag_Make, aManufacturer, strnlen(aManufacturer,32)+1);
}

OMX_ERRORTYPE CExifMetadata::setModel(const char* aModel)
{
   /*
    * EXIF Model values
    * The model name or model number of the equipment. This is the model name of number of the DSC,
    * scanner, video digitizer or other equipment that generated the image.
    * When the field is left blank, it is treated as unknown.
    */
   MSG1("Setting Model = '%s'\n", aModel);
   return iMainIFD.setFieldFromTag(IFDtag_Model, aModel, strnlen(aModel,32)+1);
}

OMX_ERRORTYPE CExifMetadata::setOrientation(OMX_SYMBIAN_ORIENTATIONORIGINTYPE aOmxOrientation)
{
   /*
    * EXIF Orientation values
    * 1 = The 0th row is at the visual top of the image, and the 0th column is the visual left-hand side.
    * 2 = The 0th row is at the visual top of the image, and the 0th column is the visual right-hand side.
    * 3 = The 0th row is at the visual bottom of the image, and the 0th column is the visual right-hand side.
    * 4 = The 0th row is at the visual bottom of the image, and the 0th column is the visual left-hand side.
    * 5 = The 0th row is the visual left-hand side of the image, and the 0th column is the visual top.
    * 6 = The 0th row is the visual right-hand side of the image, and the 0th column is the visual top.
    * 7 = The 0th row is the visual right-hand side of the image, and the 0th column is the visual bottom.
    * 8 = The 0th row is the visual left-hand side of the image, and the 0th column is the visual bottom.
    * Other = reserved
    */

   OMX_U16 orientation = 0;
   switch(aOmxOrientation)
   {
      case OMX_SYMBIAN_OrientationNotSpecified:         orientation = 1; break;
      case OMX_SYMBIAN_OrientationRowTopColumnLeft:     orientation = 1; break;
      case OMX_SYMBIAN_OrientationRowTopColumnRight:    orientation = 2; break;
      case OMX_SYMBIAN_OrientationRowBottomColumnRight: orientation = 3; break;
      case OMX_SYMBIAN_OrientationRowBottomColumnLeft:  orientation = 4; break;
      case OMX_SYMBIAN_OrientationRowLeftColumnTop:     orientation = 5; break;
      case OMX_SYMBIAN_OrientationRowRightColumnTop:    orientation = 6; break;
      case OMX_SYMBIAN_OrientationRowRightColumnBottom: orientation = 7; break;
      case OMX_SYMBIAN_OrientationRowLeftColumnBottom:  orientation = 8; break;
      default:
        MSG1("Unreconized orientation %d", aOmxOrientation);
        return OMX_ErrorBadParameter;
   }
   MSG1("Setting Orientation = %d\n", orientation);
   return iMainIFD.setFieldFromTag(IFDtag_Orientation, orientation);
}

OMX_ERRORTYPE CExifMetadata::setXResolution(OMX_U32 aXResolution)
{
   /*
    * EXIF XResolution values
    * The number of pixels per ResolutionUnit in the ImageWidth direction.
    * When the image resolution is unknown, 72 [dpi] is designated.
    */
   exif_urational_t xres;
   xres.num = aXResolution;
   xres.den = 1;
   MSG3("Setting XResolution = %lu/%lu (%f)\n", xres.num, xres.den, (float)xres.num/(float)xres.den);
   return iMainIFD.setFieldFromTag(IFDtag_XResolution, &xres );
}

OMX_ERRORTYPE CExifMetadata::setYResolution(OMX_U32 aYResolution)
{
   /*
    * EXIF YResolution values
    * The number of pixels per ResolutionUnit in the ImageLength direction.
    * The same value as XResolution is designated.
    */
   exif_urational_t yres;
   yres.num = aYResolution;
   yres.den = 1;
   MSG3("Setting YResolution = %lu/%lu (%f)\n", yres.num, yres.den, (float)yres.num/(float)yres.den);
   return iMainIFD.setFieldFromTag(IFDtag_YResolution, &yres );
}

OMX_ERRORTYPE CExifMetadata::setResolutionUnit(IFM_RESOLUTION_UNIT aResolutionUnit)
{
   /*
    * EXIF ResolutionUnit values
    * 1 = No absolute unit of measurement. Used for images that may have a non-square
    *     aspect ratio but no meaningful absolute dimensions.
    * 2 = Inch.
    * 3 = Centimeter.
    * Default = 2 (inch).
    */
   OMX_U16 resunit = 2;
   switch(aResolutionUnit)
   {
      case IFM_ResolutionUnit_NoUnit:     resunit = 1; break;
      case IFM_ResolutionUnit_Inch:       resunit = 2; break;
      case IFM_ResolutionUnit_Centimeter: resunit = 3; break;
      default:
        MSG1("Unreconized resolution unit %d", aResolutionUnit);
        return OMX_ErrorBadParameter;
   }
   MSG1("Setting ResolutionUnit = %u\n", resunit);
   return iMainIFD.setFieldFromTag(IFDtag_ResolutionUnit, resunit );
}

OMX_ERRORTYPE CExifMetadata::setYCbCrPositioning(IFM_YCbCr_POSITIONING aYCbCrPositioning)
{
   /*
    * EXIF YCbCrPositioning values
    * The position of chrominance components in relation to the luminance component.
    * This field is designated only for JPEG compressed data or uncompressed YCbCr data.
    * The TIFF default is 1 (centered); but when Y:Cb:Cr = 4:2:2 it is recommended in this standard that 2
    * (co-sited) be used to record data, in order to improve the image quality when viewed on TV systems.
    * When this field does not exist, the reader shall assume the TIFF default. In the case of Y:Cb:Cr = 4:2:0,
    * the TIFF default (centered) is recommended. If the reader does not have the capability of supporting
    * both kinds of YCbCrPositioning, it shall follow the TIFF default regardless of the value in this field.
    * It is preferable that readers be able to support both centered and co-sited positioning.
    * Default = 1
    * 1 = centered
    * 2 = co-sited
    * Other = reserved
    */
   OMX_U16 positioning = 1;
   switch(aYCbCrPositioning)
   {
      case IFM_YCbCrPositioning_Unknown:  positioning = 1; break;
      case IFM_YCbCrPositioning_Centered: positioning = 1; break;
      case IFM_YCbCrPositioning_CoSited:  positioning = 2; break;
      default:
        MSG1("Unreconized YCbCr Positioning %d", aYCbCrPositioning);
        return OMX_ErrorBadParameter;
   }
   MSG1("Setting YCbCrPositioning = %u\n", positioning);
   return iMainIFD.setFieldFromTag(IFDtag_YCbCrPositioning, positioning );
}

OMX_ERRORTYPE CExifMetadata::setIsoSpeedRatings(OMX_U32 aSensitivity)
{
   /*
    * EXIF SpeedRatings values
    * Indicates the ISO Speed and ISO Latitude of the camera or input device as specified in ISO 12232.
    * Exerpt of ISO 12232: "For some types of DSCs, such as those employing lossy compression methods,
    * it is not possible to correctly determine the noise-based ISO speed. In such cases, the ISO speed
    * of the camera is determined using the saturation based measurement, and the ISO speed latitude
    * values are not reported."
    */
   OMX_U16 ISOSpeedValue = (OMX_U16)aSensitivity;

   typedef struct {
      OMX_U16 min;
      OMX_U16 max;
   } SensitivityRange_t;

   // ISO values normalization array (ISO 12232)
   const SensitivityRange_t SensitivityRange[] =
   {
      { 8,    10   },
      { 10,   12   },
      { 12,   16   },
      { 16,   20   },
      { 20,   25   },
      { 25,   32   },
      { 32,   40   },
      { 40,   50   },
      { 50,   64   },
      { 64,   80   },
      { 80,   100  },
      { 100,  125  },
      { 125,  160  },
      { 160,  200  },
      { 200,  250  },
      { 250,  320  },
      { 320,  400  },
      { 400,  500  },
      { 500,  640  },
      { 640,  800  },
      { 800,  1000 },
      { 1000, 1250 },
      { 1250, 1600 },
      { 1600, 2000 },
      { 2000, 2500 },
      { 2500, 3200 },
      { 3200, 4000 },
      { 4000, 5000 },
      { 5000, 6400 },
      { 6400, 8000 }
   };

   for(unsigned int i=0; i<sizeof(SensitivityRange)/sizeof(SensitivityRange[0]); i++) {
      if((SensitivityRange[i].min < aSensitivity) && (aSensitivity <= SensitivityRange[i].max))
         ISOSpeedValue = SensitivityRange[i].max;
   }
   MSG1("Setting ISOSpeedRatings = %u\n", ISOSpeedValue);
   return iExifIFD.setFieldFromTag(IFDtag_ISOSpeedRatings, ISOSpeedValue);
}

OMX_ERRORTYPE CExifMetadata::setDateTimeOriginal(const IFM_DATE_TIME* pDateTime)
{
   char DateTimeString[20];
   snprintf(DateTimeString, sizeof(DateTimeString), "%.4d:%.2d:%.2d %.2d:%.2d:%.2d", pDateTime->year, pDateTime->month, pDateTime->day, pDateTime->hour, pDateTime->minutes, pDateTime->seconds);
   MSG1("Setting DateTimeOriginal = %s\n", DateTimeString);
   return iExifIFD.setFieldFromTag(IFDtag_DateTimeOriginal, DateTimeString, 20);
}

OMX_ERRORTYPE CExifMetadata::setDateTimeDigitized(const IFM_DATE_TIME* pDateTime)
{
   char DateTimeString[20];
   snprintf(DateTimeString, sizeof(DateTimeString), "%.4d:%.2d:%.2d %.2d:%.2d:%.2d", pDateTime->year, pDateTime->month, pDateTime->day, pDateTime->hour, pDateTime->minutes, pDateTime->seconds);
   MSG1("Setting DateTimeDigitized = %s\n", DateTimeString);
   return iExifIFD.setFieldFromTag(IFDtag_DateTimeDigitized, DateTimeString, 20);
}

OMX_ERRORTYPE CExifMetadata::setComponentsConfiguration(IFM_COMPONENTS_CONFIGURATION aComponentsConfig)
{
   /*
    * EXIF ComponentsConfiguration values
    * Information specific to compressed data. The channels of each component are arranged in order from the 1st
    * component to the 4th. For uncompressed data the data arrangement is given in the PhotometricInterpretation tag.
    * However, since PhotometricInterpretation can only express the order of Y,Cb and Cr, this tag is provided for cases
    * when compressed data uses components other than Y, Cb, and Cr and to enable support of other sequences.
    * Default = 4 5 6 0 (if RGB uncompressed)
    *           1 2 3 0 (other cases)
    *   0 = does not exist
    *   1 = Y
    *   2 = Cb
    *   3 = Cr
    *   4 = R
    *   5 = G
    *   6 = B
    *   Other = reserved
    */
   const exif_undefined_t CompConfigRGB[4] = { 4, 5, 6, 0 };
   const exif_undefined_t CompConfigYCbCr[4] = { 1, 2, 3, 0 };
   const exif_undefined_t* pCompConfig = CompConfigYCbCr;
   switch(aComponentsConfig)
   {
      case IFM_ComponentsConfiguration_RGB:   pCompConfig = CompConfigRGB; break;
      case IFM_ComponentsConfiguration_YCbCr: pCompConfig = CompConfigYCbCr; break;
      default:
        MSG1("Unreconized Components Configuration %d", aComponentsConfig);
        return OMX_ErrorBadParameter;
   }
   MSG4("Setting ComponentsConfiguration = '%c%c%c%c'\n", pCompConfig[0], pCompConfig[1], pCompConfig[2], pCompConfig[3] );
   return iExifIFD.setFieldFromTag(IFDtag_ComponentsConfiguration, pCompConfig, 4);
}

OMX_ERRORTYPE CExifMetadata::setPixelXDimension(OMX_U32 aPixelXDimension)
{
   /*
    * EXIF PixelXDimension values:
    * Information specific to compressed data. When a compressed file is recorded, the valid width
    * of the meaningful image shall be recorded in this tag, whether or not there is padding data
    * or a restart marker. This tag should not exist in an uncompressed file.
    * For details see section 2.8.1 and Annex F.
    */
   OMX_U16 xdim = (OMX_U16)aPixelXDimension;
   MSG1("Setting PixelXDimension = %u\n", xdim);
   return iExifIFD.setFieldFromTag(IFDtag_PixelXDimension, xdim);
}

OMX_ERRORTYPE CExifMetadata::setPixelYDimension(OMX_U32 aPixelYDimension)
{
   /*
    * EXIF PixelYDimension Values:
    * Information specific to compressed data. When a compressed file is recorded, the valid height
    * of the meaningful image shall be recorded in this tag, whether or not there is padding data
    * or a restart marker. This tag should not exist in an uncompressed file.
    * For details see section 2.8.1 and Annex F.
    * Since data padding is unnecessary in the vertical direction, the number of lines recorded
    * in this valid image height tag will in fact be the same as that recorded in the SOF
    */
   OMX_U16 ydim = (OMX_U16)aPixelYDimension;
   MSG1("Setting PixelYDimension = %u\n", ydim);
   return iExifIFD.setFieldFromTag(IFDtag_PixelYDimension, ydim);
}

OMX_ERRORTYPE CExifMetadata::setFocalLength(float aFocalLengthMm)
{
   /*
    * EXIF Focal length values:
    * The actual focal length of the lens, in mm
    */
   // Translate float into num/den (here 2 digits precision => 100 factor)
   exif_urational_t length;
   length.num = (OMX_U32)(100*aFocalLengthMm);
   length.den = 100;
   MSG3("Setting FocalLength = %lu/%lu (%f)\n", length.num, length.den, (float)length.num/(float)length.den);
   return iExifIFD.setFieldFromTag(IFDtag_FocalLength, &length );
}

OMX_ERRORTYPE CExifMetadata::setCustomRendered(OMX_BOOL bPictureEnhanced)
{
   /*
    * EXIF CustomRendered values
    * Default = 0
    * 0 = Normal process
    * 1 = Custom process
    * Other = reserved
    */
   OMX_U16 custom = bPictureEnhanced==OMX_TRUE ? 1 : 0;
   MSG1("Setting CustomRendered = %u\n", custom);
   return iExifIFD.setFieldFromTag(IFDtag_CustomRendered, custom );
}

OMX_ERRORTYPE CExifMetadata::setSceneCaptureType(OMX_SYMBIAN_SCENEMODETYPE aSceneCaptureType)
{
   /*
    * EXIF SceneCaptueType values
    * Default = 0
    * 0 = Standard
    * 1 = Landscape
    * 2 = Portrait
    * 3 = Night scene
    * Other = reserved
    */

   OMX_U16 type = 0; /* default: standard */
   switch((OMX_U32)aSceneCaptureType)
   {
      case OMX_SYMBIAN_SceneLandscape:     type = 1; break;
      case OMX_SYMBIAN_ScenePortrait:
      case OMX_SYMBIAN_SceneNightPortrait: type = 2; break;
      case OMX_STE_Scene_Candlelight:
      case OMX_STE_Scene_Sunset:
      case OMX_SYMBIAN_SceneNight:         type = 3; break;
      case OMX_SYMBIAN_SceneAuto:
      case OMX_SYMBIAN_SceneSport:
      case OMX_SYMBIAN_SceneMacro:
      case OMX_STE_Scene_Document:
      case OMX_STE_Scene_Beach:
      case OMX_STE_Scene_Snow:
      case OMX_STE_Scene_Party:
      case OMX_STE_Scene_Aqua:
      case OMX_STE_Scene_Backlight:        type = 0; break;
      default:
        MSG1("Unreconized Scene Capture Type %d", aSceneCaptureType);
        return OMX_ErrorBadParameter;
   }
   MSG1("Setting SceneCaptureType = %d\n", type);
   return iExifIFD.setFieldFromTag(IFDtag_SceneCaptureType, type );
}

OMX_ERRORTYPE CExifMetadata::setMeteringMode(OMX_METERINGTYPE aMeteringMode)
{
   /*
    * EXIF Metering mode values:
    * 0 = unknown
    * 1 = Average
    * 2 = CenterWeightedAverage
    * 3 = Spot
    * 4 = MultiSpot
    * 5 = Pattern
    * 6 = Partial
    * Other = reserved
    * 255 = other
    */

   OMX_U16 mode = 0; /* default: unknown */
   switch((OMX_U32)aMeteringMode)
   {
      case OMX_MeteringModeAverage: mode = 1; break;
      case OMX_STE_MeteringModeCenterWeighted: mode=2;break;
      case OMX_MeteringModeSpot:    mode = 3; break;
      case OMX_MeteringModeMatrix:  mode = 4; break;
      default:                      mode = 0; break;
   }
   MSG1("Setting MeteringMode = %d\n", mode);
   return iExifIFD.setFieldFromTag(IFDtag_MeteringMode, mode );
}

OMX_ERRORTYPE CExifMetadata::setContrast(OMX_S32 aContrast)
{
   /*
    * EXIF Contrast values:
    *  Default = 0
    *  0 = Normal
    *  1 = Soft
    *  2 = Hard
    *  Other = reserved
    */
   OMX_U16 contrast = 0;
   if ( aContrast == 0 ) {
      contrast = 0;
   }
   else if ( aContrast < 0) {
      contrast = 1;
   }
   else {
      contrast = 2;
   }
   MSG1("Setting Contrast = %d\n", contrast);
   return iExifIFD.setFieldFromTag(IFDtag_Contrast, contrast);
}

OMX_ERRORTYPE CExifMetadata::setLightSource(OMX_WHITEBALCONTROLTYPE aWhiteBalance)
{
   /*
    * EXIF LightSource values:
    * 0 = unknown
    * 1 = Daylight
    * 2 = Fluorescent
    * 3 = Tungsten (incandescent light)
    * 4 = Flash
    * 9 = Fine weather
    * 10 = Cloudy weather
    * 11 = Shade
    * 12 = Daylight fluorescent (D 5700  7100K)
    * 13 = Day white fluorescent (N 4600  5400K)
    * 14 = Cool white fluorescent (W 3900  4500K)
    * 15 = White fluorescent (WW 3200  3700K)
    * 17 = Standard light A
    * 18 = Standard light B
    * 19 = Standard light C
    * 20 = D55
    * 21 = D65
    * 22 = D75
    * 23 = D50
    * 24 = ISO studio tungsten
    * 255 = other light source
    * Other = reserved
    */
   OMX_U16 source = 0;
   switch(aWhiteBalance)
   {
      case OMX_WhiteBalControlSunLight:    source = 9;  break;
      case OMX_WhiteBalControlCloudy:      source = 10; break;
      case OMX_WhiteBalControlShade:       source = 11; break;
      case OMX_WhiteBalControlTungsten:    source = 3;  break;
      case OMX_WhiteBalControlFluorescent: source = 2; break;
      case OMX_WhiteBalControlIncandescent:source = 3;  break;
      case OMX_WhiteBalControlFlash:       source = 4;  break;
      case OMX_WhiteBalControlHorizon:     source = 1;  break;
      default:                             source = 0;  break;
   }
   MSG1("Setting LightSource = %d\n", source);
   return iExifIFD.setFieldFromTag(IFDtag_LightSource, source);
}

OMX_ERRORTYPE CExifMetadata::setWhiteBalance(OMX_WHITEBALCONTROLTYPE aWhiteBalance)
{
   OMX_U16 wb = 0; /* default: auto white balance */
   if (aWhiteBalance != OMX_WhiteBalControlAuto) {
      /* Manual white balance setting, or white balance disabled */
      wb = 1;
   }
   MSG1("Setting WhiteBalance = %d\n", wb);
   return iExifIFD.setFieldFromTag(IFDtag_WhiteBalance, wb);
}

OMX_ERRORTYPE CExifMetadata::setFlash( OMX_BOOL                   bFlashFired,
                                       OMX_IMAGE_FLASHCONTROLTYPE aFlashControl,
                                       OMX_BOOL                   bFlashPresent)
{
   /*
    * EXIF Flash values
    * This tag indicates the status of flash when the image was shot.
    *   Bit 0 indicates the flash firing status,
    *   Bits 1 and 2 indicate the flash return status,
    *   Bits 3 and 4 indicate the flash mode,
    *   Bit 5 indicates whether the flash function is present,
    *   Bit 6 indicates "red eye" mode.
    * Values for bit 0 indicating whether the flash fired.
    *   0b = Flash did not fire.
    *   1b = Flash fired.
    * Values for bits 1 and 2 indicating the status of returned light.
    *   00b = No strobe return detection function
    *   01b = reserved
    *   10b = Strobe return light not detected.
    *   11b = Strobe return light detected.
    * Values for bits 3 and 4 indicating the camera's flash mode.
    *   00b = unknown
    *   01b = Compulsory flash firing
    *   10b = Compulsory flash suppression
    *   11b = Auto mode
    * Values for bit 5 indicating the presence of a flash function.
    *   0b = Flash function present
    *   1b = No flash function
    * Values for bit 6 indicating the camera's red-eye mode.
    *   0b = No red-eye reduction mode or unknown
    *   1b = Red-eye reduction supported
    */

   OMX_U8 fired = bFlashFired == OMX_TRUE ? 1 : 0;
   OMX_U8 status = 0; /* Default: no strobe return detection function */
   OMX_U8 mode = 0; /* Default: unknown */
   switch(aFlashControl)
   {
     case OMX_IMAGE_FlashControlOn:   mode = 1; break;
     case OMX_IMAGE_FlashControlOff:  mode = 2; break;
     case OMX_IMAGE_FlashControlAuto: mode = 3; break;
     default:                         mode = 0; break;
   }
   OMX_U16 function = bFlashPresent == OMX_TRUE ? 0 : 1; // Caution: 0 means "flash present"
   OMX_U16 redeye = aFlashControl == OMX_IMAGE_FlashControlRedEyeReduction ? 1 : 0;
   OMX_U16 flash =   ((fired    & 0x01) << 0)
                   | ((status   & 0x03) << 1)
                   | ((mode     & 0x03) << 3)
                   | ((function & 0x01) << 5)
                   | ((redeye   & 0x01) << 6);
   MSG1("Setting Flash = %.2X\n", flash);
   return iExifIFD.setFieldFromTag(IFDtag_Flash, flash);
}

OMX_ERRORTYPE CExifMetadata::setDigitalZoomRatio(OMX_U32 nNumerator, OMX_U32 nDenominator)
{
   /*
    * EXIF DigitalZoomRatio values
    * This tag indicates the digital zoom ratio when the image was shot.
    * If the numerator of the recorded value is 0, this indicates that digital zoom was not used
    */
   exif_urational_t zoom;
   if(nNumerator == nDenominator) {
      // No digital zoom used => as per standard, num must be 0
      zoom.num = 0;
      zoom.den = nDenominator;
   }
   else {
      zoom.num = nNumerator;
      zoom.den = nDenominator;
   }
   MSG3("Setting DigitalZoomRatio = %lu/%lu (%f)\n", zoom.num, zoom.den, (float)zoom.num/(float)zoom.den);
   return iExifIFD.setFieldFromTag(IFDtag_DigitalZoomRatio, &zoom);
}

OMX_ERRORTYPE CExifMetadata::setExposureTime(OMX_U32 aExposureTimeUsec)
{
   /*
    * EXIF ExposureTime values
    * Exposure Time in seconds
    */
   exif_urational_t expotime;
   OMX_U32 seconds = aExposureTimeUsec / 1000000;
   OMX_U32 milliseconds = (aExposureTimeUsec % 1000000 ) / 1000;
   if( seconds > 0 ) {
      // Exposure time greater than 1s
      if( milliseconds <= 100) {
         // Exposure is smth like: 4.018s => set expotime as 4/1s
         expotime.num = seconds;
         expotime.den = 1;
      }
      else {
         // Exposure is smth like: 5.347s => set expotime as 53/10s
         expotime.num = 10 * seconds + ( milliseconds / 100 );
         expotime.den = 10;
      }
   }
   else {
      // Exposure time less than 1s => set expotime as 1/xyz seconds
      expotime.num = 1;
      expotime.den = 1000 / (milliseconds+1); // +1 prevents division by 0
   }
   MSG3("Setting ExposureTime = %lu/%lu (%f)\n", expotime.num, expotime.den, (float)expotime.num/(float)expotime.den);
   return iExifIFD.setFieldFromTag(IFDtag_ExposureTime, &expotime);
}

OMX_ERRORTYPE CExifMetadata::setFNumber(OMX_S32 xFNumber)
{
   /*
    * EXIF FNumber values
    * No guideline about how to format the data :/
    */
   exif_urational_t fnumber;
   fnumber.num = xFNumber;
   fnumber.den = 0x10000;
   MSG3("Setting FNumber = %lu/%lu (%f)\n", fnumber.num, fnumber.den, (float)fnumber.num/(float)fnumber.den);
   return iExifIFD.setFieldFromTag(IFDtag_FNumber, &fnumber);
}

OMX_ERRORTYPE CExifMetadata::setShutterSpeedValue(OMX_U32 aExposureTimeUsec)
{
   /*
    * EXIF ShutterSpeedValue values
    * The unit is the APEX unit (Additive System of Photographic Exposure).
    * ShutterSpeedValue = - log2 (exposure time)
    */
   float expotime = (float)aExposureTimeUsec/(float)1000000;
   exif_srational_t speed;
   speed.num = (OMX_S32)(-100*log2f(expotime)); // keep the integer part only (see APEX tables)
   speed.den = 100;
   MSG3("Setting ShutterSpeedValue = %ld/%ld (%f)\n", speed.num, speed.den, (float)speed.num/(float)speed.den);
   return iExifIFD.setFieldFromTag(IFDtag_ShutterSpeedValue, &speed);
}

OMX_ERRORTYPE CExifMetadata::setApertureValue(OMX_U32 xFNumber)
{
   /*
    * EXIF ApertureValue values
    * The lens aperture. The unit is the APEX value.
    * ApertureValue = 2 x log2(FNumber)
    */
   float fnumber = (float)xFNumber/(float)0x10000;
   exif_urational_t aperture;
   aperture.num = (OMX_U32)(2*log2f(fnumber)); // keep the integer part only (see APEX tables)
   aperture.den = 1;
   MSG3("Setting ApertureValue = %lu/%lu (%f)\n", aperture.num, aperture.den, (float)aperture.num/(float)aperture.den);
   return iExifIFD.setFieldFromTag(IFDtag_ApertureValue, &aperture);
}

OMX_ERRORTYPE CExifMetadata::setBrightnessValue(OMX_U32 aBrightness)
{
   /*
    * EXIF BrightnessValue values:
    * The value of brightness. The unit is the APEX value.
    * Ordinarily it is given in the range of -99.99 to 99.99.
    * Note that if the numerator of the recorded value is FFFFFFFF.H, Unknown shall be indicated
    */
   // Adjust brightness from [0..100] to [-99.99..+99.99] with 2 digits precision (100 factor)
   exif_srational_t brightness;
   brightness.num = (OMX_S32)(200*((OMX_S32)aBrightness- 50));
   brightness.den = (OMX_S32)100;
   MSG3("Setting BrightnessValue = %ld/%ld (%f)\n", brightness.num, brightness.den, (float)brightness.num/(float)brightness.den);
   return iExifIFD.setFieldFromTag(IFDtag_BrightnessValue, &brightness);
}

OMX_ERRORTYPE CExifMetadata::setSubjectDistance(OMX_U32 aSubjectDistance)
{
   /*
    * EXIF SubjectDistance values:
    * The distance to the subject, given in meters.
    * If the numerator of the recorded value is FFFFFFFF.H, Infinity shall be indicated;
    * And if the numerator is 0, Distance unknown shall be indicated.
    */
   exif_urational_t distance;
   distance.num = aSubjectDistance;
   distance.den = 1;
   MSG3("Setting SubjectDistance = %lu/%lu (%f)\n", distance.num, distance.den, (float)distance.num/(float)distance.den);
   return iExifIFD.setFieldFromTag(IFDtag_SubjectDistance, &distance);
}

OMX_ERRORTYPE CExifMetadata::setExposureIndex(OMX_S32 aEvCompensation)
{
   /*
    * EXIF ExposureIndex values:
    * Indicates the exposure index selected on the camera or input device at the time the image is captured.
    * Camera SAS says: ExposureIndex == EV Compensation
    */
   exif_urational_t index;
   index.num = (OMX_U32)((aEvCompensation > 0) ? (aEvCompensation) : (-aEvCompensation));
   index.den = 1;
   MSG3("Setting ExposureIndex = %lu/%lu (%f)\n", index.num, index.den, (float)index.num/(float)index.den);
   return iExifIFD.setFieldFromTag(IFDtag_ExposureIndex, &index);
}

OMX_ERRORTYPE CExifMetadata::setExposureMode(OMX_EXPOSURECONTROLTYPE aExposureControl, OMX_BOOL aBracketingEnabled)
{
   /*
    * EXIF ExposureMode values:
    * This tag indicates the exposure mode set when the image was shot. In auto-bracketing mode,
    * the camera shoots a series of frames of the same scene at different exposure settings.
    *  0 = Auto exposure
    *  1 = Manual exposure
    *  2 = Auto bracket
    *  Other = reserved
    */
   OMX_U16 mode = 0;
   if(aBracketingEnabled == OMX_FALSE) {
      // No bracketing
      switch(aExposureControl)
      {
         case OMX_ExposureControlAuto:
            mode = 0;
            break;
         case OMX_ExposureControlOff:
         case OMX_ExposureControlNight:
         case OMX_ExposureControlBackLight:
         case OMX_ExposureControlSpotLight:
         case OMX_ExposureControlSports:
         case OMX_ExposureControlSnow:
         case OMX_ExposureControlBeach:
         case OMX_ExposureControlLargeAperture:
         case OMX_ExposureControlSmallApperture:
         default:
            mode = 1;
            break;
      }
   }
   else {
       // Bracketing enabled
       mode = 2;
   }
   MSG1("Setting ExposureMode = %d\n", mode);
   return iExifIFD.setFieldFromTag(IFDtag_ExposureMode, mode);
}

OMX_ERRORTYPE CExifMetadata::setGainControl(OMX_S32 xAnalogGain, OMX_S32 xDigitalGain)
{
   /*
    * EXIF GainControl values:
    * This tag indicates the degree of overall image gain adjustment.
    *  0 = None
    *  1 = Low gain up
    *  2 = High gain up
    *  3 = Low gain down
    *  4 = High gain down
   */
   if(xAnalogGain < 0 || xDigitalGain < 0) {
      // Cannot be converted into Db
      MSG2("Invalid argument: aAnalogGain=%ld, aDigitalGain=%ld\n", xAnalogGain, xDigitalGain);
      return OMX_ErrorBadParameter;
   }
   /*
    * Here is the formula applied:
    *  AG=-0.10.1dB and DG=0dB   "none"
    *  AG<-0.1dB and DG=0dB       "low gain down"
    *  AG+DG>6.2dB                "high gain up"
    *  other                      "low gain up"
    */
   OMX_U16 gaincontrol  = 0;
   float aAnalogGainDb  = 20 * (float)log((double)xAnalogGain / (float)0x10000);
   float aDigitalGainDb = 20 * (float)log((double)xDigitalGain / (float)0x10000);

   if( (aAnalogGainDb >= -0.1 && aAnalogGainDb <= 0.1) && aDigitalGainDb == 0) {
      gaincontrol = 0; // none
   }
   else if( aAnalogGainDb < -0.1 && aDigitalGainDb == 0) {
      gaincontrol = 3; // low gain down
   }
   else if( aAnalogGainDb+aDigitalGainDb > 6.2) {
      gaincontrol = 2; // high gain up
   }
   else {
      gaincontrol = 1; // low gain up
   }
   MSG1("Setting GainControl = %d\n", gaincontrol);
   return iExifIFD.setFieldFromTag(IFDtag_GainControl, gaincontrol);
}


OMX_ERRORTYPE CExifMetadata::setDateTime(const IFM_DATE_TIME* pDateTime)
{
	char DateTimeString[20];
	snprintf(DateTimeString, sizeof(DateTimeString), "%.4d:%.2d:%.2d %.2d:%.2d:%.2d", pDateTime->year, pDateTime->month, pDateTime->day, pDateTime->hour, pDateTime->minutes, pDateTime->seconds);
	MSG1("Setting DateTime = %s\n", DateTimeString);
	return iMainIFD.setFieldFromTag(IFDtag_DateTime, DateTimeString, 20);
}
OMX_ERRORTYPE CExifMetadata::setSoftware(const char* aSoftware)
{
	MSG1("Setting Software = '%s'\n", aSoftware);
    return iMainIFD.setFieldFromTag(IFDtag_Software, aSoftware, strnlen(aSoftware,127)+1);
}
OMX_ERRORTYPE CExifMetadata::setExposureBias(OMX_S32 exposure_b)
{
	exif_srational_t exposure;
       exposure.num = (OMX_S32)(3*exposure_b/65536); // keep the integer part only (see APEX tables)
       exposure.den = 3;
       MSG3("Setting ExposureBias = %lu/%lu (%f)\n", exposure.num, exposure.den, (float)exposure.num/(float)exposure.den);
       return iExifIFD.setFieldFromTag(IFDtag_ExposureBiasValue, &exposure);

}
OMX_ERRORTYPE CExifMetadata::setInteroperabilityIFDPointer(OMX_U32 addr)
{
   MSG1("Setting Interoperability IFD Pointer = %lu\n",addr);
   return iExifIFD.setFieldFromTag(IFDtag_InteroperabilityIFDPointer, (OMX_U32)(addr));
/*TODO:Define the address for Interoperability IFD pointer*/
}

OMX_ERRORTYPE CExifMetadata::setInterOperabilityIndex(const char* aIndex)
{
       MSG1("Setting InterOperability Index = %s\n",aIndex);
      return iInterOpIFD.setFieldFromTag(IFDtag_Index, aIndex, strnlen(aIndex,32)+1);
}

OMX_ERRORTYPE CExifMetadata::setInterOperabilityVersion()
{

    char version[]="0100";
   const exif_undefined_t versionconfig[] = "0100";
   MSG4("Setting Interoperability version= '%c%c%c%c'\n", versionconfig[0], versionconfig[1], versionconfig[2],version[3] );
   return iInterOpIFD.setFieldFromTag(IFDtag_Version, versionconfig, 4);
}

OMX_ERRORTYPE CExifMetadata::setSubjectDistanceRange(OMX_U32 aSubjectDistance)
{
       OMX_U16 subject_distance_range = 0;

       /*TODO:subject distance range needs to be defined as a function of subject distance*/
	/*if(aSubjectDistance > 0)
		subject_distance_range = 0;
	else
		subject_distance_range = 1;
	*/

	MSG1("Setting Distance Range = '%d'\n", subject_distance_range);
       return iExifIFD.setFieldFromTag(IFDtag_SubjectDistanceRange, subject_distance_range);
}

OMX_ERRORTYPE CExifMetadata::setSubjectArea(OMX_SYMBIAN_RECTTYPE sROI)
{
   /*
    * EXIF SubjectArea values:
    * This tag indicates the location and area of the main subject in the overall scene.
    *   Count = 2 or 3 or 4
    *   Default = none
    * The subject location and area are defined by Count values as follows.
    * Count = 2 Indicates the location of the main subject as coordinates.
    *           The first value is the X coordinate and the second is the Y coordinate.
    * Count = 3 The area of the main subject is given as a circle. The circular area is expressed
    *           as center coordinates and diameter. The first value is the center X coordinate,
    *           the second is the center Y coordinate, and the third is the diameter.
    * Count = 4 The area of the main subject is given as a rectangle. The rectangular area is expressed
    *           as center coordinates and area dimensions. The first value is the center X coordinate,
    *           the second is the center Y coordinate, the third is the width of the area, and the fourth
    *           is the height of the area.
    * Note that the coordinate values, width, and height are expressed in relation to the upper left as
    * origin, prior to rotation processing as per the Rotation tag.
    */
   OMX_U16 area[4];
   area[0] = (OMX_U16) sROI.sTopLeft.nX + ( sROI.sSize.nWidth / 2 );
   area[1] = (OMX_U16) sROI.sTopLeft.nY + ( sROI.sSize.nHeight / 2 );
   area[2] = (OMX_U16) sROI.sSize.nWidth;
   area[3] = (OMX_U16) sROI.sSize.nHeight;
   MSG4("Setting SubjectArea = [ %d %d %d %d ]\n", area[0], area[1], area[2], area[3]);
   return iExifIFD.setFieldFromTag(IFDtag_SubjectArea, area, sizeof(area)/sizeof(area[0]));
}

#if (IMG_CONFIG_EXIF_MAKER_NOTES == IMG_CONFIG_TRUE)
OMX_ERRORTYPE CExifMetadata::setMakerNote(const void* aMakerNote, int aSize)
{
   /*
    * EXIF Makernote values:
    * A tag for manufacturers of Exif writers to record any desired information. The contents are up
    * to the manufacturer, but this tag should not be used for any other than its intended purpose.
    */
    if( aSize!=0 && aMakerNote==NULL ) {
        MSG2("Invalid MakerNote argument: aMakerNote=%p, aSize=%d\n", aMakerNote, aSize);
        return OMX_ErrorBadParameter;
    }
    else if( aSize==0 && aMakerNote==NULL ) {
        MSG0("aSize==0 && aMakerNote==NULL => no maker notes added to EXIF\n");
        return OMX_ErrorNone;
    }
   exif_undefined_t* pMakerNote = (exif_undefined_t*)aMakerNote;
   MSG2("Setting MakerNote = [ pMakerNote=%p, size=%d ]\n", pMakerNote, aSize);
   return iExifIFD.setFieldFromTag(IFDtag_MakerNote, pMakerNote, aSize);
}
#endif

OMX_ERRORTYPE CExifMetadata::setColorSpace(IFM_COLOR_SPACE aColorSpace)
{
   /*
    * EXIF ColorSpace values:
    * The color space information tag (ColorSpace) is always recorded as the color space specifier.
    * Normally sRGB (=1) is used to define the color space based on the PC monitor conditions and environment.
    * If a color space other than sRGB is used, Uncalibrated (=FFFF.H) is set. Image data recorded as
    *  Uncalibrated can be treated as sRGB when it is converted to Flashpix. On sRGB see Annex E.
    *  1 = sRGB
    *  FFFF.H = Uncalibrated
    *  Other = reserved.
    */
    OMX_U16 colorspace = 0xFFFF;
    switch(aColorSpace)
    {
       case IFM_ColorSpace_sRGB:         colorspace = 1; break;
       case IFM_ColorSpace_Uncalibrated: colorspace = 0xFFFF; break;
       default:
         MSG1("Unreconized Color Space %d", aColorSpace);
         return OMX_ErrorBadParameter;
    }
   MSG1("Setting ColorSpace = 0x%.4X\n", colorspace);
   return iExifIFD.setFieldFromTag(IFDtag_ColorSpace, colorspace);
}

OMX_ERRORTYPE CExifMetadata::initialize(const IFM_EXIF_SETTINGS* pExifSettings)
{
    IN0("\n");

    // Sanity check
    if(pExifSettings == NULL) {
        MSG0("Invalid field entry: pExifSettings==NULL\n");
        OUTR(" ", OMX_ErrorBadParameter);
        return OMX_ErrorBadParameter;
    }

    // Set all EXIF fields
    int errors = 0;
    errors += OMX_ErrorNone != setMake(pExifSettings->pManufacturer);
    errors += OMX_ErrorNone != setModel(pExifSettings->pModel);
    errors += OMX_ErrorNone != setOrientation(pExifSettings->eOrientation);
    errors += OMX_ErrorNone != setXResolution(pExifSettings->nXResolution);
    errors += OMX_ErrorNone != setYResolution(pExifSettings->nYResolution);
    errors += OMX_ErrorNone != setResolutionUnit(pExifSettings->eResolutionUnit);
    errors += OMX_ErrorNone != setSoftware(pExifSettings->pSoftware);
    errors += OMX_ErrorNone != setDateTime(&pExifSettings->sDateTime);
    errors += OMX_ErrorNone != setYCbCrPositioning(pExifSettings->eYCbCrPositioning);
    errors += OMX_ErrorNone != setExposureTime(pExifSettings->nExposureTimeUsec);
    errors += OMX_ErrorNone != setFNumber(pExifSettings->xFNumber);
    errors += OMX_ErrorNone != setIsoSpeedRatings(pExifSettings->nSensitivity);
    errors += OMX_ErrorNone != setDateTimeOriginal(&pExifSettings->sDateTime);
    errors += OMX_ErrorNone != setDateTimeDigitized(&pExifSettings->sDateTime);
    errors += OMX_ErrorNone != setComponentsConfiguration(pExifSettings->eComponentsConfig);
    errors += OMX_ErrorNone != setShutterSpeedValue(pExifSettings->nExposureTimeUsec);

#ifndef CUSTOMER_EXIF
    errors += OMX_ErrorNone != setApertureValue(pExifSettings->xFNumber);
    errors += OMX_ErrorNone != setBrightnessValue(pExifSettings->nBrightness);
    errors += OMX_ErrorNone != setSubjectDistance(pExifSettings->nSubjectDistance);
    errors += OMX_ErrorNone != setGainControl(pExifSettings->xAnalogGain, pExifSettings->xDigitalGain);
    errors += OMX_ErrorNone != setContrast(pExifSettings->nContrast);
    errors += OMX_ErrorNone != setSubjectArea(pExifSettings->sAutoFocusROI);
#endif

    errors += OMX_ErrorNone != setMeteringMode(pExifSettings->eMeteringMode);
    errors += OMX_ErrorNone != setLightSource(pExifSettings->eWhiteBalanceControl);
    errors += OMX_ErrorNone != setFlash(pExifSettings->bFlashFired, pExifSettings->eFlashControl, pExifSettings->bFlashPresent);
    errors += OMX_ErrorNone != setFocalLength(pExifSettings->fFocalLength);

#if (IMG_CONFIG_EXIF_MAKER_NOTES == IMG_CONFIG_TRUE)
    errors += OMX_ErrorNone != setMakerNote(pExifSettings->pMakerNote, pExifSettings->nMakerNoteSize);
#endif
    errors += OMX_ErrorNone != setColorSpace(pExifSettings->eColorSpace);
    errors += OMX_ErrorNone != setPixelXDimension(pExifSettings->nPictureNumPixelsX);
    errors += OMX_ErrorNone != setPixelYDimension(pExifSettings->nPictureNumPixelsY);
#ifndef CUSTOMER_EXIF
    errors += OMX_ErrorNone != setExposureIndex(pExifSettings->nEvCompensation);
#endif
    errors += OMX_ErrorNone != setCustomRendered(pExifSettings->bPictureEnhanced);
    errors += OMX_ErrorNone != setExposureMode(pExifSettings->eExposureControl, pExifSettings->bBracketingEnabled);
    errors += OMX_ErrorNone != setWhiteBalance(pExifSettings->eWhiteBalanceControl);
    errors += OMX_ErrorNone != setDigitalZoomRatio(pExifSettings->nZoomFactor, 100);		//nZoomFactor is scaled up by a factor of 100, to meet customer requirement
    errors += OMX_ErrorNone != setSceneCaptureType(pExifSettings->eSceneType);
    
    errors += OMX_ErrorNone != setExposureBias(pExifSettings->nEvCompensation);
    errors += OMX_ErrorNone != setSubjectDistanceRange(pExifSettings->nSubjectDistance);

    errors += OMX_ErrorNone != setInteroperabilityIFDPointer(iTiffHeader.size()+iMainIFD.size()+iExifIFD.size());
    errors += OMX_ErrorNone != setInterOperabilityIndex("R98");
    errors += OMX_ErrorNone != setInterOperabilityVersion();

	if(errors!=0) {
        /* Some exif fields could not be set successfully */
        MSG0("Some fields could not be set successfully\n");
        return OMX_ErrorUndefined;
    }

    // Done
    return OMX_ErrorNone;
}

OMX_ERRORTYPE CExifMetadata::serialize(const IFM_EXIF_SETTINGS* pExifSettings, OMX_U8* pBuffer, const OMX_U32 aSize)
{
   IN0("\n");

   /* Sanity check */
   if( pBuffer == NULL || aSize == 0) {
       MSG2("Invalid argument: pBuffer==%p, aSize=%lu\n", pBuffer, aSize);
       OUTR(" ",OMX_ErrorBadParameter);
       return OMX_ErrorBadParameter;
   }

   /* Check that the whole APP1 section fits into the provided buffer */
   if( size() > aSize ) {
       MSG2("Buffer too small: APP1 size = %lu > buffer size = %lu\n", size(), aSize);
       OUTR(" ",OMX_ErrorBadParameter);
       return OMX_ErrorBadParameter;
   }

   /* Initialize all data with exif settings */
   OMX_ERRORTYPE err = OMX_ErrorNone;
   err = initialize(pExifSettings);
   if ( err != OMX_ErrorNone) {
       MSG1("Failed to initialize EXIF metadata: err=%d\n", err);
       return err;
   }

   /* Check that all fields were initialized */
   if(iMainIFD.initialized()!=true) {
       MSG0("Main IFD has uninitialized fields\n");
       OUTR(" ",OMX_ErrorNotReady);
       return OMX_ErrorNotReady;
   }
   if(iExifIFD.initialized()!=true) {
       MSG0("Exif IFD has uninitialized fields\n");
       OUTR(" ",OMX_ErrorNotReady);
       return OMX_ErrorNotReady;
   }

    if(iInterOpIFD.initialized()!=true) {
       MSG0("Interop IFD has uninitialized fields\n");
       OUTR(" ",OMX_ErrorNotReady);
       return OMX_ErrorNotReady;
   }

   /* Compute address of each section */
   OMX_U8* pApp1HeaderSection = pBuffer;
   OMX_U8* pTiffHeaderSection = pApp1HeaderSection + iApp1Header.size();
   OMX_U8* pMainIfdSection    = pTiffHeaderSection + iTiffHeader.size();
   OMX_U8* pExifIfdSection    = pMainIfdSection    + iMainIFD.size();
   OMX_U8* pInterOpIfdSection = pExifIfdSection    + iExifIFD.size();
   OMX_U8* pGpsInfoIfdSection = pInterOpIfdSection    + iInterOpIFD.size();
  //OMX_U8* pGpsInfoIfdSection = pExifIfdSection    + iExifIFD.size();

   MSG1("pBuffer                  = %p\n", pBuffer);
   MSG1("EXIF metadata total size = %ld bytes\n", size());
   MSG1("APP1 header size         = %ld bytes\n", iApp1Header.size());
   MSG1("TIFF header size         = %ld bytes\n", iTiffHeader.size());
   MSG1("Main IFD size            = %ld bytes\n", iMainIFD.size());
   MSG1("Exif IFD size            = %ld bytes\n", iExifIFD.size());
   MSG1("Interoperability IFD size            = %ld bytes\n", iInterOpIFD.size());

   /* Serialize the App1 Header */
   /* Note: App1 lenght == size of whole APP1 metadata section */
   MSG0("serialize app1 header\n");
   iApp1Header.setLenght((OMX_U16)size());
   iApp1Header.serialize(pApp1HeaderSection);

   /* Serialize the TIFF Header */
   /* Note: IFD follows immediately the TIFF header => IFD offset == header size */
   MSG0("serialize TIFF header\n");
   iTiffHeader.setIfdOffset(iTiffHeader.size());
   iTiffHeader.serialize(pTiffHeaderSection);

   /* Serialize the Main IFD */
   MSG0("serialize Main IFD\n");

#if 0
   /*This next code block is to ensure that the first IFD starts at a 4-byte aligned address*/
   if (((OMX_U32)pExifIfdSection + 2)%4 != 0)
   	{
   		OMX_U32 uiAlignOffset;
		uiAlignOffset = 4 - (OMX_U32)((pExifIfdSection + 2))%4;
		pExifIfdSection += uiAlignOffset;
   	}
#endif //0

   /* Note: ExifIfdPointer is the offset of EXIF Ifd section starting from TIFF header */
   iMainIFD.setFieldFromTag(IFDtag_ExifIfdPointer, (OMX_U32)(pExifIfdSection-pTiffHeaderSection));
   /* Note: We set a valid GpsInfoIfdPointer value so that GPS data can be added correctly   */
   /* further in the OMX data flow. If GPS data are not available then the number of entries */
   /* in the GPS IFD will set to zero.                                                       */
   /* Note: GpsInfoIfdPointer is the offset of GPS Ifd section starting from TIFF header     */
   iMainIFD.setFieldFromTag(IFDtag_GpsInfoIfdPointer, (OMX_U32)(pGpsInfoIfdSection-pTiffHeaderSection));
   /* Note: second argument is the offset of Main IFD from Tiff header */
   iMainIFD.serialize(pMainIfdSection, (OMX_U32)(pMainIfdSection-pTiffHeaderSection));

   /* Serialize the Exif IFD */
   MSG0("serialize Exif IFD\n");
   /* Note: second argument is the offset of Exif IFD from Tiff header */
   iExifIFD.serialize(pExifIfdSection, (OMX_U32)(pExifIfdSection-pTiffHeaderSection));
   /* Note: second argument is the offset of Interoperability IFD from Tiff header */
   iInterOpIFD.serialize(pInterOpIfdSection, (OMX_U32)(pInterOpIfdSection-pTiffHeaderSection));

   /* Done */
   OUTR(" ",OMX_ErrorNone);
   return OMX_ErrorNone;
}

OMX_ERRORTYPE CExifMetadata::deserialize(IFM_EXIF_SETTINGS* pExifSettings, const OMX_U8* pBuffer, const OMX_U32 aSize)
{
   IN0("\n");
#if 1
   /* +CR370700 */
   /* Done */
   ExifReader Reader;
   size_t maxsize=0;
   ExifPtrDataValue Ptr;

   int result= Reader.AttachBuffer((char*)pBuffer, aSize);
   if (result==OMX_ErrorNone)
   {
       ExifTag *pTag;

   /* *************** Get all tags from IFD0 *************** */

       // TAG Make >> pManufacturer
       pTag=Reader.FindTag(eExifTag_Make);
       if(pTag!= NULL){
           pTag->GetDataAddress(Reader.GetExifBase(), Ptr, maxsize);
           pExifSettings->pManufacturer = (const char*)Ptr.cValue;
       }else{
           MSG0("TAG Make is not present in extradata ");
       }

       // TAG Model >> pModel
       pTag=Reader.FindTag(eExifTag_Model);
       if(pTag!= NULL){
           pTag->GetDataAddress(Reader.GetExifBase(), Ptr, maxsize);
           pExifSettings->pModel = (const char*)Ptr.cValue;
       }else{
           MSG0("TAG Model is not present in extradata ");
       }
       // TAG Orientation >> eOrientation
       pTag=Reader.FindTag(eExifTag_Orientation);
       if(pTag!= NULL){
           pTag->GetDataAddress(Reader.GetExifBase(), Ptr, maxsize);
           pExifSettings->eOrientation = (OMX_SYMBIAN_ORIENTATIONORIGINTYPE)*Ptr.usValue;
       }else{
           MSG0("TAG Orientation is not present in extradata ");
       }

       // TAG XResolution >> nXResolution
       pTag=Reader.FindTag(eExifTag_XResolution);
       if(pTag!= NULL){
           exif_urational_t xres;
           pTag->GetDataAddress(Reader.GetExifBase(), Ptr, maxsize);
           xres.num = *Ptr.uiValue;
           xres.den = *(Ptr.uiValue + 1);
           if(xres.den){
               pExifSettings->nXResolution = (OMX_U32)(xres.num / xres.den) ;
           }else{
               MSG0("Error reading XResolution value");
               pExifSettings->nXResolution = 0;
           }
       }else{
           MSG0("TAG XResolution is not present in extradata ");
       }

       // TAG YResolution >> nYResolution
       pTag=Reader.FindTag(eExifTag_YResolution);
       if(pTag!= NULL){
           exif_urational_t yres;
           pTag->GetDataAddress(Reader.GetExifBase(), Ptr, maxsize);
           yres.num = *Ptr.uiValue;
           yres.den = *(Ptr.uiValue + 1);
           if(yres.den){
               pExifSettings->nYResolution = (OMX_U32)(yres.num / yres.den) ;
           }else{
               MSG0("Error reading YResolution value");
               pExifSettings->nYResolution = 0;
           }
       }else{
           MSG0("TAG YResolution is not present in extradata ");
       }

       // TAG Resolution Unit >> eResolutionUnit
       pTag=Reader.FindTag(eExifTag_ResolutionUnit);
       if(pTag!= NULL){
           pTag->GetDataAddress(Reader.GetExifBase(), Ptr, maxsize);
           pExifSettings->eResolutionUnit = (IFM_RESOLUTION_UNIT)*Ptr.usValue;
       }else{
           MSG0("TAG Resolution Unit is not present in extradata ");
       }

       // TAG Software >> pSoftware
       pTag=Reader.FindTag(eExifTag_Software);
       if(pTag!= NULL){
           pTag->GetDataAddress(Reader.GetExifBase(), Ptr, maxsize);
           pExifSettings->pSoftware = (const char*)Ptr.cValue;
       }else{
           MSG0("TAG Software is not present in extradata ");
       }

       // TAG YCbCr Positioning >> eYCbCrPositioning
       pTag=Reader.FindTag(eExifTag_YCbCrPositioning);
       if(pTag!= NULL){
           pTag->GetDataAddress(Reader.GetExifBase(), Ptr, maxsize);
           pExifSettings->eYCbCrPositioning = (IFM_YCbCr_POSITIONING)*Ptr.usValue;
       }else{
           MSG0("TAG YCbCr Positioning is not present in extradata ");
       }


   /* *************** Get all tags from ExifSubIFD *************** */

       // TAG Date and Time >> sDateTime
       pTag=Reader.FindTag(eExifTag_DateTimeOriginal);
       if(pTag!= NULL){
           pTag->GetDataAddress(Reader.GetExifBase(), Ptr, maxsize);
           pExifSettings->sDateTime.year = (OMX_U16)atoi((const char*)Ptr.uiValue);
           pExifSettings->sDateTime.month = (OMX_U8)atoi((const char*)(Ptr.ucValue + 5));
           pExifSettings->sDateTime.day = (OMX_U8)atoi((const char*)(Ptr.usValue + 4));
           pExifSettings->sDateTime.hour = (OMX_U8)atoi((const char*)(Ptr.ucValue + 11));
           pExifSettings->sDateTime.minutes = (OMX_U8)atoi((const char*)(Ptr.usValue + 7));
           pExifSettings->sDateTime.seconds = (OMX_U8)atoi((const char*)(Ptr.ucValue + 17));

       }else{
           MSG0("TAG Date and Time is not present in extradata ");
       }

       // TAG ISO Speed Ratings >> nSensitivity
       pTag=Reader.FindTag(eExifTag_ISOSpeedRatings);
       if(pTag!= NULL){
           pTag->GetDataAddress(Reader.GetExifBase(), Ptr, maxsize);
           pExifSettings->nSensitivity = (OMX_U32)*Ptr.usValue;
       }else{
           MSG0("TAG ISO Speed Ratings is not present in extradata ");
       }

       // TAG Components Configuration >> eComponentsConfig
       pTag=Reader.FindTag(eExifTag_ComponentsConfiguration);
       if(pTag!= NULL){
           OMX_U32 YCbCrConfig = (0<<24)+(3<<16)+(2<<8)+(1<<0);; /* YCbCr config 1230 */
           OMX_U32 RGBConfig =(0<<24)+(6<<16)+(5<<8)+(4<<0);; /* RGB config 4560 */

           pTag->GetDataAddress(Reader.GetExifBase(), Ptr, maxsize);
           if(*Ptr.uiValue == RGBConfig){
               pExifSettings->eComponentsConfig = IFM_ComponentsConfiguration_RGB;
           }else if(*Ptr.uiValue == YCbCrConfig){
               pExifSettings->eComponentsConfig = IFM_ComponentsConfiguration_YCbCr;
           }else{
               MSG0("Error reading Components Configuration value");
           }
       }else{
           MSG0("TAG Components Configuration is not present in extradata ");
       }

       // TAG Exif Image Width >> nPictureNumPixelsX
       pTag=Reader.FindTag(eExifTag_PixelXDimension);
       if(pTag!= NULL){
           pTag->GetDataAddress(Reader.GetExifBase(), Ptr, maxsize);
           pExifSettings->nPictureNumPixelsX = (OMX_U32)*Ptr.usValue;
       }else{
           MSG0("TAG Exif Image Width is not present in extradata ");
       }

       // TAG Exif Image Height >> nPictureNumPixelsY
       pTag=Reader.FindTag(eExifTag_PixelYDimension);
       if(pTag!= NULL){
           pTag->GetDataAddress(Reader.GetExifBase(), Ptr, maxsize);
           pExifSettings->nPictureNumPixelsY = (OMX_U32)*Ptr.usValue;
       }else{
           MSG0("TAG Exif Image Height is not present in extradata ");
       }

       // TAG Light Source >> eWhiteBalanceControl
       pTag=Reader.FindTag(eExifTag_LightSource);
       if(pTag!= NULL){
           OMX_WHITEBALCONTROLTYPE   nWhiteBalanceControl;

           pTag->GetDataAddress(Reader.GetExifBase(), Ptr, maxsize);

           switch(*Ptr.usValue)
           {
                case 1:   nWhiteBalanceControl = OMX_WhiteBalControlHorizon;  break;
                case 2:   nWhiteBalanceControl = OMX_WhiteBalControlFluorescent; break;
                case 3:   nWhiteBalanceControl = OMX_WhiteBalControlTungsten; break;
                case 4:   nWhiteBalanceControl = OMX_WhiteBalControlFlash;    break;
                case 9:   nWhiteBalanceControl = OMX_WhiteBalControlSunLight; break;
                case 10:  nWhiteBalanceControl = OMX_WhiteBalControlCloudy;   break;
                case 11:  nWhiteBalanceControl = OMX_WhiteBalControlShade;    break;
                default:  nWhiteBalanceControl = OMX_WhiteBalControlAuto;     break;
           }
           pExifSettings->eWhiteBalanceControl = nWhiteBalanceControl;
       }else{
           MSG0("TAG Light Source is not present in extradata ");
       }

       // TAG Focal Length >> fFocalLength
       pTag=Reader.FindTag(eExifTag_FocalLength);
       if(pTag!= NULL){
           exif_urational_t focallength;
           pTag->GetDataAddress(Reader.GetExifBase(), Ptr, maxsize);
           focallength.num = *Ptr.uiValue;
           focallength.den = *(Ptr.uiValue + 1);
           if(focallength.den){
               pExifSettings->fFocalLength = ((float)focallength.num / (float)focallength.den) ;
           }else{
               MSG0("Error reading Focal Length value");
               pExifSettings->fFocalLength = 0;
           }
       }else{
           MSG0("TAG Focal Length is not present in extradata ");
       }

       // TAG Custom Rendered >> bPictureEnhanced
       pTag=Reader.FindTag(eExifTag_CustomRendered);
       if(pTag!= NULL){
           pTag->GetDataAddress(Reader.GetExifBase(), Ptr, maxsize);
           pExifSettings->bPictureEnhanced = (OMX_BOOL)*Ptr.ucValue;
       }else{
           MSG0("TAG Custom Rendered is not present in extradata ");
       }

       // TAG Scene Capture type >> eSceneType
       pTag=Reader.FindTag(eExifTag_SceneCaptureType);
       if(pTag!= NULL){
           OMX_SYMBIAN_SCENEMODETYPE scenemode_type = OMX_SYMBIAN_SceneAuto;
           pTag->GetDataAddress(Reader.GetExifBase(), Ptr, maxsize);

           switch(*Ptr.usValue)
           {
                case 0:  scenemode_type = OMX_SYMBIAN_SceneAuto; break;
                case 1:  scenemode_type = OMX_SYMBIAN_SceneLandscape; break;
                case 2:  scenemode_type = OMX_SYMBIAN_ScenePortrait; break;
                case 3:  scenemode_type = OMX_SYMBIAN_SceneNight; break;
                default:
                    MSG1("Unrecognized Scene Capture Type %d", *Ptr.usValue); break;
           }

           pExifSettings->eSceneType = scenemode_type;
       }else{
           MSG0("TAG Scene Capture type is not present in extradata ");
       }

       // TAG Metering Mode >> eMeteringMode
       pTag=Reader.FindTag(eExifTag_MeteringMode);
       if(pTag!= NULL){
           OMX_METERINGTYPE metering_type = OMX_MeteringModeAverage;
           pTag->GetDataAddress(Reader.GetExifBase(), Ptr, maxsize);

           switch(*Ptr.usValue)
           {
                case 1:  metering_type = OMX_MeteringModeAverage; break;
                case 2:  metering_type = (OMX_METERINGTYPE)OMX_STE_MeteringModeCenterWeighted; break;
                case 3:  metering_type = OMX_MeteringModeSpot; break;
                case 4:  metering_type = OMX_MeteringModeMatrix; break;
                default:
                    MSG1("Unrecognized Metering Mode %d", *Ptr.usValue); break;
           }

           pExifSettings->eMeteringMode = metering_type;
       }else{
           MSG0("TAG Metering Mode type is not present in extradata ");
       }

       // TAG Flash >> bFlashFired, eFlashControl, bFlashPresent
       pTag=Reader.FindTag(eExifTag_Flash);
       if(pTag!= NULL){
           OMX_IMAGE_FLASHCONTROLTYPE flash_control = OMX_IMAGE_FlashControlOn;

           pTag->GetDataAddress(Reader.GetExifBase(), Ptr, maxsize);

           switch((*Ptr.usValue & 0x0018) >> 3)
           {
                case 1: flash_control = OMX_IMAGE_FlashControlOn; break;
                case 2: flash_control = OMX_IMAGE_FlashControlOff; break;
                case 3: flash_control = OMX_IMAGE_FlashControlAuto; break;
                default:
                    MSG1("Unrecognized Flash Control %d", ((*Ptr.usValue & 0x0018) >> 3)); break;
           }

           pExifSettings->bFlashFired          = (*Ptr.usValue & 0x0001) == 0 ? OMX_FALSE : OMX_TRUE;
           pExifSettings->eFlashControl        = flash_control;
           pExifSettings->bFlashPresent        = ((*Ptr.usValue & 0x0020) >> 5) == 0 ? OMX_TRUE : OMX_FALSE; // Caution: 0 means "flash present"
       }else{
           MSG0("TAG Flash is not present in extradata ");
       }

       // TAG Digital Zoom Ratio >> nMaxNumPixelsX
       pTag=Reader.FindTag(eExifTag_DigitalZoomRatio);
       if(pTag!= NULL){
           exif_urational_t digital_zoom;
           pTag->GetDataAddress(Reader.GetExifBase(), Ptr, maxsize);
           digital_zoom.num = *Ptr.uiValue;
           digital_zoom.den = *(Ptr.uiValue + 1);
           if(digital_zoom.den){
               if(digital_zoom.num == 0){
                   pExifSettings->nMaxNumPixelsX = pExifSettings->nPictureNumPixelsX;
               }else{
                   pExifSettings->nMaxNumPixelsX =(pExifSettings->nPictureNumPixelsX * digital_zoom.num / digital_zoom.den) ;
               }
           }else{
               MSG0("Error reading Digital Zoom Ratio value");
               pExifSettings->nMaxNumPixelsX = 0;
           }
       }else{
           MSG0("TAG Digital Zoom Ratio is not present in extradata ");
       }

       // TAG Exposure Time >> nExposureTimeUsec
       pTag=Reader.FindTag(eExifTag_ExposureTime);
       if(pTag!= NULL){
           exif_urational_t exposure_time;
           pTag->GetDataAddress(Reader.GetExifBase(), Ptr, maxsize);
           exposure_time.num = *Ptr.uiValue;
           exposure_time.den = *(Ptr.uiValue + 1);
           if(exposure_time.den){
                   pExifSettings->nExposureTimeUsec = (OMX_U32)(1000000 * exposure_time.num / exposure_time.den) ;
           }else{
               MSG0("Error reading Exposure Time value");
               pExifSettings->nExposureTimeUsec = 0;
           }
       }else{
           MSG0("TAG Exposure Time is not present in extradata ");
       }

       // TAG FNumber >> xFNumber
       pTag=Reader.FindTag(eExifTag_FNumber);
       if(pTag!= NULL){
           exif_urational_t fnumber;
           pTag->GetDataAddress(Reader.GetExifBase(), Ptr, maxsize);
           fnumber.num = *Ptr.uiValue;
           fnumber.den = *(Ptr.uiValue + 1);
           if(fnumber.den){
               pExifSettings->xFNumber = (OMX_U32)(0x10000 * fnumber.num / fnumber.den) ;
           }else{
               MSG0("Error reading Fnumber value");
               pExifSettings->xFNumber = 0;
           }
       }else{
           MSG0("TAG FNumber is not present in extradata ");
       }

       // TAG Brightness >> nBrightness
       pTag=Reader.FindTag(eExifTag_BrightnessValue);
       if(pTag!= NULL){
           exif_srational_t brightness;
           pTag->GetDataAddress(Reader.GetExifBase(), Ptr, maxsize);
           brightness.num = *Ptr.iValue;
           brightness.den = *(Ptr.iValue + 1);
           if(brightness.den){
                   pExifSettings->nBrightness = (OMX_U32)(brightness.num / (2*brightness.den) + 50);
           }else{
               MSG0("Error reading Exposure Time value");
               pExifSettings->nBrightness = 0;
           }
       }else{
           MSG0("TAG Exposure Time is not present in extradata ");
       }

       // TAG Subject Distance >> nSubjectDistance
       pTag=Reader.FindTag(eExifTag_SubjectDistance);
       if(pTag!= NULL){
           exif_urational_t subject_distance;
           pTag->GetDataAddress(Reader.GetExifBase(), Ptr, maxsize);
           subject_distance.num = *Ptr.uiValue;
           subject_distance.den = *(Ptr.uiValue + 1);
           if(subject_distance.den){
                   pExifSettings->nSubjectDistance = (OMX_U32)(subject_distance.num / subject_distance.den) ;
           }else{
               MSG0("Error reading Subject Distance value");
               pExifSettings->nSubjectDistance = 0;
           }
       }else{
           MSG0("TAG Subject Distance is not present in extradata ");
       }

       // TAG Exposure Index >> nEvCompensation
       pTag=Reader.FindTag(eExifTag_ExposureIndex);
       if(pTag!= NULL){
           exif_urational_t exposure_index;
           pTag->GetDataAddress(Reader.GetExifBase(), Ptr, maxsize);
           exposure_index.num = *Ptr.uiValue;
           exposure_index.den = *(Ptr.uiValue + 1);
           if(exposure_index.den){
                   pExifSettings->nEvCompensation = (OMX_U32)(exposure_index.num / exposure_index.den) ;
           }else{
               MSG0("Error reading Exposure Index value");
               pExifSettings->nEvCompensation = 0;
           }
       }else{
           MSG0("TAG Exposure Index is not present in extradata ");
       }

       // TAG Exposure Mode >> eExposureControl, bBracketingEnabled
       pTag=Reader.FindTag(eExifTag_ExposureMode);
       if(pTag!= NULL){
           OMX_EXPOSURECONTROLTYPE exposure_control = OMX_ExposureControlOff;
           pTag->GetDataAddress(Reader.GetExifBase(), Ptr, maxsize);
           pExifSettings->bBracketingEnabled = OMX_FALSE; //default value
           switch(*Ptr.usValue)
           {
                case 0: exposure_control = OMX_ExposureControlAuto; break;
                case 1: exposure_control = OMX_ExposureControlOff; break;
                case 2: pExifSettings->bBracketingEnabled = OMX_TRUE; break;
                default:
                    MSG1("Unrecognized Exposure Mode %d", *Ptr.usValue); break;
           }
           pExifSettings->eExposureControl = exposure_control;
       }else{
           MSG0("TAG Exposure Mode is not present in extradata ");
       }

       // TAG Contrast >> nContrast
       // !!!Warning!!! Original contrast value is not embedded in extradata
       // Only the direction of contrast processing is indicated in contrast tag:
       // 0= Normal , 1= Soft , 2= Hard, Other= reserved
       pTag=Reader.FindTag(eExifTag_Contrast);
       if(pTag!= NULL){
           pTag->GetDataAddress(Reader.GetExifBase(), Ptr, maxsize);
           pExifSettings->nContrast = (OMX_S32)*Ptr.usValue;
       }else{
           MSG0("TAG Contrast is not present in extradata ");
       }

       // TAG Subject Area >> sAutoFocusROI
       pTag=Reader.FindTag(eExifTag_SubjectArea);
       if(pTag!= NULL){
           pTag->GetDataAddress(Reader.GetExifBase(), Ptr, maxsize);
           pExifSettings->sAutoFocusROI.sSize.nWidth  = (OMX_S32)*(Ptr.usValue + 2);
           pExifSettings->sAutoFocusROI.sSize.nHeight = (OMX_S32)*(Ptr.usValue + 3);
           pExifSettings->sAutoFocusROI.sTopLeft.nX   = (OMX_S32)(*Ptr.usValue - *(Ptr.usValue + 2)/2);
           pExifSettings->sAutoFocusROI.sTopLeft.nY   = (OMX_S32)(*(Ptr.usValue + 1) - *(Ptr.usValue + 3)/2);
       }else{
           MSG0("TAG Subject Area is not present in extradata ");
       }

#if (IMG_CONFIG_EXIF_MAKER_NOTES == IMG_CONFIG_TRUE)
       // TAG Maker Note >> pMakerNote, nMakerNoteSize
       pTag=Reader.FindTag(eExifTag_MakerNote);
       if(pTag!= NULL){
           pTag->GetDataAddress(Reader.GetExifBase(), Ptr, maxsize);
           pExifSettings->pMakerNote = (const void*)Ptr.cValue;
           pExifSettings->nMakerNoteSize = strlen((const  char*)Ptr.cValue)+1;
       }else{
           MSG0("TAG Make is not present in extradata ");
       }
#else
       pExifSettings->pMakerNote = NULL;
       pExifSettings->nMakerNoteSize = 0;
#endif

       // TAG Color Space >> eColorSpace
       pTag=Reader.FindTag(eExifTag_ColorSpace);
       if(pTag!= NULL){
           IFM_COLOR_SPACE color_space = IFM_ColorSpace_Uncalibrated;
           pTag->GetDataAddress(Reader.GetExifBase(), Ptr, maxsize);
           switch(*Ptr.usValue)
           {
                case 1: color_space = IFM_ColorSpace_sRGB; break;
                case 0xFFFF: color_space = IFM_ColorSpace_Uncalibrated; break;
                default:
                   MSG1("Unreconized Color Space %d", *Ptr.usValue); break;
           }
           pExifSettings->eColorSpace= color_space;
       }else{
           MSG0("TAG Color Space is not present in extradata ");
       }

       // !!!Warning!!! xAnalogGain and xDigitalGain are not embedded in extradata
       // The GainControl Tag only indicates the degree of overall image gain adjustment (1, 2, 3 or 4)
       // Return 0XFFFFFFFF for unknown values
       pExifSettings->xAnalogGain = 0xFFFFFFFF;
       pExifSettings->xDigitalGain = 0xFFFFFFFF;

   }

   OUTR(" ",OMX_ErrorNone);
   return OMX_ErrorNone;
/* -CR370700 */
#endif
   OUTR(" ",OMX_ErrorNotImplemented);
   return OMX_ErrorNotImplemented;

}

#if defined (DISPLAY_EXIF_METADATA)

// EXIF related indexes used by the EXIF reader
#define APP1_HEADER_SIZE 10
#define TIFF_HEADER_SIZE 8
#define SIZE_OF_FIELD 12
#define TAG_OFFSET 0
#define TYPE_OFFSET 2
#define COUNT_OFFSET 4
#define VALUE_OFFSET 8

// Function that prints out human readable EXIF metadata
OMX_ERRORTYPE CExifReader::print(const OMX_U8* pBuffer)
{
   /* Print the APP1 header */
   const OMX_U8* pApp1Header = pBuffer;
   OMX_U16 headerLength = 0;
   memcpy(&headerLength, pApp1Header + 2, sizeof(OMX_U16));
   MSG0("[APP1 HEADER] -------------------------------\n");
   MSG2("[APP1 HEADER] Marker: 0x%.2X%.2X\n", pApp1Header[0], pApp1Header[1]);
   MSG1("[APP1 HEADER] Length: %d\n", headerLength);
   MSG4("[APP1 HEADER] Exif  : '%c%c%c%c'\n", pApp1Header[4], pApp1Header[5], pApp1Header[6], pApp1Header[7]);
   MSG1("[APP1 HEADER] NULL  : 0x%.2X\n", pApp1Header[8]);
   MSG1("[APP1 HEADER] PAD   : 0x%.2X\n", pApp1Header[9]);

   /* Print the TIFF header */
   const OMX_U8* pTiffHeader = pApp1Header+APP1_HEADER_SIZE;
   OMX_U16 fourtyTwo = 0;
   memcpy(&fourtyTwo, pTiffHeader + 2, sizeof(OMX_U16));
   MSG0("[TIFF HEADER] -------------------------------\n");
   MSG2("[TIFF HEADER] Byte Order   : '%c%c'\n", pTiffHeader[0], pTiffHeader[1]);
   MSG1("[TIFF HEADER] Fourty Two   : %d\n", fourtyTwo);
   // Caution: OffsetOfIfd is not 4 bytes aligned field: read as little endian 32 bits value
   OMX_U32 OffsetOfIfd =  (((OMX_U32)(pTiffHeader[4]))<<0)
                         |(((OMX_U32)(pTiffHeader[5]))<<8)
                         |(((OMX_U32)(pTiffHeader[6]))<<16)
                         |(((OMX_U32)(pTiffHeader[7]))<<24);
   MSG1("[TIFF HEADER] Offset of IFD: %ld\n", OffsetOfIfd);

   /* Print the main IFD */
   const OMX_U8* pMainIfd = pTiffHeader+TIFF_HEADER_SIZE;
   OMX_U16 NumberOfMainFields = 0;
   memcpy(&NumberOfMainFields, pMainIfd, sizeof (OMX_U16));
   MSG0("[MAIN IFD] -------------------------------\n");
   MSG1("[MAIN IFD] Number of Fields: %d\n", NumberOfMainFields);
   const OMX_U8* pMainFields = pMainIfd+sizeof(OMX_U16);
   OMX_U32 ExifIfdPointerValue = 0;
   for(OMX_U16 field=0; field<NumberOfMainFields; field++)
   {
      const OMX_U8* pField = &pMainFields[field*SIZE_OF_FIELD];
      OMX_U16 tag = 0, type = 0;
      OMX_U32 count = 0;
      memcpy(&tag, pField + TAG_OFFSET, sizeof (OMX_U16));
      memcpy(&type, pField + TYPE_OFFSET, sizeof (OMX_U16));
      memcpy(&count, pField + COUNT_OFFSET, sizeof (OMX_U32));
      MSG0("[MAIN IFD] -------------------------------\n");
      MSG3("[MAIN IFD] [%.2d] Tag  : %s (=%d)\n", field, tagId2String(tag), tag);
      MSG3("[MAIN IFD] [%.2d] Type : %s (=%d)\n", field, typeId2String(type), type);
      MSG2("[MAIN IFD] [%.2d] Count: %ld\n", field, count);
      MSG2("[MAIN IFD] [%.2d] Value: %s\n", field, fieldValueString(pField, pTiffHeader));
      if(tag==IFDtag_ExifIfdPointer) {
         // Found the Exif Ifd Pointer
         ExifIfdPointerValue = *(OMX_U32*)&pField[VALUE_OFFSET];
      }
   }
   if(ExifIfdPointerValue==0) {
      MSG0("[MAIN IFD] ExifIfdPointer tag not found or null value, cannot continue.\n");
      return OMX_ErrorNone;
   }
   MSG0("[MAIN IFD] -------------------------------\n");
   OMX_U32 NextIfdOffsetValue = *(OMX_U32*)(pMainFields + NumberOfMainFields*SIZE_OF_FIELD);
   MSG1("[MAIN IFD] Next IFD: %ld\n", NextIfdOffsetValue);

   /* Print the EXIF IFD */
   const OMX_U8* pExifIfd = pTiffHeader + ExifIfdPointerValue;
   OMX_U16 NumberOfExifFields = 0;
   memcpy(&NumberOfExifFields, pExifIfd, sizeof (OMX_U16));
   MSG0("[EXIF IFD] -------------------------------\n");
   MSG1("[EXIF IFD] Number of Fields: %d\n", NumberOfExifFields);
   const OMX_U8* pExifFields = pExifIfd+sizeof(OMX_U16);
   for(OMX_U16 field=0; field<NumberOfExifFields; field++)
   {
      const OMX_U8* pField = &pExifFields[field*SIZE_OF_FIELD];
      OMX_U16 tag = 0, type = 0;
      OMX_U32 count = 0;
      memcpy(&tag, pField + TAG_OFFSET, sizeof (OMX_U16));
      memcpy(&type, pField + TYPE_OFFSET, sizeof (OMX_U16));
      memcpy(&count, pField + COUNT_OFFSET, sizeof (OMX_U32));
      MSG0("[EXIF IFD] -------------------------------\n");
      MSG3("[EXIF IFD] [%.2d] Tag  : %s (=%d)\n", field, tagId2String(tag), tag);
      MSG3("[EXIF IFD] [%.2d] Type : %s (=%d)\n", field, typeId2String(type), type);
      MSG2("[EXIF IFD] [%.2d] Count: %ld\n", field, count);
      MSG2("[EXIF IFD] [%.2d] Value: %s\n", field, fieldValueString(pField, pTiffHeader));
   } // end for
   MSG0("[EXIF IFD] -------------------------------\n");
   NextIfdOffsetValue = *(OMX_U32*)(pExifFields + NumberOfExifFields*SIZE_OF_FIELD);
   MSG1("[EXIF IFD] Next IFD: %ld\n", NextIfdOffsetValue);
   MSG0("[EXIF IFD] -------------------------------\n");

   return OMX_ErrorNone;
}

const char* CExifReader::tagId2String(const OMX_U16 aTag)
{
    #define TAG_HASH_ENTRY(tag)   { IFDtag_##tag, #tag}

    typedef struct {
       IFDtag_e tag;
       const char* str;
    } tag_hash_entry_t;

    static const tag_hash_entry_t tag_hash_table[] =
    {
       TAG_HASH_ENTRY(Make),
       TAG_HASH_ENTRY(Model),
       TAG_HASH_ENTRY(Orientation),
       TAG_HASH_ENTRY(XResolution),
       TAG_HASH_ENTRY(YResolution),
       TAG_HASH_ENTRY(ResolutionUnit),
       TAG_HASH_ENTRY(Software),
       TAG_HASH_ENTRY(DateTime),
       TAG_HASH_ENTRY(YCbCrPositioning),
       TAG_HASH_ENTRY(ExposureTime),
       TAG_HASH_ENTRY(FNumber),
       TAG_HASH_ENTRY(ExifIfdPointer),
       TAG_HASH_ENTRY(GpsInfoIfdPointer),
       TAG_HASH_ENTRY(InteroperabilityIFDPointer),
       TAG_HASH_ENTRY(ISOSpeedRatings),
       TAG_HASH_ENTRY(ExifVersion),
       TAG_HASH_ENTRY(DateTimeOriginal),
       TAG_HASH_ENTRY(DateTimeDigitized),
       TAG_HASH_ENTRY(ComponentsConfiguration),
       TAG_HASH_ENTRY(ShutterSpeedValue),
#ifndef CUSTOMER_EXIF
       TAG_HASH_ENTRY(ApertureValue),
       TAG_HASH_ENTRY(BrightnessValue),
       TAG_HASH_ENTRY(SubjectDistance),
       TAG_HASH_ENTRY(GainControl),
       TAG_HASH_ENTRY(Contrast),
       TAG_HASH_ENTRY(SubjectArea),
#endif       

	TAG_HASH_ENTRY(MeteringMode),
       TAG_HASH_ENTRY(LightSource),
       TAG_HASH_ENTRY(Flash),
       TAG_HASH_ENTRY(FocalLength),
       TAG_HASH_ENTRY(MakerNote),
       TAG_HASH_ENTRY(FlashpixVersion),
       TAG_HASH_ENTRY(ColorSpace),
       TAG_HASH_ENTRY(PixelXDimension),
       TAG_HASH_ENTRY(PixelYDimension),
       TAG_HASH_ENTRY(FlashEnergy),
#ifndef CUSTOMER_EXIF
       TAG_HASH_ENTRY(ExposureIndex),
#endif
       TAG_HASH_ENTRY(CustomRendered),
       TAG_HASH_ENTRY(ExposureMode),
       TAG_HASH_ENTRY(WhiteBalance),
       TAG_HASH_ENTRY(DigitalZoomRatio),
       TAG_HASH_ENTRY(SceneCaptureType),
       
       TAG_HASH_ENTRY(ExposureBiasValue),
       TAG_HASH_ENTRY(SubjectDistanceRange),
       TAG_HASH_ENTRY(Index),
       TAG_HASH_ENTRY(Version)
    };

   for(unsigned int i=0; i<(sizeof(tag_hash_table)/sizeof(tag_hash_table[0])); i++) {
      if(tag_hash_table[i].tag == (IFDtag_e)aTag )
         return tag_hash_table[i].str;
   }
   return (const char*)"Tag unknown";
}

const char* CExifReader::typeId2String(const OMX_U16 aType)
{
   switch(aType) {
      case 1:  return "BYTE";
      case 2:  return "ASCII";
      case 3:  return "SHORT";
      case 4:  return "LONG";
      case 5:  return "RATIONAL";
      case 7:  return "UNDEFINED";
      case 9:  return "SLONG";
      case 10: return "SRATIONAL";
   }
   return (const char*)"Type unknown";
}

const char* CExifReader::fieldValueString(const OMX_U8* pField, const OMX_U8* pTiffHeader)
{
    static char str[128]="";
    str[0]='\0';
    OMX_U16 type  = *(OMX_U16*)&pField[TYPE_OFFSET];
    OMX_U32 count = *(OMX_U32*)&pField[COUNT_OFFSET];
    OMX_U32 sizeoftype = 0;
    switch(type)
    {
       case 1:  sizeoftype = 1; break; // BYTE
       case 2:  sizeoftype = 1; break; // ASCII
       case 3:  sizeoftype = 2; break; // SHORT
       case 4:  sizeoftype = 4; break; // LONG
       case 5:  sizeoftype = 8; break; // RATIONAL (2 LONGs)
       case 7:  sizeoftype = 1; break; // UNDEFINED (BYTE)
       case 9:  sizeoftype = 4; break; // SLONG
       case 10: sizeoftype = 8; break; // SRATIONAL (2 SLONGs)
       default:
          snprintf( str, sizeof(str), "Invalid field type 0x%.4X", type);
          return (const char*)str;
    }
    // Actual size of value is: count * sizeof(type)
    OMX_U32 size = count * sizeoftype;
    void* pValue = NULL;
    if(size<=4) {
       // Value is directly contained in "ValueOffset" field
       pValue = (void*)&pField[VALUE_OFFSET];
    }
    else {
       // Value is located at "ValueOffset" bytes from TiffHeader start
       OMX_U32 ValueOffset = *(OMX_U32*)&pField[VALUE_OFFSET];
       pValue = (void*)(pTiffHeader + ValueOffset);
    }
    switch(type)
    {
       case 1:
       {
          // BYTE
          OMX_U8* pByte = (OMX_U8*)pValue;
          for(OMX_U32 i=0; i<count; i++) {
             snprintf(&str[strnlen(str,sizeof(str))], sizeof(str)-strnlen(str,sizeof(str)), "%u (0x%.2X) ", pByte[i], pByte[i]);
          }
          break;
       }
       case 2:
       {
          // ASCII
          const char* pAscii = (const char*)pValue;
          const char* pCaution = "";
          if(pAscii[count-1] != '\0') {
             pCaution = "[*** Missing NULL terminator ***]";
          }
          snprintf( str, sizeof(str), "\'%s\' %s", pAscii, pCaution);
          break;
       }
       case 3:
       {
          // SHORT
          OMX_U16* pShort = (OMX_U16*)pValue;
          for(OMX_U32 i=0; i<count; i++) {
             snprintf(&str[strnlen(str,sizeof(str))], sizeof(str)-strnlen(str,sizeof(str)), "%u (0x%.4X) ", pShort[i], pShort[i]);
          }
          break;
       }
       case 4:
       {
          // LONG
          OMX_U32* pLong = (OMX_U32*)pValue;
          for(OMX_U32 i=0; i<count; i++) {
             snprintf(&str[strnlen(str,sizeof(str))], sizeof(str)-strnlen(str,sizeof(str)), "%lu (0x%.8lX) ", pLong[i], pLong[i]);
          }
          break;
       }
       case 5:
       {
          // RATIONAL (2 LONGs)
          typedef struct
          {
             OMX_U32 num;
             OMX_U32 den;
          } urat_t;
          urat_t* pURat = (urat_t*)pValue;
          for(OMX_U32 i=0; i<count; i++) {
             snprintf(&str[strnlen(str,sizeof(str))], sizeof(str)-strnlen(str,sizeof(str)), "%lu/%lu (%f) ", pURat->num, pURat->den, (float)(pURat->num)/(float)(pURat->den));
             pURat++;
          }
          break;
       }
       case 7:
       {
          // UNDEFINED
          OMX_U8* pUndef = (OMX_U8*)pValue;
          for(OMX_U32 i=0; i<count; i++) {
             snprintf(&str[strnlen(str,sizeof(str))], sizeof(str)-strnlen(str,sizeof(str)), "0x%.2X ", pUndef[i]);
          }
          snprintf(&str[strnlen(str,sizeof(str))], sizeof(str)-strnlen(str,sizeof(str)), "('");
          for(OMX_U32 i=0; i<count; i++) {
             snprintf(&str[strnlen(str,sizeof(str))], sizeof(str)-strnlen(str,sizeof(str)), "%c", pUndef[i]);
          }
          snprintf(&str[strnlen(str,sizeof(str))], sizeof(str)-strnlen(str,sizeof(str)), "')");
          break;
       }
       case 9:
       {
          // SLONG
          OMX_S32* pSLong = (OMX_S32*)pValue;
          for(OMX_U32 i=0; i<count; i++) {
             snprintf(&str[strnlen(str,sizeof(str))], sizeof(str)-strnlen(str,sizeof(str)), " %ld (0x%.8lX) ", pSLong[i], pSLong[i]);
          }
          break;
       }
       case 10:
       {
          // SRATIONAL (2 SLONGs)
          typedef struct {
             OMX_S32 num;
             OMX_S32 den;
          } srat_t;
          srat_t* pSRat = (srat_t*)pValue;
          for(OMX_U32 i=0; i<count; i++) {
             snprintf(&str[strnlen(str,sizeof(str))], sizeof(str)-strnlen(str,sizeof(str)), "%ld/%ld (%f) ", pSRat->num, pSRat->den, (float)(pSRat->num)/(float)(pSRat->den));
             pSRat++;
          }
          break;
       }
       default:
          snprintf(str, sizeof(str), "Invalid field type %d", type);
          break;
    } // end switch
    return (const char*)str;
}
#endif // DISPLAY_EXIF_METADATA
