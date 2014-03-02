/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 * \image html STE_jpeg_RGB.jpg
 *
 * \mainpage ALGORITHM LIBRARY 
 *
 * \section intro_sec Introduction
 * Algorithm Library provides the interfaces of Metrices for the validation of the different algorithms
 * in platform independent way.
 *
 *
 * \section supported_sec Supported Effects
 *
 * \subsection effect1     1: Negative:  Tested
 * 
 * \subsection effect2     2: Black&White:  Tested
 *
 * \subsection effect3     3: Sepia:  Tested
 *
 * \subsection effect4     4: AutoFocus:  Tested and Characterization completed
 *
 * \subsection effect5     5: WhiteBalance:  Tested and Characterization is ongoing
 *
 * \subsection effect6     Note1: Bug fixes for Negative and Black&White.
 *
 * \subsection effect7     Note2: Error Tolerance added for each channel.
 *
 * \subsection effect8     Note3: extern "C" added for C++ compilation.
 *
 * \subsection effect9     Note4: Channel Average division Bug Fix.
 *
 * \subsection effect10    Note5: Added Interfaces to test from files and buffers. 
 *
 * \subsection effect11    Note6: "ValidationMetricInterfaceWithFilesBMP" can be used to test the effects directly from BMP files.
 *
 * \subsection effect12    Note7: Seperate files are added for unit test cases for different effects for clarity.
 *
 * \subsection effect13    Note8: File Routines wrappers and debug wrappers are included.
 *
 * \subsection effect14    Note9: Sepia Metrice implementation through Hue included.
 *
 * \subsection effect15    Note10: Auto Focus is implemented through luminance and Area Selection is inlcuded in the metric.
 *
 * \subsection effect16    Note11: Tolerance is added in the White Balance Metric.
 *
 * \subsection effect17    Note12: Interface is provided to include directory name of test images while running the tests.
 *
 * \subsection effect18    Note13: More Unit Tests are included in Auto Focus and White Balance.
 *
 * \subsection effect19    Note14: Auto Focus characterization is completed.
 *
 * \subsection effect20    Note15: White Balance characterization is ongoing.
 *
 * \subsection effect21    Note16: String functions support is included in the library.
 *
 * \subsection effect22    Note17: Focus Tolerance and Focus Threshold are included in AutoFocus Metric.
 *
 * \subsection effect23    Note18: Spatial Metric is added in the Library and used to Test Auto Focus images for Spatial similarity before applying Auto Focus Matric.
 *
 * \subsection effect24    Note19: test_metric_autofocus_line and test_metric_autofocus_colon functions are included in Auto Focus Unit Tests.
 *
 * \subsection effect25    Note20: Two new interfaces are added for testing, (a)"ValidationMetricInterfaceWithFiles" with an extra TAlgoImageParams parameter. (b)"ValidationMetricInterfaceWithFilesV2" - simplified version of "ValidationMetricInterfaceWithFiles".
 *
 * \subsection effect26    Note21: Color Tone Metric implemented.
 *
 * \subsection effect27    Note22: Emboss Metric implemented.
 *
 * \subsection effect28    Note23: New implementation of File Function Algo_fcloseall() and ALGO_fclose included.
 *
 * \subsection effect29    Note24: Test Interface cleaned up. Now "ValidationMetricInterfaceWithFiles" will be used for testing.
 *
 * \subsection effect30    Note25: Unit tset cases are cleaned up and organized as concluded after last release.
 *
 * \subsection effect31    Note26: All directory Size arrays are set to the power of 2.
 *
 * \subsection effect32    Note27: Metrcis_Print_Result() is used for testing the result of metrics.
 *
 * \subsection effect33    Note28: UTC's and characterization is added for Spatial Similar metric.
 *
 * \subsection effect34    Note29: UTC's are added for Emboss metric, characterization in progress.
 *
 *
 * \section interface_sec Interface
 * algointerface.h provides the API "ValidationMetricInterfaceWithFiles" which is used for the verification of the images.
 * 
 */
