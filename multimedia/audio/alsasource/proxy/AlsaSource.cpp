/*****************************************************************************/
/**
*  © ST-Ericsson, 2010 - All rights reserved
*
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   alsasource proxy
* \author  ST-Ericsson
*/
/*****************************************************************************/
#include "AlsaSource.hpp"
#include "AlsaSourceNmfHost.hpp"
#include "OMX_Types.h"
#include "audio_chipset_api_index.h"
#include "audio_sourcesink_chipset_api.h"
#include "alsasource_debug.h"

OMX_ERRORTYPE alsasourceFactoryMethod(ENS_Component_p * ppENSComponent)
{
	OMX_ERRORTYPE error = OMX_ErrorNone;
	*ppENSComponent = 0;

	LOG_I("Enter.\n");

	AlsaSource *alsaSource = new AlsaSource();
	if (alsaSource == 0)
		return OMX_ErrorInsufficientResources;

	error = alsaSource->construct();

	if (error != OMX_ErrorNone) {
		delete alsaSource;
		return error;
	}

	AlsaSourceNmfHost *alsaSourceNmfHost = new AlsaSourceNmfHost(*alsaSource);
	if (alsaSourceNmfHost == 0) {
		delete alsaSource;
		return OMX_ErrorInsufficientResources;
	}

	alsaSource->setProcessingComponent(alsaSourceNmfHost);

	*ppENSComponent = alsaSource;
	return error;
}

OMX_ERRORTYPE AlsaSource::construct()
{
	OMX_ERRORTYPE error;
	OMX_AUDIO_PARAM_PCMMODETYPE defaultPcmSettingsOut;
	AFM_PcmPort* pcmPort_p;
	int bufferSizeMin;

	alsasource_dbg_set_logs_from_properties();
	LOG_I("Enter.\n");

	defaultPcmSettingsOut.nPortIndex     = 0;
	defaultPcmSettingsOut.eNumData       = OMX_NumericalDataSigned;
	defaultPcmSettingsOut.eEndian        = OMX_EndianLittle;
	defaultPcmSettingsOut.bInterleaved   = OMX_TRUE;
	defaultPcmSettingsOut.ePCMMode       = OMX_AUDIO_PCMModeLinear;
	defaultPcmSettingsOut.nBitPerSample  = 16;
	defaultPcmSettingsOut.nChannels      = 1;
	defaultPcmSettingsOut.nSamplingRate  = 48000;
	defaultPcmSettingsOut.eChannelMapping[0] = OMX_AUDIO_ChannelLF;
	defaultPcmSettingsOut.eChannelMapping[1] = OMX_AUDIO_ChannelRF;

	error = AFM_Component::construct(1, 1, true);  // The source has one port and deployed as arm component
	if (error != OMX_ErrorNone)
		return error;

	bufferSizeMin = DEFAULT_PCM_BLOCKSIZE * 1 * defaultPcmSettingsOut.nBitPerSample / 8; // One channel minimum

	// Create PCM-port
	error = createPcmPort(0,
			OMX_DirOutput,
			bufferSizeMin,
			defaultPcmSettingsOut);
	if (error != OMX_ErrorNone) return error;

	LOG_I("Minimum buffer-size (OMX): %u [bytes]", bufferSizeMin);

	// Set communication ...
	pcmPort_p = static_cast<AFM_PcmPort *>(getPort(0));
	pcmPort_p->setSupportedProprietaryComm(AFM_NO_PROPRIETARY_COMMUNICATION);
	pcmPort_p->forceStandardTunneling();
#ifdef MMPROBE_ENABLED
	mPcmProbeEnabled[0] = OMX_FALSE;
#endif // MMPROBE_ENABLED

	// Set role
	ENS_String<20> role = "audio.alsasource";
	role.put((OMX_STRING)mRoles[0]);
	setActiveRole(mRoles[0]);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE AlsaSource::getParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure) const
{
	LOG_I("Enter.\n");

	switch ((int)nParamIndex) {
		case AFM_IndexParamPcmLayout:
		{
			LOG_I("\tOMX_INDEXTYPE: AFM_IndexParamPcmLayout (%d)\n", nParamIndex);

			AFM_PARAM_PCMLAYOUTTYPE *pcmlayout = (AFM_PARAM_PCMLAYOUTTYPE *)pComponentParameterStructure;
			if (pcmlayout->nPortIndex > 0) {
				LOG_E("\tError: pcmlayout->nPortIndex > 0!\n");

				return OMX_ErrorBadPortIndex;
			}

			OMX_AUDIO_PARAM_PCMMODETYPE &pcmSettings = ((AFM_PcmPort *) getPort(pcmlayout->nPortIndex))->getPcmSettings();
			pcmlayout->nBlockSize      = getBlockSize();
			pcmlayout->nChannels       = pcmSettings.nChannels;
			pcmlayout->nMaxChannels    = 2;
			pcmlayout->nBitsPerSample  = getSampleBitSize();
			pcmlayout->nNbBuffers      = 1;

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
			LOG_I("\tOMX_INDEXTYPE: %d not known to Alsasource.)\n", (int)nParamIndex);

			return AFM_Component::getParameter(nParamIndex, pComponentParameterStructure);
		}
	}
}

OMX_ERRORTYPE AlsaSource::setConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure)
{
	LOG_I("Enter.\n");

	if (pComponentConfigStructure == 0) {
		return OMX_ErrorBadParameter;
	}

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
			if (pPcmProbeCfg->nPortIndex > 0)
				return OMX_ErrorBadPortIndex;

			mPcmProbeId[pPcmProbeCfg->nPortIndex] = pPcmProbeCfg->nProbeIdx;
			mPcmProbeEnabled[pPcmProbeCfg->nPortIndex] = pPcmProbeCfg->bEnable;
			LOG_I("\tPcmProbe[%d]: id = %d (%s)\n", (int)pPcmProbeCfg->nPortIndex,
				(int)pPcmProbeCfg->nProbeIdx, (pPcmProbeCfg->bEnable) ? "enabled" : "disabled");

			return OMX_ErrorNone;
		}
#endif // MMPROBE_ENABLED
		default:
		{
			LOG_I("\tOMX_INDEXTYPE %d not known to AlsaSource.\n", (int)nConfigIndex);

			return AFM_Component::setConfig(nConfigIndex, pComponentConfigStructure);
		}
	}
}

OMX_ERRORTYPE AlsaSource::getConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure) const
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
			if (pPcmProbeCfg->nPortIndex > 0)
				return OMX_ErrorBadPortIndex;

			pPcmProbeCfg->nProbeIdx = mPcmProbeId[pPcmProbeCfg->nPortIndex];
			pPcmProbeCfg->bEnable = mPcmProbeEnabled[pPcmProbeCfg->nPortIndex];
			LOG_I("\tPcmProbe[%d]: id = %d (%s)\n", (int)pPcmProbeCfg->nPortIndex,
				(int)pPcmProbeCfg->nProbeIdx, (pPcmProbeCfg->bEnable) ? "enabled" : "disabled");

			return OMX_ErrorNone;
		}
#endif // MMPROBE_ENABLED
		default:
		{
			LOG_I("\tOMX_INDEXTYPE %d not known to AlsaSource.\n", (int)nConfigIndex);

			return AFM_Component::getConfig(nConfigIndex, pComponentConfigStructure);
		}
	}
}

void AlsaSource::getDeviceName(OMX_U8 *ALSADeviceName, int maxLength)
{
	LOG_I("Enter.\n");

	strncpy((char*)ALSADeviceName, (char*)mALSADeviceName, MIN(maxLength, (int)sizeof(mALSADeviceName)));
}

#ifdef MMPROBE_ENABLED
OMX_U32 AlsaSource::getPcmProbeId(OMX_U32 portIdx)
{
	LOG_I("Enter.\n");

	if (portIdx > 0)
		return OMX_ErrorBadPortIndex;

	return mPcmProbeId[portIdx];
}

OMX_BOOL AlsaSource::isPcmProbeEnabled(OMX_U32 portIdx)
{
	LOG_I("Enter.\n");

	if (portIdx > 0)
		return OMX_FALSE;

	return mPcmProbeEnabled[portIdx];
}
#endif // MMPROBE_ENABLED
