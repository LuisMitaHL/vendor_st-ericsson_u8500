/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#include "Img_EnsWrapper_Shared.h"
#ifdef __linux
	#include <pthread.h>
	#include <signal.h>
	#include <sys/time.h>
#else
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ImgEns_IOmx.h"
#include "BSD_list.h"
#include "osttrace.h"

//Omx definitions
#include <omxil/OMX_Component.h>
#include <omxil/OMX_Types.h>
#include <omxil/OMX_Index.h>

// ENS
#include "ImgEns_Component.h"
// Ens wrapper
#include "Img_EnsWrapper.h"
#include "Img_EnsWrapper_Log.h"
#include "Img_EnsWrapper_services.h"

#define PROC_CPUINFO "/proc/cpuinfo"
#define BUFFER_SIZE 1024

int computeASIC_ID()
{
	char buffer[BUFFER_SIZE];
	FILE * fd = fopen(PROC_CPUINFO, "r");
	int cpu_implementer = -1;
	int cpu_architecture = -1;
	int cpu_variant = -1;
	int cpu_part = -1;

	if (fd == NULL)
	{
		LOGE("Failed to open " PROC_CPUINFO "\n");
		return (-1);
	}
	while (fgets(buffer, BUFFER_SIZE, fd) != '\0')
	{
		/* replace the ':' with ' ' to have "architecture" as token instead of "architecture:" */
		char * tmp = buffer;
		while (*tmp != '\0')
		{
			if (*tmp == ':')
				*tmp = ' ';
			tmp++;
		}
		/* we want to make sure we are on a DB8500 so read all fields */
		sscanf(buffer, "CPU implementer %i", &cpu_implementer);
		sscanf(buffer, "CPU architecture %i", &cpu_architecture);
		sscanf(buffer, "CPU variant %i", &cpu_variant);
		sscanf(buffer, "CPU part %i", &cpu_part);
	}
	fclose(fd);

	if ((cpu_implementer != -1) && (cpu_architecture != -1) && (cpu_variant != -1) && (cpu_part != -1))
	{
		return (cpu_variant);
	}
	return (-1);
}

void dumpOMX_BUFFERHEADERTYPE(OMX_BUFFERHEADERTYPE* pBuffer, char * buffer)
{
	sprintf(buffer, "     + pBuffer              : 0x%08x\n"
		"     + nAllocLen            : 0x%08x\n"
		"     + nFilledLen           : 0x%08x\n"
		"     + nOffset              : 0x%08x\n"
		"     + pAppPrivate          : 0x%08x\n"
		"     + pPlatformPrivate     : 0x%08x\n"
		"     + pInputPortPrivate    : 0x%08x\n"
		"     + pOutputPortPrivate   : 0x%08x\n"
		"     + hMarkTargetComponent : 0x%08x\n"
		"     + pMarkData            : 0x%08x\n"
		"     + nTickCount           : 0x%08x\n"
		"     + nTimeStamp           : 0x%016llx\n"
		"     + nFlags               : 0x%08x\n"
		"     + nOutputPortIndex     : %d\n"
		"     + nInputPortIndex      : %d\n", (unsigned int) pBuffer->pBuffer, (unsigned int) pBuffer->nAllocLen, (unsigned int) pBuffer->nFilledLen,
		(unsigned int) pBuffer->nOffset, (unsigned int) pBuffer->pAppPrivate, (unsigned int) pBuffer->pPlatformPrivate, (unsigned int) pBuffer->pInputPortPrivate,
		(unsigned int) pBuffer->pOutputPortPrivate, (unsigned int) pBuffer->hMarkTargetComponent, (unsigned int) pBuffer->pMarkData, (unsigned int) pBuffer->nTickCount,
		pBuffer->nTimeStamp, (unsigned int) pBuffer->nFlags, (int) pBuffer->nOutputPortIndex, (int) pBuffer->nInputPortIndex);
}

#define OMX_BUFFERDATA_STR_BYTE_FORMAT      "%02x "
#define OMX_BUFFERDATA_STR_BYTE_LENGTH      3
#define OMX_BUFFERDATA_STR_SEPARATOR_FORMAT "[ ... ] "
#define OMX_BUFFERDATA_STR_SEPARATOR_LENGTH 8
#define OMX_BUFFERDATA_NB_END_BYTES         8

#if 0

void dumpOMX_BUFFERDATA(OMX_BUFFERHEADERTYPE* pBuffer, char * header_buffer)
{
	unsigned int i = 0;
	unsigned int n = Img_EnsWrapper::OMX_dump_level;
	OMX_U8* current = NULL;
	char* str = NULL;
	int dump_end = 1;

	if (Img_EnsWrapper::OMX_dump_buffer_size != ((Img_EnsWrapper::OMX_dump_level * OMX_BUFFERDATA_STR_BYTE_LENGTH) + OMX_BUFFERDATA_STR_SEPARATOR_LENGTH))
	{
		if (Img_EnsWrapper::OMX_dump_buffer)
		{
			free(Img_EnsWrapper::OMX_dump_buffer);
		}
		Img_EnsWrapper::OMX_dump_buffer_size = ((Img_EnsWrapper::OMX_dump_level * OMX_BUFFERDATA_STR_BYTE_LENGTH) + OMX_BUFFERDATA_STR_SEPARATOR_LENGTH);
		Img_EnsWrapper::OMX_dump_buffer = (char *) malloc(Img_EnsWrapper::OMX_dump_buffer_size);
	}

	if (pBuffer->nFilledLen == 0)
	{
		LOGW("! 0-size length buffer !");
		return;
	}

	if (pBuffer->nFilledLen <= n)
	{
		n = pBuffer->nFilledLen - 1;
		dump_end = 0;
	}
	else if (pBuffer->nFilledLen <= (n + OMX_BUFFERDATA_NB_END_BYTES))
	{
		n = pBuffer->nFilledLen;
		dump_end = 0;
	}
	else
	{
		n -= OMX_BUFFERDATA_NB_END_BYTES;
		dump_end = 1;
	}

	current = (OMX_U8*) &(pBuffer->pBuffer[pBuffer->nOffset]);
	str = Img_EnsWrapper::OMX_dump_buffer;

	for (i = 0; i < n; i++)
	{
		sprintf(str, OMX_BUFFERDATA_STR_BYTE_FORMAT, (*current));
		current++;
		str += OMX_BUFFERDATA_STR_BYTE_LENGTH;
	}

	if (dump_end)
	{
		sprintf(str, OMX_BUFFERDATA_STR_SEPARATOR_FORMAT);
		str += OMX_BUFFERDATA_STR_SEPARATOR_LENGTH;

		current = (OMX_U8*) &(pBuffer->pBuffer[pBuffer->nFilledLen]) - OMX_BUFFERDATA_NB_END_BYTES;
		for (i = 0; i < OMX_BUFFERDATA_NB_END_BYTES; i++)
		{
			sprintf(str, OMX_BUFFERDATA_STR_BYTE_FORMAT, (*current));
			current++;
			str += OMX_BUFFERDATA_STR_BYTE_LENGTH;
		}
	}

	LOGD("%s     - %s", header_buffer, Img_EnsWrapper::OMX_dump_buffer);
}


void GLOBAL_WRAPPER_CLASS::updateTraceLevel(OMX_COMPONENTTYPE *handle)
{
	{
		GET_PROPERTY(OMX_TRACE_LEVEL_PROPERTY_NAME, value, "0");
		OMX_trace_level = atoi(value);
	}
	{
		GET_PROPERTY(DEBUG_TRACE_ENABLED_PROPERTY_NAME, value, "0");
		DEBUG_trace_enabled = atoi(value);
	}
#ifdef USE_RME
	{
		GET_PROPERTY(RME_TRACE_ENABLED_PROPERTY_NAME, value, "0");
		RME_trace_enabled = atoi(value);
	}
#endif
	{
		GET_PROPERTY(STE_OMX_TRACE_PROPERTY_NAME, value, "0");

		// Update trace output if needed
		OSTTraceSelectOutput();

		// Read the OST trace spec file if any
		processOSTtraceFilteringSpec(atoi(value));
	}
	{
		GET_PROPERTY(OMX_DUMP_LEVEL_PROPERTY_NAME, value, "0");
		OMX_dump_level = atoi(value);
	}
	{
		GET_PROPERTY(OMX_DUMP_OUTPUT_PROPERTY_NAME, value, "0");
		OMX_dump_output = atoi(value);
	}
	{
		GET_PROPERTY(OMX_DUMP_NAME_PROPERTY_NAME, value, "0");
		strncpy(OMX_dump_name, value, OMX_MAX_STRINGNAME_SIZE);
	}
	if (OMX_dump_level)
	{
		LOGI("OMX buffer dump enabled on %s of %s for %d bytes", OMX_dump_output ? "output" : "input", OMX_dump_name, OMX_dump_level);
	}
}

#endif


char * strOMX_Error(OMX_ERRORTYPE error, char * buffer)
{
	switch (error)
	{
	case OMX_ErrorNone:
		return (char *) "OMX_ErrorNone";
	case OMX_ErrorInsufficientResources:
		return (char *) "OMX_ErrorInsufficientResources";
	case OMX_ErrorUndefined:
		return (char *) "OMX_ErrorUndefined";
	case OMX_ErrorInvalidComponentName:
		return (char *) "OMX_ErrorInvalidComponentName";
	case OMX_ErrorComponentNotFound:
		return (char *) "OMX_ErrorComponentNotFound";
	case OMX_ErrorInvalidComponent:
		return (char *) "OMX_ErrorInvalidComponent";
	case OMX_ErrorBadParameter:
		return (char *) "OMX_ErrorBadParameter";
	case OMX_ErrorNotImplemented:
		return (char *) "OMX_ErrorNotImplemented";
	case OMX_ErrorUnderflow:
		return (char *) "OMX_ErrorUnderflow";
	case OMX_ErrorOverflow:
		return (char *) "OMX_ErrorOverflow";
	case OMX_ErrorHardware:
		return (char *) "OMX_ErrorHardware";
	case OMX_ErrorInvalidState:
		return (char *) "OMX_ErrorInvalidState";
	case OMX_ErrorStreamCorrupt:
		return (char *) "OMX_ErrorStreamCorrupt";
	case OMX_ErrorPortsNotCompatible:
		return (char *) "OMX_ErrorPortsNotCompatible";
	case OMX_ErrorResourcesLost:
		return (char *) "OMX_ErrorResourcesLost";
	case OMX_ErrorNoMore:
		return (char *) "OMX_ErrorNoMore";
	case OMX_ErrorVersionMismatch:
		return (char *) "OMX_ErrorVersionMismatch";
	case OMX_ErrorNotReady:
		return (char *) "OMX_ErrorNotReady";
	case OMX_ErrorTimeout:
		return (char *) "OMX_ErrorTimeout";
	case OMX_ErrorSameState:
		return (char *) "OMX_ErrorSameState";
	case OMX_ErrorResourcesPreempted:
		return (char *) "OMX_ErrorResourcesPreempted";
	case OMX_ErrorPortUnresponsiveDuringAllocation:
		return (char *) "OMX_ErrorPortUnresponsiveDuringAllocation";
	case OMX_ErrorPortUnresponsiveDuringDeallocation:
		return (char *) "OMX_ErrorPortUnresponsiveDuringDeallocation";
	case OMX_ErrorPortUnresponsiveDuringStop:
		return (char *) "OMX_ErrorPortUnresponsiveDuringStop";
	case OMX_ErrorIncorrectStateTransition:
		return (char *) "OMX_ErrorIncorrectStateTransition";
	case OMX_ErrorIncorrectStateOperation:
		return (char *) "OMX_ErrorIncorrectStateOperation";
	case OMX_ErrorUnsupportedSetting:
		return (char *) "OMX_ErrorUnsupportedSetting";
	case OMX_ErrorUnsupportedIndex:
		return (char *) "OMX_ErrorUnsupportedIndex";
	case OMX_ErrorBadPortIndex:
		return (char *) "OMX_ErrorBadPortIndex";
	case OMX_ErrorPortUnpopulated:
		return (char *) "OMX_ErrorPortUnpopulated";
	case OMX_ErrorComponentSuspended:
		return (char *) "OMX_ErrorComponentSuspended";
	case OMX_ErrorDynamicResourcesUnavailable:
		return (char *) "OMX_ErrorDynamicResourcesUnavailable";
	case OMX_ErrorMbErrorsInFrame:
		return (char *) "OMX_ErrorMbErrorsInFrame";
	case OMX_ErrorFormatNotDetected:
		return (char *) "OMX_ErrorFormatNotDetected";
	case OMX_ErrorContentPipeOpenFailed:
		return (char *) "OMX_ErrorContentPipeOpenFailed";
	case OMX_ErrorContentPipeCreationFailed:
		return (char *) "OMX_ErrorContentPipeCreationFailed";
	case OMX_ErrorSeperateTablesUsed:
		return (char *) "OMX_ErrorSeperateTablesUsed";
	case OMX_ErrorTunnelingUnsupported:
		return (char *) "OMX_ErrorTunnelingUnsupported";
	case OMX_ErrorKhronosExtensions:
		return (char *) "OMX_ErrorKhronosExtensions";
	case OMX_ErrorVendorStartUnused:
		return (char *) "OMX_ErrorVendorStartUnused";
	case OMX_ErrorMax:
		return (char *) "OMX_ErrorMax";
	default:
		sprintf(buffer, "0x%08x", error);
		return buffer;
	}
}

/********************************************************************************
* Debug trace helper
********************************************************************************/

void displaySendCommand(const char* name, OMX_HANDLETYPE hComponent, OMX_COMMANDTYPE Cmd, OMX_U32 nParam, OMX_PTR /*pCmdData*/)
{
	switch (Cmd)
	{
	case OMX_CommandStateSet:
		switch (nParam)
		{
		case OMX_StateInvalid:
			LOGD("SendCommand %s hComponent=0x%08x : OMX_CommandStateSet to OMX_StateInvalid\n", name, (unsigned int)hComponent);
			break;
		case OMX_StateLoaded:
			LOGD("SendCommand %s hComponent=0x%08x : OMX_CommandStateSet to OMX_StateLoaded\n", name, (unsigned int)hComponent);
			break;
		case OMX_StateIdle:
			LOGD("SendCommand %s hComponent=0x%08x : OMX_CommandStateSet to OMX_StateIdle\n", name, (unsigned int)hComponent);
			break;
		case OMX_StateExecuting:
			LOGD("SendCommand %s hComponent=0x%08x : OMX_CommandStateSet to OMX_StateExecuting\n", name, (unsigned int)hComponent);
			break;
		case OMX_StatePause:
			LOGD("SendCommand %s hComponent=0x%08x : OMX_CommandStateSet to OMX_StatePause\n", name, (unsigned int)hComponent);
			break;
		case OMX_StateWaitForResources:
			LOGD("SendCommand %s hComponent=0x%08x : OMX_CommandStateSet to OMX_StateWaitForResources\n", name, (unsigned int)hComponent);
			break;
		default:
			LOGD("SendCommand %s hComponent=0x%08x ERROR : OMX_CommandStateSet to BAD STATE ID \n", name, (unsigned int)hComponent);
			break;
		}
		break;
	case OMX_CommandFlush:
		LOGD("SendCommand %s hComponent=0x%08x : OMX_CommandFlush - port %d\n", name, (unsigned int)hComponent, (unsigned int)nParam);
		break;
	case OMX_CommandPortDisable:
		LOGD("SendCommand %s hComponent=0x%08x : OMX_CommandPortDisable - port %d\n", name, (unsigned int)hComponent, (unsigned int)nParam);
		break;
	case OMX_CommandPortEnable:
		LOGD("SendCommand %s hComponent=0x%08x : OMX_CommandPortEnable - port %d\n", name, (unsigned int)hComponent, (unsigned int)nParam);
		break;
	case OMX_CommandMarkBuffer:
		LOGD("SendCommand %s hComponent=0x%08x : OMX_CommandMarkBuffer - port %d\n", name, (unsigned int)hComponent, (unsigned int)nParam);
		break;
	default:
		LOGD("SendCommand %s hComponent=0x%08x ERROR : BAD CMD ID", name, (unsigned int)hComponent);
		break;
	}
}
#if 0
void sprintCBMessage(char * buffer, CB_Message * message)
{
	switch (message->type)
	{
	case OMX_EVENTHANDLER:
		switch (message->args.eventHandler.eEvent)
		{
		case OMX_EventCmdComplete:
			switch ((OMX_COMMANDTYPE) message->args.eventHandler.nData1)
			{
			case OMX_CommandStateSet:
				strcpy(buffer, "OMX_EventCmdComplete: OMX_CommandStateSet to ");
				strcatOMX_STATETYPE(buffer, (OMX_STATETYPE) message->args.eventHandler.nData2);
				break;
			case OMX_CommandFlush:
				sprintf(buffer, "OMX_EventCmdComplete: OMX_CommandFlush - port %d", (int) message->args.eventHandler.nData2);
				break;
			case OMX_CommandPortDisable:
				sprintf(buffer, "OMX_EventCmdComplete: OMX_CommandPortDisable - port %d", (int) message->args.eventHandler.nData2);
				break;
			case OMX_CommandPortEnable:
				sprintf(buffer, "OMX_EventCmdComplete: OMX_CommandPortEnable - port %d", (int) message->args.eventHandler.nData2);
				break;
			case OMX_CommandMarkBuffer:
				sprintf(buffer, "OMX_EventCmdComplete: OMX_CommandMarkBuffer - port %d", (int) message->args.eventHandler.nData2);
				break;
			default:
				sprintf(buffer, "OMX_EventCmdComplete: Unknown OMX command reponse:%d", (int) message->args.eventHandler.nData1);
			} /* end switch(OMX_COMMANDTYPE) */
			break;
		case OMX_EventError:
			sprintf(buffer, "OMX_EventError - 0x%08x", (unsigned int) message->args.eventHandler.nData1);
			break;
		case OMX_EventMark:
			sprintf(buffer, "OMX_EventMark");
			break;
		case OMX_EventPortSettingsChanged:
			sprintf(buffer, "OMX_EventPortSettingsChanged - port %d", (int) message->args.eventHandler.nData1);
			break;
		case OMX_EventBufferFlag:
			sprintf(buffer, "OMX_EventBufferFlag - port %d", (int) message->args.eventHandler.nData1);
			break;
		case OMX_EventResourcesAcquired:
			sprintf(buffer, "OMX_EventResourcesAcquired");
			break;
		case OMX_EventDynamicResourcesAvailable:
			sprintf(buffer, "OMX_EventDynamicResourcesAvailable");
			break;
		case eIMGOMX_EventCmdReceived:
			sprintf(buffer, "eIMGOMX_EventCmdReceived");
			break;
		default:
			sprintf(buffer, "unknown OMX event: %d", message->args.eventHandler.eEvent);
			break;
			// case OMX_EventPortFormatDetected:
			// case OMX_EventComponentResumed:
		}
		break;
	case OMX_EMPTYBUFFERDONE:
		{
			OMX_BUFFERHEADERTYPE* pBuffer = message->args.bufferDone.pBuffer;
			sprintf(buffer, "EmptyBufferDone: pBuffer=0x%08x\n", (unsigned int) pBuffer);
		}
		break;
	case OMX_FILLBUFFERDONE:
		{
			OMX_BUFFERHEADERTYPE* pBuffer = message->args.bufferDone.pBuffer;
			sprintf(buffer, "FillBufferDone: pBuffer=0x%08x\n", (unsigned int) pBuffer);
		}
		break;
	case CB_thread_exit:
		sprintf(buffer, "CB_thread_exit");
		break;
	}
}
#endif

void strcatOMX_STATETYPE(char * buffer, OMX_STATETYPE state)
{
	switch (state)
	{
	case OMX_StateInvalid:
		strcat(buffer, "OMX_StateInvalid");
		break;
	case OMX_StateLoaded:
		strcat(buffer, "OMX_StateLoaded");
		break;
	case OMX_StateIdle:
		strcat(buffer, "OMX_StateIdle");
		break;
	case OMX_StateExecuting:
		strcat(buffer, "OMX_StateExecuting");
		break;
	case OMX_StatePause:
		strcat(buffer, "OMX_StatePause");
		break;
	case OMX_StateWaitForResources:
		strcat(buffer, "OMX_StateWaitForResources");
		break;
	case OMX_StateTestForResources:
		strcat(buffer, "OMX_StateTestForResources");
		break;
		//case OMX_StateKhronosExtensions: strcat(buffer, "OMX_StateKhronosExtensions"); break;
		//case OMX_StateVendorStartUnused: strcat(buffer, "OMX_StateVendorStartUnused"); break;
	case OMX_StateMax:
		strcat(buffer, "OMX_StateMax");
		break;
	default:
		{
			char tmp[128];
			sprintf(tmp, "0x%08x", state);
			strcat(buffer, tmp);
		}
		break;
	}
}

void strcatOMX_TUNNELSETUPTYPE(char * buffer, OMX_TUNNELSETUPTYPE* pTunnelSetup)
{
	if (pTunnelSetup)
	{
		char tmp[1024];
		sprintf(tmp, "TunnelSetup.nTunnelFlags=0x%08x TunnelSetup.eSupplier=", (unsigned int) pTunnelSetup->nTunnelFlags);
		strcat(buffer, tmp);
		strcatOMX_BUFFERSUPPLIERTYPE(buffer, pTunnelSetup->eSupplier);
	}
}

void strcatOMX_BUFFERSUPPLIERTYPE(char * buffer, OMX_BUFFERSUPPLIERTYPE eBufferSupplier)
{
	switch (eBufferSupplier)
	{
	case OMX_BufferSupplyUnspecified:
		strcat(buffer, "OMX_BufferSupplyUnspecified");
		break;
	case OMX_BufferSupplyInput:
		strcat(buffer, "OMX_BufferSupplyInput");
		break;
	case OMX_BufferSupplyOutput:
		strcat(buffer, "OMX_BufferSupplyOutput");
		break;
	case OMX_BufferSupplyKhronosExtensions:
		strcat(buffer, "OMX_BufferSupplyKhronosExtensions");
		break;
	case OMX_BufferSupplyVendorStartUnused:
		strcat(buffer, "OMX_BufferSupplyVendorStartUnused");
		break;
	case OMX_BufferSupplyMax:
		strcat(buffer, "OMX_BufferSupplyMax");
		break;
	default:
		{
			char tmp[128];
			sprintf(tmp, "0x%08x", eBufferSupplier);
			strcat(buffer, tmp);
		}
		break;
	}
}


void displayParameterAndConfig(unsigned int index, void * config)
{
	if (index || config)
	{;}
}

#ifdef _MSC_VER

	void OSTTraceInit()
	{
	}

	void OSTTraceSelectOutput()
	{
	}
#endif
