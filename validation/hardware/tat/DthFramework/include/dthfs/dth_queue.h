/*****************************************************************************/
/**
*  ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides services for power services module
* \author  ST-Ericsson
*/
/*****************************************************************************/

/**
 * @file dth_queue.h
 *
 * @brief Interface for queue algorithm (a.k.a single linked list).
 */

#ifndef DTH_QUEUE_H_
#define DTH_QUEUE_H_

#include <dthfs/dth_utils.h>

/** Singly linked node. */
struct dth_snode {
	/** reference to the next element in the sequence. */
	struct dth_snode *next;
};

/** Singly linked list. */
struct dth_queue {
	/** reference before the first node in the sequence. */
	struct dth_snode head;
	/** reference after the last node in the sequence. */
	struct dth_snode tail;
};

/**
 * Statically initialize a queue.
 */
#define dth_DEFINE_QUEUE(q) struct dth_queue (q) = { { &(q).tail }, { NULL } }

/**
 * Dynamically initialize a queue.
 * @param[in,out] queue specifies the queue
 */
static inline void dth_queue_init(struct dth_queue *queue)
{
	queue->head.next = &queue->tail;
	queue->tail.next = NULL;
}

/**
 * Check if a queue is empty.
 * @param[in] queue specifies the queue
 */
static inline int dth_queue_is_empty(const struct dth_queue *queue)
{
	return queue->head.next == &queue->tail;
}

/**
 * Add a queue node after another one.
 * @param[in,out] prev specifies the node after which to insert the new node
 * @param[in,out] node specifies the node to insert
 * @pre prev is in queue
 * @pre prev is not the tail of queue
 */
static inline void dth_queue_add_after(struct dth_snode *prev,
		struct dth_snode *node)
{
	node->next = prev->next;
	prev->next = node;
}

/**
 * Remove a queue node after another one.
 * @param[in,out] prev specifies the node after which to insert the new node
 * @pre prev is in queue
 * @pre prev is not the immediately before the tail of queue
 * @return the removed node
 */
static inline struct dth_snode *dth_queue_del_after(struct dth_snode *prev)
{
	struct dth_snode *node;
	node = prev->next;
	prev->next = node->next;
	return node;
}

/**
 * Predicate for searching queues.
 * @param[in] node specifies the node to inspect
 * @param[in,out] arg specifies the opaque data passed by the search caller
 * @return zero if and only if the node does not match the search criteria
 */
typedef int (*dth_queue_predicate_t)(const struct dth_snode *node, void *arg);

/**
 * Find a queue node immediately before another one that matches a predicate.
 * @param[in] queue specifies the queue
 * @param[in] node specifies from where to start the search
 * @param[in] cb specifies the predicate to call back when examining the nodes
 * @param[in,out] arg specifies an extra opaque data to pass to the predicate
 * @retval the last node of queue if no node was found
 * @retval the node immediately before the expected node in the queue otherwise
 * @pre node is in queue
 * @pre node is not the tail of queue
 */
static inline struct dth_snode *
dth_queue_find_before(const struct dth_queue *queue,
		const struct dth_snode *node,
		dth_queue_predicate_t cb,
		void *arg)
{
	const struct dth_snode *curr = node->next;
	while (curr != &queue->tail) {
		if (cb(curr, arg))
			break;
		node = curr;
		curr = curr->next;
	}
	return (struct dth_snode *) node;
}


/**
 * Find the queue node immediately before another one.
 * @param[in] queue specifies the queue
 * @param[in] node specifies the node immediately after the expected node
 * @retval NULL if node is the head of queue
 * @retval tail of queue if node is not in queue
 * @retval the node immediately before node in the queue otherwise
 */
static inline struct dth_snode *
dth_queue_get_prev(const struct dth_queue *queue, const struct dth_snode *node)
{
	struct dth_snode *prev = NULL;
	struct dth_snode *curr = (struct dth_snode *) &queue->head;
	do {
		if (curr == node)
			break;
		prev = curr;
		curr = curr->next;
	} while (curr != NULL);
	return prev;
}

/**
 * Find the queue node immediately after another one.
 * @param[in] node specifies the node immediately before the expected node
 * @retval NULL if node is the tail of queue
 * @retval tail of queue if node is the last node in queue
 * @retval the node immediately before node in the queue otherwise
 */
static inline struct dth_snode *
dth_queue_get_next(const struct dth_snode *node)
{
	return node->next;
}

#endif /* dth_QUEUE_H_ */
