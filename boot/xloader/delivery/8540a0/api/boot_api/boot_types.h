/*-----------------------------------------------------------------------------
 * ST Ericsson
 * Reproduction and Communication of this document is strictly prohibited
 * unless specifically authorized in writing by ST Ericsson.
 *---------------------------------------------------------------------------*/
/** \addtogroup BOOT
 * @{
 */ 
/*---------------------------------------------------------------------------*/
/** \file boot_types.h
 *  \author ST Ericsson
 *  \brief This header file provides basic types common
 */

/*---------------------------------------------------------------------------*/
#ifndef _BOOT_TYPES_H
#define _BOOT_TYPES_H

#include "boot_secure_types.h"

/* Cut ID and device ID for U8540 Cut A0 (V1.0) */
#define ROM_CUT_ID_3       0x00    
#define ROM_CUT_ID_2       0x85 
#define ROM_CUT_ID_1       0x40    
#define ROM_CUT_ID_0       0xA0
#define ROM_CUT_ID         0x008540A0

// This type stores the indication below.
#define	ADL_INDICATION			0xF0030003 /**<  Boot indication for ADL indication.*/
#define	PRODUCTION_INDICATION	0xF0030005 /**<  Boot indication for Production indication.*/
#define	NORMAL_INDICATION		0xF0030001 /**<  Boot indication for Normal indication.*/
#define ALT_NORMAL_INDICATION 	0xF0030004 /**<  Boot indication for Alt Normal indication.*/

// FLASHING and PROGRAMMING indication are the same
// This is for compatbility between ROM code and tests
// Now use FLASHING_INDICATION
#define	FLASHING_INDICATION	    0xF0030002 /**<  Boot indication for 2nd boot indication.*/
#define	PROGRAMMING_INDICATION	0xF0030002 /**<  Boot indication for 2nd boot indication.*/

#define MASK_DEBUG_INFO			0xFFFFF8FF	/* masked debug information (bits 8-10) */
#define MASK_DMA_INFO			0xFFFFEFFF	/* masked DMA information (bit  12) */
#define MASK_ALL_INFO			0xFFFFE0FF	/* masked DMA & debug information (bit 12-8) */

#define ASCII_ISSW      "ISSW"
#define ISSW_OFFSET  0x14    /* offset of "X_LOADER" string from beginning of a 512 bytes buffer */
#define ISSW_SIZE    6      /* size of "ISSW" string (including final \0 and +1 byte )*/

#define ASCII_CRKC      "CRKC"

// TOC analysis related values
#define ITEM_NB			5
#define ITEM_NAME_SIZE	12
#define ITEM_MAX_NB		16

/*Asic ID defines*/
#define SEC_ASICID_PUB_ID_SIZE 20
#define SEC_ASICID_ROOT_KEY_HASH_SIZE 20



/** \internal  Structure used to store the boot abort or error cause. */
typedef struct {
	t_uint32 error;      /**<  \internal contains the t_boot_error  or the abort cause.*/
	t_uint32 lrReg;     /**<  \internal contains the LR register*/

}t_boot_error_info;

/** \internal  Structure used to store the boot indication. */
typedef union {
	struct{
		t_bitfield unused1:8;       /**< \internal Bit[7:0]   Unused         */
		t_bitfield TraceEnable :1;  /**< \internal Bit[8]     Trace Enable   */
		t_bitfield unused2:3;       /**< \internal Bit[11:9]  Unused         */    
		t_bitfield UseDma :1;       /**< \internal Bit[12]    Use DMA on 2nd Boot   */    
		t_bitfield unused3:19;      /**< \internal Bit[31:13] Unused         */
	}bit;                   /**< \internal Boot indication in bitfield. */
	t_uint32 scenario;     /**<  \internal contains the boot indication (Normal,ADL, Production...)*/

}t_boot_indication;

/** \internal  Lists the different sources of reset. */	
typedef enum {
    	A9CPU0_WDG_RESET, /**< \internal (0), A9 CPU0 WatchDog reset occurs. */
        A9CPU1_WDG_RESET, /**< \internal (1), A9 CPU1 WatchDog reset occurs. */
        APESW_RESET,      /**< \internal (2), APE software reset occurs. */
        APE_RESET,        /**< \internal (3), APE reset occurs. */
        SEC_WDG_RESET,    /**< \internal (4), Secure WatchDog occurs. */
        POWER_ON_RESET,   /**< \internal (5), Power On reset occurs. */  
        A9_DEEP_SLEEP,    /**< \internal (6), A9 restart from Deep Sleep. */
        APE_DEEP_SLEEP,   /**< \internal (7), APE restart from Deep Sleep. */
        NO_RESET          /**< \internal (8), No Reset source found. */
} t_boot_last_reset;	    
    
/** Type of memory used in normal boot. */
/* Note: this enum should match debug value of bits DbgBootIf in t_boot_rom_conf (t_load_if)*/
typedef enum {
    MMC = 2,        /**< (2), MMC card/eMMC */
}t_boot_memType;
   
/** \internal Memory interface used in normal boot. */
/* Note: this enum should match debug value of bits DbgBootIf in t_boot_rom_conf (t_load_if)*/
typedef enum {
	MMC2_SELECTED = 1,
    MMC4_SELECTED = 2, 
	NO_MORE_MEM_AVAILABLE = 5
}t_boot_memSelectType;

/** \internal  This type stores the peripheral used at Boot time 
to send indication, and other data. */	
/* Note: this enum should match debug value of bits DbgBootIf (t_load_if)
in t_boot_rom_conf */
typedef enum {
    UART_PERIPHERAL,    /**< \internal (0), UART used.*/
    USB_PERIPHERAL,     /**< \internal (1), USB used.*/
	NO_PERIPHERAL       /**< \internal (2), unused.*/
} t_boot_peripheral ;

/** Boot interface type. */
typedef enum {
    IF_UART = UART_PERIPHERAL,  /**< (0), UART used.*/
    IF_USB = USB_PERIPHERAL,    /**< (1), USB used.*/
    IF_MMC = MMC                /**< (2), MMC card/eMMC used. */
}t_load_if;

/* Unused on 8540, kept for backward SW compatibility : defines if Memory detection is made on all memories, MMC2/4, or ONE_NAND only */
typedef enum
{
	AUTO_DETECTION = 0,     /**< \internal (0) Detection to be done in this order: MMC/ONENAND/NAND*/
    ONENAND_ONLY = 1,       /**< \internal (1) Detection to be done only on ONE_NAND  */
	MMC_ONLY = 2,           /**< \internal (2) Detection to be done only on MMC */
	NAND_ONLY = 3,          /**< \internal (3) Detection to be done only on NAND */
	UNUSED = 4              /**< \internal (4) Not to be used */
}t_mem_detection_type;

/** \internal To define the timeout value for USB after a system reset */
typedef enum
{
	USB_TIME_DEFAULT = 0,     /**< \internal (0) Default: let the value programmed for power on reset */
    USB_TIME_30S = 1,         /**< \internal (1) Set the value to 30s */
    USB_TIME_1MIN = 2,        /**< \internal (2) Set the value to 1min */
    USB_TIME_DISABLE = 3      /**< \internal (3) Disable the timeout*/
}t_usb_timeout;

/* When doing changes in this enum take care that uart management still works. */
/** UART choice, Reflect AF UART interface selection value */
typedef enum {
    UART0 = 0,    /**< (0) to choose UART0 (GPIO2-3) */
    UART1 = 1,    /**< (1) to choose UART1 (GPIO4-5) */
    UART2_B = 2,  /**< (2) to choose UART2 position B (GPIO75-85) */
    NO_UART = 3   /**< (3) invalid  */
} t_uart_number;

/* debug struct  */
typedef struct {
    t_boot_peripheral Periph; /**< \internal UART or USB or no periph */
    t_uart_number UartNb; /**< \internal if uart indicate the uart num */
} t_debug_if;

 /** \internal
 This structure is used to store all boot information read on Antifuse "ROM configuration 3" at start up time.
 It is also used for the backupRAM boot information that can be used by OS application . */
typedef union {
    struct {
		t_bitfield unused0 : 1;  /*unused on 8540, bit moved. */          
        t_bitfield MemoryDetection : 3; /*unused on 8540 : Define the detection order and type for flash and MMC: \n
                                            (000)AutoDetect (001)OneNand/FlexOneNand (010)MMC (011)NAND (other)unused: autodetection. \n
                                            Can be set by OS.*/
        t_bitfield MMCPosition : 2;     /*unused on 8540 : Define MMC interface position: 
                                            (00)MMC2_MMC4, (01)MMC2, (10)MMC4 (11)Unused.(see t_MmcPosition)\n
                                            Can be set by OS. */  
        t_bitfield UsbDisable : 1;      /**< \internal Set (1) if USB boot scenario is NOT needed. Can be set by OS. */
        t_bitfield Uart0Disable : 1;    /**< \internal Set (1) if UART0 boot scenario is NOT needed (on UART0-a).Can be set by OS. */
        t_bitfield UartCkitDisable : 1; /*unused on 8540 :  Set (1) if UART Carkit boot scenario is NOT needed. Can be set by OS. */             
        t_bitfield Uart1Disable : 1;    /**< \internal Set (1) if UART1 boot scenario is NOT needed (on UART1-a).Can be set by OS. */
        t_bitfield Uart2Disable : 1;    /**< \internal Set (1) if UART2 boot scenario is NOT needed (on UART2-b).Can be set by OS. */
        t_bitfield unused1 : 4;         /*unused */      
        t_bitfield OSMemorySelect : 3;  /*unused on 8540 : Set by OS (0 to 4), to define the memory on which TOC search
                                                 is started after a system reset or secure watchdog reset */
        t_bitfield ChipConfig : 4;      /**< \internal CHIP_ID */       
        t_bitfield UsbStringSelect : 2;  /**< \internal USB String Select: (see t_usb_string_desc)
                                             (00) default1, (01) default2, (b10) Cust, (11) disable default string.\n
                                             Can be set by OS. */
        t_bitfield UsbEnumTimeout : 2;   /**< \internal USB enumeration timeout:
                                             (00)default (01)30s (10)1min (11)disabled \n
                                             Can be set by OS.*/
        t_bitfield OSUsbFlashTimeout : 2; /**< \internal Set by OS, USB flashing message timeout after system reset: 
                                              (00)default (01)30s (10)1min (11)disabled*/
        t_bitfield OSEnableDebug : 1;   /**< \internal Set (1) to enable debug trace after software reset. */
        
        t_bitfield OSPeriphBootEnable : 1; /**< \internal Set (1) By OS, if a USB or UART boot is required
                                                 after a system reset or secure watchdog reset. */
        t_bitfield OSBootBlockSelect : 2;  /**< \internal Set by OS (0 to 3), to define the boot block on which TOC search
                                                 is started after a system reset or secure watchdog reset */
        }Bit;                           /**< \internal Boot configuration  in bitfield. */
    t_uint32 BootConf;                  /**< \internal Boot configuration. */  	    	
} t_boot_rom_conf;

/* added for partial backward compatibility with 9540 */
#define t_secure_rom_boot_info t_boot_rom_conf

/** \internal
 This structure is used to store all boot information related to peripheral and retrieved from fuse/backup/servicemode GPIO.*/
typedef struct {
    t_uint16 UsbPID; /**< \internal if UsbDescValid is TRUE, PID to be used during USB enum*/
    t_uint16 UsbVID; /**< \internal if UsbDescValid is TRUE, VID to be used during USB enum*/
    t_bool   UsbDescValid; /**< \internal if UsbDescValid is TRUE, UsbPID and UsbVID must be used during USB enum
                                          if UsbDescValid is FALSE, ROM config must be used (see also UsbStringSelect fuse) */
    t_bool   TryPeriphBoot; /**< \internal Indicate if peripheral (USB/UART) boot must be tried before memory boot */
    t_uart_number UartDbgNumber; /**< \internal UART number that must be used if UART traces are enabled */
}t_boot_periph_conf;
    
 /** \internal
 This structure is used to store all boot information read on Antifuse/backupRAM at start up time.*/
typedef union {
    struct {
        t_bitfield DbgUsbVbus : 1;      /**< \internal In case of USB boot, set to (1) if VBUS is detected.   */
        t_bitfield DbgUsbEnumStart : 1; /**< \internal In case of USB boot,set to (1) if enumeration has started (soft connect done).   */
        t_bitfield DbgUsbEnumDone : 1;  /**< \internal In case of USB boot,set to (1) if enumeration is done (set configuration received).   */
        t_bitfield DbgUsbTimeout : 1;   /**< \internal In case of USB boot,set to (1) if timeout for receiving SW image expired.  */
        t_bitfield DbgMmcInit : 1;      /**< \internal Set to (1) if Mmc init succeed.   */
        t_bitfield DbgOneNandInit : 1;  /**< \internal Set to (1) if OneNand init succeed.   */
        t_bitfield DbgBootIf : 3;       /**< \internal Gives on which interface boot has been done: see t_load_if enum value .   */
        t_bitfield DbgImageFound : 1;   /**< \internal Set to (1) if ISSW image is found on 1 of the booting interface.   */
        t_bitfield DbgImageNb : 2;      /**< \internal Set to copy image number (0 to 3) */
        t_bitfield DbgImageLoaded : 1;  /**< \internal Set to (1) if ISSW image is Loaded to internal RAM.   */
        t_bitfield DbgSubImageCopy:15;  /**< \internal Bit_n set to 1 if TOC SubImage_(n+1) is Loaded to internal RAM.   */
        t_bitfield unused: 4;           /**< \internal unused */
        }Bit;                           /**< \internal Boot trace information in bitfield. */
    t_uint32 Reg;                  /**< \internal Boot trace information in word. */  	    	
} t_boot_trace_info;

 /** \internal
 This structure is used to store the ASIC id */
typedef  struct
{
	t_uint8 SubBlockNumber;
	
	/* ID sub block */
	t_uint8 Id_SubBlockId;
	t_uint8 Id_SubBlockSize;
	t_uint8 Id_NomadikSubBlockId;
	t_uint8 Id_NomadikId[3];
	t_uint8 Id_AsicVersion;	
	t_uint8 Id_ChipOption[2];
	t_uint8 Id_ChipCustId[4];	
	/* Secure Mode Sub ID */
	t_uint8 Secure_SubBlockId;
	t_uint8 Secure_SubBlockSize;
	t_uint8 Secure_NomadikSubBlockId;
	t_uint8 Secure_ModeInfo;	
	/* Public ID Sub Block */
	t_uint8 PublicId_SubBlockId;
	t_uint8 PublicId_SubBlockSize;
	t_uint8 PublicId_NomadikSubBlockId;
	t_uint8 PublicId_Id[SEC_ASICID_PUB_ID_SIZE];	
	/* Root Key Hash Sub block */
	t_uint8 RootKey_SubBlockId;
	t_uint8 RootKey_SubBlockSize;
	t_uint8 RootKey_NomadikSubBlockId;
	t_uint8 RootKey_Hash[SEC_ASICID_ROOT_KEY_HASH_SIZE];
	/* Checksum Sub Block */
	t_uint8 Checksum_SubBlockId;
	t_uint8 Checksum_SubBlockSize;
	t_uint8 Checksum_NomadikSubBlockId;
	t_uint32 Checksum_PublicRomCrc __attribute__ ((packed));
	t_uint32 Checksum_SecureRomCrc __attribute__ ((packed));
} t_boot_asic_id_desc;


// Image areas list
typedef enum
{
	X_LOADER_ITEM,
	SECOND_ITEM,
	KEYS_ITEM,
	R_AND_D_ITEM,
	PRIMAAPP_ITEM
} t_boot_sub_image_id;

// Description of an image area
typedef struct
{
	t_uint32 StartAddress;
	t_uint32 Length;
	t_uint32 Offset;
} t_boot_data_area;

// Sub image item depiction
typedef struct
{
	t_uint32 Start;
	t_uint32 Size;
	t_uint32 Flags;
	t_uint32 Align;
	t_uint32 Load;
	char	 FileName[12];
} t_boot_toc_item;


typedef t_uint32 t_address; /**< Physical address */

#ifndef HAVE_T_SIZE
#define HAVE_T_SIZE
typedef t_uint32 t_size; /**< Size */
#endif

#endif		// _BOOT_TYPES_H
/** @} */
