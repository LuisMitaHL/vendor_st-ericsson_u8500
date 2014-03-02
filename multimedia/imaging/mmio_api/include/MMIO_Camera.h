/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef MMIO_CAMERA_H_
#define MMIO_CAMERA_H_

#include "OMX_Core.h"
#include "OMX_Image.h"

#include <sys/wait.h>
#include <semaphore.h>


#define CAM_TRACE_TIMEOUT_MSEC          30
#define CAM_TRACE_DATA_BLOCK_SIZE    124
#define CAM_TRACE_DEFAULT_MSG_ID       (0xCDCDCDCD)
#define CAM_TRACE_BLOCK_SIZE              128
#define CAM_TRACE_NB_BLOCK                 250
#define CAM_SHARED_MEM_TRACE_BUFFER_SIZE       (16 + ( CAM_TRACE_BLOCK_SIZE * CAM_TRACE_NB_BLOCK )) //4x4 bytes for header plus N blocks of 128 bytes
namespace MMIO_Camera {

    /**
    * Camera Function.
    */
    typedef enum {ePrimaryXSD, eSecondaryXSD} T_CameraFunction;

    /**
    * Camera Type.
    */
    typedef enum { eNone, eRawBayer, eYUV} T_CameraType;

    /**
    * Open the connection with the kernel driver.
    */
    OMX_ERRORTYPE initDriver();

    /**
    * Close the connection with the kernel driver.
    */
    OMX_ERRORTYPE releaseDriver();

    /**
    * Select a camera.
    * @param camera 1=primary camera
    *               2=secondary camera...
    * @param type indicate if camera is RAW or YUV or ...
    */
    OMX_ERRORTYPE selectCamera(int camera,
            T_CameraType type);

    /**
    * Release the camera.
    * @param camera 1=primary camera
    *               2=secondary camera...
    * @note you must not call any other function of this API once you've
    *       released the camera.
    */
    OMX_ERRORTYPE releaseCamera(int camera);

    /**
    * Perform any requiered initialization.
    * This function must be called before any other ones from this API.
    * Not calling this function first has undefined behavior.
    */
    OMX_ERRORTYPE initBoard(T_CameraFunction function = ePrimaryXSD);
    OMX_ERRORTYPE initHardware();

    /**
    * Perform any requiered desinitialization.
    */
    OMX_ERRORTYPE desinitBoard();

    /**
    * Switches the extclk on and off (needed for sleep management).
    */
    OMX_ERRORTYPE setExtClk(OMX_BOOL powerOn);

    /**
    * Powers the sensor.
    * Digital power (VDIG) should be always on. However the analog power
    * (VANA, steming from VAUX) must be switched on/off dynamically.
    * Once the sensor is powered on, it enters its HW standby until its
    * Xshutdown is toggled high.
    */
    OMX_ERRORTYPE powerSensor(OMX_BOOL powerOn);

    /**
    * Enable timers on MMDSP Interrupt Controller.
    */
    void initMMDSPTimer(void);

    /**
    * Reamap an area of ARM memory in XP70 IO region.
    * This area is used to exchange some statistics.
    * @note The function actually remaps a much larger size than what is needed (64Mo).
    * @param physicalAddr Physical address on ARM side of the region to remap.
    * @return corresponding address on XP70 side.
    */
    OMX_U32 mapStatisticsMemArea(OMX_U32 physicalAddr);


    OMX_ERRORTYPE setPrimaryCameraHWInterface(void);
    OMX_ERRORTYPE releasePrimaryCameraHWInterface(void);
    OMX_ERRORTYPE setSecondaryCameraHWInterface(void);
    OMX_ERRORTYPE releaseSecondaryCameraHWInterface(void);

    /**
    * Very old version of XP70 loader. Kept for backward compatibility.
    */
    OMX_ERRORTYPE loadXP70FW(const void* addr, OMX_U32 isp_fw_size);

    /**
    * Load XP70 firmware. Old version for 3-part firmware.
    * @param phyAddrExt        Pointer to extended part of the firmware. This part
    *                                         is usually stored in ESRAM. It must be align on 64Ko.
    *                    It will be remapped in REGION_0 on XP70 side.
    * @param sizeExt                Size of extended part. It is currently limited to 64Ko.
    * @param addrSplit        Pointer to main program part, that will be copied to
    *                    XP70 internal program ram.
    * @param sizeSplit        Size of main program part. max 64Ko.
    * @param addrData        Pointer to data part, that will be copied to
    *                    XP70 internal data ram.
    * @param sizeData        Size of data part. max 64Ko.
    * @note  phyAddrExt must remain valid after the call to this function
    *        since the data are not copied.
    */
    OMX_ERRORTYPE loadXP70FW(const void* phyAddrExt, OMX_U32 sizeExt,
                           const void* addrSplit, OMX_U32 sizeSplit,
                           const void* addrData, OMX_U32 sizeData);

    /**
    * Load XP70 firmware. New version for 4-part firmware.
    * @param phyAddrSdram Pointer to SDRAM extended part of the firmware.
    *                     It must be align on 64Ko.
    *                     It will be remapped in REGION_1 on XP70 side.
    * @param sizeSdram         Size of SDRAM extended part.
    *                     It is currently limited to 64Ko.
    * @param phyAddrEsram Pointer to ESRAM extended part of the firmware.
    *                                      It may not be 64Ko-align but must be contained
    *                     in a 64KB-align block.
    *                     It will be remapped in REGION_0 on XP70 side.
    * @param sizeEsram         Size of ESRAM extended part.
    *                     It is currently limited to 64Ko.
    * @param addrSplit        Pointer to main program part, that will be copied to
    *                    XP70 internal program ram.
    * @param sizeSplit        Size of main program part. max 64Ko.
    * @param addrData        Pointer to data part, that will be copied to
    *                    XP70 internal data ram.
    * @param sizeData        Size of data part. max 64Ko.
    * @note  phyAddrSdram and phyAddrEsram must remain valid after the call
    *        to this function since the data are not copied.
    */
    OMX_ERRORTYPE loadXP70FW(const void* phyAddrSdram, OMX_U32 sizeSdram,
                           const void* phyAddrEsram, OMX_U32 sizeEsram,
                           const void* addrSplit, OMX_U32 sizeSplit,
                           const void* addrData, OMX_U32 sizeData);

    /**
    * Copy 32-bit data to isp memory.
    * @param t1_dest                        address of isp memory, as seen by XP70. Refer to pictor documentation
    *                                       for an extensive list.
    * @param logical_addr        Data to be copied, stored on ARM side.
    * @param count                        number of 32-bit words in data.
    */
    OMX_ERRORTYPE ispVectorWrite( unsigned long t1_dest, const long int* logical_addr, int count );

    /**
    * Retrieve IP-GPIO numbers (i.e the gpio number as seen by XP70).
    * @param  function   Indicate which ip-gpio to query
    * @param  ip_gpio    On return, contains the IP-GPIO number
    * @return OMX_ErrorNone or other OMX error codes
    */

    OMX_ERRORTYPE getCameraFunction(T_CameraFunction function, unsigned int* ip_gpio);

    /**
    * Creates a Process which will be dumping the FW traces
    * from the buffer pointed by address in "logAddrTrace"
    * @param  logAddrTrace   address of the trace buffer
    * @param  sizeTrace   size of the trace buffer
    * @return OMX_ErrorNone or other OMX error codes
    */

    OMX_ERRORTYPE setTraceBuffer(OMX_U32 logAddrTrace, OMX_U32 sizeTrace);

    /**
     * @return size of NVM data, if present. Returns 0, otherwise.
     */
    OMX_U32 isNVMdataPresent();

    /**
     * Copies the stored NVM data in supplied buffer
     * @param aNVMBuffer Destination buffer which should be filled with NVM data
     * @return OMX_ErrorNone, if successful, appropriate OMX Error code otherwise
     */
    OMX_ERRORTYPE readNVMdata(void* aNVMBuffer);

    /**
     * Stores the NVM data present in supplied buffer
     * @param aNVMBuffer Destination buffer which contains the NVM data, to be stored
     * @param aDataSize  Size of NVM data
     * @return OMX_ErrorNone, if successful, appropriate OMX Error code otherwise
     */
    OMX_ERRORTYPE writeNVMdata(void* aNVMBuffer, OMX_U32 aDataSize);

    /**
     * Stores the NVM data present in supplied buffer
     * @param aNVMBuffer Destination buffer which contains the NVM data, to be stored
     * @param aDataSize  Size of NVM data
     * @return OMX_ErrorNone, if successful, appropriate OMX Error code otherwise
     */


    /**
    * Parents send the signal to child process
    * and parents process waits for the child process to exit
    * @return OMX_ErrorNone or other OMX error codes
    */
    OMX_ERRORTYPE resetTraceBuffer(void);

    /**
     * Creates a empty signal set waitset via sigemptyset()  which will
     * store the SIGNAL list via sigaddset();
     * currently onlySIGALRM and SIGUSR1 is put in the list.
     * It unblocks itself from the inherited signal mask to receive the signals for SIGALRM and SIGUSR1
     * and  waits for semaphore to be posted .
     * @param Trace_buff_Log_Addr  trace buffer address mapped in arm's logical address space
     */

    void executeXP70TraceProcess(unsigned int* Trace_buff_Log_Addr);

    /**
     * it is called once the process  receives the SIGALRM signal sent by settitimer().
     *  In the signal handler we dump the traces.
     * reset the signal handler to receive the SIGALRM signal or SIGUSR1 for   next time.
     * @param signum signal number sent
     */

    void XP70TraceSignalHandler(int signum);

     /**
     * calculates the nuber of log mesages to be printed and prints them
     * @param trace_log_addr  trace buffer address mapped in arm's logical address space
     */

    int dumpTrace(void* trace_log_addr);

    /**
    * Trace Buffer meta data to store prev_overwrite_count and prev_block_id.
    */
    struct FW_trace_buffer_status{
        int prev_overwrite_count;
        int prev_block_id;
    };

    /**
    * trace_block structure .
    */
    struct trace_block{
        unsigned int  msg_id;
        char data[CAM_TRACE_DATA_BLOCK_SIZE];
    };

    /**
    * mmio_trace contains tarce block to hold the data and trace buffer meta data.
    */
    struct mmio_trace{
        unsigned int nb_block;
        unsigned int block_size;
        int block_id;
        int overwrite_count;
        struct trace_block block[CAM_TRACE_NB_BLOCK];
    };


    struct trace_param{
    /**
    * itimerval to trigger the SIGALRM after given interval.
    */
     struct itimerval tout_val;
    /**
    * variablwe to check whether logging has finished and we need to stop the timer .
    */
      volatile int wait_over;
    /**
    * pid of the child process.
    */
      pid_t  ch_pid ;
    /**
    * semaphore to block the child process till alarm signal comes.
    */
     sem_t my_semaphore;
    };

    OMX_ERRORTYPE  deinterlaceFrame( char* data, void* src_hwmem_buf, void* dst_hwmem_buf, int width, int height );
    OMX_ERRORTYPE  downloadFirmware();

} /* namespace MMIO_Camera */

#endif /*MMIO_CAMERA_H_*/
