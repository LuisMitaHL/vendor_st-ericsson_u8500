/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*****************************************************************************/
/**
 * This is a Nomadik Multiprocessing Framework generated file, please do not modify it.
 */

/* Static composition of openmax_processor_wrp */
#include <inc/type.h>

#define IN_NMF_ASSEMBLY
#include <openmax_processor.nmf>
#include <_evl/postevent.nmf>
#include <_evl/armnmf_emptythisbuffer.nmf>
#include <_evl/armnmf_fillthisbuffer.nmf>
#include <_evl/sendcommand.nmf>
#include <_evl/fsminit.nmf>
#include <_evl/openmax_processor/api/Config.nmf>
#include <_evl/openmax_processor/api/Param.nmf>
#include <_cbl/armnmf_fillthisbuffer.nmf>
#include <_cbl/armnmf_emptythisbuffer.nmf>
#include <_cbl/eventhandler.nmf>
#include <_cbl/openmax_processor/api/ToOMXComponent.nmf>
#include "openmax_processor_wrp.hpp"

extern "C" IMPORT_SHARED void nmfTraceInit(void);
extern "C" IMPORT_SHARED void nmfTraceDone(void);
extern "C" void nmfTraceSetMode(t_bool onoff);
extern "C" IMPORT_SHARED void nmfRegisterComposite(void* _this, void* traceDumper);
extern "C" IMPORT_SHARED void nmfUnregisterComposite(void* _this);
extern "C" IMPORT_SHARED void nmfTraceReset(void);
extern "C" IMPORT_SHARED void nmfTraceComponent(t_nmfTraceComponentCommandDescription command, void* _this, const char* _name, const char* _templateName);
extern "C" IMPORT_SHARED void nmfTraceBind(t_nmfTraceBindCommandDescription command, void* _thisfrom, const char* _namefrom, void* _thisto, const char* _nameto);
extern "C" IMPORT_SHARED void nmfTraceInterfaceMethod(void* _this, const char* _name, unsigned int _itfmethidx);
extern "C" IMPORT_SHARED void nmfTraceActivity(t_nmfTraceActivityCommandDescription command, void* _this, unsigned int _itfmethidx);
extern "C" IMPORT_SHARED void nmfTracePanic(t_panic_reason reason, void* faultingComponent, t_uint32 info1, t_uint32 info2);
extern "C" IMPORT_SHARED void* EEgetDistributionChannel(t_uint32 priority);
extern "C" IMPORT_SHARED t_uint32 signalComponentCreation(t_uint32 priority);
extern "C" IMPORT_SHARED t_uint32 signalComponentDestruction(t_uint32 priority);
typedef t_uint32 hMutex;
extern "C" IMPORT_SHARED hMutex eeMutexCreate();
extern "C" IMPORT_SHARED t_sint32 eeMutexDestroy(hMutex mutex);
extern "C" IMPORT_SHARED void eeMutexLock(hMutex mutex);
extern "C" IMPORT_SHARED t_sint32 eeMutexLockTry(hMutex mutex);
extern "C" IMPORT_SHARED void eeMutexUnlock(hMutex mutex);
/*
 * Declare composite class implementation
 */
class openmax_processor_wrpImpl: public openmax_processor_wrp{
  protected:
       t_sint16 levelOfConstruction;
       t_sint8 startNumber, constructedNumber;
    openmax_processor &_xyuv_Processor;  // normal
    _evl_postevent _xyuv_bc0; // Processor.me->Processor.postevent [10]
    _evl_armnmf_emptythisbuffer _xybc_emptythisbuffer[10]; // Call
    NMF::InterfaceReference *_xyitfc_emptythisbuffer[10]; // Call
    _evl_armnmf_fillthisbuffer _xybc_fillthisbuffer[10]; // Call
    NMF::InterfaceReference *_xyitfc_fillthisbuffer[10]; // Call
    _evl_sendcommand _xybc_sendcommand; // Call
   NMF::InterfaceReference *_xyitfc_sendcommand; // Call
    _evl_fsminit _xybc_fsminit; // Call
   NMF::InterfaceReference *_xyitfc_fsminit; // Call
    _evl_openmax_processor_api_Config _xybc_Config; // Call
   NMF::InterfaceReference *_xyitfc_Config; // Call
    _evl_openmax_processor_api_Param _xybc_Param; // Call
   NMF::InterfaceReference *_xyitfc_Param; // Call
    Iarmnmf_fillthisbuffer _xyitfcb_emptybufferdone[10]; // Callback
    Iarmnmf_emptythisbuffer _xyitfcb_fillbufferdone[10]; // Callback
    Ieventhandler _xyitfcb_proxy; // Callback
    Iopenmax_processor_api_ToOMXComponent _xyitfcb_ToOMXComponent; // Callback

  public:
    openmax_processor_wrpImpl(openmax_processor *pComp);
    virtual ~openmax_processor_wrpImpl(void);
    virtual t_nmf_error construct(void);
    virtual void start(void);
    virtual void stop(void);
    virtual t_nmf_error destroy(void);
    virtual t_nmf_error bindFromUser(const char* name, unsigned int size, NMF::InterfaceReference* reference, unsigned int subpriority);
    virtual t_nmf_error unbindFromUser(const char* name);
    virtual t_nmf_error bindToUser(t_nmf_channel channel, const char* compositeitfname, void *clientContext, unsigned int size);
    virtual t_nmf_error unbindToUser(t_nmf_channel channel, const char* compositeitfname, void **clientContext);
    virtual t_nmf_error bindAsynchronous(const char* name, unsigned int size, NMF::Composite* target, const char* targetname, unsigned int subpriority);
    virtual t_nmf_error unbindAsynchronous(const char* name, NMF::Composite* target, const char* targetname);
    virtual t_nmf_error bindComponent(const char* name, NMF::Composite* target, const char* targetname);
    virtual t_nmf_error unbindComponent(const char* name, NMF::Composite* target, const char* targetname);
    virtual t_nmf_error readAttribute(const char* name, t_uint32 *value);
    virtual t_nmf_error getProperty(const char* name, char *value, t_uint32 valuelen);
    virtual t_nmf_component_handle getMPCComponentHandle(const char *nameinowner);
    virtual t_nmf_component_handle getComponentInterface(const char *compositeitfname, char *realitfname);
    virtual t_nmf_error bindFromMPC(t_nmf_component_handle client, const char* clientitfname, const char * compositeitfname, unsigned int size);
    virtual t_nmf_error unbindFromMPC(t_nmf_component_handle client, const char* clientitfname, const char * compositeitfname);
    virtual t_nmf_error bindToMPC(const char *compositeitfname, t_nmf_component_handle server, const char* serveritfname, unsigned int size);
    virtual t_nmf_error unbindToMPC(const char *compositeitfname, t_nmf_component_handle server, const char* serveritfname);
  protected:
    virtual t_nmf_error getInterface(const char* name, NMF::InterfaceReference* reference);
    virtual void dumpInstantiation();
    virtual void dumpDestruction();
    static void dumpInstantiationJumper(void* _this);
};

EXPORT_SHARED openmax_processor_wrp* openmax_processor_wrpCreate(openmax_processor *pComp) {
  return new openmax_processor_wrpImpl(pComp);
}
EXPORT_SHARED  void openmax_processor_wrpDestroy(openmax_processor_wrp*& instance) {
  delete ((openmax_processor_wrpImpl*)instance);
  instance = 0x0;
}

/*
 *Declare primitive component instance and methods
 */
openmax_processor_wrpImpl::openmax_processor_wrpImpl(openmax_processor *pComp): 
    startNumber(0),
  _xyuv_Processor(pComp==NULL? *new openmax_processor : *pComp)
{
  levelOfConstruction = 0;
  constructedNumber = 0;
  _xyuv_Processor.name = "Processor";
  _xyuv_Processor.proxy = (eventhandlerDescriptor*)0x0;
  _xyuv_Processor.me = (posteventDescriptor*)&_xyuv_bc0;
  _xyuv_Processor.emptybufferdone[0] = (armnmf_fillthisbufferDescriptor*)0x0;
  _xyuv_Processor.emptybufferdone[1] = (armnmf_fillthisbufferDescriptor*)0x0;
  _xyuv_Processor.emptybufferdone[2] = (armnmf_fillthisbufferDescriptor*)0x0;
  _xyuv_Processor.emptybufferdone[3] = (armnmf_fillthisbufferDescriptor*)0x0;
  _xyuv_Processor.emptybufferdone[4] = (armnmf_fillthisbufferDescriptor*)0x0;
  _xyuv_Processor.emptybufferdone[5] = (armnmf_fillthisbufferDescriptor*)0x0;
  _xyuv_Processor.emptybufferdone[6] = (armnmf_fillthisbufferDescriptor*)0x0;
  _xyuv_Processor.emptybufferdone[7] = (armnmf_fillthisbufferDescriptor*)0x0;
  _xyuv_Processor.emptybufferdone[8] = (armnmf_fillthisbufferDescriptor*)0x0;
  _xyuv_Processor.emptybufferdone[9] = (armnmf_fillthisbufferDescriptor*)0x0;
  _xyuv_Processor.fillbufferdone[0] = (armnmf_emptythisbufferDescriptor*)0x0;
  _xyuv_Processor.fillbufferdone[1] = (armnmf_emptythisbufferDescriptor*)0x0;
  _xyuv_Processor.fillbufferdone[2] = (armnmf_emptythisbufferDescriptor*)0x0;
  _xyuv_Processor.fillbufferdone[3] = (armnmf_emptythisbufferDescriptor*)0x0;
  _xyuv_Processor.fillbufferdone[4] = (armnmf_emptythisbufferDescriptor*)0x0;
  _xyuv_Processor.fillbufferdone[5] = (armnmf_emptythisbufferDescriptor*)0x0;
  _xyuv_Processor.fillbufferdone[6] = (armnmf_emptythisbufferDescriptor*)0x0;
  _xyuv_Processor.fillbufferdone[7] = (armnmf_emptythisbufferDescriptor*)0x0;
  _xyuv_Processor.fillbufferdone[8] = (armnmf_emptythisbufferDescriptor*)0x0;
  _xyuv_Processor.fillbufferdone[9] = (armnmf_emptythisbufferDescriptor*)0x0;
  _xyuv_Processor.ToOMXComponent = (openmax_processor_api_ToOMXComponentDescriptor*)0x0;
  _xyuv_bc0.name = "bc0 # Processor.me->Processor.postevent [10]";
  _xyuv_bc0.target = (posteventDescriptor*)&_xyuv_Processor;
  _xyitfcb_emptybufferdone[0] = 0x0;
  _xyitfcb_emptybufferdone[1] = 0x0;
  _xyitfcb_emptybufferdone[2] = 0x0;
  _xyitfcb_emptybufferdone[3] = 0x0;
  _xyitfcb_emptybufferdone[4] = 0x0;
  _xyitfcb_emptybufferdone[5] = 0x0;
  _xyitfcb_emptybufferdone[6] = 0x0;
  _xyitfcb_emptybufferdone[7] = 0x0;
  _xyitfcb_emptybufferdone[8] = 0x0;
  _xyitfcb_emptybufferdone[9] = 0x0;
  _xyitfcb_fillbufferdone[0] = 0x0;
  _xyitfcb_fillbufferdone[1] = 0x0;
  _xyitfcb_fillbufferdone[2] = 0x0;
  _xyitfcb_fillbufferdone[3] = 0x0;
  _xyitfcb_fillbufferdone[4] = 0x0;
  _xyitfcb_fillbufferdone[5] = 0x0;
  _xyitfcb_fillbufferdone[6] = 0x0;
  _xyitfcb_fillbufferdone[7] = 0x0;
  _xyitfcb_fillbufferdone[8] = 0x0;
  _xyitfcb_fillbufferdone[9] = 0x0;
  _xyitfcb_proxy = 0x0;
  _xyitfcb_ToOMXComponent = 0x0;
}

openmax_processor_wrpImpl::~openmax_processor_wrpImpl() { 
        delete &_xyuv_Processor;
}

t_nmf_error openmax_processor_wrpImpl::construct(void) {
  t_nmf_error error = NMF_OK;
  if(constructedNumber++ > 0)
    return NMF_INVALID_COMPONENT_STATE_TRANSITION;
  if(levelOfConstruction != 0)
    return NMF_INVALID_COMPONENT_STATE_TRANSITION;

  // Construction of Processor
  _xyuv_Processor.setMutexHandle((t_uint32) eeMutexCreate());
  if(_xyuv_Processor.getMutexHandle() == 0x0) {
    error = NMF_NO_MORE_MEMORY;
    goto out_on_error;
  }
  levelOfConstruction = 1;
  if(signalComponentCreation(1)) {
    error = NMF_NO_MORE_MEMORY;
    goto out_on_error;
  }
  levelOfConstruction = 2;
  levelOfConstruction = 3;

  // Construction of bc0
  _xyuv_bc0.setMutexHandle((t_uint32) eeMutexCreate());
  if(_xyuv_bc0.getMutexHandle() == 0x0) {
    error = NMF_NO_MORE_MEMORY;
    goto out_on_error;
  }
  _xyuv_bc0.connect(2 << 8, &_xyuv_Processor);
  _xyuv_bc0.createFifo(10, 1, 0);
  levelOfConstruction = 4;

  // Construction of emptythisbuffer[0]
  levelOfConstruction = 5;

  // Construction of emptythisbuffer[1]
  levelOfConstruction = 6;

  // Construction of emptythisbuffer[2]
  levelOfConstruction = 7;

  // Construction of emptythisbuffer[3]
  levelOfConstruction = 8;

  // Construction of emptythisbuffer[4]
  levelOfConstruction = 9;

  // Construction of emptythisbuffer[5]
  levelOfConstruction = 10;

  // Construction of emptythisbuffer[6]
  levelOfConstruction = 11;

  // Construction of emptythisbuffer[7]
  levelOfConstruction = 12;

  // Construction of emptythisbuffer[8]
  levelOfConstruction = 13;

  // Construction of emptythisbuffer[9]
  levelOfConstruction = 14;

  // Construction of fillthisbuffer[0]
  levelOfConstruction = 15;

  // Construction of fillthisbuffer[1]
  levelOfConstruction = 16;

  // Construction of fillthisbuffer[2]
  levelOfConstruction = 17;

  // Construction of fillthisbuffer[3]
  levelOfConstruction = 18;

  // Construction of fillthisbuffer[4]
  levelOfConstruction = 19;

  // Construction of fillthisbuffer[5]
  levelOfConstruction = 20;

  // Construction of fillthisbuffer[6]
  levelOfConstruction = 21;

  // Construction of fillthisbuffer[7]
  levelOfConstruction = 22;

  // Construction of fillthisbuffer[8]
  levelOfConstruction = 23;

  // Construction of fillthisbuffer[9]
  levelOfConstruction = 24;

  // Construction of sendcommand
  levelOfConstruction = 25;

  // Construction of fsminit
  levelOfConstruction = 26;

  // Construction of Config
  levelOfConstruction = 27;

  // Construction of Param
  levelOfConstruction = 28;

  // Construction of emptybufferdone[0]
  levelOfConstruction = 29;

  // Construction of emptybufferdone[1]
  levelOfConstruction = 30;

  // Construction of emptybufferdone[2]
  levelOfConstruction = 31;

  // Construction of emptybufferdone[3]
  levelOfConstruction = 32;

  // Construction of emptybufferdone[4]
  levelOfConstruction = 33;

  // Construction of emptybufferdone[5]
  levelOfConstruction = 34;

  // Construction of emptybufferdone[6]
  levelOfConstruction = 35;

  // Construction of emptybufferdone[7]
  levelOfConstruction = 36;

  // Construction of emptybufferdone[8]
  levelOfConstruction = 37;

  // Construction of emptybufferdone[9]
  levelOfConstruction = 38;

  // Construction of fillbufferdone[0]
  levelOfConstruction = 39;

  // Construction of fillbufferdone[1]
  levelOfConstruction = 40;

  // Construction of fillbufferdone[2]
  levelOfConstruction = 41;

  // Construction of fillbufferdone[3]
  levelOfConstruction = 42;

  // Construction of fillbufferdone[4]
  levelOfConstruction = 43;

  // Construction of fillbufferdone[5]
  levelOfConstruction = 44;

  // Construction of fillbufferdone[6]
  levelOfConstruction = 45;

  // Construction of fillbufferdone[7]
  levelOfConstruction = 46;

  // Construction of fillbufferdone[8]
  levelOfConstruction = 47;

  // Construction of fillbufferdone[9]
  levelOfConstruction = 48;

  // Construction of proxy
  levelOfConstruction = 49;

  // Construction of ToOMXComponent
  levelOfConstruction = 50;


  nmfRegisterComposite((void*)this, (void*)openmax_processor_wrpImpl::dumpInstantiationJumper);
  levelOfConstruction = 51;

  dumpInstantiation();
  return error;
out_on_error:
  destroy();
  return error;
}

void openmax_processor_wrpImpl::start(void) {
  if(constructedNumber == 0)
    return;
  if(startNumber++ > 0)
    return;
  _xyuv_Processor.start();
}

void openmax_processor_wrpImpl::stop(void) {
  if(constructedNumber == 0)
    return;
  if(--startNumber > 0)
    return;
  _xyuv_Processor.stop();
}

t_nmf_error openmax_processor_wrpImpl::destroy(void) {
  if(--constructedNumber > 0)
    return NMF_OK;
  if(startNumber > 0)
    return NMF_COMPONENT_NOT_STOPPED;
  if(levelOfConstruction == 0)
    return NMF_INVALID_COMPONENT_STATE_TRANSITION;

  if(levelOfConstruction >= 51) 
    nmfUnregisterComposite((void*)this);

  dumpDestruction();

  // Destruction of ToOMXComponent
  if(levelOfConstruction >= 50) {
  }

  // Destruction of proxy
  if(levelOfConstruction >= 49) {
  }

  // Destruction of fillbufferdone[9]
  if(levelOfConstruction >= 48) {
  }

  // Destruction of fillbufferdone[8]
  if(levelOfConstruction >= 47) {
  }

  // Destruction of fillbufferdone[7]
  if(levelOfConstruction >= 46) {
  }

  // Destruction of fillbufferdone[6]
  if(levelOfConstruction >= 45) {
  }

  // Destruction of fillbufferdone[5]
  if(levelOfConstruction >= 44) {
  }

  // Destruction of fillbufferdone[4]
  if(levelOfConstruction >= 43) {
  }

  // Destruction of fillbufferdone[3]
  if(levelOfConstruction >= 42) {
  }

  // Destruction of fillbufferdone[2]
  if(levelOfConstruction >= 41) {
  }

  // Destruction of fillbufferdone[1]
  if(levelOfConstruction >= 40) {
  }

  // Destruction of fillbufferdone[0]
  if(levelOfConstruction >= 39) {
  }

  // Destruction of emptybufferdone[9]
  if(levelOfConstruction >= 38) {
  }

  // Destruction of emptybufferdone[8]
  if(levelOfConstruction >= 37) {
  }

  // Destruction of emptybufferdone[7]
  if(levelOfConstruction >= 36) {
  }

  // Destruction of emptybufferdone[6]
  if(levelOfConstruction >= 35) {
  }

  // Destruction of emptybufferdone[5]
  if(levelOfConstruction >= 34) {
  }

  // Destruction of emptybufferdone[4]
  if(levelOfConstruction >= 33) {
  }

  // Destruction of emptybufferdone[3]
  if(levelOfConstruction >= 32) {
  }

  // Destruction of emptybufferdone[2]
  if(levelOfConstruction >= 31) {
  }

  // Destruction of emptybufferdone[1]
  if(levelOfConstruction >= 30) {
  }

  // Destruction of emptybufferdone[0]
  if(levelOfConstruction >= 29) {
  }

  // Destruction of Param
  if(levelOfConstruction >= 28) {
  }

  // Destruction of Config
  if(levelOfConstruction >= 27) {
  }

  // Destruction of fsminit
  if(levelOfConstruction >= 26) {
  }

  // Destruction of sendcommand
  if(levelOfConstruction >= 25) {
  }

  // Destruction of fillthisbuffer[9]
  if(levelOfConstruction >= 24) {
  }

  // Destruction of fillthisbuffer[8]
  if(levelOfConstruction >= 23) {
  }

  // Destruction of fillthisbuffer[7]
  if(levelOfConstruction >= 22) {
  }

  // Destruction of fillthisbuffer[6]
  if(levelOfConstruction >= 21) {
  }

  // Destruction of fillthisbuffer[5]
  if(levelOfConstruction >= 20) {
  }

  // Destruction of fillthisbuffer[4]
  if(levelOfConstruction >= 19) {
  }

  // Destruction of fillthisbuffer[3]
  if(levelOfConstruction >= 18) {
  }

  // Destruction of fillthisbuffer[2]
  if(levelOfConstruction >= 17) {
  }

  // Destruction of fillthisbuffer[1]
  if(levelOfConstruction >= 16) {
  }

  // Destruction of fillthisbuffer[0]
  if(levelOfConstruction >= 15) {
  }

  // Destruction of emptythisbuffer[9]
  if(levelOfConstruction >= 14) {
  }

  // Destruction of emptythisbuffer[8]
  if(levelOfConstruction >= 13) {
  }

  // Destruction of emptythisbuffer[7]
  if(levelOfConstruction >= 12) {
  }

  // Destruction of emptythisbuffer[6]
  if(levelOfConstruction >= 11) {
  }

  // Destruction of emptythisbuffer[5]
  if(levelOfConstruction >= 10) {
  }

  // Destruction of emptythisbuffer[4]
  if(levelOfConstruction >= 9) {
  }

  // Destruction of emptythisbuffer[3]
  if(levelOfConstruction >= 8) {
  }

  // Destruction of emptythisbuffer[2]
  if(levelOfConstruction >= 7) {
  }

  // Destruction of emptythisbuffer[1]
  if(levelOfConstruction >= 6) {
  }

  // Destruction of emptythisbuffer[0]
  if(levelOfConstruction >= 5) {
  }

  // Destruction of bc0
  if(levelOfConstruction >= 4) {
    _xyuv_bc0.destroyFifo();
    eeMutexLock((hMutex) _xyuv_bc0.getMutexHandle());
    eeMutexUnlock((hMutex) _xyuv_bc0.getMutexHandle());
    eeMutexDestroy((hMutex) _xyuv_bc0.getMutexHandle());
  }

  // Destruction of Processor
  if(levelOfConstruction >= 3) {
  }
  if(levelOfConstruction >= 2) {
    signalComponentDestruction(1);
  }
  if(levelOfConstruction >= 1) {
    eeMutexLock((hMutex) _xyuv_Processor.getMutexHandle());
    eeMutexUnlock((hMutex) _xyuv_Processor.getMutexHandle());
    eeMutexDestroy((hMutex) _xyuv_Processor.getMutexHandle());
  }


  levelOfConstruction = 0;
  return NMF_OK;
}

t_nmf_error openmax_processor_wrpImpl::getInterface(const char* name, NMF::InterfaceReference* reference) {
  if(compositeStrcmp(name, "emptythisbuffer[0]") == 0) {
    *reference = (armnmf_emptythisbufferDescriptor*)&_xyuv_Processor.emptythisbuffer[0];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "emptythisbuffer[1]") == 0) {
    *reference = (armnmf_emptythisbufferDescriptor*)&_xyuv_Processor.emptythisbuffer[1];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "emptythisbuffer[2]") == 0) {
    *reference = (armnmf_emptythisbufferDescriptor*)&_xyuv_Processor.emptythisbuffer[2];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "emptythisbuffer[3]") == 0) {
    *reference = (armnmf_emptythisbufferDescriptor*)&_xyuv_Processor.emptythisbuffer[3];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "emptythisbuffer[4]") == 0) {
    *reference = (armnmf_emptythisbufferDescriptor*)&_xyuv_Processor.emptythisbuffer[4];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "emptythisbuffer[5]") == 0) {
    *reference = (armnmf_emptythisbufferDescriptor*)&_xyuv_Processor.emptythisbuffer[5];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "emptythisbuffer[6]") == 0) {
    *reference = (armnmf_emptythisbufferDescriptor*)&_xyuv_Processor.emptythisbuffer[6];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "emptythisbuffer[7]") == 0) {
    *reference = (armnmf_emptythisbufferDescriptor*)&_xyuv_Processor.emptythisbuffer[7];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "emptythisbuffer[8]") == 0) {
    *reference = (armnmf_emptythisbufferDescriptor*)&_xyuv_Processor.emptythisbuffer[8];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "emptythisbuffer[9]") == 0) {
    *reference = (armnmf_emptythisbufferDescriptor*)&_xyuv_Processor.emptythisbuffer[9];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "fillthisbuffer[0]") == 0) {
    *reference = (armnmf_fillthisbufferDescriptor*)&_xyuv_Processor.fillthisbuffer[0];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "fillthisbuffer[1]") == 0) {
    *reference = (armnmf_fillthisbufferDescriptor*)&_xyuv_Processor.fillthisbuffer[1];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "fillthisbuffer[2]") == 0) {
    *reference = (armnmf_fillthisbufferDescriptor*)&_xyuv_Processor.fillthisbuffer[2];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "fillthisbuffer[3]") == 0) {
    *reference = (armnmf_fillthisbufferDescriptor*)&_xyuv_Processor.fillthisbuffer[3];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "fillthisbuffer[4]") == 0) {
    *reference = (armnmf_fillthisbufferDescriptor*)&_xyuv_Processor.fillthisbuffer[4];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "fillthisbuffer[5]") == 0) {
    *reference = (armnmf_fillthisbufferDescriptor*)&_xyuv_Processor.fillthisbuffer[5];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "fillthisbuffer[6]") == 0) {
    *reference = (armnmf_fillthisbufferDescriptor*)&_xyuv_Processor.fillthisbuffer[6];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "fillthisbuffer[7]") == 0) {
    *reference = (armnmf_fillthisbufferDescriptor*)&_xyuv_Processor.fillthisbuffer[7];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "fillthisbuffer[8]") == 0) {
    *reference = (armnmf_fillthisbufferDescriptor*)&_xyuv_Processor.fillthisbuffer[8];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "fillthisbuffer[9]") == 0) {
    *reference = (armnmf_fillthisbufferDescriptor*)&_xyuv_Processor.fillthisbuffer[9];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "sendcommand") == 0) {
    *reference = (sendcommandDescriptor*)&_xyuv_Processor;
    return NMF_OK;
  }
  if(compositeStrcmp(name, "fsminit") == 0) {
    *reference = (fsminitDescriptor*)&_xyuv_Processor;
    return NMF_OK;
  }
  if(compositeStrcmp(name, "Config") == 0) {
    *reference = (openmax_processor_api_ConfigDescriptor*)&_xyuv_Processor;
    return NMF_OK;
  }
  if(compositeStrcmp(name, "Param") == 0) {
    *reference = (openmax_processor_api_ParamDescriptor*)&_xyuv_Processor;
    return NMF_OK;
  }
  return NMF_NO_SUCH_PROVIDED_INTERFACE;
}

t_nmf_error openmax_processor_wrpImpl::bindFromUser(const char* name, unsigned int size, NMF::InterfaceReference* reference, unsigned int subpriority) {
  if(compositeStrcmp(name, "emptythisbuffer[0]") == 0) {
    _xybc_emptythisbuffer[0].destroyFifo();            // Destroy server previous binding if any
    _xybc_emptythisbuffer[0].connect(57005 << 8, &_xyuv_Processor);
    if(_xybc_emptythisbuffer[0].createFifo(size, 1, subpriority) != NMF_OK)
      return NMF_NO_MORE_MEMORY;
    _xybc_emptythisbuffer[0].target = (armnmf_emptythisbufferDescriptor*)&_xyuv_Processor.emptythisbuffer[0];
    *reference = &_xybc_emptythisbuffer[0];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "emptythisbuffer[1]") == 0) {
    _xybc_emptythisbuffer[1].destroyFifo();            // Destroy server previous binding if any
    _xybc_emptythisbuffer[1].connect(57005 << 8, &_xyuv_Processor);
    if(_xybc_emptythisbuffer[1].createFifo(size, 1, subpriority) != NMF_OK)
      return NMF_NO_MORE_MEMORY;
    _xybc_emptythisbuffer[1].target = (armnmf_emptythisbufferDescriptor*)&_xyuv_Processor.emptythisbuffer[1];
    *reference = &_xybc_emptythisbuffer[1];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "emptythisbuffer[2]") == 0) {
    _xybc_emptythisbuffer[2].destroyFifo();            // Destroy server previous binding if any
    _xybc_emptythisbuffer[2].connect(57005 << 8, &_xyuv_Processor);
    if(_xybc_emptythisbuffer[2].createFifo(size, 1, subpriority) != NMF_OK)
      return NMF_NO_MORE_MEMORY;
    _xybc_emptythisbuffer[2].target = (armnmf_emptythisbufferDescriptor*)&_xyuv_Processor.emptythisbuffer[2];
    *reference = &_xybc_emptythisbuffer[2];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "emptythisbuffer[3]") == 0) {
    _xybc_emptythisbuffer[3].destroyFifo();            // Destroy server previous binding if any
    _xybc_emptythisbuffer[3].connect(57005 << 8, &_xyuv_Processor);
    if(_xybc_emptythisbuffer[3].createFifo(size, 1, subpriority) != NMF_OK)
      return NMF_NO_MORE_MEMORY;
    _xybc_emptythisbuffer[3].target = (armnmf_emptythisbufferDescriptor*)&_xyuv_Processor.emptythisbuffer[3];
    *reference = &_xybc_emptythisbuffer[3];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "emptythisbuffer[4]") == 0) {
    _xybc_emptythisbuffer[4].destroyFifo();            // Destroy server previous binding if any
    _xybc_emptythisbuffer[4].connect(57005 << 8, &_xyuv_Processor);
    if(_xybc_emptythisbuffer[4].createFifo(size, 1, subpriority) != NMF_OK)
      return NMF_NO_MORE_MEMORY;
    _xybc_emptythisbuffer[4].target = (armnmf_emptythisbufferDescriptor*)&_xyuv_Processor.emptythisbuffer[4];
    *reference = &_xybc_emptythisbuffer[4];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "emptythisbuffer[5]") == 0) {
    _xybc_emptythisbuffer[5].destroyFifo();            // Destroy server previous binding if any
    _xybc_emptythisbuffer[5].connect(57005 << 8, &_xyuv_Processor);
    if(_xybc_emptythisbuffer[5].createFifo(size, 1, subpriority) != NMF_OK)
      return NMF_NO_MORE_MEMORY;
    _xybc_emptythisbuffer[5].target = (armnmf_emptythisbufferDescriptor*)&_xyuv_Processor.emptythisbuffer[5];
    *reference = &_xybc_emptythisbuffer[5];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "emptythisbuffer[6]") == 0) {
    _xybc_emptythisbuffer[6].destroyFifo();            // Destroy server previous binding if any
    _xybc_emptythisbuffer[6].connect(57005 << 8, &_xyuv_Processor);
    if(_xybc_emptythisbuffer[6].createFifo(size, 1, subpriority) != NMF_OK)
      return NMF_NO_MORE_MEMORY;
    _xybc_emptythisbuffer[6].target = (armnmf_emptythisbufferDescriptor*)&_xyuv_Processor.emptythisbuffer[6];
    *reference = &_xybc_emptythisbuffer[6];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "emptythisbuffer[7]") == 0) {
    _xybc_emptythisbuffer[7].destroyFifo();            // Destroy server previous binding if any
    _xybc_emptythisbuffer[7].connect(57005 << 8, &_xyuv_Processor);
    if(_xybc_emptythisbuffer[7].createFifo(size, 1, subpriority) != NMF_OK)
      return NMF_NO_MORE_MEMORY;
    _xybc_emptythisbuffer[7].target = (armnmf_emptythisbufferDescriptor*)&_xyuv_Processor.emptythisbuffer[7];
    *reference = &_xybc_emptythisbuffer[7];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "emptythisbuffer[8]") == 0) {
    _xybc_emptythisbuffer[8].destroyFifo();            // Destroy server previous binding if any
    _xybc_emptythisbuffer[8].connect(57005 << 8, &_xyuv_Processor);
    if(_xybc_emptythisbuffer[8].createFifo(size, 1, subpriority) != NMF_OK)
      return NMF_NO_MORE_MEMORY;
    _xybc_emptythisbuffer[8].target = (armnmf_emptythisbufferDescriptor*)&_xyuv_Processor.emptythisbuffer[8];
    *reference = &_xybc_emptythisbuffer[8];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "emptythisbuffer[9]") == 0) {
    _xybc_emptythisbuffer[9].destroyFifo();            // Destroy server previous binding if any
    _xybc_emptythisbuffer[9].connect(57005 << 8, &_xyuv_Processor);
    if(_xybc_emptythisbuffer[9].createFifo(size, 1, subpriority) != NMF_OK)
      return NMF_NO_MORE_MEMORY;
    _xybc_emptythisbuffer[9].target = (armnmf_emptythisbufferDescriptor*)&_xyuv_Processor.emptythisbuffer[9];
    *reference = &_xybc_emptythisbuffer[9];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "fillthisbuffer[0]") == 0) {
    _xybc_fillthisbuffer[0].destroyFifo();            // Destroy server previous binding if any
    _xybc_fillthisbuffer[0].connect(57005 << 8, &_xyuv_Processor);
    if(_xybc_fillthisbuffer[0].createFifo(size, 1, subpriority) != NMF_OK)
      return NMF_NO_MORE_MEMORY;
    _xybc_fillthisbuffer[0].target = (armnmf_fillthisbufferDescriptor*)&_xyuv_Processor.fillthisbuffer[0];
    *reference = &_xybc_fillthisbuffer[0];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "fillthisbuffer[1]") == 0) {
    _xybc_fillthisbuffer[1].destroyFifo();            // Destroy server previous binding if any
    _xybc_fillthisbuffer[1].connect(57005 << 8, &_xyuv_Processor);
    if(_xybc_fillthisbuffer[1].createFifo(size, 1, subpriority) != NMF_OK)
      return NMF_NO_MORE_MEMORY;
    _xybc_fillthisbuffer[1].target = (armnmf_fillthisbufferDescriptor*)&_xyuv_Processor.fillthisbuffer[1];
    *reference = &_xybc_fillthisbuffer[1];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "fillthisbuffer[2]") == 0) {
    _xybc_fillthisbuffer[2].destroyFifo();            // Destroy server previous binding if any
    _xybc_fillthisbuffer[2].connect(57005 << 8, &_xyuv_Processor);
    if(_xybc_fillthisbuffer[2].createFifo(size, 1, subpriority) != NMF_OK)
      return NMF_NO_MORE_MEMORY;
    _xybc_fillthisbuffer[2].target = (armnmf_fillthisbufferDescriptor*)&_xyuv_Processor.fillthisbuffer[2];
    *reference = &_xybc_fillthisbuffer[2];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "fillthisbuffer[3]") == 0) {
    _xybc_fillthisbuffer[3].destroyFifo();            // Destroy server previous binding if any
    _xybc_fillthisbuffer[3].connect(57005 << 8, &_xyuv_Processor);
    if(_xybc_fillthisbuffer[3].createFifo(size, 1, subpriority) != NMF_OK)
      return NMF_NO_MORE_MEMORY;
    _xybc_fillthisbuffer[3].target = (armnmf_fillthisbufferDescriptor*)&_xyuv_Processor.fillthisbuffer[3];
    *reference = &_xybc_fillthisbuffer[3];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "fillthisbuffer[4]") == 0) {
    _xybc_fillthisbuffer[4].destroyFifo();            // Destroy server previous binding if any
    _xybc_fillthisbuffer[4].connect(57005 << 8, &_xyuv_Processor);
    if(_xybc_fillthisbuffer[4].createFifo(size, 1, subpriority) != NMF_OK)
      return NMF_NO_MORE_MEMORY;
    _xybc_fillthisbuffer[4].target = (armnmf_fillthisbufferDescriptor*)&_xyuv_Processor.fillthisbuffer[4];
    *reference = &_xybc_fillthisbuffer[4];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "fillthisbuffer[5]") == 0) {
    _xybc_fillthisbuffer[5].destroyFifo();            // Destroy server previous binding if any
    _xybc_fillthisbuffer[5].connect(57005 << 8, &_xyuv_Processor);
    if(_xybc_fillthisbuffer[5].createFifo(size, 1, subpriority) != NMF_OK)
      return NMF_NO_MORE_MEMORY;
    _xybc_fillthisbuffer[5].target = (armnmf_fillthisbufferDescriptor*)&_xyuv_Processor.fillthisbuffer[5];
    *reference = &_xybc_fillthisbuffer[5];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "fillthisbuffer[6]") == 0) {
    _xybc_fillthisbuffer[6].destroyFifo();            // Destroy server previous binding if any
    _xybc_fillthisbuffer[6].connect(57005 << 8, &_xyuv_Processor);
    if(_xybc_fillthisbuffer[6].createFifo(size, 1, subpriority) != NMF_OK)
      return NMF_NO_MORE_MEMORY;
    _xybc_fillthisbuffer[6].target = (armnmf_fillthisbufferDescriptor*)&_xyuv_Processor.fillthisbuffer[6];
    *reference = &_xybc_fillthisbuffer[6];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "fillthisbuffer[7]") == 0) {
    _xybc_fillthisbuffer[7].destroyFifo();            // Destroy server previous binding if any
    _xybc_fillthisbuffer[7].connect(57005 << 8, &_xyuv_Processor);
    if(_xybc_fillthisbuffer[7].createFifo(size, 1, subpriority) != NMF_OK)
      return NMF_NO_MORE_MEMORY;
    _xybc_fillthisbuffer[7].target = (armnmf_fillthisbufferDescriptor*)&_xyuv_Processor.fillthisbuffer[7];
    *reference = &_xybc_fillthisbuffer[7];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "fillthisbuffer[8]") == 0) {
    _xybc_fillthisbuffer[8].destroyFifo();            // Destroy server previous binding if any
    _xybc_fillthisbuffer[8].connect(57005 << 8, &_xyuv_Processor);
    if(_xybc_fillthisbuffer[8].createFifo(size, 1, subpriority) != NMF_OK)
      return NMF_NO_MORE_MEMORY;
    _xybc_fillthisbuffer[8].target = (armnmf_fillthisbufferDescriptor*)&_xyuv_Processor.fillthisbuffer[8];
    *reference = &_xybc_fillthisbuffer[8];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "fillthisbuffer[9]") == 0) {
    _xybc_fillthisbuffer[9].destroyFifo();            // Destroy server previous binding if any
    _xybc_fillthisbuffer[9].connect(57005 << 8, &_xyuv_Processor);
    if(_xybc_fillthisbuffer[9].createFifo(size, 1, subpriority) != NMF_OK)
      return NMF_NO_MORE_MEMORY;
    _xybc_fillthisbuffer[9].target = (armnmf_fillthisbufferDescriptor*)&_xyuv_Processor.fillthisbuffer[9];
    *reference = &_xybc_fillthisbuffer[9];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "sendcommand") == 0) {
    _xybc_sendcommand.destroyFifo();            // Destroy server previous binding if any
    _xybc_sendcommand.connect(1 << 8, &_xyuv_Processor);
    if(_xybc_sendcommand.createFifo(size, 1, subpriority) != NMF_OK)
      return NMF_NO_MORE_MEMORY;
    _xybc_sendcommand.target = (sendcommandDescriptor*)&_xyuv_Processor;
    *reference = &_xybc_sendcommand;
    return NMF_OK;
  }
  if(compositeStrcmp(name, "fsminit") == 0) {
    _xybc_fsminit.destroyFifo();            // Destroy server previous binding if any
    _xybc_fsminit.connect(0 << 8, &_xyuv_Processor);
    if(_xybc_fsminit.createFifo(size, 1, subpriority) != NMF_OK)
      return NMF_NO_MORE_MEMORY;
    _xybc_fsminit.target = (fsminitDescriptor*)&_xyuv_Processor;
    *reference = &_xybc_fsminit;
    return NMF_OK;
  }
  if(compositeStrcmp(name, "Config") == 0) {
    _xybc_Config.destroyFifo();            // Destroy server previous binding if any
    _xybc_Config.connect(8 << 8, &_xyuv_Processor);
    if(_xybc_Config.createFifo(size, 1, subpriority) != NMF_OK)
      return NMF_NO_MORE_MEMORY;
    _xybc_Config.target = (openmax_processor_api_ConfigDescriptor*)&_xyuv_Processor;
    *reference = &_xybc_Config;
    return NMF_OK;
  }
  if(compositeStrcmp(name, "Param") == 0) {
    _xybc_Param.destroyFifo();            // Destroy server previous binding if any
    _xybc_Param.connect(7 << 8, &_xyuv_Processor);
    if(_xybc_Param.createFifo(size, 1, subpriority) != NMF_OK)
      return NMF_NO_MORE_MEMORY;
    _xybc_Param.target = (openmax_processor_api_ParamDescriptor*)&_xyuv_Processor;
    *reference = &_xybc_Param;
    return NMF_OK;
  }
  return NMF_NO_SUCH_PROVIDED_INTERFACE;
}

t_nmf_error openmax_processor_wrpImpl::unbindFromUser(const char* name) {
  if(compositeStrcmp(name, "emptythisbuffer[0]") == 0) {
    return _xybc_emptythisbuffer[0].destroyFifo();
  }
  if(compositeStrcmp(name, "emptythisbuffer[1]") == 0) {
    return _xybc_emptythisbuffer[1].destroyFifo();
  }
  if(compositeStrcmp(name, "emptythisbuffer[2]") == 0) {
    return _xybc_emptythisbuffer[2].destroyFifo();
  }
  if(compositeStrcmp(name, "emptythisbuffer[3]") == 0) {
    return _xybc_emptythisbuffer[3].destroyFifo();
  }
  if(compositeStrcmp(name, "emptythisbuffer[4]") == 0) {
    return _xybc_emptythisbuffer[4].destroyFifo();
  }
  if(compositeStrcmp(name, "emptythisbuffer[5]") == 0) {
    return _xybc_emptythisbuffer[5].destroyFifo();
  }
  if(compositeStrcmp(name, "emptythisbuffer[6]") == 0) {
    return _xybc_emptythisbuffer[6].destroyFifo();
  }
  if(compositeStrcmp(name, "emptythisbuffer[7]") == 0) {
    return _xybc_emptythisbuffer[7].destroyFifo();
  }
  if(compositeStrcmp(name, "emptythisbuffer[8]") == 0) {
    return _xybc_emptythisbuffer[8].destroyFifo();
  }
  if(compositeStrcmp(name, "emptythisbuffer[9]") == 0) {
    return _xybc_emptythisbuffer[9].destroyFifo();
  }
  if(compositeStrcmp(name, "fillthisbuffer[0]") == 0) {
    return _xybc_fillthisbuffer[0].destroyFifo();
  }
  if(compositeStrcmp(name, "fillthisbuffer[1]") == 0) {
    return _xybc_fillthisbuffer[1].destroyFifo();
  }
  if(compositeStrcmp(name, "fillthisbuffer[2]") == 0) {
    return _xybc_fillthisbuffer[2].destroyFifo();
  }
  if(compositeStrcmp(name, "fillthisbuffer[3]") == 0) {
    return _xybc_fillthisbuffer[3].destroyFifo();
  }
  if(compositeStrcmp(name, "fillthisbuffer[4]") == 0) {
    return _xybc_fillthisbuffer[4].destroyFifo();
  }
  if(compositeStrcmp(name, "fillthisbuffer[5]") == 0) {
    return _xybc_fillthisbuffer[5].destroyFifo();
  }
  if(compositeStrcmp(name, "fillthisbuffer[6]") == 0) {
    return _xybc_fillthisbuffer[6].destroyFifo();
  }
  if(compositeStrcmp(name, "fillthisbuffer[7]") == 0) {
    return _xybc_fillthisbuffer[7].destroyFifo();
  }
  if(compositeStrcmp(name, "fillthisbuffer[8]") == 0) {
    return _xybc_fillthisbuffer[8].destroyFifo();
  }
  if(compositeStrcmp(name, "fillthisbuffer[9]") == 0) {
    return _xybc_fillthisbuffer[9].destroyFifo();
  }
  if(compositeStrcmp(name, "sendcommand") == 0) {
    return _xybc_sendcommand.destroyFifo();
  }
  if(compositeStrcmp(name, "fsminit") == 0) {
    return _xybc_fsminit.destroyFifo();
  }
  if(compositeStrcmp(name, "Config") == 0) {
    return _xybc_Config.destroyFifo();
  }
  if(compositeStrcmp(name, "Param") == 0) {
    return _xybc_Param.destroyFifo();
  }
  return NMF_NO_SUCH_PROVIDED_INTERFACE;
}

t_nmf_error openmax_processor_wrpImpl::bindToUser(t_nmf_channel channel, const char* compositeitfname, void *clientContext, unsigned int size) {
  t_nmf_error error = NMF_NO_SUCH_REQUIRED_INTERFACE;
  if(compositeStrcmp(compositeitfname, "emptybufferdone[0]") == 0) {
    _cbl_armnmf_fillthisbuffer* cbl;
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_emptybufferdone[0].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    cbl  = new _cbl_armnmf_fillthisbuffer;
    if(cbl == 0x0)
      return NMF_NO_MORE_MEMORY;
    cbl->name = "emptybufferdone[0] -> ";
    error = cbl->createFifo(channel, size, clientContext);
    if(error != NMF_OK) {
      delete cbl;
      return NMF_NO_MORE_MEMORY;
    }
    _xyuv_Processor.emptybufferdone[0] = cbl;
    if(error != NMF_OK) {
      cbl->destroyFifo(&clientContext);
      delete cbl;
      return error;
    }
    _xyitfcb_emptybufferdone[0] = cbl;
    return NMF_OK;
  }
  if(compositeStrcmp(compositeitfname, "emptybufferdone[1]") == 0) {
    _cbl_armnmf_fillthisbuffer* cbl;
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_emptybufferdone[1].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    cbl  = new _cbl_armnmf_fillthisbuffer;
    if(cbl == 0x0)
      return NMF_NO_MORE_MEMORY;
    cbl->name = "emptybufferdone[1] -> ";
    error = cbl->createFifo(channel, size, clientContext);
    if(error != NMF_OK) {
      delete cbl;
      return NMF_NO_MORE_MEMORY;
    }
    _xyuv_Processor.emptybufferdone[1] = cbl;
    if(error != NMF_OK) {
      cbl->destroyFifo(&clientContext);
      delete cbl;
      return error;
    }
    _xyitfcb_emptybufferdone[1] = cbl;
    return NMF_OK;
  }
  if(compositeStrcmp(compositeitfname, "emptybufferdone[2]") == 0) {
    _cbl_armnmf_fillthisbuffer* cbl;
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_emptybufferdone[2].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    cbl  = new _cbl_armnmf_fillthisbuffer;
    if(cbl == 0x0)
      return NMF_NO_MORE_MEMORY;
    cbl->name = "emptybufferdone[2] -> ";
    error = cbl->createFifo(channel, size, clientContext);
    if(error != NMF_OK) {
      delete cbl;
      return NMF_NO_MORE_MEMORY;
    }
    _xyuv_Processor.emptybufferdone[2] = cbl;
    if(error != NMF_OK) {
      cbl->destroyFifo(&clientContext);
      delete cbl;
      return error;
    }
    _xyitfcb_emptybufferdone[2] = cbl;
    return NMF_OK;
  }
  if(compositeStrcmp(compositeitfname, "emptybufferdone[3]") == 0) {
    _cbl_armnmf_fillthisbuffer* cbl;
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_emptybufferdone[3].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    cbl  = new _cbl_armnmf_fillthisbuffer;
    if(cbl == 0x0)
      return NMF_NO_MORE_MEMORY;
    cbl->name = "emptybufferdone[3] -> ";
    error = cbl->createFifo(channel, size, clientContext);
    if(error != NMF_OK) {
      delete cbl;
      return NMF_NO_MORE_MEMORY;
    }
    _xyuv_Processor.emptybufferdone[3] = cbl;
    if(error != NMF_OK) {
      cbl->destroyFifo(&clientContext);
      delete cbl;
      return error;
    }
    _xyitfcb_emptybufferdone[3] = cbl;
    return NMF_OK;
  }
  if(compositeStrcmp(compositeitfname, "emptybufferdone[4]") == 0) {
    _cbl_armnmf_fillthisbuffer* cbl;
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_emptybufferdone[4].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    cbl  = new _cbl_armnmf_fillthisbuffer;
    if(cbl == 0x0)
      return NMF_NO_MORE_MEMORY;
    cbl->name = "emptybufferdone[4] -> ";
    error = cbl->createFifo(channel, size, clientContext);
    if(error != NMF_OK) {
      delete cbl;
      return NMF_NO_MORE_MEMORY;
    }
    _xyuv_Processor.emptybufferdone[4] = cbl;
    if(error != NMF_OK) {
      cbl->destroyFifo(&clientContext);
      delete cbl;
      return error;
    }
    _xyitfcb_emptybufferdone[4] = cbl;
    return NMF_OK;
  }
  if(compositeStrcmp(compositeitfname, "emptybufferdone[5]") == 0) {
    _cbl_armnmf_fillthisbuffer* cbl;
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_emptybufferdone[5].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    cbl  = new _cbl_armnmf_fillthisbuffer;
    if(cbl == 0x0)
      return NMF_NO_MORE_MEMORY;
    cbl->name = "emptybufferdone[5] -> ";
    error = cbl->createFifo(channel, size, clientContext);
    if(error != NMF_OK) {
      delete cbl;
      return NMF_NO_MORE_MEMORY;
    }
    _xyuv_Processor.emptybufferdone[5] = cbl;
    if(error != NMF_OK) {
      cbl->destroyFifo(&clientContext);
      delete cbl;
      return error;
    }
    _xyitfcb_emptybufferdone[5] = cbl;
    return NMF_OK;
  }
  if(compositeStrcmp(compositeitfname, "emptybufferdone[6]") == 0) {
    _cbl_armnmf_fillthisbuffer* cbl;
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_emptybufferdone[6].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    cbl  = new _cbl_armnmf_fillthisbuffer;
    if(cbl == 0x0)
      return NMF_NO_MORE_MEMORY;
    cbl->name = "emptybufferdone[6] -> ";
    error = cbl->createFifo(channel, size, clientContext);
    if(error != NMF_OK) {
      delete cbl;
      return NMF_NO_MORE_MEMORY;
    }
    _xyuv_Processor.emptybufferdone[6] = cbl;
    if(error != NMF_OK) {
      cbl->destroyFifo(&clientContext);
      delete cbl;
      return error;
    }
    _xyitfcb_emptybufferdone[6] = cbl;
    return NMF_OK;
  }
  if(compositeStrcmp(compositeitfname, "emptybufferdone[7]") == 0) {
    _cbl_armnmf_fillthisbuffer* cbl;
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_emptybufferdone[7].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    cbl  = new _cbl_armnmf_fillthisbuffer;
    if(cbl == 0x0)
      return NMF_NO_MORE_MEMORY;
    cbl->name = "emptybufferdone[7] -> ";
    error = cbl->createFifo(channel, size, clientContext);
    if(error != NMF_OK) {
      delete cbl;
      return NMF_NO_MORE_MEMORY;
    }
    _xyuv_Processor.emptybufferdone[7] = cbl;
    if(error != NMF_OK) {
      cbl->destroyFifo(&clientContext);
      delete cbl;
      return error;
    }
    _xyitfcb_emptybufferdone[7] = cbl;
    return NMF_OK;
  }
  if(compositeStrcmp(compositeitfname, "emptybufferdone[8]") == 0) {
    _cbl_armnmf_fillthisbuffer* cbl;
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_emptybufferdone[8].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    cbl  = new _cbl_armnmf_fillthisbuffer;
    if(cbl == 0x0)
      return NMF_NO_MORE_MEMORY;
    cbl->name = "emptybufferdone[8] -> ";
    error = cbl->createFifo(channel, size, clientContext);
    if(error != NMF_OK) {
      delete cbl;
      return NMF_NO_MORE_MEMORY;
    }
    _xyuv_Processor.emptybufferdone[8] = cbl;
    if(error != NMF_OK) {
      cbl->destroyFifo(&clientContext);
      delete cbl;
      return error;
    }
    _xyitfcb_emptybufferdone[8] = cbl;
    return NMF_OK;
  }
  if(compositeStrcmp(compositeitfname, "emptybufferdone[9]") == 0) {
    _cbl_armnmf_fillthisbuffer* cbl;
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_emptybufferdone[9].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    cbl  = new _cbl_armnmf_fillthisbuffer;
    if(cbl == 0x0)
      return NMF_NO_MORE_MEMORY;
    cbl->name = "emptybufferdone[9] -> ";
    error = cbl->createFifo(channel, size, clientContext);
    if(error != NMF_OK) {
      delete cbl;
      return NMF_NO_MORE_MEMORY;
    }
    _xyuv_Processor.emptybufferdone[9] = cbl;
    if(error != NMF_OK) {
      cbl->destroyFifo(&clientContext);
      delete cbl;
      return error;
    }
    _xyitfcb_emptybufferdone[9] = cbl;
    return NMF_OK;
  }
  if(compositeStrcmp(compositeitfname, "fillbufferdone[0]") == 0) {
    _cbl_armnmf_emptythisbuffer* cbl;
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_fillbufferdone[0].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    cbl  = new _cbl_armnmf_emptythisbuffer;
    if(cbl == 0x0)
      return NMF_NO_MORE_MEMORY;
    cbl->name = "fillbufferdone[0] -> ";
    error = cbl->createFifo(channel, size, clientContext);
    if(error != NMF_OK) {
      delete cbl;
      return NMF_NO_MORE_MEMORY;
    }
    _xyuv_Processor.fillbufferdone[0] = cbl;
    if(error != NMF_OK) {
      cbl->destroyFifo(&clientContext);
      delete cbl;
      return error;
    }
    _xyitfcb_fillbufferdone[0] = cbl;
    return NMF_OK;
  }
  if(compositeStrcmp(compositeitfname, "fillbufferdone[1]") == 0) {
    _cbl_armnmf_emptythisbuffer* cbl;
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_fillbufferdone[1].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    cbl  = new _cbl_armnmf_emptythisbuffer;
    if(cbl == 0x0)
      return NMF_NO_MORE_MEMORY;
    cbl->name = "fillbufferdone[1] -> ";
    error = cbl->createFifo(channel, size, clientContext);
    if(error != NMF_OK) {
      delete cbl;
      return NMF_NO_MORE_MEMORY;
    }
    _xyuv_Processor.fillbufferdone[1] = cbl;
    if(error != NMF_OK) {
      cbl->destroyFifo(&clientContext);
      delete cbl;
      return error;
    }
    _xyitfcb_fillbufferdone[1] = cbl;
    return NMF_OK;
  }
  if(compositeStrcmp(compositeitfname, "fillbufferdone[2]") == 0) {
    _cbl_armnmf_emptythisbuffer* cbl;
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_fillbufferdone[2].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    cbl  = new _cbl_armnmf_emptythisbuffer;
    if(cbl == 0x0)
      return NMF_NO_MORE_MEMORY;
    cbl->name = "fillbufferdone[2] -> ";
    error = cbl->createFifo(channel, size, clientContext);
    if(error != NMF_OK) {
      delete cbl;
      return NMF_NO_MORE_MEMORY;
    }
    _xyuv_Processor.fillbufferdone[2] = cbl;
    if(error != NMF_OK) {
      cbl->destroyFifo(&clientContext);
      delete cbl;
      return error;
    }
    _xyitfcb_fillbufferdone[2] = cbl;
    return NMF_OK;
  }
  if(compositeStrcmp(compositeitfname, "fillbufferdone[3]") == 0) {
    _cbl_armnmf_emptythisbuffer* cbl;
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_fillbufferdone[3].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    cbl  = new _cbl_armnmf_emptythisbuffer;
    if(cbl == 0x0)
      return NMF_NO_MORE_MEMORY;
    cbl->name = "fillbufferdone[3] -> ";
    error = cbl->createFifo(channel, size, clientContext);
    if(error != NMF_OK) {
      delete cbl;
      return NMF_NO_MORE_MEMORY;
    }
    _xyuv_Processor.fillbufferdone[3] = cbl;
    if(error != NMF_OK) {
      cbl->destroyFifo(&clientContext);
      delete cbl;
      return error;
    }
    _xyitfcb_fillbufferdone[3] = cbl;
    return NMF_OK;
  }
  if(compositeStrcmp(compositeitfname, "fillbufferdone[4]") == 0) {
    _cbl_armnmf_emptythisbuffer* cbl;
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_fillbufferdone[4].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    cbl  = new _cbl_armnmf_emptythisbuffer;
    if(cbl == 0x0)
      return NMF_NO_MORE_MEMORY;
    cbl->name = "fillbufferdone[4] -> ";
    error = cbl->createFifo(channel, size, clientContext);
    if(error != NMF_OK) {
      delete cbl;
      return NMF_NO_MORE_MEMORY;
    }
    _xyuv_Processor.fillbufferdone[4] = cbl;
    if(error != NMF_OK) {
      cbl->destroyFifo(&clientContext);
      delete cbl;
      return error;
    }
    _xyitfcb_fillbufferdone[4] = cbl;
    return NMF_OK;
  }
  if(compositeStrcmp(compositeitfname, "fillbufferdone[5]") == 0) {
    _cbl_armnmf_emptythisbuffer* cbl;
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_fillbufferdone[5].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    cbl  = new _cbl_armnmf_emptythisbuffer;
    if(cbl == 0x0)
      return NMF_NO_MORE_MEMORY;
    cbl->name = "fillbufferdone[5] -> ";
    error = cbl->createFifo(channel, size, clientContext);
    if(error != NMF_OK) {
      delete cbl;
      return NMF_NO_MORE_MEMORY;
    }
    _xyuv_Processor.fillbufferdone[5] = cbl;
    if(error != NMF_OK) {
      cbl->destroyFifo(&clientContext);
      delete cbl;
      return error;
    }
    _xyitfcb_fillbufferdone[5] = cbl;
    return NMF_OK;
  }
  if(compositeStrcmp(compositeitfname, "fillbufferdone[6]") == 0) {
    _cbl_armnmf_emptythisbuffer* cbl;
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_fillbufferdone[6].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    cbl  = new _cbl_armnmf_emptythisbuffer;
    if(cbl == 0x0)
      return NMF_NO_MORE_MEMORY;
    cbl->name = "fillbufferdone[6] -> ";
    error = cbl->createFifo(channel, size, clientContext);
    if(error != NMF_OK) {
      delete cbl;
      return NMF_NO_MORE_MEMORY;
    }
    _xyuv_Processor.fillbufferdone[6] = cbl;
    if(error != NMF_OK) {
      cbl->destroyFifo(&clientContext);
      delete cbl;
      return error;
    }
    _xyitfcb_fillbufferdone[6] = cbl;
    return NMF_OK;
  }
  if(compositeStrcmp(compositeitfname, "fillbufferdone[7]") == 0) {
    _cbl_armnmf_emptythisbuffer* cbl;
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_fillbufferdone[7].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    cbl  = new _cbl_armnmf_emptythisbuffer;
    if(cbl == 0x0)
      return NMF_NO_MORE_MEMORY;
    cbl->name = "fillbufferdone[7] -> ";
    error = cbl->createFifo(channel, size, clientContext);
    if(error != NMF_OK) {
      delete cbl;
      return NMF_NO_MORE_MEMORY;
    }
    _xyuv_Processor.fillbufferdone[7] = cbl;
    if(error != NMF_OK) {
      cbl->destroyFifo(&clientContext);
      delete cbl;
      return error;
    }
    _xyitfcb_fillbufferdone[7] = cbl;
    return NMF_OK;
  }
  if(compositeStrcmp(compositeitfname, "fillbufferdone[8]") == 0) {
    _cbl_armnmf_emptythisbuffer* cbl;
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_fillbufferdone[8].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    cbl  = new _cbl_armnmf_emptythisbuffer;
    if(cbl == 0x0)
      return NMF_NO_MORE_MEMORY;
    cbl->name = "fillbufferdone[8] -> ";
    error = cbl->createFifo(channel, size, clientContext);
    if(error != NMF_OK) {
      delete cbl;
      return NMF_NO_MORE_MEMORY;
    }
    _xyuv_Processor.fillbufferdone[8] = cbl;
    if(error != NMF_OK) {
      cbl->destroyFifo(&clientContext);
      delete cbl;
      return error;
    }
    _xyitfcb_fillbufferdone[8] = cbl;
    return NMF_OK;
  }
  if(compositeStrcmp(compositeitfname, "fillbufferdone[9]") == 0) {
    _cbl_armnmf_emptythisbuffer* cbl;
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_fillbufferdone[9].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    cbl  = new _cbl_armnmf_emptythisbuffer;
    if(cbl == 0x0)
      return NMF_NO_MORE_MEMORY;
    cbl->name = "fillbufferdone[9] -> ";
    error = cbl->createFifo(channel, size, clientContext);
    if(error != NMF_OK) {
      delete cbl;
      return NMF_NO_MORE_MEMORY;
    }
    _xyuv_Processor.fillbufferdone[9] = cbl;
    if(error != NMF_OK) {
      cbl->destroyFifo(&clientContext);
      delete cbl;
      return error;
    }
    _xyitfcb_fillbufferdone[9] = cbl;
    return NMF_OK;
  }
  if(compositeStrcmp(compositeitfname, "proxy") == 0) {
    _cbl_eventhandler* cbl;
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_proxy.IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    cbl  = new _cbl_eventhandler;
    if(cbl == 0x0)
      return NMF_NO_MORE_MEMORY;
    cbl->name = "proxy -> ";
    error = cbl->createFifo(channel, size, clientContext);
    if(error != NMF_OK) {
      delete cbl;
      return NMF_NO_MORE_MEMORY;
    }
    _xyuv_Processor.proxy = cbl;
    if(error != NMF_OK) {
      cbl->destroyFifo(&clientContext);
      delete cbl;
      return error;
    }
    _xyitfcb_proxy = cbl;
    return NMF_OK;
  }
  if(compositeStrcmp(compositeitfname, "ToOMXComponent") == 0) {
    _cbl_openmax_processor_api_ToOMXComponent* cbl;
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_ToOMXComponent.IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    cbl  = new _cbl_openmax_processor_api_ToOMXComponent;
    if(cbl == 0x0)
      return NMF_NO_MORE_MEMORY;
    cbl->name = "ToOMXComponent -> ";
    error = cbl->createFifo(channel, size, clientContext);
    if(error != NMF_OK) {
      delete cbl;
      return NMF_NO_MORE_MEMORY;
    }
    _xyuv_Processor.ToOMXComponent = cbl;
    if(error != NMF_OK) {
      cbl->destroyFifo(&clientContext);
      delete cbl;
      return error;
    }
    _xyitfcb_ToOMXComponent = cbl;
    return NMF_OK;
  }
  return error;
}

t_nmf_error openmax_processor_wrpImpl::unbindToUser(t_nmf_channel channel, const char* compositeitfname, void **clientContext) {
  if(compositeStrcmp(compositeitfname, "emptybufferdone[0]") == 0) {
    _cbl_armnmf_fillthisbuffer* cbl;
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_emptybufferdone[0].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    cbl  = (_cbl_armnmf_fillthisbuffer*)_xyitfcb_emptybufferdone[0].getReference();
    _xyuv_Processor.emptybufferdone[0] = 0x0;
    t_nmf_error error = cbl->destroyFifo(clientContext);
    if(error == NMF_OK)
      delete cbl;
    _xyitfcb_emptybufferdone[0] = 0x0;
    return error;
  }
  if(compositeStrcmp(compositeitfname, "emptybufferdone[1]") == 0) {
    _cbl_armnmf_fillthisbuffer* cbl;
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_emptybufferdone[1].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    cbl  = (_cbl_armnmf_fillthisbuffer*)_xyitfcb_emptybufferdone[1].getReference();
    _xyuv_Processor.emptybufferdone[1] = 0x0;
    t_nmf_error error = cbl->destroyFifo(clientContext);
    if(error == NMF_OK)
      delete cbl;
    _xyitfcb_emptybufferdone[1] = 0x0;
    return error;
  }
  if(compositeStrcmp(compositeitfname, "emptybufferdone[2]") == 0) {
    _cbl_armnmf_fillthisbuffer* cbl;
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_emptybufferdone[2].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    cbl  = (_cbl_armnmf_fillthisbuffer*)_xyitfcb_emptybufferdone[2].getReference();
    _xyuv_Processor.emptybufferdone[2] = 0x0;
    t_nmf_error error = cbl->destroyFifo(clientContext);
    if(error == NMF_OK)
      delete cbl;
    _xyitfcb_emptybufferdone[2] = 0x0;
    return error;
  }
  if(compositeStrcmp(compositeitfname, "emptybufferdone[3]") == 0) {
    _cbl_armnmf_fillthisbuffer* cbl;
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_emptybufferdone[3].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    cbl  = (_cbl_armnmf_fillthisbuffer*)_xyitfcb_emptybufferdone[3].getReference();
    _xyuv_Processor.emptybufferdone[3] = 0x0;
    t_nmf_error error = cbl->destroyFifo(clientContext);
    if(error == NMF_OK)
      delete cbl;
    _xyitfcb_emptybufferdone[3] = 0x0;
    return error;
  }
  if(compositeStrcmp(compositeitfname, "emptybufferdone[4]") == 0) {
    _cbl_armnmf_fillthisbuffer* cbl;
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_emptybufferdone[4].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    cbl  = (_cbl_armnmf_fillthisbuffer*)_xyitfcb_emptybufferdone[4].getReference();
    _xyuv_Processor.emptybufferdone[4] = 0x0;
    t_nmf_error error = cbl->destroyFifo(clientContext);
    if(error == NMF_OK)
      delete cbl;
    _xyitfcb_emptybufferdone[4] = 0x0;
    return error;
  }
  if(compositeStrcmp(compositeitfname, "emptybufferdone[5]") == 0) {
    _cbl_armnmf_fillthisbuffer* cbl;
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_emptybufferdone[5].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    cbl  = (_cbl_armnmf_fillthisbuffer*)_xyitfcb_emptybufferdone[5].getReference();
    _xyuv_Processor.emptybufferdone[5] = 0x0;
    t_nmf_error error = cbl->destroyFifo(clientContext);
    if(error == NMF_OK)
      delete cbl;
    _xyitfcb_emptybufferdone[5] = 0x0;
    return error;
  }
  if(compositeStrcmp(compositeitfname, "emptybufferdone[6]") == 0) {
    _cbl_armnmf_fillthisbuffer* cbl;
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_emptybufferdone[6].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    cbl  = (_cbl_armnmf_fillthisbuffer*)_xyitfcb_emptybufferdone[6].getReference();
    _xyuv_Processor.emptybufferdone[6] = 0x0;
    t_nmf_error error = cbl->destroyFifo(clientContext);
    if(error == NMF_OK)
      delete cbl;
    _xyitfcb_emptybufferdone[6] = 0x0;
    return error;
  }
  if(compositeStrcmp(compositeitfname, "emptybufferdone[7]") == 0) {
    _cbl_armnmf_fillthisbuffer* cbl;
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_emptybufferdone[7].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    cbl  = (_cbl_armnmf_fillthisbuffer*)_xyitfcb_emptybufferdone[7].getReference();
    _xyuv_Processor.emptybufferdone[7] = 0x0;
    t_nmf_error error = cbl->destroyFifo(clientContext);
    if(error == NMF_OK)
      delete cbl;
    _xyitfcb_emptybufferdone[7] = 0x0;
    return error;
  }
  if(compositeStrcmp(compositeitfname, "emptybufferdone[8]") == 0) {
    _cbl_armnmf_fillthisbuffer* cbl;
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_emptybufferdone[8].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    cbl  = (_cbl_armnmf_fillthisbuffer*)_xyitfcb_emptybufferdone[8].getReference();
    _xyuv_Processor.emptybufferdone[8] = 0x0;
    t_nmf_error error = cbl->destroyFifo(clientContext);
    if(error == NMF_OK)
      delete cbl;
    _xyitfcb_emptybufferdone[8] = 0x0;
    return error;
  }
  if(compositeStrcmp(compositeitfname, "emptybufferdone[9]") == 0) {
    _cbl_armnmf_fillthisbuffer* cbl;
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_emptybufferdone[9].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    cbl  = (_cbl_armnmf_fillthisbuffer*)_xyitfcb_emptybufferdone[9].getReference();
    _xyuv_Processor.emptybufferdone[9] = 0x0;
    t_nmf_error error = cbl->destroyFifo(clientContext);
    if(error == NMF_OK)
      delete cbl;
    _xyitfcb_emptybufferdone[9] = 0x0;
    return error;
  }
  if(compositeStrcmp(compositeitfname, "fillbufferdone[0]") == 0) {
    _cbl_armnmf_emptythisbuffer* cbl;
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_fillbufferdone[0].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    cbl  = (_cbl_armnmf_emptythisbuffer*)_xyitfcb_fillbufferdone[0].getReference();
    _xyuv_Processor.fillbufferdone[0] = 0x0;
    t_nmf_error error = cbl->destroyFifo(clientContext);
    if(error == NMF_OK)
      delete cbl;
    _xyitfcb_fillbufferdone[0] = 0x0;
    return error;
  }
  if(compositeStrcmp(compositeitfname, "fillbufferdone[1]") == 0) {
    _cbl_armnmf_emptythisbuffer* cbl;
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_fillbufferdone[1].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    cbl  = (_cbl_armnmf_emptythisbuffer*)_xyitfcb_fillbufferdone[1].getReference();
    _xyuv_Processor.fillbufferdone[1] = 0x0;
    t_nmf_error error = cbl->destroyFifo(clientContext);
    if(error == NMF_OK)
      delete cbl;
    _xyitfcb_fillbufferdone[1] = 0x0;
    return error;
  }
  if(compositeStrcmp(compositeitfname, "fillbufferdone[2]") == 0) {
    _cbl_armnmf_emptythisbuffer* cbl;
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_fillbufferdone[2].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    cbl  = (_cbl_armnmf_emptythisbuffer*)_xyitfcb_fillbufferdone[2].getReference();
    _xyuv_Processor.fillbufferdone[2] = 0x0;
    t_nmf_error error = cbl->destroyFifo(clientContext);
    if(error == NMF_OK)
      delete cbl;
    _xyitfcb_fillbufferdone[2] = 0x0;
    return error;
  }
  if(compositeStrcmp(compositeitfname, "fillbufferdone[3]") == 0) {
    _cbl_armnmf_emptythisbuffer* cbl;
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_fillbufferdone[3].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    cbl  = (_cbl_armnmf_emptythisbuffer*)_xyitfcb_fillbufferdone[3].getReference();
    _xyuv_Processor.fillbufferdone[3] = 0x0;
    t_nmf_error error = cbl->destroyFifo(clientContext);
    if(error == NMF_OK)
      delete cbl;
    _xyitfcb_fillbufferdone[3] = 0x0;
    return error;
  }
  if(compositeStrcmp(compositeitfname, "fillbufferdone[4]") == 0) {
    _cbl_armnmf_emptythisbuffer* cbl;
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_fillbufferdone[4].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    cbl  = (_cbl_armnmf_emptythisbuffer*)_xyitfcb_fillbufferdone[4].getReference();
    _xyuv_Processor.fillbufferdone[4] = 0x0;
    t_nmf_error error = cbl->destroyFifo(clientContext);
    if(error == NMF_OK)
      delete cbl;
    _xyitfcb_fillbufferdone[4] = 0x0;
    return error;
  }
  if(compositeStrcmp(compositeitfname, "fillbufferdone[5]") == 0) {
    _cbl_armnmf_emptythisbuffer* cbl;
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_fillbufferdone[5].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    cbl  = (_cbl_armnmf_emptythisbuffer*)_xyitfcb_fillbufferdone[5].getReference();
    _xyuv_Processor.fillbufferdone[5] = 0x0;
    t_nmf_error error = cbl->destroyFifo(clientContext);
    if(error == NMF_OK)
      delete cbl;
    _xyitfcb_fillbufferdone[5] = 0x0;
    return error;
  }
  if(compositeStrcmp(compositeitfname, "fillbufferdone[6]") == 0) {
    _cbl_armnmf_emptythisbuffer* cbl;
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_fillbufferdone[6].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    cbl  = (_cbl_armnmf_emptythisbuffer*)_xyitfcb_fillbufferdone[6].getReference();
    _xyuv_Processor.fillbufferdone[6] = 0x0;
    t_nmf_error error = cbl->destroyFifo(clientContext);
    if(error == NMF_OK)
      delete cbl;
    _xyitfcb_fillbufferdone[6] = 0x0;
    return error;
  }
  if(compositeStrcmp(compositeitfname, "fillbufferdone[7]") == 0) {
    _cbl_armnmf_emptythisbuffer* cbl;
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_fillbufferdone[7].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    cbl  = (_cbl_armnmf_emptythisbuffer*)_xyitfcb_fillbufferdone[7].getReference();
    _xyuv_Processor.fillbufferdone[7] = 0x0;
    t_nmf_error error = cbl->destroyFifo(clientContext);
    if(error == NMF_OK)
      delete cbl;
    _xyitfcb_fillbufferdone[7] = 0x0;
    return error;
  }
  if(compositeStrcmp(compositeitfname, "fillbufferdone[8]") == 0) {
    _cbl_armnmf_emptythisbuffer* cbl;
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_fillbufferdone[8].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    cbl  = (_cbl_armnmf_emptythisbuffer*)_xyitfcb_fillbufferdone[8].getReference();
    _xyuv_Processor.fillbufferdone[8] = 0x0;
    t_nmf_error error = cbl->destroyFifo(clientContext);
    if(error == NMF_OK)
      delete cbl;
    _xyitfcb_fillbufferdone[8] = 0x0;
    return error;
  }
  if(compositeStrcmp(compositeitfname, "fillbufferdone[9]") == 0) {
    _cbl_armnmf_emptythisbuffer* cbl;
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_fillbufferdone[9].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    cbl  = (_cbl_armnmf_emptythisbuffer*)_xyitfcb_fillbufferdone[9].getReference();
    _xyuv_Processor.fillbufferdone[9] = 0x0;
    t_nmf_error error = cbl->destroyFifo(clientContext);
    if(error == NMF_OK)
      delete cbl;
    _xyitfcb_fillbufferdone[9] = 0x0;
    return error;
  }
  if(compositeStrcmp(compositeitfname, "proxy") == 0) {
    _cbl_eventhandler* cbl;
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_proxy.IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    cbl  = (_cbl_eventhandler*)_xyitfcb_proxy.getReference();
    _xyuv_Processor.proxy = 0x0;
    t_nmf_error error = cbl->destroyFifo(clientContext);
    if(error == NMF_OK)
      delete cbl;
    _xyitfcb_proxy = 0x0;
    return error;
  }
  if(compositeStrcmp(compositeitfname, "ToOMXComponent") == 0) {
    _cbl_openmax_processor_api_ToOMXComponent* cbl;
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_ToOMXComponent.IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    cbl  = (_cbl_openmax_processor_api_ToOMXComponent*)_xyitfcb_ToOMXComponent.getReference();
    _xyuv_Processor.ToOMXComponent = 0x0;
    t_nmf_error error = cbl->destroyFifo(clientContext);
    if(error == NMF_OK)
      delete cbl;
    _xyitfcb_ToOMXComponent = 0x0;
    return error;
  }
  return NMF_NO_SUCH_REQUIRED_INTERFACE;
}

t_nmf_error openmax_processor_wrpImpl::bindAsynchronous(const char* name, unsigned int size, NMF::Composite* target, const char* targetname, unsigned int subpriority) {
  t_nmf_error error = NMF_NO_SUCH_REQUIRED_INTERFACE;

  // Check if server was a MPC
  char realname[MAX_INTERFACE_NAME_LENGTH];
  t_nmf_component_handle server = target->getComponentInterface(targetname, realname);
  if(server != 0x0) 
    return bindToMPC(name, server, realname, size);

  if(compositeStrcmp(name, "emptybufferdone[0]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_emptybufferdone[0].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    error = target->bindFromUser(targetname, size, &_xyitfcb_emptybufferdone[0], subpriority);
    if(error != NMF_OK)
      return error;
    _xyuv_Processor.emptybufferdone[0] = _xyitfcb_emptybufferdone[0];
    if(error != NMF_OK) {
      return error;
    }
    return error;
  }
  if(compositeStrcmp(name, "emptybufferdone[1]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_emptybufferdone[1].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    error = target->bindFromUser(targetname, size, &_xyitfcb_emptybufferdone[1], subpriority);
    if(error != NMF_OK)
      return error;
    _xyuv_Processor.emptybufferdone[1] = _xyitfcb_emptybufferdone[1];
    if(error != NMF_OK) {
      return error;
    }
    return error;
  }
  if(compositeStrcmp(name, "emptybufferdone[2]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_emptybufferdone[2].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    error = target->bindFromUser(targetname, size, &_xyitfcb_emptybufferdone[2], subpriority);
    if(error != NMF_OK)
      return error;
    _xyuv_Processor.emptybufferdone[2] = _xyitfcb_emptybufferdone[2];
    if(error != NMF_OK) {
      return error;
    }
    return error;
  }
  if(compositeStrcmp(name, "emptybufferdone[3]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_emptybufferdone[3].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    error = target->bindFromUser(targetname, size, &_xyitfcb_emptybufferdone[3], subpriority);
    if(error != NMF_OK)
      return error;
    _xyuv_Processor.emptybufferdone[3] = _xyitfcb_emptybufferdone[3];
    if(error != NMF_OK) {
      return error;
    }
    return error;
  }
  if(compositeStrcmp(name, "emptybufferdone[4]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_emptybufferdone[4].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    error = target->bindFromUser(targetname, size, &_xyitfcb_emptybufferdone[4], subpriority);
    if(error != NMF_OK)
      return error;
    _xyuv_Processor.emptybufferdone[4] = _xyitfcb_emptybufferdone[4];
    if(error != NMF_OK) {
      return error;
    }
    return error;
  }
  if(compositeStrcmp(name, "emptybufferdone[5]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_emptybufferdone[5].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    error = target->bindFromUser(targetname, size, &_xyitfcb_emptybufferdone[5], subpriority);
    if(error != NMF_OK)
      return error;
    _xyuv_Processor.emptybufferdone[5] = _xyitfcb_emptybufferdone[5];
    if(error != NMF_OK) {
      return error;
    }
    return error;
  }
  if(compositeStrcmp(name, "emptybufferdone[6]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_emptybufferdone[6].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    error = target->bindFromUser(targetname, size, &_xyitfcb_emptybufferdone[6], subpriority);
    if(error != NMF_OK)
      return error;
    _xyuv_Processor.emptybufferdone[6] = _xyitfcb_emptybufferdone[6];
    if(error != NMF_OK) {
      return error;
    }
    return error;
  }
  if(compositeStrcmp(name, "emptybufferdone[7]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_emptybufferdone[7].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    error = target->bindFromUser(targetname, size, &_xyitfcb_emptybufferdone[7], subpriority);
    if(error != NMF_OK)
      return error;
    _xyuv_Processor.emptybufferdone[7] = _xyitfcb_emptybufferdone[7];
    if(error != NMF_OK) {
      return error;
    }
    return error;
  }
  if(compositeStrcmp(name, "emptybufferdone[8]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_emptybufferdone[8].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    error = target->bindFromUser(targetname, size, &_xyitfcb_emptybufferdone[8], subpriority);
    if(error != NMF_OK)
      return error;
    _xyuv_Processor.emptybufferdone[8] = _xyitfcb_emptybufferdone[8];
    if(error != NMF_OK) {
      return error;
    }
    return error;
  }
  if(compositeStrcmp(name, "emptybufferdone[9]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_emptybufferdone[9].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    error = target->bindFromUser(targetname, size, &_xyitfcb_emptybufferdone[9], subpriority);
    if(error != NMF_OK)
      return error;
    _xyuv_Processor.emptybufferdone[9] = _xyitfcb_emptybufferdone[9];
    if(error != NMF_OK) {
      return error;
    }
    return error;
  }
  if(compositeStrcmp(name, "fillbufferdone[0]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_fillbufferdone[0].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    error = target->bindFromUser(targetname, size, &_xyitfcb_fillbufferdone[0], subpriority);
    if(error != NMF_OK)
      return error;
    _xyuv_Processor.fillbufferdone[0] = _xyitfcb_fillbufferdone[0];
    if(error != NMF_OK) {
      return error;
    }
    return error;
  }
  if(compositeStrcmp(name, "fillbufferdone[1]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_fillbufferdone[1].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    error = target->bindFromUser(targetname, size, &_xyitfcb_fillbufferdone[1], subpriority);
    if(error != NMF_OK)
      return error;
    _xyuv_Processor.fillbufferdone[1] = _xyitfcb_fillbufferdone[1];
    if(error != NMF_OK) {
      return error;
    }
    return error;
  }
  if(compositeStrcmp(name, "fillbufferdone[2]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_fillbufferdone[2].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    error = target->bindFromUser(targetname, size, &_xyitfcb_fillbufferdone[2], subpriority);
    if(error != NMF_OK)
      return error;
    _xyuv_Processor.fillbufferdone[2] = _xyitfcb_fillbufferdone[2];
    if(error != NMF_OK) {
      return error;
    }
    return error;
  }
  if(compositeStrcmp(name, "fillbufferdone[3]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_fillbufferdone[3].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    error = target->bindFromUser(targetname, size, &_xyitfcb_fillbufferdone[3], subpriority);
    if(error != NMF_OK)
      return error;
    _xyuv_Processor.fillbufferdone[3] = _xyitfcb_fillbufferdone[3];
    if(error != NMF_OK) {
      return error;
    }
    return error;
  }
  if(compositeStrcmp(name, "fillbufferdone[4]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_fillbufferdone[4].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    error = target->bindFromUser(targetname, size, &_xyitfcb_fillbufferdone[4], subpriority);
    if(error != NMF_OK)
      return error;
    _xyuv_Processor.fillbufferdone[4] = _xyitfcb_fillbufferdone[4];
    if(error != NMF_OK) {
      return error;
    }
    return error;
  }
  if(compositeStrcmp(name, "fillbufferdone[5]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_fillbufferdone[5].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    error = target->bindFromUser(targetname, size, &_xyitfcb_fillbufferdone[5], subpriority);
    if(error != NMF_OK)
      return error;
    _xyuv_Processor.fillbufferdone[5] = _xyitfcb_fillbufferdone[5];
    if(error != NMF_OK) {
      return error;
    }
    return error;
  }
  if(compositeStrcmp(name, "fillbufferdone[6]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_fillbufferdone[6].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    error = target->bindFromUser(targetname, size, &_xyitfcb_fillbufferdone[6], subpriority);
    if(error != NMF_OK)
      return error;
    _xyuv_Processor.fillbufferdone[6] = _xyitfcb_fillbufferdone[6];
    if(error != NMF_OK) {
      return error;
    }
    return error;
  }
  if(compositeStrcmp(name, "fillbufferdone[7]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_fillbufferdone[7].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    error = target->bindFromUser(targetname, size, &_xyitfcb_fillbufferdone[7], subpriority);
    if(error != NMF_OK)
      return error;
    _xyuv_Processor.fillbufferdone[7] = _xyitfcb_fillbufferdone[7];
    if(error != NMF_OK) {
      return error;
    }
    return error;
  }
  if(compositeStrcmp(name, "fillbufferdone[8]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_fillbufferdone[8].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    error = target->bindFromUser(targetname, size, &_xyitfcb_fillbufferdone[8], subpriority);
    if(error != NMF_OK)
      return error;
    _xyuv_Processor.fillbufferdone[8] = _xyitfcb_fillbufferdone[8];
    if(error != NMF_OK) {
      return error;
    }
    return error;
  }
  if(compositeStrcmp(name, "fillbufferdone[9]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_fillbufferdone[9].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    error = target->bindFromUser(targetname, size, &_xyitfcb_fillbufferdone[9], subpriority);
    if(error != NMF_OK)
      return error;
    _xyuv_Processor.fillbufferdone[9] = _xyitfcb_fillbufferdone[9];
    if(error != NMF_OK) {
      return error;
    }
    return error;
  }
  if(compositeStrcmp(name, "proxy") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_proxy.IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    error = target->bindFromUser(targetname, size, &_xyitfcb_proxy, subpriority);
    if(error != NMF_OK)
      return error;
    _xyuv_Processor.proxy = _xyitfcb_proxy;
    if(error != NMF_OK) {
      return error;
    }
    return error;
  }
  if(compositeStrcmp(name, "ToOMXComponent") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_ToOMXComponent.IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    error = target->bindFromUser(targetname, size, &_xyitfcb_ToOMXComponent, subpriority);
    if(error != NMF_OK)
      return error;
    _xyuv_Processor.ToOMXComponent = _xyitfcb_ToOMXComponent;
    if(error != NMF_OK) {
      return error;
    }
    return error;
  }
  return error;
}

t_nmf_error openmax_processor_wrpImpl::unbindAsynchronous(const char* name, NMF::Composite* target, const char* targetname) {
    char realname[MAX_INTERFACE_NAME_LENGTH];

  // Check if server was a MPC
    t_nmf_component_handle server = target->getComponentInterface(targetname, realname);
    if(server != 0x0) 
      return unbindToMPC(name, server, realname);

  if(compositeStrcmp(name, "emptybufferdone[0]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_emptybufferdone[0].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    target->unbindFromUser(targetname);
    _xyuv_Processor.emptybufferdone[0] = 0x0;
    _xyitfcb_emptybufferdone[0] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(name, "emptybufferdone[1]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_emptybufferdone[1].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    target->unbindFromUser(targetname);
    _xyuv_Processor.emptybufferdone[1] = 0x0;
    _xyitfcb_emptybufferdone[1] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(name, "emptybufferdone[2]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_emptybufferdone[2].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    target->unbindFromUser(targetname);
    _xyuv_Processor.emptybufferdone[2] = 0x0;
    _xyitfcb_emptybufferdone[2] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(name, "emptybufferdone[3]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_emptybufferdone[3].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    target->unbindFromUser(targetname);
    _xyuv_Processor.emptybufferdone[3] = 0x0;
    _xyitfcb_emptybufferdone[3] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(name, "emptybufferdone[4]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_emptybufferdone[4].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    target->unbindFromUser(targetname);
    _xyuv_Processor.emptybufferdone[4] = 0x0;
    _xyitfcb_emptybufferdone[4] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(name, "emptybufferdone[5]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_emptybufferdone[5].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    target->unbindFromUser(targetname);
    _xyuv_Processor.emptybufferdone[5] = 0x0;
    _xyitfcb_emptybufferdone[5] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(name, "emptybufferdone[6]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_emptybufferdone[6].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    target->unbindFromUser(targetname);
    _xyuv_Processor.emptybufferdone[6] = 0x0;
    _xyitfcb_emptybufferdone[6] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(name, "emptybufferdone[7]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_emptybufferdone[7].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    target->unbindFromUser(targetname);
    _xyuv_Processor.emptybufferdone[7] = 0x0;
    _xyitfcb_emptybufferdone[7] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(name, "emptybufferdone[8]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_emptybufferdone[8].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    target->unbindFromUser(targetname);
    _xyuv_Processor.emptybufferdone[8] = 0x0;
    _xyitfcb_emptybufferdone[8] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(name, "emptybufferdone[9]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_emptybufferdone[9].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    target->unbindFromUser(targetname);
    _xyuv_Processor.emptybufferdone[9] = 0x0;
    _xyitfcb_emptybufferdone[9] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(name, "fillbufferdone[0]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_fillbufferdone[0].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    target->unbindFromUser(targetname);
    _xyuv_Processor.fillbufferdone[0] = 0x0;
    _xyitfcb_fillbufferdone[0] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(name, "fillbufferdone[1]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_fillbufferdone[1].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    target->unbindFromUser(targetname);
    _xyuv_Processor.fillbufferdone[1] = 0x0;
    _xyitfcb_fillbufferdone[1] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(name, "fillbufferdone[2]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_fillbufferdone[2].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    target->unbindFromUser(targetname);
    _xyuv_Processor.fillbufferdone[2] = 0x0;
    _xyitfcb_fillbufferdone[2] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(name, "fillbufferdone[3]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_fillbufferdone[3].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    target->unbindFromUser(targetname);
    _xyuv_Processor.fillbufferdone[3] = 0x0;
    _xyitfcb_fillbufferdone[3] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(name, "fillbufferdone[4]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_fillbufferdone[4].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    target->unbindFromUser(targetname);
    _xyuv_Processor.fillbufferdone[4] = 0x0;
    _xyitfcb_fillbufferdone[4] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(name, "fillbufferdone[5]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_fillbufferdone[5].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    target->unbindFromUser(targetname);
    _xyuv_Processor.fillbufferdone[5] = 0x0;
    _xyitfcb_fillbufferdone[5] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(name, "fillbufferdone[6]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_fillbufferdone[6].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    target->unbindFromUser(targetname);
    _xyuv_Processor.fillbufferdone[6] = 0x0;
    _xyitfcb_fillbufferdone[6] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(name, "fillbufferdone[7]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_fillbufferdone[7].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    target->unbindFromUser(targetname);
    _xyuv_Processor.fillbufferdone[7] = 0x0;
    _xyitfcb_fillbufferdone[7] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(name, "fillbufferdone[8]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_fillbufferdone[8].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    target->unbindFromUser(targetname);
    _xyuv_Processor.fillbufferdone[8] = 0x0;
    _xyitfcb_fillbufferdone[8] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(name, "fillbufferdone[9]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_fillbufferdone[9].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    target->unbindFromUser(targetname);
    _xyuv_Processor.fillbufferdone[9] = 0x0;
    _xyitfcb_fillbufferdone[9] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(name, "proxy") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_proxy.IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    target->unbindFromUser(targetname);
    _xyuv_Processor.proxy = 0x0;
    _xyitfcb_proxy = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(name, "ToOMXComponent") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_ToOMXComponent.IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    target->unbindFromUser(targetname);
    _xyuv_Processor.ToOMXComponent = 0x0;
    _xyitfcb_ToOMXComponent = 0x0;
    return NMF_OK;
  }
  return NMF_NO_SUCH_REQUIRED_INTERFACE;
}

t_nmf_error openmax_processor_wrpImpl::bindComponent(const char* name, NMF::Composite* target, const char* targetname) {
  t_nmf_error error = NMF_NO_SUCH_REQUIRED_INTERFACE;
  if(compositeStrcmp(name, "emptybufferdone[0]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_emptybufferdone[0].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    if((error = target->getInterface(targetname, &_xyitfcb_emptybufferdone[0])) != NMF_OK)
      return error;
    _xyuv_Processor.emptybufferdone[0] = _xyitfcb_emptybufferdone[0];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "emptybufferdone[1]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_emptybufferdone[1].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    if((error = target->getInterface(targetname, &_xyitfcb_emptybufferdone[1])) != NMF_OK)
      return error;
    _xyuv_Processor.emptybufferdone[1] = _xyitfcb_emptybufferdone[1];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "emptybufferdone[2]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_emptybufferdone[2].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    if((error = target->getInterface(targetname, &_xyitfcb_emptybufferdone[2])) != NMF_OK)
      return error;
    _xyuv_Processor.emptybufferdone[2] = _xyitfcb_emptybufferdone[2];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "emptybufferdone[3]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_emptybufferdone[3].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    if((error = target->getInterface(targetname, &_xyitfcb_emptybufferdone[3])) != NMF_OK)
      return error;
    _xyuv_Processor.emptybufferdone[3] = _xyitfcb_emptybufferdone[3];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "emptybufferdone[4]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_emptybufferdone[4].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    if((error = target->getInterface(targetname, &_xyitfcb_emptybufferdone[4])) != NMF_OK)
      return error;
    _xyuv_Processor.emptybufferdone[4] = _xyitfcb_emptybufferdone[4];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "emptybufferdone[5]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_emptybufferdone[5].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    if((error = target->getInterface(targetname, &_xyitfcb_emptybufferdone[5])) != NMF_OK)
      return error;
    _xyuv_Processor.emptybufferdone[5] = _xyitfcb_emptybufferdone[5];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "emptybufferdone[6]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_emptybufferdone[6].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    if((error = target->getInterface(targetname, &_xyitfcb_emptybufferdone[6])) != NMF_OK)
      return error;
    _xyuv_Processor.emptybufferdone[6] = _xyitfcb_emptybufferdone[6];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "emptybufferdone[7]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_emptybufferdone[7].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    if((error = target->getInterface(targetname, &_xyitfcb_emptybufferdone[7])) != NMF_OK)
      return error;
    _xyuv_Processor.emptybufferdone[7] = _xyitfcb_emptybufferdone[7];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "emptybufferdone[8]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_emptybufferdone[8].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    if((error = target->getInterface(targetname, &_xyitfcb_emptybufferdone[8])) != NMF_OK)
      return error;
    _xyuv_Processor.emptybufferdone[8] = _xyitfcb_emptybufferdone[8];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "emptybufferdone[9]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_emptybufferdone[9].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    if((error = target->getInterface(targetname, &_xyitfcb_emptybufferdone[9])) != NMF_OK)
      return error;
    _xyuv_Processor.emptybufferdone[9] = _xyitfcb_emptybufferdone[9];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "fillbufferdone[0]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_fillbufferdone[0].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    if((error = target->getInterface(targetname, &_xyitfcb_fillbufferdone[0])) != NMF_OK)
      return error;
    _xyuv_Processor.fillbufferdone[0] = _xyitfcb_fillbufferdone[0];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "fillbufferdone[1]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_fillbufferdone[1].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    if((error = target->getInterface(targetname, &_xyitfcb_fillbufferdone[1])) != NMF_OK)
      return error;
    _xyuv_Processor.fillbufferdone[1] = _xyitfcb_fillbufferdone[1];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "fillbufferdone[2]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_fillbufferdone[2].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    if((error = target->getInterface(targetname, &_xyitfcb_fillbufferdone[2])) != NMF_OK)
      return error;
    _xyuv_Processor.fillbufferdone[2] = _xyitfcb_fillbufferdone[2];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "fillbufferdone[3]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_fillbufferdone[3].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    if((error = target->getInterface(targetname, &_xyitfcb_fillbufferdone[3])) != NMF_OK)
      return error;
    _xyuv_Processor.fillbufferdone[3] = _xyitfcb_fillbufferdone[3];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "fillbufferdone[4]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_fillbufferdone[4].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    if((error = target->getInterface(targetname, &_xyitfcb_fillbufferdone[4])) != NMF_OK)
      return error;
    _xyuv_Processor.fillbufferdone[4] = _xyitfcb_fillbufferdone[4];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "fillbufferdone[5]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_fillbufferdone[5].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    if((error = target->getInterface(targetname, &_xyitfcb_fillbufferdone[5])) != NMF_OK)
      return error;
    _xyuv_Processor.fillbufferdone[5] = _xyitfcb_fillbufferdone[5];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "fillbufferdone[6]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_fillbufferdone[6].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    if((error = target->getInterface(targetname, &_xyitfcb_fillbufferdone[6])) != NMF_OK)
      return error;
    _xyuv_Processor.fillbufferdone[6] = _xyitfcb_fillbufferdone[6];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "fillbufferdone[7]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_fillbufferdone[7].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    if((error = target->getInterface(targetname, &_xyitfcb_fillbufferdone[7])) != NMF_OK)
      return error;
    _xyuv_Processor.fillbufferdone[7] = _xyitfcb_fillbufferdone[7];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "fillbufferdone[8]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_fillbufferdone[8].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    if((error = target->getInterface(targetname, &_xyitfcb_fillbufferdone[8])) != NMF_OK)
      return error;
    _xyuv_Processor.fillbufferdone[8] = _xyitfcb_fillbufferdone[8];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "fillbufferdone[9]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_fillbufferdone[9].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    if((error = target->getInterface(targetname, &_xyitfcb_fillbufferdone[9])) != NMF_OK)
      return error;
    _xyuv_Processor.fillbufferdone[9] = _xyitfcb_fillbufferdone[9];
    return NMF_OK;
  }
  if(compositeStrcmp(name, "proxy") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_proxy.IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    if((error = target->getInterface(targetname, &_xyitfcb_proxy)) != NMF_OK)
      return error;
    _xyuv_Processor.proxy = _xyitfcb_proxy;
    return NMF_OK;
  }
  if(compositeStrcmp(name, "ToOMXComponent") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_ToOMXComponent.IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    if((error = target->getInterface(targetname, &_xyitfcb_ToOMXComponent)) != NMF_OK)
      return error;
    _xyuv_Processor.ToOMXComponent = _xyitfcb_ToOMXComponent;
    return NMF_OK;
  }
  return error;
}

t_nmf_error openmax_processor_wrpImpl::unbindComponent(const char* name, NMF::Composite* target, const char* targetname) {
  if(compositeStrcmp(name, "emptybufferdone[0]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_emptybufferdone[0].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    _xyuv_Processor.emptybufferdone[0] = 0x0;
    _xyitfcb_emptybufferdone[0] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(name, "emptybufferdone[1]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_emptybufferdone[1].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    _xyuv_Processor.emptybufferdone[1] = 0x0;
    _xyitfcb_emptybufferdone[1] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(name, "emptybufferdone[2]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_emptybufferdone[2].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    _xyuv_Processor.emptybufferdone[2] = 0x0;
    _xyitfcb_emptybufferdone[2] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(name, "emptybufferdone[3]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_emptybufferdone[3].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    _xyuv_Processor.emptybufferdone[3] = 0x0;
    _xyitfcb_emptybufferdone[3] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(name, "emptybufferdone[4]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_emptybufferdone[4].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    _xyuv_Processor.emptybufferdone[4] = 0x0;
    _xyitfcb_emptybufferdone[4] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(name, "emptybufferdone[5]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_emptybufferdone[5].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    _xyuv_Processor.emptybufferdone[5] = 0x0;
    _xyitfcb_emptybufferdone[5] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(name, "emptybufferdone[6]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_emptybufferdone[6].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    _xyuv_Processor.emptybufferdone[6] = 0x0;
    _xyitfcb_emptybufferdone[6] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(name, "emptybufferdone[7]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_emptybufferdone[7].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    _xyuv_Processor.emptybufferdone[7] = 0x0;
    _xyitfcb_emptybufferdone[7] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(name, "emptybufferdone[8]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_emptybufferdone[8].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    _xyuv_Processor.emptybufferdone[8] = 0x0;
    _xyitfcb_emptybufferdone[8] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(name, "emptybufferdone[9]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_emptybufferdone[9].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    _xyuv_Processor.emptybufferdone[9] = 0x0;
    _xyitfcb_emptybufferdone[9] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(name, "fillbufferdone[0]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_fillbufferdone[0].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    _xyuv_Processor.fillbufferdone[0] = 0x0;
    _xyitfcb_fillbufferdone[0] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(name, "fillbufferdone[1]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_fillbufferdone[1].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    _xyuv_Processor.fillbufferdone[1] = 0x0;
    _xyitfcb_fillbufferdone[1] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(name, "fillbufferdone[2]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_fillbufferdone[2].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    _xyuv_Processor.fillbufferdone[2] = 0x0;
    _xyitfcb_fillbufferdone[2] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(name, "fillbufferdone[3]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_fillbufferdone[3].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    _xyuv_Processor.fillbufferdone[3] = 0x0;
    _xyitfcb_fillbufferdone[3] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(name, "fillbufferdone[4]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_fillbufferdone[4].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    _xyuv_Processor.fillbufferdone[4] = 0x0;
    _xyitfcb_fillbufferdone[4] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(name, "fillbufferdone[5]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_fillbufferdone[5].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    _xyuv_Processor.fillbufferdone[5] = 0x0;
    _xyitfcb_fillbufferdone[5] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(name, "fillbufferdone[6]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_fillbufferdone[6].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    _xyuv_Processor.fillbufferdone[6] = 0x0;
    _xyitfcb_fillbufferdone[6] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(name, "fillbufferdone[7]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_fillbufferdone[7].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    _xyuv_Processor.fillbufferdone[7] = 0x0;
    _xyitfcb_fillbufferdone[7] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(name, "fillbufferdone[8]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_fillbufferdone[8].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    _xyuv_Processor.fillbufferdone[8] = 0x0;
    _xyitfcb_fillbufferdone[8] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(name, "fillbufferdone[9]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_fillbufferdone[9].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    _xyuv_Processor.fillbufferdone[9] = 0x0;
    _xyitfcb_fillbufferdone[9] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(name, "proxy") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_proxy.IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    _xyuv_Processor.proxy = 0x0;
    _xyitfcb_proxy = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(name, "ToOMXComponent") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_ToOMXComponent.IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    _xyuv_Processor.ToOMXComponent = 0x0;
    _xyitfcb_ToOMXComponent = 0x0;
    return NMF_OK;
  }
  return NMF_NO_SUCH_REQUIRED_INTERFACE;
}

t_nmf_error openmax_processor_wrpImpl::readAttribute(const char* name, t_uint32 *value) {
  return NMF_NO_SUCH_ATTRIBUTE;
}

t_nmf_error openmax_processor_wrpImpl::getProperty(const char* name, char *value, t_uint32 valuelen) {
  return NMF_NO_SUCH_PROPERTY;
}

t_nmf_component_handle openmax_processor_wrpImpl::getMPCComponentHandle(const char *nameinowner) {
  return (t_nmf_component_handle)0x0;
}
t_nmf_component_handle openmax_processor_wrpImpl:: getComponentInterface(const char *compositeitfname, char *realitfname) {
  if(compositeStrcmp(compositeitfname, "emptythisbuffer[0]") == 0) {
  }
  if(compositeStrcmp(compositeitfname, "emptythisbuffer[1]") == 0) {
  }
  if(compositeStrcmp(compositeitfname, "emptythisbuffer[2]") == 0) {
  }
  if(compositeStrcmp(compositeitfname, "emptythisbuffer[3]") == 0) {
  }
  if(compositeStrcmp(compositeitfname, "emptythisbuffer[4]") == 0) {
  }
  if(compositeStrcmp(compositeitfname, "emptythisbuffer[5]") == 0) {
  }
  if(compositeStrcmp(compositeitfname, "emptythisbuffer[6]") == 0) {
  }
  if(compositeStrcmp(compositeitfname, "emptythisbuffer[7]") == 0) {
  }
  if(compositeStrcmp(compositeitfname, "emptythisbuffer[8]") == 0) {
  }
  if(compositeStrcmp(compositeitfname, "emptythisbuffer[9]") == 0) {
  }
  if(compositeStrcmp(compositeitfname, "fillthisbuffer[0]") == 0) {
  }
  if(compositeStrcmp(compositeitfname, "fillthisbuffer[1]") == 0) {
  }
  if(compositeStrcmp(compositeitfname, "fillthisbuffer[2]") == 0) {
  }
  if(compositeStrcmp(compositeitfname, "fillthisbuffer[3]") == 0) {
  }
  if(compositeStrcmp(compositeitfname, "fillthisbuffer[4]") == 0) {
  }
  if(compositeStrcmp(compositeitfname, "fillthisbuffer[5]") == 0) {
  }
  if(compositeStrcmp(compositeitfname, "fillthisbuffer[6]") == 0) {
  }
  if(compositeStrcmp(compositeitfname, "fillthisbuffer[7]") == 0) {
  }
  if(compositeStrcmp(compositeitfname, "fillthisbuffer[8]") == 0) {
  }
  if(compositeStrcmp(compositeitfname, "fillthisbuffer[9]") == 0) {
  }
  if(compositeStrcmp(compositeitfname, "sendcommand") == 0) {
  }
  if(compositeStrcmp(compositeitfname, "fsminit") == 0) {
  }
  if(compositeStrcmp(compositeitfname, "Config") == 0) {
  }
  if(compositeStrcmp(compositeitfname, "Param") == 0) {
  }
  return (t_nmf_component_handle)0x0;
}
t_nmf_error openmax_processor_wrpImpl::bindFromMPC(t_nmf_component_handle client, const char* clientitfname, const char * compositeitfname, unsigned int size) {
#if defined(__STN_8500)
  if(compositeStrcmp(compositeitfname, "emptythisbuffer[0]") == 0) {
    _xybc_emptythisbuffer[0].destroyFifo();            // Destroy server previous binding if any
    _xybc_emptythisbuffer[0].connect(57005 << 8, &_xyuv_Processor);
    if(_xybc_emptythisbuffer[0].createFifo(size, 1, 0) != NMF_OK)
      return NMF_NO_MORE_MEMORY;
    _xybc_emptythisbuffer[0].target = (armnmf_emptythisbufferDescriptor*)&_xyuv_Processor.emptythisbuffer[0];
    return (t_nmf_error)CM_BindComponentToUser(
          (t_nmf_channel)EEgetDistributionChannel(1),
          client,
          clientitfname,
          &_xybc_emptythisbuffer[0],
          size);
  }
  if(compositeStrcmp(compositeitfname, "emptythisbuffer[1]") == 0) {
    _xybc_emptythisbuffer[1].destroyFifo();            // Destroy server previous binding if any
    _xybc_emptythisbuffer[1].connect(57005 << 8, &_xyuv_Processor);
    if(_xybc_emptythisbuffer[1].createFifo(size, 1, 0) != NMF_OK)
      return NMF_NO_MORE_MEMORY;
    _xybc_emptythisbuffer[1].target = (armnmf_emptythisbufferDescriptor*)&_xyuv_Processor.emptythisbuffer[1];
    return (t_nmf_error)CM_BindComponentToUser(
          (t_nmf_channel)EEgetDistributionChannel(1),
          client,
          clientitfname,
          &_xybc_emptythisbuffer[1],
          size);
  }
  if(compositeStrcmp(compositeitfname, "emptythisbuffer[2]") == 0) {
    _xybc_emptythisbuffer[2].destroyFifo();            // Destroy server previous binding if any
    _xybc_emptythisbuffer[2].connect(57005 << 8, &_xyuv_Processor);
    if(_xybc_emptythisbuffer[2].createFifo(size, 1, 0) != NMF_OK)
      return NMF_NO_MORE_MEMORY;
    _xybc_emptythisbuffer[2].target = (armnmf_emptythisbufferDescriptor*)&_xyuv_Processor.emptythisbuffer[2];
    return (t_nmf_error)CM_BindComponentToUser(
          (t_nmf_channel)EEgetDistributionChannel(1),
          client,
          clientitfname,
          &_xybc_emptythisbuffer[2],
          size);
  }
  if(compositeStrcmp(compositeitfname, "emptythisbuffer[3]") == 0) {
    _xybc_emptythisbuffer[3].destroyFifo();            // Destroy server previous binding if any
    _xybc_emptythisbuffer[3].connect(57005 << 8, &_xyuv_Processor);
    if(_xybc_emptythisbuffer[3].createFifo(size, 1, 0) != NMF_OK)
      return NMF_NO_MORE_MEMORY;
    _xybc_emptythisbuffer[3].target = (armnmf_emptythisbufferDescriptor*)&_xyuv_Processor.emptythisbuffer[3];
    return (t_nmf_error)CM_BindComponentToUser(
          (t_nmf_channel)EEgetDistributionChannel(1),
          client,
          clientitfname,
          &_xybc_emptythisbuffer[3],
          size);
  }
  if(compositeStrcmp(compositeitfname, "emptythisbuffer[4]") == 0) {
    _xybc_emptythisbuffer[4].destroyFifo();            // Destroy server previous binding if any
    _xybc_emptythisbuffer[4].connect(57005 << 8, &_xyuv_Processor);
    if(_xybc_emptythisbuffer[4].createFifo(size, 1, 0) != NMF_OK)
      return NMF_NO_MORE_MEMORY;
    _xybc_emptythisbuffer[4].target = (armnmf_emptythisbufferDescriptor*)&_xyuv_Processor.emptythisbuffer[4];
    return (t_nmf_error)CM_BindComponentToUser(
          (t_nmf_channel)EEgetDistributionChannel(1),
          client,
          clientitfname,
          &_xybc_emptythisbuffer[4],
          size);
  }
  if(compositeStrcmp(compositeitfname, "emptythisbuffer[5]") == 0) {
    _xybc_emptythisbuffer[5].destroyFifo();            // Destroy server previous binding if any
    _xybc_emptythisbuffer[5].connect(57005 << 8, &_xyuv_Processor);
    if(_xybc_emptythisbuffer[5].createFifo(size, 1, 0) != NMF_OK)
      return NMF_NO_MORE_MEMORY;
    _xybc_emptythisbuffer[5].target = (armnmf_emptythisbufferDescriptor*)&_xyuv_Processor.emptythisbuffer[5];
    return (t_nmf_error)CM_BindComponentToUser(
          (t_nmf_channel)EEgetDistributionChannel(1),
          client,
          clientitfname,
          &_xybc_emptythisbuffer[5],
          size);
  }
  if(compositeStrcmp(compositeitfname, "emptythisbuffer[6]") == 0) {
    _xybc_emptythisbuffer[6].destroyFifo();            // Destroy server previous binding if any
    _xybc_emptythisbuffer[6].connect(57005 << 8, &_xyuv_Processor);
    if(_xybc_emptythisbuffer[6].createFifo(size, 1, 0) != NMF_OK)
      return NMF_NO_MORE_MEMORY;
    _xybc_emptythisbuffer[6].target = (armnmf_emptythisbufferDescriptor*)&_xyuv_Processor.emptythisbuffer[6];
    return (t_nmf_error)CM_BindComponentToUser(
          (t_nmf_channel)EEgetDistributionChannel(1),
          client,
          clientitfname,
          &_xybc_emptythisbuffer[6],
          size);
  }
  if(compositeStrcmp(compositeitfname, "emptythisbuffer[7]") == 0) {
    _xybc_emptythisbuffer[7].destroyFifo();            // Destroy server previous binding if any
    _xybc_emptythisbuffer[7].connect(57005 << 8, &_xyuv_Processor);
    if(_xybc_emptythisbuffer[7].createFifo(size, 1, 0) != NMF_OK)
      return NMF_NO_MORE_MEMORY;
    _xybc_emptythisbuffer[7].target = (armnmf_emptythisbufferDescriptor*)&_xyuv_Processor.emptythisbuffer[7];
    return (t_nmf_error)CM_BindComponentToUser(
          (t_nmf_channel)EEgetDistributionChannel(1),
          client,
          clientitfname,
          &_xybc_emptythisbuffer[7],
          size);
  }
  if(compositeStrcmp(compositeitfname, "emptythisbuffer[8]") == 0) {
    _xybc_emptythisbuffer[8].destroyFifo();            // Destroy server previous binding if any
    _xybc_emptythisbuffer[8].connect(57005 << 8, &_xyuv_Processor);
    if(_xybc_emptythisbuffer[8].createFifo(size, 1, 0) != NMF_OK)
      return NMF_NO_MORE_MEMORY;
    _xybc_emptythisbuffer[8].target = (armnmf_emptythisbufferDescriptor*)&_xyuv_Processor.emptythisbuffer[8];
    return (t_nmf_error)CM_BindComponentToUser(
          (t_nmf_channel)EEgetDistributionChannel(1),
          client,
          clientitfname,
          &_xybc_emptythisbuffer[8],
          size);
  }
  if(compositeStrcmp(compositeitfname, "emptythisbuffer[9]") == 0) {
    _xybc_emptythisbuffer[9].destroyFifo();            // Destroy server previous binding if any
    _xybc_emptythisbuffer[9].connect(57005 << 8, &_xyuv_Processor);
    if(_xybc_emptythisbuffer[9].createFifo(size, 1, 0) != NMF_OK)
      return NMF_NO_MORE_MEMORY;
    _xybc_emptythisbuffer[9].target = (armnmf_emptythisbufferDescriptor*)&_xyuv_Processor.emptythisbuffer[9];
    return (t_nmf_error)CM_BindComponentToUser(
          (t_nmf_channel)EEgetDistributionChannel(1),
          client,
          clientitfname,
          &_xybc_emptythisbuffer[9],
          size);
  }
  if(compositeStrcmp(compositeitfname, "fillthisbuffer[0]") == 0) {
    _xybc_fillthisbuffer[0].destroyFifo();            // Destroy server previous binding if any
    _xybc_fillthisbuffer[0].connect(57005 << 8, &_xyuv_Processor);
    if(_xybc_fillthisbuffer[0].createFifo(size, 1, 0) != NMF_OK)
      return NMF_NO_MORE_MEMORY;
    _xybc_fillthisbuffer[0].target = (armnmf_fillthisbufferDescriptor*)&_xyuv_Processor.fillthisbuffer[0];
    return (t_nmf_error)CM_BindComponentToUser(
          (t_nmf_channel)EEgetDistributionChannel(1),
          client,
          clientitfname,
          &_xybc_fillthisbuffer[0],
          size);
  }
  if(compositeStrcmp(compositeitfname, "fillthisbuffer[1]") == 0) {
    _xybc_fillthisbuffer[1].destroyFifo();            // Destroy server previous binding if any
    _xybc_fillthisbuffer[1].connect(57005 << 8, &_xyuv_Processor);
    if(_xybc_fillthisbuffer[1].createFifo(size, 1, 0) != NMF_OK)
      return NMF_NO_MORE_MEMORY;
    _xybc_fillthisbuffer[1].target = (armnmf_fillthisbufferDescriptor*)&_xyuv_Processor.fillthisbuffer[1];
    return (t_nmf_error)CM_BindComponentToUser(
          (t_nmf_channel)EEgetDistributionChannel(1),
          client,
          clientitfname,
          &_xybc_fillthisbuffer[1],
          size);
  }
  if(compositeStrcmp(compositeitfname, "fillthisbuffer[2]") == 0) {
    _xybc_fillthisbuffer[2].destroyFifo();            // Destroy server previous binding if any
    _xybc_fillthisbuffer[2].connect(57005 << 8, &_xyuv_Processor);
    if(_xybc_fillthisbuffer[2].createFifo(size, 1, 0) != NMF_OK)
      return NMF_NO_MORE_MEMORY;
    _xybc_fillthisbuffer[2].target = (armnmf_fillthisbufferDescriptor*)&_xyuv_Processor.fillthisbuffer[2];
    return (t_nmf_error)CM_BindComponentToUser(
          (t_nmf_channel)EEgetDistributionChannel(1),
          client,
          clientitfname,
          &_xybc_fillthisbuffer[2],
          size);
  }
  if(compositeStrcmp(compositeitfname, "fillthisbuffer[3]") == 0) {
    _xybc_fillthisbuffer[3].destroyFifo();            // Destroy server previous binding if any
    _xybc_fillthisbuffer[3].connect(57005 << 8, &_xyuv_Processor);
    if(_xybc_fillthisbuffer[3].createFifo(size, 1, 0) != NMF_OK)
      return NMF_NO_MORE_MEMORY;
    _xybc_fillthisbuffer[3].target = (armnmf_fillthisbufferDescriptor*)&_xyuv_Processor.fillthisbuffer[3];
    return (t_nmf_error)CM_BindComponentToUser(
          (t_nmf_channel)EEgetDistributionChannel(1),
          client,
          clientitfname,
          &_xybc_fillthisbuffer[3],
          size);
  }
  if(compositeStrcmp(compositeitfname, "fillthisbuffer[4]") == 0) {
    _xybc_fillthisbuffer[4].destroyFifo();            // Destroy server previous binding if any
    _xybc_fillthisbuffer[4].connect(57005 << 8, &_xyuv_Processor);
    if(_xybc_fillthisbuffer[4].createFifo(size, 1, 0) != NMF_OK)
      return NMF_NO_MORE_MEMORY;
    _xybc_fillthisbuffer[4].target = (armnmf_fillthisbufferDescriptor*)&_xyuv_Processor.fillthisbuffer[4];
    return (t_nmf_error)CM_BindComponentToUser(
          (t_nmf_channel)EEgetDistributionChannel(1),
          client,
          clientitfname,
          &_xybc_fillthisbuffer[4],
          size);
  }
  if(compositeStrcmp(compositeitfname, "fillthisbuffer[5]") == 0) {
    _xybc_fillthisbuffer[5].destroyFifo();            // Destroy server previous binding if any
    _xybc_fillthisbuffer[5].connect(57005 << 8, &_xyuv_Processor);
    if(_xybc_fillthisbuffer[5].createFifo(size, 1, 0) != NMF_OK)
      return NMF_NO_MORE_MEMORY;
    _xybc_fillthisbuffer[5].target = (armnmf_fillthisbufferDescriptor*)&_xyuv_Processor.fillthisbuffer[5];
    return (t_nmf_error)CM_BindComponentToUser(
          (t_nmf_channel)EEgetDistributionChannel(1),
          client,
          clientitfname,
          &_xybc_fillthisbuffer[5],
          size);
  }
  if(compositeStrcmp(compositeitfname, "fillthisbuffer[6]") == 0) {
    _xybc_fillthisbuffer[6].destroyFifo();            // Destroy server previous binding if any
    _xybc_fillthisbuffer[6].connect(57005 << 8, &_xyuv_Processor);
    if(_xybc_fillthisbuffer[6].createFifo(size, 1, 0) != NMF_OK)
      return NMF_NO_MORE_MEMORY;
    _xybc_fillthisbuffer[6].target = (armnmf_fillthisbufferDescriptor*)&_xyuv_Processor.fillthisbuffer[6];
    return (t_nmf_error)CM_BindComponentToUser(
          (t_nmf_channel)EEgetDistributionChannel(1),
          client,
          clientitfname,
          &_xybc_fillthisbuffer[6],
          size);
  }
  if(compositeStrcmp(compositeitfname, "fillthisbuffer[7]") == 0) {
    _xybc_fillthisbuffer[7].destroyFifo();            // Destroy server previous binding if any
    _xybc_fillthisbuffer[7].connect(57005 << 8, &_xyuv_Processor);
    if(_xybc_fillthisbuffer[7].createFifo(size, 1, 0) != NMF_OK)
      return NMF_NO_MORE_MEMORY;
    _xybc_fillthisbuffer[7].target = (armnmf_fillthisbufferDescriptor*)&_xyuv_Processor.fillthisbuffer[7];
    return (t_nmf_error)CM_BindComponentToUser(
          (t_nmf_channel)EEgetDistributionChannel(1),
          client,
          clientitfname,
          &_xybc_fillthisbuffer[7],
          size);
  }
  if(compositeStrcmp(compositeitfname, "fillthisbuffer[8]") == 0) {
    _xybc_fillthisbuffer[8].destroyFifo();            // Destroy server previous binding if any
    _xybc_fillthisbuffer[8].connect(57005 << 8, &_xyuv_Processor);
    if(_xybc_fillthisbuffer[8].createFifo(size, 1, 0) != NMF_OK)
      return NMF_NO_MORE_MEMORY;
    _xybc_fillthisbuffer[8].target = (armnmf_fillthisbufferDescriptor*)&_xyuv_Processor.fillthisbuffer[8];
    return (t_nmf_error)CM_BindComponentToUser(
          (t_nmf_channel)EEgetDistributionChannel(1),
          client,
          clientitfname,
          &_xybc_fillthisbuffer[8],
          size);
  }
  if(compositeStrcmp(compositeitfname, "fillthisbuffer[9]") == 0) {
    _xybc_fillthisbuffer[9].destroyFifo();            // Destroy server previous binding if any
    _xybc_fillthisbuffer[9].connect(57005 << 8, &_xyuv_Processor);
    if(_xybc_fillthisbuffer[9].createFifo(size, 1, 0) != NMF_OK)
      return NMF_NO_MORE_MEMORY;
    _xybc_fillthisbuffer[9].target = (armnmf_fillthisbufferDescriptor*)&_xyuv_Processor.fillthisbuffer[9];
    return (t_nmf_error)CM_BindComponentToUser(
          (t_nmf_channel)EEgetDistributionChannel(1),
          client,
          clientitfname,
          &_xybc_fillthisbuffer[9],
          size);
  }
  if(compositeStrcmp(compositeitfname, "sendcommand") == 0) {
    _xybc_sendcommand.destroyFifo();            // Destroy server previous binding if any
    _xybc_sendcommand.connect(1 << 8, &_xyuv_Processor);
    if(_xybc_sendcommand.createFifo(size, 1, 0) != NMF_OK)
      return NMF_NO_MORE_MEMORY;
    _xybc_sendcommand.target = (sendcommandDescriptor*)&_xyuv_Processor;
    return (t_nmf_error)CM_BindComponentToUser(
          (t_nmf_channel)EEgetDistributionChannel(1),
          client,
          clientitfname,
          &_xybc_sendcommand,
          size);
  }
  if(compositeStrcmp(compositeitfname, "fsminit") == 0) {
    _xybc_fsminit.destroyFifo();            // Destroy server previous binding if any
    _xybc_fsminit.connect(0 << 8, &_xyuv_Processor);
    if(_xybc_fsminit.createFifo(size, 1, 0) != NMF_OK)
      return NMF_NO_MORE_MEMORY;
    _xybc_fsminit.target = (fsminitDescriptor*)&_xyuv_Processor;
    return (t_nmf_error)CM_BindComponentToUser(
          (t_nmf_channel)EEgetDistributionChannel(1),
          client,
          clientitfname,
          &_xybc_fsminit,
          size);
  }
  if(compositeStrcmp(compositeitfname, "Config") == 0) {
    _xybc_Config.destroyFifo();            // Destroy server previous binding if any
    _xybc_Config.connect(8 << 8, &_xyuv_Processor);
    if(_xybc_Config.createFifo(size, 1, 0) != NMF_OK)
      return NMF_NO_MORE_MEMORY;
    _xybc_Config.target = (openmax_processor_api_ConfigDescriptor*)&_xyuv_Processor;
    return (t_nmf_error)CM_BindComponentToUser(
          (t_nmf_channel)EEgetDistributionChannel(1),
          client,
          clientitfname,
          &_xybc_Config,
          size);
  }
  if(compositeStrcmp(compositeitfname, "Param") == 0) {
    _xybc_Param.destroyFifo();            // Destroy server previous binding if any
    _xybc_Param.connect(7 << 8, &_xyuv_Processor);
    if(_xybc_Param.createFifo(size, 1, 0) != NMF_OK)
      return NMF_NO_MORE_MEMORY;
    _xybc_Param.target = (openmax_processor_api_ParamDescriptor*)&_xyuv_Processor;
    return (t_nmf_error)CM_BindComponentToUser(
          (t_nmf_channel)EEgetDistributionChannel(1),
          client,
          clientitfname,
          &_xybc_Param,
          size);
  }
#endif
  return NMF_NO_SUCH_PROVIDED_INTERFACE;
}
t_nmf_error openmax_processor_wrpImpl::unbindFromMPC(t_nmf_component_handle client, const char* clientitfname, const char * compositeitfname) {
#if defined(__STN_8500)
  if(compositeStrcmp(compositeitfname, "emptythisbuffer[0]") == 0) {
    void *handle8106640;
    _xybc_emptythisbuffer[0].destroyFifo();
    return CM_UnbindComponentToUser(
          (t_nmf_channel)EEgetDistributionChannel(1),
          client,
          clientitfname,
          &handle8106640);
  }
  if(compositeStrcmp(compositeitfname, "emptythisbuffer[1]") == 0) {
    void *handle17320380;
    _xybc_emptythisbuffer[1].destroyFifo();
    return CM_UnbindComponentToUser(
          (t_nmf_channel)EEgetDistributionChannel(1),
          client,
          clientitfname,
          &handle17320380);
  }
  if(compositeStrcmp(compositeitfname, "emptythisbuffer[2]") == 0) {
    void *handle18929195;
    _xybc_emptythisbuffer[2].destroyFifo();
    return CM_UnbindComponentToUser(
          (t_nmf_channel)EEgetDistributionChannel(1),
          client,
          clientitfname,
          &handle18929195);
  }
  if(compositeStrcmp(compositeitfname, "emptythisbuffer[3]") == 0) {
    void *handle13419912;
    _xybc_emptythisbuffer[3].destroyFifo();
    return CM_UnbindComponentToUser(
          (t_nmf_channel)EEgetDistributionChannel(1),
          client,
          clientitfname,
          &handle13419912);
  }
  if(compositeStrcmp(compositeitfname, "emptythisbuffer[4]") == 0) {
    void *handle29131495;
    _xybc_emptythisbuffer[4].destroyFifo();
    return CM_UnbindComponentToUser(
          (t_nmf_channel)EEgetDistributionChannel(1),
          client,
          clientitfname,
          &handle29131495);
  }
  if(compositeStrcmp(compositeitfname, "emptythisbuffer[5]") == 0) {
    void *handle20324370;
    _xybc_emptythisbuffer[5].destroyFifo();
    return CM_UnbindComponentToUser(
          (t_nmf_channel)EEgetDistributionChannel(1),
          client,
          clientitfname,
          &handle20324370);
  }
  if(compositeStrcmp(compositeitfname, "emptythisbuffer[6]") == 0) {
    void *handle7578443;
    _xybc_emptythisbuffer[6].destroyFifo();
    return CM_UnbindComponentToUser(
          (t_nmf_channel)EEgetDistributionChannel(1),
          client,
          clientitfname,
          &handle7578443);
  }
  if(compositeStrcmp(compositeitfname, "emptythisbuffer[7]") == 0) {
    void *handle31822120;
    _xybc_emptythisbuffer[7].destroyFifo();
    return CM_UnbindComponentToUser(
          (t_nmf_channel)EEgetDistributionChannel(1),
          client,
          clientitfname,
          &handle31822120);
  }
  if(compositeStrcmp(compositeitfname, "emptythisbuffer[8]") == 0) {
    void *handle10284430;
    _xybc_emptythisbuffer[8].destroyFifo();
    return CM_UnbindComponentToUser(
          (t_nmf_channel)EEgetDistributionChannel(1),
          client,
          clientitfname,
          &handle10284430);
  }
  if(compositeStrcmp(compositeitfname, "emptythisbuffer[9]") == 0) {
    void *handle11546362;
    _xybc_emptythisbuffer[9].destroyFifo();
    return CM_UnbindComponentToUser(
          (t_nmf_channel)EEgetDistributionChannel(1),
          client,
          clientitfname,
          &handle11546362);
  }
  if(compositeStrcmp(compositeitfname, "fillthisbuffer[0]") == 0) {
    void *handle14440411;
    _xybc_fillthisbuffer[0].destroyFifo();
    return CM_UnbindComponentToUser(
          (t_nmf_channel)EEgetDistributionChannel(1),
          client,
          clientitfname,
          &handle14440411);
  }
  if(compositeStrcmp(compositeitfname, "fillthisbuffer[1]") == 0) {
    void *handle795840;
    _xybc_fillthisbuffer[1].destroyFifo();
    return CM_UnbindComponentToUser(
          (t_nmf_channel)EEgetDistributionChannel(1),
          client,
          clientitfname,
          &handle795840);
  }
  if(compositeStrcmp(compositeitfname, "fillthisbuffer[2]") == 0) {
    void *handle21021313;
    _xybc_fillthisbuffer[2].destroyFifo();
    return CM_UnbindComponentToUser(
          (t_nmf_channel)EEgetDistributionChannel(1),
          client,
          clientitfname,
          &handle21021313);
  }
  if(compositeStrcmp(compositeitfname, "fillthisbuffer[3]") == 0) {
    void *handle10605044;
    _xybc_fillthisbuffer[3].destroyFifo();
    return CM_UnbindComponentToUser(
          (t_nmf_channel)EEgetDistributionChannel(1),
          client,
          clientitfname,
          &handle10605044);
  }
  if(compositeStrcmp(compositeitfname, "fillthisbuffer[4]") == 0) {
    void *handle31401995;
    _xybc_fillthisbuffer[4].destroyFifo();
    return CM_UnbindComponentToUser(
          (t_nmf_channel)EEgetDistributionChannel(1),
          client,
          clientitfname,
          &handle31401995);
  }
  if(compositeStrcmp(compositeitfname, "fillthisbuffer[5]") == 0) {
    void *handle29232906;
    _xybc_fillthisbuffer[5].destroyFifo();
    return CM_UnbindComponentToUser(
          (t_nmf_channel)EEgetDistributionChannel(1),
          client,
          clientitfname,
          &handle29232906);
  }
  if(compositeStrcmp(compositeitfname, "fillthisbuffer[6]") == 0) {
    void *handle13459339;
    _xybc_fillthisbuffer[6].destroyFifo();
    return CM_UnbindComponentToUser(
          (t_nmf_channel)EEgetDistributionChannel(1),
          client,
          clientitfname,
          &handle13459339);
  }
  if(compositeStrcmp(compositeitfname, "fillthisbuffer[7]") == 0) {
    void *handle20698484;
    _xybc_fillthisbuffer[7].destroyFifo();
    return CM_UnbindComponentToUser(
          (t_nmf_channel)EEgetDistributionChannel(1),
          client,
          clientitfname,
          &handle20698484);
  }
  if(compositeStrcmp(compositeitfname, "fillthisbuffer[8]") == 0) {
    void *handle20634710;
    _xybc_fillthisbuffer[8].destroyFifo();
    return CM_UnbindComponentToUser(
          (t_nmf_channel)EEgetDistributionChannel(1),
          client,
          clientitfname,
          &handle20634710);
  }
  if(compositeStrcmp(compositeitfname, "fillthisbuffer[9]") == 0) {
    void *handle22522451;
    _xybc_fillthisbuffer[9].destroyFifo();
    return CM_UnbindComponentToUser(
          (t_nmf_channel)EEgetDistributionChannel(1),
          client,
          clientitfname,
          &handle22522451);
  }
  if(compositeStrcmp(compositeitfname, "sendcommand") == 0) {
    void *handle7295144;
    _xybc_sendcommand.destroyFifo();
    return CM_UnbindComponentToUser(
          (t_nmf_channel)EEgetDistributionChannel(1),
          client,
          clientitfname,
          &handle7295144);
  }
  if(compositeStrcmp(compositeitfname, "fsminit") == 0) {
    void *handle1603604;
    _xybc_fsminit.destroyFifo();
    return CM_UnbindComponentToUser(
          (t_nmf_channel)EEgetDistributionChannel(1),
          client,
          clientitfname,
          &handle1603604);
  }
  if(compositeStrcmp(compositeitfname, "Config") == 0) {
    void *handle7579563;
    _xybc_Config.destroyFifo();
    return CM_UnbindComponentToUser(
          (t_nmf_channel)EEgetDistributionChannel(1),
          client,
          clientitfname,
          &handle7579563);
  }
  if(compositeStrcmp(compositeitfname, "Param") == 0) {
    void *handle17103608;
    _xybc_Param.destroyFifo();
    return CM_UnbindComponentToUser(
          (t_nmf_channel)EEgetDistributionChannel(1),
          client,
          clientitfname,
          &handle17103608);
  }
#endif
  return NMF_NO_SUCH_PROVIDED_INTERFACE;
}
t_nmf_error openmax_processor_wrpImpl::bindToMPC(const char *compositeitfname, t_nmf_component_handle server, const char* serveritfname, unsigned int size) {
  t_nmf_error error = NMF_NO_SUCH_REQUIRED_INTERFACE;
#if defined(__STN_8500)
  if(compositeStrcmp(compositeitfname, "emptybufferdone[0]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_emptybufferdone[0].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    _xyitfcb_emptybufferdone[0] = (armnmf_fillthisbufferDescriptor*)0xBEEF;
    error = (t_nmf_error)CM_BindComponentFromUser(
         server, 
         serveritfname, 
         size, 
         &_xyuv_Processor.emptybufferdone[0]);
    if(error != NMF_OK) {
      return error;
    }
    return error;
  }
  if(compositeStrcmp(compositeitfname, "emptybufferdone[1]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_emptybufferdone[1].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    _xyitfcb_emptybufferdone[1] = (armnmf_fillthisbufferDescriptor*)0xBEEF;
    error = (t_nmf_error)CM_BindComponentFromUser(
         server, 
         serveritfname, 
         size, 
         &_xyuv_Processor.emptybufferdone[1]);
    if(error != NMF_OK) {
      return error;
    }
    return error;
  }
  if(compositeStrcmp(compositeitfname, "emptybufferdone[2]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_emptybufferdone[2].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    _xyitfcb_emptybufferdone[2] = (armnmf_fillthisbufferDescriptor*)0xBEEF;
    error = (t_nmf_error)CM_BindComponentFromUser(
         server, 
         serveritfname, 
         size, 
         &_xyuv_Processor.emptybufferdone[2]);
    if(error != NMF_OK) {
      return error;
    }
    return error;
  }
  if(compositeStrcmp(compositeitfname, "emptybufferdone[3]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_emptybufferdone[3].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    _xyitfcb_emptybufferdone[3] = (armnmf_fillthisbufferDescriptor*)0xBEEF;
    error = (t_nmf_error)CM_BindComponentFromUser(
         server, 
         serveritfname, 
         size, 
         &_xyuv_Processor.emptybufferdone[3]);
    if(error != NMF_OK) {
      return error;
    }
    return error;
  }
  if(compositeStrcmp(compositeitfname, "emptybufferdone[4]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_emptybufferdone[4].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    _xyitfcb_emptybufferdone[4] = (armnmf_fillthisbufferDescriptor*)0xBEEF;
    error = (t_nmf_error)CM_BindComponentFromUser(
         server, 
         serveritfname, 
         size, 
         &_xyuv_Processor.emptybufferdone[4]);
    if(error != NMF_OK) {
      return error;
    }
    return error;
  }
  if(compositeStrcmp(compositeitfname, "emptybufferdone[5]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_emptybufferdone[5].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    _xyitfcb_emptybufferdone[5] = (armnmf_fillthisbufferDescriptor*)0xBEEF;
    error = (t_nmf_error)CM_BindComponentFromUser(
         server, 
         serveritfname, 
         size, 
         &_xyuv_Processor.emptybufferdone[5]);
    if(error != NMF_OK) {
      return error;
    }
    return error;
  }
  if(compositeStrcmp(compositeitfname, "emptybufferdone[6]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_emptybufferdone[6].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    _xyitfcb_emptybufferdone[6] = (armnmf_fillthisbufferDescriptor*)0xBEEF;
    error = (t_nmf_error)CM_BindComponentFromUser(
         server, 
         serveritfname, 
         size, 
         &_xyuv_Processor.emptybufferdone[6]);
    if(error != NMF_OK) {
      return error;
    }
    return error;
  }
  if(compositeStrcmp(compositeitfname, "emptybufferdone[7]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_emptybufferdone[7].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    _xyitfcb_emptybufferdone[7] = (armnmf_fillthisbufferDescriptor*)0xBEEF;
    error = (t_nmf_error)CM_BindComponentFromUser(
         server, 
         serveritfname, 
         size, 
         &_xyuv_Processor.emptybufferdone[7]);
    if(error != NMF_OK) {
      return error;
    }
    return error;
  }
  if(compositeStrcmp(compositeitfname, "emptybufferdone[8]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_emptybufferdone[8].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    _xyitfcb_emptybufferdone[8] = (armnmf_fillthisbufferDescriptor*)0xBEEF;
    error = (t_nmf_error)CM_BindComponentFromUser(
         server, 
         serveritfname, 
         size, 
         &_xyuv_Processor.emptybufferdone[8]);
    if(error != NMF_OK) {
      return error;
    }
    return error;
  }
  if(compositeStrcmp(compositeitfname, "emptybufferdone[9]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_emptybufferdone[9].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    _xyitfcb_emptybufferdone[9] = (armnmf_fillthisbufferDescriptor*)0xBEEF;
    error = (t_nmf_error)CM_BindComponentFromUser(
         server, 
         serveritfname, 
         size, 
         &_xyuv_Processor.emptybufferdone[9]);
    if(error != NMF_OK) {
      return error;
    }
    return error;
  }
  if(compositeStrcmp(compositeitfname, "fillbufferdone[0]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_fillbufferdone[0].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    _xyitfcb_fillbufferdone[0] = (armnmf_emptythisbufferDescriptor*)0xBEEF;
    error = (t_nmf_error)CM_BindComponentFromUser(
         server, 
         serveritfname, 
         size, 
         &_xyuv_Processor.fillbufferdone[0]);
    if(error != NMF_OK) {
      return error;
    }
    return error;
  }
  if(compositeStrcmp(compositeitfname, "fillbufferdone[1]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_fillbufferdone[1].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    _xyitfcb_fillbufferdone[1] = (armnmf_emptythisbufferDescriptor*)0xBEEF;
    error = (t_nmf_error)CM_BindComponentFromUser(
         server, 
         serveritfname, 
         size, 
         &_xyuv_Processor.fillbufferdone[1]);
    if(error != NMF_OK) {
      return error;
    }
    return error;
  }
  if(compositeStrcmp(compositeitfname, "fillbufferdone[2]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_fillbufferdone[2].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    _xyitfcb_fillbufferdone[2] = (armnmf_emptythisbufferDescriptor*)0xBEEF;
    error = (t_nmf_error)CM_BindComponentFromUser(
         server, 
         serveritfname, 
         size, 
         &_xyuv_Processor.fillbufferdone[2]);
    if(error != NMF_OK) {
      return error;
    }
    return error;
  }
  if(compositeStrcmp(compositeitfname, "fillbufferdone[3]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_fillbufferdone[3].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    _xyitfcb_fillbufferdone[3] = (armnmf_emptythisbufferDescriptor*)0xBEEF;
    error = (t_nmf_error)CM_BindComponentFromUser(
         server, 
         serveritfname, 
         size, 
         &_xyuv_Processor.fillbufferdone[3]);
    if(error != NMF_OK) {
      return error;
    }
    return error;
  }
  if(compositeStrcmp(compositeitfname, "fillbufferdone[4]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_fillbufferdone[4].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    _xyitfcb_fillbufferdone[4] = (armnmf_emptythisbufferDescriptor*)0xBEEF;
    error = (t_nmf_error)CM_BindComponentFromUser(
         server, 
         serveritfname, 
         size, 
         &_xyuv_Processor.fillbufferdone[4]);
    if(error != NMF_OK) {
      return error;
    }
    return error;
  }
  if(compositeStrcmp(compositeitfname, "fillbufferdone[5]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_fillbufferdone[5].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    _xyitfcb_fillbufferdone[5] = (armnmf_emptythisbufferDescriptor*)0xBEEF;
    error = (t_nmf_error)CM_BindComponentFromUser(
         server, 
         serveritfname, 
         size, 
         &_xyuv_Processor.fillbufferdone[5]);
    if(error != NMF_OK) {
      return error;
    }
    return error;
  }
  if(compositeStrcmp(compositeitfname, "fillbufferdone[6]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_fillbufferdone[6].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    _xyitfcb_fillbufferdone[6] = (armnmf_emptythisbufferDescriptor*)0xBEEF;
    error = (t_nmf_error)CM_BindComponentFromUser(
         server, 
         serveritfname, 
         size, 
         &_xyuv_Processor.fillbufferdone[6]);
    if(error != NMF_OK) {
      return error;
    }
    return error;
  }
  if(compositeStrcmp(compositeitfname, "fillbufferdone[7]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_fillbufferdone[7].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    _xyitfcb_fillbufferdone[7] = (armnmf_emptythisbufferDescriptor*)0xBEEF;
    error = (t_nmf_error)CM_BindComponentFromUser(
         server, 
         serveritfname, 
         size, 
         &_xyuv_Processor.fillbufferdone[7]);
    if(error != NMF_OK) {
      return error;
    }
    return error;
  }
  if(compositeStrcmp(compositeitfname, "fillbufferdone[8]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_fillbufferdone[8].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    _xyitfcb_fillbufferdone[8] = (armnmf_emptythisbufferDescriptor*)0xBEEF;
    error = (t_nmf_error)CM_BindComponentFromUser(
         server, 
         serveritfname, 
         size, 
         &_xyuv_Processor.fillbufferdone[8]);
    if(error != NMF_OK) {
      return error;
    }
    return error;
  }
  if(compositeStrcmp(compositeitfname, "fillbufferdone[9]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_fillbufferdone[9].IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    _xyitfcb_fillbufferdone[9] = (armnmf_emptythisbufferDescriptor*)0xBEEF;
    error = (t_nmf_error)CM_BindComponentFromUser(
         server, 
         serveritfname, 
         size, 
         &_xyuv_Processor.fillbufferdone[9]);
    if(error != NMF_OK) {
      return error;
    }
    return error;
  }
  if(compositeStrcmp(compositeitfname, "proxy") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_proxy.IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    _xyitfcb_proxy = (eventhandlerDescriptor*)0xBEEF;
    error = (t_nmf_error)CM_BindComponentFromUser(
         server, 
         serveritfname, 
         size, 
         &_xyuv_Processor.proxy);
    if(error != NMF_OK) {
      return error;
    }
    return error;
  }
  if(compositeStrcmp(compositeitfname, "ToOMXComponent") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(! _xyitfcb_ToOMXComponent.IsNullInterface())
      return NMF_INTERFACE_ALREADY_BINDED;
    _xyitfcb_ToOMXComponent = (openmax_processor_api_ToOMXComponentDescriptor*)0xBEEF;
    error = (t_nmf_error)CM_BindComponentFromUser(
         server, 
         serveritfname, 
         size, 
         &_xyuv_Processor.ToOMXComponent);
    if(error != NMF_OK) {
      return error;
    }
    return error;
  }
#endif
  return error;
}
t_nmf_error openmax_processor_wrpImpl::unbindToMPC(const char *compositeitfname, t_nmf_component_handle server, const char* serveritfname) {
#if defined(__STN_8500)
  if(compositeStrcmp(compositeitfname, "emptybufferdone[0]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_emptybufferdone[0].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    CM_UnbindComponentFromUser(&_xyuv_Processor.emptybufferdone[0]);
    _xyuv_Processor.emptybufferdone[0] = 0;
    _xyitfcb_emptybufferdone[0] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(compositeitfname, "emptybufferdone[1]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_emptybufferdone[1].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    CM_UnbindComponentFromUser(&_xyuv_Processor.emptybufferdone[1]);
    _xyuv_Processor.emptybufferdone[1] = 0;
    _xyitfcb_emptybufferdone[1] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(compositeitfname, "emptybufferdone[2]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_emptybufferdone[2].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    CM_UnbindComponentFromUser(&_xyuv_Processor.emptybufferdone[2]);
    _xyuv_Processor.emptybufferdone[2] = 0;
    _xyitfcb_emptybufferdone[2] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(compositeitfname, "emptybufferdone[3]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_emptybufferdone[3].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    CM_UnbindComponentFromUser(&_xyuv_Processor.emptybufferdone[3]);
    _xyuv_Processor.emptybufferdone[3] = 0;
    _xyitfcb_emptybufferdone[3] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(compositeitfname, "emptybufferdone[4]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_emptybufferdone[4].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    CM_UnbindComponentFromUser(&_xyuv_Processor.emptybufferdone[4]);
    _xyuv_Processor.emptybufferdone[4] = 0;
    _xyitfcb_emptybufferdone[4] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(compositeitfname, "emptybufferdone[5]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_emptybufferdone[5].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    CM_UnbindComponentFromUser(&_xyuv_Processor.emptybufferdone[5]);
    _xyuv_Processor.emptybufferdone[5] = 0;
    _xyitfcb_emptybufferdone[5] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(compositeitfname, "emptybufferdone[6]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_emptybufferdone[6].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    CM_UnbindComponentFromUser(&_xyuv_Processor.emptybufferdone[6]);
    _xyuv_Processor.emptybufferdone[6] = 0;
    _xyitfcb_emptybufferdone[6] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(compositeitfname, "emptybufferdone[7]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_emptybufferdone[7].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    CM_UnbindComponentFromUser(&_xyuv_Processor.emptybufferdone[7]);
    _xyuv_Processor.emptybufferdone[7] = 0;
    _xyitfcb_emptybufferdone[7] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(compositeitfname, "emptybufferdone[8]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_emptybufferdone[8].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    CM_UnbindComponentFromUser(&_xyuv_Processor.emptybufferdone[8]);
    _xyuv_Processor.emptybufferdone[8] = 0;
    _xyitfcb_emptybufferdone[8] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(compositeitfname, "emptybufferdone[9]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_emptybufferdone[9].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    CM_UnbindComponentFromUser(&_xyuv_Processor.emptybufferdone[9]);
    _xyuv_Processor.emptybufferdone[9] = 0;
    _xyitfcb_emptybufferdone[9] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(compositeitfname, "fillbufferdone[0]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_fillbufferdone[0].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    CM_UnbindComponentFromUser(&_xyuv_Processor.fillbufferdone[0]);
    _xyuv_Processor.fillbufferdone[0] = 0;
    _xyitfcb_fillbufferdone[0] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(compositeitfname, "fillbufferdone[1]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_fillbufferdone[1].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    CM_UnbindComponentFromUser(&_xyuv_Processor.fillbufferdone[1]);
    _xyuv_Processor.fillbufferdone[1] = 0;
    _xyitfcb_fillbufferdone[1] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(compositeitfname, "fillbufferdone[2]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_fillbufferdone[2].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    CM_UnbindComponentFromUser(&_xyuv_Processor.fillbufferdone[2]);
    _xyuv_Processor.fillbufferdone[2] = 0;
    _xyitfcb_fillbufferdone[2] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(compositeitfname, "fillbufferdone[3]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_fillbufferdone[3].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    CM_UnbindComponentFromUser(&_xyuv_Processor.fillbufferdone[3]);
    _xyuv_Processor.fillbufferdone[3] = 0;
    _xyitfcb_fillbufferdone[3] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(compositeitfname, "fillbufferdone[4]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_fillbufferdone[4].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    CM_UnbindComponentFromUser(&_xyuv_Processor.fillbufferdone[4]);
    _xyuv_Processor.fillbufferdone[4] = 0;
    _xyitfcb_fillbufferdone[4] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(compositeitfname, "fillbufferdone[5]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_fillbufferdone[5].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    CM_UnbindComponentFromUser(&_xyuv_Processor.fillbufferdone[5]);
    _xyuv_Processor.fillbufferdone[5] = 0;
    _xyitfcb_fillbufferdone[5] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(compositeitfname, "fillbufferdone[6]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_fillbufferdone[6].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    CM_UnbindComponentFromUser(&_xyuv_Processor.fillbufferdone[6]);
    _xyuv_Processor.fillbufferdone[6] = 0;
    _xyitfcb_fillbufferdone[6] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(compositeitfname, "fillbufferdone[7]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_fillbufferdone[7].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    CM_UnbindComponentFromUser(&_xyuv_Processor.fillbufferdone[7]);
    _xyuv_Processor.fillbufferdone[7] = 0;
    _xyitfcb_fillbufferdone[7] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(compositeitfname, "fillbufferdone[8]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_fillbufferdone[8].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    CM_UnbindComponentFromUser(&_xyuv_Processor.fillbufferdone[8]);
    _xyuv_Processor.fillbufferdone[8] = 0;
    _xyitfcb_fillbufferdone[8] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(compositeitfname, "fillbufferdone[9]") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_fillbufferdone[9].IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    CM_UnbindComponentFromUser(&_xyuv_Processor.fillbufferdone[9]);
    _xyuv_Processor.fillbufferdone[9] = 0;
    _xyitfcb_fillbufferdone[9] = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(compositeitfname, "proxy") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_proxy.IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    CM_UnbindComponentFromUser(&_xyuv_Processor.proxy);
    _xyuv_Processor.proxy = 0;
    _xyitfcb_proxy = 0x0;
    return NMF_OK;
  }
  if(compositeStrcmp(compositeitfname, "ToOMXComponent") == 0) {
    if(startNumber > 0)
      return NMF_COMPONENT_NOT_STOPPED;
    if(_xyitfcb_ToOMXComponent.IsNullInterface())
      return NMF_INTERFACE_NOT_BINDED;
    CM_UnbindComponentFromUser(&_xyuv_Processor.ToOMXComponent);
    _xyuv_Processor.ToOMXComponent = 0;
    _xyitfcb_ToOMXComponent = 0x0;
    return NMF_OK;
  }
#endif
  return NMF_NO_SUCH_REQUIRED_INTERFACE;
}
void openmax_processor_wrpImpl::dumpInstantiation() {
  nmfTraceComponent(TRACE_COMPONENT_COMMAND_ADD, (NMF::Primitive*)&_xyuv_Processor, _xyuv_Processor.name, _xyuv_Processor.templateName);
  nmfTraceInterfaceMethod((NMF::Primitive*)&_xyuv_Processor, "fsmInit", (0 << 8 | 0));
  nmfTraceInterfaceMethod((NMF::Primitive*)&_xyuv_Processor, "setTunnelStatus", (0 << 8 | 1));
  nmfTraceInterfaceMethod((NMF::Primitive*)&_xyuv_Processor, "sendCommand", (1 << 8 | 0));
  nmfTraceInterfaceMethod((NMF::Primitive*)&_xyuv_Processor, "processEvent", (2 << 8 | 0));
  nmfTraceInterfaceMethod((NMF::Primitive*)&_xyuv_Processor, "start", (3 << 8 | 0));
  nmfTraceInterfaceMethod((NMF::Primitive*)&_xyuv_Processor, "stop", (4 << 8 | 0));
  nmfTraceInterfaceMethod((NMF::Primitive*)&_xyuv_Processor, "emptyThisBuffer", (5 << 8 | 0));
  nmfTraceInterfaceMethod((NMF::Primitive*)&_xyuv_Processor, "fillThisBuffer", (6 << 8 | 0));
  nmfTraceInterfaceMethod((NMF::Primitive*)&_xyuv_Processor, "setParam", (7 << 8 | 0));
  nmfTraceInterfaceMethod((NMF::Primitive*)&_xyuv_Processor, "setParameter", (7 << 8 | 1));
  nmfTraceInterfaceMethod((NMF::Primitive*)&_xyuv_Processor, "setConfig", (8 << 8 | 0));
  nmfTraceInterfaceMethod((NMF::Primitive*)&_xyuv_Processor, "setTargetComponent", (8 << 8 | 1));
  nmfTraceBind(TRACE_BIND_COMMAND_BIND_ASYNCHRONOUS, (NMF::Primitive*)&_xyuv_Processor, "me", (NMF::Primitive*)&_xyuv_Processor, "postevent");
}

void openmax_processor_wrpImpl::dumpInstantiationJumper(void* _this) {
  openmax_processor_wrpImpl* myself = (openmax_processor_wrpImpl*)_this;
  myself->dumpInstantiation();
}

void openmax_processor_wrpImpl::dumpDestruction() {
  nmfTraceBind(TRACE_BIND_COMMAND_UNBIND_ASYNCHRONOUS, (NMF::Primitive*)&_xyuv_Processor, "me", (NMF::Primitive*)&_xyuv_Processor, "postevent");
  nmfTraceComponent(TRACE_COMPONENT_COMMAND_REMOVE, (NMF::Primitive*)&_xyuv_Processor, _xyuv_Processor.name, _xyuv_Processor.templateName);
}

