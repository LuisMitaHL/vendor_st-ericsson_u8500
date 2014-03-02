/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name  : sim_file_path.c
 * Description: Sim file path structure
 * Author     : Stefan Svenberg <stefan.svenberg@stericsson.com>
 */

/*
 * The user fn returns true when the routine shall stop searching,
 * 0 if it shall continue. The user fn should deposit any result into the user
 * data argument. It is up to the user to define the function, and also to
 * define how the user data argument should be processed.
 */
typedef int file_fn_t(void *user_data,
                      uint16_t file_id,
                      const char* file_name,
                      const char* file_path);
const char *sim_file_get_path(uint16_t file_id, uint8_t app_type);
/*
 * Generic routine that applies the fn argument to each file id, file name
 * and file_path mentioned in SIM specifications according to type of
 * sim application, gsm, or usim case. the user_data is exactly what is
 * used as first arg when the fn is applied.
 */
int sim_file_some_path(file_fn_t *fn, void *user_data, uint8_t app_type);
