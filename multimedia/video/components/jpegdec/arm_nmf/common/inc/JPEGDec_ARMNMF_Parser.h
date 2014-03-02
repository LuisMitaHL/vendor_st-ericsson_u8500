/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef _JPEGDEC_ARMNMF_PARSER_H_
#define _JPEGDEC_ARMNMF_PARSER_H_

/*------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------*/

#include <string.h>
#include "OMX_Core.h"
#include "JPEGDec_Parser.h"
#include "JPEGDec_ARMNMF_ParamAndConfig.h"
#include "ENS_List.h"

#define MAX_BITSTREAM_BUFFER_LINKS 16


/*------------------------------------------------------------------------
 * Defines
 *----------------------------------------------------------------------*/
#define NB_OF_QUANT_TABLES      4

/** \brief maximum number of ac/dc huffman tables
 *  \note 3 would be sufficient but encoders could
 *        count tables from 1 to 3 instead of 0 to 2
 */
#define NB_OF_HUFF_TABLES    2

/** \brief maximum number of huffman tables 3x (AC + DC)  */
#define NB_OF_COMPONENT_HUFF_TABLES      3

/** \brief number of coefficient in quant table   */
#define QUANT_TABLE_SIZE        64

#define HUFF_BITS_TABLE_SIZE 16
#define AC_HUFFVAL_TABLE_SIZE 256
#define DC_HUFFVAL_TABLE_SIZE 12
#define DC_HUFF_TABLE_SIZE  12
#define AC_HUFF_TABLE_SIZE  256
#define HUFF_LOOKAHEAD	8	/* # of bits of lookahead */
/** \brief max huff code size (+1)   */
#define MAX_HUFF_CODE_SIZE        17

/** \brief number of component in a scan    */
#define MAX_SCAN_COMPONENT      3

/** \brief number of component in a frame    */
#define MAX_FRAME_COMPONENT 3


/** @{ \name Markers values CCITT_Rec_T81 p32*/
#define SOF0    0xffc0U  /**< \brief Baseline DCT   */
#define SOF1    0xffc1U  /**< \brief Extended sequential DCT   */
#define SOF2    0xffc2U  /**< \brief Progressive DCT   */
#define SOF3    0xffc3U  /**< \brief Lossless   */
#define SOF5    0xffc5U  /**< \brief differential sequential DCT   */
#define SOF6    0xffc6U  /**< \brief differential progressive DCT */
#define SOF7    0xffc7U  /**< \brief differential lossless   */
#define JPG     0xffc8U  /**< \brief reserved   */
#define SOF9    0xffc9U  /**< \brief extended sequential DCT   */
#define SOF10   0xffcaU  /**< \brief progressive DCT   */
#define SOF11   0xffcbU  /**< \brief Lossless  */
#define SOF13   0xffcdU  /**< \brief differential sequential DCT  */
#define SOF14   0xffceU  /**< \brief differential progressive DCT  */
#define SOF15   0xffcfU  /**< \brief differential lossless  */
#define DHT     0xffc4U  /**< \brief Define Huffman tables  */
#define DAC     0xffccU  /**< \brief Define arithmetic coding conditioning  */
#define SOI     0xffd8U  /**< \brief Start of image   */
#define EOI     0xffd9U  /**< \brief End  of image   */
#define SOS     0xffdaU  /**< \brief Start of Scan   */
#define DQT     0xffdbU  /**< \brief Define quantization tables   */
#define DNL     0xffdcU  /**< \brief Define Number of lines   */
#define DRI     0xffddU  /**< \brief Define restart interval   */
#define DHP     0xffdeU  /**< \brief Define hierarchical progression   */
#define EXP     0xffdfU  /**< \brief Expand reference components   */
#define APP0    0xffe0U  /**< \brief Application segments   */
#define APP1    0xffe1U  /**< \brief Application segments   */
#define APP2    0xffe2U  /**< \brief Application segments   */
#define APP3    0xffe3U  /**< \brief Application segments   */
#define APP4    0xffe4U  /**< \brief Application segments   */
#define APP5    0xffe5U  /**< \brief Application segments   */
#define APP6    0xffe6U  /**< \brief Application segments   */
#define APP7    0xffe7U  /**< \brief Application segments   */
#define APP8    0xffe8U  /**< \brief Application segments   */
#define APP9    0xffe9U  /**< \brief Application segments   */
#define APP10   0xffeaU  /**< \brief Application segments   */
#define APP11   0xffebU  /**< \brief Application segments   */
#define APP12   0xffecU  /**< \brief Application segments   */
#define APP13   0xffedU  /**< \brief Application segments   */
#define APP14   0xffeeU  /**< \brief Application segments   */
#define APP15   0xffefU  /**< \brief Application segments   */
#define JPG0   0xfff0U  /**< \brief Jpeg extension*/
#define JPG1   0xfff1U  /**< \brief Jpeg extension*/
#define JPG2   0xfff2U  /**< \brief Jpeg extension*/
#define JPG3   0xfff3U  /**< \brief Jpeg extension*/
#define JPG4   0xfff4U  /**< \brief Jpeg extension*/
#define JPG5   0xfff5U  /**< \brief Jpeg extension*/
#define JPG6   0xfff6U  /**< \brief Jpeg extension*/
#define JPG7   0xfff7U  /**< \brief Jpeg extension*/
#define JPG8   0xfff8U  /**< \brief Jpeg extension*/
#define JPG9   0xfff9U  /**< \brief Jpeg extension*/
#define JPG10  0xfffaU  /**< \brief Jpeg extension*/
#define JPG11  0xfffbU  /**< \brief Jpeg extension*/
#define JPG12  0xfffcU /**< \brief Jpeg extension*/
#define JPG13  0xfffdU  /**< \brief Jpeg extension*/
#define COM    0xfffeU  /**< \brief Comment   */


#define ZZ_TO_NATURAL(zz_index) jpegNaturalOrder[(zz_index)]

const OMX_S16 jpegNaturalOrder[64] = {
  0,  1,  8, 16,  9,  2,  3, 10,
 17, 24, 32, 25, 18, 11,  4,  5,
 12, 19, 26, 33, 40, 48, 41, 34,
 27, 20, 13,  6,  7, 14, 21, 28,
 35, 42, 49, 56, 57, 50, 43, 36,
 29, 22, 15, 23, 30, 37, 44, 51,
 58, 59, 52, 45, 38, 31, 39, 46,
 53, 60, 61, 54, 47, 55, 62, 63
};

/** @}*/


/* /\** @{ \name  bitstream parser functions error code   *\/ */

/* #define  BTPAR_OK               (0x0U << 8) /\**< \brief all is ok *\/ */
/* #define  BTPAR_UNSUPPORTED_MODE (0x1U <<8) /\**< \brief unsupported mode *\/ */
/* #define  BTPAR_END_OF_IMAGE     (0x2U <<8) */
/* #define  BTPAR_INVALID_PQ       (0x4U <<8) /\**< \brief 16 bit precision not supported *\/ */
/* #define  BTPAR_NOT_JFIF         (0x8U <<8) /\**< \brief JFIF not compliant *\/ */

/* /\** @} end of bitstrean error codes *\/ */


/*------------------------------------------------------------------------
 * Types
 *----------------------------------------------------------------------*/

/** \brief enumeration of frame types
 *
 */

/** \brief Decoder error codes */
typedef enum
{
    /** \brief all is ok!   */
    DJPEG_NO_ERROR=0,

    /** \brief end of bitstream data reached in current bitstream buffer:
     * Handled by the host main: -> refresh of the bitstream buffer or exit
     */
    DJPEG_END_OF_DATA,

    /** \brief error opening bitstream file for reading
     * Handled by the host main: -> exit
     */
    DJPEG_FILE_IN_ERROR,

    /** \brief error result file for writing
     * Handled by the host main: -> exit
     */
    DJPEG_FILE_OUT_ERROR,

    /** \brief no sufficient memory to store bitstream
     *  Handled by the host main: -> exit
     */
    DJPEG_MALLOC_ERROR,

    /** \brief end of file reached
     * handled by the host main: -> exit
     */
    DJPEG_FILE_END,

    /** \brief unsupported mode
     * Handled by the host main: -> exit
     */
    DJPEG_UNSUPPORTED_MODE,

    /** \brief end of image reached:
     * Handled by the host main: -> normal exit
     */
    DJPEG_END_OF_IMAGE,

   /** \brief 16 bit precision not supported
    * Handled by the host main: -> exit
    */
    DJPEG_INVALID_PQ,

    /** \brief JFIF not compliant
     * Handled by the host main: ->exit
     */
    DJPEG_NOT_JFIF,

    /** \brief invalid coefficient position in block
     * Handled by the host main: ->exit
     */

    DJPEG_BAD_HUFF_POS,

    /** \brief invalid huff dc code
     * Handled by the hamac main: -> return INVALID_BITSTREAM
     */
    DJPEG_BAD_HUFF_DC,

    /** \brief invalid huff ac code
     * Handled by the hamac main: -> return INVALID_BITSTREAM
     */
    DJPEG_BAD_HUFF_AC,

    /** \brief unexpected restart marker
     * Handled by the hamac main: -> return INVALID_BITSTREAM
     */
    DJPEG_BAD_RESTART_VAL,

    /** \brief indexed Huff or quant table not relevant
     * Handled by the hamac main: -> return INVALID_BITSTREAM
     */
    DJPEG_BAD_TABLE_INDEX,

    /** \brief the markers read are not sufficient to decompress the image (missing tables)
     * Handled by the host main: ->exit
     */
    DJPEG_BAD_MARKER,

    /** \brief bad  command line arguments
     * Handled by the host main: ->exit
     */
    DJPEG_BAD_ARG ,

    /** \brief bad  command line arguments
     * Handled by the host main: ->exit
     */
    DJPEG_ERROR,

    DJPEG_END_OF_SCAN

} t_djpegErrorCode;

typedef enum
{
    UNKNOWN = 0,
    BASELINE,
    EXTENDED_SEQUENTIAL,
    EXTENDED_PROGRESSIVE
} t_btstType;


/** \brief struct holding scan component specification parameters
 *         CCITT_Rec_T81 B.2.3
*/
typedef struct jpgScanComponent
{
    OMX_U16 componentId;
    OMX_U16 dcTableId;
    OMX_U16 acTableId;
} ts_jpgScanComponent;

/** \brief struct holding scan  specification parameters
 *         CCITT_Rec_T81 B.2.3
*/

typedef struct jpgScan
{
    OMX_U16 numberOfComponents;
    OMX_U16 startSpectralSelection;
    OMX_U16 endSpectralSelection;
    OMX_U16 approxHigh;
    OMX_U16 approxLow;
    ts_jpgScanComponent scanComponent[MAX_SCAN_COMPONENT];
} ts_jpgScan, *tps_jpgScan;

/** \brief struct holding frame component specification parameters
 *         CCITT_Rec_T81 B.2.2
*/
typedef struct jpgFrameComponent
{
    OMX_U16 h;
    OMX_U16 v;
    OMX_U16 tq;
    OMX_U16 id;
} ts_jpgFrameComponent, *tps_jpgFrameComponent;

/** \brief struct holding frame  specification parameters
 *         CCITT_Rec_T81 B.2.2
*/
typedef struct jpgFrame
{
    t_btstType type;
    OMX_U16 samplePrecision;
    OMX_U16 numberOfLines;
    OMX_U16 numberSamplePerLine;
    OMX_U16 numberOfComponents;
    ts_jpgFrameComponent frameComponent[MAX_FRAME_COMPONENT];
} ts_jpgFrame, *tps_jpgFrame;

/** \brief struct holding dc huffman table */

/** \brief struct holding ac huffman table */
#ifdef __JPEGDEC_SOFTWARE
typedef struct jpgACHuffTable
{
    OMX_U16 bits[HUFF_BITS_TABLE_SIZE];
    OMX_U16 val[AC_HUFFVAL_TABLE_SIZE];
    OMX_S32      maxcode[18];		/* largest code of length k (-1 if none) */
	OMX_S32      valoffset[17];		/* huffval[] offset for codes of length k */	
	OMX_S16      look_nbits[256]; /* # bits, or 0 if too long */
	OMX_U16      look_sym[256]; /* symbol, or unused */
} ts_jpgACHuffTable;
typedef struct jpgDCHuffTable
{
    OMX_U16 bits[HUFF_BITS_TABLE_SIZE];
    OMX_U16 val[DC_HUFFVAL_TABLE_SIZE];
    OMX_S32      maxcode[18];		/* largest code of length k (-1 if none) */
	OMX_S32      valoffset[17];		/* huffval[] offset for codes of length k */	
	OMX_S16      look_nbits[256]; /* # bits, or 0 if too long */
	OMX_U16      look_sym[256]; /* symbol, or unused */
} ts_jpgDCHuffTable;
#elif __JPEGDEC_DUAL
typedef struct jpgDCHuffTable
{
    OMX_U16 bits[HUFF_BITS_TABLE_SIZE];
    OMX_U16 val[DC_HUFFVAL_TABLE_SIZE];
    OMX_S32      maxcode[18];		/* largest code of length k (-1 if none) */
	OMX_S32      valoffset[17];		/* huffval[] offset for codes of length k */	
	OMX_S16      look_nbits[256]; /* # bits, or 0 if too long */
	OMX_U16      look_sym[256]; /* symbol, or unused */
////The hw_huff tables
    OMX_U16 code[DC_HUFF_TABLE_SIZE];
    OMX_U16 size[DC_HUFF_TABLE_SIZE];
} ts_jpgDCHuffTable;

/** \brief struct holding ac huffman table */
typedef struct jpgACHuffTable
{
/////The hw_huff tables
    OMX_U16 code[AC_HUFF_TABLE_SIZE];
    OMX_U16 size[AC_HUFF_TABLE_SIZE];
////The sw_huff tables
    OMX_S32      maxcode[18];		/* largest code of length k (-1 if none) */
	OMX_S32      valoffset[17];		/* huffval[] offset for codes of length k */	
	OMX_S16      look_nbits[256]; /* # bits, or 0 if too long */
	OMX_U16      look_sym[256]; /* symbol, or unused */
	OMX_U16 bits[HUFF_BITS_TABLE_SIZE];
    OMX_U16 val[AC_HUFFVAL_TABLE_SIZE];
	
} ts_jpgACHuffTable;
#else
typedef struct jpgACHuffTable
{
    OMX_U16 code[AC_HUFF_TABLE_SIZE];
    OMX_U16 size[AC_HUFF_TABLE_SIZE];
} ts_jpgACHuffTable;
typedef struct jpgDCHuffTable
{
    OMX_U16 code[DC_HUFF_TABLE_SIZE];
    OMX_U16 size[DC_HUFF_TABLE_SIZE];

} ts_jpgDCHuffTable;
#endif
typedef struct quantTable
{
    OMX_U16 q[QUANT_TABLE_SIZE]; /**< \brief quantization table element in natural order   */

} ts_quantTable ;

typedef ts_quantTable *tps_quantTable; /**< \brief pointer on quant table   */


/** \brief struct holding results of jpeg bitstream parsing */
typedef struct jpgInfo
{
    ts_quantTable quantTable[NB_OF_QUANT_TABLES]; /**< \brief quantization tables  */
    ts_jpgDCHuffTable dcHuffTable[NB_OF_HUFF_TABLES];
    ts_jpgACHuffTable acHuffTable[NB_OF_HUFF_TABLES];
    OMX_U16 restartInterval;
    ts_jpgFrame frame;
    ts_jpgScan scan;

} ts_jpgInfo, *tps_jpgInfo;

/** \brief struct holding results of jpeg bitstream parsing state */

typedef struct bitstreamProcessingState
{
    OMX_U32 lastMarker;
	OMX_S32 lastMarkerDataLength;
    OMX_S16 qpTableNumber;
    OMX_S16 huffTableNumber;
    OMX_S16 lastScan;
    OMX_S16 scanNumber;    /**< \brief number of the current scan (from 0 to ...)   */
	OMX_U8 	headerOrBtst;	//defualt b00=no info;b01=marker processing;b10=encoded data processing
} ts_bitstreamProcessingState;

typedef ts_bitstreamProcessingState *tps_bitstreamProcessingState;

typedef struct bitstreamBufferLink
{
    OMX_BUFFERHEADERTYPE    *omxBitstreamBuffer;          /**< \brief bitstream file */ //tarun
    OMX_U8 		*data;  	    /**< \brief buffer allocated to store bitstream data (bytes)   */
    OMX_U8 		*ptCurrentByte; /**< \brief pointer on current byte in bitstream data   */
    OMX_U32		indexCurrentBit;    /**< \brief index of current bit in bitstream data from 0 to size_in_bits -1    */
    OMX_U32    	sizeInBits;         /**< \brief bitstream size in bits   */
    OMX_BOOL    removeStuffedBytes;   /**< \brief set to true to remove 0x00 in 0xff00 sequences   */
    OMX_U16   	cntByteRemoved;        /**< \brief flag memorising how many bytes were removed during last show function */
    OMX_U8     	lastReadByte;         /**< \brief for stuffed bytes removal   */
	OMX_U8		btstBufferContentInfo; //b00=empty;b01=only markers;b10=only encoded data;b11=both header and markers
}ts_bitstreamBuffer;

typedef ts_bitstreamBuffer *tps_bitstreamBuffer; /**< \brief pointer on ts_bitstreamBuffer   */


class JPEGDEC_CLASS;
class JPEGDec_ARMNMF_ParamAndConfig_CLASS;


class JPEGDec_ARMNMF_Parser_CLASS

{
	private:

	    JPEGDEC_CLASS *pJpegNmfComp;
	    //Lists management related to payload buffer
	    ts_bitstreamBuffer bitstreamBuffer[MAX_BITSTREAM_BUFFER_LINKS];
	    ENS_List bitstreamBufferList;
	    ENS_List freeLinks;
	    ENS_ListItem_p firstItem;
	    ts_bitstreamBuffer *firstBtstBuffer;

	    OMX_BOOL isInitialized;	// For progressive mode
	    ts_jpgInfo  jpegInfo;
	    ts_bitstreamProcessingState processingState;
	    OMX_S32 totalNumberOfScans;
	    OMX_U32 sizeInBitsRead;
	    OMX_BOOL traverseEncodedData;
	    OMX_BOOL processMarker;
	    // To access the parameter datbase maintained in JPEGDec_ARMNMF_ParamAndConfig_CLASS
	    JPEGDec_ARMNMF_ParamAndConfig_CLASS *pParamAndConfig;

	public:

	    t_djpegErrorCode  btparAdvanceToNextMarker();
	    t_djpegErrorCode  btparProcessMarker(tps_jpgInfo pJpegInfo,tps_bitstreamProcessingState pProcessState);
	    void btparInit (tps_bitstreamProcessingState pProcessingState);
	    t_djpegErrorCode  btstAlignByte();
	    OMX_BOOL btstIsByteAligned();
	    t_djpegErrorCode btstShowbits(OMX_U16 n, OMX_U32 *pBits);
	    t_djpegErrorCode BTSTSHOWBITS(tps_bitstreamBuffer pBitstream,OMX_U16 n,OMX_U32 *pBits);
	    t_djpegErrorCode btstFlushbits(OMX_U16 n);
	    t_djpegErrorCode BTSTFLUSHBITS(tps_bitstreamBuffer pBitstream, OMX_U16 n);
	    t_djpegErrorCode  btstGetbits(OMX_U16 n, OMX_U32 *pBits);
	    OMX_U32  btstGetBitIndex();
	    t_djpegErrorCode readRestartIntervalDefinition(OMX_U16 *pRestart);
    	t_djpegErrorCode readDhtTable(OMX_U16 pBits[],OMX_U16 pVal[],OMX_U32 *pTc,OMX_U32 *pTh);
	    void  expandHuffmanTable(OMX_U16 pBits[], OMX_U16 pVal[],OMX_U16 pCode[],OMX_U16 pSize[]);
        void copy_hufftable(t_uint16 p_huffbits[], t_uint16 p_huffval[], t_uint16 p_bits[],t_uint16 p_val[], signed short is_dc);
	    t_djpegErrorCode readHuffmanTable(ts_jpgDCHuffTable dcTables[],ts_jpgACHuffTable acTables[]);
	    t_djpegErrorCode readQuantizationTable(tps_quantTable pQuantTable,OMX_U16 *pTq);
        void jpeg_make_d_derived_tbl_hdr (ts_jpgDCHuffTable *dc_tables,ts_jpgACHuffTable *ac_tables,signed short isDC);
    	t_djpegErrorCode skipMarkerSegment();
	    t_djpegErrorCode readFrameHeader (tps_jpgFrame p_frame);
	    t_djpegErrorCode readScanHeader (tps_jpgScan pScan);

	public:

	    JPEGDec_ARMNMF_Parser_CLASS();
    	~JPEGDec_ARMNMF_Parser_CLASS();

	    void initializeReferences(JPEGDEC_CLASS *pComp,JPEGDec_ARMNMF_ParamAndConfig_CLASS *pPC);
	    t_djpegErrorCode insertBtstBufferLinkBack(OMX_BUFFERHEADERTYPE *omxBuffer);
	    t_djpegErrorCode deleteBtstBufferLinkFront();
	    t_djpegErrorCode initializeBtstLink(OMX_BUFFERHEADERTYPE *omxBuffer,ts_bitstreamBuffer *pBtstBuffer);
	    t_djpegErrorCode updateBtstBufferList();
	    t_djpegErrorCode findLengthMarkerData(OMX_U16 *pBits);
	    t_djpegErrorCode isProcessingmarkerPossible(OMX_U16 dataLength);
	    t_djpegErrorCode sendHeader(OMX_BUFFERHEADERTYPE *pBuffer);
	    t_djpegErrorCode clearBtstBufferList();
	    ts_bitstreamBuffer* getCurrentBtstBufferLink();
	    t_djpegErrorCode findEncodedScanData();
	    t_djpegErrorCode findEOIMarker();
	    t_djpegErrorCode jpegParser();

	    void updateFrameHeader(JPEGDec_ARMNMF_ParamAndConfig_CLASS *jpegdecParamAndConfig);
	    void updateScanHeader(JPEGDec_ARMNMF_ParamAndConfig_CLASS *jpegdecParamAndConfig);
	    void updateHuffmanTables(JPEGDec_ARMNMF_ParamAndConfig_CLASS *jpegdecParamAndConfig);
	    void updateQuantizationTables(JPEGDec_ARMNMF_ParamAndConfig_CLASS *jpegdecParamAndConfig);
	    void setYUVId(JPEGDec_ARMNMF_ParamAndConfig_CLASS *jpegdecParamAndConfig);
	    void setRestartInterval(JPEGDec_ARMNMF_ParamAndConfig_CLASS *jpegdecParamAndConfig);
	    OMX_U32 LastMarker();
	    OMX_U32 GetScanNumber();
	    OMX_BOOL CheckParsingFinished();
	    OMX_S32 GetTotalNumberOfScans();
	    t_djpegErrorCode parserReset();
};

#endif // _JPEGDEC_ARMNMF_PARSER_H_








