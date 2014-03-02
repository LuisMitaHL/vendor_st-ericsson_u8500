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
#ifndef _alsasource_nmfil_host_alsasource_hpp_
#define _alsasource_nmfil_host_alsasource_hpp_

#include "alsasource/nmfil/host/wrapper/process.hpp"

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 1
#endif
#include <tinyalsa/asoundlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <list>

#define ALSA_DEVICENAME_MAX 32

/**
 * @brief Class that implements the ALSA source NMF component for the host CPU.
 */
class alsasource_nmfil_host_alsasource : public alsasource_nmfil_host_alsasourceTemplate
{
public:

	alsasource_nmfil_host_alsasource();
	~alsasource_nmfil_host_alsasource();

	// alsasource interface
	t_bool open(const alsasource_config_t *config);
	void reset(t_alsasource_reset_reason reason);
	t_bool process(t_alsasource_process_params* params);
	void produce(void);
	void setProcessCB(void *param);
	void close(void);

	// AlsaSource interface
	void setDeviceName(char deviceName[ALSA_DEVICENAME_MAX]);
	void getDeviceName(char* deviceName, t_uint32 nMax);

private:
	bool openAlsaDevice(void);
	bool closeAlsaDevice(void);
	void applySettings(void);

	t_bool mOpened;
	t_uint32 mSampleRate;
	t_uint16 mChannels;
	t_uint16 mBits;
	t_uint16 mBlockSize;
	t_uint16 mnPeriods;
	char mDeviceName[ALSA_DEVICENAME_MAX];
	struct pcm *mPlaybackHandle;
	sem_t mSemaphore;
	pthread_t mWorker;
	pthread_mutex_t mutex;
	std::list<OMX_BUFFERHEADERTYPE*> mBufferlist;
	Ialsasource_nmfil_host_wrapper_process *mIProcess;
};

#endif // _alsasource_nmfil_host_alsasource_hpp_
