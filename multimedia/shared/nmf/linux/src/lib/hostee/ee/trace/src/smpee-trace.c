/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
#include <ee/trace.nmf>

#include <inc/nmf-tracedescription.h>

static t_bool ee_trace_enabled = FALSE;

static t_uint64* volatile STMTimestampRegister;
static t_uint64* volatile STMRegister;

// TODO: to be shared !!
static void myStringCopy(char* dest, const char *src, int count)
{
    while (count-- && (*dest++ = *src++) != '\0')
        /* nothing */
        ;
}

/*
 * Component list management
 */
typedef struct {
    t_queue_link link;
    void* Composite;
    void (*dumper)(void* arg);
} t_componentElement;

static t_queue componentQueue = 0;
static hMutex mutex = 0;

/*
 * API implementation
 */
EXPORT_SHARED void nmfTraceInit()
{
    // Call underlying stm in order to get IP address for the register !!
    stm.getChannelRegister(HOST_EE_CHANNEL, (t_uint64**)&STMTimestampRegister, (t_uint64**)&STMRegister);

    if ((mutex = eeMutexCreate()) == 0)
    {
        NMF_LOG("Unable to create trace mutex, trace will be disabled !!\n");
        return;
    }

    componentQueue = nmfQueueCreate();
    // Don't check error, since either the result, we will use it


    nmfTraceReset();
}

EXPORT_SHARED void nmfTraceDone()
{
    if(componentQueue != 0)
        nmfQueueDestroy(componentQueue);

    if (mutex != 0)
        eeMutexDestroy(mutex);
}

/*
 * API implementation
 */
static t_bool findComponent(t_queue_link *pRes, void* pMatchingFunctionArgs)
{
    return (((t_componentElement*)pRes)->Composite == pMatchingFunctionArgs);
}

static t_bool travelComponent(t_queue_link *pRes, void* pMatchingFunctionArgs)
{
    ((t_componentElement*)pRes)->dumper(((t_componentElement*)pRes)->Composite);

    return 0;
}

static void nmfDumpComposite()
{
    if(componentQueue != 0)
    {
        nmfTraceReset();

        nmfQueuePopMatching(componentQueue, (void*)travelComponent, 0);
    }

}

EXPORT_SHARED void nmfRegisterComposite(void* _this, void* traceDumper)
{
    if(componentQueue != 0)
    {
        t_componentElement* pElem;

        pElem = (t_componentElement *) allocator.alloc(sizeof(t_componentElement));

        pElem->Composite = _this;
        pElem->dumper = (void (*)(void *))traceDumper;
        nmfQueuePush(componentQueue, (t_queue_link*)pElem);
    }
}

EXPORT_SHARED void nmfUnregisterComposite(void* _this)
{
    if(componentQueue != 0)
    {
        t_queue_link *pRes = nmfQueuePopMatching(componentQueue, (void*)findComponent, _this);

        NMF_ASSERT(pRes != NULL);

        allocator.free(pRes);
    }
}

void  nmfTraceSetMode(t_bool onoff)
{
    ee_trace_enabled = onoff;

    if(onoff)
        nmfDumpComposite();
}


/*
 * API implementation RAW trace
 */
#define HEADER(t, s) ((t) | (s << 16))

static void strd(unsigned long long value, volatile unsigned long long* addr)
{
    *addr = value;
}

/*
__asm void strd(unsigned long long value, unsigned long long* addr)
{
  STRD r0, r1, [r2, #0]
  BX LR
}
*/

#ifdef LINUX
 /* Use this to by-pass strict aliasing rules in below function */
#define MAY_ALIAS __attribute__((__may_alias__))
#else
#define MAY_ALIAS
#endif
typedef t_uint64 MAY_ALIAS uint64;

static void writeN(struct t_nmfTraceChannelHeader* header)
{
    uint64* volatile data = (uint64*)header;
    uint64* volatile end = (uint64*)(((unsigned int)data) + header->traceSize - sizeof(t_uint64));

    eeMutexLock(mutex);

    while(data < end)
    {
        strd(*data++, STMRegister);
    }

    strd(*data, STMTimestampRegister);

    eeMutexUnlock(mutex);
}

EXPORT_SHARED void nmfTraceReset()
{
    if(ee_trace_enabled)
    {
        struct t_nmfTraceReset   trace;

        trace.header.v = HEADER(TRACE_TYPE_RESET, sizeof(trace));

        trace.minorVersion = TRACE_MINOR_VERSION;
        trace.majorVersion = TRACE_MAJOR_VERSION;

        writeN(&trace.header.s);
    }
}

EXPORT_SHARED void nmfTraceComponent(t_nmfTraceComponentCommandDescription command, void* _this, const char* _name, const char* _templateName)
{
    if(ee_trace_enabled)
    {
        struct t_nmfTraceComponent   trace;

        trace.header.v = HEADER(TRACE_TYPE_COMPONENT, sizeof(trace));

        trace.command = (t_uint16)command;
        trace.domainId = 0x1;
        trace.componentContext = (t_uint32)_this;
        trace.componentUserContext = (t_uint32)_this;
        myStringCopy((char*)trace.componentLocalName, _name, MAX_COMPONENT_NAME_LENGTH);
        myStringCopy((char*)trace.componentTemplateName, _templateName, MAX_TEMPLATE_NAME_LENGTH);

        writeN(&trace.header.s);
    }
}

EXPORT_SHARED void nmfTraceBind(t_nmfTraceBindCommandDescription command, void* _thisfrom, const char* _namefrom, void* _thisto, const char* _nameto)
{
    if(ee_trace_enabled)
    {
        struct t_nmfTraceBind trace;

        trace.header.v = HEADER(TRACE_TYPE_BIND, sizeof(trace));

        trace.command = (t_uint16)command;
        trace.clientDomainId = 0x1;
        trace.clientComponentContext = (t_uint32)_thisfrom;
        trace.serverDomainId = 0x1;
        trace.serverComponentContext = (t_uint32)_thisto;
        myStringCopy((char*)trace.requiredItfName, _namefrom, MAX_INTERFACE_NAME_LENGTH);
        myStringCopy((char*)trace.providedItfName, _nameto, MAX_INTERFACE_NAME_LENGTH);

        writeN(&trace.header.s);
    }
}

EXPORT_SHARED void nmfTraceInterfaceMethod(void* _this, const char* _name, unsigned int _itfmethidx)
{
    if(ee_trace_enabled)
    {
#ifndef WORKSTATION
      struct t_nmfTraceMethod trace;

        trace.header.v = HEADER(TRACE_TYPE_METHOD, sizeof(trace));

        trace.domainId = 0x1;
        trace.componentContext = (t_uint32)_this;
        trace.methodId = _itfmethidx;
        myStringCopy((char*)trace.methodName, _name, MAX_INTERFACE_METHOD_NAME_LENGTH);

        writeN(&trace.header.s);
#else
        NMF_LOG("Method(%x, %d, %s)\n", _this, _itfmethidx, _name);
#endif
    }
}

EXPORT_SHARED void nmfTraceActivity(t_nmfTraceActivityCommandDescription command, void* _this, unsigned int _itfmethidx)
{
    if(ee_trace_enabled)
    {
#ifndef WORKSTATION
        struct t_nmfTraceActivity trace;

        trace.header.v = HEADER(TRACE_TYPE_ACTIVITY, sizeof(trace));

        trace.command = (t_uint16)command;
        trace.domainId = 0x1;
        trace.componentContext = (t_uint32)_this;
        trace.methodId = _itfmethidx;

        writeN(&trace.header.s);
#else
        static char* name[] = {"INVALID", "START", "END", "POST", "CALL", "RETURN"};

        NMF_LOG("Activity(%s, %x, %d)\n", name[command], _this, _itfmethidx);
#endif
    }
}

EXPORT_SHARED void nmfTracePanic(t_panic_reason reason, void* componentContext, t_uint32 info1, t_uint32 info2)
{
    if(ee_trace_enabled)
    {
#ifndef WORKSTATION
       struct t_nmfTracePanic trace;

        trace.header.v = HEADER(TRACE_TYPE_PANIC, sizeof(trace));

        trace.reason = (t_uint16)reason;
        trace.domainId = 0x1;
        trace.componentContext = (t_uint32)componentContext;
        trace.information1 = info1;
        trace.information2 = info2;

        writeN(&trace.header.s);
#else
        // Nothing to do since already print
#endif
   }
}
