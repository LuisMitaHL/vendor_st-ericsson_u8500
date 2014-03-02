These are the Lauterbach scripts for debugging ux500 platforms.

Supported boards:     Comments:
  5500_V2             APP+ACC+CEVA
  8500_V2             APP+SIA+SVA
  8500_1.X*           Only APP, no updates will be done for this target
  9540_V1             APP+SIA+SVA+PRCMU(XP70)
  8540_V1             APP+SIA+PRCMU(XP70)

### GET STARTED ###

NOTE: For ST-Ericsson developers please read more at http://wiki.lud.stericsson.com/wiki/index.php/Trace32

### Trace32 version ###

the needed software version for Trace32 are :

- for U5500 and U8500 : July 2010 (24890 build) or later
- for 9540 : October 2011  (S.2011.10.000033008 Interim Build) or later
- for L8540 : May 2012 (S.2012.02.000036773) or later

PS: version can be check in Trace32 with the command : version.view


## 8500 Linux host ##
     1. Install latest Trace32 with latest patches.
     2. Make sure that you can run t32marm from your installation of Trace32.
     3. If you have your license in a file, edit the LICENSE= in the end of
	 the scripts/config_U8500-XX.t32 with the path to that file.

     Start the session using the script ./customer_start_t32_u8500_linux

## 8500 Window host ##
     Start T32_USB.ts2 in scripts\U8XXX\U8500-XX\ and change:
       1. The File path in Advanced Settings->StartupScript to point to scripts/init.cmm.
       2. The Licensefile in Advanced Settings->Paths, if you use a file for
	   Trace32 licenses.

## 5500 Linux host ##
	Before running start_t32_u5500_linux, make sure the following is set:
	1. $T32SYS: Set to the install path for Trace32.
	2. $T32TMP: Path do a directory where you have write access.
	3. $PATH: To point to the bin directory where t32marm is located.
	4. If you use a license file for Trace32, Set $T32LICENSE to point to it.

	Run start_t32_u5500_linux

## 5500 Window host ##
	Before running start_t32_u5500_windows.bat, make sure the following is set:
	1. Trace32 install dir is in your PATH.
	2. If you use a license file for Trace32, set T32LICENSE to point to it.

 	Run start_t32_u5500_windows.bat

## 9540 Linux host ##
	Before running start_t32_l9540_linux, make sure the following is set:
	1. $T32SYS: Set to the install path for Trace32.
	2. $T32TMP: Path do a directory where you have write access.
	3. $PATH: To point to the bin directory where t32marm is located.
	4. If you use a license file for Trace32, Set $T32LICENSE to point to it.

	Run start_t32_l9540_linux

## 9540 Window host ##
	Before running start_t32_l9540_windows.bat, make sure the following is set:
	1. Trace32 install dir is in your PATH.
	2. If you use a license file for Trace32, set T32LICENSE to point to it.

	Run start_t32_l9540_windows.bat

## L8540 Linux host ##
	Before running start_t32_l8540_linux, make sure the following is set:
	1. $T32SYS: Set to the install path for Trace32.
	2. $T32TMP: Path do a directory where you have write access.
	3. $PATH: To point to the bin directory where t32marm is located.
	4. If you use a license file for Trace32, Set $T32LICENSE to point to it.

	Run start_t32_l8540_linux

## L8540 Window host ##
	Before running start_t32_l8540_windows.bat, make sure the following is set:
	1. Trace32 install dir is in your PATH.
	2. If you use a license file for Trace32, set T32LICENSE to point to it.

	Run start_t32_l8540_windows.bat


### CONFIGURE SESSION###
  Press the SETUP button in the toolbar to configure your session.
    Options
      Attach to running target: 
        Uncheck this if you want to reset the cpu before doing sys.up
      SMP debugging: 
        Check this if you want to debug both cores. 
        Note: This is only possible to do after Linux/Android has booted.
      Modem TAP is on:
        Uncheck if the modem JTAG is disabled, some boards have
	this disabled for security reasons.

    MMDSP Options
	  NMF Autoloader :
	    Check this if you want to debug NMF component on MMDSP
      NMF Repository :
        Specify the directories where autoload will find NMF .elf file. 
	    Use "From Env" button to set repositories form ANDROID_PRODUCT_OUT environment variable
	  Automatic loadmap update :
	    Check this if you want to rescan dsp memory on each component creation/destruction.
		This may be very slow and it is recommended to rescan the memory when needed via
		the button in toolbar
	  Modem TAP is on:
	    Should match the settings set on APP debugger
	
  When you are done press LOAD to start debugging.

### PTM ###
  Select ETM Trace width using the ETM button in the toolbar.
    For U5500, L9540 and L8540:
      this windows allow also to activate
      the PTM/ETM trace for selected core
  Enable PTM tracing in menu UX5X0->Trace->Use PTM,
  where UX5X0 is the CPU name =[U8500, U5500, 9540, L8540]
