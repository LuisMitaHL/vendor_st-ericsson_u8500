/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * \file    stesstatistics internal functions declarations
 * \author  ST-Ericsson Image Quality Team, Agrate (Italy)
 */

#ifndef STESTATSI_H_
#define STESTATSI_H_

#include "ste3a_stats_types.h"


void stestatsi_compute_stats_buffer(const	ste3a_histograms_stats_descriptor	& hist_stats_descriptor,
									const	ste3a_grid_stats_descriptor			& grid_stats_descriptor,
									const	ste3a_histstats_val *				  hist_frst,
									const	ste3a_gridstats_val *				  grid_frst,
											ste3a_histstats_val *				& hist_base,
											ste3a_count							& hist_offs,
											ste3a_count							& hist_size,
											ste3a_gridstats_val *				& grid_base,
											ste3a_count							& grid_offs,
											ste3a_count							& grid_size);


#endif /* STESTATSI_H_ */
