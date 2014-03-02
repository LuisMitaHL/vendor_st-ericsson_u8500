/*****************************************************************************
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
*****************************************************************************/
#ifndef _IMG_ENS_INDEX_H_
#define _IMG_ENS_INDEX_H_

/* Each OMX header must include all required header files to allow the
 *  header to compile without errors.  The includes below are required
 *  for this header file to compile successfully
 */
#include <omxil/OMX_Index.h>

#if 1
// need symbol eEnsIndex_ConfigTraceSetting
enum ImgEns_CsaIndexType
{
	eCsaIndex_StartExtensions  = OMX_IndexVendorStartUnused,

	eCsaIndex_AudioStartUnused = eCsaIndex_StartExtensions + 0x1000,
	eCsaIndex_VideoStartUnused = eCsaIndex_StartExtensions + 0x2000,
	eCsaIndex_ImageStartUnused = eCsaIndex_StartExtensions + 0x3000,

	/* Chipset Supplier specific area */
	eCsaIndex_ChipsetSupplierStartUnused = eCsaIndex_StartExtensions + 0x4000
};

/// @defgroup ens_index_class ENS Extended Indexes
/// @ingroup ens_classes
/// @{
/** ENS eXtended Indexes */
enum ImgEns_IndexType
{
	eEnsIndex_StartExtensions = eCsaIndex_ChipsetSupplierStartUnused,     /**< Base of ENS Index Extensions */
	eEnsIndex_ConfigNMFComponentList,     /** returns an ENS_List_p containing a list of t_cm_instance_handle instantiated by this ENS component */
	eEnsIndex_ConfigTraceSetting,             /**< reference: ImgEns_CONFIG_TRACESETTINGTYPE */

	eEnsIndex_DReStartUnused = (eEnsIndex_StartExtensions + 0x1000),  /// Start of index range for use in the DRe
	eEnsIndex_AFMStartUnused = (eEnsIndex_StartExtensions + 0x2000),  /// Start of index range for use in the Audio Framework Manager
	eEnsIndex_VFMStartUnused = (eEnsIndex_StartExtensions + 0x3000),  /// Start of index range for use in the Video Framework Manager
	eEnsIndex_IFMStartUnused = (eEnsIndex_StartExtensions + 0x4000),  /// Start of index range for use in the Imaging Framework Manager
	eEnsIndex_VSSStartUnused = (eEnsIndex_StartExtensions + 0x5000),  /// Start of index range for use in the Val_Source_Sink
};

#define PV_OMX_COMPONENT_CAPABILITY_TYPE_INDEX 0xFF7A347

//The OMX_GetParameter call expects the following structure to be filled for this index:
typedef struct
{
	////////////////// OMX COMPONENT CAPABILITY RELATED MEMBERS
	OMX_BOOL iIsOMXComponentMultiThreaded;
	OMX_BOOL iOMXComponentSupportsExternalOutputBufferAlloc;
	OMX_BOOL iOMXComponentSupportsExternalInputBufferAlloc;
	OMX_BOOL iOMXComponentSupportsMovableInputBuffers;
	OMX_BOOL iOMXComponentSupportsPartialFrames;
	OMX_BOOL iOMXComponentUsesNALStartCode;
	OMX_BOOL iOMXComponentCanHandleIncompleteFrames;
	OMX_BOOL iOMXComponentUsesFullAVCFrames;
} PV_OMXComponentCapabilityFlagsType;
#endif

/// @}

//Structure associated to eEnsIndex_ConfigTraceSetting
typedef struct
{
	OMX_U32         nSize;
	OMX_VERSIONTYPE nVersion;
	OMX_U16         nTraceEnable; // tracegroup bitfields to enable/disable traces
	OMX_U32         nParentHandle; // OMX component ID
} ImgEns_ConfigTraceSettingType;

#endif // _IMG_ENS_INDEX_H_

