/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*! \file ste_anm_util.c
    \brief List utilities.

    Implementation of List utilities.
*/

#define LOG_TAG "anm_util"

#include "ste_hal_anm_util.h"
#include "ste_hal_anm_dbg.h"
#include <string.h>
#include <stdarg.h>
#include <cutils/log.h>
#include "cutils/properties.h"

void ahi_list_add(struct node_audio_stream **ahi_list, void *ptr, int key)
{
    struct node_audio_stream *temp,*new_node;
    temp = *ahi_list;
    if (*ahi_list == NULL) {
        temp = (struct node_audio_stream *)anm_calloc(1, sizeof(struct node_audio_stream));
        temp->ptr = ptr;
        temp->key = key;
        temp->next = NULL;
        *ahi_list = temp;
    } else {
        temp = *ahi_list;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        new_node = (struct node_audio_stream *)anm_calloc(1, sizeof(struct node_audio_stream));
        new_node->ptr = ptr;
        new_node->key = key;
        new_node->next = NULL;
        temp->next = new_node;
    }
}

int ahi_list_count(struct node_audio_stream *ahi_list)
{
    int c=0;
    if (ahi_list == NULL) {
        /*Empty Link node_audio_stream */
        goto exit;
    }
    while (ahi_list != NULL) {
        c++;
        ahi_list = ahi_list->next;
    }
exit:
    return c;
}

void ahi_list_del(struct node_audio_stream **ahi_list, void *ptr)
{
    struct node_audio_stream *currP, *prevP;
    /* For 1st node, indicate there is no previous. */
    prevP = NULL;
    /* Visit each node, maintaining a pointer to
     * the previous node we just visited. */
    for (currP = *ahi_list; currP != NULL; prevP = currP, currP = currP->next) {
        if (currP->ptr == ptr) {  /* Found it. */
            if (prevP == NULL) {
                /* Fix beginning pointer. */
                *ahi_list = currP->next;
            } else {
                /* Fix previous node's next to
                 * skip over the removed node. */
                prevP->next = currP->next;
            }
            /* Deallocate the node. */
            anm_free(currP);
            /* Done searching. */
            return;
        }
    }
}

int ahi_list_get_key_index(struct node_audio_stream *ahi_list, int key)
{
    struct node_audio_stream *currP;
    int index = 0;

    for (currP = ahi_list; currP != NULL; currP = currP->next) {
        if (currP->key == key) {  /* Found it. */
            return index;
        }
        index++;
    }
    return -1;
}

void *ahi_list_get_key_value(struct node_audio_stream *ahi_list, int key)
{
    struct node_audio_stream *currP;

    for (currP = ahi_list; currP != NULL; currP = currP->next) {
        if (currP->key == key) {  /* Found it. */
            return currP->ptr;
        }
    }
    return NULL;
}

void ahi_list_clear(struct node_audio_stream **ahi_list)
{
    struct node_audio_stream *currP;
    int index = 0;

   if(ahi_list!=NULL) {
       while (*ahi_list) {
           currP = *ahi_list;
           *ahi_list = currP->next;
           anm_free(currP);
       }
   }
}

void* ahi_list_get_nth_node(struct node_audio_stream *ahi_list, int index)
{
    struct node_audio_stream *temp;
    temp = ahi_list;
    while (index--) {
	if(temp !=NULL) {
	    temp = temp->next;
	}
	else {
	    ALOG_WARN(" !!! ahi_list_get_nth_node : Bad usage of index");
	    return NULL;
	}
    }
    return temp->ptr;
}

int ahi_list_get_nth_key(struct node_audio_stream *ahi_list, int index)
{
    struct node_audio_stream *temp;
    temp = ahi_list;
    while (index--) {
        if(temp !=NULL) {
	    temp = temp->next;
	}
	else {
	    ALOG_WARN("!!! ahi_list_get_nth_key  : Bad usage of index");
	    return (-1);
	}
    }
    return temp->key;
}

void ahi_list_del_key(struct node_audio_stream **ahi_list, int key)
{
    struct node_audio_stream *currP, *prevP;
    /* For 1st node, indicate there is no previous. */
    prevP = NULL;

    /* Visit each node, maintaining a pointer to
     * the previous node we just visited. */
    for (currP = *ahi_list; currP != NULL; prevP = currP, currP = currP->next) {
        if (currP->key == key) {  /* Found it. */
            if (prevP == NULL) {
                /* Fix beginning pointer. */
                *ahi_list = currP->next;
            } else {
                /* Fix previous node's next to
                 * skip over the removed node. */
                prevP->next = currP->next;
            }
            /* Deallocate the node. */
            free(currP);
            /* Done searching. */
            return;
        }
    }
}

void list_add(struct node **list, const char *key, int value)
{
    struct node *temp,*new_list, *new_node;
    temp = *list;
    ALOG_INFO_VERBOSE("ENTER list_add");
    if (*list == NULL) {
        /* Empty list, add the first node */
        temp = (struct node *)anm_calloc(1, sizeof(struct node));
        strcpy((char*)temp->key, key);
        temp->value_int = value;
        temp->next = NULL;
        *list = temp;
    }
    else {
        temp = *list;
        while (temp->next != NULL) {
            temp=temp->next;
        }
        new_node = (struct node *)anm_calloc(1, sizeof(struct node));
        strcpy((char *)new_node->key, key);
        new_node->next = NULL;
        temp->next = new_node;
        new_node->value_int = value;
    }
    ALOG_INFO_VERBOSE("LEAVE list_add");
}

int list_count(struct node *list)
{
    int c = 0;
    if (list == NULL) {
        /* Empty Link node */
        goto exit;
    }
    while (list != NULL) {
        c++;
        list=list->next;
    }
exit:
    return c;
}

void list_del(struct node **list,const char *key)
{
    if (*list == NULL) {
        /*Empty Linked node.Cant Delete The Data */
        return;
    }
    else {
        struct node *old,*temp;
        int flag = 0;
        temp = *list;
        old = temp;
        while (temp != NULL) {
            if(strcmp(temp->key,key)) {
                if (temp == *list) {
                    /* First Node case */
                    *list = temp->next; /* shifted the header node */
                }
                else {
                    old->next = temp->next;
                }
                anm_free(temp);
                flag = 1;
            }
            else {
                old = temp;
                temp = temp->next;
            }
        }
    }
}

struct node* list_get_nth(struct node *list, int index)
{
    struct node *temp;
    temp = list;
    while (index--) {
        if(temp !=NULL) {
            temp = temp->next;
        }
        else {
            ALOG_WARN("!!! list_get_nth  : Bad usage of index");
	   return (NULL);
        }
    }
    return temp;
}

int list_get_value(struct node *list, char *key, char *value)
{
    struct node *temp;
    temp = list;
    while (temp != NULL) {
        if (!strcmp(temp->key,key)) {
            strcpy(value, temp->value);
            return 0;
        }
        temp = temp->next;
    }
    return -2;
}
