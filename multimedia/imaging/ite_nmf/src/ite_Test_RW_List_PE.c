/*
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ite_testenv_utils.h"
#include "cli.h"
#include "ite_Test_RW_List_PE.h"
#include "ite_main.h"
#include "ite_boardinfo.h"
#include "ite_platform.h"
#include "VhcElementDefs.h"

//for NMF

#include "ite_sia_interface_data.h"
#include "ite_sia_bootcmd.h"
#include "ite_init.h"
#include "ite_sia_init.h"
//#include "ite_alloc.h"
#include "ite_display.h"
#include "ite_event.h"
#include "ite_pageelements.h"
#include "ite_colormatrix.h"
#include "ite_grab.h"
#include "ite_vpip.h"
#include "ite_framerate.h"
#include "ite_sia_buffer.h"
//#include "ite_irq_handler.h"
//#include "ite_ske.h"
#include "ite_nmf_standard_functions.h"

#include <cm/inc/cm_macros.h>


extern struct s_grabParams grabparamsHR;
extern struct s_grabParams grabparamsLR;
extern volatile t_uint32 g_grabHR_infinite;
extern volatile t_uint32 g_grabLR_infinite;
extern ts_siapicturebuffer GrabBufferHR[];
extern ts_siapicturebuffer LCDBuffer;
extern ts_siapicturebuffer GamBuffer;

extern ts_sia_usecase usecase;


/* -----------------------------------------------------------------------
FUNCTION : ITE_GrabVPIPDisplayCmd
PURPOSE  : Command for Grab Sensor and display use case
------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_NonRegTest_PE_RW_List_cmd(int a_nb_args, char ** ap_args)
{
    LOS_Log("Not supported\n");
    UNUSED(a_nb_args);
    UNUSED(ap_args);
    /*
    if (a_nb_args == 3)
    {
    //ITE_StoreTestHeader(a_nb_args,ap_args);
    ITE_NonReg_RWListPe_InVfStillDisplay(ap_args[1], ap_args[2]);
    } 
    else
    {
    LOS_Log("Not correct command arguments\n");
}*/
    return CMD_COMPLETE;
}

/* -----------------------------------------------------------------------
FUNCTION : ITE_NonReg_RWListPe_InVfStillDisplay
PURPOSE  : Non regression test of Read and Write of 
Page Element in a list in a ViewFinderr and 
Still picture display use case
------------------------------------------------------------------------ */
void ITE_NonReg_RWListPe_InVfStillDisplay(char * ap_test_id, char * ap_grabvpip_options)
{
    LOS_Log("Not supported\n");
    UNUSED(ap_test_id);
    UNUSED(ap_grabvpip_options);
    /*
    int xsensorSize=2048;
    int ysensorSize=1536;
    
     unsigned key_pressed=0;

       ITE_InitUseCase(ap_test_id, ap_grabvpip_options,&usecase);
       ITE_Start_Env(&usecase);
       ITE_StillPreview_Prepare(&usecase);
       ITE_StillPreview_Start(INFINITY);    
       
        LOS_Log("To test Read/Write list PE Non regression test press 1\n");
        LOS_Log("To test Polling Non regression test press 2\n");
        LOS_Log("To test both press 3\n");
        LOS_Scanf("%d\n",&key_pressed);
        
         switch(key_pressed)
         {
         case 1:
         ITE_NonReg_ListPE(xsensorSize, ysensorSize);
         break;
         case 2:
         ITE_NonReg_PollingPE(xsensorSize, ysensorSize);
         break;
         case 3:
         ITE_NonReg_ListPE(xsensorSize, ysensorSize);
         ITE_NonReg_PollingPE(xsensorSize, ysensorSize);
         break;
         default:
         LOS_Log("Wrong number\n");
         break;
         }
         
          ITE_StillPreview_Stop();
          ITE_StillPreview_Free();
          ITE_Stop_Env();
    */
    
}



//use case performing a VF VGA for "frameToGrab" frames on pipe LR then perform a still capture on pipe HR at (xSnapshot,ySnapshot) resolution
void ITE_NonReg_ListPE(int xSnapshot, int ySnapshot)
{
    LOS_Log("Not supported\n");
    UNUSED(xSnapshot);
    UNUSED(ySnapshot);
    /*	
    ts_PageElement tab_pe[ISPCTL_SIZE_TAB_PE]= {{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}};
    ts_PageElement read_backtab_pe[ISPCTL_SIZE_TAB_PE]= {{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}};
    t_uint16 nb_of_pe=0;
    int status=1;
    int i=0;
    
     // AUTOFOCUS 
     ITE_writePE(FocusControls_bRange,FOCUS_RANGE_FULLRANGE);
     ITE_writePE(FocusControls_bMode,FC_TLI_MODE_MANUAL_FOCUS);
     ITE_writePE(FocusControls_bAFCommand,AF_TLI_CMD_NULL);
     ITE_writePE(FocusControls_bLensCommand,LA_CMD_NULL);
     ITE_writePE(FocusControls_bLensCommand,LA_CMD_INIT);
     ITE_writePE(FocusControls_bLensCommand,LA_CMD_NULL);
     ITE_writePE(FocusControls_bLensCommand,LA_CMD_AUTOCALIBRATE);
     
      //switch to AF continous mode
      ITE_writePE(FocusControls_bMode, FC_TLI_MODE_AF_CONTINUOUS_FOCUS);
      ITE_writePE(FocusControls_bAFCommand, AF_TLI_CMD_NULL);
      
       //##########################################################################################################
       //                                                                                                      HERE THE NON REG TEST IS STARTING;
       //##########################################################################################################
       
        //1 Test n°1: write and read back with boundary array limits, e.g. 1 and 32 Pe
        //-----------------------------------------------------------------------------------------------
        // ONE VALUE WRITTEN, ONE VALUE READ BACK
        //init tab
        for(i=0;i<ISPCTL_SIZE_TAB_PE;i++)
        {
        tab_pe[i].pe_addr=0;
        tab_pe[i].pe_data=0;
        read_backtab_pe[0].pe_addr=0;
        read_backtab_pe[0].pe_data=0;
        }
        
         //Write and read one value
         tab_pe[0].pe_addr=Pipe0Control_fSfxSolariseEnabled;
         read_backtab_pe[0].pe_addr=Pipe0Control_fSfxSolariseEnabled;
         tab_pe[0].pe_data=1;
         nb_of_pe=1;
         LOS_Log("TEST 1: WRITE ONE VALUE, READ BACK THE VALUE\n");
         ITE_writeListPE(tab_pe, nb_of_pe);
         status = ITE_readListPE(read_backtab_pe,nb_of_pe);
         if(tab_pe[0].pe_data != read_backtab_pe[0].pe_data)
         {
         LOS_Log("Value written 0x%x --> value read 0x%x\n",tab_pe[0].pe_data, read_backtab_pe[0].pe_data);
         status=0;
         }
         if(status==0)
         {
         LOS_Log("TEST 1: FAILED\n");
         }
         else
         {
         LOS_Log("TEST 1: OK TEST PASSED\n");
         }
         
          // 32 VALUES WRITTEN (a counter value at the same address, so last value to be read must be 32), ONE VALUE READ BACK (so the value 32)
          //init tab
          for(i=0;i<ISPCTL_SIZE_TAB_PE;i++)
          {
          tab_pe[i].pe_addr=0;
          tab_pe[i].pe_data=0;
          read_backtab_pe[0].pe_addr=0;
          read_backtab_pe[0].pe_data=0;
          }
          
           //Write 32 values and readback only one
           for(i=0;i<ISPCTL_SIZE_TAB_PE;i++)
           {
           tab_pe[i].pe_addr=Pipe0Control_fSfxSolariseEnabled;
           tab_pe[i].pe_data=i;
           read_backtab_pe[i].pe_addr=Pipe0Control_fSfxSolariseEnabled;
           }
           nb_of_pe=ISPCTL_SIZE_TAB_PE;
           LOS_Log("TEST 2: WRITE 32 VALUES, READ BACK THE 32nth VALUE\n");
           ITE_writeListPE(tab_pe, nb_of_pe);
           status = ITE_readListPE(read_backtab_pe,1);
           if(tab_pe[nb_of_pe-1].pe_data != read_backtab_pe[0].pe_data)
           {
           LOS_Log("Value written 0x%x --> value read 0x%x\n",tab_pe[nb_of_pe-1].pe_data, read_backtab_pe[0].pe_data);
           status=0;
           }
           if(status==0)
           {
           LOS_Log("TEST 2: FAILED\n");
           }
           else
           {
           LOS_Log("TEST 2: OK TEST PASSED\n");
           }
           
            
             // 32 VALUES WRITTEN, 32 VALUES READ BACK 
             //init tab
             for(i=0;i<ISPCTL_SIZE_TAB_PE;i++)
             {
             tab_pe[i].pe_addr=0;
             tab_pe[i].pe_data=0;
             read_backtab_pe[0].pe_addr=0;
             read_backtab_pe[0].pe_data=0;
             }
             
              //Prepare to Write and read back 32 values at 32 address
              // page "ExposureControls"
              tab_pe[0].pe_addr = ExposureControls_bMode;                                                     //0x1b00
              tab_pe[1].pe_addr = ExposureControls_bMetering;                                                 //0x1b02
              tab_pe[2].pe_addr = ExposureControls_bManualExposureTime_s_num;                                 //0x1b04
              tab_pe[3].pe_addr = ExposureControls_bManualExposureTime_s_den;                                 //0x1b06
              tab_pe[4].pe_addr = ExposureControls_fpManualDesiredExposureTime_us_MSByte;                     //0x1b09
              tab_pe[5].pe_addr = ExposureControls_fpColdStartDesiredTime_us_MSByte;                          //0x1b0d
              tab_pe[6].pe_addr = ExposureControls_iExposureCompensation;                                     //0x1b10
              tab_pe[7].pe_addr = ExposureControls_bMiscSettings;                                             //0x1b12
              tab_pe[8].pe_addr = ExposureControls_uwDirectModeCoarseIntegration_lines_MSByte;                //0x1b15
              tab_pe[9].pe_addr = ExposureControls_uwDirectModeFineIntegration_pixels_MSByte;                 //0x1b19
              tab_pe[10].pe_addr = ExposureControls_uwDirectModeCodedAnalogGain_MSByte;                       //0x1b1d
              tab_pe[11].pe_addr = ExposureControls_fpDirectModeDigitalGain_MSByte;                           //0x1b21
              tab_pe[12].pe_addr = ExposureControls_uwFlashGunModeCoarseIntegration_lines_MSByte;             //0x1b25
              tab_pe[13].pe_addr = ExposureControls_uwFlashGunModeFineIntegration_pixels_MSByte;              //0x1b29
              tab_pe[14].pe_addr = ExposureControls_uwFlashGunModeCodedAnalogGain_MSByte;                     //0x1b2d
              tab_pe[15].pe_addr = ExposureControls_fpFlashGunModeDigitalGain_MSByte;                         //0x1b31
              tab_pe[16].pe_addr = ExposureControls_fFreezeAutoExposure;                                      //0x1b34
              tab_pe[17].pe_addr = ExposureControls_fpUserMaximumIntegrationTime_us_MSByte;                   //0x1b37
              tab_pe[18].pe_addr = ExposureControls_fpRecommendFlashGunAnalogGainThreshold_MSByte;            //0x1b3b
              tab_pe[19].pe_addr = ExposureControls_fEnableHighClipForDesiredExposureTime;                    //0x1b3e
              // page "WhiteBalanceControls"
              tab_pe[20].pe_addr = WhiteBalanceControls_bMode;                                                //0x2200
              tab_pe[21].pe_addr = WhiteBalanceControls_bManualRedGain;                                       //0x2202
              tab_pe[22].pe_addr = WhiteBalanceControls_bManualGreenGain;                                     //0x2204
              tab_pe[23].pe_addr = WhiteBalanceControls_bManualBlueGain;                                      //0x2206
              tab_pe[24].pe_addr = WhiteBalanceControls_bMiscSettings;                                        //0x2208
              tab_pe[25].pe_addr = WhiteBalanceControls_fpFlashRedGain_MSByte;                                //0x220b
              tab_pe[26].pe_addr = WhiteBalanceControls_fpFlashGreenGain_MSByte;                              //0x220f
              tab_pe[27].pe_addr = WhiteBalanceControls_fpFlashBlueGain_MSByte;                               //0x2213
              tab_pe[28].pe_addr = WhiteBalanceControls_fInhibitWhiteBalancePresetModeForFlash;               //0x2216
              // page "SkintoneControl"
              tab_pe[29].pe_addr = SkintoneControl_uwSkinThreshold_MSByte;                                    //0x2101
              tab_pe[30].pe_addr = SkintoneControl_uwMappingCoeffcient1_MSByte;                               //0x2105
              tab_pe[31].pe_addr = SkintoneControl_uwMappingCoeffcient2_MSByte;                               //0x2109
              
               // page "ExposureControls"
               read_backtab_pe[0].pe_addr = ExposureControls_bMode;                                             //0x1b00
               read_backtab_pe[1].pe_addr = ExposureControls_bMetering;                                         //0x1b02
               read_backtab_pe[2].pe_addr = ExposureControls_bManualExposureTime_s_num;                         //0x1b04
               read_backtab_pe[3].pe_addr = ExposureControls_bManualExposureTime_s_den;                         //0x1b06
               read_backtab_pe[4].pe_addr = ExposureControls_fpManualDesiredExposureTime_us_MSByte;             //0x1b09
               read_backtab_pe[5].pe_addr = ExposureControls_fpColdStartDesiredTime_us_MSByte;                  //0x1b0d
               read_backtab_pe[6].pe_addr = ExposureControls_iExposureCompensation;                             //0x1b10
               read_backtab_pe[7].pe_addr = ExposureControls_bMiscSettings;                                     //0x1b12
               read_backtab_pe[8].pe_addr = ExposureControls_uwDirectModeCoarseIntegration_lines_MSByte;        //0x1b15
               read_backtab_pe[9].pe_addr = ExposureControls_uwDirectModeFineIntegration_pixels_MSByte;         //0x1b19
               read_backtab_pe[10].pe_addr = ExposureControls_uwDirectModeCodedAnalogGain_MSByte;               //0x1b1d
               read_backtab_pe[11].pe_addr = ExposureControls_fpDirectModeDigitalGain_MSByte;                   //0x1b21
               read_backtab_pe[12].pe_addr = ExposureControls_uwFlashGunModeCoarseIntegration_lines_MSByte;     //0x1b25
               read_backtab_pe[13].pe_addr = ExposureControls_uwFlashGunModeFineIntegration_pixels_MSByte;      //0x1b29
               read_backtab_pe[14].pe_addr = ExposureControls_uwFlashGunModeCodedAnalogGain_MSByte;             //0x1b2d
               read_backtab_pe[15].pe_addr = ExposureControls_fpFlashGunModeDigitalGain_MSByte;                 //0x1b31
               read_backtab_pe[16].pe_addr = ExposureControls_fFreezeAutoExposure;                              //0x1b34
               read_backtab_pe[17].pe_addr = ExposureControls_fpUserMaximumIntegrationTime_us_MSByte;           //0x1b37
               read_backtab_pe[18].pe_addr = ExposureControls_fpRecommendFlashGunAnalogGainThreshold_MSByte;    //0x1b3b
               read_backtab_pe[19].pe_addr = ExposureControls_fEnableHighClipForDesiredExposureTime;            //0x1b3e
               // page "WhiteBalanceControls"
               read_backtab_pe[20].pe_addr = WhiteBalanceControls_bMode;                                        //0x2200
               read_backtab_pe[21].pe_addr = WhiteBalanceControls_bManualRedGain;                               //0x2202
               read_backtab_pe[22].pe_addr = WhiteBalanceControls_bManualGreenGain;                             //0x2204
               read_backtab_pe[23].pe_addr = WhiteBalanceControls_bManualBlueGain;                              //0x2206
               read_backtab_pe[24].pe_addr = WhiteBalanceControls_bMiscSettings;                                //0x2208
               read_backtab_pe[25].pe_addr = WhiteBalanceControls_fpFlashRedGain_MSByte;                        //0x220b
               read_backtab_pe[26].pe_addr = WhiteBalanceControls_fpFlashGreenGain_MSByte;                      //0x220f
               read_backtab_pe[27].pe_addr = WhiteBalanceControls_fpFlashBlueGain_MSByte;                       //0x2213
               read_backtab_pe[28].pe_addr = WhiteBalanceControls_fInhibitWhiteBalancePresetModeForFlash;       //0x2216
               // page "SkintoneControl"
               read_backtab_pe[29].pe_addr = SkintoneControl_uwSkinThreshold_MSByte;                            //0x2101
               read_backtab_pe[30].pe_addr = SkintoneControl_uwMappingCoeffcient1_MSByte;                       //0x2105
               read_backtab_pe[31].pe_addr = SkintoneControl_uwMappingCoeffcient2_MSByte;                       //0x2109
               
                for(i=0;i<ISPCTL_SIZE_TAB_PE;i++)
                {
                tab_pe[i].pe_data = i;
                }
                nb_of_pe=ISPCTL_SIZE_TAB_PE;
                LOS_Log("TEST 3: WRITE 32 VALUES, READ BACK 32 VALUES\n");
                ITE_writeListPE(tab_pe, nb_of_pe);
                status = ITE_readListPE(read_backtab_pe,nb_of_pe);
                for(i=0;i<nb_of_pe;i++)
                {
                if(tab_pe[i].pe_data != read_backtab_pe[i].pe_data)
                {
                LOS_Log("Value written 0x%x --> value read 0x%x\n",tab_pe[i].pe_data, read_backtab_pe[i].pe_data);
                status=0;
                }
                }
                if(status==0)
                {
                LOS_Log("TEST 3: FAILED\n");
                }
                else
                {
                LOS_Log("TEST 3: OK TEST PASSED\n");
                }
                
                 //2 Test n°2: write and read a tab of size 0 and 33
                 //-----------------------------------------------------------------------------------------------
                 // TAB OF SIZE 0
                 //init tab
                 for(i=0;i<ISPCTL_SIZE_TAB_PE;i++)
                 {
                 tab_pe[i].pe_addr=0;
                 tab_pe[i].pe_data=0;
                 read_backtab_pe[0].pe_addr=0;
                 read_backtab_pe[0].pe_data=0;
                 }
                 
                  //Write and read a tab of size 0
                  tab_pe[0].pe_addr=Pipe0Control_fSfxSolariseEnabled;
                  read_backtab_pe[0].pe_addr=Pipe0Control_fSfxSolariseEnabled;
                  tab_pe[0].pe_data=1;
                  nb_of_pe=0;
                  LOS_Log("TEST 4: WRITE OR READ VALUE WITH WRONG TAB SIZE (size 0)\n");
                  ITE_writeListPE(tab_pe, nb_of_pe);
                  status = ITE_readListPE(read_backtab_pe,nb_of_pe);
                  if(status==0)
                  {
                  LOS_Log("TEST 4: OK TEST PASSED\n");
                  }
                  
                   // TAB OF SIZE 33
                   //init tab
                   for(i=0;i<ISPCTL_SIZE_TAB_PE;i++)
                   {
                   tab_pe[i].pe_addr=0;
                   tab_pe[i].pe_data=0;
                   read_backtab_pe[0].pe_addr=0;
                   read_backtab_pe[0].pe_data=0;
                   }
                   
                    //Write and read a tab of size 33
                    tab_pe[0].pe_addr=Pipe0Control_fSfxSolariseEnabled;
                    read_backtab_pe[0].pe_addr=Pipe0Control_fSfxSolariseEnabled;
                    tab_pe[0].pe_data=1;
                    nb_of_pe=ISPCTL_SIZE_TAB_PE+1;
                    LOS_Log("TEST 5: WRITE OR READ VALUE WITH WRONG TAB SIZE (size 33)\n");
                    ITE_writeListPE(tab_pe, nb_of_pe);
                    status = ITE_readListPE(read_backtab_pe,nb_of_pe);
                    if(status==0)
                    {
                    LOS_Log("TEST 5: OK TEST PASSED\n");
                    }
                    
                     
                      //3 Test n°3: write on a read pe
                      //-----------------------------------------------------------------------------------------------
                      //Tab of size of 3 with the read only pe at the first place
                      //init tab
                      for(i=0;i<ISPCTL_SIZE_TAB_PE;i++)
                      {
                      tab_pe[i].pe_addr=0;
                      tab_pe[i].pe_data=0;
                      read_backtab_pe[0].pe_addr=0;
                      read_backtab_pe[0].pe_data=0;
                      }
                      
                       // page "ExposureStatus" [read only]
                       tab_pe[0].pe_addr = ExposureStatus_bAlgorithmStatus;                                            //0x1b80
                       read_backtab_pe[0].pe_addr = ExposureStatus_bAlgorithmStatus;                                   //0x1b80
                       tab_pe[0].pe_data=0xA;
                       // page "ExposureControls"
                       tab_pe[1].pe_addr = ExposureControls_bMode;                                                     //0x1b00
                       tab_pe[2].pe_addr = ExposureControls_bMetering;                                                 //0x1b02
                       read_backtab_pe[1].pe_addr = ExposureControls_bMode;                                            //0x1b00
                       read_backtab_pe[2].pe_addr = ExposureControls_bMetering;                                        //0x1b02
                       tab_pe[1].pe_data=0xB;
                       tab_pe[2].pe_data=0xC;
                       nb_of_pe=3;
                       LOS_Log("TEST 6: WRITE 3 VALUES WITH THE FIRST ONE IS READ ONLY REGISTER\n");
                       ITE_writeListPE(tab_pe, nb_of_pe);
                       status = ITE_readListPE(read_backtab_pe,nb_of_pe);
                       LOS_Log("TEST 6 FINISHED\n");
                       
                        //Tab of size of 3 with the read only pe at the 2nd place
                        //init tab
                        for(i=0;i<ISPCTL_SIZE_TAB_PE;i++)
                        {
                        tab_pe[i].pe_addr=0;
                        tab_pe[i].pe_data=0;
                        read_backtab_pe[0].pe_addr=0;
                        read_backtab_pe[0].pe_data=0;
                        }
                        
                         // page "ExposureControls"
                         tab_pe[0].pe_addr = ExposureControls_bMode;                                                     //0x1b00
                         read_backtab_pe[0].pe_addr = ExposureControls_bMode;                                            //0x1b00
                         tab_pe[0].pe_data=0xA;
                         // page "ExposureStatus" [read only]
                         tab_pe[1].pe_addr = ExposureStatus_bAlgorithmStatus;                                            //0x1b80
                         read_backtab_pe[1].pe_addr = ExposureStatus_bAlgorithmStatus;                                   //0x1b80
                         tab_pe[1].pe_data=0xB;
                         // page "ExposureControls"
                         tab_pe[2].pe_addr = ExposureControls_bMetering;                                                 //0x1b02
                         read_backtab_pe[2].pe_addr = ExposureControls_bMetering;                                        //0x1b02
                         tab_pe[2].pe_data=0xC;
                         nb_of_pe=3;
                         LOS_Log("TEST 7: WRITE 3 VALUES WITH THE SECOND ONE IS READ ONLY REGISTER\n");
                         ITE_writeListPE(tab_pe, nb_of_pe);
                         status = ITE_readListPE(read_backtab_pe,nb_of_pe);
                         LOS_Log("TEST 7 FINISHED\n");
                         
                          
                           //Tab of size of 3 with the read only pe at the last place
                           //init tab
                           for(i=0;i<ISPCTL_SIZE_TAB_PE;i++)
                           {
                           tab_pe[i].pe_addr=0;
                           tab_pe[i].pe_data=0;
                           read_backtab_pe[0].pe_addr=0;
                           read_backtab_pe[0].pe_data=0;
                           }
                           
                            // page "ExposureControls"
                            tab_pe[0].pe_addr = ExposureControls_bMode;                                                     //0x1b00
                            tab_pe[1].pe_addr = ExposureControls_bMetering;                                                 //0x1b02
                            read_backtab_pe[0].pe_addr = ExposureControls_bMode;                                            //0x1b00
                            read_backtab_pe[1].pe_addr = ExposureControls_bMetering;                                        //0x1b02
                            tab_pe[0].pe_data=0xA;
                            tab_pe[1].pe_data=0xB;
                            // page "ExposureStatus" [read only]
                            tab_pe[2].pe_addr = ExposureStatus_bAlgorithmStatus;                                            //0x1b80
                            read_backtab_pe[2].pe_addr = ExposureStatus_bAlgorithmStatus;                                   //0x1b80
                            tab_pe[2].pe_data=0xC;
                            nb_of_pe=3;
                            LOS_Log("TEST 8: WRITE 3 VALUES WITH THE LAST ONE IS READ ONLY REGISTER\n");
                            ITE_writeListPE(tab_pe, nb_of_pe);
                            status = ITE_readListPE(read_backtab_pe,nb_of_pe);
                            LOS_Log("TEST 8 FINISHED\n");
                            
                             LOS_Log("Read/Write List Non-regression tests over!\n");
*/
}

void ITE_NonReg_PollingPE(int xSnapshot, int ySnapshot)
{
    LOS_Log("Not supported\n");
    UNUSED(xSnapshot);
    UNUSED(ySnapshot);
    /*
    t_uint16 pe_value;
    
     //##########################################################################################################
     //                                                                                                      HERE THE NON REG TESTs ARE STARTING;
     //##########################################################################################################
     
      // Test n°1: Send Command STOP and waiting for max 300 ms
      //----------------------------------------------------------------------------
      
       ITE_writePE(HostInterfaceManagerControl_bUserCommand, CMD_STOP);
       pe_value = ITE_WaitExpectedPeValue(HostInterfaceManagerStatus_bThisLoLevelState, LOW_LEVEL_STOPPED, 20, 300);
       if(pe_value == LOW_LEVEL_STOPPED)
       {
       LOS_Log("TEST 1 OK\n");
       }
       else
       {
       LOS_Log("TEST 1 FAILED\n");
       }
       
        // Test n°2: Send Command RUN and waiting for max 100 ms
        //------------------------------------------------------------------------------
        
         ITE_writePE(HostInterfaceManagerControl_bUserCommand, CMD_RUN);
         pe_value = ITE_WaitExpectedPeValue(HostInterfaceManagerStatus_bThisLoLevelState, LOW_LEVEL_RUNNING, 1, 100);
         if(pe_value == LOW_LEVEL_RUNNING)
         {
         LOS_Log("TEST 2 OK\n");
         }
         else
         {
         LOS_Log("TEST 2 FAILED\n");
         }
         
          // Test n°3: Write value 0x55 in page element WhiteBalanceControls_bMode and waiting for max 100 ms
          //                  and waiting for a bad value (0xAA)
          //------------------------------------------------------------------------------------------------------------------------------------
          
           ITE_writePE(WhiteBalanceControls_bMode, 0x55);
           LOS_Log("and wait for an unreachable value\n");
           pe_value = ITE_WaitExpectedPeValue(WhiteBalanceControls_bMode, 0xAA, 10, 100);
           if(pe_value == 0x55)
           {
           LOS_Log("TEST 3 OK\n");
           }
           else
           {
           LOS_Log("TEST 3 FAILED\n");
           }
           
            // Test n°4: Write value 0x44 in page element WhiteBalanceControls_bMode and waiting for max 0 ms
            //                actually this is a check value
            //------------------------------------------------------------------------------------------------------------------------------------
            
             ITE_writePE(WhiteBalanceControls_bMode, 0x44);
             pe_value = ITE_WaitExpectedPeValue(WhiteBalanceControls_bMode, 0x44, 2, 0);
             if(pe_value == 0x44)
             {
             LOS_Log("TEST 4 OK\n");
             }
             else
             {
             LOS_Log("TEST 4 FAILED\n");
             }
             
              
               // Test n°5: Write value 0x45 in page element WhiteBalanceControls_bMode and polling frequency = 0
               //                actually this is a check value
               //------------------------------------------------------------------------------------------------------------------------------------
               
                ITE_writePE(WhiteBalanceControls_bMode, 0x45);
                pe_value = ITE_WaitExpectedPeValue(WhiteBalanceControls_bMode, 0x45, 0, 2);
                if(pe_value == 0x45)
                {
                LOS_Log("TEST 5 OK\n");
                }
                else
                {
                LOS_Log("TEST 5 FAILED\n");
                }
                
                 
                  // Test n°6: Write value 0x55 in page element WhiteBalanceControls_bMode and waiting for max 0 ms
                  //                actually this is a check value
                  //------------------------------------------------------------------------------------------------------------------------------------
                  
                   ITE_writePE(WhiteBalanceControls_bMode, 0x55);
                   pe_value = ITE_WaitExpectedPeValue(WhiteBalanceControls_bMode, 0x55, 0, 0);
                   if(pe_value == 0x55)
                   {
                   LOS_Log("TEST 6 OK\n");
                   }
                   else
                   {
                   LOS_Log("TEST 6 FAILED\n");
                   }
                   
                    LOS_Log("Polling Non-regression tests over!\n");
    */
}
