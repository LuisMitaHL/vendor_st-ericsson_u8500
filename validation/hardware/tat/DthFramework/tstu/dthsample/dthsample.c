#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>		/*sleep */

#include "dthsample.h"

uint32_t reg1 = 1234;
uint32_t reg2 = 4444;
uint64_t lo_reg2 = 20;
uint64_t reserved1_reg2 = 30;
uint64_t high_reg2 = 40;
uint16_t table1[5][8] = { {0, 1, 2, 3, 4, 5, 6, 7}
,
{8, 9, 10, 11, 12, 13, 14, 15}
,
{16, 17, 18, 19, 20, 21, 22, 23}
,
{24, 25, 26, 27, 28, 29, 30, 31}
,
{32, 33, 34, 35, 36, 37, 38, 39}
};
struct dth_array table1_array = { 8, 5, (int64_t *) table1 };

char *string, *execution, *input, *output;
uint32_t reg1_value;
int8_t reg4_value, gain;
uint64_t reg5_value;
uint32_t reg6_value;
const int minGain = -5;

struct dth_element my_elements[] = {
	/* simple item */
	{"/DTH/audio/reg1", DTH_TYPE_U32, 0, 0, NULL, NULL, 100, 5000, reg1_get, reg1_set, NULL, 0, (void *)(int64_t *) & reg1},
	{"/DTH/audio/reg2", DTH_TYPE_STRING, 0, 0, NULL, NULL, 0, 0, reg2_get, reg2_set, NULL, 1, NULL},
	{"/DTH/audio/reg3", DTH_TYPE_STRING, 0, 0, NULL, NULL, 0, 0, NULL, NULL, NULL, 2, NULL},
	{"/DTH/audio/reg4", DTH_TYPE_S8, 0, 0, NULL, NULL, 0, 0, reg4_get, NULL, NULL, 3, NULL},
	{"/DTH/audio/reg5", DTH_TYPE_U64, 0, 0, "0\n1\n2\n3", NULL, 0, 0, NULL, reg5_set, NULL, 4, NULL},

	/* array */
	{"/DTH/audio/table1", DTH_TYPE_U16, 8, 5, NULL, NULL, 0, 100, table1_get, table1_set, NULL, 5, (void *)&table1_array},

	/* bitfield */
	{"/DTH/audio/reg6", DTH_TYPE_U32, 0, 0, NULL, NULL, 0, 0, my_bitfield_get, my_bitfield_set, NULL, 6, (void *)(int64_t *) & reg2},
	{"/DTH/audio/reg6/lo", 8, 0, 0, NULL, NULL, 15, 200, my_bitfield_get, my_bitfield_set, NULL, 7, (void *)&lo_reg2},
	{"/DTH/audio/reg6/reserved1", 8, 0, 0, NULL, NULL, 0, 0, NULL, NULL, NULL, 8, (void *)&reserved1_reg2},
	{"/DTH/audio/reg6/high", 16, 0, 0, NULL, NULL, 0, 0, my_bitfield_get, NULL, NULL, 9, (void *)&high_reg2},

	/* action */
	{"/DTH/audio/Audio_Loop", DTH_TYPE_STRING, 0, 0, NULL, NULL, 0, 0, my_action_get, NULL, my_action_exec, 9, NULL},
	{"/DTH/audio/Audio_Loop/Execution", DTH_TYPE_STRING, 0, 0, "Start\nStop", NULL, 0, 0, my_action_get, my_action_set, NULL, 10, NULL},
	{"/DTH/audio/Audio_Loop/Input_Accessory", DTH_TYPE_STRING, 0, 0, "MIC\nHeadset\nBT", NULL, 0, 0, NULL, my_action_set, NULL, 11, NULL},
	{"/DTH/audio/Audio_Loop/Output_Accessory", DTH_TYPE_STRING, 0, 0, "Earpiece\nHeadset\nBT", NULL, 0, 0, NULL, my_action_set, NULL, 12, NULL},
	{"/DTH/audio/Audio_Loop/Gain_in_dB", DTH_TYPE_S8, 0, 0, NULL, NULL, -5, 10, NULL, my_action_set, NULL, 13, NULL},
};


/* Example of registration code implementation in a DTH 9P Service*/


int dth_init_service()
{

	int result;

	string = calloc(500, sizeof(char));
	input = calloc(500, sizeof(char));
	output = calloc(500, sizeof(char));
	execution = calloc(500, sizeof(char));

	string = strcpy(string, "9p is funny\n");
	execution = strcpy(execution, "Stop");
	output = strcpy(output, "BT");
	input = strcpy(input, "MIC");

	reg1_value = 4567;
	reg4_value = 254;
	reg5_value = 56789;
	reg6_value = 0;
	gain = 10;

	struct dth_element *ptr = my_elements;
	while (ptr->path != NULL) {
		result = dth_register_element(ptr);
		if (result != 0)
			break;
		ptr++;
	}
	return 1;

}


/* Examples of callbacks implementation in a DTH 9P Service*/


int reg1_get(struct dth_element *elem, void *value)
{
	uint32_t *p1 = (uint32_t *) & reg1_value;
	uint32_t *p2 = (uint32_t *) value;
	*p2 = *p1;
	return 0;
}

int reg1_set(struct dth_element *elem, void *value)
{
	uint32_t *p1 = (uint32_t *) & reg1_value;
	uint32_t *p2 = (uint32_t *) value;
	*p1 = *p2;
	return 0;
}

int reg2_get(struct dth_element *elem, void *value)
{
	char *p2 = (char *)value;
	p2 = strcpy(p2, string);
	return 0;
}

int reg2_set(struct dth_element *elem, void *value)
{
	char *p2 = (char *)value;
	memset(string, '\0', strlen(string));
	string = strcpy(string, p2);

	return 0;
}

int reg4_get(struct dth_element *elem, void *value)
{
	int8_t *p = (int8_t *) value;
	*p = reg4_value;
	return 0;
}

int reg5_get(struct dth_element *elem, void *value)
{
	uint64_t *p = (uint64_t *) value;
	*p = reg5_value;
	return 0;
}

int reg5_set(struct dth_element *elem, void *value)
{
	uint64_t *p2 = (uint64_t *) value;

	switch (*p2) {
	case 0:
		reg5_value = 1111;
		break;
	case 1:
		reg5_value = 2222;
		break;
	case 2:
		reg5_value = 3333;
		break;
	case 3:
		reg5_value = 4444;
		break;
	default:
		break;
	}
	return 0;
}

int table1_get(struct dth_element *elem, void *value)
{

	struct dth_array *value_array = (struct dth_array *)value;
	int row, col;

	row = value_array->row;
	col = value_array->col;

	if ((row == elem->rows) && (col == elem->cols)) {	/* Access to the whole array. */
		int i;
		for (i = 0; i < elem->cols * elem->rows; i++)
			*(value_array->array + i) = *(table1_array.array + i);

	} else {		/* Access to a single array element. */
		/*int offset = 0 ;
		   offset = row * (elem->cols * sizeof(uint16_t));
		   offset += col * sizeof(uint16_t);
		   *(value_array->array) = *(table_array.array+ */
		*(value_array->array) = (int64_t) table1[row][col];
	}

	return 0;
}

int table1_set(struct dth_element *elem, void *value)
{
	struct dth_array *value_array = (struct dth_array *)value;
	int row, col;

	row = value_array->row;
	col = value_array->col;

	if ((row == elem->rows) && (col == elem->cols)) {	/* Access to the whole array. */
		int i;
		for (i = 0; i < elem->cols * elem->rows; i++)
			*(table1_array.array + i) = *(value_array->array + i);

	} else {		/* Access to a single array element. */
		table1[row][col] = (int16_t) (*(value_array->array));
	}

	return 0;
}

int my_bitfield_get(struct dth_element *elem, void *value)
{
	switch (elem->user_data) {
	case 6:
		{
			uint32_t *p = (uint32_t *) value;
			*p = reg6_value;
		}
		break;

	case 7:
		{
			uint64_t *p = (uint64_t *) value;
			*p = (uint64_t) (reg6_value & 0x000000FF);
		}
		break;

	case 8:
		break;

	case 9:
		{
			uint64_t *p = (uint64_t *) value;
			*p = (uint64_t) ((reg6_value & 0xFFFF0000) >> 16);
		}

	default:
		break;
	}

	return 0;
}

int my_bitfield_set(struct dth_element *elem, void *value)
{
	switch (elem->user_data) {
	case 6:
		{
			uint32_t *p = (uint32_t *) value;
			reg6_value = *p;
		}
		break;

	case 7:
		{
			uint32_t *p = (uint32_t *) value;
			uint8_t inter = *p & 0x000000FF;
			reg6_value = reg6_value | ((uint32_t) (inter));
		}
		break;

	case 8:
		break;

	case 9:
		break;

	default:
		break;
	}

	return 0;
}

int my_action_get(struct dth_element *elem, void *value)
{
	/* elem->path gives the (sub-element) path */
	/* elem->user_data gives the user_data, e.g. an ID used in a switch/case */

	char *p = (char *)value;

	switch (elem->user_data) {
	case 9:
		{
			if (strncmp(execution, "Start", 5) == 0) {
				int value = 0;
				execution = strcpy(execution, "Running");
				value = my_action_exec(elem);
				if (value < 0)
					p = strcpy(p, "KO");
				else
					p = strcpy(p, "OK");

				execution = strcpy(execution, "Finished");
			} else if (strncmp(execution, "Stop", 4) == 0) {
				execution = strcpy(execution, "Cancelled");
				p = strcpy(p, "OK");
			}
		}
		break;

	case 10:
		p = strcpy(p, execution);
		break;

	default:
		break;
	}

	return 0;
}

int my_action_set(struct dth_element *elem, void *value)
{
	/* elem->path gives the (sub-element) path */
	/* elem->user_data gives the user_data, e.g. an ID used in a switch/case */

	switch (elem->user_data) {
	case 10:	/*set "Execution" argument value = store it for next execution */
		{
			char *p = (char *)value;
			if (strncmp(p, "0", 1) == 0)
				execution = strcpy(execution, "Start");
			else if (strncmp(p, "1", 1) == 0)
				execution = strcpy(execution, "Stop");

			printf("%s\n", execution);
		}
		break;

	case 11: /* set "Input Accessory" argument value = store it for next execution */
		{
			char *p = (char *)value;
			if (strncmp(p, "0", 1) == 0)
				input = strcpy(input, "MIC");
			else if (strncmp(p, "1", 1) == 0)
				input = strcpy(input, "Headset");
			else if (strncmp(p, "2", 1) == 0)
				input = strcpy(input, "BT");
			printf("%s\n", input);
		}
		break;

	case 12: /*set "Output Accessory" argument value = store it for next execution */
		{
			char *p = (char *)value;
			if (strncmp(p, "0", 1) == 0)
				output = strcpy(output, "Earpiece");
			else if (strncmp(p, "1", 1) == 0)
				output = strcpy(output, "Headset");
			else if (strncmp(p, "2", 1) == 0)
				output = strcpy(output, "BT");

			printf("%s\n", output);
		}
		break;

	case 13: /* set "Gain in dB" argument value = store it for next execution */
		{
			int8_t *p = (int8_t *) value;
			gain = *p;
			printf("%d\n", gain);
		}
		break;

	default:
		break;
	}

	return 0;
}

int my_action_exec(struct dth_element *elem)
{
	printf("debut\n");
	int i = 0;
	while ((strncmp(execution, "Running", 5) == 0) && (i < 10)) {
		printf("%s %s %s %c\n", execution, output, input, gain);
		sleep(1);
		i++;
	}
	printf("fin\n");
	return 0;
}
