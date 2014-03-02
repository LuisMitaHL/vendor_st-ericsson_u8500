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

#include "tatlhdmi.h"

/*----------------------------------------------------------------------------*
* Procedure    : Main
*-----------------------------------------------------------------------------*
* Description  : Allow user to read or write in HDMI register via Shell Command
		tathdmi r 0xAdd or tathdmi w 0xAdd 0xVal
*-----------------------------------------------------------------------------*
* Parameter(s) :
*-----------------------------------------------------------------------------*
* Return Value : u32, TAT error
*----------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
	struct hdmi_register hdmi_reg;

	long tempval = 0;
	int vl_Error = 0;
	if ((argc == 3) && ((argv[1][0] == 'r') || (argv[1][0] == 'R'))) {
		tempval = strtol(argv[2], NULL, 0);
		hdmi_reg.offset = (uint8_t) tempval;
		hdmi_reg.value = 0;

		vl_Error = tatlx1_03hdmiReg(IOC_HDMI_REGISTER_READ, &hdmi_reg);

		if (vl_Error == 0) {
			printf("READ: Address=0x%x Data=0x%x\n",
			       (uint32_t) hdmi_reg.offset,
			       (uint32_t) hdmi_reg.value);
		} else {
			printf("READ: ERROR vl_Error%d\n", vl_Error);
		}
	} else if ((argc == 4) && ((argv[1][0] == 'w') || (argv[1][0] == 'W'))) {
		tempval = strtol(argv[2], NULL, 0);
		hdmi_reg.offset = (uint8_t) tempval;
		tempval = strtol(argv[3], NULL, 0);
		hdmi_reg.value = (uint8_t) tempval;
		vl_Error = tatlx1_03hdmiReg(IOC_HDMI_REGISTER_WRITE, &hdmi_reg);

		vl_Error = tatlx1_03hdmiReg(IOC_HDMI_REGISTER_READ, &hdmi_reg);

		if (vl_Error == 0) {
			printf("WRITE: Address=0x%x Data=0x%x\n",
			       (uint32_t) hdmi_reg.offset,
			       (uint32_t) hdmi_reg.value);
		} else {
			printf("WRITE: ERROR vl_Error%d\n", vl_Error);
		}
	} else {
		printf("tathdmi R <@ register>(8bits)\n");
		printf("tathdmi W <@ register>(8bits)  <val>(8bits)\n");
	}

	return vl_Error;
}
