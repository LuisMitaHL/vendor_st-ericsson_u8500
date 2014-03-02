/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */




#ifndef __dphy_charac_h
#define __dphy_charac_h


//------------------------------------------------------------------------
// Global Variables and defines
//------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif
	
#define YUV422  0x01E
#define RAW6    0x028 /* Raw Bayer on 6 bits. */
#define RAW7    0x029 /* Raw Bayer on 7 bits. */
#define RAW8    0x02A /* Raw Bayer 8 bits. */
#define RAW10   0x02B /* Raw Bayer 10 bits. */
#define RAW12   0x02C /* Raw Bayer 12 bits. */
#define R10C6   0xA28 /* Raw Bayer on 10 bits compressed on 6 bits. */
#define R12C6   0xC28 /* Raw Bayer on 12 bits compressed on 6 bits. */
#define R10C7   0xA29 /* Raw Bayer on 10 bits compressed on 7 bits. */
#define R12C7   0xC29 /* Raw Bayer on 12 bits compressed on 7 bits. */
#define R10C8   0xA2A /* Raw Bayer on 10 bits compressed on 8 bits. */
#define R12C8   0xC2A /* Raw Bayer on 12 bits compressed on 8 bits. */

typedef struct {
    unsigned int global_event;    /* Gloabl event status  */
    unsigned int sd_error;        /* Sensor error status  */
    unsigned int cd_error;        /* Camera error status  */
    unsigned int dphy_error;      /* DPHY error status    */
    unsigned int dphy0_dbg;       /* DPHY0 debug status   */
    unsigned int dphy1_dbg;       /* DPHY1 debug status   */
    unsigned int packet_error;    /* PACKET error status  */
    unsigned int packet_dbg;      /* PACKET debug status  */
    unsigned int sd_error_cnt;    
    unsigned int packet_error_cnt;
    unsigned int dphy_error_cnt;  
    unsigned int dma_fifo_sta;
    unsigned int stbp_peak_level;
} charac_event_status_t;


typedef struct {
    unsigned int global_event;    /* Gloabl event status  */
    unsigned int frames_ok;        /* number of frames dumbed */
    unsigned int sd_error;        /* Sensor error status  */
    unsigned int dphy_error;      /* DPHY error status    */
    unsigned int dphy0_dbg;       /* DPHY0 debug status   */
    unsigned int dphy1_dbg;       /* DPHY1 debug status   */
    unsigned int packet_error;    /* PACKET error status  */
    unsigned int packet_dbg;      /* PACKET debug status  */
} dphy_charac_status_t;

//------------------------------------------------------------------------
// Function prototypes
//------------------------------------------------------------------------

charac_event_status_t hv_top_test_DPHY_charac (unsigned int frequency, 
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
												  unsigned int nb_frames);

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
                    int      embedded_line);   /* Nb of embedded lines */
					
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
                                int      embedded_line, /* Nb of embedded lines */
								int		 hr_fmt);   

#ifdef __cplusplus
}
#endif /* defined(__cplusplus) */

#endif /* defined(__IP_tests_h) */
