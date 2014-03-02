/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef ATC_SELECTOR_H_
#define ATC_SELECTOR_H_

#include <stdbool.h>

typedef bool (*callback_t)(int, void *);

typedef struct selector_s {
    struct selector_s *next;
    int fd;
    void *data_p;
    callback_t callback;
} selector_item_t;

bool selector_init(void);
bool selector_register_callback_for_fd(int fd, callback_t callback, void *data_p);
bool selector_deregister_callback_for_fd(int fd);
void selector_loop(callback_t default_callback, void *data_p);
bool selector_default_callback(int fd, void *data_p);

#endif /* ATC_SELECTOR_H_ */
