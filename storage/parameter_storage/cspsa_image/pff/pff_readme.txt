Parameter File Format
=====================

Copyright (C) 2009 ST-Ericsson AB

PFF is an acronym for Parameter File Format.

The parameter file format is a way to store parameters in a generic file
format. The advantage is that one can use common tools in Linux or windows
to parse the file structure and format. Also it is easy to use git or other
version control systems to keep track of changes in parameters.

PFF File Structure
------------------

The PFF file structure is quite simple and generic. Each sub-directory name
consists of a range and a name. The range indicates the range of parameter
values that can be found under the sub-directory. The name indicates what type
of parameters can be found under this sub-directory. Sub-directories may be
nested but may not contain both sub-directories and parameter items. Each parameter
item consists of three parts: key, name and format.

Example of a PFF file structure:

<DIR> means PFF directory
<VAL> means PFF parameter value
<IGN> means ignored file
Note: all directory and file names in this example are arbitrarily choosen.

- - -
0x00000000-0x0000FFFF.modem                     <DIR>
        0x00000000-0x00000FFF.radio             <DIR>
                0x00000003.adam.bin             <VAL>
                0x0000000A.bertil.hex           <VAL>
                0x00000017.ceasar.txt           <VAL>
        0x00001000-0x0000FFFF.bluetooth         <DIR>
                0x00001043.david.bin            <VAL>
                0x0000101A.erik.hex             <VAL>
                0x000012b7.filip.txt            <VAL>
        comments.txt                            <IGN>
0x00010000-0x01FFFFFF.app                       <DIR>
        0x00010000.gunnar.bin                   <VAL>
        0x00011000.helge.hex                    <VAL>
        0x00012000.ivar.txt                     <VAL>
        0x00013000.julius.txt                   <VAL>
        comments_to_users_of_this_folder.txt    <IGN>
0x02000000-0x02FFFFFF.other                     <DIR>
0x02000000-0x02FFFFFF.other                     <DIR>
some_text_file.txt                              <IGN>
- - -


A sub-category may either include more sub-categories or parameter values, not
both.

PFF File Item
-------------

As stated above a parameter file item name consists of three parts: key, name and
format. The format can be one of {bin, hex, txt} which represent that a file can
be in either binary, hexadecimal or text format.

The binary format and the text format will be loaded "as is". Thus no conversion
is made.

A file that contains no data will not be stored in CSPSA, but could be used
to reserve parameter keys or mark obsolete parameters.

A file of hexadecimal values may contain one or several values in hexadecimal
form. It is optional to use prefix "0x". A value must be either 8, 16 or 32 bits,
and the values will be stored accordingly in CSPSA. Hex value of two digits
represent an 8 bit value, four digits a 16 bit value and eight digits a 32 bit
value. Space, tabs and new line characters are ignored. The character '#' marks
the rest of a line as a comment.

Example of hexadecimal file:
- - -
# One may include comments about values
# Value A       Value B         Value C
0x12            0x1234          0x12345678
0x11 0x22 0x33 # Descr a, b and c
0x1234 0x87654321
0x87654321 0x11 0x1234 0x22 0x12345678
12            1234          12345678
# Example comment: 0x1234 0x87654321
- - -


PC Tools
========

pff2cspsa
---------

The PC tool "pff2cspsa" parses a PFF structure to create a CSPSA binary
image which may be loaded directly as a binary image into a ME.

Synopsis: pff2cspsa [-d] [-f <file name>] -z <size> <file name>

Where:
  -d    Enable debug printouts
  -f    Sets the output file name
  -z    Sets the size of the CSPSA area and should be set to the same
        size as the CSPSA area on target. Otherwise one will not get an error if the
        generated image is too big.

Example:

./pff2cspsa -f cspsa_example.bin -z 0x100000 pff_example

gdf2pff
-------

The PC tool "gdf2pff" parses a GDF file and creates corresponding PFF files.

Synopsis: gdf2pff [-d] [-x] [-t] [-n <name>] [-p <path>] <file name>

Where:
  -d        Enable debug printouts
  -x        Create parameter files containing hex data instead of binary data.
  -t        Create text files if all data is in the printable range of ASCII
  -n name   Sets the common name of the generated files
  -p        Set the output path of the generated files

Example:

./gdf2pff -n nwm -p testout -x -t cspsa_default_data.gdf

Will parse all items in file 'cspsa_default_data.gdf' and generate files in
hex format, or text format if suitable, and stores them in folder 'testout'.
Files are named 'nwm', example:

        0x00000000.nwm_000B.hex
        0x00000001.nwm_0001.hex
        0x00000002.nwm_0002.hex
        0x00000003.nwm_0003.hex
        0x00000004.nwm_0004.hex
        0x00000005.nwm_0005.hex
        0x00000006.nwm_0007.hex
        0x00000007.nwm_0008.hex
        0x00000008.nwm_000A.hex
        0x000003E8.nwm_0000.hex
        0x000003ED.nwm_0006.hex
        0x000003EF.nwm_0009.hex
        0xFFFFFFFD.nwm_000C.hex
