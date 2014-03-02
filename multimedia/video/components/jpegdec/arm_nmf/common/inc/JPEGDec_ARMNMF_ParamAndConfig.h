/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __JPEGDec_ARMNMF_ParamAndConfig_H
#define __JPEGDec_ARMNMF_ParamAndConfig_H

#include "OMX_Core.h"
//#include "OMX_IVCommon.h"
#include "JPEGDec_Parser.h"

#define MAX_NUMBER_COMPONENTS 3
#define MAX_NUMBER_HUFFMAN_TABLES 2 // Only Baseline process is supported
#define NUMBER_QUANT_TABLES 4

/// @ingroup JPEGDecoder
/// @brief JPEG Decoder Quantization Table, boh Luma and Chroma.
///
/// Coefficients are ordered in zig-zag order. These are extracted from the .jpg file
/// after parsing.
typedef struct{
    /// @brief Quantization table for the Luma
    OMX_U16 QuantizationTable[64];
} JPEGDec_Proxy_QUANTIZATIONTABLE;

/// @ingroup JPEGDecoder
/// @brief JPEG Decoder Mode
/// Extracted from .jpg file. Specifies image emcoding as sequential or progressive.
/// Tarun Caution: For 8500 Progressive mode is not yet confirmed. Added for Flexibiltty in later time.
/// Thus not implemented.
typedef enum {
	SEQUENTIAL_JPEG,
	PROGRESSIVE_JPEG
} JPEGDec_Proxy_JPEGMODE;

/// @ingroup JPEGDecoder
/// @brief JPEG Decoder Expanded Huffman Tables
#ifdef __JPEGDEC_SOFTWARE
typedef struct {
	/// @brief table for number of huffman codes of different lengths for the Luma
	OMX_U16 DCHuffmanBits[16];
	/// @brief table of Huffman codes for Luma
	OMX_U16 DCHuffmanVal[12];
    OMX_S32      MaxCode[18];		/* largest code of length k (-1 if none) */
	OMX_S32      ValOffset[17];		/* huffval[] offset for codes of length k */	
	OMX_S16      Look_Nbits[256]; /* # bits, or 0 if too long */
	OMX_U16      Look_Sym[256];
} JPEGDec_Proxy_DC_HUFFMANTABLE;
typedef struct {
	/// @brief table for number of huffman codes of different lengths for the Luma
	OMX_U16 ACHuffmanBits[16];
	/// @brief table of Huffman codes for Luma
	OMX_U16 ACHuffmanVal[256];
    OMX_S32      MaxCode[18];		/* largest code of length k (-1 if none) */
	OMX_S32      ValOffset[17];		/* huffval[] offset for codes of length k */	
	OMX_S16      Look_Nbits[256]; /* # bits, or 0 if too long */
	OMX_U16      Look_Sym[256];
} JPEGDec_Proxy_AC_HUFFMANTABLE;
#elif __JPEGDEC_DUAL
typedef struct {
	/// @brief table for number of huffman codes of different lengths for the Luma
	OMX_U16 DCHuffmanBits[16];
	/// @brief table of Huffman codes for Luma
	OMX_U16 DCHuffmanVal[12];
    /// @brief table for number of huffman codes of different lengths for the Luma
	OMX_U16 DCHuffmanCode[12];
	/// @brief table of Huffman codes for Luma
	OMX_U16 DCHuffmanSize[12];
    ////new members
    OMX_S32      MaxCode[18];		/* largest code of length k (-1 if none) */
	OMX_S32      ValOffset[17];		/* huffval[] offset for codes of length k */	
	OMX_S16      Look_Nbits[256]; /* # bits, or 0 if too long */
	OMX_U16      Look_Sym[256];

} JPEGDec_Proxy_DC_HUFFMANTABLE;
typedef struct {
	/// @brief table for number of huffman codes of different lengths for the Luma
	OMX_U16 ACHuffmanBits[16];
	/// @brief table of Huffman codes for Luma
	OMX_U16 ACHuffmanVal[256];
/// @brief table for number of huffman codes of different lengths for the Luma
	OMX_U16 ACHuffmanCode[256];
	/// @brief table of Huffman codes for Luma
	OMX_U16 ACHuffmanSize[256];
    /////new members
    OMX_S32      MaxCode[18];		/* largest code of length k (-1 if none) */
	OMX_S32      ValOffset[17];		/* huffval[] offset for codes of length k */	
	OMX_S16      Look_Nbits[256]; /* # bits, or 0 if too long */
	OMX_U16      Look_Sym[256];
} JPEGDec_Proxy_AC_HUFFMANTABLE;
#else
typedef struct {
    /// @brief table for number of huffman codes of different lengths for the Luma
	OMX_U16 DCHuffmanCode[12];
	/// @brief table of Huffman codes for Luma
	OMX_U16 DCHuffmanSize[12];

} JPEGDec_Proxy_DC_HUFFMANTABLE;
typedef struct {
	/// @brief table for number of huffman codes of different lengths for the Luma
	OMX_U16 ACHuffmanCode[256];
	/// @brief table of Huffman codes for Luma
	OMX_U16 ACHuffmanSize[256];

} JPEGDec_Proxy_AC_HUFFMANTABLE;
#endif


/*! Enumeration for Downsampling Factors
*/
typedef enum {
    /*! Downsampling factor = 1
	*/
	DOWNSAMPLING_FACTOR_1 = 1,
    /*! Downsampling factor = 2
	*/
    DOWNSAMPLING_FACTOR_2 = 2,
    /*! Downsampling factor = 4
	*/
    DOWNSAMPLING_FACTOR_4 = 4,
    /*! Downsampling factor = 8
	*/
    DOWNSAMPLING_FACTOR_8 = 8
}JPEGDec_Proxy_Downsampling_Factor;


/*! Descriptor for Still Decoder Sampling factors
*/
typedef struct {
	/*! Sampling of  component in Horizontal Direction.\n
		Constraints : Possible values are 1,2 and 4. \n
    */
	OMX_U8 hSamplingFactor;

	/*! Sampling of component in Vertical Direction.\n
		Constraints : Possible values are 1,2 and 4. \n
    */
    OMX_U8 vSamplingFactor;

}JPEGDec_Proxy_Sampling_Factor;

typedef struct{
	OMX_U8 componentId;
	OMX_U8 DCHuffmanTableDestinationSelector;
	OMX_U8 ACHuffmanTableDestinationSelector;
	OMX_U8 quantizationTableDestinationSelector;
	OMX_U8 componentSelector;
}JPEGDec_Parser_Component_Attributes;

/*! Descriptor for Scan Parsed
*/
typedef struct {
	OMX_U8 *startAddress;
	OMX_U32 bitstreamOffset;
	OMX_U32 encodedDataSize;
}JPEGDec_Proxy_Scan_Encoded_Data;

/// @ingroup JPEGDecoder
/// @brief This class contains the parameters and configuration of the JPEG Encode OpenMax Component
/// It is also used to set and get the configuration, without processing.
///
/// It is derived from VFMEnc_ParamAndConfig.
/// @note The size of the frame (input and output) is stored inside the VFM_Port associated
/// with the JPEG Encoder Proxy (that can be accessed through the attribute mComponent).
/// Port IPB+0 contains the size of the original frame, and port IPB+1 contains the
/// size of the cropped encoded frame, and can be retrived using
/// mComponent->getFrameHeight(VPB+0), mComponent->getFrameWidth(VPB+0),
/// mComponent->getFrameHeight(VPB+1) and mComponent->getFrameWidth(VPB+1)

class JPEGDec_ARMNMF_ParamAndConfig_CLASS
{
	friend class JPEGDEC_CLASS;
	friend class JPEGDec_ARMNMF_Parser_CLASS;

public:
	/// @brief Constructor, that inializes the default configuration
	/// and set the OpenMax Component the configuration is related to
	JPEGDec_ARMNMF_ParamAndConfig_CLASS();
	OMX_ERRORTYPE reset();
	void initializeParamAndConfig();

protected:

	/// @brief height of the frame
	OMX_U16 frameHeight;

	/// @brief width of the frame
	OMX_U16 frameWidth;

	/// @brief Number of Components present in the Frame or Image.
	/// Maximum of three components are supported by SVA
	OMX_U16 nbComponents;

	/// @brief Describes the sampling format used in the image(420,422,444 etc.)
	JPEGDec_Proxy_Sampling_Factor samplingFactors[MAX_NUMBER_COMPONENTS];

	/// @brief Downscaling required of decoded image size. Only 1,2,4,8 downampling factors are supported.
	/// By defualt 1 is selected.
	JPEGDec_Proxy_Downsampling_Factor downsamplingFactor;

	/// @brief Restart intervals used in encoded data.
	OMX_U16 restartInterval;

	/// @brief Describes what type of JPEG mode out of baseline, extended etc. and inturn sequential or progressive
	/// is selected for encoding image.
	/// Note: Only Baseline is supported in SVA8500 which means only sequntial is supported.
	JPEGDec_Proxy_JPEGMODE mode;

	/// @brief Number of components present in current scan
	OMX_U16 nbScanComponents;

	/// @brief Which of the Components Y,Cb,Cr are present in the currrent scan.
	JPEGDec_Parser_Component_Attributes components[MAX_NUMBER_COMPONENTS];

	/// @brief JPEG uses id for components present. It is upto the parser to decide id for Y,Cb,Cr.
	/// For example in JFIF format Y=1,Cb=2,Cr=3.
	OMX_U16 IdY;
	OMX_U16 IdCb;
	OMX_U16 IdCr;

	/// @brief Used in progressive mode
	OMX_U16 startSpectralSelection;
	OMX_U16 endSpectralSelection;
	OMX_U16 successiveApproxPosition;

	/// @brief Quantization table.
	JPEGDec_Proxy_QUANTIZATIONTABLE quantizationTables[NUMBER_QUANT_TABLES];

	/// @brief Huffman tables usaed for entropy coding.
	JPEGDec_Proxy_DC_HUFFMANTABLE DCHuffmanTable[MAX_NUMBER_HUFFMAN_TABLES];

	/// @brief Huffman tables usaed for entropy coding.
	JPEGDec_Proxy_AC_HUFFMANTABLE ACHuffmanTable[MAX_NUMBER_HUFFMAN_TABLES];

	/// @brief Height of cropped frame. Equals frame height when no cropping is required
	OMX_U32 cropWindowHeight;

	/// @brief Width of cropped frame. Equals frame Width when no cropping is required
	OMX_U32 cropWindowWidth;

	/// @brief horizontal offset when cropping. 0 if no cropping
	OMX_U32 horizontal_offset;

	/// @brief vertical offset
	/// @todo before or after the rotation ?
	OMX_U32 vertical_offset;

	/// @brief Input buffer format, modified / retrieved using the indexes OMX_IndexParamImagePortFormat
	/// and OMX_IndexParamPortDefinition. Supported types in the JPEG Encoder are
	/// OMX_COLOR_FormatSTGrayScale and OMX_COLOR_FormatSTYUV420PackedSemiPlanarMB

	OMX_U8 isCroppingEnabled;
	OMX_U8 isDownScalingEnabled;
	OMX_U8 isFrameHeaderUpdated;
	OMX_U8 isScanHeaderUpdated;
	OMX_U8 isHuffmanTableUpdated;
	OMX_U8 isQuantizationTableUpdated;
	OMX_U8 isRestartIntervalUpdated;

	JPEGDec_Proxy_Scan_Encoded_Data scan;

	OMX_U8 updateData;
	OMX_BOOL completeImageProcessed;
};


#endif
