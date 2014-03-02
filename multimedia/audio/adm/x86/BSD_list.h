/*! \file BSD_list.h
    \brief Doubly linked list implementation

    Simple doubly linked list implementation pasted and modified
    from the $FreeBSD$ project (<sys/queue.h>). Removed the other list
    implementations and some debugging utilities.

    A list is headed by a single forward pointer (or an array of forward
    pointers for a hash table header). The elements are doubly linked
    so that an arbitrary element can be removed without a need to
    traverse the list. New elements can be added to the list before
    or after an existing element or at the head of the list. A list
    may only be traversed in the forward direction.
*/


/*-
* Copyright (c) 1991, 1993
* The Regents of the University of California.  All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
* 1. Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
* 4. Neither the name of the University nor the names of its contributors
*    may be used to endorse or promote products derived from this software
*    without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
* SUCH DAMAGE.
*
*      @(#)queue.h     8.5 (Berkeley) 8/20/94
*         $FreeBSD$
*/

#ifndef _BSD_LIST_H_
#define _BSD_LIST_H_

/*
* For NULL etc.
*/
#include <stddef.h>

/*
* List declarations.
*/

/**
* Defines the type needed for a list head.
* @param name The resulting type name.
* @param type The type of elements that the list will store.
*/
#define LIST_HEAD(name, type)                                           \
struct name {                                                           \
    struct type *lh_first;  /* first element */                         \
}

/**
* Initializes a list.
* @param head The list to initialize.
*/
#define LIST_HEAD_INITIALIZER(head)                                     \
{ NULL }

/**
* Declare a list entry (used for chaining together elements in the list)
* of a specific type. Here the same type stored in the list head should be
* used. The struct stored in the list should contain one of these declarations.
* @param type The type of elements being stored, should be the same as for head.
*/
#define LIST_ENTRY(type)                                                \
struct {                                                                \
    struct type *le_next;   /* next element */                          \
    struct type **le_prev;  /* address of previous next element */      \
}

/*
* List functions.
*/

/**
* Checks of the list is empty, returns a logical value.
* @param head Pointer to the list to check for emptiness.
* @returns 0: The list is not empty 1: The list is empty.
*/
#define LIST_EMPTY(head)        ((head)->lh_first == NULL)

/**
* Gets a pointer to the first element in the list.
* @param head Pointer to the list.
* @returns Pointer to the first element in the list, typed according to list
* setup.
*/
#define LIST_FIRST(head)        ((head)->lh_first)

/**
* Creates a for-loop prototype. Useful for iterating over the list. The loop
* goes from the first element to the last.
* @param var Output pointer where the next element in the list is stored at each
* iteration.
* @param head Pointer to the list.
* @param field The name of the variable where the list entries are stored. See
* LIST_ENTRY.
*/
#define LIST_FOREACH(var, head, field)                                  \
        for ((var) = LIST_FIRST((head));                                \
            (var);                                                      \
            (var) = LIST_NEXT((var), field))

/**
* Creates a removal safe for-loop prototype. Useful when iterating over the
* list with the need to remove some elements along the way. The loop
* goes from the first element to the last.
* @param var Output pointer where the next element in the list is stored at each
* iteration.
* @param head Pointer to the list.
* @param field The name of the variable where the list entries are stored. See
* LIST_ENTRY.
* @param tvar Temporary pointer where a backup element pointer can be stored in
* case of element removal.
*/
#define LIST_FOREACH_SAFE(var, head, field, tvar)                       \
        for ((var) = LIST_FIRST((head));                                \
            (var) && ((tvar) = LIST_NEXT((var), field), 1);             \
            (var) = (tvar))
/**
* Initializes an already declared list head.
* @param head Pointer to the list to initialize.
*/
#define LIST_INIT(head) do {                                            \
        LIST_FIRST((head)) = NULL;                                      \
} while (0)

/**
* Insert an element in the list after the specified element that was already in
* the list.
* @param listelm Pointer to the element to which should preceed the inserted
* element in the list.
* @param elm Pointer to the element to insert.
* @param field The name of the variable where the list entries are stored. See
* LIST_ENTRY.
*/
#define LIST_INSERT_AFTER(listelm, elm, field) do {                     \
        if ((LIST_NEXT((elm), field) = LIST_NEXT((listelm), field)) != NULL)\
            LIST_NEXT((listelm), field)->field.le_prev =                \
                &LIST_NEXT((elm), field);                               \
        LIST_NEXT((listelm), field) = (elm);                            \
        (elm)->field.le_prev = &LIST_NEXT((listelm), field);               \
} while (0)

/**
* Insert an element in the list before the specified element that was already in
* the list.
* @param listelm Pointer to the element to which should proceed the inserted
* element in the list.
* @param elm Pointer to the element to insert.
* @param field The name of the variable where the list entries are stored. See
* LIST_ENTRY.
*/
#define LIST_INSERT_BEFORE(listelm, elm, field) do {                    \
        (elm)->field.le_prev = (listelm)->field.le_prev;                \
        LIST_NEXT((elm), field) = (listelm);                            \
        *(listelm)->field.le_prev = (elm);                              \
        (listelm)->field.le_prev = &LIST_NEXT((elm), field);            \
} while (0)

/**
* Insert an element at the beginning of the list.
* @param head Pointer to the list.
* @param elm Pointer to the element to insert.
* @param field The name of the variable where the list entries are stored. See
* LIST_ENTRY.
*/
#define LIST_INSERT_HEAD(head, elm, field) do {                         \
        if ((LIST_NEXT((elm), field) = LIST_FIRST((head))) != NULL)     \
                LIST_FIRST((head))->field.le_prev = &LIST_NEXT((elm), field);\
        LIST_FIRST((head)) = (elm);                                     \
        (elm)->field.le_prev = &LIST_FIRST((head));                     \
} while (0)

/**
* Get the element following elm in the list.
* @param elm Pointer to the current element
* @param field The name of the variable where the list entries are stored. See
* LIST_ENTRY.
* @returns Pointer to the element proceeding elm.
*/
#define LIST_NEXT(elm, field)   ((elm)->field.le_next)

/**
* Remove an element from the list.
* @param elm Pointer to the element to remove.
* @param field The name of the variable where the list entries are stored. See
* LIST_ENTRY.
*/
#define LIST_REMOVE(elm, field) do {                               \
        if (LIST_NEXT((elm), field) != NULL)                        \
        LIST_NEXT((elm), field)->field.le_prev =                \
        (elm)->field.le_prev;                               \
        *(elm)->field.le_prev = LIST_NEXT((elm), field);            \
} while (0)

/**
* Swap contents of two (equally typed) lists.
* @param head1 Pointer to first list.
* @param head2 Pointer to second list.
* @param type Type of elements stored in the lists.
* @param field @param field The name of the variable where the list entries are
* stored. See
* LIST_ENTRY.
*/
#define LIST_SWAP(head1, head2, type, field) do {                       \
        struct type *swap_tmp = LIST_FIRST((head1));                    \
        LIST_FIRST((head1)) = LIST_FIRST((head2));                      \
        LIST_FIRST((head2)) = swap_tmp;                                 \
        if ((swap_tmp = LIST_FIRST((head1))) != NULL)                   \
                swap_tmp->field.le_prev = &LIST_FIRST((head1));         \
        if ((swap_tmp = LIST_FIRST((head2))) != NULL)                   \
                swap_tmp->field.le_prev = &LIST_FIRST((head2));         \
} while (0)

#endif /* !_BSD_LIST_H_ */
