/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * \file    ste3a.h
 * \brief   STE 3A library interface
 * \author  ST-Ericsson Image Quality Team, Agrate (Italy)
 */

#ifndef STE3A_H_
#define STE3A_H_

#include "ste3a_types.h"
#include "ste3a_aec_types.h"
#include "ste3a_afc_types.h"
#include "ste3a_art_types.h"
#include "ste3a_awb_types.h"
#include "ste3a_cmal_types.h"
#include "ste3a_fgal_types.h"
#include "ste3a_ppal_types.h"
#include "ste3a_stats_types.h"

struct ste3a_callbacks										// global callbacks pointers wrapper
{
	ste3a_common_callbacks	com;							//		common callbacks pointers
	ste3a_aec_callbacks		aec;							//		aec specifc callbacks pointers
	ste3a_afc_callbacks		afc;							//		afc specifc callbacks pointers
};


/***** library versioning information ****************************************
*
* returns the library revision data, to be cross-checked against the
* STE3A_REVISION_* macros
*
* outputs:
*
* major         library major revision
* minor         library minor revision
* revision      library revision
*
*****************************************************************************/
void ste3a_revision(							unsigned int					& major,
												unsigned int					& minor,
												unsigned int					& build);



/***** library instantiation *************************************************
*
* instantiates an instance of the STE 3A library and returns an opaque handle
* that will identify the new instance when calling other functions.
*
* inputs:
*
* cbs           collection of pointers to callback functions: they cannot be
*               NULL
* stats_descriptor sizing information for externally implemented statistics
*               accumulation; static data only, dynamic data (eg. AF sizes)
*               are demanded to other calls
* tuning_data   pointer to a buffer containing data from binary
*               characterization file
* tuning_szie   size of the buffer containing data from binary
*               characterization file
*
* returns:      opaque pointer to the new instance
*
*****************************************************************************/
ste3a_handle ste3a_open(				const	ste3a_callbacks					& cbs,
										const	ste3a_stats_descriptor			& stats_descriptor,
										const	void							* tuning_data,
										const	ste3a_count						  tuning_size,
										const	ste3a_calibration_data			* calibration_data);



/***** library instance finalization *****************************************
*
* finalizes an instance of the library
*
* inputs:
*
* handle        opaque pointer to the instance to be closed
*
*****************************************************************************/
void ste3a_close(						const	ste3a_handle					  handle);



/***** histograms statistics geometry computation ****************************
*
* computes the geometry for the histograms statistics
*
* inputs:
*
* handle        opaque pointer to the instance
* dzoom_factor  current digital zoom factor
*
* outputs:
*
* geometry      updated geometry
*
* returns:		flag indicating that geometry changed compared to previous
*               invocation
*
*****************************************************************************/
ste3a_bool ste3a_compute_stats_geometry_histograms(
										const	ste3a_handle					  handle,
										const	ste3a_zoom_factor				  dzoom_factor,
												ste3a_histograms_stats_geometry	& geometry);



/***** grid statistics geometry computation **********************************
*
* computes the geometry for the grid statistics
*
* inputs:
*
* handle        opaque pointer to the instance
* acq_mode      acquisition mode (viewfinder, still images, video)
* dzoom_factor  current digital zoom factor
*
* outputs:
*
* geometry      updated geometry
*
* returns:		flag indicating that geometry changed compared to previous
*               invocation
*
*****************************************************************************/
ste3a_bool ste3a_compute_stats_geometry_grid(
										const	ste3a_handle handle,
										const	ste3a_acquisition_mode			  acq_mode,
										const	ste3a_zoom_factor				  dzoom_factor,
												ste3a_grid_stats_geometry		& geometry);



/***** focus statistics geometry computation **********************************
*
* computes the geometry for the focus statistics
*
* inputs:
*
* handle        opaque pointer to the instance
* dzoom_factor  current digital zoom factor
*
* outputs:
*
* geometry      updated geometry
*
* returns:		flag indicating that geometry changed compared to previous
*               invocation
*
*****************************************************************************/
ste3a_bool ste3a_compute_stats_geometry_focus(
										const	ste3a_handle					  handle,
										const	ste3a_zoom_factor				  dzoom_factor,
												ste3a_focus_stats_geometry		& geometry);



/***** AEC coldstart exposure initialization *********************************
*
* informs the AEC algorithm about the ambient light level (read from an
* external sensor), so that an appropriate exposure level is computed for
* starting the convergence loop and having the fastest possible convergence
* time
*
* inputs:
*
* handle        opaque pointer to the instance
* light_level   current light level (from external sensor)
*
*****************************************************************************/
void ste3a_aec_compute_coldstart_exposure(	const	ste3a_handle					  handle,
											const	ste3a_light_level				  light_level);



/***** AEC coldstart exposure reset ******************************************
*
* programs the AEC algorithm for using the statically defined coldastart
* exposure
*
* inputs:
*
* handle        opaque pointer to the instance
*
*****************************************************************************/
void ste3a_aec_reset_coldstart_exposure(	const	ste3a_handle					  handle);



/***** AEC statistics reception function (flash not fired case) **************
*
* informs the AEC algorithm that a new set of statistics is available; the
* flash was not fired when the frame statistics were accumulated
*
* inputs:
*
* handle        opaque pointer to the instance
" current_time  current system time, used for convergence control
* cammod_status camera module status, for the frame statistics were acquired
*               on
* histograms_status descriptor of the histograms statistics
* grid_status   descriptor of the grid statistics
*
*****************************************************************************/
void ste3a_aec_update_noflash(			const	ste3a_handle					  handle,
										const	ste3a_timestamp					  current_time,
										const	ste3a_cammod_exposure_status	& cammod_status,
										const	ste3a_histograms_stats_status	& histograms_status,
										const	ste3a_grid_stats_status			& grid_status);



/***** AEC statistics reception function (flash fired case) ******************
*
* informs the AEC algorithm that a new set of statistics is available; the
* flash was fired when the frame statistics were accumulated
*
* inputs:
*
* handle        opaque pointer to the instance
" current_time  current system time, used for convergence control
* cammod_status camera module status, for the frame statistics were acquired
*               on
* flash_status  flasg device status, for the frame statistics were acquired on
* histograms_status descriptor of the histograms statistics
* grid_status   descriptor of the grid statistics
*
*****************************************************************************/
void ste3a_aec_update_flash(			const	ste3a_handle					  handle,
										const	ste3a_timestamp					  current_time,
										const	ste3a_cammod_exposure_status	& cammod_status,
										const	ste3a_flash_exposure_status		& flash_status,
										const	ste3a_histograms_stats_status	& histograms_status,
										const	ste3a_grid_stats_status			& grid_status);



/***** AEC invocation (next frame will be acquired without flash) ************
*
* compute the exposure parameters for a frame acquired without flash
*
* inputs:
*
* handle        opaque pointer to the instance
" current_time  current system time, used for convergence control
* isp_cg_config channel gains (from AWB)
*
* outputs:
*
* cammod_config camera module exposure parameters
* isp_dg_config exposure digital gain
* isp_er_config exposure recovery gain
* isp_cs_config contrast stretch parameters
* damper_bases  bases for dampers computations
* scene_info    scene description to be used for configuring other algorithms
* stable        flag indicating that the exposure scene is stable and that the
*               algorithm reached convergence
* flash         flag indicating the need for the flash
*
*****************************************************************************/
void ste3a_aec_evaluate_noflash(		const	ste3a_handle					  handle,
										const	ste3a_timestamp					  current_time,
										const	ste3a_isp_cg_config				& isp_cg_config,
												ste3a_cammod_exposure_config	& cammod_config,
												ste3a_isp_dg_config				& isp_dg_config,
												ste3a_isp_er_config				& isp_er_config,
												ste3a_isp_cs_config				& isp_cs_config,
												ste3a_damper_bases				& damper_bases,
												ste3a_aec_scene_info			& scene_info,
												ste3a_bool						& stable,
												ste3a_bool						& flash);



/***** AEC invocation (next frame will be acquired with flash) ***************
*
* compute the exposure parameters for a frame acquired with flash
*
* inputs:
*
* handle        opaque pointer to the instance
* current_time  current system time, used for convergence control
* flash_mode    flash mode (pre/main)
* isp_cg_config channel gains (from AWB)
*
* outputs:
*
* cammod_config camera module exposure parameters
* flash_config  flash device exposure parameters
* isp_dg_config exposure digital gain
* isp_er_config exposure recovery gain
* isp_cs_config contrast stretch parameters
* damper_bases  bases for dampers computations
* scene_info    scene description to be used for configuring other algorithms
* stable        flag indicating that the exposure scene is stable and that the
*               algorithm reached convergence
*
*****************************************************************************/
void ste3a_aec_evaluate_flash(			const	ste3a_handle					  handle,
										const	ste3a_timestamp					  current_time,
										const	ste3a_aec_flash_mode			  flash_mode,
										const	ste3a_isp_cg_config				& isp_cg_config,
												ste3a_cammod_exposure_config	& cammod_config,
												ste3a_flash_exposure_config		& flash_config,
												ste3a_isp_dg_config				& isp_dg_config,
												ste3a_isp_er_config				& isp_er_config,
												ste3a_isp_cs_config				& isp_cs_config,
												ste3a_damper_bases				& damper_bases,
												ste3a_aec_scene_info			& scene_info,
												ste3a_bool						& stable);



/***** AEC invocation (on the acquired frame for still image) ***************
*
* refines exposure parameters for the acquired frame
*
* inputs:
*
* handle        opaque pointer to the instance
* current_time  current system time, used for convergence control
* cammod_status camera module exposure parameters used during the capture phase
* isp_cg_config channel gains (from AWB)
*
* outputs:
*
* isp_dg_config exposure digital gain
* isp_er_config exposure recovery gain
* isp_cs_config contrast stretch parameters
* damper_bases  bases for dampers computations
* scene_info    scene description to be used for configuring other algorithms
*
*****************************************************************************/
void ste3a_aec_evaluate_still_noflash(	const	ste3a_handle					  handle,
										const	ste3a_timestamp					  current_time,
										const	ste3a_cammod_exposure_status	& cammod_status,
										const	ste3a_isp_cg_config				& isp_cg_config,
												ste3a_isp_dg_config				& isp_dg_config,
												ste3a_isp_er_config				& isp_er_config,
												ste3a_isp_cs_config				& isp_cs_config,
												ste3a_damper_bases				& damper_bases,
												ste3a_aec_scene_info			& scene_info);



/***** AEC invocation (on the acquired frame for still image) ***************
*
* refines exposure parameters for the acquired frame
*
* inputs:
*
* handle        opaque pointer to the instance
* current_time  current system time, used for convergence control
* cammod_status camera module exposure parameters used during the capture phase
* flash_status	flash exposure parameters used during the capture phase
* isp_cg_config channel gains (from AWB)
*
* outputs:
*
* isp_dg_config exposure digital gain
* isp_er_config exposure recovery gain
* isp_cs_config contrast stretch parameters
* damper_bases  bases for dampers computations
* scene_info    scene description to be used for configuring other algorithms
*
*****************************************************************************/
void ste3a_aec_evaluate_still_flash(	const	ste3a_handle					  handle,
										const	ste3a_timestamp					  current_time,
										const	ste3a_cammod_exposure_status	& cammod_status,
										const	ste3a_flash_exposure_status		& flash_status,
										const	ste3a_isp_cg_config				& isp_cg_config,
												ste3a_isp_dg_config				& isp_dg_config,
												ste3a_isp_er_config				& isp_er_config,
												ste3a_isp_cs_config				& isp_cs_config,
												ste3a_damper_bases				& damper_bases,
												ste3a_aec_scene_info			& scene_info);



/***** AEC lock **************************************************************
*
* locks the AEC algorithm
*
* inputs:
*
* handle        opaque pointer to the instance
*
*****************************************************************************/
void ste3a_aec_lock(					const	ste3a_handle					  handle);



/***** AEC unlock ************************************************************
*
* unlocks the AEC algorithm
*
* inputs:
*
* handle        opaque pointer to the instance
*
*****************************************************************************/
void ste3a_aec_unlock(					const	ste3a_handle					  handle);



/***** AEC internal status reset *********************************************
*
* resets the AEC algorithm status, as when no statistics update has yet been
* received
*
* inputs:
*
* handle        opaque pointer to the instance
*
*****************************************************************************/
void ste3a_aec_reset(					const	ste3a_handle					  handle);



/***** AEC internal status save **********************************************
*
* saves the AEC algorithm status, in order to restore it later; nesting calls
* to this function is not allowed, as only the most recently saved state will
* be available for being restored
*
* inputs:
*
* handle        opaque pointer to the instance
*
*****************************************************************************/
void ste3a_aec_save(					const	ste3a_handle					  handle);



/***** AEC internal status restore *******************************************
*
* restores the AEC algorithm status from the saved one; nesting calls to this
* function is not allowed, as only the most recently saved state is available
* for being restored
*
* inputs:
*
* handle        opaque pointer to the instance
*
*****************************************************************************/
void ste3a_aec_restore(					const	ste3a_handle					  handle);



/***** AEC internal status swap **********************************************
*
* swaps the current AEC algorithm status with the saved one
*
* inputs:
*
* handle        opaque pointer to the instance
*
*****************************************************************************/
void ste3a_aec_swap(					const	ste3a_handle					  handle);



/***** AEC tuning parameters update ******************************************
*
* updates the tuning parameters for the AEC algorithm
*
* inputs:
*
* handle        opaque pointer to the instance
* tuning        new set of tuning parameters
*
*****************************************************************************/
void ste3a_aec_set_tuning(				const	ste3a_handle					  handle,
										const	ste3a_aec_tuning				& tuning);



/***** AEC tuning parameters inspection **************************************
*
* returns a copy of the active tuning parameters for the AEC algorithm
*
* inputs:
*
* handle        opaque pointer to the instance
*
* outputs:
*
* tuning        current set of tuning parameters
*
*****************************************************************************/
void ste3a_aec_get_tuning(				const	ste3a_handle					  handle,
												ste3a_aec_tuning				& tuning);


/***** AEC dynamic weighting functionality enable ****************************
*
* sets up the zones geometry for the dynamic weighting functionality and
* starts it
*
* inputs:
*
* handle        opaque pointer to the instance
* geometry      initial zones geometry
*
*****************************************************************************/
void ste3a_aec_dynamic_weighting_start(	const	ste3a_handle							  handle,
										const	ste3a_aec_dynamic_weighting_geometry	& geometry);


/***** AEC dynamic weighting functionality update ****************************
*
* updates the zones geometry for the dynamic weighting functionality
*
* inputs:
*
* handle        opaque pointer to the instance
* geometry      updated zones geometry
*
*****************************************************************************/
void ste3a_aec_dynamic_weighting_update(const	ste3a_handle							  handle,
										const	ste3a_aec_dynamic_weighting_geometry	& geometry);


/***** AEC dynamic weighting functionality disable ***************************
*
* stops the dynamic weighting functionality
*
* inputs:
*
* handle        opaque pointer to the instance
*
*****************************************************************************/
void ste3a_aec_dynamic_weighting_stop(	const	ste3a_handle							  handle);



/***** AFC convergence start invocation **************************************
*
* instruct the AF algorithm to start a convergence cycle
*
* inputs:
*
* handle        opaque pointer to the instance
* mode          focusing mode (single, continuous free-running, continuous
*               auto-locking)
* stats_descriptor statistics sizing information
*
*****************************************************************************/
void ste3a_afc_start(					const	ste3a_handle					  handle,
										const	ste3a_afc_mode					  mode,
										const	ste3a_focus_stats_descriptor	& stats_descriptor);



/***** AFC convergence mode change *******************************************
*
* instruct the AF algorithm to change its convergence mode
*
* inputs:
*
* handle        opaque pointer to the instance
* mode          focusing mode (single, continuous free-running, continuous
*               auto-locking)
*
*****************************************************************************/
void ste3a_afc_change_mode(				const	ste3a_handle					  handle,
										const	ste3a_afc_mode					  mode);



/***** AFC convergence abortion **********************************************
*
* instruct the AF algorithm to abort a convergence cycle
*
* inputs:
*
* handle        opaque pointer to the instance
*
*****************************************************************************/
void ste3a_afc_stop(					const	ste3a_handle					  handle);



/***** AFC lens position synchronization ************************************
*
* inform the AF algorithm that the lens has been moved by an external actor;
* if called when a convergence loop is running it is aborted
*
* inputs:
*
* handle        opaque pointer to the instance
* displacement  lens position
*
*****************************************************************************/
void ste3a_afc_follow(					const	ste3a_handle					  handle,
										const	ste3a_logical_lens_displacement	& displacement);



/***** AFC lens position synchronization ************************************
*
* inform the AF algorithm that the lens position is no more known
* if called when a convergence loop is running it is aborted
*
* inputs:
*
* handle        opaque pointer to the instance
* displacement  lens position (in HW units)
*
*****************************************************************************/
void ste3a_afc_lose(					const	ste3a_handle					  handle);



/***** AFC statistics reception function *************************************
*
* informs the AFC algorithm that a new set of statistics is available
*
* inputs:
*
* handle        opaque pointer to the instance
" current_time  current system time, used for profiling
* scene_status  scene status associated to passed statistics
* cammod_status camera module status, for the frame statistics were acquired
*               on
* stats_status  descriptor of the focus statistics
*
*****************************************************************************/
void ste3a_afc_update(					const	ste3a_handle					  handle,
										const	ste3a_timestamp					  current_time,
										const	ste3a_scene_status				& scene_status,
										const	ste3a_cammod_focus_status		& cammod_status,
										const	ste3a_focus_stats_status		& stats_status);



/***** AFC iteration invocation **********************************************
*
* compute movement to be commanded to the lens
*
* inputs:
*
* handle        opaque pointer to the instance
" current_time  current system time, used for profiling
*
* outputs:
*
* cammod_config camera module parameters for lens movement
* state         resulting state of the AF state machine
* scene_info    scene information
*
*****************************************************************************/
void ste3a_afc_evaluate(				const	ste3a_handle					  handle,
										const	ste3a_timestamp					  current_time,
												ste3a_cammod_focus_config		& cammod_config,
												ste3a_afc_state					& state,
												ste3a_afc_scene_info			& scene_info);



/***** AFC locked zones retrieva*l *******************************************
*
* returns the index of the AF zone that the algorithm selected on the latest
* convergence
*
* inputs:
*
* handle        opaque pointer to the instance
*
* outputs:
*
* locked_zone   array of flags, one for each zone
*
*****************************************************************************/
void ste3a_afc_get_locked_zones(		const	ste3a_handle			  handle,
												ste3a_bool			   (& locked_zone)[STE3A_AFC_STATS_ZONES_MAX_COUNT]);



/***** AFC tuning parameters update ******************************************
*
* updates the tuning parameters for the AFC algorithm
*
* inputs:
*
* handle        opaque pointer to the instance
* tuning        new set of tuning parameters
*
*****************************************************************************/
void ste3a_afc_set_tuning(				const	ste3a_handle					  handle,
										const	ste3a_afc_tuning				& tuning);



/***** AFC tuning parameters inspection **************************************
*
* returns a copy of the active tuning parameters for the AFC algorithm
*
* inputs:
*
* handle        opaque pointer to the instance
*
* outputs:
*
* tuning        current set of tuning parameters
*
*****************************************************************************/
void ste3a_afc_get_tuning(				const	ste3a_handle					  handle,
												ste3a_afc_tuning				& tuning);



/***** AWB statistics reception function ************************************
*
* informs the AWB algorithm that a new set of statistics is available;
*
* inputs:
*
* handle        opaque pointer to the instance
" current_time  current system time, used for convergence control
* acq_mode      acquisition mode (viewfinder, still images, video)
* scene_status  scene status, for the frame statistics were acquired
* grid_status   descriptor of the grid statistics
*
*****************************************************************************/
void ste3a_awb_update(					const	ste3a_handle					  handle,
										const	ste3a_timestamp					  current_time,
										const	ste3a_acquisition_mode			  acq_mode,
										const	ste3a_scene_status				& scene_status,
										const	ste3a_grid_stats_status			& grid_status);



/***** AWB invocation ********************************************************
*
* compute the white balance parameters for a frame acquired
*
* inputs:
*
* handle        opaque pointer to the instance
" current_time  current system time, used for convergence control
*
* outputs:
*
* isp_cg_config channel gains gains
* isp_cm_config colour matrix
* scene_info    scene information
* stable        flag indicating that the scene is stable and that the
*               white balance algorithm reached convergence
*
*****************************************************************************/
void ste3a_awb_evaluate(				const	ste3a_handle					  handle,
										const	ste3a_timestamp					  current_time,
												ste3a_isp_cg_config				& isp_cg_config,
												ste3a_isp_cm_config				& isp_cm_config,
												ste3a_awb_scene_info			& scene_info,
												ste3a_bool						& stable);



/***** AWB lock **************************************************************
*
* locks the AWB algorithm
*
* inputs:
*
* handle        opaque pointer to the instance
*
*****************************************************************************/
void ste3a_awb_lock(					const	ste3a_handle					  handle);



/***** AWB unlock ************************************************************
*
* unlocks the AWB algorithm
*
* inputs:
*
* handle        opaque pointer to the instance
*
*****************************************************************************/
void ste3a_awb_unlock(					const	ste3a_handle					  handle);



/***** AWB internal status reset *********************************************
*
* resets the AWB algorithm status, as when no statistics update has yet been
* received
*
* inputs:
*
* handle        opaque pointer to the instance
*
*****************************************************************************/
void ste3a_awb_reset(					const	ste3a_handle					  handle);



/***** AWB internal status save **********************************************
*
* saves the AWB algorithm status, in order to restore it later; nesting calls
* to this function is not allowed, as only the most recently saved state will
* be available for being restored
*
* inputs:
*
* handle        opaque pointer to the instance
*
*****************************************************************************/
void ste3a_awb_save(					const	ste3a_handle					  handle);



/***** AWB internal status restore *******************************************
*
* restores the AWB algorithm status from the saved one; nesting calls to this
* function is not allowed, as only the most recently saved state is available
* for being restored
*
* inputs:
*
* handle        opaque pointer to the instance
*
*****************************************************************************/
void ste3a_awb_restore(					const	ste3a_handle					  handle);



/***** AWB internal status swap **********************************************
*
* swaps the current AWB algorithm status with the saved one
*
* inputs:
*
* handle        opaque pointer to the instance
*
*****************************************************************************/
void ste3a_awb_swap(					const	ste3a_handle					  handle);



/***** AWB tuning parameters update ******************************************
*
* updates the tuning parameters for the AWB algorithm
*
* inputs:
*
* handle        opaque pointer to the instance
* tuning        new set of tuning parameters
*
*****************************************************************************/
void ste3a_awb_set_tuning(				const	ste3a_handle					  handle,
										const	ste3a_awb_tuning				& tuning);



/***** AWB tuning parameters inspection **************************************
*
* returns a copy of the active tuning parameters for the AWB algorithm
*
* inputs:
*
* handle        opaque pointer to the instance
*
* outputs:
*
* tuning        current set of tuning parameters
*
*****************************************************************************/
void ste3a_awb_get_tuning(				const	ste3a_handle					  handle,
												ste3a_awb_tuning				& tuning);



/***** ART (Auto Rendition Tuning) static tone mapping computation **********
*
* computes the static tone mapping curve
*
* inputs:
*
* handle        opaque pointer to the instance
*
* outputs:
*
* isp_tm_config tone mapping config
*
*****************************************************************************/
void ste3a_art_get_static_tone_mapping(	const	ste3a_handle					  handle,
												ste3a_isp_tm_config				& isp_tm_config);



/***** ART (Auto Rendition Tuning) static gamma coding computation **********
*
* computes the static gamma coding multipliers
*
* inputs:
*
* handle        opaque pointer to the instance
*
* outputs:
*
* isp_gc_config gamma coding config
*
*****************************************************************************/
void ste3a_art_get_gamma_coding(		const	ste3a_handle					  handle,
												ste3a_isp_gc_config				& isp_gc_config);


/***** ART (Auto Rendition Tuning) evaluate for static tone mapping *********
*
* refines ISP linear parameters, without updating the tone mapping curve
*
* inputs:
*
* handle        opaque pointer to the instance
* damper_bases  set of computed damper bases
*
* input/outputs:
*
* isp_cg_config channel gains config
* isp_dg_config digital gain config
* isp_cm_config colour matrix config
* isp_er_config exposure recovery config
* isp_cs_config contrast stretch config
*
* outputs:
*
* isp_tm_config tone mapping config
*
*****************************************************************************/
void ste3a_art_evaluate_static_tonemapping(
										const	ste3a_handle					  handle,
										const	ste3a_damper_bases				& damper_bases,
												ste3a_isp_cg_config				& isp_cg_config,
												ste3a_isp_dg_config				& isp_dg_config,
												ste3a_isp_cm_config				& isp_cm_config,
												ste3a_isp_er_config				& isp_er_config,
												ste3a_isp_cs_config				& isp_cs_config);


/***** ART (Auto Rendition Tuning) evaluate for dynamic tone mapping ********
*
* refines ISP linear parameters and computes the tone mapping curve and the gamma multipliers
*
* inputs:
*
* handle        opaque pointer to the instance
* histograms_status histogram stats
* grid_status   grid stats
* damper_bases  set of computed damper bases
*
* input/outputs:
*
* isp_cg_config channel gains config
* isp_dg_config digital gain config
* isp_cm_config colour matrix config
* isp_er_config exposure recovery config
* isp_cs_config contrast stretch config
* isp_tm_config tone mapping config
* isp_gc_config gamma coding config
*
* outputs:
*
* isp_tm_config tone mapping config
*
*****************************************************************************/
void ste3a_art_evaluate_dynamic_tonemapping(
										const	ste3a_handle					  handle,
										const	ste3a_histograms_stats_status	& histograms_status,
										const	ste3a_grid_stats_status			& grid_status,
										const	ste3a_damper_bases				& damper_bases,
												ste3a_isp_cg_config				& isp_cg_config,
												ste3a_isp_dg_config				& isp_dg_config,
												ste3a_isp_cm_config				& isp_cm_config,
												ste3a_isp_er_config				& isp_er_config,
												ste3a_isp_cs_config				& isp_cs_config,
												ste3a_isp_tm_config				& isp_tm_config,
												ste3a_isp_gc_config				& isp_gc_config);


/***** ART tuning parameters update ******************************************
*
* updates the tuning parameters for the ART algorithm
*
* inputs:
*
* handle        opaque pointer to the instance
* tuning        new set of tuning parameters
*
*****************************************************************************/
void ste3a_art_set_tuning(				const	ste3a_handle					  handle,
										const	ste3a_art_tuning				& tuning);


/***** ART tuning parameters inspection **************************************
*
* returns a copy of the active tuning parameters for the ART algorithm
*
* inputs:
*
* handle        opaque pointer to the instance
*
* outputs:
*
* tuning        current set of tuning parameters
*
*****************************************************************************/
void ste3a_art_get_tuning(				const	ste3a_handle					  handle,
												ste3a_art_tuning				& tuning);


#endif /* STE3A_H_ */
