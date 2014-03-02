/*****************************************************************************/
/**
*  � ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
*  Private header file of MultiMedia Card Interface/ SD-card Interface module
* 
* author : ST-Ericsson
*/
/*****************************************************************************/




#ifndef _MMCP_H_
#define _MMCP_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------*/
#ifndef _HCL_DEFS_H
#include "hcl_defs.h"
#endif
#include "mmc.h"
#ifndef _HCL_MMC_IRQP_H_
#include "mmc_irqp.h"
#endif

/* PERIPHERAL IDENTIFICATION  */


#define MMC_PERIPHERAL_ID0  0x80
#define MMC_PERIPHERAL_ID1  0x01
#define MMC_PERIPHERAL_ID2  0x48
#define MMC_PERIPHERAL_ID3  0x00


/* P-CELL IDENTIFICATION  */
#define MMC_PCELL_ID0   0x0D
#define MMC_PCELL_ID1   0xF0
#define MMC_PCELL_ID2   0x05
#define MMC_PCELL_ID3   0xB1

/* INTERNAL PREPROCESSOR DEFINE FOR SDIO SUPPORT */

#define SDIO_SUPPORT


/**********************************************************
     COMMON DEFINES FOR BOTH LAYERS
***********************************************************/
/*****************************************************
    SHIFT_BITS
****************************************************/
#define MMC_SHIFT1          0x01
#define MMC_SHIFT2          0x02
#define MMC_SHIFT3          0x03
#define MMC_SHIFT4          0x04
#define MMC_SHIFT5          0x05
#define MMC_SHIFT6          0x06
#define MMC_SHIFT7          0x07
#define MMC_SHIFT8          0x08
#define MMC_SHIFT9          0x09
#define MMC_SHIFT10         0x0A
#define MMC_SHIFT11         0x0B
#define MMC_SHIFT12         0x0C
#define MMC_SHIFT13         0x0D
#define MMC_SHIFT14         0x0E
#define MMC_SHIFT15         0x0F
#define MMC_SHIFT16         0x10
#define MMC_SHIFT20         0x14
#define MMC_SHIFT22         0x16
#define MMC_SHIFT24         0x18
#define MMC_SHIFT26         0x1A
#define MMC_SHIFT27         0x1B
#define MMC_SHIFT28         0x1C
#define MMC_SHIFT29         0x1D
#define MMC_SHIFT30         0x1E
#define MMC_SHIFT31         0x1F

#define BIT31               0x80000000
#define PROCESSOR_CLK       200000000
#define MAXBSIZEPOWER       11
#if (defined ST_5500V2)
#define MAX_MMC_BLKSIZE     32767
#endif
#define CONTEXT_COUNT       8
#define MMC_MAXVOLTTRIAL    (0xFFFF)        /* (0x64)100 times*/




/* THERE IS ONE MORE INTERRUPT SOURCE START_BIT_ERROR */
#if (defined ST_5500V2)
#define MMCALLINTERRUPTS    (0x1FFFFFFF)
#else
#define MMCALLINTERRUPTS    (0x007FFFFF)
#endif
#define MMCCLRSTATICFLAGS   (0x004007FF)


/*USED IN POWER MANAGEMENT APIS AND ENHANCED LAYER APIs */
#define MMCCMDPATHENABLE    (0x00000400)
#define MMCDATAPATHENABLE   (0x00000001)

/*TO SET CLOCK FREQUENCY */
#define MMC_CLOCK_MASK_CLKDIV       0x000000FF
#define MMC_SET_CLKDIV(reg, val)    HCL_WRITE_BITS((reg), (val), MMC_CLOCK_MASK_CLKDIV)
/*USED IN COMMON APIs */


#define MMC_CLOCK_MASK_INVCLK       0x00008000
#define MMC_SET_INVCLK(reg, val)    HCL_WRITE_BITS((reg), (val), MMC_CLOCK_MASK_INVCLK)

#define MMC_CLOCK_MASK_NEGEDGE      0x00002000
#define MMC_SET_NEGEDGE(reg, val)    HCL_WRITE_BITS((reg), (val), MMC_CLOCK_MASK_NEGEDGE)

/* MuPoC Full has got dir enable signal and feedback clock */

#define MMC_POWER_MASK_DIREN    0x0000013C
#define MMC_POWER_MASK_FBCLK    0x00000080
#define MMC_SET_FBCLK(reg, val) HCL_WRITE_BITS((reg), ((t_uint32) (val) << MMC_SHIFT7), MMC_POWER_MASK_FBCLK)

#define EN_TRIM     0x1
#define EN_REL_WR   0x80000000
/*  DEFINES FOR ELEMEMTARY LAYER*/
#if defined(__MMCI_HCL_ELEMENTARY)

/* DEFINES FOR READING OR WRITING A REGISTER */
/*COMMON DEFINES TO ALL PLATFORMS */
/* MMC_POWER REGISTER */
#define MMC_POWER_MASK_CTRL     0x00000003
#define MMC_POWER_MASK_OPEND    0x00000040
#define MMC_POWER_MASK_RESET    0x00000400

/* MMC_CLOCK REGISTER */
#define MMC_CLOCK_MASK_CLKEN    0x00000100
#define MMC_CLOCK_MASK_PWRSAVE  0x00000200
#define MMC_CLOCK_MASK_BYPASS   0x00000400
#define MMC_CLOCK_MASK_HSENABLE 0x00002000
#define MMC_CLOCK_MASK_HWFC     0x00004000

/* MMC_DATACTRL REGISTER */
#define MMC_DATAPATH_MASK_ENABLE    0x00000001
#define MMC_DATAPATH_MASK_DIRECTION 0x00000002
#define MMC_DATAPATH_MASK_MODE      0x00000004
#define MMC_DATAPATH_MASK_DMA       0x00000008
#if (!(defined ST_5500V2))
#define MMC_DATAPATH_MASK_BLOCKSIZE 0x000000F0
#else
#define MMC_DATAPATH_MASK_BLOCKSIZE 0x7FFF0000
#endif
#define MMC_DATACNTRL_MASK_BOOTMODE 0x00002000
#define MMC_DATACNTRL_MASK_BUSYMODE 0x00004000
#define MMC_DATACNTRL_MASK_DDRMODE  0x00008000

/* SDIO support */


/* MMC_DATACTRL REGISTER */
#define MMC_DATAPATH_MASK_SDIO_ENABLE   0x00000800
#define MMC_DATAPATH_MASK_RWMODE        0x00000400
#define MMC_DATAPATH_MASK_RWSTOP        0x00000200
#define MMC_DATAPATH_MASK_RWSTART       0x00000100

/* MMC_CMD REGISTER */
#define MMC_CMD_MASK_SDIO_SUSPEND   0x00000800


/*COMMON MACROS */
#define MMC_SET_CTRL(reg, val)      HCL_WRITE_BITS((reg), (val), MMC_POWER_MASK_CTRL)
#define MMC_SET_OPEND(reg, val)     HCL_WRITE_BITS((reg), ((t_uint32) (val) << MMC_SHIFT6), MMC_POWER_MASK_OPEND)
#define MMC_SET_CENABLE(reg, val)   HCL_WRITE_BITS((reg), ((t_uint32) (val) << MMC_SHIFT8), MMC_CLOCK_MASK_CLKEN)
#define MMC_SET_PWRSAVE(reg, val)   HCL_WRITE_BITS((reg), ((t_uint32) (val) << MMC_SHIFT9), MMC_CLOCK_MASK_PWRSAVE)
#define MMC_SET_HSENABLE(reg, val)  HCL_WRITE_BITS((reg), ((t_uint32) (val) << MMC_SHIFT13), MMC_CLOCK_MASK_HSENABLE)
#define MMC_SET_BYPASS(reg, val)    HCL_WRITE_BITS((reg), ((t_uint32) (val) << MMC_SHIFT10), MMC_CLOCK_MASK_BYPASS)
#define MMC_SET_DATAPATH(reg, val)  HCL_WRITE_BITS((reg), (val), MMC_DATAPATH_MASK_ENABLE)
#define MMC_SET_DATADIR(reg, val)   HCL_WRITE_BITS((reg), ((t_uint32) (val) << MMC_SHIFT1), MMC_DATAPATH_MASK_DIRECTION)
#define MMC_SET_MODE(reg, val)      HCL_WRITE_BITS((reg), ((t_uint32) (val) << MMC_SHIFT2), MMC_DATAPATH_MASK_MODE)
#define MMC_SET_DMA(reg, val)       HCL_WRITE_BITS((reg), ((t_uint32) (val) << MMC_SHIFT3), MMC_DATAPATH_MASK_DMA)

#if (defined ST_5500V2)
/* Block_size bits is shifted to bits[31:16] in MOP8500 V2.0 */
#define MMC_SET_BLOCKSIZE(reg, val) HCL_WRITE_BITS((reg), ((t_uint32) (val) << MMC_SHIFT16), MMC_DATAPATH_MASK_BLOCKSIZE)
#else
#define MMC_SET_BLOCKSIZE(reg, val) HCL_WRITE_BITS((reg), ((t_uint32) (val) << MMC_SHIFT4), MMC_DATAPATH_MASK_BLOCKSIZE)
#endif

#define MMC_SET_BOOTMODE(reg,val)   HCL_WRITE_BITS((reg), ((t_uint32) (val) << MMC_SHIFT13),MMC_DATACNTRL_MASK_BOOTMODE)
#define MMC_SET_BUSYMODE(reg,val)   HCL_WRITE_BITS((reg), ((t_uint32) (val) << MMC_SHIFT14),MMC_DATACNTRL_MASK_BUSYMODE)
#define MMC_EN_RESET(reg, val)      HCL_WRITE_BITS((reg), ((t_uint32) (val) << MMC_SHIFT9), MMC_POWER_MASK_RESET)
#define MMC_SET_DDRMODE(reg,val)    HCL_WRITE_BITS((reg), ((t_uint32) (val) << MMC_SHIFT15),MMC_DATACNTRL_MASK_DDRMODE)


#define MMC_CLOCK_MASK_WIDEBUS  0x00001800

#define MMC_SET_WIDEBUS(reg, val)   HCL_WRITE_BITS((reg), ((t_uint32) (val) << MMC_SHIFT11), MMC_CLOCK_MASK_WIDEBUS)



/* Defines for SDIO support */
#define MMC_START_READ_WAIT(reg, val)   HCL_SET_BITS(reg, val)
#define MMC_STOP_READ_WAIT(reg, val)    HCL_SET_BITS(reg, val)

#endif /*__MMCI_HCL_ELEMENTARY*/

/* Functional layer */
#ifdef __MMCI_HCL_ENHANCED
#define R1_OUT_OF_RANGE         0x80000000
#define R1_ADDRESS_ERROR        0x40000000
#define R1_BLOCK_LEN_ERROR      0x20000000
#define R1_ERASE_SEQ_ERROR      0x10000000
#define R1_ERASE_PARAM          0x08000000
#define R1_WP_VIOLATION         0x04000000
#define R1_CARD_IS_LOCKED       0x02000000
#define R1_LOCK_UNLOCK_FAILED   0x01000000
#define R1_COM_CRC_ERROR        0x00800000
#define R1_ILLEGAL_COMMAND      0x00400000
#define R1_CARD_ECC_FAILED      0x00200000
#define R1_CC_ERROR             0x00100000
#define R1_ERROR                0x00080000
#define R1_UNDERRUN             0x00040000
#define R1_OVERRUN              0x00020000
#define R1_CID_CSD_OVERWRITE    0x00010000
#define R1_WP_ERASE_SKIP        0x00008000
#define R1_CARD_ECC_DISABLED    0x00004000
#define R1_ERASE_RESET          0x00002000
#define R1_CURRENT_STATE(x)     ((x & 0x00001E00) >> 9) /* sx, b (4 bits) */
#define R1_READY_FOR_DATA       0x000000100
#define R1_APP_CMD              0x000000020

#define MMC_ALLONE              (0xFFFFFFFF)
#define MMC_ALLZERO             (0x00000000)


/* Power Control register */
#define MMC_POWERON     (0x00000003)
#define MMC_OPENDRAIN   (0x00000040)
#define MMC_CMDDATEN            0x0000003C              /* 0x0000003C */
#define MMC_VOLTAGEWINDOWMMC    (0x80FF8000)
#define MMC_VOLTAGEWINDOWSD     (0x80010000)
#define MMC_STUFFBITS0TO32      (0xFFFFFFFF)
#define MMC_STUFFBITS0TO15      (0xFFFF)
#define MMC_SECTOR_MODE         (0x40000000)
#define MMC_BYTE_MODE           (0x00000000)
#define MMC_CHECK_PATTERN       (0x000001AA)

/* Clock Control register */

#define MMC_CLKDIVINIT  0xFF                            /*(0x0000009E)*/
//#define MMC_CLKDIVINIT  0x76  -> 8500 value   XVSZOAN check this!
#define MMC_CLKDIVINIT_NEW  0x102

/* SDIo Support */
#if defined(SDIO_SUPPORT)
#define MMC_SDIOCLKDIV  (0x00000078)
#endif

#define MMC_CLKDIVTRANS (0x00000001)
#define MMC_CLKENABLE   (0x00000100)
#define MMC_PWRSAVE     (0x00000200)
#define MMC_BYPASS      (0x00000400)
#define MMC_HWFC_EN     (0x00004000)
#define MMC_NEGEDGE     (0x00002000)
#define MMC_INVCLKOUT   (0x00008000)
/* Command register */
/* all commands */
#define MMCGO_IDLE_STATE    (0)
#define MMCSEND_OP_COND     (1)
#define MMCALL_SEND_CID     (2)
#define MMCSET_REL_ADDR     (3)
#define MMCSET_DSR          (4)

#define MMCIO_SEND_OP_COND  (5)

#define MMCHSSWITCH             (6)
#define MMCSEL_DESEL_CARD       (7)
#define MMCHSSENDEXTCSD         (8)
#define MMCSEND_CSD             (9)
#define MMCSEND_CID             (10)
#define MMCREAD_DAT_UNTIL_STOP  (11)
#define MMCSTOP_TRANSMISSION    (12)
#define MMCSEND_STATUS          (13)
#define MMCHS_BUSTEST_READ      (14)
#define MMCGO_INACTIVE_STATE    (15)
#define MMCSET_BLOCKLEN         (16)
#define MMCREAD_SINGLE_BLOCK    (17)
#define MMCREAD_MULT_BLOCK      (18)
#define MMCHS_BUSTEST_WRITE     (19)
#define MMCWRITE_DAT_UNTIL_STOP (20)
#define MMCSET_BLOCK_COUNT      (23)
#define MMCWRITE_SINGLE_BLOCK   (24)
#define MMCWRITE_MULT_BLOCK     (25)
#define MMCPROG_CID             (26)
#define MMCPROG_CSD             (27)
#define MMCSET_WRITE_PROT       (28)
#define MMCCLR_WRITE_PROT       (29)
#define MMCSEND_WRITE_PROT      (30)
#define MMCSD_ERASE_GRP_START   (32)
#define MMCSD_ERASE_GRP_END     (33)
#define MMCERASE_GRP_START      (35)
#define MMCERASE_GRP_END        (36)
#define MMCERASE                (38)
#define MMCFAST_IO              (39)
#define MMCGO_IRQ_STATE         (40)
#define MMCLOCK_UNLOCK          (42)

#define MMCIO_RW_DIRECT     (52)
#define MMCIO_RW_EXTENDED   (53)

#define MMCAPP_CMD  (55)
#define MMCGEN_CMD  (56)

/*Following commands are SD Card Specific commands. MMC_APP_CMD should be sent before sending these commands.*/
#define MMCAPP_SD_SET_BUSWIDTH          (6)
#define MMCSD_APP_STAUS                 (13)
#define MMCSD_APP_SEND_NUM_WRITE_BLOCKS (22)
#define MMCSD_APP_OP_COND               (41)
#define MMCSD_APP_SET_CLR_CARD_DETECT   (42)
#define MMCSD_APP_SEND_SCR              (51)
#define MMCSD_SEND_IF_COND              (8)

/* Command control register */
#define MMCRESPEXPECTED     (0x00000040)
#define MMCLONGRESPONSE     (0x00000080)
#define MMCENABLEINTRREQ    (0x00000100)
#define MMCENABLECMDPEND    (0x00000200)
#define MMCSDIOSUSPEND      (0x00000800)

/* Data Control register */
#define MMCREADDIR              (0x00000002)
#define MMCSTREAMMODE           (0x00000004)
#define MMCDMAENABLE            (0x00000008)
#define MMCBLOCKSIZE            (0x000000F0)
#define MMCSDIOENABLE           (0x00000800)
#define MMC_SDIO_BLOCK_POWER    (9)
#define MMC_SDIO_BLOCK_SIZE     (512)

/* Status register */
#define MMCCMDCRCFAIL   (0x00000001)
#define MMCDATACRCFAIL  (0x00000002)
#define MMCCMDTIMEOUT   (0x00000004)
#define MMCDATATIMEOUT  (0x00000008)
#define MMCTXUNDERRUN   (0x00000010)
#define MMCRXOVERRUN    (0x00000020)
#define MMCCMDRESPEND   (0x00000040)
#define MMCCMDSENT      (0x00000080)
#define MMCDATAEND      (0x00000100)

#define MMCSTARTBITERROR    (0x00000200)


#define MMCDATABLOCKEND     (0x00000400)
#define MMCCMDACTIVE        (0x00000800)
#define MMCTXACTIVE         (0x00001000)
#define MMCRXACTIVE         (0x00002000)
#define MMCTXFIFOHALFEMPTY  (0x00004000)
#define MMCRXFIFOHALFFULL   (0x00008000)
#define MMCTXFIFOFULL       (0x00010000)
#define MMCRXFIFOFULL       (0x00020000)
#define MMCTXFIFOEMPTY      (0x00040000)
#define MMCRXFIFOEMPTY      (0x00080000)
#define MMCTXDATAAVLBL      (0x00100000)
#define MMCRXDATAAVLBL      (0x00200000)
#define MMCSDIOIT           (0x00400000)

/*MASK FOR ERRORS */
#define MMCADDR_OUT_OF_RANGE        (0x80000000)
#define MMCADDR_MISALIGNED          (0x40000000)
#define MMCBLOCK_LEN_ERR            (0x20000000)
#define MMCERASE_SEQ_ERR            (0x10000000)
#define MMCBAD_ERASE_PARAM          (0x08000000)
#define MMCWRITE_PROT_VIOLATION     (0x04000000)
#define MMCLOCK_UNLOCK_FAILED       (0x01000000)
#define MMCCOM_CRC_FAILED           (0x00800000)
#define MMCILLEGAL_CMD              (0x00400000)
#define MMCCARD_ECC_FAILED          (0x00200000)
#define MMCCC_ERROR                 (0x00100000)
#define MMCGENERAL_UNKNOWN_ERROR    (0x00080000)
#define MMCSTREAM_READ_UNDERRUN     (0x00040000)
#define MMCSTREAM_WRITE_OVERRUN     (0x00020000)
#define MMCCID_CSD_OVERWRIETE       (0x00010000)
#define MMCWP_ERASE_SKIP            (0x00008000)
#define MMCCARD_ECC_DISABLED        (0x00004000)
#define MMCERASE_RESET              (0x00002000)
#define MMCAKE_SEQ_ERROR            (0x00000008)
#define MMCERRORBITS                (0xFDFFE008)

    /*SDIO Related errors */
#define MMCSDIO_OUT_OF_RANGE    (0x00000100)
#define MMCSDIO_FUNCTION_NUMBER (0x00000200)
#define MMCSDIO_UNKNOWN_ERROR   (0x00000800)
#define MMCSDIO_CURRENT_STATE   (0x00003000)
#define MMCSDIO_ILLEGAL_COMM    (0x00004000)
#define MMCSDIO_COM_CRC_ERR     (0x00008000)


/* DEFINES FOR R6 RESPONSE */
#define MMCR6GENERAL_UNKNOWN_ERROR  (0x00002000)
#define MMCR6ILLEGAL_CMD            (0x00004000)
#define MMCR6COM_CRC_FAILED         (0x00008000)


/* SDIO OPERATING VOLATGE */
#if defined(SDIO_SUPPORT)                               /*OPERATING VOLTAGE 2.7V FOR MEK OTHERWISE 3.6V*/
#define MMCSDIO_OCR_VALUE   (0x00FF8000)
#endif

/* DEFINES FOR R4 RESPONSE */
#define MMCR4CARD_READY_AFTER_INIT      (0x80000000)
#define MMCR4NO_OF_IO_FUNCS             (0x70000000)
#define MMCR4MEMORY_PRESENT             (0x08000000)
#define MMCR4OP_COND_REG                (0x00FFFFFF)
#define MMCR4CARD_READY_AFTER_INIT_sb   (31)
#define MMCR4NO_OF_IO_FUNCS_sb          (28)
#define MMCR4MEMORY_PRESENT_sb          (27)

/* SDIO RW COMMAND DEFINES */
#define MMCSDIO_ADDR_RANGE  (0x1FFFF)


/* MMC Wide bus settings */
#define MMC_CLOCK_WIDEBUS_MASK  (0x00000800)


#define MMC_WIDEBUSSUPPORT      (0x00040000)
#define MMC_SINGLEBUSSUPPORT    (0x00010000)
#define MMC_CARDLOCKED          (0x02000000)

#define MMC_MAXDATALENGTH   (0x01FFFFFF)
#define MMC_MAXBYTES        (0x01FFFFFC)                /*MAXIMUM NO OF BYTES CAN BE TRANSFFERED IS (0x1FFFFFF/4)*4 */

#define MMC_DATATIMEOUT     (0x0FFFFFFF)
#define MMC_HALFFIFO        (0x08)                      /*8 WORDS */
#define MMC_HALFFIFOBYTES   (0x20)                      /*32 BYTES */
#define MMC_CARDPROGRAMMING (0x7)
#define MMC_CARDRECEIVING   (0x6)
#define MMC_MAXBLSIZE       (2048)
#define MMC_PERMWP          (0x2000)                    /* bit13 of csd register */
#define MMC_TEMPWP          (0x1000)                    /* bit12 of csd register */
#define MMC_MINPASSLENG     (1)
#define MMC_MAXPASSLENG     (14)
#define MMC_0TO7BITS        (0x000000FF)
#define MMC_8TO15BITS       (0x0000FF00)
#define MMC_16TO23BITS      (0x00FF0000)
#define MMC_24TO31BITS      (0xFF000000)
#define MMC_CMD0TIMEOUT     (10000)
#define MMC_SDIOCMDTIMEOUT  (120000000)
#define MMC_VOLATAGEBIT1    (0xAAAA)
#define MMC_VOLATAGEBIT2    (0xCCCC)
#define MMC_VOLATAGEBIT3    (0xF0F0)
#define MMC_VOLATAGEBIT4    (0xFF00)
#define MMC_WORDDIV         (0x4)
#define MMC_BYTEINC         (0x4)
#define MMC_PWLENGTH        (0x4)                       /*MAX PASSWORD LENGTH +2 (WORDS) */
#define MMC_PWLEN2          (0x2)
#define MMC_PWLEN6          (0x6)
#define MMC_PWLEN14         (14)
#define MMC_PWEXTRABYTES    (0x2)
#define MMC_SPECVERSION     (0x3C000000)
#define MMC_SPECHIGHSPEED   (0x4)

/* COMMAND CLASS SUPPORTED */
#define MMC_CCCC_LOCK_UNLOCK    (0x80)
#define MMC_CCCC_WRITE_PROT     (0x40)
#define MMC_CCCC_ERASE          (0x20)

/* ENHANCED CONFIGURATION */

#define MMC_ERASE_GROUPDEF  (0x03AF0100)
#define MMC_PARTITION_ATT   (0x039C0100)
#define MMC_ENH_STARTADDR0  (0x03880000)
#define MMC_ENH_STARTADDR1  (0x03890000)
#define MMC_ENH_STARTADDR2  (0x038A0000)
#define MMC_ENH_STARTADDR3  (0x038B0000)
#define MMC_ENH_SIZE0       (0x038C0000)
#define MMC_ENH_SIZE1       (0x038D0000)
#define MMC_ENH_SIZE2       (0x038E0000)
#define MMC_PARTTITON_COMPLETED  (0x039B0100)

/*  STRUCTURE FOR PRIVATE USE */
typedef struct {
    t_uint32    num_wp_grp;
    t_uint32    erase_group_mult;
    t_uint32    wp_grp_size;
    t_uint32    sector_size;
    t_uint32    blk_len_factor;
    t_uint32    erase_group_size;
}t_local_temp_protect;

/* MMC Context structure */
typedef struct
{
    t_mmc_device_mode       device_mode;        /* DMA,INTERRUUPT, POLLING*/
    t_local_temp_protect    local_temp_protect;
    t_mmc_error             transfer_error;
    t_uint32                total_no_of_bytes;  /*TOTAL NO OF BYTES TO TRANSFER*/
    t_uint32                *source_buffer;
    t_uint32                *dest_buffer;
    t_mmc_event             mmc_event;          /*FOR EVENT MANAGEMENT */
    t_mmc_address_mode      address_mode;       /*BYTE Addressable , SECTOR addressable*/
} t_mmc_context;

/* CARD structure */
typedef struct
{
    t_mmc_context       mmc_context;            /*CONTAINING OTHER GLOBAL VARIABLES */
    t_uint32            blk_length;             /*BLOCK LENGTH OF SELECTED CARD */
    t_mmc_state         wide_mode;
    t_mmc_card_type     cardtype;               /*CURRENT CARD TYPE */
    t_bool              stopcmd_needed;         /*WHETHER THERE IS NEED TO SEND STOP CMD*/
    t_uint8             selected_card;          /*SELECTED CARD NO*/
    t_uint8             sdio_function;          /*CURRENT ACTIVE SDIO FUNCTION*/
    t_mmc_sdio_resp4    sdio_info;              /*SDIO card information */
} t_mmc_current_card;

/* Used for debugging purposes */
PRIVATE t_mmc_error mmc_CmdError(t_mmc_device_id);
/*Receive the response for CMD6*/
PRIVATE t_mmc_error mmc_CmdResp6Error(t_uint8, t_uint16 *,t_mmc_device_id);

/*Receive the response for CMD4*/
PRIVATE t_mmc_error mmc_CmdResp4Error(IN t_uint8, OUT t_mmc_sdio_resp4 *,t_mmc_device_id);

/*Receive the response for CMD5*/
PRIVATE t_mmc_error mmc_CmdResp5Error(IN t_uint8, OUT t_uint8 *,t_mmc_device_id);

/*Receive the response for CMD7*/
PRIVATE t_mmc_error mmc_CmdResp7Error(t_mmc_device_id);

/*Receive the response for CMD2*/
PRIVATE t_mmc_error mmc_CmdResp2Error(t_mmc_device_id);

/*Receive the response for CMD3*/
PRIVATE t_mmc_error mmc_CmdResp3Error(t_mmc_device_id);

/*Program CSD register*/
PRIVATE t_mmc_error mmc_ProgramCSD(t_uint32 card, t_uint32 bitfield, t_bool status,t_mmc_device_id);

/*Find block length for the slave device*/
#if (!(defined ST_5500V2))
PRIVATE t_mmc_error mmc_FindBlocklen(t_uint16 nobytes, t_uint8 *power);
PRIVATE t_uint8     mmc_convert_from_bytes_to_power_of_two(t_uint16 no_of_bytes);
#endif

/* Check the status of the slave device */
PRIVATE t_mmc_error mmc_IsCardProgramming(t_uint32, t_uint8 *,t_mmc_device_id);
PRIVATE void        mmc_ProcessIt(t_mmc_irq_status *,t_mmc_device_id );
PRIVATE t_mmc_error mmc_CmdResp1Error(t_uint8 cmd,t_mmc_device_id);
PRIVATE t_mmc_error mmc_SwitchResp1Error(IN t_uint8 cmd, IN t_mmc_device_id mmc_device_id);
PRIVATE void        mmc_ResetGV(t_mmc_device_id);
PRIVATE t_mmc_error mmc_FINDSCR(t_uint16, t_uint32 *,t_mmc_device_id);

/*Enable wide bus for SD card*/
PRIVATE t_mmc_error mmc_SDEnWideBus(t_uint8, t_mmc_state,t_mmc_device_id);
PRIVATE t_mmc_error mmc_HSEnWideBus(t_uint8, t_mmc_wide_mode,t_mmc_device_id);
#if defined SDIO_SUPPORT
PRIVATE t_mmc_error mmc_SDIOEnWideBus(t_uint8, t_mmc_wide_mode,t_mmc_device_id);
#endif
PRIVATE t_mmc_error mmc_TestFunctionalPins(t_uint8, t_mmc_wide_mode,t_mmc_device_id);
//PRIVATE t_mmc_error mmc_TempProtect(t_uint8, t_bool, t_mmc_device_id);

PRIVATE t_mmc_error mmc_PowerON_SDIO_COMBO(IN t_mmc_device_id mmc_device_id);
/* API to power on SDIO card */
PRIVATE t_mmc_error mmc_PowerON_SDIO(IN t_mmc_device_id mmc_device_id);

/* API to power on SD card */
PRIVATE t_mmc_error mmc_PowerON_SD(IN t_mmc_device_id mmc_device_id,t_uint32 address_mode);

/* API to power on MMC card */
PRIVATE t_mmc_error mmc_PowerON_MMC(IN t_mmc_device_id mmc_device_id);
PRIVATE t_mmc_error mmc_PowerON_MMC(IN t_mmc_device_id mmc_device_id);

PRIVATE t_mmc_error mmc_initializeSDIOcard(t_uint16 index, IN t_mmc_device_id mmc_device_id);

/* API for Setting Wide bus operation for SD card*/
PRIVATE t_mmc_error mmc_SDEnableWideBusOperation(IN t_uint8 cardno,IN t_mmc_wide_mode  wide_mode,IN t_mmc_device_id  mmc_device_id);
PRIVATE t_mmc_error mmc_ReadBytes_openend(IN t_uint32 addr,OUT t_uint32 *p_readbuff,IN t_uint32 no_of_bytes,IN t_mmc_device_id  mmc_device_id);

/*API for Read operation  by byte transfer mode */
PRIVATE t_mmc_error mmc_ReadBytes(IN t_uint8 cardno,IN t_uint32 addr,OUT t_uint32 *p_readbuff,IN t_uint32 no_of_bytes,IN t_mmc_device_id  mmc_device_id);
PRIVATE t_mmc_error mmc_WriteBytes_poll_mode(IN t_uint8 cardno,IN t_uint32 addr,IN t_uint32 *p_writebuff,IN t_uint32 no_of_bytes,IN t_mmc_device_id  mmc_device_id);

PRIVATE t_mmc_error mmc_WriteBytes_mode_sel
(
    IN t_uint8          cardno,
    IN t_uint32         addr,
    IN t_uint32         *p_writebuff,
    IN t_uint32         no_of_bytes,
    IN t_mmc_device_id  mmc_device_id
);

/* API for checking the error while write operation */
PRIVATE t_mmc_error mmc_WriteBytes_err_chk
(
    IN t_uint8          cardno,
    IN t_uint32         *p_writebuff,
    IN t_mmc_device_id  mmc_device_id
);


/* Read Block mode handler */
PRIVATE t_mmc_error mmc_ReadBlock_mode_handler
(
    IN t_uint8          cardno,
    IN t_uint32         addr,
    OUT t_uint32        *p_readbuff,
    IN t_uint16         blocksize,
    IN t_uint32         no_of_blocks,
    IN t_mmc_device_id  mmc_device_id
);

/* API for data Xfer in read mode by block mode */
PRIVATE t_mmc_error mmc_ReadBlocks
(
    IN t_uint8          cardno,
    IN t_uint32         addr,
    OUT t_uint32        *p_readbuff,
    IN t_uint16         blocksize,
    IN t_uint32         no_of_blocks,
    IN t_mmc_device_id  mmc_device_id
);

/* Read Block openend */
PRIVATE t_mmc_error mmc_ReadBlocks_openend
(
    IN t_uint8          cardno,
    IN t_uint32         addr,
    OUT t_uint32        *p_readbuff,
    IN t_uint16         blocksize,
    IN t_uint32         no_of_blocks,
    IN t_mmc_device_id  mmc_device_id
);

/* Write Block openend */
PRIVATE t_mmc_error mmc_WriteBlocks_openend
(
    IN t_uint8          cardno,
    IN t_uint32         addr,
    IN t_uint32         *p_writebuff,
    IN t_uint16         blocksize,
    IN t_uint32         no_of_blocks,
    IN t_mmc_device_id  mmc_device_id
);
PRIVATE t_mmc_error mmc_WriteBlock_poll_mode_err_chk(IN t_mmc_device_id mmc_device_id);

/* MMC WriteBlock Polling mode */
PRIVATE t_mmc_error mmc_WriteBlock_poll_mode
(
    IN t_uint8          cardno,
    IN t_uint32         addr,
    IN t_uint32         *p_writebuff,
    IN t_uint16         blocksize,
    IN t_uint32         no_of_blocks,
    IN t_mmc_device_id  mmc_device_id
);

/*API to write single block on slave device */
PRIVATE t_mmc_error mmc_WriteSingleBlock
(
    IN t_uint8          cardno,
    IN t_uint32         addr,
    IN t_uint32         *p_writebuff,
    IN t_uint16         blocksize,
    IN t_uint32         no_of_blocks,
    IN t_mmc_device_id  mmc_device_id
);

/* API to write multiple blocks on slave device */
PRIVATE t_mmc_error mmc_WriteMultiBlocks
(
    IN t_uint8          cardno,
    IN t_uint32         addr,
    IN t_uint32         *p_writebuff,
    IN t_uint16         blocksize,
    IN t_uint32         no_of_blocks,
    IN t_mmc_device_id  mmc_device_id
);

/* API for write operation setting */
PRIVATE t_mmc_error mmc_WriteBlock_setting
(
    IN t_uint8          cardno,
    IN t_uint32         addr,
    IN t_uint32         *p_writebuff,
    IN t_uint16         blocksize,
    IN t_uint32         no_of_blocks,
    IN t_mmc_device_id  mmc_device_id
);

/* SDIO Read Write Error check */
PRIVATE t_mmc_error mmc_SDIOReadWriteData_errorCHK
(
    IN t_uint8                          cardno,
    IN t_mmc_sdio_transfer_info const   *p_transfer_param,
    INOUT t_uint32                      *p_readwritebuff,
    IN t_mmc_device_id                  mmc_device_id
);


PRIVATE void mmc_SDIOReadWriteData_blkpollMODE_Read(INOUT t_uint32 *p_readwritebuff, IN t_mmc_device_id mmc_device_id);
PRIVATE void mmc_SDIOReadWriteData_blkpollMODE_Write(INOUT t_uint32 *p_readwritebuff, IN t_mmc_device_id mmc_device_id);

/* SDIO Read write polling mode */
PRIVATE t_mmc_error mmc_SDIOReadWriteData_blkpollMODE
(
    IN t_mmc_sdio_transfer_info const   *p_transfer_param,
    INOUT t_uint32                      *p_readwritebuff,
    IN t_mmc_device_id                  mmc_device_id
);
PRIVATE t_mmc_error mmc_SDIOReadWriteData_blkMODE_OpenXfer
(
    IN t_mmc_sdio_transfer_info const   *p_transfer_param,
    INOUT t_uint32                      *p_readwritebuff,
    IN t_mmc_device_id                  mmc_device_id
);

/* SDIO Read write interrupt mode */
PRIVATE void mmc_SDIOReadWriteData_blkINTMODE
(
    IN t_mmc_sdio_transfer_info const   *p_transfer_param,
    IN t_mmc_device_id                  mmc_device_id
);

/* SDIO Read write DMA mode */
PRIVATE void mmc_SDIOReadWriteData_blkDMAMODE
(
    IN t_mmc_sdio_transfer_info const   *p_transfer_param,
    IN t_mmc_device_id                  mmc_device_id
);

/* API for read / write operation with Block transfer in polling mode */
PRIVATE t_mmc_error mmc_SDIOReadWriteData_blkMode
(
    IN t_uint8                          cardno,
    IN t_mmc_sdio_transfer_info const   *p_transfer_param,
    INOUT t_uint32                      *p_readwritebuff,
    IN t_mmc_device_id                  mmc_device_id
);


/* API for read / write operation with byte transfer in polling mode */
PRIVATE t_mmc_error mmc_SDIOReadWriteData_bytePoll_Mode
(
    IN t_mmc_sdio_transfer_info const   *p_transfer_param,
    INOUT t_uint32                      *p_readwritebuff,
    IN t_mmc_device_id                  mmc_device_id
);

/* API for read / write operation with byte transfer in Interrupt mode */
PRIVATE void mmc_SDIOReadWriteData_byteINT_Mode
(
    IN t_mmc_sdio_transfer_info const   *p_transfer_param,
    IN t_mmc_device_id                  mmc_device_id
);

/* API for read / write operation with byte transfer in DMA mode */
PRIVATE void mmc_SDIOReadWriteData_byteDMA_Mode
(
    IN t_mmc_sdio_transfer_info const   *p_transfer_param,
    IN t_mmc_device_id                  mmc_device_id
);

/* SDIO Read write byte mode */
PRIVATE t_mmc_error mmc_SDIOReadWriteData_byteMode
(
    IN t_mmc_sdio_transfer_info const   *p_transfer_param,
    INOUT t_uint32                      *p_readwritebuff,
    IN t_mmc_device_id                  mmc_device_id
);

/*API to Erase data on slave device */
PRIVATE t_mmc_error mmc_Erase_set(IN t_uint32 startaddr, IN t_uint32 endaddr, IN t_mmc_device_id mmc_device_id);
PRIVATE t_mmc_error mmc_sdErase_set(IN t_uint32 startaddr, IN t_uint32 endaddr, IN t_mmc_device_id mmc_device_id);
PRIVATE t_mmc_error mmc_erase_errorChk(IN t_uint8 cardno, IN t_mmc_device_id mmc_device_id);


/* Card Lock-Unlock settings */
#if defined(__MMC_SD_SOFTWARE_WORKAROUND)
PRIVATE t_mmc_error mmc_TempProtect_SetReset
(
    IN t_uint8          cardno,
    IN t_uint32         addr,
    IN t_uint32         cmd,
    IN t_mmc_device_id  mmc_device_id
);

PRIVATE t_mmc_error mmc_TempProtect(IN t_uint8 cardno, IN t_bool status, IN t_mmc_device_id mmc_device_id);
#endif
PRIVATE t_mmc_error mmc_Lock_UnlockCard_error_chk
(
    IN t_uint8          cardno,
    IN t_uint8          lock_set_chk,
    IN t_mmc_device_id  mmc_device_id
);

/* CARD Password setting */
PRIVATE t_mmc_error mmc_SetClearPassword_init_setting
(
    IN t_uint8          cardno,
    IN t_uint8          passleng,
    IN t_uint8          *p_password,
    IN t_mmc_device_id  mmc_device_id
);
PRIVATE t_mmc_error mmc_Lock_UnlockCard_setting
(
    IN t_uint8          cardno,
    IN t_uint8          passleng,
    IN t_uint8          *p_password,
    IN t_mmc_device_id  mmc_device_id
);
PRIVATE void mmc_ProcessIRQSrc_TX_FIFO_HALF_EMPTY(IN t_mmc_device_id mmc_device_id);
PRIVATE void mmc_FilterProcessIRQSrc_sel(t_uint32 *temp_filter, t_mmc_irq_status *temp_status);
PRIVATE t_mmc_error mmc_SendSDStatus_err_chk(IN t_mmc_device_id mmc_device_id);
PRIVATE t_mmc_error mmc_FindBlocklen(IN t_uint16 nobytes, OUT t_uint8 *p_power);
PRIVATE t_mmc_error mmc_CmdResp1Error_sel(t_uint32 response_r1);
PRIVATE t_mmc_error MMC_GetSCR_errorChk(IN t_mmc_device_id mmc_device_id);
/*PRIVATE t_mmc_error mmc_GetExtCSD(IN t_uint8 cardno, OUT t_uint32 *p_extcsd, IN t_mmc_device_id mmc_device_id);*/
/*PRIVATE t_mmc_error mmc_SetBOOTMode_CMD_SDI(t_mmc_device_id mmc_device_id);*/
#if 0
PRIVATE t_mmc_error mmc_SetdeviceBootMode
(
    IN t_uint8          cardno,
    IN t_mmc_state      bootmode,
    IN t_mmc_device_id  mmc_device_id
);
#endif
/* Enable wide bus 1-bit*/
PRIVATE t_mmc_error mmc_SDEnableWideBus_onebit
(
    IN t_uint8          cardno,
    IN t_mmc_state      widen,
    IN t_mmc_device_id  mmc_device_id
);

/* Enable wide bus Multi-bit*/
PRIVATE t_mmc_error mmc_SDEnableWidebus_multibit
(
    IN t_uint8          cardno,
    IN t_mmc_state      widen,
    IN t_mmc_device_id  mmc_device_id
);
PRIVATE t_mmc_error mmc_SDEnWideBus(IN t_uint8 cardno, IN t_mmc_state widen, IN t_mmc_device_id mmc_device_id);
PRIVATE t_mmc_error mmc_HSEnWideBus(t_uint8 cardno, t_mmc_wide_mode wide_mode, IN t_mmc_device_id mmc_device_id);
PRIVATE t_mmc_error mmc_testpin_write_fifo(t_mmc_device_id mmc_device_id);
PRIVATE t_mmc_error mmc_TestFunctionalPins_write
(
    IN t_uint8          cardno,
    IN t_mmc_wide_mode  mode,
    IN t_mmc_device_id  mmc_device_id
);
PRIVATE t_mmc_error mmc_TestFunctionalPins_read
(
    IN t_uint8          cardno,
    IN t_mmc_wide_mode  mode,
    IN t_mmc_device_id  mmc_device_id
);






#endif /* (__MMCI_HCL_ENHANCED) */

/*MAX NO OF CARDS THAT CAN BE SUPPORTED.*/

#define MAXCARDS    6
typedef enum
{
    CARD_INDEX1 = 1,
    CARD_INDEX2,
    CARD_INDEX3,
    CARD_INDEX4,
    CARD_INDEX5,
    CARD_INDEX6
}card_index;

/* MMC system context */
typedef struct
{
    t_mmc_register      *p_mmc_register;
    t_uint32            mmc_dvcontext[CONTEXT_COUNT];
#if defined __MMCI_HCL_ENHANCED
    t_mmc_card_info     card_array[MAXCARDS];
    t_mmc_current_card  current_card;
    t_uint8             no_of_cards;    /* NO OF CARDS CURRENTLY ACTIVE*/
#endif
} t_mmc_system_context;

#ifdef __cplusplus
}
#endif
#endif /*MMCP*/
