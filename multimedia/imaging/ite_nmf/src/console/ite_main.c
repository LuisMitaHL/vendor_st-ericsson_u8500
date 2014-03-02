/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "ite_platform.h"
#include "cli.h"
#include "ite_nmf_cmd.h"
#include "ite_darkybox_cmd.h"
#include "ite_nmf_standard_functions.h"
#include "ite_nmf_flash_functions.h"
#include "ite_nmf_sensor_tunning_functions.h"
#include "ite_nmf_ndfilter_functions.h"
#include "ite_nmf_focuscontrol_functions.h"
#include "ite_nmf_statistic_functions.h"
#include "ite_nmf_zoom_functions.h"
#include "ite_host2sensor.h"
#include "ite_nmf_nvm_functions.h"
#include "ite_nmf_stat_and_zoom_tests.h"
#include "ite_nmf_sms_tests.h"
#include "ite_nmf_headers.h"
#include "ite_ispblocks_tests.h"


#include "cm/inc/cm.h"
#include "los/api/los_api.h"
#include "ite_main.h"
#include "misc_linux.h"

extern void Init_Test_CmdList(void);
extern void Init_algos_ITECmdList(void);
extern void Init_NVM_ITECmdList(void);
extern void Init_focus_ITECmdList(void);
extern void Init_ISP_Blocks_Configure_ITECmdList(void);

#ifdef MMTE_USB_TRUE
 #include "retargetio.h"
 #pragma import(__use_no_semihosting_swi)
#endif

void dbc_assert(int a, int errorId, const char * file, int line)
{
 UNUSED(errorId);
    if (!(a))
    {
        LOS_Log("assert : file %s, line %d\n", file, line, NULL, NULL, NULL, NULL);
    }
}


char g_out_path[KlogDirectoryLentgh];
char g_trace_path[KlogDirectoryLentgh];

#ifdef TIMELOG
ts_sia_eventtimelog eventtimelog = {
                                     .grabLRinfo   ={0,{0},{0},{ISP_INF_NONE}},
                                     .grabLRerror  ={0,{0},{0},{ISP_INF_NONE}},
                                     .grabLRdebug  ={0,{0},{0},{ISP_INF_NONE}},
                                     .grabHRinfo   ={0,{0},{0},{ISP_INF_NONE}},
                                     .grabHRerror  ={0,{0},{0},{ISP_INF_NONE}},
                                     .grabHRdebug  ={0,{0},{0},{ISP_INF_NONE}},
                                     .grabBMSinfo  ={0,{0},{0},{ISP_INF_NONE}},
                                     .grabBMSerror ={0,{0},{0},{ISP_INF_NONE}},
                                     .grabBMSdebug ={0,{0},{0},{ISP_INF_NONE}},
                                     .grabBMLinfo  ={0,{0},{0},{ISP_INF_NONE}},
                                     .grabBMLerror ={0,{0},{0},{ISP_INF_NONE}},
                                     .grabBMLdebug ={0,{0},{0},{ISP_INF_NONE}},
                                     .ispctlinfo   ={0,{0},{0},{ISP_INF_NONE}},
                                     .ispctlerror  ={0,{0},{0},{ISP_INF_NONE}},
                                     .ispzoominfo  ={0,{0},{0},{ISP_INF_NONE}},
                                     .ispstatinfo  ={0,{0},{0},{ISP_INF_NONE}},
                                     .ispfocusinfo ={0,{0},{0},{ISP_INF_NONE}},                  
                                     .ispnvminfo            ={0,{0},{0},{ISP_INF_NONE}},
                                     .ispcdccinfo           ={0,{0},{0},{ISP_INF_NONE}},
                                     .isppowerinfo          ={0,{0},{0},{ISP_INF_NONE}},
                                     .ispsmsinfo            ={0,{0},{0},{ISP_INF_NONE}},
                                     .ispsensopmodeinfo     ={0,{0},{0},{ISP_INF_NONE}},
                                     .ispsenscommitinfo     ={0,{0},{0},{ISP_INF_NONE}},
                                     .ispispcommitinfo      ={0,{0},{0},{ISP_INF_NONE}},
                                     .ispfctlrw    ={0,{0},{0},{ISP_INF_NONE}},
                                  };
#endif

ts_sia_usecase usecase = {
    .sensor = 0,
    .LR_XSize = 640,
    .LR_YSize = 480,
    .LR_resolution = VGA,
    .LR_GrbFormat = GRBFMT_R5G6B5,
    .HR_XSize = 640,
    .HR_YSize = 480,
    .HR_resolution = VGA,
    .HR_GrbFormat = GRBFMT_YUV420_MB_SEMIPLANAR_DECIMATED,
    .BMS_XSize = 640,
    .BMS_YSize = 480,
    .BMS_resolution = VGA,
    .BMS_GrbFormat = GRBFMT_RAW8,
    .BMS_Mode = BMSSTILL,
    .BMS_output = BAYERSTORE0,
    .BML_input = BAYERLOAD1,
    .BML_GrbFormat = GRBFMT_FAST_RAW8,
    .flag_backup_fw = FALSE,
    .flag_nosensor_testmode = FALSE,
    .esramGrabCacheMemHandle = 0,
    .FwTuning_handler = 0,
    .mode = MODE_UNDEFINED,
    .state = STATE_FREE,
    .MaxDZ = 8,
    .CurrentDZ = 1,
    .LR_Min_resolution = SUBQCIF,
    .HR_Min_resolution = SUBQCIF,
    .LR_Max_resolution = XGA,
    .HR_Video_Max_resolution = FULLHD,
    .HR_Still_Max_resolution = EIGHTMEGA,
    .sensormode = NORMAL_MODE,
    .HorizPos = 1,
    .BMS_woi_X = 0,
    .BMS_woi_Y = 0,
    .framerate_x100 = 0,
    .data_format = 0,
    .smiapp_powerUp_sequence = 0,
    .stripeInUse = 0,
    .stripeIndex = 0,
};

t_uint8 debug;
ts_sia_nonregselect nonregselect = {
                    {1,0,0,0,0},    //LR datapath nonreg
                    {1,1,0,0,0},    //HR datapath nonreg
                    {1,1,1,1,0},    //LRHR datapath nonreg
                    {1,0,0,0,0},    //BMS datapath nonreg
                    {1,1,0,0,0},    //BML datapath nonreg
                    {0,0,0,0,0},    //zoom nonreg
                    {0,0,0,0,0}};   //pan-tilt nonreg

char myiobuf[2048]; // patch for Trace32

extern void initNMFCallBackServices(void);
extern void NMF_CallBackThread(void *);

/// The NMF callback FIFO Identifier
t_nmf_channel nmfCallBackFifoID;


#define INIT_ATE "init.ate"

/// NMF MPC to HOST Callback message size: parameter size + 2 x 32-bits int
#define MAX_NMF_MESSAGE_SIZE 1024
#define NMF_CALL_BACK_STACK_SIZE (32*1024)
#define NMF_CM_CB_FIFO_SIZE 128


/*-----------------------------*/
/*  main                */
/*-----------------------------*/

int main(int argc, char **argv)
{

    return(ite_main(argc,argv));
}


/************************************************************************************************/

int ite_main(int argc, char **argv)
{

   int display;
   char pathname[256];
   char initfilename[256] = INIT_ATE;
   char imaging_path_value[256] = IMAGING_PATH;
   UNUSED(argv);

   /*Generate folders ite_nmf and test_results_nreg in IMAGING_PATH*/
   init_test_results_nreg(IMAGING_PATH"/ite_nmf");
   init_test_results_nreg(IMAGING_PATH"/ite_nmf/test_results_nreg");
   /*To run the script ehci copies all .ite files onto /sdcard*/
   snprintf(pathname, sizeof(pathname), IMAGING_SETUP_SCRIPT);
   system(pathname);
   LOS_Log("Get %d args\n", argc);    // workaround see M. Guene
#ifdef MMTE_USB_TRUE
   //Initializing the stdio media; endless loop is PC console is not activated
   if( NoError != InitStdioMedia(STDIO_USB_HS ,STDIO_USB_HS ) )
   {
    LOS_Log("usb init KO !!!!! \n",NULL, NULL, NULL, NULL, NULL, NULL);
    return 1;
   }
   else
   {
    LOS_Log("usb init OK \n",NULL, NULL, NULL, NULL, NULL, NULL);
   }
#endif

   initNMFCallBackServices();

   //fixe debugger limitation to 64 char per line
   setvbuf(stdin, myiobuf, _IOFBF, sizeof(myiobuf));

   // Initializes all available functions accesible thru ATE
   CLI_init();

   memset(g_out_path, 0, KlogDirectoryLentgh*sizeof (char) );

   CLI_disp_msg("Entering Init for ITE ...NMF part\n");
   Init_Nmf_ITECmdList();
   Init_DarkyBox_ITECmdList();
   Init_statistic_ITECmdList();
   Init_Flash_ITECmdList();
   Init_SENSOR_TUNNING_ITECmdList();
   Init_NDFilter_ITECmdList();
   Init_algos_ITECmdList();
   Init_focus_ITECmdList();
   Init_dz_ITECmdList();
   Init_NVM_ITECmdList();
   Init_STAT_AND_ZOOM_CmdList();
   Init_SMS_CmdList();
   Init_ISP_Blocks_Configure_ITECmdList();
   Init_UnExpectedSignalHandling();

   // sets the buffering mode of stdout to line buffering
   display = MASK_ERROR_DISPLAY | MASK_MSG_DISPLAY;
   CLI_set_display_filter(display);

   /* start command interpreter */
   CLI_disp_msg("\n\n(c) STMicroelectronics\nNOMADIK Shell - Imaging NMF Test Environment\n");
   CLI_disp_msg("Last built: %s, %s\n", __DATE__, __TIME__);

   CLI_add_alias ("IMAGING_PATH",imaging_path_value,1);
   CLI_disp_msg("\n\tIMAGING_PATH set for Linux\n");

   //Test utilisation FB
   //mle     if(DisplayStartScreen(0)) CLI_disp_msg("\nError while displaying welcome picture");

   CLI_command_loop(initfilename);

   CLI_disp_msg("bye!\n");

   return 0;
}

/* Creates Folders in /sdcard/ite_nmf */
/*Please use this function before writing a new testcase for a new FW feature*/
void init_test_results_nreg (char test_result_foldername[256])
{
 int status;
 char pathname[256];
 snprintf ( pathname,sizeof(pathname),test_result_foldername);
 status = mkdir(pathname, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

 if (status == -1)
  {
   if (EEXIST != errno)
    {
      CLI_disp_msg("\nERROR FOLDER NOT CREATED, ERRORNO:%d\n",errno);
    }
  }
 else
  {
  CLI_disp_msg("\nFOLDER %s CREATED !!\n",test_result_foldername);
  }
}

void initNMFCallBackServices(void)
{
    // Start a channel to the CM engine - must be done prior to any CM call
    CM_CreateChannel(NMF_CHANNEL_SHARED,&nmfCallBackFifoID);

    LOS_Log("initNMFCallBackServices - nmfCallBackFifoID=%d, thread_id=%d\n",
             nmfCallBackFifoID, LOS_GetCurrentId());

    // Start the thread managing the NMF MPC to HOST call backs
    LOS_ThreadCreate(NMF_CallBackThread, NULL,
             NMF_CALL_BACK_STACK_SIZE, LOS_USER_URGENT_PRIORITY,
             "NMF_MPC_2_HOST_CB_Thread");
}

void stopNMFCallBackServices()
{
    CM_CloseChannel(nmfCallBackFifoID);
}

// NMF Call Back thread function
void NMF_CallBackThread(void *fct)
 {
    char *buffer;
    void *itfref;
    UNUSED(fct);
    // Register ourself to the CB FIFO
    while(1)
    {
        if (CM_GetMessage(nmfCallBackFifoID, &itfref, &buffer, (t_bool)TRUE) == CM_OK)

    // Execute the call back
        CM_ExecuteMessage(itfref, buffer);

    }
}

void UnExpectedSignalHandler(int signo)
{	
    if(signo == ctrlc)
    {
        CLI_disp_msg("\nCtrl-c signal Called!!\n");
    }
    else
    {
        CLI_disp_msg("\nSegfault called!!\n");
    }
    ITE_stop_sia();
    ITE_Stop_Env();
    ite_nmf_exit();
	CLI_disp_msg("\nQuitting ite_nmf safely!!\n");
}	


void Init_UnExpectedSignalHandling()
{
    signal_handle(UnExpectedSignalHandler,ctrlc);
    signal_handle(UnExpectedSignalHandler,segfault);
}


#include <unistd.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "mcde_ioctls.h"

#include <nmf_test_env_image.h>

#ifdef __ARM_LINUX
//!!#define MAX(a,b) (a >= b ? a : b)
//!!#define MIN(a,b) (a < b ? a : b)

int DisplayStartScreen(int fbindex)
{

    short * frame_buffer_address;
    int fbfd = -1;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    long int screensize = 0;
    char fbname[1024];

    //#########################################################
    //Open framebuffer device
    sprintf(fbname, "/dev/fb%d", fbindex);
    fbfd = open(fbname, O_RDWR);

    if (fbfd == -1) {
        CLI_disp_msg("Error: failed to open %s.\n", fbname);
    return 1;
    }
    CLI_disp_msg("Opened %s.\n", fbname);


    //#########################################################
    //Get fixed screen information
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo)) {
        CLI_disp_msg("Error reading fixed information.\n");
    return 2;
    }

    // Get variable screen information
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
        CLI_disp_msg("Error reading variable information 1.\n");
    return 3;
    }


    vinfo.reserved[0] = 0;
    vinfo.reserved[1] = 0;
    vinfo.reserved[2] = 0;
    vinfo.xoffset = 0;
    vinfo.yoffset = 0;
    //vinfo.activate = FB_ACTIVATE_NOW;
    vinfo.activate = FB_ACTIVATE_FORCE;
    /*
     * Explicitly request 5/6/5
     */
    vinfo.bits_per_pixel = 16;
    vinfo.red.offset     = 11;
    vinfo.red.length     = 5;
    vinfo.green.offset   = 5;
    vinfo.green.length   = 6;
    vinfo.blue.offset    = 0;
    vinfo.blue.length    = 5;
    vinfo.transp.offset  = 0;
    vinfo.transp.length  = 0;

#define NUM_BUFFERS 1

    /*
     * Request NUM_BUFFERS screens (at lest 2 for page flipping)
     */
    vinfo.yres_virtual = vinfo.yres * NUM_BUFFERS;

    if (ioctl(fbfd, FBIOPUT_VSCREENINFO, &vinfo) == -1) {
        vinfo.yres_virtual = vinfo.yres;
        printf("FBIOPUT_VSCREENINFO failed, page flipping not supported");
    }

    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1)
        return -errno;


/*  mle int refreshRate = 1000000000000000LLU /
    (
     (unsigned long long )( vinfo.upper_margin + vinfo.lower_margin + vinfo.yres )
     * ( vinfo.left_margin  + vinfo.right_margin + vinfo.xres )
     * vinfo.pixclock
    );*/

/*  if (refreshRate == 0) {
        // bleagh, bad info from the driver
        refreshRate = 60*1000;  // 60 Hz
    }*/

    if ((int)(vinfo.width) <= 0 || (int)(vinfo.height) <= 0) {
        // the driver doesn't return that information
        // default to 160 dpi
        vinfo.width  = ((vinfo.xres * 25.4f)/160.0f + 0.5f);
        vinfo.height = ((vinfo.yres * 25.4f)/160.0f + 0.5f);
    }

    // mle float xdpi = (vinfo.xres * 25.4f) / vinfo.width;
    // mle float ydpi = (vinfo.yres * 25.4f) / vinfo.height;
    // mle float fps  = refreshRate / 1000.0f;


    int mScreenHeight  = vinfo.yres;
    int mScreenWidth   = vinfo.xres;
    int mBytesPerPixel = vinfo.bits_per_pixel / 8;

    // Figure out the size of the screen in bytes
    screensize = mScreenWidth * mScreenHeight * mBytesPerPixel;


    //############################################################################
    //Buffer creation

    frame_buffer_address = (short *) mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if ((int) frame_buffer_address == -1) {
        CLI_disp_msg("Error: failed to map framebuffer device to memory.\n");
    return 4;
    }


    //Specify the buffer to map to the display
    if (ioctl(fbfd, MCDE_IOCTL_SET_SOURCE_BUFFER, &frame_buffer_address)) {
        LOS_Log("Error setting source buffer information.\n");
        return (-1);
    }

    t_uint32 i,j;

    #define WORK_HEIGHT MIN(mScreenHeight,IMAGE_HEIGHT)
    #define WORK_WIDTH MIN(mScreenWidth,IMAGE_WIDTH)

    //######################################################################
    //Fill the buffer
    for (i=0;i<(t_uint32)WORK_HEIGHT;i++) {
        for (j=0;j<(t_uint32)WORK_WIDTH;j++) {
            * ((short *) (((unsigned int) frame_buffer_address) + (((i * mScreenWidth) + j) * 2))) =
            invite[i*IMAGE_WIDTH + j];
        }
    }

    //##############################################################
    /* Request update of FB */
    vinfo.yoffset = 0;
    if (ioctl(fbfd, FBIOPUT_VSCREENINFO, &vinfo)) {
        CLI_disp_msg("Error panning display.\n");
    return 5;
    }
    vinfo.yoffset = 480;
    if (ioctl(fbfd, FBIOPUT_VSCREENINFO, &vinfo)) {
        CLI_disp_msg("Error panning display.\n");
    return 5;
    }

    CLI_disp_msg("FB buffer address : %x.\n", frame_buffer_address);

    //##################################################################
    //Close framebuffer device
    close (fbfd);
    return 0;
}
#endif //__ARM_LINUX

