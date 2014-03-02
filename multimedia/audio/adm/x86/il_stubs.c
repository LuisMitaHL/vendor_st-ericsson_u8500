/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include "OMX_Core.h"
#include "OMX_Component.h"
//#include "ste_adm_server.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "AFM_Index.h"
#include "AFM_Types.h"
#include "audio_chipset_api_index.h"
#include "audio_render_chipset_api.h"
#include "OMX_IndexExt.h"
#include "OMX_CoreExt.h"
#include <pthread.h>
#include <errno.h>
#include "ste_adm_platform_adaptions.h"
#include <unistd.h>

#define MAX_PORTS (10)

struct cb_event
{
    int delay_ms;
    OMX_CALLBACKTYPE cb;
    OMX_HANDLETYPE hComponent;
    OMX_PTR pAppData;
    OMX_EVENTTYPE eEvent;
    OMX_U32 nData1;
    OMX_U32 nData2;
    OMX_PTR pEventData;
};

static pthread_mutex_t buf_event_mutex;
#define MAX_BUF_EVENTS 100
struct buf_event
{
    OMX_HANDLETYPE comp;
    OMX_BUFFERHEADERTYPE* bufhdr;
    int is_fill_this_buffer;
    OMX_HANDLETYPE peer;
} buf_events[MAX_BUF_EVENTS];
int num_buf_events=0;

#define INIT_CONFIG_STRUCT(__il_config_struct) \
    do { \
        memset(&__il_config_struct, 0, sizeof(__il_config_struct)); \
        __il_config_struct.nSize             = sizeof(__il_config_struct); \
        __il_config_struct.nVersion.nVersion = OMX_VERSION; \
    } while(0)


typedef struct IL_Test_s
{
    OMX_COMPONENTTYPE comp;
    OMX_CALLBACKTYPE  cb;
    OMX_PTR pAppData;
    OMX_STATETYPE state;
    struct {
        OMX_BOOL state;
        int      bufs;
        OMX_HANDLETYPE buddy_comp;
        OMX_U32           buddy_port;
        int      is_supplier;
    } port[MAX_PORTS];
    char name[80];
    OMX_AUDIO_PARAM_PCMMODETYPE pcm_mode;

    // Info for cscall emulation
    struct {
        int initial_codec_enabled_event_sent;
    } cscall;
    int busy;

    pthread_t event_index_changed_periodically_thread_id;
} IL_Test_t;

static OMX_HANDLETYPE event_index_changed_periodically_thread_killer = 0;
static int num_events_queued = 0;

void* send_callback_thread(void* param)
{

    struct cb_event* e = (struct cb_event*) param;

    if (e->delay_ms > 0) {
        // printf("send_callback_thread: sleeping %d ms before sending event\n", e->delay_ms);
        usleep((useconds_t) (e->delay_ms * 1000));
    }

    if (((IL_Test_t*) e->hComponent)->state == OMX_StateIdle &&
        ((IL_Test_t*) e->hComponent)->event_index_changed_periodically_thread_id != 0)
    {
        event_index_changed_periodically_thread_killer = e->hComponent;
        pthread_join(((IL_Test_t*) e->hComponent)->event_index_changed_periodically_thread_id, NULL);
        ((IL_Test_t*) e->hComponent)->event_index_changed_periodically_thread_id = 0;
    }

    if (e->cb.EventHandler) {
        ((IL_Test_t*) e->hComponent)->busy = 0;
        e->cb.EventHandler(e->hComponent, e->pAppData, e->eEvent, e->nData1,
                             e->nData2, e->pEventData);
    }
    free(param);
    num_events_queued--;
    // pthread_exit(NULL);
    return 0;
}

void event_test(struct cb_event* e, int delay)
{
    if (delay != 0 || rand() % 17 > 8) {
        struct cb_event* e_copy = malloc(sizeof(struct cb_event));
        *e_copy = *e;
        e_copy->delay_ms = delay;
        pthread_t thread;

        int retval;
        pthread_attr_t tattr;
        pthread_attr_init(&tattr);
        pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED);
        num_events_queued++;
        retval = pthread_create(&thread, &tattr, send_callback_thread, (void*) e_copy);
        if (retval != 0) printf("pthread_create returned %d, %s\n", retval, strerror(retval));
    } else {
        if (e->cb.EventHandler) {
            ((IL_Test_t*) e->hComponent)->busy = 0;
            e->cb.EventHandler(e->hComponent, e->pAppData, e->eEvent, e->nData1,
                                 e->nData2, e->pEventData);
        }
    }
}


void* buffer_event_thread(void*);
void* voicecall_on_and_off_thread(void*p);

static pthread_t buffer_event_thread_id;
static pthread_t voicecall_on_and_off_thread_id;
static int voicecall_on_and_off_thread_alive = 0;
static int buffer_event_thread_alive = 1;
OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_Init(void)
{
    pthread_mutex_init(&buf_event_mutex, NULL);
    pthread_create(&buffer_event_thread_id, NULL, buffer_event_thread, NULL);


  return OMX_ErrorNone;
}

OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_Deinit(void)
{
    printf("il_stubs.c - OMX_Deinit\n");
    usleep(1000 * 1000);
    while (num_events_queued > 0) usleep(20 * 1000);

    // pthread_mutex_lock(&buf_event_mutex);

    // voicecall_on_and_off_thread_alive = 0;
    buffer_event_thread_alive = 0;
    // pthread_join(voicecall_on_and_off_thread_id, NULL);
    pthread_join(buffer_event_thread_id, NULL);
    pthread_mutex_destroy(&buf_event_mutex);

    return OMX_ErrorNone;
}

void* event_index_changed_periodically_thread(void*p)
{
    struct cb_event* e = (struct cb_event*) p;
    while(1) {
        int a;
        for (a=0 ; a < MAX_PORTS ; a++) {
            if (e->hComponent == event_index_changed_periodically_thread_killer) {
                event_index_changed_periodically_thread_killer = 0;
                free(e);
                return 0;
            }
            usleep(50 * 1000);
        }
        event_test(e, 1);
    }

    return 0;
}
static OMX_HANDLETYPE g_ilstub_cscall = NULL;


void* voicecall_on_and_off_thread(void*p)
{
    (void) p;
    int a;
    // Keep turning the modem on and off
    while(voicecall_on_and_off_thread_alive) {
        if (g_ilstub_cscall) {
            struct cb_event e;
            e.cb = ((IL_Test_t*) g_ilstub_cscall)->cb;
            e.hComponent = g_ilstub_cscall;
            e.pAppData = ((IL_Test_t*) g_ilstub_cscall)->pAppData;
            e.eEvent = OMX_EventIndexSettingChanged;
            e.nData1 = OMX_ALL;
            e.nData2 = OMX_Symbian_IndexConfigAudioCodecEnabled;
            e.pEventData = 0;
            event_test(&e, rand() % 701);

            e.cb = ((IL_Test_t*) g_ilstub_cscall)->cb;
            e.hComponent = g_ilstub_cscall;
            e.pAppData = ((IL_Test_t*) g_ilstub_cscall)->pAppData;
            e.eEvent = OMX_EventIndexSettingChanged;
            e.nData1 = OMX_ALL;
            e.nData2 = rand()%17 > 9 ? OMX_Symbian_IndexConfigAudioSampleRate8khz : OMX_Symbian_IndexConfigAudioSampleRate16khz;
            e.pEventData = 0;
            event_test(&e, rand() % 701);

            for (a=0; voicecall_on_and_off_thread_alive && a < 1000; a++) {
                usleep(100 * 1000);
            }

            e.cb = ((IL_Test_t*) g_ilstub_cscall)->cb;
            e.hComponent = g_ilstub_cscall;
            e.pAppData = ((IL_Test_t*) g_ilstub_cscall)->pAppData;
            e.eEvent = OMX_EventIndexSettingChanged;
            e.nData1 = OMX_ALL;
            e.nData2 = OMX_Symbian_IndexConfigAudioCodecDisabled;
            e.pEventData = 0;
            event_test(&e, 0);
        }

        for (a=0; voicecall_on_and_off_thread_alive && a < 30; a++) {
            usleep(100 * 1000);
        }
    }

    return NULL;
}

static int is_input(OMX_HANDLETYPE hComponent, OMX_U32 portidx)
{
    OMX_PARAM_PORTDEFINITIONTYPE pdef;
    pdef.nPortIndex = portidx;
    OMX_ERRORTYPE res = OMX_GetParameter(hComponent, OMX_IndexParamPortDefinition, &pdef);
    assert(res == OMX_ErrorNone);
    return pdef.eDir == OMX_DirInput;
}

static int is_stub(OMX_HANDLETYPE hComponent)
{
    OMX_INDEXTYPE dummy;
    return OMX_GetExtensionIndex(hComponent, "STUB", &dummy) == OMX_ErrorNone;
}

static int is_comp(OMX_HANDLETYPE hComponent, const char* name)
{
    if (is_stub(hComponent)) {
        return strcmp(((IL_Test_t*) hComponent)->name, name) == 0;
    }
    return 0;
}


static OMX_ERRORTYPE SetParameter(
    OMX_IN  OMX_HANDLETYPE hComponent,
    OMX_IN  OMX_INDEXTYPE nIndex,
    OMX_IN  OMX_PTR pComponentParameterStructure)
{
  if (nIndex == OMX_IndexParamAudioPcm) {
    memcpy(&((IL_Test_t*) hComponent)->pcm_mode, pComponentParameterStructure, sizeof(OMX_AUDIO_PARAM_PCMMODETYPE));
    assert( ((OMX_AUDIO_PARAM_PCMMODETYPE*) pComponentParameterStructure)->nSize ==sizeof(OMX_AUDIO_PARAM_PCMMODETYPE));
  }

  return OMX_ErrorNone;
}

static OMX_ERRORTYPE SetConfig(
    OMX_IN  OMX_HANDLETYPE hComponent,
    OMX_IN  OMX_INDEXTYPE nIndex,
    OMX_IN  OMX_PTR pComponentConfigStructure)
{
  (void) hComponent;
  (void) nIndex;
  (void) pComponentConfigStructure;

  if (nIndex == OMX_IndexConfigAudioEqualizer)
  {
      OMX_AUDIO_CONFIG_EQUALIZERTYPE *eq = (OMX_AUDIO_CONFIG_EQUALIZERTYPE*) pComponentConfigStructure;
      printf ("SetConfig: OMX_IndexConfigAudioEqualizer sBandIndex=%u, sBandLevel=%u\n", (unsigned int)eq->sBandIndex.nValue, (unsigned int)eq->sBandLevel.nValue);
  }

  return OMX_ErrorNone;
}

static OMX_ERRORTYPE SendCommand(
        OMX_IN  OMX_HANDLETYPE hComponent,
        OMX_IN  OMX_COMMANDTYPE Cmd,
        OMX_IN  OMX_U32 nParam1,
        OMX_IN  OMX_PTR pCmdData)
{
    (void) pCmdData;


    if (Cmd == OMX_CommandStateSet &&
        is_comp(hComponent, "OMX.ST.AFM.pcmprocessing.spectrum_analyzer")) {
        if (((IL_Test_t*) hComponent)->state == OMX_StateIdle && nParam1 == OMX_StateExecuting) {
            struct cb_event *e = malloc(sizeof(struct cb_event));
            e->cb = ((IL_Test_t*) g_ilstub_cscall)->cb;
            e->hComponent = hComponent;
            e->pAppData = ((IL_Test_t*) g_ilstub_cscall)->pAppData;
            e->eEvent = OMX_EventIndexSettingChanged;
            e->nData1 = OMX_ALL; // TODO?
            e->nData2 = 0;       // ??
            e->pEventData = 0;
            pthread_create(&((IL_Test_t*) hComponent)->event_index_changed_periodically_thread_id,
                           0,
                           event_index_changed_periodically_thread, e);
        }
    }


    assert(!((IL_Test_t*) hComponent)->busy);
    if (Cmd == OMX_CommandStateSet) {
        assert( ((((IL_Test_t*) hComponent)->state == OMX_StateLoaded)    && nParam1 == OMX_StateIdle) ||
                ((((IL_Test_t*) hComponent)->state == OMX_StateIdle)      && nParam1 == OMX_StateLoaded) ||
                ((((IL_Test_t*) hComponent)->state == OMX_StateIdle)      && nParam1 == OMX_StateExecuting) ||
                ((((IL_Test_t*) hComponent)->state == OMX_StateExecuting) && nParam1 == OMX_StateIdle));

        ((IL_Test_t*) hComponent)->state = nParam1;
        ((IL_Test_t*) hComponent)->busy    = 1;

        if (nParam1 == OMX_StateLoaded) {
            int i;
            for (i=0 ; i < MAX_PORTS ; i++) {
                ((IL_Test_t*) hComponent)->port[i].buddy_comp = 0;
            }
        }
    }


    if (Cmd == OMX_CommandPortDisable) {
        IL_Test_t* cur = (IL_Test_t*) hComponent;

        assert(cur->port[nParam1].state);

        if (cur->port[nParam1].buddy_comp && is_stub(cur->port[nParam1].buddy_comp)) {
            IL_Test_t* buddy = ((IL_Test_t*) hComponent)->port[nParam1].buddy_comp;
            OMX_U32    buddy_port = ((IL_Test_t*) hComponent)->port[nParam1].buddy_port;

            if (buddy) {
                assert(buddy->state == cur->state);
                buddy->port[buddy_port].buddy_comp = 0;
            }
        }

        cur->port[nParam1].state = 0;
        cur->port[nParam1].buddy_comp  = 0;
        cur->busy    = 1;
    }

    if (Cmd == OMX_CommandPortEnable) {
        assert(!((IL_Test_t*) hComponent)->port[nParam1].state);

        if (((IL_Test_t*) hComponent)->port[nParam1].buddy_comp) {
            IL_Test_t* cur  = ((IL_Test_t*) hComponent);
            if (cur->port[nParam1].buddy_comp && is_stub(cur->port[nParam1].buddy_comp)) {
                IL_Test_t* buddy = ((IL_Test_t*) hComponent)->port[nParam1].buddy_comp;
                // OMX_U32    buddy_port = ((IL_Test_t*) hComponent)->port[nParam1].buddy_port;

                if (cur->state == OMX_StateLoaded) {
                    // Do not first SetupTunnel and then enable the port in
                    // Loaded state.
                    assert(0);
                } else {
                    // cur is in Idle or Executing
                    assert(buddy->state == OMX_StateIdle || buddy->state == OMX_StateExecuting);
                    // Other port is enabled or disabled, not tracked by il stubs.c currently
                }
            }
        }

        ((IL_Test_t*) hComponent)->port[nParam1].state = 1;
        ((IL_Test_t*) hComponent)->busy    = 1;
    }

    {
        struct cb_event e;
        e.cb = ((IL_Test_t*) hComponent)->cb;
        e.hComponent = hComponent;
        e.pAppData = ((IL_Test_t*) hComponent)->pAppData;
        e.eEvent = OMX_EventCmdComplete;
        e.nData1 = Cmd;
        e.nData2 = nParam1;
        e.pEventData = 0;
        event_test(&e, 0);
    }

    if (is_comp(hComponent,"OMX.ST.AFM.speech_proc")) {
      if (Cmd == OMX_CommandPortEnable && nParam1 == ADM_SPEECHPROC_DL_IN_PORT) {
             // Send an EOS event directly
             // TODO: Test dalaying this as well

             // Don't send it each time DL_OUT is enabled - that
             // is done even if the voicecall is re-routed.
             struct cb_event e;
             e.cb = ((IL_Test_t*) hComponent)->cb;
             e.hComponent = hComponent;
             e.pAppData = ((IL_Test_t*) hComponent)->pAppData;
             e.eEvent = OMX_EventBufferFlag;
             e.nData1 = ADM_SPEECHPROC_DL_OUT_PORT;
             e.nData2 = OMX_BUFFERFLAG_EOS;
             e.pEventData = 0;
             if (rand() % 17 > 7) {
                 // printf("EOS - NO DELAY\n");
                 event_test(&e, 0);
             } else {
                 // printf("EOS - DELAY\n");
                 event_test(&e, 1500);
             }
      }
  }


  // Have the cscall send the OMX_Symbian_IndexConfigAudioCodecEnabled and
  // OMX_Symbian_IndexConfigAudioCodecDisabled events to generate the proper
  // stimuli for the cascall code.
  if (is_comp(hComponent,"OMX.ST.AFM.cscall")) {
      IL_Test_t* testcomp = (IL_Test_t*) hComponent;
      if (!testcomp->cscall.initial_codec_enabled_event_sent &&
          Cmd == OMX_CommandStateSet &&
          nParam1 == OMX_StateExecuting)
      {
          assert(voicecall_on_and_off_thread_alive == 0);
          voicecall_on_and_off_thread_alive = 1;
          pthread_create(&voicecall_on_and_off_thread_id, NULL, voicecall_on_and_off_thread, NULL);
          testcomp->cscall.initial_codec_enabled_event_sent = 1;
          g_ilstub_cscall = hComponent;
      }
  }

  return OMX_ErrorNone;
}


void dequeue_buf_event()
{
    pthread_mutex_lock(&buf_event_mutex);
    if (num_buf_events > 0) {
      OMX_HANDLETYPE comp = buf_events[0].comp;
      int is_fill_this_buffer = buf_events[0].is_fill_this_buffer;
      OMX_HANDLETYPE peer = buf_events[0].peer;
      OMX_BUFFERHEADERTYPE* bufhdr = buf_events[0].bufhdr;
      memmove(buf_events, buf_events+1, (MAX_BUF_EVENTS-1) * sizeof(buf_events[0]));
      num_buf_events--;
      pthread_mutex_unlock(&buf_event_mutex);
      if (is_fill_this_buffer)
          if (peer)
              OMX_EmptyThisBuffer(peer, bufhdr);
          else
            ((IL_Test_t*) comp)->cb.FillBufferDone(comp, ((IL_Test_t*) comp)->pAppData, bufhdr);
      else
          if (peer)
              OMX_FillThisBuffer(peer, bufhdr);
          else
            ((IL_Test_t*) comp)->cb.EmptyBufferDone(comp, ((IL_Test_t*) comp)->pAppData, bufhdr);
      pthread_mutex_lock(&buf_event_mutex);
    }

    pthread_mutex_unlock(&buf_event_mutex);
}

void* buffer_event_thread(void*a)
{
    (void) a;
    while(buffer_event_thread_alive) {
        usleep(16 * 1000);
        dequeue_buf_event();
    }

  return NULL;
}

void queue_buf_event(OMX_HANDLETYPE comp, OMX_BUFFERHEADERTYPE* bufhdr, int is_fill_this_buffer)
{
    pthread_mutex_lock(&buf_event_mutex);
    assert(num_buf_events < MAX_BUF_EVENTS);
    buf_events[num_buf_events].comp = comp;
    buf_events[num_buf_events].bufhdr= bufhdr;
    buf_events[num_buf_events].is_fill_this_buffer = is_fill_this_buffer;

    if (is_fill_this_buffer) {
        buf_events[num_buf_events].peer = ((IL_Test_t*) comp)->port[bufhdr->nOutputPortIndex].buddy_comp;
    } else {
        buf_events[num_buf_events].peer = ((IL_Test_t*) comp)->port[bufhdr->nInputPortIndex].buddy_comp;
    }

    num_buf_events++;
    pthread_mutex_unlock(&buf_event_mutex);
}

static OMX_ERRORTYPE EmptyThisBuffer(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer)
{
    queue_buf_event(hComponent, pBuffer, 0);
    return OMX_ErrorNone;
}

static  OMX_ERRORTYPE FillThisBuffer(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer)
{
    if (pBuffer->nAllocLen >= 8)
        memcpy(pBuffer->pBuffer, "TestAABB", 8);

    pBuffer->nFilledLen = pBuffer->nAllocLen;
    queue_buf_event(hComponent, pBuffer, 1);
    // ((IL_Test_t*) hComponent)->cb.FillBufferDone(hComponent, ((IL_Test_t*) hComponent)->pAppData, pBuffer);
    return OMX_ErrorNone;
}


static int comp_is_in_array(OMX_HANDLETYPE hComponent, const char** array)
{
    while(*array) {
        if (is_comp(hComponent, *array)) return 1;
        array++;
    }

    return 0;
}

const char* sinks_with_ref_port[] = {
        "OMX.ST.AFM.IHF", "OMX.ST.AFM.NOKIA_AV_SINK", "OMX.ST.AFM.EARPIECE", NULL };

const char* sinks_without_ref_port[] = {
        "OMX.ST.AFM.FM_SINK", "OMX.ST.AFM.BT_MONO_SINK",
        "OMX.ST.AFM.alsasink", "OMX.ST.HAPTICS_RENDERER.PCM.VIBL",
        "OMX.ST.HAPTICS_RENDERER.PCM.VIBR", NULL };

const char* sources[] = {
    "OMX.ST.AFM.NOKIA_AV_SOURCE", "OMX.ST.AFM.MULTI_MIC",
    "OMX.ST.AFM.FM_SOURCE",  "OMX.ST.AFM.BT_MONO_SOURCE", NULL };



static OMX_ERRORTYPE GetParameter(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_INDEXTYPE nParamIndex,
            OMX_INOUT OMX_PTR pComponentParameterStructure)
{
  // *((unsigned long*) pComponentParameterStructure) = 8; // size, just to test

  if (nParamIndex == (OMX_INDEXTYPE) OMX_IndexParamPortDefinition) {
    OMX_U32 port_index = ((OMX_PARAM_PORTDEFINITIONTYPE*) pComponentParameterStructure)->nPortIndex;
    memset( ((char*) pComponentParameterStructure)+8, 0, sizeof(OMX_PARAM_PORTDEFINITIONTYPE)-8);
    ((OMX_PARAM_PORTDEFINITIONTYPE*) pComponentParameterStructure)->nBufferCountMin = 2;
    ((OMX_PARAM_PORTDEFINITIONTYPE*) pComponentParameterStructure)->nBufferCountActual = ((OMX_PARAM_PORTDEFINITIONTYPE*) pComponentParameterStructure)->nBufferCountMin;
    ((OMX_PARAM_PORTDEFINITIONTYPE*) pComponentParameterStructure)->nBufferSize = 4096;
    ((OMX_PARAM_PORTDEFINITIONTYPE*) pComponentParameterStructure)->bEnabled = ((IL_Test_t*) hComponent)->port[port_index].state;
    ((OMX_PARAM_PORTDEFINITIONTYPE*) pComponentParameterStructure)->nPortIndex = port_index;
    ((OMX_PARAM_PORTDEFINITIONTYPE*) pComponentParameterStructure)->format.audio.eEncoding = OMX_AUDIO_CodingPCM;
    if (is_comp(hComponent, "OMX.ST.AFM.mixer") ||
        is_comp(hComponent,"OMX.ST.AFM.mixer_host"))
    {
        ((OMX_PARAM_PORTDEFINITIONTYPE*) pComponentParameterStructure)->eDir = (port_index == 0) ? OMX_DirOutput : OMX_DirInput;
    }
    if (is_comp(hComponent, "OMX.ST.AFM.pcmsplitter")  ||
        is_comp(hComponent, "OMX.ST.AFM.pcmsplitter_host"))
    {
        ((OMX_PARAM_PORTDEFINITIONTYPE*) pComponentParameterStructure)->eDir = (port_index == 0) ? OMX_DirInput : OMX_DirOutput ;
    }

    if (port_index == 0)
    {
        if (comp_is_in_array(hComponent, sinks_without_ref_port) ||
            comp_is_in_array(hComponent, sinks_with_ref_port))
        {
            ((OMX_PARAM_PORTDEFINITIONTYPE*) pComponentParameterStructure)->eDir = OMX_DirInput;
        }
        if (comp_is_in_array(hComponent, sources))
        {
            ((OMX_PARAM_PORTDEFINITIONTYPE*) pComponentParameterStructure)->eDir = OMX_DirOutput;
        }
    }

    if (port_index == 1)
    {
        if (comp_is_in_array(hComponent, sinks_with_ref_port))
        {
            ((OMX_PARAM_PORTDEFINITIONTYPE*) pComponentParameterStructure)->eDir = OMX_DirOutput;
        }
    }



  }
  if (nParamIndex == (OMX_INDEXTYPE) OMX_IndexParamAudioPcm) {
    OMX_U32 port_index = ((OMX_AUDIO_PARAM_PCMMODETYPE*) pComponentParameterStructure)->nPortIndex;
    memcpy(pComponentParameterStructure, &((IL_Test_t*) hComponent)->pcm_mode, sizeof(OMX_AUDIO_PARAM_PCMMODETYPE));

    ((OMX_AUDIO_PARAM_PCMMODETYPE*) pComponentParameterStructure)->nPortIndex = port_index;
  }
  if (nParamIndex == (OMX_INDEXTYPE) OMX_IndexParamAudioSynchronized) {
    // memset to 0 to not give false positive in Valgrind
    memset(((char*) pComponentParameterStructure)+8, 0, sizeof(OMX_AUDIO_PARAM_SYNCHRONIZEDTYPE)-8);
    ((OMX_AUDIO_PARAM_SYNCHRONIZEDTYPE*)pComponentParameterStructure)->nSize = sizeof(OMX_AUDIO_PARAM_SYNCHRONIZEDTYPE);
  }

    if (nParamIndex == (OMX_INDEXTYPE) OMX_IndexParamAudioInit) {
        if (is_comp(hComponent, "OMX.ST.AFM.mixer") ||
            is_comp(hComponent, "OMX.ST.AFM.mixer_host") ||
            is_comp(hComponent, "OMX.ST.AFM.pcmsplitter") ||
            is_comp(hComponent, "OMX.ST.AFM.pcmsplitter_host"))
        {
            ((OMX_PORT_PARAM_TYPE*) pComponentParameterStructure)->nStartPortNumber = 0;
            ((OMX_PORT_PARAM_TYPE*) pComponentParameterStructure)->nPorts = 9;
        }

        if (comp_is_in_array(hComponent, sinks_with_ref_port)) {
            ((OMX_PORT_PARAM_TYPE*) pComponentParameterStructure)->nStartPortNumber = 0;
            ((OMX_PORT_PARAM_TYPE*) pComponentParameterStructure)->nPorts = 2;
        }

        if (comp_is_in_array(hComponent, sinks_without_ref_port)) {
            ((OMX_PORT_PARAM_TYPE*) pComponentParameterStructure)->nStartPortNumber = 0;
            ((OMX_PORT_PARAM_TYPE*) pComponentParameterStructure)->nPorts = 1;
        }

        if (comp_is_in_array(hComponent, sources)) {
            ((OMX_PORT_PARAM_TYPE*) pComponentParameterStructure)->nStartPortNumber = 0;
            ((OMX_PORT_PARAM_TYPE*) pComponentParameterStructure)->nPorts = 1;
        }
        if (is_comp(hComponent, "OMX.ST.AFM.cscall")) {
            ((OMX_PORT_PARAM_TYPE*) pComponentParameterStructure)->nStartPortNumber = 0;
            ((OMX_PORT_PARAM_TYPE*) pComponentParameterStructure)->nPorts = 2;
        }
    }




    if (nParamIndex == OMX_IndexParamVideoInit ||nParamIndex == OMX_IndexParamImageInit)
    {
        ((OMX_PORT_PARAM_TYPE*) pComponentParameterStructure)->nStartPortNumber = 0;
        ((OMX_PORT_PARAM_TYPE*) pComponentParameterStructure)->nPorts = 0;
    }

    if (nParamIndex == OMX_IndexParamOtherInit)
    {
        ((OMX_PORT_PARAM_TYPE*) pComponentParameterStructure)->nStartPortNumber = 0;
        ((OMX_PORT_PARAM_TYPE*) pComponentParameterStructure)->nPorts = 0;
    }


  return OMX_ErrorNone;
}

static OMX_ERRORTYPE GetConfig(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_INDEXTYPE nIndex,
            OMX_INOUT OMX_PTR pComponentConfigStructure)
{
  (void) hComponent;
  if (nIndex == AFM_IndexParamMspNbChannel) {
    // memset to 0 to not give false positive in Valgrind
    memset(((char*)pComponentConfigStructure)+8, 0, sizeof(AFM_AUDIO_PARAM_MSP_NB_CHANNEL)-8);
  }

  if (nIndex == OMX_IndexConfigAudioVolume) {
    OMX_INDEXTYPE nPortIndex = ((OMX_AUDIO_CONFIG_VOLUMETYPE*) pComponentConfigStructure)->nPortIndex;
    memset(((char*)pComponentConfigStructure)+8, 0, sizeof(OMX_AUDIO_CONFIG_VOLUMETYPE)-8);
    ((OMX_AUDIO_CONFIG_VOLUMETYPE*) pComponentConfigStructure)->nPortIndex = nPortIndex;
    ((OMX_AUDIO_CONFIG_VOLUMETYPE*) pComponentConfigStructure)->bLinear = OMX_FALSE;
    ((OMX_AUDIO_CONFIG_VOLUMETYPE*) pComponentConfigStructure)->sVolume.nMin = -50000;
    ((OMX_AUDIO_CONFIG_VOLUMETYPE*) pComponentConfigStructure)->sVolume.nMax = 0;
    ((OMX_AUDIO_CONFIG_VOLUMETYPE*) pComponentConfigStructure)->sVolume.nValue = 0;
  }

  return OMX_ErrorNone;
}

static OMX_ERRORTYPE GetExtensionIndex(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_STRING cParameterName,
            OMX_OUT OMX_INDEXTYPE* pIndexType)
{
    assert(hComponent != NULL);
    assert(pIndexType != NULL);
    assert(cParameterName != NULL);

    if (strcmp(cParameterName, "STUB") == 0) {
        return OMX_ErrorNone;
    }

    if (strcmp(cParameterName, "AFM_IndexParamSpl") == 0) {
        *pIndexType = AFM_IndexParamSpl;
        return OMX_ErrorNone;
    }
    if (strcmp(cParameterName, "OMX_IndexConfigSpllimit") == 0) {
        *pIndexType = OMX_IndexConfigSpllimit;
        return OMX_ErrorNone;
    }

    return OMX_ErrorNotImplemented;
}

static OMX_ERRORTYPE AllocateBuffer(
            OMX_IN OMX_HANDLETYPE hComponent,
            OMX_INOUT OMX_BUFFERHEADERTYPE** ppBuffer,
            OMX_IN OMX_U32 nPortIndex,
            OMX_IN OMX_PTR pAppPrivate,
            OMX_IN OMX_U32 nSizeBytes)
{
  (void) hComponent;
  (void) nPortIndex;

  *ppBuffer = malloc(sizeof(OMX_BUFFERHEADERTYPE));
  memset(*ppBuffer, 0, sizeof(OMX_BUFFERHEADERTYPE));

  (*ppBuffer)->pBuffer = malloc(nSizeBytes);
  memset((*ppBuffer)->pBuffer, 0, nSizeBytes); // Avoid Valgrind false positive
  (*ppBuffer)->pAppPrivate = pAppPrivate;
  (*ppBuffer)->nAllocLen = nSizeBytes;
  (*ppBuffer)->nInputPortIndex = nPortIndex;
  (*ppBuffer)->nOutputPortIndex = nPortIndex;
  (*ppBuffer)->pPlatformPrivate = (void*) 1;


  // ((IL_Test_t*) hComponent)->port_buf[((IL_Test_t*) hComponent)->port_bufs++] = *ppBuffer;

  return OMX_ErrorNone;
}

static OMX_ERRORTYPE FreeBuffer(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_U32 nPortIndex,
            OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer)
{
    (void) hComponent;
    (void) nPortIndex;

    if (pBuffer->pPlatformPrivate && pBuffer->pBuffer) free(pBuffer->pBuffer);
    free(pBuffer);
    return OMX_ErrorNone;
}

static OMX_ERRORTYPE UseBuffer(
            OMX_IN OMX_HANDLETYPE hComponent,
            OMX_INOUT OMX_BUFFERHEADERTYPE** ppBuffer,
            OMX_IN OMX_U32 nPortIndex,
            OMX_IN OMX_PTR pAppPrivate,
            OMX_IN OMX_U32 nSizeBytes,
            OMX_IN OMX_U8* pBuffer)
{
    (void) hComponent;
  (void) nPortIndex;

  *ppBuffer = malloc(sizeof(OMX_BUFFERHEADERTYPE));
  memset(*ppBuffer, 0, sizeof(OMX_BUFFERHEADERTYPE));

  (*ppBuffer)->pBuffer = pBuffer;
  memset((*ppBuffer)->pBuffer, 0, nSizeBytes); // Avoid Valgrind false positive
  (*ppBuffer)->pAppPrivate = pAppPrivate;
  (*ppBuffer)->nAllocLen = nSizeBytes;
  (*ppBuffer)->nInputPortIndex = nPortIndex;
  (*ppBuffer)->nOutputPortIndex = nPortIndex;
  (*ppBuffer)->pPlatformPrivate = 0;

    return OMX_ErrorNone;
}

static    OMX_ERRORTYPE GetState(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_OUT OMX_STATETYPE* pState)
{
  assert(hComponent && pState);
  *pState = ((IL_Test_t*) hComponent)->state;
  return OMX_ErrorNone;
}


static OMX_ERRORTYPE GetComponentVersion(
        OMX_IN OMX_HANDLETYPE hComponent,
        OMX_OUT OMX_STRING pComponentName,
        OMX_OUT OMX_VERSIONTYPE* pComponentVersion,
        OMX_OUT OMX_VERSIONTYPE* pSpecVersion,
        OMX_OUT OMX_UUIDTYPE* pComponentUUID)
{
    strcpy(pComponentName, ((IL_Test_t*) hComponent)->name);
    (void) pComponentVersion;
    (void) pSpecVersion;
    (void) pComponentUUID;

    return OMX_ErrorNone;
}

static OMX_ERRORTYPE ComponentTunnelRequest(
        OMX_HANDLETYPE hComp, OMX_U32 nPort, OMX_HANDLETYPE hTunneledComp,
        OMX_U32 nTunneledPort, OMX_TUNNELSETUPTYPE* pTunnelSetup)
{
    OMX_ERRORTYPE res = OMX_ErrorNone;
    int set_supplier = 0;
    OMX_PARAM_BUFFERSUPPLIERTYPE bufsup;
    INIT_CONFIG_STRUCT(bufsup);
    bufsup.eBufferSupplier = OMX_BufferSupplyOutput;

    OMX_INDEXTYPE dummy;

    int we_are_input = is_input(hComp, nPort);

    if (hTunneledComp == NULL) {
        ((IL_Test_t*) hComp)->port[nPort].buddy_comp = 0;
    } else if (we_are_input) {
        /*
        1.  Query the necessary parameters from the output port to
            determine if the ports are compatible for tunneling
        2.  If the ports are compatible, the component should store
            the tunnel step provided by the output port
        3.  Determine which port (either input or output) is the buffer
            supplier, and call OMX_SetParameter on the output port to
            indicate this selection.
        */
        set_supplier = 1; // Call SetParameter when not holding main mutex
        ((IL_Test_t*) hComp)->port[nPort].buddy_comp = hTunneledComp;
        ((IL_Test_t*) hComp)->port[nPort].buddy_port = nTunneledPort;
        ((IL_Test_t*) hComp)->port[nPort].is_supplier = (bufsup.eBufferSupplier == OMX_BufferSupplyInput);
    } else {
        // we are output port
        memset(pTunnelSetup, 0, sizeof(*pTunnelSetup));
        pTunnelSetup->eSupplier = OMX_BufferSupplyInput;
        ((IL_Test_t*) hComp)->port[nPort].buddy_comp = hTunneledComp;
        ((IL_Test_t*) hComp)->port[nPort].buddy_port = nTunneledPort;
        ((IL_Test_t*) hComp)->port[nPort].is_supplier = (pTunnelSetup->eSupplier == OMX_BufferSupplyOutput);
    }

    if (set_supplier) {
        bufsup.nPortIndex = nTunneledPort;
        res = OMX_SetParameter(hTunneledComp, OMX_IndexParamCompBufferSupplier, &bufsup);
        if (res != OMX_ErrorNone) {
            printf("ADM IL: OMX_SetParameter %X.%d failed during ComponentTunnelRequest\n", (int) hTunneledComp, (int) nTunneledPort);
        }
    }

    return res;
}




OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_GetHandle(
    OMX_OUT OMX_HANDLETYPE* pHandle,
    OMX_IN  OMX_STRING cComponentName,
    OMX_IN  OMX_PTR pAppData,
    OMX_IN  OMX_CALLBACKTYPE* pCallBacks)
{
  *pHandle = malloc(sizeof(IL_Test_t));
  memset(*pHandle, 0, sizeof(IL_Test_t));

  ((OMX_COMPONENTTYPE*) *pHandle)->SetParameter   = SetParameter;
  ((OMX_COMPONENTTYPE*) *pHandle)->SetConfig      = SetConfig;
  ((OMX_COMPONENTTYPE*) *pHandle)->SendCommand    = SendCommand;
  ((OMX_COMPONENTTYPE*) *pHandle)->GetParameter   = GetParameter;
  ((OMX_COMPONENTTYPE*) *pHandle)->GetConfig      = GetConfig;
  ((OMX_COMPONENTTYPE*) *pHandle)->FreeBuffer     = FreeBuffer;
  ((OMX_COMPONENTTYPE*) *pHandle)->AllocateBuffer = AllocateBuffer;
  ((OMX_COMPONENTTYPE*) *pHandle)->UseBuffer      = UseBuffer;
  ((OMX_COMPONENTTYPE*) *pHandle)->EmptyThisBuffer = EmptyThisBuffer;
  ((OMX_COMPONENTTYPE*) *pHandle)->FillThisBuffer = FillThisBuffer;
  ((OMX_COMPONENTTYPE*) *pHandle)->GetState       = GetState;
  ((OMX_COMPONENTTYPE*) *pHandle)->GetExtensionIndex = GetExtensionIndex;
  ((OMX_COMPONENTTYPE*) *pHandle)->pApplicationPrivate = pAppData;
  ((OMX_COMPONENTTYPE*) *pHandle)->GetComponentVersion = GetComponentVersion;
  ((OMX_COMPONENTTYPE*) *pHandle)->ComponentTunnelRequest = ComponentTunnelRequest;
  ((IL_Test_t*) *pHandle)->cb                     = *pCallBacks;
  ((IL_Test_t*) *pHandle)->pAppData               = pAppData;
  ((IL_Test_t*) *pHandle)->state                  = OMX_StateLoaded;
  ((IL_Test_t*) *pHandle)->cscall.initial_codec_enabled_event_sent = 0;
  ((IL_Test_t*) *pHandle)->busy                   = 0;

  strcpy(((IL_Test_t*) *pHandle)->name, cComponentName);

  int i;
  for (i=0 ; i < MAX_PORTS ; i++) {
      ((IL_Test_t*) *pHandle)->port[i].state       = 1; // ENABLED
      ((IL_Test_t*) *pHandle)->port[i].buddy_comp  = 0;
  }

  memset(&((IL_Test_t*) *pHandle)->pcm_mode, 0, sizeof(((IL_Test_t*) *pHandle)->pcm_mode));
  ((IL_Test_t*) *pHandle)->pcm_mode.nSize = sizeof(((IL_Test_t*) *pHandle)->pcm_mode);
  ((IL_Test_t*) *pHandle)->pcm_mode.nVersion.nVersion = OMX_VERSION;
  return OMX_ErrorNone;
}

OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_FreeHandle(
  OMX_IN  OMX_HANDLETYPE hComponent)
{
    if (is_comp(hComponent, "OMX.ST.AFM.cscall")) {
        voicecall_on_and_off_thread_alive = 0;
        pthread_join(voicecall_on_and_off_thread_id, NULL);
        usleep(1000 * 1000);
    }

    assert(((IL_Test_t*) hComponent)->state == OMX_StateLoaded);
    assert(!((IL_Test_t*) hComponent)->busy);
    assert(((IL_Test_t*) hComponent)->event_index_changed_periodically_thread_id == 0);
    memset(hComponent, 0xCB, sizeof(IL_Test_t));
    free(hComponent);
    return OMX_ErrorNone;
}

OMX_API OMX_ERRORTYPE OMX_APIENTRY OMX_SetupTunnel(
    OMX_IN  OMX_HANDLETYPE hOutput,
    OMX_IN  OMX_U32 nPortOutput,
    OMX_IN  OMX_HANDLETYPE hInput,
    OMX_IN  OMX_U32 nPortInput)
{
    assert(nPortInput  < MAX_PORTS);
    assert(nPortOutput < MAX_PORTS);

  OMX_ERRORTYPE err;
  OMX_COMPONENTTYPE* component;
  OMX_TUNNELSETUPTYPE* tunnelSetup;

  tunnelSetup = malloc(sizeof(OMX_TUNNELSETUPTYPE));
  component = (OMX_COMPONENTTYPE*)hOutput;
  tunnelSetup->nTunnelFlags = 0;
  tunnelSetup->eSupplier = OMX_BufferSupplyUnspecified;

  if (hOutput == NULL && hInput == NULL)
        return OMX_ErrorBadParameter;
  if (hOutput){
    err = (component->ComponentTunnelRequest)(hOutput, nPortOutput, hInput, nPortInput, tunnelSetup);
    if (err != OMX_ErrorNone) {
    free(tunnelSetup);
    tunnelSetup = NULL;
    return err;
    }
  }

  component = (OMX_COMPONENTTYPE*)hInput;
  if (hInput) {
    err = (component->ComponentTunnelRequest)(hInput, nPortInput, hOutput, nPortOutput, tunnelSetup);
    if (err != OMX_ErrorNone) {
      // the second stage fails. the tunnel on poutput port has to be removed
      component = (OMX_COMPONENTTYPE*)hOutput;
      err = (component->ComponentTunnelRequest)(hOutput, nPortOutput, NULL, 0, tunnelSetup);
      if (err != OMX_ErrorNone) {
        // This error should never happen. It is critical, and not recoverable
        free(tunnelSetup);
        tunnelSetup = NULL;
        return OMX_ErrorUndefined;
      }
      free(tunnelSetup);
      tunnelSetup = NULL;
      return OMX_ErrorPortsNotCompatible;
    }
  }
  free(tunnelSetup);
  tunnelSetup = NULL;


    if (is_stub(hOutput) && is_stub(hInput)) {
        IL_Test_t* in_comp    = hInput;
        IL_Test_t* out_comp   = hOutput;

        if (in_comp->state == OMX_StateLoaded) {
            assert(out_comp->state == OMX_StateLoaded);

            // The ports should be enabled.
            assert(in_comp->port[nPortInput].state);
            assert(out_comp->port[nPortOutput].state);
            if( !(in_comp->port[nPortInput].state) || !(out_comp->port[nPortOutput].state)) {
                fprintf(stderr, "!!!!!!!!!!!!! il_stubs.c SILENCED ERROR!!!!\n");
                fprintf(stderr, "!!!!!!!!!!!!! Setup tunnel in StateLoaded, incorrect port states. %X.%d (%d) -> %X.%d (%d)\n",
                    (int) out_comp, (int) nPortOutput, out_comp->port[nPortOutput].state, (int) in_comp, (int) nPortInput, in_comp->port[nPortInput].state);
                fprintf(stderr, "!!!!!!!!!!!!! il_stubs.c SILENCED ERROR!!!!\n");
            }
        } else {
            assert(in_comp->state == OMX_StateIdle || in_comp->state == OMX_StateExecuting);

            // ..then check this
            assert(out_comp->state == OMX_StateIdle || out_comp->state == OMX_StateExecuting);
            if( !(out_comp->state == OMX_StateIdle || out_comp->state == OMX_StateExecuting)) {
                fprintf(stderr, "!!!!!!!!!!!!! il_stubs.c SILENCED ERROR!!!!\n");
                fprintf(stderr, "!!!!!!!!!!!!! Setup tunnel, incorrect states. %X.%d (%d) -> %X.%d (%d)\n",
                    (int) out_comp, (int) nPortOutput, out_comp->state, (int) in_comp, (int) nPortInput, in_comp->state);
                fprintf(stderr, "!!!!!!!!!!!!! il_stubs.c SILENCED ERROR!!!!\n");
            }

            // The ports should be disabled
            assert(!in_comp->port[nPortInput].state);
            assert(!out_comp->port[nPortOutput].state);
        }
    }
    return OMX_ErrorNone;
}






