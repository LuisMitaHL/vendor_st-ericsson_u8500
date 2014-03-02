/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "ext_camera_ctrllib.h"

#undef   DBGT_LAYER
#define  DBGT_LAYER 2
#undef  DBGT_PREFIX
#define DBGT_PREFIX "DRIV"
#include "debug_trace.h"

#define TEST_ADAPTER           (I2C_ADAP_2)
#define CAM_MSG_BLOCK_SIZE 4

const char* DefinitionStrings[]={
    "info_name",
    "info_vendor",
    "info_major_ver",
    "info_minor_ver",
    "cam_i2c_address",
    "cam_init_result",
    "cam_ping_result",
    "cam_ss_frame_skip",
    "cam_vf_frame_skip",
    "cam_cc_frame_skip",
    "cam_watchdog_timeout",
    "cam_interface",
    "cam_ccp2",
    "cam_csi_bitrate",
    "cam_csi_datatype0",
    "cam_csi_datatype1",
    "cam_csi_datatype2",
    "cam_csi_datatype0_pix_wd",
    "cam_csi_datatype1_pix_wd",
    "cam_csi_datatype2_pix_wd",
    "cam_focus_result",
    "sup_sizes",
    "seq_init",
    "seq_ping",
    "seq_vf_start",
    "seq_vf_stop",
    "seq_ss_start",
    "seq_ss_stop",
    "seq_ss_take",
    "seq_ss_verify",
    "seq_focus_status",
    "seq_deinit",
    "seq_vf_return",
    "cam_pipe_number",
    "cam_horizontal_angle",
    "cam_vertical_angle",
    "cam_focal_length",
    "exif_maker_note",
    "exif_x_resolution",
    "exif_y_resolution",
    "exif_max_num_pixels_x",
    "exif_analog_gain",
    "exif_digital_gain",
    "cam_exposure_lock",
    "cam_exposure_unlock",
    "cam_awb_lock",
    "cam_awb_unlock",
    "auto_flash_on_threshold",
    "auto_flash_off_threshold",
    "seq_ss_flash_on",
    "seq_ss_flash_off",
    "cam_touch_max_x_y_values",
    "cam_zoom_max_preview_width",
    "cam_zoom_max_preview_height",
    "cam_zoom_max_picture_width",
    "cam_zoom_max_picture_height",
    "cam_flash_synchro_supported",
};

static t_uint8* GBigBuffer=NULL;

/*************************************************************************
 *
 * Exported functions
 *
 *************************************************************************/
Camera_Ctrllib::Camera_Ctrllib(
        bool is_primary_camera)
{
    status_Camera_CtrlLib_Primary = is_primary_camera;
}

/*************************************************************************
 * Camera_Ctrllib::Camera_CtrlLib_ModuleInit
 **************************************************************************/
void Camera_Ctrllib::Camera_CtrlLib_ModuleInit(void)
{
    // Intentionally left blank for future updates
}

/*************************************************************************
 * Camera_Ctrllib::Camera_CtrlLib_LoadDriver
 **************************************************************************/
int Camera_Ctrllib::Camera_CtrlLib_LoadDriver(
        OMX_HANDLETYPE omxhandle)
{
    int         error  = 0;
    t_nmf_error nmfErr = 0;

    if ((iDriverData_p = new CamCtrl_DriverData_t) != NULL)
    {
        FILE* ConfigFile = NULL;
        size_t readBytesCount = 0;
        char value[PROPERTY_VALUE_MAX];

        if(status_Camera_CtrlLib_Primary == TRUE){
            property_get("ste.cam.ext.cfg.path.primary", value, "/system/usr/share/camera/config_file/aptina_mt9p111.dat");
        }else{
            property_get("ste.cam.ext.cfg.path.secondary", value, "/system/usr/share/camera/config_file/aptina_mt9v113.dat");
        }

        ConfigFile = fopen((char*)value, "rb");
        DBGT_PTRACE("Open camera config file %s",(char*)value);

        if (ConfigFile==NULL) {
            DBGT_PTRACE("Error opening Config file, check config file path property");
            error = -1;
        } else {
            fseek (ConfigFile, 0, SEEK_END);
            fileSize=ftell (ConfigFile);
            if (fileSize < 100) {
              DBGT_PTRACE("file size =%d is incorrect",(int)fileSize);
              error = -1;
            } else {
               rewind(ConfigFile);

               // check if the buffer has been freed
               if(GBigBuffer!=NULL) free(GBigBuffer);

               // allocate the buffer for the parse
               GBigBuffer = (t_uint8*)malloc(fileSize);
               if(GBigBuffer==NULL)
                   DBGT_ASSERT("Bad allocation for GBigBuffer");

               readBytesCount = fread ((void*)GBigBuffer,1,(size_t)fileSize,ConfigFile);
               if (readBytesCount != fileSize) {
                   DBGT_PTRACE("Read failed, expected %ld bytes, actually got %d bytes\n", fileSize, readBytesCount);
                   error = -1;
               }
            }
            fclose (ConfigFile);
        }
        LoadDriver_Continued();
    }
    DBGT_PTRACE("Camera config file is read");
    return error;
}

/*************************************************************************
 * Camera_Ctrllib::LoadDriver_Continued
 **************************************************************************/
void Camera_Ctrllib::LoadDriver_Continued()
{
    int Success=1,error=0;
    DBGT_PTRACE("File reading complete");
    Success = ParseFile(iDriverData_p);
    if (Success)
    {
        DBGT_PTRACE("File parsing complete");
        Success = GetI2CAddress(iDriverData_p);
        if(!Success)
        {
            DBGT_PTRACE("GetI2CAddress failed");


            error = 1;
            goto ERROR_HANDLER;
        }

#ifndef MMIO_ENABLE_NEW_IF
        error = activate_i2c2(OMX_TRUE);
        if(error)
        {
            DBGT_PTRACE("activate_i2c2 failed");
            goto ERROR_HANDLER;
        }
#endif //MMIO_ENABLE_NEW_IF

        init_param.adapter = TEST_ADAPTER;
        init_param.slave_addr = iDriverData_p->I2C_WriteAddress;
        error = i2c_init(&init_param,&i2c_handle);
        if(error)
        {
            i2c_deinit(i2c_handle);
            goto ERROR_HANDLER;
        }
    }

ERROR_HANDLER:
    if (error)
    {
        delete iDriverData_p;
        iDriverData_p = NULL;
        DBGT_PTRACE(  "Failed loaddriver");
    }

    return;
}


/*************************************************************************
 * Camera_Ctrllib::Camera_CtrlLib_UnLoadDriver
 **************************************************************************/
void Camera_Ctrllib::Camera_CtrlLib_UnloadDriver(
        const CamCtrl_DriverData_t* DriverHandle,
        OMX_HANDLETYPE omxhandle)
{
    CamCtrl_DriverData_t* DriverData_p = (CamCtrl_DriverData_t*)DriverHandle;

    if(DriverData_p == NULL)
        DBGT_PTRACE(  "Error: DriverData_p is NULL");

    FreeParsedItems(DriverData_p);
    delete DriverData_p;
    DriverData_p=NULL;

    i2c_deinit(i2c_handle);

    // free the big buffer
    if(GBigBuffer!=NULL) free(GBigBuffer);
}


/*************************************************************************
 * Camera_Ctrllib::Camera_CtrlLib_IsAlive
 **************************************************************************/
int Camera_Ctrllib::Camera_CtrlLib_IsAlive(
        const CamCtrl_DriverData_t* DriverHandle)
{
    CamCtrl_DriverData_t* DriverData_p = (CamCtrl_DriverData_t*)DriverHandle;
    t_uint8* Sequence_p;
    int Length;
    t_uint8 RegNo;
    int NotUsed;
    int ok;

    if(DriverData_p == NULL)
        DBGT_PTRACE(  "Error: DriverData_p is NULL");

    ok = FALSE;
    if (FindDefinition(DriverData_p, CAMCTRL_SEQ_PING, &Sequence_p, &Length))
    {
        ok = ExecuteSequence(DriverData_p, Sequence_p, Length, NULL, &NotUsed);
        if (ok && (FindDefinition(DriverData_p, CAMCTRL_CAM_PING_RESULT, &Sequence_p, &Length)))
        {
            ok = (int)(Length < 30); //No more than 10 registers allowed
            RegNo = 0;
            while ((Length > 1) && ok)
            {
                ok = (int)(DriverData_p->Registers[RegNo++] == (t_uint8)ConvertNumber(Sequence_p, 2, 16));

                //Read past delimiter
                Sequence_p += 3;
                Length -= 3;
            }
        }
    }

    return((ok));
}

void Camera_Ctrllib::Camera_CtrlLib_FirmwareDownload()
{
    OMX_ERRORTYPE download_err;

    download_err = MMIO_Camera::downloadFirmware();
    if(download_err){
        DBGT_CRITICAL("ISP firmware download failed");
    }
}


#if SUPPORT_OTP_CAMERA
#define OTP_FLAG_DONE        0x4F54
#define OTP_FLAG_DONE1       0x5001
#define OTP_FLAG_DONE2       0x5002
#define OTP_FLAG_DONE3       0x5003

t_uint16 SOC5140_OTP_Start_Row_lists[] =
{
   0x0009,
   0x0045,
   0x0089,
};

t_uint16 SOC5140_RegAddr[]=
{
    0x3640,
    0x3642,
    0x3644,
    0x3646,
    0x3648,
    0x364A,
    0x364C,
    0x364E,
    0x3650,
    0x3652,
    0x3654,
    0x3656,
    0x3658,
    0x365A,
    0x365C,
    0x365E,
    0x3660,
    0x3662,
    0x3664,
    0x3666,
    0x3680,
    0x3682,
    0x3684,
    0x3686,
    0x3688,
    0x368A,
    0x368C,
    0x368E,
    0x3690,
    0x3692,
    0x3694,
    0x3696,
    0x3698,
    0x369A,
    0x369C,
    0x369E,
    0x36A0,
    0x36A2,
    0x36A4,
    0x36A6,
    0x36C0,
    0x36C2,
    0x36C4,
    0x36C6,
    0x36C8,
    0x36CA,
    0x36CC,
    0x36CE,
    0x36D0,
    0x36D2,
    0x36D4,
    0x36D6,
    0x36D8,
    0x36DA,
    0x36DC,
    0x36DE,
    0x36E0,
    0x36E2,
    0x36E4,
    0x36E6,
    0x3700,
    0x3702,
    0x3704,
    0x3706,
    0x3708,
    0x370A,
    0x370C,
    0x370E,
    0x3710,
    0x3712,
    0x3714,
    0x3716,
    0x3718,
    0x371A,
    0x371C,
    0x371E,
    0x3720,
    0x3722,
    0x3724,
    0x3726,
    0x3740,
    0x3742,
    0x3744,
    0x3746,
    0x3748,
    0x374A,
    0x374C,
    0x374E,
    0x3750,
    0x3752,
    0x3754,
    0x3756,
    0x3758,
    0x375A,
    0x375C,
    0x375E,
    0x3760,
    0x3762,
    0x3764,
    0x3766,
    0x3782,
    0x3784,
};


t_uint16 SOC5140_RegValue[102];
t_uint16 hasotp=3;// 0 =otp not exist, 1= otp exist,  3= init val

void Camera_Ctrllib::InitSensorForOTPMemRead5140()
{
    int i = 0;
    t_uint8 readValue[2] = {0};
    t_uint16 OTPCheckValue;
    t_uint8 regdata[4];


    if(status_Camera_CtrlLib_Primary==false)
    {
        LOGD("is slave camera, so no OTP function in InitSensorForOTPMemRead5140");
        return;
    }
    
    //sensor hardreset
    MMIO_Camera::setExtClk(OMX_TRUE);
    
    //LOGI("Start initialze the sensor for otp memory read new...");


    regdata[0]=0x00;
    regdata[1]=0x10;
    regdata[2]=0x03;
    regdata[3]=0x2d;


    I2C_Write(0x78,4,regdata,CAMCTRL_I2C_RETRIES, CAMCTRL_I2C_REPEAT_RETRIES);
    
    regdata[0]=0x00;
    regdata[1]=0x12;
    regdata[2]=0x00;
    regdata[3]=0xb0;
    I2C_Write(0x78,4,regdata,CAMCTRL_I2C_RETRIES, CAMCTRL_I2C_REPEAT_RETRIES);

    regdata[0]=0x00;
    regdata[1]=0x14;
    regdata[2]=0x20;
    regdata[3]=0x25;
    I2C_Write(0x78,4,regdata,CAMCTRL_I2C_RETRIES, CAMCTRL_I2C_REPEAT_RETRIES);


    regdata[0]=0x00;
    regdata[1]=0x1e;
    regdata[2]=0x07;
    regdata[3]=0x70;
    I2C_Write(0x78,4,regdata,CAMCTRL_I2C_RETRIES, CAMCTRL_I2C_REPEAT_RETRIES);

    regdata[0]=0x00;
    regdata[1]=0x22;
    regdata[2]=0x00;
    regdata[3]=0x30;
    I2C_Write(0x78,4,regdata,CAMCTRL_I2C_RETRIES, CAMCTRL_I2C_REPEAT_RETRIES);

    regdata[0]=0x00;
    regdata[1]=0x2a;
    regdata[2]=0x7f;
    regdata[3]=0x8a;
    I2C_Write(0x78,4,regdata,CAMCTRL_I2C_RETRIES, CAMCTRL_I2C_REPEAT_RETRIES);

    regdata[0]=0x00;
    regdata[1]=0x2c;
    regdata[2]=0x00;
    regdata[3]=0x00;
    I2C_Write(0x78,4,regdata,CAMCTRL_I2C_RETRIES, CAMCTRL_I2C_REPEAT_RETRIES);

    regdata[0]=0x00;
    regdata[1]=0x2e;
    regdata[2]=0x00;
    regdata[3]=0x00;
    I2C_Write(0x78,4,regdata,CAMCTRL_I2C_RETRIES, CAMCTRL_I2C_REPEAT_RETRIES);

    regdata[0]=0x00;
    regdata[1]=0x18;
    regdata[2]=0x40;
    regdata[3]=0x08;
    I2C_Write(0x78,4,regdata,CAMCTRL_I2C_RETRIES, CAMCTRL_I2C_REPEAT_RETRIES);    
    for(i = 0; i<10; i++)//POLL R0x0014[15] until its value = 1
    {
        DoDelay(50);
        regdata[0]=0x00;
        regdata[1]=0x14;
        regdata[2]=0x00;
        regdata[3]=0x00;
        I2C_Read(0x78, 2, regdata, 0xff, 2, CAMCTRL_I2C_RETRIES, CAMCTRL_I2C_REPEAT_RETRIES, readValue);
        OTPCheckValue=readValue[0]<<8|readValue[1];
        //LOGV("read 0x0014:i=%d,OTPCheckValue=0x%4x",i,OTPCheckValue);
        if((OTPCheckValue |0x7FFF) == 0xFFFF)//finish
        {
            break;
        }
    }
    OTPCheckValue = 0;
    for(i = 0; i<10; i++) // POLL R0x0018[B14] until its value = 0
    {
        DoDelay(50);
        regdata[0]=0x00;
        regdata[1]=0x18;
        regdata[2]=0x00;
        regdata[3]=0x00;
        I2C_Read(0x78, 2, regdata, 0xff, 2, CAMCTRL_I2C_RETRIES, CAMCTRL_I2C_REPEAT_RETRIES, readValue);
        OTPCheckValue=readValue[0]<<8|readValue[1];
        //LOGV("read 0x0018:i=%d,OTPCheckValue=0x%4x",i,OTPCheckValue);
        if((OTPCheckValue | 0xBFFF) != 0xFFFF)//finish
        {
            break;
        }
    }    
    regdata[0]=0x38;
    regdata[1]=0x12;
    regdata[2]=0x21;
    regdata[3]=0x24;
    I2C_Write(0x78,4,regdata,CAMCTRL_I2C_RETRIES, CAMCTRL_I2C_REPEAT_RETRIES);
    
    regdata[0]=0x38;
    regdata[1]=0x14;
    regdata[2]=0x0f;
    regdata[3]=0x1f;
    I2C_Write(0x78,4,regdata,CAMCTRL_I2C_RETRIES, CAMCTRL_I2C_REPEAT_RETRIES);
    
    regdata[0]=0x38;
    regdata[1]=0x16;
    regdata[2]=0x0f;
    regdata[3]=0x1f;
    I2C_Write(0x78,4,regdata,CAMCTRL_I2C_RETRIES, CAMCTRL_I2C_REPEAT_RETRIES);
    
    regdata[0]=0x38;
    regdata[1]=0x18;
    regdata[2]=0x44;
    regdata[3]=0x07;
    I2C_Write(0x78,4,regdata,CAMCTRL_I2C_RETRIES, CAMCTRL_I2C_REPEAT_RETRIES);
    
    regdata[0]=0x09;
    regdata[1]=0x8e;
    regdata[2]=0x60;
    regdata[3]=0x2a;
    I2C_Write(0x78,4,regdata,CAMCTRL_I2C_RETRIES, CAMCTRL_I2C_REPEAT_RETRIES);
    
    regdata[0]=0xe0;
    regdata[1]=0x2a;
    regdata[2]=0x00;
    regdata[3]=0x01;
    I2C_Write(0x78,4,regdata,CAMCTRL_I2C_RETRIES, CAMCTRL_I2C_REPEAT_RETRIES);
    
    OTPCheckValue = 0;
    for(i = 0; i<10; i++) // POLL VAR = 24, R0x0023[6] until its value = 1
    {
        DoDelay(50);
        regdata[0]=0xE0;////////////////////////////////////////////////////0xe0;
        regdata[1]=0x23;
        regdata[2]=0x00;
        regdata[3]=0x00;
        I2C_Read(0x78, 2, regdata, 0xff, 2, CAMCTRL_I2C_RETRIES, CAMCTRL_I2C_REPEAT_RETRIES, readValue);
        OTPCheckValue=readValue[0]<<8|readValue[1];
        //LOGV("read VAR = 24, R0x0023:i=%d,OTPCheckValue=0x%4x,OTPCheckValue | 0xFFBF=0x%4x",i,OTPCheckValue,OTPCheckValue | 0xFFBF);
        if((OTPCheckValue | 0xFFBF) == 0xFFFF)//finish
        {
            LOGD("break:OTPCheckValue | 0xFFBF=0x%4x",OTPCheckValue | 0xFFBF);
            break;
        }
    }   
    //LOGI("initialze the sensor for otp memory read new finish");
}


void Camera_Ctrllib::OnReadOTPShading5140()
{
    InitSensorForOTPMemRead5140();

    if(status_Camera_CtrlLib_Primary==false)
    {
        LOGD("is slave camera, so no OTP function in OnReadOTPShading5140");
        return;
    }

    //get starting address for read otp value   
    t_uint16 OTPFlagValueLower = 0;
    t_uint16 OTPFlagValueHigher = 0;      
    bool bOTPWritten[3]={FALSE,FALSE,FALSE};
    t_uint16 OTPStartAddr;
    t_uint8 regdata[4];
    t_uint8 readValue[2] = {0};
    int i = 0;

    //LOGI("get address for programming lsc  ...");   
    OTPFlagValueLower = 0;
    OTPFlagValueHigher = 0;    
    
    regdata[0]=0x38;
    regdata[1]=0x02;
    regdata[2]=0x00;
    regdata[3]=0x00;
    I2C_Write(0x78,4,regdata,CAMCTRL_I2C_RETRIES, CAMCTRL_I2C_REPEAT_RETRIES);

    regdata[0]=0x38;
    regdata[1]=0x04;
    regdata[2]=0x00;
    regdata[3]=0x01;
    I2C_Write(0x78,4,regdata,CAMCTRL_I2C_RETRIES, CAMCTRL_I2C_REPEAT_RETRIES);
    
    for(i = 0; i<8; i++)//no 02 flag
    {	
        regdata[0]=0x38;
        regdata[1]=0x02;
        regdata[2]=0x00;
        regdata[3]=0x09;/////////////////////////////////////0x09;
        I2C_Write(0x78,4,regdata,CAMCTRL_I2C_RETRIES, CAMCTRL_I2C_REPEAT_RETRIES);
    
        DoDelay(10);
        regdata[0]=0x38;
        regdata[1]=0x0c;
        regdata[2]=0x00;
        regdata[3]=0x00;
        I2C_Read(0x78, 2, regdata, 0xff, 2, CAMCTRL_I2C_RETRIES, CAMCTRL_I2C_REPEAT_RETRIES, readValue);
        OTPFlagValueLower=readValue[0]<<8|readValue[1];
        
        regdata[0]=0x38;
        regdata[1]=0x0e;
        regdata[2]=0x00;
        regdata[3]=0x00;
        I2C_Read(0x78, 2, regdata, 0xff, 2, CAMCTRL_I2C_RETRIES, CAMCTRL_I2C_REPEAT_RETRIES, readValue);
        OTPFlagValueHigher=readValue[0]<<8|readValue[1];

        //LOGV("OTPFlagValueLower = 0x%04x,OTPFlagValueHigher = 0x%04x",OTPFlagValueLower,OTPFlagValueHigher);
 
        if (OTP_FLAG_DONE == OTPFlagValueLower)
        {          
            if (OTP_FLAG_DONE1 == OTPFlagValueHigher)
            {
                bOTPWritten[0] = TRUE;
                bOTPWritten[1] = FALSE;
                bOTPWritten[2] = FALSE;               
            }
            else if (OTP_FLAG_DONE2 == OTPFlagValueHigher)
            {
                bOTPWritten[1] = TRUE;
                bOTPWritten[0] = FALSE;
                bOTPWritten[2] = FALSE;
            }
            else if (OTP_FLAG_DONE3 == OTPFlagValueHigher)
            {
                bOTPWritten[2] = TRUE;
                bOTPWritten[0] = FALSE;
                bOTPWritten[1] = FALSE;
            }
        }           
    }
    regdata[0]=0x38;
    regdata[1]=0x02;
    regdata[2]=0x00;
    regdata[3]=0x00;
    I2C_Write(0x78,4,regdata,CAMCTRL_I2C_RETRIES, CAMCTRL_I2C_REPEAT_RETRIES);
    
    if (bOTPWritten[2])
    {
        OTPStartAddr = SOC5140_OTP_Start_Row_lists[2]+2;
        hasotp=1;
    }
    else if(bOTPWritten[1])
    {
        OTPStartAddr = SOC5140_OTP_Start_Row_lists[1]+2;
        hasotp=1;
    }
    else if(bOTPWritten[0])
    {
        OTPStartAddr = SOC5140_OTP_Start_Row_lists[0]+2;
        hasotp=1;
    }
    else
    {    
        LOGE("no data in otp");   
        hasotp=0;
        goto OTPERR;
    }

    //read otp value and write to sensor
    //LOGI("Start read otp value...");   
    t_uint16 readValue16;
    
    regdata[0]=0x38;
    regdata[1]=0x02;
    regdata[2]=0x00;
    regdata[3]=0x00;
    I2C_Write(0x78,4,regdata,CAMCTRL_I2C_RETRIES, CAMCTRL_I2C_REPEAT_RETRIES);
    
    regdata[0]=0x38;
    regdata[1]=0x04;
    regdata[2]=OTPStartAddr>>8;
    regdata[3]=OTPStartAddr&0x00ff;
    I2C_Write(0x78,4,regdata,CAMCTRL_I2C_RETRIES, CAMCTRL_I2C_REPEAT_RETRIES);
    
    for(i=0; i<102; i++)
    {	
        regdata[0]=0x38;
        regdata[1]=0x02;
        regdata[2]=0x00;
        regdata[3]=0x09;
        I2C_Write(0x78,4,regdata,CAMCTRL_I2C_RETRIES, CAMCTRL_I2C_REPEAT_RETRIES);
    
        DoDelay(10);
        regdata[0]=0x38;
        regdata[1]=0x0c;
        regdata[2]=0x00;
        regdata[3]=0x00;
        I2C_Read(0x78, 2, regdata, 0xff, 2, CAMCTRL_I2C_RETRIES, CAMCTRL_I2C_REPEAT_RETRIES, readValue);
        readValue16=readValue[0]<<8|readValue[1];
        
        SOC5140_RegValue[i] = readValue16;
        //LOGV("SOC5140_RegAddr[%d]=0x%04x,SOC5140_RegValue[%d]=0x%04x",i,SOC5140_RegAddr[i],i,SOC5140_RegValue[i]);
        i++;
        regdata[0]=0x38;
        regdata[1]=0x0e;
        regdata[2]=0x00;
        regdata[3]=0x00;
        I2C_Read(0x78, 2, regdata, 0xff, 2, CAMCTRL_I2C_RETRIES, CAMCTRL_I2C_REPEAT_RETRIES, readValue);
        readValue16=readValue[0]<<8|readValue[1];
        
        SOC5140_RegValue[i] = readValue16;
        //LOGV("SOC5140_RegAddr[%d]=0x%04x,SOC5140_RegValue[%d]=0x%04x",i,SOC5140_RegAddr[i],i,SOC5140_RegValue[i]);
    }
    regdata[0]=0x38;
    regdata[1]=0x02;
    regdata[2]=0x00;
    regdata[3]=0x00;
    I2C_Write(0x78,4,regdata,CAMCTRL_I2C_RETRIES, CAMCTRL_I2C_REPEAT_RETRIES);
    
    //LOGI("read otp value finish");

OTPERR:

    //sensor hardreset
    MMIO_Camera::setExtClk(OMX_TRUE);
}
#endif

/*************************************************************************
 * Camera_Ctrllib::Camera_CtrlLib_Initialize
 **************************************************************************/
int Camera_Ctrllib::Camera_CtrlLib_Initialize(
        const CamCtrl_DriverData_t* Camera_h,
        int*         DelayTime_p)
{
    CamCtrl_DriverData_t* DriverData_p = (CamCtrl_DriverData_t*)Camera_h;
    t_uint8* Sequence_p;
    int Length;
    t_uint8 RegNo;
    int ok;

    if(DriverData_p == NULL)
        DBGT_PTRACE(  "Error: DriverData_p is NULL");

#if SUPPORT_OTP_CAMERA
    if(status_Camera_CtrlLib_Primary==true)
    {
         FILE* fcamr;
         FILE* fcamw;
         fcamr = fopen("/data/tuning/LSC4OTPCAM.bin", "rb");
         if (!fcamr) 
         {
             LOGE("Camera_CtrlLib_Initialize, read otp file fail because not exist!");
             fcamw = fopen("/data/tuning/LSC4OTPCAM.bin", "wb");
             if (!fcamw) 
             {
                  LOGE("Camera_CtrlLib_Initialize, creat file for otp data write failed!");
             } 
             else 
             {
		  //LOGE("Camera_CtrlLib_Initialize, create file for otp data write success");
                  OnReadOTPShading5140();
                  fwrite(&hasotp,2,1,fcamw);
                  fwrite(&SOC5140_RegValue,2,102,fcamw);
                  fclose(fcamw);
             }
         } 
         else 
         {
	     //LOGI("Camera_CtrlLib_Initialize, read saved otp file success");
             fread((void *)&hasotp,2,1,fcamr);
             //LOGI("For OTP test,read  hasotp=%d",hasotp);
             fread((void *)&SOC5140_RegValue,2,102,fcamr);
             fclose(fcamr);
         }
    }
#endif

    ok = FALSE;
    if (FindDefinition(DriverData_p, CAMCTRL_SEQ_INIT, &Sequence_p, &Length))
    {
        ok = ExecuteSequence(DriverData_p, Sequence_p, Length, NULL, DelayTime_p);
        if (ok && (FindDefinition(DriverData_p, CAMCTRL_CAM_INIT_RESULT, &Sequence_p, &Length)))
        {
            ok = (int)(Length < 30); //No more than 10 registers allowed
            RegNo = 0;
            while ((Length > 1) && ok)
            {
                ok = (int)(DriverData_p->Registers[RegNo++] == (t_uint8)ConvertNumber(Sequence_p, 2, 16));

                //Read past delimiter
                Sequence_p += 3;
                Length -= 3;
            }
        }
    }

    ClearUsedProperties(DriverData_p);  //Read properties are no longer valid

#if SUPPORT_OTP_CAMERA
    LOGI("For OTP test,status_Camera_CtrlLib_Primary=%d, hasotp=%d",status_Camera_CtrlLib_Primary, hasotp);
    if(status_Camera_CtrlLib_Primary && (1 == hasotp))
    {
	    //write lens shading to sensor 
	    LOGI("write otp value to camera");
	    int i=0;
	    t_uint8 regdata[4];
	    regdata[0]=0x32;
	    regdata[1]=0x10;
	    regdata[2]=0x49;
	    regdata[3]=0xb0;
	    I2C_Write(0x78,4,regdata,CAMCTRL_I2C_RETRIES, CAMCTRL_I2C_REPEAT_RETRIES);
	    for(i=0;i<102;i++)
	    {
		    regdata[0]=SOC5140_RegAddr[i]>>8;
		    regdata[1]=SOC5140_RegAddr[i]&0x00ff;
		    regdata[2]=SOC5140_RegValue[i]>>8;
		    regdata[3]=SOC5140_RegValue[i]&0x00ff;
		    I2C_Write(0x78,4,regdata,CAMCTRL_I2C_RETRIES, CAMCTRL_I2C_REPEAT_RETRIES);
	    }
	    regdata[0]=0x32;
	    regdata[1]=0x10;
	    regdata[2]=0x49;
	    regdata[3]=0xb8;
	    I2C_Write(0x78,4,regdata,CAMCTRL_I2C_RETRIES, CAMCTRL_I2C_REPEAT_RETRIES);
    }
#endif


    return((ok));
}


/*************************************************************************
 * Camera_Ctrllib::Camera_CtrlLib_Deinitialize
 **************************************************************************/
int Camera_Ctrllib::Camera_CtrlLib_Deinitialize(
        const CamCtrl_DriverData_t* Camera_h,
        int*         DelayTime_p)
{
    CamCtrl_DriverData_t* DriverData_p = (CamCtrl_DriverData_t*)Camera_h;
    t_uint8* Sequence_p;
    int Length;
    int ok;

    if(DriverData_p == NULL)
        DBGT_PTRACE(  "Error: DriverData_p is NULL");

    ok = FALSE;
    if (FindDefinition(DriverData_p, CAMCTRL_SEQ_DEINIT, &Sequence_p, &Length))
    {
        ok = ExecuteSequence(DriverData_p, Sequence_p, Length, NULL, DelayTime_p);
    }

    return((ok));
}

/*************************************************************************
 * Camera_Ctrllib::Camera_CtrlLib_Get_cam_interface_config
 **************************************************************************/
int Camera_Ctrllib::Camera_CtrlLib_Get_cam_interface_config(
        const CamCtrl_DriverData_t* Camera_h,
        t_uint8* selCam,
        t_uint8* selOutputIntf,
        t_uint8 *numofLanes,
        CamCtrl_CsiIntf_Config_t *csiConfig)
{
    CamCtrl_DriverData_t* DriverData_p = (CamCtrl_DriverData_t*)Camera_h;
    t_uint8* Sequence_p;
    int Length;
    int ok;
    int intf_value = 0;
    int dataRate = 0;
    CamCtrl_DphySpec_t  dphyVer   = CAMCTRL_DPHYSPEC_V0_81B;
    CamCtrl_EotBypass_t eotBypass = CAMCTRL_EOTBYPASS_DIS;
    t_uint8 dataType0         = 0;
    t_uint8 dataType1         = 0;
    t_uint8 dataType2         = 0;
    t_uint8 dataType0PixWidth = 0;
    t_uint8 dataType1PixWidth = 0;
    t_uint8 dataType2PixWidth = 0;

    if(DriverData_p == NULL || csiConfig == NULL)
        DBGT_PTRACE("Error: DriverData_p is NULL");

    ok = FALSE;
    if (FindDefinition(DriverData_p, CAMCTRL_CAM_INTERFACE, &Sequence_p, &Length))
    {
        intf_value = (int)ConvertNumber(Sequence_p, Length, 16);
        DBGT_PTRACE("cam interface config value: %x",intf_value);

        intf_value = intf_value & 0x00FF;

        /*Check for selected camera*/
        if (intf_value & 0x0001)
        {
            /*Secondry camera is selected*/
            *selCam = CAMCTRL_CAM_SEL_SECONDARY;
        }
        else
        {
            /*Primary camera is selected*/
            *selCam = CAMCTRL_CAM_SEL_PRIMARY;
        }

        /*Check for data interface*/
        if (CAMCTRL_CAM_DATA_INTF_CSI2 == ((intf_value & 0x0002) >> 1 ))
        {
            /*Secondry camera is selected*/
            *selOutputIntf = CAMCTRL_CAM_DATA_INTF_CSI2;

            /*Check for DHPY version*/
            if(intf_value & 0x10) {
                /*DPHY version supported is v.90*/
                dphyVer = CAMCTRL_DPHYSPEC_V0_90;
            } else {
                /*DPHY version supported is v.81b*/
                dphyVer = CAMCTRL_DPHYSPEC_V0_81B;
            }

            if (intf_value & 0x20) {
                /*eotBypass is enabled*/
                eotBypass = CAMCTRL_EOTBYPASS_EN;
            }

        }
        else
        {
            /*Primary camera is selected*/
            *selOutputIntf= CAMCTRL_CAM_DATA_INTF_CCP2;
        }

        /*If CSI2 is selected, check number of lanes requested*/
        *numofLanes = 0;
        if(CAMCTRL_CAM_DATA_INTF_CSI2 == *selOutputIntf)
        {
            if (intf_value & 0x000C) {
                *numofLanes = (intf_value & 0x000C) >> 2;
            } else {
                //cam_interface[Bit 2:3]: 0 = 4DL
                *numofLanes = 4;
            }
            DBGT_PTRACE("cam interface number of data lane: %d",*numofLanes);

            /*get the other CSI related configuration*/
            /*Get Datarate*/
            if (FindDefinition(DriverData_p, CAMCTRL_CAM_CSI_BITRATE, &Sequence_p, &Length))
            {
                dataRate= (int)ConvertNumber(Sequence_p, Length, 16);
                dataRate = dataRate & 0xFFFF;
                DBGT_PTRACE("cam interface bitrate value: %x",dataRate);
            }
            else
            {
                DBGT_PTRACE("Fatal Datarate not specified");
                ok = FALSE;
                goto EXIT;
            }

            /* Get data type 0 */
            if (FindDefinition(DriverData_p, CAMCTRL_CAM_CSI_DATATYPE0, &Sequence_p, &Length))
            {
                dataType0 = (int)ConvertNumber(Sequence_p, Length, 16);
                DBGT_PTRACE("cam interface datatype 0 value: %x",dataType0);
            }
            else
            {
                DBGT_PTRACE("Fatal datatype 0 not specified");
                ok = FALSE;
                goto EXIT;
            }

            /* Get data type 1 */
            if (FindDefinition(DriverData_p, CAMCTRL_CAM_CSI_DATATYPE1, &Sequence_p, &Length))
            {
                dataType1 = (int)ConvertNumber(Sequence_p, Length, 16);
                DBGT_PTRACE("cam interface datatype 1 value: %x",dataType1);
            }
            else
            {
                DBGT_PTRACE("Fatal datatype 1 not specified");
                ok = FALSE;
                goto EXIT;
            }

            /* Get data type 2 */
            if (FindDefinition(DriverData_p, CAMCTRL_CAM_CSI_DATATYPE2, &Sequence_p, &Length))
            {
                dataType2 = (int)ConvertNumber(Sequence_p, Length, 16);
                DBGT_PTRACE("cam interface datatype 2 value: %x",dataType2);
            }
            else
            {
                DBGT_PTRACE("Fatal datatype 2 not specified");
                ok = FALSE;
                goto EXIT;
            }

            /* Get pixel width for data type 0*/
            if (FindDefinition(DriverData_p, CAMCTRL_CAM_CSI_DATATYPE0_PIXWIDTH, &Sequence_p, &Length))
            {
                dataType0PixWidth= (int)ConvertNumber(Sequence_p, Length, 16);
                DBGT_PTRACE("cam interface datatype 0 pixwidth value: %x",dataType0PixWidth);
            }
            else
            {
                DBGT_PTRACE("Fatal datatype 0 pixwidth not specified");
                ok = FALSE;
                goto EXIT;
            }

            /* Get pixel width for data type 1*/
            if (FindDefinition(DriverData_p, CAMCTRL_CAM_CSI_DATATYPE1_PIXWIDTH, &Sequence_p, &Length))
            {
                dataType1PixWidth= (int)ConvertNumber(Sequence_p, Length, 16);
                DBGT_PTRACE("cam interface datatype 1 pixwidth value: %x",dataType1PixWidth);
            }
            else
            {
                DBGT_PTRACE("Fatal datatype 1 pixwidth not specified");
                ok = FALSE;
                goto EXIT;
            }

            /* Get pixel width for data type 2*/
            if (FindDefinition(DriverData_p, CAMCTRL_CAM_CSI_DATATYPE2_PIXWIDTH, &Sequence_p, &Length))
            {
                dataType2PixWidth= (int)ConvertNumber(Sequence_p, Length, 16);
                DBGT_PTRACE("cam interface datatype 2 pixwidth value: %x",dataType2PixWidth);
            }
            else
            {
                DBGT_PTRACE("Fatal datatype 2 pixwidth not specified");
                ok = FALSE;
                goto EXIT;
            }

            /*Populate the return struct*/
            csiConfig->Mipi_Specs_90_81b      = dphyVer;
            csiConfig->Eot_Bypass             = eotBypass;
            csiConfig->CSI_bitrate_mbps       = dataRate;
            csiConfig->CSI_DataType0          = dataType0;
            csiConfig->CSI_DataType0_PixWidth = dataType0PixWidth;
            csiConfig->CSI_DataType1          = dataType1;
            csiConfig->CSI_DataType1_PixWidth = dataType1PixWidth;
            csiConfig->CSI_DataType2          = dataType2;
            csiConfig->CSI_DataType2_PixWidth = dataType2PixWidth;
        }
        ok = TRUE;
    }

EXIT:
    return((ok));
}


/*************************************************************************
 * Camera_Ctrllib::Camera_CtrlLib_Get_ccp2_config
 **************************************************************************/
int Camera_Ctrllib::Camera_CtrlLib_Get_ccp2_config(
        const CamCtrl_DriverData_t* Camera_h,
        t_uint16* Ccp_Static_BPP,
        t_uint16* Ccp_Static_Data_Strobe)
{
    CamCtrl_DriverData_t* DriverData_p = (CamCtrl_DriverData_t*)Camera_h;
    t_uint8* Sequence_p;
    int Length;
    int ok;
    int ccp2_value = 0;

    if(DriverData_p == NULL)
        DBGT_PTRACE("Error: DriverData_p is NULL");

    ok = FALSE;
    if (FindDefinition(DriverData_p, CAMCTRL_CAM_CCP2, &Sequence_p, &Length))
    {
        ccp2_value = (int)ConvertNumber(Sequence_p, Length, 16);
        DBGT_PTRACE("CCP2 config value: %x",ccp2_value);
        ccp2_value = ccp2_value & 0x00FF;

        if (ccp2_value & 0x0007) {
            //Use 7 bits
            *Ccp_Static_BPP = 7;
        } else if (ccp2_value & 0x0008) {
            //Use 7 bits
            *Ccp_Static_BPP = 8;
        } else if (ccp2_value & 0x000A) {
            //Use 7 bits
            *Ccp_Static_BPP = 10;
        } else if (ccp2_value & 0x000C) {
            //Use 7 bits
            *Ccp_Static_BPP = 12;
        }

        if (ccp2_value & 0x0010) {
            //data/clock
            *Ccp_Static_Data_Strobe = 1;
        } else {
            //data/strobe
            *Ccp_Static_Data_Strobe = 0;
        }

        ok = TRUE;
    }

    return((ok));
}


/*************************************************************************
 * Camera_Ctrllib::Camera_CtrlLib_GetDriverInfo
 **************************************************************************/
void Camera_Ctrllib::Camera_CtrlLib_GetDriverInfo(
        const CamCtrl_DriverData_t* Camera_h,
        const int          NameSize,
        const int          VendorSize,
        t_uint8*          Name_p,
        t_uint8*          Vendor_p,
        t_uint8*          MajorVersion_p,
        t_uint8*          MinorVersion_p,
        t_uint8*          CameraOrientation_p)
{
    CamCtrl_DriverData_t* DriverData_p = (CamCtrl_DriverData_t*)Camera_h;
    t_uint8* Sequence_p;
    int Length;

    if(DriverData_p == NULL)
        DBGT_PTRACE(  "Error: DriverData_p is NULL");

    if (NameSize > 0)
    {
        *Name_p = 0;
    }
    if (VendorSize > 0)
    {
        *Vendor_p = 0;
    }
    *MajorVersion_p = 0;
    *MinorVersion_p = 0;
    *CameraOrientation_p = 0;

    if (FindDefinition(DriverData_p, CAMCTRL_INFO_NAME, &Sequence_p, &Length))
    {
        if ((int)NameSize > Length)
        {
            memcpy(Name_p, Sequence_p, (int)Length);
            *(Name_p + Length) = 0;
        }
    }
    if (FindDefinition(DriverData_p, CAMCTRL_INFO_VENDOR, &Sequence_p, &Length))
    {
        if ((int)VendorSize > Length)
        {
            memcpy(Vendor_p, Sequence_p, (int)Length);
            *(Vendor_p + Length) = 0;
        }
    }

    if (FindDefinition(DriverData_p, CAMCTRL_INFO_MAJOR_VER, &Sequence_p, &Length))
    {
        *MajorVersion_p = (t_uint8)ConvertNumber(Sequence_p, Length, 16);
    }

    if (FindDefinition(DriverData_p, CAMCTRL_INFO_MINOR_VER, &Sequence_p, &Length))
    {
        *MinorVersion_p = (t_uint8)ConvertNumber(Sequence_p, Length, 16);
    }
}


/*************************************************************************
 * Camera_Ctrllib::Camera_CtrlLib_GetSizeConfig
 **************************************************************************/
int Camera_Ctrllib::Camera_CtrlLib_GetSizeConfig(
        const CamCtrl_DriverData_t* DriverHandle,
        const t_uint8 Id, HAL_Gr_Camera_SizeConfig_t* SizeConfig_p)
{
    CamCtrl_DriverData_t* DriverData_p = (CamCtrl_DriverData_t*)DriverHandle;
    t_uint8* Sequence_p;
    int Length;
    int Found;
    int Offset;

    if(DriverData_p == NULL)
        DBGT_PTRACE(  "Error: DriverData_p is NULL");

    if (SizeConfig_p == NULL)
    {
        //no destination supplied for the requested information
        return FALSE;
    }

    Found = FALSE;
    if (FindDefinition(DriverData_p, CAMCTRL_SUP_SIZES, &Sequence_p, &Length))
    {
        while (Length > 0 && !Found)
        {
            if (GetSizeConfig(Sequence_p, Length, SizeConfig_p, &Offset))
            {
                Length -= Offset;
                Sequence_p += Offset;
                Found = SizeConfig_p->ID == Id;
            }
            else
            {
                break;
            }
        }
    }

    return((Found));
}


/*************************************************************************
 * Camera_Ctrllib::Camera_CtrlLib_SetDigitalZoom
 **************************************************************************/
int Camera_Ctrllib::Camera_CtrlLib_SetDigitalZoom(
        const CamCtrl_DriverData_t* Camera_h,
        const int Value,
        int* DelayTime_p)

{
    CamCtrl_DriverData_t* DriverData_p = (CamCtrl_DriverData_t*)Camera_h;
    const ParsedItem_t* Item_p = NULL;
    unsigned char SequenceName[30];
    int ok;

    if(DriverData_p == NULL)
        DBGT_PTRACE(  "Error: DriverData_p is NULL");

    ok = FALSE;

    sprintf((char*)SequenceName, "seq_vf_zoom_%x", Value);

    // Find exact match
    Item_p = FindNextMatchingItem(DriverData_p, NULL, SequenceName);

    if (Item_p)
    {
        // TBD: Set up variables for camera driver to use in sequence
        DBGT_PTRACE("Execute sequence: %s", Item_p->Name_p);

        ok = ExecuteSequence(DriverData_p, Item_p->Value.String_p, Item_p->Value.Length, NULL, DelayTime_p);
    }
    else
    {
        DBGT_PTRACE("No match for sequence %s", SequenceName);
    }

    return((ok));
}


/*************************************************************************
 * Camera_Ctrllib::Camera_CtrlLib_SetProperty
 **************************************************************************/
int Camera_Ctrllib::Camera_CtrlLib_SetProperty(
        const CamCtrl_DriverData_t* DriverHandle,
        const HAL_Gr_Camera_Property_t Prop,
        const int Value,
        int* const DelayTime_p)
{
    CamCtrl_DriverData_t* DriverData_p = (CamCtrl_DriverData_t*)DriverHandle;
    t_uint8* Sequence_p;
    int Length;
    t_uint8 SequenceName[32];
    int ok = FALSE;
    int OldValue;
    int OldValid;

    DBGT_PTRACE("SetProperty. Prop: %d, Value: %x", (int)Prop, Value);

    if(DriverData_p == NULL)
        DBGT_PTRACE(  "Error: DriverData_p is NULL");

    //Check if supported and supplied value is valid
    if (Prop < CAMCTRL_MAX_NUMBER_PROPERTIES) {
        if (DriverData_p->PropertyProperties[Prop].Supported) {
            // Make sure the value is a valid value
            if (DriverData_p->PropertyProperties[Prop].Values_p == NULL) {
                // No specific values are specified in the setup file (only the
                // min and max values are specified),
                if ((DriverData_p->PropertyProperties[Prop].Min <= Value) &&
                        (DriverData_p->PropertyProperties[Prop].Max >= Value)) {
                    ok = TRUE;
                }
            } else {
                // Specific values are specified in the setup file for this
                // particular property. Make sure the value is specified.
                int* Value_p = DriverData_p->PropertyProperties[Prop].Values_p;
                int i = DriverData_p->PropertyProperties[Prop].NumValuesStoredInValuesList;

                while (i--) {
                    if (*Value_p == Value) {
                        ok = TRUE;
                        break;
                    }
                    Value_p++;
                }
            }
        }

        if (ok) {
            //First check if there is a special sequence for the wanted value,
            //i.e. "seq_set_prop_x_v" => set property x to value v
            sprintf((char*)SequenceName, "seq_set_prop_%x_%x", Prop, Value);

            if (!FindItem(DriverData_p, SequenceName, &Sequence_p, &Length)) {
                //Nope, no special sequence found, so
                //look for plain "seq_set_prop_x" instead.
                sprintf((char*)SequenceName, "seq_set_prop_%x", Prop);
                if (!FindItem(DriverData_p, SequenceName, &Sequence_p, &Length)) {
                    ok = FALSE;
                }
            }
            DBGT_PTRACE("FindItem(\"%s\") -> %d", SequenceName, ok);

            if (ok) {
                //Some sequence was found
                //Set the last value to the one we want to set
                //to make the $p command work as expected together with the $u command
                //Store the old value to restore if something goes wrong
                OldValue = DriverData_p->PropertyProperties[Prop].Last;
                OldValid = DriverData_p->PropertyProperties[Prop].LastIsValid;
                DriverData_p->PropertyProperties[Prop].Last = Value;
                DriverData_p->PropertyProperties[Prop].LastIsValid = TRUE;

                ok = ExecuteSequence(DriverData_p, Sequence_p, Length, (t_uint8*)&Value, DelayTime_p);
                if (!ok) {
                    DriverData_p->PropertyProperties[Prop].Last = OldValue;
                    DriverData_p->PropertyProperties[Prop].LastIsValid = OldValid;
                }
            }
        }
    }
    return((ok));
}


/*************************************************************************
 * Camera_Ctrllib::Camera_CtrlLib_GetProperty
 **************************************************************************/
int Camera_Ctrllib::Camera_CtrlLib_GetProperty(
        const CamCtrl_DriverData_t* DriverHandle,
        const HAL_Gr_Camera_Property_t Prop,
        int* const Value_p)
{
    t_uint8* Sequence_p;
    int Length;
    t_uint8 SequenceName[21];
    CamCtrl_DriverData_t* DriverData_p = (CamCtrl_DriverData_t*)DriverHandle;
    int NotUsed;
    int ok = FALSE;

    if(DriverData_p == NULL)
        DBGT_PTRACE(  "Error: DriverData_p is NULL");

    sprintf((char*)SequenceName, "seq_get_prop_%x", Prop);

    if (FindItem(DriverData_p, SequenceName, &Sequence_p, &Length))
    {
        DriverData_p->Registers[0] = 0;
        DriverData_p->Registers[1] = 0;
        DriverData_p->Registers[2] = 0;
        DriverData_p->Registers[3] = 0;
        if (ExecuteSequence(DriverData_p, Sequence_p, Length, NULL, &NotUsed))
        {
            *Value_p = DriverData_p->Registers[0] +
                (DriverData_p->Registers[1] << 8) +
                (DriverData_p->Registers[2] << 16) +
                (DriverData_p->Registers[3] << 24);
            ok = TRUE;
        }
    }
    else  //Take from cache
    {
        if (Prop < CAMCTRL_MAX_NUMBER_PROPERTIES)
        {
            if (DriverData_p->PropertyProperties[Prop].Supported)
            {
                ok = TRUE;
                if (DriverData_p->PropertyProperties[Prop].LastIsValid)
                {
                    DBGT_PTRACE("Camera_Ctrllib::Camera_CtrlLib_GetProperty: Value fetched from cache");
                    *Value_p = DriverData_p->PropertyProperties[Prop].Last;
                }
                else
                {
                    DBGT_PTRACE("Camera_Ctrllib::Camera_CtrlLib_GetProperty: Default value returned");
                    *Value_p = DriverData_p->PropertyProperties[Prop].Default;
                }
            }
        }
    }

    return((ok));
}


/*************************************************************************
 * Camera_Ctrllib::Camera_CtrlLib_GetWatchdogTimeout
 **************************************************************************/
int Camera_Ctrllib::Camera_CtrlLib_GetWatchdogTimeout(const CamCtrl_DriverData_t* DriverHandle)
{
    CamCtrl_DriverData_t* DriverData_p = (CamCtrl_DriverData_t*)DriverHandle;
    t_uint8* Sequence_p;
    int Length;
    int WatchdogTimeout = 0;

    if(DriverData_p == NULL)
        DBGT_PTRACE(  "Error: DriverData_p is NULL");

    if (FindDefinition(DriverData_p, CAMCTRL_CAM_WATCHDOG_TIMEOUT, &Sequence_p, &Length))
    {
        WatchdogTimeout = ConvertNumber(Sequence_p, Length, 16);
        DBGT_PTRACE("Camera_Ctrllib::Camera_CtrlLib_GetWatchdogTimeout, Watchdog timeout will be %d ms.", WatchdogTimeout);
    }

    return((WatchdogTimeout));
}

/*************************************************************************
 * Camera_Ctrllib::Camera_CtrlLib_GetPipeNumber
 **************************************************************************/
int Camera_Ctrllib::Camera_CtrlLib_GetPipeNumber(const CamCtrl_DriverData_t* DriverHandle)
{
    CamCtrl_DriverData_t* DriverData_p = (CamCtrl_DriverData_t*)DriverHandle;
    t_uint8* Sequence_p;
    int Length;
    int PipeNumber = 0;

    if(DriverData_p == NULL)
        DBGT_PTRACE(  "Error: DriverData_p is NULL");

    if (FindDefinition(DriverData_p, CAMCTRL_CAM_PIPE_NUMBER, &Sequence_p, &Length))
    {
        PipeNumber = ConvertNumber(Sequence_p, Length, 16);
        DBGT_PTRACE("Camera_Ctrllib::Camera_CtrlLib_GetPipeNumber, Pipe configuration %d", PipeNumber);
    }else{
        DBGT_PTRACE("Camera_Ctrllib::Camera_CtrlLib_GetPipeNumber, Warning: Pipe number not found");
    }

    return((PipeNumber));
}

/*************************************************************************
 * Camera_Ctrllib::Camera_CtrlLib_GetExifInfo
 **************************************************************************/
void Camera_Ctrllib::Camera_CtrlLib_GetExifInfo(
        const CamCtrl_DriverData_t* Camera_h,
        const int         MakerNoteSize,
        t_uint8*          MakerNote_p,
        t_uint8*          XResolution_p,
        t_uint8*          YResolution_p,
        t_uint8*          MaxNumPixelsX_p,
        t_uint8*          AnalogGain_p,
        t_uint8*          DigitalGain_p)
{
    CamCtrl_DriverData_t* DriverData_p = (CamCtrl_DriverData_t*)Camera_h;
    t_uint8* Sequence_p;
    int Length;

    if(DriverData_p == NULL)
        DBGT_PTRACE(  "Error: DriverData_p is NULL");

    if (MakerNoteSize > 0) {
        *MakerNote_p = 0;
    }
    *XResolution_p = 0;
    *YResolution_p = 0;
    *MaxNumPixelsX_p = 0;
    *AnalogGain_p = 0;
    *DigitalGain_p = 0;

    if (FindDefinition(DriverData_p, CAMCTRL_EXIF_MAKER_NOTE, &Sequence_p, &Length))
    {
        if ((int)MakerNoteSize > Length)
        {
            memcpy(MakerNote_p, Sequence_p, (int)Length);
            *(MakerNote_p + Length) = 0;
        } else {
            DBGT_PTRACE("Camera_Ctrllib::Camera_CtrlLib_GetExifInfo, Warning: Maker note size exceeds allocated size");
        }
    }

    if (FindDefinition(DriverData_p, CAMCTRL_EXIF_X_RESOLUTION, &Sequence_p, &Length))
    {
        *XResolution_p = (t_uint8)ConvertNumber(Sequence_p, Length, 16);
    }

    if (FindDefinition(DriverData_p, CAMCTRL_EXIF_Y_RESOLUTION, &Sequence_p, &Length))
    {
        *YResolution_p = (t_uint8)ConvertNumber(Sequence_p, Length, 16);
    }

    if (FindDefinition(DriverData_p, CAMCTRL_EXIF_MAX_NUM_PIXELS_X, &Sequence_p, &Length))
    {
        *MaxNumPixelsX_p = (t_uint8)ConvertNumber(Sequence_p, Length, 16);
    }

    if (FindDefinition(DriverData_p, CAMCTRL_EXIF_ANALOG_GAIN, &Sequence_p, &Length))
    {
        *AnalogGain_p = (t_uint8)ConvertNumber(Sequence_p, Length, 16);
    }

    if (FindDefinition(DriverData_p, CAMCTRL_EXIF_DIGITAL_GAIN, &Sequence_p, &Length))
    {
        *DigitalGain_p = (t_uint8)ConvertNumber(Sequence_p, Length, 16);
    }

}

/*************************************************************************
 * Camera_Ctrllib::Camera_CtrlLib_GetFocusStatus
 **************************************************************************/

int Camera_Ctrllib::Camera_CtrlLib_GetFocusStatus(
        const CamCtrl_DriverData_t* Camera_h,
        int* const Value_p)
{
    CamCtrl_DriverData_t* DriverData_p = (CamCtrl_DriverData_t*)Camera_h;
    t_uint8* Sequence_p;
    int Length;
    t_uint8 RegNo;
    int ok;
    int NotUsed;

    if(DriverData_p == NULL)
        DBGT_PTRACE(  "Error: DriverData_p is NULL");

    ok = FALSE;
    if (FindDefinition(DriverData_p, CAMCTRL_SEQ_FOCUS_STATUS, &Sequence_p, &Length))
    {
        ok = ExecuteSequence(DriverData_p, Sequence_p, Length, NULL, &NotUsed);
        if (ok && (FindDefinition(DriverData_p, CAMCTRL_CAM_FOCUS_RESULT, &Sequence_p, &Length)))
        {
            ok = (int)(Length < 30); //No more than 10 registers allowed
            RegNo = 0;
            while ((Length > 1) && ok)
            {
                ok = (int)(DriverData_p->Registers[RegNo++] == (t_uint8)ConvertNumber(Sequence_p, 2, 16));

                //Read past delimiter
                Sequence_p += 3;
                Length -= 3;
            }
        }
    }

    if(ok){
        *Value_p = (int) OMX_FocusStatusReached;
        DBGT_PTRACE(  "Camera_CtrlLib_GetFocusStatus: OMX_FocusStatusReached");
    }else{
        *Value_p = (int) OMX_FocusStatusRequest;
        DBGT_PTRACE(  "Camera_CtrlLib_GetFocusStatus: OMX_FocusStatusRequest");
    }


    return((ok));
}


/*************************************************************************
 * Camera_Ctrllib::Camera_CtrlLib_SetViewfindSize
 **************************************************************************/
int Camera_Ctrllib::Camera_CtrlLib_SetViewfindSize(
        const CamCtrl_DriverData_t* DriverHandle,
        const t_uint8 Id,
        int* const DelayTime_p)
{
    CamCtrl_DriverData_t* DriverData_p = (CamCtrl_DriverData_t*)DriverHandle;
    t_uint8* Sequence_p;
    int Length;
    t_uint8 SequenceName[22];
    int ok;
    t_uint8 OldId;

    if(DriverData_p == NULL)
        DBGT_PTRACE(  "Error: DriverData_p is NULL");

    ok = FALSE;

    //Set the current size id to the one we want to set
    //to make the $z command work as expected together with the $u command
    //Store the old value to restore if something goes wrong
    OldId = DriverData_p->CurrentSizeId;
    DriverData_p->CurrentSizeId = Id;

    //Build sequence name like: seq_vf_config_x where x = config id
    sprintf((char*)SequenceName, "seq_vf_config_%x", Id);

    if (FindItem(DriverData_p, SequenceName, &Sequence_p, &Length))
    {
        ok = ExecuteSequence(DriverData_p, Sequence_p, Length, NULL, DelayTime_p);
    }
    if (!ok)
    {
        DriverData_p->CurrentSizeId = OldId;
    }

    return((ok));
}

/*************************************************************************
 * Camera_Ctrllib::Camera_CtrlLib_SetSnapshotSize
 **************************************************************************/
int Camera_Ctrllib::Camera_CtrlLib_SetSnapshotSize(
        const CamCtrl_DriverData_t* DriverHandle,
        const t_uint8 Id,
        int* const DelayTime_p)
{
    CamCtrl_DriverData_t* DriverData_p = (CamCtrl_DriverData_t*)DriverHandle;
    t_uint8* Sequence_p;
    int Length;
    t_uint8 SequenceName[22];
    int ok;
    t_uint8 OldId;

    if(DriverData_p == NULL)
        DBGT_PTRACE(  "Error: DriverData_p is NULL");

    ok = FALSE;

    //Set the current size id to the one we want to set
    //to make the $z command work as expected together with the $u command
    //Store the old value to restore if something goes wrong
    OldId = DriverData_p->CurrentSizeId;
    DriverData_p->CurrentSizeId = Id;

    //Build sequence name like: seq_ss_config_x where x = config id
    sprintf((char*)SequenceName, "seq_ss_config_%x", Id);

    if (FindItem(DriverData_p, SequenceName, &Sequence_p, &Length))
    {
        ok = ExecuteSequence(DriverData_p, Sequence_p, Length, NULL, DelayTime_p);
    }
    if (!ok)
    {
        DriverData_p->CurrentSizeId = OldId;
    }

    return((ok));
}

/*************************************************************************
 * Camera_Ctrllib::Camera_CtrlLib_StartViewFind
 **************************************************************************/
int Camera_Ctrllib::Camera_CtrlLib_StartViewFind(
        const CamCtrl_DriverData_t* DriverHandle,
        int* const DelayTime_p)
{
    CamCtrl_DriverData_t* DriverData_p = (CamCtrl_DriverData_t*)DriverHandle;
    t_uint8* Sequence_p;
    int Length;
    int ok;

    if(DriverData_p == NULL)
        DBGT_PTRACE(  "Error: DriverData_p is NULL");

    ok = FALSE;
    *DelayTime_p = 0;
    if (FindDefinition(DriverData_p, CAMCTRL_SEQ_VF_START, &Sequence_p, &Length))
    {
        ok = ExecuteSequence(DriverData_p, Sequence_p, Length, NULL, DelayTime_p);
    }

    return((ok));
}


/*************************************************************************
 * Camera_Ctrllib::Camera_CtrlLib_StopViewFind
 **************************************************************************/
void Camera_Ctrllib::Camera_CtrlLib_StopViewFind(
        const CamCtrl_DriverData_t* DriverHandle,
        int* const DelayTime_p)
{
    CamCtrl_DriverData_t* DriverData_p = (CamCtrl_DriverData_t*)DriverHandle;
    t_uint8* Sequence_p;
    int Length;

    if(DriverData_p == NULL)
        DBGT_PTRACE(  "Error: DriverData_p is NULL");

    *DelayTime_p = 0;
    if (FindDefinition(DriverData_p, CAMCTRL_SEQ_VF_STOP, &Sequence_p, &Length))
    {
        (void)ExecuteSequence(DriverData_p, Sequence_p, Length, NULL, DelayTime_p);
    }
}

/*************************************************************************
*
* Camera_Ctrllib::Camera_CtrlLib_ReturnViewFind
*
**************************************************************************/
int Camera_Ctrllib::Camera_CtrlLib_ReturnViewFind(const CamCtrl_DriverData_t* DriverHandle,
                             int* const DelayTime_p)
{
    CamCtrl_DriverData_t* DriverData_p = (CamCtrl_DriverData_t*)DriverHandle;
    t_uint8* Sequence_p;
    int Length;
    int ok;

    if(DriverData_p == NULL)
        DBGT_PTRACE(  "Error: DriverData_p is NULL");

    ok = FALSE;
    *DelayTime_p = 0;
    if (FindDefinition(DriverData_p, CAMCTRL_SEQ_VF_RETURN, &Sequence_p, &Length))
    {
        ok = ExecuteSequence(DriverData_p, Sequence_p, Length, NULL, DelayTime_p);
    }

    return((ok));
}

/*************************************************************************
 * Camera_Ctrllib::Camera_CtrlLib_StartSnapshot
 **************************************************************************/
int Camera_Ctrllib::Camera_CtrlLib_StartSnapshot(
        const CamCtrl_DriverData_t* DriverHandle,
        int* const DelayTime_p)
{
    CamCtrl_DriverData_t* DriverData_p = (CamCtrl_DriverData_t*)DriverHandle;
    t_uint8* Sequence_p;
    int Length;
    int ok;


    if(DriverData_p == NULL)
        DBGT_PTRACE(  "Error: DriverData_p is NULL");

    ok = FALSE;
    *DelayTime_p = 0;
    if (FindDefinition(DriverData_p, CAMCTRL_SEQ_SS_START, &Sequence_p, &Length))
    {
        ok = ExecuteSequence(DriverData_p, Sequence_p, Length, NULL, DelayTime_p);
    }

    return((ok));
}

/*************************************************************************
 * Camera_Ctrllib::Camera_CtrlLib_StopSnapshot
 **************************************************************************/
void Camera_Ctrllib::Camera_CtrlLib_StopSnapshot(
        const CamCtrl_DriverData_t* DriverHandle,
        int* const DelayTime_p)
{
    CamCtrl_DriverData_t* DriverData_p = (CamCtrl_DriverData_t*)DriverHandle;
    t_uint8* Sequence_p;
    int Length;

    if(DriverData_p == NULL)
        DBGT_PTRACE(  "Error: DriverData_p is NULL");

    *DelayTime_p = 0;
    if (FindDefinition(DriverData_p, CAMCTRL_SEQ_SS_STOP, &Sequence_p, &Length))
    {
        (void)ExecuteSequence(DriverData_p, Sequence_p, Length, NULL, DelayTime_p);
    }
}

/*************************************************************************
 * Camera_Ctrllib::Camera_CtrlLib_TakeSnapshot
 **************************************************************************/
int Camera_Ctrllib::Camera_CtrlLib_TakeSnapshot(
        const CamCtrl_DriverData_t* DriverHandle,
        int* const DelayTime_p)
{
    CamCtrl_DriverData_t* DriverData_p = (CamCtrl_DriverData_t*)DriverHandle;
    t_uint8* Sequence_p;
    int Length;
    int ok;

    if(DriverData_p == NULL)
        DBGT_PTRACE(  "Error: DriverData_p is NULL");

    ok = TRUE;
    *DelayTime_p = 0;
    if (FindDefinition(DriverData_p, CAMCTRL_SEQ_SS_TAKE, &Sequence_p, &Length))
    {
        ok = ExecuteSequence(DriverData_p, Sequence_p, Length, NULL, DelayTime_p);
    }

    return((ok));
}

/*************************************************************************
 * Camera_Ctrllib::Camera_CtrlLib_VerifySnapshot
 **************************************************************************/
int Camera_Ctrllib::Camera_CtrlLib_VerifySnapshot(
        const CamCtrl_DriverData_t* DriverHandle,
        int* const DelayTime_p)
{
    CamCtrl_DriverData_t* DriverData_p = (CamCtrl_DriverData_t*)DriverHandle;
    t_uint8* Sequence_p;
    int Length;
    int ok;

    if(DriverData_p == NULL)
        DBGT_PTRACE(  "Error: DriverData_p is NULL");

    // If the verification sequence is not present, return true.
    ok = TRUE;
    *DelayTime_p = 0;
    if (FindDefinition(DriverData_p, CAMCTRL_SEQ_SS_VERIFY, &Sequence_p, &Length))
    {
        if (ExecuteSequence(DriverData_p, Sequence_p, Length, NULL, DelayTime_p))
        {
            ok = (int) (DriverData_p->Registers[0]);
        }
    }

    return((ok));
}

/*************************************************************************
 * Camera_Ctrllib::Camera_CtrlLib_GetAutoFlashThreshold
 **************************************************************************/
 void Camera_Ctrllib::Camera_CtrlLib_GetAutoFlashThreshold(const CamCtrl_DriverData_t* Camera_h,
                                                    t_uint32*          pAutoFlashOnThreshold,
                                                    t_uint32*          pAutoFlashOffThreshold)
{
  CamCtrl_DriverData_t* DriverData_p = (CamCtrl_DriverData_t*)Camera_h;
  t_uint8* Sequence_p;
  int Length;

  if(DriverData_p == NULL)
    DBGT_PTRACE("Error: DriverData_p is NULL");


  *pAutoFlashOnThreshold = 0;
  *pAutoFlashOffThreshold = 0;


  if (FindDefinition(DriverData_p, CAMCTRL_AUTO_FLASH_ON_THRESHOLD, &Sequence_p, &Length))
  {
    *pAutoFlashOnThreshold = (t_uint32)ConvertNumber(Sequence_p, Length, 16);
  }

  if (FindDefinition(DriverData_p, CAMCTRL_AUTO_FLASH_OFF_THRESHOLD, &Sequence_p, &Length))
  {
    *pAutoFlashOffThreshold = (t_uint32)ConvertNumber(Sequence_p, Length, 16);
  }
}

/*************************************************************************
 * Camera_Ctrllib::Camera_CtrlLib_GetFlashSynchroSupport
 **************************************************************************/
 void Camera_Ctrllib::Camera_CtrlLib_GetFlashSynchroSupport(const CamCtrl_DriverData_t* Camera_h,
                                                    int*          pSynchroFlashSupported)
{
  CamCtrl_DriverData_t* DriverData_p = (CamCtrl_DriverData_t*)Camera_h;
  t_uint8* Sequence_p;
  int Length;

  if(DriverData_p == NULL)
    DBGT_PTRACE("Error: DriverData_p is NULL");


  *pSynchroFlashSupported = 0;

  if (FindDefinition(DriverData_p, CAMCTRL_SYNCHRO_FLASH_SUPPORTED, &Sequence_p, &Length))
  {
    *pSynchroFlashSupported = (int)ConvertNumber(Sequence_p, Length, 16);
  }
}

/*************************************************************************
 * Camera_Ctrllib::Camera_CtrlLib_StartFlash
 **************************************************************************/
int Camera_Ctrllib::Camera_CtrlLib_StartFlash(
        const CamCtrl_DriverData_t* DriverHandle,
        int* const DelayTime_p)
{
    CamCtrl_DriverData_t* DriverData_p = (CamCtrl_DriverData_t*)DriverHandle;
    t_uint8* Sequence_p;
    int Length;
    int ok;


    if(DriverData_p == NULL)
        DBGT_PTRACE(  "Error: DriverData_p is NULL");

    ok = FALSE;
    *DelayTime_p = 0;
    if (FindDefinition(DriverData_p, CAMCTRL_SEQ_SS_FLASH_ON, &Sequence_p, &Length))
    {
        ok = ExecuteSequence(DriverData_p, Sequence_p, Length, NULL, DelayTime_p);
    }

    return((ok));
}

/*************************************************************************
 * Camera_Ctrllib::Camera_CtrlLib_StopFlash
 **************************************************************************/
void Camera_Ctrllib::Camera_CtrlLib_StopFlash(
        const CamCtrl_DriverData_t* DriverHandle,
        int* const DelayTime_p)
{
    CamCtrl_DriverData_t* DriverData_p = (CamCtrl_DriverData_t*)DriverHandle;
    t_uint8* Sequence_p;
    int Length;

    if(DriverData_p == NULL)
        DBGT_PTRACE(  "Error: DriverData_p is NULL");

    *DelayTime_p = 0;
    if (FindDefinition(DriverData_p, CAMCTRL_SEQ_SS_FLASH_OFF, &Sequence_p, &Length))
    {
        (void)ExecuteSequence(DriverData_p, Sequence_p, Length, NULL, DelayTime_p);
    }
}

/*************************************************************************
 *
 * Camera_Ctrllib::Camera_CtrlLib_GetVfFrameSkip
 *
 **************************************************************************/
t_uint8 Camera_Ctrllib::Camera_CtrlLib_GetVfFrameSkip(
        const CamCtrl_DriverData_t* DriverHandle)
{

    CamCtrl_DriverData_t* DriverData_p = (CamCtrl_DriverData_t*)DriverHandle;
    t_uint8* Sequence_p;
    int Length;
    t_uint8 SequenceName[27];
    t_uint8 Frames = 0;
    t_uint8 ok = TRUE;

    if(DriverData_p == NULL)
        DBGT_PTRACE(  "Error: DriverData_p is NULL\n");

    //First check if there exists a specific offset for this size id,
    //otherwise use the default one
    sprintf((char*)SequenceName, "cam_vf_frame_skip");

    if (!FindItem(DriverData_p, SequenceName, &Sequence_p, &Length))
    {
        ok = FindDefinition(DriverData_p, CAMCTRL_CAM_VF_FRAME_SKIP, &Sequence_p, &Length);
    }
    else
    {
        Length--;   //Note the nice feature that FindItem returns length including \0-char,
        //while FindDefinition does not! :-)
    }
    if (ok)
    {
        Frames = (t_uint8)ConvertNumber(Sequence_p, Length, 16);
    }
    return((Frames));
}

/*************************************************************************
 *
 * Camera_Ctrllib::Camera_CtrlLib_GetCcFrameSkip
 *
 **************************************************************************/
t_uint8 Camera_Ctrllib::Camera_CtrlLib_GetCcFrameSkip(
        const CamCtrl_DriverData_t* DriverHandle)
{

    CamCtrl_DriverData_t* DriverData_p = (CamCtrl_DriverData_t*)DriverHandle;
    t_uint8* Sequence_p;
    int Length;
    t_uint8 SequenceName[27];
    t_uint8 Frames = 0;
    t_uint8 ok = TRUE;

    if(DriverData_p == NULL)
        DBGT_PTRACE(  "Error: DriverData_p is NULL\n");

    //First check if there exists a specific offset for this size id,
    //otherwise use the default one
    sprintf((char*)SequenceName, "cam_cc_frame_skip");

    if (!FindItem(DriverData_p, SequenceName, &Sequence_p, &Length))
    {
        ok = FindDefinition(DriverData_p, CAMCTRL_CAM_CC_FRAME_SKIP, &Sequence_p, &Length);
    }
    else
    {
        Length--;   //Note the nice feature that FindItem returns length including \0-char,
        //while FindDefinition does not! :-)
    }
    if (ok)
    {
        Frames = (t_uint8)ConvertNumber(Sequence_p, Length, 16);
    }
    return((Frames));
}

/*************************************************************************
 *
 * Camera_Ctrllib::Camera_CtrlLib_GetSsFrameSkip
 *
 **************************************************************************/
t_uint8 Camera_Ctrllib::Camera_CtrlLib_GetSsFrameSkip(
        const CamCtrl_DriverData_t* DriverHandle)
{

    CamCtrl_DriverData_t* DriverData_p = (CamCtrl_DriverData_t*)DriverHandle;
    t_uint8* Sequence_p;
    int Length;
    t_uint8 SequenceName[27];
    t_uint8 Frames = 0;
    t_uint8 ok = TRUE;

    if(DriverData_p == NULL)
        DBGT_PTRACE(  "Error: DriverData_p is NULL\n");

    //First check if there exists a specific offset for this size id,
    //otherwise use the default one
    sprintf((char*)SequenceName, "cam_ss_frame_skip");

    if (!FindItem(DriverData_p, SequenceName, &Sequence_p, &Length))
    {
        ok = FindDefinition(DriverData_p, CAMCTRL_CAM_SS_FRAME_SKIP, &Sequence_p, &Length);
    }
    else
    {
        Length--;   //Note the nice feature that FindItem returns length including \0-char,
        //while FindDefinition does not! :-)
    }
    if (ok)
    {
        Frames = (t_uint8)ConvertNumber(Sequence_p, Length, 16);
    }
    return((Frames));
}

/*************************************************************************
 * GetNeededJpegBufSize
 **************************************************************************/
int Camera_Ctrllib::Camera_CtrlLib_GetNeededJPEG_BufferSize(
        const CamCtrl_DriverData_t* DriverHandle,
        const int SizeId,
        int* const BufferSize_p)
{
    CamCtrl_DriverData_t* DriverData_p = (CamCtrl_DriverData_t*)DriverHandle;
    int ok = FALSE;
    t_uint8* Sequence_p;
    int Length;
    t_uint8 SequenceName[32];


    if(DriverData_p == NULL)
        DBGT_PTRACE(  "Error: DriverData_p is NULL");


    sprintf((char*)SequenceName, "cam_ss_jpeg_buf_size_%x", SizeId);

    if (FindItem(DriverData_p, SequenceName, &Sequence_p, &Length))
    {
        ok = TRUE;
        *BufferSize_p = ConvertNumber(Sequence_p, Length-1, 16);
    }

    return((ok));
}
/*************************************************************************
 * Camera_Ctrllib::Camera_CtrlLib_AwbLock
 **************************************************************************/
int Camera_Ctrllib::Camera_CtrlLib_AwbLock(
        const CamCtrl_DriverData_t* DriverHandle,
        int* const DelayTime_p)
{
    CamCtrl_DriverData_t* DriverData_p = (CamCtrl_DriverData_t*)DriverHandle;
    t_uint8* Sequence_p;
    int Length;
    int ok;


    if(DriverData_p == NULL)
        DBGT_PTRACE(  "Error: DriverData_p is NULL");

    ok = FALSE;
    *DelayTime_p = 0;
    if (FindDefinition(DriverData_p, CAMCTRL_CAM_AWB_LOCK, &Sequence_p, &Length))
    {
        ok = ExecuteSequence(DriverData_p, Sequence_p, Length, NULL, DelayTime_p);
    }
    DBGT_PTRACE("Camera_CtrlLib_AwbLock result is %d",ok);
    return((ok));
}

/*************************************************************************
 * Camera_Ctrllib::Camera_CtrlLib_ExposureLock
 **************************************************************************/
int Camera_Ctrllib::Camera_CtrlLib_ExposureLock(
        const CamCtrl_DriverData_t* DriverHandle,
        int* const DelayTime_p)
{
    CamCtrl_DriverData_t* DriverData_p = (CamCtrl_DriverData_t*)DriverHandle;
    t_uint8* Sequence_p;
    int Length;
    int ok;


    if(DriverData_p == NULL)
        DBGT_PTRACE(  "Error: DriverData_p is NULL");

    ok = FALSE;
    *DelayTime_p = 0;
    if (FindDefinition(DriverData_p, CAMCTRL_CAM_EXPOSURE_LOCK, &Sequence_p, &Length))
    {
        ok = ExecuteSequence(DriverData_p, Sequence_p, Length, NULL, DelayTime_p);
    }
    DBGT_PTRACE("Camera_CtrlLib_ExposureLock result is %d",ok);
    return((ok));
}
/*************************************************************************
 * Camera_Ctrllib::Camera_CtrlLib_AwbUnLock
 **************************************************************************/
int Camera_Ctrllib::Camera_CtrlLib_AwbUnLock(
        const CamCtrl_DriverData_t* DriverHandle,
        int* const DelayTime_p)
{
    CamCtrl_DriverData_t* DriverData_p = (CamCtrl_DriverData_t*)DriverHandle;
    t_uint8* Sequence_p;
    int Length;
    int ok;


    if(DriverData_p == NULL)
        DBGT_PTRACE(  "Error: DriverData_p is NULL");

    ok = FALSE;
    *DelayTime_p = 0;
    if (FindDefinition(DriverData_p, CAMCTRL_CAM_AWB_UNLOCK, &Sequence_p, &Length))
    {
        ok = ExecuteSequence(DriverData_p, Sequence_p, Length, NULL, DelayTime_p);
    }
    DBGT_PTRACE("Camera_CtrlLib_AwbUnLock result is %d",ok);
    return((ok));
}
/*************************************************************************
 * Camera_Ctrllib::Camera_CtrlLib_ExposureUnLock
 **************************************************************************/
int Camera_Ctrllib::Camera_CtrlLib_ExposureUnLock(
        const CamCtrl_DriverData_t* DriverHandle,
        int* const DelayTime_p)
{
    CamCtrl_DriverData_t* DriverData_p = (CamCtrl_DriverData_t*)DriverHandle;
    t_uint8* Sequence_p;
    int Length;
    int ok;


    if(DriverData_p == NULL)
        DBGT_PTRACE(  "Error: DriverData_p is NULL");

    ok = FALSE;
    *DelayTime_p = 0;
    if (FindDefinition(DriverData_p, CAMCTRL_CAM_EXPOSURE_UNLOCK, &Sequence_p, &Length))
    {
        ok = ExecuteSequence(DriverData_p, Sequence_p, Length, NULL, DelayTime_p);
    }
    DBGT_PTRACE("Camera_CtrlLib_ExposureUnLock result is %d",ok);
    return((ok));
}
/*************************************************************************
 * Camera_Ctrllib::Camera_CtrlLib_GET_MAX_TOUCH_X_Y_VALUES
 **************************************************************************/

int Camera_Ctrllib::Camera_CtrlLib_GET_MAX_TOUCH_X_Y_VALUES(
        const CamCtrl_DriverData_t* DriverHandle)

{
    CamCtrl_DriverData_t* DriverData_p = (CamCtrl_DriverData_t*)DriverHandle;
    t_uint8* Sequence_p;
    int Length;
    t_uint8 RegNo;
    int NotUsed;
    int ok;

    if(DriverData_p == NULL)
        DBGT_PTRACE(  "Error: DriverData_p is NULL");

    ok = FALSE;
        if ((FindDefinition(DriverData_p, CAMCTRL_TOUCHFOCUS_X_Y_MAXVALUE, &Sequence_p, &Length)))
        {
            ok = (int)(Length < 30); //No more than 10 registers allowed
            RegNo = 0;
            while ((Length > 1) && ok)
            {
                (int)(DriverData_p->Registers[RegNo++] = (t_uint8)ConvertNumber(Sequence_p, 2, 16));

                //Read past delimiter
                Sequence_p += 3;
                Length -= 3;
            }
        }
    return((ok));
}

/*************************************************************************
 *
 * File local functions
 *
 *************************************************************************/
/*************************************************************************
 *
 * ParseFile
 *
 * Notes on the implementation:
 * The file is parsed and the information is stored in one of the
 * following structures:
 * 1) An array of predefined entries (g_Definitions)
 * 2) An array of property definitions (g_PropertyProperties)
 * 3) A list of all other definitions (List of ParsedItem_t) such as macros
 *    and sequences for setting properties and sizes
 *
 * Each item in the ParsedItem list are given an unique ID along with the
 * definition name. When a macro is encountered, the macro name is looked
 * up in the list and if it is found, the ID for that item is inserted.
 * When executing a sequence later on, a found macro is expanded by looking
 * up the definition by its ID. In other words; macros are not expanded
 * when parsing the file; it is done first when executing the sequences.
 * Thus memory usage is kept low.
 *
 **************************************************************************/
int Camera_Ctrllib::ParseFile(
        CamCtrl_DriverData_t* DriverData_p)
{
    t_uint8 nbuf[CAMCTRL_NAME_BUFFER_SIZE];
    t_uint8* vbuf;
    t_uint8 c, prev_c = 0;
    int in, iv;
    int ReadMore;
    int ok;
    int CurrentId = 0;
    CamCtrl_IO_t* FileIO_p;

    if ((vbuf = new t_uint8[CAMCTRL_VALUE_BUFFER_SIZE]) == NULL)
    {
        return FALSE;
    }

    if ((FileIO_p = new CamCtrl_IO_t) == NULL)
    {
        delete vbuf;
        vbuf=NULL;
        return FALSE;
    }

    FileIO_p->Buffer = GBigBuffer;

    //Initialize internal lists
    DriverData_p->ItemList_p = NULL;
    memset(DriverData_p->Definitions, 0, CAMCTRL_NUMBER_DEFINITIONS * sizeof(ParsedValue_t*));
    memset(DriverData_p->PropertyProperties, 0, CAMCTRL_MAX_NUMBER_PROPERTIES * sizeof(PropertyProperty_t));

    FileIO_p->BytesInBuffer = fileSize;
    FileIO_p->CurrentByte = 0;
    ok = TRUE;

    while ((ok) && (GetByte(FileIO_p, &c) != 0))
    {
        //Flush white spaces
        if (c == ' ')
            while ((GetByte(FileIO_p, &c) != 0) && (c == ' ')){}

        //Read past comments
        if (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')))
        {
            //Read name
            int EvenNumberOfQuotesFound = TRUE;
            in = 0;
            nbuf[in++] = c;

            ReadMore = TRUE;
            while (ReadMore && (GetByte(FileIO_p, &c) != 0))
            {
                if (c == '=')
                    ReadMore = FALSE;
                else
                {
                    //Don't store white spaces
                    if ((c != ' ') && (c != '\t') && (c != '\n') && (c != '\r') &&
                            (in < CAMCTRL_NAME_BUFFER_SIZE - 1))
                        nbuf[in++] = c;
                }
            }
            //Read value
            iv = 0;
            ReadMore = TRUE;
            while (ReadMore && (GetByte(FileIO_p, &c) != 0))
            {
                if (c == ';')
                    ReadMore = FALSE;
                else
                {
                    if (c == '"')
                    {
                        EvenNumberOfQuotesFound = !EvenNumberOfQuotesFound;
                    }

                    if (EvenNumberOfQuotesFound)
                    {
                        if ((c == '/') && (prev_c != '$'))  //Comment -- flush to eol
                        {
                            while ((GetByte(FileIO_p, &c) != 0) && (c != '\n')){}
                        }

                        if (c == '#') //Macro
                        {
                            t_uint8 pbuf[CAMCTRL_NAME_BUFFER_SIZE];
                            int ip = 0;
                            int id = 0;

                            //Read macro name
                            while (ReadMore && (GetByte(FileIO_p, &c) != 0))
                            {
                                if ((c == ' ') || (c == '\t') || (c == '\n') || (c == '\r') || (c == ';') || (c == '(') || (c == ')') || (c == ',') || (c == ']'))
                                    ReadMore = FALSE;
                                else
                                {
                                    if (ip < CAMCTRL_NAME_BUFFER_SIZE -1)
                                        pbuf[ip++] = c;
                                }
                            }
                            pbuf[ip++] = 0;
                            //Look up definition id
                            if (FindItemId(DriverData_p, pbuf, &id))
                            {
                                //Store id number in value string after a macro marker
                                if (iv + 10 < CAMCTRL_VALUE_BUFFER_SIZE - 1)
                                {
                                    vbuf[iv++] = '#';
                                    iv += (int)sprintf((char*)&vbuf[iv], "%x", id);
                                    if (c != '(')
                                        vbuf[iv++] = ';';       //End marker
                                }
                            }
                            else
                            {
                                DBGT_PTRACE("CamCtrl: Parse error Macro definition %s not found",pbuf);
                                ok = FALSE;
                            }

                            ReadMore = (int)(c != ';');
                        }

                        //Don't store white spaces
                        if ((c != ' ') && (c != '\t') && (c != '\n') && (c != '\r') && (c != ';') &&
                                (iv < CAMCTRL_VALUE_BUFFER_SIZE - 1))
                        {
                            vbuf[iv++] = c;
                            prev_c = c;
                        }
                    }
                    else
                    {
                        // Handle characters within strings differently. For example, don't strip
                        // spaces and make sure all characters are legal.
                        if ((c != '\n') && (c != '\r') && (c != ';') &&
                                (iv < CAMCTRL_VALUE_BUFFER_SIZE - 1))
                        {
                            vbuf[iv++] = c;
                            prev_c = c;
                        }
                    }
                }
            }

            //Save in internal structure
            nbuf[in++] = 0;
            vbuf[iv++] = 0;
            ok = HandleParsedItem(DriverData_p, nbuf, in, vbuf, iv, CurrentId++);
        }
        //Flush to next line
        if (c != '\n')
            while ((GetByte(FileIO_p, &c) != 0) && (c != '\n')){}
    }

    if (!ok)
    {
        FreeParsedItems(DriverData_p);
    }

    delete vbuf;
    vbuf=NULL;
    delete FileIO_p;
    FileIO_p=NULL;

    return ok;
}


/*************************************************************************
 * HandleParsedItem
 **************************************************************************/
int Camera_Ctrllib::HandleParsedItem(
        CamCtrl_DriverData_t* DriverData_p,
        const t_uint8* name_p,
        int nlen,
        t_uint8* value_p,
        int vlen,
        int id)
{
    ParsedItem_t* item_p;
    int errorflag;
    int ok = TRUE;
    const t_uint8 PropValuesString[] = "prop_values_";
    int PropValuesStringLen = sizeof(PropValuesString) - 1;

    //Handle special definitions
    if (!strcmp((char*)name_p, "sup_properties"))
    {
        StorePropertyProperties(DriverData_p, value_p, vlen);
    }
    else if ( ComparePartialStrings(name_p, PropValuesString, PropValuesStringLen))
    {
        int Prop = ConvertNumber(name_p + PropValuesStringLen, nlen - PropValuesStringLen - 1, 16);
        (void) StorePropertyValues(DriverData_p, Prop, value_p);
    }

    //If it is a defintion we know of, try to store it in array
    else if (InsertIntoDefinitionArray(DriverData_p, name_p, value_p, vlen, &errorflag))
    {
        ok = (int)!errorflag;
    }
    //Store in linked list
    else
    {
        if((item_p = new ParsedItem_t) == NULL)
        {
            DBGT_PTRACE("HandleParsedItem: Memory allocation failed!");
            ok = FALSE;
        }
        else
        {
            item_p->Name_p = new t_uint8[nlen];
            item_p->Id = id;
            item_p->Value.Length = vlen;
            item_p->Value.String_p = new t_uint8[vlen];
            memcpy(item_p->Name_p, name_p, nlen);
            memcpy(item_p->Value.String_p, value_p, vlen);
            item_p->Next_p = DriverData_p->ItemList_p;
            DriverData_p->ItemList_p = item_p;
        }
    }

    return ok;
}


/*************************************************************************
 * GetByte
 **************************************************************************/
int Camera_Ctrllib::GetByte(
        CamCtrl_IO_t* const IO_p,
        t_uint8* c_p)
{
    if (IO_p->CurrentByte >= IO_p->BytesInBuffer)
    {
        return FALSE;
    }

    *c_p = IO_p->Buffer[IO_p->CurrentByte++];

    return TRUE;
}

/*************************************************************************
 * GetSizeConfig
 **************************************************************************/
int Camera_Ctrllib::GetSizeConfig(
        t_uint8* Sequence_p,
        int Length,
        HAL_Gr_Camera_SizeConfig_t* SizeConfig_p,
        int* Offset_p)
{
    int ItemLength;
    t_uint8  FoundSize = 0;
    int FoundWidth = 0;
    int FoundHeight = 0;
    int FoundType = 0;
    int bExtendedSize = FALSE;
    int FoundCamWidth = 0;
    int FoundCamHeight = 0;
    int TotalLength = 0;
    int cItems;

    cItems = 0;
    ItemLength = (int)GetNextDelimiterPos(Sequence_p, '[');
    FoundSize = (t_uint8)ConvertNumber(Sequence_p, ItemLength, 16);
    ItemLength++;
    TotalLength += ItemLength;
    Sequence_p += ItemLength ;
    Length -= ItemLength;

    //Extract width
    if (Length > 0)
    {
        ItemLength = (int)GetNextDelimiterPos(Sequence_p, ',');
        FoundWidth = (int)ConvertNumber(Sequence_p, ItemLength, 16);
        ItemLength++;
        TotalLength += ItemLength;
        Sequence_p += ItemLength;
        Length -= ItemLength;
        cItems++;
    }

    //Extract height
    if (Length > 0)
    {
        ItemLength = (int)GetNextDelimiterPos(Sequence_p, ',');
        FoundHeight = (int)ConvertNumber(Sequence_p, ItemLength, 16);
        ItemLength++;
        TotalLength += ItemLength;
        Sequence_p += ItemLength;
        Length -= ItemLength;
        cItems++;
    }

    //Extract frame rate
    if (Length > 0)
    {
        ItemLength = 0;
        bExtendedSize = FALSE;

        // First try extended sizeconfig
        // Next delimeter might be either , or ]
        {
            t_uint8* SeqEnd_p;
            int bContinue;

            bContinue = TRUE;
            SeqEnd_p = Sequence_p;

            while (bContinue)
            {
                switch (*SeqEnd_p)
                {
                case ',':
                    bExtendedSize = TRUE;
                    // Fall through!
                case ']':
                case 0:
                    bContinue = FALSE;
                    break;

                default:
                    SeqEnd_p++;
                    ItemLength++;
                    break;
                }
            }
        }

        FoundType = (int)ConvertNumber(Sequence_p, ItemLength, 16);

        if (!bExtendedSize)
        {
            //Skip delimiter between properties
            ItemLength++;
        }
        ItemLength++;
        Sequence_p += ItemLength;
        TotalLength += ItemLength;
        Length -= ItemLength;
        cItems++;
    }

    // Extended properties in sizeconfig (optional)
    // Default to width and height
    FoundCamWidth = FoundWidth;
    FoundCamHeight = FoundHeight;

    //Extract extended width
    if (bExtendedSize && Length > 0)
    {
        ItemLength = (int)GetNextDelimiterPos(Sequence_p, ',');
        FoundCamWidth = (int)ConvertNumber(Sequence_p, ItemLength, 16);
        ItemLength++;
        Sequence_p += ItemLength;
        TotalLength += ItemLength;
        Length -= ItemLength;
        cItems++;
    }

    //Extract extended height
    if (bExtendedSize && Length > 0)
    {
        ItemLength = (int)GetNextDelimiterPos(Sequence_p, ']');
        FoundCamHeight = (int)ConvertNumber(Sequence_p, ItemLength, 16);
        //Skip delimiter between properties
        ItemLength += 2;
        Sequence_p += ItemLength;
        TotalLength += ItemLength;
        Length -= ItemLength;
        cItems++;
    }

    if (SizeConfig_p != NULL)
    {
        SizeConfig_p->ID = FoundSize;
        SizeConfig_p->Width = FoundWidth;
        SizeConfig_p->Height = FoundHeight;
        SizeConfig_p->Type= FoundType;
        SizeConfig_p->CameraWidth = FoundCamWidth;
        SizeConfig_p->CameraHeight = FoundCamHeight;
    }

    if (Offset_p != NULL)
    {
        *Offset_p = TotalLength;
    }

    // Width, height and framerate is non-optional!
    return cItems >= 3;
}

/*************************************************************************
 * GetI2CAddress
 **************************************************************************/
int Camera_Ctrllib::GetI2CAddress(
        CamCtrl_DriverData_t* DriverData_p)
{
    t_uint8* Sequence_p;
    int Length=0;
    t_uint8  Address;
    int ok;

    ok = FALSE;
    if (FindDefinition(DriverData_p, CAMCTRL_CAM_I2C_ADDRESS, &Sequence_p, &Length))
    {
        Address = (t_uint8)ConvertNumber(Sequence_p, Length, 16);
        DriverData_p->I2C_WriteAddress = Address;
        DriverData_p->I2C_ReadAddress = Address + 1;
        ok = TRUE;
        DBGT_PTRACE("I2C address is 0x%X ",Address);
    }
    return ok;
}


/*************************************************************************
 * StorePropertyProperties
 **************************************************************************/
void Camera_Ctrllib::StorePropertyProperties(
        CamCtrl_DriverData_t* DriverData_p,
        t_uint8* props_p,
        int plen)
{
    int Length;
    t_uint8* Sequence_p;
    int ItemLength;
    int FoundProp;
    int FoundMin = 0;
    int FoundMax = 0;
    int FoundDefault;

    Sequence_p = (t_uint8*)props_p;
    Length = (int)plen;
    while (Length > 0)
    {
        ItemLength = (int)GetNextDelimiterPos(Sequence_p, '[');
        FoundProp = (int)ConvertNumber(Sequence_p, ItemLength, 16);
        Sequence_p += ItemLength + 1;
        Length -= (ItemLength + 1);

        if (Length > 0)
        {
            ItemLength = (int)GetNextDelimiterPos(Sequence_p, ',');
            FoundMin = ConvertNumber(Sequence_p, ItemLength, 16);
            Sequence_p += ItemLength + 1;
            Length -= (ItemLength + 1);
        }

        if (Length > 0)
        {
            ItemLength = (int)GetNextDelimiterPos(Sequence_p, ',');
            FoundMax = ConvertNumber(Sequence_p, ItemLength, 16);
            Sequence_p += ItemLength + 1;
            Length -= (ItemLength + 1);
        }

        if (Length > 0)
        {
            ItemLength = (int)GetNextDelimiterPos(Sequence_p, ']');
            FoundDefault = ConvertNumber(Sequence_p, ItemLength, 16);
            Sequence_p += ItemLength + 2; //Skip delimiter between properties
            Length -= (ItemLength + 2);

            if (FoundProp < CAMCTRL_MAX_NUMBER_PROPERTIES)
            {
                DriverData_p->PropertyProperties[FoundProp].Supported = TRUE;
                DriverData_p->PropertyProperties[FoundProp].LastIsValid = FALSE;
                DriverData_p->PropertyProperties[FoundProp].Min = FoundMin;
                DriverData_p->PropertyProperties[FoundProp].Max = FoundMax;
                DriverData_p->PropertyProperties[FoundProp].Default = FoundDefault;
            }
        }
    }
}

/*************************************************************************
 * ClearUsedProperties
 **************************************************************************/
void Camera_Ctrllib::ClearUsedProperties(
        CamCtrl_DriverData_t* DriverData_p)
{
    int i;
    for (i=0;i<CAMCTRL_MAX_NUMBER_PROPERTIES;)
        DriverData_p->PropertyProperties[i++].LastIsValid = FALSE;
}

/*************************************************************************
 * StorePropertyValues
 **************************************************************************/
int Camera_Ctrllib::StorePropertyValues(
        CamCtrl_DriverData_t* DriverData_p,
        const int Prop,
        const t_uint8* Sequence_p)
{
    int bSuccess = TRUE;
    int* Values_p;
    int NumValuesStoredInValuesList = GetValuesFromSequence(Sequence_p, 0, NULL);

    DBGT_PTRACE("StorePropertyValues: Property: %d; NumValuesStoredInValuesList: %d", Prop, NumValuesStoredInValuesList);

    Values_p = new int[NumValuesStoredInValuesList << 2];
    if (Values_p != NULL)
    {
        (void) GetValuesFromSequence(Sequence_p, NumValuesStoredInValuesList, Values_p);
        DriverData_p->PropertyProperties[Prop].Values_p = Values_p;
        DriverData_p->PropertyProperties[Prop].NumValuesStoredInValuesList = NumValuesStoredInValuesList;
    }
    else
    {
        DBGT_PTRACE("StorePropertyValues: Memory allocation failed!");
        bSuccess = FALSE;
    }


    if (bSuccess)
    {
        DBGT_PTRACE("StorePropertyValues returned TRUE");
    }
    else
    {
        DBGT_PTRACE("StorePropertyValues returned FALSE");
    }

    return bSuccess;
}

/*************************************************************************
 * FreeParsedItems
 **************************************************************************/
void Camera_Ctrllib::FreeParsedItems(
        CamCtrl_DriverData_t* DriverData_p)
{
    ParsedItem_t* Temp_p;
    ParsedItem_t* Item_p = DriverData_p->ItemList_p;
    int i;

    //Free definitions from linked list
    while (Item_p)
    {
        Temp_p = Item_p;
        Item_p = Item_p->Next_p;
        delete Temp_p->Name_p;
        Temp_p->Name_p=NULL;
        delete Temp_p->Value.String_p;
        Temp_p->Value.String_p=NULL;
        delete Temp_p;
        Temp_p=NULL;
    }

    //Free definitions from array
    for (i=0;i<CAMCTRL_NUMBER_DEFINITIONS;i++)
    {
        if (DriverData_p->Definitions[i] != NULL)
        {
            CamCtrl_DriverData_t* DriverDataNotConst_p = (CamCtrl_DriverData_t*) DriverData_p;
            delete DriverDataNotConst_p->Definitions[i]->String_p;
            DriverDataNotConst_p->Definitions[i]->String_p=NULL;
            delete DriverDataNotConst_p->Definitions[i];
            DriverDataNotConst_p->Definitions[i]=NULL;
        }
    }

    // Free property values
    for (i=0;i<CAMCTRL_MAX_NUMBER_PROPERTIES;i++)
    {
        if (DriverData_p->PropertyProperties[i].Values_p != NULL)
        {
            delete DriverData_p->PropertyProperties[i].Values_p;
            DriverData_p->PropertyProperties[i].Values_p = NULL;
            DriverData_p->PropertyProperties[i].NumValuesStoredInValuesList = 0;
        }

    }
}

/*************************************************************************
 * InsertIntoDefinitionArray
 **************************************************************************/
int Camera_Ctrllib::InsertIntoDefinitionArray(
        CamCtrl_DriverData_t* DriverData_p,
        const t_uint8* Name_p,
        const t_uint8* Value_p,
        int ValueLength, int* error_p)
{
    int found = FALSE;
    int index;

    index = 0;
    while (!found && (index < CAMCTRL_NUMBER_DEFINITIONS))
    {
        if (!strcmp((char*)Name_p, DefinitionStrings[index]))
        {
            found = TRUE;
        }
        else
        {
            index++;
        }
    }

    if (found && (index < CAMCTRL_NUMBER_DEFINITIONS))
    {
        *error_p = FALSE;
        if (DriverData_p->Definitions[index] == NULL)
        {
            if ((DriverData_p->Definitions[index] = new ParsedValue_t) == NULL)
            {
                DBGT_PTRACE("InsertIntoDefinitionArray: Memory allocation failed!");
                *error_p = TRUE;
            }
            else
            {
                DriverData_p->Definitions[index]->Length = ValueLength - 1;  //Don't count zero termination
                DriverData_p->Definitions[index]->String_p = new t_uint8[ValueLength];
                memcpy(DriverData_p->Definitions[index]->String_p, Value_p, ValueLength);
            }
        }
    }

    return found;
}

/*************************************************************************
 * FindItem
 **************************************************************************/
int Camera_Ctrllib::FindItem(
        const CamCtrl_DriverData_t* DriverData_p,
        const t_uint8* Name_p,
        t_uint8** Value_pp,
        int* Length_p)
{
    int Found = FALSE;
    ParsedItem_t* Item_p = DriverData_p->ItemList_p;

    while (Item_p && !Found)
    {
        if (!strcmp((char*)Item_p->Name_p, (char*)Name_p))
        {
            *Value_pp = Item_p->Value.String_p;
            *Length_p = (int)Item_p->Value.Length;
            Found = TRUE;
        }
        Item_p = Item_p->Next_p;
    }

    return Found;
}

/*************************************************************************
 * FindNextMatchingItem
 **************************************************************************/
const ParsedItem_t* Camera_Ctrllib::FindNextMatchingItem(
        const CamCtrl_DriverData_t* DriverData_p,
        const ParsedItem_t* PrevItem_p,
        const t_uint8* PartialName_p)
{
    ParsedItem_t* Item_p          = DriverData_p->ItemList_p;
    ParsedItem_t* FoundItem_p     = NULL;

    // Use previously found item if specified
    if (PrevItem_p != NULL)
    {
        Item_p = PrevItem_p->Next_p;
    }

    while (Item_p && !FoundItem_p)
    {
        if (!strcmp((char*)Item_p->Name_p, (char*)PartialName_p))
        {
            FoundItem_p = Item_p;
        }
        Item_p = Item_p->Next_p;
    }
    return FoundItem_p;
}


/*************************************************************************
 * FindItemId
 **************************************************************************/
int Camera_Ctrllib::FindItemId(
        const CamCtrl_DriverData_t* DriverData_p,
        const t_uint8* Name_p,
        int* Id_p)
{
    int Found = FALSE;
    ParsedItem_t* Item_p = DriverData_p->ItemList_p;

    while (Item_p && !Found)
    {
        if (!strcmp((char*)Item_p->Name_p, (char*)Name_p))
        {
            *Id_p     = Item_p->Id;
            Found = TRUE;
        }
        Item_p = Item_p->Next_p;
    }

    return Found;
}

/*************************************************************************
 * FindItemById
 **************************************************************************/
int Camera_Ctrllib::FindItemById(
        const CamCtrl_DriverData_t* DriverData_p,
        int Id,
        t_uint8** Value_pp,
        int* Length_p)
{
    int Found = FALSE;
    ParsedItem_t* Item_p = DriverData_p->ItemList_p;

    while (Item_p && !Found)
    {
        if (Item_p->Id == Id)
        {
            *Value_pp = Item_p->Value.String_p;
            *Length_p = (int)Item_p->Value.Length;
            Found = TRUE;
        }
        Item_p = Item_p->Next_p;
    }

    return Found;
}

/*************************************************************************
 * FindDefinition
 **************************************************************************/
int Camera_Ctrllib::FindDefinition(
        const CamCtrl_DriverData_t* DriverData_p,
        CamCtrl_DefinitionName_t Definition,
        t_uint8** Value_pp,
        int* Length_p)
{
    int Found = FALSE;

    if (DriverData_p->Definitions[Definition] != NULL)
    {
        *Value_pp = DriverData_p->Definitions[Definition]->String_p;
        *Length_p = (int)DriverData_p->Definitions[Definition]->Length;
        Found = TRUE;
    }

    return Found;
}

/*************************************************************************
 * GetNextDelimiterPos
 **************************************************************************/
int Camera_Ctrllib::GetNextDelimiterPos(
        const t_uint8* Sequence_p,
        t_uint8 Delimiter)
{
    int Pos=0;

    while ((*Sequence_p != Delimiter) && (*Sequence_p != 0))
    {
        Sequence_p++;
        Pos++;
    }
    return Pos;
}

/*************************************************************************
 * Name:         GetNextDelimiterPos
 *
 * Parameters:   Sequence_p           [IN]   Sequence to investigate
 *               Delimiter1           [IN]   The first delimiter to look for
 *               Delimiter2           [IN]   The second delimiter to look for
 *
 * Returns:      int               [Ret]  The distance to the first found delimiter
 *
 * Description:  This functions takes a pointer to a sequence and finds the distance to the
 *                               first of two delimiters. For example:
 *                                   Sequence_p = 123,13,176]...
 *                                   Delimiter1 = ','
 *                                   Delimiter2 = ']'
 *                               will return 3 (the distance to the ',' is 3)
 *
 **************************************************************************/
int Camera_Ctrllib::GetNextDelimiterPos2(
        const t_uint8* Sequence_p,
        const t_uint8 Delimiter1,
        const t_uint8 Delimiter2)
{
    int Pos=0;

    while ((*Sequence_p != Delimiter1) && (*Sequence_p != Delimiter2) && (*Sequence_p != 0))
    {
        Sequence_p++;
        Pos++;
    }
    return Pos;
}

/*************************************************************************
 * Name:         GetValuesFromSequence
 *
 * Parameters:   Sequence_p           [IN]   Sequence to get the values from
 *               NumToGet             [IN]   Number of values to get
 *               Values_p             [OUT]  Values from the sequence
 *
 * Returns:      int               [Ret]  The actual number of values in the sequence
 *
 * Description:  This functions takes a sequence and retrieves the values in the sequence.
 *                                   The sequence must not contain white space, must start
 *                                   with a '[' character, and end with a ']' character and
 *                                   all values must be hexadecimal and delimited with commas.
 *                                   For example: Sequence_p = [12,3f,8e,987a]
 *
 **************************************************************************/
int Camera_Ctrllib::GetValuesFromSequence(
        const t_uint8* Sequence_p,
        const int NumToGet,
        int* Values_p)
{
    int  ValueLength;
    int ValueCount = 0;
    int Length;

    // Skip the '[' character
    ValueLength = (int)GetNextDelimiterPos(Sequence_p, '[');
    ValueLength++;            // Skip the [ character
    Sequence_p += ValueLength;

    // Find out how long the sequence is minus the '[' and ']' characters
    Length = (int)GetNextDelimiterPos(Sequence_p, ']');

    //Extract the values
    while (Length > 0)
    {
        ValueCount++;
        ValueLength = (int)GetNextDelimiterPos2(Sequence_p, ',', ']');
        if (Values_p != NULL && ValueCount <= NumToGet)
        {
            *Values_p++ = (int)ConvertNumber(Sequence_p, ValueLength, 16);
        }
        ValueLength++;            // Skip the ',' or the ']' characters
        Sequence_p += ValueLength;
        Length -= ValueLength;
    }

    return ValueCount;
}


/*************************************************************************
 * ExecuteSequence
 **************************************************************************/
int Camera_Ctrllib::ExecuteSequence(
        CamCtrl_DriverData_t* DriverData_p,
        t_uint8* szSequence,
        int SequenceLength,
        const t_uint8* InByteArray_p,
        int* DelayTime_p)
{
    return ExecuteSequenceWithInbuffer(DriverData_p, szSequence, SequenceLength, InByteArray_p, DelayTime_p, NULL, 0);
}


/*************************************************************************
 * ExecuteSequenceWithInbuffer
 **************************************************************************/
int Camera_Ctrllib::ExecuteSequenceWithInbuffer(
        CamCtrl_DriverData_t* DriverData_p,
        t_uint8* szSequence,
        int SequenceLength,
        const t_uint8* InByteArray_p,
        int* DelayTime_p,
        t_uint8* InBuffer_p,
        int InBufferSize)
{
    CamCtrl_SequenceData_t* SeqData_p;
    t_uint8** InBuffer_pp;
    int ok;

    if ((SeqData_p = new CamCtrl_SequenceData_t) == NULL)
    {
        return FALSE;
    }

    if(InBuffer_p)
    {
        InBuffer_pp = &InBuffer_p;
    }
    else
    {
        InBuffer_pp = 0;
    }

    *DelayTime_p = 0;
    SeqData_p->DelayTime_p = DelayTime_p;
    SeqData_p->ParamArray_p = (t_uint8*)InByteArray_p;
    SeqData_p->WriteBufferIndex = 0;
    SeqData_p->MacroData.Depth = 0;
    SeqData_p->MacroData.NumParams = 0;

    ok = ExecuteSequencePart(DriverData_p, szSequence, SequenceLength, SeqData_p, InBuffer_pp, &InBufferSize);

    // Flush write buffer
    if (ok && (SeqData_p->WriteBufferIndex > 0))
    {
        DBGT_PTRACE("ExecuteSequenceWithInbuffer: Write %d byte(s) to I2C", SeqData_p->WriteBufferIndex);
        HAL_Gr_I2C_ErrorStatus_t ErrorStatus = I2C_Write(
                DriverData_p->I2C_WriteAddress,
                SeqData_p->WriteBufferIndex,
                SeqData_p->WriteBuffer,
                CAMCTRL_I2C_RETRIES,
                CAMCTRL_I2C_REPEAT_RETRIES);
        ok = (ErrorStatus == HAL_GR_I2C_SUCCESS);
    }

    delete SeqData_p;
    SeqData_p=NULL;
    return ok;
}


/*************************************************************************
 *
 * ExecuteSequencePart
 *
 * Executes a sequence using the supplied writebuffer.
 * The reason why this functionality is written in a separate
 * function instead of directly in ExecuteSequence is that we
 * want to call this function recursivly when handling macros.
 **************************************************************************/
int Camera_Ctrllib::ExecuteSequencePart(
        CamCtrl_DriverData_t* DriverData_p,
        t_uint8* szSequence,
        int SequenceLength,
        CamCtrl_SequenceData_t* SeqData_p,
        t_uint8** InBuffer_pp,
        int* InBufferSize_p)
{
    t_uint8             CurrentChar;
    int           ok = TRUE;

    if (SeqData_p->MacroData.Depth++ == CAMCTRL_MAX_MACRO_DEPTH) //Nested to deep!
    {
        DBGT_PTRACE("ExecuteSequencePart: Error! Max macro depth exceeded");
        ok = FALSE;
    }

    while ((SequenceLength > 0) && ok)
    {
        CurrentChar = ReadNextChar(&szSequence, &SequenceLength);

        if ((CurrentChar >= '0' && CurrentChar <= '9') ||
                (CurrentChar >= 'A' && CurrentChar <= 'F') ||
                (CurrentChar >= 'a' && CurrentChar <= 'f'))
        {

            t_uint8             NextChar = ReadNextChar(&szSequence, &SequenceLength);
            if ((NextChar >= '0' && NextChar <= '9') ||
                    (NextChar >= 'A' && NextChar <= 'F') ||
                    (NextChar >= 'a' && NextChar <= 'f'))
            {

                // Output data
                SeqData_p->WriteBuffer[SeqData_p->WriteBufferIndex++] = (t_uint8)((ConvertDigit(CurrentChar) << 4) + (ConvertDigit(NextChar)));
            }
            else
            {
                ok = FALSE;
                DBGT_PTRACE("CamCtrl: Parse error");
            }
        }
        else if (CurrentChar == '$')
        {
            // Command
            CurrentChar = ReadNextChar(&szSequence, &SequenceLength);
            switch (CurrentChar)
            {
            case 'a':
                {
                    ok = SeqCommand_a(DriverData_p, &szSequence, &SequenceLength);
                    break;
                }
            case 'b':
                {
                    ok =SeqCommand_b(DriverData_p, &szSequence, &SequenceLength, SeqData_p);
                    break;
                }
            case 'c':
                {
                    ok = SeqCommand_c(DriverData_p, &szSequence, &SequenceLength, SeqData_p);
                    break;
                }
            case 'd':
                {
                    ok =SeqCommand_d(&szSequence, &SequenceLength);
                    break;
                }
            case 'e':
                {
                    ok = SeqCommand_e(DriverData_p, &szSequence, &SequenceLength, SeqData_p);
                    break;
                }
            case 'f':
                {
                    ok = SeqCommand_f(DriverData_p, SeqData_p);
                    break;
                }
            case 'm':
                {
                    ok =SeqCommand_m(DriverData_p, &szSequence, &SequenceLength);
                    break;
                }
            case 'p':
                {
                    ok =SeqCommand_p(DriverData_p, &szSequence, &SequenceLength);
                    break;
                }
            case 'r':
                {
                    ok = SeqCommand_r(DriverData_p, &szSequence, &SequenceLength, SeqData_p, InBuffer_pp, InBufferSize_p);
                    break;
                }
            case 's':
                {
                    ok =SeqCommand_s(DriverData_p, &szSequence, &SequenceLength, SeqData_p);
                    break;
                }
            case 't':
                {
                    ok =SeqCommand_t(&szSequence, &SequenceLength, SeqData_p);
                    break;
                }
            case 'u':
                {
                    ok = SeqCommand_u(DriverData_p, &szSequence, &SequenceLength, SeqData_p);
                    break;
                }
            case 'w':
                {
                    ok =SeqCommand_w(DriverData_p, &szSequence, &SequenceLength, SeqData_p);
                    break;
                }
            case 'z':
                {
                    ok = SeqCommand_z(DriverData_p, &szSequence, &SequenceLength);
                    break;
                }
            case '<':
                {
                    ok =SeqCommand_lt(DriverData_p, &szSequence, &SequenceLength);
                    break;
                }
            case '>':
                {
                    ok =SeqCommand_gt(DriverData_p, &szSequence, &SequenceLength);
                    break;
                }
            case '*':
            case '/':
            case '+':
            case '-':
                {
                    ok =SeqCommand_aritm(CurrentChar, DriverData_p, &szSequence, &SequenceLength, SeqData_p);
                    break;
                }
            case '"':
                {
                    ok = SeqCommand_pr(DriverData_p, &szSequence, &SequenceLength);
                    break;
                }

            default:
                DBGT_PTRACE("CamCtrl: Parse error");
                ok = FALSE;
                break;
            }
        }
        else if (CurrentChar == '%')
        {
            // Write value of specified parameter on the I2C bus
            // Syntax: %[parameter]
            // Example: %0 (writes value of parameter 0)
            SeqData_p->WriteBuffer[SeqData_p->WriteBufferIndex++] = SeqData_p->ParamArray_p[ConvertDigit(ReadNextChar(&szSequence, &SequenceLength))];

        }
        else if (CurrentChar == '!')
        {
            // Execute macro parameter
            // Syntax: ![parameter]
            // Example: !0 (executes value of macro parameter 0)
            t_uint8 ParamNo;
            ParamNo = ConvertDigit(ReadNextChar(&szSequence, &SequenceLength));
            if (ParamNo < SeqData_p->MacroData.NumParams)
            {
                ok = ExecuteSequencePart(DriverData_p,
                        SeqData_p->MacroData.Param[ParamNo].String_p,
                        (int)SeqData_p->MacroData.Param[ParamNo].Length,
                        SeqData_p, InBuffer_pp, InBufferSize_p);
            }
        }
        else if (CurrentChar == '#')
        {
            /*****************************************************************************
             * Macro expansion.
             * Notes on the implementation:
             * 1) If parameters are entered in parenthesis after the macro name,
             *    these are passed to the execution of the macro in MacroParam_p.
             * 2) The parameters are then "inserted" (i.e. executed) in the macro
             *    sequence when the control character "!" appears.
             * 3) If the sequence currently executing is a macro in itself and
             *    contains parameters, the parameter ids have to be manipulated
             *    to avoid overlaps. The new parameters are appended to the existing
             *    parameter array and the parameter ids in the macro to expand are changed
             *    to appropriate higher numbers.
             * 4) After expanding the macro, the parameter ids in it are restored to
             *    the right values.
             *
             * Example: seq=#A(01, 02); A=#B(CC !0, DD !1); B=FF !1 EE !0; will expand like this:
             * 1) Execute A with parameters (01, 02)
             * 2) Change macro B to: FF !3 EE !2
             * 3) Execute B with parameters (01, 02, CC !0, DD !1)
             * 4) Execution: FF !3 EE !2 => FF DD !1 EE CC !0 => FF DD 02 EE CC 01
             * 5) Restore macro B to: FF !1 EE !0
             *****************************************************************************/
            int ItemLength;
            int ParamLength;
            t_uint8* MacroSequence_p;
            int MacroLength;
            int Id;
            t_uint8  OldParams;

            int FoundEnd;
            int  ParamChar;
            int  NestedLevel;

            ItemLength = (int)GetNextDelimiterPos(szSequence, ';');
            ParamLength = (int)GetNextDelimiterPos(szSequence, '(');
            OldParams = SeqData_p->MacroData.NumParams;

            if (ParamLength < ItemLength)   //Parameters are supplied
            {
                Id = ConvertNumber(szSequence, ParamLength, 16);
                szSequence += ParamLength + 1;
                SequenceLength -= (ParamLength + 1);

                //Fill in parameters at the next available position in the macro parameter array
                FoundEnd = FALSE;
                ParamLength = 0;
                NestedLevel = 0;
                while (!FoundEnd && (SequenceLength >= ParamLength))
                {
                    ParamChar = *(szSequence + ParamLength++);
                    if (ParamChar == ')')
                    {
                        FoundEnd = (int)(NestedLevel-- == 0);
                    }
                    if (((ParamChar == ',') && (NestedLevel == 0)) || FoundEnd)
                    {
                        SeqData_p->MacroData.Param[SeqData_p->MacroData.NumParams].String_p = szSequence;
                        SeqData_p->MacroData.Param[SeqData_p->MacroData.NumParams].Length = (int)(ParamLength - 1);
                        SeqData_p->MacroData.NumParams++;
                        szSequence += ParamLength;
                        SequenceLength -= ParamLength;
                        ParamLength = 0;
                    }
                    else if (ParamChar == '(')
                    {
                        NestedLevel++;
                    }
                }
            }
            else    //No parameters
            {
                Id = ConvertNumber(szSequence, ItemLength, 16);
                szSequence += ItemLength + 1;
                SequenceLength -= (ItemLength + 1);
            }

            if (SeqData_p->MacroData.NumParams == 16)  //Overflow!
            {
                DBGT_PTRACE("ExecuteSequencePart: Error! Macro parameter overflow (>15)");
                ok = FALSE;
            }
            if (ok && (FindItemById(DriverData_p, Id, &MacroSequence_p, &MacroLength)))
            {
                UpdateParameterId(MacroSequence_p, MacroLength - 1, (t_sint8)OldParams);
                ok = ExecuteSequencePart(DriverData_p, MacroSequence_p, (int)MacroLength, SeqData_p, InBuffer_pp, InBufferSize_p);
                UpdateParameterId(MacroSequence_p, MacroLength - 1, (t_sint8)-OldParams);
            }
            else
            {
                DBGT_PTRACE("ExecuteSequencePart: Error!Could not find macro definition");
                ok = FALSE;
            }
            SeqData_p->MacroData.NumParams = OldParams;
        }
        else if (CurrentChar == '[')
        {
            int Times;
            int RegNo;
            int ItemLength;


            RegNo = GetRegisterIndex(ReadNextChar(&szSequence, &SequenceLength));
            Times = (ConvertDigit(ReadNextChar(&szSequence, &SequenceLength))<<4)+
                ConvertDigit(ReadNextChar(&szSequence, &SequenceLength));
            ItemLength = (int)GetNextDelimiterPos(szSequence, ']');

            while (Times > 0 && DriverData_p->Registers[RegNo] != 0 && ok)
            {
                ok = ExecuteSequencePart(DriverData_p,
                        szSequence,
                        ItemLength,
                        SeqData_p, InBuffer_pp, InBufferSize_p);
                Times--;
            }

            szSequence += ItemLength + 1;
            SequenceLength -= (ItemLength + 1);

        }
        else if (CurrentChar != 0)
        {
            DBGT_PTRACE("CamCtrl: Parse error %c not valid",CurrentChar);
            ok = FALSE;
        }

        // Check write buffer
        if (SeqData_p->WriteBufferIndex >= CAMCTRL_WRITEBUFFER_SIZE)
        {
            DBGT_PTRACE("CamCtrl: Error! Forced to flush I2C write buffer (%d bytes)", SeqData_p->WriteBufferIndex);
            (void) I2C_Write(
                    DriverData_p->I2C_WriteAddress,
                    SeqData_p->WriteBufferIndex,
                    SeqData_p->WriteBuffer,
                    1,
                    1);
            SeqData_p->WriteBufferIndex = 0;
            ok = FALSE;
        }

    }

    SeqData_p->MacroData.Depth--;
    return ok;
}

/*************************************************************************
 * SeqCommand functions
 **************************************************************************/
int Camera_Ctrllib::SeqCommand_a(
        CamCtrl_DriverData_t* DriverData_p,
        t_uint8** szSequence_p,
        int* SequenceLength_p)
{
    // Changes the I2C address
    // Syntax: $a[new address]
    // Example: $a1F (Set the new I2C module address to 1F)
    t_uint8 Address;

    DBGT_PTRACE("CamCtrl: Current I2C address = %X", DriverData_p->I2C_WriteAddress);

    Address = (t_uint8)((ConvertDigit(ReadNextChar(szSequence_p, SequenceLength_p)) << 4) +
            (ConvertDigit(ReadNextChar(szSequence_p, SequenceLength_p))));
    DriverData_p->I2C_WriteAddress = Address;
    DriverData_p->I2C_ReadAddress = Address + 1;

    {
        int error=0;

        i2c_deinit(i2c_handle);

        init_param.adapter = TEST_ADAPTER;
        init_param.slave_addr = DriverData_p->I2C_WriteAddress;
        error = i2c_init(&init_param,&i2c_handle);
        if(error)
        {
            i2c_deinit(i2c_handle);
            return FALSE;
        }
    }

    DBGT_PTRACE("CamCtrl: New I2C address = %X", DriverData_p->I2C_WriteAddress);

    return TRUE;
}

int Camera_Ctrllib::SeqCommand_b(
        CamCtrl_DriverData_t* DriverData_p,
        t_uint8** szSequence_p,
        int* SequenceLength_p,
        CamCtrl_SequenceData_t* SeqData_p)
{
    // Set the specified bits (param or constant) of a specified register and store
    //  the result back to the specified register.
    // Syntax: $b[register][value/parameter][bitposition][numberofbits]
    // Example: $b0 03 2 2 (sets bits 2 and 3 to 11 of the contents in register 0)
    //          $b1 %0 1 3 (sets bits 1,2 and 3 to value of parameter 0 of the contents of register 1)
    t_uint8 Register;
    t_uint8 ValueOut;
    t_uint8 BitPosition;
    t_uint8 InputChar;
    t_uint8 ValueIn;
    t_uint8 NumberOfBits;
    t_uint8 Mask;

    Register = GetRegisterIndex(ReadNextChar(szSequence_p, SequenceLength_p));

    InputChar = ReadNextChar(szSequence_p, SequenceLength_p);
    if (InputChar == '%')
    {
        // Use parameter
        ValueIn = SeqData_p->ParamArray_p[ConvertDigit(ReadNextChar(szSequence_p, SequenceLength_p))];
    }
    else
    {
        // Use constant
        ValueIn = (t_uint8)((ConvertDigit(InputChar) << 4) + (ConvertDigit(ReadNextChar(szSequence_p, SequenceLength_p))));
    }

    BitPosition = ConvertDigit(ReadNextChar(szSequence_p, SequenceLength_p));
    NumberOfBits = ConvertDigit(ReadNextChar(szSequence_p, SequenceLength_p));

    ValueOut = (t_uint8)DriverData_p->Registers[Register];

    // Construct mask like 00011000
    Mask = (t_uint8)((0xff >> (8 - NumberOfBits)) << BitPosition);
    // Reconstruct the mask like 11100111
    Mask = (t_uint8)~Mask;

    ValueIn = (t_uint8)((ValueIn << BitPosition) + (Mask & ValueOut));

    //Don't affect higher bits when updating register
    DriverData_p->Registers[Register] =  (DriverData_p->Registers[Register] & 0xFFFFFF00) + ValueIn;
    return TRUE;

}

int Camera_Ctrllib::SeqCommand_c(
        CamCtrl_DriverData_t* DriverData_p,
        t_uint8** szSequence_p,
        int* SequenceLength_p,
        CamCtrl_SequenceData_t* SeqData_p)
{
    // If the specified register contains the specified value,
    // the sequence up to $cl or $cx is executed, otherwise
    // the sequence beginning after $cl is executed (if any)
    // Syntax: $c[register][conditional value/parameter][true sequence]$cl[false sequence]$cx
    // Note: It is not necessary to have a "false sequence"; in that case the $cl marker
    //       should be left out
    // Example: $c0 01 01 02 $cl 03 04 $cx
    //          (if register 0 contains 01, write 01 02, else write 03 04)
    //          $c1 %0 FF FE $cx
    //          (if register 1 contains the value of parameter 0, write FF FE)
    //          $c1.10 1 FF FE $cx
    //          (if bit 16 of register 1 is set to 1 write FF FE)

    t_uint8 Register;
    t_uint8 InputChar;
    int ConditionalValue;

    InputChar = ReadNextChar(szSequence_p, SequenceLength_p);
    if (InputChar == 'x')
    {
        //Do nothing; just end marker
    }
    else if (InputChar == 'l')
    {
        //Skip false part
        int FoundMarker = FALSE;
        int Level = 0;
        while (!FoundMarker && *SequenceLength_p > 0)
        {
            InputChar = ReadNextChar(szSequence_p, SequenceLength_p);
            if (InputChar == '$')
            {
                InputChar = ReadNextChar(szSequence_p, SequenceLength_p);
                if (InputChar == 'c')
                {
                    InputChar = ReadNextChar(szSequence_p, SequenceLength_p);
                    if (InputChar == 'x')
                    {
                        FoundMarker = (Level-- == 0);
                    }
                    else if (InputChar != 'l')
                    {
                        Level++;
                    }
                }
            }
        }
        if (!FoundMarker)
        {
            DBGT_PTRACE("Condition not matched, raised error condition");
            return FALSE;
        }
    }
    else
    {
        //New conditional statement
        CompType_t Compare = COMP_TYPE_EQUAL;
        int SkipTruePart; // If condition is true this should be false.
        if (InputChar == 'g' )
        {
            Compare = COMP_TYPE_GREATER_THAN;
            InputChar = ReadNextChar(szSequence_p, SequenceLength_p);
        }
        else if (InputChar == 's')
        {
            Compare = COMP_TYPE_SMALLER_THAN;
            InputChar = ReadNextChar(szSequence_p, SequenceLength_p);
        }


        Register = GetRegisterIndex(InputChar);
        InputChar = ReadNextChar(szSequence_p, SequenceLength_p);
        if (InputChar == '%')
        {
            // Use parameter
            ConditionalValue = SeqData_p->ParamArray_p[ConvertDigit(ReadNextChar(szSequence_p, SequenceLength_p))];
        }
        else if (InputChar == 'r')
        {
            t_uint8 CondRegister = GetRegisterIndex(ReadNextChar(szSequence_p, SequenceLength_p));
            ConditionalValue = DriverData_p->Registers[CondRegister];
        }
        else if (InputChar == '.')
        {
            // Read bit position and bit value
            t_uint8 BitNo = (t_uint8) ((ConvertDigit(ReadNextChar(szSequence_p, SequenceLength_p)) << 4) +
                    ConvertDigit(ReadNextChar(szSequence_p, SequenceLength_p)));
            t_uint8 BitValue = ConvertDigit(ReadNextChar(szSequence_p, SequenceLength_p));
            Compare = COMP_TYPE_BIT_EQUAL;
            ConditionalValue = (((t_uint8) (DriverData_p->Registers[Register] >> BitNo) & 1) != BitValue);
        }
        else
        {
            // Use constant
            ConditionalValue = (t_uint8)((ConvertDigit(InputChar) << 4) + (ConvertDigit(ReadNextChar(szSequence_p, SequenceLength_p))));
        }

        switch (Compare)
        {
        case COMP_TYPE_EQUAL:
            SkipTruePart = DriverData_p->Registers[Register] != ConditionalValue;
            break;

        case COMP_TYPE_GREATER_THAN:
            SkipTruePart = DriverData_p->Registers[Register] <= ConditionalValue;
            break;

        case COMP_TYPE_SMALLER_THAN:
            SkipTruePart = DriverData_p->Registers[Register] >= ConditionalValue;
            break;
        case COMP_TYPE_BIT_EQUAL:
            SkipTruePart = (int) ConditionalValue;
            break;
        default:
            SkipTruePart = FALSE;
            DBGT_PTRACE("CamCtrl: Condition not found");
            break;
        }
        if (SkipTruePart)
        {
            //Skip true part
            int FoundMarker = FALSE;
            int Level = 0;
            while (!FoundMarker)
            {
                InputChar = ReadNextChar(szSequence_p, SequenceLength_p);
                if (InputChar == '$')
                {
                    InputChar = ReadNextChar(szSequence_p, SequenceLength_p);
                    if (InputChar == 'c')
                    {
                        InputChar = ReadNextChar(szSequence_p, SequenceLength_p);
                        if (InputChar == 'x')
                        {
                            FoundMarker = (Level-- == 0); //Set marker if no else part
                        }
                        else if (InputChar == 'l')
                        {
                            FoundMarker = (Level == 0);
                        }
                        else
                        {
                            Level++;
                        }
                    }
                }
            }
        }
    }
    return TRUE;
}

int Camera_Ctrllib::SeqCommand_d(
        t_uint8** szSequence_p,
        int* SequenceLength_p)
{
    // Delay for specified amount of milliseconds before continuing sequence
    // Syntax: $d[time] (time should consist of four characters)
    // Example: $d0050 (delay for 80 ms)
    int DelayTime = ConvertNumber(*szSequence_p, 4, 16);
    *szSequence_p += 4;
    *SequenceLength_p -= 4;
    DoDelay(DelayTime);

    return TRUE;
}

int Camera_Ctrllib::SeqCommand_e(
        CamCtrl_DriverData_t* DriverData_p,
        t_uint8** szSequence_p,
        int* SequenceLength_p,
        CamCtrl_SequenceData_t* SeqData_p)
{
    // Check for error
    // Syntax: $e[register][value/parameter]
    // Example: $e0 01 (if register 0 doesn't contain 01, an error has occurred)
    //          $e1 %0 (if register 1 doesn't contain the value of parameter 0, an error has occurred)
    t_uint8 Register;
    t_uint8 InputChar;
    t_uint8 ValueIn;

    Register = ConvertDigit(ReadNextChar(szSequence_p, SequenceLength_p));
    InputChar = ReadNextChar(szSequence_p, SequenceLength_p);
    if (InputChar == '%')
        // Use parameter
        ValueIn = SeqData_p->ParamArray_p[ConvertDigit(ReadNextChar(szSequence_p, SequenceLength_p))];
    else
        // Use constant
        ValueIn = (t_uint8)((ConvertDigit(InputChar) << 4) + (ConvertDigit(ReadNextChar(szSequence_p, SequenceLength_p))));

    if ((t_uint8)DriverData_p->Registers[Register] != ValueIn)
    {
        DBGT_PTRACE("ExecuteSequencePart: Sequence raised error condition");
        return FALSE;
    }
    return TRUE;
}

int Camera_Ctrllib::SeqCommand_f(
        CamCtrl_DriverData_t* DriverData_p,
        CamCtrl_SequenceData_t* SeqData_p)
{
    // Flush I2C buffer
    // Syntax: $f
    int ok = TRUE;
    if (SeqData_p->WriteBufferIndex > 0) {
        DBGT_PDEBUG("SeqCommand_f: Write %d byte(s) to I2C", SeqData_p->WriteBufferIndex);
        if(SeqData_p->WriteBufferIndex == 4){
            uint16_t data0 = SeqData_p->WriteBuffer[0];
            data0 = (data0<<8); data0 += SeqData_p->WriteBuffer[1];
            uint16_t data1 = SeqData_p->WriteBuffer[2];
            data1 = (data1<<8); data1 += SeqData_p->WriteBuffer[3];
            DBGT_PDEBUG("data %4x, %4x",data0,data1);
        }
        HAL_Gr_I2C_ErrorStatus_t ErrorStatus = I2C_Write(
                DriverData_p->I2C_WriteAddress,
                SeqData_p->WriteBufferIndex,
                SeqData_p->WriteBuffer,
                CAMCTRL_I2C_RETRIES,
                CAMCTRL_I2C_REPEAT_RETRIES);
        ok = (ErrorStatus == HAL_GR_I2C_SUCCESS);
        SeqData_p->WriteBufferIndex = 0;
    }
    return ok;
}


int Camera_Ctrllib::SeqCommand_m(
        CamCtrl_DriverData_t* DriverData_p,
        t_uint8** szSequence_p,
        int* SequenceLength_p)
{
    // Moves (i.e. copies) a value from one register to another
    // Syntax: $m[to register][from register]
    // Example: $m0 1 (Set the value of register 0 to value of register 1)
    t_uint8 ToReg = GetRegisterIndex(ReadNextChar(szSequence_p, SequenceLength_p));
    t_uint8 FromReg = GetRegisterIndex(ReadNextChar(szSequence_p, SequenceLength_p));
    DriverData_p->Registers[ToReg] = DriverData_p->Registers[FromReg];

    return TRUE;

}

int Camera_Ctrllib::SeqCommand_p(
        CamCtrl_DriverData_t* DriverData_p,
        t_uint8** szSequence_p,
        int* SequenceLength_p)
{
    // Set a register to the value of a property
    // Syntax: $p[register][property number]
    // Example: $p0 00 (Set register 0 to the value of property 0 (auto exposure))
    t_uint8 Register;
    t_uint8 PropertyNumber;

    Register = GetRegisterIndex(ReadNextChar(szSequence_p, SequenceLength_p));

    PropertyNumber = (t_uint8)((ConvertDigit(ReadNextChar(szSequence_p, SequenceLength_p)) << 4) +
            (ConvertDigit(ReadNextChar(szSequence_p, SequenceLength_p))));

    if (PropertyNumber < CAMCTRL_MAX_NUMBER_PROPERTIES)
    {
        if (DriverData_p->PropertyProperties[PropertyNumber].LastIsValid)
        {
            DriverData_p->Registers[Register] = DriverData_p->PropertyProperties[PropertyNumber].Last;
        }
        else
        {
            DriverData_p->Registers[Register] = DriverData_p->PropertyProperties[PropertyNumber].Default;
        }
    }
    return TRUE;
}

int Camera_Ctrllib::SeqCommand_r(
        CamCtrl_DriverData_t* DriverData_p,
        t_uint8** szSequence_p,
        int* SequenceLength_p,
        CamCtrl_SequenceData_t* SeqData_p,
        t_uint8** InBuffer_pp,
        int* InBufferSize_p)
{
    // Read bytes from the I2C bus into specified registers.
    // Syntax: $r[Number of bytes] [register 1] [register 2] ...
    // Example: $r2 0 1 (reads two bytes and places it in register 0 and 1)
    int ok;
    t_uint8 i;
    t_uint8 NoBytes;
    t_uint8 ReadValues[16];
    t_uint8 reg;
    HAL_Gr_I2C_ErrorStatus_t ErrorStatus;

    NoBytes = ConvertDigit(ReadNextChar(szSequence_p, SequenceLength_p));

    // If we have something in the write buffer, we assume that this is
    // a read command, and use the combined write/read command
    if (SeqData_p->WriteBufferIndex > 0)
    {
        DBGT_PTRACE("ExecuteSequencePart: Read %d byte(s) from I2C", NoBytes);
        ErrorStatus = I2C_Read(
                DriverData_p->I2C_WriteAddress,
                SeqData_p->WriteBufferIndex,
                SeqData_p->WriteBuffer,
                DriverData_p->I2C_ReadAddress,
                NoBytes,
                CAMCTRL_I2C_RETRIES,
                CAMCTRL_I2C_REPEAT_RETRIES,
                ReadValues);
        ok = (ErrorStatus == HAL_GR_I2C_SUCCESS);
        SeqData_p->WriteBufferIndex = 0;
    }
    else
    {
        //Nothing in write buffer, so issue simple read command
        DBGT_PTRACE("ExecuteSequencePart: SimpleRead %d byte(s) from I2C", NoBytes);
        ErrorStatus = I2C_SimpleRead(
                DriverData_p->I2C_WriteAddress,
                NoBytes,
                CAMCTRL_I2C_RETRIES,
                CAMCTRL_I2C_REPEAT_RETRIES,
                ReadValues);
        ok = (ErrorStatus == HAL_GR_I2C_SUCCESS);
    }

    if (ok)
    {
        i = 0;
        reg = ReadNextChar(szSequence_p, SequenceLength_p);
        while (i < NoBytes)
        {
            DBGT_PTRACE("Byte %d:0x%x", i, ReadValues[i]);
            if (reg == 'x')
            {
                // Special case: store read values in buffer instead of internal registers
                if ( (InBuffer_pp) != NULL && InBufferSize_p != NULL && (*InBufferSize_p) > 0 )
                {
                    (**InBuffer_pp) = ReadValues[i];
                    (*InBuffer_pp)++;
                    (*InBufferSize_p)--;
                }
            }
            else if (reg == 'y')
            {
                // Special case: store read values in buffer instead of internal registers
                if ( (InBuffer_pp) != NULL && InBufferSize_p != NULL && (*InBufferSize_p) > 0 )
                {
                    (**InBuffer_pp) = ReadValues[i];
                    (*InBuffer_pp)++;
                    (*InBufferSize_p)--;
                }
                // Special case: 'y' is used to read one byte to the buffer, while 'x' reads the entire stream.
                if ((i+1) < NoBytes)
                {
                    reg = ReadNextChar(szSequence_p, SequenceLength_p);
                }
            }
            else
            {
                DriverData_p->Registers[GetRegisterIndex(reg)] = ReadValues[i];
                if ((i+1) < NoBytes)
                {
                    reg = ReadNextChar(szSequence_p, SequenceLength_p);
                }
            }
            i++;
        }
    }
    return ok;
}

int Camera_Ctrllib::SeqCommand_s(
        CamCtrl_DriverData_t* DriverData_p,
        t_uint8** szSequence_p,
        int* SequenceLength_p,
        CamCtrl_SequenceData_t* SeqData_p)
{
    // Set specified register to specified value
    // Syntax: $s[register][value/parameter]
    // Example: $s1 ff (sets register 1 to 0xff)
    //          $s0 %0 (sets register 0 to value of parameter 0)
    //          $s0.0a 1 (sets bit 10 in register 0 to 1)
    t_uint8 Register;
    t_uint8 InputChar;
    t_uint8 ValueIn;

    Register = GetRegisterIndex(ReadNextChar(szSequence_p, SequenceLength_p));

    InputChar = ReadNextChar(szSequence_p, SequenceLength_p);
    if (InputChar == '.')
    {
        t_uint8 BitNo = (t_uint8) ((ConvertDigit(ReadNextChar(szSequence_p, SequenceLength_p)) << 4) +
                ConvertDigit(ReadNextChar(szSequence_p, SequenceLength_p)));

        if (ReadNextChar(szSequence_p, SequenceLength_p) == '1')
        {
            DriverData_p->Registers[Register] |= 1 << BitNo;
        }
        else
        {
            DriverData_p->Registers[Register] &= ~((t_uint8)(1 << BitNo));
        }
    }
    else
    {
        if (InputChar == '%')
        {
            // Use parameter
            ValueIn = SeqData_p->ParamArray_p[ConvertDigit(ReadNextChar(szSequence_p, SequenceLength_p))];
        }
        else
        {
            // Use constant
            ValueIn = (t_uint8)((ConvertDigit(InputChar) << 4) + (ConvertDigit(ReadNextChar(szSequence_p, SequenceLength_p))));
        }
        DriverData_p->Registers[Register] = ValueIn;
    }
    return TRUE;
}

int Camera_Ctrllib::SeqCommand_t(
        t_uint8** szSequence_p,
        int* SequenceLength_p,
        CamCtrl_SequenceData_t* SeqData_p)
{
    // Set time to delay after sequence (delay time to pass to calling function)
    // Syntax: $t[time in ms (decimal value)] (time should consist of four characters)
    // Example: $t0200 (Set delay time to 512 ms)
    *SeqData_p->DelayTime_p = ConvertNumber(*szSequence_p, 4, 16);
    *szSequence_p += 4;
    *SequenceLength_p -= 4;

    return TRUE;
}

int Camera_Ctrllib::SeqCommand_u(
        CamCtrl_DriverData_t* DriverData_p,
        t_uint8** szSequence_p,
        int* SequenceLength_p,
        CamCtrl_SequenceData_t* SeqData_p)
{
    // Update a property, i.e. execute the sequence for
    // setting the property again with the last cached value
    // Syntax: $u[property number (two digit hexadecimal)]
    // Example: $u05 (Update property 5 (JPEG compression level))
    t_uint8 PropertyNumber;
    int PropValue;
    int Delay;
    int ok = TRUE;

    PropertyNumber = (t_uint8)((ConvertDigit(ReadNextChar(szSequence_p, SequenceLength_p)) << 4) +
            (ConvertDigit(ReadNextChar(szSequence_p, SequenceLength_p))));

    if (PropertyNumber < CAMCTRL_MAX_NUMBER_PROPERTIES)
    {
        if (DriverData_p->PropertyProperties[PropertyNumber].LastIsValid)
        {
            PropValue = DriverData_p->PropertyProperties[PropertyNumber].Last;
        }
        else
        {
            PropValue = DriverData_p->PropertyProperties[PropertyNumber].Default;
        }
        // Flush write buffer
        if (SeqData_p->WriteBufferIndex > 0)
        {
            DBGT_PTRACE("SeqCommand_u: Write %d byte(s) to I2C", SeqData_p->WriteBufferIndex);
            HAL_Gr_I2C_ErrorStatus_t ErrorStatus = I2C_Write(
                    DriverData_p->I2C_WriteAddress,
                    SeqData_p->WriteBufferIndex,
                    SeqData_p->WriteBuffer,
                    CAMCTRL_I2C_RETRIES,
                    CAMCTRL_I2C_REPEAT_RETRIES);
            ok = (ErrorStatus == HAL_GR_I2C_SUCCESS);
            SeqData_p->WriteBufferIndex = 0;
        }
        if (ok)
        {
            ok = Camera_CtrlLib_SetProperty(DriverData_p,
                    (HAL_Gr_Camera_Property_t)PropertyNumber,
                    PropValue,
                    &Delay);
            DoDelay(Delay);
        }
    }
    return ok;
}

int Camera_Ctrllib::SeqCommand_w(
        CamCtrl_DriverData_t* DriverData_p,
        t_uint8** szSequence_p,
        int* SequenceLength_p,
        CamCtrl_SequenceData_t* SeqData_p)
{
    // Write content of specified register to the I2C buffer (or actually a buffer..)
    // Syntax: $w[register]
    // Example: $w1 (writes content of register 1)

    t_uint8 Register = GetRegisterIndex(ReadNextChar(szSequence_p, SequenceLength_p));
    SeqData_p->WriteBuffer[SeqData_p->WriteBufferIndex++] = (t_uint8)DriverData_p->Registers[Register];

    return TRUE;
}

int Camera_Ctrllib::SeqCommand_z(
        CamCtrl_DriverData_t* DriverData_p,
        t_uint8** szSequence_p,
        int* SequenceLength_p)
{
    // Set a register to the current size id
    // Syntax: $z[register]
    // Example: $z1 (Set register 1 to the size id)
    t_uint8 Register = GetRegisterIndex(ReadNextChar(szSequence_p, SequenceLength_p));
    DriverData_p->Registers[Register] = DriverData_p->CurrentSizeId;


    return TRUE;
}

int Camera_Ctrllib::SeqCommand_lt(
        CamCtrl_DriverData_t* DriverData_p,
        t_uint8** szSequence_p,
        int* SequenceLength_p)
{
    // Shifts the specified register to the left
    // Syntax: $<[register][no of bits]
    // Example: $<1 2 (Shift register 1 two steps to the left)
    t_uint8 Register = GetRegisterIndex(ReadNextChar(szSequence_p, SequenceLength_p));
    DriverData_p->Registers[Register] <<= ConvertDigit(ReadNextChar(szSequence_p, SequenceLength_p));

    return TRUE;
}

int Camera_Ctrllib::SeqCommand_gt(
        CamCtrl_DriverData_t* DriverData_p,
        t_uint8** szSequence_p,
        int* SequenceLength_p)
{
    // Shifts the specified register to the right
    // Syntax: $>[register][no of bits]
    // Example: $>1 2 (Shift register 1 two steps to the right)
    t_uint8 Register = GetRegisterIndex(ReadNextChar(szSequence_p, SequenceLength_p));
    DriverData_p->Registers[Register] >>= ConvertDigit(ReadNextChar(szSequence_p, SequenceLength_p));

    return TRUE;
}

int Camera_Ctrllib::SeqCommand_aritm(
        t_uint8 op, CamCtrl_DriverData_t* DriverData_p,
        t_uint8** szSequence_p,
        int* SequenceLength_p,
        CamCtrl_SequenceData_t* SeqData_p)
{
    // Aritmethic operations on a specified register
    // Syntax: $*[register][factor (constant, register or parameter)]
    // Example: $*1 03 (Multiply register 1 by 3)
    //          $*1 r2 (Multiply register 1 by the contents of register 2)
    //          $*1 %0 (Multiply register 1 by the contents of parameter 0)
    t_uint8 Register;
    t_uint8 InputChar;
    int ValueIn;

    Register = GetRegisterIndex(ReadNextChar(szSequence_p, SequenceLength_p));
    InputChar = ReadNextChar(szSequence_p, SequenceLength_p);
    if (InputChar == '%')
    {
        // Use parameter
        ValueIn = SeqData_p->ParamArray_p[ConvertDigit(ReadNextChar(szSequence_p, SequenceLength_p))];
    }
    else if (InputChar == 'r')
    {
        // Use register
        ValueIn = DriverData_p->Registers[GetRegisterIndex(ReadNextChar(szSequence_p, SequenceLength_p))];
    }
    else
    {
        // Use constant
        ValueIn = (t_uint8)((ConvertDigit(InputChar) << 4) + (ConvertDigit(ReadNextChar(szSequence_p, SequenceLength_p))));
    }

    switch (op)
    {
    case '*':
        DriverData_p->Registers[Register] *= ValueIn;
        break;
    case '/':
        DriverData_p->Registers[Register] /= ValueIn;
        break;
    case '+':
        DriverData_p->Registers[Register] += ValueIn;
        break;
    case '-':
        DriverData_p->Registers[Register] -= ValueIn;
        break;
    default:
        break;
    }

    return TRUE;
}

int Camera_Ctrllib::SeqCommand_pr(
        CamCtrl_DriverData_t* DriverData_p,
        t_uint8** szSequence_p,
        int* SequenceLength_p)
{
    // Print a message via RS232.
    // Syntax: $" <Message to be printed> "
    // Special characters: %y, where y is a hexadecimal number between 0 and f
    //                     will print the current value of the internal
    //                     register 0xy.
    //                     %tr will register a new timestamp and print "Time Reg"
    //                     %td will print time elapsed since the last timestamp
    t_uint8 CurrentChar;
    int BytesInBufferSoFar;

    char * BufferToWrite = new char[CAMCTRL_SETUP_FILE_PRINTF_BUFFER_SIZE + CAMCTRL_MAX_CHAR_PER_PASS];

    if (BufferToWrite == NULL)
    {
        DBGT_PTRACE("r_hal_gr_camera_ctrllib.c: Failed to allocate %d bytes for printf command", CAMCTRL_SETUP_FILE_PRINTF_BUFFER_SIZE);
        return FALSE;
    }
    BufferToWrite[0] = 0;

    BytesInBufferSoFar = 0;
    while (((CurrentChar = ReadNextChar(szSequence_p, SequenceLength_p)) != '"') && (BytesInBufferSoFar <= CAMCTRL_SETUP_FILE_PRINTF_BUFFER_SIZE))
    {
        if (CurrentChar == '%')
        {
            CurrentChar = ReadNextChar(szSequence_p, SequenceLength_p);
            // If the character immediately after the %-sign is between
            // 0 and 9 or between a and s, then we should print the contents
            // of the corresponding internal register. We have 32 registers,
            // so in a perfect world we would also use t, u, and v, but it is
            // not possible because the t-character is already in use for time
            // stamp. Therefore, in the specification we'll write that there are
            // only 24 internal registers (don't want to include o, which easily
            // can be confused by a 0). However, for backwards compatibility
            // we'll leave the actual number registers at 32.
            if (IsRegister(CurrentChar, NULL))
            {
                BytesInBufferSoFar = sprintf(BufferToWrite, "%s0x%X", BufferToWrite, DriverData_p->Registers[GetRegisterIndex(CurrentChar)]);
            }
            else if ((CurrentChar == 't') || (CurrentChar == 'T'))
            {
                CurrentChar = ReadNextChar(szSequence_p, SequenceLength_p);
                if ((CurrentChar == 'r') || (CurrentChar == 'R'))
                {
                    BytesInBufferSoFar = sprintf(BufferToWrite, "%s%s", BufferToWrite, "Time Reg");
                }
                else if ((CurrentChar == 'd') || (CurrentChar == 'D'))
                {
                    BytesInBufferSoFar = sprintf(BufferToWrite, "%s", BufferToWrite);
                }
            }
        }
        else
        {
            BytesInBufferSoFar = sprintf(BufferToWrite, "%s%c", BufferToWrite, CurrentChar);
        }
    }

    if (BytesInBufferSoFar > CAMCTRL_SETUP_FILE_PRINTF_BUFFER_SIZE)
    {
        DBGT_PTRACE("Camera setup file printf buffer overflow");
        while(CurrentChar != '"')
        {
            //Skip remaining characters of the print string.
            CurrentChar = ReadNextChar(szSequence_p, SequenceLength_p);
        }

    }

    DBGT_PTRACE("%s", BufferToWrite);
    delete BufferToWrite;
    BufferToWrite=NULL;

    return TRUE;
}


/*************************************************************************
 * UpdateParameterId
 **************************************************************************/
void Camera_Ctrllib::UpdateParameterId(
        t_uint8* Sequence_p,
        int Length,
        t_sint8 ValueToAdd)
{
    int NextPos;
    t_sint8 newval[2];

    NextPos = (int)GetNextDelimiterPos(Sequence_p, '!');
    while (NextPos < Length)
    {
        Sequence_p += NextPos + 1;
        Length -= (NextPos + 1);
        sprintf((char*)newval, "%x", ConvertDigit(*Sequence_p) + ValueToAdd);
        *Sequence_p = (t_uint8)newval[0];
        NextPos = (int)GetNextDelimiterPos(Sequence_p, '!');
    }
}

/*************************************************************************
 * ReadNextChar
 **************************************************************************/
t_uint8 Camera_Ctrllib::ReadNextChar(
        t_uint8** szSequence_InOut_pp,
        int* CharsLeft_Out_p)
{
    int count = *CharsLeft_Out_p;
    t_uint8* szSequence_p = *szSequence_InOut_pp;

    while (count > 0 && (*szSequence_p == '\n'))
    {
        count--;
        szSequence_p++;
    }

    *CharsLeft_Out_p = count-1;
    *szSequence_InOut_pp = szSequence_p+1;

    return *szSequence_p;
}

/*************************************************************************
 * ConvertDigit
 **************************************************************************/
t_uint8 Camera_Ctrllib::ConvertDigit(
        t_uint8 c)
{
    if (c <= '9')
        return (t_uint8)(c - '0');
    else if (c <= 'F')
        return (t_uint8)((c - 'A') + 10);
    else
        return (t_uint8)((c - 'a') + 10);
}

/*************************************************************************
 * ConvertNumber
 **************************************************************************/
int Camera_Ctrllib::ConvertNumber(
        const t_uint8* Sequence_p,
        int Length,
        t_uint8 Base)
{
    int Result = 0;
    if (Length > 0)
    {
        Result = ConvertDigit(*Sequence_p++);
        while (--Length)
        {
            Result *= Base;
            Result += ConvertDigit(*Sequence_p++);
        }
    }
    return Result;
}

/*************************************************************************
 * IsRegister
 **************************************************************************/
int Camera_Ctrllib::IsRegister(
        t_uint8 Register,
        t_uint8* RegisterIndex_p)
{
    int IsValidRegister;
    t_uint8   RegisterIndex;

    IsValidRegister = TRUE;
    RegisterIndex = 0;

    // Register 0-9  = Properties 0-9
    // Register 10-  = Properties A-S
    if (Register >= '0' && Register <= '9')
    {
        RegisterIndex = (t_uint8)(Register - '0');
    }
    else if (Register >= 'a' && Register <= 's')
    {
        RegisterIndex = (t_uint8)(Register - 'a' + 10);
    }
    else if (Register >= 'A' && Register <= 'S')
    {
        RegisterIndex = (t_uint8)(Register - 'A' + 10);
    }
    else
    {
        IsValidRegister = FALSE;
    }

    if (RegisterIndex_p != NULL)
    {
        *RegisterIndex_p = RegisterIndex;
    }

    return IsValidRegister;
}

/*************************************************************************
 * GetRegisterIndex
 **************************************************************************/
t_uint8 Camera_Ctrllib::GetRegisterIndex(
        t_uint8 Register)
{
    t_uint8   RegisterIndex;

    if (!IsRegister(Register, &RegisterIndex))
    {
        DBGT_PTRACE("CamCtrl: Illegal register specified: %d. Using register 0 instead!", Register);
        RegisterIndex = 0;
    }

    return RegisterIndex;
}

/*************************************************************************
 * ComparePartialStrings
 **************************************************************************/
int Camera_Ctrllib::ComparePartialStrings(
        const t_uint8* string1_p,
        const t_uint8* string2_p,
        const int NumCharsToCompare)
{
    int i = NumCharsToCompare;
    t_uint8* string1Copy_p = (t_uint8*) string1_p;
    t_uint8* string2Copy_p = (t_uint8*) string2_p;

    while (i--)
    {
        t_uint8 char1 = *string1Copy_p++;
        t_uint8 char2 = *string2Copy_p++;

        if (char1 != char2) {
            return FALSE;
        }

        if (char1 == 0) {
            if (char2 == 0) {
                return TRUE;
            } else {
                return FALSE;
            }
        }

        if (char2 == 0) {
            return FALSE;
        }
    }
    return TRUE;
}


/*************************************************************************
 * ComparePartialStrings
 **************************************************************************/
void Camera_Ctrllib::DoDelay(int Time)
{
    if (Time > 0)
    {
        DBGT_PTRACE("DoDelay = %d ms", Time);
        usleep(1000*Time);
    }
}

int Camera_Ctrllib::Camera_CtrlLib_Get_LensParameters(const CamCtrl_DriverData_t*  Camera_h, int * h_angle,int *v_angle, int * focal_len)
{

    CamCtrl_DriverData_t* DriverData_p = (CamCtrl_DriverData_t*)Camera_h;
    t_uint8* Sequence_p;
    int Length;
    int ok= FALSE;

    DBGT_PROLOG("ENTERED Camera_CtrlLib_Get_ViewAngle ");
    if (FindDefinition(DriverData_p, CAMCTRL_CAM_H_VIEW_ANGLE, &Sequence_p, &Length)) {
        *h_angle= (int)ConvertNumber(Sequence_p, Length, 10);
        DBGT_PDEBUG("cam interface h_angle value: %d",*h_angle);
    }
    if (FindDefinition(DriverData_p, CAMCTRL_CAM_V_VIEW_ANGLE, &Sequence_p, &Length)) {
        *v_angle= (int)ConvertNumber(Sequence_p, Length, 10);
        DBGT_PDEBUG("cam interface v_angle value: %d",*v_angle);
    }

    if (FindDefinition(DriverData_p, CAMCTRL_CAM_FOCAL_LENGTH, &Sequence_p, &Length)) {
        *focal_len= (int)ConvertNumber(Sequence_p, Length, 10);
        DBGT_PDEBUG("cam interface v_angle value: %d",*focal_len);
    }

    ok = TRUE;

EXIT:
    DBGT_EPILOG();
    return((ok));

}
int Camera_Ctrllib::Camera_CtrlLib_Get_Supported_Zoom_Resolutions(const CamCtrl_DriverData_t*  Camera_h,int *supported_zoom_preview_width,int *supported_zoom_preview_height,int *supported_zoom_picture_width,int *supported_zoom_picture_height)
{

    CamCtrl_DriverData_t* DriverData_p = (CamCtrl_DriverData_t*)Camera_h;
    t_uint8* Sequence_p;
    int Length;
    int ok= FALSE;

    DBGT_PROLOG("ENTERED Camera_CtrlLib_Get_Supported_Zoom_Resolutions ");
    if (FindDefinition(DriverData_p, CAMCTRL_MAX_ZOOM_PREVIEW_WIDTH, &Sequence_p, &Length)) {
        *supported_zoom_preview_width= (int)ConvertNumber(Sequence_p, Length, 10);
        DBGT_PDEBUG("cam interface supported zoom preview width: %d",*supported_zoom_preview_width);
    }
    if (FindDefinition(DriverData_p, CAMCTRL_MAX_ZOOM_PREVIEW_HEIGHT, &Sequence_p, &Length)) {
        *supported_zoom_preview_height= (int)ConvertNumber(Sequence_p, Length, 10);
        DBGT_PDEBUG("cam interface supported zoom preview height: %d",*supported_zoom_preview_height);
    }

    if (FindDefinition(DriverData_p, CAMCTRL_MAX_ZOOM_PICTURE_WIDTH, &Sequence_p, &Length)) {
        *supported_zoom_picture_width= (int)ConvertNumber(Sequence_p, Length, 10);
        DBGT_PDEBUG("cam interface supported zoom picture width: %d",*supported_zoom_picture_width);
    }
    if (FindDefinition(DriverData_p, CAMCTRL_MAX_ZOOM_PICTURE_HEIGHT, &Sequence_p, &Length)) {
        *supported_zoom_picture_height= (int)ConvertNumber(Sequence_p, Length, 10);
        DBGT_PDEBUG("cam interface supported zoom preview height: %d",*supported_zoom_picture_height);
    }

    ok = TRUE;

EXIT:
    DBGT_EPILOG();
    return((ok));

}

/*************************************************************************
 * I2C_Write
 **************************************************************************/
HAL_Gr_I2C_ErrorStatus_t Camera_Ctrllib::I2C_Write(
        const t_uint8                  SlaveAddress,
        const t_uint16                 BytesToTransmit,
        t_uint8 *                      Data_p,
        const t_sint8                  Retries,
        const t_sint8                  RetryRepeats)
{
    t_sint8 NbrOfRepeats = RetryRepeats;
    t_uint32 ErrorStatus = 0;

    while (NbrOfRepeats--)
    {
        t_sint8 NbrOfTries = Retries;

        while (NbrOfTries--)
        {
            ErrorStatus = i2c_write_p(i2c_handle, Data_p,    BytesToTransmit);

            if (ErrorStatus == 0)
            {
                if ((NbrOfTries != Retries - 1) || (NbrOfRepeats != RetryRepeats - 1))
                {
                    DBGT_PTRACE("I2C Write required %d attempts to succeed", (RetryRepeats * Retries) - (RetryRepeats * NbrOfRepeats) - NbrOfTries);
                }
                return HAL_GR_I2C_SUCCESS;
            }
            DoDelay(CAMCTRL_I2C_RETRY_DELAY);
        }
        // We already waited for CAMCTRL_I2C_RETRY_DELAY above
        DoDelay((CAMCTRL_I2C_REPEAT_DELAY - CAMCTRL_I2C_RETRY_DELAY));
    }
    DBGT_PTRACE("All %d I2C Write attempts failed", RetryRepeats * Retries);

    return (HAL_Gr_I2C_ErrorStatus_t)ErrorStatus;
}


/*************************************************************************
 * I2C_Read
 **************************************************************************/
HAL_Gr_I2C_ErrorStatus_t Camera_Ctrllib::I2C_Read(
        const t_uint8                  SlaveWriteAddress,
        const t_uint16                 BytesToWrite,
        const t_uint8 *          const ReadCommand_p,
        const t_uint8                  SlaveReadAddress,
        const t_uint16                 BytesToRead,
        const t_sint8                  Retries,
        const t_sint8                  RetryRepeats,
        t_uint8 *                const Dest_p)
{
    t_sint8 NbrOfRepeats = RetryRepeats;
    t_uint32 ErrorStatus = 0;
    while (NbrOfRepeats--)
    {
        t_sint8 NbrOfTries = Retries;
        while (NbrOfTries--)
        {
            ErrorStatus = i2c_read_index16(i2c_handle, ReadCommand_p[0], ReadCommand_p[1], BytesToRead, Dest_p);

            if (ErrorStatus == HAL_GR_I2C_SUCCESS)
            {
                if ((NbrOfTries != Retries - 1) || (NbrOfRepeats != RetryRepeats - 1))
                {
                    DBGT_PTRACE("I2C Read required %d attempts to succeed", (RetryRepeats * Retries) - (RetryRepeats * NbrOfRepeats) - NbrOfTries);
                }
                return HAL_GR_I2C_SUCCESS;
            }
            DoDelay(CAMCTRL_I2C_RETRY_DELAY);
        }
        // We already waited for CAMCTRL_I2C_RETRY_DELAY above
        DoDelay((CAMCTRL_I2C_REPEAT_DELAY - CAMCTRL_I2C_RETRY_DELAY));
    }
    DBGT_PTRACE("All %d I2C Read attempts failed", RetryRepeats * Retries);
    return (HAL_Gr_I2C_ErrorStatus_t)ErrorStatus;
}


/*************************************************************************
 * I2C_SimpleRead
 **************************************************************************/
HAL_Gr_I2C_ErrorStatus_t Camera_Ctrllib::I2C_SimpleRead(
        const t_uint8                  SlaveReadAddress,
        const t_uint16                 BytesToRead,
        const t_sint8                  Retries,
        const t_sint8                  RetryRepeats,
        t_uint8 *                const Dest_p)
{
    t_sint8 NbrOfRepeats = RetryRepeats;
    t_uint32 ErrorStatus = 0;
    while (NbrOfRepeats--)
    {
        t_sint8 NbrOfTries = Retries;
        while (NbrOfTries--)
        {
            ErrorStatus = i2c_read_index16(i2c_handle, 0, 0, BytesToRead, Dest_p);

            if (ErrorStatus == HAL_GR_I2C_SUCCESS)
            {
                if ((NbrOfTries != Retries - 1) || (NbrOfRepeats != RetryRepeats - 1))
                {
                    DBGT_PTRACE("I2C Simple Read required %d attempts to succeed", (RetryRepeats * Retries) - (RetryRepeats * NbrOfRepeats) - NbrOfTries);
                }
                return HAL_GR_I2C_SUCCESS;
            }
            DoDelay(CAMCTRL_I2C_RETRY_DELAY);
        }
        // We already waited for CAMCTRL_I2C_RETRY_DELAY above
        DoDelay((CAMCTRL_I2C_REPEAT_DELAY - CAMCTRL_I2C_RETRY_DELAY));
    }
    DBGT_PTRACE("All %d I2C Simple Read attempts failed", RetryRepeats * Retries);
    return (HAL_Gr_I2C_ErrorStatus_t)ErrorStatus;
}
