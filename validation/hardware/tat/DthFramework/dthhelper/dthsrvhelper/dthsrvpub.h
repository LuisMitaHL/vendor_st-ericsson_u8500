/*****************************************************************************/
/**
*  ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file
* \brief   9P interfaces for DTH server helper
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef DTH_9PSRVHELPER_PUBLISH_H_
#define DTH_9PSRVHELPER_PUBLISH_H_

#include "dthsrvhelper.h"

/* libxml includes */
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>

void dth_publish_term(void);

/**
 * Publisher verbose mode
 * @param[in] verbose 0: print minimal info, 1: print the most info available
 */
void dth_publish_verbose(int verbose);

/**
 * Prepare publishing of an API
 * If the file already exists then its content is overriden according to the
 * future calls until dth_publish_finish() is called.
 * The file is created if missing.
 * @param[in] filename a full pathname to the file be
 *				filled with the API description
 * @param[in] mode 'c' to create the file, 'a' to reuse an existing file
 * @retval 0 if the operation succeeds else a negative value as an error code
 */
int dth_publish_init(const char *filename, char mode);

/**
 * Return publisher activation status
 * @retval 0: publisher is disabled
 * @retval 1: publisher is enabled
 */
int dth_publish_is_enabled(void);

/**
 * Publish a DTH element into the API document for the
 * current module and action if set
 * @param[in] elem a valid DTH element
 * @retval 0 if success or >0 as an errno
 */
int dth_publish_element(const struct dth_element *elem);

/*
 * Release all of the resource allocated to the publisher.
 * this function is to be called at the end.
 */
void dth_publish_termination(void);

/**
 * Change the module to which the further calls will apply.
 * If the module name is currently set the selection is changed to \a module.
 * @param[in] module a module name
 */
void dth_publish_set_module(const char *module);

/**
 * Set the path depth used to auto generate groups on
 * each dth_publish_element() call.
 * For example, if a module defines its actions with the following pattern
 * /DTH/ThisModule/Feature/Action/... then the depth could be set to 3 so
 * each Feature will be placed in a seperated group.
 * @param[in] depth a positive depth to enable the feature.
 *				A zero or negative value will negate it.
 */
void dth_publish_set_group_depth(int depth);

/**
 * Commit or cancel the document.
 * If commited, its content is flushed into the file
 * specified by dth_publish_init().
 * The document is freed and the filename reset on cancelation cancel
 * then another call to dth_publish_init() is necessary
 * to set the filename again.
 * @param[in] doValidate 0 = cancel, 1 = commit
 */
void dth_publish_validate(int doValidate);

#endif /* DTH_9PSRVHELPER_PUBLISH_H_ */

/* END OF FILE */
