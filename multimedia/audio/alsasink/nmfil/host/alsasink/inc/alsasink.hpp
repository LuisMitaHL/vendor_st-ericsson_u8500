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
#ifndef _alsasink_nmfil_host_alsasink_hpp_
#define _alsasink_nmfil_host_alsasink_hpp_

#include "alsasink/nmfil/host/wrapper/process.hpp"

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 1
#endif
#include <tinyalsa/asoundlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <list>
#include <poll.h>

#define ALSA_DEVICENAME_MAX 32

/**
 * @brief Class that implements the Tone genenrator NMF component for the host CPU.
 */
class alsasink_nmfil_host_alsasink : public alsasink_nmfil_host_alsasinkTemplate
{
public:

	alsasink_nmfil_host_alsasink();
	~alsasink_nmfil_host_alsasink();

	// alsasink interface
	t_bool open(const alsasink_config_t *config);
	void reset(t_alsasink_reset_reason reason);
	t_bool process(t_alsasink_process_params* params);
	void consume(void);
	void setProcessCB(void *param);
	void close(void);

	// AlsaSink interface
	void setDeviceName(char deviceName[ALSA_DEVICENAME_MAX]);
	void getDeviceName(char* deviceName, t_uint32 nMax);
	void setCodingType(t_uint32 codingtype);
	void getCodingType(t_uint32* codingType);
	void getLatency(AlsasinkLatency_t* latency);

private:
	bool openAlsaDevice(void);
	bool closeAlsaDevice(void);
	bool sinkWriter(void *buffer, int nbrOfFrames);
	void applySettings(void);

	t_bool mOpened;
	t_uint32 mSampleRate;
	t_uint16 mChannels;
	t_uint16 mBits;
	t_uint16 mBlockSize;
	t_uint16 mnPeriods;
	char mDeviceName[ALSA_DEVICENAME_MAX];
	int mCodingType;
	struct pcm *mPlaybackHandle;
	sem_t mSemaphore;
	pthread_t mWorker;
	pthread_mutex_t mutex;
	std::list<OMX_BUFFERHEADERTYPE*> mBufferlist;
	Ialsasink_nmfil_host_wrapper_process  *mIProcess;
	int nfds;
	struct pollfd fds;
};

#endif // _alsasink_nmfil_host_alsasink_hpp_
