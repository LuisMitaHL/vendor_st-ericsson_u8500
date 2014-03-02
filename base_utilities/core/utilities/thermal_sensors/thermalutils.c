/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>
#include <expat.h>
#include <cutils/log.h>
#include "thermalutils.h"

#define SENSORS_DEBUG

#define MAX_SENSOR_NAME_LEN   		50
#define NO_LINEAR_INTERPOLATION		0
#define DO_LINEAR_INTERPOLATION		1

#define ELEMENT_SENSORS			"Sensors"
#define ELEMENT_COUNT			"Count"
#define ELEMENT_SENSOR			"Sensor"
#define ELEMENT_CHARACTERISTICS		"Characteristics"
#define ELEMENT_INFLEXIONPOINT		"InflexionPoint"
#define ATTR_NAME			"name"
#define ATTR_COUNT			"count"
#define ATTR_TEMP			"temp"
#define ATTR_ADCOUT			"adcout"

#define SENSORS_XML_CONFIG		"/system/etc/thermal_sensors.xml"

enum ELEMENT_TAGS {
	ELE_SENSORS,
	ELE_SENSOR,
	ELE_COUNT,
	ELE_CHARACTERISTICS,
	ELE_INFLEXIONPOINT,
};

enum TEMP_CONVERSION {
	TEMP_CONV_MV_DC,
	TEMP_CONV_DC_MV,
};

struct xml_data {
	int xml_stag;
	int depth;
	int curr_sens;
	int curr_inflx;
	XML_Parser parser;
};

struct sensor_inflexion {
	short adcout;
	short temp;
};

struct sensor_attr {
	char name[MAX_SENSOR_NAME_LEN];
	int inflex_count;
	struct sensor_inflexion* inflexions;
};

struct sensors_list {
	int sensor_count;
	struct sensor_attr* attrs;
};

static struct sensors_list sensors;

void init_sensor_utils(void);
void deinit_sensor_utils(void);
int convert_sensor_mv_dc(const char *, const int, int *);
int convert_sensor_dc_mv(const char *, const int, int *);

static int convert_sensor_output(const char *, const int, int *, int);
static int find_inflex_index(int, int, int *, int);
static int linear_interpolation(int, int, int, int);
static int find_from_lookup_table(int, int, int);
static int parse_sensor_config(const char *);
static void xml_start_cb(void *, const XML_Char *, const XML_Char **);
static void xml_end_cb(void *, const XML_Char *);
static void xml_data_cb(void *, const XML_Char *, int);
static void xml_start(struct xml_data *, const XML_Char **);
static void sensors_release(void);
static void dumpsensors();

void init_thermal_utils(void)
{
	ALOGD("Thermalutils: Initializing sensor resources");
}

void deinit_thermal_utils(void)
{
	sensors_release();
}

static void sensors_release(void)
{
	int i;

	ALOGD("Thermalutils: Releasing sensor resources");
	if (sensors.attrs) {
		for (i=0; i<sensors.sensor_count; i++) {
			if ((sensors.attrs+i)->inflexions) {
				free((sensors.attrs+i)->inflexions);
				(sensors.attrs+i)->inflexions = NULL;
			}
		}
		free(sensors.attrs);
		sensors.attrs = NULL;
	}
}

int convert_sensor_mv_dc(const char *name, const int input, int *output)
{
	return convert_sensor_output(name, input, output, TEMP_CONV_MV_DC);
}

int convert_sensor_dc_mv(const char *name, const int input, int *output)
{
	return convert_sensor_output(name, input, output, TEMP_CONV_DC_MV);
}

static int convert_sensor_output(const char* name, const int input,
				 int *output, int conv)
{
	int sens_ind=-1, inf_ind=-1;
	int ret;

	if (!sensors.attrs) {
		ret = parse_sensor_config(SENSORS_XML_CONFIG);
		if (ret) {
			ALOGE("Thermalutils: Failed to parse sensor config:%d", ret);
			return ret;
		}
#ifdef SENSORS_DEBUG
		dumpsensors();
#endif
	}

	/* check for valid sensor and its index */
	for (sens_ind=0; sens_ind<sensors.sensor_count; sens_ind++) {
		if (!strncmp(name, (sensors.attrs+sens_ind)->name,
				MAX_SENSOR_NAME_LEN))
			break;
	}

	if (sens_ind >= sensors.sensor_count) {
		ALOGE("Thermalutils: Requested sensor not found");
		return ERROR_SENSOR_NOT_FOUND;
	}

	/* find inflexion point */
	ret = find_inflex_index(sens_ind, input, &inf_ind, conv);
	if (ret == ERROR_INVALID_INPUT) {
		ALOGE("Thermalutils: Index not found, invalid input");
		return ret;
	}

	if (ret == NO_LINEAR_INTERPOLATION) {
		ALOGD("Thermalutils: No interpolation required");
		*output = find_from_lookup_table(sens_ind, inf_ind, conv);
		return 0;
	}

	/* Perform interpolation */
	*output = linear_interpolation(sens_ind, inf_ind, input, conv);

	return 0;
}

static int find_inflex_index(int sens_ind, int input,
			int *inflex_ind, int conv_type)
{
	int ind;
	int ret = ERROR_INVALID_INPUT;
	int count = (sensors.attrs+sens_ind)->inflex_count;
	struct sensor_inflexion *inflex = (sensors.attrs+sens_ind)->inflexions;

	switch (conv_type) {
	case TEMP_CONV_MV_DC:
		for (ind=0; ind<count-1; ind++) {
			if ((input == (inflex+ind)->adcout)) {
				*inflex_ind = ind;
				ret = NO_LINEAR_INTERPOLATION;
				break;
			}
			if ((input == (inflex+ind+1)->adcout)) {
				*inflex_ind = ind+1;
				ret = NO_LINEAR_INTERPOLATION;
				break;
			}

			if ((input < (inflex+ind)->adcout) &&
			   (input > (inflex+(ind+1))->adcout)) {
				*inflex_ind = ind;
				ret = DO_LINEAR_INTERPOLATION;
				break;
			}
		}
		break;

	case TEMP_CONV_DC_MV:
		for (ind=0; ind<count-1; ind++) {
			if ((input == (inflex+ind)->temp)) {
				*inflex_ind = ind;
				ret = NO_LINEAR_INTERPOLATION;
				break;
			}
			if ((input == (inflex+ind+1)->temp)) {
				*inflex_ind = ind+1;
				ret = NO_LINEAR_INTERPOLATION;
				break;
			}

			if ((input > (inflex+ind)->temp) &&
			   (input < (inflex+(ind+1))->temp)) {
				*inflex_ind = ind;
				ret = DO_LINEAR_INTERPOLATION;
				break;
			}
		}
		break;

	default:
		ALOGD("Thermalutils: Invalid convertion type");
		break;
	}
	return ret;
}

/*
 * Linear interpolation is used to calculate
 * intermediate points
 * x2 = ((y2 - y1)(x3 - x1) / (y3 - y1)) + x1
 * y2 = ((x2 - x1)(y3 - y1) / (x3 - x1)) + y1
 */
static int linear_interpolation(int sens_ind, int inf_ind,
				int input, int conv_type)
{
	float result=0;
	int p1 = inf_ind;
	int p3 = inf_ind+1;
	struct sensor_inflexion *inf;
	inf  = (sensors.attrs+sens_ind)->inflexions;

	switch(conv_type) {
	case TEMP_CONV_MV_DC:
		result = (float)(input-(inf+p1)->adcout) *
			((inf+p3)->temp - (inf+p1)->temp) /
			((inf+p3)->adcout - (inf+p1)->adcout) +
			(inf+p1)->temp;
		break;

	case TEMP_CONV_DC_MV:
		result = (float)(input-(inf+p1)->temp) *
			((inf+p3)->adcout - (inf+p1)->adcout) /
			((inf+p3)->temp - (inf+p1)->temp) +
			(inf+p1)->adcout;
		break;

	default:
		ALOGD("Thermalutils: Invalid conversion type");
		break;
	}
	/* roundoff to zero decimal precision */
	if (result<0)
		return (result-0.5);
	else
		return (result+0.5);
}

static int find_from_lookup_table(int sens_ind, int inf_ind, int conv_type)
{
	int result=0;

	switch(conv_type) {
	case TEMP_CONV_MV_DC:
		result = ((sensors.attrs+sens_ind)->inflexions+inf_ind)->temp;
		break;
	case TEMP_CONV_DC_MV:
		result = ((sensors.attrs+sens_ind)->inflexions+inf_ind)->adcout;
		break;
	default:
		ALOGD("Thermalutils: Invalid conversion type");
		break;
	}

	return result;
}

static int parse_sensor_config(const char *file)
{
	char *buffer;
	int len, ret=0;
	struct xml_data xdata = {-1, -1, -1, -1, 0};

	FILE *fp = fopen(file, "r");
	if (!fp) {
		ALOGE("Thermalutils: Failed to open sensor config file");
		return ERROR_INVALID_XML_CONFIG;
	}

	/* read sensor xml config */
	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	buffer = (char*) malloc(len);
	if (!buffer) {
		ALOGE("Thermalutils: Failed to allocate memory\n");
		fclose(fp);
		return ERROR_OUT_OF_MEMORY;
	}
	fread(buffer, len, 1, fp);
	fclose(fp);

	XML_Parser parser = XML_ParserCreate("UTF-8");
	if (!parser) {
		ALOGE("Thermalutils: Failed to create xml parser\n");
		ret = ERROR_OUT_OF_MEMORY;
		goto out_buffer;
	}

	xdata.parser = parser;
	XML_SetUserData(parser, &xdata);
	XML_SetElementHandler(parser, xml_start_cb, xml_end_cb);
	XML_SetCharacterDataHandler(parser, xml_data_cb);

	/* parse xml buffer */
	if (XML_Parse(parser, buffer, len, XML_TRUE) == XML_STATUS_ERROR) {
		ALOGE("Thermalutils: Error while parsing : %s\n",
				XML_ErrorString(XML_GetErrorCode(parser)));
		sensors_release();
		ret = (XML_GetErrorCode(parser) == XML_ERROR_ABORTED) ?
				ERROR_OUT_OF_MEMORY : ERROR_INVALID_XML_CONFIG;
	}

	XML_ParserFree(parser);
out_buffer:
	free(buffer);
	return ret;
}

static void xml_start_cb(void *userdata,
			 const XML_Char *name,
			 const XML_Char **attr)
{
	struct xml_data *xdata = (struct xml_data*)userdata;

	if (!strcmp(name, ELEMENT_SENSORS)) {
		xdata->xml_stag = ELE_SENSORS;
		xdata->depth += 1;
	} else if (!strcmp(name, ELEMENT_COUNT)) {
		xdata->xml_stag = ELE_COUNT;
	} else if (!strcmp(name, ELEMENT_SENSOR)) {
		xdata->xml_stag = ELE_SENSOR;
		xdata->curr_sens += 1;
		xdata->curr_inflx =-1;
	} else if (!strcmp(name, ELEMENT_CHARACTERISTICS)) {
		xdata->xml_stag = ELE_CHARACTERISTICS;
		xdata->depth += 1;
	} else if (!strcmp(name, ELEMENT_INFLEXIONPOINT)) {
		xdata->xml_stag = ELE_INFLEXIONPOINT;
		xdata->curr_inflx += 1;
		xdata->depth += 1;
	} else {
		ALOGD("Thermalutils: Unknown xml tag found");
	}
	xml_start(xdata, attr);
}

static void xml_end_cb(void *userdata, const XML_Char *name)
{
	struct xml_data *xdata = (struct xml_data*)userdata;

	if (!strcmp(name, ELEMENT_SENSORS)) {
		xdata->depth -=1;
	} else 	if (!strcmp(name, ELEMENT_COUNT)) {
	} else 	if (!strcmp(name, ELEMENT_SENSOR)) {
	} else 	if (!strcmp(name, ELEMENT_CHARACTERISTICS)) {
		xdata->depth -=1;
	} else 	if (!strcmp(name, ELEMENT_INFLEXIONPOINT)) {
		xdata->depth -=1;
	} else {
		ALOGD("Thermalutils: Unknown xml tag found");
	}
}

static void xml_data_cb(void *userdata,
			const XML_Char *s,
			int len)
{
	int val=0;
	char data[10];
	struct xml_data *xdata = (struct xml_data*)userdata;

	switch (xdata->xml_stag) {
		case ELE_SENSORS:
			break;

		case ELE_COUNT:
			strncpy(data, s, len);
			val = atoi(data);
			sensors.sensor_count = val;
			sensors.attrs = malloc(val*sizeof(struct sensor_attr));
			if (!sensors.attrs) {
				ALOGE("Thermalutils: Failed to allocate memory");
				XML_StopParser(xdata->parser, XML_FALSE);
			}
			break;

		case ELE_SENSOR:
		case ELE_CHARACTERISTICS:
		case ELE_INFLEXIONPOINT:
			break;
		default:
			ALOGD("Thermalutils: Unknown xml tag found");
			break;
	}
}

static void xml_start(struct xml_data *xdata, const XML_Char **attr)
{
	int i, count=0, temp=0, adcout=0;
	int inflx_ind=-1;
	int sens_ind = xdata->curr_sens;

	switch (xdata->xml_stag) {
		case ELE_SENSORS:
		case ELE_COUNT:
			break;

		case ELE_SENSOR:
			for (i = 0; attr[i]; i+=2) {
				if (!strcmp(attr[i], ATTR_NAME)) {
					strcpy((sensors.attrs+sens_ind)->name,
						attr[i+1]);
				}
			}
			break;

		case ELE_CHARACTERISTICS:
			for (i = 0; attr[i]; i+=2) {
				if (!strcmp(attr[i], ATTR_COUNT)) {
					count = atoi(attr[i+1]);
					(sensors.attrs+sens_ind)->inflexions = (struct sensor_inflexion*)
							malloc(count*sizeof(struct sensor_inflexion));
					if (!(sensors.attrs+sens_ind)->inflexions) {
						ALOGE("Thermalutils: Failed to allocate memory");
						XML_StopParser(xdata->parser, XML_FALSE);
					}

					(sensors.attrs+sens_ind)->inflex_count = count;
				}
			}
			break;

		case ELE_INFLEXIONPOINT:
			inflx_ind = xdata->curr_inflx;
			for (i = 0; attr[i]; i+=2) {
				if (!strcmp(attr[i], ATTR_TEMP)) {
					temp = atoi(attr[i+1]);
					((sensors.attrs+sens_ind)->inflexions+inflx_ind)->temp = temp;
				}
				if (!strcmp(attr[i], ATTR_ADCOUT)) {
					adcout = atoi(attr[i+1]);
					((sensors.attrs+sens_ind)->inflexions+inflx_ind)->adcout = adcout;
				}
			}

		default:
			break;
	}
}

#ifdef SENSORS_DEBUG
static void dumpsensors()
{
	int i, j;
	struct sensor_attr *attr;

	ALOGD("======DEBUG INFO======");
	ALOGD("SENSORS_COUNT	:%d", sensors.sensor_count);
	for (i=0; i<sensors.sensor_count; i++) {
		attr = sensors.attrs+i;
		/* sensor name */
		ALOGD("======SENSOR=======\n");
		ALOGD("NAME		: %s\n", attr->name);
		ALOGD("INFLEX COUNT	: %d\n", attr->inflex_count);

		for(j=0; j< attr->inflex_count; j++) {
			struct sensor_inflexion* inf = attr->inflexions+j;
			ALOGD("TEMP	: %d\tADCOUT	:%d\n", inf->temp, inf->adcout);
		}
	}
}
#endif /* SENSORS_DEBUG */
