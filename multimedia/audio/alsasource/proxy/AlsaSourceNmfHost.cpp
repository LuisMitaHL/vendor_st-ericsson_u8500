/*****************************************************************************/
/**
*  © ST-Ericsson, 2010 - All rights reserved
*
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   alsasource NMF Host class
* \author  ST-Ericsson
*/
/*****************************************************************************/
#include "AlsaSourceNmfHost.hpp"
#include "AlsaSource.hpp"
#include "alsasource/nmfil/host/composite/wrapper.hpp"
#include "alsasource/nmfil/host/composite/alsasource.hpp"
#include "AFM_PcmProcessing.h"
#include "AFM_Utils.h"
#include "AFM_PcmPort.h"
#include "audio_chipset_api_index.h"
#include "audio_sourcesink_chipset_api.h"
#include "alsasource_debug.h"

AlsaSourceNmfHost::AlsaSourceNmfHost(ENS_Component &enscomp)
  : AFMNmfHost_ProcessingComp(enscomp)
{
	LOG_I("Enter.\n");

	mNmfil = NULL;
}

AlsaSourceNmfHost::~AlsaSourceNmfHost()
{
	LOG_I("Enter.\n");
}

OMX_ERRORTYPE AlsaSourceNmfHost::instantiateMain(void)
{
	t_nmf_error nmf_error;

	LOG_I("Enter.\n");

	mNmfil = alsasource_nmfil_host_composite_alsasourceCreate();
	if (mNmfil == NULL) {
		LOG_E("\talsasource_nmfil_host_composite_alsasourceCreate failed!\n");
		return OMX_ErrorInsufficientResources;
	}

	if (mNmfil->construct() != NMF_OK) {
		LOG_E("\tmNmfil->construct failed!\n");
		return OMX_ErrorInsufficientResources;
	}

	nmf_error = mNmfil->bindFromUser("configure", FIFO_SIZE, &mIAlgoConfig);
	if (nmf_error != NMF_OK) {
		LOG_E("\tError: unable to bind il configure! [%d]\n", nmf_error);
		return OMX_ErrorInsufficientResources;
	}

	mNmfMain = alsasource_nmfil_host_composite_wrapperCreate();
	if (mNmfMain == NULL) {
		LOG_E("\tError: alsasource_nmfil_host_composite_wrapperCreate failed!\n");
		return OMX_ErrorInsufficientResources;
	}

	if (mNmfMain->construct() != NMF_OK) {
		LOG_E("\tError: mNmfMain->construct failed!\n");
		return OMX_ErrorInsufficientResources;
	}

	nmf_error = mNmfMain->bindFromUser("configure", FIFO_SIZE, &mIMainConfig);
	if (nmf_error != NMF_OK) {
		LOG_E("\tError: unable to bind main configure! [%d]\n", nmf_error);
		return OMX_ErrorInsufficientResources;
	}

	nmf_error = mNmfMain->bindFromUser("process", FIFO_SIZE, &mIProcess);
	if (nmf_error != NMF_OK) {
		LOG_E("\tError: unable to bind main process! [%d]\n", nmf_error);
		return OMX_ErrorInsufficientResources;
	}

	mIAlgoConfig.setProcessCB(&mIProcess);

	nmf_error = mNmfMain->bindComponent("source", mNmfil, "source");
	if (nmf_error != NMF_OK) {
		LOG_E("\tError: unable to bind main source! [%d]\n", nmf_error);
		return OMX_ErrorInsufficientResources;
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE AlsaSourceNmfHost::deInstantiateMain(void)
{
	t_nmf_error nmf_error;

	LOG_I("Enter.\n");

	nmf_error = mNmfMain->unbindComponent("source", mNmfil, "source");
	if (nmf_error != NMF_OK) {
		LOG_E("\tError: unable to unbind main source! [%d]\n", nmf_error);
		return OMX_ErrorInsufficientResources;
	}

	nmf_error = mNmfMain->unbindFromUser("configure");
	if (nmf_error != NMF_OK) {
		LOG_E("\tError: unable to unbind main configure! [%d]\n", nmf_error);
		return OMX_ErrorInsufficientResources;
	}

	nmf_error = mNmfil->unbindFromUser("configure");
	if (nmf_error != NMF_OK) {
		LOG_E("\tError: unable to unbind il configure! [%d]\n", nmf_error);
		return OMX_ErrorInsufficientResources;
	}

	if (mNmfil->destroy() != NMF_OK) {
		LOG_E("\tError: mNmfil->destroy failed!\n");
		return OMX_ErrorInsufficientResources;
	}

	nmf_error = mNmfMain->unbindFromUser("process");
	if (nmf_error != NMF_OK) {
		LOG_E("\tError: unable to unbind main process! [%d]\n", nmf_error);
		return OMX_ErrorInsufficientResources;
	}

	alsasource_nmfil_host_composite_alsasourceDestroy((alsasource_nmfil_host_composite_alsasource*&)mNmfil);

	if (mNmfMain->destroy() != NMF_OK) {
		LOG_E("\tError: mNmfMain->destroy failed!\n");
		return OMX_ErrorInsufficientResources;
	}

	alsasource_nmfil_host_composite_wrapperDestroy((alsasource_nmfil_host_composite_wrapper*&)mNmfMain);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE AlsaSourceNmfHost::startMain(void)
{
	LOG_I("Enter.\n");

	mNmfMain->start();
	return OMX_ErrorNone;
}

OMX_ERRORTYPE AlsaSourceNmfHost::stopMain(void)
{
	LOG_I("Enter.\n");

	mNmfMain->stop();
	return OMX_ErrorNone;
}

OMX_ERRORTYPE AlsaSourceNmfHost::configureMain(void)
{
	LOG_I("Enter.\n");

	AlsaSource* proxy = static_cast<AlsaSource*>(&mENSComponent);

	char deviceName[ALSA_DEVICENAME_MAX];
	proxy->getDeviceName((OMX_U8*)deviceName, ALSA_DEVICENAME_MAX);
	LOG_I("\tDevice-name: %s\n", (char*)deviceName);
	mIAlgoConfig.setDeviceName(deviceName);

	AFM_PcmPort* outPort = (AFM_PcmPort*) proxy->getPort(PORT_IDX_OUTPUT);
	OMX_AUDIO_PARAM_PCMMODETYPE pcmSettings = outPort->getPcmSettings();

	// Set PCM config...
	alsasource_wrapper_config_t pcmConfig;
	memset(&pcmConfig, 0, sizeof(alsasource_wrapper_config_t));
	pcmConfig.sample_rate = pcmSettings.nSamplingRate;
	pcmConfig.nof_channels = pcmSettings.nChannels;
	pcmConfig.nof_bits_per_sample = pcmSettings.nBitPerSample;
	pcmConfig.block_size = DEFAULT_PCM_BLOCKSIZE;
	pcmConfig.endian_little = (pcmSettings.eEndian == OMX_EndianLittle);
	pcmConfig.interleaved = pcmSettings.bInterleaved;
	pcmConfig.numerical_data_signed = (pcmSettings.eNumData == OMX_NumericalDataSigned);

#ifdef MMPROBE_ENABLED
	pcmConfig.pcmprobe_cfg[PORT_IDX_OUTPUT].id = proxy->getPcmProbeId(PORT_IDX_OUTPUT);
	pcmConfig.pcmprobe_cfg[PORT_IDX_OUTPUT].enabled = proxy->isPcmProbeEnabled(PORT_IDX_OUTPUT);
#endif // MMPROBE_ENABLED
	mIMainConfig.setParameter(pcmConfig);
	LOG_I("Exit.\n");
	return OMX_ErrorNone;
}

OMX_ERRORTYPE AlsaSourceNmfHost::applyConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure)
{
	LOG_I("Enter.\n");

	switch ((int)nConfigIndex) {
		case OMX_IndexConfigALSADevice:
		{
			LOG_I("\tOMX_INDEXTYPE: OMX_IndexConfigALSADevice (%d)\n", (int)nConfigIndex);

			OMX_AUDIO_CONFIG_ALSADEVICETYPE* pAlsaDev = (OMX_AUDIO_CONFIG_ALSADEVICETYPE*)pComponentConfigStructure;

			char deviceName[ALSA_DEVICENAME_MAX];
			strncpy(deviceName, (char*)pAlsaDev->cDeviceName, ALSA_DEVICENAME_MAX);
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
			LOG_I("\tOMX_INDEXTYPE %d not known to AlsaSourceNmfHost.\n", (int)nConfigIndex);

			return AFMNmfHost_ProcessingComp::applyConfig(nConfigIndex, pComponentConfigStructure);
		}
	}
}

OMX_ERRORTYPE AlsaSourceNmfHost::retrieveConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure)
{
	LOG_I("Enter.\n");

	switch ((int)nConfigIndex) {
#ifdef MMPROBE_ENABLED
		case AFM_IndexConfigPcmProbe:
		{
			LOG_I("\tOMX_INDEXTYPE: AFM_IndexConfigPcmProbe (%d)\n", (int)nConfigIndex);

			AFM_AUDIO_CONFIG_PCM_PROBE* pPcmProbeCfg = (AFM_AUDIO_CONFIG_PCM_PROBE*)pComponentConfigStructure;
			AlsaSource* proxy = static_cast<AlsaSource*>(&mENSComponent);

			if (pPcmProbeCfg->nPortIndex > PORT_IDX_OUTPUT)
				return OMX_ErrorBadPortIndex;

			pPcmProbeCfg->nProbeIdx = proxy->getPcmProbeId(pPcmProbeCfg->nPortIndex);
			pPcmProbeCfg->bEnable = proxy->isPcmProbeEnabled(pPcmProbeCfg->nPortIndex);

			return OMX_ErrorNone;
		}
#endif // MMPROBE_ENABLED
		default:
		{
			LOG_I("\tOMX_INDEXTYPE %d not known to AlsaSourceNmfHost.\n", (int)nConfigIndex);

			return AFMNmfHost_ProcessingComp::retrieveConfig(nConfigIndex, pComponentConfigStructure);
		}
	}
}
