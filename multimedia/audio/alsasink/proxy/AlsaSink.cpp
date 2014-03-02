/*****************************************************************************/
/**
*  © ST-Ericsson, 2010 - All rights reserved
*
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   alsasink proxy
* \author  ST-Ericsson
*/
/*****************************************************************************/
#include "AlsaSink.hpp"
#include "AlsaSinkNmfHost.hpp"
#include "OMX_Types.h"
#include "audio_chipset_api_index.h"
#include "audio_sourcesink_chipset_api.h"
#include "alsasink_debug.h"

OMX_ERRORTYPE alsasinkFactoryMethod(ENS_Component_p * ppENSComponent)
{
	OMX_ERRORTYPE error = OMX_ErrorNone;
	*ppENSComponent = 0;

	LOG_I("Enter.\n");

	AlsaSink *alsaSink = new AlsaSink();
	if (alsaSink == 0)
		return OMX_ErrorInsufficientResources;

	error = alsaSink->construct();

	if (error != OMX_ErrorNone) {
		delete alsaSink;
		return error;
	}

	AlsaSinkNmfHost *alsaSinkNmfHost = new AlsaSinkNmfHost(*alsaSink);
	if (alsaSinkNmfHost == 0) {
		delete alsaSink;
		return OMX_ErrorInsufficientResources;
	}

	alsaSink->setProcessingComponent(alsaSinkNmfHost);

	*ppENSComponent = alsaSink;
	return error;
}

OMX_ERRORTYPE AlsaSink::construct()
{
	OMX_ERRORTYPE error;
	OMX_AUDIO_PARAM_PCMMODETYPE settingsIn_default;
	OMX_AUDIO_PARAM_PCMMODETYPE settingsFb_default;
	AFM_PcmPort* portIn,*portFb;
	int bufferSizeMin;

	alsasink_dbg_set_logs_from_properties();

	LOG_I("Enter.\n");

	settingsIn_default.nPortIndex		= 0;
	settingsIn_default.eNumData		= OMX_NumericalDataSigned;
	settingsIn_default.eEndian		= OMX_EndianLittle;
	settingsIn_default.bInterleaved		= OMX_TRUE;
	settingsIn_default.ePCMMode		= OMX_AUDIO_PCMModeLinear;
	settingsIn_default.nBitPerSample	= 16;
	settingsIn_default.nChannels		= 2;
	settingsIn_default.nSamplingRate	= 48000;
	settingsIn_default.eChannelMapping[0]	= OMX_AUDIO_ChannelLF;
	settingsIn_default.eChannelMapping[1]	= OMX_AUDIO_ChannelRF;

	settingsFb_default.nPortIndex		= 1;
	settingsFb_default.eNumData		= OMX_NumericalDataSigned;
	settingsFb_default.eEndian		= OMX_EndianLittle;
	settingsFb_default.bInterleaved		= OMX_TRUE;
	settingsFb_default.ePCMMode		= OMX_AUDIO_PCMModeLinear;
	settingsFb_default.nBitPerSample	= 16;
	settingsFb_default.nChannels		= 2;
	settingsFb_default.nSamplingRate	= 48000;
	settingsFb_default.eChannelMapping[0]	= OMX_AUDIO_ChannelLF;
	settingsFb_default.eChannelMapping[1]	= OMX_AUDIO_ChannelRF;

	error = AFM_Component::construct(2, 1, true);  // The sink has two ports and deployed as arm component
	if (error != OMX_ErrorNone)
		return error;

	bufferSizeMin = DEFAULT_PCM_BLOCKSIZE * 1 * settingsFb_default.nBitPerSample / 8; // One channel minimum

	// Create PCM-port
	error = createPcmPort(0,
			OMX_DirInput,
			bufferSizeMin,
			settingsIn_default);
	if (error != OMX_ErrorNone) return error;
	error = createPcmPort(1,
			OMX_DirOutput,
			bufferSizeMin,
			settingsFb_default);
	if (error != OMX_ErrorNone) return error;
	LOG_I("\tMinimum buffer-size (OMX): %u [bytes]", bufferSizeMin);

	// Set communication ...
	portIn = static_cast<AFM_PcmPort *>(getPort(0));
	portFb = static_cast<AFM_PcmPort *>(getPort(1));

	portIn->setSupportedProprietaryComm(AFM_NO_PROPRIETARY_COMMUNICATION);
	portIn->forceStandardTunneling();
	portFb->setSupportedProprietaryComm(AFM_NO_PROPRIETARY_COMMUNICATION);
	portFb->forceStandardTunneling();

	// Set role
	ENS_String<20> role = "audio.alsasink";
	role.put((OMX_STRING)mRoles[0]);
	setActiveRole(mRoles[0]);

	mCodingType = OMX_AUDIO_ALSASINK_CT_REFER;
#ifdef MMPROBE_ENABLED
	mPcmProbeEnabled[0] = OMX_FALSE;
	mPcmProbeEnabled[1] = OMX_FALSE;
#endif // MMPROBE_ENABLED

	return OMX_ErrorNone;
}

OMX_ERRORTYPE AlsaSink::getParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure) const
{
	LOG_I("Enter.\n");

	switch ((int)nParamIndex) {
		case AFM_IndexParamPcmLayout:
		{
			LOG_I("\tOMX_INDEXTYPE: AFM_IndexParamPcmLayout (%d)\n", nParamIndex);

			AFM_PARAM_PCMLAYOUTTYPE* pcmlayout = (AFM_PARAM_PCMLAYOUTTYPE*)pComponentParameterStructure;
			if (pcmlayout->nPortIndex > 1) {
				LOG_E("\tError: pcmlayout->nPortIndex > 1!\n");

				return OMX_ErrorBadPortIndex;
			}

			OMX_AUDIO_PARAM_PCMMODETYPE &pcmSettings = ((AFM_PcmPort*)getPort(pcmlayout->nPortIndex))->getPcmSettings();
			pcmlayout->nBlockSize      = DEFAULT_PCM_BLOCKSIZE;
			pcmlayout->nChannels       = pcmSettings.nChannels;
			pcmlayout->nMaxChannels    = 8;
			pcmlayout->nBitsPerSample  = 16;
			pcmlayout->nNbBuffers      = 2;

			LOG_I("\tpcmlayout[%d]:\n", (int)pcmlayout->nPortIndex);
			LOG_I("\t\tnBlockSize = %d\n", (int)pcmlayout->nBlockSize);
			LOG_I("\t\tnChannels = %d\n", (int)pcmlayout->nChannels);
			LOG_I("\t\tnMaxChannels = %d\n", (int)pcmlayout->nMaxChannels);
			LOG_I("\t\tnBitsPerSample = %d\n", (int) pcmlayout->nBitsPerSample);
			LOG_I("\t\tnNbBuffers = %d\n", (int)pcmlayout->nNbBuffers);
			LOG_I("\t\teEndianness: %s-Endian\n", (pcmSettings.eEndian == OMX_EndianLittle) ? "Little" : "Big");
			LOG_I("\t\teInterleaved: %s\n", (pcmSettings.bInterleaved) ? "Yes" : "No");
			LOG_I("\t\tbNumericalData: %s\n", (pcmSettings.eNumData == OMX_NumericalDataSigned) ? "Signed" : "Unsigned");
			LOG_I("\t\tnSamplingRate = %d\n", (int)pcmSettings.nSamplingRate);

			if (pcmlayout->nChannels > pcmlayout->nMaxChannels) {
				LOG_E("\tError: Unsupported number of channels (%d)!\n", (int)pcmlayout->nChannels);

				return OMX_ErrorBadParameter;
			}

			return OMX_ErrorNone;
		}
		default:
		{
			LOG_I("\tOMX_INDEXTYPE: %d not known to Alsasink.)\n", (int)nParamIndex);

			return AFM_Component::getParameter(nParamIndex, pComponentParameterStructure);
		}
	}
}

OMX_ERRORTYPE AlsaSink::setParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure)
{
	LOG_I("Enter.\n");

	if (pComponentParameterStructure == 0) {
		return OMX_ErrorBadParameter;
	}

	switch ((int)nParamIndex) {
		case OMX_IndexParamAlsasink:
		{
			LOG_I("\tOMX_INDEXTYPE: OMX_IndexParamAlsasink (%d)\n", (int)nParamIndex);
			OMX_AUDIO_PARAM_ALSASINK* pAudioParamAlsasink = (OMX_AUDIO_PARAM_ALSASINK*)pComponentParameterStructure;
			mCodingType = pAudioParamAlsasink->coding_type;
			LOG_I("\tmCodingType = %d", (int)mCodingType);

			return OMX_ErrorNone;
		}
		default:
		{
			LOG_I("\tOMX_INDEXTYPE: %d not known to Alsasink.)\n", (int)nParamIndex);

			return AFM_Component::setParameter(nParamIndex, pComponentParameterStructure);
		}
	}
}

OMX_ERRORTYPE AlsaSink::setConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure)
{
	LOG_I("Enter.\n");

	switch ((int)nConfigIndex) {
		case OMX_IndexConfigALSADevice:
		{
			LOG_I("\tOMX_INDEXTYPE: OMX_IndexConfigALSADevice (%d)\n", (int)nConfigIndex);
			OMX_AUDIO_CONFIG_ALSADEVICETYPE *pAlsaConfig = (OMX_AUDIO_CONFIG_ALSADEVICETYPE*)pComponentConfigStructure;
			strncpy((char*)mALSADeviceName, (char*)pAlsaConfig->cDeviceName, 32);
			LOG_I("\tmALSADeviceName = '%.31s'\n", (char*)mALSADeviceName);

			return OMX_ErrorNone;
		}
#ifdef MMPROBE_ENABLED
		case AFM_IndexConfigPcmProbe:
		{
			LOG_I("\tOMX_INDEXTYPE: AFM_IndexConfigPcmProbe (%d)\n", (int)nConfigIndex);
			AFM_AUDIO_CONFIG_PCM_PROBE *pPcmProbeCfg = (AFM_AUDIO_CONFIG_PCM_PROBE*)pComponentConfigStructure;
			if (pPcmProbeCfg->nPortIndex > 1)
				return OMX_ErrorBadPortIndex;

			mPcmProbeId[pPcmProbeCfg->nPortIndex] = pPcmProbeCfg->nProbeIdx;
			mPcmProbeEnabled[pPcmProbeCfg->nPortIndex] = pPcmProbeCfg->bEnable;
			LOG_I("\tmPcmProbe[%d]: id = %d (%s)\n", (int)pPcmProbeCfg->nPortIndex,
				(int)pPcmProbeCfg->nProbeIdx, (pPcmProbeCfg->bEnable) ? "enabled" : "disabled");

			return OMX_ErrorNone;
		}
#endif // MMPROBE_ENABLED
		default:
		{
			LOG_I("\tOMX_INDEXTYPE: %d not known to AlsaSink.\n", (int)nConfigIndex);

			return AFM_Component::setConfig(nConfigIndex, pComponentConfigStructure);
		}
	}
}

OMX_ERRORTYPE AlsaSink::getConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure) const
{
	LOG_I("Enter.\n");

	switch ((int)nConfigIndex) {
		case OMX_IndexConfigALSADevice:
		{
			LOG_I("\tOMX_INDEXTYPE: OMX_IndexConfigALSADevice (%d)\n", (int)nConfigIndex);
			OMX_AUDIO_CONFIG_ALSADEVICETYPE* pAlsaConfig = (OMX_AUDIO_CONFIG_ALSADEVICETYPE*)pComponentConfigStructure;
			strncpy((char*) pAlsaConfig->cDeviceName, (char*) mALSADeviceName, 32);
			LOG_I("\tpAlsaConfig->cDeviceName = '%.31s'\n", (char*) pAlsaConfig->cDeviceName);

			return OMX_ErrorNone;
		}
#ifdef MMPROBE_ENABLED
		case AFM_IndexConfigPcmProbe:
		{
			LOG_I("\tOMX_INDEXTYPE: AFM_IndexConfigPcmProbe (%d)\n", (int)nConfigIndex);
			AFM_AUDIO_CONFIG_PCM_PROBE *pPcmProbeCfg = (AFM_AUDIO_CONFIG_PCM_PROBE*)pComponentConfigStructure;
			if (pPcmProbeCfg->nPortIndex > 1)
				return OMX_ErrorBadPortIndex;

			pPcmProbeCfg->nProbeIdx = mPcmProbeId[pPcmProbeCfg->nPortIndex];
			pPcmProbeCfg->bEnable = mPcmProbeEnabled[pPcmProbeCfg->nPortIndex];
			LOG_I("\tmPcmProbe[%d]: id = %d (%s)\n", (int)pPcmProbeCfg->nPortIndex,
				(int)pPcmProbeCfg->nProbeIdx, (pPcmProbeCfg->bEnable) ? "enabled" : "disabled");

			return OMX_ErrorNone;
		}
#endif // MMPROBE_ENABLED
		default:
		{
			LOG_I("\tOMX_INDEXTYPE: %d not known to AlsaSink.\n", (int)nConfigIndex);

			return AFM_Component::getConfig(nConfigIndex, pComponentConfigStructure);
		}
	}
}

void AlsaSink::getDeviceName(OMX_U8 *ALSADeviceName, int maxLength)
{
	LOG_I("Enter.\n");

	strncpy((char*)ALSADeviceName, (char*)mALSADeviceName, MIN(maxLength, (int)sizeof(mALSADeviceName)));
}

OMX_AUDIO_ALSASINK_CT AlsaSink::getCodingType(void)
{
	LOG_I("Enter.\n");

	return mCodingType;
}

#ifdef MMPROBE_ENABLED
OMX_U32 AlsaSink::getPcmProbeId(OMX_U32 portIdx)
{
	LOG_I("Enter.\n");

	if (portIdx > 1)
		return OMX_ErrorBadPortIndex;

	return mPcmProbeId[portIdx];
}

OMX_BOOL AlsaSink::isPcmProbeEnabled(OMX_U32 portIdx)
{
	LOG_I("Enter.\n");

	if (portIdx > 1)
		return OMX_FALSE;

	return mPcmProbeEnabled[portIdx];
}
#endif // MMPROBE_ENABLED
