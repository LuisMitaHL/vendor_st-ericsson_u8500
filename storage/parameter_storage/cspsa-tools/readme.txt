--------------------------------------------------------------------------------
cspsa2nwm Copyright (C) 2010 ST-Ericsson AB
--------------------------------------------------------------------------------

This software is released either under the terms of the BSD-style
license accompanying CSPSA or a proprietary license obtained from
ST-Ericsson SA.

"cspsa2nwm" is a target tool that acts as a CSPSA client in order to read out
the NWM files that were stored inside a CSPSA area by flash-kit. These files are
stored in the root file system.

First cspsa2nwm establishes a connection to a CSPSA area by opening the named
area. The next step is checking if the ITP update flag is set. If this flag is set
cspsa2nwm starts reading NWM files in the form of parameters from CSPSA and
writes them to the file system (normally to the path /modemfs). Once all files
have been successfully copied the ITP update flag is cleared.

Usage: cspsa2nwm [-r rootdir] [-f] [-d] [-g guardFile] [-p path] [-p ...] -u <m:n> cspsaName
 -h           Prints this help.
 -g guardFile Name of the guard file. If this file exists, nothing will be
              done unless -f is specified. After copying, the guard file
              is created. The root dir will be concatenated with the guard file name
              before the file is created.
 -u <m:n>     NWM configuration structure is updated if bit n in CSPSA item
              m is set. Value m must be a 32-bit hexadecimal value, while n must
              must be a decimal value from 0 to 31.
 -f           Force copying of CSPSA parameters to files, even if the update bit
              is not set or the guard file exists
 -r rootdir   Create all files relative to the root dir. Default is /modemfs
 -d           Enable debug logging (to syslog)

 -dd          Enable debug logging (to syslog + stdout)

 -p path      Only files on the specified path with be copied from CSPA to file system. There may be
              several -p options specified, each allowing a specific path to be copied.

--------------------------------------------------------------------------------
nwm2gdf Copyright (C) 2010 ST-Ericsson AB
--------------------------------------------------------------------------------

This software is released either under the terms of the BSD-style
license accompanying CSPSA or a proprietary license obtained from
ST-Ericsson SA.

"nwm2gdf" is a PC Linux tool that parses a directory containing NWM
configuration files and produces a gdf formatted file. The gdf file contains the
sub-directory structure, and the all its files and file information, according
to the NWM GDF format.

The generated gdf file can be loaded to target, and its contents is stored into
CSPSA. At startup of the U8500 there is a CSPSA client (cspsa2nwm) that reads
out the NWM files (if necessary) and stores them to a directory in the file
system.  The original directory tree is fully re-created with original access
rights settings.  These files can then be read by the modem (via MSA) to
configure itself.

Usage: nwm2gdf [options] <directory-path>

Where [options] may be:
	-o file	name of output file, in .gdf format
	-d	enable debug logs

--------------------------------------------------------------------------------
nwm2cspsa Copyright (C) 2012 ST-Ericsson AB
--------------------------------------------------------------------------------

This software is released either under the terms of the BSD-style
license accompanying CSPSA or a proprietary license obtained from
ST-Ericsson SA.

"nwm2cspsa" is a target tool that parses one or more directory containing
NWM configuration files and stores these in the specified CSPSA area.

The tool is used when HATS has calibrated the modem and needs to write
the parameters to CSPSA.

"nwm2cspsa" first processes the directories specified and loads all file
metadata and parameter data into RAM. It then opens the CSPSA area and
zeroes key 0, so that an interrupted write to CSPSA will not result in a
state of mixed parameters. After this, the metadata and data is written
to CSPSA and lastly key 0 is updated with the number of stored parameters.

The ITP update flag is not touched.

Usage: nwm2cspsa [options] -c <CSPSA-area> <directory-path> [<subpath> ...]

Where [options] may be:
        -d      enable debug logs

<directory-path> specifies the root directory below which /modemfs is located.
If one or more <subpath> is specified, only files and directories below this
<subpath> are parsed. All directory components of <subpath> are also included.

--------------------------------------------------------------------------------
gdflist Copyright (C) 2010 ST-Ericsson AB
--------------------------------------------------------------------------------

This software is released either under the terms of the BSD-style
license accompanying CSPSA or a proprietary license obtained from
ST-Ericsson SA.

"gdflist" is a PC Linux tool that parses a gdf file generated according to the
NWM GDF format and lists its contents.

Usage: gdflist [options] <gdf-file>

Where [options] may be:
	-d	enable debug logs

--------------------------------------------------------------------------------
cspsa2map Copyright (C) 2011 ST-Ericsson AB
--------------------------------------------------------------------------------

This software is released either under the terms of the BSD-style
license accompanying CSPSA or a proprietary license obtained from
ST-Ericsson SA.

"cspsa2map" is a target tool that is used during startup together with map2q to
(when needed) restore/update modem calibration parameters from the backup in the
cspsa area.

Usage: cspsa2map -c cspsa_name -o mapfile -u m:n [-h]
 -h              Prints this help.
 -c cspsa_name   The name of the parameter storage area to read.
 -u <m:n>        Conversion is only done if bit n in CSPSA item
                 m is set. Value m must be a 32-bit hexadecimal value, while n must
                 be a decimal value from 0 to 31. This bit is reset after conversion.
 -f              Force conversion of CSPSA parameters to files, even if the update bit
                 is not set
 -x              Only read calibration parameters from CSPSA.
 -o mapfile      The out MAP/GDF file with data read from parameter storage.

--------------------------------------------------------------------------------
map2q Copyright (C) 2011 ST-Ericsson AB
--------------------------------------------------------------------------------

This software is released either under the terms of the BSD-style
license accompanying CSPSA or a proprietary license obtained from
ST-Ericsson SA.

"map2q" is a target tool that is used during startup together with cspsa2map to
(when needed) restore/update modem calibratetion parameters from the backup
in the cspsa area.

Usage: map2q <input_file> [-d] [-v] [-s<variable name>] [-m<merge_file> -c<calib_list_file>] <output_file>
 Parameters (all optional):\n");
 -d               Gives some extra detailed information during execution.
 -v               Gives progress information during executon.
 -s               Data in output file will be C source code with variable name <variable name> for the array.
                  Data format is the same as q-file
 -m               Specifies merge_file of /map/gdf/ type containing calibration data.
 -c               Specifies a file containing a list of parameters being calibration parameters.

 <input_file> is type /map/gdf/ file. <output_file> is a binary file of q-file format.
 When option '-m' is used, the parameter '-c' must also be specifed.

---------------------------------------------------------------------------------
map2cspsa Copyright (C) 2011 ST-Ericsson AB
---------------------------------------------------------------------------------

This software is released either under the terms of the BSD-style
license accompanying CSPSA or a proprietary license obtained from
ST-Ericsson SA.

"cspsa2map" is a target tool that is used together with q2map after modem calibration
to store a backup of the calibrated parameters in cspsa.

Usage: map2cpsa -c cspsa_name -i mapfile [-h]
 -h              Prints this help.
 -c cspsa_name   The name of the parameter storage area to read.
 -i mapfile      The input MAP/GDF file

--------------------------------------------------------------------------------
q2map Copyright (C) 2010 ST-Ericsson AB
--------------------------------------------------------------------------------

This software is released either under the terms of the BSD-style
license accompanying CSPSA or a proprietary license obtained from
ST-Ericsson SA.

"q2map" is a target tool that is used together with map2cspsa after modem calibration
to store a backup of the calibrated parameters in cspsa.

Usage: q2map <input_file> [-d] [-v] [-f] <output_file>
 Parameters (all optional):
 -d              Gives some extra detailed information during execution.
 -v              Gives progress information during execution.
 -f              Discards checksum error and tries to read file anyway.

 <input_file> is q-file format data file. <output_file> is a GDFS map file.


--------------------------------------------------------------------------------
q2cspsa Copyright (C) 2012 ST-Ericsson AB
--------------------------------------------------------------------------------

This software is released either under the terms of the BSD-style
license accompanying CSPSA or a proprietary license obtained from
ST-Ericsson SA.

"q2cspsa" is a target tool that is used to store a backup of the modem calibration parameters in cspsa.

Usage: q2cspsa -c <cspsa_name> -i <input_file> -k <cspsa_key> -u <m:n> [-h]
 -h               Prints this help
 -c <cspsa_name>  The name of the parameter storage area to write.
 -i <input_file>  The input q-file.
 -k <cspsa_key>   Key value for input file in CSPSA area.
                  (see value in cspsa_image/pff/0x00000000-0x0000FFFF.modem)
 -u <m:n>         Conversion is only done if bit n in CSPSA item
                  m is set. Value m must be a 32-bit hexadecimal value, while n must
                  be a decimal value from 0 to 31. This bit is reset after conversion.


--------------------------------------------------------------------------------
cspsa2q Copyright (C) 2012 ST-Ericsson AB
--------------------------------------------------------------------------------

This software is released either under the terms of the BSD-style
license accompanying CSPSA or a proprietary license obtained from
ST-Ericsson SA.

"cspsa2q" is a target tool that is used during startup to
restore/update modem calibration parameters from the backup in the
cspsa area.

Usage: cspsa2q -c <cspsa_name> -o <output_file> -k <cspsa_key> -u <m:n> -m <cspsa_key> [-d] <path> [-h]
 -h               Prints this help
 -c <cspsa_name>  The name of the parameter storage area to write.
 -o <output_file> The output q-file.
 -k <cspsa_key>   Key value for output file in CSPSA area.
                  (see value in cspsa_image/pff/0x00000000-0x0000FFFF.modem)
 -u <m:n>         Conversion is only done if bit n in CSPSA item
                  m is set. Value m must be a 32-bit hexadecimal value, while n must
                  be a decimal value from 0 to 31. This bit is reset after conversion.
 -m <cspsa_key>   Flag to check the modem calibraration validity.
                  Conversion is only done if bit n in CSPSA item is set.
                  (see value in cspsa_image/pff/0xFFFFFFFC-0xFFFFFFFF.fast_parameters)
                  Value of g must be a 32-bit hexadecimal value.

Optional:
[-f]              Force conversion of CSPSA parameters to files, even if the update bit
                  is not set.
[-d] <path>       Enable to get the log info from the CSPSA area.

