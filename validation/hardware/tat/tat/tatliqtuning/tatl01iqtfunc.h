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

#ifndef TATL01IQTFUNC_H_
#define TATL01IQTFUNC_H_

#define MMTE_TIMEOUT 10000
#define MMTE_TIMEOUT_FOR_STILL 4200

#define DEFAULT_SDCARD_DIRECTORY_PATH   "/mnt/UserFS2/"
//#define DEFAULT_SDCARD_DIRECTORY_PATH   "/mnt/SDCard//"

#define RAM_DISK_PATH_VIEWFINDER_LR 	"/iqt_rd/viewfinder/LR/"
#define RAM_DISK_PATH_STILL_LR 		"/iqt_rd/still/LR/"
#define RAM_DISK_PATH_STILL_HR 		"/iqt_rd/still/HR/"
#define RAM_DISK_PATH_STILL_RAW 	"/iqt_rd/still/RAW/"
#define RAM_DISK_PATH_VIDEO_LR 		"/iqt_rd/video/LR/"
#define RAM_DISK_PATH_VIDEO_HR 		"/iqt_rd/video/HR/"

#define MAP_FILE			"/system/usr/share/mm-valid/imaging/isp_firmware/isp8500_firmware_IMX072/device_params/V2WReg.map"
#define IQT_PE_DTH_PATHNAME		"/DTH/iq_tunning/page_elements/"

#define FW_VERSION_DTH_PATHNAME		IQT_PE_DTH_PATHNAME"FW_Version"
#define INIT_PE_DTH_PATHNAME		IQT_PE_DTH_PATHNAME"Open"
#define DEINIT_PE_DTH_PATHNAME		IQT_PE_DTH_PATHNAME"Close"

#define INIT_PE 0
#define FW_VERSION 1
#define STEP1 0
#define STEP2 1

#define GROUP_ITEM_INDEX		8
#define ITEM_INDEX			8
#define FIX_ENUM_INDEX			43

#define REGISTER_INDEX			7
#define TYPE_INDEX			23
#define ENDIAN_INDEX			25

#define ACTION_ENABLED 1
#define ACTION_DISABLED 0

typedef struct t_ValEnumElt {
        char* EnumAttr; 
        int   EnumVal;
	struct t_ValEnumElt* p_Next;
}t_ValEnumElt;

typedef struct t_EnumTypeElt {
        char* EnumType; 
        int   EnumEntries;
	t_ValEnumElt* ValEnumElt;
	struct t_EnumTypeElt* p_Next;
}t_EnumTypeElt;


struct Pe_UserData
{
	int32_t Index;
	int32_t NumBytes;
	int32_t ToggleEndian;
	int32_t DataIndex;
};



int32_t DisplayFrameNumber
#if defined (TATL01IQTFUNC_C)
=0
#endif
;

int32_t v_backup_IqtState
#if defined (TATL01IQTFUNC_C)
=0
#endif
;

int32_t CurrentStillMode
#if defined (TATL01IQTFUNC_C)
=0
#endif
;

int32_t IQT_param_array [iq_tunning_ELEMENT_COUNT]

#if defined (TATL01IQTFUNC_C)
={0}
#endif
;


int8_t* IQT_string_array [iq_tunning_ELEMENT_COUNT]

#if defined (TATL01IQTFUNC_C)
={NULL}
#endif
;

int32_t ViewFinderDisplayActivationON
#if defined (TATL01IQTFUNC_C)
=0
#endif
;
int32_t StillDisplayActivationON
#if defined (TATL01IQTFUNC_C)
=0
#endif
;

int32_t RawStillDisplayActivationON
#if defined (TATL01IQTFUNC_C)
=0
#endif
;

int32_t HrStillDisplayActivationON
#if defined (TATL01IQTFUNC_C)
=0
#endif
;

int32_t StillActionForbidden
#if defined (TATL01IQTFUNC_C)
=0
#endif
;


int32_t VideoDisplayActivationON
#if defined (TATL01IQTFUNC_C)
=0
#endif
;

int32_t* PE_param_array
#if defined (TATL01IQTFUNC_C)
={0}
#endif
;

int8_t** PE_string_array
#if defined (TATL01IQTFUNC_C)
={NULL}
#endif
;

#if defined (TATL01IQTFUNC_C)
static char *input_iqt_pipe = "/tmp/input_iqt_pipe";
static char *output_iqt_pipe = "/tmp/output_iqt_pipe";
static char *iqt_output_MMTE_task_pipe = "/tmp/iqt_output_MMTE_task_pipe";
#endif

int32_t tatln1_00ActExec(struct dth_element *elem);
int32_t tatln1_02ActSet (struct dth_element *elem, void *Value);
int32_t tatln1_03ActGet (struct dth_element *elem, void *Value);
int32_t tatln1_04ActSetDynamic (struct dth_element *elem, void *Value);
int32_t tatlm1_05InternalIqtState(char *IqtState);
int32_t tatlm1_06MkdirManagement(char *pathname);
int32_t tatlm1_07RamDiskManagement(void);
int32_t tatlm1_08SavePrefixManagement(void);
int32_t tatlm1_09SaveToSDcardManagement(int32_t UseCase);
int32_t tatlm1_10MkdirPrefixManagement(int32_t UseCase);
int32_t tatlm1_11SDcardDirectoryManagement(char *pathname,int32_t UseCase);
int32_t tatlm1_12ResolutionPrefixManagement(char *pathname,char *Resolution);
int32_t tatlm1_13RamDiskTreeManagement(char *Pathname);
int32_t tatlm1_14MmteStringManagement(char *alias,char *status);
int32_t tatlm1_15MmteValueManagement(char *alias,int value);
int32_t tatlm1_16PreTreatmentActionVF(void *Value);
int32_t tatlm1_17List(int32_t UseCase);
int32_t tatlm1_18Remove(int32_t UseCase,int32_t Resolution);
int32_t tatlm1_19PreTreatmentActionStill(void *Value);
int32_t tatlm1_20PreTreatmentActionVideo(void *Value);


int32_t tatlm1_108register_pe_dth_struct(char *mapfile);
int32_t tatlm1_101EnumManagementDebug(void);
int32_t tatlm1_102DeviceVersionManagement(char* StringOfTheMapFile);
int32_t tatlm1_103GroupItemManagement(char* StringOfTheMapFile,char* GroupItem,char* GroupItemAccessRights);
int32_t tatlm1_104PrefixAndItemPreManagement(char* StringOfTheMapFile,char* Item,char* PeItem,char* PrefixItem);
int32_t tatlm1_105EnumItemManagement(char* StringOfTheMapFile,struct dth_element* pp_DthElt,int vp_NumBytes,int vp_index);
int32_t tatlm1_106TypeItemManagement(struct dth_element* pp_DthElt,int vp_NumBytes,char* PrefixItem);
int32_t tatlm1_107ItemManagement(char* StringOfTheMapFile,char* GroupItem,char* GroupItemAccessRights);
uint32_t  tatlm1_109InitFileDescriptors(int camera_fd_input,int camera_fd_popen);
int32_t tatlm1_110FrameSkipping(char *Path);
int32_t tatlm1_111BracketManagement(char *Item);
int32_t tatlm1_112DthAclManagement(char *pp_ActionName,int vp_enable);
int32_t tatlm1_113AclManagementForTheState(int State);
int32_t tatlm1_114DthPeInitAclManagement(int vp_enable,char* pp_DthActionPathName);
int32_t tatlm1_115PeInitAction(void);
void tatlm1_116StartInitToIdle(void);
void tatlm1_117WorkAround(void);
int32_t tatlm1_118GetPeDthElementCounter (void);
int32_t tatlm1_116PreTreatmentActionRawStill(struct dth_element *elem, void *Value);
int32_t tatlm1_117PreTreatmentActionHrStill(struct dth_element *elem, void *Value);
void tatlm1_118StopStillPicture(void);
int32_t tatlm1_119ActSetResolutionVF(int Value); 
int32_t tatlm1_120ActSetResolutionStill(int Value);
int32_t tatlm1_121ActSetResolutionVideo(int Value);

#endif /* TATL01IQTFUNC_H_ */
