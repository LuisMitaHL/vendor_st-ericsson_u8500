
STE3A Library - release 2.6.120413


Package contents:

- nmxAawb                 nmx Aawb application library
- STE3A                   STE 3A library
- STE3A_CMAL_Reference    STE 3A Camera Module Abstraction Layer reference implementation
- STE3A_FGAL_Reference    STE 3A Flash Gun Abstraction Layer reference implementation
- STE3A_PPAL_8500V2       STE 3A Pixel Pipe Abstraction Layer 8500V2 implementation
- STE3A_playback          STE 3A library debug dumps playback application
- STE3A_test_app          STE 3A library test application

All source files are wrapped both as Eclipse C / MinGW projects and as Microsoft Visual Studio 2010 Express projects.


# Release notes v 2.6.120413

- STE3A library
  - bug fixes
    - AFC: fixed success/failure computation in case of multiple regions
    - AWB: fixed colour temperature limitation
    - AWB: fixed handling of flash use cases (phase 2)
  - functional changes
    - AEC: added support for ambient/overall (ambient+flash) light levels reporting
    - AWB: added support for ambient/overall (ambient+flash) light levels usage
    - AWB: added support for flash-specific temporal smoothing
  - API changes
    - types
      - "ste3a_scene_status" structure
        - "ambient_light_level" field added
        - "light_level" field renamed as "overall_light_level"
      - "ste3a_aec_scene_info" structure
        - "ambient_light_level" field added
        - "light_level" field renamed as "overall_light_level"
      - "ste3a_awb_tuning" structure
        - "flash_decay_time" field added
        - "flash_mired_decay_time" field added
- STE3A test application
  - absorbed API changes of other libraries
- STE3A playback application
  - absorbed API changes of other libraries


# Release notes v 2.5.120312

- STE3A library
  - bug fixes
    - AEC: fixed handling of lock at still capture time in flash use cases
    - AFC: fixed locked zones decision
    - AFC: fixed failed focus decision
    - AFC: fixed scene change evaluation in case of single region geometry
    - ART: fixed target exposure computation
    - ART: fixed S-curve functionality
  - functional changes
    - AFC: added "forced continue" functionality for increased accuracy on moving scenes
    - ART: added the capability of using linear interpolation for the static tone-mapping curve
  - API changes
    - functions
      - "ste3a_aec_evaluate_still" function renamed as "ste3a_aec_evaluate_still_noflash"
      - "ste3a_aec_evaluate_still_flash" function added
    - types
      - "ste3a_art_interpolation" enumeration added
      - "ste3a_art_tuning" structure
        - "st_interpolation" field added
      - "ste3a_afc_tuning" structure
        - "tracking_brgt_avgrt_threshold" field renamed as "restart_brgt_avgrt_threshold"
        - "tracking_shrp_avgrt_threshold" field renamed as "restart_shrp_avgrt_threshold"
        - "tracking_brgt_ergrt_threshold" field renamed as "restart_brgt_ergrt_threshold"
        - "tracking_shrp_ergrt_threshold" field renamed as "restart_shrp_ergrt_threshold"
        - "tracking_brgt_xcorr_threshold" field renamed as "restart_brgt_xcorr_threshold"
        - "tracking_shrp_xcorr_threshold" field renamed as "restart_shrp_xcorr_threshold"
        - "continue_brgt_avgrt_threshold" field added
        - "continue_shrp_avgrt_threshold" field added
        - "continue_brgt_ergrt_threshold" field added
        - "continue_shrp_ergrt_threshold" field added
        - "continue_brgt_xcorr_threshold" field added
        - "continue_shrp_xcorr_threshold" field added
      - "ste3a_afc_intermediates" structure
        - "restart_*" fields added
        - "restarted_*" fields added
        - "continue_*" fields added
        - "continued_*" fields added
        - "best_pos_sharpness" field added
        - "worst_pos_sharpness" field added
        - "best_pos_noise" field added
        - "worst_pos_noise" field added
        - "focus_good" field added
        - "focus_bad" field added
- STE3A test application
  - absorbed API changes of other libraries
- STE3A playback application
  - absorbed API changes of other libraries


# Release notes v 2.4.120215

- STE3A library
  - bug fixes
    - AEC: fix for the adaptive compensation feature
    - AEC: fix for the anti clipping feature
    - AWB: fix for reported stability flag
    - AFC: fix for allowing direct transition from full search to holding statuses
  - API changes
    - types
      - "ste3a_awb_tuning" structure
        - "computation_period" field type changed from integer to float
        - "fcs_computation_period" field type changed from integer to float
        - "forced_stability_delay_time" field added
        - "fcs_forced_stability_delay_time" field added


# Release notes v 2.3.120209

- STE3A library
  - external dependencies changes
    - nmx 2.1 (was 2.0)
    - ccal 2.1 (was 2.0)
  - functional changes
    - AEC: support for backlight detection
    - ART: support for dynamic tone mapping
  - bug fixes
    - AEC: exposure parameters used for the raw bayer capture now available for post-capture exposure refinement even when the algorithm is locked (fix for ER 415630)
    - AFC: sharpness statistics are now normalized against region size (fix for ER 410487)
  - cosmetic changes
    - compile time warnings fixed
    - Coverity incompliances fixed
    - some function's parameters are now declared as "const", for clarity, even if passed by value
  - API changes
    - types
      - "ste3a_focus_stats_status" structure:
         - following fields have been added:
           - "widths"
           - "heights"
      - "ste3a_aec_tuning" structure
         - following fields have been added:
          - "backlight_detection_enable"
          - "bd_weights_map"
          - "bd_weights_tbl"
          - "bd_mapped_unmapped_ratio_zero_compensation"
          - "bd_mapped_unmapped_ratio_full_compensation"
      - "ste3a_afc_tuning" structure
         - following fields have been renamed (old/new name):
           - "tracking_zero_f_avgrt_threshold" / "tracking_brgt_avgrt_threshold"
           - "tracking_high_f_avgrt_threshold" / "tracking_shrp_avgrt_threshold"
           - "tracking_zero_f_ergrt_threshold" / "tracking_brgt_ergrt_threshold"
           - "tracking_high_f_ergrt_threshold" / "tracking_shrp_ergrt_threshold"
           - "tracking_zero_f_xcorr_threshold" / "tracking_brgt_xcorr_threshold"
           - "tracking_high_f_xcorr_threshold" / "tracking_shrp_xcorr_threshold"
           - "hfn_pcount" / "sharpness_noise_pcount"
           - "hfn_plightlevel" / "sharpness_noise_plightlevel"
           - "hfn_pthreshold" / "sharpness_noise_pthreshold"
      - "ste3a_art_tuning" structure
         - following fields have been added:
           - "dynamic_tonemapping_enable"
           - "dt_gain_boost_enable"
           - "dt_adaptive_gamma_enable"
           - "dt_scurve_enable"
           - "dt_black_boost_enable"
           - "dt_gain_boost_white_point"
           - "dt_gamma_min"
           - "dt_gamma_max"
           - "dt_scurve_target"
           - "dt_scurve_min"
           - "dt_scurve_max"
           - "dt_black_boost_len"
           - "dt_black_boost_exp"
       - "ste3a_art_intermediates" structure
         - following fields have been added:
           - "hist_rgb_in"
           - "hist_rgb_cg"
           - "hist_Y_inp"
           - "hist_Y_out"
           - "hist_Y_tmp"
           - "lut_out"
           - "lut_tmp"
           - "scale"
           - "vtemp1"
           - "vtemp2"
           - "vtemp3"
           - "gain_boost"
           - "gamma_out"
           - "scurve_proportion"
    - functions
      - "ste3a_aec_evaluate_still" function
        - "current_time" parameter added
        - "cammod_status" parameter added
- STE3A_CMAL_Reference library
  - cosmetic changes
    - compile time warnings fixed
- STE3A_FGAL_Reference library
  - cosmetic changes
    - compile time warnings fixed
- STE3A test application
  - absorbed API changes of other libraries
  - cosmetic changes
    - compile time warnings fixed
- STE3A playback application
  - absorbed API changes of other libraries
  - cosmetic changes
    - compile time warnings fixed


# Release notes v 2.2.120113

- STE3A library
  - functional changes
    - AFC: added provisions for returning focus status in CAF mode
    - AFC: added support for combining "local evaluation" and "interpolation" functionalities
    - AEC: added support for half-pressed-time handling of the flash
    - AWB: added support for half-pressed-time handling of the flash
  - bug fixes
    - AEC: fixed the bug that caused the computation of a null exposure time when a fixed one was requested
    - AWB: improved accuracy with low-colour-temperature illuminants (indoor conditions)
    - AWB: temporal smoothing now not applied in flash use cases
  - API changes
    - types
      - "ste3a_afc_state" enumeration
        - "cf_pausing_k" duplicated as "cf_pausing_good_focus_k" + "cf_pausing_bad_focus_k"
        - "ca_pausing_k" duplicated as "ca_pausing_good_focus_k" + "ca_pausing_bad_focus_k"
        - "ca_holding_k" duplicated as "ca_holding_good_focus_k" + "ca_holding_bad_focus_k"
    - functions
      - "ste3a_aec_swap" function added
      - "ste3a_awb_swap" function added


# Release notes v 2.1.111207

- STE3A library
  - functional changes
    - AEC: "fast mode" added, for having faster frame rate during AFC search
    - AEC: "fast preflash mode" added, for shorter shutter lag in flash use cases
    - AFC: added the capability to resume CAF mode without triggering a full search
    - AFC: timed update/evaluate functions (for profiling only)
  - bug fixes
    - AEC: fast coldstart parameterization completed
    - AEC: moved from linear to logarithmic damping model
    - AWB: "frozen AWB" issue (introduced with STE3A 2.0) fixed
  - API changes
    - types
      - "ste3a_afc_state" enumeration
        - "ca_holding_k" option added
      - "ste3a_aec_tuning" structure
         - following fields have been added:
          - "fcs_stability_factor"
          - "pf_max_igain"
          - "pf_max_ngain"
          - "pf_min_etime"
          - "pf_mid_etime"
          - "pf_max_etime"
          - "pf_max_again"
          - "pf_max_dgain"
          - "pf_stability_factor"
          - "fast_mode_enable"
          - "fms_max_igain"
          - "fms_max_ngain"
          - "fms_min_etime"
          - "fms_mid_etime"
          - "fms_max_etime"
          - "fms_max_again"
          - "fms_max_dgain"
   - functions
      - "current_time" parameter added to the following functions:
        - "ste3a_afc_update"
        - "ste3a_afc_evaluate"


# Release notes v 2.0.111108

- STE3A library
  - dependencies changes
    - depends on nmx library rev. 2.0 (was 1.4)
    - dependes on ccal library rev. 2.0 (was 1.1)
  - functional changes
    - AEC: dynamic coldstart (SEMC's CR 358531)
    - AEC: makernotes generation
    - AEC: timed convergence model
    - AWB: improved accuracy
    - AWB: support for temporal subsampling
    - AWB: timed convergence model
  - bug fixes
    - AEC: flickering occurring on abroupt light level changes
    - AEC: quantized expsoure time sometimes exceeding the maximum allowed value
    - AEC: constrained exposure not working in case overexposure is requested
    - AFC: transition from free-running to auto-locking modes for continuous autofocus not working
    - ART: low image quality resulting from the "automatic saturation" functionality
  - optimizations
  - API changes
    - types
      - "ste3a_timestamp" type added
      - "ste3a_inv_exposure" type added
      - "ste3a_kelvin_constraint_method" enumeration added
      - "ste3a_revision" structure added
      - "ste3a_aec_tuning" structure
        - "coldstart" field changed to "default_coldstart"
        - "damper_evolution_factor" field changed to "damper_decay_time"
        - following fields have been added:
          - "dcs_pcount"
          - "dcs_plightlevel"
          - "dcs_pinvexposure"
          - "fast_coldstart_enable"
          - "fcs_max_igain"
          - "fcs_max_ngain"
          - "fcs_min_etime"
          - "fcs_mid_etime"
          - "fcs_max_etime"
          - "fcs_max_again"
          - "fcs_max_dgain"
          - "fcs_damper_max_ratio"
          - "fcs_damper_decay_time"
          - "fcs_damper_convergence_factor"
      - "ste3a_awb_tuning" structure
        - "damping_factor" field changed to "decay_time"
        - "mired_damping_factor" field changed to "mired_decay_time"
        - following fields have been added:
          - "computation_period"
          - "kelvin_constraint_method"
          - "fcs_decay_time"
          - "fcs_damping_max_ratio"
          - "fcs_stability_thr"
          - "fcs_mired_decay_time"
          - "fcs_computation_period"
          - "fcs_subsampling_factor"
          - "fast_coldstart_enable"
        - "ste3a_calibration_data" structure
          - "valid" field added
   - functions
      - "ste3a_aec_compute_coldstart_exposure" function added
      - "ste3a_aec_reset_coldstart_exposure" function added
      - "current_time" parameter added to the following functions:
        - "ste3a_aec_update_noflash"
        - "ste3a_aec_evaluate_noflash"
        - "ste3a_aec_update_flash"
        - "ste3a_aec_evaluate_flash"
        - "ste3a_awb_update"
        - "ste3a_awb_evaluate"
- STE3A_PPAL_8500V2
  - bug fixes
    - gamma values lower than 1 / 2.2 cause image quality problems
- STE3A test application
  - absorbed API changes of other libraries
- STE3A playback application
  - absorbed API changes of other libraries


# Release notes v 1.9.110907

- STE3A library
  - functional changes
    - AWB: spatial significance weighting
    - AWB: better handling of flash cases
    - AEC: added support for sensor-mode-specific sensitivity
  - bug fixes:
    - AWB: illuminant likelihood was evaluated twice
    - AWB: fixed colout temperature computation for discrete illuminants
    - AWB: better handling of numerical precision (tiny numbers are now avoided)
    - AWB: fixed pdf dependency on sky surface
    - AWB: fixed relative weighting of discrete and blackbody illuminants
  - optimizations
    - AWB: speed optimizations
  - API changes
    - types
      - struct ste3a_aec_tuning structure
        - baseline_sensitivity field subsituted by vv_sensitivity field
        - si_sensitivity field added
- STE3A playback application
  - removed support for legacy invocation mode


# Release notes v 1.8.110722

- STE3A library
  - functional changes
    - AFC: support for "early stop" functionality
    - AWB: improved support for flash use cases (automatic "promotion" of the flash-specific illuminant)
  - optimizations
    - AWB: minor optimizations in critical functions (algorithm not changed)
  - API changes
    - types
      - ste3a_afc_tuning structure
        - tuning parameters for the "early stop" functionality added


# Release notes v 1.7.110715

- STE3A library
  - functional changes
    - AEC: support for specific power and duration ranges for the pre-flashes and the main flash
  - API changes
    - functions
      - ste3a_aec_evaluate_flash function
        - "flash_mode" parameter added
    - types
      - ste3a_aec_tuning structure
        - tuning parameters for flash duration and power added
      - "ste3a_aec_flash_mode" enumeration added
- STE3A test application
  - absorbed API changes of other libraries
  - added dedicated AEC invocation for the main-flash
- STE3A playback application
  - absorbed API changes of other libraries


# Release notes v 1.6.110711

- STE3A library
  - functional changes
    - AEC: contrast strecth algorithm reworked, now called "black stretch"
    - AEC: fade to black functionality
    - AFC: option for restarting CAF on the free-running to auto-lock modes transition
    - AFC: automatic absorption of lens actuator calibration data
    - AFC: cubic interpolation (not for local evaluation)
    - AWB: speed optimizations
    - AWB: hold and modify functionality
    - AWB: temporal damping for colour temperature estimate
    - AWB: colour temperatire range tuning moved from subject presets to light presets
  - bug fixes:
    - AWB: colour temperature returned in case of fluorescent illuminants
    - AEC: maximum exposure time when antiflickering is enabled
    - "sprintf"s substituted with "snprintf"s for compliance with Coverity
  - API changes
    - types
      - ste3a_aec_tuning structure
        - contrast stretch fields substituted with black stretch ones
        - fade to black fields added
      - ste3a_afc_tuning structure
        - "force_restart_on_caf_mode_change" field added
        - "interpolation" field added
      - ste3a_awb_tuning structure
        - "min_covered_area_percent" field added
        - "mired_damping_factor" field added
      - ste3a_afc_callbacks structure
        - "ptr_cmal_set_LA_model" field added
      - ste3a_calibration_data
        - "lens_actuator" fields added
      - ste3acb_cmal_set_LA_model callback type added
      - ste3a_exposure_priority_mode enumerated values renamed
      - ste3a_damper_bases structure
        - "analog_gain" field renamed as "post_ag_gain"
- STE3A_CMAL_Reference
  - functional changes
    - updated lens actuator model initialization (supports automated initialization from the STE3A library)
  - API changes
    - functions
      - "cmal_ref_set_LA_model" removed
      - "ste3a_cmal_ref_set_LA_model" added (callback)
- STE3A test application
  - absorbed API changes of other libraries
- STE3A playback application
  - absorbed API changes of other libraries


# Release notes v 1.5.110523

- STE3A library
  - functional changes
    - AEC: dynamic weighting
    - AEC: ISO sensitivity computation
    - AFC: pausing/restarting support for continuous autofocus
    - AFC: auto-locking mode for continuous autofocus
    - AFC: subject distance computation
    - AFC: maximum numer of steps in the full search phase increased to 64
    - AFC: thresholds for restarting the continuous mode exposed in the tuning structure
    - AFC: support for multiple locked zones
    - ART: cubic interpolation for the static tone mapping curve
    - AWB: full support for calibration data
  - bug fixes
    - AEC: exposure time computation with antiflickering enabled in low light conditions
  - API changes
    - functions
      - ste3a_aec_dynamic_weighting_start added
      - ste3a_aec_dynamic_weighting_update added
      - ste3a_aec_dynamic_weighting_stop added
      - ste3a_afc_change_mode added
      - scene_info parameter added to the ste3a_afc_evaluate function
    - types
      - dynamic_weighting_weight field added to the ste3a_aec_tuning structure
      - baseline_sensitivity field added to the ste3a_aec_tuning structure
      - sensitivity field added to the ste3a_aec_scene_info structure
      - ste3a_aec_dynamic_weighting_zone structure added
      - ste3a_aec_dynamic_weighting_geometry structure added
      - ste3a_afc_mode_continuous option in the ste3a_afc_mode enumeration renamed
        as ste3a_afc_mode_continuous_free_running
      - ste3a_afc_mode_continuous_auto_locking option added to the ste3a_afc_mode enumeration
      - ste3a_afc_state enumeration reworked
      - size of the search_pdisplacement array in the ste3a_afc_tuning structure changed from 32 to 64 elements
      - tracking_*_threshold fields added to the ste3a_afc_tuning structure
      - dispdist_* fields added to the ste3a_afc_tuning structure
      - ste3a_afc_scene_info structure added
      - ste3a_distance type added
      - ste3a_afc_intermediates structure reworked
- STE3A test application
  - absorbed API changes of other libraries
- STE3A playback application
  - absorbed API changes of other libraries


# Release notes v 1.4.110325

- nmxAawb library
  - added support for compiling .c files using the C++ compiler (because of integration issues)
- STE3A library
  - functional changes
    - adaptation to calibration data (AWB, new camera characterization binary file needed)
    - locks (AEC, AWB)
    - exposure recovery gain limitation (AEC)
    - colour matrix damping (ART)
    - static tone mapping tunability (ART)
    - support for flash calibration (AWB, preliminary)
    - new dampers bases (AEC)
  - bug fixes
    - unused AFC tuning parameters initialization issue fixed
    - default AWB convergence speed issue fixed
  - API changes
    - functions
      - ste3a_aec_lock function added
      - ste3a_aec_unlock function added
      - ste3a_awb_lock function added
      - ste3a_awb_unlock function added
      - calibration_data parameter added to the ste3a_open function
      - damper_bases parameter added to the ste3a_art_evaluate_static_tonemapping function
      - damper_bases parameter added to the ste3a_art_evaluate_dynamic_tonemapping function
    - types
      - ste3a_normalized_snr type added
      - ste3a_normalized_correlation type added
      - ste3a_calibration_data structure added
      - ste3a_damper_bases structure reworked
      - er_maximum_gain field added to the ste3a_aec_tuning structure
      - tracking_xcorr_threshold field added to the ste3a_afc_tuning structure
      - as_* and st* fields added to the ste3a_art_tuning structure
      - calibrated_central_chromaticity added to the ste3a_awb_scene_info structure
- STE3A_PPAL_8500V2
  - functional changes
    - hue-preserving saturation for channel gains and colour matrix coefficients
  - bug fixes
    - gamma tables computation now implements the standard gamma coding function (linear section for dark tones)
- STE3A test application
  - absorbed API changes of other libraries
- STE3A playback application
  - absorbed API changes of other libraries


# Release notes v 1.3.101220

- nmxAawb library
  - added: former nmx subset moved to the STE3A side for IP protection issues
- STE3A library
  - absorbed the nmxAawb library creation
- STE3A test application
  - absorbed the nmxAawb library creation
- STE3A playback application
  - absorbed the nmxAawb library creation


# Release notes v 1.2.101203

- STE3A library
  - functional changes
    - continuous autofocus
    - lens driving model updated to support sensorless piezoelectric actuators
    - sharpness statistics noise model refined
  - API changes
    - enums
      - ste3a_afc_actuator_type removed
      - ste3a_afc_algorithm removed
      - ste3a_afc_state updated
      - ste3a_lens_action removed
      - ste3a_lens_sequence_reference added
    - functions
      - ste3a_afc_loose renamed as ste3a_afc_lose
      - ste3a_afc_update
        - scene_status parameter added
    - types
      - ste3a_afc_tuning structure reworked
      - ste3a_scene_status structure
        - level field renamed as light_level
      - ste3a_cammod_focus_config structure reworked
- STE3A_PPAL_8500V2
  - bug fixes
    - ste3a_ppal_8500V2_compute_cg_cm function returns wrong offset values (first signaled on 1.1.101110
- documentation
  - u8500_3a_algorithms_tuning.doc updated to revision 0.4, aligned to STE3A 1.2
- STE3A test application
  - continuous autofocus test added
  - absorbed API changes of other libraries
- STE3A playback application
  - absorbed API changes of other libraries


# Release notes v 1.1.101110

- STE3A library
  - functional changes
    - nmx library revision now internally checked (at ste3a_open and ate3a_openback times)
    - characterization data buffer size now internally checked (at ste3a_open and ate3a_openback times)
    - debug API isolated from standard API (for IP protection)
    - AWB: algorithm improvements for better maximum accuracy
    - AWB: provisions for computational complexity vs results accuracy tuning
  - API changes
    - enums
      - ste3a_computation_mode added
    - types
      - ste3a_aec_intermediates moved to debug API
      - ste3a_afc_intermediates moved to debug API
      - ste3a_art_intermediates moved to debug API
      - ste3a_awb_intermediates moved to debug API
      - ste3a_awb_tuning modified
        - damping_max_ratio field added
        - subsampling_factor field added
        - computation_mode field added
        - sensor_log_tilts field renamed as sensor_tilts
      - ste3a_awb_scene_info modified
        - surface_scores field removed
    - functions
      - ste3a_openback moved to debug API
      - ste3a_playback moved to debug API
      - ste3a_aec_get_intermediates moved to debug API
      - ste3a_afc_get_intermediates moved to debug API
      - ste3a_art_get_intermediates moved to debug API
      - ste3a_awb_get_intermediates moved to debug API
- STE3A test application
  - absorbed API changes of other libraries
- STE3A playback application
  - absorbed API changes of other libraries


# Release notes v 1.0.101029

- STE3A library
  - functional changes
    - AEC: stability improved in severe clipping conditions
    - AEC: improved playback decoding
    - AFC: local evaluation and depth priority modes
    - AFC: improved checks on the obtained focus (failure can be actually reported now)
    - AFC: improved playback decoding
    - ART: improved playback decoding
    - AWB: improved playback decoding
  - bugfixes
    - white balance gains can be not normalized (first signaled on 0.9.101001)
  - API changes
    - enums
      - ste3a_depth_priority added
    - types
      - ste3a_afc_tuning modified
        - local evaluation functionality and depth priority parameters added
      - ste3a_afc_intermediates
        - local evaluation functionality and depth priority values added
    - functions
      - ste3a_afc_get_locked_zones added
- STE3A_CMAL_Reference
  - functional changes
    - SMIA analog gain quantization model
  - API changes
    - functions
      - cmal_ref_set_ET_* functions collapsed in the cmal_ref_set_ET_model(ste3a_time min, ste3a_time max, ste3a_time base, ste3a_time step) one
      - cmal_ref_set_AG_* functions collapsed in the cmal_ref_set_AG_model(ste3a_gain min, ste3a_gain max, ste3a_gain base, ste3a_gain step) one
      - cmal_ref_set_LA_* functions collapsed in the cmal_ref_set_LA_model(ste3a_logical_lens_displacement mec_inf, ste3a_logical_lens_displacement nom_inf, ste3a_logical_lens_displacement nom_mac, ste3a_logical_lens_displacement mec_mac) one
      - cmal_ref_set_AG_model(ste3a_float num_pre_mul, ste3a_float num_post_add, ste3a_float den_pre_mul, ste3a_float den_post_add, ste3a_index min_code, ste3a_index max_code) added
- STE3A_PPAL_8500V2 library
  - bugfixes
    - incorrect gamma tables returned by the ste3a_ppal_8500V2_compute_gc function (first signaled on 0.9.101001)
- STE3A test application
  - absorbed API changes of other libraries
  - SMIA analog gain quantization model used
- STE3A playback application
  - absorbed API changes of other libraries
  - SMIA analog gain quantization model used


# Release notes v 0.9.101001

- STE3A library
  - dependencies
    - nmx 0.4 needed
  - functional changes
    - channel-specific gamma factors now managed (in order to ease special effects implementation)
    - ART added for video and viewfinder modes
  - bugfixes
    - AEC: cannot reach convergence when EV values higher than 2 are programmed (first signaled on 0.8.100917)
  - API changes
    - enums
      - ste3a_color_rendition_intent enumeration added
    - types
      - ste3a_art_tuning modified
        - color rendition intent field added
        - channel-specific gamma factor multipliers added
      - ste3a_isp_gc_config added
        - channel-specific gamma factor multipliers
    - functions
      - ste3a_art_get_gamma_coding added
      - ste3a_art_evaluate replaced by
        - ste3a_art_evaluate_static_tonemapping (for video and viewfinder modes)
        - ste3a_art_evaluate_dynamic_tonemapping (for still picture mode)
- STE3A_PPAL_8500V2 library
  - functional changes
    - channel-specific gamma factors now managed (channel-specific gamma tables returned)
  - API changes
    - types
      - ste3a_ppal_8500V2_gc_config modified
        - channel-specific gamma tables
    - functions
      - ste3a_ppal_8500V2_compute_gc modified
        - channel-specific gamma factor multipliers added in the prototype
- STE3A test application
  - absorbed API changes of other libraries
- documentation
  - "STE3A to 8500V2 dataflow.ppt" presentation
    - updated


# Release notes v 0.8.100917

- STE3A library
  - functional changes
    - AEC: ste3a_compute_stats_geometry_focus now really implemented
    - AEC: light metering functionality now not depending on AWB gains any more
    - AEC: adaptive-compensation functionality added
    - AWB: camera characterization data not exposed any more by the tuning interface
    - AWB: stability reporting now really implemented
    - AWB: default value for the temporal_smoothing tuning parameter changed from 0.0 to 0.5
    - AWB: default value for the scenes_light_level_thr_pair[0] tuning parameter changed from 3000.0 to 300.0
    - AWB: default value for the scenes_light_level_thr_pair[1] tuning parameter changed from <uninitialized> to 3000.0
  - bugfixes
    - AEC: spurious memory access solved in the steaec_update_noflash function (first signaled on 0.7.100705)
    - AEC: spurious memory access solved in the steaec_update_flash function (first signaled on 0.7.100705)
    - AEC: spurious memory access solved in the steaec_evaluate_noflash function (first signaled on 0.7.100705)
    - AEC: spurious memory access solved in the steaec_evaluate_flash function (first signaled on 0.7.100705)
    - AFC: locked state can now be reached (first signaled on 0.7.100705)
    - AWB: uninitialized tuning parameter (scenes_light_level_thr_pair[1]) fixed (first signaled on 0.7.100705)
  - API changes
    - types
      - 'bool' type not used any more, replaced by the ste3a_bool class
        - automatic conversion between bool and ste3a_bool (and vice-versa) provided
    - functions
      - bool replaced by ste3a_bool in prototypes
    - structures
      - ste3a_aec_tuning
        - light metering parameters added/renamed
          - IQ tuning frameowrk offsets changed !
        - anti-clipping parameters renamed
        - contrast-stretch parameters renamed
        - adaptive-compensation parameters added
      - ste3a_afc_tuning
        - use case specifc step sizes added
        - statistics zones geometry parameters added
      - ste3a_awb_tuning
        - content revised for removing camera characterization data
          - IQ tuning frameowrk offsets changed !
        - temporal_smoothing field renamed as damping_factor
        - stability_thr field added
- STE3A_CMAL_Reference
  - API changes
    - functions
      - bool replaced by ste3a_bool in prototypes
- STE3A_FGAL_Reference
  - API changes
    - functions
      - bool replaced by ste3a_bool in prototypes
- STE3A_PPAL_Reference
  - API changes
    - functions
      - bool replaced by ste3a_bool in prototypes
- STE3A test application
  - absorbed API changes of other libraries
  - AWB test updated for handling the stability flag
  - AFC test added
  - MS Windows executable added to delivered files
- STE3A playback application
  - absorbed API changes of other libraries
  - MS Windows executable added to delivered files
- documentation
  - "STE3A to 8500V2 dataflow.ppt" presentation
    - updated
  - "u8500_3a_algorithms_tuning.doc" document
    - added


# Release notes v 0.7.100705

- STE3A library
  - functional changes
    - AWB/AEC: ISP virtualized
    - AEC: anticlipping
    - AEC: contrast stretch
    - AEC: damper bases computation
    - AWB: algorithm refinements
    - ART: reworked
      - for handling tone mapping only
      - colour matrix and contrast stretch application now in charge of PPAL
    - AFC: usage of CMAL services for lens driver abstraction, HW units now used in the API
  - API changes
    - functions
      - ste3a_aec_evaluate_noflash
        - updated according to functional changes
      - ste3a_aec_evaluate_flash
        - updated according to functional changes
      - ste3a_aec_evaluate_still
        - updated according to functional changes
      - ste3a_awb_evaluate
        - updated according to functional changes
      - ste3a_art_evaluate_video
        - removed
      - ste3a_art_get_static_tone_mapping
        - added, according to functional changes
      - ste3a_art_evaluate_still
        - renamed as ste3a_art_evaluate
        - updated according to functional changes
      - ste3a_afc_follow
        - HW units
    - definition of external functions
      - ste3acb_cmal_convert_LA_ln
        - added (logical to normalized units conversion)
      - ste3acb_cmal_convert_LA_nl
        - added (normalized to logical units conversion)
      - ste3acb_cmal_range_LA_ninf
        - added (calibration data handling, infinity position retrieval)
      - ste3acb_cmal_range_LA_nmac
        - added (calibration data handling, macro position retrieval)
    - types
      - ste3a_pixelaverage
        - renamed as ste3a_normalized_pixel_level
      - ste3a_logical_lens_displacement
        - added
    - structures
      - ste3a_aec_tuning
        - exposure target now expressed in normalized levels
        - added anticlipping and contrast strecth parameters
      - ste3a_aec_intermediates
        - changed due to anticlipping
      - ste3a_aec_callbacks
        - ptr_ppal_closest_DG removed
      - ste3a_afc_callbacks
        - added
      - ste3a_awb_gains
        - removed
      - ste3a_awb_results
        - removed
      - ste3a_contrast_stretch
        - removed
      - ste3a_damper_bases
        - added
      - ste3a_awb_scene_info
        - added
      - ste3a_isp_dg_config
        - added (for exposure digital gain)
      - ste3a_isp_cm_config
        - offsets removed (now mapped in the ste3a_isp_cs_config block)
      - ste3a_isp_er_config
        - added (for exposure recovery, due to anticlipping)
      - ste3a_isp_cs_config
        - added (for contrast stretch)
      - ste3a_cammod_focus_config
        - updated for carrying HW units
- STE3A_CMAL_Reference
  - functional changes
    - conversion between HW and logical units for lens actuator, including adaptation to calibration data
  - API changes
    - functions
      - ste3a_cmal_ref_convert_LA_ln
        - added
      - ste3a_cmal_ref_convert_LA_nl
        - added
      - ste3a_cmal_ref_range_LA_ninf
        - added
      - ste3a_cmal_ref_range_LA_nmac
        - added
      - cmal_ref_set_LA_mec_inf
        - added
      - cmal_ref_set_LA_nom_inf
        - added
      - cmal_ref_set_LA_mec_mac
        - added
      - cmal_ref_set_LA_nom_mac
        - added
- STE3A_PPAL_8500V2
  - functional changes
    - digital gains quantization not handled any more
  - API changes
    - structures
      - ste3a_ppal_8500V2_cg_config
        - added
      - ste3a_ppal_8500V2_cm_config
        - added
      - ste3a_ppal_8500V2_gc_config
        - added
    - functions
      - ste3a_ppal_8500V2_closest_DG
        - removed
      - ste3a_ppal_8500V2_compute_cg_cm
        - added
      - ste3a_ppal_8500V2_compute_gc
        - added
- STE3A test application
  - absorbed changes ofother libraries
  - AWB driven during AEC test sequence (for demonstrating the relative dependencies)
- STE3A playback application
  - absorbed changes ofother libraries
- documentation
  - "STE3A to 8500V2 dataflow.ppt" presentation
    - added
  - sequence diagrams
    - Startup, then VF fixed focus
      - updated
    - VF fixed focus, then half press, then AF without assistant
      - updated


# Release notes v 0.6.100604

- STE3A library
  - functional changes
    - AEC, zoned weighting added
    - AEC, exposure refinement for still image added
    - AWB, temporal damping added
    - ART (Automatic Rendition Tuning) functionality added, including
      - automatic contrast stretch (for video and viewfinder)
      - automatic tone mapping (for still image)
  - API changes
    - ste3a_awb_config structure renamed to ste3a_awb_results
    - ste3a_acs_config structure renamed to ste3a_contrast_stretch
    - ste3a_aec_scene_config structure renamed to ste3a_aec_scene_info
    - ste3a_aec_evaluate_still function added
    - most of ste3a_awb_tuning structure's fields renamed
    - most of ste3a_awb_intermediates structure's fields renamed
    - weighting parameters added to the ste3a_aec_tuning structure
- STE3A test application
  - updated according to API changes of the various libraries


# Release notes v 0.5.100514

- STE3A library
  - functional changes
    - added support for binary tuning data at library instantiation time
    - library instantiation included in debug dumps (including binary tuning data)
    - sequence between AWB (first) and AEC (second) evaluate functions enforced
    - support for contract stretch computation (AEC, not yet implemented)
    - support for colour matrix adaptation (AWB, not yet implemented)
    - support for scene detection (AWB + AEC)
    - pointer to called-side context added to callbacks
    - support for gain priorities added to AEC (for implementing soft and hard ISO presets)
    - dump of intermediate data, through dedicated callback
    - basic (fixed power) flash management implemented in AEC
    - improved metering algorithm (WB weighted) in AEC
    - direct control of ISP digital gains (full 4-channels, WB gains routed through AEC)
    - better support for scene mode tuning in AWB
    - backward compatibility with binary dumps from previous version (0.4.100503) maintained (not for AWB)
  - bugfixes
    - function identification in debug dumps now working as expected
  - API changes
    - functions
      - ste3a_open (modified)
      - stera_openback (added)
      - ste3a_aec_evaluate_noflash (modified)
      - ste3a_aec_evaluate_flash (modified)
      - ste3a_awb_evaluate (modified)
    - types
      - ste3a_awb_gains (added)
      - ste3a_awb_color_matrix (added)
      - ste3a_awb_config (added)
      - ste3a_awb_tuning (modified)
      - ste3a_awb_intermediates (modified)
      - ste3a_isp_cg_config (renamed, was ste3a_isp_config)
      - ste3a_acs_config (added)
      - ste3a_aec_scene_config (added)
      - ste3a_aec_tuning (modified)
      - ste3a_mem_callbacks (modified)
      - ste3a_dbg_callbacks (modified)
      - ste3a_inf_callbacks (modified)
      - ste3a_aec_callbacks (modified)
- STE3A hardware abstraction layers
  - CMAL
    - called-side context implemented
    - min/max/base/step model (NIPS compatible) implemented for ET and AG
  - FGAL
    - called-side context implemented
  - PPAL
    - called-side context implemented
- STE3A playback application
  - updated according to API changes of the various libraries
  - ste3a_open now played back (older dumps still supported)
  - support for intermediate data dumping
- STE3A test application
  - updated according to API changes of the various libraries
  - flash use case tested for AEC


# Release notes v 0.4.100503

- STE3A library
  - improved AWB tuning interface
  - callbacks typedefs updated
  - debug dump/playback capability added for AWB and AFC
  - AEC
    - fixed a bug on requested exposure computation (visible as overexposure during integration tests)
    - better support for the "update before first execute" use case
    - traces added on the debug dumps playback
- STE3A hardware abstraction layers
  - CMAL reference implementation added
  - FGAL reference implementation added
  - PPAL 8500V2 implementation added
- STE3A playback application
  - added
- STE3A test application
  - callbacks implementation removed (CMAL, FGAL, PPAL implementations used)
  - improved test for first convergence
  - AWB dump/playback test added
  - AWB test data updated


# Release notes v 0.4.100426

- STE3A library
  - file headers updated to be compliant with ST-Ericsson policy
  - compilation warning associated to floating point constants removed
  - API access record/playback capabilities added
    - currently implemented for AEC calls only
  - enhanced portability
    - enums forced to be 32 bits wide
  - AEC functionality
    - antiflickering added
    - tuning structure updated
    - bugfixes
      - overflow of input statistics on "update" functions
      - others
  - AWB functionality
    - bugfixes
- STE3A test application
  - build field no more tested on libraries version check
  - statistics dimensions #defined
  - AEC testing functionality
    - recored/playback implemented
    - quantization modeled in CMAL callbacks
  - AWC testing functionality
    - Matlab model complinacy test added


# Release notes v 0.3.0

- full AWB API
- first AWB implementation
- new types added
- test application runs AWB
- some warnings cleared in release builds
- revision levels names revised
- test app updated for nmx versioning demonstration


# Release notes v 0.2.5 (delta vs release 0.2)

- versioning API
- typo error corrected in ste3a_acquisition_mode declaration
- comments added to type and enum declarations
- trace / API record / makernotes callbacks added
- focus stats descriptor updated


# Release notes v 0.2 (delta vs release 0.1)

- full AFC API
- basic AFC implementation
  - mono-zone/flat-weighting only
  - full-search only

Known bugs
- AEC statistics not correctly buffered (pointer stored, data not buffered)


# Release notes v 0.1

- library initialization/finalization
- full AEC API
- basic AEC implementation
  - histogram-based closed-loop metering
  - grid statistics not used
  - exposure compiler
    - iris>filter>time>again>dgain priority only
    - no internal antiflickering
  - flash not supported
  - tuning entry point (with current set of parameters)
  - inspection entry point (with current set of parameters)
  - camera module abstraction layer callbacks
  - ISP abstraction layer callbacks
  - flashgun device abstraction layer callbacks not implemented
