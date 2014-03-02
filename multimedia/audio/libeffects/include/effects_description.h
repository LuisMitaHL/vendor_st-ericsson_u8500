#ifndef __EFFECTSDESCRIPTION_H_
#define __EFFECTSDESCRIPTION_H_

#include <buffer.idt>
#include <stdbool.h>
#include <omxevent.idt>
#include <memorybank.idt>

/// max length for effect name
#define EFFECT_NAME_MAX_LENGTH   64

typedef enum {
    EFFECT_INPLACE,
    EFFECT_NOT_INPLACE
} t_EffectProcessMode;

typedef enum {
    EFFECT_ERROR_NONE,
    EFFECT_ERROR,
    EFFECT_NO_MORE_MEMORY
} t_EffectError;

typedef struct EffectInit_t {
    void            *pTraceObject;
    t_memory_bank   nMemoryBank;
    void        (*eventHandler)(OMX_EVENTTYPE event, unsigned long data1, unsigned long data2, void *pEffectCtx);
} EffectInit_t;

typedef struct PcmSettings_t {
    int     nChannels;
    int     nBlockSize;
    int     nSamplingRate;
} PcmSettings_t;

typedef struct EffectOpen_t {
    PcmSettings_t   sInputPcmSettings;
    PcmSettings_t   sOutputPcmSettings;
} EffectOpen_t;

//Structure that defines the effect interface
typedef struct {
    //construct method is the first method to be called. It is called once to initialize the library.
    //This method should allocate and initialize all the necessary data structure, and then return a pointer on its context.
    //This context will be passed as first argument in all other interface functions.
    //In case of error *pEffectCtx MUST also be set to NULL.
    t_EffectError (*effect_construct)(EffectInit_t *pInitParameters, void** pEffectCtx);

    //set static parameters. Allowed only before calling open.
    t_EffectError (*effect_set_parameter)(void *pEffectCtx, int index, void *param_struct);

    //set dynamic parameters. Allowed anytime except prior to construct.
    t_EffectError (*effect_set_config)(void *pEffectCtx, int index, void *config_struct);

    //open method must be called prior to calling processBuffer and after call to construct method.
    //It freezes the static configuration, and finalizes memory allocation (if depending on static parameters).
    //processMode is a return value to indicate if effect is working in inplace or not.
    t_EffectError (*effect_open)(void *pEffectCtx, EffectOpen_t* pOpenParameters, t_EffectProcessMode *processMode);

    //processBuffer method.
    //If effect is working in inplace mode, then processBuffer must be called with same buffer as input and output. And returned values
    //needInputBuf and filledOutputBuffer are ignored.
    t_EffectError (*effect_processBuffer)(void* pEffectCtx, Buffer_p inputBuf, Buffer_p outputBuf, bool *needInputBuf, bool *filledOutputBuffer);

    //getOutputBuffer is used to get internal effect buffer when effect is working in NOT inplace mode.
    //TODO: to be removed when a bufferPool shared between all effects.
    Buffer_p (*effect_getOutputBuffer)(void* pEffectCtx);

    //close method resets only the internal variables of the effect. It does not reset the config.
    //It should free memory allocated in open method.
    //It allows to change static parameters without calling the destroy method.
    void (*effect_close)(void* pEffectCtx);

    //called once to destroy the library. It should free all the memory used by the library.
    //After that call, pEffectCtx is no more valid.
    void (*effect_destroy)(void* pEffectCtx);
} EffectDescription_t;

typedef EffectDescription_t * (*EFFECT_FACTORYMETHODTYPE) (void);

#endif
