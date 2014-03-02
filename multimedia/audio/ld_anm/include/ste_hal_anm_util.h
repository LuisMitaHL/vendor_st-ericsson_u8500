/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*! \file ste_hal_anm_util.c
    \brief List utilities.

    Implementation of List utilities.
*/

#ifndef STE_HAL_ANM_UTIL_INCLUSION_GUARD_H
#define STE_HAL_ANM_UTIL_INCLUSION_GUARD_H

/*
 * Defines a structure for implemementing keyvalue pairs.
 */
struct node_audio_stream{
    int key;
    void *ptr;
    struct node_audio_stream *next;
};
typedef struct node_audio_stream node_audio_stream_t;

void* ahi_list_get_nth_node(struct node_audio_stream *ahi_list, int index);
void  ahi_list_del(struct node_audio_stream **ahi_list,void *ptr);
int   ahi_list_count(struct node_audio_stream *ahi_list);
void  ahi_list_add(struct node_audio_stream **ahi_list, void *ptr, int key);
int   ahi_list_get_nth_key(struct node_audio_stream *ahi_list, int index);
int   ahi_list_get_key_index(struct node_audio_stream *ahi_list, int key);
void* ahi_list_get_key_value(struct node_audio_stream *ahi_list, int key);
void  ahi_list_clear(struct node_audio_stream **ahi_list);
void  ahi_list_del_key(struct node_audio_stream **ahi_list, int key);

struct node{
    const char key[40];
    const char value[40];
    int value_int;
    struct node *next;
};
typedef struct node node_t;
extern struct node *ahi_params;
extern struct node *ap_params;

int  list_count(struct node *list);
void list_in_begin(struct node **list,int num);
void list_add(struct node **list, const char *key, int value);
void list_del(struct node**list,const char *key);
int  list_get_value(struct node *list, char *key, char *value);
struct node* list_get_nth(struct node *list, int index);

#ifdef ANM_CHECK_MEMORY
    void* CALLOC(const char *file, int line, const char*func, int num, int size);
    #define anm_calloc(n,s) CALLOC(__FILE__, __LINE__, __FUNCTION__, n,s)
    void FREE(const char *file, int line, const char*func, void *ptr);
    #define anm_free(ptr) FREE(__FILE__, __LINE__, __FUNCTION__, ptr)
#else
    #define anm_calloc(n,s) calloc(n,s)
    #define anm_free(ptr) free(ptr)
#endif

#endif /* STE_HAL_ANM_UTIL_INCLUSION_GUARD_H */
