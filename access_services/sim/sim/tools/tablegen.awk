#!/usr/bin/awk -f
######################### -*- Mode: awk -*- ###################################
## Copyright (C) 2010, ST-Ericsson
##
## File name       : tablegen.awk
## Description     : Lookup table generator
##
## Author          : Steve Critchlow <steve.critchlow@stericsson.com>
##
###############################################################################

# This takes the sim.h file, and finds the STE_CAUSE opcodes within it.
# From this, an array of strings is generated which match the enumerations.
# Such arrays are used to "pretty print" enumerations in the test code.

/^[ \t]*STE[A-Z_]+CAUSE_[A-Z]/ {
  a = $1                     # enum name is param 1
  sub("^STE_","",a)          # edit the enum
  gsub("CAUSE_","",a)
  gsub("_"," ",a)
  sub(",$","",a)
  print "  \"" a "\","       # print the string in double quotes.
}
