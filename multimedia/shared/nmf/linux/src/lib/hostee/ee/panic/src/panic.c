/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <ee/panic.nmf>
#include <unwind.h>
#include <stdio.h>
#include <string.h>

/* private functions */
static char* reasons[] = {"INVALID", "INTERNAL_PANIC", "RESERVED_PANIC", "USER_STACK_OVERFLOW",
            "SYSTEM_STACK_OVERFLOW", "UNALIGNED_LONG_ACCESS", "EVENT_FIFO_OVERFLOW", "PARAM_FIFO_OVERFLOW",
            "INTERFACE_NOT_BINDED", "USER_PANIC" , "UNBIND_INTERRUPT", "EVENT_FIFO_IN_USE"};

static void print_backtrace();

/* implement ee.api.panic.itf */
EXPORT_SHARED void Panic(t_panic_reason reason, void *THIS, t_uint32 info)
{
    t_msg_service svcMsg;

    nmfTracePanic(reason, THIS, info, 0);

    if (reason == EVENT_FIFO_OVERFLOW || reason == EVENT_FIFO_IN_USE)
        NMF_LOG("Panic(%s, %x, %s)\n", reasons[reason], THIS, (char *)info);
    else
        NMF_LOG("Panic(%s, %x, %x)\n", reasons[reason], THIS, info);
    print_backtrace();

    /* NOTE : message is allocated on stack since it will be copy for each channel by nmfPushServiceMessage() */
    /* fill panic message structure */
    svcMsg.header.msg.type = EE_CB_SVC_MSG;
    svcMsg.header.serviceType = NMF_SERVICE_PANIC;
    svcMsg.header.messageSize = sizeof(t_msg_service);
    svcMsg.data.panic.panicReason = reason;
    svcMsg.data.panic.panicSource = HOST_EE;
    svcMsg.data.panic.info.host.faultingComponent = THIS;
    svcMsg.data.panic.info.host.panicInfo1 = info;
    svcMsg.data.panic.info.host.panicInfo2 = 0;

    /* give it to ee */
    nmfPushServiceMessage(&svcMsg.header);
}

static void print_callstack_line(unsigned int addr)
{
    FILE* f=fopen("/proc/self/maps", "rb");
    if (f) {
        unsigned int start, end;
        char line[120];
        while (fgets(line, 120, f)) {
            sscanf(line, "%X-%X", &start, &end);
            if (addr >= start && addr <= end) {
                if (*line) {
                    line[strlen(line)-1] = 0; // chop \n
                    char* libname = line + strlen(line);
                    while (libname > line && *libname != ' ') libname--;
                    // Almost same format as debuggerd to re-use scripts..
                    NMF_LOG("   (debug callstack) #00  pc %x   %s\n", addr - start, libname);
                    fclose(f);
                    return;
                }
            }
        }
        fclose(f);
    }

    NMF_LOG("(debug callstack) unresolved %X\n", addr);
}

static _Unwind_Reason_Code trace_function(_Unwind_Context *context, void *arg)
{
   _Unwind_Word ip = _Unwind_GetIP(context);
   if (ip) {
            print_callstack_line((unsigned int) ip);
        }
   return _URC_OK;
}

static void print_backtrace()
{
    NMF_LOG("Start of callstack dump for debug\n");
    _Unwind_Backtrace(trace_function, 0);
    NMF_LOG("End of callstack dump for debug\n");
}

