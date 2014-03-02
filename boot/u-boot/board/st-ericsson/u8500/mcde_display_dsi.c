/*
* Copyright (C) ST-Ericsson SA 2010
*
* Author: Jimmy Rubin <jimmy.rubin@stericsson.com>
* for ST-Ericsson.
*
* License terms: GNU General Public License (GPL), version 2.
*/


#include <common.h>
#include <command.h>
#include <asm/arch/common.h>
#include "mcde_display.h"
#include "dsilink_regs.h"
#include "mcde_regs.h"
#include <malloc.h>
#include "mcde.h"
#include <linux/err.h>
#include <asm/arch/ab8500.h>

#include <spi.h>
#include "db8500_pincfg.h"
#include "db8500_pins.h"

#define DCS_CMD_EXIT_SLEEP_MODE       0x11
#define DCS_CMD_SET_DISPLAY_ON        0x29

#define TRULY_PANEL_ID 	0x00
#define SHARP_PANEL_ID 	0x22
#define BOE_PANEL_ID 	0x40

/*Disable below panels*/
#define LEAD_PANEL_ID   0xFE
#define BYD_PANEL_ID    0xFF

int mcde_dsi_dcs_write16(struct mcde_port *port, u16 cmd, u8* data, int len)
{
    int ret = 0;
    u8 cmdh = (cmd >> 8) & 0xff;
    u8 cmdl = cmd & 0xff;
    
    /*write reg offset*/
    ret = mcde_dsi_dcs_write(port, 0x00, &cmdl, 1);
    ret = mcde_dsi_dcs_write(port, cmdh, data, len);

    return ret;
}

int mcde_turn_on_lead_dsi(void)
{
    int ret = 0;
   	u8 buf[60] = {0};
            
    //Select CMD2, Page 0
	buf[0] = 0x55;
	buf[1] = 0xAA;
	buf[2] = 0x52;
	buf[3] = 0x08;
	buf[4] = 0x00;
	ret = mcde_dsi_dcs_write(main_display.port, 0xF0, buf, 5);

    // Display Control
	buf[0] = 0xFC;
	ret = mcde_dsi_dcs_write(main_display.port, 0xB1, buf, 1);

    // Source EQ
	buf[0] = 0x01;
	buf[1] = 0x02;
	buf[2] = 0x02;
	buf[3] = 0x02;
	ret = mcde_dsi_dcs_write(main_display.port, 0xB8, buf, 4);

    // Z Inversion
	buf[0] = 0x05;
	buf[1] = 0x05;
	buf[2] = 0x05;
	ret = mcde_dsi_dcs_write(main_display.port, 0xBC, buf, 3);
            
	//send the gamma values
	//manufacture command set enable and page 1
	buf[0] = 0x55;
	buf[1] = 0xAA;
	buf[2] = 0x52;
	buf[3] = 0x08;
	buf[4] = 0x01;
	ret = mcde_dsi_dcs_write(main_display.port, 0xF0, buf, 5);

    // AVDD: 6.0v
	buf[0] = 0x05;
	buf[1] = 0x05;
	buf[2] = 0x05;
	ret = mcde_dsi_dcs_write(main_display.port, 0xB0, buf, 3);

    // AVEE: -6.0v
	buf[0] = 0x05;
	buf[1] = 0x05;
	buf[2] = 0x05;
	ret = mcde_dsi_dcs_write(main_display.port, 0xB1, buf, 3);

    // AVDD: 2.5x VPNL
	buf[0] = 0x44;
	buf[1] = 0x44;
	buf[2] = 0x44;
	ret = mcde_dsi_dcs_write(main_display.port, 0xB6, buf, 3);

    // AVEE: -2.5x VPNL
	buf[0] = 0x34;
	buf[1] = 0x34;
	buf[2] = 0x34;
	ret = mcde_dsi_dcs_write(main_display.port, 0xB7, buf, 3);

    // VGLX: AVEE - AVDD + VCL
	buf[0] = 0x24;
	buf[1] = 0x24;
	buf[2] = 0x24;
	ret = mcde_dsi_dcs_write(main_display.port, 0xBA, buf, 3);

    // VGMP: 4.7V,VGSP=0V
	buf[0] = 0x00;
	buf[1] = 0x88;
	buf[2] = 0x00;
	ret = mcde_dsi_dcs_write(main_display.port, 0xBC, buf, 3);

    // VGMN: -4.7V,VGSN=-0V
	buf[0] = 0x00;
	buf[1] = 0x88;
	buf[2] = 0x00;
	ret = mcde_dsi_dcs_write(main_display.port, 0xBD, buf, 3);

    // VCOM
	buf[0] = 0x4F;
	ret = mcde_dsi_dcs_write(main_display.port, 0xBE, buf, 1);
            
	//red positive node
	//D1
	buf[0]=0x00;
	buf[1]=0x29;
	buf[2]=0x00;
	buf[3]=0x32;
	buf[4]=0x00;
	buf[5]=0x46;
	buf[6]=0x00;
	buf[7]=0x59;
	buf[8]=0x00;
	buf[9]=0x69;
	buf[10]=0x00;
	buf[11]=0x86;
	buf[12]=0x00;
	buf[13]=0x9C;
	buf[14]=0x00;
	buf[15]=0xC3;
	//D2
	buf[16]=0x00;
	buf[17]=0xE6;
	buf[18]=0x01;
	buf[19]=0x1B;
	buf[20]=0x01;
	buf[21]=0x43;
	buf[22]=0x01;
	buf[23]=0x83;
	buf[24]=0x01;
	buf[25]=0xB2;
	buf[26]=0x01;
	buf[27]=0xB3;
	buf[28]=0x01;
	buf[29]=0xDE;
	buf[30]=0x02;
	buf[31]=0x07;
	//D3
	buf[32]=0x02;
	buf[33]=0x20;
	buf[34]=0x02;
	buf[35]=0x3F;
	buf[36]=0x02;
	buf[37]=0x53;
	buf[38]=0x02;
	buf[39]=0x78;
	buf[40]=0x02;
	buf[41]=0x95;
	buf[42]=0x02;
	buf[43]=0xC6;
	buf[44]=0x02;
	buf[45]=0xEE;
	buf[46]=0x03;
	buf[47]=0x30;
	//D4
	buf[48]=0x03;
	buf[49]=0xF0;
	buf[50]=0x03;
	buf[51]=0xF4;
	ret = mcde_dsi_dcs_write(main_display.port, 0xD1, buf, 16);
	ret = mcde_dsi_dcs_write(main_display.port, 0xD2, &(buf[16]), 16);
	ret = mcde_dsi_dcs_write(main_display.port, 0xD3, &(buf[32]), 16);
	ret = mcde_dsi_dcs_write(main_display.port, 0xD4, &(buf[48]), 4);

	//green positive node
	//D5
	buf[0]=0x00;
	buf[1]=0x29;
	buf[2]=0x00;
	buf[3]=0x32;
	buf[4]=0x00;
	buf[5]=0x46;
	buf[6]=0x00;
	buf[7]=0x59;
	buf[8]=0x00;
	buf[9]=0x69;
	buf[10]=0x00;
	buf[11]=0x86;
	buf[12]=0x00;
	buf[13]=0x9C;
	buf[14]=0x00;
	buf[15]=0xC3;
	//D6
	buf[16]=0x00;
	buf[17]=0xE6;
	buf[18]=0x01;
	buf[19]=0x1B;
	buf[20]=0x01;
	buf[21]=0x43;
	buf[22]=0x01;
	buf[23]=0x83;
	buf[24]=0x01;
	buf[25]=0xB2;
	buf[26]=0x01;
	buf[27]=0xB3;
	buf[28]=0x01;
	buf[29]=0xDE;
	buf[30]=0x02;
	buf[31]=0x07;
	//D7
	buf[32]=0x02;
	buf[33]=0x20;
	buf[34]=0x02;
	buf[35]=0x3F;
	buf[36]=0x02;
	buf[37]=0x53;
	buf[38]=0x02;
	buf[39]=0x78;
	buf[40]=0x02;
	buf[41]=0x95;
	buf[42]=0x02;
	buf[43]=0xC6;
	buf[44]=0x02;
	buf[45]=0xEE;
	buf[46]=0x03;
	buf[47]=0x30;
	//D8
	buf[48]=0x03;
	buf[49]=0xF0;
	buf[50]=0x03;
	buf[51]=0xF4;
	ret = mcde_dsi_dcs_write(main_display.port, 0xD5, buf, 16);
	ret = mcde_dsi_dcs_write(main_display.port, 0xD6, &(buf[16]), 16);
	ret = mcde_dsi_dcs_write(main_display.port, 0xD7, &(buf[32]), 16);
	ret = mcde_dsi_dcs_write(main_display.port, 0xD8, &(buf[48]), 4);

	//blue positive node
	//D9
	buf[0]=0x00;
	buf[1]=0x29;
	buf[2]=0x00;
	buf[3]=0x32;
	buf[4]=0x00;
	buf[5]=0x46;
	buf[6]=0x00;
	buf[7]=0x59;
	buf[8]=0x00;
	buf[9]=0x69;
	buf[10]=0x00;
	buf[11]=0x86;
	buf[12]=0x00;
	buf[13]=0x9C;
	buf[14]=0x00;
	buf[15]=0xC3;
	//DD
	buf[16]=0x00;
	buf[17]=0xE6;
	buf[18]=0x01;
	buf[19]=0x1B;
	buf[20]=0x01;
	buf[21]=0x43;
	buf[22]=0x01;
	buf[23]=0x83;
	buf[24]=0x01;
	buf[25]=0xB2;
	buf[26]=0x01;
	buf[27]=0xB3;
	buf[28]=0x01;
	buf[29]=0xDE;
	buf[30]=0x02;
	buf[31]=0x07;
	//DE
	buf[32]=0x02;
	buf[33]=0x20;
	buf[34]=0x02;
	buf[35]=0x3F;
	buf[36]=0x02;
	buf[37]=0x53;
	buf[38]=0x02;
	buf[39]=0x78;
	buf[40]=0x02;
	buf[41]=0x95;
	buf[42]=0x02;
	buf[43]=0xC6;
	buf[44]=0x02;
	buf[45]=0xEE;
	buf[46]=0x03;
	buf[47]=0x30;
	//DF
	buf[48]=0x03;
	buf[49]=0xF0;
	buf[50]=0x03;
	buf[51]=0xF4;
	ret = mcde_dsi_dcs_write(main_display.port, 0xD9, buf, 16);
	ret = mcde_dsi_dcs_write(main_display.port, 0xDD, &(buf[16]), 16);
	ret = mcde_dsi_dcs_write(main_display.port, 0xDE, &(buf[32]), 16);
	ret = mcde_dsi_dcs_write(main_display.port, 0xDF, &(buf[48]), 4);

	//red negative node
	//E0
	buf[0]=0x00;
	buf[1]=0x29;
	buf[2]=0x00;
	buf[3]=0x32;
	buf[4]=0x00;
	buf[5]=0x46;
	buf[6]=0x00;
	buf[7]=0x59;
	buf[8]=0x00;
	buf[9]=0x69;
	buf[10]=0x00;
	buf[11]=0x86;
	buf[12]=0x00;
	buf[13]=0x9C;
	buf[14]=0x00;
	buf[15]=0xC3;
	//E1
	buf[16]=0x00;
	buf[17]=0xE6;
	buf[18]=0x01;
	buf[19]=0x1B;
	buf[20]=0x01;
	buf[21]=0x43;
	buf[22]=0x01;
	buf[23]=0x83;
	buf[24]=0x01;
	buf[25]=0xB2;
	buf[26]=0x01;
	buf[27]=0xB3;
	buf[28]=0x01;
	buf[29]=0xDE;
	buf[30]=0x02;
	buf[31]=0x07;
	//E2
	buf[32]=0x02;
	buf[33]=0x20;
	buf[34]=0x02;
	buf[35]=0x3F;
	buf[36]=0x02;
	buf[37]=0x53;
	buf[38]=0x02;
	buf[39]=0x78;
	buf[40]=0x02;
	buf[41]=0x95;
	buf[42]=0x02;
	buf[43]=0xC6;
	buf[44]=0x02;
	buf[45]=0xEE;
	buf[46]=0x03;
	buf[47]=0x30;
	//E3
	buf[48]=0x03;
	buf[49]=0xF0;
	buf[50]=0x03;
	buf[51]=0xF4;
	ret = mcde_dsi_dcs_write(main_display.port, 0xE0, buf, 16);
	ret = mcde_dsi_dcs_write(main_display.port, 0xE1, &(buf[16]), 16);
	ret = mcde_dsi_dcs_write(main_display.port, 0xE2, &(buf[32]), 16);
	ret = mcde_dsi_dcs_write(main_display.port, 0xE3, &(buf[48]), 4);

	//green negative node
	//E4
	buf[0]=0x00;
	buf[1]=0x29;
	buf[2]=0x00;
	buf[3]=0x32;
	buf[4]=0x00;
	buf[5]=0x46;
	buf[6]=0x00;
	buf[7]=0x59;
	buf[8]=0x00;
	buf[9]=0x69;
	buf[10]=0x00;
	buf[11]=0x86;
	buf[12]=0x00;
	buf[13]=0x9C;
	buf[14]=0x00;
	buf[15]=0xC3;
	//E5
	buf[16]=0x00;
	buf[17]=0xE6;
	buf[18]=0x01;
	buf[19]=0x1B;
	buf[20]=0x01;
	buf[21]=0x43;
	buf[22]=0x01;
	buf[23]=0x83;
	buf[24]=0x01;
	buf[25]=0xB2;
	buf[26]=0x01;
	buf[27]=0xB3;
	buf[28]=0x01;
	buf[29]=0xDE;
	buf[30]=0x02;
	buf[31]=0x07;
	//E6
	buf[32]=0x02;
	buf[33]=0x20;
	buf[34]=0x02;
	buf[35]=0x3F;
	buf[36]=0x02;
	buf[37]=0x53;
	buf[38]=0x02;
	buf[39]=0x78;
	buf[40]=0x02;
	buf[41]=0x95;
	buf[42]=0x02;
	buf[43]=0xC6;
	buf[44]=0x02;
	buf[45]=0xEE;
	buf[46]=0x03;
	buf[47]=0x30;
	//E7
	buf[48]=0x03;
	buf[49]=0xF0;
	buf[50]=0x03;
	buf[51]=0xF4;
	ret = mcde_dsi_dcs_write(main_display.port, 0xE4, buf, 16);
	ret = mcde_dsi_dcs_write(main_display.port, 0xE5, &(buf[16]), 16);
	ret = mcde_dsi_dcs_write(main_display.port, 0xE6, &(buf[32]), 16);
	ret = mcde_dsi_dcs_write(main_display.port, 0xE7, &(buf[48]), 4);


	//blue negative node
	//E8
	buf[0]=0x00;
	buf[1]=0x29;
	buf[2]=0x00;
	buf[3]=0x32;
	buf[4]=0x00;
	buf[5]=0x46;
	buf[6]=0x00;
	buf[7]=0x59;
	buf[8]=0x00;
	buf[9]=0x69;
	buf[10]=0x00;
	buf[11]=0x86;
	buf[12]=0x00;
	buf[13]=0x9C;
	buf[14]=0x00;
	buf[15]=0xC3;
	//E9
	buf[16]=0x00;
	buf[17]=0xE6;
	buf[18]=0x01;
	buf[19]=0x1B;
	buf[20]=0x01;
	buf[21]=0x43;
	buf[22]=0x01;
	buf[23]=0x83;
	buf[24]=0x01;
	buf[25]=0xB2;
	buf[26]=0x01;
	buf[27]=0xB3;
	buf[28]=0x01;
	buf[29]=0xDE;
	buf[30]=0x02;
	buf[31]=0x07;
	//EA
	buf[32]=0x02;
	buf[33]=0x20;
	buf[34]=0x02;
	buf[35]=0x3F;
	buf[36]=0x02;
	buf[37]=0x53;
	buf[38]=0x02;
	buf[39]=0x78;
	buf[40]=0x02;
	buf[41]=0x95;
	buf[42]=0x02;
	buf[43]=0xC6;
	buf[44]=0x02;
	buf[45]=0xEE;
	buf[46]=0x03;
	buf[47]=0x30;
	//EB
	buf[48]=0x03;
	buf[49]=0xF0;
	buf[50]=0x03;
	buf[51]=0xF4;
	ret = mcde_dsi_dcs_write(main_display.port, 0xE8, buf, 16);	
	ret = mcde_dsi_dcs_write(main_display.port, 0xE9, &(buf[16]), 16);
	ret = mcde_dsi_dcs_write(main_display.port, 0xEA, &(buf[32]), 16);
	ret = mcde_dsi_dcs_write(main_display.port, 0xEB, &(buf[48]), 4);

    buf[0]=0x02;
    ret = mcde_dsi_dcs_write(main_display.port, 0x36, buf, 1);

	//set lcd sleep out
	ret = mcde_dsi_dcs_write(main_display.port, 0x11, NULL, 0);
    
	mdelay(120);
    
	//set display on in background
    ret = mcde_dsi_dcs_write(main_display.port, 0x22, NULL, 0);
    ret = mcde_dsi_dcs_write(main_display.port, 0x29, NULL, 0);    
    ret = mcde_dsi_dcs_write(main_display.port, 0x13, NULL, 0);

    return ret;
}

int mcde_turn_on_sharp_dsi(void)
{
	int ret = 0;
	u8 buf[60] = {0};
	
	//for sharp 4.3 lcd with nt35516 driver IC
	ret = mcde_dsi_dcs_write(main_display.port, 0x11, NULL, 0);//Sleep Out 
	mdelay(120);//120ms

	//manufacture command set enable and page 1
	buf[0] = 0x55;
	buf[1] = 0xAA;
	buf[2] = 0x52;
	buf[3] = 0x08;
	buf[4] = 0x01;
	ret = mcde_dsi_dcs_write(main_display.port, 0xF0, buf, 5);

	//red positive node
	//D1
	buf[0]=0x00;
	buf[1]=0x03;
	buf[2]=0x00;
	buf[3]=0x0E;
	buf[4]=0x00;
	buf[5]=0x22;
	buf[6]=0x00;
	buf[7]=0x36;
	buf[8]=0x00;
	buf[9]=0x48;
	buf[10]=0x00;
	buf[11]=0x67;
	buf[12]=0x00;
	buf[13]=0x81;
	buf[14]=0x00;
	buf[15]=0xAC;
	//D2
	buf[16]=0x00;
	buf[17]=0xCD;
	buf[18]=0x00;
	buf[19]=0xFD;
	buf[20]=0x01;
	buf[21]=0x23;
	buf[22]=0x01;
	buf[23]=0x58;
	buf[24]=0x01;
	buf[25]=0x7D;
	buf[26]=0x01;
	buf[27]=0x7E;
	buf[28]=0x01;
	buf[29]=0x9E;
	buf[30]=0x01;
	buf[31]=0xBA;
	//D3
	buf[32]=0x01;
	buf[33]=0xCA;
	buf[34]=0x01;
	buf[35]=0xDE;
	buf[36]=0x01;
	buf[37]=0xEC;
	buf[38]=0x02;
	buf[39]=0x04;
	buf[40]=0x02;
	buf[41]=0x18;
	buf[42]=0x02;
	buf[43]=0x3D;
	buf[44]=0x02;
	buf[45]=0x6A;
	buf[46]=0x02;
	buf[47]=0x8C;
	//D4
	buf[48]=0x02;
	buf[49]=0x98;
	buf[50]=0x02;
	buf[51]=0x99;
	ret = mcde_dsi_dcs_write(main_display.port, 0xD1, buf, 16);
	ret = mcde_dsi_dcs_write(main_display.port, 0xD2, &(buf[16]), 16);
	ret = mcde_dsi_dcs_write(main_display.port, 0xD3, &(buf[32]), 16);
	ret = mcde_dsi_dcs_write(main_display.port, 0xD4, &(buf[48]), 4);

	//green positive node
	//D5
	buf[0]=0x00;
	buf[1]=0x5E;
	buf[2]=0x00;
	buf[3]=0x66;
	buf[4]=0x00;
	buf[5]=0x75;
	buf[6]=0x00;
	buf[7]=0x83;
	buf[8]=0x00;
	buf[9]=0x90;
	buf[10]=0x00;
	buf[11]=0xA7;
	buf[12]=0x00;
	buf[13]=0xBB;
	buf[14]=0x00;
	buf[15]=0xD9;
	//D6
	buf[16]=0x00;
	buf[17]=0xF1;
	buf[18]=0x01;
	buf[19]=0x1A;
	buf[20]=0x01;
	buf[21]=0x39;
	buf[22]=0x01;
	buf[23]=0x65;
	buf[24]=0x01;
	buf[25]=0x86;
	buf[26]=0x01;
	buf[27]=0x87;
	buf[28]=0x01;
	buf[29]=0xA4;
	buf[30]=0x01;
	buf[31]=0xBE;
	//D7
	buf[32]=0x01;
	buf[33]=0xCD;
	buf[34]=0x01;
	buf[35]=0xE1;
	buf[36]=0x01;
	buf[37]=0xEE;
	buf[38]=0x02;
	buf[39]=0x06;
	buf[40]=0x02;
	buf[41]=0x1A;
	buf[42]=0x02;
	buf[43]=0x3F;
	buf[44]=0x02;
	buf[45]=0x59;
	buf[46]=0x02;
	buf[47]=0x82;
	//D8
	buf[48]=0x02;
	buf[49]=0x98;
	buf[50]=0x02;
	buf[51]=0x99;
	ret = mcde_dsi_dcs_write(main_display.port, 0xD5, buf, 16);
	ret = mcde_dsi_dcs_write(main_display.port, 0xD6, &(buf[16]), 16);
	ret = mcde_dsi_dcs_write(main_display.port, 0xD7, &(buf[32]), 16);
	ret = mcde_dsi_dcs_write(main_display.port, 0xD8, &(buf[48]), 4);


	//blue positive node
	//D9
	buf[0]=0x00;
	buf[1]=0x84;
	buf[2]=0x00;
	buf[3]=0x8C;
	buf[4]=0x00;
	buf[5]=0x9B;
	buf[6]=0x00;
	buf[7]=0xA8;
	buf[8]=0x00;
	buf[9]=0xB4;
	buf[10]=0x00;
	buf[11]=0xC9;
	buf[12]=0x00;
	buf[13]=0xDA;
	buf[14]=0x00;
	buf[15]=0xF5;
	//DD
	buf[16]=0x01;
	buf[17]=0x0C;
	buf[18]=0x01;
	buf[19]=0x30;
	buf[20]=0x01;
	buf[21]=0x4A;
	buf[22]=0x01;
	buf[23]=0x72;
	buf[24]=0x01;
	buf[25]=0x90;
	buf[26]=0x01;
	buf[27]=0x91;
	buf[28]=0x01;
	buf[29]=0xAA;
	buf[30]=0x01;
	buf[31]=0xC3;
	//DE
	buf[32]=0x01;
	buf[33]=0xD2;
	buf[34]=0x01;
	buf[35]=0xE5;
	buf[36]=0x01;
	buf[37]=0xF3;
	buf[38]=0x02;
	buf[39]=0x0B;
	buf[40]=0x02;
	buf[41]=0x1A;
	buf[42]=0x02;
	buf[43]=0x95;
	buf[44]=0x02;
	buf[45]=0x98;
	buf[46]=0x02;
	buf[47]=0x99;
	//DF
	buf[48]=0x02;
	buf[49]=0x99;
	buf[50]=0x02;
	buf[51]=0x99;
	ret = mcde_dsi_dcs_write(main_display.port, 0xD9, buf, 16);
	ret = mcde_dsi_dcs_write(main_display.port, 0xDD, &(buf[16]), 16);
	ret = mcde_dsi_dcs_write(main_display.port, 0xDE, &(buf[32]), 16);
	ret = mcde_dsi_dcs_write(main_display.port, 0xDF, &(buf[48]), 4);


	//red negative node
	//E0
	buf[0]=0x00;
	buf[1]=0x01;
	buf[2]=0x00;
	buf[3]=0x0F;
	buf[4]=0x00;
	buf[5]=0x2A;
	buf[6]=0x00;
	buf[7]=0x43;
	buf[8]=0x00;
	buf[9]=0x5B;
	buf[10]=0x00;
	buf[11]=0x84;
	buf[12]=0x00;
	buf[13]=0xA8;
	buf[14]=0x00;
	buf[15]=0xE1;
	//E1
	buf[16]=0x01;
	buf[17]=0x0F;
	buf[18]=0x01;
	buf[19]=0x53;
	buf[20]=0x01;
	buf[21]=0x8A;
	buf[22]=0x01;
	buf[23]=0xDB;
	buf[24]=0x02;
	buf[25]=0x16;
	buf[26]=0x02;
	buf[27]=0x18;
	buf[28]=0x02;
	buf[29]=0x50;
	buf[30]=0x02;
	buf[31]=0x88;
	//E2
	buf[32]=0x02;
	buf[33]=0xAA;
	buf[34]=0x02;
	buf[35]=0xD8;
	buf[36]=0x02;
	buf[37]=0xFA;
	buf[38]=0x03;
	buf[39]=0x2E;
	buf[40]=0x03;
	buf[41]=0x4E;
	buf[42]=0x03;
	buf[43]=0x7D;
	buf[44]=0x03;
	buf[45]=0xB0;
	buf[46]=0x03;
	buf[47]=0xDB;
	//D4
	buf[48]=0x03;
	buf[49]=0xEC;
	buf[50]=0x03;
	buf[51]=0xEE;
	ret = mcde_dsi_dcs_write(main_display.port, 0xE0, buf, 16);
	ret = mcde_dsi_dcs_write(main_display.port, 0xE1, &(buf[16]), 16);
	ret = mcde_dsi_dcs_write(main_display.port, 0xE2, &(buf[32]), 16);
	ret = mcde_dsi_dcs_write(main_display.port, 0xE3, &(buf[48]), 4);

	//green negative node
	//E4
	buf[0]=0x00;
	buf[1]=0x78;
	buf[2]=0x00;
	buf[3]=0x83;
	buf[4]=0x00;
	buf[5]=0x97;
	buf[6]=0x00;
	buf[7]=0xAA;
	buf[8]=0x00;
	buf[9]=0xBB;
	buf[10]=0x00;
	buf[11]=0xDB;
	buf[12]=0x00;
	buf[13]=0xF6;
	buf[14]=0x01;
	buf[15]=0x20;
	//E5
	buf[16]=0x01;
	buf[17]=0x42;
	buf[18]=0x01;
	buf[19]=0x7D;
	buf[20]=0x01;
	buf[21]=0xAB;
	buf[22]=0x01;
	buf[23]=0xEF;
	buf[24]=0x02;
	buf[25]=0x26;
	buf[26]=0x02;
	buf[27]=0x27;
	buf[28]=0x02;
	buf[29]=0x5B;
	buf[30]=0x02;
	buf[31]=0x90;
	//E6
	buf[32]=0x02;
	buf[33]=0xB1;
	buf[34]=0x02;
	buf[35]=0xDF;
	buf[36]=0x03;
	buf[37]=0x02;
	buf[38]=0x03;
	buf[39]=0x32;
	buf[40]=0x03;
	buf[41]=0x51;
	buf[42]=0x03;
	buf[43]=0x7F;
	buf[44]=0x03;
	buf[45]=0x9E;
	buf[46]=0x03;
	buf[47]=0xCD;
	//E7
	buf[48]=0x03;
	buf[49]=0xEC;
	buf[50]=0x03;
	buf[51]=0xEE;
	ret = mcde_dsi_dcs_write(main_display.port, 0xE4, buf, 16);
	ret = mcde_dsi_dcs_write(main_display.port, 0xE5, &(buf[16]), 16);
	ret = mcde_dsi_dcs_write(main_display.port, 0xE6, &(buf[32]), 16);
	ret = mcde_dsi_dcs_write(main_display.port, 0xE7, &(buf[48]), 4);


	//blue negative node
	//E8
	buf[0]=0x00;
	buf[1]=0xAC;
	buf[2]=0x00;
	buf[3]=0xB6;
	buf[4]=0x00;
	buf[5]=0xCB;
	buf[6]=0x00;
	buf[7]=0xDD;
	buf[8]=0x00;
	buf[9]=0xEE;
	buf[10]=0x01;
	buf[11]=0x0A;
	buf[12]=0x01;
	buf[13]=0x21;
	buf[14]=0x01;
	buf[15]=0x48;
	//E9
	buf[16]=0x01;
	buf[17]=0x69;
	buf[18]=0x01;
	buf[19]=0x9E;
	buf[20]=0x01;
	buf[21]=0xC5;
	buf[22]=0x02;
	buf[23]=0x03;
	buf[24]=0x02;
	buf[25]=0x37;
	buf[26]=0x02;
	buf[27]=0x39;
	buf[28]=0x02;
	buf[29]=0x69;
	buf[30]=0x02;
	buf[31]=0x9C;
	//EA
	buf[32]=0x02;
	buf[33]=0xBD;
	buf[34]=0x02;
	buf[35]=0xE9;
	buf[36]=0x03;
	buf[37]=0x0D;
	buf[38]=0x03;
	buf[39]=0x3A;
	buf[40]=0x03;
	buf[41]=0x51;
	buf[42]=0x03;
	buf[43]=0xE8;
	buf[44]=0x03;
	buf[45]=0xEB;
	buf[46]=0x03;
	buf[47]=0xED;
	//EB
	buf[48]=0x03;
	buf[49]=0xED;
	buf[50]=0x03;
	buf[51]=0xEE;
	ret = mcde_dsi_dcs_write(main_display.port, 0xE8, buf, 16);
	ret = mcde_dsi_dcs_write(main_display.port, 0xE9, &(buf[16]), 16);
	ret = mcde_dsi_dcs_write(main_display.port, 0xEA, &(buf[32]), 16);
	ret = mcde_dsi_dcs_write(main_display.port, 0xEB, &(buf[48]), 4);

	//manufacture command set disable and page 0
	buf[0]=0x55;
	buf[1]=0xAA;
	buf[2]=0x52;
	buf[3]=0x00;
	buf[4]=0x00;
	ret = mcde_dsi_dcs_write(main_display.port, 0xF0, buf, 5);

	buf[0]=0x02;
	ret = mcde_dsi_dcs_write(main_display.port, 0x36, buf, 1);

	//display on
	ret = mcde_dsi_dcs_write(main_display.port, 0x29, NULL, 0);//Display On 
	return ret;
}

int mcde_turn_on_boe_dsi(void)
{
    int ret = 0;
   	u8 buf[60] = {0};

    //0xFF00
    buf[0] = 0x96;
    buf[1] = 0x08;
    buf[2] = 0x01;

    //0xFF80
    buf[3] = 0x96;
    buf[4] = 0x08;

    //0xA000
    buf[5] = 0x00;

    //0xB380
    buf[6] = 0x00;
    buf[7] = 0x00;
    buf[8] = 0x00;
    buf[9] = 0x21;
    buf[10] = 0x00;

    //0xB392
    buf[11] = 0x01;    

    //0xB3C0
    buf[12] = 0x19;

    //0xC080
    buf[13] = 0x00;
    buf[14] = 0x48;
    buf[15] = 0x00;
    buf[16] = 0x10;
    buf[17] = 0x10;
    buf[18] = 0x00;  
    buf[19] = 0x48;
    buf[20] = 0x10;
    buf[21] = 0x10;

    //0xC092
    buf[22] = 0x00;
    buf[23] = 0x17;
    buf[24] = 0x00;
    buf[25] = 0x1A;

    //0xC0A2
    buf[26] = 0x01;
    buf[27] = 0x10;
    buf[28] = 0x00;

    //0xC0B3
    buf[29] = 0x00;  
    buf[30] = 0x50;

    //0xC181
    buf[31] = 0x55;

    //0xC480
    buf[32] = 0x01; 
    buf[33] = 0x00;
    buf[34] = 0xFA;

    //0xC4A0
    buf[35] = 0x33;
    buf[36] = 0x09;
    buf[37] = 0x90;
    buf[38] = 0x2B;
    buf[39] = 0x33;  
    buf[40] = 0x09;
    buf[41] = 0x90;
    buf[42] = 0x54;

    //0xC580
    buf[43] = 0x08;
    buf[44] = 0x00;
    buf[45] = 0x90;
    buf[46] = 0x11;

    //0xC590
    buf[47] = 0xD6;
    buf[48] = 0x0A;
    buf[49] = 0x00;
    buf[50] = 0x77;
    buf[51] = 0x13;  
    buf[52] = 0x33;
    buf[53] = 0x34;

    ret = mcde_dsi_dcs_write16(main_display.port, 0xFF00, buf, 3);         if (ret) return ret;
    ret = mcde_dsi_dcs_write16(main_display.port, 0xFF80, &(buf[3]), 2);   if (ret) return ret;
    ret = mcde_dsi_dcs_write16(main_display.port, 0xA000, &(buf[5]), 1);   if (ret) return ret;
    ret = mcde_dsi_dcs_write16(main_display.port, 0xB380, &(buf[6]), 5);   if (ret) return ret;
    ret = mcde_dsi_dcs_write16(main_display.port, 0xB392, &(buf[11]), 1);  if (ret) return ret;
    ret = mcde_dsi_dcs_write16(main_display.port, 0xB3C0, &(buf[12]), 1);  if (ret) return ret;
    ret = mcde_dsi_dcs_write16(main_display.port, 0xC080, &(buf[13]), 9);  if (ret) return ret;
    ret = mcde_dsi_dcs_write16(main_display.port, 0xC092, &(buf[22]), 4);  if (ret) return ret;
    ret = mcde_dsi_dcs_write16(main_display.port, 0xC0A2, &(buf[26]), 3);  if (ret) return ret;
    ret = mcde_dsi_dcs_write16(main_display.port, 0xC0B3, &(buf[29]), 2);  if (ret) return ret;
    ret = mcde_dsi_dcs_write16(main_display.port, 0xC181, &(buf[31]), 1);  if (ret) return ret;
    ret = mcde_dsi_dcs_write16(main_display.port, 0xC480, &(buf[32]), 3);  if (ret) return ret;
    ret = mcde_dsi_dcs_write16(main_display.port, 0xC4A0, &(buf[35]), 8);  if (ret) return ret;
    ret = mcde_dsi_dcs_write16(main_display.port, 0xC580, &(buf[43]), 4);  if (ret) return ret;
    ret = mcde_dsi_dcs_write16(main_display.port, 0xC590, &(buf[47]), 7);  if (ret) return ret;

    //0xC5A0
    buf[0] = 0xD6;
    buf[1] = 0x0A;
    buf[2] = 0x00;
    buf[3] = 0x77;
    buf[4] = 0x13;  
    buf[5] = 0x33;
    buf[6] = 0x34;

    //0xC5B0
    buf[7] = 0x04;
    buf[8] = 0xF8;

    //0xC680
    buf[9] = 0x64;

    //0xC6B0
    buf[10] = 0x03;
    buf[11] = 0x10;  
    buf[12] = 0x00;
    buf[13] = 0x1F;
    buf[14] = 0x12;

    //0xC0E1
    buf[15] = 0x9F;

    //0xD000
    buf[16] = 0x01;

    //0xD100
    buf[17] = 0x01;
    buf[18] = 0x01;

    //0xB0B7
    buf[19] = 0x10;

    //0xB0C0
    buf[20] = 0x55;

    //0xB0B1
    buf[21] = 0x03;
    buf[22] = 0x06;

    //0xCB80
    buf[23] = 0x00; 
    buf[24] = 0x00;
    buf[25] = 0x00;
    buf[26] = 0x00;
    buf[27] = 0x00;
    buf[28] = 0x00;
    buf[29] = 0x00;
    buf[30] = 0x00;
    buf[31] = 0x00;
    buf[32] = 0x00;

    //0xCB90
    buf[33] = 0x00;
    buf[34] = 0x00;
    buf[35] = 0x00;
    buf[36] = 0x00;
    buf[37] = 0x00;
    buf[38] = 0x00;
    buf[39] = 0x00;
    buf[40] = 0x00;
    buf[41] = 0x00;
    buf[42] = 0x00;
    buf[43] = 0x00;
    buf[44] = 0x00;
    buf[45] = 0x00;
    buf[46] = 0x00;
    buf[47] = 0x00;

    ret = mcde_dsi_dcs_write16(main_display.port, 0xC5A0, buf, 7);         if (ret) return ret;
    ret = mcde_dsi_dcs_write16(main_display.port, 0xC5B0, &(buf[7]), 2);   if (ret) return ret;
    ret = mcde_dsi_dcs_write16(main_display.port, 0xC680, &(buf[9]), 1);   if (ret) return ret;
    ret = mcde_dsi_dcs_write16(main_display.port, 0xC6B0, &(buf[10]), 5);  if (ret) return ret;
    ret = mcde_dsi_dcs_write16(main_display.port, 0xC0E1, &(buf[15]), 1);  if (ret) return ret;
    ret = mcde_dsi_dcs_write16(main_display.port, 0xD000, &(buf[16]), 1);  if (ret) return ret;
    ret = mcde_dsi_dcs_write16(main_display.port, 0xD100, &(buf[17]), 2);  if (ret) return ret;
    ret = mcde_dsi_dcs_write16(main_display.port, 0xB0B7, &(buf[19]), 1);  if (ret) return ret;
    ret = mcde_dsi_dcs_write16(main_display.port, 0xB0C0, &(buf[20]), 1);  if (ret) return ret;
    ret = mcde_dsi_dcs_write16(main_display.port, 0xB0B1, &(buf[21]), 2);  if (ret) return ret;
    ret = mcde_dsi_dcs_write16(main_display.port, 0xCB80, &(buf[23]), 10); if (ret) return ret;
    ret = mcde_dsi_dcs_write16(main_display.port, 0xCB90, &(buf[33]), 15); if (ret) return ret;

    //0xCBA0
    buf[0] = 0x00;
    buf[1] = 0x00;
    buf[2] = 0x00;
    buf[3] = 0x00;
    buf[4] = 0x00;
    buf[5] = 0x00;
    buf[6] = 0x00;
    buf[7] = 0x00;
    buf[8] = 0x00;
    buf[9] = 0x00;
    buf[10] = 0x00;
    buf[11] = 0x00;
    buf[12] = 0x00;
    buf[13] = 0x00;
    buf[14] = 0x00;

    //0xCBB0
    buf[15] = 0x00;
    buf[16] = 0x00;
    buf[17] = 0x00;
    buf[18] = 0x00;
    buf[19] = 0x00;
    buf[20] = 0x00;
    buf[21] = 0x00;
    buf[22] = 0x00;
    buf[23] = 0x00;
    buf[24] = 0x00;

    //0xCBC0
    buf[25] = 0x00;
    buf[26] = 0x00;
    buf[27] = 0x04;
    buf[28] = 0x04;
    buf[29] = 0x04;
    buf[30] = 0x04;
    buf[31] = 0x04;
    buf[32] = 0x00;
    buf[33] = 0x00;
    buf[34] = 0x00;
    buf[35] = 0x00;
    buf[36] = 0x00;
    buf[37] = 0x00;
    buf[38] = 0x00;
    buf[39] = 0x00;

    //0xCBD0
    buf[40] = 0x00;
    buf[41] = 0x00;
    buf[42] = 0x00;
    buf[43] = 0x00;
    buf[44] = 0x00;
    buf[45] = 0x00;
    buf[46] = 0x00;
    buf[47] = 0x04;
    buf[48] = 0x04;
    buf[49] = 0x04;
    buf[50] = 0x04;
    buf[51] = 0x04;
    buf[52] = 0x00;
    buf[53] = 0x00;
    buf[54] = 0x00;

    ret = mcde_dsi_dcs_write16(main_display.port, 0xCBA0, buf, 15);        if (ret) return ret;
    ret = mcde_dsi_dcs_write16(main_display.port, 0xCBB0, &(buf[15]), 10); if (ret) return ret;
    ret = mcde_dsi_dcs_write16(main_display.port, 0xCBC0, &(buf[25]), 15); if (ret) return ret;
    ret = mcde_dsi_dcs_write16(main_display.port, 0xCBD0, &(buf[40]), 15); if (ret) return ret;

    //0xCBE0
    buf[0] = 0x00;
    buf[1] = 0x00;
    buf[2] = 0x00;
    buf[3] = 0x00;
    buf[4] = 0x00;
    buf[5] = 0x00;
    buf[6] = 0x00;
    buf[7] = 0x00;
    buf[8] = 0x00;
    buf[9] = 0x00;

    //0xCBF0
    buf[10] = 0xFF;
    buf[11] = 0xFF;
    buf[12] = 0xFF;
    buf[13] = 0xFF;
    buf[14] = 0xFF;
    buf[15] = 0xFF;
    buf[16] = 0xFF;
    buf[17] = 0xFF;
    buf[18] = 0xFF;
    buf[19] = 0xFF;
    
    //0xCC80
    buf[20] = 0x00;
    buf[21] = 0x00;
    buf[22] = 0x09;
    buf[23] = 0x0B;
    buf[24] = 0x01;
    buf[25] = 0x25;
    buf[26] = 0x26;
    buf[27] = 0x00;
    buf[28] = 0x00;
    buf[29] = 0x00;

    //0xCC90
    buf[30] = 0x00;
    buf[31] = 0x00;
    buf[32] = 0x00;
    buf[33] = 0x00;
    buf[34] = 0x00;
    buf[35] = 0x00;
    buf[36] = 0x00;
    buf[37] = 0x00;
    buf[38] = 0x00;
    buf[39] = 0x00;
    buf[40] = 0x00;
    buf[41] = 0x00;
    buf[42] = 0x0A;
    buf[43] = 0x0C;
    buf[44] = 0x02;

    //0xCCA0
    buf[45] = 0x25;
    buf[46] = 0x26;
    buf[47] = 0x00;
    buf[48] = 0x00;
    buf[49] = 0x00;
    buf[50] = 0x00;
    buf[51] = 0x00;
    buf[52] = 0x00;
    buf[53] = 0x00;
    buf[54] = 0x00;
    buf[55] = 0x00;
    buf[56] = 0x00;
    buf[57] = 0x00;
    buf[58] = 0x00;
    buf[59] = 0x00;    

    ret = mcde_dsi_dcs_write16(main_display.port, 0xCBE0, buf, 10);        if (ret) return ret;
    ret = mcde_dsi_dcs_write16(main_display.port, 0xCBF0, &(buf[10]), 10); if (ret) return ret;
    ret = mcde_dsi_dcs_write16(main_display.port, 0xCC80, &(buf[20]), 10); if (ret) return ret;
    ret = mcde_dsi_dcs_write16(main_display.port, 0xCC90, &(buf[30]), 15); if (ret) return ret;
    ret = mcde_dsi_dcs_write16(main_display.port, 0xCCA0, &(buf[45]), 15); if (ret) return ret;

    //0xCCB0
    buf[0] = 0x00;
    buf[1] = 0x00;
    buf[2] = 0x0C;
    buf[3] = 0x0A;
    buf[4] = 0x02;
    buf[5] = 0x26;
    buf[6] = 0x25;
    buf[7] = 0x00;
    buf[8] = 0x00;
    buf[9] = 0x00;

    //0xCCC0
    buf[10] = 0x00;
    buf[11] = 0x00;
    buf[12] = 0x00;
    buf[13] = 0x00;
    buf[14] = 0x00;
    buf[15] = 0x00;
    buf[16] = 0x00;
    buf[17] = 0x00;
    buf[18] = 0x00;
    buf[19] = 0x00;
    buf[20] = 0x00;
    buf[21] = 0x00;
    buf[22] = 0x0B;
    buf[23] = 0x09;
    buf[24] = 0x01;

    //0XCCD0
    buf[25] = 0x26;
    buf[26] = 0x25;
    buf[27] = 0x00;
    buf[28] = 0x00;
    buf[29] = 0x00;
    buf[30] = 0x00;
    buf[31] = 0x00;
    buf[32] = 0x00;
    buf[33] = 0x00;
    buf[34] = 0x00;
    buf[35] = 0x00;
    buf[36] = 0x00;
    buf[37] = 0x00;
    buf[38] = 0x00;
    buf[39] = 0x00;

    //0xCE80
    buf[40] = 0x86;
    buf[41] = 0x01;
    buf[42] = 0x00;
    buf[43] = 0x85;
    buf[44] = 0x01;
    buf[45] = 0x00;
    buf[46] = 0x0F;
    buf[47] = 0x00;
    buf[48] = 0x00;
    buf[49] = 0x0F;
    buf[50] = 0x00;
    buf[51] = 0x00;

    ret = mcde_dsi_dcs_write16(main_display.port, 0xCCB0, buf, 10);        if (ret) return ret;
    ret = mcde_dsi_dcs_write16(main_display.port, 0xCCC0, &(buf[10]), 15); if (ret) return ret;
    ret = mcde_dsi_dcs_write16(main_display.port, 0XCCD0, &(buf[25]), 15); if (ret) return ret;
    ret = mcde_dsi_dcs_write16(main_display.port, 0xCE80, &(buf[40]), 12); if (ret) return ret;

    //0xCE90
    buf[0] = 0xF0;
    buf[1] = 0x00;
    buf[2] = 0x00;
    buf[3] = 0xF0;
    buf[4] = 0x00;
    buf[5] = 0x00;
    buf[6] = 0xF0;
    buf[7] = 0x00;
    buf[8] = 0x00;
    buf[9] = 0xF0;
    buf[10] = 0x00;
    buf[11] = 0x00;
    buf[12] = 0x00;
    buf[13] = 0x00;

    //0xCEA0
    buf[14] = 0x18;    
    buf[15] = 0x04;
    buf[16] = 0x03;
    buf[17] = 0xC3;
    buf[18] = 0x00;
    buf[19] = 0x00;
    buf[20] = 0x12;
    buf[21] = 0x18;
    buf[22] = 0x03;
    buf[23] = 0x03;
    buf[24] = 0xC4;
    buf[25] = 0x00;
    buf[26] = 0x00;
    buf[27] = 0x12;

    //0xCEB0
    buf[28] = 0x18;
    buf[29] = 0x06;    
    buf[30] = 0x03;
    buf[31] = 0xC1;
    buf[32] = 0x00;
    buf[33] = 0x00;
    buf[34] = 0x12;
    buf[35] = 0x18;
    buf[36] = 0x05;
    buf[37] = 0x03;
    buf[38] = 0xC2;
    buf[39] = 0x00;
    buf[40] = 0x00;
    buf[41] = 0x12;

    //0xCEC0
    buf[42] = 0xF0;
    buf[43] = 0x00;
    buf[44] = 0x00;    
    buf[45] = 0x10;
    buf[46] = 0x00;
    buf[47] = 0x00;
    buf[48] = 0x00;
    buf[49] = 0xF0;
    buf[50] = 0x00;
    buf[51] = 0x00;
    buf[52] = 0x10;
    buf[53] = 0x00;
    buf[54] = 0x00;
    buf[55] = 0x00;

    ret = mcde_dsi_dcs_write16(main_display.port, 0xCE90, buf, 14);        if (ret) return ret;
    ret = mcde_dsi_dcs_write16(main_display.port, 0xCEA0, &(buf[14]), 14); if (ret) return ret;
    ret = mcde_dsi_dcs_write16(main_display.port, 0xCEB0, &(buf[28]), 14); if (ret) return ret;
    ret = mcde_dsi_dcs_write16(main_display.port, 0xCEC0, &(buf[42]), 14); if (ret) return ret;

    //0xCED0
    buf[0] = 0xF0;
    buf[1] = 0x00;
    buf[2] = 0x00;
    buf[3] = 0x10;
    buf[4] = 0x00;
    buf[5] = 0x00;
    buf[6] = 0x00;
    buf[7] = 0xF0;
    buf[8] = 0x00;
    buf[9] = 0x00;
    buf[10] = 0x10;
    buf[11] = 0x00;
    buf[12] = 0x00;
    buf[13] = 0x00;

    //0xCF80
    buf[14] = 0xF0;
    buf[15] = 0x00;
    buf[16] = 0x00;
    buf[17] = 0x10;
    buf[18] = 0x00;
    buf[19] = 0x00;
    buf[20] = 0x00;
    buf[21] = 0xF0;
    buf[22] = 0x00;
    buf[23] = 0x00;
    buf[24] = 0x10;
    buf[25] = 0x00;
    buf[26] = 0x00;
    buf[27] = 0x00;

    //0xCF90
    buf[28] = 0xF0;
    buf[29] = 0x00;
    buf[30] = 0x00;
    buf[31] = 0x10;
    buf[32] = 0x00;
    buf[33] = 0x00;
    buf[34] = 0x00;
    buf[35] = 0xF0;
    buf[36] = 0x00;
    buf[37] = 0x00;
    buf[38] = 0x10;
    buf[39] = 0x00;
    buf[40] = 0x00;
    buf[41] = 0x00;

    //0xCFA0
    buf[42] = 0xF0;
    buf[43] = 0x00;
    buf[44] = 0x00;
    buf[45] = 0x10;
    buf[46] = 0x00;
    buf[47] = 0x00;
    buf[48] = 0x00;
    buf[49] = 0xF0;
    buf[50] = 0x00;
    buf[51] = 0x00;
    buf[52] = 0x10;
    buf[53] = 0x00;
    buf[54] = 0x00;
    buf[55] = 0x00;

    ret = mcde_dsi_dcs_write16(main_display.port, 0xCED0, buf, 14);        if (ret) return ret;
    ret = mcde_dsi_dcs_write16(main_display.port, 0xCF80, &(buf[14]), 14); if (ret) return ret;
    ret = mcde_dsi_dcs_write16(main_display.port, 0xCF90, &(buf[28]), 14); if (ret) return ret;
    ret = mcde_dsi_dcs_write16(main_display.port, 0xCFA0, &(buf[42]), 14); if (ret) return ret;

    //0xCFB0
    buf[0] = 0xF0;
    buf[1] = 0x00;
    buf[2] = 0x00;
    buf[3] = 0x10;
    buf[4] = 0x00;
    buf[5] = 0x00;
    buf[6] = 0x00;
    buf[7] = 0xF0;
    buf[8] = 0x00;
    buf[9] = 0x00;
    buf[10] = 0x10;
    buf[11] = 0x00;
    buf[12] = 0x00;
    buf[13] = 0x00;

    //0xCFC0
    buf[14] = 0x02;
    buf[15] = 0x02;
    buf[16] = 0x10;
    buf[17] = 0x10;
    buf[18] = 0x00;
    buf[19] = 0x00;
    buf[20] = 0x01;
    buf[21] = 0x04;
    buf[22] = 0x00;
    buf[23] = 0x00;

    //0xD680
    buf[24] = 0x00;

    //0xD700
    buf[25] = 0x00;

    //0xD800
    buf[26] = 0xA0;
    buf[27] = 0xA0;

    //0xD900
    buf[28] = 0x7F;

    //0x3600
    buf[29]=0x40;
    ret = mcde_dsi_dcs_write(main_display.port, 0x36, &(buf[29]), 1);
    
    //0x3A00
    buf[30] = 0x77;

    //0xE100
    buf[31] = 0x00;
    buf[32] = 0x08;
    buf[33] = 0x0E;
    buf[34] = 0x0D;
    buf[35] = 0x06;
    buf[36] = 0x10;
    buf[37] = 0x0B;
    buf[38] = 0x0A;
    
    //0xE108
    buf[39] = 0x03;
    buf[40] = 0x06;
    buf[41] = 0x0A;
    buf[42] = 0x05;
    buf[43] = 0x0F;
    buf[44] = 0x11;
    buf[45] = 0x0C;
    buf[46] = 0x00;

    ret = mcde_dsi_dcs_write16(main_display.port, 0xCFB0, buf, 14);        if (ret) return ret;
    ret = mcde_dsi_dcs_write16(main_display.port, 0xCFC0, &(buf[14]), 10); if (ret) return ret;
    ret = mcde_dsi_dcs_write16(main_display.port, 0xD680, &(buf[24]), 1);  if (ret) return ret;
    ret = mcde_dsi_dcs_write16(main_display.port, 0xD700, &(buf[25]), 1);  if (ret) return ret;
    ret = mcde_dsi_dcs_write16(main_display.port, 0xD800, &(buf[26]), 2);  if (ret) return ret;
    ret = mcde_dsi_dcs_write16(main_display.port, 0xD900, &(buf[28]), 1);  if (ret) return ret;
    ret = mcde_dsi_dcs_write16(main_display.port, 0x3A00, &(buf[30]), 1);  if (ret) return ret; 
    ret = mcde_dsi_dcs_write16(main_display.port, 0xE100, &(buf[31]), 8);  if (ret) return ret;
    ret = mcde_dsi_dcs_write16(main_display.port, 0xE108, &(buf[39]), 8);  if (ret) return ret;
    
    //0xE200
    buf[0] = 0x00;
    buf[1] = 0x09;
    buf[2] = 0x0D;
    buf[3] = 0x0D;
    buf[4] = 0x06;
    buf[5] = 0x10;
    buf[6] = 0x0B;
    buf[7] = 0x0A;

    //0xE208
    buf[8] = 0x03;
    buf[9] = 0x06;
    buf[10] = 0x09;
    buf[11] = 0x05;
    buf[12] = 0x0F;
    buf[13] = 0x12;
    buf[14] = 0x0C;
    buf[15] = 0x00;

    //0xFF02
    buf[16] = 0xFF;
    buf[17] = 0xFF;
    buf[18] = 0xFF;

    //0x3500
    buf[19] = 0x00;

    //0x4400
    buf[20] = 0x00;
    buf[21] = 0x00;
    
    ret = mcde_dsi_dcs_write16(main_display.port, 0xE200, buf, 8);        if (ret) return ret;
    ret = mcde_dsi_dcs_write16(main_display.port, 0xE208, &(buf[8]), 8);  if (ret) return ret;
    ret = mcde_dsi_dcs_write16(main_display.port, 0xFF02, &(buf[16]), 3); if (ret) return ret; 
    ret = mcde_dsi_dcs_write16(main_display.port, 0x3500, &(buf[19]), 1); if (ret) return ret;
    ret = mcde_dsi_dcs_write16(main_display.port, 0x4400, &(buf[20]), 2); if (ret) return ret;
    
    //set lcd sleep out
    ret = mcde_dsi_dcs_write(main_display.port, 0x11, NULL, 0);

    mdelay(120);

    //set display on in background
    ret = mcde_dsi_dcs_write(main_display.port, 0x22, NULL, 0);
    ret = mcde_dsi_dcs_write(main_display.port, 0x29, NULL, 0);    
    ret = mcde_dsi_dcs_write(main_display.port, 0x13, NULL, 0);

    return ret; 
}

int mcde_turn_on_truly_dsi(void)
{
    int ret = 0;
   	u8 buf[60] = {0};
            
    //0xFF
	buf[0] = 0xAA;
	buf[1] = 0x55;
	buf[2] = 0x25;
	buf[3] = 0x01;
	buf[4] = 0x01;
	ret = mcde_dsi_dcs_write(main_display.port, 0xFF, buf, 5);

    //0xF2
 	buf[0] = 0x00;
	buf[1] = 0x00;
	buf[2] = 0x4A;
	buf[3] = 0x0A;
	buf[4] = 0x08;
	buf[5] = 0x00;
	buf[6] = 0x00;
	buf[7] = 0x00;
	buf[8] = 0x00;
	buf[9] = 0x00;
	buf[10] = 0x00;
	buf[11] = 0x00;
	buf[12] = 0x00;
	buf[13] = 0x00;
	buf[14] = 0x00;
	buf[15] = 0x00;
	buf[16] = 0x00;
	buf[17] = 0x0B;
	buf[18] = 0x00;
	buf[19] = 0x00;
	buf[20] = 0x00;
	buf[21] = 0x00;
	buf[22] = 0x00;
	buf[23] = 0x00;
	buf[24] = 0x00;
	buf[25] = 0x00;
	buf[26] = 0x00;
	buf[27] = 0x00;
	buf[28] = 0x40;
	buf[29] = 0x01;
	buf[30] = 0x51;
	buf[31] = 0x00;
    buf[32] = 0x01;    
    buf[33] = 0x00;
    buf[34] = 0x01;
    ret = mcde_dsi_dcs_write(main_display.port, 0xF2, buf, 15);
    ret = mcde_dsi_dcs_write16(main_display.port, 0xF20F, &(buf[15]), 15);
    ret = mcde_dsi_dcs_write16(main_display.port, 0xF21E, &(buf[30]), 5);

    //0xF3
 	buf[0] = 0x02;
	buf[1] = 0x03;
	buf[2] = 0x07;
	buf[3] = 0x45;
	buf[4] = 0x88;
	buf[5] = 0xD1;
	buf[6] = 0x0D;
    ret = mcde_dsi_dcs_write(main_display.port, 0xF3, buf, 7);
    
    //0xF0
	buf[0] = 0x55;
	buf[1] = 0xAA;
	buf[2] = 0x52;
	buf[3] = 0x08;
	buf[4] = 0x00;
	ret = mcde_dsi_dcs_write(main_display.port, 0xF0, buf, 5);

    //0xB1
	buf[0] = 0xCC;
	buf[1] = 0x00;
	buf[2] = 0x00;
	ret = mcde_dsi_dcs_write(main_display.port, 0xB1, buf, 3);

    //0xB8
	buf[0] = 0x01;
	buf[1] = 0x02;
	buf[2] = 0x02;
    buf[3] = 0x02;
	ret = mcde_dsi_dcs_write(main_display.port, 0xB8, buf, 4);

    //0xC9
    buf[0] = (0x63);
    buf[1] = (0x06);
    buf[2] = (0x0D);
    buf[3] = (0x1A);
    buf[4] = (0x17);
    buf[5] = (0x00);
	ret = mcde_dsi_dcs_write(main_display.port, 0xC9, buf, 6);

    //(0xF0); 
    buf[0] = (0x55);
    buf[1] = (0xAA);
    buf[2] = (0x52);
    buf[3] = (0x08);
    buf[4] = (0x01);
    ret = mcde_dsi_dcs_write(main_display.port, 0xF0, buf, 5);
    
    //(0xB0); 
    buf[0] = (0x05);
    buf[1] = (0x05);
    buf[2] = (0x05);
    ret = mcde_dsi_dcs_write(main_display.port, 0xB0, buf, 3);
    
    //(0xB1); 
    buf[0] = (0x05);
    buf[1] = (0x05);
    buf[2] = (0x05);
    ret = mcde_dsi_dcs_write(main_display.port, 0xB1, buf, 3);
    
    //(0xB2); 
    buf[0] = (0x01);
    buf[1] = (0x01);
    buf[2] = (0x01);
    ret = mcde_dsi_dcs_write(main_display.port, 0xB2, buf, 3);
    
    //(0xB3); 
    buf[0] = (0x0E);
    buf[1] = (0x0E);
    buf[2] = (0x0E);
    ret = mcde_dsi_dcs_write(main_display.port, 0xB3, buf, 3);

    //(0xB4); 
    buf[0] = (0x08);
    buf[1] = (0x08);
    buf[2] = (0x08);
    ret = mcde_dsi_dcs_write(main_display.port, 0xB4, buf, 3);
    
    //(0xB6);
    buf[0] = (0x44);
    buf[1] = (0x44);
    buf[2] = (0x44);
    ret = mcde_dsi_dcs_write(main_display.port, 0xB6, buf, 3);
    
    //(0xB7);
    buf[0] = (0x34);
    buf[1] = (0x34);
    buf[2] = (0x34);
    ret = mcde_dsi_dcs_write(main_display.port, 0xB7, buf, 3);
    
    //(0xB8); 
    buf[0] = (0x10);
    buf[1] = (0x10);
    buf[2] = (0x10);
    ret = mcde_dsi_dcs_write(main_display.port, 0xB8, buf, 3);    
    
    //(0xB9); 
    buf[0] = (0x26);
    buf[1] = (0x26);
    buf[2] = (0x26);
    ret = mcde_dsi_dcs_write(main_display.port, 0xB9, buf, 3);
    
    //(0xBA); 
    buf[0] = (0x34);
    buf[1] = (0x34);
    buf[2] = (0x34);
    ret = mcde_dsi_dcs_write(main_display.port, 0xBA, buf, 3);
    
    //(0xBC); 
    buf[0] = (0x00);
    buf[1] = (0xC8);
    buf[2] = (0x00);
    ret = mcde_dsi_dcs_write(main_display.port, 0xBC, buf, 3);
    
    //(0xBD); 
    buf[0] = (0x00);
    buf[1] = (0xC8);
    buf[2] = (0x00);
    ret = mcde_dsi_dcs_write(main_display.port, 0xBD, buf, 3);

    //(0xBE);
    buf[0] = (0x76);
    ret = mcde_dsi_dcs_write(main_display.port, 0xBE, buf, 1);

    //(0xC0); 
    buf[0] = (0x04);
    buf[1] = (0x00);
    ret = mcde_dsi_dcs_write(main_display.port, 0xC0, buf, 2);

    //(0xCA); 
    buf[0] = (0x00);
    ret = mcde_dsi_dcs_write(main_display.port, 0xCA, buf, 1);

    //(0xD0); 
    buf[0] = (0x0A);
    buf[1] = (0x10);
    buf[2] = (0x0D);
    buf[3] = (0x0F);
    ret = mcde_dsi_dcs_write(main_display.port, 0xD0, buf, 4);

    //(0xD1); 
    buf[0] = (0x00);
    buf[1] = (0x70);
    buf[2] = (0x00);
    buf[3] = (0xCE);
    buf[4] = (0x00);
    buf[5] = (0xF7);
    buf[6] = (0x01);
    buf[7] = (0x10);
    buf[8] = (0x01);
    buf[9] = (0x21);
    buf[10] = (0x01);
    buf[11] = (0x44);
    buf[12] = (0x01);
    buf[13] = (0x62);
    buf[14] = (0x01);
    buf[15] = (0x8D);
    ret = mcde_dsi_dcs_write(main_display.port, 0xD1, buf, 16);

    //(0xD2);
    buf[0] = (0x01);
    buf[1] = (0xAF);
    buf[2] = (0x01);
    buf[3] = (0xE4);
    buf[4] = (0x02);
    buf[5] = (0x0C);
    buf[6] = (0x02);
    buf[7] = (0x4D);
    buf[8] = (0x02);
    buf[9] = (0x82);
    buf[10] = (0x02);
    buf[11] = (0x84);
    buf[12] = (0x02);
    buf[13] = (0xB8);
    buf[14] = (0x02);
    buf[15] = (0xF0);
    ret = mcde_dsi_dcs_write(main_display.port, 0xD2, buf, 16);
    
    //(0xD3); 
    buf[0] = (0x03);
    buf[1] = (0x14);
    buf[2] = (0x03);
    buf[3] = (0x42);
    buf[4] = (0x03);
    buf[5] = (0x5E);
    buf[6] = (0x03);
    buf[7] = (0x80);
    buf[8] = (0x03);
    buf[9] = (0x97);
    buf[10] = (0x03);
    buf[11] = (0xB0);
    buf[12] = (0x03);
    buf[13] = (0xC0);
    buf[14] = (0x03);
    buf[15] = (0xDF);
    ret = mcde_dsi_dcs_write(main_display.port, 0xD3, buf, 16);
    
    //(0xD4);
    buf[0] = (0x03);
    buf[1] = (0xFD);
    buf[2] = (0x03);
    buf[3] = (0xFF);
    ret = mcde_dsi_dcs_write(main_display.port, 0xD4, buf, 4);
    
    //(0xD5); 
    buf[0] = (0x00);
    buf[1] = (0x70);
    buf[2] = (0x00);
    buf[3] = (0xCE);
    buf[4] = (0x00);
    buf[5] = (0xF7);
    buf[6] = (0x01);
    buf[7] = (0x10);
    buf[8] = (0x01);
    buf[9] = (0x21);
    buf[10] = (0x01);
    buf[11] = (0x44);
    buf[12] = (0x01);
    buf[13] = (0x62);
    buf[14] = (0x01);
    buf[15] = (0x8D);
    ret = mcde_dsi_dcs_write(main_display.port, 0xD5, buf, 16);
    
    //(0xD6); 
    buf[0] = (0x01);
    buf[1] = (0xAF);
    buf[2] = (0x01);
    buf[3] = (0xE4);
    buf[4] = (0x02);
    buf[5] = (0x0C);
    buf[6] = (0x02);
    buf[7] = (0x4D);
    buf[8] = (0x02);
    buf[9] = (0x82);
    buf[10] = (0x02);
    buf[11] = (0x84);
    buf[12] = (0x02);
    buf[13] = (0xB8);
    buf[14] = (0x02);
    buf[15] = (0xF0);
    ret = mcde_dsi_dcs_write(main_display.port, 0xD6, buf, 16);
    
    //(0xD7); 
    buf[0] = (0x03);
    buf[1] = (0x14);
    buf[2] = (0x03);
    buf[3] = (0x42);
    buf[4] = (0x03);
    buf[5] = (0x5E);
    buf[6] = (0x03);
    buf[7] = (0x80);
    buf[8] = (0x03);
    buf[9] = (0x97);
    buf[10] = (0x03);
    buf[11] = (0xB0);
    buf[12] = (0x03);
    buf[13] = (0xC0);
    buf[14] = (0x03);
    buf[15] = (0xDF);
    ret = mcde_dsi_dcs_write(main_display.port, 0xD7, buf, 16);
    
    //(0xD8); 
    buf[0] = (0x03);
    buf[1] = (0xFD);
    buf[2] = (0x03);
    buf[3] = (0xFF);
    ret = mcde_dsi_dcs_write(main_display.port, 0xD8, buf, 4);
    
    //(0xD9); 
    buf[0] = (0x00);
    buf[1] = (0x70);
    buf[2] = (0x00);
    buf[3] = (0xCE);
    buf[4] = (0x00);
    buf[5] = (0xF7);
    buf[6] = (0x01);
    buf[7] = (0x10);
    buf[8] = (0x01);
    buf[9] = (0x21);
    buf[10] = (0x01);
    buf[11] = (0x44);
    buf[12] = (0x01);
    buf[13] = (0x62);
    buf[14] = (0x01);
    buf[15] = (0x8D);
    ret = mcde_dsi_dcs_write(main_display.port, 0xD9, buf, 16);
    
    //(0xDD);
    buf[0] = (0x01);
    buf[1] = (0xAF);
    buf[2] = (0x01);
    buf[3] = (0xE4);
    buf[4] = (0x02);
    buf[5] = (0x0C);
    buf[6] = (0x02);
    buf[7] = (0x4D);
    buf[8] = (0x02);
    buf[9] = (0x82);
    buf[10] = (0x02);
    buf[11] = (0x84);
    buf[12] = (0x02);
    buf[13] = (0xB8);
    buf[14] = (0x02);
    buf[15] = (0xF0);
    ret = mcde_dsi_dcs_write(main_display.port, 0xDD, buf, 16);
    
    //(0xDE); 
    buf[0] = (0x03);
    buf[1] = (0x14);
    buf[2] = (0x03);
    buf[3] = (0x42);
    buf[4] = (0x03);
    buf[5] = (0x5E);
    buf[6] = (0x03);
    buf[7] = (0x80);
    buf[8] = (0x03);
    buf[9] = (0x97);
    buf[10] = (0x03);
    buf[11] = (0xB0);
    buf[12] = (0x03);
    buf[13] = (0xC0);
    buf[14] = (0x03);
    buf[15] = (0xDF);
    ret = mcde_dsi_dcs_write(main_display.port, 0xDE, buf, 16);
    
    //(0xDF); 
    buf[0] = (0x03);
    buf[1] = (0xFD);
    buf[2] = (0x03);
    buf[3] = (0xFF);
    ret = mcde_dsi_dcs_write(main_display.port, 0xDF, buf, 4);

    
    //(0xE0); 
    buf[0] = (0x00);
    buf[1] = (0x70);
    buf[2] = (0x00);
    buf[3] = (0xCE);
    buf[4] = (0x00);
    buf[5] = (0xF7);
    buf[6] = (0x01);
    buf[7] = (0x10);
    buf[8] = (0x01);
    buf[9] = (0x21);
    buf[10] = (0x01);
    buf[11] = (0x44);
    buf[12] = (0x01);
    buf[13] = (0x62);
    buf[14] = (0x01);
    buf[15] = (0x8D);
    ret = mcde_dsi_dcs_write(main_display.port, 0xE0, buf, 16);

    //(0xE1); 
    buf[0] = (0x01);
    buf[1] = (0xAF);
    buf[2] = (0x01);
    buf[3] = (0xE4);
    buf[4] = (0x02);
    buf[5] = (0x0C);
    buf[6] = (0x02);
    buf[7] = (0x4D);
    buf[8] = (0x02);
    buf[9] = (0x82);
    buf[10] = (0x02);
    buf[11] = (0x84);
    buf[12] = (0x02);
    buf[13] = (0xB8);
    buf[14] = (0x02);
    buf[15] = (0xF0);
    ret = mcde_dsi_dcs_write(main_display.port, 0xE1, buf, 16);

    //(0xE2); 
    buf[0] = (0x03);
    buf[1] = (0x14);
    buf[2] = (0x03);
    buf[3] = (0x42);
    buf[4] = (0x03);
    buf[5] = (0x5E);
    buf[6] = (0x03);
    buf[7] = (0x80);
    buf[8] = (0x03);
    buf[9] = (0x97);
    buf[10] = (0x03);
    buf[11] = (0xB0);
    buf[12] = (0x03);
    buf[13] = (0xC0);
    buf[14] = (0x03);
    buf[15] = (0xDF);
    ret = mcde_dsi_dcs_write(main_display.port, 0xE2, buf, 16);

    //(0xE3);
    buf[0] = (0x03);
    buf[1] = (0xFD);
    buf[2] = (0x03);
    buf[3] = (0xFF);
    ret = mcde_dsi_dcs_write(main_display.port, 0xE3, buf, 4);

    //(0xE4); 
    buf[0] = (0x00);
    buf[1] = (0x70);
    buf[2] = (0x00);
    buf[3] = (0xCE);
    buf[4] = (0x00);
    buf[5] = (0xF7);
    buf[6] = (0x01);
    buf[7] = (0x10);
    buf[8] = (0x01);
    buf[9] = (0x21);
    buf[10] = (0x01);
    buf[11] = (0x44);
    buf[12] = (0x01);
    buf[13] = (0x62);
    buf[14] = (0x01);
    buf[15] = (0x8D);
    ret = mcde_dsi_dcs_write(main_display.port, 0xE4, buf, 16);

    //(0xE5);
    buf[0] = (0x01);
    buf[1] = (0xAF);
    buf[2] = (0x01);
    buf[3] = (0xE4);
    buf[4] = (0x02);
    buf[5] = (0x0C);
    buf[6] = (0x02);
    buf[7] = (0x4D);
    buf[8] = (0x02);
    buf[9] = (0x82);
    buf[10] = (0x02);
    buf[11] = (0x84);
    buf[12] = (0x02);
    buf[13] = (0xB8);
    buf[14] = (0x02);
    buf[15] = (0xF0);
    ret = mcde_dsi_dcs_write(main_display.port, 0xE5, buf, 16);

    //(0xE6); 
    buf[0] = (0x03);
    buf[1] = (0x14);
    buf[2] = (0x03);
    buf[3] = (0x42);
    buf[4] = (0x03);
    buf[5] = (0x5E);
    buf[6] = (0x03);
    buf[7] = (0x80);
    buf[8] = (0x03);
    buf[9] = (0x97);
    buf[10] = (0x03);
    buf[11] = (0xB0);
    buf[12] = (0x03);
    buf[13] = (0xC0);
    buf[14] = (0x03);
    buf[15] = (0xDF);
    ret = mcde_dsi_dcs_write(main_display.port, 0xE6, buf, 16);

    //(0xE7); 
    buf[0] = (0x03);
    buf[1] = (0xFD);
    buf[2] = (0x03);
    buf[3] = (0xFF);
    ret = mcde_dsi_dcs_write(main_display.port, 0xE7, buf, 4);

    //(0xE8); 
    buf[0] = (0x00);
    buf[1] = (0x70);
    buf[2] = (0x00);
    buf[3] = (0xCE);
    buf[4] = (0x00);
    buf[5] = (0xF7);
    buf[6] = (0x01);
    buf[7] = (0x10);
    buf[8] = (0x01);
    buf[9] = (0x21);
    buf[10] = (0x01);
    buf[11] = (0x44);
    buf[12] = (0x01);
    buf[13] = (0x62);
    buf[14] = (0x01);
    buf[15] = (0x8D);
    ret = mcde_dsi_dcs_write(main_display.port, 0xE8, buf, 16);

    //(0xE9); 
    buf[0] = (0x01);
    buf[1] = (0xAF);
    buf[2] = (0x01);
    buf[3] = (0xE4);
    buf[4] = (0x02);
    buf[5] = (0x0C);
    buf[6] = (0x02);
    buf[7] = (0x4D);
    buf[8] = (0x02);
    buf[9] = (0x82);
    buf[10] = (0x02);
    buf[11] = (0x84);
    buf[12] = (0x02);
    buf[13] = (0xB8);
    buf[14] = (0x02);
    buf[15] = (0xF0);
    ret = mcde_dsi_dcs_write(main_display.port, 0xE9, buf, 16);

    //(0xEA); 
    buf[0] = (0x03);
    buf[1] = (0x14);
    buf[2] = (0x03);
    buf[3] = (0x42);
    buf[4] = (0x03);
    buf[5] = (0x5E);
    buf[6] = (0x03);
    buf[7] = (0x80);
    buf[8] = (0x03);
    buf[9] = (0x97);
    buf[10] = (0x03);
    buf[11] = (0xB0);
    buf[12] = (0x03);
    buf[13] = (0xC0);
    buf[14] = (0x03);
    buf[15] = (0xDF);
    ret = mcde_dsi_dcs_write(main_display.port, 0xEA, buf, 16);

    //(0xEB); 
    buf[0] = (0x03);
    buf[1] = (0xFD);
    buf[2] = (0x03);
    buf[3] = (0xFF);
    ret = mcde_dsi_dcs_write(main_display.port, 0xEB, buf, 4);

    buf[0]=0x02;
    ret = mcde_dsi_dcs_write(main_display.port, 0x36, buf, 1);

    buf[0]=0x00;
    ret = mcde_dsi_dcs_write(main_display.port, 0x35, buf, 1);

	//set lcd sleep out
	ret = mcde_dsi_dcs_write(main_display.port, 0x11, NULL, 0);
    
	mdelay(120);
    
	//set display on in background
    ret = mcde_dsi_dcs_write(main_display.port, 0x22, NULL, 0);
    ret = mcde_dsi_dcs_write(main_display.port, 0x29, NULL, 0);    
    ret = mcde_dsi_dcs_write(main_display.port, 0x13, NULL, 0);

    return ret;
}

int mcde_turn_on_byd_dsi(void)
{
    int ret = 0;
   	u8 buf[60] = {0};
    
    //0xF2
 	buf[0] = 0x00;
	buf[1] = 0x00;
	buf[2] = 0x4A;
	buf[3] = 0x0A;
	buf[4] = 0xA8;
	buf[5] = 0x00;
	buf[6] = 0x00;
	buf[7] = 0x00;
	buf[8] = 0x00;
	buf[9] = 0x00;
	buf[10] = 0x00;
	buf[11] = 0x00;
	buf[12] = 0x00;
	buf[13] = 0x00;
	buf[14] = 0x00;
	buf[15] = 0x00;
	buf[16] = 0x00;
	buf[17] = 0x0B;
	buf[18] = 0x00;
	buf[19] = 0x00;
	buf[20] = 0x00;
	buf[21] = 0x00;
	buf[22] = 0x00;
	buf[23] = 0x00;
	buf[24] = 0x00;
	buf[25] = 0x00;
	buf[26] = 0x00;
	buf[27] = 0x00;
	buf[28] = 0x40;
	buf[29] = 0x01;
	buf[30] = 0x51;
	buf[31] = 0x00;
    buf[32] = 0x01;    
    buf[33] = 0x00;
    buf[34] = 0x01;
    buf[35] = 0x09;
    ret = mcde_dsi_dcs_write(main_display.port, 0xF2, buf, 15);
    ret = mcde_dsi_dcs_write16(main_display.port, 0xF20F, &(buf[15]), 15);
    ret = mcde_dsi_dcs_write16(main_display.port, 0xF21E, &(buf[30]), 6);

    //0xF3
 	buf[0] = 0x02;
	buf[1] = 0x03;
	buf[2] = 0x07;
	buf[3] = 0x45;
	buf[4] = 0x88;
	buf[5] = 0xD1;
	buf[6] = 0x0D;
    ret = mcde_dsi_dcs_write(main_display.port, 0xF3, buf, 7);
    
    //0xF0
	buf[0] = 0x55;
	buf[1] = 0xAA;
	buf[2] = 0x52;
	buf[3] = 0x08;
	buf[4] = 0x00;
	ret = mcde_dsi_dcs_write(main_display.port, 0xF0, buf, 5);

    //0xB1
	buf[0] = 0xCC;
	buf[1] = 0x00;
	buf[2] = 0x00;
	ret = mcde_dsi_dcs_write(main_display.port, 0xB1, buf, 3);

    //0xB8
	buf[0] = 0x01;
	buf[1] = 0x02;
	buf[2] = 0x02;
    buf[3] = 0x02;
	ret = mcde_dsi_dcs_write(main_display.port, 0xB8, buf, 4);

    //0xC9
    buf[0] = (0x63);
    buf[1] = (0x06);
    buf[2] = (0x0D);
    buf[3] = (0x1A);
    buf[4] = (0x17);
    buf[5] = (0x00);
	ret = mcde_dsi_dcs_write(main_display.port, 0xC9, buf, 6);

    //(0xF0); 
    buf[0] = (0x55);
    buf[1] = (0xAA);
    buf[2] = (0x52);
    buf[3] = (0x08);
    buf[4] = (0x01);
    ret = mcde_dsi_dcs_write(main_display.port, 0xF0, buf, 5);
    
    //(0xB0); 
    buf[0] = (0x05);
    buf[1] = (0x05);
    buf[2] = (0x05);
    ret = mcde_dsi_dcs_write(main_display.port, 0xB0, buf, 3);
    
    //(0xB1); 
    buf[0] = (0x05);
    buf[1] = (0x05);
    buf[2] = (0x05);
    ret = mcde_dsi_dcs_write(main_display.port, 0xB1, buf, 3);
    
    //(0xB2); 
    buf[0] = (0x01);
    buf[1] = (0x01);
    buf[2] = (0x01);
    ret = mcde_dsi_dcs_write(main_display.port, 0xB2, buf, 3);
    
    //(0xB3); 
    buf[0] = (0x0E);
    buf[1] = (0x0E);
    buf[2] = (0x0E);
    ret = mcde_dsi_dcs_write(main_display.port, 0xB3, buf, 3);

    //(0xB4); 
    buf[0] = (0x08);
    buf[1] = (0x08);
    buf[2] = (0x08);
    ret = mcde_dsi_dcs_write(main_display.port, 0xB4, buf, 3);
    
    //(0xB6);
    buf[0] = (0x44);
    buf[1] = (0x44);
    buf[2] = (0x44);
    ret = mcde_dsi_dcs_write(main_display.port, 0xB6, buf, 3);
    
    //(0xB7);
    buf[0] = (0x34);
    buf[1] = (0x34);
    buf[2] = (0x34);
    ret = mcde_dsi_dcs_write(main_display.port, 0xB7, buf, 3);
    
    //(0xB8); 
    buf[0] = (0x10);
    buf[1] = (0x10);
    buf[2] = (0x10);
    ret = mcde_dsi_dcs_write(main_display.port, 0xB8, buf, 3);    
    
    //(0xB9); 
    buf[0] = (0x26);
    buf[1] = (0x26);
    buf[2] = (0x26);
    ret = mcde_dsi_dcs_write(main_display.port, 0xB9, buf, 3);
    
    //(0xBA); 
    buf[0] = (0x34);
    buf[1] = (0x34);
    buf[2] = (0x34);
    ret = mcde_dsi_dcs_write(main_display.port, 0xBA, buf, 3);
    
    //(0xBC); 
    buf[0] = (0x00);
    buf[1] = (0xC8);
    buf[2] = (0x00);
    ret = mcde_dsi_dcs_write(main_display.port, 0xBC, buf, 3);
    
    //(0xBD); 
    buf[0] = (0x00);
    buf[1] = (0xC8);
    buf[2] = (0x00);
    ret = mcde_dsi_dcs_write(main_display.port, 0xBD, buf, 3);

    //(0xBE);
    buf[0] = (0x92);
    ret = mcde_dsi_dcs_write(main_display.port, 0xBE, buf, 1);

    //(0xC0); 
    buf[0] = (0x04);
    buf[1] = (0x00);
    ret = mcde_dsi_dcs_write(main_display.port, 0xC0, buf, 2);

    //(0xCA); 
    buf[0] = (0x00);
    ret = mcde_dsi_dcs_write(main_display.port, 0xCA, buf, 1);

    //(0xD0); 
    buf[0] = (0x0A);
    buf[1] = (0x10);
    buf[2] = (0x0D);
    buf[3] = (0x0F);
    ret = mcde_dsi_dcs_write(main_display.port, 0xD0, buf, 4);

    //(0xD1); 
    buf[0] = (0x00);
    buf[1] = (0x70);
    buf[2] = (0x00);
    buf[3] = (0xCE);
    buf[4] = (0x00);
    buf[5] = (0xF7);
    buf[6] = (0x01);
    buf[7] = (0x10);
    buf[8] = (0x01);
    buf[9] = (0x21);
    buf[10] = (0x01);
    buf[11] = (0x44);
    buf[12] = (0x01);
    buf[13] = (0x62);
    buf[14] = (0x01);
    buf[15] = (0x8D);
    ret = mcde_dsi_dcs_write(main_display.port, 0xD1, buf, 16);

    //(0xD2);
    buf[0] = (0x01);
    buf[1] = (0xAF);
    buf[2] = (0x01);
    buf[3] = (0xE4);
    buf[4] = (0x02);
    buf[5] = (0x0C);
    buf[6] = (0x02);
    buf[7] = (0x4D);
    buf[8] = (0x02);
    buf[9] = (0x82);
    buf[10] = (0x02);
    buf[11] = (0x84);
    buf[12] = (0x02);
    buf[13] = (0xB8);
    buf[14] = (0x02);
    buf[15] = (0xF0);
    ret = mcde_dsi_dcs_write(main_display.port, 0xD2, buf, 16);
    
    //(0xD3); 
    buf[0] = (0x03);
    buf[1] = (0x14);
    buf[2] = (0x03);
    buf[3] = (0x42);
    buf[4] = (0x03);
    buf[5] = (0x5E);
    buf[6] = (0x03);
    buf[7] = (0x80);
    buf[8] = (0x03);
    buf[9] = (0x97);
    buf[10] = (0x03);
    buf[11] = (0xB0);
    buf[12] = (0x03);
    buf[13] = (0xC0);
    buf[14] = (0x03);
    buf[15] = (0xDF);
    ret = mcde_dsi_dcs_write(main_display.port, 0xD3, buf, 16);
    
    //(0xD4);
    buf[0] = (0x03);
    buf[1] = (0xFD);
    buf[2] = (0x03);
    buf[3] = (0xFF);
    ret = mcde_dsi_dcs_write(main_display.port, 0xD4, buf, 4);
    
    //(0xD5); 
    buf[0] = (0x00);
    buf[1] = (0x70);
    buf[2] = (0x00);
    buf[3] = (0xCE);
    buf[4] = (0x00);
    buf[5] = (0xF7);
    buf[6] = (0x01);
    buf[7] = (0x10);
    buf[8] = (0x01);
    buf[9] = (0x21);
    buf[10] = (0x01);
    buf[11] = (0x44);
    buf[12] = (0x01);
    buf[13] = (0x62);
    buf[14] = (0x01);
    buf[15] = (0x8D);
    ret = mcde_dsi_dcs_write(main_display.port, 0xD5, buf, 16);
    
    //(0xD6); 
    buf[0] = (0x01);
    buf[1] = (0xAF);
    buf[2] = (0x01);
    buf[3] = (0xE4);
    buf[4] = (0x02);
    buf[5] = (0x0C);
    buf[6] = (0x02);
    buf[7] = (0x4D);
    buf[8] = (0x02);
    buf[9] = (0x82);
    buf[10] = (0x02);
    buf[11] = (0x84);
    buf[12] = (0x02);
    buf[13] = (0xB8);
    buf[14] = (0x02);
    buf[15] = (0xF0);
    ret = mcde_dsi_dcs_write(main_display.port, 0xD6, buf, 16);
    
    //(0xD7); 
    buf[0] = (0x03);
    buf[1] = (0x14);
    buf[2] = (0x03);
    buf[3] = (0x42);
    buf[4] = (0x03);
    buf[5] = (0x5E);
    buf[6] = (0x03);
    buf[7] = (0x80);
    buf[8] = (0x03);
    buf[9] = (0x97);
    buf[10] = (0x03);
    buf[11] = (0xB0);
    buf[12] = (0x03);
    buf[13] = (0xC0);
    buf[14] = (0x03);
    buf[15] = (0xDF);
    ret = mcde_dsi_dcs_write(main_display.port, 0xD7, buf, 16);
    
    //(0xD8); 
    buf[0] = (0x03);
    buf[1] = (0xFD);
    buf[2] = (0x03);
    buf[3] = (0xFF);
    ret = mcde_dsi_dcs_write(main_display.port, 0xD8, buf, 4);
    
    //(0xD9); 
    buf[0] = (0x00);
    buf[1] = (0x70);
    buf[2] = (0x00);
    buf[3] = (0xCE);
    buf[4] = (0x00);
    buf[5] = (0xF7);
    buf[6] = (0x01);
    buf[7] = (0x10);
    buf[8] = (0x01);
    buf[9] = (0x21);
    buf[10] = (0x01);
    buf[11] = (0x44);
    buf[12] = (0x01);
    buf[13] = (0x62);
    buf[14] = (0x01);
    buf[15] = (0x8D);
    ret = mcde_dsi_dcs_write(main_display.port, 0xD9, buf, 16);
    
    //(0xDD);
    buf[0] = (0x01);
    buf[1] = (0xAF);
    buf[2] = (0x01);
    buf[3] = (0xE4);
    buf[4] = (0x02);
    buf[5] = (0x0C);
    buf[6] = (0x02);
    buf[7] = (0x4D);
    buf[8] = (0x02);
    buf[9] = (0x82);
    buf[10] = (0x02);
    buf[11] = (0x84);
    buf[12] = (0x02);
    buf[13] = (0xB8);
    buf[14] = (0x02);
    buf[15] = (0xF0);
    ret = mcde_dsi_dcs_write(main_display.port, 0xDD, buf, 16);
    
    //(0xDE); 
    buf[0] = (0x03);
    buf[1] = (0x14);
    buf[2] = (0x03);
    buf[3] = (0x42);
    buf[4] = (0x03);
    buf[5] = (0x5E);
    buf[6] = (0x03);
    buf[7] = (0x80);
    buf[8] = (0x03);
    buf[9] = (0x97);
    buf[10] = (0x03);
    buf[11] = (0xB0);
    buf[12] = (0x03);
    buf[13] = (0xC0);
    buf[14] = (0x03);
    buf[15] = (0xDF);
    ret = mcde_dsi_dcs_write(main_display.port, 0xDE, buf, 16);
    
    //(0xDF); 
    buf[0] = (0x03);
    buf[1] = (0xFD);
    buf[2] = (0x03);
    buf[3] = (0xFF);
    ret = mcde_dsi_dcs_write(main_display.port, 0xDF, buf, 4);

    
    //(0xE0); 
    buf[0] = (0x00);
    buf[1] = (0x70);
    buf[2] = (0x00);
    buf[3] = (0xCE);
    buf[4] = (0x00);
    buf[5] = (0xF7);
    buf[6] = (0x01);
    buf[7] = (0x10);
    buf[8] = (0x01);
    buf[9] = (0x21);
    buf[10] = (0x01);
    buf[11] = (0x44);
    buf[12] = (0x01);
    buf[13] = (0x62);
    buf[14] = (0x01);
    buf[15] = (0x8D);
    ret = mcde_dsi_dcs_write(main_display.port, 0xE0, buf, 16);

    //(0xE1); 
    buf[0] = (0x01);
    buf[1] = (0xAF);
    buf[2] = (0x01);
    buf[3] = (0xE4);
    buf[4] = (0x02);
    buf[5] = (0x0C);
    buf[6] = (0x02);
    buf[7] = (0x4D);
    buf[8] = (0x02);
    buf[9] = (0x82);
    buf[10] = (0x02);
    buf[11] = (0x84);
    buf[12] = (0x02);
    buf[13] = (0xB8);
    buf[14] = (0x02);
    buf[15] = (0xF0);
    ret = mcde_dsi_dcs_write(main_display.port, 0xE1, buf, 16);

    //(0xE2); 
    buf[0] = (0x03);
    buf[1] = (0x14);
    buf[2] = (0x03);
    buf[3] = (0x42);
    buf[4] = (0x03);
    buf[5] = (0x5E);
    buf[6] = (0x03);
    buf[7] = (0x80);
    buf[8] = (0x03);
    buf[9] = (0x97);
    buf[10] = (0x03);
    buf[11] = (0xB0);
    buf[12] = (0x03);
    buf[13] = (0xC0);
    buf[14] = (0x03);
    buf[15] = (0xDF);
    ret = mcde_dsi_dcs_write(main_display.port, 0xE2, buf, 16);

    //(0xE3);
    buf[0] = (0x03);
    buf[1] = (0xFD);
    buf[2] = (0x03);
    buf[3] = (0xFF);
    ret = mcde_dsi_dcs_write(main_display.port, 0xE3, buf, 4);

    //(0xE4); 
    buf[0] = (0x00);
    buf[1] = (0x70);
    buf[2] = (0x00);
    buf[3] = (0xCE);
    buf[4] = (0x00);
    buf[5] = (0xF7);
    buf[6] = (0x01);
    buf[7] = (0x10);
    buf[8] = (0x01);
    buf[9] = (0x21);
    buf[10] = (0x01);
    buf[11] = (0x44);
    buf[12] = (0x01);
    buf[13] = (0x62);
    buf[14] = (0x01);
    buf[15] = (0x8D);
    ret = mcde_dsi_dcs_write(main_display.port, 0xE4, buf, 16);

    //(0xE5);
    buf[0] = (0x01);
    buf[1] = (0xAF);
    buf[2] = (0x01);
    buf[3] = (0xE4);
    buf[4] = (0x02);
    buf[5] = (0x0C);
    buf[6] = (0x02);
    buf[7] = (0x4D);
    buf[8] = (0x02);
    buf[9] = (0x82);
    buf[10] = (0x02);
    buf[11] = (0x84);
    buf[12] = (0x02);
    buf[13] = (0xB8);
    buf[14] = (0x02);
    buf[15] = (0xF0);
    ret = mcde_dsi_dcs_write(main_display.port, 0xE5, buf, 16);

    //(0xE6); 
    buf[0] = (0x03);
    buf[1] = (0x14);
    buf[2] = (0x03);
    buf[3] = (0x42);
    buf[4] = (0x03);
    buf[5] = (0x5E);
    buf[6] = (0x03);
    buf[7] = (0x80);
    buf[8] = (0x03);
    buf[9] = (0x97);
    buf[10] = (0x03);
    buf[11] = (0xB0);
    buf[12] = (0x03);
    buf[13] = (0xC0);
    buf[14] = (0x03);
    buf[15] = (0xDF);
    ret = mcde_dsi_dcs_write(main_display.port, 0xE6, buf, 16);

    //(0xE7); 
    buf[0] = (0x03);
    buf[1] = (0xFD);
    buf[2] = (0x03);
    buf[3] = (0xFF);
    ret = mcde_dsi_dcs_write(main_display.port, 0xE7, buf, 4);

    //(0xE8); 
    buf[0] = (0x00);
    buf[1] = (0x70);
    buf[2] = (0x00);
    buf[3] = (0xCE);
    buf[4] = (0x00);
    buf[5] = (0xF7);
    buf[6] = (0x01);
    buf[7] = (0x10);
    buf[8] = (0x01);
    buf[9] = (0x21);
    buf[10] = (0x01);
    buf[11] = (0x44);
    buf[12] = (0x01);
    buf[13] = (0x62);
    buf[14] = (0x01);
    buf[15] = (0x8D);
    ret = mcde_dsi_dcs_write(main_display.port, 0xE8, buf, 16);

    //(0xE9); 
    buf[0] = (0x01);
    buf[1] = (0xAF);
    buf[2] = (0x01);
    buf[3] = (0xE4);
    buf[4] = (0x02);
    buf[5] = (0x0C);
    buf[6] = (0x02);
    buf[7] = (0x4D);
    buf[8] = (0x02);
    buf[9] = (0x82);
    buf[10] = (0x02);
    buf[11] = (0x84);
    buf[12] = (0x02);
    buf[13] = (0xB8);
    buf[14] = (0x02);
    buf[15] = (0xF0);
    ret = mcde_dsi_dcs_write(main_display.port, 0xE9, buf, 16);

    //(0xEA); 
    buf[0] = (0x03);
    buf[1] = (0x14);
    buf[2] = (0x03);
    buf[3] = (0x42);
    buf[4] = (0x03);
    buf[5] = (0x5E);
    buf[6] = (0x03);
    buf[7] = (0x80);
    buf[8] = (0x03);
    buf[9] = (0x97);
    buf[10] = (0x03);
    buf[11] = (0xB0);
    buf[12] = (0x03);
    buf[13] = (0xC0);
    buf[14] = (0x03);
    buf[15] = (0xDF);
    ret = mcde_dsi_dcs_write(main_display.port, 0xEA, buf, 16);

    //(0xEB); 
    buf[0] = (0x03);
    buf[1] = (0xFD);
    buf[2] = (0x03);
    buf[3] = (0xFF);
    ret = mcde_dsi_dcs_write(main_display.port, 0xEB, buf, 4);

    //0x3A
    buf[0] = 0x77;
    ret = mcde_dsi_dcs_write(main_display.port, 0x3A, buf, 1);
    
    //0x36
    buf[0] = 0x40;
    ret = mcde_dsi_dcs_write(main_display.port, 0x36, buf, 1);

    //0x35
    buf[0] = 0x00;
    ret = mcde_dsi_dcs_write(main_display.port, 0x35, buf, 1);

	//set lcd sleep out
	ret = mcde_dsi_dcs_write(main_display.port, 0x11, NULL, 0);
    
	mdelay(120);
    
	//set display on in background
	ret = mcde_dsi_dcs_write(main_display.port, 0x2C, NULL, 0);
    ret = mcde_dsi_dcs_write(main_display.port, 0x13, NULL, 0);
    ret = mcde_dsi_dcs_write(main_display.port, 0x29, NULL, 0);
    
    return ret;
}

int mcde_turn_on_display_dsi(void)
{
	int ret = 0;
	u8 buffer[1] = {0};
	u8 panel_id = 0xFF;
	
	db8500_gpio_set_output(GPIO139_GPIO, 1);
	mdelay(40); 

	db8500_gpio_set_output(GPIO139_GPIO, 0); 
	mdelay(40);
	
	db8500_gpio_set_output(GPIO139_GPIO, 1);
	mdelay(40); 
	
	ret = mcde_dsi_dcs_write(main_display.port, 0x11, NULL, 0);//Sleep Out
	mdelay(40); 

	ret = mcde_dsi_dcs_read(main_display.port, 0xDA, &panel_id, 1);
	printk("\n mcde_turn_on_display_dsi read 0xDA buffer[0] = 0x%0x ret1 = %d\n", panel_id, ret);
	mdelay(100);

	ret = mcde_dsi_dcs_read(main_display.port, 0x0A, buffer, 1);
	printk("\n mcde_turn_on_display_dsi read 0x0A  buffer[0] = 0x%0x ret1 = %d\n",buffer[0],ret);
	mdelay(40);

	
#ifdef CONFIG_VIA_DISPLAY_DSI_CMD
	u8 buf[130] = {0};
	if(buffer[0] == 0x18)  //BYD
	{
		printk("\n mcde_turn_on_display_dsi enter LCD BYD \n");
		debug("%s: enter\n", __func__);

		buf[0] = 0xFF;
		buf[1] = 0x83;
		buf[2] = 0x69;
		ret = mcde_dsi_dcs_write(main_display.port, 0xB9, buf, 3);//set EXTC
		
		buf[0]=0x01;
		buf[1]=0x00;
		buf[2]=0x34;
		buf[3]=0x07;
		buf[4]=0x00;
		buf[5]=0x0E;
		buf[6]=0x0E;
		buf[7]=0x1A;
		buf[8]=0x22;
		buf[9]=0x3F;
		buf[10]=0x3F;
		buf[11]=0x07;
		buf[12]=0x23;
		buf[13]=0x01;
		buf[14]=0xE6;
		buf[15]=0xE6;
		buf[16]=0xE6;
		buf[17]=0xE6;
		buf[18]=0xE6;
		ret = mcde_dsi_dcs_write(main_display.port, 0xB1, buf, 5);
		ret = mcde_dsi_dcs_write(main_display.port, 0xFD, &(buf[4]), 7);
		ret = mcde_dsi_dcs_write(main_display.port, 0xFD, &(buf[10]), 7);
		ret = mcde_dsi_dcs_write(main_display.port, 0xFD, &(buf[16]), 3);

		buf[0]=0x00;
		buf[1]=0x20;
		buf[2]=0x0a;
		buf[3]=0x0a;
		buf[4]=0x70;
		buf[5]=0x00;
		buf[6]=0xFF;
		buf[7]=0x00;
		buf[8]=0x00;
		buf[9]=0x00;
		buf[10]=0x00;
		buf[11]=0x03;
		buf[12]=0x03;
		buf[13]=0x00;
		buf[14]=0x01;
		ret = mcde_dsi_dcs_write(main_display.port, 0xB2, buf, 15);// SET Display 480x800

		buf[0]=0x00;
		buf[1]=0x0C;
		buf[2]=0x84;
		buf[3]=0x0C;
		buf[4]=0x01;
		ret = mcde_dsi_dcs_write(main_display.port, 0xB4, buf, 5); // SET Display CYC

		buf[0]=0x2C;
		buf[1]=0x2C;
		ret = mcde_dsi_dcs_write(main_display.port, 0xB6, buf, 2);// SET VCOM

		buf[0]=0x00;
		buf[1]=0x05;
		buf[2]=0x03;
		buf[3]=0x00;
		buf[4]=0x01;
		buf[5]=0x09;
		buf[6]=0x10;
		buf[7]=0x80;
		buf[8]=0x37;
		buf[9]=0x37;
		buf[10]=0x20;
		buf[11]=0x31;
		buf[12]=0x46;
		buf[13]=0x8A;
		buf[14]=0x57;
		buf[15]=0x9B;//16
		buf[16]=0x20;//17
		buf[17]=0x31;//18
		buf[18]=0x46;//19
		buf[19]=0x8A;//20
		buf[20]=0x57;//21
		buf[21]=0x9B;//22
		buf[22]=0x07;//23
		buf[23]=0x0F;//24
		buf[24]=0x07;//25
		buf[25]=0x00;//26
		ret = mcde_dsi_dcs_write(main_display.port, 0xD5, buf, 5);
		ret = mcde_dsi_dcs_write(main_display.port, 0xFD, &(buf[4]), 7);
		ret = mcde_dsi_dcs_write(main_display.port, 0xFD, &(buf[10]), 7);
		ret = mcde_dsi_dcs_write(main_display.port, 0xFD, &(buf[16]), 7);
		ret = mcde_dsi_dcs_write(main_display.port, 0xFD, &(buf[22]), 4);

		buf[0]=0x00;
		buf[1]=0x08;
		buf[2]=0x0D;
		buf[3]=0x2D;
		buf[4]=0x34;
		buf[5]=0x3F;
		buf[6]=0x19;
		buf[7]=0x38;
		buf[8]=0x09;
		buf[9]=0x0E;
		buf[10]=0x0E;
		buf[11]=0x12;
		buf[12]=0x14;
		buf[13]=0x12;
		buf[14]=0x14;
		buf[15]=0x13;
		buf[16]=0x19;
		buf[17]=0x00;
		buf[18]=0x08;
		buf[19]=0x0D;
		buf[20]=0x2D;
		buf[21]=0x34;
		buf[22]=0x3F;
		buf[23]=0x19;
		buf[24]=0x38;
		buf[25]=0x09;
		buf[26]=0x0E;
		buf[27]=0x0E;
		buf[28]=0x12;
		buf[29]=0x14;
		buf[30]=0x12;
		buf[31]=0x14;
		buf[32]=0x13;
		buf[33]=0x19;
		ret = mcde_dsi_dcs_write(main_display.port, 0xE0, buf, 5);
		ret = mcde_dsi_dcs_write(main_display.port, 0xFD, &(buf[4]), 7);
		ret = mcde_dsi_dcs_write(main_display.port, 0xFD, &(buf[10]), 7);
		ret = mcde_dsi_dcs_write(main_display.port, 0xFD, &(buf[16]), 7);
		ret = mcde_dsi_dcs_write(main_display.port, 0xFD, &(buf[22]), 7);
		ret = mcde_dsi_dcs_write(main_display.port, 0xFD, &(buf[28]), 6);

		buf[0]=0x00;
		buf[1]=0xa0;
		buf[2]=0xc6;
		buf[3]=0x00;
		buf[4]=0x0a;
		buf[5]=0x00;
		buf[6]=0x10;
		buf[7]=0x30;
		buf[8]=0x6C;
		buf[9]=0x02;
		buf[10]=0x11;
		buf[11]=0x18;
		buf[12]=0x40;
		ret = mcde_dsi_dcs_write(main_display.port, 0xBA, buf, 13);

		buf[0]=0x01;
		//R
		buf[1]=0x02;
		buf[2]=0x08;
		buf[3]=0x10;
		buf[4]=0x18;
		buf[5]=0x1F;
		buf[6]=0x28;
		buf[7]=0x30;
		buf[8]=0x37;
		buf[9]=0x3F;
		buf[10]=0x49;
		buf[11]=0x51;
		buf[12]=0x59;
		buf[13]=0x60;
		buf[14]=0x68;
		buf[15]=0x70;
		buf[16]=0x79;
		buf[17]=0x80;
		buf[18]=0x88;
		buf[19]=0x8F;
		buf[20]=0x97;
		buf[21]=0x9F;
		buf[22]=0xA7;
		buf[23]=0xAF;
		buf[24]=0xB8;
		buf[25]=0xC0;
		buf[26]=0xC8;
		buf[27]=0xCE;
		buf[28]=0xD7;
		buf[29]=0xE0;
		buf[30]=0xE7;
		buf[31]=0xF0;
		buf[32]=0xF7;
		buf[33]=0xFF;
		buf[34]=0x5A;
		buf[35]=0x7D;
		buf[36]=0xC4;
		buf[37]=0xEC;
		buf[38]=0x84;
		buf[39]=0x18;
		buf[40]=0x5C;
		buf[41]=0xD3;
		buf[42]=0x80;
		//G
		buf[43]=0x02;
		buf[44]=0x08;
		buf[45]=0x10;
		buf[46]=0x18;
		buf[47]=0x1F;
		buf[48]=0x28;
		buf[49]=0x30;
		buf[50]=0x37;
		buf[51]=0x3F;
		buf[52]=0x49;
		buf[53]=0x51;
		buf[54]=0x59;
		buf[55]=0x60;
		buf[56]=0x68;
		buf[57]=0x70;
		buf[58]=0x79;
		buf[59]=0x80;
		buf[60]=0x88;
		buf[61]=0x8F;
		buf[62]=0x97;
		buf[63]=0x9F;
		buf[64]=0xA7;
		buf[65]=0xAF;
		buf[66]=0xB8;
		buf[67]=0xC0;
		buf[68]=0xC8;
		buf[69]=0xCE;
		buf[70]=0xD7;
		buf[71]=0xE0;
		buf[72]=0xE7;
		buf[73]=0xF0;
		buf[74]=0xF7;
		buf[75]=0xFF;
		buf[76]=0x5A;
		buf[77]=0x7D;
		buf[78]=0xC4;
		buf[79]=0xEC;
		buf[80]=0x84;
		buf[81]=0x18;
		buf[82]=0x5C;
		buf[83]=0xD3;
		buf[84]=0x80;
		//B
		buf[85]=0x02;
		buf[86]=0x08;
		buf[87]=0x10;
		buf[88]=0x18;
		buf[89]=0x1F;
		buf[90]=0x28;
		buf[91]=0x30;
		buf[92]=0x37;
		buf[93]=0x3F;
		buf[94]=0x49;
		buf[95]=0x51;
		buf[96]=0x59;
		buf[97]=0x60;
		buf[98]=0x68;
		buf[99]=0x70;
		buf[100]=0x79;
		buf[101]=0x80;
		buf[102]=0x88;
		buf[103]=0x8F;
		buf[104]=0x97;
		buf[105]=0x9F;
		buf[106]=0xA7;
		buf[107]=0xAF;
		buf[108]=0xB8;
		buf[109]=0xC0;
		buf[110]=0xC8;
		buf[111]=0xCE;
		buf[112]=0xD7;
		buf[113]=0xE0;
		buf[114]=0xE7;
		buf[115]=0xF0;
		buf[116]=0xF7;
		buf[117]=0xFF;
		buf[118]=0x5A;
		buf[119]=0x7D;
		buf[120]=0xC4;
		buf[121]=0xEC;
		buf[122]=0x84;
		buf[123]=0x18;
		buf[124]=0x5C;
		buf[125]=0xD3;
		buf[126]=0x80;
		ret = mcde_dsi_dcs_write(main_display.port, 0xC1, buf, 5);
		ret = mcde_dsi_dcs_write(main_display.port, 0xFD, &(buf[4]), 7);
		ret = mcde_dsi_dcs_write(main_display.port, 0xFD, &(buf[10]), 7);
		ret = mcde_dsi_dcs_write(main_display.port, 0xFD, &(buf[16]), 7);
		ret = mcde_dsi_dcs_write(main_display.port, 0xFD, &(buf[22]), 7);
		ret = mcde_dsi_dcs_write(main_display.port, 0xFD, &(buf[28]), 7);
		ret = mcde_dsi_dcs_write(main_display.port, 0xFD, &(buf[34]), 7);
		ret = mcde_dsi_dcs_write(main_display.port, 0xFD, &(buf[40]), 7);
		ret = mcde_dsi_dcs_write(main_display.port, 0xFD, &(buf[46]), 7);
		ret = mcde_dsi_dcs_write(main_display.port, 0xFD, &(buf[52]), 7);
		ret = mcde_dsi_dcs_write(main_display.port, 0xFD, &(buf[58]), 7);
		ret = mcde_dsi_dcs_write(main_display.port, 0xFD, &(buf[64]), 7);
		ret = mcde_dsi_dcs_write(main_display.port, 0xFD, &(buf[70]), 7);
		ret = mcde_dsi_dcs_write(main_display.port, 0xFD, &(buf[76]), 7);
		ret = mcde_dsi_dcs_write(main_display.port, 0xFD, &(buf[82]), 7);
		ret = mcde_dsi_dcs_write(main_display.port, 0xFD, &(buf[88]), 7);
		ret = mcde_dsi_dcs_write(main_display.port, 0xFD, &(buf[94]), 7);
		ret = mcde_dsi_dcs_write(main_display.port, 0xFD, &(buf[100]), 7);
		ret = mcde_dsi_dcs_write(main_display.port, 0xFD, &(buf[106]), 7);
		ret = mcde_dsi_dcs_write(main_display.port, 0xFD, &(buf[112]), 7);
		ret = mcde_dsi_dcs_write(main_display.port, 0xFD, &(buf[118]), 7);
		ret = mcde_dsi_dcs_write(main_display.port, 0xFD, &(buf[124]), 3);


		buf[0]=0x77;
		ret = mcde_dsi_dcs_write(main_display.port, 0x3A, buf, 1);// SET ColMod

		ret = mcde_dsi_dcs_write(main_display.port, 0x11, NULL, 0);//Sleep Out
		mdelay(120); //120ms
		ret = mcde_dsi_dcs_write(main_display.port, 0x29, NULL, 0);//Display On
		ret = mcde_dsi_dcs_write(main_display.port, 0x2C, NULL, 0);//Write RAM
	} else {
		printk("\n mcde_turn_on_display_dsi enter LCD BOE \n");

		buf[0] = 0x55;
		buf[1] = 0xAA;
		buf[2] = 0x52;
		buf[3] = 0x08;
		buf[4] = 0x01;
		ret = mcde_dsi_dcs_write(main_display.port, 0xF0, buf, 5);

		buf[0] = 0x34;
		buf[1] = 0x34;
		buf[2] = 0x34;
		ret = mcde_dsi_dcs_write(main_display.port, 0xB6, buf, 3);

		buf[0] = 0x0C;
		buf[1] = 0x0C;
		buf[2] = 0x0C;
		ret = mcde_dsi_dcs_write(main_display.port, 0xB0, buf, 3);

		buf[0] = 0x24;
		buf[1] = 0x24;
		buf[2] = 0x24;
		ret = mcde_dsi_dcs_write(main_display.port, 0xB7, buf, 3);

		buf[0] = 0x0C;
		buf[1] = 0x0C;
		buf[2] = 0x0C;
		ret = mcde_dsi_dcs_write(main_display.port, 0xB1, buf, 3);

		buf[0] = 0x34;
		ret = mcde_dsi_dcs_write(main_display.port, 0xB8, buf, 1);

		buf[0] = 0x00;
		ret = mcde_dsi_dcs_write(main_display.port, 0xB2, buf, 1);

		buf[0] = 0x24;
		buf[1] = 0x24;
		buf[2] = 0x24;
		ret = mcde_dsi_dcs_write(main_display.port, 0xB9, buf, 3);

		buf[0] = 0x08;
		buf[1] = 0x08;
		buf[2] = 0x08;
		ret = mcde_dsi_dcs_write(main_display.port, 0xB3, buf, 3);

		buf[0] = 0x24;
		buf[1] = 0x24;
		buf[2] = 0x24;
		ret = mcde_dsi_dcs_write(main_display.port, 0xBA, buf, 3);

		buf[0] = 0x08;
		buf[1] = 0x08;
		buf[2] = 0x08;
		ret = mcde_dsi_dcs_write(main_display.port, 0xB5, buf, 3);

		buf[0] = 0x00;
		buf[1] = 0x80;
		buf[2] = 0x00;
		ret = mcde_dsi_dcs_write(main_display.port, 0xBC, buf, 3);

		buf[0] = 0x00;
		buf[1] = 0x80;
		buf[2] = 0x00;
		ret = mcde_dsi_dcs_write(main_display.port, 0xBD, buf, 3);

		buf[0] = 0x00;
		buf[1] = 0x4F;
		ret = mcde_dsi_dcs_write(main_display.port, 0xBE, buf, 2);

		buf[0] = 0x55;
		buf[1] = 0xAA;
		buf[2] = 0x52;
		buf[3] = 0x08;
		buf[4] = 0x00;
		ret = mcde_dsi_dcs_write(main_display.port, 0xF0, buf, 5);

		buf[0] = 0x00;
		buf[1] = 0x05;
		buf[2] = 0x02;
		buf[3] = 0x05;
		buf[4] = 0x02;
		ret = mcde_dsi_dcs_write(main_display.port, 0xB0, buf, 5);

		buf[0] = 0x05;
		ret = mcde_dsi_dcs_write(main_display.port, 0xB6, buf, 1);

		buf[0] = 0x00;
		buf[1] = 0x00;
		ret = mcde_dsi_dcs_write(main_display.port, 0xB7, buf, 2);

		buf[0] = 0x01;
		buf[1] = 0x05;
		buf[2] = 0x05;
		buf[3] = 0x05;
		ret = mcde_dsi_dcs_write(main_display.port, 0xB8, buf, 4);

		buf[0] = 0x01;
		ret = mcde_dsi_dcs_write(main_display.port, 0xBA, buf, 1);

		buf[0] = 0x00;
		buf[1] = 0x00;
		buf[2] = 0x00;
		ret = mcde_dsi_dcs_write(main_display.port, 0xBC, buf, 3);

		buf[0] = 0x03;
		buf[1] = 0x50;
		buf[2] = 0x50;
		ret = mcde_dsi_dcs_write(main_display.port, 0xCC, buf, 3);

		buf[0] = 0x01;
		buf[1] = 0x84;
		buf[2] = 0x07;
		buf[3] = 0x31;
		buf[4] = 0x00;
		ret = mcde_dsi_dcs_write(main_display.port, 0xBD, buf, 5);

		buf[0] = 0xAA;
		buf[1] = 0x55;
		buf[2] = 0x25;
		buf[3] = 0x01;
		ret = mcde_dsi_dcs_write(main_display.port, 0xF0, buf, 4);

		buf[0] = 0x77;
		ret = mcde_dsi_dcs_write(main_display.port, 0x3A, buf, 1);

		ret = mcde_dsi_dcs_write(main_display.port, 0x11, NULL, 0);
		mdelay(240); //120ms
		ret = mcde_dsi_dcs_write(main_display.port, 0x29, NULL, 0);
		ret = mcde_dsi_dcs_write(main_display.port, 0x2C, NULL, 0);

		buf[0] = 0x55;
		buf[1] = 0xAA;
		buf[2] = 0x52;
		buf[3] = 0x08;
		buf[4] = 0x00;
		ret = mcde_dsi_dcs_write(main_display.port, 0xF0, buf, 5);

		buf[0] = 0xF8;
		buf[1] = 0x00;
		ret = mcde_dsi_dcs_write(main_display.port, 0xB1, buf, 2);
	}
#else	
	switch(panel_id){
		case LEAD_PANEL_ID:
			ret = mcde_turn_on_lead_dsi();
			break;
		
		case SHARP_PANEL_ID:
			ret = mcde_turn_on_sharp_dsi();
			break;
            
		case BYD_PANEL_ID:
			ret = mcde_turn_on_byd_dsi();
			 break;
		case BOE_PANEL_ID:
			ret = mcde_turn_on_boe_dsi();
			break;

		case TRULY_PANEL_ID:
			ret = mcde_turn_on_truly_dsi();
			break;
	}
#endif
	return ret;
}

/* aux supplies */
#define MASK_LDO_VAUX1		0x3
#define MASK_LDO_VAUX1_SHIFT	0x0
#define VAUXSEL_VOLTAGE_MASK	0xf

#define VANA_ENABLE_IN_HP_MODE	0x05

#define ENABLE_PWM1		0x01
#define PWM_DUTY_LOW_1024_1024	0xFF
#define PWM_DUTY_HI_1024_1024	0x03

/*
 * regulator layout
 * @voltage: supported voltage
 * @regval: register value to be written
 */
struct regulator_voltage {
	int voltage;
	int regval;
};

/* voltage table for VAUXn regulators */
static struct regulator_voltage vauxn_table[] = {
	{ .voltage = 1100000, .regval  = 0x0, },
	{ .voltage = 1200000, .regval  = 0x1, },
	{ .voltage = 1300000, .regval  = 0x2, },
	{ .voltage = 1400000, .regval  = 0x3, },
	{ .voltage = 1500000, .regval  = 0x4, },
	{ .voltage = 1800000, .regval  = 0x5, },
	{ .voltage = 1850000, .regval  = 0x6, },
	{ .voltage = 1900000, .regval  = 0x7, },
	{ .voltage = 2500000, .regval  = 0x8, },
	{ .voltage = 2650000, .regval  = 0x9, },
	{ .voltage = 2700000, .regval  = 0xa, },
	{ .voltage = 2750000, .regval  = 0xb, },
	{ .voltage = 2800000, .regval  = 0xc, },
	{ .voltage = 2900000, .regval  = 0xd, },
	{ .voltage = 3000000, .regval  = 0xe, },
	{ .voltage = 3300000, .regval  = 0xf, },
};


/*
 * This code is derived from the handling of AB8500_LDO_VAUX1 in
 * ab8500_ldo_is_enabled in Linux.
 */
static int mcde_is_vaux1_enabled(void)
{
	int val;
	val = ab8500_read(AB8500_REGU_CTRL2,
			AB8500_REGU_VAUX12_REGU_REG);
	if (val & MASK_LDO_VAUX1)
		return TRUE;
	return FALSE;
}

/*
 * This code is derived from the handling of AB8500_LDO_VAUX1 in
 * ab8500_ldo_get_voltage in Linux.
 */
static int mcde_get_vaux1_voltage(void)
{
	int val;
	val = ab8500_read(AB8500_REGU_CTRL2,
		AB8500_REGU_VAUX1_SEL_REG);
	return vauxn_table[val & VAUXSEL_VOLTAGE_MASK].voltage;
}

static int mcde_display_power_init(void)
{
	int val;
	unsigned int i;

	debug("%s: enter\n", __func__);
	/*
	 * On v1.1 HREF boards (HREF+) and V2 boards
	 * Vaux1 needs to be enabled for the
	 * display to work.  This is done by enabling the regulators in the
	 * AB8500 via PRCMU I2C transactions.
	 *
	 * This code is derived from the handling of AB8500_LDO_VAUX1 in
	 * ab8500_ldo_enable(), ab8500_ldo_disable() and
	 * ab8500_get_best_voltage in Linux.
	 *
	 * Turn off and delay is required to have it work across soft reboots.
	 */

	val = ab8500_read(AB8500_REGU_CTRL2,
		AB8500_REGU_VAUX12_REGU_REG);
	if (val < 0) {
		printf("Read vaux1 status failed\n");
		return -EINVAL;
	}

	/* Turn off */
	if (ab8500_write(AB8500_REGU_CTRL2, AB8500_REGU_VAUX12_REGU_REG,
			   val & ~MASK_LDO_VAUX1) < 0) {
		printf("Turn off Vaux1 failed\n");
		return -EINVAL;
	}

	udelay(10 * 1000);


	/* Find voltage from vauxn table */
	for (i = 0; i < ARRAY_SIZE(vauxn_table) ; i++) {
		if (vauxn_table[i].voltage == CONFIG_SYS_DISPLAY_VOLTAGE) {
			if (ab8500_write(AB8500_REGU_CTRL2,
				AB8500_REGU_VAUX1_SEL_REG,
				vauxn_table[i].regval) < 0) {
				printf("AB8500_REGU_VAUX1_SEL_REG failed\n");
				return -EINVAL;
			}
			break;
		}
	}

	val = val & ~MASK_LDO_VAUX1;
	val = val | (1 << MASK_LDO_VAUX1_SHIFT);

	/* Turn on the supply */
	if (ab8500_write(AB8500_REGU_CTRL2,
			AB8500_REGU_VAUX12_REGU_REG, val) < 0) {
		printf("Turn on Vaux1 failed\n");
		return -EINVAL;
	}

	/*  DCI & CSI (DSI / PLL / Camera) */ /* Vana & Vpll HP mode */
	if (ab8500_write(AB8500_REGU_CTRL2, AB8500_REGU_VPLLVANA_REGU_REG,
						VANA_ENABLE_IN_HP_MODE) < 0) {
		printf("Turn on Vana failed\n");
		return -EINVAL;
	}

	/* Enable the PWM control for the backlight Main display */

	/*Enable Vintcore which supply to UPL PLL */
	if (ab8500_write(AB8500_REGU_CTRL1, AB8500_REGU_MISC1_REG,0x04) < 0) {
		printf("Enable Vintcore failed\n");
		return -EINVAL;
	}

	if (ab8500_write(AB8500_SYS_CTRL2_BLOCK,AB8500_SYSULPCLK_CTRL1_REG, 0x04) < 0) {
		printf("Request ULPCLK failed\n");
		return -EINVAL;
	}

	if (ab8500_write(AB8500_SYS_CTRL2_BLOCK,AB8500_SYSULPCLK_CTRL1_REG, 0x05) < 0) {
		printf("Switch PWM clock from SYSCLK to ULPCLK failed\n");
		return -EINVAL;
	}

	/* Enable the PWM control for the backlight Main display */
	if (ab8500_write(AB8500_MISC, AB8500_PWM_OUT_CTRL7_REG,
							ENABLE_PWM1) < 0) {
		printf("Enable PWM1 failed\n");
		return -EINVAL;
	}
#if defined(BOARD_C7_P0)
	if (ab8500_write(AB8500_MISC, AB8500_PWM_OUT_CTRL1_REG,
						PWM_DUTY_LOW_1024_1024) < 0) {
		printf("PWM_DUTY_LOW_1024_1024 failed\n");
		return -EINVAL;
	}
	if (ab8500_write(AB8500_MISC, AB8500_PWM_OUT_CTRL2_REG,
						PWM_DUTY_HI_1024_1024) < 0) {
		printf("PWM_DUTY_HI_1024_1024 failed\n");
		return -EINVAL;
	}
#else
	if (ab8500_write(AB8500_MISC, AB8500_PWM_OUT_CTRL1_REG,0x01) < 0) {//zzw
		printf("PWM_DUTY_LOW_1024_1024 failed\n");
		return -EINVAL;
	}
	if (ab8500_write(AB8500_MISC, AB8500_PWM_OUT_CTRL2_REG,0xFA) < 0) {
		printf("PWM_DUTY_HI_1024_1024 failed\n");
		return -EINVAL;
	}

#endif

	if (!mcde_is_vaux1_enabled() || mcde_get_vaux1_voltage()
					!= CONFIG_SYS_DISPLAY_VOLTAGE) {
		printf("VAUX is %d and is set to %d V should be %d\n"
			, mcde_is_vaux1_enabled(), mcde_get_vaux1_voltage()
			, CONFIG_SYS_DISPLAY_VOLTAGE);
		return -EINVAL;
	}

	return 0;
}


int mcde_startup_dsi(struct mcde_platform_data *pdata)
{
	u8 num_dsilinks;
	int ret;

	debug("%s: enter\n", __func__);
#ifndef CONFIG_VIA_DISPLAY_DSI_VID
	if (main_display.port->mode != MCDE_PORTMODE_CMD) {
		printf("%s: only CMD mode supported\n", __func__);
		return -ENODEV;
	}
#endif
	num_dsilinks = main_display.port->phy.dsi.num_data_lanes;
	mcde_init();
	ret = mcde_probe(num_dsilinks, pdata);
	if (ret) {
		printf("%s: mcde_probe() -> %d\n", __func__, ret);
		return ret;
	}
	ret = mcde_display_power_init();
	if (ret) {
		printf("%s: mcde_display_power_init() -> %d\n", __func__, ret);
		return ret;
	}
//joe debug
#if CONFIG_SYS_DISPLAY_DSI
	ret = board_mcde_display_reset();
	if (ret) {
		printf("%s: board_mcde_display_reset() -> %d\n", __func__, ret);
		return ret;
	}
#endif
	return 0;
}
