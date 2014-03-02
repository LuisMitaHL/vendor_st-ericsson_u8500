/******************************************************************************
 Copyright (c) 2009-2011, ST-Ericsson SA
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:
 
   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials
      provided with the distribution.
   3. Neither the name of the ST-Ericsson SA nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior written
      permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

#ifndef __JPEGENC_PARAMANDCONFIG_H
#define __JPEGENC_PARAMANDCONFIG_H

#include "VFM_Component.h"
#include "VFM_ParamAndConfig.h"


/// @ingroup JPEGEncoder
/// @brief JPEG Encode Quantification Table, both Luma and Chroma one
///
/// Coefficients are ordered in zig-zag order. They are automatically computed based on
/// the rotation factor and the quality factor (cf. JPEGEnc_Proxy::compute_quantization_table()).
typedef struct
{
    /// @brief Quantization table for the Luma
    OMX_U8 QuantizationLuma[64];
    /// @brief Quantization table for the Chroma
    OMX_U8 QuantizationChroma[64];
} JPEGEnc_Proxy_QUANTIFICATIONTABLE;

// forward definition
class JPEGEnc_Proxy;


/// @ingroup JPEGEncoder
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

class JPEGEnc_ParamAndConfig: public VFM_ParamAndConfig, public TraceObject
{
    friend class JPEGEnc_Proxy;
   	friend class JPEGEnc_NmfMpc_ProcessingComponent;
    friend class JPEGEnc_ArmNmfProcessingComponent;
    friend class JPEGEnc_ProcessingComponent;
   	friend class JPEGEnc_Port;

    public:
        /// @brief Constructor, that inializes the default configuration
        /// and set the OpenMax Component the configuration is related to
        JPEGEnc_ParamAndConfig(VFM_Component *component);

        void setColorType(OMX_COLOR_FORMATTYPE value)
        {
			inputBufferFormat = value;
        }

        void setCompressionType(OMX_IMAGE_CODINGTYPE value)
        {
			outputCompressionFormat = value;
        }
		
		OMX_ERRORTYPE setIndexParamResourceSvaMcps(OMX_PTR pMcpsType);

        OMX_ERRORTYPE getIndexParamResourceSvaMcps(OMX_PTR pMcpsType) const;
    
        OMX_U32 resourceSvaMcps() const;

        void resourceSvaMcps(const OMX_U32& mcps) ;
    private:
        /// @brief Set a structure of type OMX_CONFIG_ROTATIONTYPE
        /// @param [in] pt_org structure of type OMX_CONFIG_ROTATIONTYPE, that contains the
        /// rotation to set
        /// @param [out] rotation the rotation that is set
        /// @param [out] has_changed true if the value of the rotation has been modified by
        /// this operation, false otherwise
        /// @return OMX_ErrorNone if no error occured, an error otherwise (OMX_ErrorBadParameter)
        OMX_ERRORTYPE setIndexConfigCommonRotate(OMX_PTR pt_org, OMX_S32 *rotation, OMX_BOOL *has_changed);

        /// @brief Get a structure of type OMX_CONFIG_ROTATIONTYPE
        /// @param [in,out] pt_org structure of type OMX_CONFIG_ROTATIONTYPE, that will contain the
        /// rotation to get
        /// @param [in] rotation the rotation to get
        /// @return OMX_ErrorNone if no error occured, an error otherwise
        OMX_ERRORTYPE getIndexConfigCommonRotate(OMX_PTR pt_org, OMX_S32 rotation) const;

        /// @brief Set a structure of type VFM_CONFIG_TARGETBPP
        /// @param [in] pt_org structure of type VFM_CONFIG_TARGETBPP, that contains the
        /// target-bit-per_pixel to set
        /// @param [out] targetBpp the target-bit-per_pixel that is set
        /// @param [out] has_changed true if the value of the rotation has been modified by
        /// this operation, false otherwise
        /// @return OMX_ErrorNone if no error occured, an error otherwise (OMX_ErrorBadParameter)
        OMX_ERRORTYPE setIndexConfigTargetbpp(OMX_PTR pt_org, OMX_U16 *targetBpp, OMX_BOOL *has_changed);

        /// @brief Get a structure of type VFM_CONFIG_TARGETBPP
        /// @param [in,out] pt_org structure of type VFM_CONFIG_TARGETBPP, that will contain the
        /// target-bit-per_pixel to get
        /// @param [in] targetBpp the target-bit-per_pixel to get
        /// @return OMX_ErrorNone if no error occured, an error otherwise
        OMX_ERRORTYPE getIndexConfigTargetbpp(OMX_PTR pt_org, OMX_U16 targetBpp) const;
        /// @brief Set a structure of type OMX_IMAGE_PARAM_PORTFORMATTYPE. It makes use
        /// of the mComponent attribute.
        /// @param [in] pt_org structure of type OMX_IMAGE_PARAM_PORTFORMATTYPE, that contains the
        /// rotation to set
        /// @param [out] has_changed true if the parameters have been modified by
        /// this operation, false otherwise
        /// @return OMX_ErrorNone if no error occured, an error otherwise (OMX_ErrorBadParameter)
        OMX_ERRORTYPE setIndexParamImagePortFormat(OMX_PTR pt_org, OMX_BOOL *has_changed);

        /// @brief Get a structure of type OMX_IMAGE_PARAM_PORTFORMATTYPE. It makes use
        /// of the mComponent attribute.
        /// @param [in,out] pt_org structure of type OMX_IMAGE_PARAM_PORTFORMATTYPE,
        /// that will contain the information to get
        /// @return OMX_ErrorNone if no error occured, an error otherwise
        OMX_ERRORTYPE getIndexParamImagePortFormat(OMX_PTR pt_org) const;

        /// @brief Get a structure of type OMX_PARAM_PORTDEFINITIONTYPE. It makes use
        /// of the mComponent attribute.
        /// @param [in,out] pt_org structure of type OMX_PARAM_PORTDEFINITIONTYPE,
        /// that will contain the information to get
        /// @return OMX_ErrorNone if no error occured, an error otherwise
        OMX_ERRORTYPE getIndexParamPortDefinition(OMX_PTR pt_org) const;

        /// @brief Get the restart interval value (used for sync point in the
        /// encoded stream)
        /// @return the restart interval
        /// @note the value is a default value computed using the size of the frame
        t_uint16 getRestartInterval() const;
        OMX_ERRORTYPE setIndexParamQuantizationTable(OMX_PTR pt_org, OMX_BOOL *has_changed);
		OMX_ERRORTYPE getIndexParamQuantizationTable(OMX_PTR pt_org) const;
		OMX_ERRORTYPE setIndexParamHuffmanTable(OMX_PTR pt_org, OMX_BOOL *has_changed);
		OMX_ERRORTYPE getIndexParamHuffmanTable(OMX_PTR pt_org) const;
		OMX_ERRORTYPE convertHuffmanTable(t_uint8 pBits[],t_uint8 pVal[], t_uint16 pCode[], t_uint16 pSize[]);
        OMX_ERRORTYPE reset();

    private:
    	/// @brief cropping parameter. all 0 if no cropping
    	OMX_CONFIG_RECTTYPE cropping_parameter;

        /// @brief Input buffer format, modified / retrieved using the indexes OMX_IndexParamImagePortFormat
        /// and OMX_IndexParamPortDefinition. Supported types in the JPEG Encoder are
        /// OMX_COLOR_FormatMonochrome and OMX_COLOR_FormatSTYUV420PackedSemiPlanarMB
        OMX_COLOR_FORMATTYPE inputBufferFormat;
        OMX_IMAGE_CODINGTYPE outputCompressionFormat;


        /// @brief On-the-fly rotation, modified / retrieved by OMX_IndexConfigCommonRotate
        /// (cf. JPEGEnc_Proxy_CONFIG_ROTATION)
        VFM_ONFLY_ROTATION rotation;
		OMX_S32 rotation_val;
        /// @brief Quality factor of the compression (1-100), modified / retrieved
        /// using the index OMX_IndexParamQFactor (cf. OpenMax Specifications. Note that it can be
        /// called dynamically using a SetConfig()).
        /// Setting it resets isOptimizeQuantTableEnable
        OMX_U16 QFactor;
		OMX_U32 nNumber_param;

        /// @brief Target bit-per-pixel to reach, modified / retrieved by VFM_IndexConfigTargetbpp.
        /// Setting it sets isOptimizeQuantTableEnable (cf. JPEGEnc_Proxy_CONFIG_TARGETBPP).
        /// The unit is 1/256 bpp (i.e. BPP=(number of bits per pixel)*256).
        OMX_U16 targetBpp;

        /// @brief whether we use the quality factor or the targetbpp. It is reseted using
        /// OMX_IndexParamQFactor, and set using VFM_IndexConfigTargetbpp
        OMX_BOOL isOptimizeQuantTableEnable;

        /// @brief Quantization table for the Luma and the Chroma
        JPEGEnc_Proxy_QUANTIFICATIONTABLE QuantTable;

        /// @brief Is quantification table are up-to-date or not?
        VFM_Status QuantToCompute;

        /// @brief Is header buffer up-to-date or not?
        VFM_Status HeaderToGenerate;

        /// @brief Is header buffer up-to-date or not?
        VFM_Status ConfigApplicable;
		OMX_U32   m_nSvaMcps;
	protected:

			t_uint8 _LumaQuantification[64];
			t_uint8 _ChromaQuantification[64];
			t_uint8 _LumaQuantificationRotate[64];
			t_uint8 _ChromaQuantificationRotate[64];

			OMX_BOOL SAME_AC_HUFFMAN_TABLE_PROVIDED;
			OMX_BOOL SAME_DC_HUFFMAN_TABLE_PROVIDED;
			t_uint16 ParamLumaHuffmanSizeAcTable[256];
			t_uint16 ParamChromaHuffmanCodeDcTable[12];
			t_uint16 ParamChromaHuffmanSizeDcTable[12];
			t_uint16 ParamChromaHuffmanCodeAcTable[256];
			t_uint16 ParamChromaHuffmanSizeAcTable[256];
			t_uint16 ParamLumaHuffmanCodeDcTable[12];
			t_uint16 ParamLumaHuffmanCodeAcTable[256];
			t_uint16 ParamLumaHuffmanSizeDcTable[12];
			OMX_U8 _HUFFVAL_DCChroma[256];
			OMX_U8 _BITS_DCChroma[16];
			OMX_U8 _HUFFVAL_ACChroma[256];
			OMX_U8 _BITS_ACChroma[16];
			OMX_U8 _HUFFVAL_DCLuma[256];
			OMX_U8 _BITS_DCLuma[16];
			OMX_U8 _HUFFVAL_ACLuma[256];
			OMX_U8 _BITS_ACLuma[16];
			OMX_U8 _BITS_DCChroma_shifted[256];
			OMX_U8 _BITS_DCLuma_shifted[256];
			OMX_U8 _BITS_ACChroma_shifted[256];
			OMX_U8 _BITS_ACLuma_shifted[256];

};

inline OMX_U32 JPEGEnc_ParamAndConfig::resourceSvaMcps() const 
{ return m_nSvaMcps; }

inline void JPEGEnc_ParamAndConfig::resourceSvaMcps(const OMX_U32& mcps)
{ m_nSvaMcps = mcps; }


#endif
