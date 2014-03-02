/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
#include <cm/proxy/common/utils/inc/list.h>

void list_init_entry(t_list_entry *entry)
{
        entry->next = entry;
        entry->prev = entry;
}

void list_add(t_list_entry *n, t_list_entry *entry)
{
        entry->next->prev = n;
        n->next = entry->next;
        n->prev = entry;
        entry->next = n;
}

void list_del(t_list_entry *entry)
{
        entry->next->prev = entry->prev;
        entry->prev->next = entry->next;
}

int list_empty(const t_list_entry *entry)
{
        return entry->next == entry;
}
