//--========================================================================--
//*****************************************************************************
//*  ST-Ericsson                                                              *
//*  Reproduction and Communication of this document is strictly prohibited   *
//*  unless specifically authorized in writing by ST-Ericsson.                *
//*---------------------------------------------------------------------------*/
//*
//* File Name     : db8500b0_soc_settings_toshiba_400mhz.c
//* DDR           : "Elpida 4Gbits TYC0DH231234LA10 Toshiba" 400Mhz
//--========================================================================--


//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>

#include "numerics.h"
#include "xp70_memmap.h"

#define VERSION_MEMINIT_SOC "v2.1.0"

typedef unsigned char		t_uint8;
typedef unsigned long		t_uint32;

#define BOOT_OK					0
#define BOOT_INTERNAL_ERROR		0xA0010022
#define SECOND_BOOT_INDICATION	0xF0030002
#define PWR_MNGT_STARTED   		0x00000001

// HW I2C definitions
#define PRCMBaseAddress 0x80157000

// AB8500V2 voltage values to use with AVS as default
// Note (SA): will be updated with AVS fused value
// Voltage and body biasing
#define DEFAULT_VBB             0xDB

#define VBB_RET                 DEFAULT_VBB
#define VBB_MAX_OPP             0xDB
#define VBB_100_OPP             0xDB
#define VBB_50_OPP              0xDB
#define COMP_VARM_MAX_OPP       0x2F
#define COMP_VARM_100_OPP       0x2F
#define COMP_VARM_50_OPP        0x1D
#define COMP_VARM_RET           0x02
#define COMP_VAPE_100_OPP       0x29
#define COMP_VAPE_50_OPP        0x1A
#define COMP_VMOD_100_OPP       0x29
#define COMP_VMOD_50_OPP        0x1A
#define COMP_VSAFE              0x29


//IO access macro
#define  IO(addr)  (*((volatile unsigned*) (addr)))
#define SET_REG_FIELD(old_value, new_value, mask) ((old_value)&~(mask)) | ((new_value)&(mask))

#define smem(address,value)		IO(address) = (t_uint32) (value)
#define mem(address)			IO(address)

#define CFG_UART2_BASE			0x80007000
#define UART_ADDR               CFG_UART2_BASE

//#undef LOG_DEBUG
//#define LOG_DEBUG


//#####
/* Tempo macro */
//#####
#define my_wait(tempo)               \
{\
volatile int cnt=tempo;\
volatile t_uint32  temp;\
do \
{\
if (cnt <= 0) break; \
cnt--;\
temp = mem(UART_ADDR);\
} while (1);\
}


//#####
/* macro to write a character string message on UART */
//#####
#ifdef LOG_DEBUG
#define logMsg(str)               \
{\
volatile int idx=0;\
volatile t_uint32  temp;\
smem(UART_ADDR,'>');\
do \
{\
if ((str)[idx] == 0) break; \
*(t_uint32*)UART_ADDR = (str)[idx]; \
my_wait(1000);\
temp = mem(UART_ADDR);\
idx++;\
} while (1);\
}
#else
#define logMsg(x) my_wait(100000)
#endif

//#####
//Version string definition
//#####
char	version_str[128] = "@#MEM_INIT_"VERSION_MEMINIT_SOC"_"__FILE__"_"__DATE__"/"__TIME__;


///////////////////////////////////////
//Modem trace configuration
///////////////////////////////////////
void Modem_STM_config()
{
	//; ##########################################
	//; #       Configure GPIO for STM Modem
	//; ##########################################
	//; Configure GPIOs 70 to 76 in AltCx mode (AltA + AltB = AltC) 
	IO(0x8000e020) |= 0x00001FC0; //GPIO 70 to 76 AFSELA
	IO(0x8000e024) |= 0x00001FC0; //GPIO 70 to 76 AFSELB
	
	//; ##########################################
	//; #       Configure PRCMU for STM Modem
	//; ##########################################
	IO(0x80157138) |= 0x00000801; //Enable AltC3 for STM Modem signals on GPIOs 70 -> 76
}


///////////////////////////////////////
//PRCMU trace configuration
///////////////////////////////////////
void PRCMU_STM_config()
{
	//; ##########################################
	//; #       Configure GPIO for STM PRCMU
	//; ##########################################
	// Configure GPIOs 70 to 74 in AltC for APE_STM
	IO(0x8000e020) |= 0x000007C0; //AltA
	IO(0x8000e024) |= 0x000007C0; //AltB
	IO(0x8000E01C) |= 0x000007C0;	// SLPM
	
//	IO(0x8010F000) = 0x260;			// STM_CR : swap prcmu data, clkdiv=2, 1 data output only
//	IO(0x8010F000) = 0x1C8;			// STM_CR : clkdiv=16, 4 data output : config Jocelyn
//	IO(0x8010F000) = 0x048;			// STM_CR : clkdiv=4 => 50MHz, 4 data output

	IO(0x8010F000) = 0x048;		// STM_CR : clkdiv=4 => 50MHz, 4 data output
//	IO(0x8010F000) = 0x08;			// STM_CR : clkdiv=2 => 100MHz, 4 data output, with TS

	IO(0x8010F008) = 0x20;			// STM_MMC
	IO(0x8010F010) = 0x20;			// STM_TER
}


//+CAP_2400_001
#pragma O0
//+CAP_2353_001		
///////////////////////////////////////
// Test_Write_Read_DDR
///////////////////////////////////////
t_uint32 Test_Write_Read_DDR()
{
	t_uint32 address;
	t_uint32 i;
	t_uint32 data;
	t_uint32 data_written[33];
	t_uint8 byte1, byte2;

	logMsg("LpDDR test...\r\n");
	//write in RAM to test address wires [0:12] and data wires [0:27]
	//adress (0x1) to (0x8000000) (=> some address wires will be tested twice, doesn't matter)
	//data   0x1 to 0x8000000 
	address = 1;
	data = 1;	
	for (i=0; i<2; i++ )
	{
		data_written[i] = data;
		*((t_uint8*)(address)) = (t_uint8)(data);

		address = 1<<(i+1);
		data = data<<1;
	}

	/*write in RAM DDR 32bit data*/
	/*adress 0x4 to 0x8000000*/
	/*data   0x4 to 0x8000000*/ 
	for (i=2; i<28; i++ )
	{
		data_written[i] = data;
		*((t_uint32*)(address)) = (t_uint32)(data);

		address = 1<<(i+1);
		data = data<<1;
	}

	//write in RAM to test data wires [28:31]
	//address 0x800 + 4*n*(1 word)
	//data: 0x10000000 to 0x80000000 => all 32 data wires will be then tested!!!

	address = 0x800;
	for (i=1; i<=4; i++)
	{
		//write in address 0x804 data 0x10000000
		//then write in address 0x808 data 0x20000000
		//then write in address 0x80C data 0x40000000
		//then write in address 0x810 data 0x80000000
		data_written[i+27] = 1<<(i+27);
		*((t_uint32*)(address+(4*i))) = 1<<(i+27);
	}

	/*write in RAM DDR - to check bit 0 and 1 of data bus*/
	/*address 0x814*/
	/*data 0x3*/
	address = 0x814;
	data = 0x3;
	data_written[32] = data;
	*((t_uint32*)(address)) = data;

	//////////////////
	//now check the written data by the way of a readback
	//same loops as the both above...
	/////////////////

	/* test bit 0 and bit 1 of address bus */
	address = 0;

	byte1 = ((*(t_uint32*)(address)) & (0x00FF00)) >> 8;
	if (byte1 != (t_uint8)(data_written[0]))
	{
		return BOOT_INTERNAL_ERROR;
	}

	byte2 = ((*(t_uint32*)(address)) & (0xFF0000)) >> 16;
	if (byte2 != (t_uint8)(data_written[1]))
	{
		return BOOT_INTERNAL_ERROR;
	}

	/*set address for next reading back*/
	address = 0x4;

	for (i=2; i<28; i++ )
	{
		if (*((t_uint32*)(address)) != (t_uint32)(data_written[i]))
		{
			return BOOT_INTERNAL_ERROR;
		}
		address = 1<<(i+1);
	}

	address = 0x800;
	for (i=1; i<=4; i++)
	{
		if (*((t_uint32*)(address+(4*i))) != data_written[i+27])
		{
			return BOOT_INTERNAL_ERROR;
		}
	}
	/* test bit 0 and bit 1 of data bus */
	address = 0x814;
	if (*((t_uint32*)address) != data_written[32])
	{
		return BOOT_INTERNAL_ERROR;
	}

	logMsg("OK\r\n");
	return BOOT_OK;
}
//+CAP_2400_001
#pragma O3
//+CAP_2353_001		




void HWI2C_Transfer_write(char bank, char regAdd, char RegVal) 
{
	volatile t_ReqMb5 *p_ReqMB5;
	
  p_ReqMB5  = (t_ReqMb5*)(TCDM_BASE+0xE44); 
  	
	p_ReqMB5->un.I2CopType = I2CWrite;
	p_ReqMB5->un.SlaveAdd = (bank&0x1F)|0x20;		// To be used with AB8500V2	
	//p_ReqMB5->un.SlaveAdd = bank;								// To be used with AB8500V1
	p_ReqMB5->un.HwGCEn = 0;
	p_ReqMB5->un.StopEn = 1;
	p_ReqMB5->un.RegAdd = regAdd;
	p_ReqMB5->un.RegVal = RegVal;
	
	// Send mb5 (IT17) interrupt
	IO(0x80157100) = 0x20;
	
	// Check IT for ackMB5
	while((IO(0x80157494) & 0x20) != 0x20);

	// Clear IT1 ackmb5
	IO(0x8015748C)=0x20;
	
}



char HWI2C_Transfer_read(char bank, char regAdd, char RegVal) 
{
	volatile t_ReqMb5 *p_ReqMB5;
	volatile t_AckMb5 *p_AckMB5;
	volatile unsigned data;
		
  p_ReqMB5  = (t_ReqMb5*)(TCDM_BASE+0xE44); 
  p_AckMB5  = (t_AckMb5*)(TCDM_BASE+0xDF4);
  	
	p_ReqMB5->un.I2CopType = I2CRead;
	p_ReqMB5->un.SlaveAdd = (bank&0x1F)|0x20;		  // To be used with AB8500V2	
	//p_ReqMB5->un.SlaveAdd = bank;								// To be used with AB8500V1
	p_ReqMB5->un.HwGCEn = 0;
	p_ReqMB5->un.StopEn = 1;
	p_ReqMB5->un.RegAdd = regAdd;
	p_ReqMB5->un.RegVal = RegVal;
	
	// Send mb5 (IT17) interrupt
	IO(0x80157100) = 0x20;
	
	// Check IT for ackMB5
	while((IO(0x80157494) & 0x20) != 0x20);

  data = (p_AckMB5->un.RegVal) & 0xFF;

	// Clear IT1 ackmb5
	IO(0x8015748C)=0x20;
	
	return(data);
	
}

//#####
//Main function : entry point
//#####
#pragma arm section code = ".entry"
t_uint32 PlatformInit(t_uint32 boot_indication, t_uint32 boot_status)
{	
	
	 //volatile char toto;  // for debug
	 
	char pwr_current_state;
	volatile t_Header *p_header;
	volatile t_ReqMb0 *p_ReqMB0;
	volatile t_ShVar1 *p_MB_status;
	volatile t_AckMb4 *p_ackMB4;
	volatile t_DDRInit *p_ddr_mailbox;
	volatile t_AVSData *p_AVSData;
	char     ddr_status;

  unsigned i;
	
	t_uint32 uPass = BOOT_OK;

	logMsg("\r\nMEM_INIT start...\r\n");
	
	p_header         = (t_Header*)(TCDM_BASE+0xFE8);
	p_ReqMB0         = (t_ReqMb0*)(TCDM_BASE+0xFDC);
	p_MB_status      = (t_ShVar1*)(TCDM_BASE+0xFFC);
	p_ackMB4				 = (t_AckMb4*)(TCDM_BASE+0xDF8);
    
  p_AVSData = (t_AVSData *)(TCDM_BASE + 0x2E0);  
	p_ddr_mailbox    = (t_DDRInit *)(TCDM_BASE+0x000);
	
	if ( (boot_status & PWR_MNGT_STARTED) == PWR_MNGT_STARTED)
	{
		logMsg("MEM_INIT PWR_MNGT_STARTED\r\n");

		//***********************
		// All GPIOs reset
		//***********************
		IO(0x80157208) = 0x3FFFF;

		// Clear AckMB interrupt
  	IO(0x8015748C)=0xFF;
	
 		// AB8500 update to config2 to be accessible in HWI2C
 		// => Bank 4 registers will be accessible with APE HWI2C 
 		// => Sim Control registers will be accessible with Modem I2C
		// => other registers will be accessible with APE HWI2C
  	HWI2C_Transfer_write(0x03,0x00,0x02);    // write 0x2 in @0x300 (ReguSerialCtrl1 register)		

		// Enable  body biasing (vbbn + vbbp)
	  HWI2C_Transfer_write(0x04, 0x01, 0x45);
	  	  
		// BUG VI28193
		// Set VRF1Regu[1:0] to '10'
		HWI2C_Transfer_write(0x04,0x0A,0x09);

    //************************************************************
    // Copy AVS structure in xp70 data mem and call FW AVS service
    //************************************************************
    p_AVSData->Voltage[AVS_VBB_RET     ]   = VBB_RET;
    p_AVSData->Voltage[AVS_VBB_MAX_OPP ]   = VBB_MAX_OPP;
    p_AVSData->Voltage[AVS_VBB_100_OPP ]   = VBB_100_OPP;
    p_AVSData->Voltage[AVS_VBB_50_OPP  ]   = VBB_50_OPP;
    p_AVSData->Voltage[AVS_VARM_MAX_OPP]   = (1<<7)+COMP_VARM_MAX_OPP;  //  by default allow MAX opp. This is the aim of MASK_BIT7.
    p_AVSData->Voltage[AVS_VARM_100_OPP]   = COMP_VARM_100_OPP;
    p_AVSData->Voltage[AVS_VARM_50_OPP ]   = COMP_VARM_50_OPP;
    p_AVSData->Voltage[AVS_VARM_RET    ]   = COMP_VARM_RET;
    p_AVSData->Voltage[AVS_VAPE_100_OPP]   = COMP_VAPE_100_OPP;
    p_AVSData->Voltage[AVS_VAPE_50_OPP ]   = COMP_VAPE_50_OPP;
    p_AVSData->Voltage[AVS_VMOD_100_OPP]   = COMP_VMOD_100_OPP;
    p_AVSData->Voltage[AVS_VMOD_50_OPP ]   = COMP_VMOD_50_OPP;
    p_AVSData->Voltage[AVS_VSAFE       ]   = COMP_VSAFE;	

	  // AVS service request
	 	p_header->Req.un.Mb4 = AvsH;
		// MailBox4 => 
	  
		// Send IT16 to XP70 to request AVS service
		IO(0x80157100) |= 0x10; 

		while (p_ackMB4->field.AckMb4Status != (t_AckMb4Status)Avs_Ok);
			
		logMsg("AVS_INIT OK\r\n");

		// Clear AckMB4
		p_ackMB4->field.AckMb4Status = AckMb4Init;

		
		//*********************************
		// Switch xP70 Fw in ApExecute
		//*********************************
		
		// power state transition request (header of MailBox0)
		(p_header->Req).un.Mb0 = PwrStTrH;
		(p_ReqMB0->PwrStTr).un.ApPwrStTr = ApBootToApExecute;
	  
		// Send IT (nb 10) to XP70
		IO(0x80157100) |= 0x1;

		logMsg("MEM_INIT IT sent to XP70\r\n");
		
		pwr_current_state = p_MB_status->pwr_current_state_var;
		// Check mailbox status 
		while (pwr_current_state != (t_ApPwrSt)ApExecute) 
			pwr_current_state = p_MB_status->pwr_current_state_var;
	  
	 	logMsg("MEM_INIT XP70 ApExecute\r\n");

		//*********************************
		// Fill DDR settings into XP70 data area 
		//*********************************
		
		// Copy Mask for DDR controller registers
		p_ddr_mailbox->MaskCfg0[0] = 0x6DFFFFFF; 	// no param into DENALI_CTL_25/28/31 
		p_ddr_mailbox->MaskCfg0[1] = 0x9800F1FF; 	// no param into DENALI_CTL_41/42/43/48to58 /61/62 
		p_ddr_mailbox->MaskCfg0[2] = 0xFFFFF309; 	// no param into DENALI_CTL_65/66/68to71/74/75 
		p_ddr_mailbox->MaskCfg0[3] = 0xFFFFFFFF; 
		p_ddr_mailbox->MaskCfg0[4] = 0x0001E01F; 	// no param into DENALI_CTL_133to140
	
	
		// DDR settings		
		// REG_CONFIG_0 databahn registers
		p_ddr_mailbox->Cfg0[0] = 0x00000101;  //  DENALI_CTL_0
		p_ddr_mailbox->Cfg0[1] = 0x01010100;  //  DENALI_CTL_1
		p_ddr_mailbox->Cfg0[2] = 0x00010001;  //  DENALI_CTL_2
		p_ddr_mailbox->Cfg0[3] = 0x00010100;  //  DENALI_CTL_3
		p_ddr_mailbox->Cfg0[4] = 0x01010100;  //  DENALI_CTL_4
		p_ddr_mailbox->Cfg0[5] = 0x01000100;  //  DENALI_CTL_5
		p_ddr_mailbox->Cfg0[6] = 0x00000000;  //  DENALI_CTL_6
		p_ddr_mailbox->Cfg0[7] = 0x00000001;  //  DENALI_CTL_7
		p_ddr_mailbox->Cfg0[8] = 0x00000101;  //  DENALI_CTL_8
		p_ddr_mailbox->Cfg0[9] = 0x02020303;  //  DENALI_CTL_9
		p_ddr_mailbox->Cfg0[10] = 0x03030303;  //  DENALI_CTL_10
		p_ddr_mailbox->Cfg0[11] = 0x01030303;  //  DENALI_CTL_11
		p_ddr_mailbox->Cfg0[12] = 0x00000301;  //  DENALI_CTL_12
		p_ddr_mailbox->Cfg0[13] = 0x00000000;  //  DENALI_CTL_13
		p_ddr_mailbox->Cfg0[14] = 0x00060207;  //  DENALI_CTL_14
		p_ddr_mailbox->Cfg0[15] = 0x03000000;  //  DENALI_CTL_15
		p_ddr_mailbox->Cfg0[16] = 0x04020202;  //  DENALI_CTL_16
		p_ddr_mailbox->Cfg0[17] = 0x0c000f03;  //  DENALI_CTL_17
		p_ddr_mailbox->Cfg0[18] = 0x00000f0d;  //  DENALI_CTL_18
		p_ddr_mailbox->Cfg0[19] = 0x000e0000;  //  DENALI_CTL_19
		p_ddr_mailbox->Cfg0[20] = 0x0002000a;  //  DENALI_CTL_20
		p_ddr_mailbox->Cfg0[21] = 0x00060803;  //  DENALI_CTL_21
		p_ddr_mailbox->Cfg0[22] = 0x05000909;  //  DENALI_CTL_22
		p_ddr_mailbox->Cfg0[23] = 0x32002e1a;  //  DENALI_CTL_23
		p_ddr_mailbox->Cfg0[24] = 0x23003200;  //  DENALI_CTL_24
		p_ddr_mailbox->Cfg0[25] = 0x08110000;  //  DENALI_CTL_26
		p_ddr_mailbox->Cfg0[26] = 0x00000034;  //  DENALI_CTL_27
		p_ddr_mailbox->Cfg0[27] = 0x06100610;  //  DENALI_CTL_29
		p_ddr_mailbox->Cfg0[28] = 0x06100610;  //  DENALI_CTL_30
		p_ddr_mailbox->Cfg0[29] = 0xffff0000;  //  DENALI_CTL_32
		p_ddr_mailbox->Cfg0[30] = 0xffffffff;  //  DENALI_CTL_33
		p_ddr_mailbox->Cfg0[31] = 0x0000ffff;  //  DENALI_CTL_34
		p_ddr_mailbox->Cfg0[32] = 0x00100100;  //  DENALI_CTL_35
		p_ddr_mailbox->Cfg0[33] = 0x00000000;  //  DENALI_CTL_36
		p_ddr_mailbox->Cfg0[34] = 0x00030000;  //  DENALI_CTL_37
		p_ddr_mailbox->Cfg0[35] = 0x00391b4e;  //  DENALI_CTL_38
		p_ddr_mailbox->Cfg0[36] = 0x0000003a;  //  DENALI_CTL_39
		p_ddr_mailbox->Cfg0[37] = 0x00000028;  //  DENALI_CTL_40
		p_ddr_mailbox->Cfg0[38] = 0x00000000;  //  DENALI_CTL_44
		p_ddr_mailbox->Cfg0[39] = 0x00000000;  //  DENALI_CTL_45
		p_ddr_mailbox->Cfg0[40] = 0x00000000;  //  DENALI_CTL_46
		p_ddr_mailbox->Cfg0[41] = 0x00000000;  //  DENALI_CTL_47
		p_ddr_mailbox->Cfg0[42] = 0x00000000;  //  DENALI_CTL_59
		p_ddr_mailbox->Cfg0[43] = 0x00000000;  //  DENALI_CTL_60
		p_ddr_mailbox->Cfg0[44] = 0x05000003;  //  DENALI_CTL_63
		p_ddr_mailbox->Cfg0[45] = 0x00050006;  //  DENALI_CTL_64
		p_ddr_mailbox->Cfg0[46] = 0x00000000;  //  DENALI_CTL_67
		p_ddr_mailbox->Cfg0[47] = 0x00000000;  //  DENALI_CTL_72
		p_ddr_mailbox->Cfg0[48] = 0x00007c00;  //  DENALI_CTL_73
		p_ddr_mailbox->Cfg0[49] = 0x01000100;  //  DENALI_CTL_76
		p_ddr_mailbox->Cfg0[50] = 0x01000103;  //  DENALI_CTL_77
		p_ddr_mailbox->Cfg0[51] = 0x02020200;  //  DENALI_CTL_78
		p_ddr_mailbox->Cfg0[52] = 0x02000103;  //  DENALI_CTL_79
		p_ddr_mailbox->Cfg0[53] = 0x14000100;  //  DENALI_CTL_80
		p_ddr_mailbox->Cfg0[54] = 0x0001b040;  //  DENALI_CTL_81
		p_ddr_mailbox->Cfg0[55] = 0x00900000;  //  DENALI_CTL_82
		p_ddr_mailbox->Cfg0[56] = 0x01900024;  //  DENALI_CTL_83
		p_ddr_mailbox->Cfg0[57] = 0x00000014;  //  DENALI_CTL_84
		p_ddr_mailbox->Cfg0[58] = 0x00000000;  //  DENALI_CTL_85
		p_ddr_mailbox->Cfg0[59] = 0x00000000;  //  DENALI_CTL_86
		p_ddr_mailbox->Cfg0[60] = 0x00820000;  //  DENALI_CTL_87
		p_ddr_mailbox->Cfg0[61] = 0x00040082;  //  DENALI_CTL_88
		p_ddr_mailbox->Cfg0[62] = 0x00010004;  //  DENALI_CTL_89
		p_ddr_mailbox->Cfg0[63] = 0x00000001;  //  DENALI_CTL_90
		p_ddr_mailbox->Cfg0[64] = 0x00000000;  //  DENALI_CTL_91
		p_ddr_mailbox->Cfg0[65] = 0x00013880;  //  DENALI_CTL_92
		p_ddr_mailbox->Cfg0[66] = 0x00000190;  //  DENALI_CTL_93
		p_ddr_mailbox->Cfg0[67] = 0x00000fa0;  //  DENALI_CTL_94
		p_ddr_mailbox->Cfg0[68] = 0x00010100;  //  DENALI_CTL_95
		p_ddr_mailbox->Cfg0[69] = 0x01000001;  //  DENALI_CTL_96
		p_ddr_mailbox->Cfg0[70] = 0x0a020201;  //  DENALI_CTL_97
		p_ddr_mailbox->Cfg0[71] = 0x0009090a;  //  DENALI_CTL_98
		p_ddr_mailbox->Cfg0[72] = 0x0fff0000;  //  DENALI_CTL_99
		p_ddr_mailbox->Cfg0[73] = 0x00000fff;  //  DENALI_CTL_100
		p_ddr_mailbox->Cfg0[74] = 0x03021000;  //  DENALI_CTL_101
		p_ddr_mailbox->Cfg0[75] = 0x00050604;  //  DENALI_CTL_102
		p_ddr_mailbox->Cfg0[76] = 0x06100610;  //  DENALI_CTL_103
		p_ddr_mailbox->Cfg0[77] = 0x00000610;  //  DENALI_CTL_104
		p_ddr_mailbox->Cfg0[78] = 0x00080403;  //  DENALI_CTL_105
		p_ddr_mailbox->Cfg0[79] = 0x00110905;  //  DENALI_CTL_106
		p_ddr_mailbox->Cfg0[80] = 0x39341a0d;  //  DENALI_CTL_107
		p_ddr_mailbox->Cfg0[81] = 0x0304017e;  //  DENALI_CTL_108
		p_ddr_mailbox->Cfg0[82] = 0x000f0610;  //  DENALI_CTL_109
		p_ddr_mailbox->Cfg0[83] = 0x0039001d;  //  DENALI_CTL_110
		p_ddr_mailbox->Cfg0[84] = 0x001e0010;  //  DENALI_CTL_111
		p_ddr_mailbox->Cfg0[85] = 0x0089003a;  //  DENALI_CTL_112
		p_ddr_mailbox->Cfg0[86] = 0x0030c6c9;  //  DENALI_CTL_113
		p_ddr_mailbox->Cfg0[87] = 0x03494288;  //  DENALI_CTL_114
		p_ddr_mailbox->Cfg0[88] = 0x00012009;  //  DENALI_CTL_115
		p_ddr_mailbox->Cfg0[89] = 0x00048024;  //  DENALI_CTL_116
		p_ddr_mailbox->Cfg0[90] = 0x00090024;  //  DENALI_CTL_117
		p_ddr_mailbox->Cfg0[91] = 0x125224a4;  //  DENALI_CTL_118
		p_ddr_mailbox->Cfg0[92] = 0x00110022;  //  DENALI_CTL_119
		p_ddr_mailbox->Cfg0[93] = 0x00410082;  //  DENALI_CTL_120
		p_ddr_mailbox->Cfg0[94] = 0x00110022;  //  DENALI_CTL_121
		p_ddr_mailbox->Cfg0[95] = 0x00008001;  //  DENALI_CTL_122
		p_ddr_mailbox->Cfg0[96] = 0x00020004;  //  DENALI_CTL_123
		p_ddr_mailbox->Cfg0[97] = 0x00008001;  //  DENALI_CTL_124
		p_ddr_mailbox->Cfg0[98] = 0x02491001;  //  DENALI_CTL_125
		p_ddr_mailbox->Cfg0[99] = 0x02ca1001;  //  DENALI_CTL_126
		p_ddr_mailbox->Cfg0[100] = 0x044b36c9;  //  DENALI_CTL_127
		p_ddr_mailbox->Cfg0[101] = 0x019130c6;  //  DENALI_CTL_128
		p_ddr_mailbox->Cfg0[102] = 0x1c7334c6;  //  DENALI_CTL_129
		p_ddr_mailbox->Cfg0[103] = 0x0c31a906;  //  DENALI_CTL_130
		p_ddr_mailbox->Cfg0[104] = 0x0c31a083;  //  DENALI_CTL_131
		p_ddr_mailbox->Cfg0[105] = 0x0201a347;  //  DENALI_CTL_132
		p_ddr_mailbox->Cfg0[106] = 0x03cb1001;  //  DENALI_CTL_141
		p_ddr_mailbox->Cfg0[107] = 0x00010003;  //  DENALI_CTL_142
		p_ddr_mailbox->Cfg0[108] = 0x00008001;  //  DENALI_CTL_143
		p_ddr_mailbox->Cfg0[109] = 0x00010003;  //  DENALI_CTL_144
                          
	  //Lpddr2 phy
		p_ddr_mailbox->Cfg0[110] = 0x03cb1001;  // DENALI_CTL_72


		IO(0x80157438) = 0x00000000;		         // LpDDR2 start at 400Mhz

		logMsg("MEM_INIT LpDDR2 at 400Mhz settings filled\r\n");
				 
		// DDR init request (header of MailBox4)
		p_header->Req.un.Mb4 = ddrInitH;
		// MailBox4 => no need to fill MB4 structure
	  
		// Send IT16 to XP70 to init DDR 
		IO(0x80157100) |= 0x10;

		logMsg("MEM_INIT IT16 to XP70 to init DDR\r\n");
		
		ddr_status = p_ackMB4->field.AckMb4Status;
		while (ddr_status != (t_AckMb4Status)DDROn_Ok)
			ddr_status = p_ackMB4->field.AckMb4Status;
			
		logMsg("MEM_INIT DDR_ON OK\r\n");

		// Clear AckMB4
		p_ackMB4->field.AckMb4Status = AckMb4Init;
		
		// Clear PRCM_ARM_IT1 IT for ackmb4
 	  while((IO(0x80157494) & 0x10) != 0x10);
 	  // Clear PRCM_ARM_IT1 ackmb4
 	  IO(0x8015748C)=0x10;
  
	}		



	//#######################################
  //#         Modem start-up
  //#######################################
  
  // Needed to send SYSCLK_OK to modem, but should be corrected in fw xP70
  IO(0x80157318) = 0x0;      //PRCM_SYSCLKOK_DELAY (val * 32K) 0 no delay

  IO(0x801571FC) = 0x3;      // Release Modem PURX and SWReset
  IO(0x801574A4) = 0x1;      // Ack SWReset from Modem in case of reboot due to this reason
  IO(0x801574A4) = 0x0;
  
  i = 0;
  while( ((IO(0x801574A0) & 0x1) != 0x1) && (i<10000)) {
  	i++;
  }

  // Request Modem HOST_PORT interface
  IO(0x80157334) = 0x00007;   // PRCM_HOSTACCESS_REQ
                                  // [16] Wake_req = 0
                                  // [2:1] HOSTACCESS_ID = 11 (A9ss)
                                  // [0] mod_host_port_req = 1

  // Waiting for modem HOST_PORT available (other signal name HOST_PORT_OK)
  i = 0;
  while( ((IO(0x80157170) & 0x800) != 0x800) && (i<10000)) {
   	i++;
  } 	


	//####################################################################################
	//####################################################################################
	// Now, MSS is powered, and ready to execute SW.
	// the HOST PORT interface is enabled, so A9SS can access to the MSS mapping
	//####################################################################################
	//####################################################################################


	//#####
	//Configure Modem STM trace
	//#####
	Modem_STM_config();
	logMsg("MEM_INIT Modem_STM_config\r\n");
	
	//#####
	//Configure PRCMU STM trace
	//#####
	//PRCMU_STM_config();
	
	//DDR test
	if (boot_indication == SECOND_BOOT_INDICATION)
	{
	    uPass = Test_Write_Read_DDR();
	}
	logMsg("MEM_INIT end\r\n");
	
	
	return uPass;
}

