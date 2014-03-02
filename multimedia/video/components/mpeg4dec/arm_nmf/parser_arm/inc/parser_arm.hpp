/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef _PARSER_ARM_HPP_
#define _PARSER_ARM_HPP_

#include "vfm_vdc_mpeg4.idt"
#include "OMX_Video.h"
#include "Component.h"
#include "omxilosalservices.h"
#ifdef SOFT_DECODER 
#define ARM_PARSER_NMF mpeg4dec_arm_nmf_parser_arm
#define MULTIPLIER 1
#define ADD_RESS nLogicalAddress
#define ENDIANESS_CONVERSION(number) number
#else
#define ARM_PARSER_NMF mpeg4dec_arm_nmf_parser
#define MULTIPLIER 2
#define ADD_RESS nMpcAddress
#define ENDIANESS_CONVERSION(number) ((t_uint32)(number)>>16 | ((t_uint32)(number) & 0xffff)<<16)
#endif
typedef enum
{
	VFM_MPEG4_NONE = 0x0,
	VFM_MPEG4_SH = 0x1,
	VFM_MPEG4_SP = 0x2,
	VFM_MPEG4_ASP = 0x4,
	VFM_MPEG4_SORENSON = 0x8,
	VFM_MPEG4_SP_ASP = 0x80000000,
} VFM_MPEG4_CODING_TYPE;

#define VFM_HEADER_FIFO_COUNT 32
#define KVIDEO_OBJECT_LAYER_START_CODE_SMALLEST		0X120
#define KVIDEO_OBJECT_LAYER_START_CODE_GREATEST		0X12F
#define KVISUAL_OBJECT_SEQUENCE_END_CODE			0X1B1
#define KGROUP_OF_VOP_START_CODE					0X1B3
#define KVISUAL_OBJECT_START_CODE					0X1B5
#define KVOP_START_CODE								0X1B6
#define KRESYNC_MARKER								0X01
#define KSP_RESYNC_MARKER							0X01
#define KSH_RESYNC_MARKER							0X02
#define KRESYNC_MARKER_SIZE							16 // + fcode bits default
#define MP4_SC_LENGHT_IN_BITS   32

#define SH_SC_LENGTH_IN_BITS    22
#define VIS_OBJ_SEQ_CODE        0x1B0
#define SH_CODE                 0x20
/* Number of Gobs Per Frame */
#define KNUM_GOB_SQCIF			6
#define KNUM_GOB_QCIF			9
#define KNUM_GOB_CIF			18
#define KNUM_GOB_4CIF		18
#define KNUM_GOB_16CIF		18

/* Aspect ratio constants */
#define KAspect1To1				0x1
#define KSpAspectRatio1to1Num	1
#define KSpAspectRatio1to1Den	1

#define KAspect12To11			0x2
#define KSpAspectRatio12to11Num	12
#define KSpAspectRatio12to11Den	11

#define KAspect10To11			0x3
#define KSpAspectRatio10to11Num	10
#define KSpAspectRatio10to11Den 11

#define KAspect16To11			0x4
#define KSpAspectRatio16to11Num	16
#define KSpAspectRatio16to11Den 11

#define KAspect40To33			0x5
#define KSpAspectRatio40to33Num	40
#define KSpAspectRatio40to33Den	33

#define KExtendedPar			0xF //extended PAR

const OMX_S32 KInvalidGobFrameId = -1;

/*Short Header Source format */
#define SF_SQCIF             1  /* 001 */ /* not supported */
#define SF_QCIF              2  /* 010 */
#define SF_CIF               3  /* 011 */
#define SF_4CIF              4  /* 100 */ /* not supported */
#define SF_16CIF             5  /* 101 */ /* not supported */
#define SF_VGA               6  /* 110 */ /* extension!    */
#define WVGA               	 7  /* 111 */ /* extension!    */
static const OMX_U16 LUT_intra_quant_mat [64]=
{
     8, 17, 17, 20, 18, 18, 19, 19,
    21, 21, 22, 22, 22, 21, 21, 23,
    23, 23, 23, 23, 23, 25, 24, 24,
    24, 24, 25, 25, 27, 27, 26, 26,
    26, 26, 26, 27, 28, 28, 28, 28,
    28, 28, 28, 30, 30, 30, 30, 30,
    30, 32, 32, 32, 32, 32, 35, 35,
    35, 35, 38, 38, 38, 41, 41, 45
};

static const OMX_U16 LUT_nonintra_quant_mat [64]=
{
    16, 17, 17, 18, 18, 18, 19, 19,
    19, 19, 20, 20, 20, 20, 20, 21,
    21, 21, 21, 21, 21, 22, 22, 22,
    22, 22, 22, 22, 23, 23, 23, 23,
    23, 23, 23, 23, 24, 24, 24, 25,
    24, 24, 24, 25, 26, 26, 26, 26,
    25, 27, 27, 27, 27, 27, 28, 28,
    28, 28, 30, 30, 30, 31, 31, 33
};

typedef struct ts_bits_handle {
  OMX_U32 ui32_rdbfr;       /* input read buffer 32-bits */
  OMX_U8 *rdbfr;    /* input read buffer */
  OMX_U8 *rdptr;    /* input read buffer pointer */
  OMX_U8 inbfr[12]; /* 16 */ /* internal buffer */
  OMX_U32 incnt;                       /* internal counter */
  OMX_U32 incnt1;                       /* internal counter */
  /* for backward direction */
  OMX_U8 *rdptr_backward;    /* input read buffer pointer */
  OMX_U8 inbfr_backward[12]; /* 16 */ /* internal buffer */
  OMX_U32 incnt_backward;                       /* internal counter */
} ts_bits_handle;

typedef struct complexity_estimation_asp {
OMX_U32 estimation_method;
   /* shape complexity estimation */
   OMX_U32 opaque;
   OMX_U32 transparent;
   OMX_U32 intra_cae;
   OMX_U32 inter_cae;
   OMX_U32 no_update;
   OMX_U32 upsampling;
   /* texture complexity estimation set 1 */
   OMX_U32 intra_blocks;
   OMX_U32 inter_blocks;
   OMX_U32 inter4v_blocks;
   OMX_U32 not_coded_blocks;
   /* texture complexity estimation set 2 */
   OMX_U32 dct_coefs;
   OMX_U32 dct_lines;
   OMX_U32 vlc_symbols;
   OMX_U32 vcl_bits;
   /* motion compensation complexity */
   OMX_U32 apm;
   OMX_U32 npm;
   OMX_U32 interpolate_mc_q;
   OMX_U32 forw_back_mc_q;
   OMX_U32 halfpel2;
   OMX_U32 halfpel4;
   OMX_U32 sadct;
   OMX_U32 quarterpel;
} ts_complexity_estimation_asp, *tps_complexity_estimation_asp;

typedef struct {
	OMX_U32 codingType;
	OMX_VIDEO_MPEG4PROFILETYPE profile;
	OMX_VIDEO_MPEG4LEVELTYPE level;
	OMX_U16 firstFrame;
    OMX_U16 updateParamsDone;
    OMX_U16 vop_time_increment_resolution;
    OMX_U16 horizontal_size;
    OMX_U16 vertical_size;
	OMX_U32 fixed_vop_rate;
    OMX_U32 fixed_vop_time_increment;
    OMX_U32 video_object_layer_width;
    OMX_U32 video_object_layer_height;
    OMX_U16 reversible_vlc;
    OMX_U16 data_partitioned;
    OMX_U16 resync_marker_disable;
    OMX_U16 complexity_estimation_disable;
	OMX_U16 video_object_layer_verid;
    OMX_U16 lowDelay;
    OMX_U16 isInterlaced;
    OMX_U16 quant_type;
    OMX_U16 intraQuantizationMatrix[64];
    OMX_U16 nonIntraQuantizationMatrix[64];
	OMX_U16 prev_temporal_ref;
	OMX_U16 temporal_ref;
	OMX_U16 source_format;
	OMX_U16 framenum;
    OMX_U16 pict_type;
	OMX_VIDEO_PICTURETYPE pictType;
    OMX_U16 modulo_time_base;
    OMX_U16 vop_time_increment;
    OMX_U16 vop_coded;
	OMX_U16 flv_version;
	OMX_U16 flag_sorenson;
    OMX_U16 vop_rounding_type;
    OMX_U16 intra_dc_vlc_thr;
    OMX_U16 quant;
    OMX_U16 vop_fcode_forward;
	OMX_U16 quarter_sample;
    OMX_U16 vop_fcode_backward;
    OMX_BOOL isVolHeaderFound;//$ Flag for checking if VOL has occurred before GOV
    OMX_BOOL iShortHeader;
	OMX_U8 AspectRatio;
	OMX_U8 ColorPrimary;
	OMX_U16 sprite_enable;               /* whether GMC or not*/
	OMX_U16 no_of_sprite_warping_points;  /* no_of_sprite_warping_points for GMC*/
	OMX_U16 sprite_warping_accuracy;      /*sprite_warping_accuracy for GMC */
	OMX_U16 sprite_brightness_change;     /*sprite_brightness_change for GMC */
	OMX_S16 difftraj_x;                          /*Global motion vectors x */
	OMX_S16 difftraj_y;                  /*Global motion vectors x */
	OMX_U16 h263_annexsupport;
	//+ER353881
	OMX_U16 custom_pcf;
	//-ER353881
	OMX_U16 aic_mode;
	OMX_U16 df_mode;
	OMX_U16 ss_mode;
	OMX_U16 mq_mode;
	ts_complexity_estimation_asp Complexity_estimation_mp4_asp;
    } ts_bit_stream;

typedef struct{
	OMX_U32	iTimeIncementWidth;
	OMX_U8	iQuantPrecision; // defualt is 5
	OMX_U32	iMbNumberFieldWidth; //this is useful when video pictures are used
    }t_vol_header;
//t_vol_header ts_volheader,*iVolHeader;

typedef struct{
	OMX_U32	iResyncMarkerSize; //fixed value
	OMX_U32 iNumGobPerFrame;
}t_sh_header;

typedef struct{
	OMX_U32 iResyncMarkerSize;	//value updated each time a VOP header is found (Depends iVopCodeForward)
}ts_vop_header;

typedef struct {
    OMX_U32 mpcAddress;
	OMX_BOOL isFree;
    OMX_BOOL isRequestedToFree;
    ts_ddep_vdc_mpeg4_header_desc *pHeaderDesc;
} ts_vfm_vdc_mpeg4_header_desc;
#ifndef SOFT_DECODER 
class mpeg4dec_arm_nmf_parser: public Component, public mpeg4dec_arm_nmf_parserTemplate {
#else
class mpeg4dec_arm_nmf_parser_arm: public Component, public mpeg4dec_arm_nmf_parser_armTemplate {
#endif
private:
	ts_vfm_vdc_mpeg4_header_desc headerDesc[VFM_HEADER_FIFO_COUNT]; 
	ts_t1xhv_vdc_mpeg4_param_in *pMpeg4ParamIn;  
	ts_vfm_vdc_mpeg4_header_desc *pDesc;
	OMX_BUFFERHEADERTYPE *pBufferIn;
	ts_ddep_buffer_descriptor mParamBufferDesc;
    ts_ddep_buffer_descriptor mLinkListBufferDesc;
    ts_ddep_buffer_descriptor mDebugBufferDesc;

    Port  mPorts[2];
	OMX_BUFFERHEADERTYPE mBufIn[VFM_HEADER_FIFO_COUNT] ;
	OMX_BUFFERHEADERTYPE mBufOut[VFM_HEADER_FIFO_COUNT];

	t_uint32 frame_info_width;
	t_uint32 frame_info_height;
	t_uint32 config_concealmentflag;
    t_uint32 error_reporting;
	t_uint16 *errormap_ptr;
	static unsigned int msk[33];

	//+ER 354962
	omxilosalservices::OmxILOsalMutex* mMutexPendingCommand;
	OMX_U32 mNbPendingCommands;
	//-ER 354962

	OMX_ERRORTYPE GetVosHeaderDataL();//$
    OMX_ERRORTYPE GetVosEndHeaderDataL();
    OMX_ERRORTYPE GetVOHeaderDataL();//$ visual object
    OMX_ERRORTYPE GetVolHeaderDataL(OMX_U32 temp);//$
    OMX_ERRORTYPE GetShortHeaderDataL();//$
    OMX_ERRORTYPE GetGovHeaderDataL();//$
    OMX_ERRORTYPE GetVopHeaderDataL();//$
    OMX_ERRORTYPE GetVopVpHeaderDataL();//$
    OMX_ERRORTYPE GetShortHeaderGobDataL();//$
	OMX_ERRORTYPE GetSorensonDataL();
	OMX_S32 VlcTrajPoint();
    unsigned int showbits(int n);
    unsigned int getbits(int n);
    int next_start_code(void) ;
    unsigned int nextbits_bytealigned(int n);
    void flushbits(int n);
	void fillbfr();
	unsigned int getbitscount();

    ts_bits_handle base, *bits_handle;
	ts_bit_stream ts_bitstream,*bit_stream;
    t_vol_header ts_volheader,*iVolHeader;
    t_sh_header ts_ishortheader,*iShortHeader;
	ts_vop_header ts_vopheader,*iVopHeader;
public:
#ifdef SOFT_DECODER 
	virtual ~mpeg4dec_arm_nmf_parser_arm(){}
	mpeg4dec_arm_nmf_parser_arm();
#else
	virtual ~mpeg4dec_arm_nmf_parser(){}
	mpeg4dec_arm_nmf_parser();
#endif

    virtual void provideBufferForI(t_uint32 buffer_i_frame[2]);
    virtual void sendImmediateIFlag(t_uint8 immediate_i_flag);
    
//Component virtual functions
	virtual void process() ;
	virtual void reset();
	virtual void disablePortIndication(t_uint32 portIdx);
	virtual void enablePortIndication(t_uint32 portIdx);
	virtual void flushPortIndication(t_uint32 portIdx) ;

	//+ER 354962
	void start();
	void stop();

	virtual void pendingCommandAck();
	void addPendingCommand();
	//-ER 354962

	//Constructor interface from component.type
	virtual void fsmInit(fsmInit_t init);
	virtual void setTunnelStatus(t_sint16 portIdx, t_uint16 isTunneled);
	//virtual void reset(OMX_EVENTTYPE event, t_uint32 data1, t_uint32 data2) ;
	virtual void               destroy(void) ;
	virtual void sendCommand(OMX_COMMANDTYPE cmd, t_uword param);
	virtual void eventProcess();
	virtual void processEvent(void)                               {
		//printf("Fun c processEvent\n\n\n\n\n\n\n\n");
		 Component::processEvent() ; }
	virtual void fromProcComp_emptyThisBuffer(OMX_BUFFERHEADERTYPE_p pBuffer); 
	virtual void fromProcComp_fillThisBuffer(OMX_BUFFERHEADERTYPE_p pBuffer); 

    virtual void fromSharedBufferComp_emptyThisBuffer(OMX_BUFFERHEADERTYPE_p pBuffer);
    virtual void fromSharedBufferComp_fillThisBuffer(OMX_BUFFERHEADERTYPE_p pBuffer);

	virtual void fillThisHeader(t_uint32 buffer);
	virtual void memoryRequirements(t_uint32 deblocking_param_struct[10],
									ts_ddep_buffer_descriptor paramBufferDesc,
									ts_ddep_buffer_descriptor linkListBufferDesc,
									ts_ddep_buffer_descriptor debugBufferDesc,
									t_uint16 error_map,
									t_uint16 *ptr,
									t_uint8 BufCountActual,
                                    t_bool disable,
                                    void *mpc_vfm_mem_ctxt);
	virtual void InternalCopy(t_uint16 InBuffCnt,
							  t_uint32 offset);
    virtual void sendportSettings(t_uint32 width,t_uint32 height,t_uint32 concealment_flag,t_uint32 flag_sorenson,t_uint8 aspect_ratio,t_uint8 color_primary ,t_uint32 error_reporting_enable);
	virtual t_nmf_error construct();
	void processBuffer();
	OMX_BOOL buffer_available_atinput();
	OMX_BOOL buffer_available_atheaderport();
	OMX_BOOL StartCodeFound();
	void initParserBitstream(OMX_BUFFERHEADERTYPE* buf);
	void initParser();
    OMX_ERRORTYPE detectAndParseCodingType(OMX_U32 &aScValue);
  	void updateHeaderInfos(ts_ddep_vdc_mpeg4_header_desc *pDesc,OMX_U32 bitOffset);
    void updateMpeg4Params();
	//+ER401513
	void fillParamsSinceCorruptionIsMinor();
	//-ER401513
	void clearAllPendingRequestsToFree();
	ts_vfm_vdc_mpeg4_header_desc* getFreeHeaderDesc();
	void freeHeaderDesc(t_uint32* pBuffer);
    void detectPortSettingsAndNotify(ts_ddep_vdc_mpeg4_header_desc *pDesc);
	t_uint8 AspectRatio;
	t_uint8 nHorizontal;
	t_uint8 nVertical;
	t_uint8 ColorPrimary;
	t_uint8 StartCodeCount;
    OMX_BUFFERHEADERTYPE *pBufferInArray[VFM_HEADER_FIFO_COUNT];
	t_uint32 QIndex;
	t_uint32 DQIndex;
	t_uint32 InternalBufferAddress;
	t_uint8 bImmediateIFlag;
	t_uint32 pBufferForI;
	void *save_memory_context;
	//+ER329550
	OMX_BOOL do_not_parse;
	//-ER329550
	//+ER426245
	OMX_U8 port_output_was_disabled;
	//-ER426245
};

#endif /* PARSER_ARM_HPP_ */
