/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/**
* \file    flash_api.h
* \brief   Define the API between flash drivers and its clients (eg. hsmcamera)
* \author  ST-Ericsson
*
* The flash driver API is a C++ singleton class, implemented as a DLL.
* It is implemented by ST-Ericsson upon an OS dependent driver. The design of the API implementation is beyond the
* scope of this document. Possibly the implementation can rely on one or more SW drivers.
*
* ** Asynchronous Event Summary ** \n
* The driver is likely to report asynchronous events like errors, readiness lost or achieved.
* Each time such event occurs, a callback function, passed through EnableFlashMode() is called by the user library.
* The callback is called in-context.
* DoSelfTest also is asynchronous. When the self-test completes (may happen few hundreds of ms after the call),
* a callback returns the result of the self-test.
*
* ** Example Sequence Diagrams: ** \n
* \image html StillLedExternalStrobe.png "Still LED External Strobe"
* \image html StillLed.png "Still LED"
* \image html XenonExternalStrobe.png "Xenon External Strobe"
* \image html VideoLed.png "Video LED"
* \image html AFAssistAndMain.png "AF assistant and Main flash"
* \image html SelfTests.png "Self Tests"
*
*
*
*/

#ifndef __FLASH_API_H__
#define __FLASH_API_H__

#include <flash_bitfields.h>

#ifdef __SYMBIAN32__
#include <e32def.h>
typedef TUint8  flash_uint8_t;
typedef TUint16 flash_uint16_t;
typedef TUint32 flash_uint32_t;
#elif 0
typedef unsigned char  flash_uint8_t;
typedef unsigned short flash_uint16_t;
typedef unsigned int   flash_uint32_t;
#else
#include <stdint.h>
typedef uint8_t  flash_uint8_t;
typedef uint16_t flash_uint16_t;
typedef uint32_t flash_uint32_t;
#endif

/** \brief Return codes of the Flash API */
typedef enum {
    FLASH_RET_NONE                =    0, /**< No error*/
    FLASH_RET_MODE_NOT_SUPPORTED  =   -1, /**< The selected mode is not supported*/
    FLASH_RET_MODE_ENABLE_FAILURE =   -2, /**< Unable to enable a flash mode*/ // used?
    FLASH_RET_UNSUPPORTED_SETTING =   -3, /**< Setting is not supported*/
    FLASH_RET_STROBE_FAILURE      =   -4, /**< Unable to strobe the flash*/
    FLASH_RET_STATUS_FAILURE      =   -5, /**< Unable to return the flash status*/
    FLASH_RET_COUNTER_FAILURE     =   -6, /**< Unable to return the flash counter*/
    FLASH_RET_INVALID_CALL        =   -7, /**< API call not allowed in current state*/
    FLASH_RET_OUT_OF_MEM          =   -8, /**< Allocation issue, can be returned by any function*/
    FLASH_RET_HARDWARE_FAILURE    =   -9, /**< I2C communication error*/
    FLASH_RET_UNKNOWN             = -100  /**< Unknown return status*/
}TFlashReturnCode;


typedef flash_uint16_t TFlashStatus; /*Status BitMap*/

typedef unsigned int TFlashMode;

enum TCameraId { EPrimary, ESecondary};

typedef struct {
    flash_uint32_t FlashDriverType;    /**< */
    flash_uint32_t FlashDriverVersion; /**< */
    flash_uint32_t FlashLedType;       /**< */
    flash_uint32_t MaxIntensity;       /**< mA*/
    flash_uint32_t MinIntensity;       /**< mA*/
    flash_uint32_t MaxStrobeDuration;       /**< us*/
    bool           IsIntensityProgrammable; /**< The current intensity for this mode might not be programmable. */
    bool           IsDurationProgrammable;  /**< In case of 2-bit interface and level sensitive GPIO, the duration might not be programmable through a register. */
    bool           IsTimeoutProgrammable;   /**< There might be no capability for HW timeout programming. */
    flash_uint32_t NbFaultRegisters;        /**< Number of fault registers */
    flash_uint32_t NVMSize;            /**< Size of flash non volatile memory */
}TFlashDetails;

typedef flash_uint32_t TFlashSelftest; /*Flash Selftest BitMap*/

typedef void (*TCallbackFn)(TFlashError aError, TFlashStatus aEvent, void *apContext);
typedef void (*TSelfTestCallbackFn)(TFlashSelftest aResult, void *apContext);
typedef void (*TGetNVMCallbackFn)(TFlashReturnCode aReturnStatus, void *apContext);


class CFlashDriver {
    public:

        /**
         * \brief  Initialises the communication channel with the flash driver.
         *         Static factory function. Returns null if the opening failed.
         *         A single instance of the driver is instantiated at a time (singleton).
         *         It is not planned to have several clients using the user API at the same time.
         * \return an instance of the flash driver singleton
         **/
        static CFlashDriver* Open();

        /**
         * \brief  Closes the communication channel. Should be called when the driver user has finished to use it.
         **/
        virtual void Close() = 0;

        /**
         * \brief  Constructor
         **/
        virtual ~CFlashDriver() {};

        /**
         * \brief  Returns a bitfield of supported flash modes.
         *    External strobe means the driver does not have/implement the capability to strobe the flash on/off and this
         *    capability is delegated to another SW component (typically deferred to the sensor driver for still modes:
         *    the ISP FW programs the sensor so that it strobes the flash through its dedicated GPIO, on a given reference
         *    point, with a given duration). For video modes however the flash driver generally implements strobing
         *    through I2C or 2-bit logic.
         *
         *    For instance a platform could return "0b11111001100" meaning the whole flash driver supports
         *    the following capabilities:
         *      \li Xenon with external strobe
         *      \li Video led
         *      \li Still led with external strobe
         *      \li Af assistant
         *      \li Privacy indicator
         *      \li HPLED
         *      \li HPLED with external strobe
         *
         * \out aModes    Bitfield composed of FLASH_MODE_XXXX values
         * \in  aCameraId Camera for which the flash is considered.
         **/
        virtual void GetSupportedFlashModes(
                TFlashMode& aModes,
                TCameraId aCameraId=EPrimary) =0;

        /**
         * \brief  Returns the characteristics of one flash mode.
         *
         * \in  aFlashMode A single flash mode (not a combination)
         * \out aDetails   Characteristics of the selected flash mode
         * \in  aCameraId Camera for which the flash is considered.
         * \return #FLASH_RET_NONE or #FLASH_RET_MODE_NOT_SUPPORTED
         **/
        virtual TFlashReturnCode GetFlashModeDetails(
                TFlashMode aFlashMode,
                TFlashDetails& aDetails,
                TCameraId aCameraId=EPrimary) = 0;

        /**
         * \brief  Enable the HW driver for one ore more flash modes.
         *  Enabling means moving the HW driver out of shutdown mode: when enabled, the underlying flash IC can be programmed
         *  and then fired.
         *
         *  When xenon is enabled, its charging is started asap. It is the responsibility of the driver to maintain the charge
         *  of the xenon despite its natural drain and in case the flash is partially or fully discharged after a strobe.
         *
         *  The function returns after flash driver HW programming. It does not wait for the xenon charging to complete (this
         *  latter can take few seconds and is notified through callback mechanism).
         *
         *  A callback function is passed when enabling the mode. It is called by the driver each time an asynchronous event
         *  worth to be notified to the driver client occurs: change of state, errors.
         *  The following status change events are notified asynchronously: #FLASH_STATUS_BROKEN, #FLASH_STATUS_READY, #FLASH_STATUS_NOT_READY
         *
         *  Those events are a subset of the TFlashStatus enum. Other events are not signaled asynchronously.
         *  They only can be queried with GetStatus() method.
         *
         *  All errors happening asynchronously are notified through this mechanism.
         *
         *  apContext pointer enables the client to pass a
         *
         *  The callback is automatically unregistered when disabling the associated flash mode.
         *
         *  The callback is executed in the same thread as the flash driver user API implementation.
         *
         *  In case the client attempts to enable a mode that is already enabled, the order is simply ignored and no error is returned.
         *
         *
         * \in  aFlashMode Bitfield composed of FLASH_MODE_XXXX values
         * \in  aCallback  Callback to report asynchronous event to the client.
         * \in  apContext  Context that is passed back in the callback.
         * \in  aCameraId Camera for which the flash is considered.
         * \return Return codes are among: #FLASH_RET_NONE, #FLASH_RET_MODE_NOT_SUPPORTED, #FLASH_RET_MODE_ENABLE_FAILURE, #FLASH_RET_HARDWARE_FAILURE
         **/
        virtual TFlashReturnCode EnableFlashMode(
                TFlashMode aFlashMode,
                TCallbackFn aCallback,
                void *apContext,
                TCameraId aCameraId=EPrimary) = 0;

        /**
         * \brief  Set the driver configuration for the given flash mode.
         *
         * Usually a configuration applies to a single operating mode (no usage of bit-field foreseen).
         * If duration is set to 0, the strobe length is either controlled externally or through explicit strobe-off order.
         * For example, duration is set to 0 for video LED which is turned off explicitly by the flash driver client when
         * the video recording is over. The timeout parameter (in ms) enables to program a HW timeout so that the duration
         * of the strobe never exceeds this timeout value.
         *
         * A value of 0 values for duration, intensity and timeout means the client does not program them.
         * In case the client attempts to program a parameter that is not programmable, an error is returned.
         *
         * Without this function call, default settings are applied. As a consequence, once a mode is enabled through
         * EnableFlashMode, it is ready to be fired.
         *
         * Intensity is given as a percentage of the full intensity of the current flash mode.
         *
         * It is not expected the client configures the flash when it is fired. In that case the driver might
         * return FlashErrorInvalidCall.
         *
         * \in  aFlashMode A single flash mode (not a combination)
         * \in  aDuration  In us
         * \in  aIntensity In percentage of the full intensity of the current flash mode
         * \in  aTimeout   In ms
         * \in  aCameraId Camera for which the flash is considered.
         * \return Return codes are among: #FLASH_RET_NONE, #FLASH_RET_MODE_NOT_SUPPORTED, #FLASH_RET_UNSUPPORTED_SETTING, #FLASH_RET_INVALID_CALL, #FLASH_RET_HARDWARE_FAILURE
         **/
        virtual TFlashReturnCode ConfigureFlashMode(
                TFlashMode aFlashMode,
                flash_uint32_t aDuration,
                flash_uint32_t aIntensity,
                flash_uint32_t aTimeout,
                TCameraId aCameraId=EPrimary) = 0;

        /**
         * \brief  Requests the flash driver to trigger the flash IC corresponding to the aFlashMode operating mode.
         * This can be handled through I2C command or GPIO toggling (level or edge sensitive strobe signal).
         *
         * Normally there should be a single flash fired at a time; flash strobe occurs immediately, ie. no delay is implemented.
         *
         * The function returns immediately.
         *
         * The duration of the strobe can be controlled by explicitly calling Strobe(false).
         * In case the client gave a duration or a timeout value when configuring the mode, the strobe is ended automatically
         * after this duration or timeout. The client is allowed to force the flash off before the duration/timeout instant.
         *
         * The Strobe command cannot be called in case of external strobe mode. By construction those modes are triggered on/off
         * by another SW component than the flash driver (typically the sensor driver).
         *
         * \in  aFlashMode A single flash mode (not a combination)
         * \in  aEnable    Tell if flash is to be lit or unlit.
         * \in  aCameraId Camera for which the flash is considered.
         * \return Return codes are among: #FLASH_RET_NONE, #FLASH_RET_MODE_NOT_SUPPORTED, #FLASH_RET_STATUS_FAILURE, #FLASH_RET_INVALID_CALL, #FLASH_RET_HARDWARE_FAILURE
         **/
        virtual TFlashReturnCode Strobe(
                TFlashMode aFlashMode,
                bool aEnable,
                TCameraId aCameraId=EPrimary) = 0;

        /**
         * \brief  Give the status of the driver for a specific flash mode
         *
         * \in  aFlashMode   The flash mode for which to check the status
         * \out aFlashStatus Status of the driver for the specified flash mode
         * \in  aCameraId Camera for which the flash is considered.
         * \return Return codes are among: #FLASH_RET_NONE, #FLASH_RET_MODE_NOT_SUPPORTED, #FLASH_RET_STATUS_FAILURE, #FLASH_RET_HARDWARE_FAILURE
         **/
        virtual TFlashReturnCode GetStatus(
                TFlashMode aFlashMode,
                TFlashStatus& aFlashStatus,
                TCameraId aCameraId=EPrimary) = 0;

        /**
         * \brief  Gives the current xenon life counter.
         *
         * The counter is a static member.
         *
         * \out aCounter Current xenon life counter.
         * \in  aCameraId Camera for which the flash is considered.
         * \return Return codes are among: #FLASH_RET_NONE, #FLASH_RET_MODE_NOT_SUPPORTED, #FLASH_RET_COUNTER_FAILURE, #FLASH_RET_HARDWARE_FAILURE
         **/
        virtual TFlashReturnCode GetXenonLifeCounter(
                flash_uint32_t &aCounter,
                TCameraId aCameraId=EPrimary) = 0;

        /**
         * \brief  Returns the selftests that are supported by the flash driver.
         *    The driver shall only implement self-contained selftests ie selftests that are fully mastered by the
         *    flash driver (namely those relying on internal strobe) and which do not depend on any external action.
         *
         * \out aSelfTests Bitfield of supported self-tests
         * \in  aCameraId Camera for which the flash is considered.
         * \return Return codes are among: #FLASH_RET_NONE
         **/
        virtual void GetSupportedSelfTests(
                TFlashSelftest& aSelfTests,
                TCameraId aCameraId=EPrimary) = 0;

        /**
         * \brief  Launches one or more selftests.
         *  When the set of selftest is complete a callback is called and returns the results for all the self-tests.
         *
         *  Selftest for a mode can be executed without preparing nor configuring the mode. The call is self-contained.
         *
         *  All the self-test results are contained in flash_uint32_t.  The same mask as TFlashMode can be used to retrieve
         *  the result of a given self-test. If the associated bit is 1 the test passed.
         *
         *  apContext pointer enables the client to pass a context that is passed back in the callback.
         *
         * \in  aSelfTest   Bitfield of self-tests to execute
         * \in  aSelfTestCb Callback to report self-tests results
         * \in  apContext   Context that is passed back in the callback
         * \in  aCameraId Camera for which the flash is considered.
         * \return Return codes are among: #FLASH_RET_NONE, #FLASH_RET_MODE_NOT_SUPPORTED, #FLASH_RET_INVALID_CALL, #FLASH_RET_HARDWARE_FAILURE
         **/
        virtual TFlashReturnCode DoSelfTest (
                TFlashSelftest aSelfTest,
                TSelfTestCallbackFn aSelfTestCb,
                void *apContext,
                TCameraId aCameraId=EPrimary) = 0;

        /**
         * \brief  Returns the fault register values if any.
         *  It is the responsibility of the client to allocate the memory for the NbFaultRegisters reported
         *  in GetFlashModeDetails() for the current mode.
         *
         * \in  aFlashModes      Flash mode for which to check the fault registers
         * \out apFaultRegisters Fault registers for the requested flash modes.
         * \in  aCameraId Camera for which the flash is considered.
         * \return Return codes are among: #FLASH_RET_NONE, #FLASH_RET_MODE_NOT_SUPPORTED, #FLASH_RET_INVALID_CALL (in case no fault register is supported), #FLASH_RET_HARDWARE_FAILURE
         **/
        virtual TFlashReturnCode GetFaultRegisters(
                TFlashMode aFlashModes,
                flash_uint32_t *apFaultRegisters,
                TCameraId aCameraId=EPrimary) = 0;

        /**
         * \brief  Fills the buffer with the flash non volatile memory
         *  The client is responsible to allocate the buffer. To know which size is needed, the client shall call
         *  GetFlashModeDetails and look at aDetails->NVMSize.
         *  This function is asynchronous because getting the NVM may take some time, and the driver will call the provided
         *  callback functions when done.
         *
         * \in  aFlashModes      Flash mode whose IC NVM is to be copied
         * \in  BufferSize       Size of the buffer in which to copy the NVM
         * \in  NVMBuffer        Buffer in which to copy the NVM
         * \in  cb               Callback function called when done
         * \in  apContext   Context that is passed back in the callback
         * \in  aCameraId Camera for which the flash is considered.
         * \return Return codes are among: #FLASH_RET_NONE, #FLASH_RET_MODE_NOT_SUPPORTED (in case the selected mode doesn't have a NVM) #FLASH_RET_INVALID_CALL (in case the buffer is too small), #FLASH_RET_HARDWARE_FAILURE (in case the NVM can't be accessed)
         **/
        virtual TFlashReturnCode GetNVM(
                TFlashMode aFlashModes,
                flash_uint32_t BufferSize,
                void* NVMBuffer,
                TGetNVMCallbackFn cb,
                void *apContext,
                TCameraId aCameraId=EPrimary) = 0;

    protected :
        CFlashDriver() {};

};
#endif //__FLASH_API_H__
