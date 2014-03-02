
            ===================
                 NMF LINUX
            ===================


This file describes the use of NMF under Linux OS, how to compile, install and use it.

Index:
------
	0. INTRODUCTION
	1. DESCRIPTION - WHAT YOU'LL FIND THERE
	2. COMPILATION
	3. INSTALLATION
	4. USAGE

0. INTRODUCTION
===============
This part addresses the NMF Linux specific part. For all general NMF points,
please, refer to NMF Documentation.

This package is a RODOS package. To use it or compile it, you need a RODOS environment
and some other packages.
Refer to the provided components.txt file to see the required package. To summarize,
you need:

* As a basis for a Linux environment:
  - mmenv
  - makefiles
  - linux_kernel
  - linux_ramdisk
  - launcher
* To develop with NMF under Linux, you need these packages:
  - com.stericsson.nmf
  - com.stericsson.nmf.tools
  - com.stericsson.nmf.mpcee
  - com.stericsson.nmf.linux
* And to compile NMF Linux (this package), you these additional packages
  - com.stericsson.nmf.osintegration
  - com.stericsson.los.osintegration

1. DESCRIPTION - WHAT YOU'LL FIND THERE
=======================================
This package contains the Linux source files and the necessary compiled libraries and
kernel driver (kernel module) and header files to develop and use NMF under linux.

Here is the details:
  \_components.txt           # the typical rodos project file you must include to use NMF
  \_api                      # NMF headers required to build your NMF applications
  \_doc 	             # release notes and this readme.
  \_src                      # NMF Linux source files
      \_common               # common header (shared between kernel and user space)
      \_lib	             # userland libraries
          \_CMUserProxy      # CM User Proxy sources
	  \_hostee           # Host (arm/workstation) Executive Engine sources
	  \_hosteeso         # Makefile to generate hostee dynamic library
	  \_los              # LOS library (OS abstraction layer) sources
      \_driver


2. COMPILATION
==============

* Requirements
--------------
You need to have all the above packages installed, as stated in the components.txt file
provided within this package.

Add in your components.txt, this line for example:
include rod+package://com.stericsson.nmf.linux:X.Y.Z/components.txt

* Compilation
-------------
- go under shared/nmf/linux directory
- type 'make'

3. INSTALLATION
===============

* Manual installation
---------------------

This is what you need to do to install and use NMF on your board:

- copy the kernel module (file ./src//driver/cm.ko ) somewhere in your target's
  filesystem
- insert this module ( 'insmod .../cm.ko' )
- if you've linked you binary dynamically, copy
  ./lib/$PLATFORM/lib*.so in your target's filesystem (for example
  in /usr/lib)
- install all NMF components in /usr/share/nmf/repository/mmdsp_$TARGET
  (with TARGET = 8815, 8820 or 8500_ed)


* Installation with 'make'
--------------------------

Running 'make install' will install the linux specific part of NMF under your
RODOS environment.

4. USAGE
========
Your NMF application must be linked with libnmf and libnmfee if you're using
the Host Executive Engine.
Optionally, you must link with the liblos if you're application relies on the LOS API.
