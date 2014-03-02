Release Label : STSWCW1200LX_WLANDRV_A01.01.0011
--------------------------------------

Content of the release package
------------------------------

  Release.txt: Release notes

  Readme.txt: This document
 
  Source: This folder contains the source code for WLAN components

         
           |- int/eil  -> Ethernet Interface Layer
           |- int/cil  -> CFG80211 Interface Layer
           |- int/sbus -> Serial Bus
           |- int/common -> OS Abstraction Layer and Commonf defs
           |- Makefile
           |- doc - Contains ReleaseNotes,ReadMe and Sanity Test Report
           |- firmware - Contains SDD file and WSM firmware.
           |- host_umac - Contains host_umac Source Code.
           |- lib - Contains compiled umac.lib for HREF platform.
	   |- compat-wireless contains CFG80211 related source code.
         
                        
Platform supported:- HREF ED/V1/plus

Linux kernel version:- 2.6.29

WLAN HW used:- CW1200-ADB card

Dependencies on Kernel:-
-----------------------
1) Standard Linux SDIO Host Controller support.
2) CFG80211 from Linux 2.6.32
3) Open Source WPA Supplicant

Steps for compiling the package:-
-------------------------------- 

1) Copy the proj_drv_linuxstumac directory in <LINUX_SI_Release>/stelp/connectivity/wlan/ directory
2) Rename proj_drv_linuxstumac directory to cw1200_wlan.
3) Extract host_umac_os.zip .
4) Rename host_umac_os directory to umac_lib.
5) Cut umac_lib directory to one upper level so that it comes in parallel to cw1200_wlan.
3) To build the driver execute: make -f ux500.mak build-wlan STE_PLATFORM=u8500 SW_VARIANT=android all 
4) cw1200_wlan.ko module is generated.


Files to be copied to ramdisk:
-----------------------------
1) cw1200_wlan.ko   -> WLAN driver.
2) wsm.bin          -> WLAN device firmware. To be placed in the root directory of the RAMDISK.
3) sdd.bin          -> WLAN SDD file.To be placed in the root directory of the RAMDISK.
4) Make sure the Linux SDIO related .ko files are present in the RAMDISK, if not compiled into the kernel.
