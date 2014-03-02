/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
/**
 * \internal
 */
#ifndef LIST_H_
#define LIST_H_

typedef struct list_element {
	struct list_element *prev, *next;
} t_list_entry;

#define LIST_ENTRY(name) \
        t_list_entry name = { &(name), &(name) }

#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

/*!
 * \brief get the struct for this entry
 *
 * \param[in] ptr        the &t_list_entry pointer.
 * \param[in] type       the type of the struct this is embedded in.
 * \param[in] member     the name of the t_list_entry within the struct.
 */
#define list_entry(ptr, type, member) \
	((type *)( (char *)ptr - ((unsigned int) &((type *)0)->member)))
/*!
 * \brief Initialize an entry
 *
 * \param[in] entry Entry to initialize
 */
void list_init_entry(t_list_entry *entry);

/*!
 * \brief Add a new entry
 *
 * \param[in] n: new entry to be added
 * \param[in] entry list entry to add it after
 *
 * Insert a new entry after the specified entry.
 */
void list_add(t_list_entry *n, t_list_entry *entry);

/*!
 * \brief Deletes entry from list.
 *
 * \param[in] entry: the element to delete from the list.
 */
void list_del(t_list_entry *entry);

/*!
 * \brief Tests whether a list is empty
 *
 * \param[in] entry The list to test.
 */
int list_empty(const t_list_entry *entry);

#endif /*LIST_H_*/

