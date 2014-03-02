#! /usr/bin/env python

"""Extract the data from an ADM sqlite file and generate text files.

usage: extract_db.py <dbfile> <output_path>
dbfile      -- the adm sqlite file. Default: adm.sqlite
output_path -- path to the output foler. Default: current folder

NOTE: StructDef table is omitted since this data is
      generated when building the ADM.
"""

import sys, os, sqlite3, extract_hw_db

DEFAULT_DBFILE = 'adm.sqlite'
DEFAULT_OUTPUT_PATH = '.'
MAIN_DATA_FILE = "main_data.txt"

dbfile = DEFAULT_DBFILE
output_path = DEFAULT_OUTPUT_PATH + '/'

nbr_of_args = len(sys.argv)
if len(sys.argv) >= 2:
    dbfile = sys.argv[1]
if nbr_of_args >= 3:
    output_path = sys.argv[2]
    if sys.argv[2][-1] != '/':
        output_path += '/'

if not os.path.exists(dbfile):
    print "Exit. Can't find file: " + dbfile
    sys.exit()
    
if not os.path.exists(output_path):
    os.mkdir(output_path)

# Read the database line by line, store in admdata list
con = sqlite3.connect(dbfile)
admdata = []
for line in con.iterdump():
    admdata.append(line)
con.close()

# Sort the list alphabetically
admdata.sort()

output_file = open(output_path + MAIN_DATA_FILE, 'wb')
hw_data = []

for line in admdata:

    if 'INSERT INTO "StructDef"' in line:
        # Ignore 'StructDef' data.
        pass
    elif 'INSERT INTO "HW_Settings' in line:
        # Store 'HW_Settings' data for later use
        hw_data.append(line)
    elif 'INSERT INTO' in line:
        output_file.write(line + '\n')
# Uncomment this to make all lines are written to file.
# For debugging purposes, or when script becomes out-of-date
#    elif:
#        output.file.write(line + '\n')


output_file.close()

# Create hw settings files
extract_hw_db.create_hw_settings(output_path, hw_data)
