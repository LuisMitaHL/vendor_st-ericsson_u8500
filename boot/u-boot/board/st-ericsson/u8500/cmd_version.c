/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides API routine to display HBTS version
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include <common.h>
#include <command.h>
#include "hbtsengine.h"
//#include "platform.h"

//extern unsigned char AV8100_FW_Downloaded;
int AV8100_FW_Downloaded=0;
unsigned char chip_version;
extern int print_cpuinfo(void);
extern int prcmu_i2c_read(u8 reg, u16 slave);
extern int prcmu_i2c_write(u8 reg, u16 slave, u8 reg_data);
//#define Read_AB8500_Register	prcmu_i2c_read
//#define Write_AB8500_Register	prcmu_i2c_write

int Read_AB8500_Register(char bank, char regAdd, char *regVal)
{
  int val;

  val=(char)prcmu_i2c_read(bank,regAdd);
  *regVal=(char)val;
  if(val<0)
    return(val);
  else
    return(0);

}

int BoardType(void)
{
  int result=-1;
#ifdef MOP_ED_BOARD
     return MOP_ED;
#endif
#ifdef MOP_V1_BOARD
     return MOP_V1;
#endif
#ifdef HREF_ED_BOARD
     return HREF_ED;
#endif
#ifdef HREF_V1_BOARD
     return HREF_V1;
#endif
#ifdef MOP_V2_BOARD
     return MOP_V2;
#endif
#ifdef HREF_PLUS_BOARD
     return HREF_PLUS;
#endif
#ifdef HREF_PLUS_2_0_BOARD
     return HREF_PLUS_2_0;
#endif
#ifdef HREF_PLUS_20_40_BOARD
     return HREF_PLUS_2_0_40;
#endif
#ifdef HREF_PLUS_20_60_BOARD
     return HREF_PLUS_2_0_60;
#endif
#ifdef HREF_PLUS_20_70_BOARD
     return HREF_PLUS_2_0_70;
#endif
#ifdef HREF_PLUS_22_10_BOARD
     return HREF_PLUS_2_2_10;
#endif
     return result;
}

void hbts_version()
{
  char data;
  volatile unsigned int *reg;
  extern char version_string[];
  int size_of_string = sizeof ("no AB8500 release");
  char ABxxxx_release[size_of_string + 1];
  char DBxxxx_release[size_of_string + 1];
  int v_ABxxxx_HDW=AB8500_HDW;
  int v_DBxxxx_HDW=DB8500_HDW;

	printf ("\n%s\n", version_string);
	print_cpuinfo();

	switch(v_ABxxxx_HDW)
	{
	case AB8500_ED_RELEASE:
	  strcpy(ABxxxx_release,"HBTS compiled for AB8500 ED");
	  break;
	case AB8500_V1_1_RELEASE:
	  strcpy(ABxxxx_release,"HBTS compiled for AB8500 V1.1");
	  break;
	case AB8500_V1_2_RELEASE:
	  strcpy(ABxxxx_release,"HBTS compiled for AB8500 V1.2 (MPW2)");
	  break;
	case AB8500_V2_RELEASE:
	  strcpy(ABxxxx_release,"HBTS compiled for AB8500 V2.0");
	  break;
	case AB8500_V3_RELEASE:
	  strcpy(ABxxxx_release,"HBTS compiled for AB8500 V3.0");
	  break;
	case AB8500_V3_3_RELEASE:
	  strcpy(ABxxxx_release,"HBTS compiled for AB8500 V3.3");
	  break;
	case AB9540_V1_0_RELEASE:
	  strcpy(ABxxxx_release,"HBTS compiled for AB9540 V1.0");
	  break;
	default:
	  strcpy(ABxxxx_release,"None specific compilation switch used for ABxxxx");
	  break;
	}

	switch(v_DBxxxx_HDW)
	{
	case DB8500_ED_RELEASE:
	  strcpy(DBxxxx_release,"HBTS compiled for DB8500 ED");
	  break;
	case DB8500_V1_RELEASE:
	  strcpy(DBxxxx_release,"HBTS compiled for DB8500 V1.0");
	  break;
	case DB8500_V1_1_RELEASE:
	  strcpy(DBxxxx_release,"HBTS compiled for DB8500 V1.1");
	  break;
	case DB8500_V2_RELEASE:
	  strcpy(DBxxxx_release,"HBTS compiled for DB8500 V2.0");
	  break;
	case DB8500_V2_1_RELEASE:
	  strcpy(DBxxxx_release,"HBTS compiled for DB8500 V2.1");
	  break;
	case DB8500_V2_2_RELEASE:
	  strcpy(DBxxxx_release,"HBTS compiled for DB8500 V2.2");
	  break;
	case DB9540_V1_0_RELEASE:
	  strcpy(DBxxxx_release,"HBTS compiled for DB9540 V1.0");
	  break;
	default:
	  strcpy(DBxxxx_release,"None specific compilation switch used for DBxxxx");
	  break;
	}

	printf ("\n");
	printf ("Build date   : %s\n", __DATE__);
	printf ("Build time   : %s\n", __TIME__);
	printf ("Build version: HBTS %s\n", HBTS_VERSION);
	printf ("-------------------\n");

	printf ("\nHBTS software released for :\n");
	printf ("-------------------\n");


	switch(BoardType())
	{
	  case MOP_ED:
	    printf ("%s\n%s\nMOP ED\n\n",DBxxxx_release, ABxxxx_release );
	    break;
	  case MOP_V1:
	    printf ("%s\n%s\nMOP V1\n\n",DBxxxx_release, ABxxxx_release );
	    break;
	  case MOP_V2:
	    printf ("%s\n%s\nMOP V2\n\n",DBxxxx_release, ABxxxx_release );
	    break;
	  case HREF_ED:
	    printf ("%s\n%s\nHREF ED\n\n",DBxxxx_release, ABxxxx_release );
	    break;
	  case HREF_V1:
	    printf ("%s\n%s\nHREF V1\n\n",DBxxxx_release, ABxxxx_release );
	    break;
	  case HREF_PLUS:
	    printf ("%s\n%s\nHREF PLUS\n\n",DBxxxx_release, ABxxxx_release );
	    break;
	  case HREF_PLUS_2_0:
	    printf ("%s\n%s\nHREF PLUS 2.0\n\n",DBxxxx_release, ABxxxx_release );
	    break;
	  case HREF_PLUS_2_0_40:
	    printf ("%s\n%s\nHREF PLUS 2.0 V40\n\n",DBxxxx_release, ABxxxx_release );
	    break;
	  case HREF_PLUS_2_0_60:
	    printf ("%s\n%s\nHREF PLUS 2.0 V60\n\n",DBxxxx_release, ABxxxx_release );
	    break;
	  case HREF_PLUS_2_2_10:
	    printf ("%s\n%s\nHREF PLUS 2.2 V10\n\n",DBxxxx_release, ABxxxx_release );
	    break;
	  default:
	    printf ("%s\n%s\nNo board defined\n\n",DBxxxx_release,ABxxxx_release );
	    break;
	}
#ifdef MICANA1_OFF
	printf("Analog microphone 1 not connected on board\n\n");
#endif

#ifdef MICANA1_ON
	printf("Analog microphone 1 connected on board\n\n");
#endif

#ifdef C1V2D_PIN_CONNECTED
	printf("AV8100 C1V2D pin testing enabled\n");
#else
	printf("AV8100 C1V2D pin testing not enabled\n");
#endif

#ifdef I2CAPE
		printf("AB8500 - DB8500 communication over I2CAPE  -  ");
#ifdef PRCMU_NO
	printf("No PRCMU\n");
#endif
#ifdef PRCMU_I2CAPE_REGULAR
	printf("PRCMU - I2CAPE working\n");
#endif
#ifdef PRCMU_I2CAPE_WORKAROUND
	printf("PRCMU - I2CAPE workaround\n");
#endif

#else
	printf("AB8500 - DB8500 communication over SSP0  ");
#endif

	printf("\n");

	printf ("ICs hardware cut on this board:\n");
	printf ("-------------------------------\n\n");
	printf ("DBxxxx : ");

	reg = (volatile unsigned int *)(DB8500_CUT_ID_REG);

	switch(*reg)
	{
	case DB8500_CUT_ID_ED:
	  printf ("cut ED");
	break;
	case DB8500_CUT_ID_V1:
	  printf ("cut 1");
	break;
	case DB8500_CUT_ID_V1_1:
	  printf ("cut 1.1");
	break;
	case DB8500_CUT_ID_V2_0:
	  printf ("cut 2.0");
	break;
	case DB8500_CUT_ID_V2_1:
	  printf ("cut 2.1");
	break;
	case DB8500_CUT_ID_V2_2:
	  printf ("cut 2.2");
	break;
	default:
	  printf ("UNKNOWN !!! 0x%08x",*reg);
	break;
	}

	/* AB8500 cut id ; register @0x1080 gives it!*/
	printf ("\nABxxxx : ");

	Read_AB8500_Register (0x10, 0x80, &data);
	printf ("cut %d.%d",(data>>4),(data & 0x0F));

	/* Av8100 cut id */
	printf ("\nAV8100 : ");
	if (AV8100_FW_Downloaded == 0)
	{
	  AV8100_FW_Downloaded = 1;
	  printf ("UNKNOWN.. type av8100_Firmware_Download command to have the AV8100 version");
	  printf("\n");
	  return;
	}
	switch (chip_version)
	{
	case AV8100_1_1_RELEASE:
	  printf ("cut 1.1");
	break;
	case AV8100_2_1_RELEASE:
	  printf ("cut 2.1");
	break;
	case AV8100_2_2_RELEASE:
	  printf ("cut 2.2");
	break;
	default:
	  printf ("UNKNOWN...");
	break;
	}
	printf("\n");
}


int do_version (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	hbts_version();
	return 0;
}

U_BOOT_CMD(
	version,	1,		1,	do_version,
	"version - print monitor and HBTS versions\n",
	NULL
);
