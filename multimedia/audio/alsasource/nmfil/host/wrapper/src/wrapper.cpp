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

#include <alsasource/nmfil/host/wrapper.nmf>
#include <string.h>
#include <verbose.h>
#include <armnmf_dbc.h>
#include <stdio.h>
#include "../../../../proxy/alsasource_debug.h"
#ifdef MMPROBE_ENABLED
#include "r_mm_probe.h"
#endif // MMPROBE_ENABLED

alsasource_nmfil_host_wrapper::alsasource_nmfil_host_wrapper(void)
{
	mResetCount = 0;
	mStartTime = false;
	mTimeStamp = 0;
	m_alsasource_config.sample_rate = 48000;
	m_alsasource_config.nof_channels = 1;
	m_alsasource_config.nof_bits_per_sample = 16;
	m_alsasource_config.block_size = 240;
	m_alsasource_config.endian_little = true;
	m_alsasource_config.interleaved = true;
	m_alsasource_config.numerical_data_signed = true;
#ifdef MMPROBE_ENABLED
	m_alsasource_config.pcmprobe_cfg[PORT_IDX_OUTPUT].enabled = false;
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
static void configurePcmProbeDataFormat(alsasource_config_t config, DataFormat_t *mmprobe_format)
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

static void setPcmProbeCfg(t_uint32 portIdx, alsasource_wrapper_config_t config, alsasource_config_t *alsasource_config)
{
	alsasource_wrapper_pcmprobe_config_t* pPcmProbeCfg = &config.pcmprobe_cfg[portIdx];
	alsasource_pcmprobe_config_t* pAlsaSourcePcmProbeCfg = &alsasource_config->pcmprobe_cfg[portIdx];

	LOG_I("PcmProbe[%d]: id = %d (%s)\n", (int)portIdx, (int)pPcmProbeCfg->id, (pPcmProbeCfg->enabled) ? "enabled" : "disabled");

	pAlsaSourcePcmProbeCfg->id = pPcmProbeCfg->id;
	pAlsaSourcePcmProbeCfg->enabled = pPcmProbeCfg->enabled;
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

	// Close the alsasource
	source.close();
}

void METH(newFormat)(t_sample_freq freq, t_uint16 chans_nb, t_uint16 sample_bitsize)
{
	t_uint32 sample_rate;

	LOG_I("Enter.\n");

	sample_rate = convertFreqToRate(freq);

	// Store input format...
	m_alsasource_config.sample_rate = sample_rate;
	m_alsasource_config.nof_channels = chans_nb;
	m_alsasource_config.nof_bits_per_sample = sample_bitsize;

	LOG_I("Frequency: %d\n", (int)freq);
	LOG_I("SampleRate: %d\n", (int)m_alsasource_config.sample_rate);
	LOG_I("Channels: %d\n", (int)m_alsasource_config.nof_channels);
	LOG_I("Bits: %d\n", (int)m_alsasource_config.nof_bits_per_sample);
}

void METH(fsmInit)(fsmInit_t initFsm)
{
	LOG_I("Enter.\n");

	// Crashes without this!
	if (initFsm.traceInfoAddr)
		setTraceInfo(initFsm.traceInfoAddr, initFsm.id1);

	mPort[PORT_IDX_OUTPUT].init(OutputPort, false, false, NULL, NULL, 1, &outputport, 0, ((initFsm.portsDisabled & 1) != 0), ((initFsm.portsTunneled & 1) != 0), this);
	init(1, mPort, &proxy, &me, false);

	// Open source...
	bool result = source.open(&m_alsasource_config);
	// TODO: Investigate what to do if open returns with failure!
	// fsmInit should have return value to block transition to Idle in case of failure!?
	ARMNMF_DBC_ASSERT(result);

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
	Component::deliverBuffer(0, buffer);
}

// Interface for fsm
// reset is called when entering Idle state, inside Component::init called from fsmInit
void METH(reset)()
{
	LOG_I("Enter.\n");

	if (mResetCount++ > 0) {
		// If NOT first reset (Init -> Idle transition) then reset source...
		source.reset(SOURCE_RESET_REASON_STOP);
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

	source.reset(SOURCE_RESET_REASON_FLUSH);
	// TODO: Check if any pending buffers, and that in case return them
}

void METH(process)()
{
#ifdef MMPROBE_ENABLED
	alsasource_pcmprobe_config_t *pPcmProbeCfg;
	DataFormat_t pcmProbeDataFormat;
	t_bool pcmProbeDataFormatConfigured;
#endif // MMPROBE_ENABLED
	t_alsasource_process_params params;
	OMX_BUFFERHEADERTYPE *omxBufHdr = NULL;
	t_sint16 *pBufOut;
	static int count = 0;

	if(!mPort[PORT_IDX_OUTPUT].isEnabled()) {
		return;
	}

	LOG_I("alsasource_nmfil_host_wrapper::process() mToBeReturned:%d, mSentForProc:%d \n", mToBeReturned, mSentForProc);

#ifdef MMPROBE_ENABLED
	pcmProbeDataFormatConfigured = false;
#endif // MMPROBE_ENABLED

	while (mToBeReturned) {
		omxBufHdr = mPort[PORT_IDX_OUTPUT].getBuffer(0);
		omxBufHdr->nFilledLen = omxBufHdr->nAllocLen;
		omxBufHdr->nOffset = 0;
		omxBufHdr->nOutputPortIndex = 0;
		omxBufHdr->nTimeStamp = mTimeStamp;
		// Buffer time in microseconds
		mTimeStamp += (1000000 * omxBufHdr->nFilledLen) / (m_alsasource_config.sample_rate * m_alsasource_config.nof_channels * m_alsasource_config.nof_bits_per_sample / 8);

		pBufOut = (t_sint16 *)((t_uint32)omxBufHdr->pBuffer + omxBufHdr->nOffset);

		if (mStartTime) {
			omxBufHdr->nFlags |= OMX_BUFFERFLAG_STARTTIME;
			mStartTime = false;
			LOG_I("Setting nFlags = OMX_BUFFERFLAG_STARTTIME\n");
			LOG_I("mStartTime = %s\n", mStartTime ? "true" : "false");
		}
#ifdef MMPROBE_ENABLED
		pPcmProbeCfg = &m_alsasource_config.pcmprobe_cfg[PORT_IDX_OUTPUT];
		if (pPcmProbeCfg->enabled) {
			if (!pcmProbeDataFormatConfigured) {
				configurePcmProbeDataFormat(m_alsasource_config, &pcmProbeDataFormat);
				pcmProbeDataFormatConfigured = true;
			}
			LOG_I("probe pcm data, probe_id=%d, data=%08x, bytes=%d, format=%d,%d,%d,%d\n",
				(int)pPcmProbeCfg->id, (int)pBufOut, (int)omxBufHdr->nFilledLen,
				(int)pcmProbeDataFormat.SampleRate, (int)pcmProbeDataFormat.NoChannels,
				(int)pcmProbeDataFormat.Interleave, (int)pcmProbeDataFormat.Resolution);
			mmprobe_probe_V2(pPcmProbeCfg->id, pBufOut, omxBufHdr->nFilledLen, &pcmProbeDataFormat);
		}
#endif // MMPROBE_ENABLED
		mPort[PORT_IDX_OUTPUT].dequeueAndReturnBuffer();

		// Fire buffer flag event if EOS...
		if (omxBufHdr->nFlags & OMX_BUFFERFLAG_EOS) {
			LOG_I("Buffer marked as EOS.\n");
			proxy.eventHandler(OMX_EventBufferFlag, 1, omxBufHdr->nFlags);
			source.reset(SOURCE_RESET_REASON_EOS);
		}

		mToBeReturned--;
		mSentForProc--;
	}

	// If no buffers on port then return...
	if(mPort[PORT_IDX_OUTPUT].queuedBufferCount() == 0 || mPort[PORT_IDX_OUTPUT].queuedBufferCount() == mSentForProc) {
		LOG_I("\tNo buffers queued. Returning...\n");
		return;
	}

	// Point to out-buffer (do not de-queue until processed)...
	omxBufHdr = mPort[PORT_IDX_OUTPUT].getBuffer(mPort[PORT_IDX_OUTPUT].queuedBufferCount()-1);
	params.buf_hdr = omxBufHdr;
	source.process(&params);
	mSentForProc++;
}

void METH(bufferProcessed)(void)
{
	LOG_I("Processed!.\n");
	mToBeReturned++;
	Component::scheduleProcessEvent();
}

void METH(setParameter)(alsasource_wrapper_config_t config)
{
	LOG_I("Enter.\n");

	// Store input format...
	m_alsasource_config.sample_rate = config.sample_rate;
	m_alsasource_config.nof_channels = config.nof_channels;
	m_alsasource_config.nof_bits_per_sample = config.nof_bits_per_sample;
	m_alsasource_config.endian_little = config.endian_little;
	m_alsasource_config.interleaved = config.interleaved;
	m_alsasource_config.numerical_data_signed = config.numerical_data_signed;

#ifdef MMPROBE_ENABLED
	setPcmProbeCfg(PORT_IDX_OUTPUT, config, &m_alsasource_config);
#endif // MMPROBE_ENABLED

	LOG_I("\tSampleRate: %d\n", (int)m_alsasource_config.sample_rate);
	LOG_I("\tChannels: %d\n", (int)m_alsasource_config.nof_channels);
	LOG_I("\tBits: %d\n", (int)m_alsasource_config.nof_bits_per_sample);
	LOG_I("\tBlockSize: %d\n", (int)m_alsasource_config.block_size);
	LOG_I("\tEndianness: %s-Endian\n", (m_alsasource_config.endian_little) ? "Little" : "Big");
	LOG_I("\tInterleaved: %s\n", (m_alsasource_config.interleaved) ? "Yes" : "No");
	LOG_I("\tNumericalData: %s\n", (m_alsasource_config.numerical_data_signed) ? "Signed" : "Unsigned");
}
