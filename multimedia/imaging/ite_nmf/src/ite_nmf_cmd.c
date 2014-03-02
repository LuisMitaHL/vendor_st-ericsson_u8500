/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*******************************************************************************************
 * ite_nmf_cmd.c: This module implements ite commands which can be used in ITE scripts.
 * Each command is associated to a function defined as follows in ite_nmf_use_cases lib :
 * 	static void C_command_name( int, char ** );
 *******************************************************************************************/

/*-----------------------------------------*/
/* INCLUDES           					   */
/*-----------------------------------------*/
#include "ite_testenv_utils.h"
#include "cli.h"
#include "ite_nmf_cmd.h"

#include "ite_vf_stillpicture_display.h"
#include "ite_datapath_tests.h"
#include "ite_nreg_datapath_tests.h"
//mle #include "ite_nreg_digizoom_tests.h"
//mle#include "ite_Test_RW_List_PE.h"
#include "ite_operatingMode.h"
#include "ite_debug.h"
// mle#include "ite_nmf_wb_tests.h"
//mle #include "ite_nmf_effects_tests.h"
#include "ite_nmf_stress_tests.h"
// mle #include "ite_nmf_focus_tests.h"
// mle #include "ite_xml_tests.h"
#include "ite_nmf_framerate_tests.h"
#include "ite_nmf_statistic_functions.h"
#include "ite_nmf_gridiron.h"
#include "ite_nmf_adsoc.h"
#include "ite_nmf_duster.h"
#include "ite_isp_ip_tests.h"
#include "ite_nmf_sensor_output_mode.h"
#include <ite_nmf_rgb2yuvcoder.h>
#include "ite_nmf_exposure.h"
#include "ite_nmf_abort_rx.h"
#include "ite_nmf_test_forced_grabok.h"
#include "ite_nmf_glace_geometry.h"
#include "ite_nmf_histogram_geometry.h"
#include "ite_framerate.h"

t_cmd_list nmfite_cmd_list[] = {
    { "VfandStillPicture", C_ite_VfStillDisplay_cmd,
        "VfandStillPicture: View finder and still picture use case\n"
    },
    /* mle { "NonRegTest_PE_RW_List_In_VfandSP", C_ite_NonRegTest_PE_RW_List_cmd,
        "NonRegTest_PE_RW_List_In_VfandSP: Non Regression Test of Read and Write Page Element in a View finder and still picture use case\n"
    },
    */
	{ "ITE_LR_DATAPATH", C_ite_datapath_LR_cmd,
        "ITE_LR_DATAPATH: Streaming using LR pipe and check ColorBar in Output GrabBufferLR[0] Buffer\n"
    },{ "ITE_HR_DATAPATH", C_ite_datapath_HR_cmd,
        "ITE_HR_DATAPATH: Streaming using HR pipe and check ColorBar in Output GrabBufferHR[0] Buffer\n"
    },{ "ITE_HR_SmiaPP_DATAPATH", C_ite_datapath_HR_SmiaPP_cmd,
        "ITE_HR_DATAPATH_SMIAPP: Streaming using HR pipe and smia++ power-up sequence. And check ColorBar in Output GrabBufferHR[0] Buffer\n"
    },{ "ITE_LRHR_DATAPATH", C_ite_datapath_LRHR_cmd,
        "ITE_LRHR_DATAPATH: Streaming using LR and HR pipes and check ColorBar in Output GrabBufferLR[0] and GrabBufferHR[0] Buffers\n"
    },{ "ITE_BMS_DATAPATH", C_ite_datapath_BMS_cmd,
        "ITE_BMS_DATAPATH: Streaming using BMS pipe and check ColorBar in Output GrabBufferBMS[0] Buffer\n"
    },{ "ITE_FLASH_BMS_DATAPATH", C_ite_datapath_BMS_FLASH_cmd,
        "ITE_FLASH_BMS_DATAPATH: enable flash and Stream using BMS pipe\n"
    },{ "ITE_NREG_DATAPATH", C_ite_nreg_datapath_cmd,
        "ITE_NREG_DATAPATH: Streaming using different pipes and check Output Buffers and different framerates \n"
    },{ "ITE_NREG_LRDATAPATH", C_ite_nreg_LRdatapath_cmd,
        "ITE_NREG_LRDATAPATH: Streaming using LR Data Pipe and check Output Buffers and different framerates \n"
    },{ "ITE_NREG_HRDATAPATH", C_ite_nreg_HRdatapath_cmd,
        "ITE_NREG_HRDATAPATH: Streaming using HR Data Pipe and check Output Buffers and different framerates \n"
    },{ "ITE_NREG_LRHRDATAPATH", C_ite_nreg_LRHRdatapath_cmd,
        "ITE_NREG_LRHRDATAPATH: Streaming using Both LR and HR Data Pipe and check Output Buffers and different framerates \n"
    },{ "ITE_NREG_BMSDATAPATH", C_ite_nreg_BMSdatapath_cmd,
        "ITE_NREG_BMSDATAPATH: Streaming using BMS DataPath and check Output Buffers and different framerates \n"
    },{ "ITE_NREG_VIDEOBMSDATAPATH", C_ite_nreg_VideoBMSdatapath_cmd,
        "ITE_NREG_VIDEOBMSDATAPATH: Streaming using Video BMS DataPath and check Output Buffers and different framerates \n"
    },{ "ITE_NREG_LRHRBMSDATAPATH", C_ite_nreg_LRHRBMSdatapath_cmd,
        "ITE_NREG_LRHRBMSDATAPATH: Streaming using different pipes and check Output Buffers and different framerates \n"
    },{ "ITE_NREG_BMLDATAPATH", C_ite_nreg_BMLdatapath_cmd,
        "ITE_NREG_BMLDATAPATH: Streaming using BML and check Output Buffers and different framerates \n"
    },{ "ITE_NREG_VIDEOBMLDATAPATH", C_ite_nreg_VideoBMLdatapath_cmd,
        "ITE_NREG_VIDEOBMLDATAPATH: Streaming using different pipes and check Output Buffers and different framerates \n"
    },{ "ITE_NREG_ZOOMBMLDATAPATH", C_ite_nreg_ZoomBMLdatapath_cmd,
        "ITE_NREG_ZOOMBMLDATAPATH: Streaming using BML with Zoom and check Output Buffers and different framerates \n"
    },{ "ITE_NREG_ZOOMSTREAMING", C_ite_nreg_ZoomStreaming_cmd,
        "ITE_NREG_ZOOMSTREAMING: Streaming using LR and HR with Zoom and check Output Buffers and different framerates \n"
    },{ "ITE_IP_TESTS", C_ite_ip_tests_cmd,
        "ITE_IP_TESTS: Streaming using different pipes and and test various IPs\n"
    },{ "ITE_FLIP_MIRROR_TEST", C_ite_Flip_Mirror_cmd,
        "ITE_FLIP_MIRROR_TEST: Verifying the Flip and Mirror Functionality on LR Pipe\n"
    },{ "ITE_97581_BUGTEST", C_ite_test_bug97581_cmd,
        "ITE_97581_BUGTEST:  \n"
    },{ "ITE_98168_BUGTEST", C_ite_test_bug98168_cmd,
        "ITE_98168_BUGTEST:  \n"
    },{ "ITE_BASIC_BML", C_ite_basic_bml_cmd,
        "ITE_BASIC_BML: PErform a bms then a bml monitoring bml and LR events with timestamps \n"
    },
	/* mle{ "ITE_OperatingMode", C_ite_OperatingMode_cmd,
        "ITE_OperatingMode: Test OMX like OperatingModes\n"
    },{ "STILLPREVIEW_STILL", C_ite_nreg_stillpreview_still_cmd,
        "ITE_NREG_STILLPREVIEW_STILL: execute a StillPreview and Still basic usecase and check framerate and buffer\n"
    },{ "VIDEOPREVIEW_VIDEO", C_ite_nreg_videopreview_video_cmd,
        "ITE_NREG_VIDEOPREVIEW_VIDEO: execute a VideoPreview and Video basic usecase and check framerate and buffer\n"
    },{ "FORMATLRTEST", C_ite_nreg_formatlrtest_cmd,
        "ITE_NREG_FORMATLRTEST: execute a StillPreviewbasic usecase and check Colorbar and Cyan buffer\n"
    },{ "FORMATHRTEST", C_ite_nreg_formathrtest_cmd,
        "ITE_NREG_FORMATHRTEST: execute a VideoPreviewbasic usecase and check Colorbar and Cyan buffer\n"
    },     { "ITE_NREG_DIGIZOOM_TEST1", C_ite_nreg_digizoom_test1_cmd,
             "ITE_NREG_DIGIZOOM_TEST1: Test all DZ Step for all output size for VideoPreview uc\n"
    },{ "ITE_NMF_WB_TESTS", C_ite_WBTests_cmd,
        "ITE_NMF_WB_TESTS: Test WB feature \n"
	},{ "ITE_NMF_EFFECTS_TESTS", C_ite_EffectsTests_cmd,
        "ITE_NMF_EFFECTS_TESTS: Test special effects features \n"
    },*/
	{ "ITE_NMF_STRESS_TESTS", C_ite_StressTests_cmd,
        "ITE_NMF_STRESS_TESTS: Invoke NMF commands intensively \n"
    },
	/*
	{ "ITE_NMF_AUTOFOCUS_TESTS", C_ite_FocusTests_cmd,
        "ITE_NMF_AUTOFOCUS_TESTS: Test autofocus \n"
    },
	*/
	{ "ITE_NMF_FRAMERATE_TESTS", C_ite_FramerateTests_cmd,
        "ITE_NMF_FRAMERATE_TESTS: Test different manual framerate \n"
    },{ "LogPE", C_ite_dbg_PE_Log_cmd,
        "LogPE: LogPE <enable/disable>\n"
    },{ "readPE", C_ite_dbg_readPE_cmd,
        "readPE: read PE\n"
    },{ "writePE", C_ite_dbg_writePE_cmd,
        "writePE: write PE\n"
    },{ "startvpip", C_ite_dbg_startvpip_cmd,
        "startvpip: startvpip\n"
    },{ "stopvpip", C_ite_dbg_stopvpip_cmd,
        "stopvpip: stopvpip\n"
    },{ "startgrab", C_ite_dbg_startgrab_cmd,
        "startgrab: startgrab\n"
    },{ "stopgrab", C_ite_dbg_stopgrab_cmd,
        "stopgrab: stopgrab\n"
    },{ "enablepipe", C_ite_dbg_enablepipe_cmd,
        "enablepipe: enablepipe <LR/HR>\n"
    },{ "startenv", C_ite_dbg_startenv_cmd,
        "start environment: \n"
    },{ "startenv_smiapp", C_ite_dbg_startenv_smiapp_cmd,
        "start environment: \n"
    },{ "startenv_sensor_tunning", C_ite_dbg_startenv_SENSOR_TUNNING_cmd,
        "start environment: \n"
    },{ "stopenv", C_ite_dbg_stopenv_cmd,
        "stop environment: \n"
    },{ "loadFW", C_ite_dbg_loadfw_cmd,
        "loadFW: loadFW\n"
    },{ "setusecase", C_ite_dbg_setusecase_cmd,
        "setusecase: set usecase <LR/HR> <VGA/XGA...> <GRB565/ARGB4444...> | default\n"
    },{ "bmsmode", C_ite_dbg_bmsmode_cmd,
        "bmsmode: bmsmode <STILL/VIDEO>\n"
    },{ "bmsout", C_ite_dbg_bmsout_cmd,
        "bmsout: bmsout <BAYERSTORE0/BAYERSTORE2>\n"
    },{ "bmlin", C_ite_dbg_bmlin_cmd,
        "bmlin: bmlin <BAYERLOAD1/BAYERLOAD2/RGBLOAD>\n"
    },{ "storeFOVX", C_ite_dbg_StoreFOVX_cmd,
        "storeFOVX: storeFOVX \n"
    },{ "getusecase", C_ite_dbg_getusecase_cmd,
        "getusecase: getusecase\n"
    },{ "preview", C_ite_dbg_preview_cmd,
        "preview: preview <still/video> <prepare/start/stop/free>\n"
    },{ "still", C_ite_dbg_still_cmd,
        "Still: still <prepare/start/stop/free>\n"
    },{ "video", C_ite_dbg_video_cmd,
        "Video: video <prepare/start/stop/free>\n"
    },{ "LR", C_ite_dbg_LR_cmd,
        "LR: LR <prepare/start/stop/free>\n"
    },{ "HR", C_ite_dbg_HR_cmd,
        "HR: HR <prepare/start/stop/free>\n"
    },{ "LRHR", C_ite_dbg_LRHR_cmd,
        "LRHR: LRHR <prepare/start/stop/free>\n"
    },{ "bms", C_ite_dbg_bms_cmd,
        "bms: bms <prepare/start/stop/free>\n"
    },{ "bml", C_ite_dbg_bml_cmd,
        "bml: bml <prepare/start/stop/free>\n"
    },{ "display", C_ite_dbg_dispbuf_cmd,
        "display: display <LR/HR>\n"
    },{ "initbuf", C_ite_dbg_initbuf_cmd,
        "initbuf: initbuf <LR/HR>\n"
    },{ "cleanbuf", C_ite_dbg_cleanbuf_cmd,
        "cleanbuf: cleanbuf <LR/HR/BMS>\n"
    },{ "initlcdbuf", C_ite_dbg_initlcdbuf_cmd,
        "initlcdbuf: initlcdbuf <SIZE> <FORMAT>\n"
    },{ "autofps", C_ite_dbg_autoframerate_cmd,
        "autofps: autofps <min> <max>\n"
    },{ "manualfps", C_ite_dbg_manualframerate_cmd,
        "manualfps: manualfps <current_fps> <NIL> / manualfps <curr_fps> <max_fps>/manualfps <NIL> <max_fps>\n"
    },{ "testframerate", C_ite_dbg_testframerate_cmd,
        "testframerate: testframerate <pipe><resolution><format>\n"
#ifdef TEST_PERFORMANCE
    },{ "recordtime", C_ite_dbg_recordtime_cmd,
        "recordtime: recordtime <START/STOP><usecase><comment><tag>\n"
#endif
    },{ "getfps", C_ite_dbg_getframerate_cmd,
        "getfps: getfps\n"
    },{ "givergb", C_ite_dbg_givergbvalue_cmd,
        "givrgb: givrgb <LR/HR/BMS> <Xpos> <Ypos>\n"
    },{"sleepmode",C_ite_dbg_sleep_mode_xp70_cmd,
        "sleepmode: sleepmode <modeno>\n"
    },{"usagemode",C_ite_dbg_uagemode_cmd,
        "usagemode: usagemode <vf/af/still/stillnight/stillseq/video/videonight/videohq/videohs/none>\n"
    },{ "sensorparams",C_ite_dbg_test_SensorParams_cmd,
        "sensorparams:sensorparams <loop_count>\n"
    },{ "DG_test",C_ite_dbg_test_ISP_Params_cmd,
        "DG_test:DG_test <loop_count>\n"
    },{ "setresolution",C_ite_dbg_Setresolution_cmd,
        "setresolution:setresolution <LR/HR> <resolution> <format>\n"
    },{ "getresolution",C_ite_dbg_getresolution_cmd,
        "getresolution:getresolution <resolution enum value>\n"
    },{ "convu",C_ite_dbg_ConvToUpper_cmd,
        "convu:convu <string>\n"
    },
    /* mle { "dzprepare", C_ite_dbg_dz_prepare_cmd,
        "dzprepare: dzprepare\n"
    },{ "dzreset", C_ite_dbg_dz_reset_cmd,
        "dzreset: dzreset\n"
    },{ "dzstep", C_ite_dbg_dz_step_cmd,
        "dzstep: dzstep <0....84>\n"
    },{ "dztest", C_ite_dbg_dz_test_cmd,
        "dztest: dztest <LR/HR> <0....84>\n"
    },
    */
    { "sensorwrite", C_ite_dbg_sensoraccess_cmd,
        "sensorwrite: sensorwrite <index> <data>\n"
    },{ "sensorread", C_ite_dbg_sensorread_cmd,
        "sensorread: sensorread <index> \n"
    },{ "sensormode", C_ite_dbg_sensormode_cmd,
        "sensormode: sensormode <mode>\n"
    },{ "colorbartest", C_ite_dbg_colorbartest_cmd,
        "colorbartest: colorbartest <LR/HR/BMS>\n"
    },{ "buffercheck", C_ite_dbg_buffercheck_cmd,
        "buffercheck: buffercheck <LR/HR/BMS> <color>\n"
    },{ "writebmp", C_ite_dbg_writebmp_cmd,
        "writebmp: writebmp <LR/HR> <filename>\n"
    },{ "showgrabuf", C_ite_dbg_showgrabuf_cmd,
        "showgrabuf: showgrabuf <LR/HR>\n"
    },{ "wait", C_ite_dbg_wait_cmd,
        "wait: wait <sec> \n"
    },{ "itehelp", C_ite_dbg_help_cmd,
        "itehelp: itehelp \n"
    },{ "ConfigureGridIronStatic", C_ite_dbg_ConfigureGridIron_StaticPrams_cmd,
        "ConfigureGridIronStatic: ConfigureGridIronStatic\n"
    },{ "TestGridIronLiveCast", C_ite_dbg_TestGridIronLiveCast_cmd,
        "TestGridIronLiveCast: TestGridIronLiveCast\n"
#if ENABLE_GRIDIRON_DEBUG_TRACES
    },{ "PrintGridIron", C_ite_dbg_PrintGridIron_cmd,
        "PrintGridIron: PrintGridIron \n"
#endif
    },{ "ConfigureAdsoc", C_ite_dbg_ConfigureAdsoc_cmd,
        "ConfigureAdsoc: ConfigureAdsoc \n"
    },{ "VerifyAdsoc", C_ite_dbg_VerifyAdsoc_cmd,
        "VerifyAdsoc: VerifyAdsoc \n"
    },{ "ConfigureAdsocRP", C_ite_dbg_ConfigureAdsocRP_cmd,
        "ConfigureAdsocRP: ConfigureAdsocRP \n"
    },{ "VerifyAdsocRP", C_ite_dbg_VerityAdsocRP_cmd,
        "VerifyAdsocRP: VerifyAdsocRP \n"
    },{ "ConfigureDuster", C_ite_dbg_ConfigureDuster_cmd,
        "ConfigureDuster: ConfigureDuster \n"
    },{ "VerifyDuster", C_ite_dbg_VerityDuster_cmd,
        "VerifyDuster: VerifyDuster \n"
    },{ "testMaxExposureTime",C_ite_dbg_Test_MaxExposureTime_cmd,
        "testMaxExposureTime\n"
    },{ "set_abort_rx",C_ite_dbg_set_abort_rx_cmd,
        "set_abort_rx <0/1>\n"
    },{ "test_abort_rx_zoom_streaming",C_ite_dbg_Test_abort_rx_Zoom_Streaming,
        "test_abort_rx_zoom_streaming \n"
    },{ "VariableFramerate_Set",C_ite_dbg_VariableFrameRate_SetMode_cmd,
        "VariableFramerate_Set  <0/1>"
    },{ "TestVariableFrameRate",C_ite_dbg_VariableFrameRate_TestVariableFrameRate_cmd,
        "TestVariableFrameRate"
    },{ "mmio_test", C_ite_dbg_mmio_test_cmd,
        "MMIOTest: MMIO API Test \n"
    },{ "raw_dump", C_ite_RAWDump_cmd,
        "RAWDump: Takes RAW8 and RAW12 stills\n"
    },{ "configure_rgb2yuvcoder", C_ite_dbg_Configure_RGB2YUVCoder_cmd,
        "configure_rgb2yuvcoder: Configure Rgg2Yuv coder module of ISP FW\n"
    },{"test_rgb2yuvcoder", C_ite_dbg_Test_RGB2YUVCoder_cmd,
        "test_rgb2yuvcoder: Test Rgg2YuvCoder module of ISP FW\n"
    },{"set_custom_stock_matrix", C_ite_dbg_Configure_CustomStockMatrix_cmd,
        "set_custom_stock_matrix: sets stock matrix for Rgg2YuvCoder module of ISP FW\n"
    },{"set_custom_signal_range", C_ite_dbg_Configure_CustomSignalRange_cmd,
        "set_custom_signal_range: sets signal range for Rgg2YuvCoder module of ISP FW\n"
    },{"TestForcedGrabMode", C_ite_dbg_Test_Forced_GrabOK_cmd,
        "TestForcedGrabMode: Test for forced grab ok mechanism\n"
    },{"configureForcedGrabMode", C_ite_dbg_Configure_GrabMode_cmd,
        "configureForcedGrabMode: Configuring begin point for grab ok mechanism\n"
    },{ "sensorname",C_ite_sensor_select_cmd,
        "sensorname: sensorname <IMX072>\n"
    },{ "NVM_RAW",C_ite_nvm_raw_cmd,
        "NVM_RAW:NVM_RAW <enable/disable>\n"
    },{ "random",C_ite_dbg_rand_floatrange_cmd,
        "random : random <min_no> <max_no> \n"
    },{ "gen_random_val",C_ite_dbg_rand_Range_cmd,
        "gen_random_val : gen_random_val <float/int> <min_no> <max_no> [multiple]\n"
    },{ "set_sensor_param",C_ite_dbg_set_SensorParams_cmd,
        "set_sensor_param : set_sensor_param <default/exp_time> <default/a_gain>\n"
    },{ "setGlaceGeometry", C_ite_dbg_Set_Glace_Geometry_cmd,
        "setGlaceGeometry : setGlaceGeometry  <mode> <f_hBlockSizeFrac> <f_vBlockSizeFrac> <f_hROIStartFrac> <f_vROIStartFrac> <hGridSize> <vGridSize> \n"
    },{ "getGlaceGeometry", C_ite_dbg_Get_Glace_Geometry_cmd,
        "getGlaceGeometry : getGlaceGeometry \n"
    },{ "testGlaceGeometry", C_ite_dbg_Test_Glace_Geometry_cmd,
        "testGlaceGeometry : testGlaceGeometry \n"
    },{ "setbmssize", C_ite_dbg_Set_BMS_Size_cmd,
        "setbmssize : setbmssize <woi_x> <woi_y> <out_res_x> <out_res_y> \n"
    },{ "setHistogramGeometry", C_ite_dbg_Set_Histogram_Geometry_cmd,
        "setHistogramGeometry : setHistogramGeometry  <mode> <f_hSizeFrac> <f_vSizeFrac> <f_hStartFrac> <f_vStartFrac> \n"
    },{ "getHistogramGeometry", C_ite_dbg_Get_Histogram_Geometry_cmd,
        "getHistogramGeometry : getHistogramGeometry \n"
    },{ "testHistogramGeometry", C_ite_dbg_Test_Histogram_Geometry_cmd,
        "testHistogramGeometry : testHistogramGeometry \n"
    },{ "SetRXTestPattern",C_ite_dbg_SetRxTestPattern_cmd,
        "CMD:- SetRXTestPattern <RxTestPattern>,<RED>,<GR>,<BLUE>,<BG>\n"
    },{ "SetCursorPosition",C_ite_dbg_SetCursorPosition_cmd,
        "CMD:- SetCursorPosition <Horz Pos in %><Horz Wd><Vert Pos in %><Vert Wd>\n"
    },{ "SetInputModeSelection",C_ite_dbg_SetInputModeSelection_cmd,
        "CMD:- SetInputModeSelection <Rx, Sensor>"
    },{ "setExposureTime",C_ite_dbg_Set_ExposureTime_cmd,
        "setExposureTime, setExposureTime  <exp_time_us>"
    },{ "getExposureTime",C_ite_dbg_Get_ExposureTime_cmd,
        "getExposureTime, getExposureTime  "
    },{ "StoreIMAGE", C_ite_dbg_Store_IMAGE_cmd,
        "StoreIMAGE: StoreIMAGE <enable/disable>\n"
    },{ "br_control",C_ite_dbg_Test_Brightness_Control_Test_cmd,
        "br_control: br_control <PIPE><step_size>\n"
    },{ "DisplayEvents", C_ite_dbg_Display_Events_cmd,
        "DisplayEvents: DisplayEvents \n"
    },{ "Clearstats", C_ite_dbg_Clear_Stats_cmd,
        "Clearstats: Clearstats <glace/histo>\n"
    },{ "setTraceProp", C_ite_dbg_SetTraceProperty_cmd,
        "setTraceProp: setTraceProp <Trace mechanism> <LogLevel> <Enable>\n<Trace mechanism>\n"
    },{ "setGammaCurve", C_ite_dbg_SetGammaCurve_cmd,
        "setGammaCurve: use to set gamma curve"
    },{ "TestValidFrameFlag", C_ite_dbg_TestValidFrameFlag_cmd,
        "TestValidFrameFlag: use to test e_Flag_Request_ValidFrameNotification_Ctrl flag"
    }

    /*,{ "xmltest", C_ite_xml_basictest_cmd,
        "xmltest: xmltest \n"
    },{ "stress", C_ite_dbg_startstopstress_cmd,
        "stress: stress \n"
    },{ "iris", C_ite_dbg_iris_cmd,
        "iris: iris \n"
    }*/

};


/**
 * \defgroup InitNmfITECmdList
 * \brief This interface gathers all commands related to nmf imaging tests
 */
void Init_Nmf_ITECmdList(void)
{
    CLI_register_interface("NMF_ITE_CMD", sizeof(nmfite_cmd_list)/(sizeof(nmfite_cmd_list[0])), nmfite_cmd_list, 1);
}

