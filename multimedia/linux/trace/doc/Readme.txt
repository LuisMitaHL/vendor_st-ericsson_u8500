	*************
	* STM TRACE *
	*************

INTRODUCTION
============
This package provides some API to log any traces through the System
Trace Module on User Space.

The driver is now part of the kernel

USER API
========
For the User side, refer to and include the file api/stm-trace.h to see the
available user API.

The following trace API is available on both kernel and user side; it allows you to
trace any size of data value on the specified channel; these API are lock-less

void stm_trace_8(unsigned char channel, unsigned type data);
void stm_tracet_8(unsigned char channel, unsigned type data);
void stm_trace_16(unsigned char channel, unsigned type data);
void stm_tracet_16(unsigned char channel, unsigned type data);
void stm_trace_32(unsigned char channel, unsigned type data);
void stm_tracet_32(unsigned char channel, unsigned type data);
void stm_trace_64(unsigned char channel, unsigned type data);
void stm_tracet_64(unsigned char channel, unsigned type data);

     * The *_tracet_* version output the trace with a timestamp.
     * IMPORTANT: there is no synchronization neither between channels, neither
       within a channel, or between kernel and user side.
       The user is encouraged to use separate channels between kernel and user space
       and to ensure himself the required synchronisation (if necessary) within a
       given channel.
     * stm_trace_buffer(): user side API - output the buffer into a per-thread channel.
       This channel is dynamically allocated and is hidden to the user. The channel
       is free when the thread exits. Using this API with the previous one may lead
       to a mix of trace in a given channel.

EXAMPLE
=======
To use the trace on user side, add in your C-file:
(See the test under src/test for a simple example)

----------------------------------------
#include <stm-trace.h>

int main(int argc, char *argv[])
{
	int error;

	error = stm_trace_open();

	...
	stm_trace_32(channel_nb, my_data);
	...

	stm_trace_close();
}
----------------------------------------

Then compile your file:

$ $CC my_file.c -I$(MMROOT)/linux/trace/api -L$(INSTALL_LIB_DIR) -ltrace
