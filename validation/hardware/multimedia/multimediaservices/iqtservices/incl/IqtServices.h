/*****************************************************************************/
/**
*  Copyright (C) ST-Ericsson 2009
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Propose IQT function for tests 
* \author  ST-Ericsson
*/
/*****************************************************************************/



#ifndef _IQTSERV_H
#define _IQTSERV_H


#if 0
#define pe_tunning_ELEMENT_COUNT 1


int32_t PE_param_array [pe_tunning_ELEMENT_COUNT]

#if defined (TATL01IQTFUNC_C)
={0}
#endif
;


int8_t* PE_string_array [pe_tunning_ELEMENT_COUNT]

#if defined (TATL01IQTFUNC_C)
={NULL}
#endif
;
#endif

/*------------------------------------------------------*/
/*! 	\ingroup Display_services
	\def IQTSERV_NO_ERROR
	\brief  code error returned meaning that the service has been executed without error
*/

#define  IQTSERV_NO_ERROR  		((unsigned int) 0)

/*------------------------------------------------------*/
/*! 	\ingroup Display_services
	\def IQTSERV_FAILED
	\brief  code error returned meaning that an error occurs during service execution
*/
#define  IQTSERV_FAILED    		((unsigned int) 1)

/*------------------------------------------------------*/
/*! 	\ingroup Display_services
	\def VIDSERV_DEVICE_ALREADY_USED
	\brief  code error returned meaning that the device is already in use
*/
#define  IQTSERV_DEVICE_ALREADY_USED ((unsigned int) 2)

/*------------------------------------------------------*/
/*! 	\ingroup Display_services
	\def IQTSERV_INVALID_PARAMETER
	\brief  code error returned meaning that a parameter is not valid
*/
#define  IQTSERV_INVALID_PARAMETER   ((unsigned int) 3)

/*------------------------------------------------------*/
/*! 	\ingroup Display_services
	\def IQTSERV_NOT_AVAILABLE
	\brief  code error returned meaning that service is not available/not yet implemented
*/
#define  IQTSERV_NOT_AVAILABLE   ((unsigned int) 4)

int IqtServ_register_pe_dth_struct(char *mapfile);

uint32_t IqtServ_InitPipe(char *input_iqt_pipe,char *output_iqt_pipe,char *iqt_output_MMTE_task_pipe,int *fd_ip,int *fd_op,int *fd_op_mmte_task,int pts);
void IqtServ_WaitAcknowledge(int fd,char *trigger,int timeout);
uint32_t IqtServ_SetMmteAlias(int fd_in,int fd_out,char *mmte_command,int timeout);
uint32_t IqtServ_InitMmte(int fd_in,int fd_out,char *trigger,int timeout);
uint32_t IqtServ_StartPreview(int fd_in,int fd_out,char *trigger,int timeout);
uint32_t IqtServ_InitToIdle(int fd_in,int fd_out,char *trigger,int timeout);
uint32_t IqtServ_ExecutingToIdle(int fd_in,int fd_out,char *trigger,int timeout);
uint32_t IqtServ_IdleToInit(int fd_in,int fd_out,char *trigger,int timeout);
uint32_t IqtServ_StartPause(int fd_in,int fd_out,char *trigger,int timeout);
uint32_t IqtServ_StartResume(int fd_in,int fd_out,char *trigger,int timeout);
uint32_t IqtServ_StartStillPictureCapture(int fd_in,int fd_out,char *trigger,int timeout);
uint32_t IqtServ_StartRecord(int fd_in,int fd_out,char *trigger,int timeout);
uint32_t IqtServ_StopRecord(int fd_in,int fd_out,char *trigger,int timeout);
uint32_t IqtServ_StartStillBurstPictureCapture(int fd_in,int fd_out,char *trigger,int timeout);
uint32_t IqtServ_StopStillBurstPictureCapture(int fd_in,int fd_out,char *trigger,int timeout);
uint32_t IqtServ_StopPreview(int fd_in,int fd_out,char *trigger,int timeout);
uint32_t IqtServ_QuitMmte(int fd_in,int fd_out,char *trigger,int timeout);
uint32_t IqtServ_StartMmte(char *pipe_input,char *pipe_output,int fd_out,char *trigger,int timeout);
uint32_t IqtServ_CloseFileDescriptors(void);
//int IqtServ_IQTGetPipe(FILE *fd);

uint32_t IqtServ_ReadPageElement(int fd,int fd_output,int index,int* value,int timeout);
void IqtServ_AckReadPageElement(int fd,int timeout,int index,int* value);

uint32_t IqtServ_WritePageElement(int fd_in,int fd_out,int index,int value,char *trigger,int timeout);
uint32_t IqtServ_ReadCurrentState(int fd_in,int fd_out,int timeout,int* State);
void IqtServ_AckReadCurrentState(int fd,int timeout,int* State);
#endif /* _IQTSERV_H */
