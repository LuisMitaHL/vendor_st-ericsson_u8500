/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \defgroup  InterruptHandler Interrupt Handlers

 \details   This module implements all the top level interrupt service routines.
            The initialisation of all the top level interrupt service routines is
            also performed by this module.
*/
#ifndef INTERRUPTMANAGER_H_
#   define INTERRUPTMANAGER_H_

#   include "HostComms_OPInterface.h"
#   include "MasterI2C_op_interface.h"
#   include "awb_statistics_op_interface.h"
#   include "exposure_statistics_op_interface.h"
#   include "SystemConfig.h"
#   include "Exposure_OPInterface.h"

/**
 \if INCLUDE_IN_HTML_ONLY
 \file      InterruptManager.h

 \brief     This file exposes interfaces to allow other modules to use the Interrupt Handlers
            and their associated functions.

 \details   It maintains the following information about all the interrupts that are mapped into
            the STXP70 interrupt handler.
            - IRQ number: The IRQ number of the STXP70 core Interrupt handler to which an
            individual interrupt is mapped.
            - Interrupt Level: The priority of the corresponding interrupt.
            - Interrupt Synchronisation.
            - Initial enable/disable control for the corresponding interrupt.
            - Initial trigger control for the interrupt.

 \ingroup   InterruptHandler
 \endif
*/
#   include "stxp70_itc.h"
#   include "Platform.h"

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// 00: Host Comms, USER I/F
// pictor_fs.pdf, P93, Table 8
// Host Comms Interrupt
/// IRQ number
#   define IT_NO_USER_IF       (0)        // Interrupt number

    /// Interrupt priority level
#   define IT_LEVEL_USER_IF    (10)       // Interrupt Level (0 = lowest priority, 15 = highest priority)

    /// Interrupt synchronisation
#   define IT_SYNCH_USER_IF    (1)        // Interrupt synchronisation

    /// Initial enable/disable control
#   define IT_ENABLE_USER_IF   (1)        // Initial ENABLE/DISABLE conntrol for interrupt (1 = ENABLE, 0 = DISABLE)

    /// Initial interrupt trigger control
#   define IT_TRIGGER_USER_IF  (0)        // Initial trigger control. Normally used for software trigger. Set to 0 normally


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


// 01: Glace Schdule Interrupt
/// IRQ number
#   define IT_NO_GLACE_SCHEDULE       (1)        // Interrupt number

    /// Interrupt priority level
#   define IT_LEVEL_GLACE_SCHEDULE    (1)       // Interrupt Level (0 = lowest priority, 15 = highest priority)

    /// Interrupt synchronisation
#   define IT_SYNCH_GLACE_SCHEDULE    (1)        // Interrupt synchronisation

    /// Initial enable/disable control
#   define IT_ENABLE_GLACE_SCHEDULE   (1)        // Initial ENABLE/DISABLE conntrol for interrupt (1 = ENABLE, 0 = DISABLE)

    /// Initial interrupt trigger control
#   define IT_TRIGGER_GLACE_SCHEDULE  (0)        // Initial trigger control. Normally used for software trigger. Set to 0 normally


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


//  02: Master CCI Interrupt Line
// pictor_fs.pdf, P93, Table 8

/// IRQ number
#   define IT_NO_MASTER_CCI        (2)    // Interrupt number

    /// Interrupt priority level
#   define IT_LEVEL_MASTER_CCI     (3)    // Interrupt Level (0 = lowest priority, 15 = highest priority)

    /// Interrupt synchronisation
#   define IT_SYNCH_MASTER_CCI     (1)    // Interrupt synchronisation

    /// Initial enable/disable control
#   define IT_ENABLE_MASTER_CCI    (1)    // Initial ENABLE/DISABLE conntrol for interrupt (1 = ENABLE, 0 = DISABLE)

    /// Initial interrupt trigger control
#   define IT_TRIGGER_MASTER_CCI   (0)    // Initial trigger control. Normally used for software trigger. Set to 0 normally


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// 03: Line blanking elimination
// pictor_fs.pdf, Interrupt interface

/// IRQ number
#   define IT_NO_LBE        (3)    // Interrupt number

    /// Interrupt priority level
#   define IT_LEVEL_LBE      (2)    // Interrupt Level (0 = lowest priority, 15 = highest priority)

    /// Interrupt synchronisation
#   define IT_SYNCH_LBE      (1)    // Interrupt synchronisation

    /// Initial enable/disable control
#   define IT_ENABLE_LBE     (1)    // Initial ENABLE/DISABLE conntrol for interrupt (1 = ENABLE, 0 = DISABLE)

    /// Initial interrupt trigger control
#   define IT_TRIGGER_LBE    (0)    // Initial trigger control. Normally used for software trigger. Set to 0 normally


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// 04: RE Pipe Complete
// pictorv2_fs_v1.1.pdf, Interrupt interface

/// IRQ number
#   define IT_NO_REPIPE         (4)    // Interrupt number

    /// Interrupt priority level
#   define IT_LEVEL_REPIPE      (2)    // Interrupt Level (0 = lowest priority, 15 = highest priority)

    /// Interrupt synchronisation
#   define IT_SYNCH_REPIPE      (1)    // Interrupt synchronisation

    /// Initial enable/disable control
#   define IT_ENABLE_REPIPE     (1)    // Initial ENABLE/DISABLE conntrol for interrupt (1 = ENABLE, 0 = DISABLE)

    /// Initial interrupt trigger control
#   define IT_TRIGGER_REPIPE    (0)    // Initial trigger control. Normally used for software trigger. Set to 0 normally


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// 05: SMIA Rx
// pictor_fs.pdf, P93, Table 8
/// IRQ number
#   define IT_NO_SMIA_RX        (5)    // Interrupt number

    /// Interrupt priority level
#   define IT_LEVEL_SMIA_RX      (2)    // Interrupt Level (0 = lowest priority, 15 = highest priority)

    /// Interrupt synchronisation
#   define IT_SYNCH_SMIA_RX      (1)   // Interrupt synchronisation

    /// Initial enable/disable control
#   define IT_ENABLE_SMIA_RX     (1)    // Initial ENABLE/DISABLE conntrol for interrupt (1 = ENABLE, 0 = DISABLE)

    /// Initial interrupt trigger control
#   define IT_TRIGGER_SMIA_RX    (0)    // Initial trigger control. Normally used for software trigger. Set to 0 normally


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// 06: PIPE0 Video complete (OPF0)
// pictor_fs.pdf, P93, Table 8
/// IRQ number
#   define IT_NO_VIDEO_PIPE0           (6)    // Interrupt number

    /// Interrupt priority level
#   define IT_LEVEL_VIDEO_PIPE0        (2)    // Interrupt Level (0 = lowest priority, 15 = highest priority)

    /// Interrupt synchronisation
#   define IT_SYNCH_VIDEO_PIPE0        (1)    // Interrupt synchronisation

    /// Initial enable/disable control
#   define IT_ENABLE_VIDEO_PIPE0       (1)    // Initial ENABLE/DISABLE conntrol for interrupt (1 = ENABLE, 0 = DISABLE)

    /// Initial interrupt trigger control
#   define IT_TRIGGER_VIDEO_PIPE0      (0)    // Initial trigger control. Normally used for software trigger. Set to 0 normally


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// 07: PIPE1 Video complete (OPF1)
// pictor_fs.pdf, P93, Table 8
/// IRQ number
#   define IT_NO_VIDEO_PIPE1           (7)    // Interrupt number

    /// Interrupt priority level
#   define IT_LEVEL_VIDEO_PIPE1        (2)    // Interrupt Level (0 = lowest priority, 15 = highest priority)

    /// Interrupt synchronisation
#   define IT_SYNCH_VIDEO_PIPE1        (1)    // Interrupt synchronisation

    /// Initial enable/disable control
#   define IT_ENABLE_VIDEO_PIPE1       (1)    // Initial ENABLE/DISABLE conntrol for interrupt (1 = ENABLE, 0 = DISABLE)

    /// Initial interrupt trigger control
#   define IT_TRIGGER_VIDEO_PIPE1      (0)    // Initial trigger control. Normally used for software trigger. Set to 0 normally



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// 08: 256-bins histogram (AEC)
// pictor_fs.pdf, STAT0
/// IRQ number
#   define IT_NO_256_BINS_HISTOGRAM           (8)    // Interrupt number

    /// Interrupt priority level
#   define IT_LEVEL_256_BINS_HISTOGRAM        (2)    // Interrupt Level (0 = lowest priority, 15 = highest priority)

    /// Interrupt synchronisation
#   define IT_SYNCH_256_BINS_HISTOGRAM        (1)    // Interrupt synchronisation

    /// Initial enable/disable control
#   define IT_ENABLE_256_BINS_HISTOGRAM       (1)    // Initial ENABLE/DISABLE conntrol for interrupt (1 = ENABLE, 0 = DISABLE)

    /// Initial interrupt trigger control
#   define IT_TRIGGER_256_BINS_HISTOGRAM      (0)    // Initial trigger control. Normally used for software trigger. Set to 0 normally


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// 09: STAT1 Acc 6x8 Exposure
// pictor_fs.pdf, STAT1

#   define IT_NO_STAT1_ACC_6x8_EXPOSURE        (9)    // Interrupt number

    /// Interrupt priority level
#   define IT_LEVEL_STAT1_ACC_6x8_EXPOSURE     (2)    // Interrupt Level (0 = lowest priority, 15 = highest priority)

    /// Interrupt synchronisation
#   define IT_SYNCH_STAT1_ACC_6x8_EXPOSURE     (1)    // Interrupt synchronisation

    /// Initial enable/disable control
#   define IT_ENABLE_STAT1_ACC_6x8_EXPOSURE    (1)    // Initial ENABLE/DISABLE conntrol for interrupt (1 = ENABLE, 0 = DISABLE)

    /// Initial interrupt trigger control
#   define IT_TRIGGER_STAT1_ACC_6x8_EXPOSURE   (0)    // Initial trigger control. Normally used for software trigger. Set to 0 normally



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// 10: Auto Focus
// pictor_fs.pdf, STAT2
/// IRQ number
#   define IT_NO_AUTO_FOCUS           (10)    // Interrupt number

    /// Interrupt priority level
#   define IT_LEVEL_AUTO_FOCUS        (2)    // Interrupt Level (0 = lowest priority, 15 = highest priority)

    /// Interrupt synchronisation
#   define IT_SYNCH_AUTO_FOCUS        (1)    // Interrupt synchronisation

    /// Initial enable/disable control
#   define IT_ENABLE_AUTO_FOCUS       (1)    // Initial ENABLE/DISABLE conntrol for interrupt (1 = ENABLE, 0 = DISABLE)

    /// Initial interrupt trigger control
#   define IT_TRIGGER_AUTO_FOCUS      (0)    // Initial trigger control. Normally used for software trigger. Set to 0 normally



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// 11: STAT3 White Balance
// pictor_fs.pdf, P92, Table 8

#   define IT_NO_STAT3_WHITE_BALACE        (11)    // Interrupt number

    /// Interrupt priority level
#   define IT_LEVEL_STAT3_WHITE_BALACE     (2)    // Interrupt Level (0 = lowest priority, 15 = highest priority)

    /// Interrupt synchronisation
#   define IT_SYNCH_STAT3_WHITE_BALACE     (1)    // Interrupt synchronisation

    /// Initial enable/disable control
#   define IT_ENABLE_STAT3_WHITE_BALACE    (1)    // Initial ENABLE/DISABLE conntrol for interrupt (1 = ENABLE, 0 = DISABLE)

    /// Initial interrupt trigger control
#   define IT_TRIGGER_STAT3_WHITE_BALACE   (0)    // Initial trigger control. Normally used for software trigger. Set to 0 normally

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// 12: Skin tone detection
// pictor_fs.pdf, STAT4
/// IRQ number
#   define IT_NO_SKIN_TONE_DETECTION           (12)    // Interrupt number

    /// Interrupt priority level
#   define IT_LEVEL_SKIN_TONE_DETECTION        (2)    // Interrupt Level (0 = lowest priority, 15 = highest priority)

    /// Interrupt synchronisation
#   define IT_SYNCH_SKIN_TONE_DETECTION        (1)    // Interrupt synchronisation

    /// Initial enable/disable control
#   define IT_ENABLE_SKIN_TONE_DETECTION       (1)    // Initial ENABLE/DISABLE conntrol for interrupt (1 = ENABLE, 0 = DISABLE)

    /// Initial interrupt trigger control
#   define IT_TRIGGER_SKIN_TONE_DETECTION      (0)    // Initial trigger control. Normally used for software trigger. Set to 0 normally


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// 13: DMCE Pipe Complete
// pictorv2_fs_v1.1.pdf, DMCEPIPE
/// IRQ number
#   define IT_NO_DMCEPIPE           (13)    // Interrupt number

    /// Interrupt priority level
#   define IT_LEVEL_DMCEPIPE        (2)    // Interrupt Level (0 = lowest priority, 15 = highest priority)

    /// Interrupt synchronisation
#   define IT_SYNCH_DMCEPIPE        (1)    // Interrupt synchronisation

    /// Initial enable/disable control
#   define IT_ENABLE_DMCEPIPE       (1)    // Initial ENABLE/DISABLE conntrol for interrupt (1 = ENABLE, 0 = DISABLE)

    /// Initial interrupt trigger control
#   define IT_TRIGGER_DMCEPIPE      (0)    // Initial trigger control. Normally used for software trigger. Set to 0 normally


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// 14: Arctic
// pictor_fs.pdf, STAT6
/// IRQ number
#   define IT_NO_ARCTIC           (14)    // Interrupt number

    /// Interrupt priority level
#   define IT_LEVEL_ARCTIC        (2)    // Interrupt Level (0 = lowest priority, 15 = highest priority)

    /// Interrupt synchronisation
#   define IT_SYNCH_ARCTIC        (1)    // Interrupt synchronisation

    /// Initial enable/disable control
#   define IT_ENABLE_ARCTIC       (1)    // Initial ENABLE/DISABLE conntrol for interrupt (1 = ENABLE, 0 = DISABLE)

    /// Initial interrupt trigger control
#   define IT_TRIGGER_ARCTIC      (0)    // Initial trigger control. Normally used for software trigger. Set to 0 normally


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// 15: Acc WG Zones
// pictor_fs.pdf, STAT7

#   define IT_NO_STAT7_ACC_WG_ZONES        (15)    // Interrupt number

    /// Interrupt priority level
#   define IT_LEVEL_STAT7_ACC_WG_ZONES     (2)    // Interrupt Level (0 = lowest priority, 15 = highest priority)

    /// Interrupt synchronisation
#   define IT_SYNCH_STAT7_ACC_WG_ZONES     (1)    // Interrupt synchronisation

    /// Initial enable/disable control
#   define IT_ENABLE_STAT7_ACC_WG_ZONES    (1)    // Initial ENABLE/DISABLE conntrol for interrupt (1 = ENABLE, 0 = DISABLE)

    /// Initial interrupt trigger control
#   define IT_TRIGGER_STAT7_ACC_WG_ZONES   (0)    // Initial trigger control. Normally used for software trigger. Set to 0 normally

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// 16: Glace(AWB)
// pictor_fs.pdf, STAT8
/// IRQ number
#   define IT_NO_STAT8_GLACE_AWB           (16)    // Interrupt number

    /// Interrupt priority level
#   define IT_LEVEL_STAT8_GLACE_AWB        (2)    // Interrupt Level (0 = lowest priority, 15 = highest priority)

    /// Interrupt synchronisation
#   define IT_SYNCH_STAT8_GLACE_AWB        (1)    // Interrupt synchronisation

    /// Initial enable/disable control
#   define IT_ENABLE_STAT8_GLACE_AWB       (1)    // Initial ENABLE/DISABLE conntrol for interrupt (1 = ENABLE, 0 = DISABLE)

    /// Initial interrupt trigger control
#   define IT_TRIGGER_STAT8_GLACE_AWB      (0)    // Initial trigger control. Normally used for software trigger. Set to 0 normally


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// 17: GPIO
// pictor_fs.pdf, GPIO
/// IRQ number
#   define IT_NO_GPIO           (17)    // Interrupt number

    /// Interrupt priority level
#   define IT_LEVEL_GPIO        (5)    // Interrupt Level (0 = lowest priority, 15 = highest priority)

    /// Interrupt synchronisation
#   define IT_SYNCH_GPIO        (1)    // Interrupt synchronisation

    /// Initial enable/disable control
#   define IT_ENABLE_GPIO       (1)    // Initial ENABLE/DISABLE conntrol for interrupt (1 = ENABLE, 0 = DISABLE)

    /// Initial interrupt trigger control
#   define IT_TRIGGER_GPIO      (0)    // Initial trigger control. Normally used for software trigger. Set to 0 normally


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// 18: Line BURSTY Elimination
// pictorv2_fs_v1.1.pdf, LINE_BURSTY_ELIM
/// IRQ number
#   define IT_NO_LINE_BURSTY_ELIM           (18)    // Interrupt number

    /// Interrupt priority level
#   define IT_LEVEL_LINE_BURSTY_ELIM        (2)    // Interrupt Level (0 = lowest priority, 15 = highest priority)

    /// Interrupt synchronisation
#   define IT_SYNCH_LINE_BURSTY_ELIM        (1)    // Interrupt synchronisation

    /// Initial enable/disable control
#   define IT_ENABLE_LINE_BURSTY_ELIM       (1)    // Initial ENABLE/DISABLE conntrol for interrupt (1 = ENABLE, 0 = DISABLE)

    /// Initial interrupt trigger control
#   define IT_TRIGGER_LINE_BURSTY_ELIM      (0)    // Initial trigger control. Normally used for software trigger. Set to 0 normally


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// 20: Histogram Schdule Interrupt
/// IRQ number
#   define IT_NO_HISTOGRAM_SCHEDULE       (20)        // Interrupt number

    /// Interrupt priority level
#   define IT_LEVEL_HISTOGRAM_SCHEDULE    (1)       // Interrupt Level (0 = lowest priority, 15 = highest priority)

    /// Interrupt synchronisation
#   define IT_SYNCH_HISTOGRAM_SCHEDULE    (1)        // Interrupt synchronisation

    /// Initial enable/disable control
#   define IT_ENABLE_HISTOGRAM_SCHEDULE   (1)        // Initial ENABLE/DISABLE conntrol for interrupt (1 = ENABLE, 0 = DISABLE)

    /// Initial interrupt trigger control
#   define IT_TRIGGER_HISTOGRAM_SCHEDULE  (0)        // Initial trigger control. Normally used for software trigger. Set to 0 normally

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


// 23: CRM
/// IRQ number
#   define IT_NO_CRM           (23)    // Interrupt number

    /// Interrupt priority level
#   define IT_LEVEL_CRM        (4)    // Interrupt Level (0 = lowest priority, 15 = highest priority)

    /// Interrupt synchronisation
#   define IT_SYNCH_CRM        (1)    // Interrupt synchronisation

    /// Initial enable/disable control
#   define IT_ENABLE_CRM       (1)    // Initial ENABLE/DISABLE conntrol for interrupt (1 = ENABLE, 0 = DISABLE)

    /// Initial interrupt trigger control
#   define IT_TRIGGER_CRM      (0)    // Initial trigger control. Normally used for software trigger. Set to 0 normally


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// 27: CD_RAW_EOF - Camera Mode EOF Error
/// IRQ number
#   define IT_NO_ERROR_EOF            (27)     // Interrupt number

    /// Interrupt priority level
#   define IT_LEVEL_ERROR_EOF         (3)      // Interrupt Level (0 = lowest priority, 15 = highest priority)

    /// Interrupt synchronisation
#   define IT_SYNCH_ERROR_EOF         (1)      // Interrupt synchronisation

    /// Initial enable/disable control
#   define IT_ENABLE_ERROR_EOF        (1)      // Initial ENABLE/DISABLE control for interrupt (1 = ENABLE, 0 = DISABLE)

    /// Initial interrupt trigger control
#   define IT_TRIGGER_ERROR_EOF       (0)      // Initial trigger control. Normally used for software trigger. Set to 0 normally


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

extern void InterruptManager_Initialise (void)TO_EXT_DDR_PRGM_MEM;

#endif /*INTERRUPTMANAGER_H_*/

