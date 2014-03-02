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

static int tatpws_ABGpioSettings_Mode;

/* For the restore of the GPIO registers after power saving mode*/
/*  9 banks */
GpioReg a_AB_GPIO_Config_Initial[TATPWS_NB_AB_GPIOS] = { {0, 0, 0, 0} };

/* For the configuration of the GPIO registers during power saving mode*/
GpioReg a_AB_GPIO_Config_For_PWS_Mode[TATPWS_NB_AB_GPIOS] = { {0, 0, 0, 0} };

/* Set Callback for Mode Parameter of the action
 * "Analog Baseband GPIO_Settings"*/
/*****************************************************************************/
int tatpws4_ABGPIO_Settings_SetMode(struct dth_element *elem, void *Value)
{
	int vl_Error = TATPWS_NO_ERROR;
	uint8_t *pl_Mode = (uint8_t *)Value;
if (elem == NULL)
		return 0;

	PWS_SYSLOG(LOG_DEBUG, "Mode=%d", *pl_Mode);
	switch (*pl_Mode) {
	case TATPWS_GPIOSETTINGS_GET: /* Get config*/
		tatpws_ABGpioSettings_Mode = TATPWS_GPIOSETTINGS_GET;
		break;
	case TATPWS_GPIOSETTINGS_SET: /* Set config*/
		tatpws_ABGpioSettings_Mode = TATPWS_GPIOSETTINGS_SET;
		break;
	case TATPWS_GPIOSETTINGS_SAVE: /* Save config*/
		tatpws_ABGpioSettings_Mode = TATPWS_GPIOSETTINGS_SAVE;
		break;
	case TATPWS_GPIOSETTINGS_LOAD: /* Load config*/
		tatpws_ABGpioSettings_Mode = TATPWS_GPIOSETTINGS_LOAD;
		break;
	default:
		/* Invalid parameter */
		vl_Error = TATPWS_ERROR_BAD_VALUE;
		PWS_SYSLOG(LOG_DEBUG, "TATPWS_ERROR_BAD_VALUE");
		break;
	}

	return vl_Error;
}

/* Set Callback for GPIO Parameters of the action
 * "Analog Baseband GPIO_Settings"				*/
/*****************************************************************************/
int tatpws4_ABGPIO_Settings_Set(struct dth_element *elem, void *Value)
{
	int vl_Error = TATPWS_NO_ERROR;
	uint8_t *pl_ValGpio = (uint8_t *)Value;

	/* modify the current test value for the GPIO only when the action = SET */
	if (tatpws_ABGpioSettings_Mode == TATPWS_GPIOSETTINGS_SET) {
		a_AB_GPIO_Config_For_PWS_Mode[elem->user_data].v_Value = *pl_ValGpio;
		PWS_SYSLOG(LOG_DEBUG, "a_AB_GPIO_Config_For_PWS_Mode[%d].v_Value =%u =%02x", elem->user_data, *pl_ValGpio, *pl_ValGpio);
	}
	return vl_Error;
}

/* Get Callback for GPIO Parameters of the action
 * "Analog Baseband PIO_Settings"*/
/*****************************************************************************/
int tatpws4_ABGPIO_Settings_Get(struct dth_element *elem, void *Value)
{
	int vl_Error = TATPWS_NO_ERROR;

	/* read the current test value for the GPIO */
	*(uint8_t *)Value = a_AB_GPIO_Config_For_PWS_Mode[elem->user_data].v_Value;
	PWS_SYSLOG(LOG_DEBUG, "a_AB_GPIO_Config_For_PWS_Mode[%d].v_Value=%u =%02x ",
			elem->user_data, a_AB_GPIO_Config_For_PWS_Mode[elem->user_data].v_Value,
			a_AB_GPIO_Config_For_PWS_Mode[elem->user_data].v_Value);

	return vl_Error;
}

/* Fct to decode the Analog Baseband gpio config
 * from the registers raw values*/
/*****************************************************************************/
static void tatpws4_ABGPIO_Decode_Config(void)
{
#if 0
	int vl_Gpio = 0;
	FILE *fd;
	char newline[MAX_CHAR_PER_LINE] = "";

	fd = fopen("/usr/local/lib/tat/tatlpws/analog_baseband_gpios_decoding.txt", "w");
	if (fd != NULL) {
		for (vl_Gpio = 0; vl_Gpio < 32; vl_Gpio++) {
			snprintf(newline, MAX_CHAR_PER_LINE, " GPIO%i_ : Pin is GPIO", vl_Gpio);
			strncat(newline, ", input", MAX_CHAR_PER_LINE-strlen(newline));

			/* write the line in the txt file */
			fputs(newline, fd);
		}
		fclose(fd);
	} else {
		PWS_SYSLOG(LOG_DEBUG,
				"Can't open /usr/local/lib/tat/tatlpws/analog_baseband_gpios_decoding.txt!");
	}
#endif
}

/* Initialize Analog Baseband GPIO configuration structure					 */
/*****************************************************************************/
void tatpws4_ABGPIO_Settings_Init()
{
	FILE *fd = NULL;
	int i;

	/*****************************/
	/* @ initialisation*/
	/*****************************/
	a_AB_GPIO_Config_For_PWS_Mode[TATPWS_GPIO_SEL1].v_Address = a_AB_GPIO_Config_Initial[TATPWS_GPIO_SEL1].v_Address = GPIO_PAD_CONTROL_1_REG ;
	a_AB_GPIO_Config_For_PWS_Mode[TATPWS_GPIO_SEL2].v_Address = a_AB_GPIO_Config_Initial[TATPWS_GPIO_SEL2].v_Address = GPIO_PAD_CONTROL_2_REG ;
	a_AB_GPIO_Config_For_PWS_Mode[TATPWS_GPIO_SEL3].v_Address = a_AB_GPIO_Config_Initial[TATPWS_GPIO_SEL3].v_Address = GPIO_PAD_CONTROL_3_REG ;
	a_AB_GPIO_Config_For_PWS_Mode[TATPWS_GPIO_SEL4].v_Address = a_AB_GPIO_Config_Initial[TATPWS_GPIO_SEL4].v_Address = GPIO_PAD_CONTROL_4_REG ;
	a_AB_GPIO_Config_For_PWS_Mode[TATPWS_GPIO_SEL5].v_Address = a_AB_GPIO_Config_Initial[TATPWS_GPIO_SEL5].v_Address = GPIO_PAD_CONTROL_5_REG ;
	a_AB_GPIO_Config_For_PWS_Mode[TATPWS_GPIO_SEL6].v_Address = a_AB_GPIO_Config_Initial[TATPWS_GPIO_SEL6].v_Address = GPIO_PAD_CONTROL_6_REG ;

	a_AB_GPIO_Config_For_PWS_Mode[TATPWS_GPIO_DIR1].v_Address = a_AB_GPIO_Config_Initial[TATPWS_GPIO_DIR1].v_Address = GPIO_DIRECTION_1_REG ;
	a_AB_GPIO_Config_For_PWS_Mode[TATPWS_GPIO_DIR2].v_Address = a_AB_GPIO_Config_Initial[TATPWS_GPIO_DIR2].v_Address = GPIO_DIRECTION_2_REG ;
	a_AB_GPIO_Config_For_PWS_Mode[TATPWS_GPIO_DIR3].v_Address = a_AB_GPIO_Config_Initial[TATPWS_GPIO_DIR3].v_Address = GPIO_DIRECTION_3_REG ;
	a_AB_GPIO_Config_For_PWS_Mode[TATPWS_GPIO_DIR4].v_Address = a_AB_GPIO_Config_Initial[TATPWS_GPIO_DIR4].v_Address = GPIO_DIRECTION_4_REG ;
	a_AB_GPIO_Config_For_PWS_Mode[TATPWS_GPIO_DIR5].v_Address = a_AB_GPIO_Config_Initial[TATPWS_GPIO_DIR5].v_Address = GPIO_DIRECTION_5_REG ;
	a_AB_GPIO_Config_For_PWS_Mode[TATPWS_GPIO_DIR6].v_Address = a_AB_GPIO_Config_Initial[TATPWS_GPIO_DIR6].v_Address = GPIO_DIRECTION_6_REG ;

	a_AB_GPIO_Config_For_PWS_Mode[TATPWS_GPIO_OUT1].v_Address = a_AB_GPIO_Config_Initial[TATPWS_GPIO_OUT1].v_Address = GPIO_SET_OUPUT_1_REG ;
	a_AB_GPIO_Config_For_PWS_Mode[TATPWS_GPIO_OUT2].v_Address = a_AB_GPIO_Config_Initial[TATPWS_GPIO_OUT2].v_Address = GPIO_SET_OUPUT_2_REG ;
	a_AB_GPIO_Config_For_PWS_Mode[TATPWS_GPIO_OUT3].v_Address = a_AB_GPIO_Config_Initial[TATPWS_GPIO_OUT3].v_Address = GPIO_SET_OUPUT_3_REG ;
	a_AB_GPIO_Config_For_PWS_Mode[TATPWS_GPIO_OUT4].v_Address = a_AB_GPIO_Config_Initial[TATPWS_GPIO_OUT4].v_Address = GPIO_SET_OUPUT_4_REG ;
	a_AB_GPIO_Config_For_PWS_Mode[TATPWS_GPIO_OUT5].v_Address = a_AB_GPIO_Config_Initial[TATPWS_GPIO_OUT5].v_Address = GPIO_SET_OUPUT_5_REG ;
	a_AB_GPIO_Config_For_PWS_Mode[TATPWS_GPIO_OUT6].v_Address = a_AB_GPIO_Config_Initial[TATPWS_GPIO_OUT6].v_Address = GPIO_SET_OUPUT_6_REG ;

	a_AB_GPIO_Config_For_PWS_Mode[TATPWS_GPIO_PUD1].v_Address = a_AB_GPIO_Config_Initial[TATPWS_GPIO_PUD1].v_Address = GPIO_PULL_DOWN_1_REG ;
	a_AB_GPIO_Config_For_PWS_Mode[TATPWS_GPIO_PUD2].v_Address = a_AB_GPIO_Config_Initial[TATPWS_GPIO_PUD2].v_Address = GPIO_PULL_DOWN_2_REG ;
	a_AB_GPIO_Config_For_PWS_Mode[TATPWS_GPIO_PUD3].v_Address = a_AB_GPIO_Config_Initial[TATPWS_GPIO_PUD3].v_Address = GPIO_PULL_DOWN_3_REG ;
	a_AB_GPIO_Config_For_PWS_Mode[TATPWS_GPIO_PUD4].v_Address = a_AB_GPIO_Config_Initial[TATPWS_GPIO_PUD4].v_Address = GPIO_PULL_DOWN_4_REG ;
	a_AB_GPIO_Config_For_PWS_Mode[TATPWS_GPIO_PUD5].v_Address = a_AB_GPIO_Config_Initial[TATPWS_GPIO_PUD5].v_Address = GPIO_PULL_DOWN_5_REG ;
	a_AB_GPIO_Config_For_PWS_Mode[TATPWS_GPIO_PUD6].v_Address = a_AB_GPIO_Config_Initial[TATPWS_GPIO_PUD6].v_Address = GPIO_PULL_DOWN_6_REG ;

	a_AB_GPIO_Config_For_PWS_Mode[TATPWS_GPIO_ALT_FCT].v_Address = a_AB_GPIO_Config_Initial[TATPWS_GPIO_ALT_FCT].v_Address = GPIO_ALTERNATE_1_REG ;

	/*****************************/
	/* reg values initialisation */
	/*****************************/
	PWS_SYSLOG(LOG_DEBUG, "reading of %d ab8500 gpio registers for first default config", TATPWS_NB_AB_GPIOS);

	/* read the current values for registers that allow it */
	tatpws4_ABGPIO_Read(a_AB_GPIO_Config_For_PWS_Mode, TATPWS_NB_AB_GPIOS);

	/* if a value is defined in the imput configuration file, set it*/
	fd = fopen(TATPWS_IO_FILE_FOR_AB, "r");
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
							a_AB_GPIO_Config_For_PWS_Mode[i].v_Value = strtol(value, (char **)NULL, 16);
							PWS_SYSLOG(LOG_DEBUG, "a_AB_GPIO_Config_For_PWS_Mode[%d].v_Value= %08x from value=%s", i, a_AB_GPIO_Config_For_PWS_Mode[i].v_Value, value);
						} else {
							PWS_SYSLOG(LOG_DEBUG, "no default value in csv file!");
						}
					} else {
						PWS_SYSLOG(LOG_DEBUG, "corrupted csv file!, \"SaveConfig\" is not guaranted !");
					}
					i++;
					if (i == TATPWS_NB_AB_GPIOS)
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

/* Read a set of Analog BB GPIO registers									 */
/*****************************************************************************/
int tatpws4_ABGPIO_Read(GpioReg *Reg, int Nb_Regs)
{
	int ret;
	int i;
	uint8_t reg;
	for (i = 0; i < Nb_Regs; i++) {
		if (Reg->v_PreventReading == 0) {
			ret = abxxxx_read(Reg->v_Address, &reg);
			if (ret < 0)
				return ret;
			 Reg->v_Value = reg;
			PWS_SYSLOG(LOG_DEBUG, " 0x%08x, %d, AB8500_READ: Bloc=%02x, Addr=%02x, Val=%08x, ret=%d", (int)Reg, i, (Reg->v_Address & 0xFF00) >> 8, Reg->v_Address & 0x00FF, Reg->v_Value, ret);
		} else {
			PWS_SYSLOG(LOG_DEBUG, " 0x%08x, %d, NO AB8500_READ", (int)Reg, i);
		}
		Reg++;
	}

	return TATPWS_NO_ERROR;
}

/* Write a set of Analog BB GPIO registers									 */
/*****************************************************************************/
int tatpws4_ABGPIO_Write(GpioReg *Reg, int Nb_Regs)
{
	int ret;
	int i;

	for (i = 0; i < Nb_Regs; i++) {
		if (Reg->v_PreventWriting == 0) {
			ret = abxxxx_write(Reg->v_Address, (unsigned long)Reg->v_Value);
			if (ret < 0)
				return ret;

			PWS_SYSLOG(LOG_DEBUG, " 0x%08x, %d, AB8500_READ: Bloc=%02x, Addr=%02x, Val=%08x, ret=%d", (int)Reg, i, (Reg->v_Address & 0xFF00) >> 8, Reg->v_Address & 0x00FF, Reg->v_Value, ret);
		} else {
			PWS_SYSLOG(LOG_DEBUG, " 0x%08x, %d, NO AB8500_WRITE", (int)Reg, i);
		}
		Reg++;
	}

	return TATPWS_NO_ERROR;
}



/* Execution of the action "Analog Baseband GPIO_Settings"				     */
/*****************************************************************************/
int tatpws4_ABGPIO_Settings_Execute(struct dth_element *elem)
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

		if (tatpws_ABGpioSettings_Mode == TATPWS_GPIOSETTINGS_GET) {
			/* get the config */
			/* nothing to do, all is done by the gettings of the parameters of the DTH action */
			PWS_SYSLOG(LOG_DEBUG, "TATPWS_GPIOSETTINGS_GET");
		} else if (tatpws_ABGpioSettings_Mode == TATPWS_GPIOSETTINGS_SET) {
			/* set the config */
			/* nothing to do, all is done by the settings of the parameters of the DTH action */
			PWS_SYSLOG(LOG_DEBUG, "TATPWS_GPIOSETTINGS_SET");
		} else if (tatpws_ABGpioSettings_Mode == TATPWS_GPIOSETTINGS_SAVE) {
			/* Save the config into the csv file*/
			fd = fopen(TATPWS_IO_FILE_FOR_AB, "r+");
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
							snprintf(value, 9, "      %02X", a_AB_GPIO_Config_For_PWS_Mode[i].v_Value);
							PWS_SYSLOG(LOG_DEBUG, "value:%s", value);

							strncat(newline, reg, MAX_CHAR_PER_LINE-strlen(newline));
							strncat(newline, TATPWS_SEP, 2);
							strncat(newline, value, 9);
							strncat(newline, TATPWS_SEP, 2);

							if (fsetpos(fd, &pos) != 0)
								PWS_SYSLOG(LOG_DEBUG, "Can't set the position in csv file!");

							PWS_SYSLOG(LOG_DEBUG, "Write in csv file :%s", newline);

							if (fputs(newline, fd) != EOF) {
								PWS_SYSLOG(LOG_DEBUG, " New value %02x saved in csv file",
									a_AB_GPIO_Config_For_PWS_Mode[i].v_Value);
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
					if (i == TATPWS_NB_AB_GPIOS)
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
			tatpws4_ABGPIO_Decode_Config();

			PWS_SYSLOG(LOG_DEBUG, "TATPWS_GPIOSETTINGS_SAVE");
		} else if (tatpws_ABGpioSettings_Mode == TATPWS_GPIOSETTINGS_LOAD) {
			/* Load the config into the csv file*/
			char *value = NULL;
			fd = fopen(TATPWS_IO_FILE_FOR_AB, "r");
			i = 0;

			if (fd != NULL) {
				while (fgets(buf, sizeof(buf), fd) != NULL) {
					reg = strtok_r(buf, TATPWS_SEP, &saveptr);
					/* si reg begins with "GPIO" */
					if (reg != NULL) {
						if (strstr(reg, "GPIO") == reg) {
							value = strtok_r(NULL, TATPWS_SEP, &saveptr);
							if (value != NULL) {
								if (strncmp(value, "NO_VALUE", strlen("NO_VALUE")) != 0) {
									a_AB_GPIO_Config_For_PWS_Mode[i].v_Value = strtol(value, (char **)NULL, 16);
									PWS_SYSLOG(LOG_DEBUG, "a_AB_GPIO_Config_For_PWS_Mode[%d].v_Value= %08x from value=%s", i, a_AB_GPIO_Config_For_PWS_Mode[i].v_Value, value);
								} else {
									PWS_SYSLOG(LOG_DEBUG, "no default value in csv file!");
								}
							} else {
								PWS_SYSLOG(LOG_DEBUG, "corrupted csv file!, \"SaveConfig\" is not guaranted !");
							}
							i++;
							if (i == TATPWS_NB_AB_GPIOS)
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
			tatpws4_ABGPIO_Decode_Config();

			PWS_SYSLOG(LOG_DEBUG, "TATPWS_GPIOSETTINGS_LOAD");
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

/* Result of the action "Analog Baseband GPIO_Settings" */
/*****************************************************************************/
int tatpws4_ABGPIO_Settings_Result(struct dth_element *elem, void *Value)
{

	int vl_Error = TATPWS_NO_ERROR;
if (Value == NULL)
		return 0;

	/*Execute the action Action */
	vl_Error = tatpws4_ABGPIO_Settings_Execute(elem);
	PWS_SYSLOG(LOG_DEBUG, "tatpws4_ABGPIO_Settings_Execute has returned=%d", vl_Error);

	return vl_Error;
}

