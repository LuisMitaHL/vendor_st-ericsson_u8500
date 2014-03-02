/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * \file    ste3a_stats_types.h
 * \brief   STE3A statistics types
 * \author  ST-Ericsson Image Quality Team, Agrate (Italy)
 */

#ifndef STE3A_STATS_TYPES_H_
#define STE3A_STATS_TYPES_H_

#include "ste3a_types.h"


typedef uint32_t	ste3a_histstats_val;					// histogram value						[pure number ]
typedef uint8_t		ste3a_gridstats_val;					// grid stats value						[pure number ]
typedef uint32_t	ste3a_focusstats_high_f_val;			// high frequency accumulator			[pure number ]
typedef uint32_t	ste3a_focusstats_zero_f_val;			// zero frequency accumulator			[gray level  ]

struct ste3a_histograms_stats_descriptor					// histograms statistics memory descriptor
{
	ste3a_count		bins;									//		bins count
	ste3a_count		channels;								//		channels count
	ste3a_stride	bstride;								//		bin stride
	ste3a_stride	cstride;								//		channel stride
};

struct ste3a_histograms_stats_status						// histograms statistics wrapper
{
	ste3a_histograms_stats_descriptor	  descriptor;		//		memory descriptor
	ste3a_histstats_val					* values;			//		values pointer
};

struct ste3a_grid_stats_descriptor							// grid statistics memory descriptor
{
	ste3a_count		sizex;									//		grid cells count (x direction)
	ste3a_count		sizey;									//		grid cells count (y direction)
	ste3a_count		channels;								//		channels count
	ste3a_stride	xstride;								//		x stride
	ste3a_stride	ystride;								//		y stride
	ste3a_stride	cstride;								//		channel stride
};

struct ste3a_grid_stats_status								// grid statistics wrapper
{
	ste3a_grid_stats_descriptor	  descriptor;				//		memory descriptor
	ste3a_gridstats_val			* values;					//		values pointer
};

struct ste3a_focus_stats_descriptor							// focus statistics memory descriptor (for each measure)
{
	ste3a_count		windows;								//		windows count
	ste3a_count		channels;								//		channels count
	ste3a_stride	wstride;								//		window stride
	ste3a_stride	cstride;								//		channel stride
};

struct ste3a_focus_stats_status								// focus statistics wrapper
{
	ste3a_focus_stats_descriptor	  descriptor;			//		memory descriptor
	ste3a_focusstats_high_f_val		* highf_values;			//		high frequency values (focus) pointer
	ste3a_focusstats_zero_f_val		* zerof_values;			//		zero frequency values (brightness) pointer
	ste3a_count						* widths;				//		regions widths pointer
	ste3a_count						* heights;				//		regions heights pointer
};

struct ste3a_stats_descriptor								// combined statistics memory descriptor
{
	ste3a_grid_stats_descriptor	vv_grid;					//		grid statistics descriptor for video and viewfinder modes
	ste3a_grid_stats_descriptor	si_grid;					//		grid statistics descriptor for still image mode

	ste3a_histograms_stats_descriptor histogram;			//		histograms statistics descriptor
};


struct ste3a_histograms_stats_geometry						// histogram statistics geometry descriptor
{
	ste3a_frame_space	space;								//		geometric space
	ste3a_norm_length	left;								//		starting x position
	ste3a_norm_length	top;								//		starting y position
	ste3a_norm_length	width;								//		x size
	ste3a_norm_length	height;								//		y size
};

struct ste3a_grid_stats_geometry							// grid statistics geometry descriptor
{
	ste3a_frame_space	space;								//		geometric space
	ste3a_norm_length	left;								//		starting x position
	ste3a_norm_length	top;								//		starting y position
	ste3a_norm_length	cell_width;							//		cell x size
	ste3a_norm_length	cell_height;						//		cell y size
};

struct ste3a_focus_stats_zone_geometry						// focus statistics zone geometry descriptor
{
	ste3a_norm_length	left;								//		starting x position
	ste3a_norm_length	top;								//		starting y position
	ste3a_norm_length	width;								//		x size
	ste3a_norm_length	height;								//		y size
};

#define STE3A_AFC_STATS_ZONES_MAX_COUNT	10					// maximum number of afc statistics zones
#define STE3A_AFC_STATS_CHANS_MAX_COUNT	1					// maximum number of afc statistics channels

struct ste3a_focus_stats_geometry							// focus statistics geometry descriptor
{
	ste3a_frame_space				space;					//		geometric space
	ste3a_count						zones_count;			//		zones descriptors
	ste3a_focus_stats_zone_geometry	zones_array[STE3A_AFC_STATS_ZONES_MAX_COUNT];
};

#endif /* STE3A_STATS_TYPES_H_ */
