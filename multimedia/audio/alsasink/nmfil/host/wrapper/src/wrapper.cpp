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

#include <alsasink/nmfil/host/wrapper.nmf>
#include <string.h>
#include <unistd.h>
#include <verbose.h>
#include <armnmf_dbc.h>
#include <stdio.h>
#include "../../../../proxy/alsasink_debug.h"
#ifdef MMPROBE_ENABLED
#include "r_mm_probe.h"
#endif // MMPROBE_ENABLED

alsasink_nmfil_host_wrapper::alsasink_nmfil_host_wrapper(void)
{
	mResetCount = 0;
	mStartTime = false;
	mTimeStamp = 0;
	m_alsasink_config.sample_rate = 48000;
	m_alsasink_config.nof_channels = 2;
	m_alsasink_config.nof_bits_per_sample = 16;
	m_alsasink_config.block_size = 240;
	m_alsasink_config.endian_little = true;
	m_alsasink_config.interleaved = true;
	m_alsasink_config.numerical_data_signed = true;
#ifdef MMPROBE_ENABLED
	m_alsasink_config.pcmprobe_cfg[PORT_IDX_INPUT].enabled = false;
	m_alsasink_config.pcmprobe_cfg[PORT_IDX_FEEDBACK].enabled = false;
#endif // MMPROBE_ENABLED
	mToBeReturned = 0;
	mSentForProc = 0;
}

static t_uint32 convertFreqToRate(t_sample_freq freq)
{
	t_uint32 rate;
	switch (freq) {
	case FREQ_192KHZ:
		rate = 192000;
		break;
	case FREQ_176_4KHZ:
		rate = 176400;
		break;
	case FREQ_128KHZ:
		rate = 128000;
		break;
	case FREQ_96KHZ:
		rate = 96000;
		break;
	case FREQ_88_2KHZ:
		rate = 88200;
		break;
	case FREQ_64KHZ:
		rate = 64000;
		break;
	case FREQ_48KHZ:
		rate = 48000;
		break;
	case FREQ_44_1KHZ:
		rate = 44100;
		break;
	case FREQ_32KHZ:
		rate = 32000;
		break;
	case FREQ_24KHZ:
		rate = 24000;
		break;
	case FREQ_22_05KHZ:
		rate = 22050;
		break;
	case FREQ_16KHZ:
		rate = 16000;
		break;
	case FREQ_12KHZ:
		rate = 12000;
		break;
	case FREQ_11_025KHZ:
		rate = 11025;
		break;
	case FREQ_8KHZ:
		rate = 8000;
		break;
	case FREQ_7_2KHZ:
		rate = 7200;
		break;
	default:
		rate = 0;
		LOG_I("Warning: Unknown Freq (%d)!\n", freq);
		break;
	}

	return rate;
}

#ifdef MMPROBE_ENABLED
static void configurePcmProbeDataFormat(alsasink_config_t config, DataFormat_t *mmprobe_format)
{
	LOG_I("Enter.\n");

	switch (config.sample_rate) {
	case 192000:
		mmprobe_format->SampleRate = MM_PROBE_SAMPLE_RATE_192KHZ;
		break;
	case 176400:
		mmprobe_format->SampleRate = MM_PROBE_SAMPLE_RATE_176_4KHZ;
		break;
	case 128000:
		mmprobe_format->SampleRate = MM_PROBE_SAMPLE_RATE_128KHZ;
		break;
	case 96000:
		mmprobe_format->SampleRate = MM_PROBE_SAMPLE_RATE_96KHZ;
		break;
	case 88200:
		mmprobe_format->SampleRate = MM_PROBE_SAMPLE_RATE_88_2KHZ;
		break;
	case 64000:
		mmprobe_format->SampleRate = MM_PROBE_SAMPLE_RATE_64KHZ;
		break;
	case 48000:
		mmprobe_format->SampleRate = MM_PROBE_SAMPLE_RATE_48KHZ;
		break;
	case 44100:
		mmprobe_format->SampleRate = MM_PROBE_SAMPLE_RATE_44_1KHZ;
		break;
	case 32000:
		mmprobe_format->SampleRate = MM_PROBE_SAMPLE_RATE_32KHZ;
		break;
	case 24000:
		mmprobe_format->SampleRate = MM_PROBE_SAMPLE_RATE_24KHZ;
		break;
	case 22050:
		mmprobe_format->SampleRate = MM_PROBE_SAMPLE_RATE_22_05KHZ;
		break;
	case 16000:
		mmprobe_format->SampleRate = MM_PROBE_SAMPLE_RATE_16KHZ;
		break;
	case 12000:
		mmprobe_format->SampleRate = MM_PROBE_SAMPLE_RATE_12KHZ;
		break;
	case 11025:
		mmprobe_format->SampleRate = MM_PROBE_SAMPLE_RATE_11_025KHZ;
		break;
	case 8000:
		mmprobe_format->SampleRate = MM_PROBE_SAMPLE_RATE_8KHZ;
		break;
	case 7200:
		mmprobe_format->SampleRate = MM_PROBE_SAMPLE_RATE_7_2KHZ;
		break;
	default:
		LOG_E("Unsupported sample rate, %d", (int)config.sample_rate);
		mmprobe_format->SampleRate = MM_PROBE_SAMPLE_RATE_UNKNOWN;
		break;
	}

	mmprobe_format->NoChannels = (uint8_t)config.nof_channels;
	mmprobe_format->Interleave = config.interleaved ? MM_PROBE_INTERLEAVED : MM_PROBE_INTERLEAVED_NONE;

	switch (config.nof_bits_per_sample) {
	case 16:
		if (config.numerical_data_signed)
			mmprobe_format->Resolution = (config.endian_little) ? MM_PROBE_FORMAT_S16_LE : MM_PROBE_FORMAT_S16_BE;
		else
			mmprobe_format->Resolution = (config.endian_little) ? MM_PROBE_FORMAT_U16_LE : MM_PROBE_FORMAT_U16_BE;
		break;
	default:
		LOG_E("Unsupported format, nof bits per sample = %d", (int)config.nof_bits_per_sample);
		mmprobe_format->Resolution = MM_PROBE_PCM_FORMAT_UNKNOWN;
		break;
	}
}

static void setPcmProbeCfg(t_uint32 portIdx, alsasink_wrapper_config_t config, alsasink_config_t *alsasink_config)
{
	alsasink_wrapper_pcmprobe_config_t* pPcmProbeCfg = &config.pcmprobe_cfg[portIdx];
	alsasink_pcmprobe_config_t* pAlsaSinkPcmProbeCfg = &alsasink_config->pcmprobe_cfg[portIdx];

	LOG_I("PcmProbe[%d]: id = %d (%s)\n", (int)portIdx, (int)pPcmProbeCfg->id, (pPcmProbeCfg->enabled) ? "enabled" : "disabled");

	pAlsaSinkPcmProbeCfg->id = pPcmProbeCfg->id;
	pAlsaSinkPcmProbeCfg->enabled = pPcmProbeCfg->enabled;
}
#endif // MMPROBE_ENABLED

void METH(start)(void)
{
	LOG_I("Enter.\n");

	mStartTime = true;
	mTimeStamp = 0;
	LOG_I("mResetCount = %d\n", mResetCount);
	LOG_I("mStartTime = %s\n", mStartTime ? "true" : "false");
	LOG_I("mTimeStamp = %d\n", mTimeStamp);
}

void METH(stop)(void)
{
	LOG_I("Enter.\n");

	// Close the alsasink
	sink.close();
}

void METH(newFormat)(t_sample_freq freq, t_uint16 chans_nb, t_uint16 sample_bitsize)
{
	t_uint32 sample_rate;

	LOG_I("Enter.\n");

	sample_rate = convertFreqToRate(freq);

	// Store input format...
	m_alsasink_config.sample_rate = sample_rate;
	m_alsasink_config.nof_channels = chans_nb;
	m_alsasink_config.nof_bits_per_sample = sample_bitsize;

	LOG_I("Frequency: %d\n", (int)freq);
	LOG_I("SampleRate: %d\n", (int)m_alsasink_config.sample_rate);
	LOG_I("Channels: %d\n", (int)m_alsasink_config.nof_channels);
	LOG_I("Bits: %d\n", (int)m_alsasink_config.nof_bits_per_sample);
}

void METH(fsmInit)(fsmInit_t initFsm)
{
	LOG_I("Enter.\n");

	// Crashes without this!
	if (initFsm.traceInfoAddr)
		setTraceInfo(initFsm.traceInfoAddr, initFsm.id1);

	// Open sink...
	bool result = sink.open(&m_alsasink_config);
	// TODO: Investigate what to do if open returns with failure!
	// fsmInit should have return value to block transition to Idle in case of failure!?
	mPort[PORT_IDX_INPUT].init(InputPort, false, false, NULL, NULL, 1, &inputport, 0, (initFsm.portsDisabled & 1) != 0, (initFsm.portsTunneled & 1) != 0, this);
	mPort[PORT_IDX_FEEDBACK].init(OutputPort, false, false, 0, 0, 1, &outputport,1, (initFsm.portsDisabled & (1 << 1)), (initFsm.portsTunneled & (1 << 1)), this);
	init(2, mPort, &proxy, &me, false);

	LOG_I("Exit.\n");
}

void METH(setTunnelStatus) (t_sint16 portIdx, t_uint16 isTunneled)
{
	LOG_I("Enter.\n");
}

void METH(sendCommand)(OMX_COMMANDTYPE cmd, t_uword param)
{
	Component::sendCommand(cmd, param);
}

void  METH(processEvent)(void)
{
	Component::processEvent();
}

void METH(emptyThisBuffer)(OMX_BUFFERHEADERTYPE_p buffer)
{
	OMX_BUFFERHEADERTYPE* omxBuffer = (OMX_BUFFERHEADERTYPE*)buffer;
	//omxBuffer->nFilledLen = 0;

	LOG_I("Enter.\n");

	LOG_I("omxBuffer->nFilledLen = %d", (int)omxBuffer->nFilledLen);
	LOG_I("omxBuffer->nAllocLen = %d", (int)omxBuffer->nAllocLen);
	Component::deliverBuffer(0, buffer);
}

void METH(fillThisBuffer)(OMX_BUFFERHEADERTYPE_p buffer)
{
	LOG_I("Enter.\n");
	OMX_BUFFERHEADERTYPE* omxBuffer = (OMX_BUFFERHEADERTYPE*)buffer;
	omxBuffer->nFilledLen = 0;
	LOG_I("omxBuffer->nFilledLen = %d", (int)omxBuffer->nFilledLen);
	LOG_I("omxBuffer->nAllocLen = %d", (int)omxBuffer->nAllocLen);
	Component::deliverBuffer(1, buffer);
}

// Interface for fsm
// reset is called when entering Idle state, inside Component::init called from fsmInit
void METH(reset)()
{
	LOG_I("Enter.\n");

	if (mResetCount++ > 0) {
		// If NOT first reset (Init -> Idle transition) then reset sink...
		sink.reset(SINK_RESET_REASON_STOP);
	}
}

void METH(disablePortIndication)(t_uint32 portIdx)
{
	LOG_I("Enter.\n");

	// TODO: Implement
	//ARMNMF_DBC_ASSERT(0);
}

void METH(enablePortIndication)(t_uint32 portIdx)
{
	LOG_I("Enter.\n");
	// TODO: Implement
	//ARMNMF_DBC_ASSERT(0);
}

void METH(flushPortIndication)(t_uint32 portIdx)
{
	LOG_I("Enter.\n");

  sink.reset(SINK_RESET_REASON_FLUSH);
  // TODO: Check if any pending buffers, and that in case return them
}

void METH(process)()
{
#ifdef MMPROBE_ENABLED
	alsasink_pcmprobe_config_t *pPcmProbeCfg;
	DataFormat_t pcmProbeDataFormat;
	t_bool pcmProbeDataFormatConfigured;
#endif // MMPROBE_ENABLED
	t_alsasink_process_params params;
	OMX_BUFFERHEADERTYPE *mBufHdrIn = NULL;
	OMX_BUFFERHEADERTYPE *mBufHdrFb = NULL;
	t_sint16 *mBufIn;
	t_sint16 *mBufFb;
	static int count = 0;

	if(!mPort[PORT_IDX_INPUT].isEnabled()) {
		return;
	}

	LOG_I("Enter.\n");

#ifdef MMPROBE_ENABLED
	pcmProbeDataFormatConfigured = false;
#endif // MMPROBE_ENABLED

	while (mToBeReturned) {
		mBufHdrIn = mPort[PORT_IDX_INPUT].getBuffer(0);
		mBufHdrIn->nFilledLen = mBufHdrIn->nAllocLen;
		mBufHdrIn->nOffset = 0;
		mBufHdrIn->nOutputPortIndex = 0;
		mBufHdrIn->nTimeStamp = mTimeStamp;

		// Buffer time in microseconds
		int tBuffer = (1000000 * mBufHdrIn->nFilledLen) /
			(m_alsasink_config.sample_rate *
			m_alsasink_config.nof_channels *
			m_alsasink_config.nof_bits_per_sample / 8);

		mTimeStamp += tBuffer;

		mBufIn = (t_sint16 *)((t_uint32)mBufHdrIn->pBuffer + mBufHdrIn->nOffset);

		// Adding feedback port, copy data from input port to feedback port
		if(mPort[PORT_IDX_FEEDBACK].queuedBufferCount() && mPort[PORT_IDX_FEEDBACK].isEnabled()){
			mBufHdrFb = mPort[PORT_IDX_FEEDBACK].getBuffer(0);
			mBufFb = (t_sint16 *)((t_uint32)mBufHdrFb->pBuffer + mBufHdrFb->nOffset);
			memcpy(mBufFb ,mBufIn, mBufHdrIn->nFilledLen);
			mBufHdrFb->nFilledLen = mBufHdrIn->nFilledLen ;
			mBufHdrFb->nFlags     = mBufHdrIn->nFlags;
			mBufHdrFb->nTimeStamp = mBufHdrIn->nTimeStamp;
#ifdef MMPROBE_ENABLED
			pPcmProbeCfg = &m_alsasink_config.pcmprobe_cfg[PORT_IDX_FEEDBACK];
			if (pPcmProbeCfg->enabled) {
				if (!pcmProbeDataFormatConfigured) {
					configurePcmProbeDataFormat(m_alsasink_config, &pcmProbeDataFormat);
					pcmProbeDataFormatConfigured = true;
				}
				LOG_I("probe pcm data, probe_id=%d, data=%08x, bytes=%d, format=%d,%d,%d,%d\n",
					(int)pPcmProbeCfg->id, (int)mBufFb, (int)mBufHdrFb->nFilledLen,
					(int)pcmProbeDataFormat.SampleRate, (int)pcmProbeDataFormat.NoChannels,
					(int)pcmProbeDataFormat.Interleave, (int)pcmProbeDataFormat.Resolution);
				mmprobe_probe_V2(pPcmProbeCfg->id, mBufFb, mBufHdrFb->nFilledLen, &pcmProbeDataFormat);
			}
#endif // MMPROBE_ENABLED
			mPort[PORT_IDX_FEEDBACK].dequeueAndReturnBuffer();
		}

		if (mStartTime) {
			mBufHdrIn->nFlags |= OMX_BUFFERFLAG_STARTTIME;
			mStartTime = false;
			LOG_I("Setting nFlags = OMX_BUFFERFLAG_STARTTIME\n");
			LOG_I("mStartTime = %s\n", mStartTime ? "true" : "false");
		}
#ifdef MMPROBE_ENABLED
		pPcmProbeCfg = &m_alsasink_config.pcmprobe_cfg[PORT_IDX_INPUT];
		if (pPcmProbeCfg->enabled) {
			if (!pcmProbeDataFormatConfigured) {
				configurePcmProbeDataFormat(m_alsasink_config, &pcmProbeDataFormat);
				pcmProbeDataFormatConfigured = true;
			}
			LOG_I("probe pcm data, probe_id=%d, data=%08x, bytes=%d, format=%d,%d,%d,%d\n",
				(int)pPcmProbeCfg->id, (int)mBufIn, (int)mBufHdrIn->nFilledLen,
				(int)pcmProbeDataFormat.SampleRate, (int)pcmProbeDataFormat.NoChannels,
				(int)pcmProbeDataFormat.Interleave, (int)pcmProbeDataFormat.Resolution);
			mmprobe_probe_V2(pPcmProbeCfg->id, mBufIn, mBufHdrIn->nFilledLen, &pcmProbeDataFormat);
		}
#endif // MMPROBE_ENABLED
		mPort[PORT_IDX_INPUT].dequeueAndReturnBuffer();

		// Fire buffer flag event if EOS...
		if (mBufHdrIn->nFlags & OMX_BUFFERFLAG_EOS) {
			LOG_I("Buffer marked as EOS.\n");
			proxy.eventHandler(OMX_EventBufferFlag, 1, mBufHdrIn->nFlags);
			sink.reset(SINK_RESET_REASON_EOS);
		}

		mToBeReturned--;
		mSentForProc--;
	}

	// If no buffers on port then return...
	if(mPort[PORT_IDX_INPUT].queuedBufferCount() == 0 || mPort[PORT_IDX_INPUT].queuedBufferCount() == mSentForProc) {
		LOG_I("\tNo buffers queued. Returning...\n");
		return;
	}

	// Point to feedback-buffer (do not de-queue until processed)...
	mBufHdrIn = mPort[PORT_IDX_INPUT].getBuffer(mPort[PORT_IDX_INPUT].queuedBufferCount()-1);
	params.buf_hdr = mBufHdrIn;
	sink.process(&params);
	mSentForProc++;
}

void METH(bufferProcessed)(void)
{
        LOG_I("Processed!.\n");
        mToBeReturned++;
        Component::scheduleProcessEvent();
}

void METH(setParameter)(alsasink_wrapper_config_t config)
{
	LOG_I("Enter.\n");

	// Store input format...
	m_alsasink_config.sample_rate = config.sample_rate;
	m_alsasink_config.nof_channels = config.nof_channels;
	m_alsasink_config.nof_bits_per_sample = config.nof_bits_per_sample;
	m_alsasink_config.endian_little = config.endian_little;
	m_alsasink_config.interleaved = config.interleaved;
	m_alsasink_config.numerical_data_signed = config.numerical_data_signed;

#ifdef MMPROBE_ENABLED
	setPcmProbeCfg(PORT_IDX_INPUT, config, &m_alsasink_config);
	setPcmProbeCfg(PORT_IDX_FEEDBACK, config, &m_alsasink_config);
#endif // MMPROBE_ENABLED

	LOG_I("\tSampleRate: %d\n", (int)m_alsasink_config.sample_rate);
	LOG_I("\tChannels: %d\n", (int)m_alsasink_config.nof_channels);
	LOG_I("\tBits: %d\n", (int)m_alsasink_config.nof_bits_per_sample);
	LOG_I("\tBlockSize: %d\n", (int)m_alsasink_config.block_size);
	LOG_I("\tEndianness: %s-Endian\n", (m_alsasink_config.endian_little) ? "Little" : "Big");
	LOG_I("\tInterleaved: %s\n", (m_alsasink_config.interleaved) ? "Yes" : "No");
	LOG_I("\tNumericalData: %s\n", (m_alsasink_config.numerical_data_signed) ? "Signed" : "Unsigned");
}
