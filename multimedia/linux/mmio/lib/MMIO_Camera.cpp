/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#define DECLARE_AUTOVAR

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <linux/compiler.h>
#include <linux/mmio.h>
#include "ImgConfig.h"
#include "MMIO_Camera.h"
#include "mmio_linux_api.h"
#ifdef ENABLE_PMIC
  #include "i2c_lib.h"
#endif
#include "osi_trace.h"
#include <errno.h>

#define I2C_DEFAULT_TIMEOUT_ENTRY (10)
#define I2C_DEFAULT_TIMEOUT_EXIT (500)

#define SIZE_OF_BUF 100
char const *const PATH_NVM_DATA_SIZE =
    "/sys/devices/platform/misc/mmio_camera/nvm_data_size";

static int MMIO_Camera_fd = 0;
static MMIO_Camera::trace_param t_param = {{{0,0},{0,0}},0,0,{0}};

#ifdef ENABLE_PMIC
static bool i2c_passed;

OMX_ERRORTYPE set_PMIC(bool setTimeout)
{
    DBGT_PROLOG("%d\n", setTimeout);

    int ioctl_arg, err;
    i2c_init_params_t init_param;
    i2clib_handle_t i2c_handle = NULL;
    short int Data_p = 0;
    init_param.adapter = I2C_ADAP_2;
    init_param.slave_addr = 0x9E;
    ioctl_arg = MMIO_ACTIVATE_I2C_HOST;

    err = ioctl(MMIO_Camera_fd, MMIO_ACTIVATE_I2C2, &ioctl_arg);
    if (-1 == err) {
        DBGT_EPILOG("err = %d\n", err);
        return OMX_ErrorUndefined;
    }

    err = i2c_init(&init_param, &i2c_handle);
    if (err) {
        DBGT_EPILOG("err = %d\n", err);
        i2c_deinit(i2c_handle);
        return OMX_ErrorUndefined;
    }

    if (setTimeout) {
        err = i2c_set_timeout(i2c_handle, I2C_DEFAULT_TIMEOUT_ENTRY);
        if (err) {
            DBGT_EPILOG("err = %d\n", err);
            i2c_deinit(i2c_handle);
            return OMX_ErrorUndefined;
        }
    }

    Data_p = 0x8;
    Data_p = (Data_p << 8) | 0x1;
    err = i2c_write_p(i2c_handle, (u_int8_t *)&Data_p, 2);
    if (err) {
        i2c_passed = false;
        if (setTimeout) {
            err = i2c_set_timeout(i2c_handle, I2C_DEFAULT_TIMEOUT_EXIT);
            if (err) {
                DBGT_EPILOG("err = %d\n", err);
                i2c_deinit(i2c_handle);
                return OMX_ErrorUndefined;
            }
        }
        i2c_deinit(i2c_handle);

        ioctl_arg = MMIO_ACTIVATE_IPI2C2;
        err = ioctl(MMIO_Camera_fd, MMIO_ACTIVATE_I2C2, &ioctl_arg);
        if (err) {
            DBGT_EPILOG("err = %d\n", err);
            return OMX_ErrorUndefined;
        }
        return OMX_ErrorNotImplemented;
    }

    i2c_passed = true;

    if (setTimeout) {
        err = i2c_set_timeout(i2c_handle, I2C_DEFAULT_TIMEOUT_EXIT);
        if (err) {
            DBGT_EPILOG("err = %d\n", err);
            i2c_deinit(i2c_handle);
            return OMX_ErrorUndefined;
        }
    }

    Data_p = 0xC8;
    Data_p = (Data_p << 8) | 0x2;
    err = i2c_write_p(i2c_handle, (u_int8_t *)&Data_p, 2);
    Data_p = 0x0E;
    Data_p = (Data_p << 8) | 0x3;
    err = i2c_write_p(i2c_handle, (u_int8_t *)&Data_p, 2);
    Data_p = 0x00;
    Data_p = (Data_p << 8) | 0x4;
    err = i2c_write_p(i2c_handle, (u_int8_t *)&Data_p, 2);
    Data_p = 0x00;
    Data_p = (Data_p << 8) | 0x5;
    err = i2c_write_p(i2c_handle, (u_int8_t *)&Data_p, 2);
    Data_p = 0x00;
    Data_p = (Data_p << 8) | 0x6;
    err = i2c_write_p(i2c_handle, (u_int8_t *)&Data_p, 2);
    Data_p = 0x01;
    Data_p = (Data_p << 8) | 0x0;
    err = i2c_write_p(i2c_handle, (u_int8_t *)&Data_p, 2);
    usleep(1);
    Data_p = 0x09;
    Data_p = (Data_p << 8) | 0x0;
    err = i2c_write_p(i2c_handle, (u_int8_t *)&Data_p, 2);
    usleep(1);
    Data_p = 0x0B;
    Data_p = (Data_p << 8) | 0x0;
    err = i2c_write_p(i2c_handle, (u_int8_t *)&Data_p, 2);
    usleep(1);
    Data_p = 0x0F;
    Data_p = (Data_p << 8) | 0x0;
    err = i2c_write_p(i2c_handle, (u_int8_t *)&Data_p, 2);
    usleep(1);

    ioctl_arg = MMIO_ACTIVATE_IPI2C2;
    err = ioctl(MMIO_Camera_fd, MMIO_ACTIVATE_I2C2, &ioctl_arg);
    if (err) {
        DBGT_EPILOG("err = %d\n", err);
        return OMX_ErrorUndefined;
    }

    return OMX_ErrorNone;
}

OMX_ERRORTYPE unset_PMIC()
{
    DBGT_PROLOG();
    int ioctl_arg, err;
    i2c_init_params_t init_param;
    i2clib_handle_t i2c_handle = NULL;
    int Data_p = 0;
    init_param.adapter = I2C_ADAP_2;
    init_param.slave_addr = 0x9E;
    ioctl_arg = MMIO_ACTIVATE_I2C_HOST;

    err = ioctl(MMIO_Camera_fd, MMIO_ACTIVATE_I2C2, &ioctl_arg);
    if (-1 == err) {
        DBGT_EPILOG("err = %d\n", err);
        return OMX_ErrorUndefined;
    }

    err = i2c_init(&init_param, &i2c_handle);
    if (err) {
        DBGT_EPILOG("err = %d\n", err);
        i2c_deinit(i2c_handle);
        return OMX_ErrorUndefined;
    }

    Data_p = 0x00;
    Data_p = (Data_p << 8) | 0x0;

    err = i2c_write_p(i2c_handle, (u_int8_t *)&Data_p, 2);
    if (err) {
        i2c_deinit(i2c_handle);
        ioctl_arg = MMIO_ACTIVATE_IPI2C2;
        err = ioctl(MMIO_Camera_fd, MMIO_ACTIVATE_I2C2, &ioctl_arg);
        if (-1 == err) {
            DBGT_EPILOG("err = %d\n", err);
            return OMX_ErrorUndefined;
        }
        DBGT_EPILOG("err = %d", err);
        return OMX_ErrorNotImplemented;
    }

    usleep(1);
    Data_p = 0x00;
    Data_p = (Data_p << 8) | 0x1;
    err = i2c_write_p(i2c_handle, (u_int8_t *)&Data_p, 2);
    Data_p = 0x00;
    Data_p = (Data_p << 8) | 0x2;
    err = i2c_write_p(i2c_handle, (u_int8_t *)&Data_p, 2);
    Data_p = 0x00;
    Data_p = (Data_p << 8) | 0x3;
    err = i2c_write_p(i2c_handle, (u_int8_t *)&Data_p, 2);
    Data_p = 0x00;
    Data_p = (Data_p << 8) | 0x4;
    err = i2c_write_p(i2c_handle, (u_int8_t *)&Data_p, 2);
    Data_p = 0x00;
    Data_p = (Data_p << 8) | 0x5;
    err = i2c_write_p(i2c_handle, (u_int8_t *)&Data_p, 2);
    Data_p = 0x00;
    Data_p = (Data_p << 8) | 0x6;
    err = i2c_write_p(i2c_handle, (u_int8_t *)&Data_p, 2);

    ioctl_arg = MMIO_ACTIVATE_IPI2C2;
    err = ioctl(MMIO_Camera_fd, MMIO_ACTIVATE_I2C2, &ioctl_arg);
    if (err) {
        DBGT_EPILOG("err = %d\n", err);
        return OMX_ErrorUndefined;
    }

    return OMX_ErrorNone;
}
#endif

#ifdef MMIO_ENABLE_NEW_IF
static int MMIO_CameraSlot = -1;
static MMIO_Camera::T_CameraType MMIO_CameraType[2] = { MMIO_Camera::eNone, MMIO_Camera::eNone };

/******************************************************************************
 * Function Name:MMIO_Camera::selectCamera
 * -----------------------------------------
 * ParamName     |  Direction  | Description
 * -----------------------------------------
 * camera        |      in     | 1 = primar , 2 = secondary
 * type          |      in     | eRawBayer or eYUV
 *
 ******************************************************************************/
OMX_ERRORTYPE MMIO_Camera::selectCamera(int camera, T_CameraType type)
{
    GET_AND_SET_TRACE_LEVEL(mmiocamera);

    DBGT_PROLOG();

    MMIO_CameraSlot = camera;
    MMIO_CameraType[camera] = type;

    DBGT_PTRACE("MMIO_CameraSlot = %d, MMIO_CameraType = %d\n",
        MMIO_CameraSlot, MMIO_CameraType[camera]);
    DBGT_EPILOG();
    return OMX_ErrorNone;
}

/******************************************************************************
 * Function Name:MMIO_Camera::releaseCamera
 * -----------------------------------------
 * ParamName     |  Direction  | Description
 * -----------------------------------------
 * camera        |      in     | 1 = primary , 2 = secondary
 *
 ******************************************************************************/
OMX_ERRORTYPE MMIO_Camera::releaseCamera(int camera)
{
    DBGT_PROLOG("camera = %d\n", camera);

    MMIO_CameraType[camera]  = eNone;

    DBGT_EPILOG();
    return OMX_ErrorNone;
}

/******************************************************************************
 * Function Name:MMIO_Camera::initHardware
 * -----------------------------------------
 * ParamName     |  Direction  | Description
 * -----------------------------------------
 *     -         |     -       |    -
 *
 ******************************************************************************/
OMX_ERRORTYPE MMIO_Camera::initHardware()
{
    DBGT_PROLOG();

    int err;
    OMX_ERRORTYPE error = OMX_ErrorNone;

    struct mmio_input_output_t data;

    memset(&data, 0, sizeof(mmio_input_output_t));

    if (MMIO_CameraSlot == 1) {
        data.mmio_arg.camera_slot = PRIMARY_CAMERA;
    } else {
        data.mmio_arg.camera_slot = SECONDARY_CAMERA;
    }

    DBGT_PTRACE("camera slot = %d\n", data.mmio_arg.camera_slot);

    err = ioctl(MMIO_Camera_fd, MMIO_CAM_INITBOARD, &data);

    if (err) {
        DBGT_CRITICAL("MMIO_CAM_INITBOARD returned error = %d\n", err);
        error = OMX_ErrorUndefined;
    }

    DBGT_EPILOG("0x%X\n", error);

    return error;
}
#endif //MMIO_ENABLE_NEW_IF

/******************************************************************************
 * Function Name:MMIO_Camera::initDriver
 * -----------------------------------------
 * ParamName     |  Direction  | Description
 * -----------------------------------------
 *     -         |     -       |    -
 *
 ******************************************************************************/
OMX_ERRORTYPE MMIO_Camera::initDriver(void)
{
    DBGT_PROLOG();
    /*
     * The open() system call opens the mmio device driver and also does
     * any memory allocation in the kernel space if specified in the open()
     * API of the driver. It returns an unallocated file descriptor on
     * success and -1 on failure. On failure it sets 'errno' in the kernel
     * space to the err type.
     */
    char *mmiodevice;

#ifdef MMIO_ENABLE_NEW_IF
    if (MMIO_CameraType[MMIO_CameraSlot] == eRawBayer) {
        mmiodevice = (char *)"/dev/"MMIO_RAW_NAME;
    } else if (MMIO_CameraType[MMIO_CameraSlot] == eYUV) {
        mmiodevice = (char *)"/dev/"MMIO_YUV_NAME;
    } else {
        mmiodevice = (char *)"/dev/"MMIO_NAME;
    }
#else
    mmiodevice = (char *)"/dev/"MMIO_NAME;
#endif //MMIO_ENABLE_NEW_IF

    DBGT_PDEBUG("mmiodevice = %s\n", mmiodevice);

    MMIO_Camera_fd = open(mmiodevice, O_RDWR);

    if (-1 == MMIO_Camera_fd) {
        DBGT_EPILOG("Failed to open '%s' err = %d\n", mmiodevice, errno);
        return OMX_ErrorUndefined;
    } else {
        DBGT_EPILOG();
        return OMX_ErrorNone;
    }
}

/******************************************************************************
 * Function Name:MMIO_Camera::releaseDriver
 * -----------------------------------------
 * ParamName     |  Direction  | Description
 * -----------------------------------------
 *     -         |     -       |    -
 *
 ******************************************************************************/
OMX_ERRORTYPE MMIO_Camera::releaseDriver(void)
{
    DBGT_PROLOG();
    int err;

    err = close(MMIO_Camera_fd);
    MMIO_Camera_fd = 0;

    if (-1 == err) {
        DBGT_EPILOG("Failed to close driver handle. err = %d\n", errno);
        return OMX_ErrorUndefined;
    } else {
        DBGT_EPILOG();
        return OMX_ErrorNone;
    }
}

/******************************************************************************
 * Function Name:MMIO_Camera::initBoard
 * -----------------------------------------
 * ParamName     |  Direction  | Description
 * -----------------------------------------
 * sensorType    |     in      | Carries type of sensor Raw/YUV,
 *                              Primary/Secondary
 *
 ******************************************************************************/
OMX_ERRORTYPE MMIO_Camera::initBoard(T_CameraFunction cameraFunction)
{
    GET_AND_SET_TRACE_LEVEL(mmiocamera);

    DBGT_PROLOG();

    int err;
    OMX_ERRORTYPE omx_error = OMX_ErrorNone;

    omx_error = initDriver();

    if (OMX_ErrorNone != omx_error) {
        goto BAIL;
    }

    struct mmio_input_output_t data;

    memset(&data, 0, sizeof(mmio_input_output_t));

    DBGT_PDEBUG("Received sensor id %d\n", cameraFunction);

    if (cameraFunction == ePrimaryXSD) {
        data.mmio_arg.camera_slot = PRIMARY_CAMERA;
    } else {
        data.mmio_arg.camera_slot = SECONDARY_CAMERA;
    }

    err = ioctl(MMIO_Camera_fd, MMIO_CAM_INITBOARD, &data);

    if (err) {
        DBGT_CRITICAL("MMIO_CAM_INITBOARD returned error = %d\n", err);
        omx_error = OMX_ErrorUndefined;
        goto BAIL;
    }

BAIL:
    DBGT_EPILOG();
    return omx_error;
}

/******************************************************************************
 * Function Name:MMIO_Camera::desinitBoard
 * -----------------------------------------
 * ParamName     |  Direction  | Description
 * -----------------------------------------
 *     -         |     -       |    -
 *
 ******************************************************************************/
OMX_ERRORTYPE MMIO_Camera::desinitBoard()
{
    DBGT_PROLOG();
    int err;

    err = ioctl(MMIO_Camera_fd, MMIO_CAM_DESINITBOARD);

    if (err) {
        DBGT_EPILOG("err = %d\n", err);
        return OMX_ErrorUndefined;
    } else {
        DBGT_EPILOG();
        return OMX_ErrorNone;
    }
}

/******************************************************************************
 * Function Name:MMIO_Camera::initMMDSPTimer
 * -----------------------------------------
 * ParamName     |  Direction  | Description
 * -----------------------------------------
 *     -         |     -       |    -
 *
 ******************************************************************************/
void MMIO_Camera::initMMDSPTimer(void)
{
    DBGT_PROLOG();
    int err;
    err = ioctl(MMIO_Camera_fd, MMIO_CAM_INITMMDSPTIMER);
    if (err) {
        DBGT_EPILOG("err = %d\n", err);
    } else {
        DBGT_EPILOG();
    }
}

/******************************************************************************
 * Function Name:MMIO_Camera::powerSensor
 * -----------------------------------------
 * ParamName     |  Direction  | Description
 * -----------------------------------------
 * 1. power_on    |     In      | bool types carrier power status of power sensor
 *
 ******************************************************************************/

OMX_ERRORTYPE MMIO_Camera::powerSensor(OMX_BOOL power_on)
{
    DBGT_PROLOG("0x%X\n", power_on);

    int err;
    OMX_ERRORTYPE omx_error = OMX_ErrorNone;
    struct mmio_input_output_t data;

    memset(&data, 0, sizeof(mmio_input_output_t));

    if ((OMX_TRUE != power_on) && (OMX_FALSE != power_on)) {
        DBGT_CRITICAL("Bad value for power_on\n");
        omx_error = OMX_ErrorBadParameter;
        goto BAIL;
    }
#ifdef ENABLE_PMIC
    int prop_value;
    static const char *default_ste_mmio_uib = "-1";

    if ((OMX_TRUE != power_on) && (OMX_FALSE != power_on)) {
        DBGT_CRITICAL("Bad param. power_on = %d", power_on);
        return OMX_ErrorBadParameter;
    }

    if (power_on == OMX_FALSE) {
        // Unset PMIC Sequence not needed for u8500 uib
        if (i2c_passed == true) {
            unset_PMIC();
        }
    }

  #ifdef ANDROID
    #define MMIO_PROPERTY_NAME "ste.mmio.uib"
  #else
    #define MMIO_PROPERTY_NAME "ste_mmio_uib"
  #endif

    // Use the key ste.mmio.uib to store the board type
    // 0 indicates old uib with PMIC, 1 indicates u8500 uib
    else {
        GET_PROPERTY(MMIO_PROPERTY_NAME, value, default_ste_mmio_uib);
        prop_value = atoi(value);

        if (prop_value == 0) {
            set_PMIC(false);
        } else if (prop_value == 1) {
            i2c_passed = false;
        } else if (prop_value == -1) {         // If uib type not set
            // Set PMIC Sequence with Timeout
            set_PMIC(true);

            if (i2c_passed == true) {
                // old Uib with PMIC
                default_ste_mmio_uib = "0";
            } else {
                // u8500 uib
                default_ste_mmio_uib = "1";
            }
        }
    }
#endif
    data.mmio_arg.power_on = (mmio_bool_t)power_on;
    err = ioctl(MMIO_Camera_fd, MMIO_CAM_PWR_SENSOR, &data);

    if (err) {
        DBGT_CRITICAL("MMIO_CAM_PWR_SENSOR returned err = %d\n", err);
        omx_error = OMX_ErrorUndefined;
    }

BAIL:
    DBGT_EPILOG();
    return omx_error;
}

/******************************************************************************
 * Function Name:MMIO_Camera::setExtClk
 * -----------------------------------------
 * ParamName     |  Direction  | Description
 * -----------------------------------------
 * 1. clock_on    |     In      | bool types carrier power status of power sensor
 *
 ******************************************************************************/
OMX_ERRORTYPE MMIO_Camera::setExtClk(OMX_BOOL clock_on)
{
    DBGT_PROLOG("0x%X\n", clock_on);

    int err;
    OMX_ERRORTYPE omx_error = OMX_ErrorNone;

    struct mmio_input_output_t data;

    memset(&data, 0, sizeof(mmio_input_output_t));

    if ((OMX_TRUE != clock_on) && (OMX_FALSE != clock_on)) {
        DBGT_CRITICAL("Bad value for clock_on\n");
        omx_error = OMX_ErrorBadParameter;
        goto BAIL;
    }

    data.mmio_arg.power_on = (mmio_bool_t)clock_on;
    err = ioctl(MMIO_Camera_fd, MMIO_CAM_SET_EXT_CLK, &data);

    if (err) {
        DBGT_CRITICAL("MMIO_CAM_SET_EXT_CLK returned err = %d\n", err);
        omx_error = OMX_ErrorUndefined;
    }

BAIL:
    DBGT_EPILOG();
    return omx_error;
}

/******************************************************************************
 * Function Name:MMIO_Camera::setPrimaryCameraHWInterface
 * -----------------------------------------
 * ParamName     |  Direction  | Description
 * -----------------------------------------
 *    -          |     -       |     -
 *
 ******************************************************************************/
OMX_ERRORTYPE MMIO_Camera::setPrimaryCameraHWInterface(void)
{
    DBGT_PROLOG();
    int err;

    err = ioctl(MMIO_Camera_fd, MMIO_CAM_SET_PRI_HWIF);

    if (err) {
        DBGT_EPILOG("MMIO_CAM_SET_PRI_HWIF returned err = %d\n", err);
        return OMX_ErrorUndefined;
    } else {
        DBGT_EPILOG();
        return OMX_ErrorNone;
    }
}

OMX_ERRORTYPE MMIO_Camera::releasePrimaryCameraHWInterface(void)
{
    DBGT_PROLOG();
    OMX_ERRORTYPE omx_error = OMX_ErrorNone;
    omx_error = releaseDriver();
    DBGT_EPILOG();
    return omx_error;
}

/******************************************************************************
 * Function Name:MMIO_Camera::setSecondaryCameraHWInterface
 * -----------------------------------------
 * ParamName     |  Direction  | Description
 * -----------------------------------------
 *    -          |     -       |     -
 *
 ******************************************************************************/
OMX_ERRORTYPE MMIO_Camera::setSecondaryCameraHWInterface(void)
{
    DBGT_PROLOG();
    int err;

    err = ioctl(MMIO_Camera_fd, MMIO_CAM_SET_SEC_HWIF);
    if (err) {
        DBGT_EPILOG("MMIO_CAM_SET_SEC_HWIF returned err = %d\n", err);
        return OMX_ErrorUndefined;
    } else {
        DBGT_EPILOG();
        return OMX_ErrorNone;
    }
}

OMX_ERRORTYPE MMIO_Camera::releaseSecondaryCameraHWInterface(void)
{
    DBGT_PROLOG();
    OMX_ERRORTYPE omx_error = OMX_ErrorNone;
    omx_error = releaseDriver();
    DBGT_EPILOG();
    return omx_error;
}

OMX_U32 MMIO_Camera::mapStatisticsMemArea(OMX_U32 physicalAddr)
{
    DBGT_PROLOG("%u", (unsigned int)physicalAddr);

    int err;
    int retval = 0;
    OMX_ERRORTYPE omx_error = OMX_ErrorNone;
    struct mmio_input_output_t data;

    memset(&data, 0, sizeof(mmio_input_output_t));

    if (!MMIO_Camera_fd) {
        omx_error = initDriver();
        if (OMX_ErrorNone != omx_error) {
            goto BAIL;
        }
    }

    data.mmio_arg.addr_to_map = physicalAddr;
    err = ioctl(MMIO_Camera_fd, MMIO_CAM_MAP_STATS_AREA, &data);

    if (err) {
        DBGT_CRITICAL("MMIO_CAM_MAP_STATS_AREA returned err = %d\n", err);
        goto BAIL;
    } else {
        /* Driver returns back address in xp70 address space back in
         * addrToMap */
        retval = data.mmio_arg.addr_to_map;
    }

BAIL:
    DBGT_EPILOG("%u\n", (unsigned int)retval);
    return retval;
}

OMX_ERRORTYPE MMIO_Camera::loadXP70FW(const void *addr, OMX_U32 isp_fw_size)
{
    return OMX_ErrorNotImplemented;
}

OMX_ERRORTYPE MMIO_Camera::loadXP70FW(const void *phyAddrSdram,
                                      OMX_U32 sizeSdram,
                                      const void *phyAddrEsram,
                                      OMX_U32 sizeEsram,
                                      const void *addrSplit,
                                      OMX_U32 sizeSplit,
                                      const void *addrData,
                                      OMX_U32 sizeData)
{
    DBGT_PROLOG("phyAddrSdram = %p,\t"
                "sizeSdram = %u,\n"
                "phyAddrEsram = %p,\t"
                "sizeEsram = %u,\n"
                "addrSplit = %p,\t"
                "sizeSplit = %u,\n"
                "addrData = %p,\t"
                "sizeData = %u\n",
                phyAddrSdram,
                (unsigned int)sizeSdram,
                phyAddrEsram,
                (unsigned int)sizeEsram,
                addrSplit,
                (unsigned int)sizeSplit,
                addrData,
                (unsigned int)sizeData);

    int err = EINVAL;
    OMX_ERRORTYPE omx_error = OMX_ErrorNone;

    struct mmio_input_output_t data;

    memset(&data, 0, sizeof(mmio_input_output_t));

    if (sizeEsram && !phyAddrEsram) {
        omx_error = OMX_ErrorBadParameter;
        DBGT_CRITICAL("sizeEsRam = %u and phyAddrEsram is 0\n", (unsigned int)sizeEsram);
        goto BAIL;
    }

    if (sizeSplit && !addrSplit) {
        omx_error = OMX_ErrorBadParameter;
        DBGT_CRITICAL("sizeSplit = %u and addrSplit is 0\n", (unsigned int)sizeSplit);
        goto BAIL;
    }

    if (sizeData && !addrData) {
        omx_error = OMX_ErrorBadParameter;
        DBGT_CRITICAL("sizeData = %u and addrData is 0\n", (unsigned int)sizeData);
        goto BAIL;
    }

    if (sizeSdram && !phyAddrSdram) {
        omx_error = OMX_ErrorBadParameter;
        DBGT_CRITICAL("sizeSdram = %u and phyAddrSdram is 0\n", (unsigned int)sizeSdram);
        goto BAIL;
    }

    memset(&data, 0, sizeof(mmio_input_output_t));
    data.mmio_arg.xp70_fw.addr_sdram_ext = (void *)phyAddrSdram;
    data.mmio_arg.xp70_fw.size_sdram_ext = sizeSdram;
    data.mmio_arg.xp70_fw.addr_esram_ext = (void *)phyAddrEsram;
    data.mmio_arg.xp70_fw.size_esram_ext = sizeEsram;
    data.mmio_arg.xp70_fw.addr_split = (void *)addrSplit;
    data.mmio_arg.xp70_fw.size_split = sizeSplit;
    data.mmio_arg.xp70_fw.addr_data = (void *)addrData;
    data.mmio_arg.xp70_fw.size_data = sizeData;

    err = ioctl(MMIO_Camera_fd, MMIO_CAM_LOAD_XP70_FW, &data);

    if (err) {
        DBGT_CRITICAL("MMIO_CAM_LOAD_XP70_FW returned error = %d\n", err);
        omx_error = OMX_ErrorUndefined;
    }

BAIL:
    if (err) {
        DBGT_EPILOG("err = %d\n", err);
    } else {
        DBGT_EPILOG();
    }

    return omx_error;
}

/******************************************************************************
 * Function Name:MMIO_Camera:: loadXP70FW
 * -----------------------------------------
 * ParamName     |  Direction  | Description
 * -----------------------------------------
 *
 ******************************************************************************/
OMX_ERRORTYPE MMIO_Camera::loadXP70FW(const void *phyAddrExt,
                                     OMX_U32 sizeExt,
                                     const void *addrSplit,
                                     OMX_U32 sizeSplit,
                                     const void *addrData,
                                     OMX_U32 sizeData)
{
    DBGT_PROLOG("phyAddrExt = %p,\t"
                "sizeExt = %u,\n"
                "addrSplit = %p,\t"
                "sizeSplit = %u,\n"
                "addrData = %p,\t"
                "sizeData = %u\n",
                phyAddrExt,
                (unsigned int)sizeExt,
                addrSplit,
                (unsigned int)sizeSplit,
                addrData,
                (unsigned int)sizeData);

    int err = EINVAL;
    OMX_ERRORTYPE omx_error = OMX_ErrorNone;

    struct mmio_input_output_t data;

    memset(&data, 0, sizeof(mmio_input_output_t));

    if (sizeExt && !phyAddrExt) {
        omx_error = OMX_ErrorBadParameter;
        DBGT_CRITICAL("sizeExt = %u and phyAddrExt is 0\n", (unsigned int)sizeExt);
        goto BAIL;
    }
    if (sizeSplit && !addrSplit) {
        omx_error = OMX_ErrorBadParameter;
        DBGT_CRITICAL("sizeSplit = %u and addrSplit is 0\n", (unsigned int)sizeSplit);
        goto BAIL;
    }
    if (sizeData && !addrData) {
        omx_error = OMX_ErrorBadParameter;
        DBGT_CRITICAL("sizeData = %u and addrData is 0\n", (unsigned int)sizeData);
        goto BAIL;
    }

    memset(&data, 0, sizeof(mmio_input_output_t));
    data.mmio_arg.xp70_fw.addr_esram_ext = (void *)phyAddrExt;
    data.mmio_arg.xp70_fw.size_esram_ext = sizeExt;
    data.mmio_arg.xp70_fw.addr_split = (void *)addrSplit;
    data.mmio_arg.xp70_fw.size_split = sizeSplit;
    data.mmio_arg.xp70_fw.addr_data = (void *)addrData;
    data.mmio_arg.xp70_fw.size_data = sizeData;

    err = ioctl(MMIO_Camera_fd, MMIO_CAM_LOAD_XP70_FW, &data);

    if (err) {
        omx_error = OMX_ErrorUndefined;
    }

BAIL:
    if (err) {
        DBGT_EPILOG("err = %d\n", err);
    } else {
        DBGT_EPILOG();
    }

    return omx_error;
}

OMX_ERRORTYPE MMIO_Camera::ispVectorWrite(unsigned long t1_dest,
                                         const long int *logical_addr,
                                         int count)
{
    DBGT_PROLOG("t1_dest = %li, logical_addr = %p, count = %d\n", (long unsigned int)t1_dest, logical_addr, count);

    struct mmio_input_output_t ioctl_arg;
    struct isp_write_t *isp_write_p;
    int err = EINVAL;
    OMX_ERRORTYPE omx_error = OMX_ErrorNone;

    if (logical_addr == NULL) {
        omx_error = OMX_ErrorBadParameter;
        DBGT_CRITICAL("NULL logical_addr supplied\n");
        goto BAIL;
    }

    if (count < 0) {
        omx_error = OMX_ErrorBadParameter;
        DBGT_CRITICAL("Bad param. count = %d\n", count);
        goto BAIL;
    }

    isp_write_p = &ioctl_arg.mmio_arg.isp_write;
    isp_write_p->count = count;
    isp_write_p->t1_dest = t1_dest;
    isp_write_p->data = (unsigned long *)logical_addr;

    err = ioctl(MMIO_Camera_fd, MMIO_CAM_ISP_WRITE, &ioctl_arg);

    if (err) {
        omx_error = OMX_ErrorUndefined;
    }

BAIL:
    if (err) {
        DBGT_EPILOG("err = %d\n", err);
    } else {
        DBGT_EPILOG();
    }

    return omx_error;
}

OMX_ERRORTYPE activate_i2c2(int activate)
{
    DBGT_PROLOG("activate = %d\n", activate);
    int ioctl_arg;
    int err = EINVAL;
    OMX_ERRORTYPE omx_error = OMX_ErrorNone;

    switch (activate) {
        case OMX_TRUE:
            ioctl_arg = MMIO_ACTIVATE_I2C_HOST;
            break;

        case OMX_FALSE:
            ioctl_arg = MMIO_ACTIVATE_IPI2C2;
            break;

        default:
            ioctl_arg = MMIO_DEACTIVATE_I2C;
            break;
    }

    err = ioctl(MMIO_Camera_fd, MMIO_ACTIVATE_I2C2, &ioctl_arg);

    if (err) {
        omx_error = OMX_ErrorUndefined;
    }
BAIL:
    if (err) {
        DBGT_EPILOG("err = %d\n", err);
    } else {
        DBGT_EPILOG();
    }

    return omx_error;
}

OMX_ERRORTYPE enable_xshutdown_from_host(int enable, int is_active_high)
{
    DBGT_PROLOG("enable = %d, is_active_high = %d\n", enable, is_active_high);

    int ioctl_arg = 0;
    int err = EINVAL;
    OMX_ERRORTYPE omx_error = OMX_ErrorNone;

    if (enable == OMX_TRUE) {
        if (is_active_high == OMX_TRUE) {
            ioctl_arg = MMIO_XSHUTDOWN_ENABLE |
                MMIO_XSHUTDOWN_ACTIVE_HIGH;
        } else {
            ioctl_arg = MMIO_XSHUTDOWN_ENABLE;
        }
    }

    err = ioctl(MMIO_Camera_fd, MMIO_ENABLE_XSHUTDOWN_FROM_HOST,
        &ioctl_arg);

    if (err) {
        omx_error = OMX_ErrorUndefined;
    }
BAIL:
    if (err) {
        DBGT_EPILOG("err = %d\n", err);
    } else {
        DBGT_EPILOG();
    }

    return omx_error;
}

OMX_ERRORTYPE MMIO_Camera::getCameraFunction(T_CameraFunction function,
                                             unsigned int *ip_gpio)
{
    DBGT_PROLOG("function = %d\n", function);
    int err = EINVAL;
    OMX_ERRORTYPE omx_error = OMX_ErrorNone;

    struct mmio_input_output_t data;

    memset(&data, 0, sizeof(mmio_input_output_t));

    switch (function) {
        case ePrimaryXSD:
            data.mmio_arg.xshutdown_info.camera_function = PRIMARY_CAMERA;
            break;

        case eSecondaryXSD:
            data.mmio_arg.xshutdown_info.camera_function = SECONDARY_CAMERA;
            break;

        default:
            omx_error = OMX_ErrorBadParameter;
            DBGT_CRITICAL("Invalid camera function supplied = %d\n", function);
            goto BAIL;
    }

    err = ioctl(MMIO_Camera_fd, MMIO_CAM_GET_IP_GPIO, &data);

    if (err) {
        omx_error = OMX_ErrorUndefined;
    } else {
        *ip_gpio = data.mmio_arg.xshutdown_info.ip_gpio;
        DBGT_PDEBUG("*ip_gpio = %d", *ip_gpio);
    }
BAIL:
    if (err) {
        DBGT_EPILOG("err = %d\n", err);
    } else {
        DBGT_EPILOG();
    }

    return omx_error;
}


/*Description :  MMIO_Camera::setTraceBuffer(OMX_U32 logAddrTrace, OMX_U32 sizeTrace)

1> Creates a Process which will be dumping the FW traces from the buffer pointed by address in "logAddrTrace"

*/

OMX_ERRORTYPE MMIO_Camera::setTraceBuffer(OMX_U32 logAddrTrace, OMX_U32 sizeTrace)
{
    DBGT_PROLOG("logAddrTrace = %u sizeTrace = %u\n",
                (unsigned int)logAddrTrace,
                (unsigned int)sizeTrace);

    OMX_ERRORTYPE omx_error = OMX_ErrorNone;
    pid_t  pid;

    /*forking a new process to dump the trace*/
    pid = fork();
    if (pid == -1){
        /* Error:
        * When fork() returns -1, an error happened
        * (for example, number of processes reached the limit).
        */
        DBGT_ERROR("fork failed \n");

    }
    switch(errno)
    {
        case EAGAIN:
            {
                DBGT_ERROR("EAGAIN \n");
                omx_error = OMX_ErrorInsufficientResources;
                return omx_error;
            }
        case ENOMEM:
            {
                DBGT_ERROR("ENOMEM \n");
                omx_error = OMX_ErrorInsufficientResources;
                return omx_error;
            }
    }
    if (pid == 0){
        /* Child process:
        * When fork() returns 0, we are in
        * the child process.
        */
        executeXP70TraceProcess((unsigned int*)logAddrTrace);
    }
    else{
        t_param.ch_pid = pid;

    }
    DBGT_EPILOG();
    return omx_error;
}

/*Description :  MMIO_Camera::resetTraceBuffer()
1> Parents send the signal to child process  and parents process waits for the child process to exit.

*/

OMX_ERRORTYPE MMIO_Camera::resetTraceBuffer(void)
{

    DBGT_PROLOG("resetTraceBuffer");

    OMX_ERRORTYPE ret = OMX_ErrorNone;
    pthread_t handle = 0;
    int wait_status;
    int status;
    errno = 0;

    if(t_param.ch_pid){
        kill(t_param.ch_pid,SIGUSR1);
        //Waiting child process to return.
        status = waitpid(t_param.ch_pid, &wait_status,0);
        if (status == -1){
            DBGT_ERROR("waitpid returned error");
        }
        switch(errno){
            case ECHILD:
            {
                DBGT_ERROR("waitpid set ECHILD \n");
                ret = OMX_ErrorInvalidState;
                break;
            }
            case EINTR:
            {
                DBGT_ERROR("waitpid set EINTR \n");
                ret = OMX_ErrorUndefined;
                break;
            }
            case EINVAL:
            {
                DBGT_ERROR("waitpid set EINVAL \n");
                ret = OMX_ErrorBadParameter;
                break;
            }
        }
        t_param.ch_pid =0;
    }
    DBGT_EPILOG();
    return ret;
}

OMX_U32 MMIO_Camera::isNVMdataPresent()
{
    DBGT_PROLOG();

    unsigned int nvmDataSize = 0;
    int fd;
    int nread;

    char buf[SIZE_OF_BUF] = { 0 };

    fd = open(PATH_NVM_DATA_SIZE, O_RDONLY);

    if (0 <= fd) {
        DBGT_CRITICAL("Could not open sysfs entry for nvm data size, error = %d\n", errno);
        goto BAIL;
    }

    nread = read(fd, buf, SIZE_OF_BUF);

    if (-1 == nread) {
        DBGT_CRITICAL("read failed, error = %d\n", errno);
        goto BAIL;
    }
    sscanf(buf, "%u", &nvmDataSize);

BAIL:
    DBGT_EPILOG("nvmDataSize = %u\n", (unsigned int)nvmDataSize);
    if (fd) {
        close(fd);
    }
    return (OMX_U32)nvmDataSize;
}

OMX_ERRORTYPE MMIO_Camera::readNVMdata(void *aNVMBuffer)
{
    DBGT_PROLOG("aNVMBuffer = %p\n", aNVMBuffer);
    int nvmDataSize;
    int bytesRead;
    int err = EINVAL;
    OMX_ERRORTYPE omx_error = OMX_ErrorNone;

    nvmDataSize = isNVMdataPresent();

    if (nvmDataSize <= 0) {
        omx_error = OMX_ErrorIncorrectStateOperation;
        DBGT_CRITICAL("No NVM data has been stored yet\n");
        goto BAIL;
    }

    if (aNVMBuffer == NULL) {
        omx_error = OMX_ErrorBadParameter;
        DBGT_CRITICAL("aNVMBuffer is NULL\n");
        goto BAIL;
    }

    bytesRead = read(MMIO_Camera_fd, aNVMBuffer, nvmDataSize);

    if (bytesRead < nvmDataSize) {
        omx_error = OMX_ErrorUndefined;
        DBGT_CRITICAL("aNVMBuffer is NULL\n");
        goto BAIL;
    }

BAIL:
    if (err) {
        DBGT_EPILOG("err = %d\n", err);
    } else {
        DBGT_EPILOG();
    }

    return omx_error;
}

OMX_ERRORTYPE MMIO_Camera::writeNVMdata(void *aNVMBuffer, OMX_U32 aDataSize)
{
    DBGT_PROLOG("aNVMBuffer = %p,\t"
                "aDataSize = %u\n",
                aNVMBuffer,
                (unsigned int)aDataSize);

    int bytesRead;

    OMX_ERRORTYPE omx_error = OMX_ErrorNone;

    if (aDataSize == 0) {
        omx_error = OMX_ErrorBadParameter;
        DBGT_CRITICAL("aDataSize is zero\n");
        goto BAIL;
    }

    if (aNVMBuffer == NULL) {
        omx_error = OMX_ErrorBadParameter;
        DBGT_CRITICAL("aNVMBuffer is NULL\n");
        goto BAIL;
    }

    bytesRead = write(MMIO_Camera_fd, aNVMBuffer, aDataSize);

    if (-1 == bytesRead) {
        omx_error = OMX_ErrorUndefined;
        DBGT_CRITICAL("writing NVM data failed, err = %d\n", errno);
        goto BAIL;
    }
BAIL:
    DBGT_EPILOG();
    return omx_error;
}

/*
Description : MMIO_Camera::executeXP70TraceProcess
1>  creates a empty signal set waitset via sigemptyset()  which will store the SIGNAL list via sigaddset() ; currently onlySIGALRM and SIGUSR1 is put in the list.
      unblocks itself to receive the signals for SIGALRM and SIGUSR1
2> waits for semaphore to be posted .
*/

void MMIO_Camera::executeXP70TraceProcess( unsigned int* Trace_buff_Log_Addr )
{

    DBGT_PROLOG("Trace_buff_Log_Addr is %p ",Trace_buff_Log_Addr);
    int result = 0;
    OMX_ERRORTYPE omx_error = OMX_ErrorNone;
    sigset_t waitset;
    int value;
    int   exit_loop=0;
    mmio_trace * trace_ptr;


    /*Semaphore is initialized with value 0 <last arg > and its sharing scope is all threads
    in the current process which is indicated by the value zero as 2nd argument */

    result =  sem_init(&t_param.my_semaphore, 0, 0);
    if(result){
        DBGT_ASSERT("sem_init()" );
        goto BAIL_OUT;
    }

    /* making the sigset  empty */

    result = sigemptyset( &waitset );
    if(result){
        DBGT_ASSERT("sigemptyset()" );
        goto BAIL_OUT;
    }

    /* adding SIGALRM to sigset list */

    result = sigaddset( &waitset, SIGALRM );

    if(result){
        DBGT_ASSERT("sigaddset()" );
        goto BAIL_OUT;
    }

    /* adding SIGUSR1 to sigset list */
    result = sigaddset( &waitset, SIGUSR1 );

    if(result){
        DBGT_ASSERT("sigaddset() USR1" );
        goto BAIL_OUT;
    }
    /* making signal mask to receive the SIGNAL listed in  waitset. */


    result =  sigprocmask( SIG_UNBLOCK, &waitset, NULL );
    if(result){
        DBGT_ASSERT("sigprocmask()" );
        goto BAIL_OUT;
    }


    /* setting the timer to send SIGALRM evry  CAM_TRACE_TIMEOUT_MSEC * 1000; msecs */

    t_param.tout_val.it_interval.tv_sec = 0;
    t_param.tout_val.it_interval.tv_usec = CAM_TRACE_TIMEOUT_MSEC * 1000;
    t_param.tout_val.it_value.tv_sec = 0;/* set timer for "INTERVAL (10) seconds */
    t_param.tout_val.it_value.tv_usec =CAM_TRACE_TIMEOUT_MSEC * 1000 ;

    //registering the signal handler for the SIGALRM and SIGUSR1 signals



    signal(SIGALRM,XP70TraceSignalHandler);/* reset signal */
    signal(SIGUSR1,XP70TraceSignalHandler);/* reset signal */


    /* trigerring  the timer to send SIGALRM evry  CAM_TRACE_TIMEOUT_MSEC * 1000; msecs */

    result = setitimer(ITIMER_REAL, &t_param.tout_val,0);
    if(result){
        DBGT_ASSERT("setitimer" );
        goto BAIL_OUT;
    }


    trace_ptr = (mmio_trace*)(Trace_buff_Log_Addr );

    /* initing the memory as NULL as this memeory may not have been initilizaed by FW */

    if(trace_ptr)
    trace_ptr->block_id = 0;

    while(!exit_loop){

        /* Blocking in the semaphore till we get the SIGALRM or SIGUSR 1 from signal handler registereed earlier. sem_post will will be called from there*/


        sem_wait(&t_param.my_semaphore);

        switch ( t_param.wait_over)
        {
            case 2:
            {
                /*SIGUSR1 has come , we can now exit the loop as there will not be any traces left in the buffer of ISP FW*/
                exit_loop=1;
                break;

            }
            case 1:
            {
                /*SIGALRM has come , we can now dump the traces*/
                dumpTrace(Trace_buff_Log_Addr);
                 t_param.wait_over=0;
                break;
            }
        }

    }// Now we need to reset the tracestructure meta data as camera is now stopped.

BAIL_OUT:

        /*Stopping the SIGALRM signal by stoping the timer*/
        DBGT_PDEBUG(" t_param.tout_val.it_interval.tv_sec = 0;");

        t_param.tout_val.it_interval.tv_sec = 0;
        t_param.tout_val.it_interval.tv_usec = 0;
        t_param.tout_val.it_value.tv_sec = 0;/* set timer for "INTERVAL (10) seconds */
        t_param.tout_val.it_value.tv_usec =0 ;

        result = setitimer(ITIMER_REAL, &t_param.tout_val,0);
        if(result){
            DBGT_ASSERT(" resetting setitimer" );
            goto BAIL_OUT_2;
        }

BAIL_OUT_2:

        /*resetting variables*/
        dumpTrace(NULL);
         t_param.wait_over=0;


        /*destroying the semaphore*/
        sem_destroy(&t_param.my_semaphore);

    DBGT_PDEBUG("before _exit(0);");

    /* exiting the process note " underscore" we are not releasing the system resources acquired by the parent process here as it should be done by the parent process */
    DBGT_EPILOG();
    _exit(0);

}


/*Description:
MMIO_Camera::XP70TraceSignalHandler

1> it is called once the process  receives the SIGALRM signal sent by settitimer().
 In the signal handler we dump the traces.
2> reset the signal handler to receive the SIGALRM signal or SIGUSR1 for   next time.

*/
void MMIO_Camera::XP70TraceSignalHandler(int signum)
{
    DBGT_PROLOG("signum is %d \n",signum);

    int result =0;

    switch(signum)
    {
        case  SIGALRM :
        {
             t_param.wait_over =1;
            result = sem_post(&t_param.my_semaphore);
            if(result){
                DBGT_ASSERT(" sem_post error case SIGALRM " );
            }
            signal(SIGALRM,XP70TraceSignalHandler);/* reset signal */
            break;
        }
        case  SIGUSR1:
        {
             t_param.wait_over =2;
            result = sem_post(&t_param.my_semaphore);
            if(result){
                DBGT_ASSERT(" sem_post error case SIGUSR1 " );
            }
            signal(SIGUSR1,XP70TraceSignalHandler);/* reset signal */
            break;
        }
    }
    DBGT_EPILOG();
}


/*Description:
MMIO_Camera::dumpTrace
___________________________________________
|no of blocks|block size|msg id | overwrite_count |
|       4bytes |4bytes __|4bytes |4bytes               |
|__________|________|______|______________|____________________________________________________
|                  |                                                                                                                                                 |
|Loglevel:     | Message string                                                                                                                           |
|       4bytes  |  124bytes                                                                                                                                  |
|__________|___________________________________________________________________________________|
|                  |                                                                                                                                                 |
|Loglevel:     | Message string                                                                                                                           |
|       4bytes  |  124bytes                                                                                                                                  |
|__________|___________________________________________________________________________________ |
|                                                                                                                                                                     |
|                                                                                                                                                                     |
|______________________________________________________________________________________________|

1> takes the logical address of the trace buffer set during FW initialization as input parameter.
2> it resets the meta data previous block id and previous over write count once camera is stopped and it is called with NULL.
3> it calculates the nuber of log mesages to be printed and prints them
*/

int MMIO_Camera::dumpTrace(void* trace_log_addr)
{

    DBGT_PROLOG("trace_log_addr is %p",trace_log_addr);
    mmio_trace * trace_ptr;
    int block_id =0;
    int overwrite_count =0;
    int count =0;
    int i =0;
    unsigned int msg_len;
    static  MMIO_Camera::FW_trace_buffer_status m_FW_trace_status = {0,0};

//We are resetting the prev_block_id and prev_overwrite_count when thread is about to return.
    if(trace_log_addr == NULL){
           m_FW_trace_status.prev_block_id =0;
           m_FW_trace_status.prev_overwrite_count = 0;
           return 0;
     }
     trace_ptr = (mmio_trace*)trace_log_addr ;


    block_id = trace_ptr->block_id;
    overwrite_count = trace_ptr->overwrite_count;

    if (block_id >= m_FW_trace_status.prev_block_id){
        count = block_id -m_FW_trace_status.prev_block_id;
    }
    else{
        count = block_id +CAM_TRACE_NB_BLOCK -m_FW_trace_status.prev_block_id;
    }

    for (i = m_FW_trace_status.prev_block_id; count; count--) {

        if (i < 0 || i >= CAM_TRACE_NB_BLOCK || count > CAM_TRACE_NB_BLOCK){
            break;
        }


        if ( trace_ptr->block[i].msg_id !=CAM_TRACE_DEFAULT_MSG_ID){
             msg_len = strnlen((trace_ptr->block[i].data),CAM_TRACE_DATA_BLOCK_SIZE);

            /* zero terminate full length message */
            if (msg_len > 0){
                if (msg_len >= CAM_TRACE_DATA_BLOCK_SIZE)
                    trace_ptr->block[i].data[CAM_TRACE_DATA_BLOCK_SIZE -1] = '\0';

                DBGT_WARNING("%d %s\n",trace_ptr->block[i].msg_id,trace_ptr->block[i].data);

            }
        }
        i = (i + 1) % CAM_TRACE_NB_BLOCK;
    }

    m_FW_trace_status.prev_overwrite_count = overwrite_count;
    m_FW_trace_status.prev_block_id = block_id;

    DBGT_EPILOG();
    return 0;
}

OMX_ERRORTYPE MMIO_Camera::downloadFirmware()
{
    OMX_ERRORTYPE ret = OMX_ErrorNone;
    int ioret;

    DBGT_PROLOG();

    ioret = ioctl(MMIO_Camera_fd, MMIO_CAM_EXT_ISP_WRITE);
    if (ioret) {
        DBGT_CRITICAL("MMIO_CAM_EXT_ISP_WRITE returned error = %d", ioret);
        ret = OMX_ErrorUndefined;
    }

    DBGT_EPILOG();
    return ret;
}


OMX_ERRORTYPE MMIO_Camera::deinterlaceFrame( char* data, void* src_hwmem_buf, void* dst_hwmem_buf, int width, int height )
{
    int err = EINVAL;
    OMX_ERRORTYPE error = OMX_ErrorNone;
    struct mmio_input_output_t ioctl_arg;

    DBGT_PROLOG();

    // check validity of the resolution
    if (width > 4096 || width <=0)
        goto check_err;

    // check validity of the hwmem buf name
    if (src_hwmem_buf == NULL)
        goto check_err;

    if (dst_hwmem_buf == NULL)
        goto check_err;

    if (height > 4096 || height <=0)
        goto check_err;

    // check if data pointer is null
    if (data == NULL)
        goto check_err;

    ioctl_arg.mmio_arg.deint_info.width = width;
    ioctl_arg.mmio_arg.deint_info.height = height;
    ioctl_arg.mmio_arg.deint_info.src_log_addr = (void *)data;
    ioctl_arg.mmio_arg.deint_info.src_hwmem_buf = (void *)src_hwmem_buf;
    ioctl_arg.mmio_arg.deint_info.dst_hwmem_buf = (void *)dst_hwmem_buf;

    err = ioctl(MMIO_Camera_fd, MMIO_CAM_DEINTERLACE_FRAME, &ioctl_arg);
    if (err) {
        DBGT_CRITICAL("MMIO_CAM_DEINT_FRAME returned error = %d", err);
        error = OMX_ErrorUndefined;
    }

check_err:
    DBGT_EPILOG();
    return error;
}
