************************************************
QUICK HOW TO
************************************************

Purpose: This file provides the Steps for compiling the package for Linux platform:-


The following steps need to be followed to compile the UMAC package for ARM Linux platforms:-
---------------------------------------------------------------------------------------------
1) Browse to the directory "proj_host_umac/build".
2) Copy Makefile to the directory which contains umac source code (the folder containing proj_host_umac, mod_umc, mod_umi and ext_inc).
3) Use command "make CROSS_COMPILE = arm-linux" to complie pakage.
4) This will generate proj_host_umac.lib in the folder where makefile was kept.



