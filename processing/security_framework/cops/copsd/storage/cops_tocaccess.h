/*************************************************************************
 * Copyright ST-Ericsson 2010
 ************************************************************************/
#ifndef COPS_TOCACCESS_H
#define COPS_TOCACCESS_H

#include <cops_data.h>

cops_return_code_t cops_open_tochandler(void);

cops_return_code_t cops_close_tochandler(void);

cops_return_code_t cops_write_tocdata(char TOCid[12], void *buffer,
                                      size_t length);

#endif                          /*COPS_TOCACCESS_H */
