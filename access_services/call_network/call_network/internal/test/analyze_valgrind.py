#!/usr/bin/python
#
# Copyright (C) ST-Ericsson SA 2011. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

import sys

#
# ANSI escape sequences for color output
#
class color:
	PURPLE = '\033[95m'
	YELLOW = '\033[93m'
	BLUE = '\033[94m'
	GREEN = '\033[92m'
	RED = '\033[91m'
	ENDC = '\033[0m'


#
# Function for printing out header character
# that match the length of a given string.
#
def print_border_char(char, ref_string):
	strlen = len(ref_string)
	s = ""
	for i in range(0,strlen):
		s += char
	print s


#
# Function used to see if a string will match against
# the skip list.
#
def present_in_skip_list(string, skip_list):
	found_entry = False
	for entry in skip_list:
		if -1 != string.find(entry):
			found_entry = True
			break
	return found_entry


#
# This program will parse valgrind output and highlight
# issues with respect to the module test framework output.
# The idea is that this information is added to the end of
# the valgrind output to highlight what should be fixed.
#
def main():
	section = "before testcase execution"
	string = ""
	prev_line = ""
	issue_nr = 0
	skip_list = ["Memcheck", "Copyright", "rerun", "Command:"]

	file = open("valgrind.txt")
	for line in file:
		# Remove line termination char
		line = line.replace('\n','')

		# Stop parsing file when the valgrind summary comes
		if line.startswith("========="):
			return 0

		# Keep track of which testcase is running
		if line.startswith("EXECUTING TESTCASE"):
			string = line.split()
			section = "testcase " + string[2] + " " +  string[3]

		# If a Valgrind print is found (indicated with starting "==" chars
		# then print out section information together with the debug info
		if line.startswith("=="):
			# Ignore lines with no useful information
			if present_in_skip_list(line, skip_list):
				continue
			if 12 > len(line) and section.startswith("before"):
				continue

			if not prev_line.startswith("=="):
				print
				s = "Valgrind issue nr #" + str(issue_nr) + " -> " + section
				print_border_char("=", s)
				print s
				print_border_char("=", s)
				issue_nr += 1
			print color.YELLOW + line + color.ENDC
		prev_line = line
	file.close()

#
# Entry point of program
#
if __name__ == "__main__":
	try:
		main()
	except KeyboardInterrupt:
		sys.exit(1)


