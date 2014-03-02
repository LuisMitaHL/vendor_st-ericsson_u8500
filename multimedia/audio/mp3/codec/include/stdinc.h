

#ifndef	_stdinc_h_
#define	_stdinc_h_

#include <stddef.h>
#include <assert.h>

#ifndef MMDSP
#include <math.h>
#endif // MMDSP 

#ifdef DEBUG

#ifndef _POSIX_SOURCE
#define                 _POSIX_SOURCE	/* stops repeat typdef of ulong */
#endif

#include		<stdio.h>
#include		<stdlib.h>
#ifndef __CC_ARM 
#ifndef __flexcc2__
#include		<sys/types.h>
#endif // __flexcc2__
#endif // __arm__ 

#include		<string.h>
#include 		<math.h>

#define	PRINT		fprintf
#define	SO		stdout
#define	SE		stderr

#endif /* DEBUG */
#endif				/* _stdinc_h */
