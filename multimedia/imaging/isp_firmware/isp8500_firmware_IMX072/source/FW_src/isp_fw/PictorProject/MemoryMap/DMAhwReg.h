#ifndef _DMAhwReg_
#define _DMAhwReg_

#include "Platform.h"



//DMA_ENR
/*Description: This register allows to start the DMA processes corresponding to channels. Each process can be controlled independently by writing 1 into the corresponding enable bit (writing 0 has no effect). Writing 1 enables and initializes the process, and sets the corresponding status bit immediately, if the status bit was not set. If the status bit was already set (i.e. the process was running), writing 1 has no effect. This register also allows to read the status of the DMA processes corresponding to channels. The value 0 means the process is disabled (either because it was never started or because it completed its address generation), while the value 1 means the process is enabled and can serve requests. A status bit is modified either by a write access in the corresponding bit of DMA_ENR or DMA_DIR.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t IRP03_L_W :1;
        /* enable luma write channel for pipe 0 or 3*/
        uint8_t IRP03_C_W :1;
        /* enable chroma write channel for pipe 0 or 3*/
        uint8_t IRP03_C1_W :1;
        /* enable chroma1 write channel for pipe 0 or 3*/
        uint8_t IRP03_C2_W :1;
        /* enable chroma2 write channel for pipe 0 or 3*/
        uint8_t IRP03_GCF_L_W :1;
        /* enable luma grab cache buffer write channel for pipe 0 or 3*/
        uint8_t IRP03_GCF_C_W :1;
        /* enable chroma grab cache buffer write channel for pipe 0 or 3*/
        uint8_t IRP03_GCF_L_R :1;
        /* enable luma grab cache buffer read channel for pipe 0 or 3*/
        uint8_t IRP03_GCF_C_R :1;
        /* enable chroma grab cache buffer read channel for pipe 0 or 3*/
        uint8_t IRP03_VCP_W :1;
        /* enable Vertical Curve Projection accumulation write channel for pipe 0 or 3*/
        uint8_t IRP03_HCP_W :1;
        /* enable Horizontal Curve Projection accumulation write channel for pipe 0 or 3*/
        uint8_t IRP03_HCP_R :1;
        /* enable Horizontal Curve Projection accumulation read channel for pipe 0 or 3*/
        uint8_t IRP03_PRA_W :1;
        /* enable raster write channel for pipe 0 or 3*/
        uint8_t IRP0_PRA_R :1;
        /* enable raster read channel for pipe 0*/
        uint8_t IRP1_PRA_W :1;
        /* enable raster write channel for pipe 1*/
        uint8_t IRP2_PRA_W :1;
        /* enable raster write channel for pipe 2*/
        uint8_t IRP3_JPG_W :1;
        /* enable JPEG write channel for pipe 3*/
    }DMA_ENR_ts;

}DMA_ENR_tu;



//DMA_DIR
/*Description: This register allows to stop the DMA processes corresponding to channels. Each process can be controlled independently by writing 1 into the corresponding disable bit (writing 0 has no effect). Writing 1 disables the process and resets the corresponding status bit, after all data have been generated if a request was acknowledged but not completed. If the status bit was already reset (i.e. the process was stopped), writing 1 has no effect. This register also allows to read the status of the DMA processes corresponding to channels. The value 0 means the process is disabled (either because it was never started or because it completed its address generation), while the value 1 means the process is enabled and can serve requests. A status bit is modified either by a write access in the corresponding bit of DMA_ENR or DMA_DIR.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t IRP03_L_W :1;
        /* disable luma write channel for pipe 0 or 3*/
        uint8_t IRP03_C_W :1;
        /* disable chroma write channel for pipe 0 or 3*/
        uint8_t IRP03_C1_W :1;
        /* disable chroma1 write channel for pipe 0 or 3*/
        uint8_t IRP03_C2_W :1;
        /* disable chroma2 write channel for pipe 0 or 3*/
        uint8_t IRP03_GCF_L_W :1;
        /* disable luma grab cache buffer write channel for pipe 0 or 3*/
        uint8_t IRP03_GCF_C_W :1;
        /* disable chroma grab cache buffer write channel for pipe 0 or 3*/
        uint8_t IRP03_GCF_L_R :1;
        /* disable luma grab cache buffer read channel for pipe 0 or 3*/
        uint8_t IRP03_GCF_C_R :1;
        /* disable chroma grab cache buffer read channel for pipe 0 or 3*/
        uint8_t IRP03_VCP_W :1;
        /* disable Vertical Curve Projection accumulation write channel for pipe 0 or 3*/
        uint8_t IRP03_HCP_W :1;
        /* disable Horizontal Curve Projection accumulation write channel for pipe 0 or 3*/
        uint8_t IRP03_HCP_R :1;
        /* disable Horizontal Curve Projection accumulation read channel for pipe 0 or 3*/
        uint8_t IRP03_PRA_W :1;
        /* disable raster write channel for pipe 0 or 3*/
        uint8_t IRP0_PRA_R :1;
        /* disable raster read channel for pipe 0*/
        uint8_t IRP1_PRA_W :1;
        /* disable raster write channel for pipe 1*/
        uint8_t IRP2_PRA_W :1;
        /* disable raster write channel for pipe 2*/
        uint8_t IRP3_JPG_W :1;
        /* disable JPEG write channel for pipe 3*/
    }DMA_DIR_ts;

}DMA_DIR_tu;



//DMA_PEN
/*Description: This register logs whether there is a pending DMA process for each of the channels. A pending DMA process is a process that has been started (writing '1' to one of the DMA_ENR bits) while the same channel was active (started and EOT interrupt not returned). This process is automatically started by HW whenever the previous channel completes (EOT). The value 0 means there is no pending process, while the value 1 means a process is pending and will automatically be started after currently operated process completes. Only processes implementing posted tasks may have a process pending, i.e. have the associated bit in DMA_PEN set to '1'. Processes not implementing posted task always have their associated bit in DMA_PEN cleared (0).
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t IRP03_L_W :1;
        /* disable luma write channel for pipe 0 or 3*/
        uint8_t IRP03_C_W :1;
        /* disable chroma write channel for pipe 0 or 3*/
        uint8_t IRP03_C1_W :1;
        /* disable chroma1 write channel for pipe 0 or 3*/
        uint8_t IRP03_C2_W :1;
        /* disable chroma2 write channel for pipe 0 or 3*/
        uint8_t reserved0 :7;
        uint8_t IRP03_PRA_W :1;
        /* disable raster write channel for pipe 0 or 3*/
        uint8_t reserved1 :2;
        uint8_t IRP2_PRA_W :1;
        /* raster write channel for pipe 2*/
    }DMA_PEN_ts;

}DMA_PEN_tu;



//DMA_INTERLACE_EN
/*Description: Allows the jump of 1 line in the buffer. Each channel has got 2 registers to define vertical & horizontal dimensions (height, width). When individual bit is set, the height consists in an absolute height : - number of element in the height to store or load. - frame height differs to the buffer height
*/
typedef union
{
    uint32_t word;
    uint16_t data;
    struct
    {
        uint8_t IRP03_L_W :1;
        /* luma write channel for pipe 0 or 3*/
        uint8_t IRP03_C_W :1;
        /* chroma write channel for pipe 0 or 3*/
        uint8_t IRP03_C1_W :1;
        /* chroma1 write channel for pipe 0 or 3*/
        uint8_t IRP03_C2_W :1;
        /* chroma2 write channel for pipe 0 or 3*/
        uint8_t IRP03_GCF_L_W :1;
        /* luma grab cache buffer write channel for pipe 0 or 3*/
        uint8_t IRP03_GCF_C_W :1;
        /* chroma grab cache buffer write channel for pipe 0 or 3*/
        uint8_t IRP03_GCF_L_R :1;
        /* luma grab cache buffer read channel for pipe 0 or 3*/
        uint8_t IRP03_GCF_C_R :1;
        /* chroma grab cache buffer read channel for pipe 0 or 3*/
        uint8_t reserved0 :3;
        uint8_t IRP03_PRA_W :1;
        /* raster write channel for pipe 0 or 3*/
        uint8_t IRP0_PRA_R :1;
        /* raster read channel for pipe 0*/
        uint8_t IRP1_PRA_W :1;
        /* raster read channel for pipe 1*/
        uint8_t IRP2_PRA_W :1;
        /* raster read channel for pipe 2*/
    }DMA_INTERLACE_EN_ts;

}DMA_INTERLACE_EN_tu;



//DMA_ITS1
/*Description: This register is the interrupt status register for DMA.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t IRP03_L_W :1;
        /* disable luma write channel for pipe 0 or 3*/
        uint8_t IRP03_C_W :1;
        /* disable chroma write channel for pipe 0 or 3*/
        uint8_t IRP03_C1_W :1;
        /* disable chroma1 write channel for pipe 0 or 3*/
        uint8_t IRP03_C2_W :1;
        /* disable chroma2 write channel for pipe 0 or 3*/
        uint8_t IRP03_GCF_L_W :1;
        /* disable luma grab cache buffer write channel for pipe 0 or 3*/
        uint8_t IRP03_GCF_C_W :1;
        /* disable chroma grab cache buffer write channel for pipe 0 or 3*/
        uint8_t IRP03_GCF_L_R :1;
        /* disable luma grab cache buffer read channel for pipe 0 or 3*/
        uint8_t IRP03_GCF_C_R :1;
        /* disable chroma grab cache buffer read channel for pipe 0 or 3*/
        uint8_t IRP03_VCP_W :1;
        /* disable Vertical Curve Projection accumulation write channel for pipe 0 or 3*/
        uint8_t IRP03_HCP_W :1;
        /* disable Horizontal Curve Projection accumulation write channel for pipe 0 or 3*/
        uint8_t IRP03_HCP_R :1;
        /* disable Horizontal Curve Projection accumulation read channel for pipe 0 or 3*/
        uint8_t IRP03_PRA_W :1;
        /* disable raster write channel for pipe 0 or 3*/
        uint8_t IRP0_PRA_R :1;
        /* raster read channel for pipe 0*/
        uint8_t IRP1_PRA_W :1;
        /* raster write channel for pipe 1*/
        uint8_t IRP2_PRA_W :1;
        /* raster write channel for pipe 2*/
        uint8_t IRP3_JPG_W :1;
        /* JPEG write channel for pipe 3*/
    }DMA_ITS1_ts;

}DMA_ITS1_tu;



//DMA_ITS1_BCLR
/*Description: This register is the DMA interrupt status 1 clear register. Writing '1' to any of those bits clears the associated bit in DMA_ITS1 status register. Writing '0' has no effect.
*/
typedef union
{
    uint32_t word;
    uint16_t data;
    struct
    {
        uint8_t IRP03_L_W :1;
        /* disable luma write channel for pipe 0 or 3 status clear*/
        uint8_t IRP03_C_W :1;
        /* disable chroma write channel for pipe 0 or 3 status clear*/
        uint8_t IRP03_C1_W :1;
        /* disable chroma1 write channel for pipe 0 or 3 status clear*/
        uint8_t IRP03_C2_W :1;
        /* disable chroma2 write channel for pipe 0 or 3 status clear*/
        uint8_t IRP03_GCF_L_W :1;
        /* disable luma grab cache buffer write channel for pipe 0 or 3 status clear*/
        uint8_t IRP03_GCF_C_W :1;
        /* disable chroma grab cache buffer write channel for pipe 0 or 3 status clear*/
        uint8_t IRP03_GCF_L_R :1;
        /* disable luma grab cache buffer read channel for pipe 0 or 3 status clear*/
        uint8_t IRP03_GCF_C_R :1;
        /* disable chroma grab cache buffer read channel for pipe 0 or 3 status clear*/
        uint8_t IRP03_VCP_W :1;
        /* disable Vertical Curve Projection accumulation write channel for pipe 0 or 3 status clear*/
        uint8_t IRP03_HCP_W :1;
        /* disable Horizontal Curve Projection accumulation write channel for pipe 0 or 3 status clear*/
        uint8_t IRP03_HCP_R :1;
        /* disable Horizontal Curve Projection accumulation read channel for pipe 0 or 3 status clear*/
        uint8_t IRP03_PRA_W :1;
        /* disable raster write channel for pipe 0 or 3 status clear*/
        uint8_t IRP0_PRA_R :1;
        /* raster read channel for pipe 0 status clear*/
        uint8_t IRP1_PRA_W :1;
        /* raster write channel for pipe 1 status clear*/
        uint8_t IRP2_PRA_W :1;
        /* raster write channel for pipe 2 status clear*/
        uint8_t IRP3_JPG_W :1;
        /* JPEG write channel for pipe 3 status clear*/
    }DMA_ITS1_BCLR_ts;

}DMA_ITS1_BCLR_tu;



//DMA_IMR1
/*Description: This register is the DMA interrupt 0 mask register.
*/
typedef union
{
    uint32_t word;
    uint16_t data;
    struct
    {
        uint8_t IRP03_L_W :1;
        /* disable luma write channel for pipe 0 or 3*/
        uint8_t IRP03_C_W :1;
        /* disable chroma write channel for pipe 0 or 3*/
        uint8_t IRP03_C1_W :1;
        /* disable chroma1 write channel for pipe 0 or 3*/
        uint8_t IRP03_C2_W :1;
        /* disable chroma2 write channel for pipe 0 or 3*/
        uint8_t IRP03_GCF_L_W :1;
        /* disable luma grab cache buffer write channel for pipe 0 or 3*/
        uint8_t IRP03_GCF_C_W :1;
        /* disable chroma grab cache buffer write channel for pipe 0 or 3*/
        uint8_t IRP03_GCF_L_R :1;
        /* disable luma grab cache buffer read channel for pipe 0 or 3*/
        uint8_t IRP03_GCF_C_R :1;
        /* disable chroma grab cache buffer read channel for pipe 0 or 3*/
        uint8_t IRP03_VCP_W :1;
        /* disable Vertical Curve Projection accumulation write channel for pipe 0 or 3*/
        uint8_t IRP03_HCP_W :1;
        /* disable Horizontal Curve Projection accumulation write channel for pipe 0 or 3*/
        uint8_t IRP03_HCP_R :1;
        /* disable Horizontal Curve Projection accumulation read channel for pipe 0 or 3*/
        uint8_t IRP03_PRA_W :1;
        /* disable raster write channel for pipe 0 or 3*/
        uint8_t IRP0_PRA_R :1;
        /* raster read channel for pipe 0*/
        uint8_t IRP1_PRA_W :1;
        /* raster write channel for pipe 1*/
        uint8_t IRP2_PRA_W :1;
        /* raster write channel for pipe 2*/
        uint8_t IRP3_JPG_W :1;
        /* JPEG write channel for pipe 3*/
    }DMA_IMR1_ts;

}DMA_IMR1_tu;



//DMA_ITS2
/*Description: This register is the interrupt status register for DMA.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t IRP03_L_W :1;
        /* disable luma write channel for pipe 0 or 3*/
        uint8_t IRP03_C_W :1;
        /* disable chroma write channel for pipe 0 or 3*/
        uint8_t IRP03_C1_W :1;
        /* disable chroma1 write channel for pipe 0 or 3*/
        uint8_t IRP03_C2_W :1;
        /* disable chroma2 write channel for pipe 0 or 3*/
        uint8_t IRP03_GCF_L_W :1;
        /* disable luma grab cache buffer write channel for pipe 0 or 3*/
        uint8_t IRP03_GCF_C_W :1;
        /* disable chroma grab cache buffer write channel for pipe 0 or 3*/
        uint8_t IRP03_GCF_L_R :1;
        /* disable luma grab cache buffer read channel for pipe 0 or 3*/
        uint8_t IRP03_GCF_C_R :1;
        /* disable chroma grab cache buffer read channel for pipe 0 or 3*/
        uint8_t IRP03_VCP_W :1;
        /* disable Vertical Curve Projection accumulation write channel for pipe 0 or 3*/
        uint8_t IRP03_HCP_W :1;
        /* disable Horizontal Curve Projection accumulation write channel for pipe 0 or 3*/
        uint8_t IRP03_HCP_R :1;
        /* disable Horizontal Curve Projection accumulation read channel for pipe 0 or 3*/
        uint8_t IRP03_PRA_W :1;
        /* disable raster write channel for pipe 0 or 3*/
        uint8_t IRP0_PRA_R :1;
        /* raster read channel for pipe 0*/
        uint8_t IRP1_PRA_W :1;
        /* raster write channel for pipe 1*/
        uint8_t IRP2_PRA_W :1;
        /* raster write channel for pipe 2*/
        uint8_t IRP3_JPG_W :1;
        /* JPEG write channel for pipe 3*/
    }DMA_ITS2_ts;

}DMA_ITS2_tu;



//DMA_ITS2_BCLR
/*Description: This register is the DMA interrupt status 1 clear register. Writing '1' to any of those bits clears the associated bit in DMA_ITS1 status register. Writing '0' has no effect.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t IRP03_L_W :1;
        /* disable luma write channel for pipe 0 or 3 status clear*/
        uint8_t IRP03_C_W :1;
        /* disable chroma write channel for pipe 0 or 3 status clear*/
        uint8_t IRP03_C1_W :1;
        /* disable chroma1 write channel for pipe 0 or 3 status clear*/
        uint8_t IRP03_C2_W :1;
        /* disable chroma2 write channel for pipe 0 or 3 status clear*/
        uint8_t IRP03_GCF_L_W :1;
        /* disable luma grab cache buffer write channel for pipe 0 or 3 status clear*/
        uint8_t IRP03_GCF_C_W :1;
        /* disable chroma grab cache buffer write channel for pipe 0 or 3 status clear*/
        uint8_t IRP03_GCF_L_R :1;
        /* disable luma grab cache buffer read channel for pipe 0 or 3 status clear*/
        uint8_t IRP03_GCF_C_R :1;
        /* disable chroma grab cache buffer read channel for pipe 0 or 3 status clear*/
        uint8_t IRP03_VCP_W :1;
        /* disable Vertical Curve Projection accumulation write channel for pipe 0 or 3 status clear*/
        uint8_t IRP03_HCP_W :1;
        /* disable Horizontal Curve Projection accumulation write channel for pipe 0 or 3 status clear*/
        uint8_t IRP03_HCP_R :1;
        /* disable Horizontal Curve Projection accumulation read channel for pipe 0 or 3 status clear*/
        uint8_t IRP03_PRA_W :1;
        /* disable raster write channel for pipe 0 or 3 status clear*/
        uint8_t IRP0_PRA_R :1;
        /* raster read channel for pipe 0 status clear*/
        uint8_t IRP1_PRA_W :1;
        /* raster write channel for pipe 1 status clear*/
        uint8_t IRP2_PRA_W :1;
        /* raster write channel for pipe 2 status clear*/
        uint8_t IRP3_JPG_W :1;
        /* JPEG write channel for pipe 3 status clear*/
    }DMA_ITS2_BCLR_ts;

}DMA_ITS2_BCLR_tu;



//DMA_IMR2
/*Description: This register is the DMA interrupt 1 mask register
*/
typedef union
{
    uint32_t word;
    uint16_t data;
    struct
    {
        uint8_t IRP03_L_W :1;
        /* disable luma write channel for pipe 0 or 3*/
        uint8_t IRP03_C_W :1;
        /* disable chroma write channel for pipe 0 or 3*/
        uint8_t IRP03_C1_W :1;
        /* disable chroma1 write channel for pipe 0 or 3*/
        uint8_t IRP03_C2_W :1;
        /* disable chroma2 write channel for pipe 0 or 3*/
        uint8_t IRP03_GCF_L_W :1;
        /* disable luma grab cache buffer write channel for pipe 0 or 3*/
        uint8_t IRP03_GCF_C_W :1;
        /* disable chroma grab cache buffer write channel for pipe 0 or 3*/
        uint8_t IRP03_GCF_L_R :1;
        /* disable luma grab cache buffer read channel for pipe 0 or 3*/
        uint8_t IRP03_GCF_C_R :1;
        /* disable chroma grab cache buffer read channel for pipe 0 or 3*/
        uint8_t IRP03_VCP_W :1;
        /* disable Vertical Curve Projection accumulation write channel for pipe 0 or 3*/
        uint8_t IRP03_HCP_W :1;
        /* disable Horizontal Curve Projection accumulation write channel for pipe 0 or 3*/
        uint8_t IRP03_HCP_R :1;
        /* disable Horizontal Curve Projection accumulation read channel for pipe 0 or 3*/
        uint8_t IRP03_PRA_W :1;
        /* disable raster write channel for pipe 0 or 3*/
        uint8_t IRP0_PRA_R :1;
        /* raster read channel for pipe 0*/
        uint8_t IRP1_PRA_W :1;
        /* raster write channel for pipe 1*/
        uint8_t IRP2_PRA_W :1;
        /* raster write channel for pipe 2*/
        uint8_t IRP3_JPG_W :1;
        /* JPEG write channel for pipe 3*/
    }DMA_IMR2_ts;

}DMA_IMR2_tu;



//DMA_SOFT_RST
/*Description: This register commands soft reset of the individual DMA processes. When soft reset is triggered on a particular process, the DMA stops generating requests to the STBusPlug, the STBusPlug waits for all outstanding responses (for that process) to complete, the DMA resets its address generator (for that process) and the STBusPlug resets the data Fifo (associated with the process). Note residual data in Fifo are lost. DMA_STA (DMA_ENR) and DMA_FIFO_STA are both cleared. Note that read to DMA_FIFO_STA after DMA_SOFT_RST may return 1 if the source providing data has not been properly reset prior to reset DMA process.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t IRP03_L_W :1;
        /* disable luma write channel for pipe 0 or 3 soft reset*/
        uint8_t IRP03_C_W :1;
        /* disable chroma write channel for pipe 0 or 3 soft reset*/
        uint8_t IRP03_C1_W :1;
        /* disable chroma1 write channel for pipe 0 or 3 soft reset*/
        uint8_t IRP03_C2_W :1;
        /* disable chroma2 write channel for pipe 0 or 3 soft reset*/
        uint8_t IRP03_GCF_L_W :1;
        /* disable luma grab cache buffer write channel for pipe 0 or 3 soft reset*/
        uint8_t IRP03_GCF_C_W :1;
        /* disable chroma grab cache buffer write channel for pipe 0 or 3 soft reset*/
        uint8_t IRP03_GCF_L_R :1;
        /* disable luma grab cache buffer read channel for pipe 0 or 3 soft reset*/
        uint8_t IRP03_GCF_C_R :1;
        /* disable chroma grab cache buffer read channel for pipe 0 or 3 soft reset*/
        uint8_t IRP03_VCP_W :1;
        /* disable Vertical Curve Projection accumulation write channel for pipe 0 or 3 soft reset*/
        uint8_t IRP03_HCP_W :1;
        /* disable Horizontal Curve Projection accumulation write channel for pipe 0 or 3 soft reset*/
        uint8_t IRP03_HCP_R :1;
        /* disable Horizontal Curve Projection accumulation read channel for pipe 0 or 3 soft reset*/
        uint8_t IRP03_PRA_W :1;
        /* disable raster write channel for pipe 0 or 3 soft reset*/
        uint8_t IRP0_PRA_R :1;
        /* raster read channel for pipe 0 soft reset*/
        uint8_t IRP1_PRA_W :1;
        /* raster write channel for pipe 1 soft reset*/
        uint8_t IRP2_PRA_W :1;
        /* raster write channel for pipe 2 soft reset*/
        uint8_t IRP3_JPG_W :1;
        /* JPEG write channel for pipe 3 soft reset*/
    }DMA_SOFT_RST_ts;

}DMA_SOFT_RST_tu;



//DMA_FIFO_STA
/*Description: This register is a status associated with the data Fifo associated with each process. It is independant of the adress generation (reflected in DMA_STA) and is typically used to know whether there is residual data after DMA has completed (after DMA EOT) to track for an error (for processes for which transfer size is known) or to know whether a flush is required (for processed for which transfer size is unknown - e.g. JPEG frame).
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t IRP03_L_W :1;
        /* disable luma write channel for pipe 0 or 3*/
        uint8_t IRP03_C_W :1;
        /* disable chroma write channel for pipe 0 or 3*/
        uint8_t IRP03_C1_W :1;
        /* disable chroma1 write channel for pipe 0 or 3*/
        uint8_t IRP03_C2_W :1;
        /* disable chroma2 write channel for pipe 0 or 3*/
        uint8_t IRP03_GCF_L_W :1;
        /* disable luma grab cache buffer write channel for pipe 0 or 3*/
        uint8_t IRP03_GCF_C_W :1;
        /* disable chroma grab cache buffer write channel for pipe 0 or 3*/
        uint8_t reserved0 :2;
        uint8_t IRP03_VCP_W :1;
        /* disable Vertical Curve Projection accumulation write channel for pipe 0 or 3*/
        uint8_t IRP03_HCP_W :1;
        /* disable Horizontal Curve Projection accumulation write channel for pipe 0 or 3*/
        uint8_t reserved1 :1;
        uint8_t IRP03_PRA_W :1;
        /* disable raster write channel for pipe 0 or 3*/
        uint8_t reserved2 :1;
        uint8_t IRP1_PRA_W :1;
        /* raster write channel for pipe 1*/
        uint8_t IRP2_PRA_W :1;
        /* raster write channel for pipe 2*/
        uint8_t IRP3_JPG_W :1;
        /* JPEG write channel for pipe 3*/
    }DMA_FIFO_STA_ts;

}DMA_FIFO_STA_tu;



//DMA_FIFO_FLUSH
/*Description: This register commands Fifo Flush. This is necessary for processes for which the number of bytes to transfer is not known in advance (e.g. for compressed image grab). When source has completed sending data, Flush shall be triggered to force all residual data in the FIFO to be written back to memory. When flush is started, STBusPlug stops accepting new data in the FIFO and proceeds with flushing data that is already in the FIFO.When flush is completed, Status is updated and an EOT interrupt is sent (as if the DMA had completed normally). STBusPlug re-start accepting new data in the FIFO after flush has completed. After flush has completed, DMA_FIFO_STA should typically be 0. If not, this means source has re-started generating data after flush has terminated. Data sent by source during flush shall trigger a FIFO overflow event.
*/
typedef union
{
    uint32_t word;
    uint16_t data;
    struct
    {
        uint16_t reserved0 :15;
        uint8_t IRP3_JPG_W :1;
        /* JPEG write channel for pipe 3 flush*/
    }DMA_FIFO_FLUSH_ts;

}DMA_FIFO_FLUSH_tu;



//DMA_IRP03_DLS_L
/*Description: This register defines the 16 least significant bit address (LSB) of the pipe 0/3 Destination buffer Luma Start address
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t reserved0 :3;
        uint16_t DMA_IRP03_DLS_L :13;
        /* 
        when raster =0 & inter_en =0, dls[7:0]=hx00
        when raster=0 & inter_en =1, dls[7:0]=hx08 in case of bottom field programmation
        when raster=0 & inter_en =1, dls[7:0]=hx00 in case of top field programmation
        when raster=1, dls[2:0]="000"*/
    }DMA_IRP03_DLS_L_ts;

}DMA_IRP03_DLS_L_tu;



//DMA_IRP03_DLS_H
/*Description: This register defines the 16 most significant bit address (MSB) of the pipe 0/3 Destination buffer Luma Start address
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t DMA_IRP03_DLS_H;
    }DMA_IRP03_DLS_H_ts;

}DMA_IRP03_DLS_H_tu;



//DMA_IRP03_DCS_L
/*Description: This register defines the 16 least significant bit address (LSB) of the pipe 0/3 Destination buffer Chroma Start address
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t reserved0 :3;
        uint16_t DMA_IRP03_DCS_L :13;
        /* 
        - when cfc=1
        -- when raster =0 & inter_en =0, dcs[7:0]=hx00
        --when raster=0 & inter_en =1, dcs[7:0]=hx08 in case of bot field programmation
        --when raster=0 & inter_en =1, dcs[7:0]=hx00 in case of top field programmation
        --when raster=1, dcs[2:0]="000"
        -when cfc=0
        --when raster =0 & inter_en =0, dcs[6:0]=hx00
        --when raster=0 & inter_en =1, dcs[6:0]=hx08 in case of bot field programmation
        --when raster=0 & inter_en =1, dcs[6:0]=hx00 in case of top field programmation
        --when raster=1, dcs[2:0]="000"*/
    }DMA_IRP03_DCS_L_ts;

}DMA_IRP03_DCS_L_tu;



//DMA_IRP03_DCS_H
/*Description: This register defines the 16 most significant bit address (MSB) of the pipe 0/3 Destination buffer Chroma Start address
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t DMA_IRP03_DCS_H;
    }DMA_IRP03_DCS_H_ts;

}DMA_IRP03_DCS_H_tu;



//DMA_IRP03_CLS_L
/*Description: This register defines the 16 least significant bit address (LSB) of the pipe 0/3 Cache buffer Luma Start address
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t reserved0 :3;
        uint16_t DMA_IRP03_CLS_L :13;
        /* 
        -when raster =0 & inter_en =0, cls[7:0]=hx00
        -when raster=0 & inter_en =1, cls[7:0]=hx08 in case of bottom field programmation
        -when raster=0 & inter_en =1, cls[7:0]=hx00 in case of top field programmation
        -when raster=1, grab cache is not used.*/
    }DMA_IRP03_CLS_L_ts;

}DMA_IRP03_CLS_L_tu;



//DMA_IRP03_CLS_H
/*Description: This register defines the 16 most significant bit address (MSB) of the pipe 0/3 Cache buffer Luma Start address
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t DMA_IRP03_CLS_H;
    }DMA_IRP03_CLS_H_ts;

}DMA_IRP03_CLS_H_tu;



//DMA_IRP03_CCS_L
/*Description: This register defines the 16 least significant bit address (LSB) of the pipe 0/3 Cache buffer Chroma Start address
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t reserved0 :3;
        uint16_t DMA_IRP03_CCS_L :13;
        /* 
        -when cfc=1
        --when raster =0 & inter_en =0, ccs[7:0]=hx00
        --when raster=0 & inter_en =1, ccs[7:0]=hx08 in case of bot field programmation
        --when raster=0 & inter_en =1, ccs[7:0]=hx00 in case of top field programmation
        --when raster=1, grab cache is not used.
        -when cfc=0
        --when raster =0 & inter_en =0, ccs[6:0]=hx00
        --when raster=0 & inter_en =1, ccs[6:0]=hx08 in case of bot field programmation
        --when raster=0 & inter_en =1, ccs[6:0]=hx00 in case of top field programmation
        --when raster=1, grab cache is not used.*/
    }DMA_IRP03_CCS_L_ts;

}DMA_IRP03_CCS_L_tu;



//DMA_IRP03_CCS_H
/*Description: This register defines the 16 most significant bit address (MSB) of the pipe 0/3 Cache buffer Chroma Start address
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t DMA_IRP03_CCS_H;
    }DMA_IRP03_CCS_H_ts;

}DMA_IRP03_CCS_H_tu;



//DMA_IRP03_DC1S_L
/*Description: This register defines the 16 least significant bit address (LSB) of the pipe 0/3 Destination buffer Chroma 1 Start address (planar format)
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t reserved0 :3;
        uint16_t DMA_IRP03_DC1S_L :13;
        /* 
        -when cfc=1
        --when raster =0 & inter_en =0, dcs[6:0]=hx00
        --when raster=0 & inter_en =1, dcs[6:0]=hx08 in case of bot field programmation
        --when raster=0 & inter_en =1, dcs[6:0]=hx00 in case of top field programmation
        --when raster=1, dcs[2:0]="000"
        -when cfc=0
        --when raster =0 & inter_en =0, dcs[5:0]=hx00
        --when raster=0 & inter_en =1, dcs[5:0]=hx08 in case of bot field programmation
        --when raster=0 & inter_en =1, dcs[5:0]=hx00 in case of top field programmation
        --when raster=1, dcs[2:0]="000"*/
    }DMA_IRP03_DC1S_L_ts;

}DMA_IRP03_DC1S_L_tu;



//DMA_IRP03_DC1S_H
/*Description: This register defines the 16 most significant bit address (MSB) of the pipe 0/3 Destination buffer Chroma Start address
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t DMA_IRP03_DC1S_H;
    }DMA_IRP03_DC1S_H_ts;

}DMA_IRP03_DC1S_H_tu;



//DMA_IRP03_DC2S_L
/*Description: This register defines the 16 least significant bit address (LSB) of the pipe 0/3 Destination buffer Chroma 2 Start address (planar format)
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t reserved0 :3;
        uint16_t DMA_IRP03_DC2S_L :13;
        /* 
        -when cfc=1
        --when raster =0 & inter_en =0, dcs[6:0]=hx00
        --when raster=0 & inter_en =1, dcs[6:0]=hx08 in case of bot field programmation
        --when raster=0 & inter_en =1, dcs[6:0]=hx00 in case of top field programmation
        --when raster=1, dcs[2:0]="000"
        -when cfc=0
        --when raster =0 & inter_en =0, dcs[5:0]=hx00
        --when raster=0 & inter_en =1, dcs[5:0]=hx08 in case of bot field programmation
        --when raster=0 & inter_en =1, dcs[5:0]=hx00 in case of top field programmation
        --when raster=1, dcs[2:0]="000"*/
    }DMA_IRP03_DC2S_L_ts;

}DMA_IRP03_DC2S_L_tu;



//DMA_IRP03_DC2S_H
/*Description: This register defines the 16 most significant bit address (MSB) of the pipe 0/3 Destination buffer Chroma 2 Start address
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t DMA_IRP03_DC2S_H;
    }DMA_IRP03_DC2S_H_ts;

}DMA_IRP03_DC2S_H_tu;



//DMA_IRP3_DRS_L
/*Description: This register defines the 16 least significant bit address (LSB) of the pipe 0/3 Destination buffer raw Start address
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t reserved0 :3;
        uint16_t DMA_IRP3_DRS_L :13;
    }DMA_IRP3_DRS_L_ts;

}DMA_IRP3_DRS_L_tu;



//DMA_IRP3_DRS_H
/*Description: This register defines the 16 most significant bit address (MSB) of the pipe 0/3 Destination buffer raw Start address
*/
typedef union
{
    uint32_t word;
    uint16_t data;
    struct
    {
        uint16_t DMA_IRP3_DRS_H;
    }DMA_IRP3_DRS_H_ts;

}DMA_IRP3_DRS_H_tu;



//DMA_IRP03_HPS_L
/*Description: This register defines the 16 least significant bit address (LSB) of the pipe 0/3 Horizontal Projection buffer Start address
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t reserved0 :3;
        uint16_t DMA_IRP03_HPS_L :13;
    }DMA_IRP03_HPS_L_ts;

}DMA_IRP03_HPS_L_tu;



//DMA_IRP03_HPS_H
/*Description: This register defines the 16 most significant bit address (MSB) of the pipe 0/3 Horizontal Projection buffer Start address
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t DMA_IRP03_HPS_H;
    }DMA_IRP03_HPS_H_ts;

}DMA_IRP03_HPS_H_tu;



//DMA_IRP03_VPS_L
/*Description: This register defines the 16 least significant bit address (LSB) of the pipe 0/3 Vertical Projection buffer Start address
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t reserved0 :3;
        uint16_t DMA_IRP03_VPS_L :13;
    }DMA_IRP03_VPS_L_ts;

}DMA_IRP03_VPS_L_tu;



//DMA_IRP03_VPS_H
/*Description: This register defines the 16 most significant bit address (MSB) of the pipe 0/3 Vertical Projection buffer Start address
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t DMA_IRP03_VPS_H;
    }DMA_IRP03_VPS_H_ts;

}DMA_IRP03_VPS_H_tu;



//DMA_IRP3_JPGS_L
/*Description: This register defines the 16 least significant bit address (LSB) of the pipe 0/3 Vertical Projection buffer Start address
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t reserved0 :3;
        uint16_t DMA_IRP03_VPS_L :13;
    }DMA_IRP3_JPGS_L_ts;

}DMA_IRP3_JPGS_L_tu;



//DMA_IRP3_JPGS_H
/*Description: This register defines the 16 most significant bit address (MSB) of the pipe 0/3 Vertical Projection buffer Start address
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t DMA_IRP03_VPS_H;
    }DMA_IRP3_JPGS_H_ts;

}DMA_IRP3_JPGS_H_tu;



//DMA_IRP03_DFW
/*Description: This register defines the frame width of the destination buffer (in number of bytes) for channels IRP03_L/C/C1/C2/Pra_W
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t reserved0 :3;
        uint16_t DMA_IRP03_DFW :13;
        /* 
        -when raster=0: 16 bytes aligned.
        -when raster=1: 8 bytes aligned*/
    }DMA_IRP03_DFW_ts;

}DMA_IRP03_DFW_tu;



//DMA_IRP03_DWW
/*Description: This register defines the window width of the destination buffer (in number of bytes) for channels IRP03_L/C/C1/C2/Pra_W
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t reserved0 :3;
        uint16_t DMA_IRP03_DWW :13;
        /* 
        -when raster=0: 16 bytes aligned.
        -when raster=1: 8 bytes aligned*/
    }DMA_IRP03_DWW_ts;

}DMA_IRP03_DWW_tu;



//DMA_IRP03_DWH
/*Description: This register defines the window height of the destination buffer (in number of lines) for channels IRP03_L/C/C1/C2/Pra_W
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t DMA_IRP03_DWH;
        /* 
        -when raster=0 & inter_en =0, dwh[3:0]=0: 16 bytes aligned.
        -when raster=0 & inter_en=1, dwh[2:0]=0: 8 bytes aligned (number of line in the field)
        -when raster=1: byte aligned*/
    }DMA_IRP03_DWH_ts;

}DMA_IRP03_DWH_tu;



//DMA_IRP3_JPG_DBS_L
/*Description: This register defines the LSB part of the size of the JPEG destination buffer (in number of bytes) for channels IRP3_JPG. This size shall be aligned on 8-bytes boundary
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t reserved0 :3;
        uint16_t DMA_IRP3_JPG_DBS_L :13;
        /* LSB of the JPEG destination buffer (in bytes)*/
    }DMA_IRP3_JPG_DBS_L_ts;

}DMA_IRP3_JPG_DBS_L_tu;



//DMA_IRP3_JPG_DBS_H
/*Description: This register defines the MSB part of the size of the JPEG destination buffer (in number of bytes) for channels IRP3_JPG
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t DMA_IRP3_JPG_DBS_H;
        /* MSB of the JPEG destination buffer (in bytes)*/
    }DMA_IRP3_JPG_DBS_H_ts;

}DMA_IRP3_JPG_DBS_H_tu;



//DMA_IRP03_CFC
/*Description: This register defines the Pipe03 Chroma Format Configuration (vertical chroma sampling).
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t reserved0 :1;
        uint8_t IRP03_C_W :1;
        /* YUV420 chroma sampling*/
        uint8_t IRP03_C1_W :1;
        /* YUV420 chroma sampling*/
        uint8_t IRP03_C2_W :1;
        /* YUV420 chroma sampling*/
        uint8_t reserved1 :1;
        uint8_t IRP03_GCF_C_W :1;
        /* YUV420 chroma sampling*/
        uint8_t reserved2 :1;
        uint8_t IRP03_GCF_C_R :1;
        /* YUV420 chroma sampling*/
    }DMA_IRP03_CFC_ts;

}DMA_IRP03_CFC_tu;



//DMA_IRP03_RASTER
/*Description: This register specify the type of format which is scanned (macro-block or raster).
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t IRP03_L_W :1;
        /* luma write channel for pipe 0 or 3*/
        uint8_t IRP03_C_W :1;
        /* chroma write channel for pipe 0 or 3*/
        uint8_t IRP03_C1_W :1;
        /* chroma1 write channel for pipe 0 or 3*/
        uint8_t IRP03_C2_W :1;
        /* chroma2 write channel for pipe 0 or 3*/
    }DMA_IRP03_RASTER_ts;

}DMA_IRP03_RASTER_tu;



//DMA_IRP03_ROTATION
/*Description: This register configures rotation of the image during grab
*/
typedef union
{
    uint32_t word;
    uint16_t data;
    struct
    {
        uint8_t ROTATION :2;
        /* rotation of the frame*/
    }DMA_IRP03_ROTATION_ts;

}DMA_IRP03_ROTATION_tu;



//DMA_IRP0_SRS_L
/*Description: This register defines the least significant bit address (LSB) of the pipe 0 Source Raster buffer Start address
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t reserved0 :3;
        uint16_t DMA_IRP0_SRS_L :13;
    }DMA_IRP0_SRS_L_ts;

}DMA_IRP0_SRS_L_tu;



//DMA_IRP0_SRS_H
/*Description: This register defines the 16 most significant bit address (MSB) of the pipe 0 Source Raster buffer Start address
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t DMA_IRP0_SRS_H;
    }DMA_IRP0_SRS_H_ts;

}DMA_IRP0_SRS_H_tu;



//DMA_IRP0_SFW
/*Description: This register defines the frame width of the source buffer (in number of bytes) for channels IRP0_Pra_R
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t reserved0 :3;
        uint16_t DMA_IRP0_SFW :13;
        /* 8 bytes aligned*/
    }DMA_IRP0_SFW_ts;

}DMA_IRP0_SFW_tu;



//DMA_IRP0_SWW
/*Description: This register defines the window width of the source buffer (in number of bytes) for channels IRP0_Pra_R
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t reserved0 :3;
        uint16_t DMA_IRP0_SWW :13;
        /* 8 bytes aligned*/
    }DMA_IRP0_SWW_ts;

}DMA_IRP0_SWW_tu;



//DMA_IRP0_SWH
/*Description: This register defines the window height of the source buffer (in number of lines) for channels IRP0_Pra_R
*/
typedef union
{
    uint32_t word;
    uint16_t data;
    struct
    {
        uint16_t DMA_IRP0_SWH;
        /* byte aligned*/
    }DMA_IRP0_SWH_ts;

}DMA_IRP0_SWH_tu;



//DMA_IRP1_DRS_L
/*Description: This register defines the 16 least significant bit address (LSB) of the pipe 1 Destination Raster buffer Start address
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t reserved0 :3;
        uint16_t DMA_IRP1_DRS_L :13;
    }DMA_IRP1_DRS_L_ts;

}DMA_IRP1_DRS_L_tu;



//DMA_IRP1_DRS_H
/*Description: This register defines the 16 most significant bit address (MSB) of the pipe 1 Destination Raster buffer Start address
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t DMA_IRP1_DRS_H;
    }DMA_IRP1_DRS_H_ts;

}DMA_IRP1_DRS_H_tu;



//DMA_IRP1_DFW
/*Description: This register defines the frame width of the destination buffer (in number of bytes) for channels IRP1_Pra_W
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t reserved0 :3;
        uint16_t DMA_IRP1_DFW :13;
        /* 8 bytes aligned*/
    }DMA_IRP1_DFW_ts;

}DMA_IRP1_DFW_tu;



//DMA_IRP1_DWW
/*Description: This register defines the window width of the destination buffer (in number of bytes) for channels IRP1_Pra_W
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t reserved0 :3;
        uint16_t DMA_IRP1_DWW :13;
        /* 8 bytes aligned*/
    }DMA_IRP1_DWW_ts;

}DMA_IRP1_DWW_tu;



//DMA_IRP1_DWH
/*Description: This register defines the window height of the destination buffer (in number of lines) for channels IRP1_Pra_W
*/
typedef union
{
    uint32_t word;
    uint16_t data;
    struct
    {
        uint16_t DMA_IRP1_DWH;
        /* byte aligned*/
    }DMA_IRP1_DWH_ts;

}DMA_IRP1_DWH_tu;



//DMA_IRP2_DRS_L
/*Description: This register defines the 16 least significant bit address (LSB) of the pipe 2 Destination Raster buffer Start address
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t reserved0 :3;
        uint16_t DMA_IRP2_DRS_L :13;
    }DMA_IRP2_DRS_L_ts;

}DMA_IRP2_DRS_L_tu;



//DMA_IRP2_DRS_H
/*Description: This register defines the 16 most significant bit address (MSB) of the pipe 2 Destination Raster buffer Start address
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t DMA_IRP2_DRS_H;
    }DMA_IRP2_DRS_H_ts;

}DMA_IRP2_DRS_H_tu;



//DMA_IRP2_DFW
/*Description: This register defines the frame width of the destination buffer (in number of bytes) for channels IRP2_Pra_W
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t reserved0 :3;
        uint16_t DMA_IRP2_DFW :13;
        /* 8 bytes aligned*/
    }DMA_IRP2_DFW_ts;

}DMA_IRP2_DFW_tu;



//DMA_IRP2_DWW
/*Description: This register defines the window width of the destination buffer (in number of bytes) for channels IRP2_Pra_W
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t reserved0 :3;
        uint16_t DMA_IRP2_DWW :13;
        /* 8 bytes aligned*/
    }DMA_IRP2_DWW_ts;

}DMA_IRP2_DWW_tu;



//DMA_IRP2_DWH
/*Description: This register defines the window height of the destination buffer (in number of lines) for channels IRP2_Pra_W
*/
typedef union
{
    uint32_t word;
    uint16_t data;
    struct
    {
        uint16_t DMA_IRP2_DWH;
        /* byte aligned*/
    }DMA_IRP2_DWH_ts;

}DMA_IRP2_DWH_tu;



//DMA_TST_CID
/*Description: This register is a SW debug register that allows to track progress of a DMA process by register the current address of the observed DMA process in DMA_TST_SADD_L_ADDR and DMA_TST_SADD_H_ADDR. This is not intended to provide an accurate information, but rather to check whether DMA is progressing or not (by consecutive writes in this register/reads in DMA_TST_SADD_L/H_ADDR). This is recommended to write a single '1' in this register: when multiple bits are written one, the result in DMA_TST_SADD_L/H_ADDR is UNDEFINED.
*/
typedef union
{
    uint32_t word;
    uint16_t data;
    struct
    {
        uint8_t DMA_TST_CID :5;
        /* ID of the channel to observe*/
    }DMA_TST_CID_ts;

}DMA_TST_CID_tu;



//DMA_TST_SADD_L
/*Description: This register defines the 16 least significant bit address (LSB) of the observed address. This register is updated upon a write '1' in on of the bits of DMA_TST_CID.
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t reserved0 :3;
        uint16_t CURADDR_L :13;
    }DMA_TST_SADD_L_ts;

}DMA_TST_SADD_L_tu;



//DMA_TST_SADD_H
/*Description: This register defines the 16 most significant bit address (MSB) of the observed address. This register is updated upon a write '1' in on of the bits of DMA_TST_CID
*/
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t CURADDR_H;
    }DMA_TST_SADD_H_ts;

}DMA_TST_SADD_H_tu;

typedef struct
{
    DMA_ENR_tu DMA_ENR;
    DMA_DIR_tu DMA_DIR;
    DMA_PEN_tu DMA_PEN;
    DMA_INTERLACE_EN_tu DMA_INTERLACE_EN;
    uint32_t pad_DMA_ITS1;
    DMA_ITS1_tu DMA_ITS1;
    DMA_ITS1_BCLR_tu DMA_ITS1_BCLR;
    DMA_IMR1_tu DMA_IMR1;
    uint32_t pad_DMA_ITS2;
    DMA_ITS2_tu DMA_ITS2;
    DMA_ITS2_BCLR_tu DMA_ITS2_BCLR;
    DMA_IMR2_tu DMA_IMR2;
    uint32_t pad_DMA_SOFT_RST[2];
    DMA_SOFT_RST_tu DMA_SOFT_RST;
    DMA_FIFO_STA_tu DMA_FIFO_STA;
    DMA_FIFO_FLUSH_tu DMA_FIFO_FLUSH;
    uint32_t pad_DMA_IRP03_DLS_L[2];
    DMA_IRP03_DLS_L_tu DMA_IRP03_DLS_L;
    DMA_IRP03_DLS_H_tu DMA_IRP03_DLS_H;
    DMA_IRP03_DCS_L_tu DMA_IRP03_DCS_L;
    DMA_IRP03_DCS_H_tu DMA_IRP03_DCS_H;
    DMA_IRP03_CLS_L_tu DMA_IRP03_CLS_L;
    DMA_IRP03_CLS_H_tu DMA_IRP03_CLS_H;
    DMA_IRP03_CCS_L_tu DMA_IRP03_CCS_L;
    DMA_IRP03_CCS_H_tu DMA_IRP03_CCS_H;
    DMA_IRP03_DC1S_L_tu DMA_IRP03_DC1S_L;
    DMA_IRP03_DC1S_H_tu DMA_IRP03_DC1S_H;
    DMA_IRP03_DC2S_L_tu DMA_IRP03_DC2S_L;
    DMA_IRP03_DC2S_H_tu DMA_IRP03_DC2S_H;
    DMA_IRP3_DRS_L_tu DMA_IRP3_DRS_L;
    DMA_IRP3_DRS_H_tu DMA_IRP3_DRS_H;
    DMA_IRP03_HPS_L_tu DMA_IRP03_HPS_L;
    DMA_IRP03_HPS_H_tu DMA_IRP03_HPS_H;
    DMA_IRP03_VPS_L_tu DMA_IRP03_VPS_L;
    DMA_IRP03_VPS_H_tu DMA_IRP03_VPS_H;
    DMA_IRP3_JPGS_L_tu DMA_IRP3_JPGS_L;
    DMA_IRP3_JPGS_H_tu DMA_IRP3_JPGS_H;
    DMA_IRP03_DFW_tu DMA_IRP03_DFW;
    DMA_IRP03_DWW_tu DMA_IRP03_DWW;
    DMA_IRP03_DWH_tu DMA_IRP03_DWH;
    DMA_IRP3_JPG_DBS_L_tu DMA_IRP3_JPG_DBS_L;
    DMA_IRP3_JPG_DBS_H_tu DMA_IRP3_JPG_DBS_H;
    DMA_IRP03_CFC_tu DMA_IRP03_CFC;
    DMA_IRP03_RASTER_tu DMA_IRP03_RASTER;
    DMA_IRP03_ROTATION_tu DMA_IRP03_ROTATION;
    DMA_IRP0_SRS_L_tu DMA_IRP0_SRS_L;
    DMA_IRP0_SRS_H_tu DMA_IRP0_SRS_H;
    DMA_IRP0_SFW_tu DMA_IRP0_SFW;
    DMA_IRP0_SWW_tu DMA_IRP0_SWW;
    DMA_IRP0_SWH_tu DMA_IRP0_SWH;
    uint32_t pad_DMA_IRP1_DRS_L;
    DMA_IRP1_DRS_L_tu DMA_IRP1_DRS_L;
    DMA_IRP1_DRS_H_tu DMA_IRP1_DRS_H;
    DMA_IRP1_DFW_tu DMA_IRP1_DFW;
    DMA_IRP1_DWW_tu DMA_IRP1_DWW;
    DMA_IRP1_DWH_tu DMA_IRP1_DWH;
    uint32_t pad_DMA_IRP2_DRS_L;
    DMA_IRP2_DRS_L_tu DMA_IRP2_DRS_L;
    DMA_IRP2_DRS_H_tu DMA_IRP2_DRS_H;
    DMA_IRP2_DFW_tu DMA_IRP2_DFW;
    DMA_IRP2_DWW_tu DMA_IRP2_DWW;
    DMA_IRP2_DWH_tu DMA_IRP2_DWH;
    uint32_t pad_DMA_TST_CID;
    DMA_TST_CID_tu DMA_TST_CID;
    DMA_TST_SADD_L_tu DMA_TST_SADD_L;
    DMA_TST_SADD_H_tu DMA_TST_SADD_H;
}DMA_IP_ts;




//DMA_ENR


#define Get_DMA_DMA_ENR_IRP03_L_W() p_DMA_IP->DMA_ENR.DMA_ENR_ts.IRP03_L_W
#define Set_DMA_DMA_ENR_IRP03_L_W(x) (p_DMA_IP->DMA_ENR.DMA_ENR_ts.IRP03_L_W = x)


#define Get_DMA_DMA_ENR_IRP03_C_W() p_DMA_IP->DMA_ENR.DMA_ENR_ts.IRP03_C_W
#define Set_DMA_DMA_ENR_IRP03_C_W(x) (p_DMA_IP->DMA_ENR.DMA_ENR_ts.IRP03_C_W = x)


#define Get_DMA_DMA_ENR_IRP03_C1_W() p_DMA_IP->DMA_ENR.DMA_ENR_ts.IRP03_C1_W
#define Set_DMA_DMA_ENR_IRP03_C1_W(x) (p_DMA_IP->DMA_ENR.DMA_ENR_ts.IRP03_C1_W = x)


#define Get_DMA_DMA_ENR_IRP03_C2_W() p_DMA_IP->DMA_ENR.DMA_ENR_ts.IRP03_C2_W
#define Set_DMA_DMA_ENR_IRP03_C2_W(x) (p_DMA_IP->DMA_ENR.DMA_ENR_ts.IRP03_C2_W = x)


#define Get_DMA_DMA_ENR_IRP03_GCF_L_W() p_DMA_IP->DMA_ENR.DMA_ENR_ts.IRP03_GCF_L_W
#define Set_DMA_DMA_ENR_IRP03_GCF_L_W(x) (p_DMA_IP->DMA_ENR.DMA_ENR_ts.IRP03_GCF_L_W = x)


#define Get_DMA_DMA_ENR_IRP03_GCF_C_W() p_DMA_IP->DMA_ENR.DMA_ENR_ts.IRP03_GCF_C_W
#define Set_DMA_DMA_ENR_IRP03_GCF_C_W(x) (p_DMA_IP->DMA_ENR.DMA_ENR_ts.IRP03_GCF_C_W = x)


#define Get_DMA_DMA_ENR_IRP03_GCF_L_R() p_DMA_IP->DMA_ENR.DMA_ENR_ts.IRP03_GCF_L_R
#define Set_DMA_DMA_ENR_IRP03_GCF_L_R(x) (p_DMA_IP->DMA_ENR.DMA_ENR_ts.IRP03_GCF_L_R = x)


#define Get_DMA_DMA_ENR_IRP03_GCF_C_R() p_DMA_IP->DMA_ENR.DMA_ENR_ts.IRP03_GCF_C_R
#define Set_DMA_DMA_ENR_IRP03_GCF_C_R(x) (p_DMA_IP->DMA_ENR.DMA_ENR_ts.IRP03_GCF_C_R = x)


#define Get_DMA_DMA_ENR_IRP03_VCP_W() p_DMA_IP->DMA_ENR.DMA_ENR_ts.IRP03_VCP_W
#define Set_DMA_DMA_ENR_IRP03_VCP_W(x) (p_DMA_IP->DMA_ENR.DMA_ENR_ts.IRP03_VCP_W = x)


#define Get_DMA_DMA_ENR_IRP03_HCP_W() p_DMA_IP->DMA_ENR.DMA_ENR_ts.IRP03_HCP_W
#define Set_DMA_DMA_ENR_IRP03_HCP_W(x) (p_DMA_IP->DMA_ENR.DMA_ENR_ts.IRP03_HCP_W = x)


#define Get_DMA_DMA_ENR_IRP03_HCP_R() p_DMA_IP->DMA_ENR.DMA_ENR_ts.IRP03_HCP_R
#define Set_DMA_DMA_ENR_IRP03_HCP_R(x) (p_DMA_IP->DMA_ENR.DMA_ENR_ts.IRP03_HCP_R = x)


#define Get_DMA_DMA_ENR_IRP03_PRA_W() p_DMA_IP->DMA_ENR.DMA_ENR_ts.IRP03_PRA_W
#define Set_DMA_DMA_ENR_IRP03_PRA_W(x) (p_DMA_IP->DMA_ENR.DMA_ENR_ts.IRP03_PRA_W = x)


#define Get_DMA_DMA_ENR_IRP0_PRA_R() p_DMA_IP->DMA_ENR.DMA_ENR_ts.IRP0_PRA_R
#define Set_DMA_DMA_ENR_IRP0_PRA_R(x) (p_DMA_IP->DMA_ENR.DMA_ENR_ts.IRP0_PRA_R = x)


#define Get_DMA_DMA_ENR_IRP1_PRA_W() p_DMA_IP->DMA_ENR.DMA_ENR_ts.IRP1_PRA_W
#define Set_DMA_DMA_ENR_IRP1_PRA_W(x) (p_DMA_IP->DMA_ENR.DMA_ENR_ts.IRP1_PRA_W = x)


#define Get_DMA_DMA_ENR_IRP2_PRA_W() p_DMA_IP->DMA_ENR.DMA_ENR_ts.IRP2_PRA_W
#define Set_DMA_DMA_ENR_IRP2_PRA_W(x) (p_DMA_IP->DMA_ENR.DMA_ENR_ts.IRP2_PRA_W = x)


#define Get_DMA_DMA_ENR_IRP3_JPG_W() p_DMA_IP->DMA_ENR.DMA_ENR_ts.IRP3_JPG_W
#define Set_DMA_DMA_ENR_IRP3_JPG_W(x) (p_DMA_IP->DMA_ENR.DMA_ENR_ts.IRP3_JPG_W = x)
#define Set_DMA_DMA_ENR(IRP03_L_W,IRP03_C_W,IRP03_C1_W,IRP03_C2_W,IRP03_GCF_L_W,IRP03_GCF_C_W,IRP03_GCF_L_R,IRP03_GCF_C_R,IRP03_VCP_W,IRP03_HCP_W,IRP03_HCP_R,IRP03_PRA_W,IRP0_PRA_R,IRP1_PRA_W,IRP2_PRA_W,IRP3_JPG_W) (p_DMA_IP->DMA_ENR.word = (uint16_t)IRP03_L_W<<0 | (uint16_t)IRP03_C_W<<1 | (uint16_t)IRP03_C1_W<<2 | (uint16_t)IRP03_C2_W<<3 | (uint16_t)IRP03_GCF_L_W<<4 | (uint16_t)IRP03_GCF_C_W<<5 | (uint16_t)IRP03_GCF_L_R<<6 | (uint16_t)IRP03_GCF_C_R<<7 | (uint16_t)IRP03_VCP_W<<8 | (uint16_t)IRP03_HCP_W<<9 | (uint16_t)IRP03_HCP_R<<10 | (uint16_t)IRP03_PRA_W<<11 | (uint16_t)IRP0_PRA_R<<12 | (uint16_t)IRP1_PRA_W<<13 | (uint16_t)IRP2_PRA_W<<14 | (uint16_t)IRP3_JPG_W<<15)
#define Get_DMA_DMA_ENR() p_DMA_IP->DMA_ENR.word
#define Set_DMA_DMA_ENR_word(x) (p_DMA_IP->DMA_ENR.word = x)


//DMA_DIR


#define Get_DMA_DMA_DIR_IRP03_L_W() p_DMA_IP->DMA_DIR.DMA_DIR_ts.IRP03_L_W
#define Set_DMA_DMA_DIR_IRP03_L_W(x) (p_DMA_IP->DMA_DIR.DMA_DIR_ts.IRP03_L_W = x)


#define Get_DMA_DMA_DIR_IRP03_C_W() p_DMA_IP->DMA_DIR.DMA_DIR_ts.IRP03_C_W
#define Set_DMA_DMA_DIR_IRP03_C_W(x) (p_DMA_IP->DMA_DIR.DMA_DIR_ts.IRP03_C_W = x)


#define Get_DMA_DMA_DIR_IRP03_C1_W() p_DMA_IP->DMA_DIR.DMA_DIR_ts.IRP03_C1_W
#define Set_DMA_DMA_DIR_IRP03_C1_W(x) (p_DMA_IP->DMA_DIR.DMA_DIR_ts.IRP03_C1_W = x)


#define Get_DMA_DMA_DIR_IRP03_C2_W() p_DMA_IP->DMA_DIR.DMA_DIR_ts.IRP03_C2_W
#define Set_DMA_DMA_DIR_IRP03_C2_W(x) (p_DMA_IP->DMA_DIR.DMA_DIR_ts.IRP03_C2_W = x)


#define Get_DMA_DMA_DIR_IRP03_GCF_L_W() p_DMA_IP->DMA_DIR.DMA_DIR_ts.IRP03_GCF_L_W
#define Set_DMA_DMA_DIR_IRP03_GCF_L_W(x) (p_DMA_IP->DMA_DIR.DMA_DIR_ts.IRP03_GCF_L_W = x)


#define Get_DMA_DMA_DIR_IRP03_GCF_C_W() p_DMA_IP->DMA_DIR.DMA_DIR_ts.IRP03_GCF_C_W
#define Set_DMA_DMA_DIR_IRP03_GCF_C_W(x) (p_DMA_IP->DMA_DIR.DMA_DIR_ts.IRP03_GCF_C_W = x)


#define Get_DMA_DMA_DIR_IRP03_GCF_L_R() p_DMA_IP->DMA_DIR.DMA_DIR_ts.IRP03_GCF_L_R
#define Set_DMA_DMA_DIR_IRP03_GCF_L_R(x) (p_DMA_IP->DMA_DIR.DMA_DIR_ts.IRP03_GCF_L_R = x)


#define Get_DMA_DMA_DIR_IRP03_GCF_C_R() p_DMA_IP->DMA_DIR.DMA_DIR_ts.IRP03_GCF_C_R
#define Set_DMA_DMA_DIR_IRP03_GCF_C_R(x) (p_DMA_IP->DMA_DIR.DMA_DIR_ts.IRP03_GCF_C_R = x)


#define Get_DMA_DMA_DIR_IRP03_VCP_W() p_DMA_IP->DMA_DIR.DMA_DIR_ts.IRP03_VCP_W
#define Set_DMA_DMA_DIR_IRP03_VCP_W(x) (p_DMA_IP->DMA_DIR.DMA_DIR_ts.IRP03_VCP_W = x)


#define Get_DMA_DMA_DIR_IRP03_HCP_W() p_DMA_IP->DMA_DIR.DMA_DIR_ts.IRP03_HCP_W
#define Set_DMA_DMA_DIR_IRP03_HCP_W(x) (p_DMA_IP->DMA_DIR.DMA_DIR_ts.IRP03_HCP_W = x)


#define Get_DMA_DMA_DIR_IRP03_HCP_R() p_DMA_IP->DMA_DIR.DMA_DIR_ts.IRP03_HCP_R
#define Set_DMA_DMA_DIR_IRP03_HCP_R(x) (p_DMA_IP->DMA_DIR.DMA_DIR_ts.IRP03_HCP_R = x)


#define Get_DMA_DMA_DIR_IRP03_PRA_W() p_DMA_IP->DMA_DIR.DMA_DIR_ts.IRP03_PRA_W
#define Set_DMA_DMA_DIR_IRP03_PRA_W(x) (p_DMA_IP->DMA_DIR.DMA_DIR_ts.IRP03_PRA_W = x)


#define Get_DMA_DMA_DIR_IRP0_PRA_R() p_DMA_IP->DMA_DIR.DMA_DIR_ts.IRP0_PRA_R
#define Set_DMA_DMA_DIR_IRP0_PRA_R(x) (p_DMA_IP->DMA_DIR.DMA_DIR_ts.IRP0_PRA_R = x)


#define Get_DMA_DMA_DIR_IRP1_PRA_W() p_DMA_IP->DMA_DIR.DMA_DIR_ts.IRP1_PRA_W
#define Set_DMA_DMA_DIR_IRP1_PRA_W(x) (p_DMA_IP->DMA_DIR.DMA_DIR_ts.IRP1_PRA_W = x)


#define Get_DMA_DMA_DIR_IRP2_PRA_W() p_DMA_IP->DMA_DIR.DMA_DIR_ts.IRP2_PRA_W
#define Set_DMA_DMA_DIR_IRP2_PRA_W(x) (p_DMA_IP->DMA_DIR.DMA_DIR_ts.IRP2_PRA_W = x)


#define Get_DMA_DMA_DIR_IRP3_JPG_W() p_DMA_IP->DMA_DIR.DMA_DIR_ts.IRP3_JPG_W
#define Set_DMA_DMA_DIR_IRP3_JPG_W(x) (p_DMA_IP->DMA_DIR.DMA_DIR_ts.IRP3_JPG_W = x)
#define Set_DMA_DMA_DIR(IRP03_L_W,IRP03_C_W,IRP03_C1_W,IRP03_C2_W,IRP03_GCF_L_W,IRP03_GCF_C_W,IRP03_GCF_L_R,IRP03_GCF_C_R,IRP03_VCP_W,IRP03_HCP_W,IRP03_HCP_R,IRP03_PRA_W,IRP0_PRA_R,IRP1_PRA_W,IRP2_PRA_W,IRP3_JPG_W) (p_DMA_IP->DMA_DIR.word = (uint16_t)IRP03_L_W<<0 | (uint16_t)IRP03_C_W<<1 | (uint16_t)IRP03_C1_W<<2 | (uint16_t)IRP03_C2_W<<3 | (uint16_t)IRP03_GCF_L_W<<4 | (uint16_t)IRP03_GCF_C_W<<5 | (uint16_t)IRP03_GCF_L_R<<6 | (uint16_t)IRP03_GCF_C_R<<7 | (uint16_t)IRP03_VCP_W<<8 | (uint16_t)IRP03_HCP_W<<9 | (uint16_t)IRP03_HCP_R<<10 | (uint16_t)IRP03_PRA_W<<11 | (uint16_t)IRP0_PRA_R<<12 | (uint16_t)IRP1_PRA_W<<13 | (uint16_t)IRP2_PRA_W<<14 | (uint16_t)IRP3_JPG_W<<15)
#define Get_DMA_DMA_DIR() p_DMA_IP->DMA_DIR.word
#define Set_DMA_DMA_DIR_word(x) (p_DMA_IP->DMA_DIR.word = x)


//DMA_PEN


#define Get_DMA_DMA_PEN_IRP03_L_W() p_DMA_IP->DMA_PEN.DMA_PEN_ts.IRP03_L_W
#define Is_DMA_DMA_PEN_IRP03_L_W_B_0x0() (p_DMA_IP->DMA_PEN.DMA_PEN_ts.IRP03_L_W == IRP03_L_W_B_0x0)
#define IRP03_L_W_B_0x0 0x0    //process not pending
#define Is_DMA_DMA_PEN_IRP03_L_W_B_0x1() (p_DMA_IP->DMA_PEN.DMA_PEN_ts.IRP03_L_W == IRP03_L_W_B_0x1)
#define IRP03_L_W_B_0x1 0x1    //process pending


#define Get_DMA_DMA_PEN_IRP03_C_W() p_DMA_IP->DMA_PEN.DMA_PEN_ts.IRP03_C_W
#define Is_DMA_DMA_PEN_IRP03_C_W_B_0x0() (p_DMA_IP->DMA_PEN.DMA_PEN_ts.IRP03_C_W == IRP03_C_W_B_0x0)
#define IRP03_C_W_B_0x0 0x0    //process not pending
#define Is_DMA_DMA_PEN_IRP03_C_W_B_0x1() (p_DMA_IP->DMA_PEN.DMA_PEN_ts.IRP03_C_W == IRP03_C_W_B_0x1)
#define IRP03_C_W_B_0x1 0x1    //process pending


#define Get_DMA_DMA_PEN_IRP03_C1_W() p_DMA_IP->DMA_PEN.DMA_PEN_ts.IRP03_C1_W
#define Is_DMA_DMA_PEN_IRP03_C1_W_B_0x0() (p_DMA_IP->DMA_PEN.DMA_PEN_ts.IRP03_C1_W == IRP03_C1_W_B_0x0)
#define IRP03_C1_W_B_0x0 0x0    //process not pending
#define Is_DMA_DMA_PEN_IRP03_C1_W_B_0x1() (p_DMA_IP->DMA_PEN.DMA_PEN_ts.IRP03_C1_W == IRP03_C1_W_B_0x1)
#define IRP03_C1_W_B_0x1 0x1    //process pending


#define Get_DMA_DMA_PEN_IRP03_C2_W() p_DMA_IP->DMA_PEN.DMA_PEN_ts.IRP03_C2_W
#define Is_DMA_DMA_PEN_IRP03_C2_W_B_0x0() (p_DMA_IP->DMA_PEN.DMA_PEN_ts.IRP03_C2_W == IRP03_C2_W_B_0x0)
#define IRP03_C2_W_B_0x0 0x0    //process not pending
#define Is_DMA_DMA_PEN_IRP03_C2_W_B_0x1() (p_DMA_IP->DMA_PEN.DMA_PEN_ts.IRP03_C2_W == IRP03_C2_W_B_0x1)
#define IRP03_C2_W_B_0x1 0x1    //process pending


#define Get_DMA_DMA_PEN_IRP03_PRA_W() p_DMA_IP->DMA_PEN.DMA_PEN_ts.IRP03_PRA_W
#define Is_DMA_DMA_PEN_IRP03_PRA_W_B_0x0() (p_DMA_IP->DMA_PEN.DMA_PEN_ts.IRP03_PRA_W == IRP03_PRA_W_B_0x0)
#define IRP03_PRA_W_B_0x0 0x0    //process not pending
#define Is_DMA_DMA_PEN_IRP03_PRA_W_B_0x1() (p_DMA_IP->DMA_PEN.DMA_PEN_ts.IRP03_PRA_W == IRP03_PRA_W_B_0x1)
#define IRP03_PRA_W_B_0x1 0x1    //process pending


#define Get_DMA_DMA_PEN_IRP2_PRA_W() p_DMA_IP->DMA_PEN.DMA_PEN_ts.IRP2_PRA_W
#define Is_DMA_DMA_PEN_IRP2_PRA_W_B_0x0() (p_DMA_IP->DMA_PEN.DMA_PEN_ts.IRP2_PRA_W == IRP2_PRA_W_B_0x0)
#define IRP2_PRA_W_B_0x0 0x0    //process not pending
#define Is_DMA_DMA_PEN_IRP2_PRA_W_B_0x1() (p_DMA_IP->DMA_PEN.DMA_PEN_ts.IRP2_PRA_W == IRP2_PRA_W_B_0x1)
#define IRP2_PRA_W_B_0x1 0x1    //process pending
#define Get_DMA_DMA_PEN() p_DMA_IP->DMA_PEN.word


//DMA_INTERLACE_EN


#define Get_DMA_DMA_INTERLACE_EN_IRP03_L_W() p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP03_L_W
#define Is_DMA_DMA_INTERLACE_EN_IRP03_L_W_B_0x0() (p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP03_L_W == IRP03_L_W_B_0x0)
#define Set_DMA_DMA_INTERLACE_EN_IRP03_L_W__B_0x0() (p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP03_L_W = IRP03_L_W_B_0x0)
#define IRP03_L_W_B_0x0 0x0    //not interlaced
#define Is_DMA_DMA_INTERLACE_EN_IRP03_L_W_B_0x1() (p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP03_L_W == IRP03_L_W_B_0x1)
#define Set_DMA_DMA_INTERLACE_EN_IRP03_L_W__B_0x1() (p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP03_L_W = IRP03_L_W_B_0x1)
#define IRP03_L_W_B_0x1 0x1    //interlaced


#define Get_DMA_DMA_INTERLACE_EN_IRP03_C_W() p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP03_C_W
#define Is_DMA_DMA_INTERLACE_EN_IRP03_C_W_B_0x0() (p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP03_C_W == IRP03_C_W_B_0x0)
#define Set_DMA_DMA_INTERLACE_EN_IRP03_C_W__B_0x0() (p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP03_C_W = IRP03_C_W_B_0x0)
#define IRP03_C_W_B_0x0 0x0    //not interlaced
#define Is_DMA_DMA_INTERLACE_EN_IRP03_C_W_B_0x1() (p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP03_C_W == IRP03_C_W_B_0x1)
#define Set_DMA_DMA_INTERLACE_EN_IRP03_C_W__B_0x1() (p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP03_C_W = IRP03_C_W_B_0x1)
#define IRP03_C_W_B_0x1 0x1    //interlaced


#define Get_DMA_DMA_INTERLACE_EN_IRP03_C1_W() p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP03_C1_W
#define Is_DMA_DMA_INTERLACE_EN_IRP03_C1_W_B_0x0() (p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP03_C1_W == IRP03_C1_W_B_0x0)
#define Set_DMA_DMA_INTERLACE_EN_IRP03_C1_W__B_0x0() (p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP03_C1_W = IRP03_C1_W_B_0x0)
#define IRP03_C1_W_B_0x0 0x0    //not interlaced
#define Is_DMA_DMA_INTERLACE_EN_IRP03_C1_W_B_0x1() (p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP03_C1_W == IRP03_C1_W_B_0x1)
#define Set_DMA_DMA_INTERLACE_EN_IRP03_C1_W__B_0x1() (p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP03_C1_W = IRP03_C1_W_B_0x1)
#define IRP03_C1_W_B_0x1 0x1    //interlaced


#define Get_DMA_DMA_INTERLACE_EN_IRP03_C2_W() p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP03_C2_W
#define Is_DMA_DMA_INTERLACE_EN_IRP03_C2_W_B_0x0() (p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP03_C2_W == IRP03_C2_W_B_0x0)
#define Set_DMA_DMA_INTERLACE_EN_IRP03_C2_W__B_0x0() (p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP03_C2_W = IRP03_C2_W_B_0x0)
#define IRP03_C2_W_B_0x0 0x0    //not interlaced
#define Is_DMA_DMA_INTERLACE_EN_IRP03_C2_W_B_0x1() (p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP03_C2_W == IRP03_C2_W_B_0x1)
#define Set_DMA_DMA_INTERLACE_EN_IRP03_C2_W__B_0x1() (p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP03_C2_W = IRP03_C2_W_B_0x1)
#define IRP03_C2_W_B_0x1 0x1    //interlaced


#define Get_DMA_DMA_INTERLACE_EN_IRP03_GCF_L_W() p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP03_GCF_L_W
#define Is_DMA_DMA_INTERLACE_EN_IRP03_GCF_L_W_B_0x0() (p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP03_GCF_L_W == IRP03_GCF_L_W_B_0x0)
#define Set_DMA_DMA_INTERLACE_EN_IRP03_GCF_L_W__B_0x0() (p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP03_GCF_L_W = IRP03_GCF_L_W_B_0x0)
#define IRP03_GCF_L_W_B_0x0 0x0    //not interlaced
#define Is_DMA_DMA_INTERLACE_EN_IRP03_GCF_L_W_B_0x1() (p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP03_GCF_L_W == IRP03_GCF_L_W_B_0x1)
#define Set_DMA_DMA_INTERLACE_EN_IRP03_GCF_L_W__B_0x1() (p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP03_GCF_L_W = IRP03_GCF_L_W_B_0x1)
#define IRP03_GCF_L_W_B_0x1 0x1    //interlaced


#define Get_DMA_DMA_INTERLACE_EN_IRP03_GCF_C_W() p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP03_GCF_C_W
#define Is_DMA_DMA_INTERLACE_EN_IRP03_GCF_C_W_B_0x0() (p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP03_GCF_C_W == IRP03_GCF_C_W_B_0x0)
#define Set_DMA_DMA_INTERLACE_EN_IRP03_GCF_C_W__B_0x0() (p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP03_GCF_C_W = IRP03_GCF_C_W_B_0x0)
#define IRP03_GCF_C_W_B_0x0 0x0    //not interlaced
#define Is_DMA_DMA_INTERLACE_EN_IRP03_GCF_C_W_B_0x1() (p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP03_GCF_C_W == IRP03_GCF_C_W_B_0x1)
#define Set_DMA_DMA_INTERLACE_EN_IRP03_GCF_C_W__B_0x1() (p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP03_GCF_C_W = IRP03_GCF_C_W_B_0x1)
#define IRP03_GCF_C_W_B_0x1 0x1    //interlaced


#define Get_DMA_DMA_INTERLACE_EN_IRP03_GCF_L_R() p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP03_GCF_L_R
#define Is_DMA_DMA_INTERLACE_EN_IRP03_GCF_L_R_B_0x0() (p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP03_GCF_L_R == IRP03_GCF_L_R_B_0x0)
#define Set_DMA_DMA_INTERLACE_EN_IRP03_GCF_L_R__B_0x0() (p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP03_GCF_L_R = IRP03_GCF_L_R_B_0x0)
#define IRP03_GCF_L_R_B_0x0 0x0    //not interlaced
#define Is_DMA_DMA_INTERLACE_EN_IRP03_GCF_L_R_B_0x1() (p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP03_GCF_L_R == IRP03_GCF_L_R_B_0x1)
#define Set_DMA_DMA_INTERLACE_EN_IRP03_GCF_L_R__B_0x1() (p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP03_GCF_L_R = IRP03_GCF_L_R_B_0x1)
#define IRP03_GCF_L_R_B_0x1 0x1    //interlaced


#define Get_DMA_DMA_INTERLACE_EN_IRP03_GCF_C_R() p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP03_GCF_C_R
#define Is_DMA_DMA_INTERLACE_EN_IRP03_GCF_C_R_B_0x0() (p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP03_GCF_C_R == IRP03_GCF_C_R_B_0x0)
#define Set_DMA_DMA_INTERLACE_EN_IRP03_GCF_C_R__B_0x0() (p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP03_GCF_C_R = IRP03_GCF_C_R_B_0x0)
#define IRP03_GCF_C_R_B_0x0 0x0    //not interlaced
#define Is_DMA_DMA_INTERLACE_EN_IRP03_GCF_C_R_B_0x1() (p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP03_GCF_C_R == IRP03_GCF_C_R_B_0x1)
#define Set_DMA_DMA_INTERLACE_EN_IRP03_GCF_C_R__B_0x1() (p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP03_GCF_C_R = IRP03_GCF_C_R_B_0x1)
#define IRP03_GCF_C_R_B_0x1 0x1    //interlaced


#define Get_DMA_DMA_INTERLACE_EN_IRP03_PRA_W() p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP03_PRA_W
#define Is_DMA_DMA_INTERLACE_EN_IRP03_PRA_W_B_0x0() (p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP03_PRA_W == IRP03_PRA_W_B_0x0)
#define Set_DMA_DMA_INTERLACE_EN_IRP03_PRA_W__B_0x0() (p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP03_PRA_W = IRP03_PRA_W_B_0x0)
#define IRP03_PRA_W_B_0x0 0x0    //not interlaced
#define Is_DMA_DMA_INTERLACE_EN_IRP03_PRA_W_B_0x1() (p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP03_PRA_W == IRP03_PRA_W_B_0x1)
#define Set_DMA_DMA_INTERLACE_EN_IRP03_PRA_W__B_0x1() (p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP03_PRA_W = IRP03_PRA_W_B_0x1)
#define IRP03_PRA_W_B_0x1 0x1    //interlaced


#define Get_DMA_DMA_INTERLACE_EN_IRP0_PRA_R() p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP0_PRA_R
#define Is_DMA_DMA_INTERLACE_EN_IRP0_PRA_R_B_0x0() (p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP0_PRA_R == IRP0_PRA_R_B_0x0)
#define Set_DMA_DMA_INTERLACE_EN_IRP0_PRA_R__B_0x0() (p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP0_PRA_R = IRP0_PRA_R_B_0x0)
#define IRP0_PRA_R_B_0x0 0x0    //not interlaced
#define Is_DMA_DMA_INTERLACE_EN_IRP0_PRA_R_B_0x1() (p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP0_PRA_R == IRP0_PRA_R_B_0x1)
#define Set_DMA_DMA_INTERLACE_EN_IRP0_PRA_R__B_0x1() (p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP0_PRA_R = IRP0_PRA_R_B_0x1)
#define IRP0_PRA_R_B_0x1 0x1    //interlaced


#define Get_DMA_DMA_INTERLACE_EN_IRP1_PRA_W() p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP1_PRA_W
#define Is_DMA_DMA_INTERLACE_EN_IRP1_PRA_W_B_0x0() (p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP1_PRA_W == IRP1_PRA_W_B_0x0)
#define Set_DMA_DMA_INTERLACE_EN_IRP1_PRA_W__B_0x0() (p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP1_PRA_W = IRP1_PRA_W_B_0x0)
#define IRP1_PRA_W_B_0x0 0x0    //not interlaced
#define Is_DMA_DMA_INTERLACE_EN_IRP1_PRA_W_B_0x1() (p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP1_PRA_W == IRP1_PRA_W_B_0x1)
#define Set_DMA_DMA_INTERLACE_EN_IRP1_PRA_W__B_0x1() (p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP1_PRA_W = IRP1_PRA_W_B_0x1)
#define IRP1_PRA_W_B_0x1 0x1    //interlaced


#define Get_DMA_DMA_INTERLACE_EN_IRP2_PRA_W() p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP2_PRA_W
#define Is_DMA_DMA_INTERLACE_EN_IRP2_PRA_W_B_0x0() (p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP2_PRA_W == IRP2_PRA_W_B_0x0)
#define Set_DMA_DMA_INTERLACE_EN_IRP2_PRA_W__B_0x0() (p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP2_PRA_W = IRP2_PRA_W_B_0x0)
#define IRP2_PRA_W_B_0x0 0x0    //not interlaced
#define Is_DMA_DMA_INTERLACE_EN_IRP2_PRA_W_B_0x1() (p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP2_PRA_W == IRP2_PRA_W_B_0x1)
#define Set_DMA_DMA_INTERLACE_EN_IRP2_PRA_W__B_0x1() (p_DMA_IP->DMA_INTERLACE_EN.DMA_INTERLACE_EN_ts.IRP2_PRA_W = IRP2_PRA_W_B_0x1)
#define IRP2_PRA_W_B_0x1 0x1    //interlaced
#define Set_DMA_DMA_INTERLACE_EN(IRP03_L_W,IRP03_C_W,IRP03_C1_W,IRP03_C2_W,IRP03_GCF_L_W,IRP03_GCF_C_W,IRP03_GCF_L_R,IRP03_GCF_C_R,IRP03_PRA_W,IRP0_PRA_R,IRP1_PRA_W,IRP2_PRA_W) (p_DMA_IP->DMA_INTERLACE_EN.word = (uint32_t)IRP03_L_W<<0 | (uint32_t)IRP03_C_W<<1 | (uint32_t)IRP03_C1_W<<2 | (uint32_t)IRP03_C2_W<<3 | (uint32_t)IRP03_GCF_L_W<<4 | (uint32_t)IRP03_GCF_C_W<<5 | (uint32_t)IRP03_GCF_L_R<<6 | (uint32_t)IRP03_GCF_C_R<<7 | (uint32_t)IRP03_PRA_W<<11 | (uint32_t)IRP0_PRA_R<<12 | (uint32_t)IRP1_PRA_W<<13 | (uint32_t)IRP2_PRA_W<<14)
#define Get_DMA_DMA_INTERLACE_EN() p_DMA_IP->DMA_INTERLACE_EN.word
#define Set_DMA_DMA_INTERLACE_EN_word(x) (p_DMA_IP->DMA_INTERLACE_EN.word = x)


//DMA_ITS1


#define Get_DMA_DMA_ITS1_IRP03_L_W() p_DMA_IP->DMA_ITS1.DMA_ITS1_ts.IRP03_L_W
#define Is_DMA_DMA_ITS1_IRP03_L_W_B_0x0() (p_DMA_IP->DMA_ITS1.DMA_ITS1_ts.IRP03_L_W == IRP03_L_W_B_0x0)
#define IRP03_L_W_B_0x0 0x0    //no event ending
#define Is_DMA_DMA_ITS1_IRP03_L_W_B_0x1() (p_DMA_IP->DMA_ITS1.DMA_ITS1_ts.IRP03_L_W == IRP03_L_W_B_0x1)
#define IRP03_L_W_B_0x1 0x1    //event pending


#define Get_DMA_DMA_ITS1_IRP03_C_W() p_DMA_IP->DMA_ITS1.DMA_ITS1_ts.IRP03_C_W
#define Is_DMA_DMA_ITS1_IRP03_C_W_B_0x0() (p_DMA_IP->DMA_ITS1.DMA_ITS1_ts.IRP03_C_W == IRP03_C_W_B_0x0)
#define IRP03_C_W_B_0x0 0x0    //no event ending
#define Is_DMA_DMA_ITS1_IRP03_C_W_B_0x1() (p_DMA_IP->DMA_ITS1.DMA_ITS1_ts.IRP03_C_W == IRP03_C_W_B_0x1)
#define IRP03_C_W_B_0x1 0x1    //event pending


#define Get_DMA_DMA_ITS1_IRP03_C1_W() p_DMA_IP->DMA_ITS1.DMA_ITS1_ts.IRP03_C1_W
#define Is_DMA_DMA_ITS1_IRP03_C1_W_B_0x0() (p_DMA_IP->DMA_ITS1.DMA_ITS1_ts.IRP03_C1_W == IRP03_C1_W_B_0x0)
#define IRP03_C1_W_B_0x0 0x0    //no event ending
#define Is_DMA_DMA_ITS1_IRP03_C1_W_B_0x1() (p_DMA_IP->DMA_ITS1.DMA_ITS1_ts.IRP03_C1_W == IRP03_C1_W_B_0x1)
#define IRP03_C1_W_B_0x1 0x1    //event pending


#define Get_DMA_DMA_ITS1_IRP03_C2_W() p_DMA_IP->DMA_ITS1.DMA_ITS1_ts.IRP03_C2_W
#define Is_DMA_DMA_ITS1_IRP03_C2_W_B_0x0() (p_DMA_IP->DMA_ITS1.DMA_ITS1_ts.IRP03_C2_W == IRP03_C2_W_B_0x0)
#define IRP03_C2_W_B_0x0 0x0    //no event ending
#define Is_DMA_DMA_ITS1_IRP03_C2_W_B_0x1() (p_DMA_IP->DMA_ITS1.DMA_ITS1_ts.IRP03_C2_W == IRP03_C2_W_B_0x1)
#define IRP03_C2_W_B_0x1 0x1    //event pending


#define Get_DMA_DMA_ITS1_IRP03_GCF_L_W() p_DMA_IP->DMA_ITS1.DMA_ITS1_ts.IRP03_GCF_L_W
#define Is_DMA_DMA_ITS1_IRP03_GCF_L_W_B_0x0() (p_DMA_IP->DMA_ITS1.DMA_ITS1_ts.IRP03_GCF_L_W == IRP03_GCF_L_W_B_0x0)
#define IRP03_GCF_L_W_B_0x0 0x0    //no event ending
#define Is_DMA_DMA_ITS1_IRP03_GCF_L_W_B_0x1() (p_DMA_IP->DMA_ITS1.DMA_ITS1_ts.IRP03_GCF_L_W == IRP03_GCF_L_W_B_0x1)
#define IRP03_GCF_L_W_B_0x1 0x1    //event pending


#define Get_DMA_DMA_ITS1_IRP03_GCF_C_W() p_DMA_IP->DMA_ITS1.DMA_ITS1_ts.IRP03_GCF_C_W
#define Is_DMA_DMA_ITS1_IRP03_GCF_C_W_B_0x0() (p_DMA_IP->DMA_ITS1.DMA_ITS1_ts.IRP03_GCF_C_W == IRP03_GCF_C_W_B_0x0)
#define IRP03_GCF_C_W_B_0x0 0x0    //no event ending
#define Is_DMA_DMA_ITS1_IRP03_GCF_C_W_B_0x1() (p_DMA_IP->DMA_ITS1.DMA_ITS1_ts.IRP03_GCF_C_W == IRP03_GCF_C_W_B_0x1)
#define IRP03_GCF_C_W_B_0x1 0x1    //event pending


#define Get_DMA_DMA_ITS1_IRP03_GCF_L_R() p_DMA_IP->DMA_ITS1.DMA_ITS1_ts.IRP03_GCF_L_R
#define Is_DMA_DMA_ITS1_IRP03_GCF_L_R_B_0x0() (p_DMA_IP->DMA_ITS1.DMA_ITS1_ts.IRP03_GCF_L_R == IRP03_GCF_L_R_B_0x0)
#define IRP03_GCF_L_R_B_0x0 0x0    //no event ending
#define Is_DMA_DMA_ITS1_IRP03_GCF_L_R_B_0x1() (p_DMA_IP->DMA_ITS1.DMA_ITS1_ts.IRP03_GCF_L_R == IRP03_GCF_L_R_B_0x1)
#define IRP03_GCF_L_R_B_0x1 0x1    //event pending


#define Get_DMA_DMA_ITS1_IRP03_GCF_C_R() p_DMA_IP->DMA_ITS1.DMA_ITS1_ts.IRP03_GCF_C_R
#define Is_DMA_DMA_ITS1_IRP03_GCF_C_R_B_0x0() (p_DMA_IP->DMA_ITS1.DMA_ITS1_ts.IRP03_GCF_C_R == IRP03_GCF_C_R_B_0x0)
#define IRP03_GCF_C_R_B_0x0 0x0    //no event ending
#define Is_DMA_DMA_ITS1_IRP03_GCF_C_R_B_0x1() (p_DMA_IP->DMA_ITS1.DMA_ITS1_ts.IRP03_GCF_C_R == IRP03_GCF_C_R_B_0x1)
#define IRP03_GCF_C_R_B_0x1 0x1    //event pending


#define Get_DMA_DMA_ITS1_IRP03_VCP_W() p_DMA_IP->DMA_ITS1.DMA_ITS1_ts.IRP03_VCP_W
#define Is_DMA_DMA_ITS1_IRP03_VCP_W_B_0x0() (p_DMA_IP->DMA_ITS1.DMA_ITS1_ts.IRP03_VCP_W == IRP03_VCP_W_B_0x0)
#define IRP03_VCP_W_B_0x0 0x0    //no event ending
#define Is_DMA_DMA_ITS1_IRP03_VCP_W_B_0x1() (p_DMA_IP->DMA_ITS1.DMA_ITS1_ts.IRP03_VCP_W == IRP03_VCP_W_B_0x1)
#define IRP03_VCP_W_B_0x1 0x1    //event pending


#define Get_DMA_DMA_ITS1_IRP03_HCP_W() p_DMA_IP->DMA_ITS1.DMA_ITS1_ts.IRP03_HCP_W
#define Is_DMA_DMA_ITS1_IRP03_HCP_W_B_0x0() (p_DMA_IP->DMA_ITS1.DMA_ITS1_ts.IRP03_HCP_W == IRP03_HCP_W_B_0x0)
#define IRP03_HCP_W_B_0x0 0x0    //no event ending
#define Is_DMA_DMA_ITS1_IRP03_HCP_W_B_0x1() (p_DMA_IP->DMA_ITS1.DMA_ITS1_ts.IRP03_HCP_W == IRP03_HCP_W_B_0x1)
#define IRP03_HCP_W_B_0x1 0x1    //event pending


#define Get_DMA_DMA_ITS1_IRP03_HCP_R() p_DMA_IP->DMA_ITS1.DMA_ITS1_ts.IRP03_HCP_R
#define Is_DMA_DMA_ITS1_IRP03_HCP_R_B_0x0() (p_DMA_IP->DMA_ITS1.DMA_ITS1_ts.IRP03_HCP_R == IRP03_HCP_R_B_0x0)
#define IRP03_HCP_R_B_0x0 0x0    //no event ending
#define Is_DMA_DMA_ITS1_IRP03_HCP_R_B_0x1() (p_DMA_IP->DMA_ITS1.DMA_ITS1_ts.IRP03_HCP_R == IRP03_HCP_R_B_0x1)
#define IRP03_HCP_R_B_0x1 0x1    //event pending


#define Get_DMA_DMA_ITS1_IRP03_PRA_W() p_DMA_IP->DMA_ITS1.DMA_ITS1_ts.IRP03_PRA_W
#define Is_DMA_DMA_ITS1_IRP03_PRA_W_B_0x0() (p_DMA_IP->DMA_ITS1.DMA_ITS1_ts.IRP03_PRA_W == IRP03_PRA_W_B_0x0)
#define IRP03_PRA_W_B_0x0 0x0    //no event ending
#define Is_DMA_DMA_ITS1_IRP03_PRA_W_B_0x1() (p_DMA_IP->DMA_ITS1.DMA_ITS1_ts.IRP03_PRA_W == IRP03_PRA_W_B_0x1)
#define IRP03_PRA_W_B_0x1 0x1    //event pending


#define Get_DMA_DMA_ITS1_IRP0_PRA_R() p_DMA_IP->DMA_ITS1.DMA_ITS1_ts.IRP0_PRA_R
#define Is_DMA_DMA_ITS1_IRP0_PRA_R_B_0x0() (p_DMA_IP->DMA_ITS1.DMA_ITS1_ts.IRP0_PRA_R == IRP0_PRA_R_B_0x0)
#define IRP0_PRA_R_B_0x0 0x0    //no event ending
#define Is_DMA_DMA_ITS1_IRP0_PRA_R_B_0x1() (p_DMA_IP->DMA_ITS1.DMA_ITS1_ts.IRP0_PRA_R == IRP0_PRA_R_B_0x1)
#define IRP0_PRA_R_B_0x1 0x1    //event pending


#define Get_DMA_DMA_ITS1_IRP1_PRA_W() p_DMA_IP->DMA_ITS1.DMA_ITS1_ts.IRP1_PRA_W
#define Is_DMA_DMA_ITS1_IRP1_PRA_W_B_0x0() (p_DMA_IP->DMA_ITS1.DMA_ITS1_ts.IRP1_PRA_W == IRP1_PRA_W_B_0x0)
#define IRP1_PRA_W_B_0x0 0x0    //no event ending
#define Is_DMA_DMA_ITS1_IRP1_PRA_W_B_0x1() (p_DMA_IP->DMA_ITS1.DMA_ITS1_ts.IRP1_PRA_W == IRP1_PRA_W_B_0x1)
#define IRP1_PRA_W_B_0x1 0x1    //event pending


#define Get_DMA_DMA_ITS1_IRP2_PRA_W() p_DMA_IP->DMA_ITS1.DMA_ITS1_ts.IRP2_PRA_W
#define Is_DMA_DMA_ITS1_IRP2_PRA_W_B_0x0() (p_DMA_IP->DMA_ITS1.DMA_ITS1_ts.IRP2_PRA_W == IRP2_PRA_W_B_0x0)
#define IRP2_PRA_W_B_0x0 0x0    //no event ending
#define Is_DMA_DMA_ITS1_IRP2_PRA_W_B_0x1() (p_DMA_IP->DMA_ITS1.DMA_ITS1_ts.IRP2_PRA_W == IRP2_PRA_W_B_0x1)
#define IRP2_PRA_W_B_0x1 0x1    //event pending


#define Get_DMA_DMA_ITS1_IRP3_JPG_W() p_DMA_IP->DMA_ITS1.DMA_ITS1_ts.IRP3_JPG_W
#define Is_DMA_DMA_ITS1_IRP3_JPG_W_B_0x0() (p_DMA_IP->DMA_ITS1.DMA_ITS1_ts.IRP3_JPG_W == IRP3_JPG_W_B_0x0)
#define IRP3_JPG_W_B_0x0 0x0    //no event ending
#define Is_DMA_DMA_ITS1_IRP3_JPG_W_B_0x1() (p_DMA_IP->DMA_ITS1.DMA_ITS1_ts.IRP3_JPG_W == IRP3_JPG_W_B_0x1)
#define IRP3_JPG_W_B_0x1 0x1    //event pending
#define Get_DMA_DMA_ITS1() p_DMA_IP->DMA_ITS1.word


//DMA_ITS1_BCLR


#define Set_DMA_DMA_ITS1_BCLR_IRP03_L_W(x) (p_DMA_IP->DMA_ITS1_BCLR.DMA_ITS1_BCLR_ts.IRP03_L_W = x)


#define Set_DMA_DMA_ITS1_BCLR_IRP03_C_W(x) (p_DMA_IP->DMA_ITS1_BCLR.DMA_ITS1_BCLR_ts.IRP03_C_W = x)


#define Set_DMA_DMA_ITS1_BCLR_IRP03_C1_W(x) (p_DMA_IP->DMA_ITS1_BCLR.DMA_ITS1_BCLR_ts.IRP03_C1_W = x)


#define Set_DMA_DMA_ITS1_BCLR_IRP03_C2_W(x) (p_DMA_IP->DMA_ITS1_BCLR.DMA_ITS1_BCLR_ts.IRP03_C2_W = x)


#define Set_DMA_DMA_ITS1_BCLR_IRP03_GCF_L_W(x) (p_DMA_IP->DMA_ITS1_BCLR.DMA_ITS1_BCLR_ts.IRP03_GCF_L_W = x)


#define Set_DMA_DMA_ITS1_BCLR_IRP03_GCF_C_W(x) (p_DMA_IP->DMA_ITS1_BCLR.DMA_ITS1_BCLR_ts.IRP03_GCF_C_W = x)


#define Set_DMA_DMA_ITS1_BCLR_IRP03_GCF_L_R(x) (p_DMA_IP->DMA_ITS1_BCLR.DMA_ITS1_BCLR_ts.IRP03_GCF_L_R = x)


#define Set_DMA_DMA_ITS1_BCLR_IRP03_GCF_C_R(x) (p_DMA_IP->DMA_ITS1_BCLR.DMA_ITS1_BCLR_ts.IRP03_GCF_C_R = x)


#define Set_DMA_DMA_ITS1_BCLR_IRP03_VCP_W(x) (p_DMA_IP->DMA_ITS1_BCLR.DMA_ITS1_BCLR_ts.IRP03_VCP_W = x)


#define Set_DMA_DMA_ITS1_BCLR_IRP03_HCP_W(x) (p_DMA_IP->DMA_ITS1_BCLR.DMA_ITS1_BCLR_ts.IRP03_HCP_W = x)


#define Set_DMA_DMA_ITS1_BCLR_IRP03_HCP_R(x) (p_DMA_IP->DMA_ITS1_BCLR.DMA_ITS1_BCLR_ts.IRP03_HCP_R = x)


#define Set_DMA_DMA_ITS1_BCLR_IRP03_PRA_W(x) (p_DMA_IP->DMA_ITS1_BCLR.DMA_ITS1_BCLR_ts.IRP03_PRA_W = x)


#define Set_DMA_DMA_ITS1_BCLR_IRP0_PRA_R(x) (p_DMA_IP->DMA_ITS1_BCLR.DMA_ITS1_BCLR_ts.IRP0_PRA_R = x)


#define Set_DMA_DMA_ITS1_BCLR_IRP1_PRA_W(x) (p_DMA_IP->DMA_ITS1_BCLR.DMA_ITS1_BCLR_ts.IRP1_PRA_W = x)


#define Set_DMA_DMA_ITS1_BCLR_IRP2_PRA_W(x) (p_DMA_IP->DMA_ITS1_BCLR.DMA_ITS1_BCLR_ts.IRP2_PRA_W = x)


#define Set_DMA_DMA_ITS1_BCLR_IRP3_JPG_W(x) (p_DMA_IP->DMA_ITS1_BCLR.DMA_ITS1_BCLR_ts.IRP3_JPG_W = x)
#define Set_DMA_DMA_ITS1_BCLR(IRP03_L_W,IRP03_C_W,IRP03_C1_W,IRP03_C2_W,IRP03_GCF_L_W,IRP03_GCF_C_W,IRP03_GCF_L_R,IRP03_GCF_C_R,IRP03_VCP_W,IRP03_HCP_W,IRP03_HCP_R,IRP03_PRA_W,IRP0_PRA_R,IRP1_PRA_W,IRP2_PRA_W,IRP3_JPG_W) (p_DMA_IP->DMA_ITS1_BCLR.word = (uint32_t)IRP03_L_W<<0 | (uint32_t)IRP03_C_W<<1 | (uint32_t)IRP03_C1_W<<2 | (uint32_t)IRP03_C2_W<<3 | (uint32_t)IRP03_GCF_L_W<<4 | (uint32_t)IRP03_GCF_C_W<<5 | (uint32_t)IRP03_GCF_L_R<<6 | (uint32_t)IRP03_GCF_C_R<<7 | (uint32_t)IRP03_VCP_W<<8 | (uint32_t)IRP03_HCP_W<<9 | (uint32_t)IRP03_HCP_R<<10 | (uint32_t)IRP03_PRA_W<<11 | (uint32_t)IRP0_PRA_R<<12 | (uint32_t)IRP1_PRA_W<<13 | (uint32_t)IRP2_PRA_W<<14 | (uint32_t)IRP3_JPG_W<<15)
#define Set_DMA_DMA_ITS1_BCLR_word(x) (p_DMA_IP->DMA_ITS1_BCLR.word = x)


//DMA_IMR1


#define Get_DMA_DMA_IMR1_IRP03_L_W() p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_L_W
#define Is_DMA_DMA_IMR1_IRP03_L_W_B_0x0() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_L_W == IRP03_L_W_B_0x0)
#define Set_DMA_DMA_IMR1_IRP03_L_W__B_0x0() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_L_W = IRP03_L_W_B_0x0)
#define IRP03_L_W_B_0x0 0x0    //event disabled
#define Is_DMA_DMA_IMR1_IRP03_L_W_B_0x1() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_L_W == IRP03_L_W_B_0x1)
#define Set_DMA_DMA_IMR1_IRP03_L_W__B_0x1() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_L_W = IRP03_L_W_B_0x1)
#define IRP03_L_W_B_0x1 0x1    //event enabled


#define Get_DMA_DMA_IMR1_IRP03_C_W() p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_C_W
#define Is_DMA_DMA_IMR1_IRP03_C_W_B_0x0() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_C_W == IRP03_C_W_B_0x0)
#define Set_DMA_DMA_IMR1_IRP03_C_W__B_0x0() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_C_W = IRP03_C_W_B_0x0)
#define IRP03_C_W_B_0x0 0x0    //event disabled
#define Is_DMA_DMA_IMR1_IRP03_C_W_B_0x1() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_C_W == IRP03_C_W_B_0x1)
#define Set_DMA_DMA_IMR1_IRP03_C_W__B_0x1() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_C_W = IRP03_C_W_B_0x1)
#define IRP03_C_W_B_0x1 0x1    //event enabled


#define Get_DMA_DMA_IMR1_IRP03_C1_W() p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_C1_W
#define Is_DMA_DMA_IMR1_IRP03_C1_W_B_0x0() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_C1_W == IRP03_C1_W_B_0x0)
#define Set_DMA_DMA_IMR1_IRP03_C1_W__B_0x0() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_C1_W = IRP03_C1_W_B_0x0)
#define IRP03_C1_W_B_0x0 0x0    //event disabled
#define Is_DMA_DMA_IMR1_IRP03_C1_W_B_0x1() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_C1_W == IRP03_C1_W_B_0x1)
#define Set_DMA_DMA_IMR1_IRP03_C1_W__B_0x1() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_C1_W = IRP03_C1_W_B_0x1)
#define IRP03_C1_W_B_0x1 0x1    //event enabled


#define Get_DMA_DMA_IMR1_IRP03_C2_W() p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_C2_W
#define Is_DMA_DMA_IMR1_IRP03_C2_W_B_0x0() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_C2_W == IRP03_C2_W_B_0x0)
#define Set_DMA_DMA_IMR1_IRP03_C2_W__B_0x0() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_C2_W = IRP03_C2_W_B_0x0)
#define IRP03_C2_W_B_0x0 0x0    //event disabled
#define Is_DMA_DMA_IMR1_IRP03_C2_W_B_0x1() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_C2_W == IRP03_C2_W_B_0x1)
#define Set_DMA_DMA_IMR1_IRP03_C2_W__B_0x1() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_C2_W = IRP03_C2_W_B_0x1)
#define IRP03_C2_W_B_0x1 0x1    //event enabled


#define Get_DMA_DMA_IMR1_IRP03_GCF_L_W() p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_GCF_L_W
#define Is_DMA_DMA_IMR1_IRP03_GCF_L_W_B_0x0() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_GCF_L_W == IRP03_GCF_L_W_B_0x0)
#define Set_DMA_DMA_IMR1_IRP03_GCF_L_W__B_0x0() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_GCF_L_W = IRP03_GCF_L_W_B_0x0)
#define IRP03_GCF_L_W_B_0x0 0x0    //event disabled
#define Is_DMA_DMA_IMR1_IRP03_GCF_L_W_B_0x1() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_GCF_L_W == IRP03_GCF_L_W_B_0x1)
#define Set_DMA_DMA_IMR1_IRP03_GCF_L_W__B_0x1() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_GCF_L_W = IRP03_GCF_L_W_B_0x1)
#define IRP03_GCF_L_W_B_0x1 0x1    //event enabled


#define Get_DMA_DMA_IMR1_IRP03_GCF_C_W() p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_GCF_C_W
#define Is_DMA_DMA_IMR1_IRP03_GCF_C_W_B_0x0() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_GCF_C_W == IRP03_GCF_C_W_B_0x0)
#define Set_DMA_DMA_IMR1_IRP03_GCF_C_W__B_0x0() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_GCF_C_W = IRP03_GCF_C_W_B_0x0)
#define IRP03_GCF_C_W_B_0x0 0x0    //event disabled
#define Is_DMA_DMA_IMR1_IRP03_GCF_C_W_B_0x1() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_GCF_C_W == IRP03_GCF_C_W_B_0x1)
#define Set_DMA_DMA_IMR1_IRP03_GCF_C_W__B_0x1() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_GCF_C_W = IRP03_GCF_C_W_B_0x1)
#define IRP03_GCF_C_W_B_0x1 0x1    //event enabled


#define Get_DMA_DMA_IMR1_IRP03_GCF_L_R() p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_GCF_L_R
#define Is_DMA_DMA_IMR1_IRP03_GCF_L_R_B_0x0() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_GCF_L_R == IRP03_GCF_L_R_B_0x0)
#define Set_DMA_DMA_IMR1_IRP03_GCF_L_R__B_0x0() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_GCF_L_R = IRP03_GCF_L_R_B_0x0)
#define IRP03_GCF_L_R_B_0x0 0x0    //event disabled
#define Is_DMA_DMA_IMR1_IRP03_GCF_L_R_B_0x1() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_GCF_L_R == IRP03_GCF_L_R_B_0x1)
#define Set_DMA_DMA_IMR1_IRP03_GCF_L_R__B_0x1() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_GCF_L_R = IRP03_GCF_L_R_B_0x1)
#define IRP03_GCF_L_R_B_0x1 0x1    //event enabled


#define Get_DMA_DMA_IMR1_IRP03_GCF_C_R() p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_GCF_C_R
#define Is_DMA_DMA_IMR1_IRP03_GCF_C_R_B_0x0() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_GCF_C_R == IRP03_GCF_C_R_B_0x0)
#define Set_DMA_DMA_IMR1_IRP03_GCF_C_R__B_0x0() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_GCF_C_R = IRP03_GCF_C_R_B_0x0)
#define IRP03_GCF_C_R_B_0x0 0x0    //event disabled
#define Is_DMA_DMA_IMR1_IRP03_GCF_C_R_B_0x1() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_GCF_C_R == IRP03_GCF_C_R_B_0x1)
#define Set_DMA_DMA_IMR1_IRP03_GCF_C_R__B_0x1() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_GCF_C_R = IRP03_GCF_C_R_B_0x1)
#define IRP03_GCF_C_R_B_0x1 0x1    //event enabled


#define Get_DMA_DMA_IMR1_IRP03_VCP_W() p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_VCP_W
#define Is_DMA_DMA_IMR1_IRP03_VCP_W_B_0x0() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_VCP_W == IRP03_VCP_W_B_0x0)
#define Set_DMA_DMA_IMR1_IRP03_VCP_W__B_0x0() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_VCP_W = IRP03_VCP_W_B_0x0)
#define IRP03_VCP_W_B_0x0 0x0    //event disabled
#define Is_DMA_DMA_IMR1_IRP03_VCP_W_B_0x1() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_VCP_W == IRP03_VCP_W_B_0x1)
#define Set_DMA_DMA_IMR1_IRP03_VCP_W__B_0x1() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_VCP_W = IRP03_VCP_W_B_0x1)
#define IRP03_VCP_W_B_0x1 0x1    //event enabled


#define Get_DMA_DMA_IMR1_IRP03_HCP_W() p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_HCP_W
#define Is_DMA_DMA_IMR1_IRP03_HCP_W_B_0x0() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_HCP_W == IRP03_HCP_W_B_0x0)
#define Set_DMA_DMA_IMR1_IRP03_HCP_W__B_0x0() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_HCP_W = IRP03_HCP_W_B_0x0)
#define IRP03_HCP_W_B_0x0 0x0    //event disabled
#define Is_DMA_DMA_IMR1_IRP03_HCP_W_B_0x1() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_HCP_W == IRP03_HCP_W_B_0x1)
#define Set_DMA_DMA_IMR1_IRP03_HCP_W__B_0x1() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_HCP_W = IRP03_HCP_W_B_0x1)
#define IRP03_HCP_W_B_0x1 0x1    //event enabled


#define Get_DMA_DMA_IMR1_IRP03_HCP_R() p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_HCP_R
#define Is_DMA_DMA_IMR1_IRP03_HCP_R_B_0x0() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_HCP_R == IRP03_HCP_R_B_0x0)
#define Set_DMA_DMA_IMR1_IRP03_HCP_R__B_0x0() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_HCP_R = IRP03_HCP_R_B_0x0)
#define IRP03_HCP_R_B_0x0 0x0    //event disabled
#define Is_DMA_DMA_IMR1_IRP03_HCP_R_B_0x1() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_HCP_R == IRP03_HCP_R_B_0x1)
#define Set_DMA_DMA_IMR1_IRP03_HCP_R__B_0x1() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_HCP_R = IRP03_HCP_R_B_0x1)
#define IRP03_HCP_R_B_0x1 0x1    //event enabled


#define Get_DMA_DMA_IMR1_IRP03_PRA_W() p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_PRA_W
#define Is_DMA_DMA_IMR1_IRP03_PRA_W_B_0x0() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_PRA_W == IRP03_PRA_W_B_0x0)
#define Set_DMA_DMA_IMR1_IRP03_PRA_W__B_0x0() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_PRA_W = IRP03_PRA_W_B_0x0)
#define IRP03_PRA_W_B_0x0 0x0    //event disabled
#define Is_DMA_DMA_IMR1_IRP03_PRA_W_B_0x1() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_PRA_W == IRP03_PRA_W_B_0x1)
#define Set_DMA_DMA_IMR1_IRP03_PRA_W__B_0x1() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP03_PRA_W = IRP03_PRA_W_B_0x1)
#define IRP03_PRA_W_B_0x1 0x1    //event enabled


#define Get_DMA_DMA_IMR1_IRP0_PRA_R() p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP0_PRA_R
#define Is_DMA_DMA_IMR1_IRP0_PRA_R_B_0x0() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP0_PRA_R == IRP0_PRA_R_B_0x0)
#define Set_DMA_DMA_IMR1_IRP0_PRA_R__B_0x0() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP0_PRA_R = IRP0_PRA_R_B_0x0)
#define IRP0_PRA_R_B_0x0 0x0    //event disabled
#define Is_DMA_DMA_IMR1_IRP0_PRA_R_B_0x1() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP0_PRA_R == IRP0_PRA_R_B_0x1)
#define Set_DMA_DMA_IMR1_IRP0_PRA_R__B_0x1() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP0_PRA_R = IRP0_PRA_R_B_0x1)
#define IRP0_PRA_R_B_0x1 0x1    //event enabled


#define Get_DMA_DMA_IMR1_IRP1_PRA_W() p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP1_PRA_W
#define Is_DMA_DMA_IMR1_IRP1_PRA_W_B_0x0() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP1_PRA_W == IRP1_PRA_W_B_0x0)
#define Set_DMA_DMA_IMR1_IRP1_PRA_W__B_0x0() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP1_PRA_W = IRP1_PRA_W_B_0x0)
#define IRP1_PRA_W_B_0x0 0x0    //event disabled
#define Is_DMA_DMA_IMR1_IRP1_PRA_W_B_0x1() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP1_PRA_W == IRP1_PRA_W_B_0x1)
#define Set_DMA_DMA_IMR1_IRP1_PRA_W__B_0x1() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP1_PRA_W = IRP1_PRA_W_B_0x1)
#define IRP1_PRA_W_B_0x1 0x1    //event enabled


#define Get_DMA_DMA_IMR1_IRP2_PRA_W() p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP2_PRA_W
#define Is_DMA_DMA_IMR1_IRP2_PRA_W_B_0x0() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP2_PRA_W == IRP2_PRA_W_B_0x0)
#define Set_DMA_DMA_IMR1_IRP2_PRA_W__B_0x0() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP2_PRA_W = IRP2_PRA_W_B_0x0)
#define IRP2_PRA_W_B_0x0 0x0    //event disabled
#define Is_DMA_DMA_IMR1_IRP2_PRA_W_B_0x1() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP2_PRA_W == IRP2_PRA_W_B_0x1)
#define Set_DMA_DMA_IMR1_IRP2_PRA_W__B_0x1() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP2_PRA_W = IRP2_PRA_W_B_0x1)
#define IRP2_PRA_W_B_0x1 0x1    //event enabled


#define Get_DMA_DMA_IMR1_IRP3_JPG_W() p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP3_JPG_W
#define Is_DMA_DMA_IMR1_IRP3_JPG_W_B_0x0() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP3_JPG_W == IRP3_JPG_W_B_0x0)
#define Set_DMA_DMA_IMR1_IRP3_JPG_W__B_0x0() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP3_JPG_W = IRP3_JPG_W_B_0x0)
#define IRP3_JPG_W_B_0x0 0x0    //event disabled
#define Is_DMA_DMA_IMR1_IRP3_JPG_W_B_0x1() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP3_JPG_W == IRP3_JPG_W_B_0x1)
#define Set_DMA_DMA_IMR1_IRP3_JPG_W__B_0x1() (p_DMA_IP->DMA_IMR1.DMA_IMR1_ts.IRP3_JPG_W = IRP3_JPG_W_B_0x1)
#define IRP3_JPG_W_B_0x1 0x1    //event enabled
#define Set_DMA_DMA_IMR1(IRP03_L_W,IRP03_C_W,IRP03_C1_W,IRP03_C2_W,IRP03_GCF_L_W,IRP03_GCF_C_W,IRP03_GCF_L_R,IRP03_GCF_C_R,IRP03_VCP_W,IRP03_HCP_W,IRP03_HCP_R,IRP03_PRA_W,IRP0_PRA_R,IRP1_PRA_W,IRP2_PRA_W,IRP3_JPG_W) (p_DMA_IP->DMA_IMR1.word = (uint32_t)IRP03_L_W<<0 | (uint32_t)IRP03_C_W<<1 | (uint32_t)IRP03_C1_W<<2 | (uint32_t)IRP03_C2_W<<3 | (uint32_t)IRP03_GCF_L_W<<4 | (uint32_t)IRP03_GCF_C_W<<5 | (uint32_t)IRP03_GCF_L_R<<6 | (uint32_t)IRP03_GCF_C_R<<7 | (uint32_t)IRP03_VCP_W<<8 | (uint32_t)IRP03_HCP_W<<9 | (uint32_t)IRP03_HCP_R<<10 | (uint32_t)IRP03_PRA_W<<11 | (uint32_t)IRP0_PRA_R<<12 | (uint32_t)IRP1_PRA_W<<13 | (uint32_t)IRP2_PRA_W<<14 | (uint32_t)IRP3_JPG_W<<15)
#define Get_DMA_DMA_IMR1() p_DMA_IP->DMA_IMR1.word
#define Set_DMA_DMA_IMR1_word(x) (p_DMA_IP->DMA_IMR1.word = x)


//DMA_ITS2


#define Get_DMA_DMA_ITS2_IRP03_L_W() p_DMA_IP->DMA_ITS2.DMA_ITS2_ts.IRP03_L_W
#define Is_DMA_DMA_ITS2_IRP03_L_W_B_0x0() (p_DMA_IP->DMA_ITS2.DMA_ITS2_ts.IRP03_L_W == IRP03_L_W_B_0x0)
#define IRP03_L_W_B_0x0 0x0    //no event ending
#define Is_DMA_DMA_ITS2_IRP03_L_W_B_0x1() (p_DMA_IP->DMA_ITS2.DMA_ITS2_ts.IRP03_L_W == IRP03_L_W_B_0x1)
#define IRP03_L_W_B_0x1 0x1    //event pending


#define Get_DMA_DMA_ITS2_IRP03_C_W() p_DMA_IP->DMA_ITS2.DMA_ITS2_ts.IRP03_C_W
#define Is_DMA_DMA_ITS2_IRP03_C_W_B_0x0() (p_DMA_IP->DMA_ITS2.DMA_ITS2_ts.IRP03_C_W == IRP03_C_W_B_0x0)
#define IRP03_C_W_B_0x0 0x0    //no event ending
#define Is_DMA_DMA_ITS2_IRP03_C_W_B_0x1() (p_DMA_IP->DMA_ITS2.DMA_ITS2_ts.IRP03_C_W == IRP03_C_W_B_0x1)
#define IRP03_C_W_B_0x1 0x1    //event pending


#define Get_DMA_DMA_ITS2_IRP03_C1_W() p_DMA_IP->DMA_ITS2.DMA_ITS2_ts.IRP03_C1_W
#define Is_DMA_DMA_ITS2_IRP03_C1_W_B_0x0() (p_DMA_IP->DMA_ITS2.DMA_ITS2_ts.IRP03_C1_W == IRP03_C1_W_B_0x0)
#define IRP03_C1_W_B_0x0 0x0    //no event ending
#define Is_DMA_DMA_ITS2_IRP03_C1_W_B_0x1() (p_DMA_IP->DMA_ITS2.DMA_ITS2_ts.IRP03_C1_W == IRP03_C1_W_B_0x1)
#define IRP03_C1_W_B_0x1 0x1    //event pending


#define Get_DMA_DMA_ITS2_IRP03_C2_W() p_DMA_IP->DMA_ITS2.DMA_ITS2_ts.IRP03_C2_W
#define Is_DMA_DMA_ITS2_IRP03_C2_W_B_0x0() (p_DMA_IP->DMA_ITS2.DMA_ITS2_ts.IRP03_C2_W == IRP03_C2_W_B_0x0)
#define IRP03_C2_W_B_0x0 0x0    //no event ending
#define Is_DMA_DMA_ITS2_IRP03_C2_W_B_0x1() (p_DMA_IP->DMA_ITS2.DMA_ITS2_ts.IRP03_C2_W == IRP03_C2_W_B_0x1)
#define IRP03_C2_W_B_0x1 0x1    //event pending


#define Get_DMA_DMA_ITS2_IRP03_GCF_L_W() p_DMA_IP->DMA_ITS2.DMA_ITS2_ts.IRP03_GCF_L_W
#define Is_DMA_DMA_ITS2_IRP03_GCF_L_W_B_0x0() (p_DMA_IP->DMA_ITS2.DMA_ITS2_ts.IRP03_GCF_L_W == IRP03_GCF_L_W_B_0x0)
#define IRP03_GCF_L_W_B_0x0 0x0    //no event ending
#define Is_DMA_DMA_ITS2_IRP03_GCF_L_W_B_0x1() (p_DMA_IP->DMA_ITS2.DMA_ITS2_ts.IRP03_GCF_L_W == IRP03_GCF_L_W_B_0x1)
#define IRP03_GCF_L_W_B_0x1 0x1    //event pending


#define Get_DMA_DMA_ITS2_IRP03_GCF_C_W() p_DMA_IP->DMA_ITS2.DMA_ITS2_ts.IRP03_GCF_C_W
#define Is_DMA_DMA_ITS2_IRP03_GCF_C_W_B_0x0() (p_DMA_IP->DMA_ITS2.DMA_ITS2_ts.IRP03_GCF_C_W == IRP03_GCF_C_W_B_0x0)
#define IRP03_GCF_C_W_B_0x0 0x0    //no event ending
#define Is_DMA_DMA_ITS2_IRP03_GCF_C_W_B_0x1() (p_DMA_IP->DMA_ITS2.DMA_ITS2_ts.IRP03_GCF_C_W == IRP03_GCF_C_W_B_0x1)
#define IRP03_GCF_C_W_B_0x1 0x1    //event pending


#define Get_DMA_DMA_ITS2_IRP03_GCF_L_R() p_DMA_IP->DMA_ITS2.DMA_ITS2_ts.IRP03_GCF_L_R
#define Is_DMA_DMA_ITS2_IRP03_GCF_L_R_B_0x0() (p_DMA_IP->DMA_ITS2.DMA_ITS2_ts.IRP03_GCF_L_R == IRP03_GCF_L_R_B_0x0)
#define IRP03_GCF_L_R_B_0x0 0x0    //no event ending
#define Is_DMA_DMA_ITS2_IRP03_GCF_L_R_B_0x1() (p_DMA_IP->DMA_ITS2.DMA_ITS2_ts.IRP03_GCF_L_R == IRP03_GCF_L_R_B_0x1)
#define IRP03_GCF_L_R_B_0x1 0x1    //event pending


#define Get_DMA_DMA_ITS2_IRP03_GCF_C_R() p_DMA_IP->DMA_ITS2.DMA_ITS2_ts.IRP03_GCF_C_R
#define Is_DMA_DMA_ITS2_IRP03_GCF_C_R_B_0x0() (p_DMA_IP->DMA_ITS2.DMA_ITS2_ts.IRP03_GCF_C_R == IRP03_GCF_C_R_B_0x0)
#define IRP03_GCF_C_R_B_0x0 0x0    //no event ending
#define Is_DMA_DMA_ITS2_IRP03_GCF_C_R_B_0x1() (p_DMA_IP->DMA_ITS2.DMA_ITS2_ts.IRP03_GCF_C_R == IRP03_GCF_C_R_B_0x1)
#define IRP03_GCF_C_R_B_0x1 0x1    //event pending


#define Get_DMA_DMA_ITS2_IRP03_VCP_W() p_DMA_IP->DMA_ITS2.DMA_ITS2_ts.IRP03_VCP_W
#define Is_DMA_DMA_ITS2_IRP03_VCP_W_B_0x0() (p_DMA_IP->DMA_ITS2.DMA_ITS2_ts.IRP03_VCP_W == IRP03_VCP_W_B_0x0)
#define IRP03_VCP_W_B_0x0 0x0    //no event ending
#define Is_DMA_DMA_ITS2_IRP03_VCP_W_B_0x1() (p_DMA_IP->DMA_ITS2.DMA_ITS2_ts.IRP03_VCP_W == IRP03_VCP_W_B_0x1)
#define IRP03_VCP_W_B_0x1 0x1    //event pending


#define Get_DMA_DMA_ITS2_IRP03_HCP_W() p_DMA_IP->DMA_ITS2.DMA_ITS2_ts.IRP03_HCP_W
#define Is_DMA_DMA_ITS2_IRP03_HCP_W_B_0x0() (p_DMA_IP->DMA_ITS2.DMA_ITS2_ts.IRP03_HCP_W == IRP03_HCP_W_B_0x0)
#define IRP03_HCP_W_B_0x0 0x0    //no event ending
#define Is_DMA_DMA_ITS2_IRP03_HCP_W_B_0x1() (p_DMA_IP->DMA_ITS2.DMA_ITS2_ts.IRP03_HCP_W == IRP03_HCP_W_B_0x1)
#define IRP03_HCP_W_B_0x1 0x1    //event pending


#define Get_DMA_DMA_ITS2_IRP03_HCP_R() p_DMA_IP->DMA_ITS2.DMA_ITS2_ts.IRP03_HCP_R
#define Is_DMA_DMA_ITS2_IRP03_HCP_R_B_0x0() (p_DMA_IP->DMA_ITS2.DMA_ITS2_ts.IRP03_HCP_R == IRP03_HCP_R_B_0x0)
#define IRP03_HCP_R_B_0x0 0x0    //no event ending
#define Is_DMA_DMA_ITS2_IRP03_HCP_R_B_0x1() (p_DMA_IP->DMA_ITS2.DMA_ITS2_ts.IRP03_HCP_R == IRP03_HCP_R_B_0x1)
#define IRP03_HCP_R_B_0x1 0x1    //event pending


#define Get_DMA_DMA_ITS2_IRP03_PRA_W() p_DMA_IP->DMA_ITS2.DMA_ITS2_ts.IRP03_PRA_W
#define Is_DMA_DMA_ITS2_IRP03_PRA_W_B_0x0() (p_DMA_IP->DMA_ITS2.DMA_ITS2_ts.IRP03_PRA_W == IRP03_PRA_W_B_0x0)
#define IRP03_PRA_W_B_0x0 0x0    //no event ending
#define Is_DMA_DMA_ITS2_IRP03_PRA_W_B_0x1() (p_DMA_IP->DMA_ITS2.DMA_ITS2_ts.IRP03_PRA_W == IRP03_PRA_W_B_0x1)
#define IRP03_PRA_W_B_0x1 0x1    //event pending


#define Get_DMA_DMA_ITS2_IRP0_PRA_R() p_DMA_IP->DMA_ITS2.DMA_ITS2_ts.IRP0_PRA_R
#define Is_DMA_DMA_ITS2_IRP0_PRA_R_B_0x0() (p_DMA_IP->DMA_ITS2.DMA_ITS2_ts.IRP0_PRA_R == IRP0_PRA_R_B_0x0)
#define IRP0_PRA_R_B_0x0 0x0    //no event ending
#define Is_DMA_DMA_ITS2_IRP0_PRA_R_B_0x1() (p_DMA_IP->DMA_ITS2.DMA_ITS2_ts.IRP0_PRA_R == IRP0_PRA_R_B_0x1)
#define IRP0_PRA_R_B_0x1 0x1    //event pending


#define Get_DMA_DMA_ITS2_IRP1_PRA_W() p_DMA_IP->DMA_ITS2.DMA_ITS2_ts.IRP1_PRA_W
#define Is_DMA_DMA_ITS2_IRP1_PRA_W_B_0x0() (p_DMA_IP->DMA_ITS2.DMA_ITS2_ts.IRP1_PRA_W == IRP1_PRA_W_B_0x0)
#define IRP1_PRA_W_B_0x0 0x0    //no event ending
#define Is_DMA_DMA_ITS2_IRP1_PRA_W_B_0x1() (p_DMA_IP->DMA_ITS2.DMA_ITS2_ts.IRP1_PRA_W == IRP1_PRA_W_B_0x1)
#define IRP1_PRA_W_B_0x1 0x1    //event pending


#define Get_DMA_DMA_ITS2_IRP2_PRA_W() p_DMA_IP->DMA_ITS2.DMA_ITS2_ts.IRP2_PRA_W
#define Is_DMA_DMA_ITS2_IRP2_PRA_W_B_0x0() (p_DMA_IP->DMA_ITS2.DMA_ITS2_ts.IRP2_PRA_W == IRP2_PRA_W_B_0x0)
#define IRP2_PRA_W_B_0x0 0x0    //no event ending
#define Is_DMA_DMA_ITS2_IRP2_PRA_W_B_0x1() (p_DMA_IP->DMA_ITS2.DMA_ITS2_ts.IRP2_PRA_W == IRP2_PRA_W_B_0x1)
#define IRP2_PRA_W_B_0x1 0x1    //event pending


#define Get_DMA_DMA_ITS2_IRP3_JPG_W() p_DMA_IP->DMA_ITS2.DMA_ITS2_ts.IRP3_JPG_W
#define Is_DMA_DMA_ITS2_IRP3_JPG_W_B_0x0() (p_DMA_IP->DMA_ITS2.DMA_ITS2_ts.IRP3_JPG_W == IRP3_JPG_W_B_0x0)
#define IRP3_JPG_W_B_0x0 0x0    //no event ending
#define Is_DMA_DMA_ITS2_IRP3_JPG_W_B_0x1() (p_DMA_IP->DMA_ITS2.DMA_ITS2_ts.IRP3_JPG_W == IRP3_JPG_W_B_0x1)
#define IRP3_JPG_W_B_0x1 0x1    //event pending
#define Get_DMA_DMA_ITS2() p_DMA_IP->DMA_ITS2.word


//DMA_ITS2_BCLR


#define Set_DMA_DMA_ITS2_BCLR_IRP03_L_W(x) (p_DMA_IP->DMA_ITS2_BCLR.DMA_ITS2_BCLR_ts.IRP03_L_W = x)


#define Set_DMA_DMA_ITS2_BCLR_IRP03_C_W(x) (p_DMA_IP->DMA_ITS2_BCLR.DMA_ITS2_BCLR_ts.IRP03_C_W = x)


#define Set_DMA_DMA_ITS2_BCLR_IRP03_C1_W(x) (p_DMA_IP->DMA_ITS2_BCLR.DMA_ITS2_BCLR_ts.IRP03_C1_W = x)


#define Set_DMA_DMA_ITS2_BCLR_IRP03_C2_W(x) (p_DMA_IP->DMA_ITS2_BCLR.DMA_ITS2_BCLR_ts.IRP03_C2_W = x)


#define Set_DMA_DMA_ITS2_BCLR_IRP03_GCF_L_W(x) (p_DMA_IP->DMA_ITS2_BCLR.DMA_ITS2_BCLR_ts.IRP03_GCF_L_W = x)


#define Set_DMA_DMA_ITS2_BCLR_IRP03_GCF_C_W(x) (p_DMA_IP->DMA_ITS2_BCLR.DMA_ITS2_BCLR_ts.IRP03_GCF_C_W = x)


#define Set_DMA_DMA_ITS2_BCLR_IRP03_GCF_L_R(x) (p_DMA_IP->DMA_ITS2_BCLR.DMA_ITS2_BCLR_ts.IRP03_GCF_L_R = x)


#define Set_DMA_DMA_ITS2_BCLR_IRP03_GCF_C_R(x) (p_DMA_IP->DMA_ITS2_BCLR.DMA_ITS2_BCLR_ts.IRP03_GCF_C_R = x)


#define Set_DMA_DMA_ITS2_BCLR_IRP03_VCP_W(x) (p_DMA_IP->DMA_ITS2_BCLR.DMA_ITS2_BCLR_ts.IRP03_VCP_W = x)


#define Set_DMA_DMA_ITS2_BCLR_IRP03_HCP_W(x) (p_DMA_IP->DMA_ITS2_BCLR.DMA_ITS2_BCLR_ts.IRP03_HCP_W = x)


#define Set_DMA_DMA_ITS2_BCLR_IRP03_HCP_R(x) (p_DMA_IP->DMA_ITS2_BCLR.DMA_ITS2_BCLR_ts.IRP03_HCP_R = x)


#define Set_DMA_DMA_ITS2_BCLR_IRP03_PRA_W(x) (p_DMA_IP->DMA_ITS2_BCLR.DMA_ITS2_BCLR_ts.IRP03_PRA_W = x)


#define Set_DMA_DMA_ITS2_BCLR_IRP0_PRA_R(x) (p_DMA_IP->DMA_ITS2_BCLR.DMA_ITS2_BCLR_ts.IRP0_PRA_R = x)


#define Set_DMA_DMA_ITS2_BCLR_IRP1_PRA_W(x) (p_DMA_IP->DMA_ITS2_BCLR.DMA_ITS2_BCLR_ts.IRP1_PRA_W = x)


#define Set_DMA_DMA_ITS2_BCLR_IRP2_PRA_W(x) (p_DMA_IP->DMA_ITS2_BCLR.DMA_ITS2_BCLR_ts.IRP2_PRA_W = x)


#define Set_DMA_DMA_ITS2_BCLR_IRP3_JPG_W(x) (p_DMA_IP->DMA_ITS2_BCLR.DMA_ITS2_BCLR_ts.IRP3_JPG_W = x)
#define Set_DMA_DMA_ITS2_BCLR(IRP03_L_W,IRP03_C_W,IRP03_C1_W,IRP03_C2_W,IRP03_GCF_L_W,IRP03_GCF_C_W,IRP03_GCF_L_R,IRP03_GCF_C_R,IRP03_VCP_W,IRP03_HCP_W,IRP03_HCP_R,IRP03_PRA_W,IRP0_PRA_R,IRP1_PRA_W,IRP2_PRA_W,IRP3_JPG_W) (p_DMA_IP->DMA_ITS2_BCLR.word = (uint16_t)IRP03_L_W<<0 | (uint16_t)IRP03_C_W<<1 | (uint16_t)IRP03_C1_W<<2 | (uint16_t)IRP03_C2_W<<3 | (uint16_t)IRP03_GCF_L_W<<4 | (uint16_t)IRP03_GCF_C_W<<5 | (uint16_t)IRP03_GCF_L_R<<6 | (uint16_t)IRP03_GCF_C_R<<7 | (uint16_t)IRP03_VCP_W<<8 | (uint16_t)IRP03_HCP_W<<9 | (uint16_t)IRP03_HCP_R<<10 | (uint16_t)IRP03_PRA_W<<11 | (uint16_t)IRP0_PRA_R<<12 | (uint16_t)IRP1_PRA_W<<13 | (uint16_t)IRP2_PRA_W<<14 | (uint16_t)IRP3_JPG_W<<15)
#define Set_DMA_DMA_ITS2_BCLR_word(x) (p_DMA_IP->DMA_ITS2_BCLR.word = x)


//DMA_IMR2


#define Get_DMA_DMA_IMR2_IRP03_L_W() p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_L_W
#define Is_DMA_DMA_IMR2_IRP03_L_W_B_0x0() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_L_W == IRP03_L_W_B_0x0)
#define Set_DMA_DMA_IMR2_IRP03_L_W__B_0x0() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_L_W = IRP03_L_W_B_0x0)
#define IRP03_L_W_B_0x0 0x0    //event disabled
#define Is_DMA_DMA_IMR2_IRP03_L_W_B_0x1() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_L_W == IRP03_L_W_B_0x1)
#define Set_DMA_DMA_IMR2_IRP03_L_W__B_0x1() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_L_W = IRP03_L_W_B_0x1)
#define IRP03_L_W_B_0x1 0x1    //event enabled


#define Get_DMA_DMA_IMR2_IRP03_C_W() p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_C_W
#define Is_DMA_DMA_IMR2_IRP03_C_W_B_0x0() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_C_W == IRP03_C_W_B_0x0)
#define Set_DMA_DMA_IMR2_IRP03_C_W__B_0x0() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_C_W = IRP03_C_W_B_0x0)
#define IRP03_C_W_B_0x0 0x0    //event disabled
#define Is_DMA_DMA_IMR2_IRP03_C_W_B_0x1() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_C_W == IRP03_C_W_B_0x1)
#define Set_DMA_DMA_IMR2_IRP03_C_W__B_0x1() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_C_W = IRP03_C_W_B_0x1)
#define IRP03_C_W_B_0x1 0x1    //event enabled


#define Get_DMA_DMA_IMR2_IRP03_C1_W() p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_C1_W
#define Is_DMA_DMA_IMR2_IRP03_C1_W_B_0x0() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_C1_W == IRP03_C1_W_B_0x0)
#define Set_DMA_DMA_IMR2_IRP03_C1_W__B_0x0() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_C1_W = IRP03_C1_W_B_0x0)
#define IRP03_C1_W_B_0x0 0x0    //event disabled
#define Is_DMA_DMA_IMR2_IRP03_C1_W_B_0x1() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_C1_W == IRP03_C1_W_B_0x1)
#define Set_DMA_DMA_IMR2_IRP03_C1_W__B_0x1() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_C1_W = IRP03_C1_W_B_0x1)
#define IRP03_C1_W_B_0x1 0x1    //event enabled


#define Get_DMA_DMA_IMR2_IRP03_C2_W() p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_C2_W
#define Is_DMA_DMA_IMR2_IRP03_C2_W_B_0x0() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_C2_W == IRP03_C2_W_B_0x0)
#define Set_DMA_DMA_IMR2_IRP03_C2_W__B_0x0() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_C2_W = IRP03_C2_W_B_0x0)
#define IRP03_C2_W_B_0x0 0x0    //event disabled
#define Is_DMA_DMA_IMR2_IRP03_C2_W_B_0x1() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_C2_W == IRP03_C2_W_B_0x1)
#define Set_DMA_DMA_IMR2_IRP03_C2_W__B_0x1() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_C2_W = IRP03_C2_W_B_0x1)
#define IRP03_C2_W_B_0x1 0x1    //event enabled


#define Get_DMA_DMA_IMR2_IRP03_GCF_L_W() p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_GCF_L_W
#define Is_DMA_DMA_IMR2_IRP03_GCF_L_W_B_0x0() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_GCF_L_W == IRP03_GCF_L_W_B_0x0)
#define Set_DMA_DMA_IMR2_IRP03_GCF_L_W__B_0x0() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_GCF_L_W = IRP03_GCF_L_W_B_0x0)
#define IRP03_GCF_L_W_B_0x0 0x0    //event disabled
#define Is_DMA_DMA_IMR2_IRP03_GCF_L_W_B_0x1() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_GCF_L_W == IRP03_GCF_L_W_B_0x1)
#define Set_DMA_DMA_IMR2_IRP03_GCF_L_W__B_0x1() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_GCF_L_W = IRP03_GCF_L_W_B_0x1)
#define IRP03_GCF_L_W_B_0x1 0x1    //event enabled


#define Get_DMA_DMA_IMR2_IRP03_GCF_C_W() p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_GCF_C_W
#define Is_DMA_DMA_IMR2_IRP03_GCF_C_W_B_0x0() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_GCF_C_W == IRP03_GCF_C_W_B_0x0)
#define Set_DMA_DMA_IMR2_IRP03_GCF_C_W__B_0x0() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_GCF_C_W = IRP03_GCF_C_W_B_0x0)
#define IRP03_GCF_C_W_B_0x0 0x0    //event disabled
#define Is_DMA_DMA_IMR2_IRP03_GCF_C_W_B_0x1() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_GCF_C_W == IRP03_GCF_C_W_B_0x1)
#define Set_DMA_DMA_IMR2_IRP03_GCF_C_W__B_0x1() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_GCF_C_W = IRP03_GCF_C_W_B_0x1)
#define IRP03_GCF_C_W_B_0x1 0x1    //event enabled


#define Get_DMA_DMA_IMR2_IRP03_GCF_L_R() p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_GCF_L_R
#define Is_DMA_DMA_IMR2_IRP03_GCF_L_R_B_0x0() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_GCF_L_R == IRP03_GCF_L_R_B_0x0)
#define Set_DMA_DMA_IMR2_IRP03_GCF_L_R__B_0x0() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_GCF_L_R = IRP03_GCF_L_R_B_0x0)
#define IRP03_GCF_L_R_B_0x0 0x0    //event disabled
#define Is_DMA_DMA_IMR2_IRP03_GCF_L_R_B_0x1() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_GCF_L_R == IRP03_GCF_L_R_B_0x1)
#define Set_DMA_DMA_IMR2_IRP03_GCF_L_R__B_0x1() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_GCF_L_R = IRP03_GCF_L_R_B_0x1)
#define IRP03_GCF_L_R_B_0x1 0x1    //event enabled


#define Get_DMA_DMA_IMR2_IRP03_GCF_C_R() p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_GCF_C_R
#define Is_DMA_DMA_IMR2_IRP03_GCF_C_R_B_0x0() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_GCF_C_R == IRP03_GCF_C_R_B_0x0)
#define Set_DMA_DMA_IMR2_IRP03_GCF_C_R__B_0x0() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_GCF_C_R = IRP03_GCF_C_R_B_0x0)
#define IRP03_GCF_C_R_B_0x0 0x0    //event disabled
#define Is_DMA_DMA_IMR2_IRP03_GCF_C_R_B_0x1() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_GCF_C_R == IRP03_GCF_C_R_B_0x1)
#define Set_DMA_DMA_IMR2_IRP03_GCF_C_R__B_0x1() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_GCF_C_R = IRP03_GCF_C_R_B_0x1)
#define IRP03_GCF_C_R_B_0x1 0x1    //event enabled


#define Get_DMA_DMA_IMR2_IRP03_VCP_W() p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_VCP_W
#define Is_DMA_DMA_IMR2_IRP03_VCP_W_B_0x0() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_VCP_W == IRP03_VCP_W_B_0x0)
#define Set_DMA_DMA_IMR2_IRP03_VCP_W__B_0x0() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_VCP_W = IRP03_VCP_W_B_0x0)
#define IRP03_VCP_W_B_0x0 0x0    //event disabled
#define Is_DMA_DMA_IMR2_IRP03_VCP_W_B_0x1() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_VCP_W == IRP03_VCP_W_B_0x1)
#define Set_DMA_DMA_IMR2_IRP03_VCP_W__B_0x1() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_VCP_W = IRP03_VCP_W_B_0x1)
#define IRP03_VCP_W_B_0x1 0x1    //event enabled


#define Get_DMA_DMA_IMR2_IRP03_HCP_W() p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_HCP_W
#define Is_DMA_DMA_IMR2_IRP03_HCP_W_B_0x0() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_HCP_W == IRP03_HCP_W_B_0x0)
#define Set_DMA_DMA_IMR2_IRP03_HCP_W__B_0x0() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_HCP_W = IRP03_HCP_W_B_0x0)
#define IRP03_HCP_W_B_0x0 0x0    //event disabled
#define Is_DMA_DMA_IMR2_IRP03_HCP_W_B_0x1() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_HCP_W == IRP03_HCP_W_B_0x1)
#define Set_DMA_DMA_IMR2_IRP03_HCP_W__B_0x1() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_HCP_W = IRP03_HCP_W_B_0x1)
#define IRP03_HCP_W_B_0x1 0x1    //event enabled


#define Get_DMA_DMA_IMR2_IRP03_HCP_R() p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_HCP_R
#define Is_DMA_DMA_IMR2_IRP03_HCP_R_B_0x0() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_HCP_R == IRP03_HCP_R_B_0x0)
#define Set_DMA_DMA_IMR2_IRP03_HCP_R__B_0x0() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_HCP_R = IRP03_HCP_R_B_0x0)
#define IRP03_HCP_R_B_0x0 0x0    //event disabled
#define Is_DMA_DMA_IMR2_IRP03_HCP_R_B_0x1() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_HCP_R == IRP03_HCP_R_B_0x1)
#define Set_DMA_DMA_IMR2_IRP03_HCP_R__B_0x1() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_HCP_R = IRP03_HCP_R_B_0x1)
#define IRP03_HCP_R_B_0x1 0x1    //event enabled


#define Get_DMA_DMA_IMR2_IRP03_PRA_W() p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_PRA_W
#define Is_DMA_DMA_IMR2_IRP03_PRA_W_B_0x0() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_PRA_W == IRP03_PRA_W_B_0x0)
#define Set_DMA_DMA_IMR2_IRP03_PRA_W__B_0x0() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_PRA_W = IRP03_PRA_W_B_0x0)
#define IRP03_PRA_W_B_0x0 0x0    //event disabled
#define Is_DMA_DMA_IMR2_IRP03_PRA_W_B_0x1() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_PRA_W == IRP03_PRA_W_B_0x1)
#define Set_DMA_DMA_IMR2_IRP03_PRA_W__B_0x1() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP03_PRA_W = IRP03_PRA_W_B_0x1)
#define IRP03_PRA_W_B_0x1 0x1    //event enabled


#define Get_DMA_DMA_IMR2_IRP0_PRA_R() p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP0_PRA_R
#define Is_DMA_DMA_IMR2_IRP0_PRA_R_B_0x0() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP0_PRA_R == IRP0_PRA_R_B_0x0)
#define Set_DMA_DMA_IMR2_IRP0_PRA_R__B_0x0() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP0_PRA_R = IRP0_PRA_R_B_0x0)
#define IRP0_PRA_R_B_0x0 0x0    //event disabled
#define Is_DMA_DMA_IMR2_IRP0_PRA_R_B_0x1() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP0_PRA_R == IRP0_PRA_R_B_0x1)
#define Set_DMA_DMA_IMR2_IRP0_PRA_R__B_0x1() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP0_PRA_R = IRP0_PRA_R_B_0x1)
#define IRP0_PRA_R_B_0x1 0x1    //event enabled


#define Get_DMA_DMA_IMR2_IRP1_PRA_W() p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP1_PRA_W
#define Is_DMA_DMA_IMR2_IRP1_PRA_W_B_0x0() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP1_PRA_W == IRP1_PRA_W_B_0x0)
#define Set_DMA_DMA_IMR2_IRP1_PRA_W__B_0x0() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP1_PRA_W = IRP1_PRA_W_B_0x0)
#define IRP1_PRA_W_B_0x0 0x0    //event disabled
#define Is_DMA_DMA_IMR2_IRP1_PRA_W_B_0x1() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP1_PRA_W == IRP1_PRA_W_B_0x1)
#define Set_DMA_DMA_IMR2_IRP1_PRA_W__B_0x1() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP1_PRA_W = IRP1_PRA_W_B_0x1)
#define IRP1_PRA_W_B_0x1 0x1    //event enabled


#define Get_DMA_DMA_IMR2_IRP2_PRA_W() p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP2_PRA_W
#define Is_DMA_DMA_IMR2_IRP2_PRA_W_B_0x0() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP2_PRA_W == IRP2_PRA_W_B_0x0)
#define Set_DMA_DMA_IMR2_IRP2_PRA_W__B_0x0() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP2_PRA_W = IRP2_PRA_W_B_0x0)
#define IRP2_PRA_W_B_0x0 0x0    //event disabled
#define Is_DMA_DMA_IMR2_IRP2_PRA_W_B_0x1() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP2_PRA_W == IRP2_PRA_W_B_0x1)
#define Set_DMA_DMA_IMR2_IRP2_PRA_W__B_0x1() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP2_PRA_W = IRP2_PRA_W_B_0x1)
#define IRP2_PRA_W_B_0x1 0x1    //event enabled


#define Get_DMA_DMA_IMR2_IRP3_JPG_W() p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP3_JPG_W
#define Is_DMA_DMA_IMR2_IRP3_JPG_W_B_0x0() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP3_JPG_W == IRP3_JPG_W_B_0x0)
#define Set_DMA_DMA_IMR2_IRP3_JPG_W__B_0x0() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP3_JPG_W = IRP3_JPG_W_B_0x0)
#define IRP3_JPG_W_B_0x0 0x0    //event disabled
#define Is_DMA_DMA_IMR2_IRP3_JPG_W_B_0x1() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP3_JPG_W == IRP3_JPG_W_B_0x1)
#define Set_DMA_DMA_IMR2_IRP3_JPG_W__B_0x1() (p_DMA_IP->DMA_IMR2.DMA_IMR2_ts.IRP3_JPG_W = IRP3_JPG_W_B_0x1)
#define IRP3_JPG_W_B_0x1 0x1    //event enabled
#define Set_DMA_DMA_IMR2(IRP03_L_W,IRP03_C_W,IRP03_C1_W,IRP03_C2_W,IRP03_GCF_L_W,IRP03_GCF_C_W,IRP03_GCF_L_R,IRP03_GCF_C_R,IRP03_VCP_W,IRP03_HCP_W,IRP03_HCP_R,IRP03_PRA_W,IRP0_PRA_R,IRP1_PRA_W,IRP2_PRA_W,IRP3_JPG_W) (p_DMA_IP->DMA_IMR2.word = (uint32_t)IRP03_L_W<<0 | (uint32_t)IRP03_C_W<<1 | (uint32_t)IRP03_C1_W<<2 | (uint32_t)IRP03_C2_W<<3 | (uint32_t)IRP03_GCF_L_W<<4 | (uint32_t)IRP03_GCF_C_W<<5 | (uint32_t)IRP03_GCF_L_R<<6 | (uint32_t)IRP03_GCF_C_R<<7 | (uint32_t)IRP03_VCP_W<<8 | (uint32_t)IRP03_HCP_W<<9 | (uint32_t)IRP03_HCP_R<<10 | (uint32_t)IRP03_PRA_W<<11 | (uint32_t)IRP0_PRA_R<<12 | (uint32_t)IRP1_PRA_W<<13 | (uint32_t)IRP2_PRA_W<<14 | (uint32_t)IRP3_JPG_W<<15)
#define Get_DMA_DMA_IMR2() p_DMA_IP->DMA_IMR2.word
#define Set_DMA_DMA_IMR2_word(x) (p_DMA_IP->DMA_IMR2.word = x)


//DMA_SOFT_RST


#define Set_DMA_DMA_SOFT_RST_IRP03_L_W(x) (p_DMA_IP->DMA_SOFT_RST.DMA_SOFT_RST_ts.IRP03_L_W = x)


#define Set_DMA_DMA_SOFT_RST_IRP03_C_W(x) (p_DMA_IP->DMA_SOFT_RST.DMA_SOFT_RST_ts.IRP03_C_W = x)


#define Set_DMA_DMA_SOFT_RST_IRP03_C1_W(x) (p_DMA_IP->DMA_SOFT_RST.DMA_SOFT_RST_ts.IRP03_C1_W = x)


#define Set_DMA_DMA_SOFT_RST_IRP03_C2_W(x) (p_DMA_IP->DMA_SOFT_RST.DMA_SOFT_RST_ts.IRP03_C2_W = x)


#define Set_DMA_DMA_SOFT_RST_IRP03_GCF_L_W(x) (p_DMA_IP->DMA_SOFT_RST.DMA_SOFT_RST_ts.IRP03_GCF_L_W = x)


#define Set_DMA_DMA_SOFT_RST_IRP03_GCF_C_W(x) (p_DMA_IP->DMA_SOFT_RST.DMA_SOFT_RST_ts.IRP03_GCF_C_W = x)


#define Set_DMA_DMA_SOFT_RST_IRP03_GCF_L_R(x) (p_DMA_IP->DMA_SOFT_RST.DMA_SOFT_RST_ts.IRP03_GCF_L_R = x)


#define Set_DMA_DMA_SOFT_RST_IRP03_GCF_C_R(x) (p_DMA_IP->DMA_SOFT_RST.DMA_SOFT_RST_ts.IRP03_GCF_C_R = x)


#define Set_DMA_DMA_SOFT_RST_IRP03_VCP_W(x) (p_DMA_IP->DMA_SOFT_RST.DMA_SOFT_RST_ts.IRP03_VCP_W = x)


#define Set_DMA_DMA_SOFT_RST_IRP03_HCP_W(x) (p_DMA_IP->DMA_SOFT_RST.DMA_SOFT_RST_ts.IRP03_HCP_W = x)


#define Set_DMA_DMA_SOFT_RST_IRP03_HCP_R(x) (p_DMA_IP->DMA_SOFT_RST.DMA_SOFT_RST_ts.IRP03_HCP_R = x)


#define Set_DMA_DMA_SOFT_RST_IRP03_PRA_W(x) (p_DMA_IP->DMA_SOFT_RST.DMA_SOFT_RST_ts.IRP03_PRA_W = x)


#define Set_DMA_DMA_SOFT_RST_IRP0_PRA_R(x) (p_DMA_IP->DMA_SOFT_RST.DMA_SOFT_RST_ts.IRP0_PRA_R = x)


#define Set_DMA_DMA_SOFT_RST_IRP1_PRA_W(x) (p_DMA_IP->DMA_SOFT_RST.DMA_SOFT_RST_ts.IRP1_PRA_W = x)


#define Set_DMA_DMA_SOFT_RST_IRP2_PRA_W(x) (p_DMA_IP->DMA_SOFT_RST.DMA_SOFT_RST_ts.IRP2_PRA_W = x)


#define Set_DMA_DMA_SOFT_RST_IRP3_JPG_W(x) (p_DMA_IP->DMA_SOFT_RST.DMA_SOFT_RST_ts.IRP3_JPG_W = x)
#define Set_DMA_DMA_SOFT_RST(IRP03_L_W,IRP03_C_W,IRP03_C1_W,IRP03_C2_W,IRP03_GCF_L_W,IRP03_GCF_C_W,IRP03_GCF_L_R,IRP03_GCF_C_R,IRP03_VCP_W,IRP03_HCP_W,IRP03_HCP_R,IRP03_PRA_W,IRP0_PRA_R,IRP1_PRA_W,IRP2_PRA_W,IRP3_JPG_W) (p_DMA_IP->DMA_SOFT_RST.word = (uint16_t)IRP03_L_W<<0 | (uint16_t)IRP03_C_W<<1 | (uint16_t)IRP03_C1_W<<2 | (uint16_t)IRP03_C2_W<<3 | (uint16_t)IRP03_GCF_L_W<<4 | (uint16_t)IRP03_GCF_C_W<<5 | (uint16_t)IRP03_GCF_L_R<<6 | (uint16_t)IRP03_GCF_C_R<<7 | (uint16_t)IRP03_VCP_W<<8 | (uint16_t)IRP03_HCP_W<<9 | (uint16_t)IRP03_HCP_R<<10 | (uint16_t)IRP03_PRA_W<<11 | (uint16_t)IRP0_PRA_R<<12 | (uint16_t)IRP1_PRA_W<<13 | (uint16_t)IRP2_PRA_W<<14 | (uint16_t)IRP3_JPG_W<<15)
#define Set_DMA_DMA_SOFT_RST_word(x) (p_DMA_IP->DMA_SOFT_RST.word = x)


//DMA_FIFO_STA


#define Get_DMA_DMA_FIFO_STA_IRP03_L_W() p_DMA_IP->DMA_FIFO_STA.DMA_FIFO_STA_ts.IRP03_L_W
#define Is_DMA_DMA_FIFO_STA_IRP03_L_W_B_0x0() (p_DMA_IP->DMA_FIFO_STA.DMA_FIFO_STA_ts.IRP03_L_W == IRP03_L_W_B_0x0)
#define IRP03_L_W_B_0x0 0x0    //no residual word in data Fifo
#define Is_DMA_DMA_FIFO_STA_IRP03_L_W_B_0x1() (p_DMA_IP->DMA_FIFO_STA.DMA_FIFO_STA_ts.IRP03_L_W == IRP03_L_W_B_0x1)
#define IRP03_L_W_B_0x1 0x1    //1 or more residual words in data Fifo


#define Get_DMA_DMA_FIFO_STA_IRP03_C_W() p_DMA_IP->DMA_FIFO_STA.DMA_FIFO_STA_ts.IRP03_C_W
#define Is_DMA_DMA_FIFO_STA_IRP03_C_W_B_0x0() (p_DMA_IP->DMA_FIFO_STA.DMA_FIFO_STA_ts.IRP03_C_W == IRP03_C_W_B_0x0)
#define IRP03_C_W_B_0x0 0x0    //no residual word in data Fifo
#define Is_DMA_DMA_FIFO_STA_IRP03_C_W_B_0x1() (p_DMA_IP->DMA_FIFO_STA.DMA_FIFO_STA_ts.IRP03_C_W == IRP03_C_W_B_0x1)
#define IRP03_C_W_B_0x1 0x1    //1 or more residual words in data Fifo


#define Get_DMA_DMA_FIFO_STA_IRP03_C1_W() p_DMA_IP->DMA_FIFO_STA.DMA_FIFO_STA_ts.IRP03_C1_W
#define Is_DMA_DMA_FIFO_STA_IRP03_C1_W_B_0x0() (p_DMA_IP->DMA_FIFO_STA.DMA_FIFO_STA_ts.IRP03_C1_W == IRP03_C1_W_B_0x0)
#define IRP03_C1_W_B_0x0 0x0    //no residual word in data Fifo
#define Is_DMA_DMA_FIFO_STA_IRP03_C1_W_B_0x1() (p_DMA_IP->DMA_FIFO_STA.DMA_FIFO_STA_ts.IRP03_C1_W == IRP03_C1_W_B_0x1)
#define IRP03_C1_W_B_0x1 0x1    //1 or more residual words in data Fifo


#define Get_DMA_DMA_FIFO_STA_IRP03_C2_W() p_DMA_IP->DMA_FIFO_STA.DMA_FIFO_STA_ts.IRP03_C2_W
#define Is_DMA_DMA_FIFO_STA_IRP03_C2_W_B_0x0() (p_DMA_IP->DMA_FIFO_STA.DMA_FIFO_STA_ts.IRP03_C2_W == IRP03_C2_W_B_0x0)
#define IRP03_C2_W_B_0x0 0x0    //no residual word in data Fifo
#define Is_DMA_DMA_FIFO_STA_IRP03_C2_W_B_0x1() (p_DMA_IP->DMA_FIFO_STA.DMA_FIFO_STA_ts.IRP03_C2_W == IRP03_C2_W_B_0x1)
#define IRP03_C2_W_B_0x1 0x1    //1 or more residual words in data Fifo


#define Get_DMA_DMA_FIFO_STA_IRP03_GCF_L_W() p_DMA_IP->DMA_FIFO_STA.DMA_FIFO_STA_ts.IRP03_GCF_L_W
#define Is_DMA_DMA_FIFO_STA_IRP03_GCF_L_W_B_0x0() (p_DMA_IP->DMA_FIFO_STA.DMA_FIFO_STA_ts.IRP03_GCF_L_W == IRP03_GCF_L_W_B_0x0)
#define IRP03_GCF_L_W_B_0x0 0x0    //no residual word in data Fifo
#define Is_DMA_DMA_FIFO_STA_IRP03_GCF_L_W_B_0x1() (p_DMA_IP->DMA_FIFO_STA.DMA_FIFO_STA_ts.IRP03_GCF_L_W == IRP03_GCF_L_W_B_0x1)
#define IRP03_GCF_L_W_B_0x1 0x1    //1 or more residual words in data Fifo


#define Get_DMA_DMA_FIFO_STA_IRP03_GCF_C_W() p_DMA_IP->DMA_FIFO_STA.DMA_FIFO_STA_ts.IRP03_GCF_C_W
#define Is_DMA_DMA_FIFO_STA_IRP03_GCF_C_W_B_0x0() (p_DMA_IP->DMA_FIFO_STA.DMA_FIFO_STA_ts.IRP03_GCF_C_W == IRP03_GCF_C_W_B_0x0)
#define IRP03_GCF_C_W_B_0x0 0x0    //no residual word in data Fifo
#define Is_DMA_DMA_FIFO_STA_IRP03_GCF_C_W_B_0x1() (p_DMA_IP->DMA_FIFO_STA.DMA_FIFO_STA_ts.IRP03_GCF_C_W == IRP03_GCF_C_W_B_0x1)
#define IRP03_GCF_C_W_B_0x1 0x1    //1 or more residual words in data Fifo


#define Get_DMA_DMA_FIFO_STA_IRP03_VCP_W() p_DMA_IP->DMA_FIFO_STA.DMA_FIFO_STA_ts.IRP03_VCP_W
#define Is_DMA_DMA_FIFO_STA_IRP03_VCP_W_B_0x0() (p_DMA_IP->DMA_FIFO_STA.DMA_FIFO_STA_ts.IRP03_VCP_W == IRP03_VCP_W_B_0x0)
#define IRP03_VCP_W_B_0x0 0x0    //no residual word in data Fifo
#define Is_DMA_DMA_FIFO_STA_IRP03_VCP_W_B_0x1() (p_DMA_IP->DMA_FIFO_STA.DMA_FIFO_STA_ts.IRP03_VCP_W == IRP03_VCP_W_B_0x1)
#define IRP03_VCP_W_B_0x1 0x1    //1 or more residual words in data Fifo


#define Get_DMA_DMA_FIFO_STA_IRP03_HCP_W() p_DMA_IP->DMA_FIFO_STA.DMA_FIFO_STA_ts.IRP03_HCP_W
#define Is_DMA_DMA_FIFO_STA_IRP03_HCP_W_B_0x0() (p_DMA_IP->DMA_FIFO_STA.DMA_FIFO_STA_ts.IRP03_HCP_W == IRP03_HCP_W_B_0x0)
#define IRP03_HCP_W_B_0x0 0x0    //no residual word in data Fifo
#define Is_DMA_DMA_FIFO_STA_IRP03_HCP_W_B_0x1() (p_DMA_IP->DMA_FIFO_STA.DMA_FIFO_STA_ts.IRP03_HCP_W == IRP03_HCP_W_B_0x1)
#define IRP03_HCP_W_B_0x1 0x1    //1 or more residual words in data Fifo


#define Get_DMA_DMA_FIFO_STA_IRP03_PRA_W() p_DMA_IP->DMA_FIFO_STA.DMA_FIFO_STA_ts.IRP03_PRA_W
#define Is_DMA_DMA_FIFO_STA_IRP03_PRA_W_B_0x0() (p_DMA_IP->DMA_FIFO_STA.DMA_FIFO_STA_ts.IRP03_PRA_W == IRP03_PRA_W_B_0x0)
#define IRP03_PRA_W_B_0x0 0x0    //no residual word in data Fifo
#define Is_DMA_DMA_FIFO_STA_IRP03_PRA_W_B_0x1() (p_DMA_IP->DMA_FIFO_STA.DMA_FIFO_STA_ts.IRP03_PRA_W == IRP03_PRA_W_B_0x1)
#define IRP03_PRA_W_B_0x1 0x1    //1 or more residual words in data Fifo


#define Get_DMA_DMA_FIFO_STA_IRP1_PRA_W() p_DMA_IP->DMA_FIFO_STA.DMA_FIFO_STA_ts.IRP1_PRA_W
#define Is_DMA_DMA_FIFO_STA_IRP1_PRA_W_B_0x0() (p_DMA_IP->DMA_FIFO_STA.DMA_FIFO_STA_ts.IRP1_PRA_W == IRP1_PRA_W_B_0x0)
#define IRP1_PRA_W_B_0x0 0x0    //no residual word in data Fifo
#define Is_DMA_DMA_FIFO_STA_IRP1_PRA_W_B_0x1() (p_DMA_IP->DMA_FIFO_STA.DMA_FIFO_STA_ts.IRP1_PRA_W == IRP1_PRA_W_B_0x1)
#define IRP1_PRA_W_B_0x1 0x1    //1 or more residual words in data Fifo


#define Get_DMA_DMA_FIFO_STA_IRP2_PRA_W() p_DMA_IP->DMA_FIFO_STA.DMA_FIFO_STA_ts.IRP2_PRA_W
#define Is_DMA_DMA_FIFO_STA_IRP2_PRA_W_B_0x0() (p_DMA_IP->DMA_FIFO_STA.DMA_FIFO_STA_ts.IRP2_PRA_W == IRP2_PRA_W_B_0x0)
#define IRP2_PRA_W_B_0x0 0x0    //no residual word in data Fifo
#define Is_DMA_DMA_FIFO_STA_IRP2_PRA_W_B_0x1() (p_DMA_IP->DMA_FIFO_STA.DMA_FIFO_STA_ts.IRP2_PRA_W == IRP2_PRA_W_B_0x1)
#define IRP2_PRA_W_B_0x1 0x1    //1 or more residual words in data Fifo


#define Get_DMA_DMA_FIFO_STA_IRP3_JPG_W() p_DMA_IP->DMA_FIFO_STA.DMA_FIFO_STA_ts.IRP3_JPG_W
#define Is_DMA_DMA_FIFO_STA_IRP3_JPG_W_B_0x0() (p_DMA_IP->DMA_FIFO_STA.DMA_FIFO_STA_ts.IRP3_JPG_W == IRP3_JPG_W_B_0x0)
#define IRP3_JPG_W_B_0x0 0x0    //no residual word in data Fifo
#define Is_DMA_DMA_FIFO_STA_IRP3_JPG_W_B_0x1() (p_DMA_IP->DMA_FIFO_STA.DMA_FIFO_STA_ts.IRP3_JPG_W == IRP3_JPG_W_B_0x1)
#define IRP3_JPG_W_B_0x1 0x1    //1 or more residual words in data Fifo
#define Get_DMA_DMA_FIFO_STA() p_DMA_IP->DMA_FIFO_STA.word


//DMA_FIFO_FLUSH


#define Set_DMA_DMA_FIFO_FLUSH_IRP3_JPG_W(x) (p_DMA_IP->DMA_FIFO_FLUSH.DMA_FIFO_FLUSH_ts.IRP3_JPG_W = x)
#define Set_DMA_DMA_FIFO_FLUSH(IRP3_JPG_W) (p_DMA_IP->DMA_FIFO_FLUSH.word = (uint32_t)IRP3_JPG_W<<15)
#define Set_DMA_DMA_FIFO_FLUSH_word(x) (p_DMA_IP->DMA_FIFO_FLUSH.word = x)


//DMA_IRP03_DLS_L


#define Get_DMA_DMA_IRP03_DLS_L_DMA_IRP03_DLS_L() p_DMA_IP->DMA_IRP03_DLS_L.DMA_IRP03_DLS_L_ts.DMA_IRP03_DLS_L
#define Set_DMA_DMA_IRP03_DLS_L_DMA_IRP03_DLS_L(x) (p_DMA_IP->DMA_IRP03_DLS_L.DMA_IRP03_DLS_L_ts.DMA_IRP03_DLS_L = x)
#define Set_DMA_DMA_IRP03_DLS_L(DMA_IRP03_DLS_L) (p_DMA_IP->DMA_IRP03_DLS_L.word = (uint16_t)DMA_IRP03_DLS_L<<3)
#define Get_DMA_DMA_IRP03_DLS_L() p_DMA_IP->DMA_IRP03_DLS_L.word
#define Set_DMA_DMA_IRP03_DLS_L_word(x) (p_DMA_IP->DMA_IRP03_DLS_L.word = x)


//DMA_IRP03_DLS_H


#define Get_DMA_DMA_IRP03_DLS_H_DMA_IRP03_DLS_H() p_DMA_IP->DMA_IRP03_DLS_H.DMA_IRP03_DLS_H_ts.DMA_IRP03_DLS_H
#define Set_DMA_DMA_IRP03_DLS_H_DMA_IRP03_DLS_H(x) (p_DMA_IP->DMA_IRP03_DLS_H.DMA_IRP03_DLS_H_ts.DMA_IRP03_DLS_H = x)
#define Set_DMA_DMA_IRP03_DLS_H(DMA_IRP03_DLS_H) (p_DMA_IP->DMA_IRP03_DLS_H.word = (uint16_t)DMA_IRP03_DLS_H<<0)
#define Get_DMA_DMA_IRP03_DLS_H() p_DMA_IP->DMA_IRP03_DLS_H.word
#define Set_DMA_DMA_IRP03_DLS_H_word(x) (p_DMA_IP->DMA_IRP03_DLS_H.word = x)


//DMA_IRP03_DCS_L


#define Get_DMA_DMA_IRP03_DCS_L_DMA_IRP03_DCS_L() p_DMA_IP->DMA_IRP03_DCS_L.DMA_IRP03_DCS_L_ts.DMA_IRP03_DCS_L
#define Set_DMA_DMA_IRP03_DCS_L_DMA_IRP03_DCS_L(x) (p_DMA_IP->DMA_IRP03_DCS_L.DMA_IRP03_DCS_L_ts.DMA_IRP03_DCS_L = x)
#define Set_DMA_DMA_IRP03_DCS_L(DMA_IRP03_DCS_L) (p_DMA_IP->DMA_IRP03_DCS_L.word = (uint16_t)DMA_IRP03_DCS_L<<3)
#define Get_DMA_DMA_IRP03_DCS_L() p_DMA_IP->DMA_IRP03_DCS_L.word
#define Set_DMA_DMA_IRP03_DCS_L_word(x) (p_DMA_IP->DMA_IRP03_DCS_L.word = x)


//DMA_IRP03_DCS_H


#define Get_DMA_DMA_IRP03_DCS_H_DMA_IRP03_DCS_H() p_DMA_IP->DMA_IRP03_DCS_H.DMA_IRP03_DCS_H_ts.DMA_IRP03_DCS_H
#define Set_DMA_DMA_IRP03_DCS_H_DMA_IRP03_DCS_H(x) (p_DMA_IP->DMA_IRP03_DCS_H.DMA_IRP03_DCS_H_ts.DMA_IRP03_DCS_H = x)
#define Set_DMA_DMA_IRP03_DCS_H(DMA_IRP03_DCS_H) (p_DMA_IP->DMA_IRP03_DCS_H.word = (uint16_t)DMA_IRP03_DCS_H<<0)
#define Get_DMA_DMA_IRP03_DCS_H() p_DMA_IP->DMA_IRP03_DCS_H.word
#define Set_DMA_DMA_IRP03_DCS_H_word(x) (p_DMA_IP->DMA_IRP03_DCS_H.word = x)


//DMA_IRP03_CLS_L


#define Get_DMA_DMA_IRP03_CLS_L_DMA_IRP03_CLS_L() p_DMA_IP->DMA_IRP03_CLS_L.DMA_IRP03_CLS_L_ts.DMA_IRP03_CLS_L
#define Set_DMA_DMA_IRP03_CLS_L_DMA_IRP03_CLS_L(x) (p_DMA_IP->DMA_IRP03_CLS_L.DMA_IRP03_CLS_L_ts.DMA_IRP03_CLS_L = x)
#define Set_DMA_DMA_IRP03_CLS_L(DMA_IRP03_CLS_L) (p_DMA_IP->DMA_IRP03_CLS_L.word = (uint16_t)DMA_IRP03_CLS_L<<3)
#define Get_DMA_DMA_IRP03_CLS_L() p_DMA_IP->DMA_IRP03_CLS_L.word
#define Set_DMA_DMA_IRP03_CLS_L_word(x) (p_DMA_IP->DMA_IRP03_CLS_L.word = x)


//DMA_IRP03_CLS_H


#define Get_DMA_DMA_IRP03_CLS_H_DMA_IRP03_CLS_H() p_DMA_IP->DMA_IRP03_CLS_H.DMA_IRP03_CLS_H_ts.DMA_IRP03_CLS_H
#define Set_DMA_DMA_IRP03_CLS_H_DMA_IRP03_CLS_H(x) (p_DMA_IP->DMA_IRP03_CLS_H.DMA_IRP03_CLS_H_ts.DMA_IRP03_CLS_H = x)
#define Set_DMA_DMA_IRP03_CLS_H(DMA_IRP03_CLS_H) (p_DMA_IP->DMA_IRP03_CLS_H.word = (uint16_t)DMA_IRP03_CLS_H<<0)
#define Get_DMA_DMA_IRP03_CLS_H() p_DMA_IP->DMA_IRP03_CLS_H.word
#define Set_DMA_DMA_IRP03_CLS_H_word(x) (p_DMA_IP->DMA_IRP03_CLS_H.word = x)


//DMA_IRP03_CCS_L


#define Get_DMA_DMA_IRP03_CCS_L_DMA_IRP03_CCS_L() p_DMA_IP->DMA_IRP03_CCS_L.DMA_IRP03_CCS_L_ts.DMA_IRP03_CCS_L
#define Set_DMA_DMA_IRP03_CCS_L_DMA_IRP03_CCS_L(x) (p_DMA_IP->DMA_IRP03_CCS_L.DMA_IRP03_CCS_L_ts.DMA_IRP03_CCS_L = x)
#define Set_DMA_DMA_IRP03_CCS_L(DMA_IRP03_CCS_L) (p_DMA_IP->DMA_IRP03_CCS_L.word = (uint16_t)DMA_IRP03_CCS_L<<3)
#define Get_DMA_DMA_IRP03_CCS_L() p_DMA_IP->DMA_IRP03_CCS_L.word
#define Set_DMA_DMA_IRP03_CCS_L_word(x) (p_DMA_IP->DMA_IRP03_CCS_L.word = x)


//DMA_IRP03_CCS_H


#define Get_DMA_DMA_IRP03_CCS_H_DMA_IRP03_CCS_H() p_DMA_IP->DMA_IRP03_CCS_H.DMA_IRP03_CCS_H_ts.DMA_IRP03_CCS_H
#define Set_DMA_DMA_IRP03_CCS_H_DMA_IRP03_CCS_H(x) (p_DMA_IP->DMA_IRP03_CCS_H.DMA_IRP03_CCS_H_ts.DMA_IRP03_CCS_H = x)
#define Set_DMA_DMA_IRP03_CCS_H(DMA_IRP03_CCS_H) (p_DMA_IP->DMA_IRP03_CCS_H.word = (uint16_t)DMA_IRP03_CCS_H<<0)
#define Get_DMA_DMA_IRP03_CCS_H() p_DMA_IP->DMA_IRP03_CCS_H.word
#define Set_DMA_DMA_IRP03_CCS_H_word(x) (p_DMA_IP->DMA_IRP03_CCS_H.word = x)


//DMA_IRP03_DC1S_L


#define Get_DMA_DMA_IRP03_DC1S_L_DMA_IRP03_DC1S_L() p_DMA_IP->DMA_IRP03_DC1S_L.DMA_IRP03_DC1S_L_ts.DMA_IRP03_DC1S_L
#define Set_DMA_DMA_IRP03_DC1S_L_DMA_IRP03_DC1S_L(x) (p_DMA_IP->DMA_IRP03_DC1S_L.DMA_IRP03_DC1S_L_ts.DMA_IRP03_DC1S_L = x)
#define Set_DMA_DMA_IRP03_DC1S_L(DMA_IRP03_DC1S_L) (p_DMA_IP->DMA_IRP03_DC1S_L.word = (uint16_t)DMA_IRP03_DC1S_L<<3)
#define Get_DMA_DMA_IRP03_DC1S_L() p_DMA_IP->DMA_IRP03_DC1S_L.word
#define Set_DMA_DMA_IRP03_DC1S_L_word(x) (p_DMA_IP->DMA_IRP03_DC1S_L.word = x)


//DMA_IRP03_DC1S_H


#define Get_DMA_DMA_IRP03_DC1S_H_DMA_IRP03_DC1S_H() p_DMA_IP->DMA_IRP03_DC1S_H.DMA_IRP03_DC1S_H_ts.DMA_IRP03_DC1S_H
#define Set_DMA_DMA_IRP03_DC1S_H_DMA_IRP03_DC1S_H(x) (p_DMA_IP->DMA_IRP03_DC1S_H.DMA_IRP03_DC1S_H_ts.DMA_IRP03_DC1S_H = x)
#define Set_DMA_DMA_IRP03_DC1S_H(DMA_IRP03_DC1S_H) (p_DMA_IP->DMA_IRP03_DC1S_H.word = (uint16_t)DMA_IRP03_DC1S_H<<0)
#define Get_DMA_DMA_IRP03_DC1S_H() p_DMA_IP->DMA_IRP03_DC1S_H.word
#define Set_DMA_DMA_IRP03_DC1S_H_word(x) (p_DMA_IP->DMA_IRP03_DC1S_H.word = x)


//DMA_IRP03_DC2S_L


#define Get_DMA_DMA_IRP03_DC2S_L_DMA_IRP03_DC2S_L() p_DMA_IP->DMA_IRP03_DC2S_L.DMA_IRP03_DC2S_L_ts.DMA_IRP03_DC2S_L
#define Set_DMA_DMA_IRP03_DC2S_L_DMA_IRP03_DC2S_L(x) (p_DMA_IP->DMA_IRP03_DC2S_L.DMA_IRP03_DC2S_L_ts.DMA_IRP03_DC2S_L = x)
#define Set_DMA_DMA_IRP03_DC2S_L(DMA_IRP03_DC2S_L) (p_DMA_IP->DMA_IRP03_DC2S_L.word = (uint16_t)DMA_IRP03_DC2S_L<<3)
#define Get_DMA_DMA_IRP03_DC2S_L() p_DMA_IP->DMA_IRP03_DC2S_L.word
#define Set_DMA_DMA_IRP03_DC2S_L_word(x) (p_DMA_IP->DMA_IRP03_DC2S_L.word = x)


//DMA_IRP03_DC2S_H


#define Get_DMA_DMA_IRP03_DC2S_H_DMA_IRP03_DC2S_H() p_DMA_IP->DMA_IRP03_DC2S_H.DMA_IRP03_DC2S_H_ts.DMA_IRP03_DC2S_H
#define Set_DMA_DMA_IRP03_DC2S_H_DMA_IRP03_DC2S_H(x) (p_DMA_IP->DMA_IRP03_DC2S_H.DMA_IRP03_DC2S_H_ts.DMA_IRP03_DC2S_H = x)
#define Set_DMA_DMA_IRP03_DC2S_H(DMA_IRP03_DC2S_H) (p_DMA_IP->DMA_IRP03_DC2S_H.word = (uint16_t)DMA_IRP03_DC2S_H<<0)
#define Get_DMA_DMA_IRP03_DC2S_H() p_DMA_IP->DMA_IRP03_DC2S_H.word
#define Set_DMA_DMA_IRP03_DC2S_H_word(x) (p_DMA_IP->DMA_IRP03_DC2S_H.word = x)


//DMA_IRP3_DRS_L


#define Get_DMA_DMA_IRP3_DRS_L_DMA_IRP3_DRS_L() p_DMA_IP->DMA_IRP3_DRS_L.DMA_IRP3_DRS_L_ts.DMA_IRP3_DRS_L
#define Set_DMA_DMA_IRP3_DRS_L_DMA_IRP3_DRS_L(x) (p_DMA_IP->DMA_IRP3_DRS_L.DMA_IRP3_DRS_L_ts.DMA_IRP3_DRS_L = x)
#define Set_DMA_DMA_IRP3_DRS_L(DMA_IRP3_DRS_L) (p_DMA_IP->DMA_IRP3_DRS_L.word = (uint16_t)DMA_IRP3_DRS_L<<3)
#define Get_DMA_DMA_IRP3_DRS_L() p_DMA_IP->DMA_IRP3_DRS_L.word
#define Set_DMA_DMA_IRP3_DRS_L_word(x) (p_DMA_IP->DMA_IRP3_DRS_L.word = x)


//DMA_IRP3_DRS_H


#define Get_DMA_DMA_IRP3_DRS_H_DMA_IRP3_DRS_H() p_DMA_IP->DMA_IRP3_DRS_H.DMA_IRP3_DRS_H_ts.DMA_IRP3_DRS_H
#define Set_DMA_DMA_IRP3_DRS_H_DMA_IRP3_DRS_H(x) (p_DMA_IP->DMA_IRP3_DRS_H.DMA_IRP3_DRS_H_ts.DMA_IRP3_DRS_H = x)
#define Set_DMA_DMA_IRP3_DRS_H(DMA_IRP3_DRS_H) (p_DMA_IP->DMA_IRP3_DRS_H.word = (uint32_t)DMA_IRP3_DRS_H<<0)
#define Get_DMA_DMA_IRP3_DRS_H() p_DMA_IP->DMA_IRP3_DRS_H.word
#define Set_DMA_DMA_IRP3_DRS_H_word(x) (p_DMA_IP->DMA_IRP3_DRS_H.word = x)


//DMA_IRP03_HPS_L


#define Get_DMA_DMA_IRP03_HPS_L_DMA_IRP03_HPS_L() p_DMA_IP->DMA_IRP03_HPS_L.DMA_IRP03_HPS_L_ts.DMA_IRP03_HPS_L
#define Set_DMA_DMA_IRP03_HPS_L_DMA_IRP03_HPS_L(x) (p_DMA_IP->DMA_IRP03_HPS_L.DMA_IRP03_HPS_L_ts.DMA_IRP03_HPS_L = x)
#define Set_DMA_DMA_IRP03_HPS_L(DMA_IRP03_HPS_L) (p_DMA_IP->DMA_IRP03_HPS_L.word = (uint16_t)DMA_IRP03_HPS_L<<3)
#define Get_DMA_DMA_IRP03_HPS_L() p_DMA_IP->DMA_IRP03_HPS_L.word
#define Set_DMA_DMA_IRP03_HPS_L_word(x) (p_DMA_IP->DMA_IRP03_HPS_L.word = x)


//DMA_IRP03_HPS_H


#define Get_DMA_DMA_IRP03_HPS_H_DMA_IRP03_HPS_H() p_DMA_IP->DMA_IRP03_HPS_H.DMA_IRP03_HPS_H_ts.DMA_IRP03_HPS_H
#define Set_DMA_DMA_IRP03_HPS_H_DMA_IRP03_HPS_H(x) (p_DMA_IP->DMA_IRP03_HPS_H.DMA_IRP03_HPS_H_ts.DMA_IRP03_HPS_H = x)
#define Set_DMA_DMA_IRP03_HPS_H(DMA_IRP03_HPS_H) (p_DMA_IP->DMA_IRP03_HPS_H.word = (uint16_t)DMA_IRP03_HPS_H<<0)
#define Get_DMA_DMA_IRP03_HPS_H() p_DMA_IP->DMA_IRP03_HPS_H.word
#define Set_DMA_DMA_IRP03_HPS_H_word(x) (p_DMA_IP->DMA_IRP03_HPS_H.word = x)


//DMA_IRP03_VPS_L


#define Get_DMA_DMA_IRP03_VPS_L_DMA_IRP03_VPS_L() p_DMA_IP->DMA_IRP03_VPS_L.DMA_IRP03_VPS_L_ts.DMA_IRP03_VPS_L
#define Set_DMA_DMA_IRP03_VPS_L_DMA_IRP03_VPS_L(x) (p_DMA_IP->DMA_IRP03_VPS_L.DMA_IRP03_VPS_L_ts.DMA_IRP03_VPS_L = x)
#define Set_DMA_DMA_IRP03_VPS_L(DMA_IRP03_VPS_L) (p_DMA_IP->DMA_IRP03_VPS_L.word = (uint16_t)DMA_IRP03_VPS_L<<3)
#define Get_DMA_DMA_IRP03_VPS_L() p_DMA_IP->DMA_IRP03_VPS_L.word
#define Set_DMA_DMA_IRP03_VPS_L_word(x) (p_DMA_IP->DMA_IRP03_VPS_L.word = x)


//DMA_IRP03_VPS_H


#define Get_DMA_DMA_IRP03_VPS_H_DMA_IRP03_VPS_H() p_DMA_IP->DMA_IRP03_VPS_H.DMA_IRP03_VPS_H_ts.DMA_IRP03_VPS_H
#define Set_DMA_DMA_IRP03_VPS_H_DMA_IRP03_VPS_H(x) (p_DMA_IP->DMA_IRP03_VPS_H.DMA_IRP03_VPS_H_ts.DMA_IRP03_VPS_H = x)
#define Set_DMA_DMA_IRP03_VPS_H(DMA_IRP03_VPS_H) (p_DMA_IP->DMA_IRP03_VPS_H.word = (uint16_t)DMA_IRP03_VPS_H<<0)
#define Get_DMA_DMA_IRP03_VPS_H() p_DMA_IP->DMA_IRP03_VPS_H.word
#define Set_DMA_DMA_IRP03_VPS_H_word(x) (p_DMA_IP->DMA_IRP03_VPS_H.word = x)


//DMA_IRP3_JPGS_L


#define Get_DMA_DMA_IRP3_JPGS_L_DMA_IRP03_VPS_L() p_DMA_IP->DMA_IRP3_JPGS_L.DMA_IRP3_JPGS_L_ts.DMA_IRP03_VPS_L
#define Set_DMA_DMA_IRP3_JPGS_L_DMA_IRP03_VPS_L(x) (p_DMA_IP->DMA_IRP3_JPGS_L.DMA_IRP3_JPGS_L_ts.DMA_IRP03_VPS_L = x)
#define Set_DMA_DMA_IRP3_JPGS_L(DMA_IRP03_VPS_L) (p_DMA_IP->DMA_IRP3_JPGS_L.word = (uint16_t)DMA_IRP03_VPS_L<<3)
#define Get_DMA_DMA_IRP3_JPGS_L() p_DMA_IP->DMA_IRP3_JPGS_L.word
#define Set_DMA_DMA_IRP3_JPGS_L_word(x) (p_DMA_IP->DMA_IRP3_JPGS_L.word = x)


//DMA_IRP3_JPGS_H


#define Get_DMA_DMA_IRP3_JPGS_H_DMA_IRP03_VPS_H() p_DMA_IP->DMA_IRP3_JPGS_H.DMA_IRP3_JPGS_H_ts.DMA_IRP03_VPS_H
#define Set_DMA_DMA_IRP3_JPGS_H_DMA_IRP03_VPS_H(x) (p_DMA_IP->DMA_IRP3_JPGS_H.DMA_IRP3_JPGS_H_ts.DMA_IRP03_VPS_H = x)
#define Set_DMA_DMA_IRP3_JPGS_H(DMA_IRP03_VPS_H) (p_DMA_IP->DMA_IRP3_JPGS_H.word = (uint16_t)DMA_IRP03_VPS_H<<0)
#define Get_DMA_DMA_IRP3_JPGS_H() p_DMA_IP->DMA_IRP3_JPGS_H.word
#define Set_DMA_DMA_IRP3_JPGS_H_word(x) (p_DMA_IP->DMA_IRP3_JPGS_H.word = x)


//DMA_IRP03_DFW


#define Get_DMA_DMA_IRP03_DFW_DMA_IRP03_DFW() p_DMA_IP->DMA_IRP03_DFW.DMA_IRP03_DFW_ts.DMA_IRP03_DFW
#define Set_DMA_DMA_IRP03_DFW_DMA_IRP03_DFW(x) (p_DMA_IP->DMA_IRP03_DFW.DMA_IRP03_DFW_ts.DMA_IRP03_DFW = x)
#define Set_DMA_DMA_IRP03_DFW(DMA_IRP03_DFW) (p_DMA_IP->DMA_IRP03_DFW.word = (uint16_t)DMA_IRP03_DFW<<3)
#define Get_DMA_DMA_IRP03_DFW() p_DMA_IP->DMA_IRP03_DFW.word
#define Set_DMA_DMA_IRP03_DFW_word(x) (p_DMA_IP->DMA_IRP03_DFW.word = x)


//DMA_IRP03_DWW


#define Get_DMA_DMA_IRP03_DWW_DMA_IRP03_DWW() p_DMA_IP->DMA_IRP03_DWW.DMA_IRP03_DWW_ts.DMA_IRP03_DWW
#define Set_DMA_DMA_IRP03_DWW_DMA_IRP03_DWW(x) (p_DMA_IP->DMA_IRP03_DWW.DMA_IRP03_DWW_ts.DMA_IRP03_DWW = x)
#define Set_DMA_DMA_IRP03_DWW(DMA_IRP03_DWW) (p_DMA_IP->DMA_IRP03_DWW.word = (uint16_t)DMA_IRP03_DWW<<3)
#define Get_DMA_DMA_IRP03_DWW() p_DMA_IP->DMA_IRP03_DWW.word
#define Set_DMA_DMA_IRP03_DWW_word(x) (p_DMA_IP->DMA_IRP03_DWW.word = x)


//DMA_IRP03_DWH


#define Get_DMA_DMA_IRP03_DWH_DMA_IRP03_DWH() p_DMA_IP->DMA_IRP03_DWH.DMA_IRP03_DWH_ts.DMA_IRP03_DWH
#define Set_DMA_DMA_IRP03_DWH_DMA_IRP03_DWH(x) (p_DMA_IP->DMA_IRP03_DWH.DMA_IRP03_DWH_ts.DMA_IRP03_DWH = x)
#define Set_DMA_DMA_IRP03_DWH(DMA_IRP03_DWH) (p_DMA_IP->DMA_IRP03_DWH.word = (uint16_t)DMA_IRP03_DWH<<0)
#define Get_DMA_DMA_IRP03_DWH() p_DMA_IP->DMA_IRP03_DWH.word
#define Set_DMA_DMA_IRP03_DWH_word(x) (p_DMA_IP->DMA_IRP03_DWH.word = x)


//DMA_IRP3_JPG_DBS_L


#define Get_DMA_DMA_IRP3_JPG_DBS_L_DMA_IRP3_JPG_DBS_L() p_DMA_IP->DMA_IRP3_JPG_DBS_L.DMA_IRP3_JPG_DBS_L_ts.DMA_IRP3_JPG_DBS_L
#define Set_DMA_DMA_IRP3_JPG_DBS_L_DMA_IRP3_JPG_DBS_L(x) (p_DMA_IP->DMA_IRP3_JPG_DBS_L.DMA_IRP3_JPG_DBS_L_ts.DMA_IRP3_JPG_DBS_L = x)
#define Set_DMA_DMA_IRP3_JPG_DBS_L(DMA_IRP3_JPG_DBS_L) (p_DMA_IP->DMA_IRP3_JPG_DBS_L.word = (uint16_t)DMA_IRP3_JPG_DBS_L<<3)
#define Get_DMA_DMA_IRP3_JPG_DBS_L() p_DMA_IP->DMA_IRP3_JPG_DBS_L.word
#define Set_DMA_DMA_IRP3_JPG_DBS_L_word(x) (p_DMA_IP->DMA_IRP3_JPG_DBS_L.word = x)


//DMA_IRP3_JPG_DBS_H


#define Get_DMA_DMA_IRP3_JPG_DBS_H_DMA_IRP3_JPG_DBS_H() p_DMA_IP->DMA_IRP3_JPG_DBS_H.DMA_IRP3_JPG_DBS_H_ts.DMA_IRP3_JPG_DBS_H
#define Set_DMA_DMA_IRP3_JPG_DBS_H_DMA_IRP3_JPG_DBS_H(x) (p_DMA_IP->DMA_IRP3_JPG_DBS_H.DMA_IRP3_JPG_DBS_H_ts.DMA_IRP3_JPG_DBS_H = x)
#define Set_DMA_DMA_IRP3_JPG_DBS_H(DMA_IRP3_JPG_DBS_H) (p_DMA_IP->DMA_IRP3_JPG_DBS_H.word = (uint16_t)DMA_IRP3_JPG_DBS_H<<0)
#define Get_DMA_DMA_IRP3_JPG_DBS_H() p_DMA_IP->DMA_IRP3_JPG_DBS_H.word
#define Set_DMA_DMA_IRP3_JPG_DBS_H_word(x) (p_DMA_IP->DMA_IRP3_JPG_DBS_H.word = x)


//DMA_IRP03_CFC


#define Get_DMA_DMA_IRP03_CFC_IRP03_C_W() p_DMA_IP->DMA_IRP03_CFC.DMA_IRP03_CFC_ts.IRP03_C_W
#define Is_DMA_DMA_IRP03_CFC_IRP03_C_W_B_0x0() (p_DMA_IP->DMA_IRP03_CFC.DMA_IRP03_CFC_ts.IRP03_C_W == IRP03_C_W_B_0x0)
#define Set_DMA_DMA_IRP03_CFC_IRP03_C_W__B_0x0() (p_DMA_IP->DMA_IRP03_CFC.DMA_IRP03_CFC_ts.IRP03_C_W = IRP03_C_W_B_0x0)
#define IRP03_C_W_B_0x0 0x0    //YUV420 chroma sampling
#define Is_DMA_DMA_IRP03_CFC_IRP03_C_W_B_0x1() (p_DMA_IP->DMA_IRP03_CFC.DMA_IRP03_CFC_ts.IRP03_C_W == IRP03_C_W_B_0x1)
#define Set_DMA_DMA_IRP03_CFC_IRP03_C_W__B_0x1() (p_DMA_IP->DMA_IRP03_CFC.DMA_IRP03_CFC_ts.IRP03_C_W = IRP03_C_W_B_0x1)
#define IRP03_C_W_B_0x1 0x1    //YUV422 chroma sampling


#define Get_DMA_DMA_IRP03_CFC_IRP03_C1_W() p_DMA_IP->DMA_IRP03_CFC.DMA_IRP03_CFC_ts.IRP03_C1_W
#define Is_DMA_DMA_IRP03_CFC_IRP03_C1_W_B_0x0() (p_DMA_IP->DMA_IRP03_CFC.DMA_IRP03_CFC_ts.IRP03_C1_W == IRP03_C1_W_B_0x0)
#define Set_DMA_DMA_IRP03_CFC_IRP03_C1_W__B_0x0() (p_DMA_IP->DMA_IRP03_CFC.DMA_IRP03_CFC_ts.IRP03_C1_W = IRP03_C1_W_B_0x0)
#define IRP03_C1_W_B_0x0 0x0    //YUV420 chroma sampling
#define Is_DMA_DMA_IRP03_CFC_IRP03_C1_W_B_0x1() (p_DMA_IP->DMA_IRP03_CFC.DMA_IRP03_CFC_ts.IRP03_C1_W == IRP03_C1_W_B_0x1)
#define Set_DMA_DMA_IRP03_CFC_IRP03_C1_W__B_0x1() (p_DMA_IP->DMA_IRP03_CFC.DMA_IRP03_CFC_ts.IRP03_C1_W = IRP03_C1_W_B_0x1)
#define IRP03_C1_W_B_0x1 0x1    //YUV422 chroma sampling


#define Get_DMA_DMA_IRP03_CFC_IRP03_C2_W() p_DMA_IP->DMA_IRP03_CFC.DMA_IRP03_CFC_ts.IRP03_C2_W
#define Is_DMA_DMA_IRP03_CFC_IRP03_C2_W_B_0x0() (p_DMA_IP->DMA_IRP03_CFC.DMA_IRP03_CFC_ts.IRP03_C2_W == IRP03_C2_W_B_0x0)
#define Set_DMA_DMA_IRP03_CFC_IRP03_C2_W__B_0x0() (p_DMA_IP->DMA_IRP03_CFC.DMA_IRP03_CFC_ts.IRP03_C2_W = IRP03_C2_W_B_0x0)
#define IRP03_C2_W_B_0x0 0x0    //YUV420 chroma sampling
#define Is_DMA_DMA_IRP03_CFC_IRP03_C2_W_B_0x1() (p_DMA_IP->DMA_IRP03_CFC.DMA_IRP03_CFC_ts.IRP03_C2_W == IRP03_C2_W_B_0x1)
#define Set_DMA_DMA_IRP03_CFC_IRP03_C2_W__B_0x1() (p_DMA_IP->DMA_IRP03_CFC.DMA_IRP03_CFC_ts.IRP03_C2_W = IRP03_C2_W_B_0x1)
#define IRP03_C2_W_B_0x1 0x1    //YUV422 chroma sampling


#define Get_DMA_DMA_IRP03_CFC_IRP03_GCF_C_W() p_DMA_IP->DMA_IRP03_CFC.DMA_IRP03_CFC_ts.IRP03_GCF_C_W
#define Is_DMA_DMA_IRP03_CFC_IRP03_GCF_C_W_B_0x0() (p_DMA_IP->DMA_IRP03_CFC.DMA_IRP03_CFC_ts.IRP03_GCF_C_W == IRP03_GCF_C_W_B_0x0)
#define Set_DMA_DMA_IRP03_CFC_IRP03_GCF_C_W__B_0x0() (p_DMA_IP->DMA_IRP03_CFC.DMA_IRP03_CFC_ts.IRP03_GCF_C_W = IRP03_GCF_C_W_B_0x0)
#define IRP03_GCF_C_W_B_0x0 0x0    //YUV420 chroma sampling
#define Is_DMA_DMA_IRP03_CFC_IRP03_GCF_C_W_B_0x1() (p_DMA_IP->DMA_IRP03_CFC.DMA_IRP03_CFC_ts.IRP03_GCF_C_W == IRP03_GCF_C_W_B_0x1)
#define Set_DMA_DMA_IRP03_CFC_IRP03_GCF_C_W__B_0x1() (p_DMA_IP->DMA_IRP03_CFC.DMA_IRP03_CFC_ts.IRP03_GCF_C_W = IRP03_GCF_C_W_B_0x1)
#define IRP03_GCF_C_W_B_0x1 0x1    //YUV422 chroma sampling


#define Get_DMA_DMA_IRP03_CFC_IRP03_GCF_C_R() p_DMA_IP->DMA_IRP03_CFC.DMA_IRP03_CFC_ts.IRP03_GCF_C_R
#define Is_DMA_DMA_IRP03_CFC_IRP03_GCF_C_R_B_0x0() (p_DMA_IP->DMA_IRP03_CFC.DMA_IRP03_CFC_ts.IRP03_GCF_C_R == IRP03_GCF_C_R_B_0x0)
#define Set_DMA_DMA_IRP03_CFC_IRP03_GCF_C_R__B_0x0() (p_DMA_IP->DMA_IRP03_CFC.DMA_IRP03_CFC_ts.IRP03_GCF_C_R = IRP03_GCF_C_R_B_0x0)
#define IRP03_GCF_C_R_B_0x0 0x0    //YUV420 chroma sampling
#define Is_DMA_DMA_IRP03_CFC_IRP03_GCF_C_R_B_0x1() (p_DMA_IP->DMA_IRP03_CFC.DMA_IRP03_CFC_ts.IRP03_GCF_C_R == IRP03_GCF_C_R_B_0x1)
#define Set_DMA_DMA_IRP03_CFC_IRP03_GCF_C_R__B_0x1() (p_DMA_IP->DMA_IRP03_CFC.DMA_IRP03_CFC_ts.IRP03_GCF_C_R = IRP03_GCF_C_R_B_0x1)
#define IRP03_GCF_C_R_B_0x1 0x1    //YUV422 chroma sampling
#define Set_DMA_DMA_IRP03_CFC(IRP03_C_W,IRP03_C1_W,IRP03_C2_W,IRP03_GCF_C_W,IRP03_GCF_C_R) (p_DMA_IP->DMA_IRP03_CFC.word = (uint16_t)IRP03_C_W<<1 | (uint16_t)IRP03_C1_W<<2 | (uint16_t)IRP03_C2_W<<3 | (uint16_t)IRP03_GCF_C_W<<5 | (uint16_t)IRP03_GCF_C_R<<7)
#define Get_DMA_DMA_IRP03_CFC() p_DMA_IP->DMA_IRP03_CFC.word
#define Set_DMA_DMA_IRP03_CFC_word(x) (p_DMA_IP->DMA_IRP03_CFC.word = x)


//DMA_IRP03_RASTER


#define Get_DMA_DMA_IRP03_RASTER_IRP03_L_W() p_DMA_IP->DMA_IRP03_RASTER.DMA_IRP03_RASTER_ts.IRP03_L_W
#define Is_DMA_DMA_IRP03_RASTER_IRP03_L_W_B_0x0() (p_DMA_IP->DMA_IRP03_RASTER.DMA_IRP03_RASTER_ts.IRP03_L_W == IRP03_L_W_B_0x0)
#define Set_DMA_DMA_IRP03_RASTER_IRP03_L_W__B_0x0() (p_DMA_IP->DMA_IRP03_RASTER.DMA_IRP03_RASTER_ts.IRP03_L_W = IRP03_L_W_B_0x0)
#define IRP03_L_W_B_0x0 0x0    //macro-block
#define Is_DMA_DMA_IRP03_RASTER_IRP03_L_W_B_0x1() (p_DMA_IP->DMA_IRP03_RASTER.DMA_IRP03_RASTER_ts.IRP03_L_W == IRP03_L_W_B_0x1)
#define Set_DMA_DMA_IRP03_RASTER_IRP03_L_W__B_0x1() (p_DMA_IP->DMA_IRP03_RASTER.DMA_IRP03_RASTER_ts.IRP03_L_W = IRP03_L_W_B_0x1)
#define IRP03_L_W_B_0x1 0x1    //raster


#define Get_DMA_DMA_IRP03_RASTER_IRP03_C_W() p_DMA_IP->DMA_IRP03_RASTER.DMA_IRP03_RASTER_ts.IRP03_C_W
#define Is_DMA_DMA_IRP03_RASTER_IRP03_C_W_B_0x0() (p_DMA_IP->DMA_IRP03_RASTER.DMA_IRP03_RASTER_ts.IRP03_C_W == IRP03_C_W_B_0x0)
#define Set_DMA_DMA_IRP03_RASTER_IRP03_C_W__B_0x0() (p_DMA_IP->DMA_IRP03_RASTER.DMA_IRP03_RASTER_ts.IRP03_C_W = IRP03_C_W_B_0x0)
#define IRP03_C_W_B_0x0 0x0    //macro-block
#define Is_DMA_DMA_IRP03_RASTER_IRP03_C_W_B_0x1() (p_DMA_IP->DMA_IRP03_RASTER.DMA_IRP03_RASTER_ts.IRP03_C_W == IRP03_C_W_B_0x1)
#define Set_DMA_DMA_IRP03_RASTER_IRP03_C_W__B_0x1() (p_DMA_IP->DMA_IRP03_RASTER.DMA_IRP03_RASTER_ts.IRP03_C_W = IRP03_C_W_B_0x1)
#define IRP03_C_W_B_0x1 0x1    //raster


#define Get_DMA_DMA_IRP03_RASTER_IRP03_C1_W() p_DMA_IP->DMA_IRP03_RASTER.DMA_IRP03_RASTER_ts.IRP03_C1_W
#define Is_DMA_DMA_IRP03_RASTER_IRP03_C1_W_B_0x0() (p_DMA_IP->DMA_IRP03_RASTER.DMA_IRP03_RASTER_ts.IRP03_C1_W == IRP03_C1_W_B_0x0)
#define Set_DMA_DMA_IRP03_RASTER_IRP03_C1_W__B_0x0() (p_DMA_IP->DMA_IRP03_RASTER.DMA_IRP03_RASTER_ts.IRP03_C1_W = IRP03_C1_W_B_0x0)
#define IRP03_C1_W_B_0x0 0x0    //macro-block
#define Is_DMA_DMA_IRP03_RASTER_IRP03_C1_W_B_0x1() (p_DMA_IP->DMA_IRP03_RASTER.DMA_IRP03_RASTER_ts.IRP03_C1_W == IRP03_C1_W_B_0x1)
#define Set_DMA_DMA_IRP03_RASTER_IRP03_C1_W__B_0x1() (p_DMA_IP->DMA_IRP03_RASTER.DMA_IRP03_RASTER_ts.IRP03_C1_W = IRP03_C1_W_B_0x1)
#define IRP03_C1_W_B_0x1 0x1    //raster


#define Get_DMA_DMA_IRP03_RASTER_IRP03_C2_W() p_DMA_IP->DMA_IRP03_RASTER.DMA_IRP03_RASTER_ts.IRP03_C2_W
#define Is_DMA_DMA_IRP03_RASTER_IRP03_C2_W_B_0x0() (p_DMA_IP->DMA_IRP03_RASTER.DMA_IRP03_RASTER_ts.IRP03_C2_W == IRP03_C2_W_B_0x0)
#define Set_DMA_DMA_IRP03_RASTER_IRP03_C2_W__B_0x0() (p_DMA_IP->DMA_IRP03_RASTER.DMA_IRP03_RASTER_ts.IRP03_C2_W = IRP03_C2_W_B_0x0)
#define IRP03_C2_W_B_0x0 0x0    //macro-block
#define Is_DMA_DMA_IRP03_RASTER_IRP03_C2_W_B_0x1() (p_DMA_IP->DMA_IRP03_RASTER.DMA_IRP03_RASTER_ts.IRP03_C2_W == IRP03_C2_W_B_0x1)
#define Set_DMA_DMA_IRP03_RASTER_IRP03_C2_W__B_0x1() (p_DMA_IP->DMA_IRP03_RASTER.DMA_IRP03_RASTER_ts.IRP03_C2_W = IRP03_C2_W_B_0x1)
#define IRP03_C2_W_B_0x1 0x1    //raster
#define Set_DMA_DMA_IRP03_RASTER(IRP03_L_W,IRP03_C_W,IRP03_C1_W,IRP03_C2_W) (p_DMA_IP->DMA_IRP03_RASTER.word = (uint16_t)IRP03_L_W<<0 | (uint16_t)IRP03_C_W<<1 | (uint16_t)IRP03_C1_W<<2 | (uint16_t)IRP03_C2_W<<3)
#define Get_DMA_DMA_IRP03_RASTER() p_DMA_IP->DMA_IRP03_RASTER.word
#define Set_DMA_DMA_IRP03_RASTER_word(x) (p_DMA_IP->DMA_IRP03_RASTER.word = x)


//DMA_IRP03_ROTATION


#define Get_DMA_DMA_IRP03_ROTATION_ROTATION() p_DMA_IP->DMA_IRP03_ROTATION.DMA_IRP03_ROTATION_ts.ROTATION
#define Is_DMA_DMA_IRP03_ROTATION_ROTATION_B_0x0() (p_DMA_IP->DMA_IRP03_ROTATION.DMA_IRP03_ROTATION_ts.ROTATION == ROTATION_B_0x0)
#define Set_DMA_DMA_IRP03_ROTATION_ROTATION__B_0x0() (p_DMA_IP->DMA_IRP03_ROTATION.DMA_IRP03_ROTATION_ts.ROTATION = ROTATION_B_0x0)
#define ROTATION_B_0x0 0x0    //no rotation
#define Is_DMA_DMA_IRP03_ROTATION_ROTATION_B_0x1() (p_DMA_IP->DMA_IRP03_ROTATION.DMA_IRP03_ROTATION_ts.ROTATION == ROTATION_B_0x1)
#define Set_DMA_DMA_IRP03_ROTATION_ROTATION__B_0x1() (p_DMA_IP->DMA_IRP03_ROTATION.DMA_IRP03_ROTATION_ts.ROTATION = ROTATION_B_0x1)
#define ROTATION_B_0x1 0x1    //rotation +90\xfb
#define Is_DMA_DMA_IRP03_ROTATION_ROTATION_B_0x3() (p_DMA_IP->DMA_IRP03_ROTATION.DMA_IRP03_ROTATION_ts.ROTATION == ROTATION_B_0x3)
#define Set_DMA_DMA_IRP03_ROTATION_ROTATION__B_0x3() (p_DMA_IP->DMA_IRP03_ROTATION.DMA_IRP03_ROTATION_ts.ROTATION = ROTATION_B_0x3)
#define ROTATION_B_0x3 0x3    //rotation -90\xfb
#define Set_DMA_DMA_IRP03_ROTATION(ROTATION) (p_DMA_IP->DMA_IRP03_ROTATION.word = (uint32_t)ROTATION<<0)
#define Get_DMA_DMA_IRP03_ROTATION() p_DMA_IP->DMA_IRP03_ROTATION.word
#define Set_DMA_DMA_IRP03_ROTATION_word(x) (p_DMA_IP->DMA_IRP03_ROTATION.word = x)


//DMA_IRP0_SRS_L


#define Get_DMA_DMA_IRP0_SRS_L_DMA_IRP0_SRS_L() p_DMA_IP->DMA_IRP0_SRS_L.DMA_IRP0_SRS_L_ts.DMA_IRP0_SRS_L
#define Set_DMA_DMA_IRP0_SRS_L_DMA_IRP0_SRS_L(x) (p_DMA_IP->DMA_IRP0_SRS_L.DMA_IRP0_SRS_L_ts.DMA_IRP0_SRS_L = x)
#define Set_DMA_DMA_IRP0_SRS_L(DMA_IRP0_SRS_L) (p_DMA_IP->DMA_IRP0_SRS_L.word = (uint16_t)DMA_IRP0_SRS_L<<3)
#define Get_DMA_DMA_IRP0_SRS_L() p_DMA_IP->DMA_IRP0_SRS_L.word
#define Set_DMA_DMA_IRP0_SRS_L_word(x) (p_DMA_IP->DMA_IRP0_SRS_L.word = x)


//DMA_IRP0_SRS_H


#define Get_DMA_DMA_IRP0_SRS_H_DMA_IRP0_SRS_H() p_DMA_IP->DMA_IRP0_SRS_H.DMA_IRP0_SRS_H_ts.DMA_IRP0_SRS_H
#define Set_DMA_DMA_IRP0_SRS_H_DMA_IRP0_SRS_H(x) (p_DMA_IP->DMA_IRP0_SRS_H.DMA_IRP0_SRS_H_ts.DMA_IRP0_SRS_H = x)
#define Set_DMA_DMA_IRP0_SRS_H(DMA_IRP0_SRS_H) (p_DMA_IP->DMA_IRP0_SRS_H.word = (uint16_t)DMA_IRP0_SRS_H<<0)
#define Get_DMA_DMA_IRP0_SRS_H() p_DMA_IP->DMA_IRP0_SRS_H.word
#define Set_DMA_DMA_IRP0_SRS_H_word(x) (p_DMA_IP->DMA_IRP0_SRS_H.word = x)


//DMA_IRP0_SFW


#define Get_DMA_DMA_IRP0_SFW_DMA_IRP0_SFW() p_DMA_IP->DMA_IRP0_SFW.DMA_IRP0_SFW_ts.DMA_IRP0_SFW
#define Set_DMA_DMA_IRP0_SFW_DMA_IRP0_SFW(x) (p_DMA_IP->DMA_IRP0_SFW.DMA_IRP0_SFW_ts.DMA_IRP0_SFW = x)
#define Set_DMA_DMA_IRP0_SFW(DMA_IRP0_SFW) (p_DMA_IP->DMA_IRP0_SFW.word = (uint16_t)DMA_IRP0_SFW<<3)
#define Get_DMA_DMA_IRP0_SFW() p_DMA_IP->DMA_IRP0_SFW.word
#define Set_DMA_DMA_IRP0_SFW_word(x) (p_DMA_IP->DMA_IRP0_SFW.word = x)


//DMA_IRP0_SWW


#define Get_DMA_DMA_IRP0_SWW_DMA_IRP0_SWW() p_DMA_IP->DMA_IRP0_SWW.DMA_IRP0_SWW_ts.DMA_IRP0_SWW
#define Set_DMA_DMA_IRP0_SWW_DMA_IRP0_SWW(x) (p_DMA_IP->DMA_IRP0_SWW.DMA_IRP0_SWW_ts.DMA_IRP0_SWW = x)
#define Set_DMA_DMA_IRP0_SWW(DMA_IRP0_SWW) (p_DMA_IP->DMA_IRP0_SWW.word = (uint16_t)DMA_IRP0_SWW<<3)
#define Get_DMA_DMA_IRP0_SWW() p_DMA_IP->DMA_IRP0_SWW.word
#define Set_DMA_DMA_IRP0_SWW_word(x) (p_DMA_IP->DMA_IRP0_SWW.word = x)


//DMA_IRP0_SWH


#define Get_DMA_DMA_IRP0_SWH_DMA_IRP0_SWH() p_DMA_IP->DMA_IRP0_SWH.DMA_IRP0_SWH_ts.DMA_IRP0_SWH
#define Set_DMA_DMA_IRP0_SWH_DMA_IRP0_SWH(x) (p_DMA_IP->DMA_IRP0_SWH.DMA_IRP0_SWH_ts.DMA_IRP0_SWH = x)
#define Set_DMA_DMA_IRP0_SWH(DMA_IRP0_SWH) (p_DMA_IP->DMA_IRP0_SWH.word = (uint32_t)DMA_IRP0_SWH<<0)
#define Get_DMA_DMA_IRP0_SWH() p_DMA_IP->DMA_IRP0_SWH.word
#define Set_DMA_DMA_IRP0_SWH_word(x) (p_DMA_IP->DMA_IRP0_SWH.word = x)


//DMA_IRP1_DRS_L


#define Get_DMA_DMA_IRP1_DRS_L_DMA_IRP1_DRS_L() p_DMA_IP->DMA_IRP1_DRS_L.DMA_IRP1_DRS_L_ts.DMA_IRP1_DRS_L
#define Set_DMA_DMA_IRP1_DRS_L_DMA_IRP1_DRS_L(x) (p_DMA_IP->DMA_IRP1_DRS_L.DMA_IRP1_DRS_L_ts.DMA_IRP1_DRS_L = x)
#define Set_DMA_DMA_IRP1_DRS_L(DMA_IRP1_DRS_L) (p_DMA_IP->DMA_IRP1_DRS_L.word = (uint16_t)DMA_IRP1_DRS_L<<3)
#define Get_DMA_DMA_IRP1_DRS_L() p_DMA_IP->DMA_IRP1_DRS_L.word
#define Set_DMA_DMA_IRP1_DRS_L_word(x) (p_DMA_IP->DMA_IRP1_DRS_L.word = x)


//DMA_IRP1_DRS_H


#define Get_DMA_DMA_IRP1_DRS_H_DMA_IRP1_DRS_H() p_DMA_IP->DMA_IRP1_DRS_H.DMA_IRP1_DRS_H_ts.DMA_IRP1_DRS_H
#define Set_DMA_DMA_IRP1_DRS_H_DMA_IRP1_DRS_H(x) (p_DMA_IP->DMA_IRP1_DRS_H.DMA_IRP1_DRS_H_ts.DMA_IRP1_DRS_H = x)
#define Set_DMA_DMA_IRP1_DRS_H(DMA_IRP1_DRS_H) (p_DMA_IP->DMA_IRP1_DRS_H.word = (uint16_t)DMA_IRP1_DRS_H<<0)
#define Get_DMA_DMA_IRP1_DRS_H() p_DMA_IP->DMA_IRP1_DRS_H.word
#define Set_DMA_DMA_IRP1_DRS_H_word(x) (p_DMA_IP->DMA_IRP1_DRS_H.word = x)


//DMA_IRP1_DFW


#define Get_DMA_DMA_IRP1_DFW_DMA_IRP1_DFW() p_DMA_IP->DMA_IRP1_DFW.DMA_IRP1_DFW_ts.DMA_IRP1_DFW
#define Set_DMA_DMA_IRP1_DFW_DMA_IRP1_DFW(x) (p_DMA_IP->DMA_IRP1_DFW.DMA_IRP1_DFW_ts.DMA_IRP1_DFW = x)
#define Set_DMA_DMA_IRP1_DFW(DMA_IRP1_DFW) (p_DMA_IP->DMA_IRP1_DFW.word = (uint16_t)DMA_IRP1_DFW<<3)
#define Get_DMA_DMA_IRP1_DFW() p_DMA_IP->DMA_IRP1_DFW.word
#define Set_DMA_DMA_IRP1_DFW_word(x) (p_DMA_IP->DMA_IRP1_DFW.word = x)


//DMA_IRP1_DWW


#define Get_DMA_DMA_IRP1_DWW_DMA_IRP1_DWW() p_DMA_IP->DMA_IRP1_DWW.DMA_IRP1_DWW_ts.DMA_IRP1_DWW
#define Set_DMA_DMA_IRP1_DWW_DMA_IRP1_DWW(x) (p_DMA_IP->DMA_IRP1_DWW.DMA_IRP1_DWW_ts.DMA_IRP1_DWW = x)
#define Set_DMA_DMA_IRP1_DWW(DMA_IRP1_DWW) (p_DMA_IP->DMA_IRP1_DWW.word = (uint16_t)DMA_IRP1_DWW<<3)
#define Get_DMA_DMA_IRP1_DWW() p_DMA_IP->DMA_IRP1_DWW.word
#define Set_DMA_DMA_IRP1_DWW_word(x) (p_DMA_IP->DMA_IRP1_DWW.word = x)


//DMA_IRP1_DWH


#define Get_DMA_DMA_IRP1_DWH_DMA_IRP1_DWH() p_DMA_IP->DMA_IRP1_DWH.DMA_IRP1_DWH_ts.DMA_IRP1_DWH
#define Set_DMA_DMA_IRP1_DWH_DMA_IRP1_DWH(x) (p_DMA_IP->DMA_IRP1_DWH.DMA_IRP1_DWH_ts.DMA_IRP1_DWH = x)
#define Set_DMA_DMA_IRP1_DWH(DMA_IRP1_DWH) (p_DMA_IP->DMA_IRP1_DWH.word = (uint32_t)DMA_IRP1_DWH<<0)
#define Get_DMA_DMA_IRP1_DWH() p_DMA_IP->DMA_IRP1_DWH.word
#define Set_DMA_DMA_IRP1_DWH_word(x) (p_DMA_IP->DMA_IRP1_DWH.word = x)


//DMA_IRP2_DRS_L


#define Get_DMA_DMA_IRP2_DRS_L_DMA_IRP2_DRS_L() p_DMA_IP->DMA_IRP2_DRS_L.DMA_IRP2_DRS_L_ts.DMA_IRP2_DRS_L
#define Set_DMA_DMA_IRP2_DRS_L_DMA_IRP2_DRS_L(x) (p_DMA_IP->DMA_IRP2_DRS_L.DMA_IRP2_DRS_L_ts.DMA_IRP2_DRS_L = x)
#define Set_DMA_DMA_IRP2_DRS_L(DMA_IRP2_DRS_L) (p_DMA_IP->DMA_IRP2_DRS_L.word = (uint16_t)DMA_IRP2_DRS_L<<3)
#define Get_DMA_DMA_IRP2_DRS_L() p_DMA_IP->DMA_IRP2_DRS_L.word
#define Set_DMA_DMA_IRP2_DRS_L_word(x) (p_DMA_IP->DMA_IRP2_DRS_L.word = x)


//DMA_IRP2_DRS_H


#define Get_DMA_DMA_IRP2_DRS_H_DMA_IRP2_DRS_H() p_DMA_IP->DMA_IRP2_DRS_H.DMA_IRP2_DRS_H_ts.DMA_IRP2_DRS_H
#define Set_DMA_DMA_IRP2_DRS_H_DMA_IRP2_DRS_H(x) (p_DMA_IP->DMA_IRP2_DRS_H.DMA_IRP2_DRS_H_ts.DMA_IRP2_DRS_H = x)
#define Set_DMA_DMA_IRP2_DRS_H(DMA_IRP2_DRS_H) (p_DMA_IP->DMA_IRP2_DRS_H.word = (uint16_t)DMA_IRP2_DRS_H<<0)
#define Get_DMA_DMA_IRP2_DRS_H() p_DMA_IP->DMA_IRP2_DRS_H.word
#define Set_DMA_DMA_IRP2_DRS_H_word(x) (p_DMA_IP->DMA_IRP2_DRS_H.word = x)


//DMA_IRP2_DFW


#define Get_DMA_DMA_IRP2_DFW_DMA_IRP2_DFW() p_DMA_IP->DMA_IRP2_DFW.DMA_IRP2_DFW_ts.DMA_IRP2_DFW
#define Set_DMA_DMA_IRP2_DFW_DMA_IRP2_DFW(x) (p_DMA_IP->DMA_IRP2_DFW.DMA_IRP2_DFW_ts.DMA_IRP2_DFW = x)
#define Set_DMA_DMA_IRP2_DFW(DMA_IRP2_DFW) (p_DMA_IP->DMA_IRP2_DFW.word = (uint16_t)DMA_IRP2_DFW<<3)
#define Get_DMA_DMA_IRP2_DFW() p_DMA_IP->DMA_IRP2_DFW.word
#define Set_DMA_DMA_IRP2_DFW_word(x) (p_DMA_IP->DMA_IRP2_DFW.word = x)


//DMA_IRP2_DWW


#define Get_DMA_DMA_IRP2_DWW_DMA_IRP2_DWW() p_DMA_IP->DMA_IRP2_DWW.DMA_IRP2_DWW_ts.DMA_IRP2_DWW
#define Set_DMA_DMA_IRP2_DWW_DMA_IRP2_DWW(x) (p_DMA_IP->DMA_IRP2_DWW.DMA_IRP2_DWW_ts.DMA_IRP2_DWW = x)
#define Set_DMA_DMA_IRP2_DWW(DMA_IRP2_DWW) (p_DMA_IP->DMA_IRP2_DWW.word = (uint16_t)DMA_IRP2_DWW<<3)
#define Get_DMA_DMA_IRP2_DWW() p_DMA_IP->DMA_IRP2_DWW.word
#define Set_DMA_DMA_IRP2_DWW_word(x) (p_DMA_IP->DMA_IRP2_DWW.word = x)


//DMA_IRP2_DWH


#define Get_DMA_DMA_IRP2_DWH_DMA_IRP2_DWH() p_DMA_IP->DMA_IRP2_DWH.DMA_IRP2_DWH_ts.DMA_IRP2_DWH
#define Set_DMA_DMA_IRP2_DWH_DMA_IRP2_DWH(x) (p_DMA_IP->DMA_IRP2_DWH.DMA_IRP2_DWH_ts.DMA_IRP2_DWH = x)
#define Set_DMA_DMA_IRP2_DWH(DMA_IRP2_DWH) (p_DMA_IP->DMA_IRP2_DWH.word = (uint32_t)DMA_IRP2_DWH<<0)
#define Get_DMA_DMA_IRP2_DWH() p_DMA_IP->DMA_IRP2_DWH.word
#define Set_DMA_DMA_IRP2_DWH_word(x) (p_DMA_IP->DMA_IRP2_DWH.word = x)


//DMA_TST_CID


#define Set_DMA_DMA_TST_CID_DMA_TST_CID(x) (p_DMA_IP->DMA_TST_CID.DMA_TST_CID_ts.DMA_TST_CID = x)
#define Set_DMA_DMA_TST_CID(DMA_TST_CID) (p_DMA_IP->DMA_TST_CID.word = (uint32_t)DMA_TST_CID<<0)
#define Get_DMA_DMA_TST_CID() p_DMA_IP->DMA_TST_CID.word
#define Set_DMA_DMA_TST_CID_word(x) (p_DMA_IP->DMA_TST_CID.word = x)


//DMA_TST_SADD_L


#define Get_DMA_DMA_TST_SADD_L_CURADDR_L() p_DMA_IP->DMA_TST_SADD_L.DMA_TST_SADD_L_ts.CURADDR_L
#define Get_DMA_DMA_TST_SADD_L() p_DMA_IP->DMA_TST_SADD_L.word


//DMA_TST_SADD_H


#define Get_DMA_DMA_TST_SADD_H_CURADDR_H() p_DMA_IP->DMA_TST_SADD_H.DMA_TST_SADD_H_ts.CURADDR_H
#define Get_DMA_DMA_TST_SADD_H() p_DMA_IP->DMA_TST_SADD_H.word

extern volatile DMA_IP_ts *p_DMA_IP;

#endif 

