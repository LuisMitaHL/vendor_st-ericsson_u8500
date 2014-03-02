/*
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*/
/* \brief   Exif reader
*/
#ifdef _MSC_VER
	#include "stdafx.h"
	#define EXIF_Log printf
	#include "stdio.h"
#else

// todo : use correct trace API

//	#include <los/api/los_api.h>
//	#define EXIF_Log LOS_Log
    #define EXIF_Log(...)
#endif
#include <string.h>
#include "ExifReader.h"

#ifndef S_OK
	#define S_OK 0
#endif

// TIFF spec http://partners.adobe.com/public/developer/en/tiff/TIFF6.pdf
#if 0 //not used
static const TExifAPP1Marker Exif_DefaultAPP1Marker=
{
	ExifReader::eExif_AppMarker_0,
	ExifReader::eExif_AppMarker_1,
	0,  //LengthOfField
	"Exif\0",
	ExifReader::eLittleEndian,
	ExifReader::eHeader_byte_42,
	8, // OffsetOfIFD_0
	0, // OffsetOfIFD_1
};
#endif

const ExifTagDecription ExifTagDecriptionTable[]=
{
	//Tags relating to image data structure
	{  256, eExif_SHORT     , 1       , "ImageWidth"},
	{  256, eExif_LONG      , 1       , "ImageWidth"},
	{  257, eExif_SHORT     , 1       , "ImageLength"},
	{  257, eExif_LONG      , 1       , "ImageLength"},
	{  258, eExif_SHORT     , 3       , "BitsPerSample"},
	{  259, eExif_SHORT     , 1       , "Compression"},
	{  262, eExif_SHORT     , 1       , "PhotometricInterpretation"},
	{  274, eExif_SHORT     , 1       , "Orientation"},
	{  277, eExif_SHORT     , 1       , "SamplesPerPixel"},
	{  284, eExif_SHORT     , 1       , "PlanarConfiguration"},
	{  530, eExif_SHORT     , 2       , "YCbCrSubSampling"},
	{  531, eExif_SHORT     , 1       , "YCbCrPositioning"},
	{  282, eExif_RATIONAL  , 1       , "XResolution"},
	{  283, eExif_RATIONAL  , 1       , "YResolution"},
	{  296, eExif_SHORT     , 1       , "ResolutionUnit"},
	//Tags relating to recording offset
	{  273, eExif_SHORT     , 0       , "StripOffsets"},
	{  273, eExif_LONG      , 0       , "StripOffsets"},
	{  278, eExif_SHORT     , 1       , "RowsPerStrip"},
	{  278, eExif_LONG      , 1       , "RowsPerStrip"},
	{  279, eExif_SHORT     , 0       , "StripByteCounts"},
	{  279, eExif_LONG      , 0       , "StripByteCounts"},
	{  513, eExif_LONG      , 1       , "JPEGInterchangeFormat"},
	{  514, eExif_LONG      , 1       , "JPEGInterchangeFormatLength"},
	//Tags relating to image data characteristics
	{  301, eExif_SHORT     , 3*256   , "TransferFunction"},
	{  305, eExif_ASCII     , 0       , "Software"},
	{  306, eExif_ASCII     , 20      , "DateTime"},
	{  318, eExif_RATIONAL  , 2       , "WhitePoint"},
	{  319, eExif_RATIONAL  , 6       , "PrimaryChromaticities"},
	{  529, eExif_RATIONAL  , 3       , "YCbCrCoefficients"},
	{  532, eExif_RATIONAL  , 6       , "ReferenceBlackWhite"},
	//Other tags
	{  306, eExif_ASCII     , 20      , "DateTime"},
	{  270, eExif_ASCII     , 0       , "ImageDescription"},
	{  271, eExif_ASCII     , 0       , "Make"},
	{  272, eExif_ASCII     , 0       , "Model"},
	{  305, eExif_ASCII     , 0       , "Software"},
	{  315, eExif_ASCII     , 0       , "Artist"},
	{33432, eExif_ASCII     , 0       , "Copyright"},
	//Tags Relating to Version
	{36864, eExif_UNDEFINED , 4       , "ExifVersion"},
	{40960, eExif_UNDEFINED , 4       , "FlashpixVersion"},
	//Tag Relating to Image Data Characteristics
	{40961, eExif_SHORT     , 1       , "ColorSpace"},
	//Tags Relating to Image Configuration
	{37121, eExif_UNDEFINED , 4       , "ComponentsConfiguration"},
	{37122, eExif_RATIONAL  , 1       , "CompressedBitsPerPixel"},
	{40962, eExif_LONG      , 1       , "PixelXDimension"},
	{40962, eExif_SHORT     , 1       , "PixelXDimension"},
	{40963, eExif_LONG      , 1       , "PixelYDimension"},
	{40963, eExif_SHORT     , 1       , "PixelYDimension"},
	//Tags Relating to User Information
	{37500, eExif_UNDEFINED , 0       , "MakerNote"},
	{37510, eExif_UNDEFINED , 0       , "UserComment"},
	//Tag Relating to Related File Information
	{40964, eExif_ASCII     , 13      , "RelatedSoundFile"},
	//Tags Relating to Date and Time
	{36867, eExif_ASCII     , 20      , "DateTimeOriginal"},
	{36868, eExif_ASCII     , 20      , "DateTimeDigitized"},
	{37520, eExif_ASCII     , 0       , "SubSecTime"},
	{37521, eExif_ASCII     , 0       , "SubSecTimeOriginal"},
	{37522, eExif_ASCII     , 0       , "SubSecTimeDigitized"},
	//Tags Relating to Picture-Taking Conditions
	{33434, eExif_RATIONAL  , 1       , "ExposureTime"},
	{33437, eExif_RATIONAL  , 1       , "FNumber"},
	{34850, eExif_SHORT     , 1       , "ExposureProgram"},
	{34852, eExif_ASCII     , 0       , "SpectralSensitivity"},
	{34855, eExif_SHORT     , 0       , "ISOSpeedRatings"},
	{34856, eExif_UNDEFINED , 0       , "OECF"},
	{37377, eExif_SRATIONAL , 1       , "ShutterSpeedValue"},
	{37378, eExif_RATIONAL  , 1       , "ApertureValue"},
	{37379, eExif_SRATIONAL , 1       , "BrightnessValue"},
	{37380, eExif_SRATIONAL , 1       , "ExposureBiasValue"},
	{37381, eExif_RATIONAL  , 1       , "MaxApertureValue"},
	{37382, eExif_RATIONAL  , 1       , "SubjectDistance"},
	{37383, eExif_SHORT     , 1       , "MeteringMode"},
	{37384, eExif_SHORT     , 1       , "LightSource"},
	{37385, eExif_SHORT     , 1       , "Flash"},
	{37386, eExif_RATIONAL  , 1       , "FocalLength"},
	{37396, eExif_SHORT     , 2       , "SubjectArea"},
	{37396, eExif_SHORT     , 3       , "SubjectArea"},
	{37396, eExif_SHORT     , 4       , "SubjectArea"},
	{41483, eExif_RATIONAL  , 1       , "FlashEnergy"},
	{41484, eExif_UNDEFINED , 0       , "SpatialFrequencyResponse"},
	{41486, eExif_RATIONAL  , 1       , "FocalPlaneXResolution"},
	{41487, eExif_RATIONAL  , 1       , "FocalPlaneYResolution"},
	{41488, eExif_SHORT     , 1       , "FocalPlaneResolutionUnit"},
	{41492, eExif_SHORT     , 2       , "SubjectLocation"},
	{41493, eExif_RATIONAL  , 1       , "ExposureIndex"},
	{41495, eExif_SHORT     , 1       , "SensingMethod"},
	{41728, eExif_UNDEFINED , 1       , "FileSource"},
	{41729, eExif_UNDEFINED , 1       , "SceneType"},
	{41730, eExif_UNDEFINED , 0       , "CFAPattern"},
	{41985, eExif_SHORT     , 1       , "CustomRendered"},
	{41986, eExif_SHORT     , 1       , "ExposureMode"},
	{41987, eExif_SHORT     , 1       , "WhiteBalance"},
	{41988, eExif_RATIONAL  , 1       , "DigitalZoomRatio"},
	{41989, eExif_SHORT     , 1       , "FocalLengthIn35mmFilm"},
	{41990, eExif_SHORT     , 1       , "SceneCaptureType"},
	//{41991, eExif_RATIONAL  , 1       , "GainControl"}, // spec says once RATIONAL, once SHORT
	{41991, eExif_SHORT     , 1       , "GainControl"},
	{41992, eExif_SHORT     , 1       , "Contrast"},
	{41993, eExif_SHORT     , 1       , "Saturation"},
	{41994, eExif_SHORT     , 1       , "Sharpness"},
	{41995, eExif_UNDEFINED , 0       , "DeviceSettingDescription"},
	{41996, eExif_SHORT     , 1       , "SubjectDistanceRange"},
	//Tags Relating to GPS
	{    0, eExif_BYTE      , 4       , "GPSVersionID"},
	{    1, eExif_ASCII     , 2       , "GPSLatitudeRef"},
	{    2, eExif_RATIONAL  , 3       , "GPSLatitude"},
	{    3, eExif_ASCII     , 2       , "GPSLongitudeRef"},
	{    4, eExif_RATIONAL  , 3       , "GPSLongitude"},
	{    5, eExif_BYTE      , 1       , "GPSAltitudeRef"},
	{    6, eExif_RATIONAL  , 1       , "GPSAltitude"},
	{    7, eExif_RATIONAL  , 3       , "GPSTimeStamp"},
	{    8, eExif_ASCII     , 0       , "GPSSatellites"},
	{    9, eExif_ASCII     , 2       , "GPSStatus"},
	{   10, eExif_ASCII     , 2       , "GPSMeasureMode"},
	{   11, eExif_RATIONAL  , 1       , "GPSDOP"},
	{   12, eExif_ASCII     , 2       , "GPSSpeedRef"},
	{   13, eExif_RATIONAL  , 1       , "GPSSpeed"},
	{   14, eExif_ASCII     , 2       , "GPSTrackRef"},
	{   15, eExif_RATIONAL  , 1       , "GPSTrack"},
	{   16, eExif_ASCII     , 2       , "GPSImgDirectionRef"},
	{   17, eExif_RATIONAL  , 1       , "GPSImgDirection"},
	{   18, eExif_ASCII     , 0       , "GPSMapDatum"},
	{   19, eExif_ASCII     , 2       , "GPSDestLatitudeRef"},
	{   20, eExif_RATIONAL  , 3       , "GPSDestLatitude"},
	{   21, eExif_ASCII     , 2       , "GPSDestLongitudeRef"},
	{   22, eExif_RATIONAL  , 3       , "GPSDestLongitude"},
	{   23, eExif_ASCII     , 2       , "GPSDestBearingRef"},
	{   24, eExif_RATIONAL  , 1       , "GPSDestBearing"},
	{   25, eExif_ASCII     , 2       , "GPSDestDistanceRef"},
	{   26, eExif_RATIONAL  , 1       , "GPSDestDistance"},
	{   27, eExif_UNDEFINED , 0       , "GPSProcessingMethod"},
	{   28, eExif_UNDEFINED , 0       , "GPSAreaInformation"},
	{   29, eExif_ASCII     , 11      , "GPSDateStamp"},
	{   30, eExif_SHORT     , 1       , "GPSDifferential"},
	//Other Tags
	{42016, eExif_ASCII     , 33      , "ImageUniqueID"},
	//Exif-specific IFD
	{34665, eExif_LONG      , 1       , "Exif IFD Pointer"},
	{34853, eExif_LONG      , 1       , "GPS IFD"},
	{40965, eExif_LONG      , 1       , "Interoperability IFD Pointer"},
};

static const unsigned int ExifTagSize[]=
{ // Gives the size in byte for given enum
	0,
	1,//eExif_BYTE     = 1,
	1,//eExif_ASCII    = 2,
	2,//eExif_SHORT    = 3,
	4,//eExif_LONG     = 4,
	8,//eExif_RATIONAL = 5,
	1,//eExif_SBYTE    = 6,
	1,//eExif_UNDEFINED= 7,
	2,//eExif_SSHORT   = 8,
	4,//eExif_SLONG    = 9,
	8,//eExif_SRATIONAL= 10,
	4,//eExif_FLOAT    = 11,
	8,//eExif_DOUBLE   = 12,
	0,//eExif_MAX      = 13,
};


unsigned int GetExifTagSizeof(int type)
//*************************************************************************************************************
{ // Return the size of the exif tag
	if ((type > 0) && (type <= eExif_MAX))
		return(ExifTagSize[type]);
	return(0);
}

const ExifTagDecription *GetExifTagDecription(const unsigned short val, enumExifType type)
//*************************************************************************************************************
{
	size_t Nbr=sizeof(ExifTagDecriptionTable)/sizeof(ExifTagDecriptionTable[0]);
	const ExifTagDecription *ptr=ExifTagDecriptionTable;
	while(Nbr >0)
	{
		if ( (val ==ptr->tag) && ( (type== eExif_UNKNOW) || ( type==ptr->type) ) )
			return(ptr);
		++ptr;
		--Nbr;
	}
	return(NULL); //Not found
}


const ExifTagDecription *GetExifTagDecription(const char *name, enumExifType type)
//*************************************************************************************************************
{
	size_t Nbr=sizeof(ExifTagDecriptionTable)/sizeof(ExifTagDecriptionTable[0]);
	const ExifTagDecription *ptr=ExifTagDecriptionTable;
	while(Nbr >0)
	{
		if ( (strcmp(name, ptr->comment)==0) && ( (type== eExif_UNKNOW) || ( type==ptr->type) ) )
			return(ptr);
		++ptr;
		--Nbr;
	}
	return(NULL); //Not found
}

//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************

unsigned int  ExifTag::GetOffset() const
//*************************************************************************************************************
{ // return the required size in byte
	const unsigned int value_sizeof=GetExifTagSizeof(Type);
	if ((Count * value_sizeof) > 4)
		return(Value_Offset);
	return(0); //No real offset
}

unsigned int ExifTag::GetOffset( size_t &size) const
//*************************************************************************************************************
{ // return the required size in byte
	const unsigned int value_sizeof=GetExifTagSizeof(Type);
	size=Count * value_sizeof;
	if (size > 4)
		return(Value_Offset);
	return(0); //No real offset
}

size_t ExifTag::GetDataSize() const
//*************************************************************************************************************
{ // return the required size in byte
	const unsigned int value_sizeof=GetExifTagSizeof(Type);
	return(Count * value_sizeof);
}

int ExifTag::GetDataAddress(const char *pExifBase, ExifPtrDataValue &DataAddr, size_t &size) const
//*************************************************************************************************************
{ // return the offset value and put the data address in the union
	const unsigned int value_sizeof=GetExifTagSizeof(Type);
	char *TargetOffset=(char*)pExifBase+Value_Offset;
	if ( (Count * value_sizeof) > 4)
	{ //It a real offset
		DataAddr.cValue=TargetOffset;
		size=Count * value_sizeof;
		return(Value_Offset);
	}
	else
	{ //Data is coded in the 'offset' field
		DataAddr.uiValue=(unsigned int *)&Value_Offset;
		size=4;
		return(0); // No offset
	}
}

int ExifTag::DisplayItem(const char *pExifBase) const
//*************************************************************************************************************
{
	const ExifTagDecription *pTagDesc;
	pTagDesc=GetExifTagDecription(Tag);
	unsigned int StripCount=Count;
	const unsigned int MaxCount=10;
	if (StripCount >MaxCount)
		StripCount=MaxCount;
	// compute if we are using offset or value
	ExifPtrDataValue Ptr;
	size_t size;
	GetDataAddress(pExifBase, Ptr, size);

	//compute comment
	const char *comment;
	if (pTagDesc)
	{
		comment=pTagDesc->comment;
	}
	else
	{
		comment="!!! Unknow !!!";
	}
    comment=comment;
	unsigned int offset=GetOffset();
	EXIF_Log("Type=%2d Count=%4d offset=%6u tag=%5d %-30s ", Type, Count, offset, Tag, comment);
	switch(Type)
	{
	case eExif_BYTE:
		{
			EXIF_Log("Value=%6d", *Ptr.ucValue);
			for (unsigned int nbr=1; nbr < StripCount; ++nbr) EXIF_Log(" Val[%d]=%u", nbr, *(++Ptr.ucValue));
		}
		break;
	case eExif_SHORT:
		{
			EXIF_Log("Value=%6d", *Ptr.usValue);
			for (unsigned int nbr=1; nbr < StripCount; ++nbr) EXIF_Log(" Val[%d]=%u", nbr, *(++Ptr.usValue));
		}
		break;
	case eExif_LONG:
		{
			EXIF_Log("Value=%6d", *Ptr.uiValue);
			for (unsigned int nbr=1; nbr < Count; ++nbr) EXIF_Log(" Val[%d]=%u", nbr, *(++Ptr.uiValue));
		}
		break;
	case eExif_SBYTE:
		{
			EXIF_Log("Value=%6d", *Ptr.cValue);
			for (unsigned int nbr=1; nbr < StripCount; ++nbr) EXIF_Log(" Val[%d]=%d", nbr, *(++Ptr.cValue));
		}
		break;
	case eExif_SSHORT:
		{
			EXIF_Log("Value=%6d", *Ptr.sValue);
			for (unsigned int nbr=1; nbr < StripCount; ++nbr) EXIF_Log(" Val[%d]=%d", nbr, *(++Ptr.sValue));
		}
		break;
	case eExif_SLONG:
		{
			EXIF_Log("Value=%6d", *Ptr.iValue);
			for (unsigned int nbr=1; nbr < StripCount; ++nbr) EXIF_Log(" Val[%d]=%d", nbr, *(++Ptr.iValue));
		}
		break;
	case eExif_FLOAT:
		EXIF_Log("Value=%6f",  *Ptr.fValue);
		for (unsigned int nbr=1; nbr < StripCount; ++nbr) EXIF_Log(" Val[%d]=%f", nbr, *(++Ptr.fValue));
		break;
	case eExif_DOUBLE:
		EXIF_Log("TValue='%g'", *Ptr.dValue);
		for (unsigned int nbr=1; nbr < StripCount; ++nbr) EXIF_Log(" Val[%d]=%g", nbr, *(++Ptr.dValue));
		break;
	case eExif_SRATIONAL:
		for (unsigned int nbr=0; nbr < StripCount; ++nbr)
		{
			if (nbr ==0) {
				EXIF_Log("Value=");
			}
			else {
				EXIF_Log(" Val[%d]=", nbr);
			}

			int numerateur=Ptr.iValue[0], denominateur=Ptr.iValue[1];
			double result=0.;
            result=result; // remove warning
			if (denominateur!=0)
			{
				result=(float)numerateur/(float)denominateur;
				if (nbr == 0)
				{
					if (denominateur==1) {
						EXIF_Log("%6d", numerateur);
					}
					else {
						EXIF_Log("%6g (%d/%d)", result, numerateur, denominateur);
					}
				}
				else
				{
					if (denominateur==1) {
						EXIF_Log("%d", numerateur);
					}
					else {
						EXIF_Log("%g (%d/%d)", result, numerateur, denominateur);
					}
				}
			}
			else {
				EXIF_Log("%d/%d !!!! DIVISION by 0 ", comment, numerateur, denominateur);
			}
			Ptr.uiValue+=2;
		}
		break;
	case eExif_RATIONAL:
		for (unsigned int nbr=0; nbr < StripCount; ++nbr)
		{
			if (nbr ==0) {
				EXIF_Log("Value=");
			}
			else {
				EXIF_Log(" Val[%d]=", nbr);
			}

			unsigned int numerateur=Ptr.uiValue[0], denominateur=Ptr.uiValue[1];
			double result=0.;
            result=result;
			if (denominateur!=0)
			{
				result=(float)numerateur/(float)denominateur;

				if (nbr == 0)
				{
					if (denominateur==1) {
						EXIF_Log("%6d", numerateur);
					}
					else {
						EXIF_Log("%6g (%u/%u)", result, numerateur, denominateur);
					}
				}
				else
				{
					if (denominateur==1) {
						EXIF_Log("%d", numerateur);
					}
					else {
						EXIF_Log("%g (%u/%u)", result, numerateur, denominateur);
					}
				}
			}
			else {
				EXIF_Log("%u/%u !!!! DIVISION by 0", comment, numerateur, denominateur);
			}
			Ptr.uiValue+=2;
		}
		break;

	case eExif_UNDEFINED:
		{
			StripCount=Count;
			if (StripCount > 32)
				StripCount=32;
			for (unsigned int nbr=0; nbr < StripCount; ++nbr) EXIF_Log("%02X, ", *(++Ptr.ucValue));
			Ptr.ucValue-=StripCount;
			EXIF_Log("\"");
			for (unsigned int nbr=0; nbr < StripCount; ++nbr) EXIF_Log("%c", *(++Ptr.ucValue));
			EXIF_Log("\"");
		}
		break;
	case eExif_ASCII:
		StripCount=Count;
		if (Count > 128) {
			EXIF_Log("\"%*s\"... stripped", 128, Ptr.cValue);
		}
		else {
			EXIF_Log("\"%*s\"", Count, Ptr.cValue);
		}
		break;
	default:
		EXIF_Log("offset=%6d", Value_Offset);
		break;
	}
	offset=offset;
	return(S_OK);
}

//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************
ExifIfd_Manager::ExifIfd_Manager(char *ptr, char * exif_base)
//*************************************************************************************************************
{
	m_BasePtr = ptr;
	m_ExifBase= exif_base;
}

void ExifIfd_Manager::SetBaseAddr( char *ifd_addr, char * exif_base)
//*************************************************************************************************************
{
	m_BasePtr  = ifd_addr;
	m_ExifBase = exif_base;
}

unsigned short ExifIfd_Manager::GetNbTag() const
//*************************************************************************************************************
{
	if (m_BasePtr==NULL)
		return(0);
	return(*(unsigned short *)m_BasePtr);
}

unsigned int ExifIfd_Manager::GetStartOffset() const
//*************************************************************************************************************
{
	return(m_BasePtr  -m_ExifBase);
}

ExifTag *ExifIfd_Manager::GetAt(unsigned short index) const
//*************************************************************************************************************
{
	if (m_BasePtr==NULL)
		return(NULL);
	unsigned short count=GetNbTag();
	if (index >= count)
	{
		return(NULL);
	}
	ExifTag *ptr=(ExifTag *)(m_BasePtr+sizeof(short));
	return(ptr+index);
}


ExifTag *ExifIfd_Manager::FindTag(unsigned short tag) const
//*************************************************************************************************************
{
	if (m_BasePtr==NULL)
		return(NULL);
	unsigned short count=GetNbTag();
	ExifTag *ptr=(ExifTag *)(m_BasePtr+sizeof(short));
	while( count >0)
	{
		if(ptr->Tag==tag)
			return(ptr);
		++ptr;
		--count;
	}
	return(NULL);
}

const char *ExifIfd_Manager::GetNextIfd() const
//*************************************************************************************************************
{
	if (m_BasePtr==NULL)
		return(NULL);
	unsigned short count=GetNbTag();
	ExifTag *ptr=GetAt(0);
	if (ptr ==NULL)
		return(NULL);
	ptr+=count; // Now point to end
	unsigned int offset=*(unsigned int *)ptr;
	if (offset==0)
		return(NULL);
	else
		return(m_BasePtr+offset);
}

unsigned int ExifIfd_Manager::GetEnd() const
//*************************************************************************************************************
{ //Return the offset off the next ifd or the next freee address
	if (m_BasePtr==NULL)
		return(0);
	const unsigned short count=GetNbTag();
	ExifTag *ptr=GetAt(0);
	if (ptr ==NULL)
		return((m_BasePtr+ (2*sizeof(unsigned int)))- m_ExifBase);
	ptr+=count; // Now point to end
	unsigned int offset=*(unsigned int *)ptr; // It's the next ifd
	if (offset!=0)
		return(offset); //Next Ifd is found don't go further
	unsigned int  current_offset, max_offset;
	max_offset= ((((char *)ptr)+sizeof(unsigned int)) - m_ExifBase);
	size_t size;
	ptr=GetAt(0);
	for (unsigned short i=0; i< count; ++i)
	{
		current_offset=ptr->GetOffset(size);
		if (current_offset !=0)
		{
			current_offset+=size;
			if (max_offset < current_offset)
				max_offset=current_offset;
		}
		++ptr;
	}
	return(max_offset);
}


//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************

ExifReader::ExifReader()
//*************************************************************************************************************
{
	m_pExifMarker= NULL;
	m_Size       = 0;
	m_AllocSize  = 0;
	m_UsedSize   = 0;
}

bool ExifReader::IsLittleEndian() const
//*************************************************************************************************************
{
	if(m_pExifMarker->TiffHeader_ByteOrder==eLittleEndian)
		return(true);
	else
		return(false);
}

int ExifReader::AttachBuffer(char *pBuff, size_t buffer_size /* Maximum buffer size */)
//*************************************************************************************************************
{
	if ((pBuff==NULL) || (buffer_size==0))
		return(-1);
	m_Size   = buffer_size;
	m_AllocSize=0;
	m_pExifMarker=(TExifAPP1Marker *)pBuff;
	if(m_pExifMarker->MarkerPrefix != eExif_AppMarker_0)
	{
		EXIF_Log("Exif error : invalid MarkerPrefix found 0x%X instead of 0x%X\n",m_pExifMarker->MarkerPrefix, eExif_AppMarker_0);
		return(-1);
	}
	if(m_pExifMarker->APP1 != eExif_AppMarker_1)
	{
		EXIF_Log("Exif error : APP1\n", m_pExifMarker->APP1, eExif_AppMarker_1);
		return(-2);
	}

	EXIF_Log("Exif info : LengthOfField = %d = 0x%x\n", m_pExifMarker->LengthOfField, m_pExifMarker->LengthOfField);

	if (strncmp((const char *)m_pExifMarker->ExifIndentifierCoder, "Exif", 5)!=0)
	{
		EXIF_Log("Exif error : invlaid IndentifierCoder found '%5c' instead of 'Exif'\n", m_pExifMarker->ExifIndentifierCoder);
		return(-3);
	}

	switch(m_pExifMarker->TiffHeader_ByteOrder)
	{
	case eLittleEndian:
		EXIF_Log("Exif info : TiffHeader.ByteOrder LITTLE_ENDIAN\n");
		break;
	case eBigEndian:
		EXIF_Log("Exif info : TiffHeader.ByteOrder BIG_ENDIAN\n");
		break;
	default:
		EXIF_Log("Exif error : TiffHeader.ByteOrder invalid value %d\n", m_pExifMarker->TiffHeader_ByteOrder);
		return(-4);
	}

	if(m_pExifMarker->TiffHeader_filed_42 != eHeader_byte_42)
	{
		EXIF_Log("Exif error : TiffHeader.filed_42 found %d instead of %d\n", m_pExifMarker->TiffHeader_filed_42, eHeader_byte_42);
		return(-5);
	}
	EXIF_Log("Number of IFD=%d\n", GetNbOfIfd());
	return(S_OK);
}

char *ExifReader::GetIfd0() const
//*************************************************************************************************************
{ // Get the root ifd0
	if (m_pExifMarker==NULL)
		return( NULL);
	if (m_pExifMarker->LengthOfField==0)
		return( NULL);
	char *pIfd0= (char *)(&m_pExifMarker->TiffHeader_ByteOrder); //The offset is relative to start of TIFF HEADER
	size_t offset;
	if (IsLittleEndian()==true)
		offset= m_pExifMarker->TiffHeader_OffsetOfIFD_0 + ((m_pExifMarker->TiffHeader_OffsetOfIFD_1) << 16);
	else
		offset= m_pExifMarker->TiffHeader_OffsetOfIFD_1 + ((m_pExifMarker->TiffHeader_OffsetOfIFD_0) << 16);
	pIfd0 += offset;
	return(pIfd0);
}

char *ExifReader::GetIfd_Exif() const
//*************************************************************************************************************
{//Return specific IDF Exif pointer from IDF0
	return(GetIfd(eExifTag_IdfExifPointer));
}

char *ExifReader::GetIfd_Gps() const
//*************************************************************************************************************
{//Return specific IDF GPS pointer from IDF0
	return(GetIfd(eExifTag_IdfGpsPointer));
}


char *ExifReader::GetIfd_Interoperability() const
//*************************************************************************************************************
{//Return specific IDF Interoperability pointer from IDF0
	return(GetIfd(eExifTag_IdfInteroperabilityPointer));
}


char *ExifReader::GetIfd(unsigned int index) const
//*************************************************************************************************************
{//Return specific IDF pointer from IDF0
	char * const pIfd0= GetIfd0();
	char *pIfd= pIfd0;
	unsigned int count=0;
	ExifIfd_Manager Ifd;
	//Not found try special ptr
	if ( (index==eExifTag_IdfGpsPointer) || (index==eExifTag_IdfExifPointer) )
	{
		Ifd.SetBaseAddr(pIfd0, GetExifBase());
		ExifTag *pTag;
		pTag= Ifd.FindTag((unsigned short)index);
		if (pTag)
		{
			return(GetExifBase() + pTag->Value_Offset); //ifd found
		}
	}
	else if (index==eExifTag_IdfInteroperabilityPointer)
	{
		Ifd.SetBaseAddr(pIfd0, GetExifBase());
		ExifTag *pTag;
		pTag= Ifd.FindTag(eExifTag_IdfExifPointer);
		if (pTag)
		{
			Ifd.SetBaseAddr(GetExifBase() + pTag->Value_Offset, GetExifBase());
			pTag= Ifd.FindTag(eExifTag_IdfInteroperabilityPointer);
			if (pTag)
			{
				return(GetExifBase() + pTag->Value_Offset); //ifd found
			}
		}
	}
	pIfd= pIfd0;
	while (pIfd!=NULL)
	{
		if (index==count)
			return(pIfd);
		Ifd.SetBaseAddr(pIfd, GetExifBase());
		pIfd =(char *)Ifd.GetNextIfd();
		++count;
	}
	return(NULL); //Not found
}

int ExifReader::GetNbOfIfd() const
//*************************************************************************************************************
{
	char *pIfd= GetIfd0();
	unsigned short count=0;
	ExifIfd_Manager Ifd;
	while (pIfd!=NULL)
	{
		Ifd.SetBaseAddr(pIfd, GetExifBase());
		pIfd =(char *)Ifd.GetNextIfd();
		++count;
	}
	return((int)count);
}


size_t ExifReader::ComputeMaxOffset()
//*************************************************************************************************************
{ //Compute
	char *pIfd= GetIfd0();
	size_t CurentOffset=0, MaxOffset=0;
	ExifIfd_Manager Ifd;
	while (pIfd!=NULL)
	{
		Ifd.SetBaseAddr(pIfd, GetExifBase());
		CurentOffset= Ifd.GetEnd();
		pIfd =(char *)Ifd.GetNextIfd();
		if (CurentOffset > MaxOffset)
			MaxOffset=CurentOffset;
	}
	return((int)MaxOffset);
}

char * ExifReader::GetExifBase() const
//*************************************************************************************************************
{
	if (m_pExifMarker==NULL)
		return( NULL);
	return (char *)(&m_pExifMarker->TiffHeader_ByteOrder);
}

int ExifReader::DumpAll()
//*************************************************************************************************************
{
	char *pIfd0=GetIfd0();
	if (pIfd0==NULL)
		return(-1);
	ExifIfd_Manager Ifd(pIfd0, GetExifBase());
	unsigned short Ifd_index=0;
	unsigned int MaxOffset;
	while (pIfd0 !=0)
	{
		Ifd.SetBaseAddr(pIfd0, GetExifBase());
		EXIF_Log("\nIFD[%d] Entries=%d, Offset Start=%u Max=%u\n", Ifd_index, Ifd.GetNbTag(), Ifd.GetStartOffset(), Ifd.GetEnd());
		DumpIFD(Ifd);
		pIfd0 =(char *)Ifd.GetNextIfd();
		++Ifd_index;
	}

	pIfd0=GetIfd(eExifTag_IdfExifPointer);
	if (pIfd0!=NULL)
	{ //Dump EXIF IFD pointer
		Ifd.SetBaseAddr(pIfd0, GetExifBase());
		EXIF_Log("\nIfdExifPointer Entries=%d Offset Start=%u Max=%u\n", Ifd.GetNbTag(), Ifd.GetStartOffset(), Ifd.GetEnd());
		DumpIFD(Ifd, "IfdExifPointer - ");
	}
	pIfd0=GetIfd(eExifTag_IdfGpsPointer);
	if (pIfd0!=NULL)
	{ //Dump GPS IFD pointer
		Ifd.SetBaseAddr(pIfd0, GetExifBase());
		EXIF_Log("\nIdfGpsPointer Entries=%d Offset Start=%u Max=%u\n", Ifd.GetNbTag(), Ifd.GetStartOffset(), Ifd.GetEnd());
		DumpIFD(Ifd, "IdfGpsPointer - ");
	}

	pIfd0=GetIfd(eExifTag_IdfInteroperabilityPointer);
	if (pIfd0!=NULL)
	{ //Dump Interoperability IFD pointer
		Ifd.SetBaseAddr(pIfd0, GetExifBase());
		EXIF_Log("\nIdfInteroperabilityPointer Entries=%d Offset Start=%u Max=%u\n", Ifd.GetNbTag(), Ifd.GetStartOffset(), Ifd.GetEnd());
		DumpIFD(Ifd, "IdfInteroperabilityPointer - ");
	}
	MaxOffset=ComputeMaxOffset();
	EXIF_Log("\nMax offset=%u\n", MaxOffset);
	MaxOffset=MaxOffset;


	return(S_OK);
}

int ExifReader::DumpIFD(ExifIfd_Manager &Ifd, const char *Header) const
//*************************************************************************************************************
{
	unsigned short nbr;
	if (Header==NULL)
		Header="";
	ExifTag *pExifTag;
	nbr=Ifd.GetNbTag();
	pExifTag=Ifd.GetAt(0);
	for (unsigned short index=0; index < nbr; ++index)
	{
		EXIF_Log("%s%3d: ", Header, index);
		pExifTag->DisplayItem(GetExifBase());
		EXIF_Log("\n");
		++pExifTag;
	}
	return(S_OK);
}


ExifTag *ExifReader::FindTag(const char *name, char **ppIfd) const
//*************************************************************************************************************
{ //Find a tag by name. Return the address of the ifd if required
	const ExifTagDecription *pTag=GetExifTagDecription(name, eExif_UNKNOW);
	if (pTag!=NULL)
		return(FindTag(pTag->tag, ppIfd));
	else
		return(NULL);
}

#if 0
ExifTag *ExifReader::AddTag(const char *name, ExifIfd_Manager **pIfd) const
//*************************************************************************************************************
{ //Find a tag by name. Return the address of the ifd if required
	const ExifTagDecription *pTag=GetExifTagDecription(name);
	if (pTag!=NULL)
		return(FindTag(pTag->tag, pIfd));
	else
		return(NULL);
}


#endif

int ExifReader::ModifyTag(unsigned short tag, unsigned short type, unsigned short count, const char *pNewData, ExifTag **ppReturnTag)
//*************************************************************************************************************
{ //Find a tag by name. Return the address of the ifd if required
	char *pIfd;
	ExifTag *pTagOld=FindTag(tag, &pIfd);
	ExifTag *pTag=NULL;
	while (pTagOld!=NULL)
	{
		ExifTag NewTag;
		NewTag.Count       = count;
		NewTag.Type        = type;
		NewTag.Tag         = tag;
		NewTag.Value_Offset= 0;
		size_t size_new, size_old;
		size_new = NewTag.GetDataSize();
		size_old=pTagOld->GetDataSize();
		if (size_new != size_old)
		{
//			WOM_ASSERT(0);
		}
		size_t size;
		ExifPtrDataValue Ptr;
		pTagOld->GetDataAddress(GetExifBase(), Ptr, size);
		memcpy(Ptr.cValue, pNewData, size_new);
		pTagOld->Count = count;
		pTagOld->Type  = type;
		pTagOld->Tag   = tag;
		break;
	}
	if (ppReturnTag!=NULL)
		*ppReturnTag=pTag;
	if (pTag==NULL)
		return(-1);
	else
		return(S_OK);
}

ExifTag *ExifReader::FindTag(unsigned short tag, char **ppIfd) const
//*************************************************************************************************************
{ //iterate on each idf an try to retrieve current exit tag
	char *pIfd  = GetIfd0();
	unsigned short count=0;
	ExifIfd_Manager Ifd;
	ExifIfd_Manager Ifd0;
	Ifd0.SetBaseAddr(pIfd, GetExifBase()); //attach the new ifd
	ExifTag *pTag=NULL;
	//ExifPtrDataValue TagPtr;
	//size_t size;
	while (pIfd !=NULL)
	{
		Ifd.SetBaseAddr(pIfd, GetExifBase()); //attach the new ifd
		pTag=Ifd.FindTag(tag);
		if (pTag!=NULL)
			break;
		pIfd =(char *)Ifd.GetNextIfd();
		//Point to next ifd. ifd0 contains 2 other sections
		if (count < 4)
		{
			pIfd=NULL;
			if (count==0)
			{ //try to retreive eExifTag_IdfExifPointer = 34665, //From ifd0
				pTag= Ifd0.FindTag(eExifTag_IdfExifPointer);
				if (pTag)
				{
					pIfd=GetExifBase() + pTag->Value_Offset; //ifd found
				}
				else
					count=1;
			}
			if (count==1)
			{
				pTag= Ifd0.FindTag(eExifTag_IdfGpsPointer);
				if (pTag)
				{
					pIfd=GetExifBase() + pTag->Value_Offset; //ifd found
				}
				else
					count=2;
			}
			if (count==2)
			{
				pTag= Ifd0.FindTag(eExifTag_IdfInteroperabilityPointer);
				if (pTag)
				{
					pIfd=GetExifBase() + pTag->Value_Offset; //ifd found
				}
				else
					count=3;
			}
			if (pIfd==NULL)
				pIfd=(char *)Ifd0.GetNextIfd();
		}
		++count;
		pTag=NULL;
	}
	if (ppIfd!=NULL)
		*ppIfd=pIfd; //Return the ifd
	return(pTag);
}

