#ifndef __WATCHDOG_H__
#define __WATCHDOG_H__

#include "services.h"
#include "ab8500_core_services.h"

#define WATCHDOG_TIMEOUT 10

t_ser_ab8500_core_error HWI2C_Enabe_WD(uint8 secs);
void HWI2C_Do_WatchDog_Disable(void);
void HWI2C_Do_WatchDog_Kick(void);
uint32 HWI2C_Set_Watchdog_Delayed_Kicker(uint32 DelayTime);
uint8 wait_status(uint32 addr, uint32 mask, uint32 status);

#endif/* __WATCHDOG_H__ */

/* End of file - watchdog.h */
