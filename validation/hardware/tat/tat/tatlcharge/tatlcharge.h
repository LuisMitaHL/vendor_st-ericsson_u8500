/*****************************************************************************/
/**
*  ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides services for power services module
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef TATLCHARGE_H_
#define TATLCHARGE_H_

#include <libab.h>

#define SYSLOG					CHARGE_SYSLOG
#define SYSLOGLOC				CHARGE_SYSLOGLOC
#define SYSLOGSTR				CHARGE_SYSLOGSTR

#include "dthcharge.h"
#include "tatl01chargefunc.h"
#include "tatl02chargegen.h"

#ifdef HATSCONF_AB_LIB_8505
#include "ab9540_Charger_types.h"
#include "ab9540_Control_types.h"
#include "ab9540_Gauge_types.h"
#include "ab9540_GPIO_types.h"
#include "ab9540_USB_types.h"
#else
#include "ab8500_Charger_types.h"
#include "ab8500_Control_types.h"
#include "ab8500_Gauge_types.h"
#include "ab8500_GPIO_types.h"
#include "ab8500_USB_types.h"
#endif
#endif /* TATLCHARGE_H_*/
