#ifndef DTHSAMPLE_H_
#define DTHSAMPLE_H_

#include "9psrvhelper.h"

int dth_init_service();

int reg1_get(struct dth_element *elem, void *value);
int reg1_set(struct dth_element *elem, void *value);
int reg2_get(struct dth_element *elem, void *value);
int reg2_set(struct dth_element *elem, void *value);
int reg4_get(struct dth_element *elem, void *value);
int reg5_set(struct dth_element *elem, void *value);
int table1_get(struct dth_element *elem, void *value);
int table1_set(struct dth_element *elem, void *value);
int my_bitfield_get(struct dth_element *elem, void *value);
int my_bitfield_set(struct dth_element *elem, void *value);
int my_action_get(struct dth_element *elem, void *value);
int my_action_set(struct dth_element *elem, void *value);
int my_action_exec(struct dth_element *elem);

#endif

