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

#ifndef TATL06I2C_H_
#define TATL06I2C_H_

/* ----------------------------------------------------------------------- */
/* Constants                                                               */
/* ----------------------------------------------------------------------- */
#define	I2C0_DEVICE_NAME		"/dev/i2c-0"
#define	I2C0_DEVICE_ADRESS		0x44 /* TC35893 KeyPad Expander. */
#define	I2C0_REGISTER_ADRESS	0x81 /* Software version register. */
#define	I2C2_DEVICE_NAME		"/dev/i2c-2"
#define	I2C2_DEVICE_ADRESS		0x33 /* LP5521TMX. */
#define	I2C2_REGISTER_ADRESS	0x81
#define	I2C3_DEVICE_NAME		"/dev/i2c-3"
#define	I2C3_DEVICE_ADRESS		0x4B /* Touch Screen */
#define	I2C3_REGISTER_ADRESS	0x7E /* Dummy register */

/* ----------------------------------------------------------------------- */
/* Typedefs                                                                */
/* ----------------------------------------------------------------------- */

/**
 * Definition of data needed to manage display
 */
typedef struct {
	DTH_COEX_ORDER order; /**< Task to be done by the process (START/STOP). */
	DTH_COEX_STATE state; /**< State of the process (ACTIVATED/DEACTIVATED). */
	char *error ; /**< Error string .*/

	char *deviceName ; /**< Name of I2C device. */
	unsigned char deviceAddress ; /**< Adress of the device on I2C bus. */
	unsigned char registerAddress ; /**< Adress of the register to be accessed. */
} DTH_I2C_PROCESS_DATA ;

/* ----------------------------------------------------------------------- */
/* Global variables                                                        */
/* ----------------------------------------------------------------------- */
DTH_I2C_PROCESS_DATA v_tatcoex_i2c0_data; /* Data needed for I2C0. */
DTH_I2C_PROCESS_DATA v_tatcoex_i2c2_data; /* Data needed for I2C2. */
DTH_I2C_PROCESS_DATA v_tatcoex_i2c3_data; /* Data needed for I2C3. */
pthread_t v_tatcoex_i2c0_thread; /* Thread to generate continuous access on I2C0 bus. */
pthread_t v_tatcoex_i2c2_thread; /* Thread to generate continuous access on I2C2 bus. */
pthread_t v_tatcoex_i2c3_thread; /* Thread to generate continuous access on I2C3 bus. */

/* ----------------------------------------------------------------------- */
/*  Main DTH entry points                                                  */
/* ----------------------------------------------------------------------- */

/**
 * Start/Stop traffic on MC buses.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl06_00I2C_Exec(struct dth_element *elem);

/**
 * Get outputs arguments.
 * @param[in] elem specifies the DTH element.
 * @param[in] value specifies the argument to set.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl06_01I2C_Get(struct dth_element *elem, void *value);

/**
 * Set inputs arguments.
 * @param[in] elem specifies the DTH element.
 * @param[in] value specifies the argument to set.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl06_02I2C_Set(struct dth_element *elem, void *value);

/* ----------------------------------------------------------------------- */
/* Initialization                                                          */
/* ----------------------------------------------------------------------- */

/**
 *  Initialize data structure needed for I2C buses.
 *  @param[in] data data needed to manage I2C buses.
 *  @param[in] deviceInstance I2C bus number used by the device.
 *  @param[in] deviceAddress Adress of the device on I2C bus.
 *  @param[in] registerAddress Adress of the register to be accessed.
 */
void tatl06_03Init_I2C_Data(DTH_I2C_PROCESS_DATA *data, char *deviceName,
		unsigned char deviceAddress, unsigned char registerAddress);

#endif /* TATL06I2C_H_ */

