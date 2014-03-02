/*
 * Thermal Service Manager
 *
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef __MITIGATIONACTIONS_H__
#define __MITIGATIONACTIONS_H__

#define PLATFORM_HARD_SHUTDOWN_TIMEOUT_SECS (15)

/**
 * init cpufreq actions
 */
void actions_cpufreq_init(void);

/**
 * Shutdown the platform
 */
void action_shutdown(void);

/**
 * Set CPU to OPP1 and enable dynamic scaling
 */
void action_cpuperflevel1(void);

/**
 * Force CPU to OPP2
 */
void action_cpuperflevel2(void);

/**
 * Force CPU to OPP3
 */
void action_cpuperflevel3(void);

/**
 * Force CPU to OPP4
 */
void action_cpuperflevel4(void);

/**
 * Stop platform from charging battery
 */
void action_stopbatterycharge(void);

/**
 * Allow platform to charge battery (if needed)
 */
void action_startbatterycharge(void);

#endif
