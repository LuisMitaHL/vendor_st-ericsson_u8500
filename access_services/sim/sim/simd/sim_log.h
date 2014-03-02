
#ifndef SIM_LOG_H
#define SIM_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "util_log.h"

void sim_log_init();
void sim_log_close();
util_log_type_t sim_get_log_level();


#define  SIM_LOG_E(...) util_print_log(UTIL_LOG_MODULE_SIM, sim_get_log_level(), UTIL_LOG_TYPE_ERROR, __func__, __VA_ARGS__)
#define  SIM_LOG_W(...) util_print_log(UTIL_LOG_MODULE_SIM, sim_get_log_level(), UTIL_LOG_TYPE_WARNING, __func__, __VA_ARGS__)
#define  SIM_LOG_I(...) util_print_log(UTIL_LOG_MODULE_SIM, sim_get_log_level(), UTIL_LOG_TYPE_INFO, __func__, __VA_ARGS__)
#define  SIM_LOG_D(...) util_print_log(UTIL_LOG_MODULE_SIM, sim_get_log_level(), UTIL_LOG_TYPE_DEBUG, __func__, __VA_ARGS__)
#define  SIM_LOG_V(...) util_print_log(UTIL_LOG_MODULE_SIM, sim_get_log_level(), UTIL_LOG_TYPE_VERBOSE, __func__, __VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif /*SIM_LOG_H */


