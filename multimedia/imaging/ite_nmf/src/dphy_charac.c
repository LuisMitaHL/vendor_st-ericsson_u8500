/********* DPHY characterization test 8500_V1.0 ************/



#include "dphy_charac.h"
#include "crm.h"
#include "dma_charac.h"
#include "ipp.h"
#include "mmdsp.h"
#include "pictor_full.h"
#include "proj.h"
#include "sia.h"
#include "stbp.h"
#include "sia_register_fct.h"
#include "hi_register_acces.h"
#include <los/api/los_api.h>

#define CHARAC
#define DMA_EOT        (1<<0)
#define SENSOR_ERR     (1<<1)
#define DPHY_ERR       (1<<2)
#define PACKET_ERR     (1<<3)
#define DPHY_TIMEOUT	 (1<<4)
#define CAMERA_ERR     (1<<5)
#define PARAM_ERR      (1<<6)
#define OK 1

#define PIPE0 1<<0
#define PIPE1 1<<1
#define IRP03_L_W   1<<0
#define IRP03_C_W   1<<1
#define IRP03_PRA_W 1<<11
#define IRP1_PRA_W  1<<13
#define YUV420MB_2B 0
#define YUV422R_1B  1


charac_event_status_t hv_top_test_DPHY_charac ( unsigned int frequency, 
                                                unsigned int CSI_if,      /* (0 csi0)  (1 csi1) (2 ccp) */
                                                unsigned int CSI_release, /* 0 = v0.81 / 1 = v0.90 */
                                                unsigned int lane_nb, 
                                                unsigned int swap,
                                                //unsigned int bpp,
												unsigned int address, 
                                                unsigned int increment_add, 
                                                unsigned int width, 
                                                unsigned int height, 
												unsigned int ifmt,        /* Input format: RAW6/7/8/10/12 & compressed mode */
                                                unsigned int ofmt,        /* Output format: RAW8/12 */
                                                unsigned int nb_frames) 
{

  /* Local variables */
  int i;
  unsigned int process        = 0x4000; /* IRP2_PRA_W */
  //unsigned int format         = 0x2A;       /* RAW8 */
  //unsigned int bpp            = 8;          /* 8bpp */
  unsigned int ibpp, tmp;
  unsigned int owidth;
  charac_event_status_t status;

    /* Init status */
  status.global_event   = 0;
  status.sd_error       = 0;
  status.dphy_error     = 0;
  status.dphy0_dbg      = 0;
  status.dphy1_dbg      = 0;
  status.packet_error   = 0;
  status.packet_dbg     = 0;
  status.sd_error_cnt   = 0;
  status.packet_error_cnt = 0;
  status.dphy_error_cnt = 0;

  /* ibpp & ofmt definition */
  switch(ifmt){
    case RAW6:
    case R10C6:
    case R12C6:
      ibpp = 6; break;
    case RAW7:
    case R10C7:
    case R12C7:
      ibpp = 7; break;
    case RAW8:
    case R10C8:
    case R12C8:
      ibpp = 8; break;
    case RAW10:
      ibpp = 10; break;
    case RAW12:
      ibpp = 12; break;
    default:
      ibpp = 8; break;
  }

  /* output width definition */
  if(ofmt == RAW12){
    owidth = ((width * 3/2)+ 23)/24*24;
  }else{
    owidth = (width + 23)/24*24;
  }
  
  for (i=0 ; ( unsigned int)i<nb_frames ; i++){

    /*** SIA clocks config. ***/
    SIA_R_W(SIA_CLOCK_ENABLE, 0x07);

    /*** STBUS PLUG config. ***/
    STBP_R_W(MAX_OPC_SZ8  , 0x0006);
    STBP_R_W(MAX_CHCK_SZ8 , 0x0000);
    STBP_R_W(MAX_MSSG_SZ8 , 0x0001);
    STBP_R_W(PRIO_LEVEL8  , 0x1001);
    STBP_R_W(PROCESS_EN   , STBP_R_R(PROCESS_EN) | (process & 0xFFFF));
    STBP_R_W(PORT_CTRL    , 0x07c0);

    /*** DMA config. ***/
    /* Start adress */
    if (increment_add == 1) {
      DMA_R_W(IRP2_DRS_L,  (address+ i*(height*owidth))        & 0xFFF8);
      DMA_R_W(IRP2_DRS_H, ((address+ i*(height*owidth)) >> 16) & 0xFFFF);


    }else{
      DMA_R_W(IRP2_DRS_L,  (address)        & 0xFFF8);
      DMA_R_W(IRP2_DRS_H, ((address) >> 16) & 0xFFFF);
    }

    /* Window conf. (RAW8) */

    DMA_R_W(IRP2_DFW, owidth);
    DMA_R_W(IRP2_DWW, owidth);
    DMA_R_W(IRP2_DWH, height);

    /* Unmask BMS EOT IT */
    DMA_R_W(IMR2, DMA_R_R(IMR2) | (process & 0xFFFF));

    /* Start */
    DMA_R_W(ENR, process & 0xFFFF);

    if(i==0){
      /*** CRM config. ***/
      CRM_R_W(CHECKER_DETECT_WDW,0x0050);
      CRM_R_W(STATIC_CLK_SD_HISPEED_SOURCE_SEL,0x0);  /* sensor clock */
      CRM_R_W(STATIC_CLK_SD_LOSPEED_SOURCE_SEL,0x2);  /* emulated clock */
      CRM_R_W(STATIC_CLK_PIPE_SOURCE_SEL,0x2);        /* emulated clock */
      CRM_R_W(STATIC_CLK_BMS_SOURCE_SEL,0x0);         /* sensor clock */
      /* clocks division ratio */
      CRM_R_W(CLK_HOST_IPP_DIV,0x2);
      CRM_R_W(CLK_PIPE_IN_DIV,0x1);
      CRM_R_W(CLK_PICTOR_GPIO_DIV,0x2);
      /* clocks enable */
      CRM_R_W(EN_CLK_BMS          , 0x0001);
      CRM_R_W(EN_CLK_SD_HISPEED   , 0x0001);
      CRM_R_W(EN_CLK_SD_LOSPEED   , 0x0001);
      CRM_R_W(EN_CLK_PIPE         , 0x0001);
      CRM_R_W(EN_CLK_PICTOR_GPIO  , 0x0001);
      /* enable interruption */
      CRM_R_W(CHECKER_ITM_BSET,0x3);

      /*** IPP config. ***/
      IPP_R_W(SD_RESET, 0x0001);
      /* CSI2 & DPHY config. */
      if(CSI_if == 0){
        switch (swap) {
          case 321 : IPP_R_W(STATIC_CSI0_DATA_LANES, 3<<10 | 2<<7 | 1<<4 | (lane_nb & 0x3)); break;
          case 312 : IPP_R_W(STATIC_CSI0_DATA_LANES, 3<<10 | 1<<7 | 2<<4 | (lane_nb & 0x3)); break;
          case 231 : IPP_R_W(STATIC_CSI0_DATA_LANES, 2<<10 | 3<<7 | 1<<4 | (lane_nb & 0x3)); break;
          case 213 : IPP_R_W(STATIC_CSI0_DATA_LANES, 2<<10 | 1<<7 | 3<<4 | (lane_nb & 0x3)); break;
          case 132 : IPP_R_W(STATIC_CSI0_DATA_LANES, 1<<10 | 3<<7 | 2<<4 | (lane_nb & 0x3)); break;
          case 123 : IPP_R_W(STATIC_CSI0_DATA_LANES, 1<<10 | 2<<7 | 3<<4 | (lane_nb & 0x3)); break;
          default  : IPP_R_W(STATIC_CSI0_DATA_LANES, 3<<10 | 2<<7 | 1<<4 | (lane_nb & 0x3)); break;
        };
      }
      if(CSI_if == 1){
        IPP_R_W(STATIC_CSI0_DATA_LANES      , (0xD1<<4)|0x1);                /* 1DL without any swap */
      }

      switch (CSI_if) {
        default :
        case 0  : /* CSI0 */   
          IPP_R_W(CSI2_DPHY0_CL_CTRL         , (unsigned int)(2000/frequency)|(CSI_release<<8)); /* we need the truncated result */
          IPP_R_W(CSI2_DPHY0_DL1_CTRL        , 0x8);
          IPP_R_W(CSI2_DPHY0_DL2_CTRL        , 0x8);                          
                  IPP_R_W(CSI2_DPHY0_DL3_CTRL        , 0x8);                          
                  break;
        case 1  : /* CSI0 */   
                  IPP_R_W(CSI2_DPHY1_CL_CTRL         , (unsigned int)(2000/frequency)|(CSI_release<<8)); /* we need the truncated result */
                  IPP_R_W(CSI2_DPHY1_DL1_CTRL        , 0x8);
                  break;
      }

      IPP_R_W(STATIC_CSI2RX_DATA_TYPE_W , (0x12<<8) | (ifmt & 0x3F));
      IPP_R_W(STATIC_CSI2RX_PIX_WIDTH_W , (ibpp << 4) | ibpp);  /* WARNING: SMIA embedded lines */
      IPP_R_W(SD_STATIC_SDG_CSI_MODE    ,  0x0);                /* WARNING: SMIA embedded lines */
      /* Start DPHY compensation processus */
      IPP_R_W(DPHY_COMPENSATION_CTRL,0x1);                                    /* Enable compensation clock */
      IPP_R_W(DPHY_COMPENSATION_CTRL,IPP_R_R(DPHY_COMPENSATION_CTRL) | 0x2);  /* Start the compensation */

      /* Poll on IPP_DPHY_COMPENSATION_STATUS register to detect the end of the compensation */
      while((IPP_R_R(DPHY_COMPENSATION_STATUS) & 0x1) != OK){
#ifndef CHARAC

        unsigned int time;
        get_time(&time);
        PRINTF_LEVEL(DETAIL,("\ttime %d ns, polling on IPP_DPHY_COMPENSATION_STATUS...\n",time));
        advance_time(50);                                     
#endif
      }
#ifndef CHARAC

      PRINTF_LEVEL(DETAIL,("***** The compensation read code is %x.\n",
            (IPP_R_R(DPHY_COMPENSATION_STATUS) & 0xE) ));
#endif
      IPP_R_W(DPHY_COMPENSATION_CTRL,IPP_R_R(DPHY_COMPENSATION_CTRL) & ~0x1); /* Disable compensation clock */

      /* Enable input interface and corresponding interruptions */
      switch (CSI_if) {
        default :
        case 0  : IPP_R_W(STATIC_TOP_IF_SEL,  0x0); break; /* CSI0 */
        case 1  : IPP_R_W(STATIC_TOP_IF_SEL,  0x1); break; /* CSI1 */
        case 2  : IPP_R_W(STATIC_TOP_IF_SEL,  0x2);        /* CCP */
		  IPP_R_W(STATIC_CCP_IF,  ibpp<<8 |1);  	   /* Dataclock */
          CRM_R_W(STATIC_SD_CCP_PIXEL_CLOCK_DIV, ibpp * 2);
		  IPP_R_W(INTERNAL_EN_CLK_CCP,0x1); /* Enable CCP clock  */



      };

      if ( CSI_if == 1 ) {
        IPP_R_W(CSI2_DPHY_ERROR_ITM_BSET    , 0x7000);			// Disable CSI1_ERR_CONTROL 0xF000=>0xC000
        IPP_R_W(CSI2_DPHY0_DBG_ITM_BSET     , 0x0000);
        IPP_R_W(CSI2_DPHY1_DBG_ITM_BSET     , 0x0003);
      } else {
        switch (lane_nb) {
          default :
          case 1 : switch (swap) {
                     default  :
                     case 321 : 
                     case 231 : IPP_R_W(CSI2_DPHY_ERROR_ITM_BSET, 1<<9 | 1<<6 | 1<<3 | 1);
                                IPP_R_W(CSI2_DPHY0_DBG_ITM_BSET,                1<<3 | 1);
                                break;
                     case 312 : 
                     case 132 : IPP_R_W(CSI2_DPHY_ERROR_ITM_BSET, 2<<9 | 2<<6 | 2<<3 | 2);
                                IPP_R_W(CSI2_DPHY0_DBG_ITM_BSET,                2<<3 | 2);
                                break;
                     case 213 : 
                     case 123 : IPP_R_W(CSI2_DPHY_ERROR_ITM_BSET, 4<<9 | 4<<6 | 4<<3 | 4);
                                IPP_R_W(CSI2_DPHY0_DBG_ITM_BSET,                4<<3 | 4);
                                break;
                   }; break;
          case 2 : switch (swap) {
                     default  :
                     case 321 : 
                     case 312 : IPP_R_W(CSI2_DPHY_ERROR_ITM_BSET, 3<<9 | 3<<6 | 3<<3 | 3);
                                IPP_R_W(CSI2_DPHY0_DBG_ITM_BSET,                3<<3 | 3);
                                break;
                     case 231 : 
                     case 213 : IPP_R_W(CSI2_DPHY_ERROR_ITM_BSET, 5<<9 | 5<<6 | 5<<3 | 5);
                                IPP_R_W(CSI2_DPHY0_DBG_ITM_BSET,                5<<3 | 5);
                                break;
                     case 132 : 
                     case 123 : IPP_R_W(CSI2_DPHY_ERROR_ITM_BSET, 6<<9 | 6<<6 | 6<<3 | 6);
                                IPP_R_W(CSI2_DPHY0_DBG_ITM_BSET,                6<<3 | 6);
                                break;
					}; break;
          case 3 :                
                    IPP_R_W(CSI2_DPHY_ERROR_ITM_BSET, 0xFFF); 


                    IPP_R_W(CSI2_DPHY0_DBG_ITM_BSET, 0x3F);
                    break;
                   
        }
      }

      IPP_R_W(CSI2_PACKET_ERROR_ITM_BSET, 0x3);
      IPP_R_W(CSI2_PACKET_DBG_ITM_BSET, 0x1);

      /* DPHY CONTROL ERR disable (ED & v1 workaround) */
      IPP_R_W(CSI2_DPHY_ERROR_ITM_BCLR, 7 << 9);
      IPP_R_W(SD_ERROR_ITS_BCLR, 1 << 11);
      IPP_R_W(CSI2_DPHY_ERROR_ITS_BCLR, 7 << 9);
      /* End of workaround */

      /* BMS config. */
      if(ofmt == RAW8){
        IPP_R_W(SD_BMS_FORMAT, 0x0001);
      }else{
        IPP_R_W(SD_BMS_FORMAT, 0x0000);
      }
      IPP_R_W(SD_BMS_ENABLE_SET, 0x1);
      
	  
	  /* SMIA-RX conf */
      tmp = 0;    
      if ((ifmt == R10C6)||(ifmt == R12C6)||
          (ifmt == R10C7)||(ifmt == R12C7)||
          (ifmt == R10C8)||(ifmt == R12C8))
        tmp |= 1<<12;   /* DCPX enable */

      tmp |= (ibpp & 0xF);    /* Input pix width */
      if ((ifmt == R10C6) ||
          (ifmt == R10C7) ||
          (ifmt == R10C8)){   /* Output range of data */
        tmp |= 0xA << 4;
      } else {
        tmp |= 0xC << 4;
      }
      /* hsync active high */
      tmp |= 0x1 << 9;
      /* vsync active high */
      tmp |= 0x1 << 10;

      if ( CSI_if == 2 ){ // CCP
        IPP_R_W(SD_STATIC_CCP_EN, 0x0001);
      }else{
        tmp |= 1 << 8; /* Enable parrallel interface */
      }
      ISP_R_W(ISP_SMIARX_SETUP,tmp);
	  
	  

      ISP_R_W(ISP_CE0_IDP_GATE_ENABLE,0x1);
      ISP_R_W(ISP_RE_BAYER_LOAD_ENABLE, 0x0001);
      ISP_R_W(ISP_DMCE_BAYER_LOAD_ENABLE, 0x0001);
      ISP_R_W(ISP_BABYLON_ENABLE, 0x0001);
      ISP_R_W(ISP_DMCE_RGB_LOAD_ENABLE, 0x0001);
      ISP_R_W(ISP_CE0_IDP_GATE_ENABLE, 0x0001);
      ISP_R_W(ISP_CE0_IDP_GATE_SELECT, 0x0001);
      ISP_R_W(ISP_CE1_IDP_GATE_ENABLE, 0x0001);
      ISP_R_W(ISP_CE1_IDP_GATE_SELECT, 0x0001);
      ISP_R_W(ISP_BAYER_STORE_SD_CLK_ENABLE, 0x0001);
      ISP_R_W(ISP_BAYER_STORE_SD_CLK_SELECT, 0x0000);
      //ISP_R_W(ISP_SMIARX_CTRL, 0x0001);
      // AB Modif
      ISP_R_W(ISP_SMIARX_CTRL, 0x0000);
      /* Unmask ITs */ 
      IPP_R_W(SD_ERROR_ITM_BSET         , 0x0FFF);
      /* SDG enable */ 
      
      IPP_R_W(DPHY_TOP_IF_EN,     0x1);
    }else{
      IPP_R_W(SD_BMS_ENABLE_SET, 0x1);    
    }

    /* SIA platform specific, BFM enable */
#ifndef CHARAC

    hv_set_mmdsp_gpio_enable(0xFFFF);
    if ( CSI_if == 0 ) { /* CSI0 */
      hv_set_mmdsp_gpio_CSI2_bfm0_enable(1);
    }else{  /* CSI1 & CPP */
      hv_set_mmdsp_gpio_CSI2_bfm1_enable(1);
    }
#endif

    /* Wait for sensor clk */
    if (i == 0){
      while ( (CRM_R_R(CHECKER_ITS) & 0x1) != 0x1 ) {}
      CRM_R_W(CHECKER_ITS_BCLR, 0x0001);  /* Clear IT */
      CRM_R_W(EMUL_SENSOR_CLOCK, 0x0);    /* Sensor mode */
    }
    // AB Modif

    ISP_R_W(ISP_SMIARX_CTRL, 0x0001); /* Rx Start */

    while (status.global_event == 0){
#ifndef CHARAC

      unsigned int time=0;
      get_time(&time);
      PRINTF_LEVEL(DETAIL,("\ttime %d ns, Wait for event...\n",time));
#endif

      /* Wait for DMA IRP2_PRA_W EOT */
      if((DMA_R_R(ENR) & process) == 0) {
        status.global_event |= DMA_EOT;
#ifndef CHARAC

        PRINTF_LEVEL(DETAIL,("\ttime %d ns, End of DMA process...\n",time));

        if(DMA_R_R(FIFO_STA) & process)


          PRINTF_LEVEL(DETAIL,("\ttime %d ns, DMA Fifo not empty...\n",time));
#endif
      }


      /* Wait for sensor error */
      if ((status.sd_error = (IPP_R_R(SD_ERROR_ITS) & IPP_R_R(SD_ERROR_ITM))) != 0){
        status.global_event |= SENSOR_ERR;

        // Clear interrupts registers 
        IPP_R_W(SD_ERROR_ITS_BCLR, 0xFFFF);
#ifndef CHARAC

        PRINTF_LEVEL(DETAIL,("\ttime %d ns, Sensor error (0x%08X) detected...\n",time, status.sd_error));
#endif
        if (status.sd_error & 1<<11){
          status.global_event |= DPHY_ERR;

          status.dphy_error = IPP_R_R(CSI2_DPHY_ERROR_ITS) & IPP_R_R(CSI2_DPHY_ERROR_ITM);
          status.dphy0_dbg  = IPP_R_R(CSI2_DPHY0_DBG_ITS) & IPP_R_R(CSI2_DPHY0_DBG_ITM);
          status.dphy1_dbg  = IPP_R_R(CSI2_DPHY1_DBG_ITS) & IPP_R_R(CSI2_DPHY1_DBG_ITM);
          // Clear interrupts registers 
          IPP_R_W(CSI2_DPHY_ERROR_ITS_BCLR, 0xFFFF);
          IPP_R_W(CSI2_DPHY0_DBG_ITS_BCLR, 0xFFFF);
          IPP_R_W(CSI2_DPHY1_DBG_ITS_BCLR, 0xFFFF);
#ifndef CHARAC

          PRINTF_LEVEL(DETAIL,("\ttime %d ns, CSI2 DPHY error (0x%08X) detected...\n",time,status.dphy_error));
#endif
        }
        if (status.sd_error & 1<<10){

          status.global_event |= PACKET_ERR;
          status.packet_error = IPP_R_R(CSI2_PACKET_ERROR_ITS) & IPP_R_R(CSI2_PACKET_ERROR_ITM);
          status.packet_dbg   = IPP_R_R(CSI2_PACKET_DBG_ITS) & IPP_R_R(CSI2_PACKET_DBG_ITM);
          // Clear interrupts registers 
          IPP_R_W(CSI2_PACKET_ERROR_ITS_BCLR, 0xFFFF);
          IPP_R_W(CSI2_PACKET_DBG_ITS_BCLR, 0xFFFF);
#ifndef CHARAC

          PRINTF_LEVEL(DETAIL,("\ttime %d ns, CSI2 DPHY packet error (0x%08X) detected...\n",time, status.packet_error));
#endif
        }

        // BMS & DMA reset 
        IPP_R_W(SD_BMS_ENABLE_CLR, 0x0001);   /* Stop BMS */
        DMA_R_W(SOFT_RST, 1 << 11 );			    /* Stop PRA_W process */
 //       ISP_R_W(ISP_SMIARX_CTRL, 0x10000); // Rx Abort 
 //       ISP_R_W(ISP_SMIARX_CTRL, 0); 
	return status;

      }
    }
  }

  return status;
}


/* Defines */
#define PIPE0 1<<0
#define PIPE1 1<<1
#define IRP03_PRA_W 1<<11
#define IRP0_PRA_R  1<<12
#define IRP1_PRA_W  1<<13

int sia_bml_grab ( unsigned int clock_div,       /* 0x8=200MHz, 0x9=170MHz, 0xA=150MHz, 0xC=133LHz, 0xD=120MHz, 0xE=114MHz, 0x10=100MHz, 0x14=80MHz, 0x1C=57MHz */
                    unsigned int pipe_sel,        /* PIPE0=0x1, PIPE1=0x2 */
                    unsigned int in_sadd,      
                    unsigned int frame_width,     /* Must be a multiple of 24pix */ 
                    unsigned int frame_height,    /* Without embedded lines */
                    unsigned int out0_sadd, 
		    unsigned int out0_width,
		    unsigned int out0_height,
                    unsigned int out1_sadd, 
		    unsigned int out1_width,
		    unsigned int out1_height,
                    int      increment_add,   /* 0=No increment, 1=increment */ 
                    int      nb_frames,
                    int      embedded_line)   /* Nb of embedded lines */
{

  /* Local variables */
  int i;
  unsigned int process;
  
  unsigned int in_frame_sadd      = in_sadd + embedded_line * frame_width; /* Drop embedded lines */
  unsigned int out0_frame_width   = (unsigned int)((out0_width)/8*8);
  unsigned int out0_frame_height  = (unsigned int)(out0_height);
  unsigned int out1_frame_width   = (unsigned int)((out1_width)/8*8);
  unsigned int out1_frame_height  = (unsigned int)(out1_height);
 // unsigned int out0_frame_width   = (unsigned int)((frame_width - 4)/8*8);
 // unsigned int out0_frame_height  = (unsigned int)(frame_height - 4);
 // unsigned int out1_frame_width   = (unsigned int)((frame_width - 4)/8*8);
 // unsigned int out1_frame_height  = (unsigned int)(frame_height - 4);
   
  /* Function */
  for (i=0 ; i<nb_frames ; i++){
    /*** SIA clocks config. ***/
    SIA_R_W(SIA_CLOCK_ENABLE, 0x07);
   
    /*** STBUS PLUG config. ***/
    /* Dma process definition */
    process = IRP0_PRA_R;;
    if(pipe_sel & PIPE0) process |= IRP03_PRA_W;
    if(pipe_sel & PIPE1) process |= IRP1_PRA_W;
    /* ipipe0 */
    STBP_R_W(MAX_OPC_SZ2,0x6);
    STBP_R_W(MAX_CHCK_SZ2,0x0);
    STBP_R_W(MAX_MSSG_SZ2,0x1);
    /* opipe03_lraster */
    STBP_R_W(MAX_OPC_SZ4,0x6);
    STBP_R_W(MAX_CHCK_SZ4,0x0);
    STBP_R_W(MAX_MSSG_SZ4,0x1);
    /* opipe1 */
    STBP_R_W(MAX_OPC_SZ7,0x6);
    STBP_R_W(MAX_CHCK_SZ7,0x0);
    STBP_R_W(MAX_MSSG_SZ7,0x2);

    //STBP_R_W(PROCESS_EN   , STBP_R_R(PROCESS_EN) | (process & 0xFFFF));
    STBP_R_W(PROCESS_EN   ,  process & 0xFFFF);
    //STBP_R_W(PORT_CTRL    , 0x07c0);

    /*** DMA config. ***/
    /** iPipe0 configuration **/
    /* Start add conf. */
    DMA_R_W(IRP0_SRS_L,  (in_frame_sadd)        & 0xFFF8);
    DMA_R_W(IRP0_SRS_H, ((in_frame_sadd) >> 16) & 0xFFFF);
    /* Frame size conf. */
    DMA_R_W(IRP0_SFW, frame_width);
    DMA_R_W(IRP0_SWW, frame_width);
    DMA_R_W(IRP0_SWH, frame_height);

    /* oPipe0 configuration */
    if(pipe_sel & PIPE0){
      /* Start add conf. */
      if (increment_add == 1) {
        DMA_R_W(IRP3_DRS_L,  (out0_sadd+ i*(out0_frame_height*out0_frame_width))        & 0xFFF8);
        DMA_R_W(IRP3_DRS_H, ((out0_sadd+ i*(out0_frame_height*out0_frame_width)) >> 16) & 0xFFFF);
      }else{
        DMA_R_W(IRP3_DRS_L,  (out0_sadd)        & 0xFFF8);
        DMA_R_W(IRP3_DRS_H, ((out0_sadd) >> 16) & 0xFFFF);
      }
      /* Frame size conf. */
      DMA_R_W(IRP03_DFW, out0_frame_width*2);
      DMA_R_W(IRP03_DWW, out0_frame_width*2);
      DMA_R_W(IRP03_DWH, out0_frame_height);
    }

    /* oPipe1 configuration */
    if(pipe_sel & PIPE1){
      /* Start add conf. */
      if (increment_add == 1) {
        DMA_R_W(IRP1_DRS_L,  (out1_sadd+ i*(out1_frame_height*out1_frame_width))        & 0xFFF8);
        DMA_R_W(IRP1_DRS_H, ((out1_sadd+ i*(out1_frame_height*out1_frame_width)) >> 16) & 0xFFFF);
      }else{
        DMA_R_W(IRP1_DRS_L,  (out1_sadd)        & 0xFFF8);
        DMA_R_W(IRP1_DRS_H, ((out1_sadd) >> 16) & 0xFFFF);
      }
      /* Frame size conf. */
      DMA_R_W(IRP1_DFW, out1_frame_width*2);
      DMA_R_W(IRP1_DWW, out1_frame_width*2);
      DMA_R_W(IRP1_DWH, out1_frame_height);
    }

    /** Unmask BMS EOT IT **/
    //DMA_R_W(IMR2, DMA_R_R(IMR2) | (process & 0xFFFF));
    DMA_R_W(IMR2, (process & 0xFFFF));

    /** Start DMA **/
    //DMA_R_W(ENR, process & 0xFFFF);

    if(i==0){
      /*** CRM config. ***/
      CRM_R_W(CHECKER_DETECT_WDW,0x0050);
      CRM_R_W(STATIC_CLK_SD_HISPEED_SOURCE_SEL,0x2);  /* emulated clock */
      CRM_R_W(STATIC_CLK_SD_LOSPEED_SOURCE_SEL,0x2);  /* emulated clock */
      CRM_R_W(STATIC_CLK_PIPE_SOURCE_SEL,0x1);        /* pipe_in  clock */
      CRM_R_W(STATIC_CLK_BMS_SOURCE_SEL,0x2);         /* emulated clock */
      /* clocks division ratio */
      CRM_R_W(CLK_HOST_IPP_DIV,0x2);
      CRM_R_W(CLK_PIPE_IN_DIV,clock_div);
      CRM_R_W(CLK_PICTOR_GPIO_DIV,0x2);
      /* clocks enable */
      CRM_R_W(EN_CLK_BML          , 0x0001);
      /*CRM_R_W(EN_CLK_BMS          , 0x0001);
      CRM_R_W(EN_CLK_SD_HISPEED   , 0x0001);
      CRM_R_W(EN_CLK_SD_LOSPEED   , 0x0001);*/
      CRM_R_W(EN_CLK_PIPE         , 0x0001);
      CRM_R_W(EN_CLK_PIPE_IN      , 0x0001);
      CRM_R_W(EN_CLK_PICTOR_GPIO  , 0x0001);
      /* enable interruption */
      CRM_R_W(CHECKER_ITM_BSET,0x3);

      /*** IPP config. ***/
      // IPP_R_W(SD_RESET, 0x0001);
      /** BML specific. config. (After RE RAW8) **/
      IPP_R_W(SD_STATIC_LOAD_FORMAT, 0x5);              /* RAW8 after RE */
      ISP_R_W(ISP_DMCE_BAYER_LOAD_ENABLE, 0x1);         /* enable ISP_DMCE_BAYER_LOAD */
      ISP_R_W(ISP_DMCE_BAYER_LOAD_SELECT, 0x1);         /* Bayer load 2 from figure 10 */
      IPP_R_W(SD_STATIC_LOAD_LINETYPE0, 0x1 <<12 | 1);  /* line of embedded data */
      IPP_R_W(SD_STATIC_LOAD_LINETYPE1, 0x3 <<12 | 0);  
      IPP_R_W(SD_STATIC_LOAD_LINETYPE2, 0xD <<12 | (frame_height-1)); 
      IPP_R_W(SD_STATIC_LOAD_LINETYPE3, 0xE <<12 | 1);
      IPP_R_W(SD_STATIC_LOAD_LINETYPE4, 0x3 <<12 | 0xF);
      IPP_R_W(SD_STATIC_LOAD_LINE_TIME_LSB_EN, ((frame_width)<<1)+1);
      IPP_R_W(SD_STATIC_LOAD_LINE_TIME_MSB,    ((frame_width)>>15)&0xFF);
      IPP_R_W(SD_STATIC_LOAD_WW, frame_width);
      IPP_R_W(SD_STATIC_MEM_LOAD_EN, 0x1);
      /** PRY config. **/
      IPP_R_W(SD_PRY0_FORMAT,     0x1 << 8);  /* YUV422R_1B */
      IPP_R_W(SD_PRY1_FORMAT,     0x1 << 8);  /* YUV422R_1B */
      /** ISP config. **/
      /* RE config. */
      ISP_R_W(ISP_RE_BAYER_LOAD_ENABLE,   0x1);
      ISP_R_W(ISP_RE_BAYER_LOAD_SELECT,   0x1);
      /* DMCE config. */
      ISP_R_W(ISP_DMCE_BAYER_LOAD_ENABLE, 0x1);
      ISP_R_W(ISP_BABYLON_ENABLE,         0x1);
      ISP_R_W(ISP_DMCE_RGB_LOAD_ENABLE,   0x1);
      /* CE config. */
      ISP_R_W(ISP_CE0_IDP_GATE_ENABLE,0x1);
      if ((pipe_sel & PIPE0) == PIPE0) {
        /* Pipe0 is used */
        ISP_R_W(ISP_CE0_IDP_GATE_SELECT,0x0);
      }
      else {
        /* Pipe0 is not used : avoid useless toggleing */
        ISP_R_W(ISP_CE0_IDP_GATE_SELECT,0x1);
      }

      ISP_R_W(ISP_CE1_IDP_GATE_ENABLE,0x1);
      if ((pipe_sel & PIPE1) == PIPE1) {
        /* Pipe1 is used */
        ISP_R_W(ISP_CE1_IDP_GATE_SELECT,0x0);
      }
      else {
        /* Pipe1 is not ised : avoid useless toggleing */
        ISP_R_W(ISP_CE1_IDP_GATE_SELECT,0x1);
      }

      /* enable mux */
      if ((pipe_sel & PIPE0) == PIPE0) {
        ISP_R_W(ISP_CE0_MUX_GPS_ENABLE,0x1);
        ISP_R_W(ISP_CE0_MUX_SHARP_ENABLE,0x1);
        ISP_R_W(ISP_CE0_MUX_UNSHARP_ENABLE,0x1);
        ISP_R_W(ISP_CE0_MUX_MATRIX_ADSOC_ENABLE,0x1);
      }

      if ((pipe_sel & PIPE1) == PIPE1) {
        ISP_R_W(ISP_CE1_MUX_GPS_ENABLE,0x1);
        ISP_R_W(ISP_CE1_MUX_SHARP_ENABLE,0x1);
        ISP_R_W(ISP_CE1_MUX_UNSHARP_ENABLE,0x1);
        ISP_R_W(ISP_CE1_MUX_MATRIX_ADSOC_ENABLE,0x1);
      }

      /*CE Modules Enable */
      if ((pipe_sel & PIPE0) == PIPE0) {

        /* MATRIX register to follow Cmodel configuration */
        ISP_R_W(ISP_CE0_MATRIX_ENABLE,1);
        ISP_R_W(ISP_CE0_MATRIX_RCOF00,1024);
        ISP_R_W(ISP_CE0_MATRIX_RCOF01,0);
        ISP_R_W(ISP_CE0_MATRIX_RCOF02,0);
        ISP_R_W(ISP_CE0_MATRIX_RCOF10,0);
        ISP_R_W(ISP_CE0_MATRIX_RCOF11,1024);
        ISP_R_W(ISP_CE0_MATRIX_RCOF12,0);
        ISP_R_W(ISP_CE0_MATRIX_RCOF20,0);
        ISP_R_W(ISP_CE0_MATRIX_RCOF21,0);
        ISP_R_W(ISP_CE0_MATRIX_RCOF22,1024);
        ISP_R_W(ISP_CE0_MATRIX_R_OFFSET,0);
        ISP_R_W(ISP_CE0_MATRIX_G_OFFSET,0);
        ISP_R_W(ISP_CE0_MATRIX_B_OFFSET,0);

        /* YUV CODER configuration to follow Cmodel configuration */
        ISP_R_W(ISP_CE0_CODER_Y_COF00,(t_sint16) 76);
        ISP_R_W(ISP_CE0_CODER_Y_COF01,(t_sint16) 149);
        ISP_R_W(ISP_CE0_CODER_Y_COF02,(t_sint16) 29);

        ISP_R_W(ISP_CE0_CODER_CB_COF10,(t_sint16) -43);
        ISP_R_W(ISP_CE0_CODER_CB_COF11,(t_sint16) -84);
        ISP_R_W(ISP_CE0_CODER_CB_COF12,(t_sint16) 127);

        ISP_R_W(ISP_CE0_CODER_CR_COF20,(t_sint16) 127);
        ISP_R_W(ISP_CE0_CODER_CR_COF21,(t_sint16) -106);
        ISP_R_W(ISP_CE0_CODER_CR_COF22,(t_sint16) -21);

        ISP_R_W(ISP_CE0_CODER_CH0_MIN_CLIP,0);
        ISP_R_W(ISP_CE0_CODER_CH1_MIN_CLIP,0);
        ISP_R_W(ISP_CE0_CODER_CH2_MIN_CLIP,0);

        ISP_R_W(ISP_CE0_CODER_CH0_MAX_CLIP,1023);
        ISP_R_W(ISP_CE0_CODER_CH1_MAX_CLIP,1023);
        ISP_R_W(ISP_CE0_CODER_CH2_MAX_CLIP,1023);

        ISP_R_W(ISP_CE0_CODER_YFLOOR,0);
        ISP_R_W(ISP_CE0_CODER_CBFLOOR,128);
        ISP_R_W(ISP_CE0_CODER_CRFLOOR,128);

        ISP_R_W(ISP_CE0_CODER_FORMAT,0x2); /* configure YUVCODER to YUV422 instead of RGB */
        ISP_R_W(ISP_CE0_CODER_ENABLE,0x1);
        /* diseable dither ! that is enable by default */
        ISP_R_W(ISP_CE0_DITHER_ENABLE,0);

        /* CE0 Crop */
        ISP_R_W(ISP_CE0_CROP_H_START,       0);
        ISP_R_W(ISP_CE0_CROP_V_START,       0);
        ISP_R_W(ISP_CE0_CROP_H_SIZE,        out0_frame_width);
        ISP_R_W(ISP_CE0_CROP_V_SIZE,        out0_frame_height);
        ISP_R_W(ISP_CE0_CROP_IACTIVE_PIXS,  frame_width-4);
        ISP_R_W(ISP_CE0_CROP_IACTIVE_LINES, frame_height-4);
        ISP_R_W(ISP_CE0_CROP_SOF_RETIMING,  0);
        ISP_R_W(ISP_CE0_CROP_ENABLE,        0x1);
      }
      if ((pipe_sel & PIPE1) == PIPE1) {

        /* MATRIX register to follow Cmodel configuration */
        ISP_R_W(ISP_CE1_MATRIX_ENABLE,1);
        ISP_R_W(ISP_CE1_MATRIX_RCOF00,1024);
        ISP_R_W(ISP_CE1_MATRIX_RCOF01,0);
        ISP_R_W(ISP_CE1_MATRIX_RCOF02,0);
        ISP_R_W(ISP_CE1_MATRIX_RCOF10,0);
        ISP_R_W(ISP_CE1_MATRIX_RCOF11,1024);
        ISP_R_W(ISP_CE1_MATRIX_RCOF12,0);
        ISP_R_W(ISP_CE1_MATRIX_RCOF20,0);
        ISP_R_W(ISP_CE1_MATRIX_RCOF21,0);
        ISP_R_W(ISP_CE1_MATRIX_RCOF22,1024);
        ISP_R_W(ISP_CE1_MATRIX_R_OFFSET,0);
        ISP_R_W(ISP_CE1_MATRIX_G_OFFSET,0);
        ISP_R_W(ISP_CE1_MATRIX_B_OFFSET,0);

        ISP_R_W(ISP_CE1_CODER_Y_COF00,(t_sint16) 76);
        ISP_R_W(ISP_CE1_CODER_Y_COF01,(t_sint16) 149);
        ISP_R_W(ISP_CE1_CODER_Y_COF02,(t_sint16) 29);

        ISP_R_W(ISP_CE1_CODER_CB_COF10,(t_sint16) -43);
        ISP_R_W(ISP_CE1_CODER_CB_COF11,(t_sint16) -84);
        ISP_R_W(ISP_CE1_CODER_CB_COF12,(t_sint16) 127);

        ISP_R_W(ISP_CE1_CODER_CR_COF20,(t_sint16) 127);
        ISP_R_W(ISP_CE1_CODER_CR_COF21,(t_sint16) -106);
        ISP_R_W(ISP_CE1_CODER_CR_COF22,(t_sint16) -21);

        ISP_R_W(ISP_CE1_CODER_CH0_MIN_CLIP,0);
        ISP_R_W(ISP_CE1_CODER_CH1_MIN_CLIP,0);
        ISP_R_W(ISP_CE1_CODER_CH2_MIN_CLIP,0);

        ISP_R_W(ISP_CE1_CODER_CH0_MAX_CLIP,1023);
        ISP_R_W(ISP_CE1_CODER_CH1_MAX_CLIP,1023);
        ISP_R_W(ISP_CE1_CODER_CH2_MAX_CLIP,1023);

        ISP_R_W(ISP_CE1_CODER_YFLOOR,0);
        ISP_R_W(ISP_CE1_CODER_CBFLOOR,128);
        ISP_R_W(ISP_CE1_CODER_CRFLOOR,128);

        ISP_R_W(ISP_CE1_CODER_FORMAT,0x2); /* configure YUVCODER to YUV422 instead of RGB */
        ISP_R_W(ISP_CE1_CODER_ENABLE,0x1);
        /* disable dither ! that is enable by default */
        ISP_R_W(ISP_CE1_DITHER_ENABLE,0);

        /* CE1 Crop */
        ISP_R_W(ISP_CE1_CROP_H_START,       0);
        ISP_R_W(ISP_CE1_CROP_V_START,       0);
        ISP_R_W(ISP_CE1_CROP_H_SIZE,        out1_frame_width);
        ISP_R_W(ISP_CE1_CROP_V_SIZE,        out1_frame_height);
        ISP_R_W(ISP_CE1_CROP_IACTIVE_PIXS,  frame_width-4);
        ISP_R_W(ISP_CE1_CROP_IACTIVE_LINES, frame_height-4);
        ISP_R_W(ISP_CE1_CROP_SOF_RETIMING,  0);
        ISP_R_W(ISP_CE1_CROP_ENABLE,        0x1);
      }
      /* OPF config. */
      ISP_R_W(ISP_OPF0_ENABLE,0x1);
      ISP_R_W(ISP_OPF0_CONTROL,0x1C00 | (5<<13));
      ISP_R_W(ISP_OPF0_PIXVALID_LTYPES, (1<<5));
      ISP_R_W(ISP_OPF0_YCBCR_FLIP,0x0);
      ISP_R_W(ISP_OPF0_HSYNC_RISING,0x0);
      ISP_R_W(ISP_OPF0_HSYNC_FALLING,out0_frame_width);
      ISP_R_W(ISP_OPF0_VSYNC_RISING_COARSE,0x0);
      ISP_R_W(ISP_OPF0_VSYNC_RISING_FINE,0x0);
      //ISP_R_W(ISP_OPF0_VSYNC_FALLING_COARSE,out0_frame_height + 6 - 1);
      ISP_R_W(ISP_OPF0_VSYNC_FALLING_COARSE,out0_frame_height + 5 + embedded_line  - 1);
	  ISP_R_W(ISP_OPF0_VSYNC_FALLING_FINE,0x1);
      ISP_R_W(ISP_OPF0_HCROP_RISING,0x0);
      ISP_R_W(ISP_OPF0_HCROP_FALLING,out0_frame_width-1);
      ISP_R_W(ISP_OPF0_VCROP_RISING_COARSE,0x0);
      //ISP_R_W(ISP_OPF0_VCROP_FALLING_COARSE,out0_frame_height + 6 -1);
	  ISP_R_W(ISP_OPF0_VCROP_FALLING_COARSE,out0_frame_height + 5 + embedded_line -1);
      ISP_R_W(ISP_OPF0_LAST_NONACT_LINE,0x0);
      ISP_R_W(ISP_OPF1_ENABLE,0x1);
      ISP_R_W(ISP_OPF1_CONTROL,0x1C00 | (5<<13));
      ISP_R_W(ISP_OPF1_PIXVALID_LTYPES, (1<<5));
      ISP_R_W(ISP_OPF1_YCBCR_FLIP,0x0);
      ISP_R_W(ISP_OPF1_HSYNC_RISING,0x0);
      ISP_R_W(ISP_OPF1_HSYNC_FALLING,out1_frame_width);
      ISP_R_W(ISP_OPF1_VSYNC_RISING_COARSE,0x0);
      ISP_R_W(ISP_OPF1_VSYNC_RISING_FINE,0x0);
      //ISP_R_W(ISP_OPF1_VSYNC_FALLING_COARSE,out1_frame_height + 6 - 1);
	  ISP_R_W(ISP_OPF1_VSYNC_FALLING_COARSE,out1_frame_height + 5 + embedded_line - 1);
      ISP_R_W(ISP_OPF1_VSYNC_FALLING_FINE,0x1);
      ISP_R_W(ISP_OPF1_HCROP_RISING,0x0);
      ISP_R_W(ISP_OPF1_HCROP_FALLING,out1_frame_width - 1);
      ISP_R_W(ISP_OPF1_VCROP_RISING_COARSE,0x0);
      //ISP_R_W(ISP_OPF1_VCROP_FALLING_COARSE,out1_frame_height + 6 - 1);
	  ISP_R_W(ISP_OPF1_VCROP_FALLING_COARSE,out1_frame_height + 5 + embedded_line - 1);
      ISP_R_W(ISP_OPF1_LAST_NONACT_LINE,0x0);
    }
    /* Start PRY */
    IPP_R_W(SD_PRY0_ENABLE_SET, 1);
    IPP_R_W(SD_PRY1_ENABLE_SET, 1);

    //IPP_R_W(SD_STATIC_LOAD_BML_PIXEL_ORDER,2);

    
    /** Start DMA **/
    DMA_R_W(ENR, process & 0xFFFF);

    /* Wait for DMA EOT */
    while((DMA_R_R(ENR) & process) != 0){
#ifndef CHARAC
        unsigned int time=0;
        get_time(&time);
        PRINTF_LEVEL(DETAIL,("\ttime %d ns,wait for DMA EOT...\n",time));
        advance_time(500);   
#endif  
      }
    if(DMA_R_R(FIFO_STA) & process){
#ifndef CHARAC
      printf("\tERROR: DMA fifo not empty...\n");
#endif
      return 0;
    }
#ifndef CHARAC
    printf("\tEnd of DMA process...\n");
#endif
  }
  return 1;
}

charac_event_status_t sia_grab (unsigned int frequency, 
                                unsigned int CSI_if,          /* (0 csi0) (1 csi1) (2 ccp) */
                                unsigned int CSI_release,     /* 0 = v0.81 / 1 = v0.90 */
                                unsigned int lane_nb,         /* In CSI0 mode only */
                                unsigned int swap,            /* In CSI0 mode only */
                                unsigned int pipe_sel,        /* PIPE0=0x1, PIPE1=0x2 */
                                unsigned int frame_width,     /* Must be a multiple of 16pix */ 
                                unsigned int frame_height,    /* Without embedded lines */
                                unsigned int out0_sadd, 
                                unsigned int out0_width,      /* Must be a 8pix aligned */
                                unsigned int out0_height,     /* Must be 4 pix less than frame_width */
                                unsigned int out1_sadd, 
                                unsigned int out1_width,      /* Must be a 8pix aligned */
                                unsigned int out1_height,     /* Must be 4 pix less than frame_width */
                                int      increment_add,   /* 0=No increment, 1=increment */ 
                                int      nb_frames,
                                int      embedded_line,   /* Nb of embedded lines */
								int		 hr_fmt)          /* Pipe HR format    YUV420MB_2B or YUV422R_1B   */ 
{
  /* Local variables */
  int i;
  unsigned int process = 0;  //__NO_WARNING__ not being set before usage
  unsigned int format = 0x2A;       /* RAW8 */
  unsigned int bpp    = 8;          /* 8bpp */
  charac_event_status_t status;
  int param_err = 0;
  unsigned long time0 = 0,time1;  //__NO_WARNING__ not being set before usage
  t_uint32 maesuredFrameRate;
  
  /* Check parameters */
  if(frame_width%(2) !=0) {
/*
#ifdef SIA_DEBUG
    PRINTF_LEVEL(DETAIL, ("\tERROR: Frame width is not 16pix aligned.\n"));
#endif
*/
    LOS_Log("Input Frame width is not a multiple of 2 \n", NULL, NULL, NULL, NULL, NULL, NULL);
    param_err ++;
  }
  if((out0_width%(4) != 0) || (out0_width>(frame_width-4))) {
/*
#ifdef SIA_DEBUG
    PRINTF_LEVEL(DETAIL, ("\tERROR: Output0 width is not 8pix aligned or bigger than frame width.\n"));
#endif
*/
   ///should be out0_width%(16) for MB format
   LOS_Log("Incorrect Width requested on HR Buffer \n", NULL, NULL, NULL, NULL, NULL, NULL);
    param_err ++;
  }
  if((out1_width%(4) != 0) || (out1_width>(frame_width-4))) {
/*
  #ifdef SIA_DEBUG
    PRINTF_LEVEL(DETAIL, ("\tERROR: Output1 width is not 8pix aligned or bigger than frame width.\n"));
#endif
*/
    LOS_Log("Incorrect Width requested on LR Buffer \n", NULL, NULL, NULL, NULL, NULL, NULL);
    param_err ++;
  }
  if(out0_height > frame_height-4) {
/*
  #ifdef SIA_DEBUG
    PRINTF_LEVEL(DETAIL, ("\tERROR: Output0 Frame height is bigger than frame height - 4.\n"));
#endif
*/
    LOS_Log("Incorrect Height requested on HR Buffer \n", NULL, NULL, NULL, NULL, NULL, NULL);
    param_err ++;
  }
  if(out1_height > frame_height-4) {
/*
#ifdef SIA_DEBUG
    PRINTF_LEVEL(DETAIL, ("\tERROR: Output1 Frame height is bigger than frame height - 4.\n"));
#endif
*/
    LOS_Log("Incorrect Height requested on LR Buffer \n", NULL, NULL, NULL, NULL, NULL, NULL);
	param_err ++;
  }

  if(param_err){
    status.global_event = PARAM_ERR;
    return status;
  }
   
  
  
  /* Function */
  for (i=0 ; i<nb_frames ; i++){
    
	
	/* Init status */
    status.global_event   = 0;
    status.sd_error       = 0;
    status.dphy_error     = 0;
    status.dphy0_dbg      = 0;
    status.dphy1_dbg      = 0;
    status.packet_error   = 0;
    status.packet_dbg     = 0;

    //DG fix
	if(i==0){
	
	/*** SIA clocks config. ***/
    SIA_R_W(SIA_CLOCK_ENABLE, 0x07);

    /*** STBUS PLUG config. ***/
    /* Dma process definition */
    process =0;
    if(pipe_sel & PIPE0){
     switch(hr_fmt){
       case YUV420MB_2B:
        process |= IRP03_L_W | IRP03_C_W;
        break;
       case YUV422R_1B:
          process |= IRP03_PRA_W ;
          break;
       default:
          #ifndef CHARAC
		  PRINTF_LEVEL(DETAIL, ("\tERROR: Pipe HR format not supported\n"));
		  #endif
		  break;
     }
    }
    if(pipe_sel & PIPE1) process |= IRP1_PRA_W;
    if(hr_fmt == YUV420MB_2B){
      /* opipe03_lraster */
      STBP_R_W(MAX_OPC_SZ4,0x3); 
      STBP_R_W(MAX_CHCK_SZ4,0x0);
      STBP_R_W(MAX_MSSG_SZ4,0x1);
      STBP_R_W(PRIO_LEVEL4,0x1001);
      /* opipe03_cc1 */
      STBP_R_W(MAX_OPC_SZ5,0x3); 
      STBP_R_W(MAX_CHCK_SZ5,0x0);
      STBP_R_W(MAX_MSSG_SZ5,0x1);
    }else{
      /* opipe03_lraster */
      STBP_R_W(MAX_OPC_SZ4,0x6);
      STBP_R_W(MAX_CHCK_SZ4,0x0);
      STBP_R_W(MAX_MSSG_SZ4,0x1);
    }
    /* opipe1 */
    STBP_R_W(MAX_OPC_SZ7,0x6);
    STBP_R_W(MAX_CHCK_SZ7,0x0);
    STBP_R_W(MAX_MSSG_SZ7,0x2);

    STBP_R_W(PROCESS_EN   , STBP_R_R(PROCESS_EN) | (process & 0xFFFF));
    STBP_R_W(PORT_CTRL    , 0x07c0);  /* All process in DDR */

    /*** DMA config. ***/
    /* oPipe0 configuration */
    if(pipe_sel & PIPE0){
      /* Start add conf. */
      if(hr_fmt == YUV420MB_2B){
        if (increment_add == 1) {
          DMA_R_W(IRP03_DLS_L,  (out0_sadd+ i*(out0_height*out0_width)*3/2)        & 0xFFF8);
          DMA_R_W(IRP03_DLS_H, ((out0_sadd+ i*(out0_height*out0_width)*3/2) >> 16) & 0xFFFF);
          DMA_R_W(IRP03_DCS_L,  (out0_sadd+ (i+1)*(out0_height*out0_width))        & 0xFFF8);
          DMA_R_W(IRP03_DCS_H, ((out0_sadd+ (i+1)*(out0_height*out0_width)) >> 16) & 0xFFFF);
        }else{
          DMA_R_W(IRP03_DLS_L,  (out0_sadd)        & 0xFFF8);
          DMA_R_W(IRP03_DLS_H, ((out0_sadd) >> 16) & 0xFFFF);
          DMA_R_W(IRP03_DCS_L,  (out0_sadd+ 1*(out0_height*out0_width))        & 0xFFF8);
          DMA_R_W(IRP03_DCS_H, ((out0_sadd+ 1*(out0_height*out0_width)) >> 16) & 0xFFFF);
        }
        /* Frame size conf. */
        DMA_R_W(IRP03_DFW, out0_width);   /* YUV420MB */
        DMA_R_W(IRP03_DWW, out0_width);   /* YUV420MB */
        DMA_R_W(IRP03_DWH, out0_height);
		//DMA_R_W(IRP03_DWH, out0_height-1);
      }else{
        if (increment_add == 1) {
          DMA_R_W(IRP3_DRS_L,  (out0_sadd+ i*(out0_height*out0_width)*2)        & 0xFFF8);
          DMA_R_W(IRP3_DRS_H, ((out0_sadd+ i*(out0_height*out0_width)*2) >> 16) & 0xFFFF);
        }else{
          DMA_R_W(IRP3_DRS_L,  (out0_sadd)        & 0xFFF8);
          DMA_R_W(IRP3_DRS_H, ((out0_sadd) >> 16) & 0xFFFF);        
        }
        /* Frame size conf. */
        DMA_R_W(IRP03_DFW, out0_width*2);   /* YUV422R_1B */
        DMA_R_W(IRP03_DWW, out0_width*2);   /* YUV422R_1B */
        //DG Fix: grab one line less as one line is missing in the buffer and prevent grab'ends
		DMA_R_W(IRP03_DWH, out0_height);
		//DMA_R_W(IRP03_DWH, out0_height-1);
      }
    }

    /* oPipe1 configuration */
    if(pipe_sel & PIPE1){
      /* Start add conf. */
      if (increment_add == 1) {
        DMA_R_W(IRP1_DRS_L,  (out1_sadd+ i*(out1_height*out1_width*2))        & 0xFFF8);
        DMA_R_W(IRP1_DRS_H, ((out1_sadd+ i*(out1_height*out1_width*2)) >> 16) & 0xFFFF);
      }else{
        DMA_R_W(IRP1_DRS_L,  (out1_sadd)        & 0xFFF8);
        DMA_R_W(IRP1_DRS_H, ((out1_sadd) >> 16) & 0xFFFF);
      }
      /* Frame size conf. */
      DMA_R_W(IRP1_DFW, out1_width*2);  /* YUV422R_1B */
      DMA_R_W(IRP1_DWW, out1_width*2);
      //DG Fix: grab one line less as one line is missing in the buffer and prevent grab'ends
	  DMA_R_W(IRP1_DWH, out1_height);
	  //DMA_R_W(IRP1_DWH, out1_height-1);
	  
    }

    /** Unmask DMA EOT IT **/
    DMA_R_W(IMR2, DMA_R_R(IMR2) | (process & 0xFFFF));

    //DG fix
	//if(i==0){
      /*** CRM config. ***/
      CRM_R_W(CHECKER_DETECT_WDW,0x0050);
      CRM_R_W(STATIC_CLK_SD_HISPEED_SOURCE_SEL,0x0);  /* sensor clock */
      CRM_R_W(STATIC_CLK_SD_LOSPEED_SOURCE_SEL,0x0);  /* sensor clock */
      CRM_R_W(STATIC_CLK_PIPE_SOURCE_SEL,0x0);        /* sensor clock */
      /* clocks division ratio */
      CRM_R_W(CLK_HOST_IPP_DIV,0x2);
      CRM_R_W(CLK_PIPE_IN_DIV,0x8);
	  CRM_R_W(CLK_PICTOR_GPIO_DIV,0x2);
      /* clocks enable */
      CRM_R_W(EN_CLK_SD_HISPEED   , 0x0001);
      CRM_R_W(EN_CLK_SD_LOSPEED   , 0x0001);
      CRM_R_W(EN_CLK_PIPE         , 0x0001);
      CRM_R_W(EN_CLK_PICTOR_GPIO  , 0x0001);
      /* enable interruption */
      CRM_R_W(CHECKER_ITM_BSET,0x3);

      /*** IPP config. ***/
      IPP_R_W(SD_RESET, 0x0001);
      /* CSI2 & DPHY config. */
      if(CSI_if == 0){
        switch (swap) {
          case 321 : IPP_R_W(STATIC_CSI0_DATA_LANES, 3<<10 | 2<<7 | 1<<4 | (lane_nb & 0x3)); break;
          case 312 : IPP_R_W(STATIC_CSI0_DATA_LANES, 3<<10 | 1<<7 | 2<<4 | (lane_nb & 0x3)); break;
          case 231 : IPP_R_W(STATIC_CSI0_DATA_LANES, 2<<10 | 3<<7 | 1<<4 | (lane_nb & 0x3)); break;
          case 213 : IPP_R_W(STATIC_CSI0_DATA_LANES, 2<<10 | 1<<7 | 3<<4 | (lane_nb & 0x3)); break;
          case 132 : IPP_R_W(STATIC_CSI0_DATA_LANES, 1<<10 | 3<<7 | 2<<4 | (lane_nb & 0x3)); break;
          case 123 : IPP_R_W(STATIC_CSI0_DATA_LANES, 1<<10 | 2<<7 | 3<<4 | (lane_nb & 0x3)); break;
          default  : IPP_R_W(STATIC_CSI0_DATA_LANES, 3<<10 | 2<<7 | 1<<4 | (lane_nb & 0x3)); break;
        };
      }
      if(CSI_if == 1){
        IPP_R_W(STATIC_CSI0_DATA_LANES      , (0xD1<<4)|0x1);                /* 1DL without any swap */
      }

      switch (CSI_if) {
        default :
        case 0  : /* CSI0 */   
          IPP_R_W(CSI2_DPHY0_CL_CTRL         , (unsigned int)(2000/frequency)|(CSI_release<<8)); /* we need the truncated result */
          IPP_R_W(CSI2_DPHY0_DL1_CTRL        , 0x8);
          IPP_R_W(CSI2_DPHY0_DL2_CTRL        , 0x8);                          
          IPP_R_W(CSI2_DPHY0_DL3_CTRL        , 0x8);                          
          break;
        case 1  : /* CSI1 */   
          IPP_R_W(CSI2_DPHY1_CL_CTRL         , (unsigned int)(2000/frequency)|(CSI_release<<8)); /* we need the truncated result */
          IPP_R_W(CSI2_DPHY1_DL1_CTRL        , 0x8);
          break;
      }

      IPP_R_W(STATIC_CSI2RX_DATA_TYPE_W , (0x12<<8) | (format & 0x3F));
      IPP_R_W(STATIC_CSI2RX_PIX_WIDTH_W , (0x08<<4) | (bpp & 0xF));
      /* Start DPHY compensation processus */
      IPP_R_W(DPHY_COMPENSATION_CTRL,0x1);                                    /* Enable compensation clock */
      IPP_R_W(DPHY_COMPENSATION_CTRL,IPP_R_R(DPHY_COMPENSATION_CTRL) | 0x2);  /* Start the compensation */

      /* Poll on IPP_DPHY_COMPENSATION_STATUS register to detect the end of the compensation */
      while((IPP_R_R(DPHY_COMPENSATION_STATUS) & 0x1) != OK){
#ifndef CHARAC
        unsigned int time;
        get_time(&time);
        PRINTF_LEVEL(DETAIL,("\ttime %d ns, polling on IPP_DPHY_COMPENSATION_STATUS...\n",time));
        advance_time(50);                                     
#endif
      }
#ifndef CHARAC
      PRINTF_LEVEL(DETAIL,("***** The compensation read code is %x.\n",
            (IPP_R_R(DPHY_COMPENSATION_STATUS) & 0xE) ));
#endif
      LOS_Log("The compensation read code is %x.\n", (IPP_R_R(DPHY_COMPENSATION_STATUS) & 0xE), NULL, NULL, NULL, NULL, NULL);
      IPP_R_W(DPHY_COMPENSATION_CTRL,IPP_R_R(DPHY_COMPENSATION_CTRL) & ~0x1); /* Disable compensation clock */

      /* Enable input interface and corresponding interruptions */
      switch (CSI_if) {
        default :
        case 0  : IPP_R_W(STATIC_TOP_IF_SEL,  0x0); break;  /* CSI0 */
        case 1  : IPP_R_W(STATIC_TOP_IF_SEL,  0x1); break;  /* CSI1 */
        case 2  : IPP_R_W(STATIC_TOP_IF_SEL,  0x2);         /* CCP */
                  IPP_R_W(STATIC_CCP_IF,      bpp<<8 | 1);  /* Data clock */
                  CRM_R_W(STATIC_SD_CCP_PIXEL_CLOCK_DIV, bpp * 2);
                  IPP_R_W(INTERNAL_EN_CLK_CCP,0x1);         /* Enable CCP clock  */
                  IPP_R_W(SD_STATIC_CCP_EN, 0x0001);
      };

      if ( CSI_if == 1 ) {
        IPP_R_W(CSI2_DPHY_ERROR_ITM_BSET    , 0x7000);			// Disable CSI1_ERR_CONTROL 0xF000=>0xC000
        IPP_R_W(CSI2_DPHY0_DBG_ITM_BSET     , 0x0000);
        IPP_R_W(CSI2_DPHY1_DBG_ITM_BSET     , 0x0003);
      } else {
        switch (lane_nb) {
          default :
          case 1 : switch (swap) {
                     default  :
                     case 321 : 
                     case 231 : IPP_R_W(CSI2_DPHY_ERROR_ITM_BSET, 1<<9 | 1<<6 | 1<<3 | 1);
                                IPP_R_W(CSI2_DPHY0_DBG_ITM_BSET,            1<<3 | 1);
                                break;
                     case 312 : 
                     case 132 : IPP_R_W(CSI2_DPHY_ERROR_ITM_BSET, 2<<9 | 2<<6 | 2<<3 | 2);
                                IPP_R_W(CSI2_DPHY0_DBG_ITM_BSET,            2<<3 | 2);
                                break;
                     case 213 : 
                     case 123 : IPP_R_W(CSI2_DPHY_ERROR_ITM_BSET, 4<<9 | 4<<6 | 4<<3 | 4);
                                IPP_R_W(CSI2_DPHY0_DBG_ITM_BSET,            4<<3 | 4);
                                break;
                   }; break;
          case 2 : switch (swap) {
                     default  :
                     case 321 : 
                     case 312 : IPP_R_W(CSI2_DPHY_ERROR_ITM_BSET, 3<<9 | 3<<6 | 3<<3 | 3);
                                IPP_R_W(CSI2_DPHY0_DBG_ITM_BSET,            3<<3 | 3);
                                break;
                     case 231 : 
                     case 213 : IPP_R_W(CSI2_DPHY_ERROR_ITM_BSET, 5<<9 | 5<<6 | 5<<3 | 5);
                                IPP_R_W(CSI2_DPHY0_DBG_ITM_BSET,            5<<3 | 5);
                                break;
                     case 132 : 
                     case 123 : IPP_R_W(CSI2_DPHY_ERROR_ITM_BSET, 6<<9 | 6<<6 | 6<<3 | 6);
                                IPP_R_W(CSI2_DPHY0_DBG_ITM_BSET,                6<<3 | 6);
                                break;
                   }
				   break;
          case 3 : IPP_R_W(CSI2_DPHY_ERROR_ITM_BSET, 0xFFF); 
                   IPP_R_W(CSI2_DPHY0_DBG_ITM_BSET, 0x3F);
                   break;
        }
      }
      IPP_R_W(CSI2_PACKET_ERROR_ITM_BSET, 0x3);
      IPP_R_W(CSI2_PACKET_DBG_ITM_BSET, 0x1);

      /* DPHY CONTROL ERR disable (ED & v1 workaround) */
      IPP_R_W(CSI2_DPHY_ERROR_ITM_BCLR, 7 << 9);
      IPP_R_W(SD_ERROR_ITS_BCLR, 1 << 11);
      IPP_R_W(CSI2_DPHY_ERROR_ITS_BCLR, 7 << 9);
      /* End of workaround */

      /** ISP config. **/
      /* RX config. */
      if ( CSI_if == 2 ){ // CCP
        ISP_R_W(ISP_SMIARX_SETUP,0x06C8);
      }else{
        ISP_R_W(ISP_SMIARX_SETUP,0x07C8);   /* Parallel input activation */
      }
      /* SD conf */
      ISP_R_W(ISP_SD_IDP_GATE_ENABLE,0x1);
      /* RE config. */
      ISP_R_W(ISP_RE_BAYER_LOAD_ENABLE,0x1);
      /* DMCE config. */
      ISP_R_W(ISP_DMCE_BAYER_LOAD_ENABLE, 0x1);
      ISP_R_W(ISP_BABYLON_ENABLE,         0x1);
      ISP_R_W(ISP_DMCE_RGB_LOAD_ENABLE,   0x1);
      /* CE config. */
      ISP_R_W(ISP_CE0_IDP_GATE_ENABLE,0x1);
      if ((pipe_sel & PIPE0) == PIPE0) {
        /* Pipe0 is used */
        ISP_R_W(ISP_CE0_IDP_GATE_SELECT,0x0);
      }
      else {
        /* Pipe0 is not used : avoid useless toggleing */
        ISP_R_W(ISP_CE0_IDP_GATE_SELECT,0x1);
      }

      ISP_R_W(ISP_CE1_IDP_GATE_ENABLE,0x1);
      if ((pipe_sel & PIPE1) == PIPE1) {
        /* Pipe1 is used */
        ISP_R_W(ISP_CE1_IDP_GATE_SELECT,0x0);
      }
      else {
        /* Pipe1 is not ised : avoid useless toggleing */
        ISP_R_W(ISP_CE1_IDP_GATE_SELECT,0x1);
      }

      /* enable mux */
      if ((pipe_sel & PIPE0) == PIPE0) {
        ISP_R_W(ISP_CE0_MUX_GPS_ENABLE,0x1);
        ISP_R_W(ISP_CE0_MUX_SHARP_ENABLE,0x1);
        ISP_R_W(ISP_CE0_MUX_UNSHARP_ENABLE,0x1);
        ISP_R_W(ISP_CE0_MUX_MATRIX_ADSOC_ENABLE,0x1);
      }

      if ((pipe_sel & PIPE1) == PIPE1) {
        ISP_R_W(ISP_CE1_MUX_GPS_ENABLE,0x1);
        ISP_R_W(ISP_CE1_MUX_SHARP_ENABLE,0x1);
        ISP_R_W(ISP_CE1_MUX_UNSHARP_ENABLE,0x1);
        ISP_R_W(ISP_CE1_MUX_MATRIX_ADSOC_ENABLE,0x1);
      }

      /* CE Modules Enable */
      if ((pipe_sel & PIPE0) == PIPE0) {

        /* MATRIX register to follow Cmodel configuration */
        ISP_R_W(ISP_CE0_MATRIX_ENABLE,1);
        ISP_R_W(ISP_CE0_MATRIX_RCOF00,1024);
        ISP_R_W(ISP_CE0_MATRIX_RCOF01,0);
        ISP_R_W(ISP_CE0_MATRIX_RCOF02,0);
        ISP_R_W(ISP_CE0_MATRIX_RCOF10,0);
        ISP_R_W(ISP_CE0_MATRIX_RCOF11,1024);
        ISP_R_W(ISP_CE0_MATRIX_RCOF12,0);
        ISP_R_W(ISP_CE0_MATRIX_RCOF20,0);
        ISP_R_W(ISP_CE0_MATRIX_RCOF21,0);
        ISP_R_W(ISP_CE0_MATRIX_RCOF22,1024);
        ISP_R_W(ISP_CE0_MATRIX_R_OFFSET,0);
        ISP_R_W(ISP_CE0_MATRIX_G_OFFSET,0);
        ISP_R_W(ISP_CE0_MATRIX_B_OFFSET,0);

        /* YUV CODER configuration to follow Cmodel configuration */
        ISP_R_W(ISP_CE0_CODER_Y_COF00,(t_sint16) 76); 
        ISP_R_W(ISP_CE0_CODER_Y_COF01,(t_sint16) 149);
        ISP_R_W(ISP_CE0_CODER_Y_COF02,(t_sint16) 29);

        ISP_R_W(ISP_CE0_CODER_CB_COF10,(t_sint16) -43);
        ISP_R_W(ISP_CE0_CODER_CB_COF11,(t_sint16) -84);
        ISP_R_W(ISP_CE0_CODER_CB_COF12,(t_sint16) 127);

        ISP_R_W(ISP_CE0_CODER_CR_COF20,(t_sint16) 127);
        ISP_R_W(ISP_CE0_CODER_CR_COF21,(t_sint16) -106);
        ISP_R_W(ISP_CE0_CODER_CR_COF22,(t_sint16) -21);

        ISP_R_W(ISP_CE0_CODER_CH0_MIN_CLIP,0);
        ISP_R_W(ISP_CE0_CODER_CH1_MIN_CLIP,0);
        ISP_R_W(ISP_CE0_CODER_CH2_MIN_CLIP,0);

        ISP_R_W(ISP_CE0_CODER_CH0_MAX_CLIP,1023);
        ISP_R_W(ISP_CE0_CODER_CH1_MAX_CLIP,1023);
        ISP_R_W(ISP_CE0_CODER_CH2_MAX_CLIP,1023);

        ISP_R_W(ISP_CE0_CODER_YFLOOR,0);
        ISP_R_W(ISP_CE0_CODER_CBFLOOR,128);
        ISP_R_W(ISP_CE0_CODER_CRFLOOR,128);

        ISP_R_W(ISP_CE0_CODER_FORMAT,0x2); /* configure YUVCODER to YUV422 instead of RGB */
        ISP_R_W(ISP_CE0_CODER_ENABLE,0x1);
        /* diseable dither ! that is enable by default */
        ISP_R_W(ISP_CE0_DITHER_ENABLE,0);

        /* CE0 Crop */
        ISP_R_W(ISP_CE0_CROP_H_START,       0);
        ISP_R_W(ISP_CE0_CROP_V_START,       0);
        ISP_R_W(ISP_CE0_CROP_H_SIZE,        out0_width);
        ISP_R_W(ISP_CE0_CROP_V_SIZE,        out0_height);
		ISP_R_W(ISP_CE0_CROP_IACTIVE_PIXS,  frame_width-4);
        ISP_R_W(ISP_CE0_CROP_IACTIVE_LINES, frame_height-4);
        ISP_R_W(ISP_CE0_CROP_SOF_RETIMING,  0);
        ISP_R_W(ISP_CE0_CROP_ENABLE,        0x1);

      }
      if ((pipe_sel & PIPE1) == PIPE1) {

        /* MATRIX register to follow Cmodel configuration */
        ISP_R_W(ISP_CE1_MATRIX_ENABLE,1);
        ISP_R_W(ISP_CE1_MATRIX_RCOF00,1024);
        ISP_R_W(ISP_CE1_MATRIX_RCOF01,0);
        ISP_R_W(ISP_CE1_MATRIX_RCOF02,0);
        ISP_R_W(ISP_CE1_MATRIX_RCOF10,0);
        ISP_R_W(ISP_CE1_MATRIX_RCOF11,1024);
        ISP_R_W(ISP_CE1_MATRIX_RCOF12,0);
        ISP_R_W(ISP_CE1_MATRIX_RCOF20,0);
        ISP_R_W(ISP_CE1_MATRIX_RCOF21,0);
        ISP_R_W(ISP_CE1_MATRIX_RCOF22,1024);
        ISP_R_W(ISP_CE1_MATRIX_R_OFFSET,0);
        ISP_R_W(ISP_CE1_MATRIX_G_OFFSET,0);
        ISP_R_W(ISP_CE1_MATRIX_B_OFFSET,0);

        ISP_R_W(ISP_CE1_CODER_Y_COF00,(t_sint16) 76);
        ISP_R_W(ISP_CE1_CODER_Y_COF01,(t_sint16) 149);
        ISP_R_W(ISP_CE1_CODER_Y_COF02,(t_sint16) 29);

        ISP_R_W(ISP_CE1_CODER_CB_COF10,(t_sint16) -43);
        ISP_R_W(ISP_CE1_CODER_CB_COF11,(t_sint16) -84);
        ISP_R_W(ISP_CE1_CODER_CB_COF12,(t_sint16) 127);

        ISP_R_W(ISP_CE1_CODER_CR_COF20,(t_sint16) 127);
        ISP_R_W(ISP_CE1_CODER_CR_COF21,(t_sint16) -106);
        ISP_R_W(ISP_CE1_CODER_CR_COF22,(t_sint16) -21);

        ISP_R_W(ISP_CE1_CODER_CH0_MIN_CLIP,0);
        ISP_R_W(ISP_CE1_CODER_CH1_MIN_CLIP,0);
        ISP_R_W(ISP_CE1_CODER_CH2_MIN_CLIP,0);

        ISP_R_W(ISP_CE1_CODER_CH0_MAX_CLIP,1023);
        ISP_R_W(ISP_CE1_CODER_CH1_MAX_CLIP,1023);
        ISP_R_W(ISP_CE1_CODER_CH2_MAX_CLIP,1023);

        ISP_R_W(ISP_CE1_CODER_YFLOOR,0);
        ISP_R_W(ISP_CE1_CODER_CBFLOOR,128);
        ISP_R_W(ISP_CE1_CODER_CRFLOOR,128);

        ISP_R_W(ISP_CE1_CODER_FORMAT,0x2); /* configure YUVCODER to YUV instead of RGB */
        ISP_R_W(ISP_CE1_CODER_ENABLE,0x1);
        /* disable dither ! that is enable by default */
        ISP_R_W(ISP_CE1_DITHER_ENABLE,0);

        /* CE1 Crop */
        ISP_R_W(ISP_CE1_CROP_H_START,       0);
        ISP_R_W(ISP_CE1_CROP_V_START,       0);
        ISP_R_W(ISP_CE1_CROP_H_SIZE,        out1_width);
        ISP_R_W(ISP_CE1_CROP_V_SIZE,        out1_height);
        ISP_R_W(ISP_CE1_CROP_IACTIVE_PIXS,  frame_width-4);
        ISP_R_W(ISP_CE1_CROP_IACTIVE_LINES, frame_height-4);
        ISP_R_W(ISP_CE1_CROP_SOF_RETIMING,  0);
        ISP_R_W(ISP_CE1_CROP_ENABLE,        0x1);

      }
      /* OPF config. */
      ISP_R_W(ISP_OPF0_ENABLE,0x1);
      ISP_R_W(ISP_OPF0_CONTROL,0x1C00 | (5<<13));
      ISP_R_W(ISP_OPF0_PIXVALID_LTYPES, (1<<5));
      ISP_R_W(ISP_OPF0_YCBCR_FLIP,0x0);
      ISP_R_W(ISP_OPF0_HSYNC_RISING,0x0);
      ISP_R_W(ISP_OPF0_HSYNC_FALLING,out0_width);
      ISP_R_W(ISP_OPF0_VSYNC_RISING_COARSE,0x0);
      ISP_R_W(ISP_OPF0_VSYNC_RISING_FINE,0x0);
      ISP_R_W(ISP_OPF0_VSYNC_FALLING_COARSE,out0_height + 5 + embedded_line  - 1);
      ISP_R_W(ISP_OPF0_VSYNC_FALLING_FINE,0x1);
      ISP_R_W(ISP_OPF0_HCROP_RISING,0x0);
      ISP_R_W(ISP_OPF0_HCROP_FALLING,out0_width-1);
      ISP_R_W(ISP_OPF0_VCROP_RISING_COARSE,0x0);
      ISP_R_W(ISP_OPF0_VCROP_FALLING_COARSE,out0_height + 5 + embedded_line  -1);
      ISP_R_W(ISP_OPF0_LAST_NONACT_LINE,0x0);
      ISP_R_W(ISP_OPF1_ENABLE,0x1);
      ISP_R_W(ISP_OPF1_CONTROL,0x1C00 | (5<<13));
      ISP_R_W(ISP_OPF1_PIXVALID_LTYPES, (1<<5));
      ISP_R_W(ISP_OPF1_YCBCR_FLIP,0x0);
      ISP_R_W(ISP_OPF1_HSYNC_RISING,0x0);
      ISP_R_W(ISP_OPF1_HSYNC_FALLING,out1_width);
      ISP_R_W(ISP_OPF1_VSYNC_RISING_COARSE,0x0);
      ISP_R_W(ISP_OPF1_VSYNC_RISING_FINE,0x0);
      ISP_R_W(ISP_OPF1_VSYNC_FALLING_COARSE,out1_height + 5 + embedded_line  - 1);
      ISP_R_W(ISP_OPF1_VSYNC_FALLING_FINE,0x1);
      ISP_R_W(ISP_OPF1_HCROP_RISING,0x0);
      ISP_R_W(ISP_OPF1_HCROP_FALLING,out1_width - 1);
      ISP_R_W(ISP_OPF1_VCROP_RISING_COARSE,0x0);
      ISP_R_W(ISP_OPF1_VCROP_FALLING_COARSE,out1_height + 5 + embedded_line  - 1);
      ISP_R_W(ISP_OPF1_LAST_NONACT_LINE,0x0);

      /** PRY config. **/
      if(hr_fmt == YUV420MB_2B){
        IPP_R_W(SD_PRY0_FORMAT,     0x6 << 8);  /* YUV420MB_2B */
      }else{
        IPP_R_W(SD_PRY0_FORMAT,     0x1 << 8);  /* YUV422R_1B */
      }
      IPP_R_W(SD_PRY1_FORMAT,     0x1 << 8);    /* YUV422R_1B */
      /* Unmask ITs */ 
      IPP_R_W(SD_ERROR_ITM_BSET         , 0x0FFF);
      /* SDG enable */ 
      IPP_R_W(DPHY_TOP_IF_EN,     0x1);
	  
	  
    }

    /* Start PRY */
    if ((pipe_sel & PIPE0) == PIPE0) {
      IPP_R_W(SD_PRY0_ENABLE_SET, 1);
    }
    if ((pipe_sel & PIPE1) == PIPE1) {
      IPP_R_W(SD_PRY1_ENABLE_SET, 1);
    }

    /** Start DMA **/
    DMA_R_W(ENR, process & 0xFFFF);

    /* SIA platform specific, BFM enable */
#ifdef SIA_DEBUG
    hv_set_mmdsp_gpio_enable(0xFFFF);
    if ( CSI_if == 0 ) { /* CSI0 */
      hv_set_mmdsp_gpio_CSI2_bfm0_enable(1);
    }else{  /* CSI1 & CPP */
      hv_set_mmdsp_gpio_CSI2_bfm1_enable(1);
    }
#endif

    /* Wait for sensor clk */
    if (i == 0){
      while ( (CRM_R_R(CHECKER_ITS) & 0x1) != 0x1 ) {}
      CRM_R_W(CHECKER_ITS_BCLR, 0x0001);  /* Clear IT */
      CRM_R_W(EMUL_SENSOR_CLOCK, 0x0);    /* Sensor mode */
    }

    if (i == 0){
	  //start timer for framerate count
	  time0=LOS_getSystemTime();
	}
	
	/* RX start */
    ISP_R_W(ISP_SMIARX_CTRL, 0x0001); /* Rx Start */
    while (status.global_event == 0){
#ifdef SIA_DEBUG
      unsigned int time=0;
      get_time(&time);
      PRINTF_LEVEL(DETAIL,("\ttime %d ns, Wait for event...\n",time));
#endif
    

      /* Wait for DMA IRP2_PRA_W EOT */
      if((DMA_R_R(ENR) & process) == 0) {
        status.global_event |= DMA_EOT;
#ifdef SIA_DEBUG
        PRINTF_LEVEL(DETAIL,("\ttime %d ns, End of DMA process...\n",time));
        if(DMA_R_R(FIFO_STA) & process)
          PRINTF_LEVEL(DETAIL,("\ttime %d ns, DMA Fifo not empty...\n",time));
#endif
      }
	  
	  //DG add to monitor grab frame:
	  //LOS_Log("Frame nb= %d at time= %d  grabbed...Status_global_event=%x \n", i, LOS_getSystemTime(), status.global_event, NULL, NULL, NULL);

      /* Wait for sensor error */
      
	  if ((status.sd_error = (IPP_R_R(SD_ERROR_ITS) & IPP_R_R(SD_ERROR_ITM))) != 0)
	  {
	    //For debug
		//if (hr_fmt == YUV422R_1B)
		//{
	        status.global_event |= SENSOR_ERR;
	        // Clear interrupts registers 
	        IPP_R_W(SD_ERROR_ITS_BCLR, 0xFFFF);
#ifdef SIA_DEBUG
	        PRINTF_LEVEL(DETAIL,("\ttime %d ns, Sensor error (0x%08X) detected...\n",time, status.sd_error));
#endif
	        LOS_Log("Sensor error (0x%08X) detected...\n", status.sd_error, NULL, NULL, NULL, NULL, NULL);
	        if (status.sd_error & 1<<11)
			{
	          status.global_event |= DPHY_ERR;
	          status.dphy_error = IPP_R_R(CSI2_DPHY_ERROR_ITS) & IPP_R_R(CSI2_DPHY_ERROR_ITM);
	          status.dphy0_dbg  = IPP_R_R(CSI2_DPHY0_DBG_ITS) & IPP_R_R(CSI2_DPHY0_DBG_ITM);
	          status.dphy1_dbg  = IPP_R_R(CSI2_DPHY1_DBG_ITS) & IPP_R_R(CSI2_DPHY1_DBG_ITM);
	          // Clear interrupts registers 
	          IPP_R_W(CSI2_DPHY_ERROR_ITS_BCLR, 0xFFFF);
	          IPP_R_W(CSI2_DPHY0_DBG_ITS_BCLR, 0xFFFF);
	          IPP_R_W(CSI2_DPHY1_DBG_ITS_BCLR, 0xFFFF);
#ifdef SIA_DEBUG
	          PRINTF_LEVEL(DETAIL,("\ttime %d ns, CSI2 DPHY error (0x%08X) detected...\n",time,status.dphy_error));
#endif
	        }
	        if (status.sd_error & 1<<10)
			{
	          status.global_event |= PACKET_ERR;
	          status.packet_error = IPP_R_R(CSI2_PACKET_ERROR_ITS) & IPP_R_R(CSI2_PACKET_ERROR_ITM);
	          status.packet_dbg   = IPP_R_R(CSI2_PACKET_DBG_ITS) & IPP_R_R(CSI2_PACKET_DBG_ITM);
	          // Clear interrupts registers 
	          IPP_R_W(CSI2_PACKET_ERROR_ITS_BCLR, 0xFFFF);
	          IPP_R_W(CSI2_PACKET_DBG_ITS_BCLR, 0xFFFF);
#ifdef SIA_DEBUG
	          PRINTF_LEVEL(DETAIL,("\ttime %d ns, CSI2 DPHY packet error (0x%08X) detected...\n",time, status.packet_error));
#endif
	        }

	        // BMS & DMA reset 
	        IPP_R_W(SD_BMS_ENABLE_CLR, 0x0001);   // Stop BMS 
	        DMA_R_W(SOFT_RST, 1 << 11 );			    // Stop PRA_W process 
	        return status;
      //}
	    
    }
	}
	//DG add to monitor grab frame:
	//LOS_Log("--> Frame nb= %d at time= %d  grabbed successfully ............. \n", i, LOS_getSystemTime(), NULL, NULL, NULL, NULL);
  }
  time1=LOS_getSystemTime(); 
  maesuredFrameRate=(t_uint32) (nb_frames*1000000/(time1-time0));
  LOS_Log("Framerate between time0 in us (%d) and time1 (%d)= %d fr/sec \n", time0, time1, maesuredFrameRate, NULL, NULL, NULL);
  return status;
}


