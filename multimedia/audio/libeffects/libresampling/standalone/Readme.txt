libperf:

Introduction
------------

Pprofiler is a low-overhead, intrusive performance monitoring tool for
cortex-A9. Profiling an intensive code (like AAC/MP3/MPEG-surround) manually
is an exceedingly important, time-consuming and laborious task. Pprofiler
would automate this process providing detailed function profile. This would
save developer's time and help optimize source-code efficiently. Along with
profiling information (call tree, cycles, cache misses etc.), it also
generates an optimization-summary report, providing suggestions (top-five
functions, inlining, IPC) to optimize the code. In complex use cases, like
MPEG-surround, overhead with Pprofiler is found to be less than 1%.

Pprofiler makes use of hardware performance counters provided on cortex-A9
processor. It is implemented as a C-lib which can be enabled by using
instrumentation flags provided by ARM at compile time to source code. Source
code of Pprofiler is generic enough and can be modified to profile different
attributes (instruction count, write miss etc.) on different processors. It
promises to help CA9 developers, wherever perf/memory optimization is
required. This should provide considerable usage in CA9-codes.
Pprofiler can be used with RVDS toolset version 4 (build
471) or later. Currently, Pprofiler is in beta state and is being used in MMD
audio-codec team.




Usage:
------

This library provides two functions startperf() and endperf() to profile code. 
These functions are to be called from inside the code to be profiled.

tree structure for libperf:

libperf
|-- Readme.txt
|-- bin
|   `-- pperf
|-- include
|   `-- tperf_api.h
`-- lib
    |-- ca9
    |   |-- debug
    |   |   `-- libtperf.a
    |   `-- release
    |       `-- libtperf.a
    `-- x86
        |-- debug
        |   `-- libtperf.a
        `-- release
            `-- libtperf.a


libperf contain release and debug binaries for x86 and ca9 platform. Incase there are unknown errors they can be debugged by linking debug binaries instead of release

Naming conventions:
-------------------

tperf -> [t]ap [perf]ormance
    This refers to generation of profile.bin file by running code compiled 
with instrumenation.

pperf -> [p]rocess [perf]ormance
    This refers to generation of TMR & TR files from [profile.bin + executable(e.g .axf)]

TMR -> Tree Merged Report, you would normally need this
TR  -> Tree Report, used in case more detailed call chain is required



Recompiling tperf/pperf 

1. compiling: Normally not required


   <make x86/ca9> - 
                    1. creates lib/[x86/ca9]/[release/debug]/libtperf.a & bin/pperf for respective platform 
                    2. copies appropriate libs to ../libperf, which is the delivery package for the platform
   <make clean> - clean obj's and binaries/libs

As a user of Pprofiler library, only libperf would be required. There is as such no need to change the source code.


Steps to generate profile using tperf & pperf
---------------------------------------------


1. Identify the code to be profiled
    Let us say libamr is the lib to be profiled.
  
2. Identify the points between which all the functions are to be profiled.

    use startperf() to mark the starting point and endperf() to mark the end of profile.
    startperf & endperf are declared in tperf_api.h provided. So, tperf_api.h need to be included

    compile the code which needs to be profiled with following flags,

   -finstrument-functions  - gcc
   -gnu-instrument        - armcc

    These are compilation flags, must only be put in the core libs of 
    codec/code. Please try to put all the pprofiler code with #ifdef PPROFILER

4. link your code with libtperf.a

5. Run the code

6. you should be able to see profile.bin(raw binary profile)

7. Now to decode profile.bin, executable binary is also required(e.g 
   codec.axf for CA9)

   bin/pperf profile.bin codec.axf
   This should generate files like sample_report/TR00.pp & 
   sample_report/TRM00.pp in your current directory.

Note: 

1. Do not forget to save your compiled axf & profile.bin.
2. code must be compiled with RVDS 4.0 build 471 atleast. It can be run with RVDS 4.0(no minumum requirement for build)
3. Source file for RVDS 4.0 build 471 is available in toolset/rvds4build471.posh 
4. startperf() and endperf() should not be nested. otherwise expect "unexpected results" :-)
5. startperf() and endperf() can be used multiple times in the code. 
6. Pprofiler is by default compiled for support of 2^16 function calls. To increase this open tperf.h & change PLIST_SHIFT_SIZE preprocessor define. 
   Code needs to recompiled for the same.


FAQS:

1. startperf() and endperf() not found while linking. 

        This means libtperf.a is not properly linked. 

2. Output of TMR looks cluttered
        
        Do not forget to disable line wrap in your viewer

3. "internal fault error, please contact your supplier" while compiling some of the files

    This is sometime due to non-standard C usage. There are some C lines which could be making it difficult for the compiler to insert profiling code in the 
    functions. They are normally return statements of the functions.   
     a. compile particular files without -gnu-instrument flags manually and restart the make process.
     b. Open the culprit C file and modify the problem causing code, usually simplifying the return statement.
       
4. execution on board stops when I use profiling library

        Please check your scatter file. This should provide atleast 2 MB heap. Heap requirement depends on PLIST_SIZE define and is displayed
        when Pprofiler executes.

Enhancements expected:
---------------------

1. Optmization summary report. 

2. Better synthesized cache results.

3. Please send any enhancement suggestions to chandan.thakur@stericsson.com



compile-step lines e.g:
-----------------------

ifeq ($(PPROFILER),true)
CPPFLAGS+=-I$(MMROOT)/shared/libperf/include/ -DPPROFILER
CFLAGS+=--gnu-instrument
endif


link-step:
---------

ifeq ($(PPROFILER),true)
LIBS+=$(MMROOT)/shared/libperf/lib/libtperf_ca9.a
endif


