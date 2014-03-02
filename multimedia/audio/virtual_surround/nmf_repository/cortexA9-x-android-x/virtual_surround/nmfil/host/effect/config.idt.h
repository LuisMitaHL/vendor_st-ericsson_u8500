/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*****************************************************************************/
/**
 * This is a Nomadik Multiprocessing Framework generated file, please do not modify it.
 */

/* Generated virtual_surround/nmfil/host/effect/config.idt defined type */
#if !defined(_VIRTUAL_SURROUND_NMFIL_HOST_EFFECT_CONFIG_H_)
#define _VIRTUAL_SURROUND_NMFIL_HOST_EFFECT_CONFIG_H_

#include <samplefreq.idt.h>

typedef enum t_xyuv_t_virtual_surround_mode {
  STANDARD_DOWN_MIX=1,
  SURROUND_DOWN_MIX,
  SURROUND_VIRTUALIZATION} t_virtual_surround_mode;

typedef enum t_xyuv_t_room_simulation_mode {
  ROOM_SIMULATION_MIXED=1,
  ROOM_SIMULATION_VIRTUALIZED} t_room_simulation_mode;

typedef enum t_xyuv_t_room_simulation_room_type {
  ROOM_TYPE_DEFAULT=0,
  ROOM_TYPE_ROOM,
  ROOM_TYPE_LIVING_ROOM,
  ROOM_TYPE_AUDITORIUM,
  ROOM_TYPE_CONCERT_HALL,
  ROOM_TYPE_ARENA,
  ROOM_TYPE_SMALL_ROOM,
  ROOM_TYPE_MEDIUM_ROOM,
  ROOM_TYPE_LARGE_ROOM,
  ROOM_TYPE_MEDIUM_HALL,
  ROOM_TYPE_LARGE_HALL} t_room_simulation_room_type;

typedef struct t_xyuv_t_mix_gain {
  t_sint16 left_front_gain;
  t_sint16 right_front_gain;
  t_sint16 center_front_gain;
  t_sint16 left_surround_gain;
  t_sint16 right_surround_gain;
  t_sint16 low_frequency_effects_gain;
  t_sint16 front_room_simulation_gain;
  t_sint16 surround_room_simulation_gain;
} t_mix_gain;

typedef struct t_xyuv_t_virtual_surround_config {
  t_bool enable;
  t_bool room_simulation_front;
  t_bool room_simulation_surround;
  t_virtual_surround_mode virtual_surround_mode;
  t_room_simulation_mode room_simulation_mode;
  t_room_simulation_room_type room_type;
  t_mix_gain gains;
} t_virtual_surround_config;

#endif
