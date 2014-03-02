/*****************************************************************************/
/**
*  (c) ST-Ericsson, 2010 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief
* \author ST-Ericsson
*/
/*****************************************************************************/
#include <alsasink/nmfil/host/alsasink.nmf>
#include <armnmf_dbc.h>
#include <sys/time.h> /* workaround */
#include <tinyalsa/asoundlib.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <errno.h>
#include <semaphore.h>
#include <list>
#include <poll.h>
#include <linux/ioctl.h>
#include <sound/asound.h>
#include "OMX_Core.h"
#include <alsactrl_alsa.h>
#include <unistd.h>
#include "../../../../proxy/alsasink_debug.h"

#define POLL_TIME_MS 500
#define MAX_RETRY_COUNT 5

static void* entry_worker(void* param);

alsasink_nmfil_host_alsasink::alsasink_nmfil_host_alsasink()
{
	int err;

	LOG_I("Enter.\n");

	strncpy(mDeviceName, "default", sizeof(mDeviceName));
	mSampleRate = 48000;
	mBits = 16;
	mChannels = 2;
	mBlockSize = 240;
	mnPeriods = 4;
	mPlaybackHandle = NULL;
	mIProcess = NULL;
	mOpened = false;
	mCodingType = 1; //PCM
	pthread_mutex_init(&mutex, NULL);
	memset((void *)&fds, 0, sizeof(struct pollfd));
}

alsasink_nmfil_host_alsasink::~alsasink_nmfil_host_alsasink()
{
	LOG_I("Enter.\n");

	if (mOpened) {
		closeAlsaDevice();
	}
	pthread_mutex_destroy(&mutex);
}

void alsasink_nmfil_host_alsasink::consume(void)
{
	OMX_BUFFERHEADERTYPE* omxBufHdr;
	int nbrOfFrames;
	t_bool result;
	void *buffer;

	(void)sem_wait(&mSemaphore);

	while (mOpened) {
		pthread_mutex_lock(&mutex);
		omxBufHdr = mBufferlist.front();
		mBufferlist.pop_front();
		pthread_mutex_unlock(&mutex);

		nbrOfFrames = pcm_bytes_to_frames(mPlaybackHandle, omxBufHdr->nFilledLen);
		buffer = omxBufHdr->pBuffer + omxBufHdr->nOffset;

		result = sinkWriter(buffer, nbrOfFrames);

		if (result == false) {
			// In case of write failure, consume buffer at a controlled rate until
			// upper layer re-routes. Sleeping prevents buffer rush.
			int tBuffer = (1000000 * omxBufHdr->nFilledLen) / (mSampleRate * mChannels * mBits / 8);
			usleep(tBuffer);
		}
		mIProcess->bufferProcessed();
		(void)sem_wait(&mSemaphore);
	}
}

bool alsasink_nmfil_host_alsasink::sinkWriter(void *buffer, int nbrOfFrames)
{
	int ret, nbrbytes_to_write;

	LOG_I("Frames (ALSA) left to write: %d\n", (int)nbrOfFrames);

	nbrbytes_to_write = pcm_frames_to_bytes(mPlaybackHandle, nbrOfFrames);
	ret = pcm_write(mPlaybackHandle, buffer, nbrbytes_to_write);
	if (ret == 0) {
		LOG_I("Frames (ALSA) written: %d\n", (int)nbrOfFrames);
	} else {
		if (!poll(&fds, 1, POLL_TIME_MS)) {
			LOG_E("Error: Poll timeout. Unknown error when writing to PCM."
					"Error = %s", pcm_get_error(mPlaybackHandle));
			return false;
		}

		if (fds.revents & (POLLRDHUP|POLLERR|POLLHUP|POLLNVAL)) {
			LOG_E("Polling failed revents: 0x%X\n", fds.revents);
			return false;
		} else if (!(fds.revents & (POLLOUT|POLLWRNORM))) {
			LOG_I("Poll again revents 0x%X.\n", fds.revents);
		}
	}

	return true;
}

t_bool METH(open)(const alsasink_config_t *config)
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

void METH(reset)(t_alsasink_reset_reason reason)
{
	LOG_I("Enter.\n");
}

void METH(close)(void)
{
	LOG_I("Enter.\n");

	if (!closeAlsaDevice()) {
		LOG_E("Error: Unable to close ALSA device!\n");
	}
}

t_bool METH(process)(t_alsasink_process_params* params)
{
	LOG_I("Enter.\n");

	if (!mOpened) {
		openAlsaDevice();
	}

	if (!mOpened) {
		LOG_E("Warning: ALSA device not open! Returning...\n");

		// Signal that buffer has been processed so that the audio chain doesn't hang
		if (mIProcess != NULL) {

			// Sleep a while to prevent buffer rush
			OMX_BUFFERHEADERTYPE* omxBufHdr = params->buf_hdr;
			int tBuffer = (1000000 * omxBufHdr->nFilledLen) / (mSampleRate * mChannels * mBits / 8);
			usleep(tBuffer);

			// Signal that buffer has been processed
			mIProcess->bufferProcessed();
		}
		return false;
	}

	pthread_mutex_lock(&mutex);
	mBufferlist.push_back((OMX_BUFFERHEADERTYPE*)params->buf_hdr);
	pthread_mutex_unlock(&mutex);

	sem_post(&mSemaphore);
	return true;
}

void METH(setProcessCB)(void *param)
{
	mIProcess = (Ialsasink_nmfil_host_wrapper_process*)param;
}

unsigned int alsasink_pcm_get_latency(struct pcm *pcm, signed long *latency)
{
	int ret;
	if (!pcm)
		return -1;
	ret = ioctl(*(int *)pcm, SNDRV_PCM_IOCTL_DELAY, latency);
	return ret;
}

void METH(getLatency)(AlsasinkLatency_t* latency)
{
	int ret;
	long latencyFrames;

	LOG_I("Enter.\n");

	if (!mOpened) {
		LOG_E("Error: Device is not opened\n");
		latency->succeeded = false;
		return;
	}

	if ((ret = alsasink_pcm_get_latency(mPlaybackHandle, &latencyFrames)) < 0) {
		LOG_E("Error: Failed to retrieve delay (%s)\n", strerror(ret));
		latency->succeeded = false;
		return;
	}

	latency->latencyMs = (latencyFrames * 1000) / mSampleRate;
	latency->succeeded = true;
	LOG_I("latency->latencyMs: %lu, latencyFrames: %lu\n", latency->latencyMs, latencyFrames);
	return;
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

	strncpy(deviceName, mDeviceName,(int)nMax < (int)sizeof(deviceName) ? nMax :
			(int)sizeof(deviceName));
}

void METH(setCodingType)(t_uint32 codingType)
{
	LOG_I("Enter (codingType = %d\n", (int)codingType);

	mCodingType = (int)codingType;
}

void METH(getCodingType)(t_uint32* codingType)
{
	LOG_I("Enter (mCodingType = %d\n", (int)mCodingType);

	*codingType = (t_uint32)mCodingType;
}

bool METH(openAlsaDevice)(void)
{
	int err, idx_card, idx_dev, tryNbLoop;
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
	/*case 8:
		pcmFormat = PCM_FORMAT_S8_LE;
		format_string = (char*)"S8";
		break; NOT SUPPORTED IN TINYALSA*/
	case 16:
		pcmFormat = PCM_FORMAT_S16_LE;
		format_string = (char*)"S16_LE";
		break;
	/*case 24:
		pcmFormat = PCM_FORMAT_S24_LE;
		format_string = (char*)"S24_LE";
		break; NOT SUPPORTED IN TINYALSA*/
	case 32:
		pcmFormat = PCM_FORMAT_S32_LE;
		format_string = (char*)"S32_LE";
		break;
	default:
		LOG_E("Error: Unsupported bits per sample (%d)!\n", mBits);
		return false;
	}
	flags = PCM_OUT;
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
	idx_card = -1;
	err = audio_hal_alsa_get_card_and_device_idx(mDeviceName, PCM_OUT, &idx_card, &idx_dev);
	tryNbLoop = 0;
retry:
	if (tryNbLoop < MAX_RETRY_COUNT) {
		if (err < 0) {
			LOG_I("Warning: ALSA-device %s not found! Trying to open ALSA-device"
					"in case the device is a plugin-device.\n", mDeviceName);
			mPlaybackHandle = pcm_open(idx_card, idx_dev, flags, &config);
			if (!mPlaybackHandle || !pcm_is_ready(mPlaybackHandle)) {
				LOG_E("ERROR: Open ALSA-device %s, Stream PCM_OUT failed (ret = %s)!\n",
				dev_name_hw, pcm_get_error(mPlaybackHandle));
				tryNbLoop++;
				LOG_I("Retrying pcm_open %dth time", tryNbLoop);
				goto retry;
			}
		} else {
			sprintf(dev_name_hw, "hw:%d,%d", idx_card, idx_dev);
			LOG_I("ALSA hw-device found: %s.\n", dev_name_hw);
			LOG_I("Opening ALSA-device %s.\n", dev_name_hw);
			mPlaybackHandle = pcm_open(idx_card, idx_dev, flags, &config);
			if (!mPlaybackHandle || !pcm_is_ready(mPlaybackHandle)) {
				LOG_E("ERROR: Open ALSA-device %s, Stream PCM_OUT failed (ret = %s)!\n",
				dev_name_hw, pcm_get_error(mPlaybackHandle));
				tryNbLoop++;
				LOG_I("Retrying pcm_open %dth time", tryNbLoop);
				goto retry;
			}
		}
	} else {
		LOG_E("All retries failed : unable to open the device");
		closeAlsaDevice();
		return false;
	}
	/* Device is opened now */
	mOpened = true;
	pthread_create(&mWorker, 0, entry_worker, this);
	mBufferlist.clear();
	sem_init(&mSemaphore, 0, 0);

	LOG_I("mCodingType = %d.\n", mCodingType);

	//Check if the previous function audio_hal_alsa_get_card_and_device_idx succeed
	// If not, no need to call audio_hal_alsa_set_control for non HDMI sinks
	if (idx_card != -1) {
		audio_hal_alsa_open_controls_cardno(idx_card);
		err = audio_hal_alsa_set_control("HDMI Coding Type", 0, mCodingType);
		audio_hal_alsa_close_controls();
	}
	if (err < 0) {
		LOG_E("%s: Warning: Unable to set HDMI Coding Type!\n", __func__);
	}
	memset(&fds, 0, sizeof(struct pollfd));
	fds.fd = *(int *)mPlaybackHandle;
	fds.events = POLLOUT | POLLERR | POLLNVAL | POLLRDHUP | POLLHUP | POLLWRNORM;
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
	alsasink_nmfil_host_alsasink *sink = (alsasink_nmfil_host_alsasink*)param;
	sink->consume();
	return NULL;
}
