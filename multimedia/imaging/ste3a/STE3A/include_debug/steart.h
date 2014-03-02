/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * \file    steart.h
 * \brief   STE Auto Rendition Tuning interface
 * \author  ST-Ericsson Image Quality Team, Agrate (Italy)
 */

#ifndef STEART_H_
#define STEART_H_

#include "ste3a_art_types.h"
#include "ste3a_art_debug.h"
#include "ste3a_ppal_types.h"
#include "ste3a_stats_types.h"
#include "stearti.h"


#define STEART_STATS_HIST_BINS_MAX_COUNT		256
#define STEART_STATS_HIST_CHANNELS_MAX_COUNT	3
#define STEART_STATS_GRID_SIZEX_MAX_COUNT		80
#define STEART_STATS_GRID_SIZEY_MAX_COUNT		60
#define STEART_STATS_GRID_CHANNELS_MAX_COUNT	4


struct steart_stats_status
{
	ste3a_histograms_stats_status	hist;
	ste3a_grid_stats_status			grid;
};

struct steart_stats_descriptor
{
	ste3a_histograms_stats_descriptor	hist;
	ste3a_grid_stats_descriptor			grid;
};

struct steart_obj
{
	ste3a_common_callbacks	com_cbs;
	ste3a_art_tuning		tuning;
	ste3a_art_intermediates intermediates;
	steart_internal			internal;			// internal data
};

steart_obj * steart_open(const ste3a_common_callbacks & com_cbs, const steart_stats_descriptor & stats_descriptor);

void steart_close(steart_obj * aec);

void steart_get_static_tone_mapping(			steart_obj						* art,
												ste3a_isp_tm_config				& isp_tm_config);

void steart_get_gamma_coding(					steart_obj						* art,
												ste3a_isp_gc_config				& isp_gc_config);

void steart_evaluate_static_tonemapping(		steart_obj						* art,
										const	ste3a_damper_bases				& damper_bases,
												ste3a_isp_cg_config				& isp_cg_config,
												ste3a_isp_dg_config				& isp_dg_config,
												ste3a_isp_cm_config				& isp_cm_config,
												ste3a_isp_er_config				& isp_er_config,
												ste3a_isp_cs_config				& isp_cs_config);

void steart_evaluate_dynamic_tonemapping(		steart_obj						* art,
										const	steart_stats_status				& stats_status,
										const	ste3a_damper_bases				& damper_bases,
												ste3a_isp_cg_config				& isp_cg_config,
												ste3a_isp_dg_config				& isp_dg_config,
												ste3a_isp_cm_config				& isp_cm_config,
												ste3a_isp_er_config				& isp_er_config,
												ste3a_isp_cs_config				& isp_cs_config,
												ste3a_isp_tm_config				& isp_tm_config,
												ste3a_isp_gc_config				& isp_gc_config);

void steart_set_tuning(							steart_obj						* art,
										const	ste3a_art_tuning				& tuning);

void steart_get_tuning(							steart_obj						* art,
												ste3a_art_tuning				& tuning);

void steart_get_intermediates(					steart_obj						* art,
												ste3a_art_intermediates			& intermediates);

ste3a_bool steart_playback(						steart_obj						* art,
										const	void							* dump,
												void *							& next,
												ste3a_bool						& match);

#endif /* STEART_H_ */
