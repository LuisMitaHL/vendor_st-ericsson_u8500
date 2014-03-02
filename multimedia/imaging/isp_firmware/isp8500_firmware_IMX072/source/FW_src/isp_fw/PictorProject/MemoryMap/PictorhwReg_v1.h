/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 


#ifndef _PictorhwReg_V1_
#define _PictorhwReg_V1_

#include "Platform.h"

//ISP_SMIARX_V1_CTRL
/*Description: Rx Streaming control
*/
typedef union
{
    uint32_t word;
    uint32_t data;
    struct
    {
        uint8_t rx_start :1;
        /* Start Rx - can be cleared after setting. Both rx_stop and rx_abort must be cleared before Rx will start*/
        uint8_t reserved0 :7;
        uint8_t rx_stop :1;
        /* Stop Rx (at end of current frame)rx_abort control must be cleared for the end-of-frame stop.Overides the rx_start control*/
        uint8_t reserved1 :7;
        uint8_t rx_abort :1;
        /* Stop Rx immediatly Overrides both rx_stop and rx_start controls*/
    }ISP_SMIARX_V1_CTRL_ts;

}ISP_SMIARX_V1_CTRL_tu;



//ISP_SMIARX_V1_SETUP
/*Description: Rx Status
*/
typedef union
{
    uint32_t word;
    uint32_t data;
    struct
    {
        uint8_t ip_pix_width :4;
        /* width of received pixel data (number of bits per pixel) - 6,7,8,10 or 12*/
        uint8_t op_pix_width :4;
        /* width of output pixel data from SMIA Rx*/
        uint8_t parallel_enable :1;
        /* Enable parallel interface input.*/
        uint8_t vsync_polarity :1;
        /* Polarity of input VSYNC:*/
        uint8_t hsync_polarity :1;
        /* Polarity of input HSYNC:*/
        uint8_t ds_mode :1;
        /* Select whether data strobe encoding is enabled for selected CCP input*/
        uint8_t dcpx_enable :1;
        /* Bayer decompression module enable*/
        uint8_t ccp_sel :1;
        /* Select which CCP input (0 or 1) is input to deserialiser (star_decoder)Note: this signal is not used by the Rx, it is provided to control an external module*/
        uint8_t reset_ccp_rx_n :1;
        /* May be connected to the CCP receivers to clear out their state in the event of lost synchronisationNote: this signal is not used by the Rx, it is provided to control an external module*/
        uint8_t reserved0 :1;
        uint8_t auto_sync :1;
        /* Rx Synchronisation source:*/
        uint8_t pattern_type :3;
        /* Test pattern selection:*/
        uint8_t no_sof :1;
        /* Allow operation with no status-line in data-stream (must override all frame-setup registers).If set, must override all frame-setup registers.*/
        uint8_t reserved1 :3;
        uint8_t clr_ccp_shift_sync :1;
        /* Clear CCP Shifted Sync-code flagNote: this signal is not used by the Rx, it is provided to control an external module*/
        uint8_t clr_ccp_crc_error :1;
        /* Clear CCP CRC Error flagNote: this signal is not used by the Rx, it is provided to control an external module*/
        uint8_t clr_pixel_sync_error :1;
        /* Clear Unpack pixel Sync error flag*/
        uint8_t clr_aborted_iframe :1;
        /* Clears aborted interframe flag*/
        uint8_t clr_insuff_iline :1;
        /* Clears insufficient interline flag*/
        uint8_t fifo_rd_rst :1;
        /* Resets input data fifo*/
    }ISP_SMIARX_V1_SETUP_ts;

}ISP_SMIARX_V1_SETUP_tu;



//ISP_SMIARX_V1_FRAME_CTRL
/*Description: Frame control register
*/
typedef union
{
    uint32_t word;
    uint32_t data;
    struct
    {
        uint8_t latency_lines;
        /* Number of lines to wait after image capture before attempting to grab the next frame (safety factor)*/
        uint8_t stream_size;
        /* Set the number of frames to stream. When set to zero, stream continuous video*/
        uint8_t start_grab_dly;
        /* Number of frames (from sensor) to skip before streaming data*/
        uint8_t ffhc_control :2;
        /* Controls frame-format h-crop (see Design Reference*/
        uint8_t stop_at_interframe :1;
        /* When stopping Rx, stop at the start of interframe, rather than at the end of interframe.*/
    }ISP_SMIARX_V1_FRAME_CTRL_ts;

}ISP_SMIARX_V1_FRAME_CTRL_tu;



//ISP_SMIARX_V1_STATUS
/*Description: Status of the Rx block
*/
typedef union
{
    uint32_t word;
    uint32_t data;
    struct
    {
        uint8_t frame_counter;
        /* Count of frames streamed through Rx*/
        uint8_t rx_state :2;
        /* Current state of the idp_gen module*/
        uint8_t ff_state :3;
        /* Current state of the frame formatter machine*/
        uint8_t snoop_state :3;
        /* Current state of the snoop module*/
        uint8_t fifo_rd_usedw :5;
        /* Rx fifo used words*/
        uint8_t fifo_empty :1;
        /* Rx fifo empty*/
        uint8_t reserved0 :2;
        uint8_t pixel_sync_error :1;
        /* SMIA decoder detected a pixel synchronisation error.*/
        uint8_t aborted_iframe :1;
        /* Aborted interframe flag.A new input frame has arrived before the full requested frame (line_length x frame_length) has completed.Status only, not an error.*/
        uint8_t insuff_iline :1;
        /* Insufficient interline error flag.The input data has fewer than 6-pixels of interline time.*/
        uint8_t fifo_under :1;
        /* Input data fifo has under-flowed*/
        uint8_t fifo_over :1;
        /* Input data fifo has over-flowed*/
    }ISP_SMIARX_V1_STATUS_ts;

}ISP_SMIARX_V1_STATUS_tu;



//ISP_SMIARX_V1_TPAT_DATA_RG
/*Description: Test Data for pixels in Red/Green line
*/
typedef union
{
    uint32_t word;
    uint32_t data;
    struct
    {
        uint16_t tpat_data_gir :13;
        /* Test data value for GIR pixels*/
        uint8_t reserved0 :3;
        uint16_t tpat_data_red :13;
        /* Test data value for RED pixels*/
    }ISP_SMIARX_V1_TPAT_DATA_RG_ts;

}ISP_SMIARX_V1_TPAT_DATA_RG_tu;



//ISP_SMIARX_V1_TPAT_DATA_BG
/*Description: Test Data for pixels in Blue/Green line
*/
typedef union
{
    uint32_t word;
    uint32_t data;
    struct
    {
        uint16_t tpat_data_gib :13;
        uint8_t reserved0 :3;
        uint16_t tpat_data_blu :13;
    }ISP_SMIARX_V1_TPAT_DATA_BG_ts;

}ISP_SMIARX_V1_TPAT_DATA_BG_tu;



//ISP_SMIARX_V1_TPAT_HCUR_WP
/*Description: Test pattern Horizontal cursor control
*/
typedef union
{
    uint32_t word;
    uint32_t data;
    struct
    {
        uint16_t tpat_hcur_posn :12;
        uint8_t reserved0 :4;
        uint16_t tpat_hcur_width :12;
    }ISP_SMIARX_V1_TPAT_HCUR_WP_ts;

}ISP_SMIARX_V1_TPAT_HCUR_WP_tu;



//ISP_SMIARX_V1_TPAT_VCUR_WP
/*Description: Test pattern Horizontal cursor control
*/
typedef union
{
    uint32_t word;
    uint32_t data;
    struct
    {
        uint16_t tpat_vcur_posn :12;
        /* Vertical cursor position in pixels.*/
        uint8_t reserved0 :4;
        uint16_t tpat_vcur_width :12;
        /* Vertical cursor width in pixels*/
    }ISP_SMIARX_V1_TPAT_VCUR_WP_ts;

}ISP_SMIARX_V1_TPAT_VCUR_WP_tu;



//ISP_SMIARX_V1_CCP_STATUS
/*Description: Status of the CCPRx
*/
typedef union
{
    uint32_t word;
    uint16_t data;
    struct
    {
        uint8_t ccp_channel :4;
        /* CCP Channel extracted from sync-codes.Note: this signal is not generated by the Rx, it is provided by an external module*/
        uint8_t reserved0 :4;
        uint8_t ccp_false_sync :1;
        /* CCP Rx detected a false sync-codeNote: this signal is not generated by the Rx, it is provided by an external module*/
        uint8_t ccp_shift_sync :1;
        /* CCP Rx detected a shifted sync-codeNote: this signal is not generated by the Rx, it is provided by an external module*/
        uint8_t ccp_crc_error :1;
        /* CCP Rx detected a CRC error in the received line.Note: this signal is not generated by the Rx, it is provided by an external module*/
    }ISP_SMIARX_V1_CCP_STATUS_ts;

}ISP_SMIARX_V1_CCP_STATUS_tu;



//ISP_SMIARX_V1_HK_REQ_ADDR_0
/*Description: Programmable address to extract from status line data
*/
typedef union
{
    uint32_t word;
    uint16_t data;
    struct
    {
        uint16_t hk_req_addr;
    }ISP_SMIARX_V1_HK_REQ_ADDR_0_ts;

}ISP_SMIARX_V1_HK_REQ_ADDR_0_tu;



//ISP_SMIARX_V1_HK_REQ_ADDR_1
/*Description: Programmable address to extract from status line data
*/
typedef union
{
    uint32_t word;
    uint16_t data;
    struct
    {
        uint16_t hk_req_addr;
    }ISP_SMIARX_V1_HK_REQ_ADDR_1_ts;

}ISP_SMIARX_V1_HK_REQ_ADDR_1_tu;



//ISP_SMIARX_V1_HK_REQ_ADDR_2
/*Description: Programmable address to extract from status line data
*/
typedef union
{
    uint32_t word;
    uint16_t data;
    struct
    {
        uint16_t hk_req_addr;
    }ISP_SMIARX_V1_HK_REQ_ADDR_2_ts;

}ISP_SMIARX_V1_HK_REQ_ADDR_2_tu;



//ISP_SMIARX_V1_HK_REQ_ADDR_3
/*Description: Programmable address to extract from status line data
*/
typedef union
{
    uint32_t word;
    uint16_t data;
    struct
    {
        uint16_t hk_req_addr;
    }ISP_SMIARX_V1_HK_REQ_ADDR_3_ts;

}ISP_SMIARX_V1_HK_REQ_ADDR_3_tu;



//ISP_SMIARX_V1_HK_REQ_DATA_0
/*Description: Data extracted from Status Line
*/
typedef union
{
    uint32_t word;
    uint8_t data;
    struct
    {
        uint8_t hk_req_data;
    }ISP_SMIARX_V1_HK_REQ_DATA_0_ts;

}ISP_SMIARX_V1_HK_REQ_DATA_0_tu;



//ISP_SMIARX_V1_HK_REQ_DATA_1
/*Description: Data extracted from Status Line
*/
typedef union
{
    uint32_t word;
    uint8_t data;
    struct
    {
        uint8_t hk_req_data;
    }ISP_SMIARX_V1_HK_REQ_DATA_1_ts;

}ISP_SMIARX_V1_HK_REQ_DATA_1_tu;



//ISP_SMIARX_V1_HK_REQ_DATA_2
/*Description: Data extracted from Status Line
*/
typedef union
{
    uint32_t word;
    uint8_t data;
    struct
    {
        uint8_t hk_req_data;
    }ISP_SMIARX_V1_HK_REQ_DATA_2_ts;

}ISP_SMIARX_V1_HK_REQ_DATA_2_tu;



//ISP_SMIARX_V1_HK_REQ_DATA_3
/*Description: Data extracted from Status Line
*/
typedef union
{
    uint32_t word;
    uint8_t data;
    struct
    {
        uint8_t hk_req_data;
    }ISP_SMIARX_V1_HK_REQ_DATA_3_ts;

}ISP_SMIARX_V1_HK_REQ_DATA_3_tu;



//ISP_SMIARX_V1_INT_COUNT_0
/*Description: Parameter used with rx_pix_count to generate an interrupt
*/
typedef union
{
    uint32_t word;
    uint32_t data;
    struct
    {
        uint16_t pix_count;
        /* Programs the pixel count for Rx interrupt n*/
        uint16_t line_count;
        /* Programs the line count for Rx interrupt n*/
    }ISP_SMIARX_V1_INT_COUNT_0_ts;

}ISP_SMIARX_V1_INT_COUNT_0_tu;



//ISP_SMIARX_V1_INT_COUNT_1
/*Description: Parameter used with rx_pix_count to generate an interrupt
*/
typedef union
{
    uint32_t word;
    uint32_t data;
    struct
    {
        uint16_t pix_count;
        /* Programs the pixel count for Rx interrupt n*/
        uint16_t line_count;
        /* Programs the line count for Rx interrupt n*/
    }ISP_SMIARX_V1_INT_COUNT_1_ts;

}ISP_SMIARX_V1_INT_COUNT_1_tu;



//ISP_SMIARX_V1_INT_COUNT_2
/*Description: Parameter used with rx_pix_count to generate an interrupt
*/
typedef union
{
    uint32_t word;
    uint32_t data;
    struct
    {
        uint16_t pix_count;
        /* Programs the pixel count for Rx interrupt n*/
        uint16_t line_count;
        /* Programs the line count for Rx interrupt n*/
    }ISP_SMIARX_V1_INT_COUNT_2_ts;

}ISP_SMIARX_V1_INT_COUNT_2_tu;



//ISP_SMIARX_V1_INT_COUNT_3
/*Description: Parameter used with rx_pix_count to generate an interrupt
*/
typedef union
{
    uint32_t word;
    uint32_t data;
    struct
    {
        uint16_t pix_count;
        /* Programs the pixel count for Rx interrupt n*/
        uint16_t line_count;
        /* Programs the line count for Rx interrupt n*/
    }ISP_SMIARX_V1_INT_COUNT_3_ts;

}ISP_SMIARX_V1_INT_COUNT_3_tu;



//ISP_SMIARX_V1_DFV
/*Description: Interrupt control register
*/
typedef union
{
    uint32_t word;
    uint8_t data;
    struct
    {
        uint8_t dfv_enable :1;
        /* DFV feature enable*/
        uint8_t dfv_count_0_interrupt_ctrl :1;
        /* Counter n Interrupt DFV control. A count_n interrupt is generated everytime its associated control bit value changes*/
        uint8_t dfv_count_1_interrupt_ctrl :1;
        /* Counter n Interrupt DFV control. A count_n interrupt is generated everytime its associated control bit value changes*/
        uint8_t dfv_count_2_interrupt_ctrl :1;
        /* Counter n Interrupt DFV control. A count_n interrupt is generated everytime its associated control bit value changes*/
        uint8_t dfv_count_3_interrupt_ctrl :1;
        /* Counter n Interrupt DFV control. A count_n interrupt is generated everytime its associated control bit value changes*/
        uint8_t dfv_seq_complete_interrupt_ctrl :1;
        /* Sequence Complete DFV control. A seq_complete interrupt is generated everytime this control bit value changes*/
    }ISP_SMIARX_V1_DFV_ts;

}ISP_SMIARX_V1_DFV_tu;



//ISP_SMIARX_V1_COUNT_INT_EN
/*Description: Interrupt enable control
*/
typedef union
{
    uint32_t word;
    uint8_t data;
    struct
    {
        uint8_t rx_line_count_0_int_en :1;
        /* Counter n interrupt enable*/
        uint8_t rx_line_count_1_int_en :1;
        /* Counter n interrupt enable*/
        uint8_t rx_line_count_2_int_en :1;
        /* Counter n interrupt enable*/
        uint8_t rx_line_count_3_int_en :1;
        /* Counter n interrupt enable*/
        uint8_t rx_seq_complete_int_en :1;
        /* Sequence complete interrupt enable*/
    }ISP_SMIARX_V1_COUNT_INT_EN_ts;

}ISP_SMIARX_V1_COUNT_INT_EN_tu;



//ISP_SMIARX_V1_OVERRIDE
/*Description: Device ID value from status line
*/
typedef union
{
    uint32_t word;
    uint32_t data;
    struct
    {
        uint32_t override :23;
        /* If one bit is set then snooped value is overriden.
        Bit 22: y odd increment LSBit\\
        Bit 21: y even increment LSBit\\
        Bit 20: x odd increment LSBit\\
        Bit 19: x even increment LSBit\\
        Bit 18: pixel order\\
        Bit 17: line length\\
        Bit 16: field length\\
        Bit 15: SMIA descriptor 14\\
        Bit 14: SMIA descriptor 13\\
        Bit 13: SMIA descriptor 12\\
        Bit 12: SMIA descriptor 11\\
        Bit 11: SMIA descriptor 10\\
        Bit 10: SMIA descriptor 9\\
        Bit 9: SMIA descriptor 8\\
        Bit 8: SMIA descriptor 7\\
        Bit 7: SMIA descriptor 6\\
        Bit 6: SMIA descriptor 5\\
        Bit 5: SMIA descriptor 4\\
        Bit 4: SMIA descriptor 3\\
        Bit 3: SMIA descriptor 2\\
        Bit 2: SMIA descriptor 1\\
        Bit 1: SMIA descriptor 0\\
        Bit 0: SMIA model subtype*/
    }ISP_SMIARX_V1_OVERRIDE_ts;

}ISP_SMIARX_V1_OVERRIDE_tu;



//ISP_SMIARX_V1_SENSOR_ID
/*Description: Device ID value from status line
*/
typedef union
{
    uint32_t word;
    uint16_t data;
    struct
    {
        uint16_t sensor_id;
    }ISP_SMIARX_V1_SENSOR_ID_ts;

}ISP_SMIARX_V1_SENSOR_ID_tu;



//ISP_SMIARX_V1_PIXEL_ORDER
/*Description: Color Pixel order
*/
typedef union
{
    uint32_t word;
    uint16_t data;
    struct
    {
        uint8_t pixel_order :2;
        /* Pixel order from status line*/
        uint8_t reserved0 :6;
        uint8_t pixel_order_ovr :2;
        /* Pixel order value for override*/
    }ISP_SMIARX_V1_PIXEL_ORDER_ts;

}ISP_SMIARX_V1_PIXEL_ORDER_tu;



//ISP_SMIARX_V1_AUTO_HENV_LENGTH
/*Description: Number of pixels in active henv output from idp_gen when running in auto_sync mode (see RX_SETUP register)
*/
typedef union
{
    uint32_t word;
    uint16_t data;
    struct
    {
        uint16_t auto_henv_length :12;
    }ISP_SMIARX_V1_AUTO_HENV_LENGTH_ts;

}ISP_SMIARX_V1_AUTO_HENV_LENGTH_tu;



//ISP_SMIARX_V1_FRAME_LENGTH
/*Description: Frame Length
*/
typedef union
{
    uint32_t word;
    uint32_t data;
    struct
    {
        uint16_t frame_length;
        /* Frame length value from status line*/
        uint16_t frame_length_ovr;
        /* Frame length value for override*/
    }ISP_SMIARX_V1_FRAME_LENGTH_ts;

}ISP_SMIARX_V1_FRAME_LENGTH_tu;



//ISP_SMIARX_V1_LINE_LENGTH
/*Description: Line Length
*/
typedef union
{
    uint32_t word;
    uint32_t data;
    struct
    {
        uint16_t line_length;
        /* Line length value from status line*/
        uint16_t line_length_ovr;
        /* Line length value for override*/
    }ISP_SMIARX_V1_LINE_LENGTH_ts;

}ISP_SMIARX_V1_LINE_LENGTH_tu;



//ISP_SMIARX_V1_X_ADDR_START
/*Description: X address start value from status line
*/
typedef union
{
    uint32_t word;
    uint16_t data;
    struct
    {
        uint16_t x_addr_start;
    }ISP_SMIARX_V1_X_ADDR_START_ts;

}ISP_SMIARX_V1_X_ADDR_START_tu;



//ISP_SMIARX_V1_Y_ADDR_START
/*Description: Y address start from status line
*/
typedef union
{
    uint32_t word;
    uint16_t data;
    struct
    {
        uint16_t y_addr_start;
    }ISP_SMIARX_V1_Y_ADDR_START_ts;

}ISP_SMIARX_V1_Y_ADDR_START_tu;



//ISP_SMIARX_V1_X_ADDR_END
/*Description: X address end value from status line
*/
typedef union
{
    uint32_t word;
    uint16_t data;
    struct
    {
        uint16_t x_addr_end;
    }ISP_SMIARX_V1_X_ADDR_END_ts;

}ISP_SMIARX_V1_X_ADDR_END_tu;



//ISP_SMIARX_V1_Y_ADDR_END
/*Description: Y address end value from status line
*/
typedef union
{
    uint32_t word;
    uint16_t data;
    struct
    {
        uint16_t y_addr_end;
    }ISP_SMIARX_V1_Y_ADDR_END_ts;

}ISP_SMIARX_V1_Y_ADDR_END_tu;



//ISP_SMIARX_V1_X_OUTPUT_SIZE
/*Description: X output size value from status line
*/
typedef union
{
    uint32_t word;
    uint16_t data;
    struct
    {
        uint16_t x_output_size;
    }ISP_SMIARX_V1_X_OUTPUT_SIZE_ts;

}ISP_SMIARX_V1_X_OUTPUT_SIZE_tu;



//ISP_SMIARX_V1_Y_OUTPUT_SIZE
/*Description: Y output size value from status line
*/
typedef union
{
    uint32_t word;
    uint16_t data;
    struct
    {
        uint16_t y_output_size;
    }ISP_SMIARX_V1_Y_OUTPUT_SIZE_ts;

}ISP_SMIARX_V1_Y_OUTPUT_SIZE_tu;



//ISP_SMIARX_V1_ADDR_INCREMENTS
/*Description: Address increments control
*/
typedef union
{
    uint32_t word;
    uint8_t data;
    struct
    {
        uint8_t x_even_inc_lsb :1;
        /* X Address even increment value from status line*/
        uint8_t x_even_inc_lsb_ovr :1;
        /* X Address even increment value for override*/
        uint8_t x_odd_inc_lsb :1;
        /* X Address odd increment value from status line*/
        uint8_t x_odd_inc_lsb_ovr :1;
        /* X Address odd increment value for override*/
        uint8_t y_even_inc_lsb :1;
        /* Y Address even increment value from status line*/
        uint8_t y_even_inc_lsb_ovr :1;
        /* Y Address even increment value for override*/
        uint8_t y_odd_inc_lsb :1;
        /* Y Address odd increment value from status line*/
        uint8_t y_odd_inc_lsb_ovr :1;
        /* Y Address odd increment value for override*/
    }ISP_SMIARX_V1_ADDR_INCREMENTS_ts;

}ISP_SMIARX_V1_ADDR_INCREMENTS_tu;



//ISP_SMIARX_V1_FINE_EXPOSURE
/*Description: Fine exposure value from status line
*/
typedef union
{
    uint32_t word;
    uint16_t data;
    struct
    {
        uint16_t fine_exposure;
    }ISP_SMIARX_V1_FINE_EXPOSURE_ts;

}ISP_SMIARX_V1_FINE_EXPOSURE_tu;



//ISP_SMIARX_V1_COARSE_EXPOSURE
/*Description: Coarse exposure from status line
*/
typedef union
{
    uint32_t word;
    uint16_t data;
    struct
    {
        uint16_t coarse_exposure;
    }ISP_SMIARX_V1_COARSE_EXPOSURE_ts;

}ISP_SMIARX_V1_COARSE_EXPOSURE_tu;



//ISP_SMIARX_V1_ANALOG_GAIN
/*Description: Analogue Gain value from status line
*/
typedef union
{
    uint32_t word;
    uint16_t data;
    struct
    {
        uint16_t analog_gain;
    }ISP_SMIARX_V1_ANALOG_GAIN_ts;

}ISP_SMIARX_V1_ANALOG_GAIN_tu;



//ISP_SMIARX_V1_MODEL_SUBTYPE
/*Description: SMIA Frame Format Model Subtype
*/
typedef union
{
    uint32_t word;
    uint16_t data;
    struct
    {
        uint8_t frame_format_model_subtype;
        /* Frame format subtype value from status line*/
        uint8_t frame_format_model_subtype_ovr;
        /* Frame format subtype value for override*/
    }ISP_SMIARX_V1_MODEL_SUBTYPE_ts;

}ISP_SMIARX_V1_MODEL_SUBTYPE_tu;



//ISP_SMIARX_V1_DESCRIPTOR_0
/*Description: SMIA Frame Format Descriptor (snooped from status line),
*/
typedef union
{
    uint32_t word;
    uint32_t data;
    struct
    {
        uint16_t frame_format_descriptor;
        /* Frame format descriptor from status line[15:12] - Pixel/Line Code,[11:0] - Number of pixels/lines*/
        uint16_t frame_format_descriptor_ovr;
        /* Frame format descriptor for override[31:28] - Pixel/Line Code[27:16] - Number of pixels/lines*/
    }ISP_SMIARX_V1_DESCRIPTOR_0_ts;

}ISP_SMIARX_V1_DESCRIPTOR_0_tu;



//ISP_SMIARX_V1_DESCRIPTOR_1
/*Description: SMIA Frame Format Descriptor (snooped from status line),
*/
typedef union
{
    uint32_t word;
    uint32_t data;
    struct
    {
        uint16_t frame_format_descriptor;
        /* Frame format descriptor from status line[15:12] - Pixel/Line Code,[11:0] - Number of pixels/lines*/
        uint16_t frame_format_descriptor_ovr;
        /* Frame format descriptor for override[31:28] - Pixel/Line Code[27:16] - Number of pixels/lines*/
    }ISP_SMIARX_V1_DESCRIPTOR_1_ts;

}ISP_SMIARX_V1_DESCRIPTOR_1_tu;



//ISP_SMIARX_V1_DESCRIPTOR_2
/*Description: SMIA Frame Format Descriptor (snooped from status line),
*/
typedef union
{
    uint32_t word;
    uint32_t data;
    struct
    {
        uint16_t frame_format_descriptor;
        /* Frame format descriptor from status line[15:12] - Pixel/Line Code,[11:0] - Number of pixels/lines*/
        uint16_t frame_format_descriptor_ovr;
        /* Frame format descriptor for override[31:28] - Pixel/Line Code[27:16] - Number of pixels/lines*/
    }ISP_SMIARX_V1_DESCRIPTOR_2_ts;

}ISP_SMIARX_V1_DESCRIPTOR_2_tu;



//ISP_SMIARX_V1_DESCRIPTOR_3
/*Description: SMIA Frame Format Descriptor (snooped from status line),
*/
typedef union
{
    uint32_t word;
    uint32_t data;
    struct
    {
        uint16_t frame_format_descriptor;
        /* Frame format descriptor from status line[15:12] - Pixel/Line Code,[11:0] - Number of pixels/lines*/
        uint16_t frame_format_descriptor_ovr;
        /* Frame format descriptor for override[31:28] - Pixel/Line Code[27:16] - Number of pixels/lines*/
    }ISP_SMIARX_V1_DESCRIPTOR_3_ts;

}ISP_SMIARX_V1_DESCRIPTOR_3_tu;



//ISP_SMIARX_V1_DESCRIPTOR_4
/*Description: SMIA Frame Format Descriptor (snooped from status line),
*/
typedef union
{
    uint32_t word;
    uint32_t data;
    struct
    {
        uint16_t frame_format_descriptor;
        /* Frame format descriptor from status line[15:12] - Pixel/Line Code,[11:0] - Number of pixels/lines*/
        uint16_t frame_format_descriptor_ovr;
        /* Frame format descriptor for override[31:28] - Pixel/Line Code[27:16] - Number of pixels/lines*/
    }ISP_SMIARX_V1_DESCRIPTOR_4_ts;

}ISP_SMIARX_V1_DESCRIPTOR_4_tu;



//ISP_SMIARX_V1_DESCRIPTOR_5
/*Description: SMIA Frame Format Descriptor (snooped from status line),
*/
typedef union
{
    uint32_t word;
    uint32_t data;
    struct
    {
        uint16_t frame_format_descriptor;
        /* Frame format descriptor from status line[15:12] - Pixel/Line Code,[11:0] - Number of pixels/lines*/
        uint16_t frame_format_descriptor_ovr;
        /* Frame format descriptor for override[31:28] - Pixel/Line Code[27:16] - Number of pixels/lines*/
    }ISP_SMIARX_V1_DESCRIPTOR_5_ts;

}ISP_SMIARX_V1_DESCRIPTOR_5_tu;



//ISP_SMIARX_V1_DESCRIPTOR_6
/*Description: SMIA Frame Format Descriptor (snooped from status line),
*/
typedef union
{
    uint32_t word;
    uint32_t data;
    struct
    {
        uint16_t frame_format_descriptor;
        /* Frame format descriptor from status line[15:12] - Pixel/Line Code,[11:0] - Number of pixels/lines*/
        uint16_t frame_format_descriptor_ovr;
        /* Frame format descriptor for override[31:28] - Pixel/Line Code[27:16] - Number of pixels/lines*/
    }ISP_SMIARX_V1_DESCRIPTOR_6_ts;

}ISP_SMIARX_V1_DESCRIPTOR_6_tu;



//ISP_SMIARX_V1_DESCRIPTOR_7
/*Description: SMIA Frame Format Descriptor (snooped from status line),
*/
typedef union
{
    uint32_t word;
    uint32_t data;
    struct
    {
        uint16_t frame_format_descriptor;
        /* Frame format descriptor from status line[15:12] - Pixel/Line Code,[11:0] - Number of pixels/lines*/
        uint16_t frame_format_descriptor_ovr;
        /* Frame format descriptor for override[31:28] - Pixel/Line Code[27:16] - Number of pixels/lines*/
    }ISP_SMIARX_V1_DESCRIPTOR_7_ts;

}ISP_SMIARX_V1_DESCRIPTOR_7_tu;



//ISP_SMIARX_V1_DESCRIPTOR_8
/*Description: SMIA Frame Format Descriptor (snooped from status line),
*/
typedef union
{
    uint32_t word;
    uint32_t data;
    struct
    {
        uint16_t frame_format_descriptor;
        /* Frame format descriptor from status line[15:12] - Pixel/Line Code,[11:0] - Number of pixels/lines*/
        uint16_t frame_format_descriptor_ovr;
        /* Frame format descriptor for override[31:28] - Pixel/Line Code[27:16] - Number of pixels/lines*/
    }ISP_SMIARX_V1_DESCRIPTOR_8_ts;

}ISP_SMIARX_V1_DESCRIPTOR_8_tu;



//ISP_SMIARX_V1_DESCRIPTOR_9
/*Description: SMIA Frame Format Descriptor (snooped from status line),
*/
typedef union
{
    uint32_t word;
    uint32_t data;
    struct
    {
        uint16_t frame_format_descriptor;
        /* Frame format descriptor from status line[15:12] - Pixel/Line Code,[11:0] - Number of pixels/lines*/
        uint16_t frame_format_descriptor_ovr;
        /* Frame format descriptor for override[31:28] - Pixel/Line Code[27:16] - Number of pixels/lines*/
    }ISP_SMIARX_V1_DESCRIPTOR_9_ts;

}ISP_SMIARX_V1_DESCRIPTOR_9_tu;



//ISP_SMIARX_V1_DESCRIPTOR_10
/*Description: SMIA Frame Format Descriptor (snooped from status line),
*/
typedef union
{
    uint32_t word;
    uint32_t data;
    struct
    {
        uint16_t frame_format_descriptor;
        /* Frame format descriptor from status line[15:12] - Pixel/Line Code,[11:0] - Number of pixels/lines*/
        uint16_t frame_format_descriptor_ovr;
        /* Frame format descriptor for override[31:28] - Pixel/Line Code[27:16] - Number of pixels/lines*/
    }ISP_SMIARX_V1_DESCRIPTOR_10_ts;

}ISP_SMIARX_V1_DESCRIPTOR_10_tu;



//ISP_SMIARX_V1_DESCRIPTOR_11
/*Description: SMIA Frame Format Descriptor (snooped from status line),
*/
typedef union
{
    uint32_t word;
    uint32_t data;
    struct
    {
        uint16_t frame_format_descriptor;
        /* Frame format descriptor from status line[15:12] - Pixel/Line Code,[11:0] - Number of pixels/lines*/
        uint16_t frame_format_descriptor_ovr;
        /* Frame format descriptor for override[31:28] - Pixel/Line Code[27:16] - Number of pixels/lines*/
    }ISP_SMIARX_V1_DESCRIPTOR_11_ts;

}ISP_SMIARX_V1_DESCRIPTOR_11_tu;



//ISP_SMIARX_V1_DESCRIPTOR_12
/*Description: SMIA Frame Format Descriptor (snooped from status line),
*/
typedef union
{
    uint32_t word;
    uint32_t data;
    struct
    {
        uint16_t frame_format_descriptor;
        /* Frame format descriptor from status line[15:12] - Pixel/Line Code,[11:0] - Number of pixels/lines*/
        uint16_t frame_format_descriptor_ovr;
        /* Frame format descriptor for override[31:28] - Pixel/Line Code[27:16] - Number of pixels/lines*/
    }ISP_SMIARX_V1_DESCRIPTOR_12_ts;

}ISP_SMIARX_V1_DESCRIPTOR_12_tu;



//ISP_SMIARX_V1_DESCRIPTOR_13
/*Description: SMIA Frame Format Descriptor (snooped from status line),
*/
typedef union
{
    uint32_t word;
    uint32_t data;
    struct
    {
        uint16_t frame_format_descriptor;
        /* Frame format descriptor from status line[15:12] - Pixel/Line Code,[11:0] - Number of pixels/lines*/
        uint16_t frame_format_descriptor_ovr;
        /* Frame format descriptor for override[31:28] - Pixel/Line Code[27:16] - Number of pixels/lines*/
    }ISP_SMIARX_V1_DESCRIPTOR_13_ts;

}ISP_SMIARX_V1_DESCRIPTOR_13_tu;



//ISP_SMIARX_V1_DESCRIPTOR_14
/*Description: SMIA Frame Format Descriptor (snooped from status line),
*/
typedef union
{
    uint32_t word;
    uint32_t data;
    struct
    {
        uint16_t frame_format_descriptor;
        /* Frame format descriptor from status line[15:12] - Pixel/Line Code,[11:0] - Number of pixels/lines*/
        uint16_t frame_format_descriptor_ovr;
        /* Frame format descriptor for override[31:28] - Pixel/Line Code[27:16] - Number of pixels/lines*/
    }ISP_SMIARX_V1_DESCRIPTOR_14_ts;

}ISP_SMIARX_V1_DESCRIPTOR_14_tu;



//ISP_SMIARX_V1_TYPE_TRANS_0
/*Description: Defines translation SMIA to IDP linetype
*/
typedef union
{
    uint32_t word;
    uint32_t data;
    struct
    {
        uint8_t type_trans_black :3;
        /* IDP LineType value for SMIA BLACK lines*/
        uint8_t reserved0 :1;
        uint8_t type_trans_dark :3;
        /* IDP LineType value for SMIA DARK lines*/
        uint8_t reserved1 :1;
        uint8_t type_trans_visible :3;
        /* IDP LineType value for SMIA VISIBLE lines*/
        uint8_t reserved2 :1;
        uint8_t type_trans_dummy :3;
        /* IDP LineType value for SMIA DUMMY lines*/
        uint8_t reserved3 :1;
        uint8_t type_trans_mspec_0 :3;
        /* IDP LineType value for SMIA MAN_SPEC_0 lines*/
        uint8_t reserved4 :1;
        uint8_t type_trans_mspec_1 :3;
        /* IDP LineType value for SMIA MAN_SPEC_1 lines*/
        uint8_t reserved5 :1;
        uint8_t type_trans_mspec_2 :3;
        /* IDP LineType value for SMIA MAN_SPEC_2 lines*/
        uint8_t reserved6 :1;
        uint8_t type_trans_mspec_3 :3;
        /* IDP LineType value for SMIA MAN_SPEC_3 lines*/
    }ISP_SMIARX_V1_TYPE_TRANS_0_ts;

}ISP_SMIARX_V1_TYPE_TRANS_0_tu;



//ISP_SMIARX_V1_TYPE_TRANS_1
/*Description: Defines translation SMIA to IDP linetype
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t type_trans_mspec_4 :3;
        /* IDP LineType value for SMIA MAN_SPEC_4 lines*/
        uint8_t reserved0 :1;
        uint8_t type_trans_mspec_5 :3;
        /* IDP LineType value for SMIA MAN_SPEC_5 lines*/
        uint8_t reserved1 :1;
        uint8_t type_trans_mspec_6 :3;
        /* IDP LineType value for SMIA MAN_SPEC_6 lines*/
    }ISP_SMIARX_V1_TYPE_TRANS_1_ts;

}ISP_SMIARX_V1_TYPE_TRANS_1_tu;

typedef struct
{
    ISP_SMIARX_V1_CTRL_tu ISP_SMIARX_V1_CTRL;
    ISP_SMIARX_V1_SETUP_tu ISP_SMIARX_V1_SETUP;
    ISP_SMIARX_V1_FRAME_CTRL_tu ISP_SMIARX_V1_FRAME_CTRL;
    ISP_SMIARX_V1_STATUS_tu ISP_SMIARX_V1_STATUS;
    ISP_SMIARX_V1_TPAT_DATA_RG_tu ISP_SMIARX_V1_TPAT_DATA_RG;
    ISP_SMIARX_V1_TPAT_DATA_BG_tu ISP_SMIARX_V1_TPAT_DATA_BG;
    ISP_SMIARX_V1_TPAT_HCUR_WP_tu ISP_SMIARX_V1_TPAT_HCUR_WP;
    ISP_SMIARX_V1_TPAT_VCUR_WP_tu ISP_SMIARX_V1_TPAT_VCUR_WP;
    ISP_SMIARX_V1_CCP_STATUS_tu ISP_SMIARX_V1_CCP_STATUS;
    ISP_SMIARX_V1_HK_REQ_ADDR_0_tu ISP_SMIARX_V1_HK_REQ_ADDR_0;
    ISP_SMIARX_V1_HK_REQ_ADDR_1_tu ISP_SMIARX_V1_HK_REQ_ADDR_1;
    ISP_SMIARX_V1_HK_REQ_ADDR_2_tu ISP_SMIARX_V1_HK_REQ_ADDR_2;
    ISP_SMIARX_V1_HK_REQ_ADDR_3_tu ISP_SMIARX_V1_HK_REQ_ADDR_3;
    ISP_SMIARX_V1_HK_REQ_DATA_0_tu ISP_SMIARX_V1_HK_REQ_DATA_0;
    ISP_SMIARX_V1_HK_REQ_DATA_1_tu ISP_SMIARX_V1_HK_REQ_DATA_1;
    ISP_SMIARX_V1_HK_REQ_DATA_2_tu ISP_SMIARX_V1_HK_REQ_DATA_2;
    ISP_SMIARX_V1_HK_REQ_DATA_3_tu ISP_SMIARX_V1_HK_REQ_DATA_3;
    ISP_SMIARX_V1_INT_COUNT_0_tu ISP_SMIARX_V1_INT_COUNT_0;
    ISP_SMIARX_V1_INT_COUNT_1_tu ISP_SMIARX_V1_INT_COUNT_1;
    ISP_SMIARX_V1_INT_COUNT_2_tu ISP_SMIARX_V1_INT_COUNT_2;
    ISP_SMIARX_V1_INT_COUNT_3_tu ISP_SMIARX_V1_INT_COUNT_3;
    ISP_SMIARX_V1_DFV_tu ISP_SMIARX_V1_DFV;
    ISP_SMIARX_V1_COUNT_INT_EN_tu ISP_SMIARX_V1_COUNT_INT_EN;
    ISP_SMIARX_V1_OVERRIDE_tu ISP_SMIARX_V1_OVERRIDE;
    ISP_SMIARX_V1_SENSOR_ID_tu ISP_SMIARX_V1_SENSOR_ID;
    ISP_SMIARX_V1_PIXEL_ORDER_tu ISP_SMIARX_V1_PIXEL_ORDER;
    ISP_SMIARX_V1_AUTO_HENV_LENGTH_tu ISP_SMIARX_V1_AUTO_HENV_LENGTH;
    ISP_SMIARX_V1_FRAME_LENGTH_tu ISP_SMIARX_V1_FRAME_LENGTH;
    ISP_SMIARX_V1_LINE_LENGTH_tu ISP_SMIARX_V1_LINE_LENGTH;
    ISP_SMIARX_V1_X_ADDR_START_tu ISP_SMIARX_V1_X_ADDR_START;
    ISP_SMIARX_V1_Y_ADDR_START_tu ISP_SMIARX_V1_Y_ADDR_START;
    ISP_SMIARX_V1_X_ADDR_END_tu ISP_SMIARX_V1_X_ADDR_END;
    ISP_SMIARX_V1_Y_ADDR_END_tu ISP_SMIARX_V1_Y_ADDR_END;
    ISP_SMIARX_V1_X_OUTPUT_SIZE_tu ISP_SMIARX_V1_X_OUTPUT_SIZE;
    ISP_SMIARX_V1_Y_OUTPUT_SIZE_tu ISP_SMIARX_V1_Y_OUTPUT_SIZE;
    ISP_SMIARX_V1_ADDR_INCREMENTS_tu ISP_SMIARX_V1_ADDR_INCREMENTS;
    ISP_SMIARX_V1_FINE_EXPOSURE_tu ISP_SMIARX_V1_FINE_EXPOSURE;
    ISP_SMIARX_V1_COARSE_EXPOSURE_tu ISP_SMIARX_V1_COARSE_EXPOSURE;
    ISP_SMIARX_V1_ANALOG_GAIN_tu ISP_SMIARX_V1_ANALOG_GAIN;
    ISP_SMIARX_V1_MODEL_SUBTYPE_tu ISP_SMIARX_V1_MODEL_SUBTYPE;
    ISP_SMIARX_V1_DESCRIPTOR_0_tu ISP_SMIARX_V1_DESCRIPTOR_0;
    ISP_SMIARX_V1_DESCRIPTOR_1_tu ISP_SMIARX_V1_DESCRIPTOR_1;
    ISP_SMIARX_V1_DESCRIPTOR_2_tu ISP_SMIARX_V1_DESCRIPTOR_2;
    ISP_SMIARX_V1_DESCRIPTOR_3_tu ISP_SMIARX_V1_DESCRIPTOR_3;
    ISP_SMIARX_V1_DESCRIPTOR_4_tu ISP_SMIARX_V1_DESCRIPTOR_4;
    ISP_SMIARX_V1_DESCRIPTOR_5_tu ISP_SMIARX_V1_DESCRIPTOR_5;
    ISP_SMIARX_V1_DESCRIPTOR_6_tu ISP_SMIARX_V1_DESCRIPTOR_6;
    ISP_SMIARX_V1_DESCRIPTOR_7_tu ISP_SMIARX_V1_DESCRIPTOR_7;
    ISP_SMIARX_V1_DESCRIPTOR_8_tu ISP_SMIARX_V1_DESCRIPTOR_8;
    ISP_SMIARX_V1_DESCRIPTOR_9_tu ISP_SMIARX_V1_DESCRIPTOR_9;
    ISP_SMIARX_V1_DESCRIPTOR_10_tu ISP_SMIARX_V1_DESCRIPTOR_10;
    ISP_SMIARX_V1_DESCRIPTOR_11_tu ISP_SMIARX_V1_DESCRIPTOR_11;
    ISP_SMIARX_V1_DESCRIPTOR_12_tu ISP_SMIARX_V1_DESCRIPTOR_12;
    ISP_SMIARX_V1_DESCRIPTOR_13_tu ISP_SMIARX_V1_DESCRIPTOR_13;
    ISP_SMIARX_V1_DESCRIPTOR_14_tu ISP_SMIARX_V1_DESCRIPTOR_14;
    ISP_SMIARX_V1_TYPE_TRANS_0_tu ISP_SMIARX_V1_TYPE_TRANS_0;
    ISP_SMIARX_V1_TYPE_TRANS_1_tu ISP_SMIARX_V1_TYPE_TRANS_1;
}ISP_SMIARX_V1_IP_ts;




//ISP_SMIARX_V1_CTRL


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_CTRL_rx_start() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_CTRL.ISP_SMIARX_V1_CTRL_ts.rx_start
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_CTRL_rx_start(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_CTRL.ISP_SMIARX_V1_CTRL_ts.rx_start = x)


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_CTRL_rx_stop() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_CTRL.ISP_SMIARX_V1_CTRL_ts.rx_stop
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_CTRL_rx_stop(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_CTRL.ISP_SMIARX_V1_CTRL_ts.rx_stop = x)


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_CTRL_rx_abort() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_CTRL.ISP_SMIARX_V1_CTRL_ts.rx_abort
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_CTRL_rx_abort(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_CTRL.ISP_SMIARX_V1_CTRL_ts.rx_abort = x)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_CTRL(rx_start,rx_stop,rx_abort) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_CTRL.word = (uint32_t)rx_start<<0 | (uint32_t)rx_stop<<8 | (uint32_t)rx_abort<<16)
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_CTRL() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_CTRL.word
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_CTRL_word(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_CTRL.word = x)


//ISP_SMIARX_V1_SETUP


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_ip_pix_width() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.ip_pix_width
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_ip_pix_width_B_0x6() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.ip_pix_width == ip_pix_width_B_0x6)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_ip_pix_width__B_0x6() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.ip_pix_width = ip_pix_width_B_0x6)
#define ip_pix_width_B_0x6 0x6    //(6_BITS)
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_ip_pix_width_B_0x7() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.ip_pix_width == ip_pix_width_B_0x7)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_ip_pix_width__B_0x7() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.ip_pix_width = ip_pix_width_B_0x7)
#define ip_pix_width_B_0x7 0x7    //(7_BITS)
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_ip_pix_width_B_0x8() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.ip_pix_width == ip_pix_width_B_0x8)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_ip_pix_width__B_0x8() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.ip_pix_width = ip_pix_width_B_0x8)
#define ip_pix_width_B_0x8 0x8    //(8_BITS)
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_ip_pix_width_B_0xA() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.ip_pix_width == ip_pix_width_B_0xA)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_ip_pix_width__B_0xA() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.ip_pix_width = ip_pix_width_B_0xA)
#define ip_pix_width_B_0xA 0xA    //(10_BITS)
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_ip_pix_width_B_0xC() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.ip_pix_width == ip_pix_width_B_0xC)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_ip_pix_width__B_0xC() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.ip_pix_width = ip_pix_width_B_0xC)
#define ip_pix_width_B_0xC 0xC    //(12_BITS)


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_op_pix_width() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.op_pix_width
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_op_pix_width_B_0x6() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.op_pix_width == op_pix_width_B_0x6)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_op_pix_width__B_0x6() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.op_pix_width = op_pix_width_B_0x6)
#define op_pix_width_B_0x6 0x6    //(6_BITS)
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_op_pix_width_B_0x7() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.op_pix_width == op_pix_width_B_0x7)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_op_pix_width__B_0x7() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.op_pix_width = op_pix_width_B_0x7)
#define op_pix_width_B_0x7 0x7    //(7_BITS)
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_op_pix_width_B_0x8() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.op_pix_width == op_pix_width_B_0x8)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_op_pix_width__B_0x8() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.op_pix_width = op_pix_width_B_0x8)
#define op_pix_width_B_0x8 0x8    //(8_BITS)
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_op_pix_width_B_0xA() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.op_pix_width == op_pix_width_B_0xA)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_op_pix_width__B_0xA() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.op_pix_width = op_pix_width_B_0xA)
#define op_pix_width_B_0xA 0xA    //(10_BITS)
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_op_pix_width_B_0xC() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.op_pix_width == op_pix_width_B_0xC)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_op_pix_width__B_0xC() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.op_pix_width = op_pix_width_B_0xC)
#define op_pix_width_B_0xC 0xC    //(12_BITS)


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_parallel_enable() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.parallel_enable
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_parallel_enable(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.parallel_enable = x)


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_vsync_polarity() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.vsync_polarity
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_vsync_polarity_ACTIVE_LOW() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.vsync_polarity == vsync_polarity_ACTIVE_LOW)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_vsync_polarity__ACTIVE_LOW() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.vsync_polarity = vsync_polarity_ACTIVE_LOW)
#define vsync_polarity_ACTIVE_LOW 0x0    //Active Low
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_vsync_polarity_ACTIVE_HIGH() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.vsync_polarity == vsync_polarity_ACTIVE_HIGH)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_vsync_polarity__ACTIVE_HIGH() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.vsync_polarity = vsync_polarity_ACTIVE_HIGH)
#define vsync_polarity_ACTIVE_HIGH 0x1    //Active High


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_hsync_polarity() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.hsync_polarity
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_hsync_polarity_ACTIVE_LOW() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.hsync_polarity == hsync_polarity_ACTIVE_LOW)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_hsync_polarity__ACTIVE_LOW() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.hsync_polarity = hsync_polarity_ACTIVE_LOW)
#define hsync_polarity_ACTIVE_LOW 0x0    //Active Low
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_hsync_polarity_ACTIVE_HIGH() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.hsync_polarity == hsync_polarity_ACTIVE_HIGH)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_hsync_polarity__ACTIVE_HIGH() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.hsync_polarity = hsync_polarity_ACTIVE_HIGH)
#define hsync_polarity_ACTIVE_HIGH 0x1    //Active High


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_ds_mode() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.ds_mode
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_ds_mode_DISABLE() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.ds_mode == ds_mode_DISABLE)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_ds_mode__DISABLE() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.ds_mode = ds_mode_DISABLE)
#define ds_mode_DISABLE 0x0    //Disable Data/Strobe encoding
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_ds_mode_ENABLE() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.ds_mode == ds_mode_ENABLE)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_ds_mode__ENABLE() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.ds_mode = ds_mode_ENABLE)
#define ds_mode_ENABLE 0x1    //Enable Data/Strobe encoding


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_dcpx_enable() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.dcpx_enable
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_dcpx_enable_DISABLE() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.dcpx_enable == dcpx_enable_DISABLE)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_dcpx_enable__DISABLE() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.dcpx_enable = dcpx_enable_DISABLE)
#define dcpx_enable_DISABLE 0x0    //Disable bayer decompression
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_dcpx_enable_ENABLE() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.dcpx_enable == dcpx_enable_ENABLE)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_dcpx_enable__ENABLE() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.dcpx_enable = dcpx_enable_ENABLE)
#define dcpx_enable_ENABLE 0x1    //Enable bayer decompression


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_ccp_sel() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.ccp_sel
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_ccp_sel(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.ccp_sel = x)


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_reset_ccp_rx_n() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.reset_ccp_rx_n
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_reset_ccp_rx_n(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.reset_ccp_rx_n = x)


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_auto_sync() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.auto_sync
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_auto_sync_SYNCHRO_SENSOR() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.auto_sync == auto_sync_SYNCHRO_SENSOR)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_auto_sync__SYNCHRO_SENSOR() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.auto_sync = auto_sync_SYNCHRO_SENSOR)
#define auto_sync_SYNCHRO_SENSOR 0x0    //Sensor (Serial or Parallel)
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_auto_sync_SYNCHRO_AUTO() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.auto_sync == auto_sync_SYNCHRO_AUTO)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_auto_sync__SYNCHRO_AUTO() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.auto_sync = auto_sync_SYNCHRO_AUTO)
#define auto_sync_SYNCHRO_AUTO 0x1    //Auto on rx_start


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_pattern_type() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.pattern_type
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_pattern_type_PATTERN_DISABLE() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.pattern_type == pattern_type_PATTERN_DISABLE)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_pattern_type__PATTERN_DISABLE() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.pattern_type = pattern_type_PATTERN_DISABLE)
#define pattern_type_PATTERN_DISABLE 0x0    //Disabled
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_pattern_type_PATTERN_COLOUR_BAR() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.pattern_type == pattern_type_PATTERN_COLOUR_BAR)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_pattern_type__PATTERN_COLOUR_BAR() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.pattern_type = pattern_type_PATTERN_COLOUR_BAR)
#define pattern_type_PATTERN_COLOUR_BAR 0x1    //Colour Bars
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_pattern_type_PATTERN_GRAD_BAR() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.pattern_type == pattern_type_PATTERN_GRAD_BAR)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_pattern_type__PATTERN_GRAD_BAR() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.pattern_type = pattern_type_PATTERN_GRAD_BAR)
#define pattern_type_PATTERN_GRAD_BAR 0x2    //Graduated Colour Bars
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_pattern_type_PATTERN_DIAG_GREY() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.pattern_type == pattern_type_PATTERN_DIAG_GREY)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_pattern_type__PATTERN_DIAG_GREY() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.pattern_type = pattern_type_PATTERN_DIAG_GREY)
#define pattern_type_PATTERN_DIAG_GREY 0x3    //Diagonal Greyscale
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_pattern_type_PATTERN_PSEUDORANDOM() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.pattern_type == pattern_type_PATTERN_PSEUDORANDOM)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_pattern_type__PATTERN_PSEUDORANDOM() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.pattern_type = pattern_type_PATTERN_PSEUDORANDOM)
#define pattern_type_PATTERN_PSEUDORANDOM 0x4    //Pseudo-random
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_pattern_type_PATTERN_HOR_GREY() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.pattern_type == pattern_type_PATTERN_HOR_GREY)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_pattern_type__PATTERN_HOR_GREY() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.pattern_type = pattern_type_PATTERN_HOR_GREY)
#define pattern_type_PATTERN_HOR_GREY 0x5    //Horizontal Greyscale
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_pattern_type_PATTERN_VERT_GREY() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.pattern_type == pattern_type_PATTERN_VERT_GREY)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_pattern_type__PATTERN_VERT_GREY() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.pattern_type = pattern_type_PATTERN_VERT_GREY)
#define pattern_type_PATTERN_VERT_GREY 0x6    //Vertical Greyscale
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_pattern_type_PATTERN_SOLID() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.pattern_type == pattern_type_PATTERN_SOLID)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_pattern_type__PATTERN_SOLID() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.pattern_type = pattern_type_PATTERN_SOLID)
#define pattern_type_PATTERN_SOLID 0x7    //Solid colour


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_no_sof() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.no_sof
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_no_sof(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.no_sof = x)


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_clr_ccp_shift_sync() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.clr_ccp_shift_sync
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_clr_ccp_shift_sync(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.clr_ccp_shift_sync = x)


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_clr_ccp_crc_error() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.clr_ccp_crc_error
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_clr_ccp_crc_error(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.clr_ccp_crc_error = x)


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_clr_pixel_sync_error() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.clr_pixel_sync_error
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_clr_pixel_sync_error(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.clr_pixel_sync_error = x)


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_clr_aborted_iframe() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.clr_aborted_iframe
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_clr_aborted_iframe(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.clr_aborted_iframe = x)


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_clr_insuff_iline() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.clr_insuff_iline
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_clr_insuff_iline(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.clr_insuff_iline = x)


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_fifo_rd_rst() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.fifo_rd_rst
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_fifo_rd_rst(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.ISP_SMIARX_V1_SETUP_ts.fifo_rd_rst = x)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP(ip_pix_width,op_pix_width,parallel_enable,vsync_polarity,hsync_polarity,ds_mode,dcpx_enable,ccp_sel,reset_ccp_rx_n,auto_sync,pattern_type,no_sof,clr_ccp_shift_sync,clr_ccp_crc_error,clr_pixel_sync_error,clr_aborted_iframe,clr_insuff_iline,fifo_rd_rst) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.word = (uint32_t)ip_pix_width<<0 | (uint32_t)op_pix_width<<4 | (uint32_t)parallel_enable<<8 | (uint32_t)vsync_polarity<<9 | (uint32_t)hsync_polarity<<10 | (uint32_t)ds_mode<<11 | (uint32_t)dcpx_enable<<12 | (uint32_t)ccp_sel<<13 | (uint32_t)reset_ccp_rx_n<<14 | (uint32_t)auto_sync<<16 | (uint32_t)pattern_type<<17 | (uint32_t)no_sof<<20 | (uint32_t)clr_ccp_shift_sync<<24 | (uint32_t)clr_ccp_crc_error<<25 | (uint32_t)clr_pixel_sync_error<<26 | (uint32_t)clr_aborted_iframe<<27 | (uint32_t)clr_insuff_iline<<28 | (uint32_t)fifo_rd_rst<<29)
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.word
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_SETUP_word(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SETUP.word = x)


//ISP_SMIARX_V1_FRAME_CTRL


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_FRAME_CTRL_latency_lines() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_FRAME_CTRL.ISP_SMIARX_V1_FRAME_CTRL_ts.latency_lines
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_FRAME_CTRL_latency_lines(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_FRAME_CTRL.ISP_SMIARX_V1_FRAME_CTRL_ts.latency_lines = x)


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_FRAME_CTRL_stream_size() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_FRAME_CTRL.ISP_SMIARX_V1_FRAME_CTRL_ts.stream_size
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_FRAME_CTRL_stream_size(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_FRAME_CTRL.ISP_SMIARX_V1_FRAME_CTRL_ts.stream_size = x)


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_FRAME_CTRL_start_grab_dly() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_FRAME_CTRL.ISP_SMIARX_V1_FRAME_CTRL_ts.start_grab_dly
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_FRAME_CTRL_start_grab_dly(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_FRAME_CTRL.ISP_SMIARX_V1_FRAME_CTRL_ts.start_grab_dly = x)


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_FRAME_CTRL_ffhc_control() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_FRAME_CTRL.ISP_SMIARX_V1_FRAME_CTRL_ts.ffhc_control
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_FRAME_CTRL_ffhc_control_FFHC_DISABLE() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_FRAME_CTRL.ISP_SMIARX_V1_FRAME_CTRL_ts.ffhc_control == ffhc_control_FFHC_DISABLE)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_FRAME_CTRL_ffhc_control__FFHC_DISABLE() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_FRAME_CTRL.ISP_SMIARX_V1_FRAME_CTRL_ts.ffhc_control = ffhc_control_FFHC_DISABLE)
#define ffhc_control_FFHC_DISABLE 0x0    //Disable/Bypass
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_FRAME_CTRL_ffhc_control_FFHC_FULL_AUTO() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_FRAME_CTRL.ISP_SMIARX_V1_FRAME_CTRL_ts.ffhc_control == ffhc_control_FFHC_FULL_AUTO)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_FRAME_CTRL_ffhc_control__FFHC_FULL_AUTO() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_FRAME_CTRL.ISP_SMIARX_V1_FRAME_CTRL_ts.ffhc_control = ffhc_control_FFHC_FULL_AUTO)
#define ffhc_control_FFHC_FULL_AUTO 0x1    //Fully automatic
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_FRAME_CTRL_ffhc_control_FFHC_SEMI_AUTO() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_FRAME_CTRL.ISP_SMIARX_V1_FRAME_CTRL_ts.ffhc_control == ffhc_control_FFHC_SEMI_AUTO)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_FRAME_CTRL_ffhc_control__FFHC_SEMI_AUTO() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_FRAME_CTRL.ISP_SMIARX_V1_FRAME_CTRL_ts.ffhc_control = ffhc_control_FFHC_SEMI_AUTO)
#define ffhc_control_FFHC_SEMI_AUTO 0x2    //Semi-automatic (Static SOL columns)
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_FRAME_CTRL_ffhc_control_FFHC_MANUAL() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_FRAME_CTRL.ISP_SMIARX_V1_FRAME_CTRL_ts.ffhc_control == ffhc_control_FFHC_MANUAL)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_FRAME_CTRL_ffhc_control__FFHC_MANUAL() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_FRAME_CTRL.ISP_SMIARX_V1_FRAME_CTRL_ts.ffhc_control = ffhc_control_FFHC_MANUAL)
#define ffhc_control_FFHC_MANUAL 0x3    //Manual (Static SOL and EOL columns)


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_FRAME_CTRL_stop_at_interframe() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_FRAME_CTRL.ISP_SMIARX_V1_FRAME_CTRL_ts.stop_at_interframe
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_FRAME_CTRL_stop_at_interframe(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_FRAME_CTRL.ISP_SMIARX_V1_FRAME_CTRL_ts.stop_at_interframe = x)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_FRAME_CTRL(latency_lines,stream_size,start_grab_dly,ffhc_control,stop_at_interframe) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_FRAME_CTRL.word = (uint32_t)latency_lines<<0 | (uint32_t)stream_size<<8 | (uint32_t)start_grab_dly<<16 | (uint32_t)ffhc_control<<24 | (uint32_t)stop_at_interframe<<26)
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_FRAME_CTRL() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_FRAME_CTRL.word
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_FRAME_CTRL_word(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_FRAME_CTRL.word = x)


//ISP_SMIARX_V1_STATUS


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_STATUS_frame_counter() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_STATUS.ISP_SMIARX_V1_STATUS_ts.frame_counter


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_STATUS_rx_state() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_STATUS.ISP_SMIARX_V1_STATUS_ts.rx_state
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_STATUS_rx_state_GEN_IDLE() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_STATUS.ISP_SMIARX_V1_STATUS_ts.rx_state == rx_state_GEN_IDLE)
#define rx_state_GEN_IDLE 0x0    //Idle
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_STATUS_rx_state_GEN_WAITVSTART() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_STATUS.ISP_SMIARX_V1_STATUS_ts.rx_state == rx_state_GEN_WAITVSTART)
#define rx_state_GEN_WAITVSTART 0x1    //wait vstart
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_STATUS_rx_state_GEN_RUNNING() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_STATUS.ISP_SMIARX_V1_STATUS_ts.rx_state == rx_state_GEN_RUNNING)
#define rx_state_GEN_RUNNING 0x2    //running
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_STATUS_rx_state_GEN_WAITFRAME() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_STATUS.ISP_SMIARX_V1_STATUS_ts.rx_state == rx_state_GEN_WAITFRAME)
#define rx_state_GEN_WAITFRAME 0x3    //wait frame


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_STATUS_ff_state() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_STATUS.ISP_SMIARX_V1_STATUS_ts.ff_state
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_STATUS_ff_state_FF_IDLE() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_STATUS.ISP_SMIARX_V1_STATUS_ts.ff_state == ff_state_FF_IDLE)
#define ff_state_FF_IDLE 0x0    //Idle
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_STATUS_ff_state_FF_WAIT_EOSD() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_STATUS.ISP_SMIARX_V1_STATUS_ts.ff_state == ff_state_FF_WAIT_EOSD)
#define ff_state_FF_WAIT_EOSD 0x1    //Waiting for end of status/Data
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_STATUS_ff_state_FF_FIRSTLINE() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_STATUS.ISP_SMIARX_V1_STATUS_ts.ff_state == ff_state_FF_FIRSTLINE)
#define ff_state_FF_FIRSTLINE 0x2    //First line of frame (1-cycle state)
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_STATUS_ff_state_FF_TRANSLATE() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_STATUS.ISP_SMIARX_V1_STATUS_ts.ff_state == ff_state_FF_TRANSLATE)
#define ff_state_FF_TRANSLATE 0x3    //Translate current row/descriptor
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_STATUS_ff_state_FF_UPDATE() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_STATUS.ISP_SMIARX_V1_STATUS_ts.ff_state == ff_state_FF_UPDATE)
#define ff_state_FF_UPDATE 0x4    //Update row/descriptor
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_STATUS_ff_state_FF_INTERFRAME() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_STATUS.ISP_SMIARX_V1_STATUS_ts.ff_state == ff_state_FF_INTERFRAME)
#define ff_state_FF_INTERFRAME 0x5    //Interframe


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_STATUS_snoop_state() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_STATUS.ISP_SMIARX_V1_STATUS_ts.snoop_state
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_STATUS_snoop_state_SNOOP_IDLE() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_STATUS.ISP_SMIARX_V1_STATUS_ts.snoop_state == snoop_state_SNOOP_IDLE)
#define snoop_state_SNOOP_IDLE 0x0    //Idle
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_STATUS_snoop_state_SNOOP_TAG_LSN() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_STATUS.ISP_SMIARX_V1_STATUS_ts.snoop_state == snoop_state_SNOOP_TAG_LSN)
#define snoop_state_SNOOP_TAG_LSN 0x1    //Data tag (LS-Nibble)
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_STATUS_snoop_state_SNOOP_TAG() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_STATUS.ISP_SMIARX_V1_STATUS_ts.snoop_state == snoop_state_SNOOP_TAG)
#define snoop_state_SNOOP_TAG 0x2    //Data Tag
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_STATUS_snoop_state_SNOOP_REG_LSM() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_STATUS.ISP_SMIARX_V1_STATUS_ts.snoop_state == snoop_state_SNOOP_REG_LSM)
#define snoop_state_SNOOP_REG_LSM 0x3    //Register Data (LS-Nibble)
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_STATUS_snoop_state_SNOOP_REG() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_STATUS.ISP_SMIARX_V1_STATUS_ts.snoop_state == snoop_state_SNOOP_REG)
#define snoop_state_SNOOP_REG 0x4    //Register Data


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_STATUS_fifo_rd_usedw() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_STATUS.ISP_SMIARX_V1_STATUS_ts.fifo_rd_usedw


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_STATUS_fifo_empty() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_STATUS.ISP_SMIARX_V1_STATUS_ts.fifo_empty


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_STATUS_pixel_sync_error() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_STATUS.ISP_SMIARX_V1_STATUS_ts.pixel_sync_error


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_STATUS_aborted_iframe() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_STATUS.ISP_SMIARX_V1_STATUS_ts.aborted_iframe


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_STATUS_insuff_iline() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_STATUS.ISP_SMIARX_V1_STATUS_ts.insuff_iline


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_STATUS_fifo_under() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_STATUS.ISP_SMIARX_V1_STATUS_ts.fifo_under


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_STATUS_fifo_over() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_STATUS.ISP_SMIARX_V1_STATUS_ts.fifo_over
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_STATUS() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_STATUS.word


//ISP_SMIARX_V1_TPAT_DATA_RG


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_TPAT_DATA_RG_tpat_data_gir() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TPAT_DATA_RG.ISP_SMIARX_V1_TPAT_DATA_RG_ts.tpat_data_gir
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TPAT_DATA_RG_tpat_data_gir(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TPAT_DATA_RG.ISP_SMIARX_V1_TPAT_DATA_RG_ts.tpat_data_gir = x)


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_TPAT_DATA_RG_tpat_data_red() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TPAT_DATA_RG.ISP_SMIARX_V1_TPAT_DATA_RG_ts.tpat_data_red
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TPAT_DATA_RG_tpat_data_red(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TPAT_DATA_RG.ISP_SMIARX_V1_TPAT_DATA_RG_ts.tpat_data_red = x)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TPAT_DATA_RG(tpat_data_gir,tpat_data_red) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TPAT_DATA_RG.word = (uint32_t)tpat_data_gir<<0 | (uint32_t)tpat_data_red<<16)
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_TPAT_DATA_RG() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TPAT_DATA_RG.word
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TPAT_DATA_RG_word(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TPAT_DATA_RG.word = x)


//ISP_SMIARX_V1_TPAT_DATA_BG


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_TPAT_DATA_BG_tpat_data_gib() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TPAT_DATA_BG.ISP_SMIARX_V1_TPAT_DATA_BG_ts.tpat_data_gib
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TPAT_DATA_BG_tpat_data_gib(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TPAT_DATA_BG.ISP_SMIARX_V1_TPAT_DATA_BG_ts.tpat_data_gib = x)


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_TPAT_DATA_BG_tpat_data_blu() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TPAT_DATA_BG.ISP_SMIARX_V1_TPAT_DATA_BG_ts.tpat_data_blu
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TPAT_DATA_BG_tpat_data_blu(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TPAT_DATA_BG.ISP_SMIARX_V1_TPAT_DATA_BG_ts.tpat_data_blu = x)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TPAT_DATA_BG(tpat_data_gib,tpat_data_blu) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TPAT_DATA_BG.word = (uint32_t)tpat_data_gib<<0 | (uint32_t)tpat_data_blu<<16)
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_TPAT_DATA_BG() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TPAT_DATA_BG.word
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TPAT_DATA_BG_word(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TPAT_DATA_BG.word = x)


//ISP_SMIARX_V1_TPAT_HCUR_WP


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_TPAT_HCUR_WP_tpat_hcur_posn() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TPAT_HCUR_WP.ISP_SMIARX_V1_TPAT_HCUR_WP_ts.tpat_hcur_posn
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TPAT_HCUR_WP_tpat_hcur_posn(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TPAT_HCUR_WP.ISP_SMIARX_V1_TPAT_HCUR_WP_ts.tpat_hcur_posn = x)


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_TPAT_HCUR_WP_tpat_hcur_width() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TPAT_HCUR_WP.ISP_SMIARX_V1_TPAT_HCUR_WP_ts.tpat_hcur_width
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TPAT_HCUR_WP_tpat_hcur_width(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TPAT_HCUR_WP.ISP_SMIARX_V1_TPAT_HCUR_WP_ts.tpat_hcur_width = x)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TPAT_HCUR_WP(tpat_hcur_posn,tpat_hcur_width) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TPAT_HCUR_WP.word = (uint32_t)tpat_hcur_posn<<0 | (uint32_t)tpat_hcur_width<<16)
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_TPAT_HCUR_WP() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TPAT_HCUR_WP.word
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TPAT_HCUR_WP_word(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TPAT_HCUR_WP.word = x)


//ISP_SMIARX_V1_TPAT_VCUR_WP


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_TPAT_VCUR_WP_tpat_vcur_posn() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TPAT_VCUR_WP.ISP_SMIARX_V1_TPAT_VCUR_WP_ts.tpat_vcur_posn
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TPAT_VCUR_WP_tpat_vcur_posn(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TPAT_VCUR_WP.ISP_SMIARX_V1_TPAT_VCUR_WP_ts.tpat_vcur_posn = x)


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_TPAT_VCUR_WP_tpat_vcur_width() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TPAT_VCUR_WP.ISP_SMIARX_V1_TPAT_VCUR_WP_ts.tpat_vcur_width
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TPAT_VCUR_WP_tpat_vcur_width(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TPAT_VCUR_WP.ISP_SMIARX_V1_TPAT_VCUR_WP_ts.tpat_vcur_width = x)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TPAT_VCUR_WP(tpat_vcur_posn,tpat_vcur_width) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TPAT_VCUR_WP.word = (uint32_t)tpat_vcur_posn<<0 | (uint32_t)tpat_vcur_width<<16)
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_TPAT_VCUR_WP() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TPAT_VCUR_WP.word
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TPAT_VCUR_WP_word(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TPAT_VCUR_WP.word = x)


//ISP_SMIARX_V1_CCP_STATUS


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_CCP_STATUS_ccp_channel() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_CCP_STATUS.ISP_SMIARX_V1_CCP_STATUS_ts.ccp_channel


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_CCP_STATUS_ccp_false_sync() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_CCP_STATUS.ISP_SMIARX_V1_CCP_STATUS_ts.ccp_false_sync


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_CCP_STATUS_ccp_shift_sync() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_CCP_STATUS.ISP_SMIARX_V1_CCP_STATUS_ts.ccp_shift_sync


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_CCP_STATUS_ccp_crc_error() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_CCP_STATUS.ISP_SMIARX_V1_CCP_STATUS_ts.ccp_crc_error
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_CCP_STATUS() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_CCP_STATUS.word


//ISP_SMIARX_V1_HK_REQ_ADDR_0


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_HK_REQ_ADDR_0_hk_req_addr() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_HK_REQ_ADDR_0.ISP_SMIARX_V1_HK_REQ_ADDR_0_ts.hk_req_addr
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_HK_REQ_ADDR_0_hk_req_addr(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_HK_REQ_ADDR_0.ISP_SMIARX_V1_HK_REQ_ADDR_0_ts.hk_req_addr = x)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_HK_REQ_ADDR_0(hk_req_addr) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_HK_REQ_ADDR_0.word = (uint32_t)hk_req_addr<<0)
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_HK_REQ_ADDR_0() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_HK_REQ_ADDR_0.word
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_HK_REQ_ADDR_0_word(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_HK_REQ_ADDR_0.word = x)


//ISP_SMIARX_V1_HK_REQ_ADDR_1


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_HK_REQ_ADDR_1_hk_req_addr() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_HK_REQ_ADDR_1.ISP_SMIARX_V1_HK_REQ_ADDR_1_ts.hk_req_addr
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_HK_REQ_ADDR_1_hk_req_addr(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_HK_REQ_ADDR_1.ISP_SMIARX_V1_HK_REQ_ADDR_1_ts.hk_req_addr = x)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_HK_REQ_ADDR_1(hk_req_addr) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_HK_REQ_ADDR_1.word = (uint32_t)hk_req_addr<<0)
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_HK_REQ_ADDR_1() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_HK_REQ_ADDR_1.word
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_HK_REQ_ADDR_1_word(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_HK_REQ_ADDR_1.word = x)


//ISP_SMIARX_V1_HK_REQ_ADDR_2


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_HK_REQ_ADDR_2_hk_req_addr() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_HK_REQ_ADDR_2.ISP_SMIARX_V1_HK_REQ_ADDR_2_ts.hk_req_addr
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_HK_REQ_ADDR_2_hk_req_addr(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_HK_REQ_ADDR_2.ISP_SMIARX_V1_HK_REQ_ADDR_2_ts.hk_req_addr = x)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_HK_REQ_ADDR_2(hk_req_addr) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_HK_REQ_ADDR_2.word = (uint32_t)hk_req_addr<<0)
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_HK_REQ_ADDR_2() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_HK_REQ_ADDR_2.word
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_HK_REQ_ADDR_2_word(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_HK_REQ_ADDR_2.word = x)


//ISP_SMIARX_V1_HK_REQ_ADDR_3


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_HK_REQ_ADDR_3_hk_req_addr() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_HK_REQ_ADDR_3.ISP_SMIARX_V1_HK_REQ_ADDR_3_ts.hk_req_addr
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_HK_REQ_ADDR_3_hk_req_addr(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_HK_REQ_ADDR_3.ISP_SMIARX_V1_HK_REQ_ADDR_3_ts.hk_req_addr = x)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_HK_REQ_ADDR_3(hk_req_addr) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_HK_REQ_ADDR_3.word = (uint32_t)hk_req_addr<<0)
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_HK_REQ_ADDR_3() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_HK_REQ_ADDR_3.word
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_HK_REQ_ADDR_3_word(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_HK_REQ_ADDR_3.word = x)


//ISP_SMIARX_V1_HK_REQ_DATA_0


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_HK_REQ_DATA_0_hk_req_data() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_HK_REQ_DATA_0.ISP_SMIARX_V1_HK_REQ_DATA_0_ts.hk_req_data
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_HK_REQ_DATA_0() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_HK_REQ_DATA_0.word = )
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_HK_REQ_DATA_0() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_HK_REQ_DATA_0.word
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_HK_REQ_DATA_0_word(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_HK_REQ_DATA_0.word = x)


//ISP_SMIARX_V1_HK_REQ_DATA_1


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_HK_REQ_DATA_1_hk_req_data() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_HK_REQ_DATA_1.ISP_SMIARX_V1_HK_REQ_DATA_1_ts.hk_req_data
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_HK_REQ_DATA_1() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_HK_REQ_DATA_1.word = )
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_HK_REQ_DATA_1() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_HK_REQ_DATA_1.word
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_HK_REQ_DATA_1_word(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_HK_REQ_DATA_1.word = x)


//ISP_SMIARX_V1_HK_REQ_DATA_2


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_HK_REQ_DATA_2_hk_req_data() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_HK_REQ_DATA_2.ISP_SMIARX_V1_HK_REQ_DATA_2_ts.hk_req_data
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_HK_REQ_DATA_2() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_HK_REQ_DATA_2.word = )
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_HK_REQ_DATA_2() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_HK_REQ_DATA_2.word
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_HK_REQ_DATA_2_word(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_HK_REQ_DATA_2.word = x)


//ISP_SMIARX_V1_HK_REQ_DATA_3


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_HK_REQ_DATA_3_hk_req_data() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_HK_REQ_DATA_3.ISP_SMIARX_V1_HK_REQ_DATA_3_ts.hk_req_data
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_HK_REQ_DATA_3() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_HK_REQ_DATA_3.word = )
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_HK_REQ_DATA_3() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_HK_REQ_DATA_3.word
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_HK_REQ_DATA_3_word(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_HK_REQ_DATA_3.word = x)


//ISP_SMIARX_V1_INT_COUNT_0


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_INT_COUNT_0_pix_count() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_INT_COUNT_0.ISP_SMIARX_V1_INT_COUNT_0_ts.pix_count
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_INT_COUNT_0_pix_count(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_INT_COUNT_0.ISP_SMIARX_V1_INT_COUNT_0_ts.pix_count = x)


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_INT_COUNT_0_line_count() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_INT_COUNT_0.ISP_SMIARX_V1_INT_COUNT_0_ts.line_count
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_INT_COUNT_0_line_count(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_INT_COUNT_0.ISP_SMIARX_V1_INT_COUNT_0_ts.line_count = x)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_INT_COUNT_0(pix_count,line_count) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_INT_COUNT_0.word = (uint32_t)pix_count<<0 | (uint32_t)line_count<<16)
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_INT_COUNT_0() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_INT_COUNT_0.word
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_INT_COUNT_0_word(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_INT_COUNT_0.word = x)


//ISP_SMIARX_V1_INT_COUNT_1


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_INT_COUNT_1_pix_count() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_INT_COUNT_1.ISP_SMIARX_V1_INT_COUNT_1_ts.pix_count
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_INT_COUNT_1_pix_count(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_INT_COUNT_1.ISP_SMIARX_V1_INT_COUNT_1_ts.pix_count = x)


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_INT_COUNT_1_line_count() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_INT_COUNT_1.ISP_SMIARX_V1_INT_COUNT_1_ts.line_count
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_INT_COUNT_1_line_count(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_INT_COUNT_1.ISP_SMIARX_V1_INT_COUNT_1_ts.line_count = x)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_INT_COUNT_1(pix_count,line_count) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_INT_COUNT_1.word = (uint32_t)pix_count<<0 | (uint32_t)line_count<<16)
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_INT_COUNT_1() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_INT_COUNT_1.word
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_INT_COUNT_1_word(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_INT_COUNT_1.word = x)


//ISP_SMIARX_V1_INT_COUNT_2


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_INT_COUNT_2_pix_count() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_INT_COUNT_2.ISP_SMIARX_V1_INT_COUNT_2_ts.pix_count
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_INT_COUNT_2_pix_count(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_INT_COUNT_2.ISP_SMIARX_V1_INT_COUNT_2_ts.pix_count = x)


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_INT_COUNT_2_line_count() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_INT_COUNT_2.ISP_SMIARX_V1_INT_COUNT_2_ts.line_count
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_INT_COUNT_2_line_count(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_INT_COUNT_2.ISP_SMIARX_V1_INT_COUNT_2_ts.line_count = x)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_INT_COUNT_2(pix_count,line_count) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_INT_COUNT_2.word = (uint32_t)pix_count<<0 | (uint32_t)line_count<<16)
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_INT_COUNT_2() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_INT_COUNT_2.word
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_INT_COUNT_2_word(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_INT_COUNT_2.word = x)


//ISP_SMIARX_V1_INT_COUNT_3


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_INT_COUNT_3_pix_count() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_INT_COUNT_3.ISP_SMIARX_V1_INT_COUNT_3_ts.pix_count
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_INT_COUNT_3_pix_count(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_INT_COUNT_3.ISP_SMIARX_V1_INT_COUNT_3_ts.pix_count = x)


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_INT_COUNT_3_line_count() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_INT_COUNT_3.ISP_SMIARX_V1_INT_COUNT_3_ts.line_count
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_INT_COUNT_3_line_count(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_INT_COUNT_3.ISP_SMIARX_V1_INT_COUNT_3_ts.line_count = x)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_INT_COUNT_3(pix_count,line_count) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_INT_COUNT_3.word = (uint32_t)pix_count<<0 | (uint32_t)line_count<<16)
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_INT_COUNT_3() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_INT_COUNT_3.word
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_INT_COUNT_3_word(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_INT_COUNT_3.word = x)


//ISP_SMIARX_V1_DFV


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_DFV_dfv_enable() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DFV.ISP_SMIARX_V1_DFV_ts.dfv_enable
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DFV_dfv_enable(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DFV.ISP_SMIARX_V1_DFV_ts.dfv_enable = x)


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_DFV_dfv_count_0_interrupt_ctrl() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DFV.ISP_SMIARX_V1_DFV_ts.dfv_count_0_interrupt_ctrl
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DFV_dfv_count_0_interrupt_ctrl(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DFV.ISP_SMIARX_V1_DFV_ts.dfv_count_0_interrupt_ctrl = x)


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_DFV_dfv_count_1_interrupt_ctrl() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DFV.ISP_SMIARX_V1_DFV_ts.dfv_count_1_interrupt_ctrl
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DFV_dfv_count_1_interrupt_ctrl(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DFV.ISP_SMIARX_V1_DFV_ts.dfv_count_1_interrupt_ctrl = x)


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_DFV_dfv_count_2_interrupt_ctrl() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DFV.ISP_SMIARX_V1_DFV_ts.dfv_count_2_interrupt_ctrl
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DFV_dfv_count_2_interrupt_ctrl(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DFV.ISP_SMIARX_V1_DFV_ts.dfv_count_2_interrupt_ctrl = x)


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_DFV_dfv_count_3_interrupt_ctrl() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DFV.ISP_SMIARX_V1_DFV_ts.dfv_count_3_interrupt_ctrl
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DFV_dfv_count_3_interrupt_ctrl(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DFV.ISP_SMIARX_V1_DFV_ts.dfv_count_3_interrupt_ctrl = x)


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_DFV_dfv_seq_complete_interrupt_ctrl() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DFV.ISP_SMIARX_V1_DFV_ts.dfv_seq_complete_interrupt_ctrl
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DFV_dfv_seq_complete_interrupt_ctrl(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DFV.ISP_SMIARX_V1_DFV_ts.dfv_seq_complete_interrupt_ctrl = x)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DFV(dfv_enable,dfv_count_0_interrupt_ctrl,dfv_count_1_interrupt_ctrl,dfv_count_2_interrupt_ctrl,dfv_count_3_interrupt_ctrl,dfv_seq_complete_interrupt_ctrl) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DFV.word = (uint32_t)dfv_enable<<0 | (uint32_t)dfv_count_0_interrupt_ctrl<<1 | (uint32_t)dfv_count_1_interrupt_ctrl<<2 | (uint32_t)dfv_count_2_interrupt_ctrl<<3 | (uint32_t)dfv_count_3_interrupt_ctrl<<4 | (uint32_t)dfv_seq_complete_interrupt_ctrl<<5)
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_DFV() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DFV.word
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DFV_word(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DFV.word = x)


//ISP_SMIARX_V1_COUNT_INT_EN


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_COUNT_INT_EN_rx_line_count_0_int_en() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_COUNT_INT_EN.ISP_SMIARX_V1_COUNT_INT_EN_ts.rx_line_count_0_int_en
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_COUNT_INT_EN_rx_line_count_0_int_en(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_COUNT_INT_EN.ISP_SMIARX_V1_COUNT_INT_EN_ts.rx_line_count_0_int_en = x)


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_COUNT_INT_EN_rx_line_count_1_int_en() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_COUNT_INT_EN.ISP_SMIARX_V1_COUNT_INT_EN_ts.rx_line_count_1_int_en
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_COUNT_INT_EN_rx_line_count_1_int_en(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_COUNT_INT_EN.ISP_SMIARX_V1_COUNT_INT_EN_ts.rx_line_count_1_int_en = x)


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_COUNT_INT_EN_rx_line_count_2_int_en() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_COUNT_INT_EN.ISP_SMIARX_V1_COUNT_INT_EN_ts.rx_line_count_2_int_en
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_COUNT_INT_EN_rx_line_count_2_int_en(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_COUNT_INT_EN.ISP_SMIARX_V1_COUNT_INT_EN_ts.rx_line_count_2_int_en = x)


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_COUNT_INT_EN_rx_line_count_3_int_en() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_COUNT_INT_EN.ISP_SMIARX_V1_COUNT_INT_EN_ts.rx_line_count_3_int_en
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_COUNT_INT_EN_rx_line_count_3_int_en(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_COUNT_INT_EN.ISP_SMIARX_V1_COUNT_INT_EN_ts.rx_line_count_3_int_en = x)


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_COUNT_INT_EN_rx_seq_complete_int_en() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_COUNT_INT_EN.ISP_SMIARX_V1_COUNT_INT_EN_ts.rx_seq_complete_int_en
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_COUNT_INT_EN_rx_seq_complete_int_en(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_COUNT_INT_EN.ISP_SMIARX_V1_COUNT_INT_EN_ts.rx_seq_complete_int_en = x)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_COUNT_INT_EN(rx_line_count_0_int_en,rx_line_count_1_int_en,rx_line_count_2_int_en,rx_line_count_3_int_en,rx_seq_complete_int_en) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_COUNT_INT_EN.word = (uint32_t)rx_line_count_0_int_en<<0 | (uint32_t)rx_line_count_1_int_en<<1 | (uint32_t)rx_line_count_2_int_en<<2 | (uint32_t)rx_line_count_3_int_en<<3 | (uint32_t)rx_seq_complete_int_en<<4)
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_COUNT_INT_EN() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_COUNT_INT_EN.word
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_COUNT_INT_EN_word(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_COUNT_INT_EN.word = x)


//ISP_SMIARX_V1_OVERRIDE


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_OVERRIDE_override() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_OVERRIDE.ISP_SMIARX_V1_OVERRIDE_ts.override
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_OVERRIDE_override(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_OVERRIDE.ISP_SMIARX_V1_OVERRIDE_ts.override = x)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_OVERRIDE(override) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_OVERRIDE.word = (uint32_t)override<<0)
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_OVERRIDE() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_OVERRIDE.word
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_OVERRIDE_word(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_OVERRIDE.word = x)


//ISP_SMIARX_V1_SENSOR_ID


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_SENSOR_ID_sensor_id() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SENSOR_ID.ISP_SMIARX_V1_SENSOR_ID_ts.sensor_id
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_SENSOR_ID() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_SENSOR_ID.word


//ISP_SMIARX_V1_PIXEL_ORDER


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_PIXEL_ORDER_pixel_order() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_PIXEL_ORDER.ISP_SMIARX_V1_PIXEL_ORDER_ts.pixel_order
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_PIXEL_ORDER_pixel_order_B_0x0() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_PIXEL_ORDER.ISP_SMIARX_V1_PIXEL_ORDER_ts.pixel_order == pixel_order_B_0x0)
#define pixel_order_B_0x0 0x0    //GR
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_PIXEL_ORDER_pixel_order_B_0x1() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_PIXEL_ORDER.ISP_SMIARX_V1_PIXEL_ORDER_ts.pixel_order == pixel_order_B_0x1)
#define pixel_order_B_0x1 0x1    //RG
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_PIXEL_ORDER_pixel_order_B_0x2() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_PIXEL_ORDER.ISP_SMIARX_V1_PIXEL_ORDER_ts.pixel_order == pixel_order_B_0x2)
#define pixel_order_B_0x2 0x2    //BG
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_PIXEL_ORDER_pixel_order_B_0x3() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_PIXEL_ORDER.ISP_SMIARX_V1_PIXEL_ORDER_ts.pixel_order == pixel_order_B_0x3)
#define pixel_order_B_0x3 0x3    //GB


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_PIXEL_ORDER_pixel_order_ovr() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_PIXEL_ORDER.ISP_SMIARX_V1_PIXEL_ORDER_ts.pixel_order_ovr
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_PIXEL_ORDER_pixel_order_ovr_B_0x0() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_PIXEL_ORDER.ISP_SMIARX_V1_PIXEL_ORDER_ts.pixel_order_ovr == pixel_order_ovr_B_0x0)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_PIXEL_ORDER_pixel_order_ovr__B_0x0() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_PIXEL_ORDER.ISP_SMIARX_V1_PIXEL_ORDER_ts.pixel_order_ovr = pixel_order_ovr_B_0x0)
#define pixel_order_ovr_B_0x0 0x0    //GR
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_PIXEL_ORDER_pixel_order_ovr_B_0x1() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_PIXEL_ORDER.ISP_SMIARX_V1_PIXEL_ORDER_ts.pixel_order_ovr == pixel_order_ovr_B_0x1)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_PIXEL_ORDER_pixel_order_ovr__B_0x1() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_PIXEL_ORDER.ISP_SMIARX_V1_PIXEL_ORDER_ts.pixel_order_ovr = pixel_order_ovr_B_0x1)
#define pixel_order_ovr_B_0x1 0x1    //RG
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_PIXEL_ORDER_pixel_order_ovr_B_0x2() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_PIXEL_ORDER.ISP_SMIARX_V1_PIXEL_ORDER_ts.pixel_order_ovr == pixel_order_ovr_B_0x2)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_PIXEL_ORDER_pixel_order_ovr__B_0x2() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_PIXEL_ORDER.ISP_SMIARX_V1_PIXEL_ORDER_ts.pixel_order_ovr = pixel_order_ovr_B_0x2)
#define pixel_order_ovr_B_0x2 0x2    //BG
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_PIXEL_ORDER_pixel_order_ovr_B_0x3() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_PIXEL_ORDER.ISP_SMIARX_V1_PIXEL_ORDER_ts.pixel_order_ovr == pixel_order_ovr_B_0x3)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_PIXEL_ORDER_pixel_order_ovr__B_0x3() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_PIXEL_ORDER.ISP_SMIARX_V1_PIXEL_ORDER_ts.pixel_order_ovr = pixel_order_ovr_B_0x3)
#define pixel_order_ovr_B_0x3 0x3    //GB
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_PIXEL_ORDER(pixel_order_ovr) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_PIXEL_ORDER.word = (uint32_t)pixel_order_ovr<<8)
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_PIXEL_ORDER() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_PIXEL_ORDER.word
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_PIXEL_ORDER_word(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_PIXEL_ORDER.word = x)


//ISP_SMIARX_V1_AUTO_HENV_LENGTH


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_AUTO_HENV_LENGTH_auto_henv_length() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_AUTO_HENV_LENGTH.ISP_SMIARX_V1_AUTO_HENV_LENGTH_ts.auto_henv_length
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_AUTO_HENV_LENGTH_auto_henv_length(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_AUTO_HENV_LENGTH.ISP_SMIARX_V1_AUTO_HENV_LENGTH_ts.auto_henv_length = x)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_AUTO_HENV_LENGTH(auto_henv_length) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_AUTO_HENV_LENGTH.word = (uint32_t)auto_henv_length<<0)
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_AUTO_HENV_LENGTH() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_AUTO_HENV_LENGTH.word
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_AUTO_HENV_LENGTH_word(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_AUTO_HENV_LENGTH.word = x)


//ISP_SMIARX_V1_FRAME_LENGTH


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_FRAME_LENGTH_frame_length() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_FRAME_LENGTH.ISP_SMIARX_V1_FRAME_LENGTH_ts.frame_length


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_FRAME_LENGTH_frame_length_ovr() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_FRAME_LENGTH.ISP_SMIARX_V1_FRAME_LENGTH_ts.frame_length_ovr
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_FRAME_LENGTH_frame_length_ovr(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_FRAME_LENGTH.ISP_SMIARX_V1_FRAME_LENGTH_ts.frame_length_ovr = x)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_FRAME_LENGTH(frame_length_ovr) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_FRAME_LENGTH.word = (uint32_t)frame_length_ovr<<16)
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_FRAME_LENGTH() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_FRAME_LENGTH.word
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_FRAME_LENGTH_word(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_FRAME_LENGTH.word = x)


//ISP_SMIARX_V1_LINE_LENGTH


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_LINE_LENGTH_line_length() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_LINE_LENGTH.ISP_SMIARX_V1_LINE_LENGTH_ts.line_length


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_LINE_LENGTH_line_length_ovr() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_LINE_LENGTH.ISP_SMIARX_V1_LINE_LENGTH_ts.line_length_ovr
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_LINE_LENGTH_line_length_ovr(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_LINE_LENGTH.ISP_SMIARX_V1_LINE_LENGTH_ts.line_length_ovr = x)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_LINE_LENGTH(line_length_ovr) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_LINE_LENGTH.word = (uint32_t)line_length_ovr<<16)
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_LINE_LENGTH() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_LINE_LENGTH.word
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_LINE_LENGTH_word(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_LINE_LENGTH.word = x)


//ISP_SMIARX_V1_X_ADDR_START


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_X_ADDR_START_x_addr_start() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_X_ADDR_START.ISP_SMIARX_V1_X_ADDR_START_ts.x_addr_start
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_X_ADDR_START() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_X_ADDR_START.word


//ISP_SMIARX_V1_Y_ADDR_START


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_Y_ADDR_START_y_addr_start() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_Y_ADDR_START.ISP_SMIARX_V1_Y_ADDR_START_ts.y_addr_start
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_Y_ADDR_START() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_Y_ADDR_START.word


//ISP_SMIARX_V1_X_ADDR_END


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_X_ADDR_END_x_addr_end() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_X_ADDR_END.ISP_SMIARX_V1_X_ADDR_END_ts.x_addr_end
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_X_ADDR_END() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_X_ADDR_END.word


//ISP_SMIARX_V1_Y_ADDR_END


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_Y_ADDR_END_y_addr_end() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_Y_ADDR_END.ISP_SMIARX_V1_Y_ADDR_END_ts.y_addr_end
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_Y_ADDR_END() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_Y_ADDR_END.word


//ISP_SMIARX_V1_X_OUTPUT_SIZE


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_X_OUTPUT_SIZE_x_output_size() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_X_OUTPUT_SIZE.ISP_SMIARX_V1_X_OUTPUT_SIZE_ts.x_output_size
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_X_OUTPUT_SIZE() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_X_OUTPUT_SIZE.word


//ISP_SMIARX_V1_Y_OUTPUT_SIZE


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_Y_OUTPUT_SIZE_y_output_size() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_Y_OUTPUT_SIZE.ISP_SMIARX_V1_Y_OUTPUT_SIZE_ts.y_output_size
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_Y_OUTPUT_SIZE() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_Y_OUTPUT_SIZE.word


//ISP_SMIARX_V1_ADDR_INCREMENTS


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_ADDR_INCREMENTS_x_even_inc_lsb() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_ADDR_INCREMENTS.ISP_SMIARX_V1_ADDR_INCREMENTS_ts.x_even_inc_lsb


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_ADDR_INCREMENTS_x_even_inc_lsb_ovr() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_ADDR_INCREMENTS.ISP_SMIARX_V1_ADDR_INCREMENTS_ts.x_even_inc_lsb_ovr
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_ADDR_INCREMENTS_x_even_inc_lsb_ovr(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_ADDR_INCREMENTS.ISP_SMIARX_V1_ADDR_INCREMENTS_ts.x_even_inc_lsb_ovr = x)


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_ADDR_INCREMENTS_x_odd_inc_lsb() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_ADDR_INCREMENTS.ISP_SMIARX_V1_ADDR_INCREMENTS_ts.x_odd_inc_lsb


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_ADDR_INCREMENTS_x_odd_inc_lsb_ovr() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_ADDR_INCREMENTS.ISP_SMIARX_V1_ADDR_INCREMENTS_ts.x_odd_inc_lsb_ovr
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_ADDR_INCREMENTS_x_odd_inc_lsb_ovr(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_ADDR_INCREMENTS.ISP_SMIARX_V1_ADDR_INCREMENTS_ts.x_odd_inc_lsb_ovr = x)


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_ADDR_INCREMENTS_y_even_inc_lsb() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_ADDR_INCREMENTS.ISP_SMIARX_V1_ADDR_INCREMENTS_ts.y_even_inc_lsb


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_ADDR_INCREMENTS_y_even_inc_lsb_ovr() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_ADDR_INCREMENTS.ISP_SMIARX_V1_ADDR_INCREMENTS_ts.y_even_inc_lsb_ovr
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_ADDR_INCREMENTS_y_even_inc_lsb_ovr(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_ADDR_INCREMENTS.ISP_SMIARX_V1_ADDR_INCREMENTS_ts.y_even_inc_lsb_ovr = x)


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_ADDR_INCREMENTS_y_odd_inc_lsb() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_ADDR_INCREMENTS.ISP_SMIARX_V1_ADDR_INCREMENTS_ts.y_odd_inc_lsb


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_ADDR_INCREMENTS_y_odd_inc_lsb_ovr() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_ADDR_INCREMENTS.ISP_SMIARX_V1_ADDR_INCREMENTS_ts.y_odd_inc_lsb_ovr
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_ADDR_INCREMENTS_y_odd_inc_lsb_ovr(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_ADDR_INCREMENTS.ISP_SMIARX_V1_ADDR_INCREMENTS_ts.y_odd_inc_lsb_ovr = x)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_ADDR_INCREMENTS(x_even_inc_lsb_ovr,x_odd_inc_lsb_ovr,y_even_inc_lsb_ovr,y_odd_inc_lsb_ovr) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_ADDR_INCREMENTS.word = (uint32_t)x_even_inc_lsb_ovr<<1 | (uint32_t)x_odd_inc_lsb_ovr<<3 | (uint32_t)y_even_inc_lsb_ovr<<5 | (uint32_t)y_odd_inc_lsb_ovr<<7)
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_ADDR_INCREMENTS() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_ADDR_INCREMENTS.word
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_ADDR_INCREMENTS_word(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_ADDR_INCREMENTS.word = x)


//ISP_SMIARX_V1_FINE_EXPOSURE


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_FINE_EXPOSURE_fine_exposure() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_FINE_EXPOSURE.ISP_SMIARX_V1_FINE_EXPOSURE_ts.fine_exposure
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_FINE_EXPOSURE() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_FINE_EXPOSURE.word


//ISP_SMIARX_V1_COARSE_EXPOSURE


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_COARSE_EXPOSURE_coarse_exposure() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_COARSE_EXPOSURE.ISP_SMIARX_V1_COARSE_EXPOSURE_ts.coarse_exposure
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_COARSE_EXPOSURE() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_COARSE_EXPOSURE.word


//ISP_SMIARX_V1_ANALOG_GAIN


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_ANALOG_GAIN_analog_gain() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_ANALOG_GAIN.ISP_SMIARX_V1_ANALOG_GAIN_ts.analog_gain
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_ANALOG_GAIN() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_ANALOG_GAIN.word


//ISP_SMIARX_V1_MODEL_SUBTYPE


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_MODEL_SUBTYPE_frame_format_model_subtype() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_MODEL_SUBTYPE.ISP_SMIARX_V1_MODEL_SUBTYPE_ts.frame_format_model_subtype


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_MODEL_SUBTYPE_frame_format_model_subtype_ovr() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_MODEL_SUBTYPE.ISP_SMIARX_V1_MODEL_SUBTYPE_ts.frame_format_model_subtype_ovr
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_MODEL_SUBTYPE_frame_format_model_subtype_ovr(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_MODEL_SUBTYPE.ISP_SMIARX_V1_MODEL_SUBTYPE_ts.frame_format_model_subtype_ovr = x)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_MODEL_SUBTYPE(frame_format_model_subtype_ovr) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_MODEL_SUBTYPE.word = (uint32_t)frame_format_model_subtype_ovr<<8)
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_MODEL_SUBTYPE() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_MODEL_SUBTYPE.word
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_MODEL_SUBTYPE_word(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_MODEL_SUBTYPE.word = x)


//ISP_SMIARX_V1_DESCRIPTOR_0


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_0_frame_format_descriptor() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_0.ISP_SMIARX_V1_DESCRIPTOR_0_ts.frame_format_descriptor


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_0_frame_format_descriptor_ovr() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_0.ISP_SMIARX_V1_DESCRIPTOR_0_ts.frame_format_descriptor_ovr
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_0_frame_format_descriptor_ovr(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_0.ISP_SMIARX_V1_DESCRIPTOR_0_ts.frame_format_descriptor_ovr = x)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_0(frame_format_descriptor_ovr) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_0.word = (uint32_t)frame_format_descriptor_ovr<<16)
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_0() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_0.word
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_0_word(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_0.word = x)


//ISP_SMIARX_V1_DESCRIPTOR_1


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_1_frame_format_descriptor() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_1.ISP_SMIARX_V1_DESCRIPTOR_1_ts.frame_format_descriptor


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_1_frame_format_descriptor_ovr() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_1.ISP_SMIARX_V1_DESCRIPTOR_1_ts.frame_format_descriptor_ovr
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_1_frame_format_descriptor_ovr(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_1.ISP_SMIARX_V1_DESCRIPTOR_1_ts.frame_format_descriptor_ovr = x)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_1(frame_format_descriptor_ovr) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_1.word = (uint32_t)frame_format_descriptor_ovr<<16)
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_1() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_1.word
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_1_word(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_1.word = x)


//ISP_SMIARX_V1_DESCRIPTOR_2


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_2_frame_format_descriptor() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_2.ISP_SMIARX_V1_DESCRIPTOR_2_ts.frame_format_descriptor


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_2_frame_format_descriptor_ovr() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_2.ISP_SMIARX_V1_DESCRIPTOR_2_ts.frame_format_descriptor_ovr
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_2_frame_format_descriptor_ovr(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_2.ISP_SMIARX_V1_DESCRIPTOR_2_ts.frame_format_descriptor_ovr = x)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_2(frame_format_descriptor_ovr) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_2.word = (uint32_t)frame_format_descriptor_ovr<<16)
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_2() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_2.word
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_2_word(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_2.word = x)


//ISP_SMIARX_V1_DESCRIPTOR_3


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_3_frame_format_descriptor() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_3.ISP_SMIARX_V1_DESCRIPTOR_3_ts.frame_format_descriptor


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_3_frame_format_descriptor_ovr() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_3.ISP_SMIARX_V1_DESCRIPTOR_3_ts.frame_format_descriptor_ovr
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_3_frame_format_descriptor_ovr(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_3.ISP_SMIARX_V1_DESCRIPTOR_3_ts.frame_format_descriptor_ovr = x)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_3(frame_format_descriptor_ovr) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_3.word = (uint32_t)frame_format_descriptor_ovr<<16)
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_3() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_3.word
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_3_word(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_3.word = x)


//ISP_SMIARX_V1_DESCRIPTOR_4


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_4_frame_format_descriptor() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_4.ISP_SMIARX_V1_DESCRIPTOR_4_ts.frame_format_descriptor


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_4_frame_format_descriptor_ovr() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_4.ISP_SMIARX_V1_DESCRIPTOR_4_ts.frame_format_descriptor_ovr
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_4_frame_format_descriptor_ovr(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_4.ISP_SMIARX_V1_DESCRIPTOR_4_ts.frame_format_descriptor_ovr = x)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_4(frame_format_descriptor_ovr) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_4.word = (uint32_t)frame_format_descriptor_ovr<<16)
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_4() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_4.word
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_4_word(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_4.word = x)


//ISP_SMIARX_V1_DESCRIPTOR_5


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_5_frame_format_descriptor() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_5.ISP_SMIARX_V1_DESCRIPTOR_5_ts.frame_format_descriptor


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_5_frame_format_descriptor_ovr() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_5.ISP_SMIARX_V1_DESCRIPTOR_5_ts.frame_format_descriptor_ovr
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_5_frame_format_descriptor_ovr(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_5.ISP_SMIARX_V1_DESCRIPTOR_5_ts.frame_format_descriptor_ovr = x)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_5(frame_format_descriptor_ovr) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_5.word = (uint32_t)frame_format_descriptor_ovr<<16)
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_5() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_5.word
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_5_word(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_5.word = x)


//ISP_SMIARX_V1_DESCRIPTOR_6


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_6_frame_format_descriptor() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_6.ISP_SMIARX_V1_DESCRIPTOR_6_ts.frame_format_descriptor


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_6_frame_format_descriptor_ovr() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_6.ISP_SMIARX_V1_DESCRIPTOR_6_ts.frame_format_descriptor_ovr
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_6_frame_format_descriptor_ovr(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_6.ISP_SMIARX_V1_DESCRIPTOR_6_ts.frame_format_descriptor_ovr = x)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_6(frame_format_descriptor_ovr) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_6.word = (uint32_t)frame_format_descriptor_ovr<<16)
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_6() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_6.word
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_6_word(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_6.word = x)


//ISP_SMIARX_V1_DESCRIPTOR_7


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_7_frame_format_descriptor() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_7.ISP_SMIARX_V1_DESCRIPTOR_7_ts.frame_format_descriptor


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_7_frame_format_descriptor_ovr() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_7.ISP_SMIARX_V1_DESCRIPTOR_7_ts.frame_format_descriptor_ovr
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_7_frame_format_descriptor_ovr(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_7.ISP_SMIARX_V1_DESCRIPTOR_7_ts.frame_format_descriptor_ovr = x)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_7(frame_format_descriptor_ovr) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_7.word = (uint32_t)frame_format_descriptor_ovr<<16)
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_7() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_7.word
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_7_word(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_7.word = x)


//ISP_SMIARX_V1_DESCRIPTOR_8


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_8_frame_format_descriptor() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_8.ISP_SMIARX_V1_DESCRIPTOR_8_ts.frame_format_descriptor


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_8_frame_format_descriptor_ovr() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_8.ISP_SMIARX_V1_DESCRIPTOR_8_ts.frame_format_descriptor_ovr
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_8_frame_format_descriptor_ovr(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_8.ISP_SMIARX_V1_DESCRIPTOR_8_ts.frame_format_descriptor_ovr = x)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_8(frame_format_descriptor_ovr) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_8.word = (uint32_t)frame_format_descriptor_ovr<<16)
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_8() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_8.word
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_8_word(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_8.word = x)


//ISP_SMIARX_V1_DESCRIPTOR_9


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_9_frame_format_descriptor() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_9.ISP_SMIARX_V1_DESCRIPTOR_9_ts.frame_format_descriptor


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_9_frame_format_descriptor_ovr() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_9.ISP_SMIARX_V1_DESCRIPTOR_9_ts.frame_format_descriptor_ovr
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_9_frame_format_descriptor_ovr(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_9.ISP_SMIARX_V1_DESCRIPTOR_9_ts.frame_format_descriptor_ovr = x)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_9(frame_format_descriptor_ovr) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_9.word = (uint32_t)frame_format_descriptor_ovr<<16)
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_9() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_9.word
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_9_word(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_9.word = x)


//ISP_SMIARX_V1_DESCRIPTOR_10


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_10_frame_format_descriptor() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_10.ISP_SMIARX_V1_DESCRIPTOR_10_ts.frame_format_descriptor


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_10_frame_format_descriptor_ovr() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_10.ISP_SMIARX_V1_DESCRIPTOR_10_ts.frame_format_descriptor_ovr
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_10_frame_format_descriptor_ovr(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_10.ISP_SMIARX_V1_DESCRIPTOR_10_ts.frame_format_descriptor_ovr = x)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_10(frame_format_descriptor_ovr) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_10.word = (uint32_t)frame_format_descriptor_ovr<<16)
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_10() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_10.word
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_10_word(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_10.word = x)


//ISP_SMIARX_V1_DESCRIPTOR_11


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_11_frame_format_descriptor() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_11.ISP_SMIARX_V1_DESCRIPTOR_11_ts.frame_format_descriptor


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_11_frame_format_descriptor_ovr() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_11.ISP_SMIARX_V1_DESCRIPTOR_11_ts.frame_format_descriptor_ovr
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_11_frame_format_descriptor_ovr(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_11.ISP_SMIARX_V1_DESCRIPTOR_11_ts.frame_format_descriptor_ovr = x)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_11(frame_format_descriptor_ovr) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_11.word = (uint32_t)frame_format_descriptor_ovr<<16)
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_11() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_11.word
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_11_word(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_11.word = x)


//ISP_SMIARX_V1_DESCRIPTOR_12


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_12_frame_format_descriptor() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_12.ISP_SMIARX_V1_DESCRIPTOR_12_ts.frame_format_descriptor


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_12_frame_format_descriptor_ovr() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_12.ISP_SMIARX_V1_DESCRIPTOR_12_ts.frame_format_descriptor_ovr
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_12_frame_format_descriptor_ovr(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_12.ISP_SMIARX_V1_DESCRIPTOR_12_ts.frame_format_descriptor_ovr = x)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_12(frame_format_descriptor_ovr) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_12.word = (uint32_t)frame_format_descriptor_ovr<<16)
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_12() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_12.word
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_12_word(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_12.word = x)


//ISP_SMIARX_V1_DESCRIPTOR_13


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_13_frame_format_descriptor() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_13.ISP_SMIARX_V1_DESCRIPTOR_13_ts.frame_format_descriptor


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_13_frame_format_descriptor_ovr() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_13.ISP_SMIARX_V1_DESCRIPTOR_13_ts.frame_format_descriptor_ovr
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_13_frame_format_descriptor_ovr(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_13.ISP_SMIARX_V1_DESCRIPTOR_13_ts.frame_format_descriptor_ovr = x)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_13(frame_format_descriptor_ovr) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_13.word = (uint32_t)frame_format_descriptor_ovr<<16)
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_13() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_13.word
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_13_word(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_13.word = x)


//ISP_SMIARX_V1_DESCRIPTOR_14


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_14_frame_format_descriptor() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_14.ISP_SMIARX_V1_DESCRIPTOR_14_ts.frame_format_descriptor


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_14_frame_format_descriptor_ovr() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_14.ISP_SMIARX_V1_DESCRIPTOR_14_ts.frame_format_descriptor_ovr
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_14_frame_format_descriptor_ovr(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_14.ISP_SMIARX_V1_DESCRIPTOR_14_ts.frame_format_descriptor_ovr = x)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_14(frame_format_descriptor_ovr) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_14.word = (uint32_t)frame_format_descriptor_ovr<<16)
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_14() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_14.word
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_14_word(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_DESCRIPTOR_14.word = x)


//ISP_SMIARX_V1_TYPE_TRANS_0


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_black() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_black
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_black_BLANK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_black == type_trans_black_BLANK)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_black__BLANK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_black = type_trans_black_BLANK)
#define type_trans_black_BLANK 0x0    //BLACK lines will be converted to BLANK lines
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_black_BLACK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_black == type_trans_black_BLACK)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_black__BLACK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_black = type_trans_black_BLACK)
#define type_trans_black_BLACK 0x1    //BLACK lines will be converted to BLACK lines
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_black_DARK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_black == type_trans_black_DARK)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_black__DARK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_black = type_trans_black_DARK)
#define type_trans_black_DARK 0x2    //BLACK lines will be converted to DARK lines
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_black_ACTIVE() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_black == type_trans_black_ACTIVE)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_black__ACTIVE() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_black = type_trans_black_ACTIVE)
#define type_trans_black_ACTIVE 0x3    //BLACK lines will be converted to ACTIVE lines


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_dark() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_dark
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_dark_BLANK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_dark == type_trans_dark_BLANK)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_dark__BLANK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_dark = type_trans_dark_BLANK)
#define type_trans_dark_BLANK 0x0    //DARK lines will be converted to BLANK lines
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_dark_BLACK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_dark == type_trans_dark_BLACK)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_dark__BLACK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_dark = type_trans_dark_BLACK)
#define type_trans_dark_BLACK 0x1    //DARK lines will be converted to BLACK lines
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_dark_DARK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_dark == type_trans_dark_DARK)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_dark__DARK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_dark = type_trans_dark_DARK)
#define type_trans_dark_DARK 0x2    //DARK lines will be converted to DARK lines
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_dark_ACTIVE() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_dark == type_trans_dark_ACTIVE)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_dark__ACTIVE() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_dark = type_trans_dark_ACTIVE)
#define type_trans_dark_ACTIVE 0x3    //DARK lines will be converted to ACTIVE lines


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_visible() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_visible
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_visible_BLANK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_visible == type_trans_visible_BLANK)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_visible__BLANK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_visible = type_trans_visible_BLANK)
#define type_trans_visible_BLANK 0x0    //VISIBLE lines will be converted to BLANK lines
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_visible_BLACK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_visible == type_trans_visible_BLACK)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_visible__BLACK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_visible = type_trans_visible_BLACK)
#define type_trans_visible_BLACK 0x1    //VISIBLE lines will be converted to BLACK lines
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_visible_DARK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_visible == type_trans_visible_DARK)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_visible__DARK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_visible = type_trans_visible_DARK)
#define type_trans_visible_DARK 0x2    //VISIBLE lines will be converted to DARK lines
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_visible_ACTIVE() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_visible == type_trans_visible_ACTIVE)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_visible__ACTIVE() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_visible = type_trans_visible_ACTIVE)
#define type_trans_visible_ACTIVE 0x3    //VISIBLE lines will be converted to ACTIVE lines


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_dummy() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_dummy
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_dummy_BLANK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_dummy == type_trans_dummy_BLANK)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_dummy__BLANK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_dummy = type_trans_dummy_BLANK)
#define type_trans_dummy_BLANK 0x0    //DUMMY lines will be converted to BLANK lines
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_dummy_BLACK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_dummy == type_trans_dummy_BLACK)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_dummy__BLACK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_dummy = type_trans_dummy_BLACK)
#define type_trans_dummy_BLACK 0x1    //DUMMY lines will be converted to BLACK lines
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_dummy_DARK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_dummy == type_trans_dummy_DARK)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_dummy__DARK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_dummy = type_trans_dummy_DARK)
#define type_trans_dummy_DARK 0x2    //DUMMY lines will be converted to DARK lines
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_dummy_ACTIVE() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_dummy == type_trans_dummy_ACTIVE)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_dummy__ACTIVE() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_dummy = type_trans_dummy_ACTIVE)
#define type_trans_dummy_ACTIVE 0x3    //DUMMY lines will be converted to ACTIVE lines


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_mspec_0() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_mspec_0
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_mspec_0_BLANK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_mspec_0 == type_trans_mspec_0_BLANK)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_mspec_0__BLANK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_mspec_0 = type_trans_mspec_0_BLANK)
#define type_trans_mspec_0_BLANK 0x0    //MAN_SPEC_0 lines will be converted to BLANK lines
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_mspec_0_BLACK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_mspec_0 == type_trans_mspec_0_BLACK)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_mspec_0__BLACK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_mspec_0 = type_trans_mspec_0_BLACK)
#define type_trans_mspec_0_BLACK 0x1    //MAN_SPEC_0 lines will be converted to BLACK lines
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_mspec_0_DARK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_mspec_0 == type_trans_mspec_0_DARK)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_mspec_0__DARK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_mspec_0 = type_trans_mspec_0_DARK)
#define type_trans_mspec_0_DARK 0x2    //MAN_SPEC_0 lines will be converted to DARK lines
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_mspec_0_ACTIVE() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_mspec_0 == type_trans_mspec_0_ACTIVE)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_mspec_0__ACTIVE() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_mspec_0 = type_trans_mspec_0_ACTIVE)
#define type_trans_mspec_0_ACTIVE 0x3    //MAN_SPEC_0 lines will be converted to ACTIVE lines


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_mspec_1() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_mspec_1
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_mspec_1_BLANK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_mspec_1 == type_trans_mspec_1_BLANK)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_mspec_1__BLANK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_mspec_1 = type_trans_mspec_1_BLANK)
#define type_trans_mspec_1_BLANK 0x0    //MAN_SPEC_1 lines will be converted to BLANK lines
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_mspec_1_BLACK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_mspec_1 == type_trans_mspec_1_BLACK)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_mspec_1__BLACK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_mspec_1 = type_trans_mspec_1_BLACK)
#define type_trans_mspec_1_BLACK 0x1    //MAN_SPEC_1 lines will be converted to BLACK lines
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_mspec_1_DARK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_mspec_1 == type_trans_mspec_1_DARK)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_mspec_1__DARK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_mspec_1 = type_trans_mspec_1_DARK)
#define type_trans_mspec_1_DARK 0x2    //MAN_SPEC_1 lines will be converted to DARK lines
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_mspec_1_ACTIVE() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_mspec_1 == type_trans_mspec_1_ACTIVE)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_mspec_1__ACTIVE() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_mspec_1 = type_trans_mspec_1_ACTIVE)
#define type_trans_mspec_1_ACTIVE 0x3    //MAN_SPEC_1 lines will be converted to ACTIVE lines


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_mspec_2() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_mspec_2
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_mspec_2_BLANK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_mspec_2 == type_trans_mspec_2_BLANK)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_mspec_2__BLANK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_mspec_2 = type_trans_mspec_2_BLANK)
#define type_trans_mspec_2_BLANK 0x0    //MAN_SPEC_2 lines will be converted to BLANK lines
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_mspec_2_BLACK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_mspec_2 == type_trans_mspec_2_BLACK)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_mspec_2__BLACK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_mspec_2 = type_trans_mspec_2_BLACK)
#define type_trans_mspec_2_BLACK 0x1    //MAN_SPEC_2 lines will be converted to BLACK lines
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_mspec_2_DARK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_mspec_2 == type_trans_mspec_2_DARK)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_mspec_2__DARK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_mspec_2 = type_trans_mspec_2_DARK)
#define type_trans_mspec_2_DARK 0x2    //MAN_SPEC_2 lines will be converted to DARK lines
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_mspec_2_ACTIVE() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_mspec_2 == type_trans_mspec_2_ACTIVE)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_mspec_2__ACTIVE() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_mspec_2 = type_trans_mspec_2_ACTIVE)
#define type_trans_mspec_2_ACTIVE 0x3    //MAN_SPEC_2 lines will be converted to ACTIVE lines


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_mspec_3() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_mspec_3
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_mspec_3_BLANK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_mspec_3 == type_trans_mspec_3_BLANK)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_mspec_3__BLANK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_mspec_3 = type_trans_mspec_3_BLANK)
#define type_trans_mspec_3_BLANK 0x0    //MAN_SPEC_3 lines will be converted to BLANK lines
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_mspec_3_BLACK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_mspec_3 == type_trans_mspec_3_BLACK)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_mspec_3__BLACK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_mspec_3 = type_trans_mspec_3_BLACK)
#define type_trans_mspec_3_BLACK 0x1    //MAN_SPEC_3 lines will be converted to BLACK lines
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_mspec_3_DARK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_mspec_3 == type_trans_mspec_3_DARK)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_mspec_3__DARK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_mspec_3 = type_trans_mspec_3_DARK)
#define type_trans_mspec_3_DARK 0x2    //MAN_SPEC_3 lines will be converted to DARK lines
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_mspec_3_ACTIVE() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_mspec_3 == type_trans_mspec_3_ACTIVE)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_type_trans_mspec_3__ACTIVE() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.ISP_SMIARX_V1_TYPE_TRANS_0_ts.type_trans_mspec_3 = type_trans_mspec_3_ACTIVE)
#define type_trans_mspec_3_ACTIVE 0x3    //MAN_SPEC_3 lines will be converted to ACTIVE lines
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0(type_trans_black,type_trans_dark,type_trans_visible,type_trans_dummy,type_trans_mspec_0,type_trans_mspec_1,type_trans_mspec_2,type_trans_mspec_3) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.word = (uint32_t)type_trans_black<<0 | (uint32_t)type_trans_dark<<4 | (uint32_t)type_trans_visible<<8 | (uint32_t)type_trans_dummy<<12 | (uint32_t)type_trans_mspec_0<<16 | (uint32_t)type_trans_mspec_1<<20 | (uint32_t)type_trans_mspec_2<<24 | (uint32_t)type_trans_mspec_3<<28)
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.word
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_0_word(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_0.word = x)


//ISP_SMIARX_V1_TYPE_TRANS_1


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_1_type_trans_mspec_4() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_1.ISP_SMIARX_V1_TYPE_TRANS_1_ts.type_trans_mspec_4
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_1_type_trans_mspec_4_BLANK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_1.ISP_SMIARX_V1_TYPE_TRANS_1_ts.type_trans_mspec_4 == type_trans_mspec_4_BLANK)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_1_type_trans_mspec_4__BLANK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_1.ISP_SMIARX_V1_TYPE_TRANS_1_ts.type_trans_mspec_4 = type_trans_mspec_4_BLANK)
#define type_trans_mspec_4_BLANK 0x0    //MAN_SPEC_4 lines will be converted to BLANK lines
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_1_type_trans_mspec_4_BLACK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_1.ISP_SMIARX_V1_TYPE_TRANS_1_ts.type_trans_mspec_4 == type_trans_mspec_4_BLACK)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_1_type_trans_mspec_4__BLACK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_1.ISP_SMIARX_V1_TYPE_TRANS_1_ts.type_trans_mspec_4 = type_trans_mspec_4_BLACK)
#define type_trans_mspec_4_BLACK 0x1    //MAN_SPEC_4 lines will be converted to BLACK lines
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_1_type_trans_mspec_4_DARK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_1.ISP_SMIARX_V1_TYPE_TRANS_1_ts.type_trans_mspec_4 == type_trans_mspec_4_DARK)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_1_type_trans_mspec_4__DARK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_1.ISP_SMIARX_V1_TYPE_TRANS_1_ts.type_trans_mspec_4 = type_trans_mspec_4_DARK)
#define type_trans_mspec_4_DARK 0x2    //MAN_SPEC_4 lines will be converted to DARK lines
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_1_type_trans_mspec_4_ACTIVE() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_1.ISP_SMIARX_V1_TYPE_TRANS_1_ts.type_trans_mspec_4 == type_trans_mspec_4_ACTIVE)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_1_type_trans_mspec_4__ACTIVE() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_1.ISP_SMIARX_V1_TYPE_TRANS_1_ts.type_trans_mspec_4 = type_trans_mspec_4_ACTIVE)
#define type_trans_mspec_4_ACTIVE 0x3    //MAN_SPEC_4 lines will be converted to ACTIVE lines


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_1_type_trans_mspec_5() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_1.ISP_SMIARX_V1_TYPE_TRANS_1_ts.type_trans_mspec_5
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_1_type_trans_mspec_5_BLANK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_1.ISP_SMIARX_V1_TYPE_TRANS_1_ts.type_trans_mspec_5 == type_trans_mspec_5_BLANK)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_1_type_trans_mspec_5__BLANK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_1.ISP_SMIARX_V1_TYPE_TRANS_1_ts.type_trans_mspec_5 = type_trans_mspec_5_BLANK)
#define type_trans_mspec_5_BLANK 0x0    //MAN_SPEC_5 lines will be converted to BLANK lines
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_1_type_trans_mspec_5_BLACK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_1.ISP_SMIARX_V1_TYPE_TRANS_1_ts.type_trans_mspec_5 == type_trans_mspec_5_BLACK)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_1_type_trans_mspec_5__BLACK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_1.ISP_SMIARX_V1_TYPE_TRANS_1_ts.type_trans_mspec_5 = type_trans_mspec_5_BLACK)
#define type_trans_mspec_5_BLACK 0x1    //MAN_SPEC_5 lines will be converted to BLACK lines
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_1_type_trans_mspec_5_DARK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_1.ISP_SMIARX_V1_TYPE_TRANS_1_ts.type_trans_mspec_5 == type_trans_mspec_5_DARK)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_1_type_trans_mspec_5__DARK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_1.ISP_SMIARX_V1_TYPE_TRANS_1_ts.type_trans_mspec_5 = type_trans_mspec_5_DARK)
#define type_trans_mspec_5_DARK 0x2    //MAN_SPEC_5 lines will be converted to DARK lines
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_1_type_trans_mspec_5_ACTIVE() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_1.ISP_SMIARX_V1_TYPE_TRANS_1_ts.type_trans_mspec_5 == type_trans_mspec_5_ACTIVE)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_1_type_trans_mspec_5__ACTIVE() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_1.ISP_SMIARX_V1_TYPE_TRANS_1_ts.type_trans_mspec_5 = type_trans_mspec_5_ACTIVE)
#define type_trans_mspec_5_ACTIVE 0x3    //MAN_SPEC_5 lines will be converted to ACTIVE lines


#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_1_type_trans_mspec_6() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_1.ISP_SMIARX_V1_TYPE_TRANS_1_ts.type_trans_mspec_6
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_1_type_trans_mspec_6_BLANK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_1.ISP_SMIARX_V1_TYPE_TRANS_1_ts.type_trans_mspec_6 == type_trans_mspec_6_BLANK)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_1_type_trans_mspec_6__BLANK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_1.ISP_SMIARX_V1_TYPE_TRANS_1_ts.type_trans_mspec_6 = type_trans_mspec_6_BLANK)
#define type_trans_mspec_6_BLANK 0x0    //MAN_SPEC_6 lines will be converted to BLANK lines
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_1_type_trans_mspec_6_BLACK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_1.ISP_SMIARX_V1_TYPE_TRANS_1_ts.type_trans_mspec_6 == type_trans_mspec_6_BLACK)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_1_type_trans_mspec_6__BLACK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_1.ISP_SMIARX_V1_TYPE_TRANS_1_ts.type_trans_mspec_6 = type_trans_mspec_6_BLACK)
#define type_trans_mspec_6_BLACK 0x1    //MAN_SPEC_6 lines will be converted to BLACK lines
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_1_type_trans_mspec_6_DARK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_1.ISP_SMIARX_V1_TYPE_TRANS_1_ts.type_trans_mspec_6 == type_trans_mspec_6_DARK)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_1_type_trans_mspec_6__DARK() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_1.ISP_SMIARX_V1_TYPE_TRANS_1_ts.type_trans_mspec_6 = type_trans_mspec_6_DARK)
#define type_trans_mspec_6_DARK 0x2    //MAN_SPEC_6 lines will be converted to DARK lines
#define Is_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_1_type_trans_mspec_6_ACTIVE() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_1.ISP_SMIARX_V1_TYPE_TRANS_1_ts.type_trans_mspec_6 == type_trans_mspec_6_ACTIVE)
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_1_type_trans_mspec_6__ACTIVE() (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_1.ISP_SMIARX_V1_TYPE_TRANS_1_ts.type_trans_mspec_6 = type_trans_mspec_6_ACTIVE)
#define type_trans_mspec_6_ACTIVE 0x3    //MAN_SPEC_6 lines will be converted to ACTIVE lines
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_1(type_trans_mspec_4,type_trans_mspec_5,type_trans_mspec_6) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_1.word = (uint16_t)type_trans_mspec_4<<0 | (uint16_t)type_trans_mspec_5<<4 | (uint16_t)type_trans_mspec_6<<8)
#define Get_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_1() p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_1.word
#define Set_ISP_SMIARX_V1_ISP_SMIARX_V1_TYPE_TRANS_1_word(x) (p_ISP_SMIARX_V1_IP->ISP_SMIARX_V1_TYPE_TRANS_1.word = x)

extern volatile ISP_SMIARX_V1_IP_ts *p_ISP_SMIARX_V1_IP;
#endif

