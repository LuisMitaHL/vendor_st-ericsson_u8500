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

#ifndef TATL02MEMORY_H_
#define TATL02MEMORY_H_

/* ----------------------------------------------------------------------- */
/* Constants                                                               */
/* ----------------------------------------------------------------------- */
#define	MC0_TARGET		"/usr/local/bin/tat/tatlcoex/mc0"
#define	MC0_FILE		"/usr/local/bin/tat/tatlcoex/mc0/mc0process"
#define	MC2_TARGET		"/usr/local/bin/tat/tatlcoex/mc2"
#define	MC2_FILE		"/usr/local/bin/tat/tatlcoex/mc0/mc2process"
#define	MC4_TARGET		"/usr/local/bin/tat/tatlcoex/mc4"
#define	MC4_FILE		"/usr/local/bin/tat/tatlcoex/mc4/mc4process"

char block_name_mc0[20];
char block_type_mc0[10];
char block_name_mc2[20];
char block_type_mc2[10];
char block_name_mc4[20];
char block_type_mc4[10];

/* ----------------------------------------------------------------------- */
/* Typedefs                                                                */
/* ----------------------------------------------------------------------- */

/**
 * Definition of data needed to manage the memory process
 */
typedef struct{
	DTH_COEX_ORDER order; /**< Task to be done by the process. */
	DTH_COEX_STATE state; /**< State of the process. */
	char *error ; /**< Error string .*/
	char *source; /**< Path to the special device file that enables access to the memory device. */
	char *target; /**< Directory where the memory device must be mounted. */
	char *file ; /**< File name where perform continuous writing accesses. */
	char *fstype ; /**< Filesystem type. */
} DTH_MEMORY_PROCESS_DATA ;


/* ----------------------------------------------------------------------- */
/* Global variables                                                        */
/* ----------------------------------------------------------------------- */
DTH_MEMORY_PROCESS_DATA v_tatcoex_mc0_data; /* Data needed for MC0 process. */
DTH_MEMORY_PROCESS_DATA v_tatcoex_mc2_data; /* Data needed for MC2 process. */
DTH_MEMORY_PROCESS_DATA v_tatcoex_mc4_data; /* Data needed for MC4 process. */
pthread_t v_tatcoex_mc0_thread; /* Thread to generate continuous access on MMC/SD. */
pthread_t v_tatcoex_mc2_thread; /* Thread to generate continuous access on the pop eMMC. */
pthread_t v_tatcoex_mc4_thread; /* Thread to generate continuous acess on the first eMMC. */

/* ----------------------------------------------------------------------- */
/*  Main DTH entry points                                                  */
/* ----------------------------------------------------------------------- */

/**
 * Start/Stop traffic on MC buses.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl02_00Memory_Exec(struct dth_element *elem);

/**
 * Get outputs arguments.
 * @param[in] elem specifies the DTH element.
 * @param[in] value specifies the argument to set.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl02_01Memory_Get(struct dth_element *elem, void *value);

/**
 * Set inputs arguments.
 * @param[in] elem specifies the DTH element.
 * @param[in] value specifies the argument to set.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl02_02Memory_Set(struct dth_element *elem, void *value);

/* ----------------------------------------------------------------------- */
/* Initialization                                                          */
/* ----------------------------------------------------------------------- */

/**
 *  Initialize data structure needed for MC buses.
 *  @param[in] data data needed to manage MC buses.
 *  @param[in] source Path to the special device file that enables access to the memory device.
 *  @param[in] target Directory where the memory device must be mounted.
 *  @param[in] file File name where perform continuous writing accesses.
 *  @param[in] fstype Width of the picture to be displayed.
 */
void tatl02_03Init_Mc_Data(DTH_MEMORY_PROCESS_DATA *data, char *source, char *target, char *file, char *fstype);

/* ----------------------------------------------------------------------- */
/* Module private functions                                                */
/* ----------------------------------------------------------------------- */

/**
 *  Perform continuous writing accesses on memory device.
 *	@param[in] threadData data needed for memory processing.
 */
void tatl02_04Memory_Process(void *threadData);

/**
 *  Free memory device
 *	@param[in] fd file descriptor
 *	@param[in] data needed for the memory processing.
 *	@param[in] error error string
 */
void tatl02_05Free_Memory_Device(int fd, DTH_MEMORY_PROCESS_DATA *threadData, char *error);

#endif /* TATL02MEMORY_H_ */


