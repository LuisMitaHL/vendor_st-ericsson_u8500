/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _EXIF_MIXER_CORE_H_
#define _EXIF_MIXER_CORE_H_

#include "Component.h"
#include "OMX_Types.h"


#include "Component.h"
#include "ENS_List.h"
#include "OMX_Component.h"
//#include "VFM_Types.h"
//#include "VFM_Index.h"
#include "video_chipset_api_index.h"
#include "video_generic_chipset_api.h"


#include "ENS_DBC.h"


#define EXIF_MIXER_CORE_FIFO_SIZE_MAX 4
#define EXIF_MIXER_PORTS_NB 3

#define NO_OF_TAGS 7

// link list structure for
/*
struct buffer_list{
	buffer_list *nextBufferLink;
	buffer_list *prevBufferLink;
	OMX_BUFFERHEADERTYPE *pBuffer;
};
*/

enum processingCode {
	END_NO_PROCESSING,
	INVALID_HEADER,
	IFD1_TAG_MISSING,
	FULL_PROCESS_DONE,
	HALF_PROCESS_DONE,
	IFD1_SUBTAG_NOTFOUND,
	GPS_SUBTAG_NOTFOUND,
	PROCESSING_NOT_DONE,
	INCORRECT_THUMBNAIL_SIZE,
	FATAL_BITSTREAM_ERROR
};

// Exif SubIFD Tags
#define EXIF_OFFSET 0x8769
#define PixelXDim 0xA002
#define PixelYDim 0xA003
#define Date_Time_Dzt 0x9004

/* IFD1 tags */
#define ImageWidth     				0x0100
#define ImageLength   				0x0101
#define Compression     			0x0103
#define FIXED_CODE     				0x2A00
#define Orientation					0x0112
#define XResolution 				0x011A
#define YResolution 				0x011B
#define Resolution_Unit				0x0128
#define JPEGInterchangeFormat   	0x0201
#define JPEGInterchangeFormatLength 0x0202

/* GPS Tags */
#define GPS_TAG 0x8825
/* GPS sub tags */
#define GPSVersionID		0x0000
#define GPSLatitudeRef		0x0001
#define GPSLatitude	    	0x0002
#define GPSLongitudeRef		0x0003
#define GPSLongitude		0x0004
#define GPSAltitudeRef		0x0005
#define GPSAltitude	    	0x0006
#define GPSTimeStamp		0x0007
#define GPSSatellites		0x0008
#define GPSStatus	    	0x0009
#define GPSMeasureMode		0x000A
#define GPSDOP	        	0x000B
#define GPSSpeedRef	    	0x000C
#define GPSSpeed			0x000D
#define GPSTrackRef			0x000E
#define GPSTrack			0x000F
#define GPSImgDirectionRef	0x0010
#define GPSImgDirection		0x0011
#define GPSMapDatum			0x0012
#define GPSDestLatitudeRef	0x0013
#define GPSDestLatitude		0x0014
#define GPSDestLongitudeRef	0x0015
#define GPSDestLongitude	0x0016
#define GPSDestBearingRef	0x0017
#define GPSDestBearing		0x0018
#define GPSDestDistanceRef	0x0019
#define GPSDestDistance		0x001A
#define GPSProcessingMethod	0x001B
#define GPSAreaInformation	0x001C
#define GPSDateStamp		0x001D
#define GPSDifferential		0x001E

#define SOUTH_LATITUDE 0x53
#define NORTH_LATITUDE 0x4E

#define EAST_LONGITUDE 0x45
#define WEST_LONGITUDE 0x57

#define JPEG_COMPRESSION 0x00000006

#define APP1_START  0xFFE1
#define EXIF_HEADER 0x45786966
#define IFD0_OFFSET 0x08000000

#define INTEL_FORMAT 0x4949
#define MOTO_FORMAT 0x4D4D

#define ENTRY_SIZE 12 //12 byte is the size of each directory entry

#define NO_GPS_TAGS 12
#define SIZE_DATA_MAPUM 7
#define SIZE_DATE_STAMP 11
#define SIZE_GPS_PR_MTD 32

class exif_mixer_arm_nmf_mixer_core;

/*
typedef  struct Trace {
	t_uint16      mId1;
	TraceInfo_t * mTraceInfoPtr;
} TRACE_t;
*/


class EXIFParser: public TraceObject
{
	OMX_U32 currentPosition;
	OMX_U32 headerSize;
	OMX_U8 *exifHeaderBuffer;
	OMX_U32 app1StartPos;
	OMX_U32 ifd1_offset;
	OMX_BOOL consider_endianness;
	OMX_U32 exif_size_location;
	OMX_U8 start_of_exif_offset;
	OMX_U32 start_of_ifd0;
	// +ER 349081
	OMX_U16 ort_type;
	OMX_U32	ort_comps;
	OMX_U32 ort_data;
	// -ER 349081
/*
endianness_flag : OMX_FALSE means bytes are in BIG endian format in buffer
endianness_flag : OMX_TRUE means bytes are in LITTLE endian format in buffer
*/
	OMX_BOOL endianness_flag;


	public:
	EXIFParser();
	OMX_U32 valid_tag, valid_offset;
	OMX_U32 save_current;
    OMX_U32 val_take;
	OMX_BOOL is_ifd1_tag;
	OMX_BOOL isEXIFsegmentempty;
	OMX_U32 TotalDirEntries;
	OMX_U32 thumbnailPosition;
	OMX_BOOL headerCorrectionReq;
	OMX_U32 bytes_data;
	OMX_CONFIG_GPSLOCATIONTYPE gpslocation;
	OMX_CONFIG_GPSLOCATIONTYPE *globalPtr;
	OMX_U32 gps_processingMethod_len;
	OMX_U8 gps_processing_ascii[SIZE_GPS_PR_MTD];
	OMX_U8 date_stamp[SIZE_DATE_STAMP];
	OMX_U32 thumbnail_size;
	void initialize_data(OMX_BUFFERHEADERTYPE *pBuffer);
	OMX_ERRORTYPE isEXIFbufferEmpty();
	void correctHeaderSize(OMX_U32 thumbsizeOffset,OMX_BUFFERHEADERTYPE *pBuffer);
	OMX_ERRORTYPE validate_exif_header();
	OMX_ERRORTYPE locate_ifd1_section();
	OMX_U32 return_CurrentPosition();
	OMX_ERRORTYPE correctPixelDimension(OMX_U32 xDim, OMX_U32 yDim);
	OMX_ERRORTYPE setIFD1tagposition();
	OMX_ERRORTYPE createGPStagposition(OMX_BOOL create_gps_tag,OMX_U32 loc_gps_dir);

	OMX_ERRORTYPE locate_and_set_ifd1_tags(OMX_U32 *position);
	OMX_ERRORTYPE ReadByte(OMX_U8 *);
	OMX_ERRORTYPE ReadTwoByte(OMX_U16 *pHalfWord);
    OMX_ERRORTYPE GetNextEXIFStartCode(OMX_U16 *aStartCodeIndex);
    OMX_U32 bytes_per_component(OMX_U16 tempSize);
	void skip_current_directory();
	OMX_ERRORTYPE validate_thumbnail_size();
	void write_gps_value(OMX_U32 GPSTagValue);
	void write_in_bitstream(OMX_U32 position,OMX_U32 value_to_write);
	void write_byte_in_bitstream(OMX_U32 position,OMX_U8 value_to_write);
	void write_entry_in_bitstream(OMX_U32 position,OMX_U32 value_to_write);
    OMX_U32 check_odd_even(OMX_U32 *offsetPosition);
    OMX_ERRORTYPE locate_gps_tag(OMX_U32 *);
    OMX_ERRORTYPE read_directory_entry(OMX_U32 *tempValue,OMX_U32 *tempBufferPosition, OMX_BOOL *isOffset);
	OMX_ERRORTYPE find_ifd1tag(OMX_BOOL isGPSReq);
	OMX_ERRORTYPE calculate_DataSize(OMX_U32 *DataSize,OMX_U32 isGPSReq);
public:
	//Component *comp_ptr;
	//exif_mixer_arm_nmf_mixer_core *comp_ptr;
	
};


class exif_mixer_arm_nmf_mixer_core: public Component, public exif_mixer_arm_nmf_mixer_coreTemplate
{
    public:
//		TRACE_t traceObject;
    	OMX_CONFIG_GPSLOCATIONTYPE globalgpslocation;
		OMX_U32 xDim,yDim;
        exif_mixer_arm_nmf_mixer_core();
        virtual ~exif_mixer_arm_nmf_mixer_core();
        virtual void start();
        //Constructor interface from component.type
	#ifdef NEWFSMINIT
		virtual void fsmInit(fsmInit_t initFSM);
	#else
		virtual void fsmInit(t_uint16 portsDisabled,t_uint16 portsTunneled);
	#endif
        virtual void setTunnelStatus(t_sint16 portIdx, t_uint16 isTunneled);

        // virtual void functions from Component.h
        virtual void reset();
        virtual void disablePortIndication(t_uint32 portIdx);
        virtual void enablePortIndication(t_uint32 portIdx);
        virtual void flushPortIndication(t_uint32 portIdx);
        virtual void process();
        virtual t_nmf_error construct(void);
        void    ResolveDependencies();
        // fsm.component.component.type interface sendcommand method sendCommand
        virtual void sendCommand(OMX_COMMANDTYPE cmd, t_uword param);
        // fsm.component.component.type interface posteven method processEvent
        virtual void processEvent(void);
        virtual void emptyThisBuffer(OMX_BUFFERHEADERTYPE_p buffer, t_uint8 portIndex);
        virtual void fillThisBuffer(OMX_BUFFERHEADERTYPE_p buffer);
        virtual void setConfigure(t_uint32 index, void* );
		virtual void setDimension(t_uint32 width, t_uint32 height);
        virtual void getConfigure(t_uint32 index, void* );
        virtual void setParam(t_uint32 portIndex, t_uint32 fifoSize, t_uint32 direction, t_uint32 buffSupplierType);
        virtual void finish(void);
        virtual void eventProcessOutput(void);
        OMX_BOOL buffer_available_atinput(OMX_U32 PortIndex);
        OMX_BOOL buffer_available_atoutput();
        void returnbuffer_Port(OMX_BUFFERHEADERTYPE **pBuffer,OMX_U32 portIndex);
        virtual void eventProcess(void);
        void processActual();
        void process_exif_image();
        void exif_mixer_armnmf_assert(OMX_U32 condition, OMX_U32 errorType, OMX_U32 line, OMX_BOOL isFatal);

        OMX_OTHER_EXTRADATATYPE extradata;

    private:
        EXIFParser exifparser;
        OMX_BOOL processActualExecution;
    	OMX_BUFFERHEADERTYPE * pBuffer1;
		OMX_BUFFERHEADERTYPE * pBuffer2;
		OMX_BUFFERHEADERTYPE * pBuffer3;
        Port      mPorts[EXIF_MIXER_PORTS_NB];
        OMX_BOOL  isBufferDequeued[EXIF_MIXER_PORTS_NB];
        //p_buffer_list InputList1,InputList2,OutputList; //start nodes of link list
        OMX_BOOL inputDependencyResolved, outputDependencyResolved;
        processingCode execCode;
        OMX_BOOL isThumbnailEnabled;
        OMX_BOOL download_parameters;
};



#endif //_EXIF_MIXER_CORE_H_
