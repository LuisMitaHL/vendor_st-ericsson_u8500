/*****************************************************************************/
/**
*  (c) ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief
* \author ST-Ericsson
*/
/*****************************************************************************/
#include <alsasource/nmfil/host/alsasource.nmf>
#include <armnmf_dbc.h>
#include <sys/time.h> /* workaround */
#include <tinyalsa/asoundlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <list>
#include "OMX_Core.h"
#include <alsactrl_alsa.h>
#include <unistd.h>
#include "../../../../proxy/alsasource_debug.h"

static void* entry_worker(void* param);

alsasource_nmfil_host_alsasource::alsasource_nmfil_host_alsasource()
{
	int err;
	LOG_I("Enter.\n");
	strncpy(mDeviceName, "default", sizeof(mDeviceName));
	mSampleRate = 48000;
	mBits = 16;
	mChannels = 1;
	mBlockSize = 240;
	mnPeriods = 4;
	mPlaybackHandle = NULL;
	mIProcess = NULL;
	mOpened = false;
	pthread_mutex_init(&mutex, NULL);
}

alsasource_nmfil_host_alsasource::~alsasource_nmfil_host_alsasource()
{
	LOG_I("Enter.\n");
	if (mOpened) {
		closeAlsaDevice();
	}
	pthread_mutex_destroy(&mutex);
}

void alsasource_nmfil_host_alsasource::produce(void)
{
	OMX_BUFFERHEADERTYPE* omxBufHdr;
	int ret;
	void *buffer;
	(void)sem_wait(&mSemaphore);

	while (mOpened) {
		pthread_mutex_lock(&mutex);
		omxBufHdr = mBufferlist.front();
		mBufferlist.pop_front();
		pthread_mutex_unlock(&mutex);
		buffer = omxBufHdr->pBuffer + omxBufHdr->nOffset;
		LOG_I("Read buffer size = %d", omxBufHdr->nAllocLen);

		ret = pcm_read(mPlaybackHandle, buffer, omxBufHdr->nAllocLen);

		if (0 == ret) {
			LOG_I("Frames (ALSA) read: %d\n",
							(int)pcm_bytes_to_frames(mPlaybackHandle, omxBufHdr->nAllocLen));
			omxBufHdr->nFilledLen += omxBufHdr->nAllocLen;
			buffer = ((char*)buffer) + omxBufHdr->nAllocLen;
		} else {
			LOG_E("Error: Unable to handle error (%d) when reading from PCM. Error : %s\n",
						ret, pcm_get_error(mPlaybackHandle));
			int tBuffer = (1000000 * omxBufHdr->nFilledLen) / (mSampleRate * mChannels * mBits / 8);
			usleep(tBuffer);
			break;
		}

		mIProcess->bufferProcessed();
		(void)sem_wait(&mSemaphore);
	}
}

t_bool METH(open)(const alsasource_config_t *config)
{
	LOG_I("Enter.\n");
	mSampleRate = config->sample_rate;
	mChannels = config->nof_channels;
	mBits = config->nof_bits_per_sample;
	mBlockSize = config->block_size;
	LOG_I("SampleRate: %d\n", (int)mSampleRate);
	LOG_I("Channels: %d\n", (int)mChannels);
	LOG_I("Bits: %d\n", (int)mBits);
	LOG_I("Block-size: %d\n", (int)mBlockSize);
	if (!openAlsaDevice()) {
		return false;
	}
	return true;
}

void METH(reset)(t_alsasource_reset_reason reason)
{
	LOG_I("Enter.\n");
}

void METH(close)(void)
{
	LOG_I("Enter.\n");
	if (!closeAlsaDevice()) {
		LOG_I("Error: Unable to close ALSA device!\n");
	}
}

t_bool METH(process)(t_alsasource_process_params* params)
{
	pthread_mutex_lock(&mutex);
	mBufferlist.push_back((OMX_BUFFERHEADERTYPE*)params->buf_hdr);
	pthread_mutex_unlock(&mutex);
	sem_post(&mSemaphore);
	return true;
}

void METH(setProcessCB)(void *param)
{
	mIProcess = (Ialsasource_nmfil_host_wrapper_process*)param;
}

void METH(setDeviceName)(char deviceName[ALSA_DEVICENAME_MAX])
{
	LOG_I("Enter (deviceName = '%s').\n", (char*)deviceName);
	if (strncmp(mDeviceName, (char*)deviceName, ALSA_DEVICENAME_MAX)) {
		/* Card doesn't match old value */
		strncpy(mDeviceName, deviceName, ALSA_DEVICENAME_MAX);
		/* Already open, update with new settings */
		if (mOpened)
			applySettings();
	}
}

void METH(getDeviceName)(char* deviceName, t_uint32 nMax)
{
	LOG_I("Enter (mDeviceName = '%s').\n", (char*)mDeviceName);
	strncpy(deviceName, mDeviceName, (int)nMax < (int)sizeof(deviceName) ? nMax : (int)sizeof(deviceName));
}

bool METH(openAlsaDevice)(void)
{
	int err, idx_card, idx_dev;
	enum pcm_format pcmFormat;
	t_uint32 alsa_rate;
	unsigned int flags = 0;
	int period_size, period_size_wanted, buffer_size, buffer_size_wanted;
	char* format_string = NULL;
	char dev_name_hw[64];
	const char* card_name_p = "default";
	struct pcm_config config;

	LOG_I("Enter.\n");
	LOG_I("Sample-rate: %d\n", (int)mSampleRate);
	switch (mBits) {
	/*case 8: NOT SUPPORTED IN TINYALSA
		pcmFormat = PCM_FORMAT_S8_LE;
		format_string = (char*)"S8";
		break;*/
	case 16:
		pcmFormat = PCM_FORMAT_S16_LE;
		format_string = (char*)"S16_LE";
		break;
	/*case 24: NOT SUPPORTED IN TINYALSA
		pcmFormat = PCM_FORMAT_S24_LE;
		format_string = (char*)"S24_LE";
		break;*/
	case 32:
		pcmFormat = PCM_FORMAT_S32_LE;
		format_string = (char*)"S32_LE";
		break;
	default:
		LOG_I("Error: Unsupported bits per sample (%d)!\n", mBits);
		return false;
	}
	flags = PCM_IN;
	memset(&config, 0, sizeof(struct pcm_config));
	config.channels = mChannels;
	config.rate = mSampleRate;
	config.period_size = mBlockSize;
	config.period_count = mnPeriods;
	config.format = pcmFormat;
	LOG_I("Sample rate: %u", mSampleRate);
	LOG_I("No of Channels: %u", mChannels);
	LOG_I("Bits per sample: %d", mBits);
	LOG_I("PCM format: %s", format_string);
	LOG_I("Device-name: %s", mDeviceName);
	LOG_I("Block Size: %d", mBlockSize);
	LOG_I("Periods Count: %d", mnPeriods);
	err = audio_hal_alsa_get_card_and_device_idx(mDeviceName, PCM_IN, &idx_card, &idx_dev);
	if (err < 0) {
		LOG_I("Warning: ALSA-device %s not found! Trying to open ALSA-device in case the"
				"device is a plugin-device.\n", mDeviceName);
		mPlaybackHandle = pcm_open(idx_card, idx_dev, flags, &config);
		if (!mPlaybackHandle || !pcm_is_ready(mPlaybackHandle)) {
			LOG_E("ERROR: Open ALSA-device %s, Stream PCM_IN failed (ret = %s)!\n",
				dev_name_hw, pcm_get_error(mPlaybackHandle));
			return false;
		}
	} else {
		sprintf(dev_name_hw, "hw:%d,%d", idx_card, idx_dev);
		LOG_I("ALSA hw-device found: %s.\n", dev_name_hw);
		LOG_I("Opening ALSA-device %s.\n", dev_name_hw);
		mPlaybackHandle = pcm_open(idx_card, idx_dev, flags, &config);
		if (!mPlaybackHandle || !pcm_is_ready(mPlaybackHandle)) {
			LOG_E("ERROR: Open ALSA-device %s, stream PCM_IN failed (ret = %s)!\n",
			dev_name_hw, pcm_get_error(mPlaybackHandle));
			return false;
		}
	}
	mOpened = true;
	pthread_create(&mWorker, 0, entry_worker, this);
	mBufferlist.clear();
	sem_init(&mSemaphore, 0, 0);
	return true;
}

bool METH(closeAlsaDevice)(void)
{
	LOG_I("Enter.\n");
	if (!mOpened) {
		LOG_I("Warning: ALSA device not open! Returning...\n");
		return true;
	}
	mOpened = false;
	sem_post(&mSemaphore);
	pthread_join(mWorker, NULL);
	sem_destroy(&mSemaphore);
	LOG_I("mPlaybackHandle = %d\n", (int)mPlaybackHandle);
	if (mPlaybackHandle != NULL)
		pcm_close(mPlaybackHandle);
	mPlaybackHandle = NULL;
	return true;
}

void METH(applySettings)(void)
{
	bool ret;
	LOG_I("Enter.\n");
	ret = closeAlsaDevice();
	if (!ret) {
		LOG_I("Failed to close ALSA device\n");
		return;
	}
	ret = openAlsaDevice();
	if (!ret) {
		LOG_I("Failed to open ALSA device\n");
	}
	LOG_I("Exit.\n");
	return;
}

static void* entry_worker(void* param)
{
	alsasource_nmfil_host_alsasource *source = (alsasource_nmfil_host_alsasource*)param;
	source->produce();
	return NULL;
}
