/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * \file    ppal.h
 * \brief   STE 3A PPAL (pixel pipe abstraction layer) 8500V2 library interface
 * \author  ST-Ericsson Image Quality Team, Agrate (Italy)
 */

#ifndef PPAL_H_
#define PPAL_H_


#include "ste3a.h"


#define STE3A_PPAL_8500V2_GAMMACODING_LUT_SIZE		(128 + 1)

struct ste3a_ppal_8500V2_cg_config
{
	float mul_rr;
	float mul_gr;
	float mul_gb;
	float mul_bb;
};

struct ste3a_ppal_8500V2_cm_config
{
	float pre_mul_r_in_r;
	float pre_mul_g_in_r;
	float pre_mul_b_in_r;
	float pre_mul_r_in_g;
	float pre_mul_g_in_g;
	float pre_mul_b_in_g;
	float pre_mul_r_in_b;
	float pre_mul_g_in_b;
	float pre_mul_b_in_b;

	int post_add_r;
	int post_add_g;
	int post_add_b;
};

struct ste3a_ppal_8500V2_gc_config
{
	int lut_r[STE3A_PPAL_8500V2_GAMMACODING_LUT_SIZE];
	int lut_g[STE3A_PPAL_8500V2_GAMMACODING_LUT_SIZE];
	int lut_b[STE3A_PPAL_8500V2_GAMMACODING_LUT_SIZE];
};

void ste3a_ppal_8500V2_compute_cg_cm(		const	ste3a_isp_cg_config			& isp_cg_config,
											const	ste3a_isp_dg_config			& isp_dg_config,
											const	ste3a_isp_cm_config			& isp_cm_config,
											const	ste3a_isp_er_config			& isp_er_config,
											const	ste3a_isp_cs_config			& isp_cs_config,
													ste3a_ppal_8500V2_cg_config	& x8500V2_cg_config,
													ste3a_ppal_8500V2_cm_config	& x8500V2_cm_config);

void ste3a_ppal_8500V2_compute_gc(			const	ste3a_isp_tm_config			& isp_tm_config,
											const	ste3a_isp_gc_config			& isp_gc_config,
													ste3a_ppal_8500V2_gc_config	& x8500V2_gc_config);

#endif /* PPAL_H_ */
