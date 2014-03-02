#ifndef _SIAhwReg_
#define _SIAhwReg_

#include "Platform.h"



//SIA_RESET
typedef union
{
    uint32_t word;
    uint16_t data;
    struct
    {
        uint8_t SIA_CORE :1;
        /* SIA_CORE (DMA, STBP, VCF,ROT, PROJ) reset bit: read returns 0. Note LICN is not reset by this soft reset.*/
        uint8_t IPP :1;
        /* IPP reset bit: read returns 0*/
        uint8_t VCF :1;
        /* VCF soft reset bit: read returns 0*/
        uint8_t PROJ :1;
        /* PROJ soft reset bit: read returns 0*/
        uint8_t ROT :1;
        /* ROT soft reset bit: read returns 0*/
    }SIA_RESET_ts;

}SIA_RESET_tu;



//SIA_CLK_ENABLE
typedef union
{
    uint32_t word;
    uint16_t data;
    struct
    {
        uint8_t DMA :1;
        /* enable DMA input clock:
        read returns last written value*/
        uint8_t STBP :1;
        /* enable STBP input clock:
        read returns last written value*/
        uint8_t IPP :1;
        /* enable IPP input clock:
        read returns last written value*/
        uint8_t VCF :1;
        /* enable VCF input clock:
        read returns last written value*/
        uint8_t PROJ :1;
        /* enable PROJ input clock (note STBP
        read returns last written value*/
    }SIA_CLK_ENABLE_ts;

}SIA_CLK_ENABLE_tu;



//SIA_IDN_HRV
typedef union
{
    uint32_t word;
    uint16_t data;
    struct
    {
        uint8_t R0 :4;
        /* minor revision number:
        read returns actual metal fix revision of SIA, write has no effect
        This field shall have logic provision to be changed by metal fix.*/
        uint8_t R1 :4;
        /* minor revision number (micro-architecture / RTL change):
        read returns actual minor revision of SIA
        write has no effect*/
        uint8_t R2 :4;
        /* major revision number (architecture change):
        read returns actual major revision of SIA
        write has no effect*/
        uint8_t DXO :1;
        /* Status of DxO Fuse:*/
    }SIA_IDN_HRV_ts;

}SIA_IDN_HRV_tu;



//SIA_VCF_ENABLE
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t EN :1;
        /* Vertical chroma filtering (VCF) enable. Read returns last written value.*/
    }SIA_VCF_ENABLE_ts;

}SIA_VCF_ENABLE_tu;



//SIA_ROTATION
typedef union
{
    uint32_t word;
    uint16_t data;
    struct
    {
        uint8_t ROTATION :2;
        /* rotation of the frame*/
    }SIA_ROTATION_ts;

}SIA_ROTATION_tu;



//ISP_MCU_SYS_ADDR_0_HI
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t REGION_BA_HI;
        /* MSBs of base address in system memory map of logical region x*/
    }ISP_MCU_SYS_ADDR_0_HI_ts;

}ISP_MCU_SYS_ADDR_0_HI_tu;



//ISP_MCU_SYS_SIZE0
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t REGION_SZ :3;
        /* size of logical region x*/
    }ISP_MCU_SYS_SIZE0_ts;

}ISP_MCU_SYS_SIZE0_tu;



//ISP_MCU_SYS_ADDR_1_HI
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t REGION_BA_HI;
        /* MSBs of base address in system memory map of logical region x*/
    }ISP_MCU_SYS_ADDR_1_HI_ts;

}ISP_MCU_SYS_ADDR_1_HI_tu;



//ISP_MCU_SYS_SIZE1
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t REGION_SZ :3;
        /* size of logical region x*/
    }ISP_MCU_SYS_SIZE1_ts;

}ISP_MCU_SYS_SIZE1_tu;



//ISP_MCU_SYS_ADDR_2_HI
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t REGION_BA_HI;
        /* MSBs of base address in system memory map of logical region x*/
    }ISP_MCU_SYS_ADDR_2_HI_ts;

}ISP_MCU_SYS_ADDR_2_HI_tu;



//ISP_MCU_SYS_SIZE2
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t REGION_SZ :3;
        /* size of logical region x*/
    }ISP_MCU_SYS_SIZE2_ts;

}ISP_MCU_SYS_SIZE2_tu;



//ISP_MCU_SYS_ADDR_3_HI
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t REGION_BA_HI;
        /* MSBs of base address in system memory map of logical region x*/
    }ISP_MCU_SYS_ADDR_3_HI_ts;

}ISP_MCU_SYS_ADDR_3_HI_tu;



//ISP_MCU_SYS_SIZE3
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t REGION_SZ :3;
        /* size of logical region x*/
    }ISP_MCU_SYS_SIZE3_ts;

}ISP_MCU_SYS_SIZE3_tu;



//ISP_MCU_SYS_ADDR_4_HI
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t REGION_BA_HI;
        /* MSBs of base address in system memory map of logical region x*/
    }ISP_MCU_SYS_ADDR_4_HI_ts;

}ISP_MCU_SYS_ADDR_4_HI_tu;



//ISP_MCU_SYS_SIZE4
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t REGION_SZ :3;
        /* size of logical region x*/
    }ISP_MCU_SYS_SIZE4_ts;

}ISP_MCU_SYS_SIZE4_tu;



//ISP_MCU_SYS_ADDR_5_HI
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t REGION_BA_HI;
        /* MSBs of base address in system memory map of logical region x*/
    }ISP_MCU_SYS_ADDR_5_HI_ts;

}ISP_MCU_SYS_ADDR_5_HI_tu;



//ISP_MCU_SYS_SIZE5
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t REGION_SZ :3;
        /* size of logical region x*/
    }ISP_MCU_SYS_SIZE5_ts;

}ISP_MCU_SYS_SIZE5_tu;



//ISP_MCU_SYS_ADDR_6_HI
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t REGION_BA_HI;
        /* MSBs of base address in system memory map of logical region x*/
    }ISP_MCU_SYS_ADDR_6_HI_ts;

}ISP_MCU_SYS_ADDR_6_HI_tu;



//ISP_MCU_SYS_SIZE6
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t REGION_SZ :3;
        /* size of logical region x*/
    }ISP_MCU_SYS_SIZE6_ts;

}ISP_MCU_SYS_SIZE6_tu;



//ISP_MCU_SYS_ADDR_7_HI
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t REGION_BA_HI;
        /* MSBs of base address in system memory map of logical region x*/
    }ISP_MCU_SYS_ADDR_7_HI_ts;

}ISP_MCU_SYS_ADDR_7_HI_tu;



//ISP_MCU_SYS_SIZE7
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t REGION_SZ :3;
        /* size of logical region x*/
    }ISP_MCU_SYS_SIZE7_ts;

}ISP_MCU_SYS_SIZE7_tu;



//ISP_MCU_IO_ADDR_0_HI
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t REGION_BA_HI;
        /* MSBs of base address in system memory map of logical IO region x*/
    }ISP_MCU_IO_ADDR_0_HI_ts;

}ISP_MCU_IO_ADDR_0_HI_tu;



//ISP_MCU_IO_ADDR_1_HI
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t REGION_BA_HI;
        /* MSBs of base address in system memory map of logical IO region x*/
    }ISP_MCU_IO_ADDR_1_HI_ts;

}ISP_MCU_IO_ADDR_1_HI_tu;



//ISP_MCU_IO_ADDR_2_HI
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t REGION_BA_HI;
        /* MSBs of base address in system memory map of logical IO region x*/
    }ISP_MCU_IO_ADDR_2_HI_ts;

}ISP_MCU_IO_ADDR_2_HI_tu;



//ISP_MCU_IO_ADDR_3_HI
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t REGION_BA_HI;
        /* MSBs of base address in system memory map of logical IO region x*/
    }ISP_MCU_IO_ADDR_3_HI_ts;

}ISP_MCU_IO_ADDR_3_HI_tu;



//ISP_MCU_IO_ADDR_4_HI
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t REGION_BA_HI;
        /* MSBs of base address in system memory map of logical IO region x*/
    }ISP_MCU_IO_ADDR_4_HI_ts;

}ISP_MCU_IO_ADDR_4_HI_tu;



//ISP_MCU_IO_ADDR_5_HI
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t REGION_BA_HI;
        /* MSBs of base address in system memory map of logical IO region x*/
    }ISP_MCU_IO_ADDR_5_HI_ts;

}ISP_MCU_IO_ADDR_5_HI_tu;



//ISP_MCU_IO_ADDR_6_HI
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t REGION_BA_HI;
        /* MSBs of base address in system memory map of logical IO region x*/
    }ISP_MCU_IO_ADDR_6_HI_ts;

}ISP_MCU_IO_ADDR_6_HI_tu;



//ISP_MCU_IO_ADDR_7_HI
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t REGION_BA_HI;
        /* MSBs of base address in system memory map of logical IO region x*/
    }ISP_MCU_IO_ADDR_7_HI_ts;

}ISP_MCU_IO_ADDR_7_HI_tu;



//ISP_MEM_PAGE
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t INDEX :6;
        /* page index to the ISP memories*/
    }ISP_MEM_PAGE_ts;

}ISP_MEM_PAGE_tu;



//SIA_XBUS_BYTE_ENABLE
typedef union
{
    uint32_t word;
    uint16_t data;
    struct
    {
        uint8_t BE0 :2;
        /* Byte-enable:*/
        uint8_t reserved0 :6;
        uint8_t BE1 :2;
        /* Byte-enable:*/
    }SIA_XBUS_BYTE_ENABLE_ts;

}SIA_XBUS_BYTE_ENABLE_tu;



//SIA_PIPE03_SELECT
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t AP :1;
        /* Active Pipe:*/
    }SIA_PIPE03_SELECT_ts;

}SIA_PIPE03_SELECT_tu;



//DXO_PDP_PAGE
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t INDEX :3;
        /* page index to the DxO PDP registers*/
    }DXO_PDP_PAGE_ts;

}DXO_PDP_PAGE_tu;



//DXO_DPP_PAGE
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t INDEX :6;
        /* page index to the DxO DPP registers*/
    }DXO_DPP_PAGE_ts;

}DXO_DPP_PAGE_tu;



//DXO_DOP7_PAGE
typedef union
{
    uint32_t word;
    uint16_t data;
    struct
    {
        uint8_t INDEX :5;
        /* page index to the DxO DOP7 registers*/
    }DXO_DOP7_PAGE_ts;

}DXO_DOP7_PAGE_tu;



//LICN_ITS
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t BUSERR :1;
        /* LICN bus error on MMDSP access*/
    }LICN_ITS_ts;

}LICN_ITS_tu;



//LICN_ITS_BCLR
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t BUSERR :1;
        /* no effect*/
    }LICN_ITS_BCLR_ts;

}LICN_ITS_BCLR_tu;



//LICN_ITS_BSET
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t BUSERR :1;
        /* no effect*/
    }LICN_ITS_BSET_ts;

}LICN_ITS_BSET_tu;



//LICN_ITM
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t BUSERR :1;
        /* does not contribute to LICN interrupt*/
    }LICN_ITM_ts;

}LICN_ITM_tu;



//LICN_ITM_BCLR
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t BUSERR :1;
        /* no effect*/
    }LICN_ITM_BCLR_ts;

}LICN_ITM_BCLR_tu;



//LICN_ITM_BSET
typedef union
{
    uint32_t word;
    uint16_t data;
    struct
    {
        uint8_t BUSERR :1;
        /* no effect*/
    }LICN_ITM_BSET_ts;

}LICN_ITM_BSET_tu;



//ISP_MCU_SYS_ADDR_0_LO
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t reserved0 :12;
        uint8_t REGION_BA_LO :4;
        /* LSBs of base address in system memory map of logical region x*/
    }ISP_MCU_SYS_ADDR_0_LO_ts;

}ISP_MCU_SYS_ADDR_0_LO_tu;



//ISP_MCU_SYS_ADDR_1_LO
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t reserved0 :12;
        uint8_t REGION_BA_LO :4;
        /* LSBs of base address in system memory map of logical region x*/
    }ISP_MCU_SYS_ADDR_1_LO_ts;

}ISP_MCU_SYS_ADDR_1_LO_tu;



//ISP_MCU_SYS_ADDR_2_LO
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t reserved0 :12;
        uint8_t REGION_BA_LO :4;
        /* LSBs of base address in system memory map of logical region x*/
    }ISP_MCU_SYS_ADDR_2_LO_ts;

}ISP_MCU_SYS_ADDR_2_LO_tu;



//ISP_MCU_SYS_ADDR_3_LO
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t reserved0 :12;
        uint8_t REGION_BA_LO :4;
        /* LSBs of base address in system memory map of logical region x*/
    }ISP_MCU_SYS_ADDR_3_LO_ts;

}ISP_MCU_SYS_ADDR_3_LO_tu;



//ISP_MCU_SYS_ADDR_4_LO
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t reserved0 :12;
        uint8_t REGION_BA_LO :4;
        /* LSBs of base address in system memory map of logical region x*/
    }ISP_MCU_SYS_ADDR_4_LO_ts;

}ISP_MCU_SYS_ADDR_4_LO_tu;



//ISP_MCU_SYS_ADDR_5_LO
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t reserved0 :12;
        uint8_t REGION_BA_LO :4;
        /* LSBs of base address in system memory map of logical region x*/
    }ISP_MCU_SYS_ADDR_5_LO_ts;

}ISP_MCU_SYS_ADDR_5_LO_tu;



//ISP_MCU_SYS_ADDR_6_LO
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t reserved0 :12;
        uint8_t REGION_BA_LO :4;
        /* LSBs of base address in system memory map of logical region x*/
    }ISP_MCU_SYS_ADDR_6_LO_ts;

}ISP_MCU_SYS_ADDR_6_LO_tu;



//ISP_MCU_SYS_ADDR_7_LO
typedef union
{
    uint32_t word;
    uint16_t data;
    struct
    {
        uint16_t reserved0 :12;
        uint8_t REGION_BA_LO :4;
        /* LSBs of base address in system memory map of logical region x*/
    }ISP_MCU_SYS_ADDR_7_LO_ts;

}ISP_MCU_SYS_ADDR_7_LO_tu;



//ISP_MCU_IO_ADDR_0_LO
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t reserved0 :12;
        uint8_t REGION_BA_LO :4;
        /* LSBs of base address in system memory map of logical IO region x*/
    }ISP_MCU_IO_ADDR_0_LO_ts;

}ISP_MCU_IO_ADDR_0_LO_tu;



//ISP_MCU_IO_ADDR_1_LO
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t reserved0 :12;
        uint8_t REGION_BA_LO :4;
        /* LSBs of base address in system memory map of logical IO region x*/
    }ISP_MCU_IO_ADDR_1_LO_ts;

}ISP_MCU_IO_ADDR_1_LO_tu;



//ISP_MCU_IO_ADDR_2_LO
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t reserved0 :12;
        uint8_t REGION_BA_LO :4;
        /* LSBs of base address in system memory map of logical IO region x*/
    }ISP_MCU_IO_ADDR_2_LO_ts;

}ISP_MCU_IO_ADDR_2_LO_tu;



//ISP_MCU_IO_ADDR_3_LO
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t reserved0 :12;
        uint8_t REGION_BA_LO :4;
        /* LSBs of base address in system memory map of logical IO region x*/
    }ISP_MCU_IO_ADDR_3_LO_ts;

}ISP_MCU_IO_ADDR_3_LO_tu;



//ISP_MCU_IO_ADDR_4_LO
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t reserved0 :12;
        uint8_t REGION_BA_LO :4;
        /* LSBs of base address in system memory map of logical IO region x*/
    }ISP_MCU_IO_ADDR_4_LO_ts;

}ISP_MCU_IO_ADDR_4_LO_tu;



//ISP_MCU_IO_ADDR_5_LO
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t reserved0 :12;
        uint8_t REGION_BA_LO :4;
        /* LSBs of base address in system memory map of logical IO region x*/
    }ISP_MCU_IO_ADDR_5_LO_ts;

}ISP_MCU_IO_ADDR_5_LO_tu;



//ISP_MCU_IO_ADDR_6_LO
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint16_t reserved0 :12;
        uint8_t REGION_BA_LO :4;
        /* LSBs of base address in system memory map of logical IO region x*/
    }ISP_MCU_IO_ADDR_6_LO_ts;

}ISP_MCU_IO_ADDR_6_LO_tu;



//ISP_MCU_IO_ADDR_7_LO
typedef union
{
    uint32_t word;
    uint16_t data;
    struct
    {
        uint16_t reserved0 :12;
        uint8_t REGION_BA_LO :4;
        /* LSBs of base address in system memory map of logical IO region x*/
    }ISP_MCU_IO_ADDR_7_LO_ts;

}ISP_MCU_IO_ADDR_7_LO_tu;



//ISP_MCU_IO_SIZE0
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t REGION_SZ :3;
        /* size of logical region x*/
    }ISP_MCU_IO_SIZE0_ts;

}ISP_MCU_IO_SIZE0_tu;



//ISP_MCU_IO_SIZE1
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t REGION_SZ :3;
        /* size of logical region x*/
    }ISP_MCU_IO_SIZE1_ts;

}ISP_MCU_IO_SIZE1_tu;



//ISP_MCU_IO_SIZE2
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t REGION_SZ :3;
        /* size of logical region x*/
    }ISP_MCU_IO_SIZE2_ts;

}ISP_MCU_IO_SIZE2_tu;



//ISP_MCU_IO_SIZE3
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t REGION_SZ :3;
        /* size of logical region x*/
    }ISP_MCU_IO_SIZE3_ts;

}ISP_MCU_IO_SIZE3_tu;



//ISP_MCU_IO_SIZE4
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t REGION_SZ :3;
        /* size of logical region x*/
    }ISP_MCU_IO_SIZE4_ts;

}ISP_MCU_IO_SIZE4_tu;



//ISP_MCU_IO_SIZE5
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t REGION_SZ :3;
        /* size of logical region x*/
    }ISP_MCU_IO_SIZE5_ts;

}ISP_MCU_IO_SIZE5_tu;



//ISP_MCU_IO_SIZE6
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t REGION_SZ :3;
        /* size of logical region x*/
    }ISP_MCU_IO_SIZE6_ts;

}ISP_MCU_IO_SIZE6_tu;



//ISP_MCU_IO_SIZE7
typedef union
{
    uint16_t word;
    uint16_t data;
    struct
    {
        uint8_t REGION_SZ :3;
        /* size of logical region x*/
    }ISP_MCU_IO_SIZE7_ts;

}ISP_MCU_IO_SIZE7_tu;

typedef struct
{
    SIA_RESET_tu SIA_RESET;
    uint32_t pad_SIA_CLK_ENABLE[3];
    SIA_CLK_ENABLE_tu SIA_CLK_ENABLE;
    uint32_t pad_SIA_IDN_HRV[3];
    SIA_IDN_HRV_tu SIA_IDN_HRV;
    uint32_t pad_SIA_VCF_ENABLE[3];
    SIA_VCF_ENABLE_tu SIA_VCF_ENABLE;
    SIA_ROTATION_tu SIA_ROTATION;
    uint32_t pad_ISP_MCU_SYS_ADDR_0_HI[2];
    ISP_MCU_SYS_ADDR_0_HI_tu ISP_MCU_SYS_ADDR_0_HI;
    ISP_MCU_SYS_SIZE0_tu ISP_MCU_SYS_SIZE0;
    ISP_MCU_SYS_ADDR_1_HI_tu ISP_MCU_SYS_ADDR_1_HI;
    ISP_MCU_SYS_SIZE1_tu ISP_MCU_SYS_SIZE1;
    ISP_MCU_SYS_ADDR_2_HI_tu ISP_MCU_SYS_ADDR_2_HI;
    ISP_MCU_SYS_SIZE2_tu ISP_MCU_SYS_SIZE2;
    ISP_MCU_SYS_ADDR_3_HI_tu ISP_MCU_SYS_ADDR_3_HI;
    ISP_MCU_SYS_SIZE3_tu ISP_MCU_SYS_SIZE3;
    ISP_MCU_SYS_ADDR_4_HI_tu ISP_MCU_SYS_ADDR_4_HI;
    ISP_MCU_SYS_SIZE4_tu ISP_MCU_SYS_SIZE4;
    ISP_MCU_SYS_ADDR_5_HI_tu ISP_MCU_SYS_ADDR_5_HI;
    ISP_MCU_SYS_SIZE5_tu ISP_MCU_SYS_SIZE5;
    ISP_MCU_SYS_ADDR_6_HI_tu ISP_MCU_SYS_ADDR_6_HI;
    ISP_MCU_SYS_SIZE6_tu ISP_MCU_SYS_SIZE6;
    ISP_MCU_SYS_ADDR_7_HI_tu ISP_MCU_SYS_ADDR_7_HI;
    ISP_MCU_SYS_SIZE7_tu ISP_MCU_SYS_SIZE7;
    ISP_MCU_IO_ADDR_0_HI_tu ISP_MCU_IO_ADDR_0_HI;
    ISP_MCU_IO_ADDR_1_HI_tu ISP_MCU_IO_ADDR_1_HI;
    ISP_MCU_IO_ADDR_2_HI_tu ISP_MCU_IO_ADDR_2_HI;
    ISP_MCU_IO_ADDR_3_HI_tu ISP_MCU_IO_ADDR_3_HI;
    ISP_MCU_IO_ADDR_4_HI_tu ISP_MCU_IO_ADDR_4_HI;
    ISP_MCU_IO_ADDR_5_HI_tu ISP_MCU_IO_ADDR_5_HI;
    ISP_MCU_IO_ADDR_6_HI_tu ISP_MCU_IO_ADDR_6_HI;
    ISP_MCU_IO_ADDR_7_HI_tu ISP_MCU_IO_ADDR_7_HI;
    ISP_MEM_PAGE_tu ISP_MEM_PAGE;
    SIA_XBUS_BYTE_ENABLE_tu SIA_XBUS_BYTE_ENABLE;
    uint32_t pad_SIA_PIPE03_SELECT[2];
    SIA_PIPE03_SELECT_tu SIA_PIPE03_SELECT;
    DXO_PDP_PAGE_tu DXO_PDP_PAGE;
    DXO_DPP_PAGE_tu DXO_DPP_PAGE;
    DXO_DOP7_PAGE_tu DXO_DOP7_PAGE;
    uint32_t pad_LICN_ITS;
    LICN_ITS_tu LICN_ITS;
    LICN_ITS_BCLR_tu LICN_ITS_BCLR;
    LICN_ITS_BSET_tu LICN_ITS_BSET;
    LICN_ITM_tu LICN_ITM;
    LICN_ITM_BCLR_tu LICN_ITM_BCLR;
    LICN_ITM_BSET_tu LICN_ITM_BSET;
    ISP_MCU_SYS_ADDR_0_LO_tu ISP_MCU_SYS_ADDR_0_LO;
    ISP_MCU_SYS_ADDR_1_LO_tu ISP_MCU_SYS_ADDR_1_LO;
    ISP_MCU_SYS_ADDR_2_LO_tu ISP_MCU_SYS_ADDR_2_LO;
    ISP_MCU_SYS_ADDR_3_LO_tu ISP_MCU_SYS_ADDR_3_LO;
    ISP_MCU_SYS_ADDR_4_LO_tu ISP_MCU_SYS_ADDR_4_LO;
    ISP_MCU_SYS_ADDR_5_LO_tu ISP_MCU_SYS_ADDR_5_LO;
    ISP_MCU_SYS_ADDR_6_LO_tu ISP_MCU_SYS_ADDR_6_LO;
    ISP_MCU_SYS_ADDR_7_LO_tu ISP_MCU_SYS_ADDR_7_LO;
    uint32_t pad_ISP_MCU_IO_ADDR_0_LO[3];
    ISP_MCU_IO_ADDR_0_LO_tu ISP_MCU_IO_ADDR_0_LO;
    ISP_MCU_IO_ADDR_1_LO_tu ISP_MCU_IO_ADDR_1_LO;
    ISP_MCU_IO_ADDR_2_LO_tu ISP_MCU_IO_ADDR_2_LO;
    ISP_MCU_IO_ADDR_3_LO_tu ISP_MCU_IO_ADDR_3_LO;
    ISP_MCU_IO_ADDR_4_LO_tu ISP_MCU_IO_ADDR_4_LO;
    ISP_MCU_IO_ADDR_5_LO_tu ISP_MCU_IO_ADDR_5_LO;
    ISP_MCU_IO_ADDR_6_LO_tu ISP_MCU_IO_ADDR_6_LO;
    ISP_MCU_IO_ADDR_7_LO_tu ISP_MCU_IO_ADDR_7_LO;
    uint32_t pad_ISP_MCU_IO_SIZE0[3];
    ISP_MCU_IO_SIZE0_tu ISP_MCU_IO_SIZE0;
    ISP_MCU_IO_SIZE1_tu ISP_MCU_IO_SIZE1;
    ISP_MCU_IO_SIZE2_tu ISP_MCU_IO_SIZE2;
    ISP_MCU_IO_SIZE3_tu ISP_MCU_IO_SIZE3;
    ISP_MCU_IO_SIZE4_tu ISP_MCU_IO_SIZE4;
    ISP_MCU_IO_SIZE5_tu ISP_MCU_IO_SIZE5;
    ISP_MCU_IO_SIZE6_tu ISP_MCU_IO_SIZE6;
    ISP_MCU_IO_SIZE7_tu ISP_MCU_IO_SIZE7;
}_IP_ts;




//SIA_RESET


#define Get__SIA_RESET_SIA_CORE() p__IP->SIA_RESET.SIA_RESET_ts.SIA_CORE
#define Is__SIA_RESET_SIA_CORE_B_0x0() (p__IP->SIA_RESET.SIA_RESET_ts.SIA_CORE == SIA_CORE_B_0x0)
#define Set__SIA_RESET_SIA_CORE__B_0x0() (p__IP->SIA_RESET.SIA_RESET_ts.SIA_CORE = SIA_CORE_B_0x0)
#define SIA_CORE_B_0x0 0x0    //no effect
#define Is__SIA_RESET_SIA_CORE_B_0x1() (p__IP->SIA_RESET.SIA_RESET_ts.SIA_CORE == SIA_CORE_B_0x1)
#define Set__SIA_RESET_SIA_CORE__B_0x1() (p__IP->SIA_RESET.SIA_RESET_ts.SIA_CORE = SIA_CORE_B_0x1)
#define SIA_CORE_B_0x1 0x1    //applies reset to module. Write is blocking until after hard reset has been applied.


#define Get__SIA_RESET_IPP() p__IP->SIA_RESET.SIA_RESET_ts.IPP
#define Is__SIA_RESET_IPP_B_0x0() (p__IP->SIA_RESET.SIA_RESET_ts.IPP == IPP_B_0x0)
#define Set__SIA_RESET_IPP__B_0x0() (p__IP->SIA_RESET.SIA_RESET_ts.IPP = IPP_B_0x0)
#define IPP_B_0x0 0x0    //no effect
#define Is__SIA_RESET_IPP_B_0x1() (p__IP->SIA_RESET.SIA_RESET_ts.IPP == IPP_B_0x1)
#define Set__SIA_RESET_IPP__B_0x1() (p__IP->SIA_RESET.SIA_RESET_ts.IPP = IPP_B_0x1)
#define IPP_B_0x1 0x1    //applies reset to module. Write is blocking until after hard reset has been applied.


#define Get__SIA_RESET_VCF() p__IP->SIA_RESET.SIA_RESET_ts.VCF
#define Is__SIA_RESET_VCF_B_0x0() (p__IP->SIA_RESET.SIA_RESET_ts.VCF == VCF_B_0x0)
#define Set__SIA_RESET_VCF__B_0x0() (p__IP->SIA_RESET.SIA_RESET_ts.VCF = VCF_B_0x0)
#define VCF_B_0x0 0x0    //no effect
#define Is__SIA_RESET_VCF_B_0x1() (p__IP->SIA_RESET.SIA_RESET_ts.VCF == VCF_B_0x1)
#define Set__SIA_RESET_VCF__B_0x1() (p__IP->SIA_RESET.SIA_RESET_ts.VCF = VCF_B_0x1)
#define VCF_B_0x1 0x1    //applies reset to module. Write is blocking until after hard reset has been applied.


#define Get__SIA_RESET_PROJ() p__IP->SIA_RESET.SIA_RESET_ts.PROJ
#define Is__SIA_RESET_PROJ_B_0x0() (p__IP->SIA_RESET.SIA_RESET_ts.PROJ == PROJ_B_0x0)
#define Set__SIA_RESET_PROJ__B_0x0() (p__IP->SIA_RESET.SIA_RESET_ts.PROJ = PROJ_B_0x0)
#define PROJ_B_0x0 0x0    //no effect
#define Is__SIA_RESET_PROJ_B_0x1() (p__IP->SIA_RESET.SIA_RESET_ts.PROJ == PROJ_B_0x1)
#define Set__SIA_RESET_PROJ__B_0x1() (p__IP->SIA_RESET.SIA_RESET_ts.PROJ = PROJ_B_0x1)
#define PROJ_B_0x1 0x1    //applies reset to module. Write is blocking until after hard reset has been applied.


#define Get__SIA_RESET_ROT() p__IP->SIA_RESET.SIA_RESET_ts.ROT
#define Is__SIA_RESET_ROT_B_0x0() (p__IP->SIA_RESET.SIA_RESET_ts.ROT == ROT_B_0x0)
#define Set__SIA_RESET_ROT__B_0x0() (p__IP->SIA_RESET.SIA_RESET_ts.ROT = ROT_B_0x0)
#define ROT_B_0x0 0x0    //no effect
#define Is__SIA_RESET_ROT_B_0x1() (p__IP->SIA_RESET.SIA_RESET_ts.ROT == ROT_B_0x1)
#define Set__SIA_RESET_ROT__B_0x1() (p__IP->SIA_RESET.SIA_RESET_ts.ROT = ROT_B_0x1)
#define ROT_B_0x1 0x1    //applies reset to module. Write is blocking until after hard reset has been applied.
#define Set__SIA_RESET(SIA_CORE,IPP,VCF,PROJ,ROT) (p__IP->SIA_RESET.word = (uint32_t)SIA_CORE<<0 | (uint32_t)IPP<<1 | (uint32_t)VCF<<2 | (uint32_t)PROJ<<3 | (uint32_t)ROT<<4)
#define Get__SIA_RESET() p__IP->SIA_RESET.word
#define Set__SIA_RESET_word(x) (p__IP->SIA_RESET.word = x)


//SIA_CLK_ENABLE


#define Get__SIA_CLK_ENABLE_DMA() p__IP->SIA_CLK_ENABLE.SIA_CLK_ENABLE_ts.DMA
#define Is__SIA_CLK_ENABLE_DMA_B_0x0() (p__IP->SIA_CLK_ENABLE.SIA_CLK_ENABLE_ts.DMA == DMA_B_0x0)
#define Set__SIA_CLK_ENABLE_DMA__B_0x0() (p__IP->SIA_CLK_ENABLE.SIA_CLK_ENABLE_ts.DMA = DMA_B_0x0)
#define DMA_B_0x0 0x0    //disables module input clock
#define Is__SIA_CLK_ENABLE_DMA_B_0x1() (p__IP->SIA_CLK_ENABLE.SIA_CLK_ENABLE_ts.DMA == DMA_B_0x1)
#define Set__SIA_CLK_ENABLE_DMA__B_0x1() (p__IP->SIA_CLK_ENABLE.SIA_CLK_ENABLE_ts.DMA = DMA_B_0x1)
#define DMA_B_0x1 0x1    //enables module input clock


#define Get__SIA_CLK_ENABLE_STBP() p__IP->SIA_CLK_ENABLE.SIA_CLK_ENABLE_ts.STBP
#define Is__SIA_CLK_ENABLE_STBP_B_0x0() (p__IP->SIA_CLK_ENABLE.SIA_CLK_ENABLE_ts.STBP == STBP_B_0x0)
#define Set__SIA_CLK_ENABLE_STBP__B_0x0() (p__IP->SIA_CLK_ENABLE.SIA_CLK_ENABLE_ts.STBP = STBP_B_0x0)
#define STBP_B_0x0 0x0    //disables module input clock
#define Is__SIA_CLK_ENABLE_STBP_B_0x1() (p__IP->SIA_CLK_ENABLE.SIA_CLK_ENABLE_ts.STBP == STBP_B_0x1)
#define Set__SIA_CLK_ENABLE_STBP__B_0x1() (p__IP->SIA_CLK_ENABLE.SIA_CLK_ENABLE_ts.STBP = STBP_B_0x1)
#define STBP_B_0x1 0x1    //enables module input clock


#define Get__SIA_CLK_ENABLE_IPP() p__IP->SIA_CLK_ENABLE.SIA_CLK_ENABLE_ts.IPP
#define Is__SIA_CLK_ENABLE_IPP_B_0x0() (p__IP->SIA_CLK_ENABLE.SIA_CLK_ENABLE_ts.IPP == IPP_B_0x0)
#define Set__SIA_CLK_ENABLE_IPP__B_0x0() (p__IP->SIA_CLK_ENABLE.SIA_CLK_ENABLE_ts.IPP = IPP_B_0x0)
#define IPP_B_0x0 0x0    //disables module input clock
#define Is__SIA_CLK_ENABLE_IPP_B_0x1() (p__IP->SIA_CLK_ENABLE.SIA_CLK_ENABLE_ts.IPP == IPP_B_0x1)
#define Set__SIA_CLK_ENABLE_IPP__B_0x1() (p__IP->SIA_CLK_ENABLE.SIA_CLK_ENABLE_ts.IPP = IPP_B_0x1)
#define IPP_B_0x1 0x1    //enables module input clock


#define Get__SIA_CLK_ENABLE_VCF() p__IP->SIA_CLK_ENABLE.SIA_CLK_ENABLE_ts.VCF
#define Is__SIA_CLK_ENABLE_VCF_B_0x0() (p__IP->SIA_CLK_ENABLE.SIA_CLK_ENABLE_ts.VCF == VCF_B_0x0)
#define Set__SIA_CLK_ENABLE_VCF__B_0x0() (p__IP->SIA_CLK_ENABLE.SIA_CLK_ENABLE_ts.VCF = VCF_B_0x0)
#define VCF_B_0x0 0x0    //disables module input clock
#define Is__SIA_CLK_ENABLE_VCF_B_0x1() (p__IP->SIA_CLK_ENABLE.SIA_CLK_ENABLE_ts.VCF == VCF_B_0x1)
#define Set__SIA_CLK_ENABLE_VCF__B_0x1() (p__IP->SIA_CLK_ENABLE.SIA_CLK_ENABLE_ts.VCF = VCF_B_0x1)
#define VCF_B_0x1 0x1    //enables module input clock


#define Get__SIA_CLK_ENABLE_PROJ() p__IP->SIA_CLK_ENABLE.SIA_CLK_ENABLE_ts.PROJ
#define Is__SIA_CLK_ENABLE_PROJ_B_0x0() (p__IP->SIA_CLK_ENABLE.SIA_CLK_ENABLE_ts.PROJ == PROJ_B_0x0)
#define Set__SIA_CLK_ENABLE_PROJ__B_0x0() (p__IP->SIA_CLK_ENABLE.SIA_CLK_ENABLE_ts.PROJ = PROJ_B_0x0)
#define PROJ_B_0x0 0x0    //disables module input clock
#define Is__SIA_CLK_ENABLE_PROJ_B_0x1() (p__IP->SIA_CLK_ENABLE.SIA_CLK_ENABLE_ts.PROJ == PROJ_B_0x1)
#define Set__SIA_CLK_ENABLE_PROJ__B_0x1() (p__IP->SIA_CLK_ENABLE.SIA_CLK_ENABLE_ts.PROJ = PROJ_B_0x1)
#define PROJ_B_0x1 0x1    //enables module input clock
#define Set__SIA_CLK_ENABLE(DMA,STBP,IPP,VCF,PROJ) (p__IP->SIA_CLK_ENABLE.word = (uint32_t)DMA<<0 | (uint32_t)STBP<<1 | (uint32_t)IPP<<2 | (uint32_t)VCF<<3 | (uint32_t)PROJ<<4)
#define Get__SIA_CLK_ENABLE() p__IP->SIA_CLK_ENABLE.word
#define Set__SIA_CLK_ENABLE_word(x) (p__IP->SIA_CLK_ENABLE.word = x)


//SIA_IDN_HRV


#define Get__SIA_IDN_HRV_R0() p__IP->SIA_IDN_HRV.SIA_IDN_HRV_ts.R0


#define Get__SIA_IDN_HRV_R1() p__IP->SIA_IDN_HRV.SIA_IDN_HRV_ts.R1


#define Get__SIA_IDN_HRV_R2() p__IP->SIA_IDN_HRV.SIA_IDN_HRV_ts.R2
#define Is__SIA_IDN_HRV_R2_B_0x1() (p__IP->SIA_IDN_HRV.SIA_IDN_HRV_ts.R2 == R2_B_0x1)
#define R2_B_0x1 0x1    //SIA for 8820A
#define Is__SIA_IDN_HRV_R2_B_0x2() (p__IP->SIA_IDN_HRV.SIA_IDN_HRV_ts.R2 == R2_B_0x2)
#define R2_B_0x2 0x2    //SIA for 8820B
#define Is__SIA_IDN_HRV_R2_B_0x3() (p__IP->SIA_IDN_HRV.SIA_IDN_HRV_ts.R2 == R2_B_0x3)
#define R2_B_0x3 0x3    //SIA for 8500ED
#define Is__SIA_IDN_HRV_R2_B_0x4() (p__IP->SIA_IDN_HRV.SIA_IDN_HRV_ts.R2 == R2_B_0x4)
#define R2_B_0x4 0x4    //SIA for 8500V1
#define Is__SIA_IDN_HRV_R2_B_0x5() (p__IP->SIA_IDN_HRV.SIA_IDN_HRV_ts.R2 == R2_B_0x5)
#define R2_B_0x5 0x5    //SIA for 8500V2
#define Is__SIA_IDN_HRV_R2_B_0x6() (p__IP->SIA_IDN_HRV.SIA_IDN_HRV_ts.R2 == R2_B_0x6)
#define R2_B_0x6 0x6    //SIA for 5500V1
#define Is__SIA_IDN_HRV_R2_B_0x7() (p__IP->SIA_IDN_HRV.SIA_IDN_HRV_ts.R2 == R2_B_0x7)
#define R2_B_0x7 0x7    //SIA for 5500V2
#define Is__SIA_IDN_HRV_R2_B_0x8() (p__IP->SIA_IDN_HRV.SIA_IDN_HRV_ts.R2 == R2_B_0x8)
#define R2_B_0x8 0x8    //SIA for 9540V1
#define Is__SIA_IDN_HRV_R2_B_0x9() (p__IP->SIA_IDN_HRV.SIA_IDN_HRV_ts.R2 == R2_B_0x9)
#define R2_B_0x9 0x9    //SIA for 9600V1
#define Is__SIA_IDN_HRV_R2_B_0xA() (p__IP->SIA_IDN_HRV.SIA_IDN_HRV_ts.R2 == R2_B_0xA)
#define R2_B_0xA 0xA    //SIA for 9600V2
#define Is__SIA_IDN_HRV_R2_B_0xC() (p__IP->SIA_IDN_HRV.SIA_IDN_HRV_ts.R2 == R2_B_0xC)
#define R2_B_0xC 0xC    //SIA for 8540V1


#define Get__SIA_IDN_HRV_DXO() p__IP->SIA_IDN_HRV.SIA_IDN_HRV_ts.DXO
#define Is__SIA_IDN_HRV_DXO_B_0x0() (p__IP->SIA_IDN_HRV.SIA_IDN_HRV_ts.DXO == DXO_B_0x0)
#define DXO_B_0x0 0x0    //DxO module disabled
#define Is__SIA_IDN_HRV_DXO_B_0x1() (p__IP->SIA_IDN_HRV.SIA_IDN_HRV_ts.DXO == DXO_B_0x1)
#define DXO_B_0x1 0x1    //DxO module enabled
#define Get__SIA_IDN_HRV() p__IP->SIA_IDN_HRV.word


//SIA_VCF_ENABLE


#define Get__SIA_VCF_ENABLE_EN() p__IP->SIA_VCF_ENABLE.SIA_VCF_ENABLE_ts.EN
#define Is__SIA_VCF_ENABLE_EN_B_0x0() (p__IP->SIA_VCF_ENABLE.SIA_VCF_ENABLE_ts.EN == EN_B_0x0)
#define Set__SIA_VCF_ENABLE_EN__B_0x0() (p__IP->SIA_VCF_ENABLE.SIA_VCF_ENABLE_ts.EN = EN_B_0x0)
#define EN_B_0x0 0x0    //disables VCF.
#define Is__SIA_VCF_ENABLE_EN_B_0x1() (p__IP->SIA_VCF_ENABLE.SIA_VCF_ENABLE_ts.EN == EN_B_0x1)
#define Set__SIA_VCF_ENABLE_EN__B_0x1() (p__IP->SIA_VCF_ENABLE.SIA_VCF_ENABLE_ts.EN = EN_B_0x1)
#define EN_B_0x1 0x1    //enables VCF.
#define Set__SIA_VCF_ENABLE(EN) (p__IP->SIA_VCF_ENABLE.word = (uint16_t)EN<<0)
#define Get__SIA_VCF_ENABLE() p__IP->SIA_VCF_ENABLE.word
#define Set__SIA_VCF_ENABLE_word(x) (p__IP->SIA_VCF_ENABLE.word = x)


//SIA_ROTATION


#define Get__SIA_ROTATION_ROTATION() p__IP->SIA_ROTATION.SIA_ROTATION_ts.ROTATION
#define Is__SIA_ROTATION_ROTATION_B_0x0() (p__IP->SIA_ROTATION.SIA_ROTATION_ts.ROTATION == ROTATION_B_0x0)
#define Set__SIA_ROTATION_ROTATION__B_0x0() (p__IP->SIA_ROTATION.SIA_ROTATION_ts.ROTATION = ROTATION_B_0x0)
#define ROTATION_B_0x0 0x0    //no rotation
#define Is__SIA_ROTATION_ROTATION_B_0x1() (p__IP->SIA_ROTATION.SIA_ROTATION_ts.ROTATION == ROTATION_B_0x1)
#define Set__SIA_ROTATION_ROTATION__B_0x1() (p__IP->SIA_ROTATION.SIA_ROTATION_ts.ROTATION = ROTATION_B_0x1)
#define ROTATION_B_0x1 0x1    //rotation +90\xfb
#define Is__SIA_ROTATION_ROTATION_B_0x3() (p__IP->SIA_ROTATION.SIA_ROTATION_ts.ROTATION == ROTATION_B_0x3)
#define Set__SIA_ROTATION_ROTATION__B_0x3() (p__IP->SIA_ROTATION.SIA_ROTATION_ts.ROTATION = ROTATION_B_0x3)
#define ROTATION_B_0x3 0x3    //rotation -90\xfb
#define Set__SIA_ROTATION(ROTATION) (p__IP->SIA_ROTATION.word = (uint32_t)ROTATION<<0)
#define Get__SIA_ROTATION() p__IP->SIA_ROTATION.word
#define Set__SIA_ROTATION_word(x) (p__IP->SIA_ROTATION.word = x)


//ISP_MCU_SYS_ADDR_0_HI


#define Get__ISP_MCU_SYS_ADDR_0_HI_REGION_BA_HI() p__IP->ISP_MCU_SYS_ADDR_0_HI.ISP_MCU_SYS_ADDR_0_HI_ts.REGION_BA_HI
#define Set__ISP_MCU_SYS_ADDR_0_HI_REGION_BA_HI(x) (p__IP->ISP_MCU_SYS_ADDR_0_HI.ISP_MCU_SYS_ADDR_0_HI_ts.REGION_BA_HI = x)
#define Set__ISP_MCU_SYS_ADDR_0_HI(REGION_BA_HI) (p__IP->ISP_MCU_SYS_ADDR_0_HI.word = (uint16_t)REGION_BA_HI<<0)
#define Get__ISP_MCU_SYS_ADDR_0_HI() p__IP->ISP_MCU_SYS_ADDR_0_HI.word
#define Set__ISP_MCU_SYS_ADDR_0_HI_word(x) (p__IP->ISP_MCU_SYS_ADDR_0_HI.word = x)


//ISP_MCU_SYS_SIZE0


#define Get__ISP_MCU_SYS_SIZE0_REGION_SZ() p__IP->ISP_MCU_SYS_SIZE0.ISP_MCU_SYS_SIZE0_ts.REGION_SZ
#define Is__ISP_MCU_SYS_SIZE0_REGION_SZ_B_0x0() (p__IP->ISP_MCU_SYS_SIZE0.ISP_MCU_SYS_SIZE0_ts.REGION_SZ == REGION_SZ_B_0x0)
#define Set__ISP_MCU_SYS_SIZE0_REGION_SZ__B_0x0() (p__IP->ISP_MCU_SYS_SIZE0.ISP_MCU_SYS_SIZE0_ts.REGION_SZ = REGION_SZ_B_0x0)
#define REGION_SZ_B_0x0 0x0    //64kB
#define Is__ISP_MCU_SYS_SIZE0_REGION_SZ_B_0x1() (p__IP->ISP_MCU_SYS_SIZE0.ISP_MCU_SYS_SIZE0_ts.REGION_SZ == REGION_SZ_B_0x1)
#define Set__ISP_MCU_SYS_SIZE0_REGION_SZ__B_0x1() (p__IP->ISP_MCU_SYS_SIZE0.ISP_MCU_SYS_SIZE0_ts.REGION_SZ = REGION_SZ_B_0x1)
#define REGION_SZ_B_0x1 0x1    //1MB
#define Is__ISP_MCU_SYS_SIZE0_REGION_SZ_B_0x2() (p__IP->ISP_MCU_SYS_SIZE0.ISP_MCU_SYS_SIZE0_ts.REGION_SZ == REGION_SZ_B_0x2)
#define Set__ISP_MCU_SYS_SIZE0_REGION_SZ__B_0x2() (p__IP->ISP_MCU_SYS_SIZE0.ISP_MCU_SYS_SIZE0_ts.REGION_SZ = REGION_SZ_B_0x2)
#define REGION_SZ_B_0x2 0x2    //16MB
#define Is__ISP_MCU_SYS_SIZE0_REGION_SZ_B_0x3() (p__IP->ISP_MCU_SYS_SIZE0.ISP_MCU_SYS_SIZE0_ts.REGION_SZ == REGION_SZ_B_0x3)
#define Set__ISP_MCU_SYS_SIZE0_REGION_SZ__B_0x3() (p__IP->ISP_MCU_SYS_SIZE0.ISP_MCU_SYS_SIZE0_ts.REGION_SZ = REGION_SZ_B_0x3)
#define REGION_SZ_B_0x3 0x3    //256MB
#define Is__ISP_MCU_SYS_SIZE0_REGION_SZ_B_0x4() (p__IP->ISP_MCU_SYS_SIZE0.ISP_MCU_SYS_SIZE0_ts.REGION_SZ == REGION_SZ_B_0x4)
#define Set__ISP_MCU_SYS_SIZE0_REGION_SZ__B_0x4() (p__IP->ISP_MCU_SYS_SIZE0.ISP_MCU_SYS_SIZE0_ts.REGION_SZ = REGION_SZ_B_0x4)
#define REGION_SZ_B_0x4 0x4    //4kB
#define Is__ISP_MCU_SYS_SIZE0_REGION_SZ_B_0x5() (p__IP->ISP_MCU_SYS_SIZE0.ISP_MCU_SYS_SIZE0_ts.REGION_SZ == REGION_SZ_B_0x5)
#define Set__ISP_MCU_SYS_SIZE0_REGION_SZ__B_0x5() (p__IP->ISP_MCU_SYS_SIZE0.ISP_MCU_SYS_SIZE0_ts.REGION_SZ = REGION_SZ_B_0x5)
#define REGION_SZ_B_0x5 0x5    //8kB
#define Is__ISP_MCU_SYS_SIZE0_REGION_SZ_B_0x6() (p__IP->ISP_MCU_SYS_SIZE0.ISP_MCU_SYS_SIZE0_ts.REGION_SZ == REGION_SZ_B_0x6)
#define Set__ISP_MCU_SYS_SIZE0_REGION_SZ__B_0x6() (p__IP->ISP_MCU_SYS_SIZE0.ISP_MCU_SYS_SIZE0_ts.REGION_SZ = REGION_SZ_B_0x6)
#define REGION_SZ_B_0x6 0x6    //16kB
#define Is__ISP_MCU_SYS_SIZE0_REGION_SZ_B_0x7() (p__IP->ISP_MCU_SYS_SIZE0.ISP_MCU_SYS_SIZE0_ts.REGION_SZ == REGION_SZ_B_0x7)
#define Set__ISP_MCU_SYS_SIZE0_REGION_SZ__B_0x7() (p__IP->ISP_MCU_SYS_SIZE0.ISP_MCU_SYS_SIZE0_ts.REGION_SZ = REGION_SZ_B_0x7)
#define REGION_SZ_B_0x7 0x7    //32kB
#define Set__ISP_MCU_SYS_SIZE0(REGION_SZ) (p__IP->ISP_MCU_SYS_SIZE0.word = (uint16_t)REGION_SZ<<0)
#define Get__ISP_MCU_SYS_SIZE0() p__IP->ISP_MCU_SYS_SIZE0.word
#define Set__ISP_MCU_SYS_SIZE0_word(x) (p__IP->ISP_MCU_SYS_SIZE0.word = x)


//ISP_MCU_SYS_ADDR_1_HI


#define Get__ISP_MCU_SYS_ADDR_1_HI_REGION_BA_HI() p__IP->ISP_MCU_SYS_ADDR_1_HI.ISP_MCU_SYS_ADDR_1_HI_ts.REGION_BA_HI
#define Set__ISP_MCU_SYS_ADDR_1_HI_REGION_BA_HI(x) (p__IP->ISP_MCU_SYS_ADDR_1_HI.ISP_MCU_SYS_ADDR_1_HI_ts.REGION_BA_HI = x)
#define Set__ISP_MCU_SYS_ADDR_1_HI(REGION_BA_HI) (p__IP->ISP_MCU_SYS_ADDR_1_HI.word = (uint16_t)REGION_BA_HI<<0)
#define Get__ISP_MCU_SYS_ADDR_1_HI() p__IP->ISP_MCU_SYS_ADDR_1_HI.word
#define Set__ISP_MCU_SYS_ADDR_1_HI_word(x) (p__IP->ISP_MCU_SYS_ADDR_1_HI.word = x)


//ISP_MCU_SYS_SIZE1


#define Get__ISP_MCU_SYS_SIZE1_REGION_SZ() p__IP->ISP_MCU_SYS_SIZE1.ISP_MCU_SYS_SIZE1_ts.REGION_SZ
#define Is__ISP_MCU_SYS_SIZE1_REGION_SZ_B_0x0() (p__IP->ISP_MCU_SYS_SIZE1.ISP_MCU_SYS_SIZE1_ts.REGION_SZ == REGION_SZ_B_0x0)
#define Set__ISP_MCU_SYS_SIZE1_REGION_SZ__B_0x0() (p__IP->ISP_MCU_SYS_SIZE1.ISP_MCU_SYS_SIZE1_ts.REGION_SZ = REGION_SZ_B_0x0)
#define REGION_SZ_B_0x0 0x0    //64kB
#define Is__ISP_MCU_SYS_SIZE1_REGION_SZ_B_0x1() (p__IP->ISP_MCU_SYS_SIZE1.ISP_MCU_SYS_SIZE1_ts.REGION_SZ == REGION_SZ_B_0x1)
#define Set__ISP_MCU_SYS_SIZE1_REGION_SZ__B_0x1() (p__IP->ISP_MCU_SYS_SIZE1.ISP_MCU_SYS_SIZE1_ts.REGION_SZ = REGION_SZ_B_0x1)
#define REGION_SZ_B_0x1 0x1    //1MB
#define Is__ISP_MCU_SYS_SIZE1_REGION_SZ_B_0x2() (p__IP->ISP_MCU_SYS_SIZE1.ISP_MCU_SYS_SIZE1_ts.REGION_SZ == REGION_SZ_B_0x2)
#define Set__ISP_MCU_SYS_SIZE1_REGION_SZ__B_0x2() (p__IP->ISP_MCU_SYS_SIZE1.ISP_MCU_SYS_SIZE1_ts.REGION_SZ = REGION_SZ_B_0x2)
#define REGION_SZ_B_0x2 0x2    //16MB
#define Is__ISP_MCU_SYS_SIZE1_REGION_SZ_B_0x3() (p__IP->ISP_MCU_SYS_SIZE1.ISP_MCU_SYS_SIZE1_ts.REGION_SZ == REGION_SZ_B_0x3)
#define Set__ISP_MCU_SYS_SIZE1_REGION_SZ__B_0x3() (p__IP->ISP_MCU_SYS_SIZE1.ISP_MCU_SYS_SIZE1_ts.REGION_SZ = REGION_SZ_B_0x3)
#define REGION_SZ_B_0x3 0x3    //256MB
#define Is__ISP_MCU_SYS_SIZE1_REGION_SZ_B_0x4() (p__IP->ISP_MCU_SYS_SIZE1.ISP_MCU_SYS_SIZE1_ts.REGION_SZ == REGION_SZ_B_0x4)
#define Set__ISP_MCU_SYS_SIZE1_REGION_SZ__B_0x4() (p__IP->ISP_MCU_SYS_SIZE1.ISP_MCU_SYS_SIZE1_ts.REGION_SZ = REGION_SZ_B_0x4)
#define REGION_SZ_B_0x4 0x4    //4kB
#define Is__ISP_MCU_SYS_SIZE1_REGION_SZ_B_0x5() (p__IP->ISP_MCU_SYS_SIZE1.ISP_MCU_SYS_SIZE1_ts.REGION_SZ == REGION_SZ_B_0x5)
#define Set__ISP_MCU_SYS_SIZE1_REGION_SZ__B_0x5() (p__IP->ISP_MCU_SYS_SIZE1.ISP_MCU_SYS_SIZE1_ts.REGION_SZ = REGION_SZ_B_0x5)
#define REGION_SZ_B_0x5 0x5    //8kB
#define Is__ISP_MCU_SYS_SIZE1_REGION_SZ_B_0x6() (p__IP->ISP_MCU_SYS_SIZE1.ISP_MCU_SYS_SIZE1_ts.REGION_SZ == REGION_SZ_B_0x6)
#define Set__ISP_MCU_SYS_SIZE1_REGION_SZ__B_0x6() (p__IP->ISP_MCU_SYS_SIZE1.ISP_MCU_SYS_SIZE1_ts.REGION_SZ = REGION_SZ_B_0x6)
#define REGION_SZ_B_0x6 0x6    //16kB
#define Is__ISP_MCU_SYS_SIZE1_REGION_SZ_B_0x7() (p__IP->ISP_MCU_SYS_SIZE1.ISP_MCU_SYS_SIZE1_ts.REGION_SZ == REGION_SZ_B_0x7)
#define Set__ISP_MCU_SYS_SIZE1_REGION_SZ__B_0x7() (p__IP->ISP_MCU_SYS_SIZE1.ISP_MCU_SYS_SIZE1_ts.REGION_SZ = REGION_SZ_B_0x7)
#define REGION_SZ_B_0x7 0x7    //32kB
#define Set__ISP_MCU_SYS_SIZE1(REGION_SZ) (p__IP->ISP_MCU_SYS_SIZE1.word = (uint16_t)REGION_SZ<<0)
#define Get__ISP_MCU_SYS_SIZE1() p__IP->ISP_MCU_SYS_SIZE1.word
#define Set__ISP_MCU_SYS_SIZE1_word(x) (p__IP->ISP_MCU_SYS_SIZE1.word = x)


//ISP_MCU_SYS_ADDR_2_HI


#define Get__ISP_MCU_SYS_ADDR_2_HI_REGION_BA_HI() p__IP->ISP_MCU_SYS_ADDR_2_HI.ISP_MCU_SYS_ADDR_2_HI_ts.REGION_BA_HI
#define Set__ISP_MCU_SYS_ADDR_2_HI_REGION_BA_HI(x) (p__IP->ISP_MCU_SYS_ADDR_2_HI.ISP_MCU_SYS_ADDR_2_HI_ts.REGION_BA_HI = x)
#define Set__ISP_MCU_SYS_ADDR_2_HI(REGION_BA_HI) (p__IP->ISP_MCU_SYS_ADDR_2_HI.word = (uint16_t)REGION_BA_HI<<0)
#define Get__ISP_MCU_SYS_ADDR_2_HI() p__IP->ISP_MCU_SYS_ADDR_2_HI.word
#define Set__ISP_MCU_SYS_ADDR_2_HI_word(x) (p__IP->ISP_MCU_SYS_ADDR_2_HI.word = x)


//ISP_MCU_SYS_SIZE2


#define Get__ISP_MCU_SYS_SIZE2_REGION_SZ() p__IP->ISP_MCU_SYS_SIZE2.ISP_MCU_SYS_SIZE2_ts.REGION_SZ
#define Is__ISP_MCU_SYS_SIZE2_REGION_SZ_B_0x0() (p__IP->ISP_MCU_SYS_SIZE2.ISP_MCU_SYS_SIZE2_ts.REGION_SZ == REGION_SZ_B_0x0)
#define Set__ISP_MCU_SYS_SIZE2_REGION_SZ__B_0x0() (p__IP->ISP_MCU_SYS_SIZE2.ISP_MCU_SYS_SIZE2_ts.REGION_SZ = REGION_SZ_B_0x0)
#define REGION_SZ_B_0x0 0x0    //64kB
#define Is__ISP_MCU_SYS_SIZE2_REGION_SZ_B_0x1() (p__IP->ISP_MCU_SYS_SIZE2.ISP_MCU_SYS_SIZE2_ts.REGION_SZ == REGION_SZ_B_0x1)
#define Set__ISP_MCU_SYS_SIZE2_REGION_SZ__B_0x1() (p__IP->ISP_MCU_SYS_SIZE2.ISP_MCU_SYS_SIZE2_ts.REGION_SZ = REGION_SZ_B_0x1)
#define REGION_SZ_B_0x1 0x1    //1MB
#define Is__ISP_MCU_SYS_SIZE2_REGION_SZ_B_0x2() (p__IP->ISP_MCU_SYS_SIZE2.ISP_MCU_SYS_SIZE2_ts.REGION_SZ == REGION_SZ_B_0x2)
#define Set__ISP_MCU_SYS_SIZE2_REGION_SZ__B_0x2() (p__IP->ISP_MCU_SYS_SIZE2.ISP_MCU_SYS_SIZE2_ts.REGION_SZ = REGION_SZ_B_0x2)
#define REGION_SZ_B_0x2 0x2    //16MB
#define Is__ISP_MCU_SYS_SIZE2_REGION_SZ_B_0x3() (p__IP->ISP_MCU_SYS_SIZE2.ISP_MCU_SYS_SIZE2_ts.REGION_SZ == REGION_SZ_B_0x3)
#define Set__ISP_MCU_SYS_SIZE2_REGION_SZ__B_0x3() (p__IP->ISP_MCU_SYS_SIZE2.ISP_MCU_SYS_SIZE2_ts.REGION_SZ = REGION_SZ_B_0x3)
#define REGION_SZ_B_0x3 0x3    //256MB
#define Is__ISP_MCU_SYS_SIZE2_REGION_SZ_B_0x4() (p__IP->ISP_MCU_SYS_SIZE2.ISP_MCU_SYS_SIZE2_ts.REGION_SZ == REGION_SZ_B_0x4)
#define Set__ISP_MCU_SYS_SIZE2_REGION_SZ__B_0x4() (p__IP->ISP_MCU_SYS_SIZE2.ISP_MCU_SYS_SIZE2_ts.REGION_SZ = REGION_SZ_B_0x4)
#define REGION_SZ_B_0x4 0x4    //4kB
#define Is__ISP_MCU_SYS_SIZE2_REGION_SZ_B_0x5() (p__IP->ISP_MCU_SYS_SIZE2.ISP_MCU_SYS_SIZE2_ts.REGION_SZ == REGION_SZ_B_0x5)
#define Set__ISP_MCU_SYS_SIZE2_REGION_SZ__B_0x5() (p__IP->ISP_MCU_SYS_SIZE2.ISP_MCU_SYS_SIZE2_ts.REGION_SZ = REGION_SZ_B_0x5)
#define REGION_SZ_B_0x5 0x5    //8kB
#define Is__ISP_MCU_SYS_SIZE2_REGION_SZ_B_0x6() (p__IP->ISP_MCU_SYS_SIZE2.ISP_MCU_SYS_SIZE2_ts.REGION_SZ == REGION_SZ_B_0x6)
#define Set__ISP_MCU_SYS_SIZE2_REGION_SZ__B_0x6() (p__IP->ISP_MCU_SYS_SIZE2.ISP_MCU_SYS_SIZE2_ts.REGION_SZ = REGION_SZ_B_0x6)
#define REGION_SZ_B_0x6 0x6    //16kB
#define Is__ISP_MCU_SYS_SIZE2_REGION_SZ_B_0x7() (p__IP->ISP_MCU_SYS_SIZE2.ISP_MCU_SYS_SIZE2_ts.REGION_SZ == REGION_SZ_B_0x7)
#define Set__ISP_MCU_SYS_SIZE2_REGION_SZ__B_0x7() (p__IP->ISP_MCU_SYS_SIZE2.ISP_MCU_SYS_SIZE2_ts.REGION_SZ = REGION_SZ_B_0x7)
#define REGION_SZ_B_0x7 0x7    //32kB
#define Set__ISP_MCU_SYS_SIZE2(REGION_SZ) (p__IP->ISP_MCU_SYS_SIZE2.word = (uint16_t)REGION_SZ<<0)
#define Get__ISP_MCU_SYS_SIZE2() p__IP->ISP_MCU_SYS_SIZE2.word
#define Set__ISP_MCU_SYS_SIZE2_word(x) (p__IP->ISP_MCU_SYS_SIZE2.word = x)


//ISP_MCU_SYS_ADDR_3_HI


#define Get__ISP_MCU_SYS_ADDR_3_HI_REGION_BA_HI() p__IP->ISP_MCU_SYS_ADDR_3_HI.ISP_MCU_SYS_ADDR_3_HI_ts.REGION_BA_HI
#define Set__ISP_MCU_SYS_ADDR_3_HI_REGION_BA_HI(x) (p__IP->ISP_MCU_SYS_ADDR_3_HI.ISP_MCU_SYS_ADDR_3_HI_ts.REGION_BA_HI = x)
#define Set__ISP_MCU_SYS_ADDR_3_HI(REGION_BA_HI) (p__IP->ISP_MCU_SYS_ADDR_3_HI.word = (uint16_t)REGION_BA_HI<<0)
#define Get__ISP_MCU_SYS_ADDR_3_HI() p__IP->ISP_MCU_SYS_ADDR_3_HI.word
#define Set__ISP_MCU_SYS_ADDR_3_HI_word(x) (p__IP->ISP_MCU_SYS_ADDR_3_HI.word = x)


//ISP_MCU_SYS_SIZE3


#define Get__ISP_MCU_SYS_SIZE3_REGION_SZ() p__IP->ISP_MCU_SYS_SIZE3.ISP_MCU_SYS_SIZE3_ts.REGION_SZ
#define Is__ISP_MCU_SYS_SIZE3_REGION_SZ_B_0x0() (p__IP->ISP_MCU_SYS_SIZE3.ISP_MCU_SYS_SIZE3_ts.REGION_SZ == REGION_SZ_B_0x0)
#define Set__ISP_MCU_SYS_SIZE3_REGION_SZ__B_0x0() (p__IP->ISP_MCU_SYS_SIZE3.ISP_MCU_SYS_SIZE3_ts.REGION_SZ = REGION_SZ_B_0x0)
#define REGION_SZ_B_0x0 0x0    //64kB
#define Is__ISP_MCU_SYS_SIZE3_REGION_SZ_B_0x1() (p__IP->ISP_MCU_SYS_SIZE3.ISP_MCU_SYS_SIZE3_ts.REGION_SZ == REGION_SZ_B_0x1)
#define Set__ISP_MCU_SYS_SIZE3_REGION_SZ__B_0x1() (p__IP->ISP_MCU_SYS_SIZE3.ISP_MCU_SYS_SIZE3_ts.REGION_SZ = REGION_SZ_B_0x1)
#define REGION_SZ_B_0x1 0x1    //1MB
#define Is__ISP_MCU_SYS_SIZE3_REGION_SZ_B_0x2() (p__IP->ISP_MCU_SYS_SIZE3.ISP_MCU_SYS_SIZE3_ts.REGION_SZ == REGION_SZ_B_0x2)
#define Set__ISP_MCU_SYS_SIZE3_REGION_SZ__B_0x2() (p__IP->ISP_MCU_SYS_SIZE3.ISP_MCU_SYS_SIZE3_ts.REGION_SZ = REGION_SZ_B_0x2)
#define REGION_SZ_B_0x2 0x2    //16MB
#define Is__ISP_MCU_SYS_SIZE3_REGION_SZ_B_0x3() (p__IP->ISP_MCU_SYS_SIZE3.ISP_MCU_SYS_SIZE3_ts.REGION_SZ == REGION_SZ_B_0x3)
#define Set__ISP_MCU_SYS_SIZE3_REGION_SZ__B_0x3() (p__IP->ISP_MCU_SYS_SIZE3.ISP_MCU_SYS_SIZE3_ts.REGION_SZ = REGION_SZ_B_0x3)
#define REGION_SZ_B_0x3 0x3    //256MB
#define Is__ISP_MCU_SYS_SIZE3_REGION_SZ_B_0x4() (p__IP->ISP_MCU_SYS_SIZE3.ISP_MCU_SYS_SIZE3_ts.REGION_SZ == REGION_SZ_B_0x4)
#define Set__ISP_MCU_SYS_SIZE3_REGION_SZ__B_0x4() (p__IP->ISP_MCU_SYS_SIZE3.ISP_MCU_SYS_SIZE3_ts.REGION_SZ = REGION_SZ_B_0x4)
#define REGION_SZ_B_0x4 0x4    //4kB
#define Is__ISP_MCU_SYS_SIZE3_REGION_SZ_B_0x5() (p__IP->ISP_MCU_SYS_SIZE3.ISP_MCU_SYS_SIZE3_ts.REGION_SZ == REGION_SZ_B_0x5)
#define Set__ISP_MCU_SYS_SIZE3_REGION_SZ__B_0x5() (p__IP->ISP_MCU_SYS_SIZE3.ISP_MCU_SYS_SIZE3_ts.REGION_SZ = REGION_SZ_B_0x5)
#define REGION_SZ_B_0x5 0x5    //8kB
#define Is__ISP_MCU_SYS_SIZE3_REGION_SZ_B_0x6() (p__IP->ISP_MCU_SYS_SIZE3.ISP_MCU_SYS_SIZE3_ts.REGION_SZ == REGION_SZ_B_0x6)
#define Set__ISP_MCU_SYS_SIZE3_REGION_SZ__B_0x6() (p__IP->ISP_MCU_SYS_SIZE3.ISP_MCU_SYS_SIZE3_ts.REGION_SZ = REGION_SZ_B_0x6)
#define REGION_SZ_B_0x6 0x6    //16kB
#define Is__ISP_MCU_SYS_SIZE3_REGION_SZ_B_0x7() (p__IP->ISP_MCU_SYS_SIZE3.ISP_MCU_SYS_SIZE3_ts.REGION_SZ == REGION_SZ_B_0x7)
#define Set__ISP_MCU_SYS_SIZE3_REGION_SZ__B_0x7() (p__IP->ISP_MCU_SYS_SIZE3.ISP_MCU_SYS_SIZE3_ts.REGION_SZ = REGION_SZ_B_0x7)
#define REGION_SZ_B_0x7 0x7    //32kB
#define Set__ISP_MCU_SYS_SIZE3(REGION_SZ) (p__IP->ISP_MCU_SYS_SIZE3.word = (uint16_t)REGION_SZ<<0)
#define Get__ISP_MCU_SYS_SIZE3() p__IP->ISP_MCU_SYS_SIZE3.word
#define Set__ISP_MCU_SYS_SIZE3_word(x) (p__IP->ISP_MCU_SYS_SIZE3.word = x)


//ISP_MCU_SYS_ADDR_4_HI


#define Get__ISP_MCU_SYS_ADDR_4_HI_REGION_BA_HI() p__IP->ISP_MCU_SYS_ADDR_4_HI.ISP_MCU_SYS_ADDR_4_HI_ts.REGION_BA_HI
#define Set__ISP_MCU_SYS_ADDR_4_HI_REGION_BA_HI(x) (p__IP->ISP_MCU_SYS_ADDR_4_HI.ISP_MCU_SYS_ADDR_4_HI_ts.REGION_BA_HI = x)
#define Set__ISP_MCU_SYS_ADDR_4_HI(REGION_BA_HI) (p__IP->ISP_MCU_SYS_ADDR_4_HI.word = (uint16_t)REGION_BA_HI<<0)
#define Get__ISP_MCU_SYS_ADDR_4_HI() p__IP->ISP_MCU_SYS_ADDR_4_HI.word
#define Set__ISP_MCU_SYS_ADDR_4_HI_word(x) (p__IP->ISP_MCU_SYS_ADDR_4_HI.word = x)


//ISP_MCU_SYS_SIZE4


#define Get__ISP_MCU_SYS_SIZE4_REGION_SZ() p__IP->ISP_MCU_SYS_SIZE4.ISP_MCU_SYS_SIZE4_ts.REGION_SZ
#define Is__ISP_MCU_SYS_SIZE4_REGION_SZ_B_0x0() (p__IP->ISP_MCU_SYS_SIZE4.ISP_MCU_SYS_SIZE4_ts.REGION_SZ == REGION_SZ_B_0x0)
#define Set__ISP_MCU_SYS_SIZE4_REGION_SZ__B_0x0() (p__IP->ISP_MCU_SYS_SIZE4.ISP_MCU_SYS_SIZE4_ts.REGION_SZ = REGION_SZ_B_0x0)
#define REGION_SZ_B_0x0 0x0    //64kB
#define Is__ISP_MCU_SYS_SIZE4_REGION_SZ_B_0x1() (p__IP->ISP_MCU_SYS_SIZE4.ISP_MCU_SYS_SIZE4_ts.REGION_SZ == REGION_SZ_B_0x1)
#define Set__ISP_MCU_SYS_SIZE4_REGION_SZ__B_0x1() (p__IP->ISP_MCU_SYS_SIZE4.ISP_MCU_SYS_SIZE4_ts.REGION_SZ = REGION_SZ_B_0x1)
#define REGION_SZ_B_0x1 0x1    //1MB
#define Is__ISP_MCU_SYS_SIZE4_REGION_SZ_B_0x2() (p__IP->ISP_MCU_SYS_SIZE4.ISP_MCU_SYS_SIZE4_ts.REGION_SZ == REGION_SZ_B_0x2)
#define Set__ISP_MCU_SYS_SIZE4_REGION_SZ__B_0x2() (p__IP->ISP_MCU_SYS_SIZE4.ISP_MCU_SYS_SIZE4_ts.REGION_SZ = REGION_SZ_B_0x2)
#define REGION_SZ_B_0x2 0x2    //16MB
#define Is__ISP_MCU_SYS_SIZE4_REGION_SZ_B_0x3() (p__IP->ISP_MCU_SYS_SIZE4.ISP_MCU_SYS_SIZE4_ts.REGION_SZ == REGION_SZ_B_0x3)
#define Set__ISP_MCU_SYS_SIZE4_REGION_SZ__B_0x3() (p__IP->ISP_MCU_SYS_SIZE4.ISP_MCU_SYS_SIZE4_ts.REGION_SZ = REGION_SZ_B_0x3)
#define REGION_SZ_B_0x3 0x3    //256MB
#define Is__ISP_MCU_SYS_SIZE4_REGION_SZ_B_0x4() (p__IP->ISP_MCU_SYS_SIZE4.ISP_MCU_SYS_SIZE4_ts.REGION_SZ == REGION_SZ_B_0x4)
#define Set__ISP_MCU_SYS_SIZE4_REGION_SZ__B_0x4() (p__IP->ISP_MCU_SYS_SIZE4.ISP_MCU_SYS_SIZE4_ts.REGION_SZ = REGION_SZ_B_0x4)
#define REGION_SZ_B_0x4 0x4    //4kB
#define Is__ISP_MCU_SYS_SIZE4_REGION_SZ_B_0x5() (p__IP->ISP_MCU_SYS_SIZE4.ISP_MCU_SYS_SIZE4_ts.REGION_SZ == REGION_SZ_B_0x5)
#define Set__ISP_MCU_SYS_SIZE4_REGION_SZ__B_0x5() (p__IP->ISP_MCU_SYS_SIZE4.ISP_MCU_SYS_SIZE4_ts.REGION_SZ = REGION_SZ_B_0x5)
#define REGION_SZ_B_0x5 0x5    //8kB
#define Is__ISP_MCU_SYS_SIZE4_REGION_SZ_B_0x6() (p__IP->ISP_MCU_SYS_SIZE4.ISP_MCU_SYS_SIZE4_ts.REGION_SZ == REGION_SZ_B_0x6)
#define Set__ISP_MCU_SYS_SIZE4_REGION_SZ__B_0x6() (p__IP->ISP_MCU_SYS_SIZE4.ISP_MCU_SYS_SIZE4_ts.REGION_SZ = REGION_SZ_B_0x6)
#define REGION_SZ_B_0x6 0x6    //16kB
#define Is__ISP_MCU_SYS_SIZE4_REGION_SZ_B_0x7() (p__IP->ISP_MCU_SYS_SIZE4.ISP_MCU_SYS_SIZE4_ts.REGION_SZ == REGION_SZ_B_0x7)
#define Set__ISP_MCU_SYS_SIZE4_REGION_SZ__B_0x7() (p__IP->ISP_MCU_SYS_SIZE4.ISP_MCU_SYS_SIZE4_ts.REGION_SZ = REGION_SZ_B_0x7)
#define REGION_SZ_B_0x7 0x7    //32kB
#define Set__ISP_MCU_SYS_SIZE4(REGION_SZ) (p__IP->ISP_MCU_SYS_SIZE4.word = (uint16_t)REGION_SZ<<0)
#define Get__ISP_MCU_SYS_SIZE4() p__IP->ISP_MCU_SYS_SIZE4.word
#define Set__ISP_MCU_SYS_SIZE4_word(x) (p__IP->ISP_MCU_SYS_SIZE4.word = x)


//ISP_MCU_SYS_ADDR_5_HI


#define Get__ISP_MCU_SYS_ADDR_5_HI_REGION_BA_HI() p__IP->ISP_MCU_SYS_ADDR_5_HI.ISP_MCU_SYS_ADDR_5_HI_ts.REGION_BA_HI
#define Set__ISP_MCU_SYS_ADDR_5_HI_REGION_BA_HI(x) (p__IP->ISP_MCU_SYS_ADDR_5_HI.ISP_MCU_SYS_ADDR_5_HI_ts.REGION_BA_HI = x)
#define Set__ISP_MCU_SYS_ADDR_5_HI(REGION_BA_HI) (p__IP->ISP_MCU_SYS_ADDR_5_HI.word = (uint16_t)REGION_BA_HI<<0)
#define Get__ISP_MCU_SYS_ADDR_5_HI() p__IP->ISP_MCU_SYS_ADDR_5_HI.word
#define Set__ISP_MCU_SYS_ADDR_5_HI_word(x) (p__IP->ISP_MCU_SYS_ADDR_5_HI.word = x)


//ISP_MCU_SYS_SIZE5


#define Get__ISP_MCU_SYS_SIZE5_REGION_SZ() p__IP->ISP_MCU_SYS_SIZE5.ISP_MCU_SYS_SIZE5_ts.REGION_SZ
#define Is__ISP_MCU_SYS_SIZE5_REGION_SZ_B_0x0() (p__IP->ISP_MCU_SYS_SIZE5.ISP_MCU_SYS_SIZE5_ts.REGION_SZ == REGION_SZ_B_0x0)
#define Set__ISP_MCU_SYS_SIZE5_REGION_SZ__B_0x0() (p__IP->ISP_MCU_SYS_SIZE5.ISP_MCU_SYS_SIZE5_ts.REGION_SZ = REGION_SZ_B_0x0)
#define REGION_SZ_B_0x0 0x0    //64kB
#define Is__ISP_MCU_SYS_SIZE5_REGION_SZ_B_0x1() (p__IP->ISP_MCU_SYS_SIZE5.ISP_MCU_SYS_SIZE5_ts.REGION_SZ == REGION_SZ_B_0x1)
#define Set__ISP_MCU_SYS_SIZE5_REGION_SZ__B_0x1() (p__IP->ISP_MCU_SYS_SIZE5.ISP_MCU_SYS_SIZE5_ts.REGION_SZ = REGION_SZ_B_0x1)
#define REGION_SZ_B_0x1 0x1    //1MB
#define Is__ISP_MCU_SYS_SIZE5_REGION_SZ_B_0x2() (p__IP->ISP_MCU_SYS_SIZE5.ISP_MCU_SYS_SIZE5_ts.REGION_SZ == REGION_SZ_B_0x2)
#define Set__ISP_MCU_SYS_SIZE5_REGION_SZ__B_0x2() (p__IP->ISP_MCU_SYS_SIZE5.ISP_MCU_SYS_SIZE5_ts.REGION_SZ = REGION_SZ_B_0x2)
#define REGION_SZ_B_0x2 0x2    //16MB
#define Is__ISP_MCU_SYS_SIZE5_REGION_SZ_B_0x3() (p__IP->ISP_MCU_SYS_SIZE5.ISP_MCU_SYS_SIZE5_ts.REGION_SZ == REGION_SZ_B_0x3)
#define Set__ISP_MCU_SYS_SIZE5_REGION_SZ__B_0x3() (p__IP->ISP_MCU_SYS_SIZE5.ISP_MCU_SYS_SIZE5_ts.REGION_SZ = REGION_SZ_B_0x3)
#define REGION_SZ_B_0x3 0x3    //256MB
#define Is__ISP_MCU_SYS_SIZE5_REGION_SZ_B_0x4() (p__IP->ISP_MCU_SYS_SIZE5.ISP_MCU_SYS_SIZE5_ts.REGION_SZ == REGION_SZ_B_0x4)
#define Set__ISP_MCU_SYS_SIZE5_REGION_SZ__B_0x4() (p__IP->ISP_MCU_SYS_SIZE5.ISP_MCU_SYS_SIZE5_ts.REGION_SZ = REGION_SZ_B_0x4)
#define REGION_SZ_B_0x4 0x4    //4kB
#define Is__ISP_MCU_SYS_SIZE5_REGION_SZ_B_0x5() (p__IP->ISP_MCU_SYS_SIZE5.ISP_MCU_SYS_SIZE5_ts.REGION_SZ == REGION_SZ_B_0x5)
#define Set__ISP_MCU_SYS_SIZE5_REGION_SZ__B_0x5() (p__IP->ISP_MCU_SYS_SIZE5.ISP_MCU_SYS_SIZE5_ts.REGION_SZ = REGION_SZ_B_0x5)
#define REGION_SZ_B_0x5 0x5    //8kB
#define Is__ISP_MCU_SYS_SIZE5_REGION_SZ_B_0x6() (p__IP->ISP_MCU_SYS_SIZE5.ISP_MCU_SYS_SIZE5_ts.REGION_SZ == REGION_SZ_B_0x6)
#define Set__ISP_MCU_SYS_SIZE5_REGION_SZ__B_0x6() (p__IP->ISP_MCU_SYS_SIZE5.ISP_MCU_SYS_SIZE5_ts.REGION_SZ = REGION_SZ_B_0x6)
#define REGION_SZ_B_0x6 0x6    //16kB
#define Is__ISP_MCU_SYS_SIZE5_REGION_SZ_B_0x7() (p__IP->ISP_MCU_SYS_SIZE5.ISP_MCU_SYS_SIZE5_ts.REGION_SZ == REGION_SZ_B_0x7)
#define Set__ISP_MCU_SYS_SIZE5_REGION_SZ__B_0x7() (p__IP->ISP_MCU_SYS_SIZE5.ISP_MCU_SYS_SIZE5_ts.REGION_SZ = REGION_SZ_B_0x7)
#define REGION_SZ_B_0x7 0x7    //32kB
#define Set__ISP_MCU_SYS_SIZE5(REGION_SZ) (p__IP->ISP_MCU_SYS_SIZE5.word = (uint16_t)REGION_SZ<<0)
#define Get__ISP_MCU_SYS_SIZE5() p__IP->ISP_MCU_SYS_SIZE5.word
#define Set__ISP_MCU_SYS_SIZE5_word(x) (p__IP->ISP_MCU_SYS_SIZE5.word = x)


//ISP_MCU_SYS_ADDR_6_HI


#define Get__ISP_MCU_SYS_ADDR_6_HI_REGION_BA_HI() p__IP->ISP_MCU_SYS_ADDR_6_HI.ISP_MCU_SYS_ADDR_6_HI_ts.REGION_BA_HI
#define Set__ISP_MCU_SYS_ADDR_6_HI_REGION_BA_HI(x) (p__IP->ISP_MCU_SYS_ADDR_6_HI.ISP_MCU_SYS_ADDR_6_HI_ts.REGION_BA_HI = x)
#define Set__ISP_MCU_SYS_ADDR_6_HI(REGION_BA_HI) (p__IP->ISP_MCU_SYS_ADDR_6_HI.word = (uint16_t)REGION_BA_HI<<0)
#define Get__ISP_MCU_SYS_ADDR_6_HI() p__IP->ISP_MCU_SYS_ADDR_6_HI.word
#define Set__ISP_MCU_SYS_ADDR_6_HI_word(x) (p__IP->ISP_MCU_SYS_ADDR_6_HI.word = x)


//ISP_MCU_SYS_SIZE6


#define Get__ISP_MCU_SYS_SIZE6_REGION_SZ() p__IP->ISP_MCU_SYS_SIZE6.ISP_MCU_SYS_SIZE6_ts.REGION_SZ
#define Is__ISP_MCU_SYS_SIZE6_REGION_SZ_B_0x0() (p__IP->ISP_MCU_SYS_SIZE6.ISP_MCU_SYS_SIZE6_ts.REGION_SZ == REGION_SZ_B_0x0)
#define Set__ISP_MCU_SYS_SIZE6_REGION_SZ__B_0x0() (p__IP->ISP_MCU_SYS_SIZE6.ISP_MCU_SYS_SIZE6_ts.REGION_SZ = REGION_SZ_B_0x0)
#define REGION_SZ_B_0x0 0x0    //64kB
#define Is__ISP_MCU_SYS_SIZE6_REGION_SZ_B_0x1() (p__IP->ISP_MCU_SYS_SIZE6.ISP_MCU_SYS_SIZE6_ts.REGION_SZ == REGION_SZ_B_0x1)
#define Set__ISP_MCU_SYS_SIZE6_REGION_SZ__B_0x1() (p__IP->ISP_MCU_SYS_SIZE6.ISP_MCU_SYS_SIZE6_ts.REGION_SZ = REGION_SZ_B_0x1)
#define REGION_SZ_B_0x1 0x1    //1MB
#define Is__ISP_MCU_SYS_SIZE6_REGION_SZ_B_0x2() (p__IP->ISP_MCU_SYS_SIZE6.ISP_MCU_SYS_SIZE6_ts.REGION_SZ == REGION_SZ_B_0x2)
#define Set__ISP_MCU_SYS_SIZE6_REGION_SZ__B_0x2() (p__IP->ISP_MCU_SYS_SIZE6.ISP_MCU_SYS_SIZE6_ts.REGION_SZ = REGION_SZ_B_0x2)
#define REGION_SZ_B_0x2 0x2    //16MB
#define Is__ISP_MCU_SYS_SIZE6_REGION_SZ_B_0x3() (p__IP->ISP_MCU_SYS_SIZE6.ISP_MCU_SYS_SIZE6_ts.REGION_SZ == REGION_SZ_B_0x3)
#define Set__ISP_MCU_SYS_SIZE6_REGION_SZ__B_0x3() (p__IP->ISP_MCU_SYS_SIZE6.ISP_MCU_SYS_SIZE6_ts.REGION_SZ = REGION_SZ_B_0x3)
#define REGION_SZ_B_0x3 0x3    //256MB
#define Is__ISP_MCU_SYS_SIZE6_REGION_SZ_B_0x4() (p__IP->ISP_MCU_SYS_SIZE6.ISP_MCU_SYS_SIZE6_ts.REGION_SZ == REGION_SZ_B_0x4)
#define Set__ISP_MCU_SYS_SIZE6_REGION_SZ__B_0x4() (p__IP->ISP_MCU_SYS_SIZE6.ISP_MCU_SYS_SIZE6_ts.REGION_SZ = REGION_SZ_B_0x4)
#define REGION_SZ_B_0x4 0x4    //4kB
#define Is__ISP_MCU_SYS_SIZE6_REGION_SZ_B_0x5() (p__IP->ISP_MCU_SYS_SIZE6.ISP_MCU_SYS_SIZE6_ts.REGION_SZ == REGION_SZ_B_0x5)
#define Set__ISP_MCU_SYS_SIZE6_REGION_SZ__B_0x5() (p__IP->ISP_MCU_SYS_SIZE6.ISP_MCU_SYS_SIZE6_ts.REGION_SZ = REGION_SZ_B_0x5)
#define REGION_SZ_B_0x5 0x5    //8kB
#define Is__ISP_MCU_SYS_SIZE6_REGION_SZ_B_0x6() (p__IP->ISP_MCU_SYS_SIZE6.ISP_MCU_SYS_SIZE6_ts.REGION_SZ == REGION_SZ_B_0x6)
#define Set__ISP_MCU_SYS_SIZE6_REGION_SZ__B_0x6() (p__IP->ISP_MCU_SYS_SIZE6.ISP_MCU_SYS_SIZE6_ts.REGION_SZ = REGION_SZ_B_0x6)
#define REGION_SZ_B_0x6 0x6    //16kB
#define Is__ISP_MCU_SYS_SIZE6_REGION_SZ_B_0x7() (p__IP->ISP_MCU_SYS_SIZE6.ISP_MCU_SYS_SIZE6_ts.REGION_SZ == REGION_SZ_B_0x7)
#define Set__ISP_MCU_SYS_SIZE6_REGION_SZ__B_0x7() (p__IP->ISP_MCU_SYS_SIZE6.ISP_MCU_SYS_SIZE6_ts.REGION_SZ = REGION_SZ_B_0x7)
#define REGION_SZ_B_0x7 0x7    //32kB
#define Set__ISP_MCU_SYS_SIZE6(REGION_SZ) (p__IP->ISP_MCU_SYS_SIZE6.word = (uint16_t)REGION_SZ<<0)
#define Get__ISP_MCU_SYS_SIZE6() p__IP->ISP_MCU_SYS_SIZE6.word
#define Set__ISP_MCU_SYS_SIZE6_word(x) (p__IP->ISP_MCU_SYS_SIZE6.word = x)


//ISP_MCU_SYS_ADDR_7_HI


#define Get__ISP_MCU_SYS_ADDR_7_HI_REGION_BA_HI() p__IP->ISP_MCU_SYS_ADDR_7_HI.ISP_MCU_SYS_ADDR_7_HI_ts.REGION_BA_HI
#define Set__ISP_MCU_SYS_ADDR_7_HI_REGION_BA_HI(x) (p__IP->ISP_MCU_SYS_ADDR_7_HI.ISP_MCU_SYS_ADDR_7_HI_ts.REGION_BA_HI = x)
#define Set__ISP_MCU_SYS_ADDR_7_HI(REGION_BA_HI) (p__IP->ISP_MCU_SYS_ADDR_7_HI.word = (uint16_t)REGION_BA_HI<<0)
#define Get__ISP_MCU_SYS_ADDR_7_HI() p__IP->ISP_MCU_SYS_ADDR_7_HI.word
#define Set__ISP_MCU_SYS_ADDR_7_HI_word(x) (p__IP->ISP_MCU_SYS_ADDR_7_HI.word = x)


//ISP_MCU_SYS_SIZE7


#define Get__ISP_MCU_SYS_SIZE7_REGION_SZ() p__IP->ISP_MCU_SYS_SIZE7.ISP_MCU_SYS_SIZE7_ts.REGION_SZ
#define Is__ISP_MCU_SYS_SIZE7_REGION_SZ_B_0x0() (p__IP->ISP_MCU_SYS_SIZE7.ISP_MCU_SYS_SIZE7_ts.REGION_SZ == REGION_SZ_B_0x0)
#define Set__ISP_MCU_SYS_SIZE7_REGION_SZ__B_0x0() (p__IP->ISP_MCU_SYS_SIZE7.ISP_MCU_SYS_SIZE7_ts.REGION_SZ = REGION_SZ_B_0x0)
#define REGION_SZ_B_0x0 0x0    //64kB
#define Is__ISP_MCU_SYS_SIZE7_REGION_SZ_B_0x1() (p__IP->ISP_MCU_SYS_SIZE7.ISP_MCU_SYS_SIZE7_ts.REGION_SZ == REGION_SZ_B_0x1)
#define Set__ISP_MCU_SYS_SIZE7_REGION_SZ__B_0x1() (p__IP->ISP_MCU_SYS_SIZE7.ISP_MCU_SYS_SIZE7_ts.REGION_SZ = REGION_SZ_B_0x1)
#define REGION_SZ_B_0x1 0x1    //1MB
#define Is__ISP_MCU_SYS_SIZE7_REGION_SZ_B_0x2() (p__IP->ISP_MCU_SYS_SIZE7.ISP_MCU_SYS_SIZE7_ts.REGION_SZ == REGION_SZ_B_0x2)
#define Set__ISP_MCU_SYS_SIZE7_REGION_SZ__B_0x2() (p__IP->ISP_MCU_SYS_SIZE7.ISP_MCU_SYS_SIZE7_ts.REGION_SZ = REGION_SZ_B_0x2)
#define REGION_SZ_B_0x2 0x2    //16MB
#define Is__ISP_MCU_SYS_SIZE7_REGION_SZ_B_0x3() (p__IP->ISP_MCU_SYS_SIZE7.ISP_MCU_SYS_SIZE7_ts.REGION_SZ == REGION_SZ_B_0x3)
#define Set__ISP_MCU_SYS_SIZE7_REGION_SZ__B_0x3() (p__IP->ISP_MCU_SYS_SIZE7.ISP_MCU_SYS_SIZE7_ts.REGION_SZ = REGION_SZ_B_0x3)
#define REGION_SZ_B_0x3 0x3    //256MB
#define Is__ISP_MCU_SYS_SIZE7_REGION_SZ_B_0x4() (p__IP->ISP_MCU_SYS_SIZE7.ISP_MCU_SYS_SIZE7_ts.REGION_SZ == REGION_SZ_B_0x4)
#define Set__ISP_MCU_SYS_SIZE7_REGION_SZ__B_0x4() (p__IP->ISP_MCU_SYS_SIZE7.ISP_MCU_SYS_SIZE7_ts.REGION_SZ = REGION_SZ_B_0x4)
#define REGION_SZ_B_0x4 0x4    //4kB
#define Is__ISP_MCU_SYS_SIZE7_REGION_SZ_B_0x5() (p__IP->ISP_MCU_SYS_SIZE7.ISP_MCU_SYS_SIZE7_ts.REGION_SZ == REGION_SZ_B_0x5)
#define Set__ISP_MCU_SYS_SIZE7_REGION_SZ__B_0x5() (p__IP->ISP_MCU_SYS_SIZE7.ISP_MCU_SYS_SIZE7_ts.REGION_SZ = REGION_SZ_B_0x5)
#define REGION_SZ_B_0x5 0x5    //8kB
#define Is__ISP_MCU_SYS_SIZE7_REGION_SZ_B_0x6() (p__IP->ISP_MCU_SYS_SIZE7.ISP_MCU_SYS_SIZE7_ts.REGION_SZ == REGION_SZ_B_0x6)
#define Set__ISP_MCU_SYS_SIZE7_REGION_SZ__B_0x6() (p__IP->ISP_MCU_SYS_SIZE7.ISP_MCU_SYS_SIZE7_ts.REGION_SZ = REGION_SZ_B_0x6)
#define REGION_SZ_B_0x6 0x6    //16kB
#define Is__ISP_MCU_SYS_SIZE7_REGION_SZ_B_0x7() (p__IP->ISP_MCU_SYS_SIZE7.ISP_MCU_SYS_SIZE7_ts.REGION_SZ == REGION_SZ_B_0x7)
#define Set__ISP_MCU_SYS_SIZE7_REGION_SZ__B_0x7() (p__IP->ISP_MCU_SYS_SIZE7.ISP_MCU_SYS_SIZE7_ts.REGION_SZ = REGION_SZ_B_0x7)
#define REGION_SZ_B_0x7 0x7    //32kB
#define Set__ISP_MCU_SYS_SIZE7(REGION_SZ) (p__IP->ISP_MCU_SYS_SIZE7.word = (uint16_t)REGION_SZ<<0)
#define Get__ISP_MCU_SYS_SIZE7() p__IP->ISP_MCU_SYS_SIZE7.word
#define Set__ISP_MCU_SYS_SIZE7_word(x) (p__IP->ISP_MCU_SYS_SIZE7.word = x)


//ISP_MCU_IO_ADDR_0_HI


#define Get__ISP_MCU_IO_ADDR_0_HI_REGION_BA_HI() p__IP->ISP_MCU_IO_ADDR_0_HI.ISP_MCU_IO_ADDR_0_HI_ts.REGION_BA_HI
#define Set__ISP_MCU_IO_ADDR_0_HI_REGION_BA_HI(x) (p__IP->ISP_MCU_IO_ADDR_0_HI.ISP_MCU_IO_ADDR_0_HI_ts.REGION_BA_HI = x)
#define Set__ISP_MCU_IO_ADDR_0_HI(REGION_BA_HI) (p__IP->ISP_MCU_IO_ADDR_0_HI.word = (uint16_t)REGION_BA_HI<<0)
#define Get__ISP_MCU_IO_ADDR_0_HI() p__IP->ISP_MCU_IO_ADDR_0_HI.word
#define Set__ISP_MCU_IO_ADDR_0_HI_word(x) (p__IP->ISP_MCU_IO_ADDR_0_HI.word = x)


//ISP_MCU_IO_ADDR_1_HI


#define Get__ISP_MCU_IO_ADDR_1_HI_REGION_BA_HI() p__IP->ISP_MCU_IO_ADDR_1_HI.ISP_MCU_IO_ADDR_1_HI_ts.REGION_BA_HI
#define Set__ISP_MCU_IO_ADDR_1_HI_REGION_BA_HI(x) (p__IP->ISP_MCU_IO_ADDR_1_HI.ISP_MCU_IO_ADDR_1_HI_ts.REGION_BA_HI = x)
#define Set__ISP_MCU_IO_ADDR_1_HI(REGION_BA_HI) (p__IP->ISP_MCU_IO_ADDR_1_HI.word = (uint16_t)REGION_BA_HI<<0)
#define Get__ISP_MCU_IO_ADDR_1_HI() p__IP->ISP_MCU_IO_ADDR_1_HI.word
#define Set__ISP_MCU_IO_ADDR_1_HI_word(x) (p__IP->ISP_MCU_IO_ADDR_1_HI.word = x)


//ISP_MCU_IO_ADDR_2_HI


#define Get__ISP_MCU_IO_ADDR_2_HI_REGION_BA_HI() p__IP->ISP_MCU_IO_ADDR_2_HI.ISP_MCU_IO_ADDR_2_HI_ts.REGION_BA_HI
#define Set__ISP_MCU_IO_ADDR_2_HI_REGION_BA_HI(x) (p__IP->ISP_MCU_IO_ADDR_2_HI.ISP_MCU_IO_ADDR_2_HI_ts.REGION_BA_HI = x)
#define Set__ISP_MCU_IO_ADDR_2_HI(REGION_BA_HI) (p__IP->ISP_MCU_IO_ADDR_2_HI.word = (uint16_t)REGION_BA_HI<<0)
#define Get__ISP_MCU_IO_ADDR_2_HI() p__IP->ISP_MCU_IO_ADDR_2_HI.word
#define Set__ISP_MCU_IO_ADDR_2_HI_word(x) (p__IP->ISP_MCU_IO_ADDR_2_HI.word = x)


//ISP_MCU_IO_ADDR_3_HI


#define Get__ISP_MCU_IO_ADDR_3_HI_REGION_BA_HI() p__IP->ISP_MCU_IO_ADDR_3_HI.ISP_MCU_IO_ADDR_3_HI_ts.REGION_BA_HI
#define Set__ISP_MCU_IO_ADDR_3_HI_REGION_BA_HI(x) (p__IP->ISP_MCU_IO_ADDR_3_HI.ISP_MCU_IO_ADDR_3_HI_ts.REGION_BA_HI = x)
#define Set__ISP_MCU_IO_ADDR_3_HI(REGION_BA_HI) (p__IP->ISP_MCU_IO_ADDR_3_HI.word = (uint16_t)REGION_BA_HI<<0)
#define Get__ISP_MCU_IO_ADDR_3_HI() p__IP->ISP_MCU_IO_ADDR_3_HI.word
#define Set__ISP_MCU_IO_ADDR_3_HI_word(x) (p__IP->ISP_MCU_IO_ADDR_3_HI.word = x)


//ISP_MCU_IO_ADDR_4_HI


#define Get__ISP_MCU_IO_ADDR_4_HI_REGION_BA_HI() p__IP->ISP_MCU_IO_ADDR_4_HI.ISP_MCU_IO_ADDR_4_HI_ts.REGION_BA_HI
#define Set__ISP_MCU_IO_ADDR_4_HI_REGION_BA_HI(x) (p__IP->ISP_MCU_IO_ADDR_4_HI.ISP_MCU_IO_ADDR_4_HI_ts.REGION_BA_HI = x)
#define Set__ISP_MCU_IO_ADDR_4_HI(REGION_BA_HI) (p__IP->ISP_MCU_IO_ADDR_4_HI.word = (uint16_t)REGION_BA_HI<<0)
#define Get__ISP_MCU_IO_ADDR_4_HI() p__IP->ISP_MCU_IO_ADDR_4_HI.word
#define Set__ISP_MCU_IO_ADDR_4_HI_word(x) (p__IP->ISP_MCU_IO_ADDR_4_HI.word = x)


//ISP_MCU_IO_ADDR_5_HI


#define Get__ISP_MCU_IO_ADDR_5_HI_REGION_BA_HI() p__IP->ISP_MCU_IO_ADDR_5_HI.ISP_MCU_IO_ADDR_5_HI_ts.REGION_BA_HI
#define Set__ISP_MCU_IO_ADDR_5_HI_REGION_BA_HI(x) (p__IP->ISP_MCU_IO_ADDR_5_HI.ISP_MCU_IO_ADDR_5_HI_ts.REGION_BA_HI = x)
#define Set__ISP_MCU_IO_ADDR_5_HI(REGION_BA_HI) (p__IP->ISP_MCU_IO_ADDR_5_HI.word = (uint16_t)REGION_BA_HI<<0)
#define Get__ISP_MCU_IO_ADDR_5_HI() p__IP->ISP_MCU_IO_ADDR_5_HI.word
#define Set__ISP_MCU_IO_ADDR_5_HI_word(x) (p__IP->ISP_MCU_IO_ADDR_5_HI.word = x)


//ISP_MCU_IO_ADDR_6_HI


#define Get__ISP_MCU_IO_ADDR_6_HI_REGION_BA_HI() p__IP->ISP_MCU_IO_ADDR_6_HI.ISP_MCU_IO_ADDR_6_HI_ts.REGION_BA_HI
#define Set__ISP_MCU_IO_ADDR_6_HI_REGION_BA_HI(x) (p__IP->ISP_MCU_IO_ADDR_6_HI.ISP_MCU_IO_ADDR_6_HI_ts.REGION_BA_HI = x)
#define Set__ISP_MCU_IO_ADDR_6_HI(REGION_BA_HI) (p__IP->ISP_MCU_IO_ADDR_6_HI.word = (uint16_t)REGION_BA_HI<<0)
#define Get__ISP_MCU_IO_ADDR_6_HI() p__IP->ISP_MCU_IO_ADDR_6_HI.word
#define Set__ISP_MCU_IO_ADDR_6_HI_word(x) (p__IP->ISP_MCU_IO_ADDR_6_HI.word = x)


//ISP_MCU_IO_ADDR_7_HI


#define Get__ISP_MCU_IO_ADDR_7_HI_REGION_BA_HI() p__IP->ISP_MCU_IO_ADDR_7_HI.ISP_MCU_IO_ADDR_7_HI_ts.REGION_BA_HI
#define Set__ISP_MCU_IO_ADDR_7_HI_REGION_BA_HI(x) (p__IP->ISP_MCU_IO_ADDR_7_HI.ISP_MCU_IO_ADDR_7_HI_ts.REGION_BA_HI = x)
#define Set__ISP_MCU_IO_ADDR_7_HI(REGION_BA_HI) (p__IP->ISP_MCU_IO_ADDR_7_HI.word = (uint16_t)REGION_BA_HI<<0)
#define Get__ISP_MCU_IO_ADDR_7_HI() p__IP->ISP_MCU_IO_ADDR_7_HI.word
#define Set__ISP_MCU_IO_ADDR_7_HI_word(x) (p__IP->ISP_MCU_IO_ADDR_7_HI.word = x)


//ISP_MEM_PAGE


#define Get__ISP_MEM_PAGE_INDEX() p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX
#define Is__ISP_MEM_PAGE_INDEX_B_0x0() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX == INDEX_B_0x0)
#define Set__ISP_MEM_PAGE_INDEX__B_0x0() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX = INDEX_B_0x0)
#define INDEX_B_0x0 0x0    //should not be used (direct access available)
#define Is__ISP_MEM_PAGE_INDEX_B_0x1() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX == INDEX_B_0x1)
#define Set__ISP_MEM_PAGE_INDEX__B_0x1() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX = INDEX_B_0x1)
#define INDEX_B_0x1 0x1    //should not be used (direct access available)
#define Is__ISP_MEM_PAGE_INDEX_B_0x2() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX == INDEX_B_0x2)
#define Set__ISP_MEM_PAGE_INDEX__B_0x2() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX = INDEX_B_0x2)
#define INDEX_B_0x2 0x2    //Pictor ISP memories
#define Is__ISP_MEM_PAGE_INDEX_B_0x3() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX == INDEX_B_0x3)
#define Set__ISP_MEM_PAGE_INDEX__B_0x3() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX = INDEX_B_0x3)
#define INDEX_B_0x3 0x3    //Pictor ISP memories
#define Is__ISP_MEM_PAGE_INDEX_B_0x4() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX == INDEX_B_0x4)
#define Set__ISP_MEM_PAGE_INDEX__B_0x4() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX = INDEX_B_0x4)
#define INDEX_B_0x4 0x4    //Pictor ISP memories
#define Is__ISP_MEM_PAGE_INDEX_B_0x5() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX == INDEX_B_0x5)
#define Set__ISP_MEM_PAGE_INDEX__B_0x5() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX = INDEX_B_0x5)
#define INDEX_B_0x5 0x5    //Pictor ISP memories
#define Is__ISP_MEM_PAGE_INDEX_B_0x6() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX == INDEX_B_0x6)
#define Set__ISP_MEM_PAGE_INDEX__B_0x6() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX = INDEX_B_0x6)
#define INDEX_B_0x6 0x6    //Pictor ISP memories
#define Is__ISP_MEM_PAGE_INDEX_B_0x7() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX == INDEX_B_0x7)
#define Set__ISP_MEM_PAGE_INDEX__B_0x7() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX = INDEX_B_0x7)
#define INDEX_B_0x7 0x7    //Pictor ISP memories
#define Is__ISP_MEM_PAGE_INDEX_B_0x8() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX == INDEX_B_0x8)
#define Set__ISP_MEM_PAGE_INDEX__B_0x8() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX = INDEX_B_0x8)
#define INDEX_B_0x8 0x8    //Pictor ISP memories
#define Is__ISP_MEM_PAGE_INDEX_B_0x9() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX == INDEX_B_0x9)
#define Set__ISP_MEM_PAGE_INDEX__B_0x9() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX = INDEX_B_0x9)
#define INDEX_B_0x9 0x9    //Pictor ISP memories
#define Is__ISP_MEM_PAGE_INDEX_B_0xA() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX == INDEX_B_0xA)
#define Set__ISP_MEM_PAGE_INDEX__B_0xA() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX = INDEX_B_0xA)
#define INDEX_B_0xA 0xA    //Pictor ISP memories
#define Is__ISP_MEM_PAGE_INDEX_B_0xB() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX == INDEX_B_0xB)
#define Set__ISP_MEM_PAGE_INDEX__B_0xB() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX = INDEX_B_0xB)
#define INDEX_B_0xB 0xB    //Pictor ISP memories
#define Is__ISP_MEM_PAGE_INDEX_B_0xC() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX == INDEX_B_0xC)
#define Set__ISP_MEM_PAGE_INDEX__B_0xC() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX = INDEX_B_0xC)
#define INDEX_B_0xC 0xC    //Pictor ISP memories
#define Is__ISP_MEM_PAGE_INDEX_B_0xD() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX == INDEX_B_0xD)
#define Set__ISP_MEM_PAGE_INDEX__B_0xD() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX = INDEX_B_0xD)
#define INDEX_B_0xD 0xD    //Pictor ISP memories
#define Is__ISP_MEM_PAGE_INDEX_B_0xE() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX == INDEX_B_0xE)
#define Set__ISP_MEM_PAGE_INDEX__B_0xE() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX = INDEX_B_0xE)
#define INDEX_B_0xE 0xE    //Pictor ISP memories
#define Is__ISP_MEM_PAGE_INDEX_B_0xF() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX == INDEX_B_0xF)
#define Set__ISP_MEM_PAGE_INDEX__B_0xF() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX = INDEX_B_0xF)
#define INDEX_B_0xF 0xF    //Pictor ISP memories
#define Is__ISP_MEM_PAGE_INDEX_B_0x10() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX == INDEX_B_0x10)
#define Set__ISP_MEM_PAGE_INDEX__B_0x10() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX = INDEX_B_0x10)
#define INDEX_B_0x10 0x10    //Pictor ISP memories
#define Is__ISP_MEM_PAGE_INDEX_B_0x11() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX == INDEX_B_0x11)
#define Set__ISP_MEM_PAGE_INDEX__B_0x11() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX = INDEX_B_0x11)
#define INDEX_B_0x11 0x11    //Pictor ISP memories
#define Is__ISP_MEM_PAGE_INDEX_B_0x12() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX == INDEX_B_0x12)
#define Set__ISP_MEM_PAGE_INDEX__B_0x12() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX = INDEX_B_0x12)
#define INDEX_B_0x12 0x12    //Pictor ISP memories
#define Is__ISP_MEM_PAGE_INDEX_B_0x13() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX == INDEX_B_0x13)
#define Set__ISP_MEM_PAGE_INDEX__B_0x13() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX = INDEX_B_0x13)
#define INDEX_B_0x13 0x13    //Pictor ISP memories
#define Is__ISP_MEM_PAGE_INDEX_B_0x14() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX == INDEX_B_0x14)
#define Set__ISP_MEM_PAGE_INDEX__B_0x14() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX = INDEX_B_0x14)
#define INDEX_B_0x14 0x14    //Pictor ISP memories
#define Is__ISP_MEM_PAGE_INDEX_B_0x15() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX == INDEX_B_0x15)
#define Set__ISP_MEM_PAGE_INDEX__B_0x15() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX = INDEX_B_0x15)
#define INDEX_B_0x15 0x15    //Pictor ISP memories
#define Is__ISP_MEM_PAGE_INDEX_B_0x16() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX == INDEX_B_0x16)
#define Set__ISP_MEM_PAGE_INDEX__B_0x16() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX = INDEX_B_0x16)
#define INDEX_B_0x16 0x16    //Pictor ISP memories
#define Is__ISP_MEM_PAGE_INDEX_B_0x17() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX == INDEX_B_0x17)
#define Set__ISP_MEM_PAGE_INDEX__B_0x17() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX = INDEX_B_0x17)
#define INDEX_B_0x17 0x17    //Pictor ISP memories
#define Is__ISP_MEM_PAGE_INDEX_B_0x18() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX == INDEX_B_0x18)
#define Set__ISP_MEM_PAGE_INDEX__B_0x18() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX = INDEX_B_0x18)
#define INDEX_B_0x18 0x18    //Pictor ISP memories
#define Is__ISP_MEM_PAGE_INDEX_B_0x19() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX == INDEX_B_0x19)
#define Set__ISP_MEM_PAGE_INDEX__B_0x19() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX = INDEX_B_0x19)
#define INDEX_B_0x19 0x19    //Pictor ISP memories
#define Is__ISP_MEM_PAGE_INDEX_B_0x20() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX == INDEX_B_0x20)
#define Set__ISP_MEM_PAGE_INDEX__B_0x20() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX = INDEX_B_0x20)
#define INDEX_B_0x20 0x20    //Pictor MCU Program memory (L2)
#define Is__ISP_MEM_PAGE_INDEX_B_0x21() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX == INDEX_B_0x21)
#define Set__ISP_MEM_PAGE_INDEX__B_0x21() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX = INDEX_B_0x21)
#define INDEX_B_0x21 0x21    //Pictor MCU Program memory (L2)
#define Is__ISP_MEM_PAGE_INDEX_B_0x22() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX == INDEX_B_0x22)
#define Set__ISP_MEM_PAGE_INDEX__B_0x22() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX = INDEX_B_0x22)
#define INDEX_B_0x22 0x22    //Pictor MCU Program memory (L2)
#define Is__ISP_MEM_PAGE_INDEX_B_0x23() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX == INDEX_B_0x23)
#define Set__ISP_MEM_PAGE_INDEX__B_0x23() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX = INDEX_B_0x23)
#define INDEX_B_0x23 0x23    //Pictor MCU Program memory (L2)
#define Is__ISP_MEM_PAGE_INDEX_B_0x24() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX == INDEX_B_0x24)
#define Set__ISP_MEM_PAGE_INDEX__B_0x24() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX = INDEX_B_0x24)
#define INDEX_B_0x24 0x24    //Pictor MCU Program memory (L2)
#define Is__ISP_MEM_PAGE_INDEX_B_0x25() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX == INDEX_B_0x25)
#define Set__ISP_MEM_PAGE_INDEX__B_0x25() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX = INDEX_B_0x25)
#define INDEX_B_0x25 0x25    //Pictor MCU Program memory (L2)
#define Is__ISP_MEM_PAGE_INDEX_B_0x26() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX == INDEX_B_0x26)
#define Set__ISP_MEM_PAGE_INDEX__B_0x26() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX = INDEX_B_0x26)
#define INDEX_B_0x26 0x26    //Pictor MCU Program memory (L2)
#define Is__ISP_MEM_PAGE_INDEX_B_0x27() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX == INDEX_B_0x27)
#define Set__ISP_MEM_PAGE_INDEX__B_0x27() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX = INDEX_B_0x27)
#define INDEX_B_0x27 0x27    //Pictor MCU Program memory (L2)
#define Is__ISP_MEM_PAGE_INDEX_B_0x30() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX == INDEX_B_0x30)
#define Set__ISP_MEM_PAGE_INDEX__B_0x30() (p__IP->ISP_MEM_PAGE.ISP_MEM_PAGE_ts.INDEX = INDEX_B_0x30)
#define INDEX_B_0x30 0x30    //Pictor MCU Data memory (TCDM)
#define Set__ISP_MEM_PAGE(INDEX) (p__IP->ISP_MEM_PAGE.word = (uint16_t)INDEX<<0)
#define Get__ISP_MEM_PAGE() p__IP->ISP_MEM_PAGE.word
#define Set__ISP_MEM_PAGE_word(x) (p__IP->ISP_MEM_PAGE.word = x)


//SIA_XBUS_BYTE_ENABLE


#define Get__SIA_XBUS_BYTE_ENABLE_BE0() p__IP->SIA_XBUS_BYTE_ENABLE.SIA_XBUS_BYTE_ENABLE_ts.BE0
#define Is__SIA_XBUS_BYTE_ENABLE_BE0_B_0x0() (p__IP->SIA_XBUS_BYTE_ENABLE.SIA_XBUS_BYTE_ENABLE_ts.BE0 == BE0_B_0x0)
#define Set__SIA_XBUS_BYTE_ENABLE_BE0__B_0x0() (p__IP->SIA_XBUS_BYTE_ENABLE.SIA_XBUS_BYTE_ENABLE_ts.BE0 = BE0_B_0x0)
#define BE0_B_0x0 0x0    //both LSB and MSB
#define Is__SIA_XBUS_BYTE_ENABLE_BE0_B_0x1() (p__IP->SIA_XBUS_BYTE_ENABLE.SIA_XBUS_BYTE_ENABLE_ts.BE0 == BE0_B_0x1)
#define Set__SIA_XBUS_BYTE_ENABLE_BE0__B_0x1() (p__IP->SIA_XBUS_BYTE_ENABLE.SIA_XBUS_BYTE_ENABLE_ts.BE0 = BE0_B_0x1)
#define BE0_B_0x1 0x1    //LSB only
#define Is__SIA_XBUS_BYTE_ENABLE_BE0_B_0x2() (p__IP->SIA_XBUS_BYTE_ENABLE.SIA_XBUS_BYTE_ENABLE_ts.BE0 == BE0_B_0x2)
#define Set__SIA_XBUS_BYTE_ENABLE_BE0__B_0x2() (p__IP->SIA_XBUS_BYTE_ENABLE.SIA_XBUS_BYTE_ENABLE_ts.BE0 = BE0_B_0x2)
#define BE0_B_0x2 0x2    //MSB only
#define Is__SIA_XBUS_BYTE_ENABLE_BE0_B_0x3() (p__IP->SIA_XBUS_BYTE_ENABLE.SIA_XBUS_BYTE_ENABLE_ts.BE0 == BE0_B_0x3)
#define Set__SIA_XBUS_BYTE_ENABLE_BE0__B_0x3() (p__IP->SIA_XBUS_BYTE_ENABLE.SIA_XBUS_BYTE_ENABLE_ts.BE0 = BE0_B_0x3)
#define BE0_B_0x3 0x3    //both LSB and MSB


#define Get__SIA_XBUS_BYTE_ENABLE_BE1() p__IP->SIA_XBUS_BYTE_ENABLE.SIA_XBUS_BYTE_ENABLE_ts.BE1
#define Is__SIA_XBUS_BYTE_ENABLE_BE1_B_0x0() (p__IP->SIA_XBUS_BYTE_ENABLE.SIA_XBUS_BYTE_ENABLE_ts.BE1 == BE1_B_0x0)
#define Set__SIA_XBUS_BYTE_ENABLE_BE1__B_0x0() (p__IP->SIA_XBUS_BYTE_ENABLE.SIA_XBUS_BYTE_ENABLE_ts.BE1 = BE1_B_0x0)
#define BE1_B_0x0 0x0    //both LSB and MSB
#define Is__SIA_XBUS_BYTE_ENABLE_BE1_B_0x1() (p__IP->SIA_XBUS_BYTE_ENABLE.SIA_XBUS_BYTE_ENABLE_ts.BE1 == BE1_B_0x1)
#define Set__SIA_XBUS_BYTE_ENABLE_BE1__B_0x1() (p__IP->SIA_XBUS_BYTE_ENABLE.SIA_XBUS_BYTE_ENABLE_ts.BE1 = BE1_B_0x1)
#define BE1_B_0x1 0x1    //LSB only
#define Is__SIA_XBUS_BYTE_ENABLE_BE1_B_0x2() (p__IP->SIA_XBUS_BYTE_ENABLE.SIA_XBUS_BYTE_ENABLE_ts.BE1 == BE1_B_0x2)
#define Set__SIA_XBUS_BYTE_ENABLE_BE1__B_0x2() (p__IP->SIA_XBUS_BYTE_ENABLE.SIA_XBUS_BYTE_ENABLE_ts.BE1 = BE1_B_0x2)
#define BE1_B_0x2 0x2    //MSB only
#define Is__SIA_XBUS_BYTE_ENABLE_BE1_B_0x3() (p__IP->SIA_XBUS_BYTE_ENABLE.SIA_XBUS_BYTE_ENABLE_ts.BE1 == BE1_B_0x3)
#define Set__SIA_XBUS_BYTE_ENABLE_BE1__B_0x3() (p__IP->SIA_XBUS_BYTE_ENABLE.SIA_XBUS_BYTE_ENABLE_ts.BE1 = BE1_B_0x3)
#define BE1_B_0x3 0x3    //both LSB and MSB
#define Set__SIA_XBUS_BYTE_ENABLE(BE0,BE1) (p__IP->SIA_XBUS_BYTE_ENABLE.word = (uint32_t)BE0<<0 | (uint32_t)BE1<<8)
#define Get__SIA_XBUS_BYTE_ENABLE() p__IP->SIA_XBUS_BYTE_ENABLE.word
#define Set__SIA_XBUS_BYTE_ENABLE_word(x) (p__IP->SIA_XBUS_BYTE_ENABLE.word = x)


//SIA_PIPE03_SELECT


#define Get__SIA_PIPE03_SELECT_AP() p__IP->SIA_PIPE03_SELECT.SIA_PIPE03_SELECT_ts.AP
#define Is__SIA_PIPE03_SELECT_AP_B_0x0() (p__IP->SIA_PIPE03_SELECT.SIA_PIPE03_SELECT_ts.AP == AP_B_0x0)
#define Set__SIA_PIPE03_SELECT_AP__B_0x0() (p__IP->SIA_PIPE03_SELECT.SIA_PIPE03_SELECT_ts.AP = AP_B_0x0)
#define AP_B_0x0 0x0    //Pipe 0
#define Is__SIA_PIPE03_SELECT_AP_B_0x1() (p__IP->SIA_PIPE03_SELECT.SIA_PIPE03_SELECT_ts.AP == AP_B_0x1)
#define Set__SIA_PIPE03_SELECT_AP__B_0x1() (p__IP->SIA_PIPE03_SELECT.SIA_PIPE03_SELECT_ts.AP = AP_B_0x1)
#define AP_B_0x1 0x1    //Pipe3
#define Set__SIA_PIPE03_SELECT(AP) (p__IP->SIA_PIPE03_SELECT.word = (uint16_t)AP<<0)
#define Get__SIA_PIPE03_SELECT() p__IP->SIA_PIPE03_SELECT.word
#define Set__SIA_PIPE03_SELECT_word(x) (p__IP->SIA_PIPE03_SELECT.word = x)


//DXO_PDP_PAGE


#define Get__DXO_PDP_PAGE_INDEX() p__IP->DXO_PDP_PAGE.DXO_PDP_PAGE_ts.INDEX
#define Set__DXO_PDP_PAGE_INDEX(x) (p__IP->DXO_PDP_PAGE.DXO_PDP_PAGE_ts.INDEX = x)
#define Set__DXO_PDP_PAGE(INDEX) (p__IP->DXO_PDP_PAGE.word = (uint16_t)INDEX<<0)
#define Get__DXO_PDP_PAGE() p__IP->DXO_PDP_PAGE.word
#define Set__DXO_PDP_PAGE_word(x) (p__IP->DXO_PDP_PAGE.word = x)


//DXO_DPP_PAGE


#define Get__DXO_DPP_PAGE_INDEX() p__IP->DXO_DPP_PAGE.DXO_DPP_PAGE_ts.INDEX
#define Set__DXO_DPP_PAGE_INDEX(x) (p__IP->DXO_DPP_PAGE.DXO_DPP_PAGE_ts.INDEX = x)
#define Set__DXO_DPP_PAGE(INDEX) (p__IP->DXO_DPP_PAGE.word = (uint16_t)INDEX<<0)
#define Get__DXO_DPP_PAGE() p__IP->DXO_DPP_PAGE.word
#define Set__DXO_DPP_PAGE_word(x) (p__IP->DXO_DPP_PAGE.word = x)


//DXO_DOP7_PAGE


#define Get__DXO_DOP7_PAGE_INDEX() p__IP->DXO_DOP7_PAGE.DXO_DOP7_PAGE_ts.INDEX
#define Set__DXO_DOP7_PAGE_INDEX(x) (p__IP->DXO_DOP7_PAGE.DXO_DOP7_PAGE_ts.INDEX = x)
#define Set__DXO_DOP7_PAGE(INDEX) (p__IP->DXO_DOP7_PAGE.word = (uint32_t)INDEX<<0)
#define Get__DXO_DOP7_PAGE() p__IP->DXO_DOP7_PAGE.word
#define Set__DXO_DOP7_PAGE_word(x) (p__IP->DXO_DOP7_PAGE.word = x)


//LICN_ITS


#define Get__LICN_ITS_BUSERR() p__IP->LICN_ITS.LICN_ITS_ts.BUSERR
#define Is__LICN_ITS_BUSERR_B_0x0() (p__IP->LICN_ITS.LICN_ITS_ts.BUSERR == BUSERR_B_0x0)
#define BUSERR_B_0x0 0x0    //no error pending
#define Is__LICN_ITS_BUSERR_B_0x1() (p__IP->LICN_ITS.LICN_ITS_ts.BUSERR == BUSERR_B_0x1)
#define BUSERR_B_0x1 0x1    //error pending
#define Get__LICN_ITS() p__IP->LICN_ITS.word


//LICN_ITS_BCLR


#define Set__LICN_ITS_BCLR_BUSERR__B_0x0() (p__IP->LICN_ITS_BCLR.LICN_ITS_BCLR_ts.BUSERR = BUSERR_B_0x0)
#define BUSERR_B_0x0 0x0    //no effect
#define Set__LICN_ITS_BCLR_BUSERR__B_0x1() (p__IP->LICN_ITS_BCLR.LICN_ITS_BCLR_ts.BUSERR = BUSERR_B_0x1)
#define BUSERR_B_0x1 0x1    //clears error
#define Set__LICN_ITS_BCLR(BUSERR) (p__IP->LICN_ITS_BCLR.word = (uint16_t)BUSERR<<0)
#define Set__LICN_ITS_BCLR_word(x) (p__IP->LICN_ITS_BCLR.word = x)


//LICN_ITS_BSET


#define Set__LICN_ITS_BSET_BUSERR__B_0x0() (p__IP->LICN_ITS_BSET.LICN_ITS_BSET_ts.BUSERR = BUSERR_B_0x0)
#define BUSERR_B_0x0 0x0    //no effect
#define Set__LICN_ITS_BSET_BUSERR__B_0x1() (p__IP->LICN_ITS_BSET.LICN_ITS_BSET_ts.BUSERR = BUSERR_B_0x1)
#define BUSERR_B_0x1 0x1    //set error
#define Set__LICN_ITS_BSET(BUSERR) (p__IP->LICN_ITS_BSET.word = (uint16_t)BUSERR<<0)
#define Set__LICN_ITS_BSET_word(x) (p__IP->LICN_ITS_BSET.word = x)


//LICN_ITM


#define Get__LICN_ITM_BUSERR() p__IP->LICN_ITM.LICN_ITM_ts.BUSERR
#define Is__LICN_ITM_BUSERR_B_0x0() (p__IP->LICN_ITM.LICN_ITM_ts.BUSERR == BUSERR_B_0x0)
#define BUSERR_B_0x0 0x0    //does not contribute to LICN interrupt
#define Is__LICN_ITM_BUSERR_B_0x1() (p__IP->LICN_ITM.LICN_ITM_ts.BUSERR == BUSERR_B_0x1)
#define BUSERR_B_0x1 0x1    //contributes to LICN interrupt
#define Get__LICN_ITM() p__IP->LICN_ITM.word


//LICN_ITM_BCLR


#define Set__LICN_ITM_BCLR_BUSERR__B_0x0() (p__IP->LICN_ITM_BCLR.LICN_ITM_BCLR_ts.BUSERR = BUSERR_B_0x0)
#define BUSERR_B_0x0 0x0    //no effect
#define Set__LICN_ITM_BCLR_BUSERR__B_0x1() (p__IP->LICN_ITM_BCLR.LICN_ITM_BCLR_ts.BUSERR = BUSERR_B_0x1)
#define BUSERR_B_0x1 0x1    //clears mask
#define Set__LICN_ITM_BCLR(BUSERR) (p__IP->LICN_ITM_BCLR.word = (uint16_t)BUSERR<<0)
#define Set__LICN_ITM_BCLR_word(x) (p__IP->LICN_ITM_BCLR.word = x)


//LICN_ITM_BSET


#define Set__LICN_ITM_BSET_BUSERR__B_0x0() (p__IP->LICN_ITM_BSET.LICN_ITM_BSET_ts.BUSERR = BUSERR_B_0x0)
#define BUSERR_B_0x0 0x0    //no effect
#define Set__LICN_ITM_BSET_BUSERR__B_0x1() (p__IP->LICN_ITM_BSET.LICN_ITM_BSET_ts.BUSERR = BUSERR_B_0x1)
#define BUSERR_B_0x1 0x1    //sets mask
#define Set__LICN_ITM_BSET(BUSERR) (p__IP->LICN_ITM_BSET.word = (uint32_t)BUSERR<<0)
#define Set__LICN_ITM_BSET_word(x) (p__IP->LICN_ITM_BSET.word = x)


//ISP_MCU_SYS_ADDR_0_LO


#define Get__ISP_MCU_SYS_ADDR_0_LO_REGION_BA_LO() p__IP->ISP_MCU_SYS_ADDR_0_LO.ISP_MCU_SYS_ADDR_0_LO_ts.REGION_BA_LO
#define Set__ISP_MCU_SYS_ADDR_0_LO_REGION_BA_LO(x) (p__IP->ISP_MCU_SYS_ADDR_0_LO.ISP_MCU_SYS_ADDR_0_LO_ts.REGION_BA_LO = x)
#define Set__ISP_MCU_SYS_ADDR_0_LO(REGION_BA_LO) (p__IP->ISP_MCU_SYS_ADDR_0_LO.word = (uint16_t)REGION_BA_LO<<12)
#define Get__ISP_MCU_SYS_ADDR_0_LO() p__IP->ISP_MCU_SYS_ADDR_0_LO.word
#define Set__ISP_MCU_SYS_ADDR_0_LO_word(x) (p__IP->ISP_MCU_SYS_ADDR_0_LO.word = x)


//ISP_MCU_SYS_ADDR_1_LO


#define Get__ISP_MCU_SYS_ADDR_1_LO_REGION_BA_LO() p__IP->ISP_MCU_SYS_ADDR_1_LO.ISP_MCU_SYS_ADDR_1_LO_ts.REGION_BA_LO
#define Set__ISP_MCU_SYS_ADDR_1_LO_REGION_BA_LO(x) (p__IP->ISP_MCU_SYS_ADDR_1_LO.ISP_MCU_SYS_ADDR_1_LO_ts.REGION_BA_LO = x)
#define Set__ISP_MCU_SYS_ADDR_1_LO(REGION_BA_LO) (p__IP->ISP_MCU_SYS_ADDR_1_LO.word = (uint16_t)REGION_BA_LO<<12)
#define Get__ISP_MCU_SYS_ADDR_1_LO() p__IP->ISP_MCU_SYS_ADDR_1_LO.word
#define Set__ISP_MCU_SYS_ADDR_1_LO_word(x) (p__IP->ISP_MCU_SYS_ADDR_1_LO.word = x)


//ISP_MCU_SYS_ADDR_2_LO


#define Get__ISP_MCU_SYS_ADDR_2_LO_REGION_BA_LO() p__IP->ISP_MCU_SYS_ADDR_2_LO.ISP_MCU_SYS_ADDR_2_LO_ts.REGION_BA_LO
#define Set__ISP_MCU_SYS_ADDR_2_LO_REGION_BA_LO(x) (p__IP->ISP_MCU_SYS_ADDR_2_LO.ISP_MCU_SYS_ADDR_2_LO_ts.REGION_BA_LO = x)
#define Set__ISP_MCU_SYS_ADDR_2_LO(REGION_BA_LO) (p__IP->ISP_MCU_SYS_ADDR_2_LO.word = (uint16_t)REGION_BA_LO<<12)
#define Get__ISP_MCU_SYS_ADDR_2_LO() p__IP->ISP_MCU_SYS_ADDR_2_LO.word
#define Set__ISP_MCU_SYS_ADDR_2_LO_word(x) (p__IP->ISP_MCU_SYS_ADDR_2_LO.word = x)


//ISP_MCU_SYS_ADDR_3_LO


#define Get__ISP_MCU_SYS_ADDR_3_LO_REGION_BA_LO() p__IP->ISP_MCU_SYS_ADDR_3_LO.ISP_MCU_SYS_ADDR_3_LO_ts.REGION_BA_LO
#define Set__ISP_MCU_SYS_ADDR_3_LO_REGION_BA_LO(x) (p__IP->ISP_MCU_SYS_ADDR_3_LO.ISP_MCU_SYS_ADDR_3_LO_ts.REGION_BA_LO = x)
#define Set__ISP_MCU_SYS_ADDR_3_LO(REGION_BA_LO) (p__IP->ISP_MCU_SYS_ADDR_3_LO.word = (uint16_t)REGION_BA_LO<<12)
#define Get__ISP_MCU_SYS_ADDR_3_LO() p__IP->ISP_MCU_SYS_ADDR_3_LO.word
#define Set__ISP_MCU_SYS_ADDR_3_LO_word(x) (p__IP->ISP_MCU_SYS_ADDR_3_LO.word = x)


//ISP_MCU_SYS_ADDR_4_LO


#define Get__ISP_MCU_SYS_ADDR_4_LO_REGION_BA_LO() p__IP->ISP_MCU_SYS_ADDR_4_LO.ISP_MCU_SYS_ADDR_4_LO_ts.REGION_BA_LO
#define Set__ISP_MCU_SYS_ADDR_4_LO_REGION_BA_LO(x) (p__IP->ISP_MCU_SYS_ADDR_4_LO.ISP_MCU_SYS_ADDR_4_LO_ts.REGION_BA_LO = x)
#define Set__ISP_MCU_SYS_ADDR_4_LO(REGION_BA_LO) (p__IP->ISP_MCU_SYS_ADDR_4_LO.word = (uint16_t)REGION_BA_LO<<12)
#define Get__ISP_MCU_SYS_ADDR_4_LO() p__IP->ISP_MCU_SYS_ADDR_4_LO.word
#define Set__ISP_MCU_SYS_ADDR_4_LO_word(x) (p__IP->ISP_MCU_SYS_ADDR_4_LO.word = x)


//ISP_MCU_SYS_ADDR_5_LO


#define Get__ISP_MCU_SYS_ADDR_5_LO_REGION_BA_LO() p__IP->ISP_MCU_SYS_ADDR_5_LO.ISP_MCU_SYS_ADDR_5_LO_ts.REGION_BA_LO
#define Set__ISP_MCU_SYS_ADDR_5_LO_REGION_BA_LO(x) (p__IP->ISP_MCU_SYS_ADDR_5_LO.ISP_MCU_SYS_ADDR_5_LO_ts.REGION_BA_LO = x)
#define Set__ISP_MCU_SYS_ADDR_5_LO(REGION_BA_LO) (p__IP->ISP_MCU_SYS_ADDR_5_LO.word = (uint16_t)REGION_BA_LO<<12)
#define Get__ISP_MCU_SYS_ADDR_5_LO() p__IP->ISP_MCU_SYS_ADDR_5_LO.word
#define Set__ISP_MCU_SYS_ADDR_5_LO_word(x) (p__IP->ISP_MCU_SYS_ADDR_5_LO.word = x)


//ISP_MCU_SYS_ADDR_6_LO


#define Get__ISP_MCU_SYS_ADDR_6_LO_REGION_BA_LO() p__IP->ISP_MCU_SYS_ADDR_6_LO.ISP_MCU_SYS_ADDR_6_LO_ts.REGION_BA_LO
#define Set__ISP_MCU_SYS_ADDR_6_LO_REGION_BA_LO(x) (p__IP->ISP_MCU_SYS_ADDR_6_LO.ISP_MCU_SYS_ADDR_6_LO_ts.REGION_BA_LO = x)
#define Set__ISP_MCU_SYS_ADDR_6_LO(REGION_BA_LO) (p__IP->ISP_MCU_SYS_ADDR_6_LO.word = (uint16_t)REGION_BA_LO<<12)
#define Get__ISP_MCU_SYS_ADDR_6_LO() p__IP->ISP_MCU_SYS_ADDR_6_LO.word
#define Set__ISP_MCU_SYS_ADDR_6_LO_word(x) (p__IP->ISP_MCU_SYS_ADDR_6_LO.word = x)


//ISP_MCU_SYS_ADDR_7_LO


#define Get__ISP_MCU_SYS_ADDR_7_LO_REGION_BA_LO() p__IP->ISP_MCU_SYS_ADDR_7_LO.ISP_MCU_SYS_ADDR_7_LO_ts.REGION_BA_LO
#define Set__ISP_MCU_SYS_ADDR_7_LO_REGION_BA_LO(x) (p__IP->ISP_MCU_SYS_ADDR_7_LO.ISP_MCU_SYS_ADDR_7_LO_ts.REGION_BA_LO = x)
#define Set__ISP_MCU_SYS_ADDR_7_LO(REGION_BA_LO) (p__IP->ISP_MCU_SYS_ADDR_7_LO.word = (uint32_t)REGION_BA_LO<<12)
#define Get__ISP_MCU_SYS_ADDR_7_LO() p__IP->ISP_MCU_SYS_ADDR_7_LO.word
#define Set__ISP_MCU_SYS_ADDR_7_LO_word(x) (p__IP->ISP_MCU_SYS_ADDR_7_LO.word = x)


//ISP_MCU_IO_ADDR_0_LO


#define Get__ISP_MCU_IO_ADDR_0_LO_REGION_BA_LO() p__IP->ISP_MCU_IO_ADDR_0_LO.ISP_MCU_IO_ADDR_0_LO_ts.REGION_BA_LO
#define Set__ISP_MCU_IO_ADDR_0_LO_REGION_BA_LO(x) (p__IP->ISP_MCU_IO_ADDR_0_LO.ISP_MCU_IO_ADDR_0_LO_ts.REGION_BA_LO = x)
#define Set__ISP_MCU_IO_ADDR_0_LO(REGION_BA_LO) (p__IP->ISP_MCU_IO_ADDR_0_LO.word = (uint16_t)REGION_BA_LO<<12)
#define Get__ISP_MCU_IO_ADDR_0_LO() p__IP->ISP_MCU_IO_ADDR_0_LO.word
#define Set__ISP_MCU_IO_ADDR_0_LO_word(x) (p__IP->ISP_MCU_IO_ADDR_0_LO.word = x)


//ISP_MCU_IO_ADDR_1_LO


#define Get__ISP_MCU_IO_ADDR_1_LO_REGION_BA_LO() p__IP->ISP_MCU_IO_ADDR_1_LO.ISP_MCU_IO_ADDR_1_LO_ts.REGION_BA_LO
#define Set__ISP_MCU_IO_ADDR_1_LO_REGION_BA_LO(x) (p__IP->ISP_MCU_IO_ADDR_1_LO.ISP_MCU_IO_ADDR_1_LO_ts.REGION_BA_LO = x)
#define Set__ISP_MCU_IO_ADDR_1_LO(REGION_BA_LO) (p__IP->ISP_MCU_IO_ADDR_1_LO.word = (uint16_t)REGION_BA_LO<<12)
#define Get__ISP_MCU_IO_ADDR_1_LO() p__IP->ISP_MCU_IO_ADDR_1_LO.word
#define Set__ISP_MCU_IO_ADDR_1_LO_word(x) (p__IP->ISP_MCU_IO_ADDR_1_LO.word = x)


//ISP_MCU_IO_ADDR_2_LO


#define Get__ISP_MCU_IO_ADDR_2_LO_REGION_BA_LO() p__IP->ISP_MCU_IO_ADDR_2_LO.ISP_MCU_IO_ADDR_2_LO_ts.REGION_BA_LO
#define Set__ISP_MCU_IO_ADDR_2_LO_REGION_BA_LO(x) (p__IP->ISP_MCU_IO_ADDR_2_LO.ISP_MCU_IO_ADDR_2_LO_ts.REGION_BA_LO = x)
#define Set__ISP_MCU_IO_ADDR_2_LO(REGION_BA_LO) (p__IP->ISP_MCU_IO_ADDR_2_LO.word = (uint16_t)REGION_BA_LO<<12)
#define Get__ISP_MCU_IO_ADDR_2_LO() p__IP->ISP_MCU_IO_ADDR_2_LO.word
#define Set__ISP_MCU_IO_ADDR_2_LO_word(x) (p__IP->ISP_MCU_IO_ADDR_2_LO.word = x)


//ISP_MCU_IO_ADDR_3_LO


#define Get__ISP_MCU_IO_ADDR_3_LO_REGION_BA_LO() p__IP->ISP_MCU_IO_ADDR_3_LO.ISP_MCU_IO_ADDR_3_LO_ts.REGION_BA_LO
#define Set__ISP_MCU_IO_ADDR_3_LO_REGION_BA_LO(x) (p__IP->ISP_MCU_IO_ADDR_3_LO.ISP_MCU_IO_ADDR_3_LO_ts.REGION_BA_LO = x)
#define Set__ISP_MCU_IO_ADDR_3_LO(REGION_BA_LO) (p__IP->ISP_MCU_IO_ADDR_3_LO.word = (uint16_t)REGION_BA_LO<<12)
#define Get__ISP_MCU_IO_ADDR_3_LO() p__IP->ISP_MCU_IO_ADDR_3_LO.word
#define Set__ISP_MCU_IO_ADDR_3_LO_word(x) (p__IP->ISP_MCU_IO_ADDR_3_LO.word = x)


//ISP_MCU_IO_ADDR_4_LO


#define Get__ISP_MCU_IO_ADDR_4_LO_REGION_BA_LO() p__IP->ISP_MCU_IO_ADDR_4_LO.ISP_MCU_IO_ADDR_4_LO_ts.REGION_BA_LO
#define Set__ISP_MCU_IO_ADDR_4_LO_REGION_BA_LO(x) (p__IP->ISP_MCU_IO_ADDR_4_LO.ISP_MCU_IO_ADDR_4_LO_ts.REGION_BA_LO = x)
#define Set__ISP_MCU_IO_ADDR_4_LO(REGION_BA_LO) (p__IP->ISP_MCU_IO_ADDR_4_LO.word = (uint16_t)REGION_BA_LO<<12)
#define Get__ISP_MCU_IO_ADDR_4_LO() p__IP->ISP_MCU_IO_ADDR_4_LO.word
#define Set__ISP_MCU_IO_ADDR_4_LO_word(x) (p__IP->ISP_MCU_IO_ADDR_4_LO.word = x)


//ISP_MCU_IO_ADDR_5_LO


#define Get__ISP_MCU_IO_ADDR_5_LO_REGION_BA_LO() p__IP->ISP_MCU_IO_ADDR_5_LO.ISP_MCU_IO_ADDR_5_LO_ts.REGION_BA_LO
#define Set__ISP_MCU_IO_ADDR_5_LO_REGION_BA_LO(x) (p__IP->ISP_MCU_IO_ADDR_5_LO.ISP_MCU_IO_ADDR_5_LO_ts.REGION_BA_LO = x)
#define Set__ISP_MCU_IO_ADDR_5_LO(REGION_BA_LO) (p__IP->ISP_MCU_IO_ADDR_5_LO.word = (uint16_t)REGION_BA_LO<<12)
#define Get__ISP_MCU_IO_ADDR_5_LO() p__IP->ISP_MCU_IO_ADDR_5_LO.word
#define Set__ISP_MCU_IO_ADDR_5_LO_word(x) (p__IP->ISP_MCU_IO_ADDR_5_LO.word = x)


//ISP_MCU_IO_ADDR_6_LO


#define Get__ISP_MCU_IO_ADDR_6_LO_REGION_BA_LO() p__IP->ISP_MCU_IO_ADDR_6_LO.ISP_MCU_IO_ADDR_6_LO_ts.REGION_BA_LO
#define Set__ISP_MCU_IO_ADDR_6_LO_REGION_BA_LO(x) (p__IP->ISP_MCU_IO_ADDR_6_LO.ISP_MCU_IO_ADDR_6_LO_ts.REGION_BA_LO = x)
#define Set__ISP_MCU_IO_ADDR_6_LO(REGION_BA_LO) (p__IP->ISP_MCU_IO_ADDR_6_LO.word = (uint16_t)REGION_BA_LO<<12)
#define Get__ISP_MCU_IO_ADDR_6_LO() p__IP->ISP_MCU_IO_ADDR_6_LO.word
#define Set__ISP_MCU_IO_ADDR_6_LO_word(x) (p__IP->ISP_MCU_IO_ADDR_6_LO.word = x)


//ISP_MCU_IO_ADDR_7_LO


#define Get__ISP_MCU_IO_ADDR_7_LO_REGION_BA_LO() p__IP->ISP_MCU_IO_ADDR_7_LO.ISP_MCU_IO_ADDR_7_LO_ts.REGION_BA_LO
#define Set__ISP_MCU_IO_ADDR_7_LO_REGION_BA_LO(x) (p__IP->ISP_MCU_IO_ADDR_7_LO.ISP_MCU_IO_ADDR_7_LO_ts.REGION_BA_LO = x)
#define Set__ISP_MCU_IO_ADDR_7_LO(REGION_BA_LO) (p__IP->ISP_MCU_IO_ADDR_7_LO.word = (uint32_t)REGION_BA_LO<<12)
#define Get__ISP_MCU_IO_ADDR_7_LO() p__IP->ISP_MCU_IO_ADDR_7_LO.word
#define Set__ISP_MCU_IO_ADDR_7_LO_word(x) (p__IP->ISP_MCU_IO_ADDR_7_LO.word = x)


//ISP_MCU_IO_SIZE0


#define Get__ISP_MCU_IO_SIZE0_REGION_SZ() p__IP->ISP_MCU_IO_SIZE0.ISP_MCU_IO_SIZE0_ts.REGION_SZ
#define Is__ISP_MCU_IO_SIZE0_REGION_SZ_B_0x0() (p__IP->ISP_MCU_IO_SIZE0.ISP_MCU_IO_SIZE0_ts.REGION_SZ == REGION_SZ_B_0x0)
#define Set__ISP_MCU_IO_SIZE0_REGION_SZ__B_0x0() (p__IP->ISP_MCU_IO_SIZE0.ISP_MCU_IO_SIZE0_ts.REGION_SZ = REGION_SZ_B_0x0)
#define REGION_SZ_B_0x0 0x0    //16MB
#define Is__ISP_MCU_IO_SIZE0_REGION_SZ_B_0x1() (p__IP->ISP_MCU_IO_SIZE0.ISP_MCU_IO_SIZE0_ts.REGION_SZ == REGION_SZ_B_0x1)
#define Set__ISP_MCU_IO_SIZE0_REGION_SZ__B_0x1() (p__IP->ISP_MCU_IO_SIZE0.ISP_MCU_IO_SIZE0_ts.REGION_SZ = REGION_SZ_B_0x1)
#define REGION_SZ_B_0x1 0x1    //1MB
#define Is__ISP_MCU_IO_SIZE0_REGION_SZ_B_0x2() (p__IP->ISP_MCU_IO_SIZE0.ISP_MCU_IO_SIZE0_ts.REGION_SZ == REGION_SZ_B_0x2)
#define Set__ISP_MCU_IO_SIZE0_REGION_SZ__B_0x2() (p__IP->ISP_MCU_IO_SIZE0.ISP_MCU_IO_SIZE0_ts.REGION_SZ = REGION_SZ_B_0x2)
#define REGION_SZ_B_0x2 0x2    //64kB
#define Is__ISP_MCU_IO_SIZE0_REGION_SZ_B_0x3() (p__IP->ISP_MCU_IO_SIZE0.ISP_MCU_IO_SIZE0_ts.REGION_SZ == REGION_SZ_B_0x3)
#define Set__ISP_MCU_IO_SIZE0_REGION_SZ__B_0x3() (p__IP->ISP_MCU_IO_SIZE0.ISP_MCU_IO_SIZE0_ts.REGION_SZ = REGION_SZ_B_0x3)
#define REGION_SZ_B_0x3 0x3    //32MB
#define Is__ISP_MCU_IO_SIZE0_REGION_SZ_B_0x4() (p__IP->ISP_MCU_IO_SIZE0.ISP_MCU_IO_SIZE0_ts.REGION_SZ == REGION_SZ_B_0x4)
#define Set__ISP_MCU_IO_SIZE0_REGION_SZ__B_0x4() (p__IP->ISP_MCU_IO_SIZE0.ISP_MCU_IO_SIZE0_ts.REGION_SZ = REGION_SZ_B_0x4)
#define REGION_SZ_B_0x4 0x4    //4kB
#define Is__ISP_MCU_IO_SIZE0_REGION_SZ_B_0x5() (p__IP->ISP_MCU_IO_SIZE0.ISP_MCU_IO_SIZE0_ts.REGION_SZ == REGION_SZ_B_0x5)
#define Set__ISP_MCU_IO_SIZE0_REGION_SZ__B_0x5() (p__IP->ISP_MCU_IO_SIZE0.ISP_MCU_IO_SIZE0_ts.REGION_SZ = REGION_SZ_B_0x5)
#define REGION_SZ_B_0x5 0x5    //8kB
#define Is__ISP_MCU_IO_SIZE0_REGION_SZ_B_0x6() (p__IP->ISP_MCU_IO_SIZE0.ISP_MCU_IO_SIZE0_ts.REGION_SZ == REGION_SZ_B_0x6)
#define Set__ISP_MCU_IO_SIZE0_REGION_SZ__B_0x6() (p__IP->ISP_MCU_IO_SIZE0.ISP_MCU_IO_SIZE0_ts.REGION_SZ = REGION_SZ_B_0x6)
#define REGION_SZ_B_0x6 0x6    //16kB
#define Is__ISP_MCU_IO_SIZE0_REGION_SZ_B_0x7() (p__IP->ISP_MCU_IO_SIZE0.ISP_MCU_IO_SIZE0_ts.REGION_SZ == REGION_SZ_B_0x7)
#define Set__ISP_MCU_IO_SIZE0_REGION_SZ__B_0x7() (p__IP->ISP_MCU_IO_SIZE0.ISP_MCU_IO_SIZE0_ts.REGION_SZ = REGION_SZ_B_0x7)
#define REGION_SZ_B_0x7 0x7    //32kB
#define Set__ISP_MCU_IO_SIZE0(REGION_SZ) (p__IP->ISP_MCU_IO_SIZE0.word = (uint16_t)REGION_SZ<<0)
#define Get__ISP_MCU_IO_SIZE0() p__IP->ISP_MCU_IO_SIZE0.word
#define Set__ISP_MCU_IO_SIZE0_word(x) (p__IP->ISP_MCU_IO_SIZE0.word = x)


//ISP_MCU_IO_SIZE1


#define Get__ISP_MCU_IO_SIZE1_REGION_SZ() p__IP->ISP_MCU_IO_SIZE1.ISP_MCU_IO_SIZE1_ts.REGION_SZ
#define Is__ISP_MCU_IO_SIZE1_REGION_SZ_B_0x0() (p__IP->ISP_MCU_IO_SIZE1.ISP_MCU_IO_SIZE1_ts.REGION_SZ == REGION_SZ_B_0x0)
#define Set__ISP_MCU_IO_SIZE1_REGION_SZ__B_0x0() (p__IP->ISP_MCU_IO_SIZE1.ISP_MCU_IO_SIZE1_ts.REGION_SZ = REGION_SZ_B_0x0)
#define REGION_SZ_B_0x0 0x0    //16MB
#define Is__ISP_MCU_IO_SIZE1_REGION_SZ_B_0x1() (p__IP->ISP_MCU_IO_SIZE1.ISP_MCU_IO_SIZE1_ts.REGION_SZ == REGION_SZ_B_0x1)
#define Set__ISP_MCU_IO_SIZE1_REGION_SZ__B_0x1() (p__IP->ISP_MCU_IO_SIZE1.ISP_MCU_IO_SIZE1_ts.REGION_SZ = REGION_SZ_B_0x1)
#define REGION_SZ_B_0x1 0x1    //1MB
#define Is__ISP_MCU_IO_SIZE1_REGION_SZ_B_0x2() (p__IP->ISP_MCU_IO_SIZE1.ISP_MCU_IO_SIZE1_ts.REGION_SZ == REGION_SZ_B_0x2)
#define Set__ISP_MCU_IO_SIZE1_REGION_SZ__B_0x2() (p__IP->ISP_MCU_IO_SIZE1.ISP_MCU_IO_SIZE1_ts.REGION_SZ = REGION_SZ_B_0x2)
#define REGION_SZ_B_0x2 0x2    //64kB
#define Is__ISP_MCU_IO_SIZE1_REGION_SZ_B_0x3() (p__IP->ISP_MCU_IO_SIZE1.ISP_MCU_IO_SIZE1_ts.REGION_SZ == REGION_SZ_B_0x3)
#define Set__ISP_MCU_IO_SIZE1_REGION_SZ__B_0x3() (p__IP->ISP_MCU_IO_SIZE1.ISP_MCU_IO_SIZE1_ts.REGION_SZ = REGION_SZ_B_0x3)
#define REGION_SZ_B_0x3 0x3    //32MB
#define Is__ISP_MCU_IO_SIZE1_REGION_SZ_B_0x4() (p__IP->ISP_MCU_IO_SIZE1.ISP_MCU_IO_SIZE1_ts.REGION_SZ == REGION_SZ_B_0x4)
#define Set__ISP_MCU_IO_SIZE1_REGION_SZ__B_0x4() (p__IP->ISP_MCU_IO_SIZE1.ISP_MCU_IO_SIZE1_ts.REGION_SZ = REGION_SZ_B_0x4)
#define REGION_SZ_B_0x4 0x4    //4kB
#define Is__ISP_MCU_IO_SIZE1_REGION_SZ_B_0x5() (p__IP->ISP_MCU_IO_SIZE1.ISP_MCU_IO_SIZE1_ts.REGION_SZ == REGION_SZ_B_0x5)
#define Set__ISP_MCU_IO_SIZE1_REGION_SZ__B_0x5() (p__IP->ISP_MCU_IO_SIZE1.ISP_MCU_IO_SIZE1_ts.REGION_SZ = REGION_SZ_B_0x5)
#define REGION_SZ_B_0x5 0x5    //8kB
#define Is__ISP_MCU_IO_SIZE1_REGION_SZ_B_0x6() (p__IP->ISP_MCU_IO_SIZE1.ISP_MCU_IO_SIZE1_ts.REGION_SZ == REGION_SZ_B_0x6)
#define Set__ISP_MCU_IO_SIZE1_REGION_SZ__B_0x6() (p__IP->ISP_MCU_IO_SIZE1.ISP_MCU_IO_SIZE1_ts.REGION_SZ = REGION_SZ_B_0x6)
#define REGION_SZ_B_0x6 0x6    //16kB
#define Is__ISP_MCU_IO_SIZE1_REGION_SZ_B_0x7() (p__IP->ISP_MCU_IO_SIZE1.ISP_MCU_IO_SIZE1_ts.REGION_SZ == REGION_SZ_B_0x7)
#define Set__ISP_MCU_IO_SIZE1_REGION_SZ__B_0x7() (p__IP->ISP_MCU_IO_SIZE1.ISP_MCU_IO_SIZE1_ts.REGION_SZ = REGION_SZ_B_0x7)
#define REGION_SZ_B_0x7 0x7    //32kB
#define Set__ISP_MCU_IO_SIZE1(REGION_SZ) (p__IP->ISP_MCU_IO_SIZE1.word = (uint16_t)REGION_SZ<<0)
#define Get__ISP_MCU_IO_SIZE1() p__IP->ISP_MCU_IO_SIZE1.word
#define Set__ISP_MCU_IO_SIZE1_word(x) (p__IP->ISP_MCU_IO_SIZE1.word = x)


//ISP_MCU_IO_SIZE2


#define Get__ISP_MCU_IO_SIZE2_REGION_SZ() p__IP->ISP_MCU_IO_SIZE2.ISP_MCU_IO_SIZE2_ts.REGION_SZ
#define Is__ISP_MCU_IO_SIZE2_REGION_SZ_B_0x0() (p__IP->ISP_MCU_IO_SIZE2.ISP_MCU_IO_SIZE2_ts.REGION_SZ == REGION_SZ_B_0x0)
#define Set__ISP_MCU_IO_SIZE2_REGION_SZ__B_0x0() (p__IP->ISP_MCU_IO_SIZE2.ISP_MCU_IO_SIZE2_ts.REGION_SZ = REGION_SZ_B_0x0)
#define REGION_SZ_B_0x0 0x0    //16MB
#define Is__ISP_MCU_IO_SIZE2_REGION_SZ_B_0x1() (p__IP->ISP_MCU_IO_SIZE2.ISP_MCU_IO_SIZE2_ts.REGION_SZ == REGION_SZ_B_0x1)
#define Set__ISP_MCU_IO_SIZE2_REGION_SZ__B_0x1() (p__IP->ISP_MCU_IO_SIZE2.ISP_MCU_IO_SIZE2_ts.REGION_SZ = REGION_SZ_B_0x1)
#define REGION_SZ_B_0x1 0x1    //1MB
#define Is__ISP_MCU_IO_SIZE2_REGION_SZ_B_0x2() (p__IP->ISP_MCU_IO_SIZE2.ISP_MCU_IO_SIZE2_ts.REGION_SZ == REGION_SZ_B_0x2)
#define Set__ISP_MCU_IO_SIZE2_REGION_SZ__B_0x2() (p__IP->ISP_MCU_IO_SIZE2.ISP_MCU_IO_SIZE2_ts.REGION_SZ = REGION_SZ_B_0x2)
#define REGION_SZ_B_0x2 0x2    //64kB
#define Is__ISP_MCU_IO_SIZE2_REGION_SZ_B_0x3() (p__IP->ISP_MCU_IO_SIZE2.ISP_MCU_IO_SIZE2_ts.REGION_SZ == REGION_SZ_B_0x3)
#define Set__ISP_MCU_IO_SIZE2_REGION_SZ__B_0x3() (p__IP->ISP_MCU_IO_SIZE2.ISP_MCU_IO_SIZE2_ts.REGION_SZ = REGION_SZ_B_0x3)
#define REGION_SZ_B_0x3 0x3    //32MB
#define Is__ISP_MCU_IO_SIZE2_REGION_SZ_B_0x4() (p__IP->ISP_MCU_IO_SIZE2.ISP_MCU_IO_SIZE2_ts.REGION_SZ == REGION_SZ_B_0x4)
#define Set__ISP_MCU_IO_SIZE2_REGION_SZ__B_0x4() (p__IP->ISP_MCU_IO_SIZE2.ISP_MCU_IO_SIZE2_ts.REGION_SZ = REGION_SZ_B_0x4)
#define REGION_SZ_B_0x4 0x4    //4kB
#define Is__ISP_MCU_IO_SIZE2_REGION_SZ_B_0x5() (p__IP->ISP_MCU_IO_SIZE2.ISP_MCU_IO_SIZE2_ts.REGION_SZ == REGION_SZ_B_0x5)
#define Set__ISP_MCU_IO_SIZE2_REGION_SZ__B_0x5() (p__IP->ISP_MCU_IO_SIZE2.ISP_MCU_IO_SIZE2_ts.REGION_SZ = REGION_SZ_B_0x5)
#define REGION_SZ_B_0x5 0x5    //8kB
#define Is__ISP_MCU_IO_SIZE2_REGION_SZ_B_0x6() (p__IP->ISP_MCU_IO_SIZE2.ISP_MCU_IO_SIZE2_ts.REGION_SZ == REGION_SZ_B_0x6)
#define Set__ISP_MCU_IO_SIZE2_REGION_SZ__B_0x6() (p__IP->ISP_MCU_IO_SIZE2.ISP_MCU_IO_SIZE2_ts.REGION_SZ = REGION_SZ_B_0x6)
#define REGION_SZ_B_0x6 0x6    //16kB
#define Is__ISP_MCU_IO_SIZE2_REGION_SZ_B_0x7() (p__IP->ISP_MCU_IO_SIZE2.ISP_MCU_IO_SIZE2_ts.REGION_SZ == REGION_SZ_B_0x7)
#define Set__ISP_MCU_IO_SIZE2_REGION_SZ__B_0x7() (p__IP->ISP_MCU_IO_SIZE2.ISP_MCU_IO_SIZE2_ts.REGION_SZ = REGION_SZ_B_0x7)
#define REGION_SZ_B_0x7 0x7    //32kB
#define Set__ISP_MCU_IO_SIZE2(REGION_SZ) (p__IP->ISP_MCU_IO_SIZE2.word = (uint16_t)REGION_SZ<<0)
#define Get__ISP_MCU_IO_SIZE2() p__IP->ISP_MCU_IO_SIZE2.word
#define Set__ISP_MCU_IO_SIZE2_word(x) (p__IP->ISP_MCU_IO_SIZE2.word = x)


//ISP_MCU_IO_SIZE3


#define Get__ISP_MCU_IO_SIZE3_REGION_SZ() p__IP->ISP_MCU_IO_SIZE3.ISP_MCU_IO_SIZE3_ts.REGION_SZ
#define Is__ISP_MCU_IO_SIZE3_REGION_SZ_B_0x0() (p__IP->ISP_MCU_IO_SIZE3.ISP_MCU_IO_SIZE3_ts.REGION_SZ == REGION_SZ_B_0x0)
#define Set__ISP_MCU_IO_SIZE3_REGION_SZ__B_0x0() (p__IP->ISP_MCU_IO_SIZE3.ISP_MCU_IO_SIZE3_ts.REGION_SZ = REGION_SZ_B_0x0)
#define REGION_SZ_B_0x0 0x0    //16MB
#define Is__ISP_MCU_IO_SIZE3_REGION_SZ_B_0x1() (p__IP->ISP_MCU_IO_SIZE3.ISP_MCU_IO_SIZE3_ts.REGION_SZ == REGION_SZ_B_0x1)
#define Set__ISP_MCU_IO_SIZE3_REGION_SZ__B_0x1() (p__IP->ISP_MCU_IO_SIZE3.ISP_MCU_IO_SIZE3_ts.REGION_SZ = REGION_SZ_B_0x1)
#define REGION_SZ_B_0x1 0x1    //1MB
#define Is__ISP_MCU_IO_SIZE3_REGION_SZ_B_0x2() (p__IP->ISP_MCU_IO_SIZE3.ISP_MCU_IO_SIZE3_ts.REGION_SZ == REGION_SZ_B_0x2)
#define Set__ISP_MCU_IO_SIZE3_REGION_SZ__B_0x2() (p__IP->ISP_MCU_IO_SIZE3.ISP_MCU_IO_SIZE3_ts.REGION_SZ = REGION_SZ_B_0x2)
#define REGION_SZ_B_0x2 0x2    //64kB
#define Is__ISP_MCU_IO_SIZE3_REGION_SZ_B_0x3() (p__IP->ISP_MCU_IO_SIZE3.ISP_MCU_IO_SIZE3_ts.REGION_SZ == REGION_SZ_B_0x3)
#define Set__ISP_MCU_IO_SIZE3_REGION_SZ__B_0x3() (p__IP->ISP_MCU_IO_SIZE3.ISP_MCU_IO_SIZE3_ts.REGION_SZ = REGION_SZ_B_0x3)
#define REGION_SZ_B_0x3 0x3    //32MB
#define Is__ISP_MCU_IO_SIZE3_REGION_SZ_B_0x4() (p__IP->ISP_MCU_IO_SIZE3.ISP_MCU_IO_SIZE3_ts.REGION_SZ == REGION_SZ_B_0x4)
#define Set__ISP_MCU_IO_SIZE3_REGION_SZ__B_0x4() (p__IP->ISP_MCU_IO_SIZE3.ISP_MCU_IO_SIZE3_ts.REGION_SZ = REGION_SZ_B_0x4)
#define REGION_SZ_B_0x4 0x4    //4kB
#define Is__ISP_MCU_IO_SIZE3_REGION_SZ_B_0x5() (p__IP->ISP_MCU_IO_SIZE3.ISP_MCU_IO_SIZE3_ts.REGION_SZ == REGION_SZ_B_0x5)
#define Set__ISP_MCU_IO_SIZE3_REGION_SZ__B_0x5() (p__IP->ISP_MCU_IO_SIZE3.ISP_MCU_IO_SIZE3_ts.REGION_SZ = REGION_SZ_B_0x5)
#define REGION_SZ_B_0x5 0x5    //8kB
#define Is__ISP_MCU_IO_SIZE3_REGION_SZ_B_0x6() (p__IP->ISP_MCU_IO_SIZE3.ISP_MCU_IO_SIZE3_ts.REGION_SZ == REGION_SZ_B_0x6)
#define Set__ISP_MCU_IO_SIZE3_REGION_SZ__B_0x6() (p__IP->ISP_MCU_IO_SIZE3.ISP_MCU_IO_SIZE3_ts.REGION_SZ = REGION_SZ_B_0x6)
#define REGION_SZ_B_0x6 0x6    //16kB
#define Is__ISP_MCU_IO_SIZE3_REGION_SZ_B_0x7() (p__IP->ISP_MCU_IO_SIZE3.ISP_MCU_IO_SIZE3_ts.REGION_SZ == REGION_SZ_B_0x7)
#define Set__ISP_MCU_IO_SIZE3_REGION_SZ__B_0x7() (p__IP->ISP_MCU_IO_SIZE3.ISP_MCU_IO_SIZE3_ts.REGION_SZ = REGION_SZ_B_0x7)
#define REGION_SZ_B_0x7 0x7    //32kB
#define Set__ISP_MCU_IO_SIZE3(REGION_SZ) (p__IP->ISP_MCU_IO_SIZE3.word = (uint16_t)REGION_SZ<<0)
#define Get__ISP_MCU_IO_SIZE3() p__IP->ISP_MCU_IO_SIZE3.word
#define Set__ISP_MCU_IO_SIZE3_word(x) (p__IP->ISP_MCU_IO_SIZE3.word = x)


//ISP_MCU_IO_SIZE4


#define Get__ISP_MCU_IO_SIZE4_REGION_SZ() p__IP->ISP_MCU_IO_SIZE4.ISP_MCU_IO_SIZE4_ts.REGION_SZ
#define Is__ISP_MCU_IO_SIZE4_REGION_SZ_B_0x0() (p__IP->ISP_MCU_IO_SIZE4.ISP_MCU_IO_SIZE4_ts.REGION_SZ == REGION_SZ_B_0x0)
#define Set__ISP_MCU_IO_SIZE4_REGION_SZ__B_0x0() (p__IP->ISP_MCU_IO_SIZE4.ISP_MCU_IO_SIZE4_ts.REGION_SZ = REGION_SZ_B_0x0)
#define REGION_SZ_B_0x0 0x0    //16MB
#define Is__ISP_MCU_IO_SIZE4_REGION_SZ_B_0x1() (p__IP->ISP_MCU_IO_SIZE4.ISP_MCU_IO_SIZE4_ts.REGION_SZ == REGION_SZ_B_0x1)
#define Set__ISP_MCU_IO_SIZE4_REGION_SZ__B_0x1() (p__IP->ISP_MCU_IO_SIZE4.ISP_MCU_IO_SIZE4_ts.REGION_SZ = REGION_SZ_B_0x1)
#define REGION_SZ_B_0x1 0x1    //1MB
#define Is__ISP_MCU_IO_SIZE4_REGION_SZ_B_0x2() (p__IP->ISP_MCU_IO_SIZE4.ISP_MCU_IO_SIZE4_ts.REGION_SZ == REGION_SZ_B_0x2)
#define Set__ISP_MCU_IO_SIZE4_REGION_SZ__B_0x2() (p__IP->ISP_MCU_IO_SIZE4.ISP_MCU_IO_SIZE4_ts.REGION_SZ = REGION_SZ_B_0x2)
#define REGION_SZ_B_0x2 0x2    //64kB
#define Is__ISP_MCU_IO_SIZE4_REGION_SZ_B_0x3() (p__IP->ISP_MCU_IO_SIZE4.ISP_MCU_IO_SIZE4_ts.REGION_SZ == REGION_SZ_B_0x3)
#define Set__ISP_MCU_IO_SIZE4_REGION_SZ__B_0x3() (p__IP->ISP_MCU_IO_SIZE4.ISP_MCU_IO_SIZE4_ts.REGION_SZ = REGION_SZ_B_0x3)
#define REGION_SZ_B_0x3 0x3    //32MB
#define Is__ISP_MCU_IO_SIZE4_REGION_SZ_B_0x4() (p__IP->ISP_MCU_IO_SIZE4.ISP_MCU_IO_SIZE4_ts.REGION_SZ == REGION_SZ_B_0x4)
#define Set__ISP_MCU_IO_SIZE4_REGION_SZ__B_0x4() (p__IP->ISP_MCU_IO_SIZE4.ISP_MCU_IO_SIZE4_ts.REGION_SZ = REGION_SZ_B_0x4)
#define REGION_SZ_B_0x4 0x4    //4kB
#define Is__ISP_MCU_IO_SIZE4_REGION_SZ_B_0x5() (p__IP->ISP_MCU_IO_SIZE4.ISP_MCU_IO_SIZE4_ts.REGION_SZ == REGION_SZ_B_0x5)
#define Set__ISP_MCU_IO_SIZE4_REGION_SZ__B_0x5() (p__IP->ISP_MCU_IO_SIZE4.ISP_MCU_IO_SIZE4_ts.REGION_SZ = REGION_SZ_B_0x5)
#define REGION_SZ_B_0x5 0x5    //8kB
#define Is__ISP_MCU_IO_SIZE4_REGION_SZ_B_0x6() (p__IP->ISP_MCU_IO_SIZE4.ISP_MCU_IO_SIZE4_ts.REGION_SZ == REGION_SZ_B_0x6)
#define Set__ISP_MCU_IO_SIZE4_REGION_SZ__B_0x6() (p__IP->ISP_MCU_IO_SIZE4.ISP_MCU_IO_SIZE4_ts.REGION_SZ = REGION_SZ_B_0x6)
#define REGION_SZ_B_0x6 0x6    //16kB
#define Is__ISP_MCU_IO_SIZE4_REGION_SZ_B_0x7() (p__IP->ISP_MCU_IO_SIZE4.ISP_MCU_IO_SIZE4_ts.REGION_SZ == REGION_SZ_B_0x7)
#define Set__ISP_MCU_IO_SIZE4_REGION_SZ__B_0x7() (p__IP->ISP_MCU_IO_SIZE4.ISP_MCU_IO_SIZE4_ts.REGION_SZ = REGION_SZ_B_0x7)
#define REGION_SZ_B_0x7 0x7    //32kB
#define Set__ISP_MCU_IO_SIZE4(REGION_SZ) (p__IP->ISP_MCU_IO_SIZE4.word = (uint16_t)REGION_SZ<<0)
#define Get__ISP_MCU_IO_SIZE4() p__IP->ISP_MCU_IO_SIZE4.word
#define Set__ISP_MCU_IO_SIZE4_word(x) (p__IP->ISP_MCU_IO_SIZE4.word = x)


//ISP_MCU_IO_SIZE5


#define Get__ISP_MCU_IO_SIZE5_REGION_SZ() p__IP->ISP_MCU_IO_SIZE5.ISP_MCU_IO_SIZE5_ts.REGION_SZ
#define Is__ISP_MCU_IO_SIZE5_REGION_SZ_B_0x0() (p__IP->ISP_MCU_IO_SIZE5.ISP_MCU_IO_SIZE5_ts.REGION_SZ == REGION_SZ_B_0x0)
#define Set__ISP_MCU_IO_SIZE5_REGION_SZ__B_0x0() (p__IP->ISP_MCU_IO_SIZE5.ISP_MCU_IO_SIZE5_ts.REGION_SZ = REGION_SZ_B_0x0)
#define REGION_SZ_B_0x0 0x0    //16MB
#define Is__ISP_MCU_IO_SIZE5_REGION_SZ_B_0x1() (p__IP->ISP_MCU_IO_SIZE5.ISP_MCU_IO_SIZE5_ts.REGION_SZ == REGION_SZ_B_0x1)
#define Set__ISP_MCU_IO_SIZE5_REGION_SZ__B_0x1() (p__IP->ISP_MCU_IO_SIZE5.ISP_MCU_IO_SIZE5_ts.REGION_SZ = REGION_SZ_B_0x1)
#define REGION_SZ_B_0x1 0x1    //1MB
#define Is__ISP_MCU_IO_SIZE5_REGION_SZ_B_0x2() (p__IP->ISP_MCU_IO_SIZE5.ISP_MCU_IO_SIZE5_ts.REGION_SZ == REGION_SZ_B_0x2)
#define Set__ISP_MCU_IO_SIZE5_REGION_SZ__B_0x2() (p__IP->ISP_MCU_IO_SIZE5.ISP_MCU_IO_SIZE5_ts.REGION_SZ = REGION_SZ_B_0x2)
#define REGION_SZ_B_0x2 0x2    //64kB
#define Is__ISP_MCU_IO_SIZE5_REGION_SZ_B_0x3() (p__IP->ISP_MCU_IO_SIZE5.ISP_MCU_IO_SIZE5_ts.REGION_SZ == REGION_SZ_B_0x3)
#define Set__ISP_MCU_IO_SIZE5_REGION_SZ__B_0x3() (p__IP->ISP_MCU_IO_SIZE5.ISP_MCU_IO_SIZE5_ts.REGION_SZ = REGION_SZ_B_0x3)
#define REGION_SZ_B_0x3 0x3    //32MB
#define Is__ISP_MCU_IO_SIZE5_REGION_SZ_B_0x4() (p__IP->ISP_MCU_IO_SIZE5.ISP_MCU_IO_SIZE5_ts.REGION_SZ == REGION_SZ_B_0x4)
#define Set__ISP_MCU_IO_SIZE5_REGION_SZ__B_0x4() (p__IP->ISP_MCU_IO_SIZE5.ISP_MCU_IO_SIZE5_ts.REGION_SZ = REGION_SZ_B_0x4)
#define REGION_SZ_B_0x4 0x4    //4kB
#define Is__ISP_MCU_IO_SIZE5_REGION_SZ_B_0x5() (p__IP->ISP_MCU_IO_SIZE5.ISP_MCU_IO_SIZE5_ts.REGION_SZ == REGION_SZ_B_0x5)
#define Set__ISP_MCU_IO_SIZE5_REGION_SZ__B_0x5() (p__IP->ISP_MCU_IO_SIZE5.ISP_MCU_IO_SIZE5_ts.REGION_SZ = REGION_SZ_B_0x5)
#define REGION_SZ_B_0x5 0x5    //8kB
#define Is__ISP_MCU_IO_SIZE5_REGION_SZ_B_0x6() (p__IP->ISP_MCU_IO_SIZE5.ISP_MCU_IO_SIZE5_ts.REGION_SZ == REGION_SZ_B_0x6)
#define Set__ISP_MCU_IO_SIZE5_REGION_SZ__B_0x6() (p__IP->ISP_MCU_IO_SIZE5.ISP_MCU_IO_SIZE5_ts.REGION_SZ = REGION_SZ_B_0x6)
#define REGION_SZ_B_0x6 0x6    //16kB
#define Is__ISP_MCU_IO_SIZE5_REGION_SZ_B_0x7() (p__IP->ISP_MCU_IO_SIZE5.ISP_MCU_IO_SIZE5_ts.REGION_SZ == REGION_SZ_B_0x7)
#define Set__ISP_MCU_IO_SIZE5_REGION_SZ__B_0x7() (p__IP->ISP_MCU_IO_SIZE5.ISP_MCU_IO_SIZE5_ts.REGION_SZ = REGION_SZ_B_0x7)
#define REGION_SZ_B_0x7 0x7    //32kB
#define Set__ISP_MCU_IO_SIZE5(REGION_SZ) (p__IP->ISP_MCU_IO_SIZE5.word = (uint16_t)REGION_SZ<<0)
#define Get__ISP_MCU_IO_SIZE5() p__IP->ISP_MCU_IO_SIZE5.word
#define Set__ISP_MCU_IO_SIZE5_word(x) (p__IP->ISP_MCU_IO_SIZE5.word = x)


//ISP_MCU_IO_SIZE6


#define Get__ISP_MCU_IO_SIZE6_REGION_SZ() p__IP->ISP_MCU_IO_SIZE6.ISP_MCU_IO_SIZE6_ts.REGION_SZ
#define Is__ISP_MCU_IO_SIZE6_REGION_SZ_B_0x0() (p__IP->ISP_MCU_IO_SIZE6.ISP_MCU_IO_SIZE6_ts.REGION_SZ == REGION_SZ_B_0x0)
#define Set__ISP_MCU_IO_SIZE6_REGION_SZ__B_0x0() (p__IP->ISP_MCU_IO_SIZE6.ISP_MCU_IO_SIZE6_ts.REGION_SZ = REGION_SZ_B_0x0)
#define REGION_SZ_B_0x0 0x0    //16MB
#define Is__ISP_MCU_IO_SIZE6_REGION_SZ_B_0x1() (p__IP->ISP_MCU_IO_SIZE6.ISP_MCU_IO_SIZE6_ts.REGION_SZ == REGION_SZ_B_0x1)
#define Set__ISP_MCU_IO_SIZE6_REGION_SZ__B_0x1() (p__IP->ISP_MCU_IO_SIZE6.ISP_MCU_IO_SIZE6_ts.REGION_SZ = REGION_SZ_B_0x1)
#define REGION_SZ_B_0x1 0x1    //1MB
#define Is__ISP_MCU_IO_SIZE6_REGION_SZ_B_0x2() (p__IP->ISP_MCU_IO_SIZE6.ISP_MCU_IO_SIZE6_ts.REGION_SZ == REGION_SZ_B_0x2)
#define Set__ISP_MCU_IO_SIZE6_REGION_SZ__B_0x2() (p__IP->ISP_MCU_IO_SIZE6.ISP_MCU_IO_SIZE6_ts.REGION_SZ = REGION_SZ_B_0x2)
#define REGION_SZ_B_0x2 0x2    //64kB
#define Is__ISP_MCU_IO_SIZE6_REGION_SZ_B_0x3() (p__IP->ISP_MCU_IO_SIZE6.ISP_MCU_IO_SIZE6_ts.REGION_SZ == REGION_SZ_B_0x3)
#define Set__ISP_MCU_IO_SIZE6_REGION_SZ__B_0x3() (p__IP->ISP_MCU_IO_SIZE6.ISP_MCU_IO_SIZE6_ts.REGION_SZ = REGION_SZ_B_0x3)
#define REGION_SZ_B_0x3 0x3    //32MB
#define Is__ISP_MCU_IO_SIZE6_REGION_SZ_B_0x4() (p__IP->ISP_MCU_IO_SIZE6.ISP_MCU_IO_SIZE6_ts.REGION_SZ == REGION_SZ_B_0x4)
#define Set__ISP_MCU_IO_SIZE6_REGION_SZ__B_0x4() (p__IP->ISP_MCU_IO_SIZE6.ISP_MCU_IO_SIZE6_ts.REGION_SZ = REGION_SZ_B_0x4)
#define REGION_SZ_B_0x4 0x4    //4kB
#define Is__ISP_MCU_IO_SIZE6_REGION_SZ_B_0x5() (p__IP->ISP_MCU_IO_SIZE6.ISP_MCU_IO_SIZE6_ts.REGION_SZ == REGION_SZ_B_0x5)
#define Set__ISP_MCU_IO_SIZE6_REGION_SZ__B_0x5() (p__IP->ISP_MCU_IO_SIZE6.ISP_MCU_IO_SIZE6_ts.REGION_SZ = REGION_SZ_B_0x5)
#define REGION_SZ_B_0x5 0x5    //8kB
#define Is__ISP_MCU_IO_SIZE6_REGION_SZ_B_0x6() (p__IP->ISP_MCU_IO_SIZE6.ISP_MCU_IO_SIZE6_ts.REGION_SZ == REGION_SZ_B_0x6)
#define Set__ISP_MCU_IO_SIZE6_REGION_SZ__B_0x6() (p__IP->ISP_MCU_IO_SIZE6.ISP_MCU_IO_SIZE6_ts.REGION_SZ = REGION_SZ_B_0x6)
#define REGION_SZ_B_0x6 0x6    //16kB
#define Is__ISP_MCU_IO_SIZE6_REGION_SZ_B_0x7() (p__IP->ISP_MCU_IO_SIZE6.ISP_MCU_IO_SIZE6_ts.REGION_SZ == REGION_SZ_B_0x7)
#define Set__ISP_MCU_IO_SIZE6_REGION_SZ__B_0x7() (p__IP->ISP_MCU_IO_SIZE6.ISP_MCU_IO_SIZE6_ts.REGION_SZ = REGION_SZ_B_0x7)
#define REGION_SZ_B_0x7 0x7    //32kB
#define Set__ISP_MCU_IO_SIZE6(REGION_SZ) (p__IP->ISP_MCU_IO_SIZE6.word = (uint16_t)REGION_SZ<<0)
#define Get__ISP_MCU_IO_SIZE6() p__IP->ISP_MCU_IO_SIZE6.word
#define Set__ISP_MCU_IO_SIZE6_word(x) (p__IP->ISP_MCU_IO_SIZE6.word = x)


//ISP_MCU_IO_SIZE7


#define Get__ISP_MCU_IO_SIZE7_REGION_SZ() p__IP->ISP_MCU_IO_SIZE7.ISP_MCU_IO_SIZE7_ts.REGION_SZ
#define Is__ISP_MCU_IO_SIZE7_REGION_SZ_B_0x0() (p__IP->ISP_MCU_IO_SIZE7.ISP_MCU_IO_SIZE7_ts.REGION_SZ == REGION_SZ_B_0x0)
#define Set__ISP_MCU_IO_SIZE7_REGION_SZ__B_0x0() (p__IP->ISP_MCU_IO_SIZE7.ISP_MCU_IO_SIZE7_ts.REGION_SZ = REGION_SZ_B_0x0)
#define REGION_SZ_B_0x0 0x0    //16MB
#define Is__ISP_MCU_IO_SIZE7_REGION_SZ_B_0x1() (p__IP->ISP_MCU_IO_SIZE7.ISP_MCU_IO_SIZE7_ts.REGION_SZ == REGION_SZ_B_0x1)
#define Set__ISP_MCU_IO_SIZE7_REGION_SZ__B_0x1() (p__IP->ISP_MCU_IO_SIZE7.ISP_MCU_IO_SIZE7_ts.REGION_SZ = REGION_SZ_B_0x1)
#define REGION_SZ_B_0x1 0x1    //1MB
#define Is__ISP_MCU_IO_SIZE7_REGION_SZ_B_0x2() (p__IP->ISP_MCU_IO_SIZE7.ISP_MCU_IO_SIZE7_ts.REGION_SZ == REGION_SZ_B_0x2)
#define Set__ISP_MCU_IO_SIZE7_REGION_SZ__B_0x2() (p__IP->ISP_MCU_IO_SIZE7.ISP_MCU_IO_SIZE7_ts.REGION_SZ = REGION_SZ_B_0x2)
#define REGION_SZ_B_0x2 0x2    //64kB
#define Is__ISP_MCU_IO_SIZE7_REGION_SZ_B_0x3() (p__IP->ISP_MCU_IO_SIZE7.ISP_MCU_IO_SIZE7_ts.REGION_SZ == REGION_SZ_B_0x3)
#define Set__ISP_MCU_IO_SIZE7_REGION_SZ__B_0x3() (p__IP->ISP_MCU_IO_SIZE7.ISP_MCU_IO_SIZE7_ts.REGION_SZ = REGION_SZ_B_0x3)
#define REGION_SZ_B_0x3 0x3    //32MB
#define Is__ISP_MCU_IO_SIZE7_REGION_SZ_B_0x4() (p__IP->ISP_MCU_IO_SIZE7.ISP_MCU_IO_SIZE7_ts.REGION_SZ == REGION_SZ_B_0x4)
#define Set__ISP_MCU_IO_SIZE7_REGION_SZ__B_0x4() (p__IP->ISP_MCU_IO_SIZE7.ISP_MCU_IO_SIZE7_ts.REGION_SZ = REGION_SZ_B_0x4)
#define REGION_SZ_B_0x4 0x4    //4kB
#define Is__ISP_MCU_IO_SIZE7_REGION_SZ_B_0x5() (p__IP->ISP_MCU_IO_SIZE7.ISP_MCU_IO_SIZE7_ts.REGION_SZ == REGION_SZ_B_0x5)
#define Set__ISP_MCU_IO_SIZE7_REGION_SZ__B_0x5() (p__IP->ISP_MCU_IO_SIZE7.ISP_MCU_IO_SIZE7_ts.REGION_SZ = REGION_SZ_B_0x5)
#define REGION_SZ_B_0x5 0x5    //8kB
#define Is__ISP_MCU_IO_SIZE7_REGION_SZ_B_0x6() (p__IP->ISP_MCU_IO_SIZE7.ISP_MCU_IO_SIZE7_ts.REGION_SZ == REGION_SZ_B_0x6)
#define Set__ISP_MCU_IO_SIZE7_REGION_SZ__B_0x6() (p__IP->ISP_MCU_IO_SIZE7.ISP_MCU_IO_SIZE7_ts.REGION_SZ = REGION_SZ_B_0x6)
#define REGION_SZ_B_0x6 0x6    //16kB
#define Is__ISP_MCU_IO_SIZE7_REGION_SZ_B_0x7() (p__IP->ISP_MCU_IO_SIZE7.ISP_MCU_IO_SIZE7_ts.REGION_SZ == REGION_SZ_B_0x7)
#define Set__ISP_MCU_IO_SIZE7_REGION_SZ__B_0x7() (p__IP->ISP_MCU_IO_SIZE7.ISP_MCU_IO_SIZE7_ts.REGION_SZ = REGION_SZ_B_0x7)
#define REGION_SZ_B_0x7 0x7    //32kB
#define Set__ISP_MCU_IO_SIZE7(REGION_SZ) (p__IP->ISP_MCU_IO_SIZE7.word = (uint16_t)REGION_SZ<<0)
#define Get__ISP_MCU_IO_SIZE7() p__IP->ISP_MCU_IO_SIZE7.word
#define Set__ISP_MCU_IO_SIZE7_word(x) (p__IP->ISP_MCU_IO_SIZE7.word = x)

extern volatile _IP_ts *p__IP;

#endif 

