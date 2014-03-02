/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides API routines in user land for the AB8500 driver
* \author  ST-Ericsson
*/
/*****************************************************************************/
#include <libab.h>

// for V2 files
unsigned char I2CRead( unsigned short  register_address)
{
	unsigned char val;
	int ret;

	ret = abxxxx_read(register_address, &val);
	if( ret < 0)
		return ret;

    return val;
}

void I2CWrite( unsigned short register_address, unsigned char data )
{

	abxxxx_write(register_address, data);

}
// for V1 files
unsigned char SPIRead( unsigned short  register_address)
{
	unsigned char val;
	int ret;

	ret = abxxxx_read(register_address, &val);
	if( ret < 0)
		return ret;

    return val;
}

void SPIWrite( unsigned short register_address, unsigned char data )
{

	abxxxx_write(register_address, data);

}

