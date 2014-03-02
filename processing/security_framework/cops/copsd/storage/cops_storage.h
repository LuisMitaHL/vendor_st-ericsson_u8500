/*************************************************************************
 * Copyright ST-Ericsson 2010
 ************************************************************************/
#ifndef COPS_STORAGE_H
#define COPS_STORAGE_H

#include <cops_data.h>

cops_return_code_t cops_storage_read(cops_data_t *cd,
                                     enum cops_data_type data_type);
cops_return_code_t cops_storage_write(cops_data_t *cd,
                                      enum cops_data_type data_type);

#endif                          /*COPS_STORAGE_H */
