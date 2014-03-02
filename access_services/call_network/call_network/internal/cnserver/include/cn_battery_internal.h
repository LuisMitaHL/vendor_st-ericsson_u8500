/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef CN_BATTERY_INTERNAL_H_
#define CN_BATTERY_INTERNAL_H_
/* SIM client functions */

#define CN_BATTERY_TEXT_CHARGING "Charging"
#define CN_BATTERY_STATUS_FILE "/sys/class/power_supply/ab8500_chargalg/status"
#define CN_BATTERY_CAPACITY_FILE "/sys/class/power_supply/ab8500_fg/capacity"
#define CN_BATTERY_FOLDER_STATUS "/devices/platform/ab8500-i2c.0/ab8500-chargalg.0/power_supply/ab8500_chargalg"
#define CN_BATTERY_FOLDER_CAPACITY "/devices/platform/ab8500-i2c.0/ab8500-fg.0/power_supply/ab8500_fg"
#define CN_BATTERY_BUFFER_SIZE 128
#define CN_BATTERY_LOW_LIMIT 10

cn_bool_t  cn_battery_client_open_session(void);
cn_void_t  cn_battery_client_close_session(void);
int cn_battery_client_select_callback(const int fd, const void *data_p);

cn_bool_t cn_battery_is_low(void);
void cn_battery_modified(void);

#endif /* CN_BATTERY_INTERNAL_H_ */
