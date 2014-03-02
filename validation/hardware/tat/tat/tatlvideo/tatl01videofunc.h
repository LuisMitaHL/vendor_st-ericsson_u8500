/*
* =====================================================================================
* 
*        Filename:  tatl01videofunc.h
* 
*     Description:  
* 
*         Version:  1.0
*         Created:  27/08/2009 
*        Revision:  none
*        Compiler:  
* 
*          Author: GVO,  
*         � Copyright ST-Ericsson, 2009. All Rights Reserved
* 
* =====================================================================================
*/

#ifndef TATL01VIDEOFUNC_H_
#define TATL01VIDEOFUNC_H_

FILE *fd_popen2;
typedef struct {
  uint8_t v_type;
  uint16_t v_Lines;
  uint16_t v_Columns;	
  uint32_t v_ColorDef1;
  uint32_t v_ColorDef2;
  uint32_t v_BufferSize;
  uint32_t v_DisplayParam;
  uint32_t *p_RGB32BitBuffer;
  uint8_t *p_RGB8BitBuffer;
}t_tatvideo_DisplayPattern;


typedef struct {
  uint8_t v_DisplayID;
  uint8_t v_Option;
  uint16_t v_Height;
  uint16_t v_Width;	
}t_tatvideo_DisplayManagement;


#define DEFAULT_DIRECTORY_PATH   "/mnt/UserFS2/"
//#define DEFAULT_DIRECTORY_PATH   "/mnt/SDCard//"

int32_t VIDEO_param_array [VIDEO_ELEMENT_COUNT]
#if defined (TATL01VIDEOFUNC_C)
={0}
#endif
;

int8_t* VIDEO_string_array [VIDEO_ELEMENT_COUNT]
#if defined (TATL01VIDEOFUNC_C)
={NULL}
#endif
;

int32_t v_backup_CameraState
#if defined (TATL01VIDEOFUNC_C)
=0
#endif
;

int32_t v_MMTE_started
#if defined (TATL01VIDEOFUNC_C)
=0
#endif
;
int32_t v_PE_Stub
#if defined (TATL01VIDEOFUNC_C)
=0
#endif
;

int32_t InitSecondaryDisplayDone
#if defined (TATL01VIDEOFUNC_C)
=0
#endif
;

int32_t InitTvoCvbsAB8500DisplayDone
#if defined (TATL01VIDEOFUNC_C)
=0
#endif
;

int32_t InitTvoCvbsAV8100DisplayDone
#if defined (TATL01VIDEOFUNC_C)
=0
#endif
;

int32_t InitTvoHdmiDisplayDone
#if defined (TATL01VIDEOFUNC_C)
=0
#endif
;
#define ENABLED 1
#define DISABLED 0

int32_t tatlm1_00ActExec(struct dth_element *elem);
int32_t tatlm1_02ActSet (struct dth_element *elem, void *Value);
int32_t tatlm1_03ActGet (struct dth_element *elem, void *Value);
int32_t tatlm1_04ActSetDynamic (struct dth_element *elem, void *Value);
int32_t tatlm1_05RawSavingManagement(void);
int32_t tatlm1_06MmteStringManagement(char *alias,char *status);
int32_t tatlm1_07InternalIqtState(char *IqtState);
int32_t tatlm1_08DthAclManagement(char *pp_ActionName,int vp_enable);
int32_t tatlm1_09AclManagementForTheState(int State);
int32_t tatlm1_10ActSetResolutionVF(int Value);
int32_t tatlm1_11ActSetResolutionStill(int Value);
int32_t tatlm1_12ActSetResolutionVideo(int Value);

#endif /* TATL01VIDEOFUNC_H_ */
