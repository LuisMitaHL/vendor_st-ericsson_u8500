/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/* \brief   Exif 
*/

#include "ite_exif.h"

#include <stdio.h>
#include <string.h>

#include <los/api/los_api.h>

TExifCheckElement g_CheckTablle[]={	
//Tags relating to image data structure
{256, ExifT_SHORT, 1, "ImageWidth "},
{256, ExifT_LONG, 1, "ImageWidth "},
{257, ExifT_SHORT, 1, "ImageLength "},
{257, ExifT_LONG, 1, "ImageLength "},
{258, ExifT_SHORT, 3, "BitsPerSample "},
{259, ExifT_SHORT, 1, "Compression "},
{262, ExifT_SHORT, 1, "PhotometricInterpretation "},
{274, ExifT_SHORT, 1, "Orientation "},
{277, ExifT_SHORT, 1, "SamplesPerPixel "},
{284, ExifT_SHORT, 1, "PlanarConfiguration "},
{530, ExifT_SHORT, 2, "YCbCrSubSampling "},
{531, ExifT_SHORT, 1, "YCbCrPositioning "},
{282, ExifT_RATIONAL, 1, "XResolution "},
{283, ExifT_RATIONAL, 1, "YResolution "},
{296, ExifT_SHORT, 1, "ResolutionUnit "},
//Tags relating to recording offset 
{273, ExifT_SHORT, 0, "StripOffsets "},
{273, ExifT_LONG, 0, "StripOffsets "},
{278, ExifT_SHORT, 1, "RowsPerStrip "},
{278, ExifT_LONG, 1, "RowsPerStrip "},
{279, ExifT_SHORT, 0, "StripByteCounts "},
{279, ExifT_LONG, 0, "StripByteCounts "},
{513, ExifT_LONG, 1, "JPEGInterchangeFormat "},
{514, ExifT_LONG, 1, "JPEGInterchangeFormatLength "},
//Tags relating to image data characteristics
{301, ExifT_SHORT, 3*256, "TransferFunction "},
{318, ExifT_RATIONAL, 2, "WhitePoint "},
{319, ExifT_RATIONAL, 6, "PrimaryChromaticities "},
{529, ExifT_RATIONAL, 3, "YCbCrCoefficients "},
{532, ExifT_RATIONAL, 6, "ReferenceBlackWhite "},
//Other tags
{306, ExifT_ASCII, 20, "DateTime "},
{270, ExifT_ASCII, 0, "ImageDescription "},
{271, ExifT_ASCII, 0, "Make "},
{272, ExifT_ASCII, 0, "Model "},
{305, ExifT_ASCII, 0, "Software "},
{315, ExifT_ASCII, 0, "Artist "},
{33432, ExifT_ASCII, 0, "Copyright "},
//Tags Relating to Version
{36864, ExifT_UNDEFINED, 4, "ExifVersion "},
{40960, ExifT_UNDEFINED, 4, "FlashpixVersion "},
//Tag Relating to Image Data Characteristics
{40961, ExifT_SHORT, 1, "ColorSpace "},
//Tags Relating to Image Configuration
{37121, ExifT_UNDEFINED, 4, "ComponentsConfiguration "},
{37122, ExifT_RATIONAL, 1, "CompressedBitsPerPixel "},
{40962, ExifT_LONG, 1, "PixelXDimension "},
{40962, ExifT_SHORT, 1, "PixelXDimension "},
{40963, ExifT_LONG, 1, "PixelYDimension "},
{40963, ExifT_SHORT, 1, "PixelYDimension "},
//Tags Relating to User Information 
{37500, ExifT_UNDEFINED, 0, "MakerNote "},
{37510, ExifT_UNDEFINED, 0, "UserComment "},
//Tag Relating to Related File Information
{40964, ExifT_ASCII, 13, "RelatedSoundFile "},
//Tags Relating to Date and Time
{36867, ExifT_ASCII, 20, "DateTimeOriginal "},
{36868, ExifT_ASCII, 20, "DateTimeDigitized "},
{37520, ExifT_ASCII, 0, "SubSecTime "},
{37521, ExifT_ASCII, 0, "SubSecTimeOriginal "},
{37522, ExifT_ASCII, 0, "SubSecTimeDigitized "},
//Tags Relating to Picture-Taking Conditions
{33434, ExifT_RATIONAL, 1, "ExposureTime "},
{33437, ExifT_RATIONAL, 1, "FNumber "},
{34850, ExifT_SHORT, 1, "ExposureProgram "},
{34852, ExifT_ASCII, 0, "SpectralSensitivity "},
{34855, ExifT_SHORT, 0, "ISOSpeedRatings "},
{34856, ExifT_UNDEFINED, 0, "OECF "},
{37377, ExifT_SRATIONAL, 1, "ShutterSpeedValue "},
{37378, ExifT_RATIONAL, 1, "ApertureValue "},
{37379, ExifT_SRATIONAL, 1, "BrightnessValue "},
{37380, ExifT_SRATIONAL, 1, "ExposureBiasValue "},
{37381, ExifT_RATIONAL, 1, "MaxApertureValue "},
{37382, ExifT_RATIONAL, 1, "SubjectDistance "},
{37383, ExifT_SHORT, 1, "MeteringMode "},
{37384, ExifT_SHORT, 1, "LightSource "},
{37385, ExifT_SHORT, 1, "Flash "},
{37386, ExifT_RATIONAL, 1, "FocalLength "},
{37396, ExifT_SHORT, 2, "SubjectArea "},
{37396, ExifT_SHORT, 3, "SubjectArea "},
{37396, ExifT_SHORT, 4, "SubjectArea "},
{41483, ExifT_RATIONAL, 1, "FlashEnergy "},
{41484, ExifT_UNDEFINED, 0, "SpatialFrequencyResponse "},
{41486, ExifT_RATIONAL, 1, "FocalPlaneXResolution "},
{41487, ExifT_RATIONAL, 1, "FocalPlaneYResolution "},
{41488, ExifT_SHORT, 1, "FocalPlaneResolutionUnit "},
{41492, ExifT_SHORT, 2, "SubjectLocation "},
{41493, ExifT_RATIONAL, 1, "ExposureIndex "},
{41495, ExifT_SHORT, 1, "SensingMethod "},
{41728, ExifT_UNDEFINED, 1, "FileSource "},
{41729, ExifT_UNDEFINED, 1, "SceneType "},
{41730, ExifT_UNDEFINED, 0, "CFAPattern "},
{41985, ExifT_SHORT, 1, "CustomRendered "},
{41986, ExifT_SHORT, 1, "ExposureMode "},
{41987, ExifT_SHORT, 1, "WhiteBalance "},
{41988, ExifT_RATIONAL, 1, "DigitalZoomRatio "},
{41989, ExifT_SHORT, 1, "FocalLengthIn35mmFilm "},
{41990, ExifT_SHORT, 1, "SceneCaptureType "},
//{41991, ExifT_RATIONAL, 1, "GainControl "}, // spec says once RATIONAL, once SHORT
{41991, ExifT_SHORT, 1, "GainControl "},
{41992, ExifT_SHORT, 1, "Contrast "},
{41993, ExifT_SHORT, 1, "Saturation "},
{41994, ExifT_SHORT, 1, "Sharpness "},
{41995, ExifT_UNDEFINED, 0, "DeviceSettingDescription "},
{41996, ExifT_SHORT, 1, "SubjectDistanceRange "},
//Tags Relating to GPS
{0, ExifT_BYTE, 4, "GPSVersionID "},
{1, ExifT_ASCII, 2, "GPSLatitudeRef "},
{2, ExifT_RATIONAL, 3, "GPSLatitude "},
{3, ExifT_ASCII, 2, "GPSLongitudeRef "},
{4, ExifT_RATIONAL, 3, "GPSLongitude "},
{5, ExifT_BYTE, 1, "GPSAltitudeRef "},
{6, ExifT_RATIONAL, 1, "GPSAltitude "},
{7, ExifT_RATIONAL, 3, "GPSTimeStamp "},
{8, ExifT_ASCII, 0, "GPSSatellites "},
{9, ExifT_ASCII, 2, "GPSStatus "},
{10, ExifT_ASCII, 2, "GPSMeasureMode "},
{11, ExifT_RATIONAL, 1, "GPSDOP "},
{12, ExifT_ASCII, 2, "GPSSpeedRef "},
{13, ExifT_RATIONAL, 1, "GPSSpeed "},
{14, ExifT_ASCII, 2, "GPSTrackRef "},
{15, ExifT_RATIONAL, 1, "GPSTrack "},
{16, ExifT_ASCII, 2, "GPSImgDirectionRef "},
{17, ExifT_RATIONAL, 1, "GPSImgDirection "},
{18, ExifT_ASCII, 0, "GPSMapDatum "},
{19, ExifT_ASCII, 2, "GPSDestLatitudeRef "},
{20, ExifT_RATIONAL, 3, "GPSDestLatitude "},
{21, ExifT_ASCII, 2, "GPSDestLongitudeRef "},
{22, ExifT_RATIONAL, 3, "GPSDestLongitude "},
{23, ExifT_ASCII, 2, "GPSDestBearingRef "},
{24, ExifT_RATIONAL, 1, "GPSDestBearing "},
{25, ExifT_ASCII, 2, "GPSDestDistanceRef "},
{26, ExifT_RATIONAL, 1, "GPSDestDistance "},
{27, ExifT_UNDEFINED, 0, "GPSProcessingMethod "},
{28, ExifT_UNDEFINED, 0, " GPSAreaInformation "},
{29, ExifT_ASCII, 11, "GPSDateStamp "},
{30, ExifT_SHORT, 1, "GPSDifferential "},
//Other Tags
{42016, ExifT_ASCII, 33, "ImageUniqueID "},
//Exif-specific IFD
{34665, ExifT_LONG, 1, "Exif IFD Pointer "},
{34853, ExifT_LONG, 1, "GPS IFD "},
{40965, ExifT_LONG, 1, "Interoperability IFD Pointer "},
 };

TExifError ite_exif_seek_tag(unsigned short tag, TExifIDType type, unsigned short count)
{
	TExifError errCode = ExifErrorTagNotFound;
	bool isFound = false;
	
	for(unsigned int i=0; i< sizeof(g_CheckTablle) / sizeof(TExifCheckElement); i++)
	{
		if(g_CheckTablle[i].tag == tag)
		{
			if(g_CheckTablle[i].type == type)
			{
				if(g_CheckTablle[i].count != 0)
				{
					if(g_CheckTablle[i].count == count)
					{
						isFound = true;
					}
				}
				else
				{
					isFound = true;
				}
				if(isFound == true)
				{
					LOS_Log("Exif info : tag found in our data base :)\n\tcomment : %s\n", g_CheckTablle[i].comment);
					errCode = ExifErrorNone;
					break;
				}
			}
		}
	}

	if(errCode != ExifErrorNone)
	{
		LOS_Log("Exif warning : no match between tag %d, type %d, count %d :(\n", tag, type, count);
	}

	return errCode;
}

TExifError ite_exif_IFD_struct_process_tag(char *pDataTag, char *pDataTiffHeader, unsigned int *pExifIFDPointer)
{
	TExifError errCode = ExifErrorNone;
	unsigned short *tmpU16 = (unsigned short *)pDataTag;
	unsigned short Tag = tmpU16[0];
	unsigned short tmType = tmpU16[1];
	unsigned char count_0 = pDataTag[4];
	unsigned char count_1 = pDataTag[5];
	unsigned char count_2 = pDataTag[6];
	unsigned char count_3 = pDataTag[7];
	unsigned int count = 0;
	unsigned char valueOffset_0 = pDataTag[8];
	unsigned char valueOffset_1 = pDataTag[9];
	unsigned char valueOffset_2 = pDataTag[10];
	unsigned char valueOffset_3 = pDataTag[11];
	unsigned int valueOffset = 0;
	unsigned int sizeOfTypeInBits = 0;
	unsigned short swappedTag = ((Tag&0xff)<<8) + ((Tag&0xff00)>>8);

	if(pExifIFDPointer == NULL)
	{
		LOS_Log("Exif error : ExifErrorBadParameter pExifIFDPointer\n");
		errCode = ExifErrorBadParameter;
		goto label_end;
	}

	LOS_Log("Tag %d = 0x%x, swappedTag = 0x%x\n", Tag, Tag, swappedTag);

	switch(tmType)
	{
		case ExifT_BYTE:
			LOS_Log("ExifT_BYTE\n");
			//An 8-bit unsigned integer 
			sizeOfTypeInBits = 8;
			break;
		case ExifT_ASCII:
			LOS_Log("ExifT_ASCII\n");
			//An 8-bit byte containing one 7-bit ASCII code. The final byte is terminated with NULL
			sizeOfTypeInBits = 8;
			break;
		case ExifT_SHORT:
			LOS_Log("ExifT_SHORT\n");
			//A 16-bit (2-byte) unsigned integer
			sizeOfTypeInBits = 16;
			break;
		case ExifT_LONG:
			LOS_Log("ExifT_LONG\n");
			//A 32-bit (4-byte) unsigned integer
			sizeOfTypeInBits = 32;
			break;
		case ExifT_RATIONAL:
			LOS_Log("ExifT_RATIONAL\n");
			//Two LONGs. The first LONG is the numerator and the second LONG expresses the denominator
			sizeOfTypeInBits = 64;
			break;
		case ExifT_UNDEFINED:
			LOS_Log("ExifT_UNDEFINED\n");
			//An 8-bit byte that can take any value depending on the field definition
			sizeOfTypeInBits = 8;
			break;
		case ExifT_SLONG:
			LOS_Log("ExifT_SLONG\n");
			//A 32-bit (4-byte) signed integer (2's complement notation)
			sizeOfTypeInBits = 32;
			break;
		case ExifT_SRATIONAL:
			LOS_Log("ExifT_SRATIONAL\n");
			//Two SLONGs. The first SLONG is the numerator and the second SLONG is the denominator
			sizeOfTypeInBits = 64;
			break;
		default:
			LOS_Log("Exif error : bad Type = %d = 0x%x\n", tmType, tmType);
			break;
	}
	//LOS_Log("count_0 %d = 0x%x\n", count_0, count_0);
	//LOS_Log("count_1 %d = 0x%x\n", count_1, count_1);
	//LOS_Log("count_2 %d = 0x%x\n", count_2, count_2);
	//LOS_Log("count_3 %d = 0x%x\n", count_3, count_3);
	count = (unsigned int)count_0 + (((unsigned int)count_1) << 8) + (((unsigned int)count_2) << 16) + (((unsigned int)count_3) << 24);
	LOS_Log("count %d = 0x%x\n", count, count);

	errCode = ite_exif_seek_tag(Tag, (TExifIDType)tmType, count);
	if(errCode != ExifErrorNone)
	{
		LOS_Log("Exif error : no match between tag %d, type %d, count %d\n", Tag, tmType, count);
		//goto label_end;
	}

	//LOS_Log("valueOffset_0 %d = 0x%x\n", valueOffset_0, valueOffset_0);
	//LOS_Log("valueOffset_1 %d = 0x%x\n", valueOffset_1, valueOffset_1);
	//LOS_Log("valueOffset_2 %d = 0x%x\n", valueOffset_2, valueOffset_2);
	//LOS_Log("valueOffset_3 %d = 0x%x\n", valueOffset_3, valueOffset_3);
	valueOffset = (unsigned int)valueOffset_0 + (((unsigned int)valueOffset_1) << 8) + (((unsigned int)valueOffset_2) << 16) + (((unsigned int)valueOffset_3) << 24);
	LOS_Log("valueOffset %d = 0x%x\n", valueOffset, valueOffset);

	if( Tag != 34665 )
	{
		LOS_Log("count * sizeOfTypeInBits %d = 0x%x\n", count * sizeOfTypeInBits, count * sizeOfTypeInBits);
		if(count * sizeOfTypeInBits > 32)
		{
			LOS_Log("Exif info : field is an offset : %d , 0x%x\n", valueOffset, valueOffset);
			switch(tmType)
			{
				case ExifT_BYTE:
					{
					//An 8-bit unsigned integer 
					unsigned char *tmpDat = (unsigned char *)( (char*)pDataTiffHeader + valueOffset);
					for(unsigned int i = 0; i < count ; i++)
					{
						LOS_Log("Exif info : BYTE : %d\n", tmpDat[i]);
					}
					break;
					}
				case ExifT_ASCII:
					{
					//An 8-bit byte containing one 7-bit ASCII code. The final byte is terminated with NULL
					char *tmpDat = (char *)( (char*)pDataTiffHeader + valueOffset);
					LOS_Log("Exif info : ASCII : %s\n", tmpDat);
					break;
					}
				case ExifT_SHORT:
					{
					//A 16-bit (2-byte) unsigned integer
					unsigned short *tmpDat = (unsigned short *)( (char*)pDataTiffHeader + valueOffset);
					for(unsigned int i = 0; i < count ; i++)
					{
						LOS_Log("Exif info : SHORT : %d\n", tmpDat[i]);
					}
					break;
					}
				case ExifT_LONG:
					{
					//A 32-bit (4-byte) unsigned integer
					unsigned long *tmpDat = (unsigned long *)( (char*)pDataTiffHeader + valueOffset);
					for(unsigned int i = 0; i < count ; i++)
					{
						LOS_Log("Exif info : LONG : %d\n", tmpDat[i]);
					}
					break;
					}
				case ExifT_RATIONAL:
					{
					//Two LONGs. The first LONG is the numerator and the second LONG expresses the denominator
					unsigned long *tmpDat = (unsigned long *)( (char*)pDataTiffHeader + valueOffset);
					for(unsigned int i = 0; i < count ; i++)
					{
						LOS_Log("Exif info : RATIONAL : %d / %d\n", tmpDat[2*i], tmpDat[2*i +1]);
					}
					break;
					}
				case ExifT_UNDEFINED:
					{
					//An 8-bit byte that can take any value depending on the field definition
					unsigned char *tmpDat = (unsigned char *)( (char*)pDataTiffHeader + valueOffset);
					for(unsigned int i = 0; i < count ; i++)
					{
						LOS_Log("Exif info : UNDEFINED : %d\n", tmpDat[i]);
					}
					break;
					}
				case ExifT_SLONG:
					{
					//A 32-bit (4-byte) signed integer (2's complement notation)
					signed long *tmpDat = (signed long *)( (char*)pDataTiffHeader + valueOffset);
					for(unsigned int i = 0; i < count ; i++)
					{
						LOS_Log("Exif info : SLONG : %d\n", tmpDat[i]);
					}
					break;
					}
				case ExifT_SRATIONAL:
					{
					//Two SLONGs. The first SLONG is the numerator and the second SLONG is the denominator
					signed long *tmpDat = (signed long *)( (char*)pDataTiffHeader + valueOffset);
					for(unsigned int i = 0; i < count ; i++)
					{
						LOS_Log("Exif info : SRATIONAL : %d / %d\n", tmpDat[2*i], tmpDat[2*i +1]);
					}
					break;
					}
				default:
					break;
			}
		}
		else
		{
			LOS_Log("Exif info : field is a value : %d\n", valueOffset);
		}
	}
	else
	{
		LOS_Log("Exif info : field is an offset pointer : %d , 0x%x\n", valueOffset, valueOffset);
		*pExifIFDPointer = valueOffset; 
	}

label_end:
	return errCode;
}

TExifError ite_exif_IFD_struct_process(char *pDataIFD, char *pDataTiffHeader, unsigned int *pNextIFDOffset)
{
	TExifError errCode = ExifErrorNone, errCode_2 = ExifErrorNone;
	unsigned short *tmpU16 = (unsigned short*)pDataIFD;
	unsigned short interoperabilityNumber_a = tmpU16[0];
	unsigned short interoperabilityNumber_b = 0;
	char *pCurrentData = (char*)NULL;
	unsigned int exifIFDPointer_a = 0, exifIFDPointer_b = 0, exifIFDPointer_backup = 0;
	unsigned char count_0 = 0, count_1 = 0, count_2 = 0, count_3 = 0;

	if(pNextIFDOffset == NULL)
	{
		LOS_Log("Exif error : ExifErrorBadParameter pNextIFDOffset\n");
		errCode = ExifErrorBadParameter;
		goto label_end;
	}

	LOS_Log("interoperabilityNumber_a %d = 0x%x\n", interoperabilityNumber_a, interoperabilityNumber_a);	
	pCurrentData = &pDataIFD[2];
	for(unsigned short i = 0; i < interoperabilityNumber_a; i++)
	{
		LOS_Log("\n\t processing tag %d / %d \n", i+1, interoperabilityNumber_a);
		exifIFDPointer_a = 0;
		errCode_2 = ite_exif_IFD_struct_process_tag(pCurrentData, pDataTiffHeader, &exifIFDPointer_a);
		if(errCode_2 != ExifErrorNone)
		{
			errCode = errCode_2;
		}
		if(exifIFDPointer_a != 0)
		{
			exifIFDPointer_backup = exifIFDPointer_a;
		}
		pCurrentData += (2+2+4+4);
	}

	count_0 = pCurrentData[0];
	count_1 = pCurrentData[1];
	count_2 = pCurrentData[2];
	count_3 = pCurrentData[3];
	*pNextIFDOffset = (unsigned int)count_0 + (((unsigned int)count_1) << 8) + (((unsigned int)count_2) << 16) + (((unsigned int)count_3) << 24);
	LOS_Log("Exif info : *pNextIFDOffset %d = 0x%x\n", *pNextIFDOffset, *pNextIFDOffset);
	if(*pNextIFDOffset == 0)
	{
		LOS_Log("Exif info : no other IFD (IFD1) to process");
	}
	else
	{
		LOS_Log("Exif info : another IFD (IFD1) to process");
	}

	if(exifIFDPointer_backup != 0)
	{
		LOS_Log("\n\t\t need to process Exif IFD.........\n");
		tmpU16 = (unsigned short *) ( (char *)pDataTiffHeader + exifIFDPointer_backup );
		interoperabilityNumber_b = tmpU16[0];
		LOS_Log("interoperabilityNumber_b %d = 0x%x\n", interoperabilityNumber_b, interoperabilityNumber_b);	
		pCurrentData = pDataTiffHeader + exifIFDPointer_backup + 2;
		for(unsigned short i = 0; i < interoperabilityNumber_b; i++)
		{
			LOS_Log("\n\t processing tag %d / %d \n", i+1, interoperabilityNumber_b);	
			exifIFDPointer_b = 0;
			errCode_2 = ite_exif_IFD_struct_process_tag(pCurrentData, pDataTiffHeader, &exifIFDPointer_b);
			if(errCode_2 != ExifErrorNone)
			{
				errCode = errCode_2;
			}
			pCurrentData += (2+2+4+4);
		}
	}

label_end :
	return errCode;
}


TExifError ite_parse_exif(void *pExifData)
{
	TExifAPP1Marker *handleApp1Marker = (TExifAPP1Marker*)pExifData;
	TExifError errorCode = ExifErrorNone;
	unsigned int TiffHeader_OffsetOfIFD = 0;
	char *pDataTiffHeader = (char*)NULL;
	char *pDataIFD = (char*)NULL;

	if(handleApp1Marker->MarkerPrefix != ITE_EXIF__APP1MARKER_MARKER_PREFIX)
	{
		LOS_Log("Exif error : MarkerPrefix\n");
		errorCode = ExifErrorParsing;
		goto label_end;
	}
	else
	{
		//LOS_Log("Exif ok : MarkerPrefix\n");
	}

	if(handleApp1Marker->APP1 != ITE_EXIF__APP1MARKER_APP1)
	{
		LOS_Log("Exif error : APP1\n");
		errorCode = ExifErrorParsing;
		goto label_end;
	}

	LOS_Log("Exif info : LengthOfField = %d = 0x%x\n", handleApp1Marker->LengthOfField, handleApp1Marker->LengthOfField);

	if(handleApp1Marker->ExifIndentifierCoder[0] != ITE_EXIF__APP1MARKER_EXIF_IDENTIFIER_CODER_0)
	{
		LOS_Log("Exif error : ExifIndentifierCoder[0]\n");
		errorCode = ExifErrorParsing;
		goto label_end;
	}
	if(handleApp1Marker->ExifIndentifierCoder[1] != ITE_EXIF__APP1MARKER_EXIF_IDENTIFIER_CODER_1)
	{
		LOS_Log("Exif error : ExifIndentifierCoder[1]\n");
		errorCode = ExifErrorParsing;
		goto label_end;
	}
	if(handleApp1Marker->ExifIndentifierCoder[2] != ITE_EXIF__APP1MARKER_EXIF_IDENTIFIER_CODER_2)
	{
		LOS_Log("Exif error : ExifIndentifierCoder[2]\n");
		errorCode = ExifErrorParsing;
		goto label_end;
	}
	if(handleApp1Marker->ExifIndentifierCoder[3] != ITE_EXIF__APP1MARKER_EXIF_IDENTIFIER_CODER_3)
	{
		LOS_Log("Exif error : ExifIndentifierCoder[3]\n");
		errorCode = ExifErrorParsing;
		goto label_end;
	}
	if(handleApp1Marker->ExifIndentifierCoder[4] != ITE_EXIF__APP1MARKER_EXIF_IDENTIFIER_CODER_4)
	{
		LOS_Log("Exif error : ExifIndentifierCoder[4]\n");
		errorCode = ExifErrorParsing;
		goto label_end;
	}

	switch(handleApp1Marker->TiffHeader_ByteOrder)
	{
		case ITE_EXIF__TIFF_HEADER_BYTE_ORDER_LITTLE_ENDIAN:
			LOS_Log("Exif info : TiffHeader.ByteOrder LITTLE_ENDIAN\n");break;
		case ITE_EXIF__TIFF_HEADER_BYTE_ORDER_BIG_ENDIAN:
			LOS_Log("Exif info : TiffHeader.ByteOrder BIG_ENDIAN\n");break;
		default:
			LOS_Log("Exif error : TiffHeader.ByteOrder\n");
			errorCode = ExifErrorParsing;
			goto label_end;
	}

	if(handleApp1Marker->TiffHeader_filed_42 != ITE_EXIF__TIFF_HEADER_BYTE_42)
	{
		LOS_Log("Exif error : TiffHeader.filed_42\n");
		errorCode = ExifErrorParsing;
		goto label_end;
	}

	//LOS_Log("Exif info : TiffHeader_OffsetOfIFD_0 = %d\n", handleApp1Marker->TiffHeader_OffsetOfIFD_0 );
	//LOS_Log("Exif info : TiffHeader_OffsetOfIFD_1 = %d\n", handleApp1Marker->TiffHeader_OffsetOfIFD_1 );

	TiffHeader_OffsetOfIFD = handleApp1Marker->TiffHeader_OffsetOfIFD_0 + (handleApp1Marker->TiffHeader_OffsetOfIFD_1 << 16);
	LOS_Log("Exif info : TiffHeader_OffsetOfIFD = %d = 0x%x\n", TiffHeader_OffsetOfIFD, TiffHeader_OffsetOfIFD );
	if(TiffHeader_OffsetOfIFD == ITE_EXIF__TIFF_HEADER_BYTE_OFFSET_OF_IFD)
	{
		//LOS_Log("Exif info : the TIFF header is followed immediately by the 0th IFD"); 
	}

	pDataTiffHeader = (char*)pExifData;
	pDataTiffHeader += (1+1+2+6);

	pDataIFD = pDataTiffHeader;
	pDataIFD += TiffHeader_OffsetOfIFD;
	{
		unsigned int exifIFDPointer = 0;
		errorCode = ite_exif_IFD_struct_process(pDataIFD, pDataTiffHeader, &exifIFDPointer);
		if(exifIFDPointer == 0)
		{
			LOS_Log("Exif info : no other IFD to process ... so it is finised\n");
		}
		else
		{
			LOS_Log("Exif info : another IFD to process ... so do it\n");
		}
		if(errorCode != ExifErrorNone)
		{
			LOS_Log("Exif error : ite_exif_IFD_struct_process\n");
			goto label_end;
		}
	}

label_end :

	return errorCode;
}

