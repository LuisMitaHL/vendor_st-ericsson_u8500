/*****************************************************************************/
/**
*  ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides services for power services module
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include "tatpws.h"

int tatpws_GpioSettings_Mode = 0;

/* For the configuration of the GPIO registers during power saving mode*/
/*  9 banks */
GpioReg a_GPIO_Config_For_PWS_Mode[TATPWS_NB_GPIO] = { {0, 0, 0, 0} };

/* For the restore of the GPIO registers after power saving mode*/
/*  9 banks */
GpioReg a_GPIO_Config_Initial[TATPWS_NB_GPIO] = { {0, 0, 0, 0} };

/* Fct to decode the gpio config from the registers raw values			     */
/*****************************************************************************/
static void tatpws1_GPIO_Decode_Config(void)
{
	int vl_Bank = 0;
	int vl_Gpio = 0;
	int vl_AFSELA = 0;
	int vl_AFSELB = 0;
	int vl_DIR = 0;
	int vl_PDIS = 0;
	int vl_DAT = 0;
	FILE *fd;
	char newline[MAX_CHAR_PER_LINE] = "";

	fd = fopen("/usr/local/lib/tat/tatlpws/gpios_decoding.txt", "w");
	if (fd != NULL) {

		for (vl_Bank = 0; vl_Bank < TATPWS_NB_BANKS; vl_Bank++) {

			for (vl_Gpio = 0; vl_Gpio < 32; vl_Gpio++) {

				vl_AFSELA = ((a_GPIO_Config_For_PWS_Mode[TATPWS_MAX_GPIO*vl_Bank+TATPWS_GPIO_AFSELA].v_Value>>vl_Gpio) & 0x1);
				vl_AFSELB = ((a_GPIO_Config_For_PWS_Mode[TATPWS_MAX_GPIO*vl_Bank+TATPWS_GPIO_AFSELB].v_Value>>vl_Gpio) & 0x1);
				vl_PDIS = ((a_GPIO_Config_For_PWS_Mode[TATPWS_MAX_GPIO*vl_Bank+TATPWS_GPIO_PDIS].v_Value>>vl_Gpio) & 0x1);

				if (((a_GPIO_Config_For_PWS_Mode[TATPWS_MAX_GPIO*vl_Bank+TATPWS_GPIO_DATS].v_Value>>vl_Gpio) & 0x1) == 1)
					vl_DAT = 1;
				else if (((a_GPIO_Config_For_PWS_Mode[TATPWS_MAX_GPIO*vl_Bank+TATPWS_GPIO_DATC].v_Value>>vl_Gpio) & 0x1) == 1)
					vl_DAT = 0;
				else
					vl_DAT = ((a_GPIO_Config_For_PWS_Mode[TATPWS_MAX_GPIO*vl_Bank+TATPWS_GPIO_DAT].v_Value>>vl_Gpio) & 0x1);

				if (((a_GPIO_Config_For_PWS_Mode[TATPWS_MAX_GPIO*vl_Bank+TATPWS_GPIO_DIRS].v_Value>>vl_Gpio) & 0x1) == 1)
					vl_DIR = 1;
				else if (((a_GPIO_Config_For_PWS_Mode[TATPWS_MAX_GPIO*vl_Bank+TATPWS_GPIO_DIRC].v_Value>>vl_Gpio) & 0x1) == 1)
					vl_DIR = 0;
				else
					vl_DIR = ((a_GPIO_Config_For_PWS_Mode[TATPWS_MAX_GPIO*vl_Bank+TATPWS_GPIO_DIR].v_Value>>vl_Gpio) & 0x1);

				/* determine if the GPIO is configure as GPIO or alternate function*/
				if ((vl_AFSELA == 0) && (vl_AFSELB == 0)) {
					/* GPIO */
					snprintf(newline, MAX_CHAR_PER_LINE, " GPIO%i_%i : Pin is GPIO", vl_Bank, vl_Gpio);

					/* direction */
					if (vl_DIR == 0)
						strncat(newline, ", input", MAX_CHAR_PER_LINE-strlen(newline));
					else
						strncat(newline, ", output", MAX_CHAR_PER_LINE-strlen(newline));

					/* pull-up/down	*/
					if (vl_PDIS == 1) {
						strncat(newline, ", pull-up/down disabled par PDIS.\n", MAX_CHAR_PER_LINE-strlen(newline));
					} else {
						if (vl_DAT == 0)
							strncat(newline, ", pull-down enabled.\n", MAX_CHAR_PER_LINE-strlen(newline));
						else
							strncat(newline, ", pull-up enabled.\n", MAX_CHAR_PER_LINE-strlen(newline));
					}

				} else {
					/* alternate function */
					snprintf(newline, MAX_CHAR_PER_LINE, " GPIO%i_%i : Pin is alternate function (input or output)", vl_Bank, vl_Gpio);

					/* pull-up/down	*/
					if (vl_PDIS == 1) {
						strncat(newline, ", pull-up/down disabled par PDIS.\n", MAX_CHAR_PER_LINE-strlen(newline));
					} else {
						if (vl_DAT == 0)
							strncat(newline, ", pull-down enabled.\n", MAX_CHAR_PER_LINE-strlen(newline));
						else
							strncat(newline, ", pull-up enabled.\n", MAX_CHAR_PER_LINE-strlen(newline));
					}
				}
				/* write the line in the txt file */
				fputs(newline, fd);
			}
		}

		fclose(fd);
	} else {
		PWS_SYSLOG(LOG_DEBUG, "Can't open /usr/local/lib/tat/tatlpws/gpios_decoding.txt!");
	}
}

/* Initialize GPIO configuration structure									 */
/*****************************************************************************/
void tatpws1_GPIO_Settings_Init()
{
	unsigned int vl_BankBaseAddr = 0;
	FILE *fd = NULL;
	int i;

	/*****************************/
	/* @ initialisation*/
	/*****************************/
	fd = fopen("/usr/local/lib/tat/tatlpws/gpiosaddr.csv","r");
	if (fd == NULL) {
		/* static management of the registers addresses*/
		PWS_SYSLOG(LOG_DEBUG, "static management of the registers addresses");

		vl_BankBaseAddr = 0x8012e000;
		a_GPIO_Config_For_PWS_Mode[TATPWS_GPIO_DAT].v_Address = a_GPIO_Config_Initial[TATPWS_GPIO_DAT].v_Address = vl_BankBaseAddr ;
		a_GPIO_Config_For_PWS_Mode[TATPWS_GPIO_DATS].v_Address = a_GPIO_Config_Initial[TATPWS_GPIO_DATS].v_Address = vl_BankBaseAddr +0x04;
		a_GPIO_Config_For_PWS_Mode[TATPWS_GPIO_DATC].v_Address = a_GPIO_Config_Initial[TATPWS_GPIO_DATC].v_Address = vl_BankBaseAddr +0x08;
		a_GPIO_Config_For_PWS_Mode[TATPWS_GPIO_PDIS].v_Address = a_GPIO_Config_Initial[TATPWS_GPIO_PDIS].v_Address = vl_BankBaseAddr +0x0c;	
		a_GPIO_Config_For_PWS_Mode[TATPWS_GPIO_DIR].v_Address = a_GPIO_Config_Initial[TATPWS_GPIO_DIR].v_Address =  vl_BankBaseAddr +0x10;
		a_GPIO_Config_For_PWS_Mode[TATPWS_GPIO_DIRS].v_Address = a_GPIO_Config_Initial[TATPWS_GPIO_DIRS].v_Address =  vl_BankBaseAddr +0x14;
		a_GPIO_Config_For_PWS_Mode[TATPWS_GPIO_DIRC].v_Address = a_GPIO_Config_Initial[TATPWS_GPIO_DIRC].v_Address =  vl_BankBaseAddr +0x18;
		a_GPIO_Config_For_PWS_Mode[TATPWS_GPIO_SLMP].v_Address = a_GPIO_Config_Initial[TATPWS_GPIO_SLMP].v_Address =  vl_BankBaseAddr +0x1c;
		a_GPIO_Config_For_PWS_Mode[TATPWS_GPIO_AFSELA].v_Address = a_GPIO_Config_Initial[TATPWS_GPIO_AFSELA].v_Address =  vl_BankBaseAddr +0x20;
		a_GPIO_Config_For_PWS_Mode[TATPWS_GPIO_AFSELB].v_Address = a_GPIO_Config_Initial[TATPWS_GPIO_AFSELB].v_Address =  vl_BankBaseAddr +0x24;
		a_GPIO_Config_For_PWS_Mode[TATPWS_GPIO_RWMSC].v_Address = a_GPIO_Config_Initial[TATPWS_GPIO_RWMSC].v_Address =  vl_BankBaseAddr +0x50;
		a_GPIO_Config_For_PWS_Mode[TATPWS_GPIO_FWMSC].v_Address = a_GPIO_Config_Initial[TATPWS_GPIO_FWMSC].v_Address =  vl_BankBaseAddr +0x54;

		vl_BankBaseAddr = 0x8012e080;
		a_GPIO_Config_For_PWS_Mode[1*TATPWS_MAX_GPIO+TATPWS_GPIO_DAT].v_Address = a_GPIO_Config_Initial[1*TATPWS_MAX_GPIO +TATPWS_GPIO_DAT].v_Address = vl_BankBaseAddr ;
		a_GPIO_Config_For_PWS_Mode[1*TATPWS_MAX_GPIO+TATPWS_GPIO_DATS].v_Address = a_GPIO_Config_Initial[1*TATPWS_MAX_GPIO +TATPWS_GPIO_DATS].v_Address = vl_BankBaseAddr +0x04;
		a_GPIO_Config_For_PWS_Mode[1*TATPWS_MAX_GPIO+TATPWS_GPIO_DATC].v_Address = a_GPIO_Config_Initial[1*TATPWS_MAX_GPIO +TATPWS_GPIO_DATC].v_Address = vl_BankBaseAddr +0x08;
		a_GPIO_Config_For_PWS_Mode[1*TATPWS_MAX_GPIO+TATPWS_GPIO_PDIS].v_Address = a_GPIO_Config_Initial[1*TATPWS_MAX_GPIO +TATPWS_GPIO_PDIS].v_Address = vl_BankBaseAddr +0x0c;	
		a_GPIO_Config_For_PWS_Mode[1*TATPWS_MAX_GPIO+TATPWS_GPIO_DIR].v_Address = a_GPIO_Config_Initial[1*TATPWS_MAX_GPIO +TATPWS_GPIO_DIR].v_Address = vl_BankBaseAddr +0x10;
		a_GPIO_Config_For_PWS_Mode[1*TATPWS_MAX_GPIO+TATPWS_GPIO_DIRS].v_Address = a_GPIO_Config_Initial[1*TATPWS_MAX_GPIO +TATPWS_GPIO_DIRS].v_Address = vl_BankBaseAddr +0x14;
		a_GPIO_Config_For_PWS_Mode[1*TATPWS_MAX_GPIO+TATPWS_GPIO_DIRC].v_Address = a_GPIO_Config_Initial[1*TATPWS_MAX_GPIO +TATPWS_GPIO_DIRC].v_Address = vl_BankBaseAddr +0x18;
		a_GPIO_Config_For_PWS_Mode[1*TATPWS_MAX_GPIO+TATPWS_GPIO_SLMP].v_Address = a_GPIO_Config_Initial[1*TATPWS_MAX_GPIO +TATPWS_GPIO_SLMP].v_Address = vl_BankBaseAddr +0x1c;
		a_GPIO_Config_For_PWS_Mode[1*TATPWS_MAX_GPIO+TATPWS_GPIO_AFSELA].v_Address = a_GPIO_Config_Initial[1*TATPWS_MAX_GPIO +TATPWS_GPIO_AFSELA].v_Address = vl_BankBaseAddr +0x20;
		a_GPIO_Config_For_PWS_Mode[1*TATPWS_MAX_GPIO+TATPWS_GPIO_AFSELB].v_Address = a_GPIO_Config_Initial[1*TATPWS_MAX_GPIO +TATPWS_GPIO_AFSELB].v_Address = vl_BankBaseAddr +0x24;
		a_GPIO_Config_For_PWS_Mode[1*TATPWS_MAX_GPIO+TATPWS_GPIO_RWMSC].v_Address = a_GPIO_Config_Initial[1*TATPWS_MAX_GPIO +TATPWS_GPIO_RWMSC].v_Address = vl_BankBaseAddr +0x50;
		a_GPIO_Config_For_PWS_Mode[1*TATPWS_MAX_GPIO+TATPWS_GPIO_FWMSC].v_Address = a_GPIO_Config_Initial[1*TATPWS_MAX_GPIO +TATPWS_GPIO_FWMSC].v_Address = vl_BankBaseAddr +0x54;

		vl_BankBaseAddr = 0x8000e000;
		a_GPIO_Config_For_PWS_Mode[2*TATPWS_MAX_GPIO+TATPWS_GPIO_DAT].v_Address = a_GPIO_Config_Initial[2*TATPWS_MAX_GPIO +TATPWS_GPIO_DAT].v_Address = vl_BankBaseAddr ;
		a_GPIO_Config_For_PWS_Mode[2*TATPWS_MAX_GPIO+TATPWS_GPIO_DATS].v_Address = a_GPIO_Config_Initial[2*TATPWS_MAX_GPIO +TATPWS_GPIO_DATS].v_Address = vl_BankBaseAddr +0x04;
		a_GPIO_Config_For_PWS_Mode[2*TATPWS_MAX_GPIO+TATPWS_GPIO_DATC].v_Address = a_GPIO_Config_Initial[2*TATPWS_MAX_GPIO +TATPWS_GPIO_DATC].v_Address = vl_BankBaseAddr +0x08;
		a_GPIO_Config_For_PWS_Mode[2*TATPWS_MAX_GPIO+TATPWS_GPIO_PDIS].v_Address = a_GPIO_Config_Initial[2*TATPWS_MAX_GPIO +TATPWS_GPIO_PDIS].v_Address = vl_BankBaseAddr +0x0c;	
		a_GPIO_Config_For_PWS_Mode[2*TATPWS_MAX_GPIO+TATPWS_GPIO_DIR].v_Address = a_GPIO_Config_Initial[2*TATPWS_MAX_GPIO +TATPWS_GPIO_DIR].v_Address =  vl_BankBaseAddr +0x10;
		a_GPIO_Config_For_PWS_Mode[2*TATPWS_MAX_GPIO+TATPWS_GPIO_DIRS].v_Address = a_GPIO_Config_Initial[2*TATPWS_MAX_GPIO +TATPWS_GPIO_DIRS].v_Address =  vl_BankBaseAddr +0x14;
		a_GPIO_Config_For_PWS_Mode[2*TATPWS_MAX_GPIO+TATPWS_GPIO_DIRC].v_Address = a_GPIO_Config_Initial[2*TATPWS_MAX_GPIO +TATPWS_GPIO_DIRC].v_Address =  vl_BankBaseAddr +0x18;
		a_GPIO_Config_For_PWS_Mode[2*TATPWS_MAX_GPIO+TATPWS_GPIO_SLMP].v_Address = a_GPIO_Config_Initial[2*TATPWS_MAX_GPIO +TATPWS_GPIO_SLMP].v_Address =  vl_BankBaseAddr +0x1c;
		a_GPIO_Config_For_PWS_Mode[2*TATPWS_MAX_GPIO+TATPWS_GPIO_AFSELA].v_Address = a_GPIO_Config_Initial[2*TATPWS_MAX_GPIO +TATPWS_GPIO_AFSELA].v_Address =  vl_BankBaseAddr +0x20;
		a_GPIO_Config_For_PWS_Mode[2*TATPWS_MAX_GPIO+TATPWS_GPIO_AFSELB].v_Address = a_GPIO_Config_Initial[2*TATPWS_MAX_GPIO +TATPWS_GPIO_AFSELB].v_Address =  vl_BankBaseAddr +0x24;
		a_GPIO_Config_For_PWS_Mode[2*TATPWS_MAX_GPIO+TATPWS_GPIO_RWMSC].v_Address = a_GPIO_Config_Initial[2*TATPWS_MAX_GPIO +TATPWS_GPIO_RWMSC].v_Address =  vl_BankBaseAddr +0x50;
		a_GPIO_Config_For_PWS_Mode[2*TATPWS_MAX_GPIO+TATPWS_GPIO_FWMSC].v_Address = a_GPIO_Config_Initial[2*TATPWS_MAX_GPIO +TATPWS_GPIO_FWMSC].v_Address =  vl_BankBaseAddr +0x54;

		vl_BankBaseAddr = 0x8000e080;
		a_GPIO_Config_For_PWS_Mode[3*TATPWS_MAX_GPIO+TATPWS_GPIO_DAT].v_Address = a_GPIO_Config_Initial[3*TATPWS_MAX_GPIO +TATPWS_GPIO_DAT].v_Address = vl_BankBaseAddr ;
		a_GPIO_Config_For_PWS_Mode[3*TATPWS_MAX_GPIO+TATPWS_GPIO_DATS].v_Address = a_GPIO_Config_Initial[3*TATPWS_MAX_GPIO +TATPWS_GPIO_DATS].v_Address = vl_BankBaseAddr +0x04;
		a_GPIO_Config_For_PWS_Mode[3*TATPWS_MAX_GPIO+TATPWS_GPIO_DATC].v_Address = a_GPIO_Config_Initial[3*TATPWS_MAX_GPIO +TATPWS_GPIO_DATC].v_Address = vl_BankBaseAddr +0x08;
		a_GPIO_Config_For_PWS_Mode[3*TATPWS_MAX_GPIO+TATPWS_GPIO_PDIS].v_Address = a_GPIO_Config_Initial[3*TATPWS_MAX_GPIO +TATPWS_GPIO_PDIS].v_Address = vl_BankBaseAddr +0x0c;	
		a_GPIO_Config_For_PWS_Mode[3*TATPWS_MAX_GPIO+TATPWS_GPIO_DIR].v_Address = a_GPIO_Config_Initial[3*TATPWS_MAX_GPIO +TATPWS_GPIO_DIR].v_Address =  vl_BankBaseAddr +0x10;
		a_GPIO_Config_For_PWS_Mode[3*TATPWS_MAX_GPIO+TATPWS_GPIO_DIRS].v_Address = a_GPIO_Config_Initial[3*TATPWS_MAX_GPIO +TATPWS_GPIO_DIRS].v_Address =  vl_BankBaseAddr +0x14;
		a_GPIO_Config_For_PWS_Mode[3*TATPWS_MAX_GPIO+TATPWS_GPIO_DIRC].v_Address = a_GPIO_Config_Initial[3*TATPWS_MAX_GPIO +TATPWS_GPIO_DIRC].v_Address =  vl_BankBaseAddr +0x18;
		a_GPIO_Config_For_PWS_Mode[3*TATPWS_MAX_GPIO+TATPWS_GPIO_SLMP].v_Address = a_GPIO_Config_Initial[3*TATPWS_MAX_GPIO +TATPWS_GPIO_SLMP].v_Address =  vl_BankBaseAddr +0x1c;
		a_GPIO_Config_For_PWS_Mode[3*TATPWS_MAX_GPIO+TATPWS_GPIO_AFSELA].v_Address = a_GPIO_Config_Initial[3*TATPWS_MAX_GPIO +TATPWS_GPIO_AFSELA].v_Address =  vl_BankBaseAddr +0x20;
		a_GPIO_Config_For_PWS_Mode[3*TATPWS_MAX_GPIO+TATPWS_GPIO_AFSELB].v_Address = a_GPIO_Config_Initial[3*TATPWS_MAX_GPIO +TATPWS_GPIO_AFSELB].v_Address =  vl_BankBaseAddr +0x24;
		a_GPIO_Config_For_PWS_Mode[3*TATPWS_MAX_GPIO+TATPWS_GPIO_RWMSC].v_Address = a_GPIO_Config_Initial[3*TATPWS_MAX_GPIO +TATPWS_GPIO_RWMSC].v_Address =  vl_BankBaseAddr +0x50;
		a_GPIO_Config_For_PWS_Mode[3*TATPWS_MAX_GPIO+TATPWS_GPIO_FWMSC].v_Address = a_GPIO_Config_Initial[3*TATPWS_MAX_GPIO +TATPWS_GPIO_FWMSC].v_Address =  vl_BankBaseAddr +0x54;

		vl_BankBaseAddr = 0x8000e100;
		a_GPIO_Config_For_PWS_Mode[4*TATPWS_MAX_GPIO+TATPWS_GPIO_DAT].v_Address = a_GPIO_Config_Initial[4*TATPWS_MAX_GPIO +TATPWS_GPIO_DAT].v_Address = vl_BankBaseAddr ;
		a_GPIO_Config_For_PWS_Mode[4*TATPWS_MAX_GPIO+TATPWS_GPIO_DATS].v_Address = a_GPIO_Config_Initial[4*TATPWS_MAX_GPIO +TATPWS_GPIO_DATS].v_Address = vl_BankBaseAddr +0x04;
		a_GPIO_Config_For_PWS_Mode[4*TATPWS_MAX_GPIO+TATPWS_GPIO_DATC].v_Address = a_GPIO_Config_Initial[4*TATPWS_MAX_GPIO +TATPWS_GPIO_DATC].v_Address = vl_BankBaseAddr +0x08;
		a_GPIO_Config_For_PWS_Mode[4*TATPWS_MAX_GPIO+TATPWS_GPIO_PDIS].v_Address = a_GPIO_Config_Initial[4*TATPWS_MAX_GPIO +TATPWS_GPIO_PDIS].v_Address = vl_BankBaseAddr +0x0c;	
		a_GPIO_Config_For_PWS_Mode[4*TATPWS_MAX_GPIO+TATPWS_GPIO_DIR].v_Address = a_GPIO_Config_Initial[4*TATPWS_MAX_GPIO +TATPWS_GPIO_DIR].v_Address =  vl_BankBaseAddr +0x10;
		a_GPIO_Config_For_PWS_Mode[4*TATPWS_MAX_GPIO+TATPWS_GPIO_DIRS].v_Address = a_GPIO_Config_Initial[4*TATPWS_MAX_GPIO +TATPWS_GPIO_DIRS].v_Address =  vl_BankBaseAddr +0x14;
		a_GPIO_Config_For_PWS_Mode[4*TATPWS_MAX_GPIO+TATPWS_GPIO_DIRC].v_Address = a_GPIO_Config_Initial[4*TATPWS_MAX_GPIO +TATPWS_GPIO_DIRC].v_Address =  vl_BankBaseAddr +0x18;
		a_GPIO_Config_For_PWS_Mode[4*TATPWS_MAX_GPIO+TATPWS_GPIO_SLMP].v_Address = a_GPIO_Config_Initial[4*TATPWS_MAX_GPIO +TATPWS_GPIO_SLMP].v_Address =  vl_BankBaseAddr +0x1c;
		a_GPIO_Config_For_PWS_Mode[4*TATPWS_MAX_GPIO+TATPWS_GPIO_AFSELA].v_Address = a_GPIO_Config_Initial[4*TATPWS_MAX_GPIO +TATPWS_GPIO_AFSELA].v_Address =  vl_BankBaseAddr +0x20;
		a_GPIO_Config_For_PWS_Mode[4*TATPWS_MAX_GPIO+TATPWS_GPIO_AFSELB].v_Address = a_GPIO_Config_Initial[4*TATPWS_MAX_GPIO +TATPWS_GPIO_AFSELB].v_Address =  vl_BankBaseAddr +0x24;
		a_GPIO_Config_For_PWS_Mode[4*TATPWS_MAX_GPIO+TATPWS_GPIO_RWMSC].v_Address = a_GPIO_Config_Initial[4*TATPWS_MAX_GPIO +TATPWS_GPIO_RWMSC].v_Address =  vl_BankBaseAddr +0x50;
		a_GPIO_Config_For_PWS_Mode[4*TATPWS_MAX_GPIO+TATPWS_GPIO_FWMSC].v_Address = a_GPIO_Config_Initial[4*TATPWS_MAX_GPIO +TATPWS_GPIO_FWMSC].v_Address =  vl_BankBaseAddr +0x54;

		vl_BankBaseAddr = 0x8000e180;
		a_GPIO_Config_For_PWS_Mode[5*TATPWS_MAX_GPIO+TATPWS_GPIO_DAT].v_Address = a_GPIO_Config_Initial[5*TATPWS_MAX_GPIO +TATPWS_GPIO_DAT].v_Address = vl_BankBaseAddr ;
		a_GPIO_Config_For_PWS_Mode[5*TATPWS_MAX_GPIO+TATPWS_GPIO_DATS].v_Address = a_GPIO_Config_Initial[5*TATPWS_MAX_GPIO +TATPWS_GPIO_DATS].v_Address = vl_BankBaseAddr +0x04;
		a_GPIO_Config_For_PWS_Mode[5*TATPWS_MAX_GPIO+TATPWS_GPIO_DATC].v_Address = a_GPIO_Config_Initial[5*TATPWS_MAX_GPIO +TATPWS_GPIO_DATC].v_Address = vl_BankBaseAddr +0x08;
		a_GPIO_Config_For_PWS_Mode[5*TATPWS_MAX_GPIO+TATPWS_GPIO_PDIS].v_Address = a_GPIO_Config_Initial[5*TATPWS_MAX_GPIO +TATPWS_GPIO_PDIS].v_Address = vl_BankBaseAddr +0x0c;	
		a_GPIO_Config_For_PWS_Mode[5*TATPWS_MAX_GPIO+TATPWS_GPIO_DIR].v_Address = a_GPIO_Config_Initial[5*TATPWS_MAX_GPIO +TATPWS_GPIO_DIR].v_Address =  vl_BankBaseAddr +0x10;
		a_GPIO_Config_For_PWS_Mode[5*TATPWS_MAX_GPIO+TATPWS_GPIO_DIRS].v_Address = a_GPIO_Config_Initial[5*TATPWS_MAX_GPIO +TATPWS_GPIO_DIRS].v_Address =  vl_BankBaseAddr +0x14;
		a_GPIO_Config_For_PWS_Mode[5*TATPWS_MAX_GPIO+TATPWS_GPIO_DIRC].v_Address = a_GPIO_Config_Initial[5*TATPWS_MAX_GPIO +TATPWS_GPIO_DIRC].v_Address =  vl_BankBaseAddr +0x18;
		a_GPIO_Config_For_PWS_Mode[5*TATPWS_MAX_GPIO+TATPWS_GPIO_SLMP].v_Address = a_GPIO_Config_Initial[5*TATPWS_MAX_GPIO +TATPWS_GPIO_SLMP].v_Address =  vl_BankBaseAddr +0x1c;
		a_GPIO_Config_For_PWS_Mode[5*TATPWS_MAX_GPIO+TATPWS_GPIO_AFSELA].v_Address = a_GPIO_Config_Initial[5*TATPWS_MAX_GPIO +TATPWS_GPIO_AFSELA].v_Address =  vl_BankBaseAddr +0x20;
		a_GPIO_Config_For_PWS_Mode[5*TATPWS_MAX_GPIO+TATPWS_GPIO_AFSELB].v_Address = a_GPIO_Config_Initial[5*TATPWS_MAX_GPIO +TATPWS_GPIO_AFSELB].v_Address =  vl_BankBaseAddr +0x24;
		a_GPIO_Config_For_PWS_Mode[5*TATPWS_MAX_GPIO+TATPWS_GPIO_RWMSC].v_Address = a_GPIO_Config_Initial[5*TATPWS_MAX_GPIO +TATPWS_GPIO_RWMSC].v_Address =  vl_BankBaseAddr +0x50;
		a_GPIO_Config_For_PWS_Mode[5*TATPWS_MAX_GPIO+TATPWS_GPIO_FWMSC].v_Address = a_GPIO_Config_Initial[5*TATPWS_MAX_GPIO +TATPWS_GPIO_FWMSC].v_Address =  vl_BankBaseAddr +0x54;

		vl_BankBaseAddr = 0x8011e000;
		a_GPIO_Config_For_PWS_Mode[6*TATPWS_MAX_GPIO+TATPWS_GPIO_DAT].v_Address = a_GPIO_Config_Initial[6*TATPWS_MAX_GPIO +TATPWS_GPIO_DAT].v_Address = vl_BankBaseAddr ;
		a_GPIO_Config_For_PWS_Mode[6*TATPWS_MAX_GPIO+TATPWS_GPIO_DATS].v_Address = a_GPIO_Config_Initial[6*TATPWS_MAX_GPIO +TATPWS_GPIO_DATS].v_Address = vl_BankBaseAddr +0x04;
		a_GPIO_Config_For_PWS_Mode[6*TATPWS_MAX_GPIO+TATPWS_GPIO_DATC].v_Address = a_GPIO_Config_Initial[6*TATPWS_MAX_GPIO +TATPWS_GPIO_DATC].v_Address = vl_BankBaseAddr +0x08;
		a_GPIO_Config_For_PWS_Mode[6*TATPWS_MAX_GPIO+TATPWS_GPIO_PDIS].v_Address = a_GPIO_Config_Initial[6*TATPWS_MAX_GPIO +TATPWS_GPIO_PDIS].v_Address = vl_BankBaseAddr +0x0c;	
		a_GPIO_Config_For_PWS_Mode[6*TATPWS_MAX_GPIO+TATPWS_GPIO_DIR].v_Address = a_GPIO_Config_Initial[6*TATPWS_MAX_GPIO +TATPWS_GPIO_DIR].v_Address =  vl_BankBaseAddr +0x10;
		a_GPIO_Config_For_PWS_Mode[6*TATPWS_MAX_GPIO+TATPWS_GPIO_DIRS].v_Address = a_GPIO_Config_Initial[6*TATPWS_MAX_GPIO +TATPWS_GPIO_DIRS].v_Address =  vl_BankBaseAddr +0x14;
		a_GPIO_Config_For_PWS_Mode[6*TATPWS_MAX_GPIO+TATPWS_GPIO_DIRC].v_Address = a_GPIO_Config_Initial[6*TATPWS_MAX_GPIO +TATPWS_GPIO_DIRC].v_Address =  vl_BankBaseAddr +0x18;
		a_GPIO_Config_For_PWS_Mode[6*TATPWS_MAX_GPIO+TATPWS_GPIO_SLMP].v_Address = a_GPIO_Config_Initial[6*TATPWS_MAX_GPIO +TATPWS_GPIO_SLMP].v_Address =  vl_BankBaseAddr +0x1c;
		a_GPIO_Config_For_PWS_Mode[6*TATPWS_MAX_GPIO+TATPWS_GPIO_AFSELA].v_Address = a_GPIO_Config_Initial[6*TATPWS_MAX_GPIO +TATPWS_GPIO_AFSELA].v_Address =  vl_BankBaseAddr +0x20;
		a_GPIO_Config_For_PWS_Mode[6*TATPWS_MAX_GPIO+TATPWS_GPIO_AFSELB].v_Address = a_GPIO_Config_Initial[6*TATPWS_MAX_GPIO +TATPWS_GPIO_AFSELB].v_Address =  vl_BankBaseAddr +0x24;
		a_GPIO_Config_For_PWS_Mode[6*TATPWS_MAX_GPIO+TATPWS_GPIO_RWMSC].v_Address = a_GPIO_Config_Initial[6*TATPWS_MAX_GPIO +TATPWS_GPIO_RWMSC].v_Address =  vl_BankBaseAddr +0x50;
		a_GPIO_Config_For_PWS_Mode[6*TATPWS_MAX_GPIO+TATPWS_GPIO_FWMSC].v_Address = a_GPIO_Config_Initial[6*TATPWS_MAX_GPIO +TATPWS_GPIO_FWMSC].v_Address =  vl_BankBaseAddr +0x54;

		vl_BankBaseAddr = 0x8011e080;
		a_GPIO_Config_For_PWS_Mode[7*TATPWS_MAX_GPIO+TATPWS_GPIO_DAT].v_Address = a_GPIO_Config_Initial[7*TATPWS_MAX_GPIO +TATPWS_GPIO_DAT].v_Address = vl_BankBaseAddr ;
		a_GPIO_Config_For_PWS_Mode[7*TATPWS_MAX_GPIO+TATPWS_GPIO_DATS].v_Address = a_GPIO_Config_Initial[7*TATPWS_MAX_GPIO +TATPWS_GPIO_DATS].v_Address = vl_BankBaseAddr +0x04;
		a_GPIO_Config_For_PWS_Mode[7*TATPWS_MAX_GPIO+TATPWS_GPIO_DATC].v_Address = a_GPIO_Config_Initial[7*TATPWS_MAX_GPIO +TATPWS_GPIO_DATC].v_Address = vl_BankBaseAddr +0x08;
		a_GPIO_Config_For_PWS_Mode[7*TATPWS_MAX_GPIO+TATPWS_GPIO_PDIS].v_Address = a_GPIO_Config_Initial[7*TATPWS_MAX_GPIO +TATPWS_GPIO_PDIS].v_Address = vl_BankBaseAddr +0x0c;	
		a_GPIO_Config_For_PWS_Mode[7*TATPWS_MAX_GPIO+TATPWS_GPIO_DIR].v_Address = a_GPIO_Config_Initial[7*TATPWS_MAX_GPIO +TATPWS_GPIO_DIR].v_Address =  vl_BankBaseAddr +0x10;
		a_GPIO_Config_For_PWS_Mode[7*TATPWS_MAX_GPIO+TATPWS_GPIO_DIRS].v_Address = a_GPIO_Config_Initial[7*TATPWS_MAX_GPIO +TATPWS_GPIO_DIRS].v_Address =  vl_BankBaseAddr +0x14;
		a_GPIO_Config_For_PWS_Mode[7*TATPWS_MAX_GPIO+TATPWS_GPIO_DIRC].v_Address = a_GPIO_Config_Initial[7*TATPWS_MAX_GPIO +TATPWS_GPIO_DIRC].v_Address =  vl_BankBaseAddr +0x18;
		a_GPIO_Config_For_PWS_Mode[7*TATPWS_MAX_GPIO+TATPWS_GPIO_SLMP].v_Address = a_GPIO_Config_Initial[7*TATPWS_MAX_GPIO +TATPWS_GPIO_SLMP].v_Address =  vl_BankBaseAddr +0x1c;
		a_GPIO_Config_For_PWS_Mode[7*TATPWS_MAX_GPIO+TATPWS_GPIO_AFSELA].v_Address = a_GPIO_Config_Initial[7*TATPWS_MAX_GPIO +TATPWS_GPIO_AFSELA].v_Address =  vl_BankBaseAddr +0x20;
		a_GPIO_Config_For_PWS_Mode[7*TATPWS_MAX_GPIO+TATPWS_GPIO_AFSELB].v_Address = a_GPIO_Config_Initial[7*TATPWS_MAX_GPIO +TATPWS_GPIO_AFSELB].v_Address =  vl_BankBaseAddr +0x24;
		a_GPIO_Config_For_PWS_Mode[7*TATPWS_MAX_GPIO+TATPWS_GPIO_RWMSC].v_Address = a_GPIO_Config_Initial[7*TATPWS_MAX_GPIO +TATPWS_GPIO_RWMSC].v_Address =  vl_BankBaseAddr +0x50;
		a_GPIO_Config_For_PWS_Mode[7*TATPWS_MAX_GPIO+TATPWS_GPIO_FWMSC].v_Address = a_GPIO_Config_Initial[7*TATPWS_MAX_GPIO +TATPWS_GPIO_FWMSC].v_Address =  vl_BankBaseAddr +0x54;

		vl_BankBaseAddr=0xa03fe000;
		a_GPIO_Config_For_PWS_Mode[8*TATPWS_MAX_GPIO+TATPWS_GPIO_DAT].v_Address = a_GPIO_Config_Initial[8*TATPWS_MAX_GPIO +TATPWS_GPIO_DAT].v_Address = vl_BankBaseAddr ;
		a_GPIO_Config_For_PWS_Mode[8*TATPWS_MAX_GPIO+TATPWS_GPIO_DATS].v_Address = a_GPIO_Config_Initial[8*TATPWS_MAX_GPIO +TATPWS_GPIO_DATS].v_Address = vl_BankBaseAddr +0x04;
		a_GPIO_Config_For_PWS_Mode[8*TATPWS_MAX_GPIO+TATPWS_GPIO_DATC].v_Address = a_GPIO_Config_Initial[8*TATPWS_MAX_GPIO +TATPWS_GPIO_DATC].v_Address = vl_BankBaseAddr +0x08;
		a_GPIO_Config_For_PWS_Mode[8*TATPWS_MAX_GPIO+TATPWS_GPIO_PDIS].v_Address = a_GPIO_Config_Initial[8*TATPWS_MAX_GPIO +TATPWS_GPIO_PDIS].v_Address = vl_BankBaseAddr +0x0c;	
		a_GPIO_Config_For_PWS_Mode[8*TATPWS_MAX_GPIO+TATPWS_GPIO_DIR].v_Address = a_GPIO_Config_Initial[8*TATPWS_MAX_GPIO +TATPWS_GPIO_DIR].v_Address =  vl_BankBaseAddr +0x10;
		a_GPIO_Config_For_PWS_Mode[8*TATPWS_MAX_GPIO+TATPWS_GPIO_DIRS].v_Address = a_GPIO_Config_Initial[8*TATPWS_MAX_GPIO +TATPWS_GPIO_DIRS].v_Address =  vl_BankBaseAddr +0x14;
		a_GPIO_Config_For_PWS_Mode[8*TATPWS_MAX_GPIO+TATPWS_GPIO_DIRC].v_Address = a_GPIO_Config_Initial[8*TATPWS_MAX_GPIO +TATPWS_GPIO_DIRC].v_Address =  vl_BankBaseAddr +0x18;
		a_GPIO_Config_For_PWS_Mode[8*TATPWS_MAX_GPIO+TATPWS_GPIO_SLMP].v_Address = a_GPIO_Config_Initial[8*TATPWS_MAX_GPIO +TATPWS_GPIO_SLMP].v_Address =  vl_BankBaseAddr +0x1c;
		a_GPIO_Config_For_PWS_Mode[8*TATPWS_MAX_GPIO+TATPWS_GPIO_AFSELA].v_Address = a_GPIO_Config_Initial[8*TATPWS_MAX_GPIO +TATPWS_GPIO_AFSELA].v_Address =  vl_BankBaseAddr +0x20;
		a_GPIO_Config_For_PWS_Mode[8*TATPWS_MAX_GPIO+TATPWS_GPIO_AFSELB].v_Address = a_GPIO_Config_Initial[8*TATPWS_MAX_GPIO +TATPWS_GPIO_AFSELB].v_Address =  vl_BankBaseAddr +0x24;
		a_GPIO_Config_For_PWS_Mode[8*TATPWS_MAX_GPIO+TATPWS_GPIO_RWMSC].v_Address = a_GPIO_Config_Initial[8*TATPWS_MAX_GPIO +TATPWS_GPIO_RWMSC].v_Address =  vl_BankBaseAddr +0x50;
		a_GPIO_Config_For_PWS_Mode[8*TATPWS_MAX_GPIO+TATPWS_GPIO_FWMSC].v_Address = a_GPIO_Config_Initial[8*TATPWS_MAX_GPIO +TATPWS_GPIO_FWMSC].v_Address =  vl_BankBaseAddr +0x54;

	} else {
		/* dynamic management of the registers addresses*/
		PWS_SYSLOG(LOG_DEBUG, "dynamic management of the registers addresses: Not implemented !!!!!");			
/*        for (i=0; i<TATPWS_NB_GPIO; i++) {*/
/*            a_GPIO_Config_For_PWS_Mode[i].v_Address= a_GPIO_Config_Initial[i].v_Address= V_VAL_READ_FROM_INIFILE ;*/
/*        }*/

		fclose (fd);
	}

	/*****************************/
	/* PreventReading initialisation */
	/*****************************/
	/*	Because the read from GPIO_DATS returns the GPIO_DAT register,
		Because the read from GPIO_DATC returns the GPIO_DAT register,
		Because the read from GPIO_DIRS returns the GPIO_DIR register,
		Because the read from GPIO_DIRC returns the GPIO_DIR register,
		We will never read their values.
	*/
	for (i = 0; i < TATPWS_NB_BANKS; i++) {
		a_GPIO_Config_For_PWS_Mode[i*TATPWS_MAX_GPIO+TATPWS_GPIO_DATS].v_PreventReading =
		 a_GPIO_Config_Initial[i*TATPWS_MAX_GPIO +TATPWS_GPIO_DATS].v_PreventReading = 1;

		a_GPIO_Config_For_PWS_Mode[i*TATPWS_MAX_GPIO+TATPWS_GPIO_DATC].v_PreventReading =
		 a_GPIO_Config_Initial[i*TATPWS_MAX_GPIO +TATPWS_GPIO_DATC].v_PreventReading = 1;

		a_GPIO_Config_For_PWS_Mode[i*TATPWS_MAX_GPIO+TATPWS_GPIO_DIRS].v_PreventReading =
		 a_GPIO_Config_Initial[i*TATPWS_MAX_GPIO+TATPWS_GPIO_DIRS].v_PreventReading = 1;

		a_GPIO_Config_For_PWS_Mode[i*TATPWS_MAX_GPIO+TATPWS_GPIO_DIRC].v_PreventReading =
		 a_GPIO_Config_Initial[i*TATPWS_MAX_GPIO+TATPWS_GPIO_DIRC].v_PreventReading = 1;
	}

	/*****************************/
	/* PreventWriting initialisation */
	/*****************************/
	/*	For the configuration of the GPIO registers during power mode,
		We do not must write the GPIO_DAT register and GPIO_DIR register.
		Theses ones are only modified by DATS/DATC resp DIRS/DIRC registers
	*/
	for (i = 0; i < TATPWS_NB_BANKS; i++) {
		a_GPIO_Config_For_PWS_Mode[i*TATPWS_MAX_GPIO+TATPWS_GPIO_DAT].v_PreventWriting = 1;
		a_GPIO_Config_For_PWS_Mode[i*TATPWS_MAX_GPIO+TATPWS_GPIO_DIR].v_PreventWriting = 1;
	}

	/*	For the restore of the GPIO registers after power mode,
		We do not must write the GPIO_DATS/DATC register and GPIO_DIRC/DIRS register.
		It is managed by the DAT resp DIR registers
	*/
	for (i = 0; i < TATPWS_NB_BANKS; i++) {
		a_GPIO_Config_Initial[i*TATPWS_MAX_GPIO+TATPWS_GPIO_DATS].v_PreventWriting = 1;
		a_GPIO_Config_Initial[i*TATPWS_MAX_GPIO+TATPWS_GPIO_DATC].v_PreventWriting = 1;
		a_GPIO_Config_Initial[i*TATPWS_MAX_GPIO+TATPWS_GPIO_DIRS].v_PreventWriting = 1;
		a_GPIO_Config_Initial[i*TATPWS_MAX_GPIO+TATPWS_GPIO_DIRC].v_PreventWriting = 1;
	}

	/*****************************/
	/* reg values initialisation */
	/*****************************/
	PWS_SYSLOG(LOG_DEBUG, "reading of %d gpio registers for first default config", TATPWS_NB_GPIO);

	/* read the current values for registers that allow it */
	tatpws1_GPIO_Read(a_GPIO_Config_For_PWS_Mode, TATPWS_NB_GPIO);

	/* For DATS/DATC registers, calculate theirs values from DAT register values */
	for (i = 0; i < TATPWS_NB_BANKS; i++) {
		a_GPIO_Config_For_PWS_Mode[i*TATPWS_MAX_GPIO+TATPWS_GPIO_DATS].v_Value =
		 a_GPIO_Config_For_PWS_Mode[i*TATPWS_MAX_GPIO+TATPWS_GPIO_DAT].v_Value;
		a_GPIO_Config_For_PWS_Mode[i*TATPWS_MAX_GPIO+TATPWS_GPIO_DATC].v_Value =
		 ~(a_GPIO_Config_For_PWS_Mode[i*TATPWS_MAX_GPIO+TATPWS_GPIO_DAT].v_Value);
		a_GPIO_Config_For_PWS_Mode[i*TATPWS_MAX_GPIO+TATPWS_GPIO_DIRS].v_Value =
		 a_GPIO_Config_For_PWS_Mode[i*TATPWS_MAX_GPIO+TATPWS_GPIO_DIR].v_Value;
		a_GPIO_Config_For_PWS_Mode[i*TATPWS_MAX_GPIO+TATPWS_GPIO_DIRC].v_Value =
		 ~(a_GPIO_Config_For_PWS_Mode[i*TATPWS_MAX_GPIO+TATPWS_GPIO_DIR].v_Value);
	}

	fd = fopen(TATPWS_IO_FILE, "r");
	if (fd != NULL) {
		char buf[MAX_CHAR_PER_LINE];
		char *reg = NULL;
		char *value = NULL;
		char *saveptr;

		/* dynamic management of the registers values (saved test values) */
		PWS_SYSLOG(LOG_DEBUG, "try to get the registers values from csv file");

		i = 0;

		while (fgets(buf, sizeof(buf), fd) != NULL) {
			reg = strtok_r(buf, TATPWS_SEP, &saveptr);
			/* si reg begins with "GPIO" */
			if (reg != NULL) {
				if (strstr(reg, "GPIO") == reg) {
					value = strtok_r(NULL, TATPWS_SEP, &saveptr);
					if (value != NULL) {
						if (strncmp(value, "NO_VALUE", strlen("NO_VALUE")) != 0) {
							a_GPIO_Config_For_PWS_Mode[i].v_Value = strtol(value, (char **)NULL, 16);
							PWS_SYSLOG(LOG_DEBUG, "a_GPIO_Config_For_PWS_Mode[%d].v_Value= %08x from value=%s", i, a_GPIO_Config_For_PWS_Mode[i].v_Value, value);
						} else {
							PWS_SYSLOG(LOG_DEBUG, "no default value in csv file!");
						}
					} else {
						PWS_SYSLOG(LOG_DEBUG, "corrupted csv file!, \"SaveConfig\" is not guaranted !");
					}
					i++;
					if (i == TATPWS_NB_GPIO)
						break;
				} else {
					PWS_SYSLOG(LOG_DEBUG, "skip line from csv file!");
				}
			} else {
				PWS_SYSLOG(LOG_DEBUG, "no token!");
			}
		}
		fclose(fd);

	} else {
		PWS_SYSLOG(LOG_DEBUG, "csv file does not exist, \"SaveConfig\" is not guaranted !");
	}

}

/* Read a set of GPIO registers								                 */
/*****************************************************************************/
int tatpws1_GPIO_Read(GpioReg *Reg, int Nb_Regs)
{
	regDB_addr_t addr;
	regDB_val_t val;
	int i = 0;
	int error = (TATPWS_NO_ERROR);

	if (Reg == NULL)
		return 0;
	if (Nb_Regs == 0)
		return 0;

	system("echo 1 > /sys/kernel/debug/clk/p1_pclk9/enable");
	system("echo 1 > /sys/kernel/debug/clk/p3_pclk8/enable");
	system("echo 1 > /sys/kernel/debug/clk/p2_pclk11/enable");
	system("echo 1 > /sys/kernel/debug/clk/p5_pclk1/enable");

	for (i = 0; i < Nb_Regs; i++) {
		if (Reg->v_PreventReading == 0) {
			addr = Reg->v_Address;
			fflush(NULL);
			error = db_u32_read(addr, &val);
			if (error >= 0)
				Reg->v_Value = val;
			else {
				PWS_SYSLOG(LOG_DEBUG, " 0x%08x, %d,ERROR READ", (int)Reg, i);
				return error;
			}
		} else {
			PWS_SYSLOG(LOG_DEBUG, " 0x%08x, %d, NO HWREG_READ", (int)Reg, i);
		}
		Reg++;
	}

	system("echo 0 > /sys/kernel/debug/clk/p1_pclk9/enable");
	system("echo 0 > /sys/kernel/debug/clk/p3_pclk8/enable");
	system("echo 0 > /sys/kernel/debug/clk/p2_pclk11/enable");
	system("echo 0 > /sys/kernel/debug/clk/p5_pclk1/enable");

	return error;
}

/* Write a set of GPIO registers											 */
/*****************************************************************************/
int tatpws1_GPIO_Write(GpioReg *Reg, int Nb_Regs)
{
	regDB_addr_t addr;
	regDB_val_t val;
	int i = 0;
	int error = (TATPWS_NO_ERROR);

	if (Reg == NULL)
		return 0;
	if (Nb_Regs == 0)
		return 0;

	system("echo 1 > /sys/kernel/debug/clk/p1_pclk9/enable");
	system("echo 1 > /sys/kernel/debug/clk/p3_pclk8/enable");
	system("echo 1 > /sys/kernel/debug/clk/p2_pclk11/enable");
	system("echo 1 > /sys/kernel/debug/clk/p5_pclk1/enable");

	for (i = 0; i < Nb_Regs; i++) {
		if (Reg->v_PreventReading == 0) {
			addr = Reg->v_Address;
			val = Reg->v_Value;
			fflush(NULL);
			error = db_u32_write(addr, val);
		} else {
			PWS_SYSLOG(LOG_DEBUG, " 0x%08x, %d, NO HWREG_READ", (int)Reg, i);
		}
		Reg++;
	}

	system("echo 0 > /sys/kernel/debug/clk/p1_pclk9/enable");
	system("echo 0 > /sys/kernel/debug/clk/p3_pclk8/enable");
	system("echo 0 > /sys/kernel/debug/clk/p2_pclk11/enable");
	system("echo 0 > /sys/kernel/debug/clk/p5_pclk1/enable");

	return error;
}

/* Execution of the action "GPIO_Settings"				                     */
/*****************************************************************************/
int tatpws1_GPIO_Settings_Execute(struct dth_element *elem)
{
	int vl_Error = TATPWS_NO_ERROR;
	FILE *fd;
	char buf[MAX_CHAR_PER_LINE];
	char *reg = NULL;
	char value[10];
	char *saveptr;
	char newline[MAX_CHAR_PER_LINE] = "";
	int i;
	fpos_t pos;

	switch (elem->user_data) {
	case ACTION_GPIO_SETTINGS:

		if (tatpws_GpioSettings_Mode == TATPWS_GPIOSETTINGS_GET) {
			/* get the config */
			/* nothing to do, all is done by the gettings of the parameters of the DTH action */
			PWS_SYSLOG(LOG_DEBUG, "TATPWS_GPIOSETTINGS_GET");

		} else if (tatpws_GpioSettings_Mode == TATPWS_GPIOSETTINGS_SET) {
			/* set the config */
			PWS_SYSLOG(LOG_DEBUG, "TATPWS_GPIOSETTINGS_SET");
		} else if (tatpws_GpioSettings_Mode == TATPWS_GPIOSETTINGS_SAVE) {

			/* Save the config into the csv file*/
			fd = fopen(TATPWS_IO_FILE, "r+");
			i = 0;
			if (fd != NULL) {
				if (fgetpos(fd, &pos) != 0)
					PWS_SYSLOG(LOG_DEBUG, "Can't get the first position in csv file!");

				while (fgets(buf, sizeof(buf), fd) != NULL) {
						PWS_SYSLOG(LOG_DEBUG, "line: %s", buf);

						reg = strtok_r(buf, TATPWS_SEP, &saveptr);
						/* si reg begins with "GPIO" */
						if (reg != NULL) {
							if (strstr(reg, "GPIO") == reg) {
								snprintf(value, 9, "%08X", a_GPIO_Config_For_PWS_Mode[i].v_Value);
								PWS_SYSLOG(LOG_DEBUG, "value:%s", value);

								strncat(newline, reg, MAX_CHAR_PER_LINE-strlen(newline));
								strncat(newline, TATPWS_SEP, 2);
								strncat(newline, value, 9);
								strncat(newline, TATPWS_SEP, 2);

								if (fsetpos(fd, &pos) != 0)
									PWS_SYSLOG(LOG_DEBUG, "Can't set the position in csv file!");

								PWS_SYSLOG(LOG_DEBUG, "Write in csv file :%s", newline);

								if (fputs(newline, fd) != EOF) {
									PWS_SYSLOG(LOG_DEBUG, " New value %08x saved in csv file",
										a_GPIO_Config_For_PWS_Mode[i].v_Value);
								} else {
									PWS_SYSLOG(LOG_DEBUG, " can't modify csv file!");
								}
								i++;

							} else {
								PWS_SYSLOG(LOG_DEBUG, "skip line from csv file!");
							}
						} else {
							PWS_SYSLOG(LOG_DEBUG, "no token!");
						}
						if (i == TATPWS_NB_GPIO)
							break;
						if (fgetpos(fd, &pos) != 0)
							PWS_SYSLOG(LOG_DEBUG, "Can't get the position in csv file!");

						newline[0] = 0;

					}
					PWS_SYSLOG(LOG_DEBUG, "Close csv file, %i lines have been modified!", i);
					fclose(fd);
				} else {
					PWS_SYSLOG(LOG_DEBUG, "can't open csv file for writing!");
				}

				/* Decode the config into the txt file*/
				tatpws1_GPIO_Decode_Config();

				PWS_SYSLOG(LOG_DEBUG, "TATPWS_GPIOSETTINGS_SAVE");
			} else if (tatpws_GpioSettings_Mode == TATPWS_GPIOSETTINGS_LOAD) {
				char *value = NULL;
				/* Load the config into the csv file*/
				fd = fopen(TATPWS_IO_FILE, "r");
				i = 0;
				PWS_SYSLOG(LOG_DEBUG, "TATPWS_GPIOSETTINGS_LOAD avant le fd");
				if (fd != NULL) {
					PWS_SYSLOG(LOG_DEBUG, "TATPWS_GPIOSETTINGS_LOAD apres le fd");
					while (fgets(buf, sizeof(buf), fd) != NULL) {
						PWS_SYSLOG(LOG_DEBUG, "TATPWS_GPIOSETTINGS_LOAD apres le while");
							reg = strtok_r(buf, TATPWS_SEP, &saveptr);
							/* si reg begins with "GPIO" */
							if (reg != NULL) {
								if (strstr(reg, "GPIO") == reg) {
									value = strtok_r(NULL, TATPWS_SEP, &saveptr);
									if (value != NULL) {
										if (strncmp(value, "NO_VALUE", strlen("NO_VALUE")) != 0) {
											a_GPIO_Config_For_PWS_Mode[i].v_Value = strtol(value, (char **)NULL, 16);
											PWS_SYSLOG(LOG_DEBUG, "a_GPIO_Config_For_PWS_Mode[%d].v_Value= %08x from value=%s", i, a_GPIO_Config_For_PWS_Mode[i].v_Value, value);
										} else {
											PWS_SYSLOG(LOG_DEBUG, "no default value in csv file!");
										}
									} else {
										PWS_SYSLOG(LOG_DEBUG, "corrupted csv file!, \"SaveConfig\" is not guaranted !");
									}
									i++;
									if (i == TATPWS_NB_GPIO)
										break;
								} else {
									PWS_SYSLOG(LOG_DEBUG, "skip line from csv file!");
								}
							} else {
								PWS_SYSLOG(LOG_DEBUG, "no token!");
							}
						}
					fclose(fd);
				} else {
					PWS_SYSLOG(LOG_DEBUG, "can't open csv file for writing!");
				}

				/* Decode the config into the txt file*/
				tatpws1_GPIO_Decode_Config();

				PWS_SYSLOG(LOG_DEBUG, "TATPWS_GPIOSETTINGS_LOAD__");
			} else {
				PWS_SYSLOG(LOG_DEBUG, "Unknown mode!");
			}
		break;

	default:
			vl_Error = TATPWS_ERROR_BAD_REQ;
			PWS_SYSLOG(LOG_DEBUG, "TATPWS_ERROR_BAD_REQ");

		break;
	}
  return vl_Error;
}

/* Result of the action "GPIO_Settings"									     */
/*****************************************************************************/
int tatpws1_GPIO_Settings_Result(struct dth_element *elem, void *Value)
{

	int vl_Error = TATPWS_NO_ERROR;
if (Value == NULL)
		return 0;

	/*Execute the action Action */
	vl_Error = tatpws1_GPIO_Settings_Execute(elem);
	PWS_SYSLOG(LOG_DEBUG,
			"tatpws1_GPIO_Settings_Execute has returned=%d", vl_Error);

	return vl_Error;
}

/* Set Callback for Mode Parameter of the action "GPIO_Settings"			 */
/*****************************************************************************/
int tatpws1_GPIO_Settings_SetMode(struct dth_element *elem, void *Value)
{
	int vl_Error = TATPWS_NO_ERROR;
	uint8_t *pl_Mode = (uint8_t *)Value;
if (elem == NULL)
		return 0;

	PWS_SYSLOG(LOG_DEBUG, "Mode=%d", *pl_Mode);
	switch (*pl_Mode) {
	case TATPWS_GPIOSETTINGS_GET: /* Get config*/
		tatpws_GpioSettings_Mode = TATPWS_GPIOSETTINGS_GET;
		break;
	case TATPWS_GPIOSETTINGS_SET: /* Set config*/
		tatpws_GpioSettings_Mode = TATPWS_GPIOSETTINGS_SET;
		break;
	case TATPWS_GPIOSETTINGS_SAVE: /* Save config*/
		tatpws_GpioSettings_Mode = TATPWS_GPIOSETTINGS_SAVE;
		break;
	case TATPWS_GPIOSETTINGS_LOAD:/*load config*/
		tatpws_GpioSettings_Mode = TATPWS_GPIOSETTINGS_LOAD;
		PWS_SYSLOG(LOG_DEBUG, "TATPWS_GPIOSETTINGS_LOAD_setmode");
		break;
	default:
		/* Invalid parameter */
		vl_Error = TATPWS_ERROR_BAD_VALUE;
		PWS_SYSLOG(LOG_DEBUG, "TATPWS_ERROR_BAD_VALUE");
		break;
	}
	return vl_Error;
}

/* Set Callback for GPIO Parameters of the action "GPIO_Settings"			 */
/*****************************************************************************/
int tatpws1_GPIO_Settings_Set(struct dth_element *elem, void *Value)
{
	int vl_Error = TATPWS_NO_ERROR;
	uint32_t *pl_ValGpio = (uint32_t *)Value;

	/* modify the current test value for the GPIO only when the action = SET */
	if (tatpws_GpioSettings_Mode == TATPWS_GPIOSETTINGS_SET) {
		a_GPIO_Config_For_PWS_Mode[elem->user_data].v_Value = *pl_ValGpio;
		PWS_SYSLOG(LOG_DEBUG,
				"a_GPIO_Config_For_PWS_Mode[%d].v_Value =%u =%08x",
				elem->user_data, *pl_ValGpio, *pl_ValGpio);
	}

	return vl_Error;
}

/* Get Callback for GPIO Parameters of the action "GPIO_Settings"			 */
/*****************************************************************************/
int tatpws1_GPIO_Settings_Get(struct dth_element *elem, void *Value)
{
	int vl_Error = TATPWS_NO_ERROR;

	/* read the current test value for the GPIO */
	*(uint32_t *)Value = a_GPIO_Config_For_PWS_Mode[elem->user_data].v_Value;
	PWS_SYSLOG(LOG_DEBUG, "a_GPIO_Config_For_PWS_Mode[%d].v_Value=%u =%08x ",
			elem->user_data,
			a_GPIO_Config_For_PWS_Mode[elem->user_data].v_Value,
			a_GPIO_Config_For_PWS_Mode[elem->user_data].v_Value);
	return vl_Error;
}


