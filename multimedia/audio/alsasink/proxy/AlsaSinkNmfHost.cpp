/*****************************************************************************/
/**
*  © ST-Ericsson, 2010 - All rights reserved
*
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   alsasink NMF Host class
* \author  ST-Ericsson
*/
/*****************************************************************************/
#include "AlsaSinkNmfHost.hpp"
#include "AlsaSink.hpp"
#include "alsasink/nmfil/host/composite/wrapper.hpp"
#include "AFM_PcmProcessing.h"
#include "AFM_Utils.h"
#include "AFM_PcmPort.h"
#include "audio_chipset_api_index.h"
#include "audio_sourcesink_chipset_api.h"
#include "alsasink_debug.h"

AlsaSinkNmfHost::AlsaSinkNmfHost(ENS_Component &enscomp)
  : AFMNmfHost_ProcessingComp(enscomp)
{
	LOG_I("Enter.\n");
}

AlsaSinkNmfHost::~AlsaSinkNmfHost()
{
	LOG_I("Enter.\n");
}

OMX_ERRORTYPE AlsaSinkNmfHost::instantiateMain(void)
{
	t_nmf_error nmf_error;

	LOG_I("Enter.\n");

	mNmfMain = alsasink_nmfil_host_composite_wrapperCreate();
	if (mNmfMain == NULL) {
		LOG_E("alsasink_nmfil_host_composite_wrapperCreate failed!\n");
		return OMX_ErrorInsufficientResources;
	}
	if (mNmfMain->construct() != NMF_OK) {
		LOG_E("mNmfMain->construct()  failed!\n");
		return OMX_ErrorInsufficientResources;
	}

	nmf_error = mNmfMain->bindFromUser("configure_alsasink", FIFO_SIZE, &mIAlgoConfig);
	if (nmf_error != NMF_OK) {
		LOG_E("mNmfMain->bindFromUser failed!\n");
		return OMX_ErrorInsufficientResources;
	}

	nmf_error = mNmfMain->getInterface("configure_alsasink", &mIAlgoSyncConfig);
	if (nmf_error != NMF_OK) {
		LOG_E("mNmfMain->getInterface failed!\n");
		return OMX_ErrorInsufficientResources;
	}

	LOG_I("\tBind from user 'configure'...\n");
	nmf_error = mNmfMain->bindFromUser("configure", FIFO_SIZE, &mIMainConfig);
	if (nmf_error != NMF_OK) {
		LOG_E("\tError: mNmfMain->bindFromUser returned %d!\n", nmf_error);
		return OMX_ErrorInsufficientResources;
	}

	LOG_I("Bind the NMF technology ...\n");
	nmf_error = mNmfMain->bindFromUser("process", FIFO_SIZE, &mIProcess);
	if (nmf_error != NMF_OK) {
		LOG_E("Error: bindFromUser returned %d!\n", nmf_error);
		return OMX_ErrorInsufficientResources;
	}

	mIAlgoConfig.setProcessCB(&mIProcess);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE AlsaSinkNmfHost::deInstantiateMain(void)
{
	t_nmf_error nmf_error;

	LOG_I("Enter.\n");

	nmf_error = mNmfMain->unbindFromUser("configure");
	if (nmf_error != NMF_OK) {
		LOG_E("\tError: unbindFromUser returned %d!\n", nmf_error);
		return OMX_ErrorInsufficientResources;
	}

	nmf_error = mNmfMain->unbindFromUser("configure_alsasink");
	if (nmf_error != NMF_OK)
		return OMX_ErrorInsufficientResources;

	nmf_error = mNmfMain->unbindFromUser("process");
	if (nmf_error != NMF_OK)
		return OMX_ErrorInsufficientResources;

	if (mNmfMain->destroy() != NMF_OK)
		return OMX_ErrorInsufficientResources;

	alsasink_nmfil_host_composite_wrapperDestroy((alsasink_nmfil_host_composite_wrapper*&)mNmfMain);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE AlsaSinkNmfHost::startMain(void)
{
	LOG_I("Enter.\n");

	mNmfMain->start();
	return OMX_ErrorNone;
}

OMX_ERRORTYPE AlsaSinkNmfHost::stopMain(void)
{
	LOG_I("Enter.\n");

	mNmfMain->stop();
	return OMX_ErrorNone;
}

OMX_ERRORTYPE AlsaSinkNmfHost::configureMain(void)
{
	LOG_I("Enter.\n");

	AlsaSink* proxy = static_cast<AlsaSink*>(&mENSComponent);

	char deviceName[32];
	proxy->getDeviceName((OMX_U8*)deviceName, 32); 
	LOG_I("\tDevice-name: %s\n", (char*)deviceName);
	mIAlgoConfig.setDeviceName(deviceName);

	int codingType = proxy->getCodingType();
	LOG_I("\tCoding-type: %d\n", codingType);
	mIAlgoConfig.setCodingType((t_uint32)codingType);

	AFM_PcmPort* inPort = (AFM_PcmPort*) proxy->getPort(PORT_IDX_INPUT);
	OMX_AUDIO_PARAM_PCMMODETYPE pcmSettings = inPort->getPcmSettings();

	// Set PCM config...
	alsasink_wrapper_config_t pcmConfig;
	memset(&pcmConfig, 0, sizeof(alsasink_wrapper_config_t));
	pcmConfig.sample_rate = pcmSettings.nSamplingRate;
	pcmConfig.nof_channels = pcmSettings.nChannels;
	pcmConfig.nof_bits_per_sample = pcmSettings.nBitPerSample;
	pcmConfig.block_size = DEFAULT_PCM_BLOCKSIZE;
	pcmConfig.endian_little = (pcmSettings.eEndian == OMX_EndianLittle);
	pcmConfig.interleaved = pcmSettings.bInterleaved;
	pcmConfig.numerical_data_signed = (pcmSettings.eNumData == OMX_NumericalDataSigned);

#ifdef MMPROBE_ENABLED
	pcmConfig.pcmprobe_cfg[PORT_IDX_INPUT].id = proxy->getPcmProbeId(PORT_IDX_INPUT);
	pcmConfig.pcmprobe_cfg[PORT_IDX_INPUT].enabled = proxy->isPcmProbeEnabled(PORT_IDX_INPUT);
	pcmConfig.pcmprobe_cfg[PORT_IDX_FEEDBACK].id = proxy->getPcmProbeId(PORT_IDX_FEEDBACK);
	pcmConfig.pcmprobe_cfg[PORT_IDX_FEEDBACK].enabled = proxy->isPcmProbeEnabled(PORT_IDX_FEEDBACK);
#endif // MMPROBE_ENABLED

	mIMainConfig.setParameter(pcmConfig);
	LOG_I("Exit.\n");
	return OMX_ErrorNone;
}

OMX_ERRORTYPE AlsaSinkNmfHost::applyConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure)
{
	LOG_I("Enter.\n");

	switch ((int)nConfigIndex) {
		case OMX_IndexConfigALSADevice:
		{
			LOG_I("\tOMX_INDEXTYPE: OMX_IndexConfigALSADevice (%d)\n", (int)nConfigIndex);

			OMX_AUDIO_CONFIG_ALSADEVICETYPE* pAlsaDev = (OMX_AUDIO_CONFIG_ALSADEVICETYPE*)pComponentConfigStructure;

			char deviceName[OMX_MAX_STRINGNAME_SIZE];
			strncpy(deviceName, (char*)pAlsaDev->cDeviceName, OMX_MAX_STRINGNAME_SIZE);
			LOG_I("\tdeviceName = '%.31s'\n", (char*)deviceName);
			mIAlgoConfig.setDeviceName(deviceName);

			return OMX_ErrorNone;
		}
#ifdef MMPROBE_ENABLED
		case AFM_IndexConfigPcmProbe:
		{
			LOG_I("\tOMX_INDEXTYPE: AFM_IndexConfigPcmProbe (%d)\n", (int)nConfigIndex);

			return configureMain();
		}
#endif // MMPROBE_ENABLED
		default:
		{
			LOG_I("\tOMX_INDEXTYPE %d not known to AlsaSinkNmfHost.\n", (int)nConfigIndex);
			return AFMNmfHost_ProcessingComp::applyConfig(nConfigIndex, pComponentConfigStructure);
		}
	}
}

OMX_ERRORTYPE AlsaSinkNmfHost::retrieveConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure)
{
	LOG_I("Enter.\n");

	switch ((int)nConfigIndex) {
		case OMX_IndexConfigAudioLatency:
		{
			LOG_I("\tOMX_INDEXTYPE: OMX_IndexConfigAudioLatency (%d)\n", (int)nConfigIndex);

			CHECK_STRUCT_SIZE_AND_VERSION(pComponentConfigStructure,OMX_AUDIO_CONFIG_LATENCYTYPE);
			OMX_AUDIO_CONFIG_LATENCYTYPE *pLatencyCfg = static_cast<OMX_AUDIO_CONFIG_LATENCYTYPE*>(pComponentConfigStructure);

			AlsasinkLatency_t latency;
			mIAlgoSyncConfig.getLatency(&latency);
			if (latency.succeeded) {
				pLatencyCfg->nLatency = latency.latencyMs;
				return OMX_ErrorNone;
			} else {
				LOG_E("\tError: failed to retrieve delay!\n");
				return OMX_ErrorUndefined;
			}
		}
#ifdef MMPROBE_ENABLED
		case AFM_IndexConfigPcmProbe:
		{
			LOG_I("\tOMX_INDEXTYPE: AFM_IndexConfigPcmProbe (%d)\n", (int)nConfigIndex);

			AFM_AUDIO_CONFIG_PCM_PROBE* pPcmProbeCfg = (AFM_AUDIO_CONFIG_PCM_PROBE*)pComponentConfigStructure;
			AlsaSink* proxy = static_cast<AlsaSink*>(&mENSComponent);

			if (pPcmProbeCfg->nPortIndex > PORT_IDX_FEEDBACK)
				return OMX_ErrorBadPortIndex;

			pPcmProbeCfg->nProbeIdx = proxy->getPcmProbeId(pPcmProbeCfg->nPortIndex);
			pPcmProbeCfg->bEnable = proxy->isPcmProbeEnabled(pPcmProbeCfg->nPortIndex);

			return OMX_ErrorNone;
		}
#endif // MMPROBE_ENABLED
		default:
		{
			LOG_I("\tOMX_INDEXTYPE %d not known to AlsaSinkNmfHost.\n", (int)nConfigIndex);
			return AFMNmfHost_ProcessingComp::retrieveConfig(nConfigIndex, pComponentConfigStructure);
		}
	}
}
