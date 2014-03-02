/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef INCLUSION_GUARD_STE_ADM_CSALL_OMX_H
#define INCLUSION_GUARD_STE_ADM_CSALL_OMX_H

#include "ste_adm_client.h"
#include "ste_adm_omx_tool.h"

typedef struct {
  void(*codec_enabled_fp)();
  void(*codec_disabled_fp)();
  int cscall_codec_active;
  void(*cscall_samplerate_fp)();
  unsigned int cscall_samplerate;
  uint voip_ul_samplerate;
  uint voip_ul_channels;
  uint voip_dl_samplerate;
  uint voip_dl_channels;
  int voip_enabled;
  int dict_mode;
  int dict_mute;

  il_comp_t voice_srcsnk_h;
  il_comp_t speechproc_h;
  il_comp_t dict_rec_mix_h;
  il_comp_t dict_split_dl_h;
  il_comp_t dict_split_ul_h;
  il_comp_t dict_mix_ul_h;
  il_comp_t dict_play_split_h;
  il_comp_t voip_mix_dl_h;
  il_comp_t modem_src_h;
  il_comp_t modem_sink_h;

  il_comp_t dict_app_comp_h;
  OMX_U32 dict_app_comp_port;

  OMX_U32 capturer_port, reference_port, renderer_port;
  il_comp_t renderer_h, capturer_h, reference_h;
  int loopback_enabled;
  int codec_type;
} cscall_omx_state_t;

extern cscall_omx_state_t g_cscall_omx_state;

ste_adm_res_t adm_cscall_omx_init_cscall( void(*codec_enabled_fp)(),
                                          void(*codec_disabled_fp)(),
                                          void(*cscall_samplerate_fp)());
ste_adm_res_t adm_cscall_omx_init_cscall_slim( void(*codec_enabled_fp)(),
                                          void(*codec_disabled_fp)(),
                                          void(*cscall_samplerate_fp)());
ste_adm_res_t adm_cscall_omx_init_cscall_fat(void(*codec_enabled_fp)(),
                                          void(*codec_disabled_fp)(),
                                          void(*cscall_samplerate_fp)());
ste_adm_res_t adm_cscall_omx_destroy_cscall();

ste_adm_res_t adm_cscall_omx_activate_cscall(const char* in_dev, const char* out_dev);
ste_adm_res_t adm_cscall_omx_deactivate_cscall();
ste_adm_res_t adm_cscall_omx_connect_voicecall_input(il_comp_t comp, OMX_U32 port);
ste_adm_res_t adm_cscall_omx_connect_voicecall_output(il_comp_t comp, OMX_U32 port, il_comp_t echo_comp, OMX_U32 echo_port);
ste_adm_res_t adm_cscall_omx_disconnect_voicecall_input();
ste_adm_res_t adm_cscall_omx_disconnect_voicecall_output();

il_comp_t adm_cscall_omx_get_speechproc_handle();
int             adm_cscall_omx_get_samplerate();

ste_adm_res_t adm_cscall_omx_enable_dict_rec(int num_bufs, int bufsz, int samplerate, int channels);
ste_adm_res_t adm_cscall_omx_disable_dict_rec();
ste_adm_res_t adm_cscall_omx_set_dict_mode(int mode);
ste_adm_res_t adm_cscall_omx_set_dict_mute(int mute);
ste_adm_res_t adm_cscall_omx_get_dict_mute(int *mute);
int adm_cscall_get_dict_rec_comp(il_comp_t* comp_p, OMX_U32* port_p);

ste_adm_res_t adm_cscall_omx_enable_dict_play(int num_bufs, int bufsz, int samplerate, int channels);
ste_adm_res_t adm_cscall_omx_disable_dict_play();
ste_adm_res_t adm_cscall_omx_enable_dict_play_app(il_comp_t new_dict_app_comp_h, OMX_U32 new_dict_app_comp_port);
ste_adm_res_t adm_cscall_omx_disable_dict_play_app();
int adm_cscall_get_dict_play_comp(il_comp_t* comp_p, OMX_U32* port_p);

ste_adm_res_t adm_cscall_omx_enable_voip(int in_samplerate, int in_channels,
    int out_samplerate, int out_channels);
ste_adm_res_t adm_cscall_omx_disable_voip();
int adm_cscall_get_voip_in_comp(il_comp_t* comp_p, OMX_U32* port_p);
int adm_cscall_get_voip_out_comp(il_comp_t* comp_p, OMX_U32* port_p);

ste_adm_res_t adm_cscall_omx_enable_voip_output(int num_bufs, int bufsz);
ste_adm_res_t adm_cscall_omx_enable_voip_input(int num_bufs, int bufsz);
ste_adm_res_t adm_cscall_omx_disable_voip_output(void);
ste_adm_res_t adm_cscall_omx_disable_voip_input(void);

ste_adm_res_t adm_cscall_omx_rescan_speech_proc(const char* input_name, const char* output_name);

ste_adm_res_t cscall_omx_set_loopback_mode(int loopback_enabled, int codec_type);
ste_adm_res_t adm_cscall_create_cscall_cmp(cscall_omx_state_t* state_p);


// Services dedicated to Slim modem
ste_adm_res_t adm_cscall_create_cscall_cmp_slim(cscall_omx_state_t* state_p);
ste_adm_res_t adm_cscall_omx_activate_cscall_slim(const char* in_dev, const char* out_dev);
ste_adm_res_t adm_cscall_omx_connect_voicecall_input_slim(il_comp_t comp, OMX_U32 port);
ste_adm_res_t adm_cscall_omx_connect_voicecall_output_slim(il_comp_t comp, OMX_U32 port, il_comp_t echo_comp, OMX_U32 echo_port);
ste_adm_res_t adm_cscall_omx_disconnect_voicecall_input_slim();
ste_adm_res_t adm_cscall_omx_disconnect_voicecall_output_slim();
ste_adm_res_t adm_cscall_omx_deactivate_cscall_slim();
ste_adm_res_t cscall_omx_set_loopback_mode_slim(int loopback_enabled, int codec_type);
ste_adm_res_t adm_cscall_omx_enable_dict_rec_slim(int num_bufs, int bufsz, int samplerate, int channels);
ste_adm_res_t adm_cscall_omx_disable_dict_rec_slim();
int adm_cscall_get_dict_rec_comp_slim(il_comp_t* comp_p, OMX_U32* port_p);
ste_adm_res_t adm_cscall_omx_enable_dict_play_slim(int num_bufs, int bufsz, int samplerate, int channels);
ste_adm_res_t adm_cscall_omx_disable_dict_play_slim();
int adm_cscall_get_dict_play_comp_slim(il_comp_t* comp_p, OMX_U32* port_p);

// Services dedicated to Fat modem
ste_adm_res_t adm_cscall_create_cscall_cmp_fat(cscall_omx_state_t* state_p);
ste_adm_res_t adm_cscall_omx_activate_cscall_fat(const char* in_dev, const char* out_dev);
ste_adm_res_t adm_cscall_omx_connect_voicecall_input_fat(il_comp_t comp, OMX_U32 port);
ste_adm_res_t adm_cscall_omx_connect_voicecall_output_fat(il_comp_t comp, OMX_U32 port, il_comp_t echo_comp, OMX_U32 echo_port);
ste_adm_res_t adm_cscall_omx_disconnect_voicecall_input_fat();
ste_adm_res_t adm_cscall_omx_disconnect_voicecall_output_fat();
ste_adm_res_t adm_cscall_omx_deactivate_cscall_fat();
ste_adm_res_t cscall_omx_set_loopback_mode_fat(int loopback_enabled, int codec_type);
ste_adm_res_t adm_cscall_omx_enable_dict_rec_fat(int num_bufs, int bufsz, int samplerate, int channels);
ste_adm_res_t adm_cscall_omx_disable_dict_rec_fat();
int adm_cscall_get_dict_rec_comp_fat(il_comp_t* comp_p, OMX_U32* port_p);
ste_adm_res_t adm_cscall_omx_enable_dict_play_fat(int num_bufs, int bufsz, int samplerate, int channels);
ste_adm_res_t adm_cscall_omx_disable_dict_play_fat();
int adm_cscall_get_dict_play_comp_fat(il_comp_t* comp_p, OMX_U32* port_p);

ste_adm_res_t start_async_configure_speechproc_comp(const char* input_name, const char* output_name, int *start_async_done );
ste_adm_res_t wait_end_of_async_configure_speechproc_comp(void);



#endif // INCLUSION_GUARD_STE_ADM_CSALL_OMX_H
