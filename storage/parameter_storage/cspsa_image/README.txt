--------------------------------------------------------------------------------
Copyright (C) 2010 ST-Ericsson AB
--------------------------------------------------------------------------------

This directory contains common production parameters for U8500.

It contains the following files:

+-------------------------------+-----------------------------------------------+
| File name                     | Comment                                       |
+-------------------------------+-----------------------------------------------+
| u8500_default_cspsa.bin       | CSPSA binary image that can be downloaded     |
|                               | into targe system.                            |
+-------------------------------+-----------------------------------------------+
| u8500_default_cspsa_nwm.gdf   | GDF representation of NWM file structure      |
|                               | under directory 'nwm'.                        |
+-------------------------------+-----------------------------------------------+
| u8500_default_cspsa.gdf       | List of items in CSPSA binary image. Can      |
|                               | be used to backtrack changes in PFF or NWM.   |
+-------------------------------+-----------------------------------------------+
| u8500_default_cspsa.sh        | Script to generate new binary, gdf and list   |
|                               | files after updated NWM or PFF.               |
+-------------------------------+-----------------------------------------------+

Directory 'nwm' contains NWM default configuration files and directory
'pff' contains other production parameters in a Parameter File Format file
structure. There is a text file in directory 'pff' to describe the pff format.

After a change in files under 'nwm' or 'pff' run script './u8500_default_cspsa.sh'
to process the changes and to generate a new CSPSA binary image 
'u8500_default_cspsa.bin'. Double check the intended change by comparing 
'u8500_default_cspsa.gdf' with the intended changes.

The script 'u8500_default_cspsa.sh' does the following:

 1) Runs 'nwm2gdf' to generate a GDF file with NWM directory 'nwm'
 2) Runs 'gdf2pff' with the GDF containing NWM configuration into the appropriate 
    directory under 'pff' which stores the NWM items as PFF files.
 3) Runs 'pff2cspsa' on the directory 'pff' which creates file
    'u8500_default_cspsa.bin'.
 4) Runs 'cspsalist' to create file 'u8500_default_cspsa.gdf' to allow
    manual inspection of the result.

PFF: Parameter File Format
NWM: Nokia Wireless Modem
GDF: Global Data File
