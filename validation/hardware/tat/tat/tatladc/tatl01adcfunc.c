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

#define DTHL01ADCFUNC_C
#include "tatladc.h"
#include <libab.h>
#undef DTHL01ADCFUNC_C

static int v_NumberofcyclicRead;
static char p_previousPATHstorage[BUFFERMAX] = { 0 };

static float ADC_float[ADC_ELEMENT_COUNT] = { 0 };
static int ADC_param[ADC_ELEMENT_COUNT] = { 0 };

struct matchTablestring {
  int   v_ADCSelect;
  char *p_ADCStr;
};

static struct matchTablestring p_ADC_Name[] = {
	{ADCSWSEL_BATCTRL, "BATCTRL_mV"},
	{ADCSWSEL_RBATCTRL, "RBATCTRL_mOhms"},
	{ADCSWSEL_BATTEMP, "BATTEMP_C"},
#ifdef HATSCONF_AB_LIB_8500
	{ADCSWSEL_MAINCHVOLT, "MAINCHVOLT_mV"},
#endif
	{ADCSWSEL_ACCDET1, "ACCDET1_mV"},
	{ADCSWSEL_ACCDET2, "ACCDET2_mV"},
	{ADCSWSEL_ADCAUX1, "ADCAUX1_mV"},
	{ADCSWSEL_ADCAUX2, "ADCAUX2_mV"},
	{ADCSWSEL_VBATA, "VBAT_mV"},
	{ADCSWSEL_VBUS, "VBUS_mV"},
#ifdef HATSCONF_AB_LIB_8500
	{ADCSWSEL_MAINCHCUR, "MAINCHCUR_mA"},
#endif
	{ADCSWSEL_USBCHCUR, "USBCHCUR_mA"},
	{ADCSWSEL_BACKUPBAT, "BACKUPBAT_mV"},
	{0xff, "0xff"},
};

/*---------------------------------------------------------------------------*
* Procedure    : SetADCGainOffsetCalib
*----------------------------------------------------------------------------*
* Description  : Calcul gain and offset from OTP value
*----------------------------------------------------------------------------*
* Parameter(s) : None
*----------------------------------------------------------------------------*
* Return value : None
*---------------------------------------------------------------------------*/
static void SetADCGainOffsetCalib()
{
	uint8_t GpADCCal1;
	uint8_t GpADCCal2;
	uint8_t GpADCCal3;
	uint8_t GpADCCal4;
	uint8_t GpADCCal5;
	uint8_t GpADCCal6;
	uint8_t GpADCCal7;
	float vmainhigh = 0;
	float vmainlow = 0;
	float btemphigh = 0;
	float btemplow = 0;
	float vbathigh = 0;
	float vbatlow = 0;

	abxxxx_read(0x150f, &GpADCCal1);
	abxxxx_read(0x1510, &GpADCCal2);
	abxxxx_read(0x1511, &GpADCCal3);
	abxxxx_read(0x1512, &GpADCCal4);
	abxxxx_read(0x1513, &GpADCCal5);
	abxxxx_read(0x1514, &GpADCCal6);
	abxxxx_read(0x1515, &GpADCCal7);

	vmainhigh = (((GpADCCal1 & 0x03) << 8) |
		     ((GpADCCal2 & 0x3f) << 2) | ((GpADCCal3 & 0xc0) >> 6));

	vmainlow = ((GpADCCal3 & 0x3e) >> 1);

	btemphigh = (((GpADCCal3 & 0x01) << 9) |
		     (GpADCCal4 << 1) | ((GpADCCal5 & 0x80) >> 7));

	btemplow = ((GpADCCal5 & 0x7c) >> 2);
	vbathigh = (((GpADCCal5 & 0x03) << 8) | GpADCCal6);
	vbatlow = ((GpADCCal7 & 0xfc) >> 2);

	ADC_param[OUT_VTEMPH] = btemphigh;
	ADC_param[OUT_VTEMPL] = btemplow;
#ifdef HATSCONF_AB_LIB_8500
	ADC_param[OUT_VMAINH] = vmainhigh;
	ADC_param[OUT_VMAINL] = vmainlow;
#endif
	ADC_param[OUT_VBATH] = vbathigh;
	ADC_param[OUT_VBATL] = vbatlow;

	if (btemphigh == btemplow) {
		ADC_float[GAINADC_TEMP] = 1;
		ADC_float[OFFSETADC_TEMP] = 0;
	} else {
		ADC_float[GAINADC_TEMP] = (1300 - 21) / (btemphigh - btemplow);
		ADC_float[OFFSETADC_TEMP] =
		    1300 - ((1300 - 21) / (btemphigh - btemplow)) * btemphigh;
	}

#ifdef HATSCONF_AB_LIB_8500
	if (vmainhigh == vmainlow) {
		ADC_float[GAINADC_MAIN] = 1;
		ADC_float[OFFSETADC_MAIN] = 0;
	} else {
		ADC_float[GAINADC_MAIN] =
		    (19500 - 315) / (vmainhigh - vmainlow);
		ADC_float[OFFSETADC_MAIN] =
		    19500 -
		    ((19500 - 315) / (vmainhigh - vmainlow)) * vmainhigh;
	}
#endif
	if (vbathigh == vbatlow) {
		ADC_float[GAINADC_VBAT] = 1;
		ADC_float[OFFSETADC_VBAT] = 0;
	} else {
		ADC_float[GAINADC_VBAT] = (4700 - 2380) / (vbathigh - vbatlow);
		ADC_float[OFFSETADC_VBAT] =
		    4700 - ((4700 - 2380) / (vbathigh - vbatlow)) * vbathigh;
	}
	ADC_SYSLOG(LOG_DEBUG, "\n");
}


/*---------------------------------------------------------------------------*
* Procedure    : Set_Average_Sample
*----------------------------------------------------------------------------*
* Description  : Set average sample in debugfs
*----------------------------------------------------------------------------*
* Parameter(s) : None
*----------------------------------------------------------------------------*
* Return Value : None
*----------------------------------------------------------------------------*/
static void Set_Average_Sample(void)
{
	int fd = 0;
	int val = 0;
	char tmp[5];

	switch (ADC_param[IN_AVG]) {
		case 0:
			val = SAMPLE_1;
			break;
		case 1:
			val = SAMPLE_4;
			break;
		case 2:
			val = SAMPLE_8;
			break;
		default:
			val = SAMPLE_16;
			break;
	}

	fd = open(SAMPLE, O_RDWR);
	if (fd < 0) {
		printf("error opening sample debugfs file\n");
	} else {
		snprintf(tmp, sizeof(tmp), "%d", val);
		write(fd, tmp, sizeof(unsigned long));
		close(fd);
	}
}

/*---------------------------------------------------------------------------*
* Procedure    : Set_Trig_Edge
*----------------------------------------------------------------------------*
* Description  : Set trig edge value
*----------------------------------------------------------------------------*
* Parameter(s) : None
*----------------------------------------------------------------------------*
* Return Value : None
*----------------------------------------------------------------------------*/
static void Set_Trig_Edge(void)
{
	int fd = 0;
	char tmp[5];

	fd = open(TRIG_EDGE, O_RDWR);
	if (fd < 0) {
		printf("error opening trig edge debugfs file\n");
	} else {
		snprintf(tmp, sizeof(tmp), "%d", ADC_param[IN_TRIGEDGE]);
		write(fd, tmp, sizeof(unsigned long));
		close(fd);
	}
}

/*---------------------------------------------------------------------------*
* Procedure    : Set_Trig_Timer
*----------------------------------------------------------------------------*
* Description  : Set trig timer value
*----------------------------------------------------------------------------*
* Parameter(s) : None
*----------------------------------------------------------------------------*
* Return Value : None
*----------------------------------------------------------------------------*/
static void Set_Trig_Timer(void)
{
	int fd = 0;
	char tmp[5];

	fd = open(TRIG_TIMER, O_RDWR);
	if (fd < 0) {
		printf("error opening trig timer debugfs file\n");
	} else {
		snprintf(tmp, sizeof(tmp), "%d", ADC_param[IN_TRIGTIMER]);
		write(fd, tmp, sizeof(unsigned long));
		close(fd);
	}
}

/*---------------------------------------------------------------------------*
* Procedure    : Set_Raw_Calib_Values
*----------------------------------------------------------------------------*
* Description  : Save raw value converted in mV
*----------------------------------------------------------------------------*
* Parameter(s) : const char path: file path in debugfs
*				 float conv: raw value converted in mV
*----------------------------------------------------------------------------*
* Return Value : 0 if success, -1 if an error occurs
*---------------------------------------------------------------------------*/
static int Set_Raw_Calib_Values(const char *path, float conv)
{
	FILE *fd = NULL;
	int err = 0;

	fd = fopen(path, "r");
	if (fd == NULL) {
		printf("error opening %s\n", path);
		err = -1;
		goto end;
	}
	fscanf(fd, "%d,0x%X", &ADC_param[OUT_VALCALIB], &ADC_param[OUT_VAL]);
	fclose(fd);
	ADC_float[OUT_VALCONV] = ADC_param[OUT_VAL] * conv;

end:
	return err;
}

/*---------------------------------------------------------------------------*
* Procedure    : Set_Btemp_Deg
*----------------------------------------------------------------------------*
* Description  : Save raw calibrated value converted in degree celcius
*----------------------------------------------------------------------------*
* Parameter(s) : float value_calib: raw calibrated value
*----------------------------------------------------------------------------*
* Return Value : None
*---------------------------------------------------------------------------*/
static void Set_BTemp_Deg(float value_calib)
{
	float ln;
	float power;

	ln = log((((-value_calib/1000)*230000)/((value_calib/1000)-2))/47000);
	power = pow((((ln)/4050)+(1/298.15)),-1);
	ADC_float[OUT_VALCALIBDEG] = (float) (power-273.15);
}

/*---------------------------------------------------------------------------*
* Procedure    : Write_To_Uint_Debugfs_File
*----------------------------------------------------------------------------*
* Description  : Write to a debugfs file that supports unsigned integer
*----------------------------------------------------------------------------*
* Parameter(s) :
*	path:	debugfs file location
*	index:	position of parameter to be set in the ADC param table
*----------------------------------------------------------------------------*
* Return Value : 0 if success else error code
*----------------------------------------------------------------------------*/
static int Write_To_Uint_Debugfs_File(const char *path, const int index)
{
	int fd = 0;
	int ret = 0;
	char tmp[5];

	fd = open(path, O_WRONLY);
	if (fd < 0)
		return fd;

	snprintf(tmp, sizeof(tmp), "%d", ADC_param[index]);
	write(fd, tmp, sizeof(unsigned long));

	return ret;
}

/*---------------------------------------------------------------------------*
* Procedure    : Read_From_Uint_Debugfs_File
*----------------------------------------------------------------------------*
* Description  : Read from a debugfs file that supports unsigned integer
*----------------------------------------------------------------------------*
* Parameter(s) :
*	path:	debugfs file location
*	index:	position of parameter to be set in the ADC param table
*----------------------------------------------------------------------------*
* Return Value : 0 if success else error code
*----------------------------------------------------------------------------*/
static int Read_From_Uint_Debugfs_File(const char *path, const int index)
{
	FILE *fd = NULL;
	int ret = 0;

	fd = fopen(path, "r");
	if (!fd)
		return -1;

	fscanf(fd, "%d", &ADC_param[index]);
	fclose(fd);

	return ret;
}

/*---------------------------------------------------------------------------*
* Procedure    : Get_One_Sample
*----------------------------------------------------------------------------*
* Description  : Read one sample
* ---------------------------------------------------------------------------*
* Parameter(s) : None
*----------------------------------------------------------------------------*
* Return Value : 0 if success else error code
*----------------------------------------------------------------------------*/
static int Get_One_Sample(void)
{
	FILE *fd = NULL;
	int ret = 0;

	fd = fopen(ONE_SAMPLE, "r");
	if (!fd)
		return -1;

	fscanf(fd, "0x%X,%d,%d", &ADC_param[OUT_CCSVAL],
			&ADC_param[OUT_CCSVALCALIB], &ADC_param[OUT_CCSVALCALIBCONV]);
	fclose(fd);

	return ret;
}

/*---------------------------------------------------------------------------*
* Procedure    : Get_One_Sample_Nconv
*----------------------------------------------------------------------------*
* Description  : Read number of samples used
*---------------------------------------------------------------------------*
* Parameter(s) : None
*----------------------------------------------------------------------------*
* Return Value : 0 if success else error code
*----------------------------------------------------------------------------*/
static int Get_One_Sample_Nconv(void)
{
	FILE *fd = NULL;
	int ret = 0;

	fd = fopen(ONE_SAMPLE_NCONV_ACCU, "r");
	if (!fd)
		return -1;

	fscanf(fd, "%d,%d", &ADC_param[OUT_CCNCONVACCU],
			&ADC_param[OUT_CCNCONVACCUCONV]);
	fclose(fd);

	return ret;
}

/*---------------------------------------------------------------------------*
* Procedure    : Get_Accu_Sample
*----------------------------------------------------------------------------*
* Description  : Read accumulators registers after N samples
* ---------------------------------------------------------------------------*
* Parameter(s) : None
*----------------------------------------------------------------------------*
* Return Value : 0 if success else error code
*----------------------------------------------------------------------------*/
static int Get_Accu_Sample(void)
{
	FILE *fd = NULL;
	int ret = 0;

	fd = fopen(READ_ACCU, "r");
	if (!fd)
		return -1;

	fscanf(fd, "0x%X,%d", &ADC_param[OUT_ACCVAL],
			&ADC_param[OUT_ACCVALCONV]);
	fclose(fd);

	return ret;
}


/*---------------------------------------------------------------------------*
* Procedure    : GasGaugeAction
*----------------------------------------------------------------------------*
* Description  : Execute GasGauge Action
*----------------------------------------------------------------------------*
* Parameter(s) : uint8_t vp_swselect, action
*----------------------------------------------------------------------------*
* Return value : uint32_t, TAT error
*---------------------------------------------------------------------------*/
static int GasGaugeAction(uint8_t vp_swselect)
{
	int vl_Error = TAT_ERROR_OFF;

	switch (vp_swselect) {
	case ACTGG_INIT_CC:
		switch (ADC_param[IN_CCPWRUPENA]) {
		case 0:
			Write_To_Uint_Debugfs_File(INIT_CC_2, IN_CCPWRUPENA);
			Write_To_Uint_Debugfs_File(INIT_CC_1, IN_CCPWRUPENA);
			break;
		case 1:
			Write_To_Uint_Debugfs_File(INIT_CC_1, IN_CCPWRUPENA);
			Write_To_Uint_Debugfs_File(INIT_CC_2, IN_CCPWRUPENA);
			break;
		default:
			break;
		}
		break;

	case ACTGG_CALAUTO:
		switch (ADC_param[IN_CCINTAVGOFF]) {
		case 0:
			ADC_param[IN_CCINTAVGOFF] = 16;
			break;
		case 1:
			ADC_param[IN_CCINTAVGOFF] = 8;
			break;
		default:
			ADC_param[IN_CCINTAVGOFF] = 4;
			break;
		}
		Write_To_Uint_Debugfs_File(CALAUTO_AVG, IN_CCINTAVGOFF);
		Read_From_Uint_Debugfs_File(CALAUTO_OFFSET, OUT_CCINTAVGOFFREG);
		break;

	case ACTGG_CALMANUAL:
		Read_From_Uint_Debugfs_File(CALMANU_SAMPLE, OUT_CCSVAL);
		break;

	case ACTGG_WCCCOUNOFFREG:
		Write_To_Uint_Debugfs_File(CALMANU_OFFSET, IN_CCCOUNTOFFREG);
		break;

	case ACTGG_CCSAMPLE:
		Write_To_Uint_Debugfs_File(ONE_SAMPLE_RST, IN_RESETCC);
		Write_To_Uint_Debugfs_File(ONE_SAMPLE_MUX_OFFSET, IN_CCMUX);
		Get_One_Sample();
		Get_One_Sample_Nconv();
		break;

	case ACTGG_CCNCONVACCU:
		Write_To_Uint_Debugfs_File(READ_ACCU_RST, IN_RESETACC);
		Write_To_Uint_Debugfs_File(READ_ACCU_MUX_OFFSET, IN_CCMUXACC);
		Write_To_Uint_Debugfs_File(READ_ACCU_NB_SAMPLES, IN_NCONVACCU);
		Get_Accu_Sample();
		break;

	default:
		printf("GAS GAUGE BAD CMD\n");
		vl_Error = TAT_ERROR_CASE;
    break;
  }

	return vl_Error;
}

/*---------------------------------------------------------------------------*
* Procedure    : ADCAction
*----------------------------------------------------------------------------*
* Description  : ADC Action to execute
*----------------------------------------------------------------------------*
* Parameter(s) : uint8_t vp_swselect, action
*----------------------------------------------------------------------------*
* Return value : uint32_t, TAT error
*---------------------------------------------------------------------------*/
static int ADCAction(uint8_t vp_swselect)
{
	int ret = 0;

	Set_Average_Sample();

	switch (vp_swselect) {
	case ADCSWSEL_VBATA:
		if (ADC_param[IN_TRIGBURST]) {
			Set_Trig_Edge();
			Set_Trig_Timer();
			ret = Set_Raw_Calib_Values(HW_MAINBATV, MAINBATV_CONV);
		} else {
			ret = Set_Raw_Calib_Values(MAINBATV, MAINBATV_CONV);
		}
		ADC_float[OUT_VALCONV] += 2300 ;
		break;
	case ADCSWSEL_USBCHCUR:
		ret = Set_Raw_Calib_Values(IUSBCHARGE, IUSBCHARGE_CONV);
		break;
#ifdef HATSCONF_AB_LIB_8500
	case ADCSWSEL_MAINCHCUR:
		ret = Set_Raw_Calib_Values(IMAINCHARGE, IMAINCHARGE_CONV);
		break;
	case ADCSWSEL_MAINCHVOLT:
		ret = Set_Raw_Calib_Values(VMAINCHARGE, VMAINCHARGE_CONV);
		break;
#endif
	case ADCSWSEL_VBUS:
		ret = Set_Raw_Calib_Values(V_BUS, V_BUS_CONV);
		break;
	case ADCSWSEL_BATCTRL:
		ret = Set_Raw_Calib_Values(BATCTRL, BATCTRL_CONV);
		break;
	case ADCSWSEL_BATTEMP:
		ret = Set_Raw_Calib_Values(BATTEMP, BATTEMP_CONV);
		Set_BTemp_Deg(ADC_float[OUT_VALCONV]);
		break;
	case ADCSWSEL_BACKUPBAT:
		ret = Set_Raw_Calib_Values(BK_BAT, BK_BAT_CONV);
		break;
	case ADCSWSEL_ADCAUX1:
		ret = Set_Raw_Calib_Values(AUX1, AUX1_CONV);
		break;
	case ADCSWSEL_ADCAUX2:
		ret = Set_Raw_Calib_Values(AUX2, AUX2_CONV);
		break;
	case ADCSWSEL_ACCDET1:
		ret = Set_Raw_Calib_Values(ACC_DETECT_1, ACC_DETECT_1_CONV);
		break;
	case ADCSWSEL_ACCDET2:
		ret = Set_Raw_Calib_Values(ACC_DETECT_2, ACC_DETECT_2_CONV);
		break;

			default:
		ret = -1;
	}

	return ret;
}

/*---------------------------------------------------------------------------*
* Procedure    : cyclicRead
*----------------------------------------------------------------------------*
* Description  : Get the DTH ELement value Output Parameters
*----------------------------------------------------------------------------*
* Parameter(s) : dth_element *, Structure of DTH element
*----------------------------------------------------------------------------*
* Return value : uint32_t, TAT error
*---------------------------------------------------------------------------*/
static int cyclicRead(struct dth_element *elem)
{
	int vl_Error = TAT_ERROR_OFF;
	FILE *pFile = NULL;
	int tmp = 0;

	if (elem == NULL) {
		vl_Error = TAT_MISC_ERR;
		goto error;
	}

	if (ADC_string[IN_FILESTORAGE] == NULL) {
		vl_Error = TAT_MISC_ERR;
		goto error;
	}



	if (strcmp(ADC_string[IN_FILESTORAGE], p_previousPATHstorage) != 0) {

		v_NumberofcyclicRead = 0;
		strcpy(p_previousPATHstorage, ADC_string[IN_FILESTORAGE]);
		printf("%s Reset path: %s\n", __func__,\
				ADC_string[IN_FILESTORAGE]);

	}

	/* create the file if not exist */
	pFile = fopen(ADC_string[IN_FILESTORAGE], "r");
	if (pFile != NULL) {
		fclose(pFile);
	} else {
		printf("%s create file %s\n", __func__,\
				ADC_string[IN_FILESTORAGE]);
		v_NumberofcyclicRead = 0;
		pFile = fopen(ADC_string[IN_FILESTORAGE], "w");

		if (pFile == NULL) {
			printf("%s %s %s\n", __func__, strerror(errno),
				ADC_string[IN_FILESTORAGE]);
			vl_Error = errno;
			goto error;
		}

		fprintf(pFile, "cycle;");

		while (p_ADC_Name[tmp].v_ADCSelect != 0xff) {
			fprintf(pFile, "%s;", p_ADC_Name[tmp].p_ADCStr);
			tmp++;
		}

		fprintf(pFile, "\n");
		fclose(pFile);
	}

	pFile = fopen(ADC_string[IN_FILESTORAGE], "a");
	if (pFile == NULL) {
		printf("%s %s %s\n", __func__, strerror(errno),
				ADC_string[IN_FILESTORAGE]);
		vl_Error = errno;
		goto error;
	}

	printf("%s cycle%d\n", __func__, v_NumberofcyclicRead);
	fprintf(pFile, "%d;", v_NumberofcyclicRead);

	for (tmp = ADCSWSEL_BATCTRL; tmp <= ADCSWSEL_BACKUPBAT; tmp++) {
		ADCAction(tmp);
		ADC_param[ACT_CYCLICREAD + tmp] = ADC_param[OUT_VAL];
		fprintf(pFile, "%d;", ADC_param[ACT_CYCLICREAD + tmp]);
	}

	fprintf(pFile, "\n");
	fclose(pFile);

	v_NumberofcyclicRead++;

error:
	return vl_Error;

}

/*---------------------------------------------------------------------------*
* Procedure    : dthl41_00ActADC_exec
*----------------------------------------------------------------------------*
* Description  : Execute the Cmd Action sent by DTH
*----------------------------------------------------------------------------*
* Parameter(s) : dth_element *, the  DTH element
*----------------------------------------------------------------------------*
* Return value : uint32_t, TAT error
*---------------------------------------------------------------------------*/
int dthl41_00ActADC_exec(struct dth_element *elem)
{
	int vl_Error = 0;
	ADC_SYSLOG(LOG_DEBUG, "elem->user_data: %d\n", elem->user_data);

	switch (elem->user_data) {
	case ADCSWSEL_BATCTRL:
	case ADCSWSEL_BATTEMP:
#ifdef HATSCONF_AB_LIB_8500
	case ADCSWSEL_MAINCHVOLT:
	case ADCSWSEL_MAINCHCUR:
#endif
	case ADCSWSEL_ACCDET1:
	case ADCSWSEL_ACCDET2:
	case ADCSWSEL_ADCAUX1:
	case ADCSWSEL_ADCAUX2:
	case ADCSWSEL_RBATCTRL:
	case ADCSWSEL_VBUS:
	case ADCSWSEL_USBCHCUR:
	case ADCSWSEL_BACKUPBAT:
	case ADCSWSEL_VBATA:
		vl_Error = ADCAction(elem->user_data);
		break;
	case ACT_CYCLICREAD:
		vl_Error = cyclicRead(elem);
		break;
	case ACT_GETOTPVAL:
		SetADCGainOffsetCalib();
		break;
	case ACTGG_CALAUTO:
	case ACTGG_CALMANUAL:
	case ACTGG_CCSAMPLE:
	case ACTGG_CCNCONVACCU:
	case ACTGG_INIT_CC:
	case ACTGG_WCCCOUNOFFREG:
		vl_Error = GasGaugeAction(elem->user_data);
		break;
	default:
		vl_Error = -5;
	}
	return vl_Error;
}

/*---------------------------------------------------------------------------*
* Procedure    : dthl41_03ActADCParam_Get
*----------------------------------------------------------------------------*
* Description  : Get the DTH ELement value Output Parameters
*----------------------------------------------------------------------------*
* Parameter(s) : dth_element *, Structure of DTH element
*		  void *, value of Dth Element
*----------------------------------------------------------------------------*
* Return value : uint32_t, TAT error
*---------------------------------------------------------------------------*/
int dthl41_03ActADCParam_Get(struct dth_element *elem, void *value)
{
	int vl_Error = TAT_ERROR_OFF;
	ADC_SYSLOG(LOG_DEBUG, "elem->user_data :%d\n", elem->user_data);

	switch (elem->type) {

	case DTH_TYPE_U8:
		ADC_SYSLOG(LOG_DEBUG, "type uint8_t: %d\n", ADC_param[elem->user_data]);
		*((uint8_t *) value) = ADC_param[elem->user_data];
		break;
	case DTH_TYPE_U16:
		*((uint16_t *) value) = ADC_param[elem->user_data];
		break;
	case DTH_TYPE_S16:
		*((int16_t *) value) = ADC_param[elem->user_data];
		break;
	case DTH_TYPE_U32:
		if (elem->user_data == OUT_VALCONV)
			*((uint32_t *) value) = ADC_param[elem->user_data];
		else
			*((uint32_t *) value) = ADC_param[elem->user_data];
		break;
	case DTH_TYPE_S32:
		if (elem->user_data == OUT_VALCONV)
			*((int32_t *) value) = ADC_param[elem->user_data];
		else
			*((int32_t *) value) = ADC_param[elem->user_data];
		break;
	case DTH_TYPE_STRING:
		ADC_SYSLOG(LOG_DEBUG, "type string: %s\n", ADC_string[elem->user_data]);
		if (ADC_string[elem->user_data] != NULL)
		{
			strncpy((char *)value, ADC_string[elem->user_data],
				strlen(ADC_string[elem->user_data]));
		}
		else
		{
			strncpy((char *)value, "NULL", 5);
		}
		break;
	case DTH_TYPE_FLOAT:
		ADC_SYSLOG(LOG_DEBUG, "type float: %f\n", ADC_float[elem->user_data]);
		*((float *)value) = ADC_float[elem->user_data];
		break;
	default:
		printf("%s problem with TYPE\n", __func__);
		vl_Error = TAT_ERROR_CASE;
		break;
	}

	if (!(elem->user_data >= OUT_CCINTAVGOFFREG)) {
		ADC_param[elem->user_data] = 0;
		ADC_float[elem->user_data] = 0;
	}

	return vl_Error;
}

/*---------------------------------------------------------------------------*
* Procedure    : dthl41_02ActADCParam_Set
*----------------------------------------------------------------------------*
* Description  : Set the DTH Element value Input Parameters
*----------------------------------------------------------------------------*
* Parameter(s) : dth_element *, Structure of DTH element
*		  void *, value of Dth Element
*----------------------------------------------------------------------------*
* Return value : uint32_t, TAT error
*---------------------------------------------------------------------------*/
int dthl41_02ActADCParam_Set(struct dth_element *elem, void *value)
{
	int vl_Error = TAT_ERROR_OFF;
	ADC_SYSLOG(LOG_DEBUG, "value=%s\n", (char * )value);
	switch (elem->type) {
		case DTH_TYPE_U8:
    ADC_param[elem->user_data]= *(uint8_t *)value;
    break;
	case DTH_TYPE_S8:
    ADC_param[elem->user_data]= *(int8_t*)value;
    break;
	case DTH_TYPE_U16:
    ADC_param [elem->user_data]= *(uint16_t *)value;
    break;
	case DTH_TYPE_S16:
    ADC_param[elem->user_data]= *(int16_t*)value;
    break;
	case DTH_TYPE_U32:
    ADC_param[elem->user_data]= *(uint32_t *)value;
    break;
	case DTH_TYPE_S32:
    ADC_param[elem->user_data]= *(int32_t*)value;
    break;
	case DTH_TYPE_U64:
    ADC_param[elem->user_data]= *(uint64_t *)value;
    break;
	case DTH_TYPE_S64:
    ADC_param[elem->user_data]= *(int64_t*)value;
    break;
	case DTH_TYPE_STRING:
		if ((char *)value != NULL) {
			int len = strlen((char *)value);
			if (ADC_string[elem->user_data] != NULL)
				free(ADC_string[elem->user_data]);

			ADC_string[elem->user_data] =
			    (char *)calloc(len + 1, sizeof(char));
			strncpy(ADC_string[elem->user_data], (char *)value,
				len);
		}
		break;
	default:
		printf("%s problem with TYPE\n", __func__);
		vl_Error = TAT_ERROR_CASE;
		break;
	}
	return vl_Error;
}

