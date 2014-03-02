/*************************************************************************
 * Copyright ST-Ericsson 2011
 ************************************************************************/
#ifndef COPS_ERROR_H
#define COPS_ERROR_H

int cops_errno(void);

char *cops_strerror(int errnum);

int cops_error_is_eagain(int errnum);

#endif /* COPS_ERROR_H */
