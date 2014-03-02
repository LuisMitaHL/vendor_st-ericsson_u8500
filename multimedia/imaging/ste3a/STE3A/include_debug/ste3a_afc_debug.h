/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * \file    ste3a_afc_debug.h
 * \brief   STE3A AFC debug types
 * \author  ST-Ericsson Image Quality Team, Agrate (Italy)
 */

#ifndef STE3A_AFC_DEBUG_H_
#define STE3A_AFC_DEBUG_H_

#include "ste3a_types.h"
#include "ste3a_stats_types.h"


struct ste3a_afc_intermediates											// afc intermediate values
{
	ste3a_count					locked_zones_count;										//	number of locked zones
	ste3a_index					locked_zones_array[STE3A_AFC_STATS_ZONES_MAX_COUNT];	//	array of locked zones indexes
	ste3a_float					restart_eval_avg_sr;									//	sharpness averages min/max ratio
	ste3a_float					restart_eval_avg_br;									//	brightness averages min/max ratio
	ste3a_float					restart_eval_erg_sr;									//	sharpness energies min/max ratio
	ste3a_float					restart_eval_erg_br;									//	brightness energies min/max ratio
	ste3a_float					restart_eval_nxc_sf;									//	sharpness normalized cross-correlation factor
	ste3a_float					restart_eval_nxc_bf;									//	brightness normalized cross-correlation factor
	ste3a_bool					restarted_avg_sr;										//	sharpness averages min/max ratio caused a restart
	ste3a_bool					restarted_avg_br;										//	brightness averages min/max ratio caused a restart
	ste3a_bool					restarted_erg_sr;										//	sharpness energies min/max ratio caused a restart
	ste3a_bool					restarted_erg_br;										//	brightness energies min/max ratio caused a restart
	ste3a_bool					restarted_nxc_sf;										//	sharpness normalized cross-correlation factor caused a restart
	ste3a_bool					restarted_nxc_bf;										//	brightness normalized cross-correlation factor caused a restart
	ste3a_bool					restarted;												//	one of the criteria caused a restart
	ste3a_float					continue_eval_avg_sr;									//	sharpness averages min/max ratio
	ste3a_float					continue_eval_avg_br;									//	brightness averages min/max ratio
	ste3a_float					continue_eval_erg_sr;									//	sharpness energies min/max ratio
	ste3a_float					continue_eval_erg_br;									//	brightness energies min/max ratio
	ste3a_float					continue_eval_nxc_sf;									//	sharpness normalized cross-correlation factor
	ste3a_float					continue_eval_nxc_bf;									//	brightness normalized cross-correlation factor
	ste3a_bool					continued_avg_sr;										//	sharpness averages min/max ratio caused a restart
	ste3a_bool					continued_avg_br;										//	brightness averages min/max ratio caused a restart
	ste3a_bool					continued_erg_sr;										//	sharpness energies min/max ratio caused a restart
	ste3a_bool					continued_erg_br;										//	brightness energies min/max ratio caused a restart
	ste3a_bool					continued_nxc_sf;										//	sharpness normalized cross-correlation factor caused a restart
	ste3a_bool					continued_nxc_bf;										//	brightness normalized cross-correlation factor caused a restart
	ste3a_bool					continued;												//	one of the criteria caused a restart
	ste3a_normalized_sharpness	best_pos_sharpness;
	ste3a_normalized_sharpness	worst_pos_sharpness;
	ste3a_normalized_sharpness	best_pos_noise;
	ste3a_normalized_sharpness	worst_pos_noise;
	ste3a_bool					focus_good;
	ste3a_bool					focus_bad;
};

#endif /* STE3A_AFC_DEBUG_H_ */
