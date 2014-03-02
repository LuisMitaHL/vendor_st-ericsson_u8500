/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#include "exif_mixer/arm_nmf/mixer_core.nmf"
#include "mixer_core.hpp"
#include <stdio.h>
#include "OMX_Component.h"
#include <string.h>
#include "mixer_core.hpp"

#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
	#include "video_components_exif_mixer_arm_nmf_mixer_core_src_mixer_coreTraces.h"
#endif //for OST_TRACE_COMPILER_IN_USE

#ifdef PACKET_VIDEO_SUPPORT
#define LOG_TAG "exif_mixer"
#include<cutils/log.h>
#endif


#define WORD_SIZE 4

#define ENDIANCONVERT16(a) ((a >> 8) | ((a & 0xff) << 8))
#define ENDIANCONVERT32(a) ((a >> 24) | ((a & 0x00FF0000) >> 8) | ((a & 0xFF00) << 8) | ((a & 0xff) << 24))

#define OSTTRACE_ERROR(error) \
			if (error) \
			{ \
				exif_mixer_armnmf_assert((0==1), error, __LINE__, OMX_FALSE); \
			}


#define CHECKERROR_SENDEVENT(error) \
        if (OMX_ErrorNone != error) \
        proxy.eventHandler(OMX_EventError,(OMX_U32)error,1);


void exif_mixer_arm_nmf_mixer_core::exif_mixer_armnmf_assert(OMX_U32 condition, OMX_U32 errorType, OMX_U32 line, OMX_BOOL isFatal)
{
    if (!condition)
    {
        OstTraceFiltInst2(TRACE_ERROR, "EXIFMIXER_ARMNMF: errorType : 0x%x error line %d\n", errorType,line);
        if (isFatal)
        {
            DBC_ASSERT(0==1);
        }
    }
}


void exif_mixer_arm_nmf_mixer_core::disablePortIndication(t_uint32 portIdx)
{
	OstTraceFiltInst1(TRACE_DEBUG,"exif_mixer_arm_nmf_mixer_core::disablePortIndication<at line %d>",__LINE__);
}

void exif_mixer_arm_nmf_mixer_core::enablePortIndication(t_uint32 portIdx)
{
	OstTraceFiltInst1(TRACE_DEBUG,"exif_mixer_arm_nmf_mixer_core::enablePortIndication<at line %d>",__LINE__);
}

void exif_mixer_arm_nmf_mixer_core::flushPortIndication(t_uint32 portIdx)
{
	OstTraceFiltInst1(TRACE_DEBUG,"exif_mixer_arm_nmf_mixer_core::flushPortIndication<at line %d>",__LINE__);
}
void METH(reset)()
{
	OstTraceFiltInst1(TRACE_API,"exif_mixer_arm_nmf_mixer_core::reset<at line %d>",__LINE__);
}

void exif_mixer_arm_nmf_mixer_core::start()
{
}

void exif_mixer_arm_nmf_mixer_core::finish(void)
{
		OstTraceFiltInst1(TRACE_API,"exif_mixer_arm_nmf_mixer_core::finish<at line %d>",__LINE__);
}


OMX_ERRORTYPE EXIFParser::ReadByte(OMX_U8 *pChar)
{
	*pChar = exifHeaderBuffer[currentPosition];

	currentPosition++;
	if (currentPosition >= headerSize)
	{
		return OMX_ErrorNoMore;
	}
	else
	{
		return OMX_ErrorNone;
	}
}

EXIFParser::EXIFParser()
{
	save_current = 0;
	currentPosition = 0;
	thumbnailPosition = 0;
	TotalDirEntries = 0;
	headerCorrectionReq = OMX_FALSE;
	headerSize = 0;
	exifHeaderBuffer = 0;
	app1StartPos = 0;
	ifd1_offset = 0;
	consider_endianness = OMX_FALSE;
	exif_size_location = 0;
	start_of_exif_offset = 0;
	start_of_ifd0 = 0;
	isEXIFsegmentempty = OMX_FALSE;
	bytes_data = 24;//Default if no GPS tag to be inserted, no Thumbnail to be inserted, and no Extradata is there
	endianness_flag = OMX_FALSE;
	gpslocation.bAltitudeRefAboveSea = OMX_FALSE;
	gpslocation.bLatitudeRefNorth = OMX_FALSE;
	gpslocation.bLocationKnown = OMX_FALSE;
	gpslocation.bLongitudeRefEast = OMX_FALSE;
	gpslocation.nAltitudeMeters = 0;
	gpslocation.nLatitudeDegrees = 0;
	gpslocation.nLatitudeMinutes = 0;
	gpslocation.nLatitudeSeconds = 0;
	gpslocation.nLongitudeDegrees = 0;
	gpslocation.nLongitudeMinutes = 0;
	gpslocation.nLongitudeSeconds = 0;
	gpslocation.nHours = 0;
	gpslocation.nMinutes = 0;
	gpslocation.nSeconds = 0;
	/* ER 429860 */
	gpslocation.nYear = 0;
	gpslocation.nMonth = 0;
	gpslocation.nDay = 0;
	/* ER 429860 */
	gpslocation.nSize = 0;
	gpslocation.nPortIndex = 0;
	gpslocation.processingValue = (OMX_KEY_GPS_PROCESSING_METHODTYPE)0;
	gps_processingMethod_len = 0;
	for(int as=0;as<SIZE_GPS_PR_MTD;as++)
	{
		gps_processing_ascii[as]=0x00;
		gpslocation.nPrMethodDataValue[as]=0;
	}
	globalPtr = 0;
	is_ifd1_tag = OMX_FALSE;
	thumbnail_size  = 0;
	// +ER 349081
	ort_type	= 0;
	ort_comps	= 0;
	ort_data	= 0;
	// -ER 349081
	valid_tag=0;
	valid_offset=0;
}


OMX_ERRORTYPE EXIFParser::ReadTwoByte(OMX_U16 *pHalfWord)
{
	OMX_ERRORTYPE error;
	OMX_U8 tempByte;
	error = ReadByte(&tempByte);
	*pHalfWord = tempByte;
	*pHalfWord <<=8;
	error = ReadByte(&tempByte);
	*pHalfWord |= tempByte;

	return error;
}


/*
Assumption : used only to find 2-byte codes
It would place the current Position just after the code
return codes:
OMX_ErrorNoMore : code found but not it is end of bitstream
OMX_ErrorNone   : code found and many bytes left (GOOD case)
OMX_ErrorStreamCorrupt : end of bitstream and code NOT found
*/
OMX_ERRORTYPE EXIFParser::GetNextEXIFStartCode(OMX_U16 *aStartCodeIndex)
{
/* This function if used for finding out 2-byte codes could be header codes
   or IFD tags
*/
   OstTraceFiltInst2(TRACE_API,"Inside GetNextEXIFStartCode Line no. currentPosition : 0x%x at Line no. %d",currentPosition,__LINE__);
   OMX_U16 tempdata;
   OMX_ERRORTYPE error = OMX_ErrorNone;
   OMX_BOOL found = OMX_FALSE;

   if ((currentPosition+1) >= headerSize)
   {
   		   error = OMX_ErrorNoMore;
   }

	while ((OMX_ErrorNone == error) && (OMX_FALSE == found))
	{
        error = ReadTwoByte(&tempdata);

	    if ((consider_endianness) && (!endianness_flag))
		{
		   tempdata = ENDIANCONVERT16(tempdata);
   		}

   		switch (tempdata)
   		{
			case APP1_START :
							OstTraceFiltInst1(TRACE_DEBUG,"EXIFParser::GetNextEXIFStartCode - Appl segment TAG found at %d ",__LINE__);
	                      	  found = OMX_TRUE;
	                      	  *aStartCodeIndex = APP1_START;
		                      break;

	   		case ImageWidth :
	   						OstTraceFiltInst1(TRACE_DEBUG,"EXIFParser::GetNextEXIFStartCode - ImageWidth TAG found at %d ",__LINE__);
   		                  	  found = OMX_TRUE;
   		                  	  *aStartCodeIndex = ImageWidth;
	   		                  break;

	   		case ImageLength :
	   						OstTraceFiltInst1(TRACE_DEBUG,"EXIFParser::GetNextEXIFStartCode - ImageLength TAG found at %d ",__LINE__);
	   						   *aStartCodeIndex = ImageLength;
	   		                   found = OMX_TRUE;
	   		                   break;

	   		case Compression :
	   						OstTraceFiltInst1(TRACE_DEBUG,"EXIFParser::GetNextEXIFStartCode - Compression TAG found at %d ",__LINE__);
	   		    			   *aStartCodeIndex = Compression;
	   		 		           found = OMX_TRUE;
	   		                   break;

	   		case JPEGInterchangeFormat :
	   							OstTraceFiltInst1(TRACE_DEBUG,"EXIFParser::GetNextEXIFStartCode - JpegIFOffset TAG found at %d ",__LINE__);
	   		                    *aStartCodeIndex = JPEGInterchangeFormat;
	   		                  	found = OMX_TRUE;
	   		                    break;

	   		case JPEGInterchangeFormatLength :
	   							OstTraceFiltInst1(TRACE_DEBUG,"EXIFParser::GetNextEXIFStartCode - JpegIFByteCount TAG found at %d ",__LINE__);
	   							   *aStartCodeIndex = JPEGInterchangeFormatLength;
	   		                  	   found = OMX_TRUE;
	   		                       break;

	   	    case GPS_TAG       :
	   							OstTraceFiltInst1(TRACE_DEBUG,"EXIFParser::GetNextEXIFStartCode - GPS TAG found at %d ",__LINE__);
	   	    					 *aStartCodeIndex = GPS_TAG;
	   		                  	 found = OMX_TRUE;
	   		                     break;
			case Orientation :
	   							OstTraceFiltInst1(TRACE_DEBUG,"EXIFParser::GetNextEXIFStartCode - Orientation TAG found at %d ",__LINE__);
	   							   *aStartCodeIndex = Orientation;
	   		                  	   found = OMX_TRUE;
	   		                       break;
			case XResolution :
	   							OstTraceFiltInst1(TRACE_DEBUG,"EXIFParser::GetNextEXIFStartCode - XResolution TAG found at %d ",__LINE__);
	   							   *aStartCodeIndex = XResolution;
	   		                  	   found = OMX_TRUE;
	   		                       break;
			case YResolution :
	   							OstTraceFiltInst1(TRACE_DEBUG,"EXIFParser::GetNextEXIFStartCode - YResolution TAG found at %d ",__LINE__);
	   							   *aStartCodeIndex = YResolution;
	   		                  	   found = OMX_TRUE;
	   		                       break;
			case Resolution_Unit :
	   							OstTraceFiltInst1(TRACE_DEBUG,"EXIFParser::GetNextEXIFStartCode - Resolution_Unit TAG found at %d ",__LINE__);
	   							   *aStartCodeIndex = Resolution_Unit;
	   		                  	   found = OMX_TRUE;
	   		                       break;

		}
   	}

   	/* if end of bitstream but code NOT found */
   	if ((OMX_ErrorNoMore == error) && (OMX_FALSE == found))
   	{
		error = OMX_ErrorStreamCorrupt;
	}
	OstTraceFiltInst1(TRACE_API,"GetNextEXIFStartCode DONE Line no. <%d> ",__LINE__);
    return error;
}


/*
	This function will put GPS tags based on the global values
	and it will put the offset position of IFD1 tag
*/
OMX_ERRORTYPE EXIFParser::createGPStagposition(OMX_BOOL create_gps_tag,OMX_U32 loc_gps_dir)
{
	OMX_U32 tempPosition;
	OMX_U32 tempOffsetPosition=0;
	OMX_U32 counter_gps=0;
	OMX_U32 tempPosition_IFD1;
	OMX_U32 dynamic_gps_key[5][2] = {
									{1,3},
									{1,3},
									{1,1},
									{1,3},
									{1,gps_processingMethod_len+8}
								};
	OstTraceFiltInst1(TRACE_API,"Inside createGPStagposition => Entering function <%d>",__LINE__);

	//only 1 directory entry will be created and following info is to be wriiten
	//there are 11 directory entriews reqiuired for GPS structure

	//Checking valid keys
	valid_tag = NO_GPS_TAGS;
	valid_offset = (sizeof(OMX_U64)*11)+(sizeof(OMX_U8)*(SIZE_DATA_MAPUM + SIZE_DATE_STAMP))+gps_processingMethod_len+8;
	
	if((gpslocation.nLatitudeDegrees==0)&&(gpslocation.nLatitudeMinutes==0)&&(gpslocation.nLatitudeSeconds==0))
	{
		valid_tag = valid_tag -2;
		dynamic_gps_key[0][0]=0;
		valid_offset = valid_offset - dynamic_gps_key[0][1];
	}
	if((gpslocation.nLongitudeDegrees==0)&&(gpslocation.nLongitudeMinutes==0)&&(gpslocation.nLongitudeSeconds==0))
	{
		valid_tag = valid_tag -2;
		dynamic_gps_key[1][0]=0;
		valid_offset = valid_offset - dynamic_gps_key[1][1];
	}
	if(gpslocation.nAltitudeMeters==0)
	{
		valid_tag = valid_tag -2;
		dynamic_gps_key[2][0]=0;
		valid_offset = valid_offset - dynamic_gps_key[2][1];
	}
	if((gpslocation.nHours==0)&&(gpslocation.nMinutes==0)&&(gpslocation.nSeconds==0))
	{
		valid_tag = valid_tag -1;
		dynamic_gps_key[3][0]=0;
		valid_offset = valid_offset - dynamic_gps_key[3][1];
	}
	if(gpslocation.processingValue==0)//In case processingValue has 0 value (OMX_Undefined_Format) then this tag is invalid
	{
		valid_tag = valid_tag -1;
		dynamic_gps_key[4][0]=0;
		valid_offset = valid_offset - dynamic_gps_key[4][1];
	}
	else
		dynamic_gps_key[4][1]=gps_processingMethod_len+8;

    /* ER 429860 */
    if(!gpslocation.nYear || !gpslocation.nMonth || !gpslocation.nDay)
    {
        valid_offset = valid_offset - SIZE_DATE_STAMP;
        valid_tag--;
    }
    /* ER 429860 */

	counter_gps = valid_tag; //will be used to calculate offsets for different GPS_TAG's
		
	//Checking done
	OstTraceFiltInst2(TRACE_FLOW,"createGPStagposition::Total no. of valid tag : %d  and total offset : %d>",valid_tag,valid_offset);
	
	tempPosition = start_of_ifd0; //placing the ptr at start of IFD0 tag
	if(create_gps_tag)
	{
		OstTraceFiltInst1(TRACE_FLOW,"createGPStagposition => EXIF header is empty and user wants to insert IFD0 and GPS tag <%d>",__LINE__);
		write_entry_in_bitstream(tempPosition,0x0001); //writing the number of directory entries i.e 1
		tempPosition +=2;
	
		OstTraceFiltInst1(TRACE_FLOW,"createGPStagposition::Writing the GPS Offset tag entry <line no %d>",__LINE__);
		//writing 0x0001 in bitstream to denote only one directory entry
		//only ONE entry for GPS offset
		write_entry_in_bitstream(tempPosition,0x8825); //writing the Tag number
		tempPosition +=2;
		write_entry_in_bitstream(tempPosition,0x0004); //writing the data format
		tempPosition +=2;
		write_in_bitstream(tempPosition,0x00000001); //writing the number of component
		tempPosition +=4;
		write_in_bitstream(tempPosition,((tempPosition+8)-app1StartPos)); //writing the offset to actual GPS tags
		tempPosition +=4;
	
		//writing the position for IFD1 start position
		//computing the value of actual starting Position for IFD1
		//tempPosition is the current position
		//4 is used to compensate for IFD1 offset 4 bytes i.e size occupied by itself
		//(ENTRY_SIZE*NO_GPS_TAGS) is the number of directory entries - for GPS
		//2 is used to denote number of directory entries for GPS Offset segment
		//app1StartPos is the offset so should be subtracted from current position
		OstTraceFiltInst2(TRACE_FLOW,"createGPStagposition::For Position of IFD1 ## tempPosition : %d + Const : (4+2) +  <at line %d>",tempPosition,__LINE__);
		OstTraceFiltInst2(TRACE_FLOW,"createGPStagposition::For Position of IFD1 ## + DirectoryEntries : %d <at line %d> ",(ENTRY_SIZE*valid_tag),__LINE__);
		OstTraceFiltInst2(TRACE_FLOW,"createGPStagposition::For Position of IFD1 ## +  valid_offset : %d <at line %d>",valid_offset,__LINE__);
	
		//tempOffsetPosition = tempPosition+4+2+(ENTRY_SIZE*NO_GPS_TAGS)+(sizeof(OMX_U64)*10)+(sizeof(OMX_U8)*(SIZE_DATA_MAPUM + SIZE_DATE_STAMP))+gps_processingMethod_len+8;//gps_processingMethod_len+8 is data size of GPSProcessingMethod
		/* ER 451344 */
		tempOffsetPosition = tempPosition + 4 + 2 + (ENTRY_SIZE * valid_tag) + valid_offset + 4; /* + 4 for no next IFD */
		/* ER 451344 */
		
		tempOffsetPosition +=(OMX_U32)check_odd_even(&tempOffsetPosition);
		
		OstTraceFiltInst2(TRACE_DEBUG,"createGPStagposition::Value of IFD1 offset : %d <at line %d>",(tempOffsetPosition - app1StartPos),__LINE__);
	//	write_in_bitstream(tempPosition,tempOffsetPosition - app1StartPos); //writing the offset to IFD1 section	
		tempPosition_IFD1= tempPosition;
		tempPosition +=4;
	}
	else
	{
		OstTraceFiltInst2(TRACE_FLOW,"createGPStagposition, EXIF header is not empty, IFD0 is already inserted and user wants to insert GPS tag at 0x%x Line <%d>",loc_gps_dir,__LINE__);
		tempPosition = loc_gps_dir;
	}
	OstTraceFiltInst2(TRACE_FLOW,"createGPStagposition::Writing the number of directory entry : %d <at line %d>",valid_tag,__LINE__);
	write_entry_in_bitstream(tempPosition,valid_tag); //NO_GPS_TAGS entries as shown above
	tempPosition +=2;

	OstTraceFiltInst1(TRACE_DEBUG,"createGPStagposition::Writing the GPS Entries <line %d> ",__LINE__);
	OstTraceFiltInst2(TRACE_DEBUG,"createGPStagposition::Absolute position for writing GPSVersion : %d <line %d>",tempPosition,__LINE__);

	//1st TAG writing GPS Version
	write_entry_in_bitstream(tempPosition,0x0000); //writing the Version Tag number
	tempPosition +=2;
	write_entry_in_bitstream(tempPosition,0x0001); //writing the number of components
	tempPosition +=2;
	write_in_bitstream(tempPosition,0x00000004); //writing the bytes/component
	tempPosition +=4;
	write_in_bitstream(tempPosition,0x00000202); //writing the actual data
	tempPosition +=4;

	counter_gps--;

	if(dynamic_gps_key[0][0])
	{
		OstTraceFiltInst2(TRACE_DEBUG,"createGPStagposition::Absolute position for writing GPSLatitudeRef : %d <line %d>",tempPosition,__LINE__);
		write_entry_in_bitstream(tempPosition,0x0001); //writing the GPSLatitudeRef Tag number
		tempPosition +=2;
		write_entry_in_bitstream(tempPosition,0x0002); //writing the number of components
		tempPosition +=2;
		write_in_bitstream(tempPosition,0x00000002); //writing the bytes/component
		tempPosition +=4;
	
		switch (gpslocation.bLatitudeRefNorth)
		{
				case OMX_FALSE :
						OstTraceFiltInst1(TRACE_DEBUG,"createGPStagposition::Writing SOUTH_LATITUDE in GPSLatitudeRef <line %d>",__LINE__);
						write_in_bitstream(tempPosition,0x00000053); //writing the actual data
						break;
	
				case OMX_BOOL_MAX:
				case OMX_TRUE:
						OstTraceFiltInst1(TRACE_DEBUG,"createGPStagposition::Writing NORTH_LATITUDE in GPSLatitudeRef <line %d>",__LINE__);
					write_in_bitstream(tempPosition,0x0000004E); //writing the actual data
					break;
		}
		tempPosition +=4;

		OstTraceFiltInst2(TRACE_DEBUG,"createGPStagposition::Absolute position for writing GPSLatitudeTAG : %d <line %d>",tempPosition,__LINE__);
		//writing GPSLatitude
		write_entry_in_bitstream(tempPosition,0x0002); //writing the GPSLatitude number
		tempPosition +=2;
		write_entry_in_bitstream(tempPosition,0x0005); //writing the number of components
		tempPosition +=2;
		write_in_bitstream(tempPosition,0x00000003); //writing the bytes/component
		tempPosition +=4;
		//8 more directory entries to follow after this i.e
		//(1)GPSLongitudeRef
		//(2)GPSLongitude
		//(3)GPSAltitudeRef
		//(4)GPSAltitude
		//(5)GPSTimeStamp
		//(6)GPSMapDatum
		//(7)GPSDateStamp
		//(8)GPSProcessingMethod
		
		counter_gps -=2; //decreased by two as two tags are already written

		OMX_U32 offset_latitude = (ENTRY_SIZE*(counter_gps));
		/* ER 451344 */
		tempOffsetPosition = tempPosition + 4 + offset_latitude - app1StartPos + 4;
		/* ER 451344 */
		
		if(check_odd_even(&tempOffsetPosition))
		{
			tempOffsetPosition++;
			val_take ++;
		}

		write_in_bitstream(tempPosition,tempOffsetPosition); //writing the offset value to actual data
		tempPosition +=4;
		
		tempOffsetPosition += app1StartPos;
		//now writing actual GPSLatitude data
		//tempOffsetPosition = tempPosition + offset_latitude;
	
		//tempOffsetPosition +=(OMX_U32)check_odd_even(&tempOffsetPosition);
		
	
		OstTraceFiltInst2(TRACE_DEBUG,"createGPStagposition::Absolute position for writing GPSLatitude : %d <line %d>",tempOffsetPosition,__LINE__);
		write_in_bitstream(tempOffsetPosition,((gpslocation.nLatitudeDegrees>>32) & 0xFFFFFFFF)); //Latitude Degrees
		tempOffsetPosition +=4;
		write_in_bitstream(tempOffsetPosition,(gpslocation.nLatitudeDegrees & 0xFFFFFFFF)); //Writing the denominator
		tempOffsetPosition +=4;
	
		write_in_bitstream(tempOffsetPosition,((gpslocation.nLatitudeMinutes>>32) & 0xFFFFFFFF)); //Latitude Minutes
		tempOffsetPosition +=4;
		write_in_bitstream(tempOffsetPosition,(gpslocation.nLatitudeMinutes & 0xFFFFFFFF)); //Writing the denominator
		tempOffsetPosition +=4;
	
		write_in_bitstream(tempOffsetPosition,((gpslocation.nLatitudeSeconds>>32) & 0xFFFFFFFF)); //Latitude Seconds
		tempOffsetPosition +=4;
		write_in_bitstream(tempOffsetPosition,(gpslocation.nLatitudeSeconds & 0xFFFFFFFF)); //Writing the denominator
			
	}

	if(dynamic_gps_key[1][0])
	{
		OstTraceFiltInst2(TRACE_DEBUG,"createGPStagposition::Absolute position for writing GPSLongitudeRef : %d <at line %d>",tempPosition,__LINE__);
		write_entry_in_bitstream(tempPosition,0x0003); //writing the GPSLatitudeRef Tag number
		tempPosition +=2;
		write_entry_in_bitstream(tempPosition,0x0002); //writing the number of components
		tempPosition +=2;
		write_in_bitstream(tempPosition,0x00000002); //writing the bytes/component
		tempPosition +=4;
	
		switch (gpslocation.bLongitudeRefEast)
		{
				case OMX_FALSE :
						OstTraceFiltInst1(TRACE_DEBUG,"createGPStagposition::Writing WEST in GPSLongitudeRef <at line %d>",__LINE__);
						write_in_bitstream(tempPosition,0x00000057); //writing the actual data
						break;
	
				case OMX_BOOL_MAX:
				case OMX_TRUE:
						OstTraceFiltInst1(TRACE_DEBUG,"createGPStagposition::Writing EAST in GPSLongitudeRef <at line %d>",__LINE__);
					write_in_bitstream(tempPosition,0x00000045); //writing the actual data
					break;
		}
		tempPosition +=4;
	
		OstTraceFiltInst2(TRACE_DEBUG,"createGPStagposition::Absolute position for writing GPSLongitudeTAG : %d <at line %d>",tempPosition,__LINE__);
	
		//writing GPSLongitude
		write_entry_in_bitstream(tempPosition,0x0004); //writing the GPSLongitude number
		tempPosition +=2;
		write_entry_in_bitstream(tempPosition,0x0005); //writing the number of components
		tempPosition +=2;
		write_in_bitstream(tempPosition,0x00000003); //writing the bytes/component
		tempPosition +=4;
		//6 more directory entries to follow after this i.e
		//(1)GPSAltitudeRef
		//(2)GPSAltitude
		//(3)GPSTimeStamp
		//(4)GPSMapDatum
		//(5)GPSDateStamp
		//(6)GPSProcessingMethod
		//(7) Also account for the THREE OMX_U64 values already writen by Latitude
				
		counter_gps -=2; //decreased by two as two tags are already written

		OMX_U32 offset_longitude = (ENTRY_SIZE*(counter_gps)) + (sizeof(OMX_U64)*(dynamic_gps_key[0][0]*dynamic_gps_key[0][1]));

		/* ER 451344 */
		tempOffsetPosition = tempPosition + 4 + offset_longitude - app1StartPos + 4;
		/* ER 451344 */
		
		if(check_odd_even(&tempOffsetPosition))
		{
			tempOffsetPosition++;
			val_take ++;
		}

		write_in_bitstream(tempPosition,tempOffsetPosition); //writing the offset value to actual data
		tempPosition +=4;
		tempOffsetPosition += app1StartPos;
	
		//now writing actual GPSLongitude data
		//tempOffsetPosition = tempPosition + offset_longitude;
	
		//tempOffsetPosition +=(OMX_U32)check_odd_even(&tempOffsetPosition);

		OstTraceFiltInst2(TRACE_DEBUG,"createGPStagposition::Absolute position for writing GPSLongitude : %d <at line %d>",tempOffsetPosition,__LINE__);
	
		write_in_bitstream(tempOffsetPosition,((gpslocation.nLongitudeDegrees>>32) & 0xFFFFFFFF)); //nLongitudeDegrees
		tempOffsetPosition +=4;
		write_in_bitstream(tempOffsetPosition,(gpslocation.nLongitudeDegrees & 0xFFFFFFFF)); //Writing the denominator
		tempOffsetPosition +=4;
	
		write_in_bitstream(tempOffsetPosition,((gpslocation.nLongitudeMinutes>>32) & 0xFFFFFFFF)); //nLongitudeMinutes
		tempOffsetPosition +=4;
		write_in_bitstream(tempOffsetPosition,(gpslocation.nLongitudeMinutes & 0xFFFFFFFF)); //Writing the denominator
		tempOffsetPosition +=4;
	
		write_in_bitstream(tempOffsetPosition,((gpslocation.nLongitudeSeconds>>32) & 0xFFFFFFFF)); //nLongitudeSeconds
		tempOffsetPosition +=4;
		write_in_bitstream(tempOffsetPosition,(gpslocation.nLongitudeSeconds & 0xFFFFFFFF)); //Writing the denominator
		
	}

	if(dynamic_gps_key[2][0])
	{
		OstTraceFiltInst2(TRACE_DEBUG,"createGPStagposition::Absolute position for writing GPSAltitudeRef : %d <at line %d>",tempPosition,__LINE__);
		write_entry_in_bitstream(tempPosition,0x0005); //writing the GPSAltitudeRef Tag number
		tempPosition +=2;
		write_entry_in_bitstream(tempPosition,0x0001); //writing the number of components
		tempPosition +=2;
		write_in_bitstream(tempPosition,0x00000001); //writing the bytes/component
		tempPosition +=4;
	
		switch (gpslocation.bAltitudeRefAboveSea)
		{
				case OMX_FALSE :
						OstTraceFiltInst1(TRACE_DEBUG,"createGPStagposition::Writing below Sea Level <at line %d>",__LINE__);
						write_in_bitstream(tempPosition,0x00000001); //writing the actual data
						break;
	
				case OMX_BOOL_MAX:
				case OMX_TRUE:
						OstTraceFiltInst1(TRACE_DEBUG,"createGPStagposition::Writing above Sea Level <at line %d>",__LINE__);
					write_in_bitstream(tempPosition,0x00000000); //writing the actual data
					break;
		}
		tempPosition +=4;
	
		OstTraceFiltInst2(TRACE_DEBUG,"createGPStagposition::Absolute position for writing GPSAltitudeTAG : %d <at line %d> ",tempPosition,__LINE__);
		write_entry_in_bitstream(tempPosition,0x0006); //writing the GPSAltitude Tag number
		tempPosition +=2;
		write_entry_in_bitstream(tempPosition,0x0005); //writing the bytes/component
		tempPosition +=2;
		write_in_bitstream(tempPosition,0x00000001); //writing number of components
		tempPosition +=4;
		//(1)GPSTimeStamp
		//(2)GPSMapDatum
		//(3)GPSDateStamp
		//(4)GPSProcessingMethod
		//(5) Also account for the SIX OMX_U64 values already writen by Latitude and Longitude
				
		counter_gps -=2; //decreased by two as two tags are already written
		
		OMX_U32 offset_altitude = (ENTRY_SIZE*(counter_gps)) + (sizeof(OMX_U64)*(dynamic_gps_key[0][0]*dynamic_gps_key[0][1] + dynamic_gps_key[1][0]*dynamic_gps_key[1][1]));

		/* ER 451344 */
		tempOffsetPosition = tempPosition + 4 + offset_altitude - app1StartPos + 4;
		/* ER 451344 */

		if(check_odd_even(&tempOffsetPosition))
		{
			tempOffsetPosition++;
			val_take ++;
		}

		write_in_bitstream(tempPosition,tempOffsetPosition); //writing the offset value to actual data
		tempPosition +=4;
		tempOffsetPosition += app1StartPos;

		OstTraceFiltInst2(TRACE_DEBUG,"createGPStagposition::Absolute position for writing GPSAltitude values : %d <at line %d>",tempOffsetPosition,__LINE__);
		write_in_bitstream(tempOffsetPosition,((gpslocation.nAltitudeMeters>>32) & 0xFFFFFFFF)); //nAltitudeMeters
		tempOffsetPosition +=4;
		write_in_bitstream(tempOffsetPosition,(gpslocation.nAltitudeMeters & 0xFFFFFFFF)); //Writing the lower value of nAltitudeMeters

			
	}
	if(dynamic_gps_key[3][0])
	{
		// +ER 348736
		OstTraceFiltInst2(TRACE_DEBUG,"createGPStagposition::Absolute position for writing GPSTimeStamp : %d <line %d>",tempPosition,__LINE__);
		//Writing GPSTimeStamp
		write_entry_in_bitstream(tempPosition,0x0007); //writing the GPSTimeStamp number
		tempPosition +=2;
		write_entry_in_bitstream(tempPosition,0x0005); //writing the number of components
		tempPosition +=2;
		write_in_bitstream(tempPosition,0x00000003); //writing the bytes/component
		tempPosition +=4;
		//(1)GPSMapDatum
		//(2)GPSDateStamp
		//(3)GPSProcessingMethod
		//(4)Also account for the SEVEN OMX_U64 values 
				
		counter_gps --;
		OMX_U32 offset_time = (ENTRY_SIZE*(counter_gps)) + (sizeof(OMX_U64)*(dynamic_gps_key[0][0]*dynamic_gps_key[0][1] + dynamic_gps_key[1][0]*dynamic_gps_key[1][1]+dynamic_gps_key[2][0]*dynamic_gps_key[2][1]));
		
		/* ER 451344 */
		tempOffsetPosition = tempPosition + 4 + offset_time - app1StartPos + 4;
		/* ER 451344 */

		if(check_odd_even(&tempOffsetPosition))
		{
			tempOffsetPosition++;
			val_take ++;
		}

		write_in_bitstream(tempPosition,tempOffsetPosition); //writing the offset value to actual data
		tempPosition +=4;
		tempOffsetPosition += app1StartPos;
	
		OstTraceFiltInst2(TRACE_DEBUG,"createGPStagposition::Absolute position for writing GPSTimeStamp : %d <line %d>",tempOffsetPosition,__LINE__);
		write_in_bitstream(tempOffsetPosition,((gpslocation.nHours >>32) & 0xFFFFFFFF)); //Hour Value
		tempOffsetPosition +=4;
		write_in_bitstream(tempOffsetPosition,(gpslocation.nHours & 0xFFFFFFFF)); //Writing the denominator
		tempOffsetPosition +=4;
	
		write_in_bitstream(tempOffsetPosition,((gpslocation.nMinutes >>32) & 0xFFFFFFFF)); //Minutes Value
		tempOffsetPosition +=4;
		write_in_bitstream(tempOffsetPosition,(gpslocation.nMinutes & 0xFFFFFFFF)); //Writing the denominator
		tempOffsetPosition +=4;
	
		write_in_bitstream(tempOffsetPosition,((gpslocation.nSeconds >>32) & 0xFFFFFFFF)); //Seconds Value
		tempOffsetPosition +=4;
		write_in_bitstream(tempOffsetPosition,(gpslocation.nSeconds & 0xFFFFFFFF)); //Writing the denominator
		// -ER348736
		
	}

	//9th TAG GPSStatus
	// +ER 348736
	OstTraceFiltInst2(TRACE_DEBUG,"createGPStagposition::Absolute position for writing GPSStatus : %d <line %d>",tempPosition,__LINE__);
	//Writing GPSTimeStamp
	write_entry_in_bitstream(tempPosition,0x0009); //writing the GPSStatus Tag number
	tempPosition +=2;
	write_entry_in_bitstream(tempPosition,0x0002); //writing the bytes/component
	tempPosition +=2;
	write_in_bitstream(tempPosition,0x00000002); //writing the number of components
	tempPosition +=4;
	write_in_bitstream(tempPosition,0x00000041); //writing the actual data (Hardcoded value of GPSStatus to 'A' for Er No. 370139)
	tempPosition +=4;

	counter_gps --;

	// 10th TAG GPS MAPDATUM
	OstTraceFiltInst2(TRACE_DEBUG,"createGPStagposition::Absolute position for writing GPSDateMapum Tag: %d <line %d>",tempPosition,__LINE__);
	//Writing GPSMapDatum
	write_entry_in_bitstream(tempPosition,GPSMapDatum); //writing the GPSMapDatum 
	tempPosition +=2;
	write_entry_in_bitstream(tempPosition,0x0002); //writing the bytes/component
	tempPosition +=2;
	write_in_bitstream(tempPosition,SIZE_DATA_MAPUM); //writing the number of components
	tempPosition +=4;
	counter_gps --;
	//(1)GPSDateStamp
	//(2)GPSProcessingMethod
	//Also account for the TEN OMX_U64 values 
	OMX_U32 offset_map = (ENTRY_SIZE*(counter_gps)) + (sizeof(OMX_U64)*(dynamic_gps_key[0][0]*dynamic_gps_key[0][1] + dynamic_gps_key[1][0]*dynamic_gps_key[1][1]+dynamic_gps_key[2][0]*dynamic_gps_key[2][1]+dynamic_gps_key[3][0]*dynamic_gps_key[3][1]));
	
	/* ER 451344 */
	tempOffsetPosition = tempPosition + 4 + offset_map - app1StartPos + 4;
	/* ER 451344 */

	if(check_odd_even(&tempOffsetPosition))
	{
		tempOffsetPosition++;
		val_take ++;
	}

	write_in_bitstream(tempPosition,tempOffsetPosition); //writing the offset value to actual data
	tempPosition +=4;
	tempOffsetPosition += app1StartPos;

	OstTraceFiltInst2(TRACE_DEBUG,"createGPStagposition::Absolute position for writing GPSDataMapum Data : %d <line %d>",tempOffsetPosition,__LINE__);
	OMX_U8 map_datum[SIZE_DATA_MAPUM]= {0x57,0x47,0x53,0x2D,0x38,0x34,0x00};//WGS-84\0
	for(int as = 0;as<SIZE_DATA_MAPUM;as++)
		write_byte_in_bitstream(tempOffsetPosition++,map_datum[as]);
	
// 11th TAG GPS PROCESSING METHOD
	if(dynamic_gps_key[4][0])
	{
		OstTraceFiltInst2(TRACE_DEBUG,"createGPStagposition::Absolute position for writing GPSProcessingMethod Tag: %d <line %d>",tempPosition,__LINE__);
		//Writing GPSprocessMethod
		write_entry_in_bitstream(tempPosition,GPSProcessingMethod); //writing the GPSDateStamp 
		tempPosition +=2;
		write_entry_in_bitstream(tempPosition,0x0007); //writing the bytes/component
		tempPosition +=2;
		write_in_bitstream(tempPosition,gps_processingMethod_len+8); //writing the number of components
		tempPosition +=4;
		//GPSProcessingMethod
		//Account for the TEN OMX_U64 values + 7 OMX_U8 Values 

		counter_gps --;

		OMX_U32 offset_processing = (ENTRY_SIZE*1) + (sizeof(OMX_U64)*(dynamic_gps_key[0][0]*dynamic_gps_key[0][1] + dynamic_gps_key[1][0]*dynamic_gps_key[1][1]+dynamic_gps_key[2][0]*dynamic_gps_key[2][1]+dynamic_gps_key[3][0]*dynamic_gps_key[3][1])) + (sizeof(OMX_U8)*(SIZE_DATA_MAPUM)); 
		
		/* ER 451344 */
		tempOffsetPosition = tempPosition + 4 + offset_processing - app1StartPos + 4;
		/* ER 451344 */

		if(check_odd_even(&tempOffsetPosition))
		{
			tempOffsetPosition++;
			val_take ++;
		}

		write_in_bitstream(tempPosition,tempOffsetPosition); //writing the offset value to actual data
		tempPosition +=4;
		tempOffsetPosition += app1StartPos;
	
		OstTraceFiltInst2(TRACE_DEBUG,"createGPStagposition::Absolute position for writing GPSDateStamp Data : %d <line %d>",tempOffsetPosition,__LINE__);
	
	/*	if(gpslocation.processingValue==OMX_ASCII_Format)
			OMX_U8 ascii_val[8]={0x41,0x53,0x43,0x49,0x49,0x00,0x00,0x00};
		else if(gpslocation.processingValue==OMX_JIS_Format)
			OMX_U8 ascii_val[8]={0x4A,0x49,0x53,0x00,0x00,0x00,0x00,0x00};
		else if(gpslocation.processingValue==OMX_Unicode_Format)
			OMX_U8 ascii_val[8]={0x55,0x4E,0x49,0x43,0x4F,0x44,0x45,0x00};
		else
			OMX_U8 ascii_val[8]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};*/
		OMX_U8 ascii_val[8]={0x41,0x53,0x43,0x49,0x49,0x00,0x00,0x00};
		for(int a1 = 0;a1<8;a1++)
			write_byte_in_bitstream(tempOffsetPosition++,ascii_val[a1]);
	
		for(unsigned int a2 = 0; a2 < gps_processingMethod_len; a2++)
			write_byte_in_bitstream(tempOffsetPosition++, gps_processing_ascii[a2]);
	}

// 12th TAG GPS DATESTAMP
    /* ER 429860 */
	if(gpslocation.nYear && gpslocation.nMonth && gpslocation.nDay)
	{
		OstTraceFiltInst2(TRACE_DEBUG,"createGPStagposition::Absolute position for writing GPSDateStamp Tag: %d <line %d>",tempPosition,__LINE__);
		//Writing GPSDateMapum
		write_entry_in_bitstream(tempPosition, GPSDateStamp); //writing the GPSDateStamp
		tempPosition +=2;
		write_entry_in_bitstream(tempPosition,0x0002); //writing the bytes/component
		tempPosition +=2;
		write_in_bitstream(tempPosition,0x0000000B); //writing the number of components
		tempPosition +=4;

		counter_gps --;

	//Account for the TEN OMX_U64 values + SEVEN OMX_U8 Values + gps_processingMethod_len + 8
	OMX_U32 offset_date = (sizeof(OMX_U64)*(dynamic_gps_key[0][0]*dynamic_gps_key[0][1] + dynamic_gps_key[1][0]*dynamic_gps_key[1][1]+dynamic_gps_key[2][0]*dynamic_gps_key[2][1]+dynamic_gps_key[3][0]*dynamic_gps_key[3][1]))+ (sizeof(OMX_U8)*dynamic_gps_key[4][0]*dynamic_gps_key[4][1]) + (sizeof(OMX_U8)*(SIZE_DATA_MAPUM)); 
	
	/* ER 451344 */
	tempOffsetPosition = tempPosition + 4 + offset_date - app1StartPos + 4;
	/* ER 451344 */

	if(check_odd_even(&tempOffsetPosition))
	{
		tempOffsetPosition++;
		val_take ++;
	}

	write_in_bitstream(tempPosition,tempOffsetPosition); //writing the offset value to actual data
	tempPosition +=4;
	tempOffsetPosition += app1StartPos;

	OstTraceFiltInst2(TRACE_DEBUG,"createGPStagposition::Absolute position for writing GPSDateStamp Data : %d <line %d>",tempOffsetPosition,__LINE__);
	for(int as = 0;as<SIZE_DATE_STAMP;as++)
		write_byte_in_bitstream(tempOffsetPosition++,date_stamp[as]);
	tempOffsetPosition +=(OMX_U32)check_odd_even(&tempOffsetPosition);
	}
    /* ER 429860 */

	/* ER 451344 */
	write_in_bitstream(tempPosition, 0x0); // No next IFD entry, 4 bytes
	tempPosition +=4;
	/* ER 451344 */
	
	if(create_gps_tag)
	{
		OstTraceFiltInst1(TRACE_FLOW,"WRITING IFD1 offset at create_gps_tag for empty IFD0 <at line %d>",__LINE__);
		write_in_bitstream(tempPosition_IFD1,(tempOffsetPosition - app1StartPos)); //rewriting the offset to IFD1 section	
	}

	OstTraceFiltInst2(TRACE_API,"createGPStagposition::Ending of writing GPS data, at Position : %d  <at line %d>",(tempOffsetPosition),__LINE__);

	isEXIFsegmentempty = OMX_FALSE;
	
	return OMX_ErrorNone;
}

OMX_ERRORTYPE EXIFParser::setIFD1tagposition()
{
	OMX_U32 tempPosition;
	OMX_U32 tempOffsetPosition;
	OMX_ERRORTYPE exif_error;

	OstTraceFiltInst1(TRACE_API,"Inside function setIFD1tagposition <at line %d>",__LINE__);

	tempPosition = start_of_ifd0; //placing the ptr at start of IFD0 tag

	if (isEXIFsegmentempty) //no IFD0 does not exist
	{
		write_entry_in_bitstream(tempPosition,0x0000); //no directory entry for IFD0
		tempPosition +=2;
		write_in_bitstream(tempPosition,(tempPosition+4-app1StartPos)); //offset Position for IFD1
		tempPosition +=4;
	}
	exif_error = locate_ifd1_section();
	tempPosition = currentPosition;
	OstTraceFiltInst2(TRACE_FLOW,"setIFD1tagposition::Start Point of inserting IFD1 : %d <at line %d>",tempPosition,__LINE__);
	if(OMX_ErrorNone != exif_error)
	{
		OstTraceFiltInst2(TRACE_ERROR,"setIFD1tagposition::locate_ifd1_section generated error from setIFD1tagposition : %d <at line %d>",exif_error,__LINE__);
		return exif_error;
	}

	//now we are at the starting point of IFD1 so we have to put IFD1 tags
	//we have to write 3 directory entries
	//(1)Compression tag
	//(2)Orientation
	//(3)XResolution
	//(4)YResolution
	//(5)Resolution Unit
	//(6)JpegIFOffset tag
	//(7)JpegIFByteCount tag
	write_entry_in_bitstream(tempPosition,0x0007); //no. of directory entry for IFD0
	tempPosition +=2;

	OstTraceFiltInst1(TRACE_DEBUG,"setIFD1tagposition::Writing Compression tag <at line %d>",__LINE__);
	//writing Compression tag
	write_entry_in_bitstream(tempPosition,Compression); //Compression tag
	tempPosition +=2;
	write_entry_in_bitstream(tempPosition,0x0003); //Short 
	tempPosition +=2;
	write_in_bitstream(tempPosition,0x00000001); //writing the bytes/component
	tempPosition +=4;
	write_in_bitstream(tempPosition,0x00000006); //To denote that Thumbnail is in JPEG format
	tempPosition +=4;

	// +ER 349081
	OstTraceFiltInst1(TRACE_DEBUG,"setIFD1tagposition::Writing Orientation tag <at line %d>",__LINE__);
	//writing Orientation tag
	write_entry_in_bitstream(tempPosition,Orientation); //Orientation tag
	tempPosition +=2;
	write_entry_in_bitstream(tempPosition,ort_type); //Short 
	tempPosition +=2;
	write_in_bitstream(tempPosition,ort_comps); //writing the bytes/component
	tempPosition +=4;
	write_in_bitstream(tempPosition,ort_data); //To denote Orientation as Top/Left
	tempPosition +=4;
	// -ER 349081

	OstTraceFiltInst1(TRACE_DEBUG,"setIFD1tagposition::Writing XResolution tag <at line %d>",__LINE__);
	//writing XResolution tag
	write_entry_in_bitstream(tempPosition,XResolution); //XResolution tag
	tempPosition +=2;
	write_entry_in_bitstream(tempPosition,0x0005); //Rational
	tempPosition +=2;
	write_in_bitstream(tempPosition,0x00000001); //writing the bytes/component
	tempPosition +=4;

	//tempPosition is the current position
	//4 bytes for space occupied by writing current
	//12 is the size of one directory entry; 4 directories are still left
	/* ER 451344 */
	tempOffsetPosition = tempPosition + 4 + 12 * 4 + 4; /* +4 for next IFD entry */
	/* ER 451344 */
	write_in_bitstream(tempPosition,tempOffsetPosition-app1StartPos); //Offset to XResolution Data
	tempPosition +=4;
	{
		//Writing data of XResolution Tag
		OstTraceFiltInst1(TRACE_DEBUG,"setIFD1tagposition::Writing Data of XResolution tag at Address : 0x%x",tempOffsetPosition);
		//First 4 Bytes is default value of 72dpi
		write_in_bitstream(tempOffsetPosition,0x00000048); //Offset to XResolution Data
		tempOffsetPosition += 4;
		//Second 4 Bytes is default count of 1
		write_in_bitstream(tempOffsetPosition,0x00000001); //Offset to XResolution Data
	}

	OstTraceFiltInst1(TRACE_DEBUG,"setIFD1tagposition::Writing YResolution tag <at line %d>",__LINE__);
	//writing YResolution tag
	write_entry_in_bitstream(tempPosition,YResolution); //YResolution tag
	tempPosition +=2;
	write_entry_in_bitstream(tempPosition,0x0005); //Rational
	tempPosition +=2;
	write_in_bitstream(tempPosition,0x00000001); //writing the bytes/component
	tempPosition +=4;

	//tempPosition is the current position
	//4 bytes for space occupied by writing current
	//12 is the size of one directory entry; 3 directories are still left
	//8 Bytes needed by XResolution
	/* ER 451344 */
	tempOffsetPosition = tempPosition + 4 + 12 * 3 + 8 + 4; /* +4 for next IFD entry */
	/* ER 451344 */
	write_in_bitstream(tempPosition,tempOffsetPosition-app1StartPos); //Offset to YResolution Data
	tempPosition +=4;
	{
		//Writing data of YResolution Tag
		OstTraceFiltInst1(TRACE_DEBUG,"setIFD1tagposition::Writing Data of YResolution tag at Address : 0x%x",tempOffsetPosition);
		//First 4 Bytes is default value of 72dpi
		write_in_bitstream(tempOffsetPosition,0x00000048); //Offset to XResolution Data
		tempOffsetPosition += 4;
		//Second 4 Bytes is default count of 1
		write_in_bitstream(tempOffsetPosition,0x00000001); //Offset to XResolution Data
	}
	OstTraceFiltInst1(TRACE_DEBUG,"setIFD1tagposition::Writing Resolution Unit tag <at line %d>",__LINE__);
	//writing Resolution_Unit tag
	write_entry_in_bitstream(tempPosition,Resolution_Unit); //Resolution_Unit tag
	tempPosition +=2;
	write_entry_in_bitstream(tempPosition,0x0003); //Short
	tempPosition +=2;
	write_in_bitstream(tempPosition,0x00000001); //writing the bytes/component
	tempPosition +=4;
	write_in_bitstream(tempPosition,0x00000002); //To denote Resolution Unit as inch
	tempPosition +=4;

	OstTraceFiltInst1(TRACE_DEBUG,"setIFD1tagposition::Writing JPEGInterchangeFormat tag < at line %d>",__LINE__);
	//writing JPEGInterchangeFormat tag
	write_entry_in_bitstream(tempPosition,JPEGInterchangeFormat); //JPEGInterchangeFormat tag
	tempPosition +=2;
	write_entry_in_bitstream(tempPosition,0x0004); // Long
	tempPosition +=2;
	write_in_bitstream(tempPosition,0x00000001); //writing the bytes/component
	tempPosition +=4;

	//tempPosition is the current position
	//4 bytes for space occupied by writing current
	//12 is the size of one directory entry
	//2*8 Bytes (8 each needed by XResolution and YResolution)
	/* ER 451344 */
	tempOffsetPosition = tempPosition + 4 + 12 + 2 * 8 + 4; /* + 4 for next IFD entry */
	/* ER 451344 */
	write_in_bitstream(tempPosition,tempOffsetPosition-app1StartPos); //Offset to Thumbnail
	tempPosition +=4;

	OstTraceFiltInst1(TRACE_DEBUG,"setIFD1tagposition::Writing  JPEGInterchangeFormatLength tag <at line %d>",__LINE__);
	//writing JPEGInterchangeFormatLength tag
	write_entry_in_bitstream(tempPosition,JPEGInterchangeFormatLength); //JPEGInterchangeFormatLength tag
	tempPosition +=2;
	write_entry_in_bitstream(tempPosition,0x0004); //Long
	tempPosition +=2;
	write_in_bitstream(tempPosition,0x00000001); //Writing the bytes/component
	tempPosition +=4;

	write_in_bitstream(tempPosition,0x0); //Size of Thumbnail is Unknown so we put ZERO
	
	tempPosition +=4;

	/* ER 451344 */
	write_in_bitstream(tempPosition, 0x0); // No next IFD, 4 bytes
	tempPosition +=4;
	/* ER 451344 */
	
	OstTraceFiltInst1(TRACE_API,"setIFD1tagposition DONE <at line %d>",__LINE__);
	return exif_error;
}


OMX_U32 EXIFParser::bytes_per_component(OMX_U16 tempSize)
{
	OMX_U32 value = 0;

	switch(tempSize)
	{
		case 1: value =1; break;
		case 2: value =1; break;
		case 3: value =2; break;
		case 4: value =4; break;
		case 5: value =8; break;
		case 6: value =1; break;
		case 7: value =1; break;
	}
	return value;
}


OMX_U32 EXIFParser::return_CurrentPosition()
{
	return currentPosition;
}


void EXIFParser::write_in_bitstream(OMX_U32 position,OMX_U32 value_to_write)
{
   if (!endianness_flag)
   {
		exifHeaderBuffer[position]   = (value_to_write & 0xFF);
		exifHeaderBuffer[position+1] = (value_to_write >> 8) & 0xFF;
		exifHeaderBuffer[position+2] = (value_to_write >>16) & 0xFF;
		exifHeaderBuffer[position+3] = (value_to_write >>24) & 0xFF;
	}
	else
	{
		exifHeaderBuffer[position] =   (value_to_write >>24) & 0xFF;
		exifHeaderBuffer[position+1] = (value_to_write >>16) & 0xFF;
		exifHeaderBuffer[position+2] = (value_to_write >> 8) & 0xFF;
		exifHeaderBuffer[position+3] = (value_to_write & 0xFF);
	}
}


void EXIFParser::write_entry_in_bitstream(OMX_U32 position,OMX_U32 value_to_write)
{
   if (!endianness_flag)
   {
		exifHeaderBuffer[position]   = (value_to_write & 0xFF);
		exifHeaderBuffer[position+1] = (value_to_write >> 8) & 0xFF;
	}
	else
	{
		exifHeaderBuffer[position] =   (value_to_write >>24) & 0xFF;
		exifHeaderBuffer[position+1] = (value_to_write >>16) & 0xFF;
	}
}

void EXIFParser::write_byte_in_bitstream(OMX_U32 position,OMX_U8 value_to_write)
{
	exifHeaderBuffer[position]=(value_to_write & 0xFF);
}
/*
It is assumed that currently the pointer is at the start of IFD 0 tag
*/
OMX_ERRORTYPE EXIFParser::locate_gps_tag(OMX_U32 *position)
{
	OstTraceFiltInst1(TRACE_API,"In locate_gps_tag <at line %d>",__LINE__);
	OMX_U16 tagType,no_gps_subtags;
	OMX_U16 tempSize;
	OMX_ERRORTYPE exif_error;
	OMX_U32 tempData,value;
	OMX_BOOL isOffset = OMX_FALSE;
	OMX_U32 tempBufferPosition = 0;
	OMX_U32 location_values[6] = {0};
	OMX_U32 gpstag_off=0;;
	OMX_U8 index;


	currentPosition = start_of_ifd0; //placing the ptr at start of IFD0 tag
	OstTraceFiltInst2(TRACE_FLOW,"In locate_gps_tag:: Trying to locate the GPS <current location in file %d> < at line no %d>",currentPosition,__LINE__);
	exif_error = ReadTwoByte(&tempSize);//calculating no of directories
	if (!endianness_flag)
		tempSize = ENDIANCONVERT16(tempSize);
	gpstag_off = start_of_ifd0 + 2 + (tempSize - 1) * ENTRY_SIZE;
	currentPosition = gpstag_off;
		OstTraceFiltInst2(TRACE_FLOW,"In locate_gps_tag:: Location of GPS tag offset <current location in file %d> <at line no %d>",currentPosition,__LINE__);
	exif_error = ReadTwoByte(&tagType);
	if (!endianness_flag)
		 tagType = ENDIANCONVERT16(tagType);
	if ((OMX_ErrorNone != exif_error) ||(GPS_TAG != tagType))
	{
		OstTraceFiltInst1(TRACE_ERROR,"In locate_gps_tag:: ERROR returned < at line no %d>",__LINE__);
	    return OMX_ErrorStreamCorrupt;
	}
	OstTraceFiltInst2(TRACE_DEBUG,"In locate_gps_tag:: Trying to locate the GPS <current location in file %d> < at line no %d>",currentPosition,__LINE__);
	//reading the bytes/component entry
	exif_error = ReadTwoByte(&tempSize);
	if (OMX_ErrorNone != exif_error) return OMX_ErrorStreamCorrupt;

	if (!endianness_flag)
		tempSize = ENDIANCONVERT16(tempSize);

	value = bytes_per_component(tempSize);
	if (!value) return OMX_ErrorUndefined;

	// reading no. of components
	exif_error = ReadTwoByte(&tempSize);
	tempData = tempSize;
	tempData <<=16;
	exif_error = ReadTwoByte(&tempSize);
	tempData |= tempSize;

	if (!endianness_flag)
		tempData = ENDIANCONVERT32(tempData);

	isOffset = OMX_TRUE; //it is always the offset position in GPS_TAG


	//offset value or data value
	exif_error = ReadTwoByte(&tempSize);
	tempData = tempSize;
	tempData <<=16;
	exif_error = ReadTwoByte(&tempSize);
	tempData |= tempSize;
	if (!endianness_flag)
		tempData = ENDIANCONVERT32(tempData);
	if (isOffset) //read the data value from the offset position
	{
		tempBufferPosition = currentPosition;
		currentPosition = tempData + app1StartPos;
	}

	OstTraceFiltInst3(TRACE_DEBUG,"locate_gps_tag::Value of tempBufferPosition %d and currentPosition %d < at line no %d>",tempBufferPosition,currentPosition,__LINE__);
	//reading the bytes/component entry
	exif_error = ReadTwoByte(&tempSize);
	if (OMX_ErrorNone != exif_error) return OMX_ErrorStreamCorrupt;

	if (!endianness_flag)
		tempSize = ENDIANCONVERT16(tempSize);

    //tempSize is the number of directory
    OstTraceFiltInst2(TRACE_DEBUG,"locate_gps_tag::Number of directory entries for GPS IFD : %d <at line %d>",tempSize,__LINE__);

    no_gps_subtags = tempSize;
	tempData = currentPosition;
	if(no_gps_subtags == 0)
	{
		OstTraceFiltInst1(TRACE_FLOW,"locate_gps_tag:: Writing GPS Data into Input Buffer <at line %d>",__LINE__);
		exif_error = createGPStagposition(OMX_FALSE,(currentPosition-2));
		no_gps_subtags = NO_GPS_TAGS;
		currentPosition = tempData;
	}
    //currently the cursor is just atfer tag 0x8825
	*position = currentPosition;
	OstTraceFiltInst1(TRACE_API,"locate_gps_tag DONE <at line %d>",__LINE__);
	//now place the cursor the at the start of data offset for GPS info
	return OMX_ErrorNone;
}

void exif_mixer_arm_nmf_mixer_core::setDimension(t_uint32 width, t_uint32 height)
{
	//+ER 354962
	//-ER 354962
	xDim = width;
	yDim = height;
	//+ER 354962
	iCommandAck.pendingCommandAck();
	//-ER 354962
}

void exif_mixer_arm_nmf_mixer_core::setConfigure(t_uint32 index,void* pComponentParameterStructure)
{
	switch (index)
	{
    	case OMX_IndexConfigImageGPSLocation:
    		download_parameters = OMX_TRUE;
    		OMX_CONFIG_GPSLOCATIONTYPE *pt = (OMX_CONFIG_GPSLOCATIONTYPE *)pComponentParameterStructure;
    		globalgpslocation.nLatitudeDegrees = pt->nLatitudeDegrees;
    		globalgpslocation.nLatitudeMinutes = pt->nLatitudeMinutes;
    		globalgpslocation.nLatitudeSeconds = pt->nLatitudeSeconds;
    		globalgpslocation.nLongitudeDegrees = pt->nLongitudeDegrees;
    		globalgpslocation.nLongitudeMinutes = pt->nLongitudeMinutes;
    		globalgpslocation.nLongitudeSeconds = pt->nLongitudeSeconds;
    		globalgpslocation.nAltitudeMeters = pt->nAltitudeMeters;
    		globalgpslocation.bLatitudeRefNorth = pt->bLatitudeRefNorth;
    		globalgpslocation.bLongitudeRefEast = pt->bLongitudeRefEast;
    		globalgpslocation.bAltitudeRefAboveSea = pt->bAltitudeRefAboveSea;
    		globalgpslocation.bLocationKnown = pt->bLocationKnown;
			globalgpslocation.nHours = pt->nHours;
    		globalgpslocation.nMinutes = pt->nMinutes;
    		globalgpslocation.nSeconds = pt->nSeconds;
    		/* ER 429860 */
    		if(pt->nYear && pt->nMonth && pt->nDay)
    		{
    			globalgpslocation.nYear = pt->nYear;
    			globalgpslocation.nMonth = pt->nMonth;
    			globalgpslocation.nDay = pt->nDay;
                snprintf((char *)(&exifparser.date_stamp[0]), SIZE_DATE_STAMP, "%04llu:%02llu:%02llu", pt->nYear, pt->nMonth, pt->nDay);
                OstTraceInt3(TRACE_API,"MIXER_Core : setConfigure : OMX_IndexConfigImageGPSLocation nYear=%d nMonth=%d nDay=%d",pt->nYear,pt->nMonth,pt->nDay);
    		}
    		/* ER 429860 */
    		globalgpslocation.processingValue = pt->processingValue;// At Present Not considered
			for(int as=0;as<SIZE_GPS_PR_MTD;as++)
				globalgpslocation.nPrMethodDataValue[as]=pt->nPrMethodDataValue[as];
			exifparser.gps_processingMethod_len = 0;
			for(int a=0;a<SIZE_GPS_PR_MTD;a++)
			{
				exifparser.gps_processingMethod_len++;
				exifparser.gps_processing_ascii[a]=globalgpslocation.nPrMethodDataValue[a];
				if(exifparser.gps_processing_ascii[a]==0x00)
					break;
			}
			if (exifparser.gps_processingMethod_len%2)	//RG
			{
				exifparser.gps_processingMethod_len++;
			}		//RG	
			OstTraceInt2(TRACE_API,"MIXER_Core : setConfigure : Bytes of GPSprocessingmethod Data of format : %d is %d Bytes",pt->processingValue,exifparser.gps_processingMethod_len);
			break;
	}
	//+ER 354962
	iCommandAck.pendingCommandAck();
	//-ER 354962
}

void exif_mixer_arm_nmf_mixer_core::getConfigure(t_uint32 index,void* pComponentParameterStructure)
{
	switch (index)
	{
    	case OMX_IndexConfigImageGPSLocation:
    		OMX_CONFIG_GPSLOCATIONTYPE *pt = (OMX_CONFIG_GPSLOCATIONTYPE *)pComponentParameterStructure;
    		pt->nLatitudeDegrees = globalgpslocation.nLatitudeDegrees;
    		pt->nLatitudeMinutes = globalgpslocation.nLatitudeMinutes;
    		pt->nLatitudeSeconds = globalgpslocation.nLatitudeSeconds;
    		pt->nLongitudeDegrees = globalgpslocation.nLongitudeDegrees;
    		pt->nLongitudeMinutes = globalgpslocation.nLongitudeMinutes;
    		pt->nLongitudeSeconds = globalgpslocation.nLongitudeSeconds;
    		pt->nAltitudeMeters = globalgpslocation.nAltitudeMeters;
    		pt->bLatitudeRefNorth = globalgpslocation.bLatitudeRefNorth;
    		pt->bLongitudeRefEast = globalgpslocation.bLongitudeRefEast;
    		pt->bAltitudeRefAboveSea = globalgpslocation.bAltitudeRefAboveSea;
    		pt->bLocationKnown = globalgpslocation.bLocationKnown;
			pt->nHours = globalgpslocation.nHours;
    		pt->nMinutes = globalgpslocation.nMinutes;
    		pt->nSeconds = globalgpslocation.nSeconds;
    		/* ER 429860 */
    		pt->nYear = globalgpslocation.nYear;
    		pt->nMonth = globalgpslocation.nMonth;
    		pt->nDay = globalgpslocation.nDay;
    		/* ER 429860 */
    		pt->processingValue = globalgpslocation.processingValue;// At Present Not considered
			for(int as=0;as<SIZE_GPS_PR_MTD;as++)
				pt->nPrMethodDataValue[as]=globalgpslocation.nPrMethodDataValue[as];
    		break;
	}
	//+ER 354962
	iCommandAck.pendingCommandAck();
	//-ER 354962
}

/*
This is used to skip the directory. Here it is assumed that we have read first two bytes of directory
and we want to skip the remaining 10 bytes
*/
void EXIFParser::skip_current_directory()
{
	currentPosition +=10;
}

OMX_ERRORTYPE EXIFParser::read_directory_entry(OMX_U32 *tempValue,OMX_U32 *tempBufferPosition, OMX_BOOL *isOffset)
{
	OstTraceFiltInst1(TRACE_API,"In read_directory_entry  <at line %d>",__LINE__);
	OMX_ERRORTYPE exif_error;
//	OMX_BOOL isOffset = OMX_FALSE;
	OMX_U16 tempSize;
	OMX_U32 tempData,value;
//	OMX_U8 checkValue = 0;
	OMX_U32 bytes_to_read;

	*isOffset = OMX_FALSE;

	//reading the bytes/component entry
	exif_error = ReadTwoByte(&tempSize);
	if (OMX_ErrorNone != exif_error)
	{
		return OMX_ErrorStreamCorrupt;
	}

	if (!endianness_flag)
		tempSize = ENDIANCONVERT16(tempSize);

	value = bytes_per_component(tempSize);
	if (!value)
	{
		return OMX_ErrorUndefined;
	}

	// reading no. of components
	exif_error = ReadTwoByte(&tempSize);
	tempData = tempSize;
	tempData <<=16;
	exif_error = ReadTwoByte(&tempSize);
	tempData |= tempSize;

	if (!endianness_flag)
		tempData = ENDIANCONVERT32(tempData);

	bytes_to_read = (tempData * value);
	if ((tempData * value) > 4) *isOffset = OMX_TRUE;

	//offset value or data value
	exif_error = ReadTwoByte(&tempSize);
	tempData = tempSize;
	tempData <<=16;
	exif_error = ReadTwoByte(&tempSize);
	tempData |= tempSize;
	if (!endianness_flag)
		tempData = ENDIANCONVERT32(tempData);

	*tempValue = tempData;


	if (*isOffset) //read the data value from the offset position
	{
		save_current = currentPosition; 
		*tempBufferPosition = currentPosition;
		currentPosition = *tempValue + app1StartPos;
		OstTraceFiltInst3(TRACE_DEBUG,"CAUTION :: Reading %d bytes from offset : %d <at line %d>",bytes_to_read,currentPosition,__LINE__);
		if (bytes_to_read<=WORD_SIZE)
		{
			//reading the offset value
			exif_error = ReadTwoByte(&tempSize);
			tempData = tempSize;
			tempData <<=16;
			exif_error = ReadTwoByte(&tempSize);
			tempData |= tempSize;
			if (!endianness_flag)
				tempData = ENDIANCONVERT32(tempData);
			*tempValue = tempData;
		}
		else
		{
			OstTraceFiltInst1(TRACE_DEBUG,"Interpresting the ptr as an array of OMX_U32 type <at line %d>",__LINE__);
			while (bytes_to_read/WORD_SIZE)
			{
				//reading the offset value
				exif_error = ReadTwoByte(&tempSize);
				tempData = tempSize;
				tempData <<=16;
				exif_error = ReadTwoByte(&tempSize);
				tempData |= tempSize;
				if (!endianness_flag)
					tempData = ENDIANCONVERT32(tempData);

				*tempValue = tempData;
				tempValue ++; //incremented by word size
				bytes_to_read -=WORD_SIZE;
			}
		}
	}
	OstTraceFiltInst1(TRACE_API,"read_directory_entry DONE <at line %d>",__LINE__);
	return exif_error;
}


/*
It is assumed that at this Point we are at the start if IFD1 tag
This function returns the Position where the Thumbnail would be inserted
*/
OMX_ERRORTYPE EXIFParser::locate_and_set_ifd1_tags(OMX_U32 *position)
{
	OstTraceFiltInst1(TRACE_API,"In locate_and_set_ifd1_tags  <at line %d>",__LINE__);
	OMX_ERRORTYPE exif_error;
	OMX_U16 tempSize;
	OMX_U16 tagType;
	OMX_U32 tempData = 0;
	OMX_BOOL isOffset = OMX_FALSE;
	OMX_U32 tempBufferPosition = 0;
	OMX_U8 checkValue = 0;
	OMX_U32 dirCounter=0;
	OMX_U32 temparray[1] ={0};
	OstTraceFiltInst2(TRACE_FLOW,"locate_and_set_ifd1_tags::Start Point of Parsing IFD1 : %d <at line %d>",currentPosition,__LINE__);
	//reading the number of Directory entries
	//reading the bytes/component entry
	exif_error = ReadTwoByte(&tempSize);
	if (OMX_ErrorNone != exif_error) return OMX_ErrorStreamCorrupt;

	if (!endianness_flag)
		tempSize = ENDIANCONVERT16(tempSize);

	TotalDirEntries = tempSize;

	OstTraceFiltInst2(TRACE_DEBUG,"locate_and_set_ifd1_tags::Number of Directory entries in IFD1 : %d <at line %d>",TotalDirEntries,__LINE__);

	if (TotalDirEntries <NO_OF_TAGS)
	{
		OstTraceFiltInst1(TRACE_ERROR,"locate_and_set_ifd1_tags::EXIF Header does not have enough IFD1 tags required =>ERROR<= <at line %d>",__LINE__);
		return OMX_ErrorStreamCorrupt;
	}

    while ((NO_OF_TAGS != checkValue) && (dirCounter < TotalDirEntries))
    {
		exif_error = GetNextEXIFStartCode(&tagType);
		exif_error = read_directory_entry(temparray,&tempBufferPosition,&isOffset);
		tempData = temparray[0];
		OstTraceFiltInst2(TRACE_DEBUG,"locate_and_set_ifd1_tags: tempBufferPosition: 0x%x <Offset %d>",tempBufferPosition,isOffset);
		switch (tagType)
		{
			case Compression :
								OstTraceFiltInst1(TRACE_DEBUG,"Inside locate_and_set_ifd1_tags=>For Compression subtag <at line %d>",__LINE__);
								if (JPEG_COMPRESSION != tempData) //compression value
								{
									currentPosition = currentPosition-4;
									if (!endianness_flag)
									{
										exifHeaderBuffer[currentPosition] = 0x06;
										exifHeaderBuffer[currentPosition+1] = 0x00;
										exifHeaderBuffer[currentPosition+2] = 0x00;
										exifHeaderBuffer[currentPosition+3] = 0x00;
									}
									else
									{
										exifHeaderBuffer[currentPosition] = 0x00;
										exifHeaderBuffer[currentPosition+1] = 0x00;
										exifHeaderBuffer[currentPosition+2] = 0x00;
										exifHeaderBuffer[currentPosition+3] = 0x06;
									}
									currentPosition = currentPosition+4;
								}
								checkValue++;
								break;

			case JPEGInterchangeFormat : /* code for JpegIFOffset */
								OstTraceFiltInst1(TRACE_DEBUG,"Inside locate_and_set_ifd1_tags=>JPEGInterchangeFormat <at line %d>",__LINE__);
								*position = tempData + app1StartPos;
								checkValue++;
								break;

			case JPEGInterchangeFormatLength : /* code for JpegIFByteCount */
									/* writing the thumbnail size */
									OstTraceFiltInst1(TRACE_DEBUG,"Inside locate_and_set_ifd1_tags=>JPEGInterchangeFormatLength <at line %d>",__LINE__);
									currentPosition = currentPosition-4;
									if (!endianness_flag)
									{
										exifHeaderBuffer[currentPosition]   = (thumbnail_size & 0xFF);
										exifHeaderBuffer[currentPosition+1] = (thumbnail_size >> 8) & 0xFF;
										exifHeaderBuffer[currentPosition+2] = (thumbnail_size >>16) & 0xFF;
										exifHeaderBuffer[currentPosition+3] = (thumbnail_size >>24) & 0xFF;
									}
									else
									{
										exifHeaderBuffer[currentPosition] =   (thumbnail_size >>24) & 0xFF;
										exifHeaderBuffer[currentPosition+1] = (thumbnail_size >>16) & 0xFF;
										exifHeaderBuffer[currentPosition+2] = (thumbnail_size >> 8) & 0xFF;
										exifHeaderBuffer[currentPosition+3] = (thumbnail_size & 0xFF);
									}
									currentPosition = currentPosition+4;
								    checkValue++;
									break;
			case YResolution : 
								OstTraceFiltInst1(TRACE_DEBUG,"Inside locate_and_set_ifd1_tags=>YResolution <at line %d>",__LINE__);
								checkValue++;
								break;
			case XResolution : 
								OstTraceFiltInst1(TRACE_DEBUG,"Inside locate_and_set_ifd1_tags=>XResolution <at line %d>",__LINE__);
								checkValue++;
								break;
			case Orientation : 
								OstTraceFiltInst1(TRACE_DEBUG,"Inside locate_and_set_ifd1_tags=>Orientation <at line %d>",__LINE__);
								checkValue++;
								break;
			case Resolution_Unit : 
								OstTraceFiltInst1(TRACE_DEBUG,"Inside locate_and_set_ifd1_tags=>Resolution_Unit <at line %d>",__LINE__);
								checkValue++;
								break;
		}
		if (isOffset)
				currentPosition = save_current; //after the data or offset value

//		currentPosition = tempBufferPosition + 4; //after the data or offset value
		isOffset = OMX_FALSE;
		dirCounter++;
	}
	thumbnailPosition = *position;
	OstTraceFiltInst1(TRACE_API,"locate_and_set_ifd1_tags DONE <at line %d>",__LINE__);
	return OMX_ErrorNone;
}

/*
It is put at the first byte of the IFD0 section by start_of_ifd0
*/
OMX_ERRORTYPE EXIFParser::locate_ifd1_section(void)
{
	OstTraceFiltInst1(TRACE_API,"In locate_ifd1_section  <at line %d>",__LINE__);
	OMX_ERRORTYPE exif_error;
	OMX_U16 tempSize;
	OMX_U32 tempData;

	currentPosition = start_of_ifd0;

	exif_error = ReadTwoByte(&tempSize);
	if (!endianness_flag)
		tempSize = ENDIANCONVERT16(tempSize);

	currentPosition += (tempSize * ENTRY_SIZE);
	//read the offset value to ifd1 section
	//reading the EXIF header TAG
	exif_error = ReadTwoByte(&tempSize);
	tempData = tempSize;
	tempData <<=16;
	exif_error = ReadTwoByte(&tempSize);
	tempData |= tempSize;

	if (!endianness_flag)
		tempData = ENDIANCONVERT32(tempData);

	if (tempData == 0)
		exif_error = OMX_ErrorNoMore;
	else
		ifd1_offset = tempData;


	//actual starting position of IFD1 tag
	currentPosition = tempData + app1StartPos;
	OstTraceFiltInst1(TRACE_API,"locate_ifd1_section  DONE <at line %d>",__LINE__);
	return exif_error;
}


OMX_U32 EXIFParser::check_odd_even(OMX_U32 *offsetPosition)
{
	OMX_U32 increment= 0;

	if ((*offsetPosition)%2)
	{
		OstTraceFiltInst1(TRACE_FLOW,"In check_odd_even function.. Odd offset position.. Value of offsetPosition is %d>",(*offsetPosition));
		increment= 1;
	}

	return increment;
}

void EXIFParser::initialize_data(OMX_BUFFERHEADERTYPE *pBuffer)
{
	OstTraceFiltInst1(TRACE_API,"In initialize_data   <at line %d>",__LINE__);
	save_current = 0;
	currentPosition = 0;	
	val_take = 0;
	app1StartPos = 0;
	headerSize = pBuffer->nFilledLen; //total size of the EXIF image would be calculated again
	endianness_flag = OMX_FALSE;
	TotalDirEntries = 0;
	exifHeaderBuffer = (OMX_U8 *)(pBuffer->pBuffer + pBuffer->nOffset);
	thumbnail_size = 0;
	consider_endianness = OMX_FALSE;
	exif_size_location = 0;
	start_of_exif_offset = 0;
	start_of_ifd0 = 0;
	is_ifd1_tag = OMX_FALSE;
	thumbnailPosition = 0;
	isEXIFsegmentempty = OMX_FALSE;
	gpslocation.nLatitudeDegrees = 0;
	gpslocation.nLatitudeMinutes = 0;
	gpslocation.nLatitudeSeconds = 0;
	gpslocation.nLongitudeDegrees = 0;
	gpslocation.nLongitudeMinutes = 0;
	gpslocation.nLongitudeSeconds = 0;
	gpslocation.nAltitudeMeters = 0;
	gpslocation.bLatitudeRefNorth = OMX_FALSE;
	gpslocation.bLongitudeRefEast = OMX_FALSE;
	gpslocation.bAltitudeRefAboveSea = OMX_FALSE;
	gpslocation.bLocationKnown = OMX_FALSE;
	gpslocation.nHours = 0;
	gpslocation.nMinutes = 0;
	gpslocation.nSeconds = 0;
	/* ER 429860 */
	gpslocation.nYear = 0;
	gpslocation.nMonth = 0;
	gpslocation.nDay = 0;
	/* ER 429860 */
	gpslocation.processingValue =  (OMX_KEY_GPS_PROCESSING_METHODTYPE)0;
	for(int as=0;as<SIZE_GPS_PR_MTD;as++)
		gpslocation.nPrMethodDataValue[as]=0;
	globalPtr = 0x0;
	headerCorrectionReq = OMX_FALSE;
	// +ER 349081
	ort_type	= 0x0003;
	ort_comps	= 0x00000001;
	ort_data	= 0x00000001;
	valid_tag=0;
	valid_offset=0;
	// -ER 349081
	//date_stamp[SIZE_DATE_STAMP] = {0x89,0x89,0x89,0x89,0x3A,0x77,0x77,0x3A,0x68,0x68,0x00};//YYYY:MM:DD\0
}

void EXIFParser::correctHeaderSize(OMX_U32 thumbsizeOffset,OMX_BUFFERHEADERTYPE *pBuffer)
{
	//actual header size
	if (headerCorrectionReq)
	{
		OstTraceFiltInst2(TRACE_FLOW,"Updating the header Size : %d <at line %d>",thumbsizeOffset,__LINE__);
		//updating the same in buffer
		exifHeaderBuffer = (OMX_U8 *)(pBuffer->pBuffer + pBuffer->nOffset);
		exifHeaderBuffer[exif_size_location]   = (thumbsizeOffset >> 8) & 0xFF;
		exifHeaderBuffer[exif_size_location+1] = thumbsizeOffset & 0xFF;
	}
}

OMX_ERRORTYPE EXIFParser::isEXIFbufferEmpty()
{
	OMX_ERRORTYPE exif_error;
	OMX_U16 tempSize;
//	OMX_U32 tempData;

	currentPosition = start_of_ifd0;

	//reading the number of directory entries
	exif_error = ReadTwoByte(&tempSize);

	if (OMX_ErrorNone != exif_error)
	{
		OstTraceFiltInst1(TRACE_FLOW,"======>FATAL : Could not read Value for IFD0 Offset<=============== <at line %d>",__LINE__);
		//CHECKERROR_SENDEVENT(exif_error);
	}

	if (!endianness_flag)
		tempSize = ENDIANCONVERT16(tempSize);

	if (tempSize == 0)
	{
		isEXIFsegmentempty = OMX_TRUE;
	}

	return exif_error;
}

OMX_ERRORTYPE EXIFParser::validate_thumbnail_size()
{
	OMX_ERRORTYPE exif_error = OMX_ErrorStreamCorrupt;

	OstTraceFiltInst3(TRACE_API,"Inside validate_thumbnail_size <-> Value of thumbnailPosition : %d,  thumbnail_size : %d and headerSize : %d ",thumbnailPosition,thumbnail_size,headerSize);
	if ((thumbnailPosition+thumbnail_size)<=headerSize)
	{
		headerCorrectionReq = OMX_FALSE;
		exif_error = OMX_ErrorNone;
	}
	return exif_error;
}
OMX_ERRORTYPE EXIFParser::calculate_DataSize(OMX_U32 *DataSize,OMX_U32 isGPSReq)
{
	OstTraceFiltInst0(TRACE_API,"EXIF Mixer: mixer_core : Entering calculate_DataSize");
	OMX_U16 tempSize = 0;
	OMX_U32 tempData = 0,gpstag_off = 0;
	OMX_ERRORTYPE exif_error = OMX_ErrorNone;
	if(isEXIFsegmentempty)
	{
		OstTraceFiltInst0(TRACE_FLOW,"EXIF Mixer: mixer_core : calculate_DataSize No Thumbnail, NO GPS, No IFD0");
		*DataSize = start_of_ifd0 + 2;
	}
	else
	{
		currentPosition = start_of_ifd0;
		exif_error = ReadTwoByte(&tempSize);
		if (OMX_ErrorNone != exif_error) 
			return OMX_ErrorStreamCorrupt;
		if (!endianness_flag)
			tempSize = ENDIANCONVERT16(tempSize);
		gpstag_off = start_of_ifd0 + 2 + (tempSize - 1) * ENTRY_SIZE ;//last directory of ifd0 should be GPS_TAG if ifd0 is not empty
		currentPosition = gpstag_off;
		currentPosition += 8;//pointing to GPS offset
		exif_error = ReadTwoByte(&tempSize);
		tempData = tempSize;
		tempData <<=16;
		exif_error = ReadTwoByte(&tempSize);
		tempData |= tempSize;
		if (!endianness_flag)
			tempData = ENDIANCONVERT32(tempData);
		gpstag_off = tempData;//GPS Offset
		if(isGPSReq)
		{
			OstTraceFiltInst0(TRACE_FLOW,"EXIF Mixer: mixer_core : calculate_DataSize No Thumbnail, YEs GPS \n");	
			*DataSize = gpstag_off + app1StartPos + 2 +(ENTRY_SIZE*valid_tag)+valid_offset+ val_take;
		}
		else
		{	
			OstTraceFiltInst0(TRACE_FLOW,"EXIF Mixer: mixer_core : calculate_DataSize No Thumbnail, IFD0 is yes but no GPS");	
			*DataSize = gpstag_off + app1StartPos;
		}
	}
	OstTraceFiltInst0(TRACE_API,"EXIF Mixer: mixer_core : Exiting calculate_DataSize");
	return exif_error;
}

OMX_ERRORTYPE EXIFParser::correctPixelDimension(OMX_U32 xDim, OMX_U32 yDim)
{
	OstTraceFiltInst0(TRACE_API,"EXIF Mixer: mixer_core : Entering correctPixelDimension");
	OMX_ERRORTYPE exif_error;
	OMX_U32 flag_x = 0,flag_y = 0,flag_exif = 0;
	OMX_U16 tempSize = 0;
	OMX_U32 tempData = 0,exif_off = 0,dir_exif = 0,tempvalue = 0,value=0,dir_ifd0 = 0;

	currentPosition = start_of_ifd0;
	exif_error = ReadTwoByte(&tempSize);
	if (OMX_ErrorNone != exif_error) 
		return OMX_ErrorStreamCorrupt;
	if (!endianness_flag)
		tempSize = ENDIANCONVERT16(tempSize);

	dir_ifd0 = tempSize;
	OstTraceFiltInst1(TRACE_FLOW,"EXIF Mixer: mixer_core : No. of IFD0 directories : %d",dir_ifd0);
	while(dir_ifd0--)
	{
		exif_error = ReadTwoByte(&tempSize);
		if (OMX_ErrorNone != exif_error) 
			return OMX_ErrorStreamCorrupt;
		if (!endianness_flag)
			  tempSize = ENDIANCONVERT16(tempSize);
		OstTraceFiltInst1(TRACE_FLOW,"EXIF Mixer: mixer_core : IFD0 Tag Type : 0x%x",tempSize);
		if(EXIF_OFFSET == tempSize)
		{
			flag_exif = 1;
			break;
		}
		// +ER 349081
		if(tempSize == Orientation)
		{
			exif_error = ReadTwoByte(&tempSize);
			if (OMX_ErrorNone != exif_error) 
				return OMX_ErrorStreamCorrupt;
			if (!endianness_flag)
				tempSize = ENDIANCONVERT16(tempSize);
			ort_type = tempSize;//Bytes per Component Field

			exif_error = ReadTwoByte(&tempSize);
			tempData = tempSize;
			tempData <<=16;
			exif_error = ReadTwoByte(&tempSize);
			tempData |= tempSize;
			if (!endianness_flag)
				tempData = ENDIANCONVERT32(tempData);
			ort_comps = tempData;//No. of Components

			exif_error = ReadTwoByte(&tempSize);
			tempData = tempSize;
			tempData <<=16;
			exif_error = ReadTwoByte(&tempSize);
			tempData |= tempSize;
			if (!endianness_flag)
				tempData = ENDIANCONVERT32(tempData);
			ort_data = tempData;//Data Value
		}
		else
			currentPosition = currentPosition + 10;
		// -ER 349081
	}
	if(flag_exif)
	{
		// Getting Offset to EXIF SubIFD
		OstTraceFiltInst0(TRACE_FLOW,"EXIF Mixer: mixer_core : ExifOFFSET exists");
		currentPosition += 6;//pointing to Exif Offset
		exif_error = ReadTwoByte(&tempSize);
		tempData = tempSize;
		tempData <<=16;
		exif_error = ReadTwoByte(&tempSize);
		tempData |= tempSize;
		if (!endianness_flag)
			tempData = ENDIANCONVERT32(tempData);
		exif_off = tempData;//Exif SubIFD Offset
		currentPosition = app1StartPos + exif_off;
		OstTraceFiltInst1(TRACE_FLOW,"EXIF Mixer: mixer_core : Value of Offset : 0x%x\n",exif_off);
		exif_error = ReadTwoByte(&tempSize);
		if (OMX_ErrorNone != exif_error) return OMX_ErrorStreamCorrupt;
		if (!endianness_flag)
			tempSize = ENDIANCONVERT16(tempSize);
		dir_exif = tempSize;
		OstTraceFiltInst1(TRACE_FLOW,"EXIF Mixer: mixer_core : ExifOFFSET exists and directories at Exif SubIFD : %d",dir_exif);
		if(tempSize == 0)
		{
			OstTraceFiltInst0(TRACE_FLOW,"EXIF Mixer: mixer_core : ExifOFFSET exists but no directories at Exif SubIFD");
			return OMX_ErrorNone;
		}
		else
		{
			while(dir_exif--)
			{
				exif_error = ReadTwoByte(&tempSize);
				if (OMX_ErrorNone != exif_error) return OMX_ErrorStreamCorrupt;
				if (!endianness_flag)
					tempSize = ENDIANCONVERT16(tempSize);
				OstTraceFiltInst1(TRACE_FLOW,"EXIF Mixer: mixer_core : Exif SubIFD Tag : 0x%x",tempSize);
				
/* Parsing removed for ER 429860*/
#if 0
				if(tempSize == Date_Time_Dzt)
				{
					OstTraceFiltInst0(TRACE_FLOW,"EXIF Mixer: mixer_core : Date Time Digitized exists");
					OMX_U32 save_cur_pos=0;
					OMX_U8 val_data = 0;
					save_cur_pos = currentPosition;
					currentPosition = currentPosition + 6;
					exif_error = ReadTwoByte(&tempSize);
					tempData = tempSize;
					tempData <<=16;
					exif_error = ReadTwoByte(&tempSize);
					tempData |= tempSize;
					if (!endianness_flag)
						tempData = ENDIANCONVERT32(tempData);
					currentPosition = tempData + app1StartPos;
					for(int as=0;as<SIZE_DATE_STAMP;as++)
					{
						exif_error = ReadByte(&val_data);
						if (OMX_ErrorNone != exif_error) 
							return OMX_ErrorStreamCorrupt;
						date_stamp[as]=val_data;
					}
					date_stamp[4]=0x3A;
					date_stamp[7]=0x3A;
					date_stamp[10]=0x00;
					currentPosition = save_cur_pos;
				}
#endif
/* Parsing removed for ER 429860*/

				if((tempSize==PixelXDim)&&(!flag_x))
				{
					OstTraceFiltInst0(TRACE_FLOW,"EXIF Mixer: mixer_core : PixelXDimension exists");
					flag_x = 1;
					tempvalue = currentPosition + 6;
					write_in_bitstream(tempvalue,xDim);
				}
				if((tempSize==PixelYDim)&&(!flag_y))
				{
					OstTraceFiltInst0(TRACE_FLOW,"EXIF Mixer: mixer_core : PixelYDimension exists");
					flag_y = 1;
					tempvalue = currentPosition + 6;
					write_in_bitstream(tempvalue,yDim);
				}
				currentPosition = currentPosition + 10;
				if(flag_x && flag_y)
				{
					OstTraceFiltInst0(TRACE_API,"EXIF Mixer: mixer_core : Exiting correctPixelDimension");	
					return OMX_ErrorNone;
				}
			}
		}
	}
	else
	{
		OstTraceFiltInst0(TRACE_API,"EXIF Mixer: mixer_core : ExifOFFSET doesn't exists and exiting correctPixelDimension");
		return OMX_ErrorNone;
	}
	OstTraceFiltInst0(TRACE_API,"EXIF Mixer: mixer_core : Exiting correctPixelDimension");	
	return OMX_ErrorNone;
}

OMX_ERRORTYPE EXIFParser::find_ifd1tag(OMX_BOOL isGPSReq)
{
	OstTraceFiltInst0(TRACE_API,"EXIF Mixer: mixer_core : Entering find_ifd1tag\n");
	OMX_ERRORTYPE exif_error;
	OMX_BOOL isOffset = OMX_FALSE;
	OMX_U16 tempSize, tagType=0;
	OMX_U32 tempData = 0,gpstag_off = 0,TotalDirEntries = 0,dir_gps = 0;
	OMX_U32 dirCounter= 0,tempvalue = 0,value = 0,bytes_to_read = 0,offset_ifd1=0;

	currentPosition = start_of_ifd0;
	exif_error = ReadTwoByte(&tempSize);
	if (OMX_ErrorNone != exif_error) return OMX_ErrorStreamCorrupt;

	if (!endianness_flag)
		tempSize = ENDIANCONVERT16(tempSize);
	TotalDirEntries = tempSize;//Total no. of directories in IFD0

	//Getting GPS Tag
	gpstag_off = start_of_ifd0 + 2 + (tempSize - 1) * ENTRY_SIZE ;//last directory of ifd0 should be GPS_TAG if ifd0 is not empty
	currentPosition = gpstag_off;
	exif_error = ReadTwoByte(&tagType);
	if (!endianness_flag)
		  tagType = ENDIANCONVERT16(tagType);
	if ((OMX_ErrorNone != exif_error) ||(GPS_TAG != tagType))
		return OMX_ErrorStreamCorrupt;

	// Getting GPS Offset
	currentPosition += 6;//pointing to GPS offset
	exif_error = ReadTwoByte(&tempSize);
	tempData = tempSize;
	tempData <<=16;
	exif_error = ReadTwoByte(&tempSize);
	tempData |= tempSize;
	if (!endianness_flag)
		tempData = ENDIANCONVERT32(tempData);
	gpstag_off = tempData;//GPS Offset


	if(!isGPSReq)
	{
		OstTraceFiltInst0(TRACE_FLOW,"EXIF Mixer: mixer_core : In find_ifd1tag, GPS Data not required\n");
		ifd1_offset = gpstag_off + app1StartPos;
		write_entry_in_bitstream(ifd1_offset,0x0000); // Setting GPS no. of directories to zero (2 bytes)
		write_in_bitstream((ifd1_offset + 2), 0x0); // No next IFD entry, 4 bytes 	/* ER 451344 */
		ifd1_offset = gpstag_off + 2;
	}
	else
	{
		OstTraceFiltInst0(TRACE_FLOW,"EXIF Mixer: mixer_core : In find_ifd1tag, GPS Data required\n");
		currentPosition = gpstag_off + app1StartPos;
		exif_error = ReadTwoByte(&tempSize);
		if (OMX_ErrorNone != exif_error) return OMX_ErrorStreamCorrupt;
		if (!endianness_flag)
			tempSize = ENDIANCONVERT16(tempSize);
		dir_gps = tempSize;//Total no. of directories in GPS
		OstTraceFiltInst2(TRACE_DEBUG,"Total no of directory entries in GPS tag %d <at line no %d> ",dir_gps,__LINE__);
		while(dirCounter < dir_gps)
		{
			exif_error = ReadTwoByte(&tempSize); //skipping tag value
			exif_error = ReadTwoByte(&tempSize); //data format
			if (OMX_ErrorNone != exif_error)
			{
				return OMX_ErrorStreamCorrupt;
			}
			if (!endianness_flag)
				tempSize = ENDIANCONVERT16(tempSize);
			value = bytes_per_component(tempSize);
			if (!value)
			{
				return OMX_ErrorUndefined;
			}

			// reading no. of components
			exif_error = ReadTwoByte(&tempSize);
			tempData = tempSize;
			tempData <<=16;
			exif_error = ReadTwoByte(&tempSize);
			tempData |= tempSize;
			if (!endianness_flag)
			tempData = ENDIANCONVERT32(tempData);
			bytes_to_read = (tempData * value);
			if ((tempData * value) > 4)
				isOffset = OMX_TRUE;
			if(isOffset)
			{
				offset_ifd1 += bytes_to_read;
				isOffset = OMX_FALSE;
			}
			currentPosition += 4; //skipping data/offset field
			dirCounter++;
		}
		ifd1_offset = gpstag_off + 2 + dir_gps*ENTRY_SIZE + offset_ifd1 + val_take; //gps_processingMethod_len is added
		ifd1_offset +=(OMX_U32)check_odd_even(&ifd1_offset);		
		OstTraceFiltInst3(TRACE_DEBUG,"Value of ifd1_offset %d and Value of val_take <at line no %d> ",ifd1_offset,val_take,__LINE__);
	}

	/* ER 451344 */
	ifd1_offset += 4; 
	/* ER 451344 */
	
	//Writing The ifd1 offset after ifd0
	tempvalue = start_of_ifd0 + 2 + TotalDirEntries*ENTRY_SIZE;
	
	write_in_bitstream(tempvalue,ifd1_offset); //writing this offset Position for IFD1 at the end of IFD0
	exif_error = setIFD1tagposition();
	currentPosition = app1StartPos + ifd1_offset;//because this is requirement by locate_and_set_ifd1_tags()
	OstTraceFiltInst0(TRACE_API,"EXIF Mixer: mixer_core : Exiting find_ifd1tag\n");
	return exif_error;
}

OMX_ERRORTYPE EXIFParser::validate_exif_header()
{
	OstTraceFiltInst0(TRACE_API,"EXIF Mixer: mixer_core : Entering validate_exif_header\n");
	OMX_ERRORTYPE exif_error;
	OMX_U16 tempSize = 0;
	OMX_U32 tempData;

	exif_error = GetNextEXIFStartCode(&tempSize); //search for APP1 segment
	if ((OMX_ErrorNone != exif_error) && (APP1_START != tempSize)) return exif_error;

	start_of_exif_offset = currentPosition - 2;

    exif_size_location = currentPosition;
	/* reading the APP1 segment size */
	/* PS: Would have to change the segment size */
	exif_error = ReadTwoByte(&tempSize);
	if (OMX_ErrorNone != exif_error) return exif_error;

	/* read the EXIF header code */
	if (tempSize > headerSize) return OMX_ErrorUndefined;

	headerSize = tempSize; //size of EXIF header

	//reading the EXIF header TAG
	exif_error = ReadTwoByte(&tempSize);
	tempData = tempSize;
	tempData <<=16;
	exif_error = ReadTwoByte(&tempSize);
	tempData |= tempSize;
	if (EXIF_HEADER != tempData) return OMX_ErrorStreamCorrupt;

	// reading the 0x0000 half-word
	exif_error = ReadTwoByte(&tempSize);
	if (tempSize) return OMX_ErrorStreamCorrupt;

	/* reading the endianness half word */
	exif_error = ReadTwoByte(&tempSize);
	switch(tempSize)
	{
		case INTEL_FORMAT :
							OstTraceFiltInst1(TRACE_DEBUG,"Values after this are in LITTLE Endian <at line %d>",__LINE__);
							consider_endianness = OMX_TRUE;
							break;
		case MOTO_FORMAT :
							OstTraceFiltInst1(TRACE_DEBUG,"Values after this is in BIG endian <at line %d>",__LINE__);
							consider_endianness = OMX_TRUE;
							endianness_flag = OMX_TRUE;
		                   break;
		default : return OMX_ErrorStreamCorrupt;
	}
	app1StartPos = currentPosition-2;

	/* reading the fixed CODE 0x002A */
	exif_error = ReadTwoByte(&tempSize);
	if (endianness_flag)
		tempSize = ENDIANCONVERT16(tempSize);

	if (FIXED_CODE != tempSize) return OMX_ErrorStreamCorrupt;

	/* reading the IFD0_OFFSET last thing to check in header validation */
	exif_error = ReadTwoByte(&tempSize);
	tempData = tempSize;
	tempData <<=16;
	exif_error = ReadTwoByte(&tempSize);
	tempData |= tempSize;
	if (endianness_flag)
		tempData = ENDIANCONVERT32(tempData);

	//it is a recommendation that offset be 0x00000008 but may not always be true
	//if (IFD0_OFFSET != tempData) return OMX_ErrorStreamCorrupt;

	tempData = ENDIANCONVERT32(tempData);

	//validation of header complete and buffer ptr would be at start of IFD0
	currentPosition = tempData + app1StartPos;
	start_of_ifd0 = currentPosition;
	OstTraceFiltInst0(TRACE_API,"EXIF Mixer: mixer_core : Exiting validate_exif_header\n");
	return exif_error;
}

OMX_BOOL exif_mixer_arm_nmf_mixer_core::buffer_available_atinput(OMX_U32 PortIndex)
{
    OMX_BUFFERHEADERTYPE **pBuffer = 0;
    if (PortIndex == 0)
    {
		pBuffer = &pBuffer1;
	}

    if (PortIndex == 1)
    {
		pBuffer = &pBuffer2;
	}
	if (!pBuffer) return OMX_FALSE;

	if(((*pBuffer) == NULL) && (mPorts[PortIndex].queuedBufferCount()))
	{
		*pBuffer = mPorts[PortIndex].dequeueBuffer();
		isBufferDequeued[PortIndex] = OMX_TRUE;
		OstTraceFiltInst3(TRACE_FLOW,"Dequeueing Input buffer : 0x%x from Port : %d <at line %d>",(OMX_U32)*pBuffer,PortIndex,__LINE__);
		return OMX_TRUE;
	}
	else if(*pBuffer)
		return OMX_TRUE;
	else
		return OMX_FALSE;
}

OMX_BOOL exif_mixer_arm_nmf_mixer_core::buffer_available_atoutput()
{
	if((pBuffer3 == NULL) && (mPorts[2].queuedBufferCount()))
	{
		pBuffer3 = mPorts[2].dequeueBuffer();
		isBufferDequeued[2] = OMX_TRUE;
		OstTraceFiltInst2(TRACE_FLOW,"Dequeueing Output buffer : 0x%x <at line %d>",(OMX_U32)pBuffer3,__LINE__);
		return OMX_TRUE;
	}
	else if(pBuffer3)
		return OMX_TRUE;
	else
		return OMX_FALSE;
}

void exif_mixer_arm_nmf_mixer_core::eventProcess(void)
{
	OstTraceFiltInst1(TRACE_API,"Inside eventProcess function <at line %d>",__LINE__);
	ResolveDependencies();
}


void exif_mixer_arm_nmf_mixer_core::ResolveDependencies()
{
	OstTraceFiltInst1(TRACE_API,"Inside ResolveDependencies function <at line %d>",__LINE__);
    /* getting input and output buffers */
	if(!inputDependencyResolved)
	{
		if (mPorts[0].isEnabled()) //when thumbnail port is enabled
		{
			isThumbnailEnabled = OMX_TRUE;
			if((buffer_available_atinput(0)) && (buffer_available_atinput(1)))
			{
				inputDependencyResolved = OMX_TRUE;
			}
		}
		else
		{
			isThumbnailEnabled = OMX_FALSE;
			if(buffer_available_atinput(1))
			{
				inputDependencyResolved = OMX_TRUE;
			}
		}
	}
	if(!outputDependencyResolved)
	{
		if(buffer_available_atoutput())
		{
			outputDependencyResolved = OMX_TRUE;
		}

	}

	if (inputDependencyResolved && outputDependencyResolved )
	{
		OstTraceFiltInst1(TRACE_FLOW,"All dependencies resolved <at line %d>",__LINE__);
		processActual();
	}
}

void exif_mixer_arm_nmf_mixer_core::process_exif_image()
{	
	OMX_ERRORTYPE error;
	OMX_U32 thumbnailposition=0;	
	OMX_U8 *pDestBuffer;
	OMX_U32 tempPosition;
	OMX_BOOL set_ifd1_req = OMX_TRUE;
	OstTraceFiltInst1(TRACE_API,"Entering actual processing function line no : %d ",__LINE__);

	processActualExecution = OMX_TRUE;

	exifparser.initialize_data(pBuffer2);

	exifparser.globalPtr = &globalgpslocation;

	exifparser.gpslocation.nLatitudeDegrees = globalgpslocation.nLatitudeDegrees;
	exifparser.gpslocation.nLatitudeMinutes = globalgpslocation.nLatitudeMinutes;
    exifparser.gpslocation.nLatitudeSeconds = globalgpslocation.nLatitudeSeconds;
	exifparser.gpslocation.nLongitudeDegrees = globalgpslocation.nLongitudeDegrees;
	exifparser.gpslocation.nLongitudeMinutes = globalgpslocation.nLongitudeMinutes;
	exifparser.gpslocation.nLongitudeSeconds = globalgpslocation.nLongitudeSeconds;
	exifparser.gpslocation.nAltitudeMeters = globalgpslocation.nAltitudeMeters;
	exifparser.gpslocation.bLatitudeRefNorth = globalgpslocation.bLatitudeRefNorth;
	// +ER 355538
	exifparser.gpslocation.bLongitudeRefEast = globalgpslocation.bLongitudeRefEast;
	// -ER 355538
	exifparser.gpslocation.bAltitudeRefAboveSea = globalgpslocation.bAltitudeRefAboveSea;
	exifparser.gpslocation.bLocationKnown = globalgpslocation.bLocationKnown;
	exifparser.gpslocation.nHours = globalgpslocation.nHours;
	exifparser.gpslocation.nMinutes = globalgpslocation.nMinutes;
    exifparser.gpslocation.nSeconds = globalgpslocation.nSeconds;

    /* ER 429860 */
    exifparser.gpslocation.nYear = globalgpslocation.nYear;
    exifparser.gpslocation.nMonth = globalgpslocation.nMonth;
    exifparser.gpslocation.nDay = globalgpslocation.nDay;
    /* ER 429860 */

	exifparser.gpslocation.processingValue = globalgpslocation.processingValue;
	for(int asa=0;asa<SIZE_GPS_PR_MTD;asa++)
		exifparser.gpslocation.nPrMethodDataValue[asa]=globalgpslocation.nPrMethodDataValue[asa];

	//validating the EXIF header
	OstTraceFiltInst1(TRACE_FLOW,"process_exif_image::About to call validate_exif_header from process_exif_image <at line %d>",__LINE__);

	error = exifparser.validate_exif_header();
	if (OMX_ErrorNone != error)
	{
		execCode = INVALID_HEADER;
		OstTraceFiltInst2(TRACE_ERROR,"execCode : %d <at line %d>",execCode,__LINE__);
	}

	OSTTRACE_ERROR(error);

	CHECKERROR_SENDEVENT(error);

	OstTraceFiltInst2(TRACE_FLOW,"process_exif_image::Value of globalgpslocation.bLocationKnown : %d <at line %d>",globalgpslocation.bLocationKnown,__LINE__);
	OstTraceFiltInst1(TRACE_FLOW,"process_exif_image::Abt to call isEXIFbufferEmpty <at line %d>",__LINE__);

	error = exifparser.isEXIFbufferEmpty();
	OSTTRACE_ERROR(error);
	CHECKERROR_SENDEVENT(error);

	if(!exifparser.isEXIFsegmentempty)
	{
		OstTraceFiltInst2(TRACE_FLOW,"Mixer_Core : process_exif_image : Before calling correctPixelDimension xDim : %d, yDim : %d\n",xDim,yDim);
		error = exifparser.correctPixelDimension(xDim,yDim);
	}

	if (exifparser.isEXIFsegmentempty) //EXIF segment beyong the header is Empty
	{
		OstTraceFiltInst1(TRACE_FLOW,"EXIF Header segment is Empty <at line %d>",__LINE__);
		OSTTRACE_ERROR(error);

		CHECKERROR_SENDEVENT(error);

		if (download_parameters && globalgpslocation.bLocationKnown) //change for ER 352049
		{
			OstTraceFiltInst1(TRACE_DEBUG,"process_exif_image::Abt to call createGPStagposition <at line %d>",__LINE__);
			error = exifparser.createGPStagposition(OMX_TRUE,0);
			OstTraceFiltInst1(TRACE_DEBUG,"process_exif_image::IFD1 Tag has already been set in this particular use case",__LINE__);
			set_ifd1_req = OMX_FALSE;
			//after this it is expected that position for IFD1 has been created
			//so isEXIFsegmentempty = OMX_FALSE
		}
		if (isThumbnailEnabled && !(globalgpslocation.bLocationKnown))
		{
			OstTraceFiltInst2(TRACE_DEBUG,"Abt to call setIFD1tagposition when isThumbnailEnabled : %d <at line %d>",isThumbnailEnabled,__LINE__);
			error = exifparser.setIFD1tagposition();
		}
		else
		{
		OstTraceFiltInst1(TRACE_DEBUG,"IFD1 Tag not inserted as Thumbnail insertion is disabled <at line %d>",__LINE__);
		}
	}

	if (download_parameters && globalgpslocation.bLocationKnown) //change for ER 352049
	{
		OstTraceFiltInst1(TRACE_DEBUG,"Locating GPS offset <at line %d>",__LINE__);
    	//locate the GPS tag and place the ptr at the data position for GPS info
		error = exifparser.locate_gps_tag(&tempPosition);
		if (OMX_ErrorNone != error)
		{
			execCode = GPS_SUBTAG_NOTFOUND;

			OstTraceFiltInst1(TRACE_DEBUG,"GPS SubTag not found <at line %d>",__LINE__);
			OstTraceFiltInst2(TRACE_DEBUG,"execCode : %d <at line %d>",execCode,__LINE__);
		}
	}

	if (!download_parameters)
	{
		OstTraceFiltInst1(TRACE_DEBUG,"process_exif_image::Retreiving GPS Data if setconfig not done Line no. : %d",__LINE__);
		globalgpslocation.nLatitudeDegrees = exifparser.gpslocation.nLatitudeDegrees;
		globalgpslocation.nLatitudeMinutes = exifparser.gpslocation.nLatitudeMinutes;
    	globalgpslocation.nLatitudeSeconds = exifparser.gpslocation.nLatitudeSeconds;
		globalgpslocation.nLongitudeDegrees = exifparser.gpslocation.nLongitudeDegrees;
		globalgpslocation.nLongitudeMinutes = exifparser.gpslocation.nLongitudeMinutes;
		globalgpslocation.nLongitudeSeconds = exifparser.gpslocation.nLongitudeSeconds;
		globalgpslocation.nAltitudeMeters = exifparser.gpslocation.nAltitudeMeters;
		globalgpslocation.bLatitudeRefNorth = exifparser.gpslocation.bLatitudeRefNorth;
		globalgpslocation.bLongitudeRefEast = exifparser.gpslocation.bLongitudeRefEast;
		globalgpslocation.bAltitudeRefAboveSea = exifparser.gpslocation.bAltitudeRefAboveSea;
		globalgpslocation.bLocationKnown = exifparser.gpslocation.bLocationKnown;
		globalgpslocation.nHours = exifparser.gpslocation.nHours;
		globalgpslocation.nMinutes = exifparser.gpslocation.nMinutes;
    	globalgpslocation.nSeconds = exifparser.gpslocation.nSeconds;
    	/* ER 429860 */
    	globalgpslocation.nYear = exifparser.gpslocation.nYear;
    	globalgpslocation.nMonth = exifparser.gpslocation.nMonth;
    	globalgpslocation.nDay = exifparser.gpslocation.nDay;
    	/* ER 429860 */
		globalgpslocation.processingValue = exifparser.gpslocation.processingValue;
		for(int a=0;a<SIZE_GPS_PR_MTD;a++)
			globalgpslocation.nPrMethodDataValue[a]=exifparser.gpslocation.nPrMethodDataValue[a];
	}

	//locating the IFD1 section
	if(isThumbnailEnabled)
	{
		OstTraceFiltInst2(TRACE_FLOW,"Exif_Mixer: process_exif_image: Locating Ifd1 Offset, execCode : %d <at line %d>",execCode,__LINE__);
		if(!exifparser.isEXIFsegmentempty)
		{
			exifparser.is_ifd1_tag = OMX_TRUE;
			error = exifparser.locate_ifd1_section();
			if((error==OMX_ErrorNone)&&(set_ifd1_req))// It means camera has set value of ifd1 offset, which is erroneous
			{
				OstTraceFiltInst1(TRACE_WARNING,"Error: IFD1 tag value set by camera which isn't allowed, therefore it will be overwritten <at line %d>",__LINE__);
				exifparser.is_ifd1_tag = OMX_TRUE;

			}
			else
			{
				OstTraceFiltInst2(TRACE_DEBUG,"IFD1 tag value not set by camera, execCode : %d <at line %d>",execCode,__LINE__);
				error = OMX_ErrorNone;
			}
		}
		else
		{
			OstTraceFiltInst1(TRACE_DEBUG,"process_exif_image::use case of empty EXIF Segment, No GPS to be inserted <at line %d>",__LINE__);
			exifparser.is_ifd1_tag = OMX_TRUE;
			error = exifparser.locate_ifd1_section();
		}
	}
	if((isThumbnailEnabled)&&(!exifparser.isEXIFsegmentempty)&&(error==OMX_ErrorNone))
	{
		OstTraceFiltInst1(TRACE_FLOW,"EXIF Mixer : process_exif_image, Calculating ifd1 offset line no : %d ",__LINE__);
		error = exifparser.find_ifd1tag(globalgpslocation.bLocationKnown);
	}

	if (isThumbnailEnabled)
	{
		if (OMX_ErrorNone != error)
		{
			exifparser.is_ifd1_tag = OMX_FALSE;
			execCode = IFD1_TAG_MISSING;
			OstTraceFiltInst2(TRACE_ERROR,"execCode : %d <at line %d>",execCode,__LINE__);
		}

		OSTTRACE_ERROR(error);
		CHECKERROR_SENDEVENT(error);

		if (exifparser.is_ifd1_tag)
		{
			//locating the sections and updating IFD1 sub-tags
			exifparser.thumbnail_size = pBuffer1->nFilledLen;
			OstTraceFiltInst2(TRACE_FLOW,"Thumbnail size: %d <at line %d>",exifparser.thumbnail_size,__LINE__);
			error = exifparser.locate_and_set_ifd1_tags(&thumbnailposition);
			if (OMX_ErrorNone != error)
			{
				exifparser.is_ifd1_tag = OMX_FALSE;
				execCode = IFD1_SUBTAG_NOTFOUND;
				OstTraceFiltInst2(TRACE_ERROR,"execCode : %d <at line %d>",execCode,__LINE__);
			}

			OSTTRACE_ERROR(error);
			CHECKERROR_SENDEVENT(error);
			exifparser.is_ifd1_tag = OMX_TRUE;

			error = exifparser.validate_thumbnail_size();
			if (OMX_ErrorNone != error)
			{
				exifparser.headerCorrectionReq = OMX_TRUE;
				execCode = INCORRECT_THUMBNAIL_SIZE;
				OstTraceFiltInst1(TRACE_ERROR,"=>CAUTION<= Thumbnail size is MORE than space in EXIF header segment <at line %d>",__LINE__);
				OstTraceFiltInst1(TRACE_ERROR,"Would be corrected Later in CODE <at line %d>",__LINE__);
				OSTTRACE_ERROR(error);
				CHECKERROR_SENDEVENT(error);
			}
		}
	}
	exifparser.headerCorrectionReq = OMX_TRUE;
	OMX_U32 bytes_to_copy,new_offset;
	if ((exifparser.is_ifd1_tag) && (isThumbnailEnabled))
	{
		//get actual header size and update in EXIF header
		//correct thumbnail position found
		//Changes to reduce fileSize
		OstTraceFiltInst1(TRACE_FLOW,"Value of thumbnailposition: 0x%x",thumbnailposition);
		bytes_to_copy = thumbnailposition + pBuffer1->nFilledLen;
		new_offset = 65538 - bytes_to_copy ;
		pDestBuffer = pBuffer2->pBuffer + pBuffer2->nOffset + new_offset;
		OstTraceFiltInst2(TRACE_FLOW,"Value of new_offset: 0x%x, bytes_to_copy : 0x%x",new_offset,bytes_to_copy);
		memmove(pDestBuffer,pBuffer2->pBuffer + pBuffer2->nOffset,(OMX_U32)thumbnailposition);
		pBuffer2->nOffset = pBuffer2->nOffset + new_offset;//Updating Offset
		pBuffer2->nFilledLen = pBuffer2->nFilledLen - new_offset ;//Updating Filled Length
		exifparser.correctHeaderSize((bytes_to_copy - 4),pBuffer2);//Excluding SOI Tag and APP1 Marker 
		pDestBuffer = pBuffer2->pBuffer + pBuffer2->nOffset + thumbnailposition;//Copying Thumbnail
		memcpy(pDestBuffer,(pBuffer1->pBuffer + pBuffer1->nOffset),pBuffer1->nFilledLen);
	}
	else
	{
		OstTraceFiltInst0(TRACE_FLOW,"process_exif_image :: When thumbnail is not to be inserted, Calculating valid EXIF HEADER Data");
		error = exifparser.calculate_DataSize(&bytes_to_copy,globalgpslocation.bLocationKnown);
		OSTTRACE_ERROR(error);
		CHECKERROR_SENDEVENT(error);
		new_offset = 65538 - bytes_to_copy ;
		pDestBuffer = pBuffer2->pBuffer + pBuffer2->nOffset + new_offset;
		memcpy(pDestBuffer,pBuffer2->pBuffer + pBuffer2->nOffset,bytes_to_copy);
		pBuffer2->nOffset = pBuffer2->nOffset + new_offset;//Updating Offset
		pBuffer2->nFilledLen = pBuffer2->nFilledLen - new_offset ;//Updating Filled Length
		exifparser.correctHeaderSize((bytes_to_copy - 4),pBuffer2);//Excluding SOI Tag and APP1 Marker 
	}
	memcpy(pBuffer3->pBuffer,(pBuffer2->pBuffer + pBuffer2->nOffset),pBuffer2->nFilledLen);
	pBuffer3->nFilledLen = pBuffer2->nFilledLen;
	pBuffer3->nFlags = pBuffer2->nFlags;
	pBuffer3->nTimeStamp = pBuffer2->nTimeStamp;

	//copy the OMX_CONFIG_GPSLOCATIONTYPE structure after the nFilledLen
	if ((download_parameters) &&(!globalgpslocation.bLocationKnown))
	{
		OMX_U8 *ptr;
		OMX_OTHER_EXTRADATATYPE *tempPtr;
		OMX_U32 tempSize,tempValue;

		pBuffer3->nFlags |= OMX_BUFFERFLAG_EXTRADATA;

	OstTraceFiltInst1(TRACE_FLOW,"Putting the GPS in EXTRADATA for Output buffer <at line %d>",__LINE__);
	OstTraceFiltInst3(TRACE_DEBUG,"Size of OMX_OTHER_EXTRADATATYPE : %d OMX_U8 : %d and OMX_CONFIG_GPSLOCATIONTYPE : %d ",sizeof(OMX_OTHER_EXTRADATATYPE),sizeof(OMX_U8),sizeof(OMX_CONFIG_GPSLOCATIONTYPE));
		//adding Padding bytes if end is not word aligned
		tempSize = (((sizeof(OMX_OTHER_EXTRADATATYPE) - sizeof(OMX_U8) + sizeof(OMX_CONFIG_GPSLOCATIONTYPE)) + 3)/4) * 4;

	OstTraceFiltInst2(TRACE_DEBUG,"Value of tempSize : %d <at line %d>",tempSize,__LINE__);
		extradata.nSize = tempSize;
		extradata.nVersion.s.nVersionMajor    = 1;
		extradata.nVersion.s.nVersionMinor    = 1;
		extradata.nVersion.s.nRevision	= 1;
		extradata.nVersion.s.nStep		= 0;
		extradata.nPortIndex = 2;
		extradata.eType = OMX_ExtraDataQuantization;
		extradata.nDataSize = sizeof(OMX_CONFIG_GPSLOCATIONTYPE);

		//copying the OMX_OTHER_EXTRADATATYPE structure
		ptr = pBuffer3->pBuffer + pBuffer3->nOffset + pBuffer3->nFilledLen;

	OstTraceFiltInst2(TRACE_DEBUG,"Position of copying OMX_OTHER_EXTRADATATYPE ptr : 0x%x <at line %d>",(OMX_U32)ptr,__LINE__);

		tempValue = (((OMX_U32)ptr + 3)/4)*4; //making it word aligned
		ptr = (OMX_U8 *)tempValue;

		OstTraceFiltInst2(TRACE_DEBUG,"Actual Position of copying OMX_OTHER_EXTRADATATYPE ptr : 0x%x <at line %d>",(OMX_U32)ptr,__LINE__);

		memcpy(ptr,(OMX_U8*)&extradata,sizeof(OMX_OTHER_EXTRADATATYPE));

		//copying the OMX_CONFIG_GPSLOCATIONTYPE structure
		tempPtr = (OMX_OTHER_EXTRADATATYPE *)ptr;

		ptr = &(tempPtr->data[0]);

		OstTraceFiltInst2(TRACE_DEBUG,"Position of copying OMX_CONFIG_GPSLOCATIONTYPE ptr : 0x%x <at line %d>",(OMX_U32)ptr,__LINE__);
		memcpy(ptr,(OMX_U8*)&globalgpslocation,sizeof(OMX_CONFIG_GPSLOCATIONTYPE));

		extradata.nSize = sizeof(OMX_OTHER_EXTRADATATYPE);
		extradata.nVersion.s.nVersionMajor    = 1;
		extradata.nVersion.s.nVersionMinor    = 1;
		extradata.nVersion.s.nRevision	= 1;
		extradata.nVersion.s.nStep		= 0;
		extradata.nPortIndex = 2;
		extradata.eType = OMX_ExtraDataNone;
		extradata.nDataSize = 0;

		//copying the OMX_OTHER_EXTRADATATYPE structure
		tempValue = ((((OMX_U32)(pBuffer3->pBuffer + pBuffer3->nOffset + pBuffer3->nFilledLen)) + 3)/4);
		tempValue = tempValue *4;
		tempValue = tempValue + tempSize;
		ptr = (OMX_U8 *)tempValue;

		OstTraceFiltInst2(TRACE_DEBUG,"Position of copying end OMX_OTHER_EXTRADATATYPE ptr : 0x%x <at line %d>",(OMX_U32)ptr,__LINE__);
		memcpy(ptr,(OMX_U8*)&extradata,sizeof(OMX_OTHER_EXTRADATATYPE));

	}
	execCode = FULL_PROCESS_DONE;
	OstTraceFiltInst1(TRACE_API,"Full processing done,Exiting process_exif_image arm_nmf <at line %d>",__LINE__);
	processActualExecution = OMX_FALSE;
	download_parameters = OMX_FALSE;//change for ER 352049
}



void exif_mixer_arm_nmf_mixer_core::returnbuffer_Port(OMX_BUFFERHEADERTYPE **pBuffer,OMX_U32 portIndex)
{
	if (isBufferDequeued[portIndex] && (*pBuffer))
	{
	OstTraceFiltInst3(TRACE_FLOW,"Returning the buffer : 0x%x from Port : %d <at line %d>",(OMX_U32)*pBuffer,portIndex,__LINE__);
		if(isDispatching())
		{
			mPorts[portIndex].returnBuffer(*pBuffer);
		}
		else
		{
			returnBufferAsync(portIndex,*pBuffer);
   		}
		*pBuffer = 0x0;
		isBufferDequeued[portIndex] = OMX_FALSE;
	}
	else
		OstTraceFiltInst3(TRACE_FLOW,"Buffer : 0x%x from Port : %d is ALREADY FREE <at line %d>",(OMX_U32)*pBuffer,portIndex,__LINE__);

}



void exif_mixer_arm_nmf_mixer_core::processActual()
{
	OstTraceFiltInst1(TRACE_API,"Entering processActual <at line %d>",__LINE__);
	OstTraceFiltInst3(TRACE_FLOW,"Value of pBuffer1 : 0x%x  pBuffer2 : 0x%x pBuffer3 : 0x%x ",(OMX_U32)pBuffer1,(OMX_U32)pBuffer2,(OMX_U32)pBuffer3);
	if (isThumbnailEnabled)
	{
		OstTraceFiltInst1(TRACE_DEBUG,"Thumbnail enabled : full processing <at line %d>",__LINE__);
     	if ((pBuffer1->nFilledLen != 0) && (pBuffer2->nFilledLen != 0))
     	{
			OstTraceFiltInst1(TRACE_DEBUG,"Normal case with Thumbnail line no <at line %d>",__LINE__);
			process_exif_image();
		}
		else
		{
			OstTraceFiltInst1(TRACE_DEBUG,"return without any processing from full processing <at line %d>",__LINE__);
			//actual processing
			OstTraceFiltInst1(TRACE_DEBUG,"REturn without processing <at line %d>",__LINE__);
			execCode = END_NO_PROCESSING;
		}
	}
	else
	{
		if (pBuffer2->nFilledLen != 0) //only JPEG buffer
		{
			OstTraceFiltInst1(TRACE_DEBUG,"No Thumbnail only buffer copy operation <at line %d>",__LINE__);
			OstTraceFiltInst1(TRACE_DEBUG,"Case with no Thumbnail line no : %d \n",__LINE__);
			process_exif_image();
		}
		else
		{
			//actual processing
			OstTraceFiltInst1(TRACE_DEBUG,"REturn without processing line no : %d \n",__LINE__);
			execCode = END_NO_PROCESSING;
		}
	}
	OstTraceFiltInst1(TRACE_DEBUG,"Returning the buffers <at line %d>",__LINE__);
    inputDependencyResolved = OMX_FALSE;
    outputDependencyResolved = OMX_FALSE;
	iSendLogEvent.eventProcessOutput();
	OstTraceFiltInst1(TRACE_API,"Exiting processActual <at line %d>",__LINE__);
	//OstTraceFiltInst2("\n codec_algo.controlAlgo exiting");
}


void exif_mixer_arm_nmf_mixer_core::eventProcessOutput(void)
{
	OMX_BOOL isEOSTrue = OMX_FALSE;
	OMX_U32 tempFlags = 0;

    OstTraceFiltInst1(TRACE_API,"Inside eventProcessOutput function <at line %d>",__LINE__);
	if(pBuffer2->nFlags & OMX_BUFFERFLAG_EOS)
	{
		OstTraceFiltInst1(TRACE_DEBUG,"Inside nFlags <at line %d>",__LINE__);
		isEOSTrue = OMX_TRUE;
	}

	if (isThumbnailEnabled)
	{
		pBuffer1->nOffset = 0;
		pBuffer1->nFilledLen = 0;
		OstTraceFiltInst1(TRACE_DEBUG,"Return buffer for Port 0 <at line %d>",__LINE__);
		returnbuffer_Port(&pBuffer1,0);
	}

	pBuffer2->nOffset = 0; //ER No. 402257
	pBuffer2->nFilledLen = 0; //making the buffer size to Zero

	OstTraceFiltInst1(TRACE_FLOW,"Return buffer for Port 1 <at line %d>",__LINE__);

	returnbuffer_Port(&pBuffer2,1);

    //check for EOS & Generate event if required
	if(isEOSTrue)
	{
		pBuffer3->nFlags |= OMX_BUFFERFLAG_EOS;
		if (FULL_PROCESS_DONE == execCode || HALF_PROCESS_DONE == execCode)
				pBuffer3->nFlags |= OMX_BUFFERFLAG_ENDOFFRAME;

		tempFlags = pBuffer3->nFlags;
	}
	OstTraceFiltInst1(TRACE_FLOW,"Return buffer for Port 2 <at line %d>",__LINE__);
	returnbuffer_Port(&pBuffer3,2);

	inputDependencyResolved		=OMX_FALSE;
	outputDependencyResolved	=OMX_FALSE;

	if(isEOSTrue)
	{
		OstTraceFiltInst1(TRACE_FLOW,"Sending OMX_EventBufferFlag, arm_nmf <at line %d>",__LINE__);
		proxy.eventHandler(OMX_EventBufferFlag, 2, tempFlags);
		isEOSTrue = OMX_FALSE;
	}


	iSendLogEvent.eventProcess();
	OstTraceFiltInst1(TRACE_API,"End of eventProcessOutput function <at line %d>",__LINE__);
}



exif_mixer_arm_nmf_mixer_core::exif_mixer_arm_nmf_mixer_core()
{
	inputDependencyResolved	  = OMX_FALSE;
	outputDependencyResolved  = OMX_FALSE;
	isThumbnailEnabled = OMX_TRUE;
	execCode = PROCESSING_NOT_DONE;
	pBuffer1 = 0x0;
	pBuffer2 = 0x0;
	pBuffer3 = 0x0;
	isBufferDequeued[0] = OMX_FALSE;
	isBufferDequeued[1] = OMX_FALSE;
	isBufferDequeued[2] = OMX_FALSE;
	processActualExecution = OMX_FALSE;
	globalgpslocation.nLatitudeDegrees = 0;
	globalgpslocation.nLatitudeMinutes = 0;
	globalgpslocation.nLatitudeSeconds = 0;
	globalgpslocation.nLongitudeDegrees = 0;
	globalgpslocation.nLongitudeMinutes = 0;
	globalgpslocation.nLongitudeSeconds = 0;
	globalgpslocation.nAltitudeMeters = 0;
	globalgpslocation.bLatitudeRefNorth = OMX_FALSE;
	globalgpslocation.bLongitudeRefEast = OMX_FALSE;
	globalgpslocation.bAltitudeRefAboveSea = OMX_FALSE;
	globalgpslocation.bLocationKnown = OMX_FALSE;
	globalgpslocation.nHours = 0;
	globalgpslocation.nMinutes = 0;
	globalgpslocation.nSeconds = 0;
	/* ER 429860 */
	globalgpslocation.nYear = 0;
	globalgpslocation.nMonth = 0;
	globalgpslocation.nDay = 0;
	/* ER 429860 */
	globalgpslocation.nPortIndex = 0;
	globalgpslocation.processingValue =  (OMX_KEY_GPS_PROCESSING_METHODTYPE)0;
	for(int as=0;as<SIZE_GPS_PR_MTD;as++)
		globalgpslocation.nPrMethodDataValue[as]=0;
	download_parameters = OMX_FALSE;

 	exifparser.globalPtr = 0;
 	exifparser.thumbnail_size = 0;

 	extradata.eType = (OMX_EXTRADATATYPE)0;
 	extradata.nDataSize = 0;
 	extradata.nPortIndex = 0;
 	extradata.nSize = 0;
 	globalgpslocation.nSize = 0;
 	extradata.data[0] = 0;
	xDim = 0;
	yDim = 0;
	//traceObject.mId1 = 0;
	//traceObject.mTraceInfoPtr = 0;
}



exif_mixer_arm_nmf_mixer_core::~exif_mixer_arm_nmf_mixer_core()
{

}


void exif_mixer_arm_nmf_mixer_core::process()
{
	OstTraceFiltInst1(TRACE_FLOW,"inside process function <at line %d>",__LINE__);
	ResolveDependencies();
}

void exif_mixer_arm_nmf_mixer_core::processEvent(void)
{
    Component::processEvent();
}


void METH(setTunnelStatus)(t_sint16 portIdx, t_uint16 isTunneled)
{
    if (portIdx == -1)
    {
        for (int i=0; i<EXIF_MIXER_PORTS_NB; i++)
        {
            if (isTunneled & (1<<i))
            {
                mPorts[i].setTunnelStatus(true);
            }
            else
            {
                mPorts[i].setTunnelStatus(false);
            }
        }
    }
    else
    {
        if (isTunneled & (1<<portIdx))
        {
            mPorts[portIdx].setTunnelStatus(true);
        }
        else
        {
            mPorts[portIdx].setTunnelStatus(false);
        }
    }
}

void exif_mixer_arm_nmf_mixer_core::emptyThisBuffer(OMX_BUFFERHEADERTYPE_p pBufferHeader, t_uint8 PortIdx)
{
	if(pBufferHeader->nFilledLen==0)
    {
        OstTraceFiltInst0(TRACE_ERROR, "In EXIFMixer : Mixer_Core : Empty Buffer Pushed");
        proxy.eventHandler(OMX_EventError,OMX_ErrorUnderflow,0);
    }
    Component::deliverBuffer( PortIdx, pBufferHeader);

}

t_nmf_error exif_mixer_arm_nmf_mixer_core::construct(void)
{
	return NMF_OK;
}


void exif_mixer_arm_nmf_mixer_core::setParam(t_uint32 portIndex, t_uint32 fifoSize, t_uint32 direction, t_uint32 buffSupplierType)
{
}


void exif_mixer_arm_nmf_mixer_core::fillThisBuffer(OMX_BUFFERHEADERTYPE_p pBufferHeader)
{


    Component::deliverBuffer( 2, pBufferHeader);

}


void METH(sendCommand)(OMX_COMMANDTYPE cmd, t_uword parameter)
{
    OstTraceFiltInst1(TRACE_API,"sendCommand received <at line %d>",__LINE__);
	switch (cmd)
	{
		case OMX_CommandStateSet:
		{
			switch (parameter)
			{
				case OMX_StatePause:
				case OMX_StateExecuting: break;
				case OMX_StateIdle:
						if(!processActualExecution)
						{
							returnbuffer_Port(&pBuffer1,0);
							returnbuffer_Port(&pBuffer2,1);
							returnbuffer_Port(&pBuffer3,2);
							inputDependencyResolved		=OMX_FALSE;
							outputDependencyResolved	=OMX_FALSE;
						}
						break;
			}
		}
		break;

		case OMX_CommandFlush:
		case OMX_CommandPortDisable:
		{
			switch (parameter)
			{
				case 0:
						returnbuffer_Port(&pBuffer1,0);
						inputDependencyResolved		=OMX_FALSE;
					break;

				case 1:
						returnbuffer_Port(&pBuffer2,1);
						inputDependencyResolved	=OMX_FALSE;
					break;

				case 2:
						returnbuffer_Port(&pBuffer3,2);
						outputDependencyResolved	=OMX_FALSE;
					break;
			}
		}
		break;

		case OMX_CommandPortEnable:
			break;
		default:
			//To remove warning in linux
			break;

	}

	Component::sendCommand(cmd, parameter) ;
}


//< Changes from Week12 in ENS
#ifdef NEWFSMINIT
void METH(fsmInit)(fsmInit_t initFSM)
#else
void METH(fsmInit)(t_uint16 portsDisabled,t_uint16 portsTunneled)
#endif
{
#ifdef NEWFSMINIT
    t_uint16 portsDisabled = initFSM.portsDisabled;
    t_uint16 portsTunneled = initFSM.portsTunneled;
#endif

    // trace initialization
    if (initFSM.traceInfoAddr)
    {
        setTraceInfo(initFSM.traceInfoAddr, initFSM.id1);
		exifparser.setTraceInfo(initFSM.traceInfoAddr, initFSM.id1);
		//exifparser.comp_ptr = this;
		//exifparser.comp_ptr->traceObject.mId1 = initFSM.id1;
		//exifparser.comp_ptr->traceObject.mTraceInfoPtr = initFSM.traceInfoAddr;
	}
	//printf ("Inside FSM Init function <at line %d>",__LINE__)

    	//Input port can keep buffers till complete data is not available
	mPorts[0].init(InputPort, /* port direction */
								 false, /* isBufferSupplier */
								 true, /* isHWPort */
								 0, /* port we're sharing with */
								 0, /* initialBuffers ptrFIXME: mBufIn_list*/
								 EXIF_MIXER_CORE_FIFO_SIZE_MAX, /* bufferCount */
								 &emptybufferdone[0], /* return interface */
								 0, /* portIdx */
								 portsDisabled&1, /* isDisabled */
								 portsTunneled&1,
								 (Component *)this /* componentOwner */
								 );

	mPorts[1].init(InputPort, /* port direction */
								 false, /* isBufferSupplier */
								 true, /* isHWPort */
								 0, /* port we're sharing with */
								 0, /* initialBuffers ptr FIXME: mBufOut_list*/
								 EXIF_MIXER_CORE_FIFO_SIZE_MAX, /* bufferCount */
								 &emptybufferdone[1], /* return interface */
								 1, /* portIdx */
								 (portsDisabled >> 1)&1, /* isDisabled */
								 (portsTunneled>>1)&1,
								 (Component *)this /* componentOwner */
								 );

	mPorts[2].init(OutputPort, /* port direction */
								 false, /* isBufferSupplier */
								 true, /* isHWPort */
								 0, /* port we're sharing with */
								 0, /* initialBuffers ptr FIXME: mBufOut_list*/
								 EXIF_MIXER_CORE_FIFO_SIZE_MAX, /* bufferCount */
								 &fillbufferdone, /* return interface */
								 2, /* portIdx */
								 (portsDisabled >> 1)&1, /* isDisabled */
								 (portsTunneled>>1)&1,
								 (Component *)this /* componentOwner */
								 );

    init(EXIF_MIXER_PORTS_NB, mPorts, &proxy, &me, 0); //FIXME:invalid parameter is to be initilaized properly.

   //printf ("Exiting FSM Init function <at line %d>",__LINE__)
}
