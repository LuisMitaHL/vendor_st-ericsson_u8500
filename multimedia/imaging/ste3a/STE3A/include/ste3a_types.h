/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * \file    ste3a_types.h
 * \brief   STE 3A types
 * \author  ST-Ericsson Image Quality Team, Agrate (Italy)
 */

#ifndef STE3A_TYPES_H_
#define STE3A_TYPES_H_

#ifdef _MSC_VER // Microsoft C compiler ?
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stddef.h>
#include <stdint.h>

#define STE3A_REVISION_MAJOR	2
#define STE3A_REVISION_MINOR	6
#define STE3A_REVISION_BUILD	120413

typedef float		ste3a_float;							// floating point						[pure number   ]
typedef uint32_t	ste3a_count;							// count								[pure number   ]
typedef uint32_t	ste3a_index;							// index from 0							[pure number   ]
typedef float		ste3a_time;								// duration								[milliseconds  ]
typedef float		ste3a_gain;								// gain (> 1) or attenuation (< 1)		[pure number   ]
typedef float		ste3a_power;							// normalized power (0 -> 0, 1 -> max)	[pure number   ]
typedef float		ste3a_energy;							// normalized energy					[milliseconds  ]
typedef int32_t		ste3a_stride;							// memory stride (in elements)			[pure number   ]
typedef float		ste3a_exposure;							// duration * gain						[milliseconds  ]
typedef float		ste3a_sensitivity;						// exposure to illuminance factor       [1/(msec*lux)  ]
typedef	uint32_t	ste3a_pixel_level;						// image channel level					[pure number   ]
typedef float		ste3a_normalized_pixel_level;			// pixel level normalized in the [0, 1]	[pure number   ]
typedef float		ste3a_normalized_snr;					// s/n ratio normalized in the [0, 1]	[pure number   ]
typedef float		ste3a_exposure_comp;					// exposure compensation				[stops         ]
typedef float		ste3a_norm_length;						// ratio between distances or positions	[pure number   ]
typedef float		ste3a_zoom_factor;						// zoom factor (> 1)					[pure number   ]
typedef float		ste3a_percentage;						// type for values from 0 to 100		[pure number   ]
typedef	float		ste3a_light_level;						// illuminance level					[lux           ]
typedef float		ste3a_normalized_lens_displacement;		// normalized lens displacement	 		(0 - > infinity, 1 -> macro)
typedef float		ste3a_kelvin;							// temperature in kelvin				[pure number   ]
typedef float		ste3a_normalized_correlation;			// normalized correlation coefficient	[pure number   ]
typedef float		ste3a_distance;							// distance						 		[meters        ]
typedef uint32_t	ste3a_timestamp;						// timestamp							[milliseconds  ]
typedef float		ste3a_inv_exposure;						// 1 / exposure							[1/milliseconds]
typedef float		ste3a_normalized_sharpness;				// normalized average sharpness			[pure number   ]
typedef float		ste3a_normalized_brightness;			// normalized average brightness		[pure number   ]

struct ste3a_revision
{
	unsigned int	major;
	unsigned int	minor;
	unsigned int	build;
};

class ste3a_bool
{
	public:

		ste3a_bool();

		ste3a_bool(const bool & other);

		ste3a_bool(const ste3a_bool & other);

		ste3a_bool & operator = (const bool & other);

		ste3a_bool & operator = (const ste3a_bool & other);

		bool operator == (const bool & other) const;

		bool operator == (const ste3a_bool & other) const;

		operator bool() const;

	private:

		uint32_t value;
};

#define STE3A_ENUM_BASE	(1L << 16)

enum ste3a_trace_level										// traces verbosity levels
{
	ste3a_trace_level_information = STE3A_ENUM_BASE,		//		information
	ste3a_trace_level_warning,								//		warning
	ste3a_trace_level_error									//		error
};

enum ste3a_type_to_dump										// type of data to dump
{
	ste3a_dump_type_nmx = STE3A_ENUM_BASE					//		nmx matrix
};

enum ste3a_frame_space										// geometric spaces fro stats geometry
{
	ste3a_frame_space_full = STE3A_ENUM_BASE,				//		ISP-input size
	ste3a_frame_space_dzoom 								//		ISO-output size
};

enum ste3a_exposure_metering_mode							// AEC metering mode
{
	ste3a_exposure_metering_basic_cl = STE3A_ENUM_BASE		//		closed loop (with brightness target)
};

enum ste3a_exposure_priority_mode							// AEC priority mode
{
	ste3a_exposure_priority_intfad = STE3A_ENUM_BASE,		//		iris first, then ND filter, then exposure time, then fading gain, then analog gain, then digital gain (suited for fully automatic mode)
	ste3a_exposure_priority_Aintfd,							//		analog gain first, then iris, then ND filter, then exposure time, then digital gain (suited for automatic mode + soft ISO)
	ste3a_exposure_priority_ADintf							//		analog gain first, then digital gain, then iris, then ND filter, then exposure time (suited for automatic mode + hard ISO)
};

enum ste3a_exposure_rule_driver								// input selector for exposure rule step
{
	ste3a_exposure_rule_driver_original = STE3A_ENUM_BASE,	//		overall required exposure
	ste3a_exposure_rule_driver_residual						//		residual exposure/gain from previous step
};

enum ste3a_acquisition_mode									// acquisition mode
{
	ste3a_acquisition_viewfinder = STE3A_ENUM_BASE,			//		viewfinder (both still picture and video)
	ste3a_acquisition_still_image,							//		still imagec acquisition
	ste3a_acquisition_video									//		video recording
};

struct ste3a_scene_status									// information about the scene context associated to statistics
{
	ste3a_light_level 				ambient_light_level;	//		measured light level (ambient)
	ste3a_light_level 				overall_light_level;	//		measured light level (ambient + flash)
	ste3a_bool						flash_fired;			//		flash fired
};

struct ste3a_color_matrix									// RGB to RGB conversion matrix
{
	ste3a_float	r_in_r;										//		R in R
	ste3a_float	g_in_r;										//		G in R
	ste3a_float	b_in_r;										//		B in R
	ste3a_float	r_in_g;										//		R in G
	ste3a_float	g_in_g;										//		G in G
	ste3a_float	b_in_g;										//		B in G
	ste3a_float	r_in_b;										//		R in B
	ste3a_float	g_in_b;										//		G in B
	ste3a_float	b_in_b;										//		B in B
};

struct ste3a_damper_bases									// bases value for dampers computation
{
	ste3a_time						integration_time;				//	integration time					[milliseconds]
	ste3a_gain						post_ag_gain;					//	analog gain * fading gain			[pure number]
	ste3a_gain						post_dg_minimum_gain;			//	post_ag_gain * minimum digital gain	[pure number]
	ste3a_gain						post_dg_average_gain;			//	post_ag_gain * average digital gain	[pure number]
	ste3a_gain						post_er_minimum_gain;			//	post_dg_minimum_gain * er gain		[pure number]
	ste3a_gain						post_er_average_gain;			//	post_dg_average_gain * er gain		[pure number]
	ste3a_gain						post_cs_minimum_gain;			//	post_er_minimum_gain * cs gain		[pure number]
	ste3a_gain						post_cs_average_gain;			//	post_er_average_gain * cs gain		[pure number]
	ste3a_normalized_pixel_level	normalized_pixel_level;			//	average channel level				[pure number [0, 1]]
	ste3a_normalized_pixel_level	normalized_pixel_level_red;		//	average red channel level			[pure number [0, 1]]
	ste3a_normalized_pixel_level	normalized_pixel_level_green;	//	average green channel level			[pure number [0, 1]]
	ste3a_normalized_pixel_level	normalized_pixel_level_blue;	//	average blue channel level			[pure number [0, 1]]
	ste3a_normalized_snr			normalized_snr;					//	average s/n ratio					[pure number]			note: a constant noise level (1/100 of the dynamic range) is assumed
	ste3a_normalized_snr			normalized_snr_red;				//	average red channel s/n ratio		[pure number]											"
	ste3a_normalized_snr			normalized_snr_green;			//	average green channel s/n ratio		[pure number]											"
	ste3a_normalized_snr			normalized_snr_blue;			//	average blue channel s/n ratio		[pure number]											"
};

typedef void * ste3acb_mem_malloc(const void * context, size_t size);

/***** memory allocation callback ********************************************
*
* requests the allocation of a chunk of memory
*
* inputs:
*
* context       context for the called function
* size          requested memory size, in bytes
*
* returns:      pointer to allocated memory, or NULL in case of failure
*
*****************************************************************************/


typedef void ste3acb_mem_free(const void * context, void * pointer);

/***** memory release callback ***********************************************
*
* requests the release of an allocated chunk of memory
*
* inputs:
*
* context       context for the called function
* pointer       pointer to memory to be released
*
*****************************************************************************/


typedef void ste3acb_dbg_trace(const void * context, const ste3a_trace_level level, const char * text);

/***** trace callback ********************************************************
*
* requests the printing of a trace message, for debugging purpose
*
* inputs:
*
* context       context for the called function
* level         trace level (information/warning/error)
* text          message to be displayed
*
*****************************************************************************/


typedef void ste3acb_dbg_record(const void * context, const ste3a_count bytes, const void * data);

/***** API logging callback **************************************************
*
* requests the storage of a memory dump of an api access, for later playback
*
* inputs:
*
* context       context for the called function
* bytes         number of bytes to be appended
* data          pointer to data to be appended
*
*****************************************************************************/


typedef void ste3acb_dbg_dump(const void * context, const void * data, ste3a_type_to_dump type, const char * name);

/***** data dumping callback **************************************************
*
* inputs:
*
* context       context for the called function
* data          pointer to data to be appended
* name          object name to dump
*
*****************************************************************************/


typedef void ste3acb_inf_makernotes(const void * context, const ste3a_count bytes, const void * data);

/***** makernotes logging callback *******************************************
*
* requests the storage of a memory dump on an api access, for makernotes
* storage
*
* inputs:
*
* context       context for the called function
* bytes         number of bytes to be appended
* data          pointer to data to be appended
*
*****************************************************************************/


struct ste3a_mem_callbacks									// memory allocation callbacks pointers wrapper
{
	ste3acb_mem_malloc		* ptr_mem_malloc;				//		malloc pointer (CANNOT BE NULL)
	ste3acb_mem_free		* ptr_mem_free;					//		free pointer (CANNOT BE NULL)

	void					* context;						//		pointer to callback's implementation context
};

struct ste3a_dbg_callbacks									// debugging callbacks pointers wrapper
{
	ste3acb_dbg_trace		* ptr_dbg_trace;				//		trace pointer (can be NULL)
	ste3acb_dbg_record		* ptr_dbg_record;				//		record pointer (can be NULL)
	ste3acb_dbg_dump		* ptr_dbg_dump;					//		dump pointer (can be NULL)

	void					* context;						//		pointer to callback's implementation context
};

struct ste3a_inf_callbacks									// information storage callbacks pointers wrapper
{
	ste3acb_inf_makernotes	* ptr_inf_makernotes;			//		makernotes pointer (can be NULL)

	void					* context;						//		pointer to callback's implementation context
};

struct ste3a_common_callbacks								// com mon callbacks pointers wrapper
{
	ste3a_mem_callbacks mem;								//		memory allocation callbacks pointers
	ste3a_dbg_callbacks dbg;								//		debugging callbacks pointers
	ste3a_inf_callbacks inf;								//		information storage callbacks pointers
};

struct ste3a_obj;											// library handle declaration (opaque pointer)
															//
typedef ste3a_obj * ste3a_handle;							//


#endif /* STE3A_TYPES_H_ */
