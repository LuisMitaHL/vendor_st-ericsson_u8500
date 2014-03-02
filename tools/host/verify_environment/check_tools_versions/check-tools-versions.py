#!/usr/bin/env python

import sys
import subprocess
import string
import os.path
import re
import time

# Table of tools to check versions
# application name, version command, expected version value
# you can specify version with =, >, >=, < or <=
# If nothing is specified = is assumed.


#
# Checking mechanism
# For general tools
#     ['commandname']                                          Checks if the tools exists or not by issuing 'which command'
#     ['commandname', 'xxx']                                   Checks if the tools exists or not by issuing 'which command'
#     ['commandname', '-v', '>=3.1', '<=4' ]                   Checks if the tool version is satisfying the required criteria
#     ['packagename', 'package']                               Checks if the package is present or not
#
# Note: You should not provide version check for package, otherwise it will fail.
#
universal_application_table = [
                        [ 'as', '--version', '>=2.18.0.20080103' ],
                        [ 'cat', '--version', '>=6.10' ],
                        [ 'cp', '--version', '>=6.10' ],
                        [ '/bin/echo', '--version', '>=6.10' ],
                        [ 'find', '--version', '>=4.2.32' ],
                        [ 'gcc', '--version', '>=4.2.3'],
                        [ 'grep', '-V', '>=2.5.3' ],
                        [ 'gzip', '--version', '>=1.3.12' ],
                        [ 'install', '--version', '>=6.10' ],
                        [ 'ln', '--version', '>=6.10' ],
                        [ 'ls', '--version', '>=6.10' ],
                        [ 'make', '-v', '>=3.81' ],
                        [ 'mkdir', '--version', '>=6.10' ],
                        [ 'mv', '--version', '>=6.10' ],
                        [ 'perl', '-v', '>=5.8.8' ],
                        [ 'python', '-V', '>=2.5.2' ],
                        [ 'readlink', '--version', '>=6.10' ],
                        [ 'rm', '--version', '>=6.10' ],
                        [ 'sed', '--version', '>=4.1.5' ],
                        [ 'sort', '--version', '>=6.10' ],
                        [ 'tar', '--version', '>=1.19' ],
                        [ 'touch', '--version', '>=6.10' ],
                        [ 'xargs', '--version', '>=4.2.32' ],
                        [ 'uniq', '--version', '>=6.10' ],
                        [ 'zip', '-v', '>=2.32' ],
                        [ 'yes', '--version', '>=6.10' ] ]

non_ste_application_table = [
                        [ 'libusb-dev', 'package' ],
                        [ 'doxygen', 'package' ],
                        [ 'xmlto', 'package' ]
                        ]

# STEricsson platform specific applications.
ste_application_table = [
                        ]
# Android specific applications.
android_application_table = [
                        [ 'java', '-version', '>=1.6' ],
                        [ 'sun-java6-jdk', 'package' ],
                        [ 'git', '--version', '>=1.7'],
                        [ 'git-core', 'package' ],
                        [ 'gnupg', 'package' ],
                        [ 'flex', 'package' ],
                        [ 'bison', 'package' ],
                        [ 'gperf', 'package' ],
                        [ 'build-essential', 'package' ],
                        [ 'zip', 'package' ],
                        [ 'curl', 'package' ],
                        [ 'zlib1g-dev', 'package' ],
                        [ 'libc6-dev', 'package' ],
                        [ 'libc6-dev-i386', 'package' ],
                        [ 'lib32ncurses5-dev', 'package' ],
                        [ 'ia32-libs', 'package' ],
                        [ 'x11proto-core-dev', 'package' ],
                        [ 'libx11-dev', 'package' ],
                        [ 'lib32readline5-dev', 'package' ],
                        [ 'lib32z1-dev', 'package' ],
                        [ 'libgl1-mesa-dev', 'package' ],
                        [ 'gcc-multilib', 'package' ],
                        [ 'g\\\+\\\+-multilib', 'package' ],
                        [ 'mingw32', 'package' ],
                        [ 'tofrodos', 'package' ],
                        [ 'python-markdown', 'package' ],
                        [ 'libxml2-utils', 'package' ],
                        [ 'xsltproc', 'package' ]
                        ]


def show_help():
    print ('Syntax: check_version.pl <tool>')
    print ('if no tool is specified all tools will be checked.')

def parse_operator(version):

  operator = version[0]
  operator += version[1]

  if operator[1] != "=":
    operator = version[0]

  if operator in ["<", ">", "=", "<=", ">="]:
    t_version = re.sub(operator, "", version, 1)
    return (operator, t_version)

  # assume equal if no operator specified.
  return ("=", version)

# This function remove the unnecessary strings and bring the
# version to clean format like 2.1.1
def clean_version_num(version):

    ver = version

    ver = re.sub("v", "", ver) # special case for perl, i.e. v5.8.8
    ver = re.sub("_", "", ver) # special case for java, i.e. 1.5.0_16
    ver = re.sub('"', "", ver) # special case for java, i.e. "1.5.0_16"
    ver = re.sub("\[\'", "", ver) # special case for pkg-config

    if not ver.isdigit():
        for i in range(len(ver)):
            char = ver[i]
            if not char.isdigit():
                if not (char == '.'):
                  ver = ver[:i]
                  break

    return ver

def compare_version(operator, req_version, present_version):

  result = 0

  #This flag will be used when the version is same
  # and the operator is < or >
  same_version = 0

  req_version = re.sub("\*", ".", req_version, 1)

  if operator == "=":
    m = re.search(req_version, present_version)
    if m == None:
      result = -1
  else:
    # version string must contain a '.'
    if present_version.find('.') == -1:
      return -1

    #Clean the version number
    req_num = clean_version_num(req_version)
    present_num = clean_version_num(present_version)

    # Now split the version  number with '.' to compare each bit
    req_num_parts = req_num.split('.')
    present_num_parts = present_num.split('.')

    # If the length are not same then we need to pad with '0'
    # so that we can do a comparision
    #
    # For example
    #        Req Version | Giv Version | Formatted Req version | Formatted Giv Version
    #
    #          2.1         2.1.1.1        2.1.0.0                2.1.1.1
    #          2.1.1.1     2.1            2.1.1.1                2.1.0.0
    if (len(req_num_parts) != len(present_num_parts)):
       while len(req_num_parts) > len(present_num_parts):
             present_num_parts = present_num_parts + ['0']

       while len(present_num_parts) > len(req_num_parts):
             req_num_parts = req_num_parts + ['0']

    #Start actual test of comparing the version
    for i in range(len(req_num_parts)):
        try:
           present_num = int(present_num_parts[i])
           req_num = int(req_num_parts[i])
        except:
           return -1

	#If both are same no need to compare, just continue to the next bit
        if (present_num == req_num):
           result = 0
           same_version = 1
           continue

        #reset the flag
        same_version = 0
        result = 0;

        #Compare based on the given operator
        if (operator == ">") or (operator == ">="):
          # The given version should be greater than the required, else wrong
          if (present_num < req_num):
            result = -1
            break
	  # Else is required to break the for loop, as the given version is
          # already greater than the required version, no need of further comparision
          else:
            result = 0
            break

	elif (operator == "<") or (operator == "<="):
          # The required version should be greater than the given, else wrong
          if (present_num > req_num):
            result = -1
            break
	  # Else is required to break the for loop, as the given version is
          # already less than the required version, no need of further comparision
          else:
            result = 0
            break

  # Two versions are same, so we need to do a final check for the type of operator
  if (result == 0) and (same_version == 1):
    if (operator == ">") or (operator == "<"):
      result = -1

  return result

def version_check(app, vercmd, version):
  cmd = app + ' ' + vercmd

  run = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
  out, err = [ e.splitlines() for e in run.communicate() ]
  run.wait()

  # Some applications doesn't return 0 when given version parameter.
  if not run.returncode == 0 and not run.returncode == 5 and not run.returncode == 2:
    return -1

  if(len(out) == 0):
    out = repr(err)
  else:
    out = repr(out)

  (operator, version) = parse_operator(version)
  ver_split = out.split()

  found = False
  for word in ver_split:
    word = re.sub("\'\,", "", word)
    word = re.sub("\'\]", "", word)
    if re.search('[0-9]+[\.]{1}[0-9]+', word) != None:
      # Only check version on "version" numbers
      # Only check the first version number found.
      if compare_version(operator, version, word) == 0:
        found = True
      break

  if not found:
    return -1

  return 0

def package_check(pkgtype, pkg):
  failure = 0
  cmd = '%s --get-selections | awk \'/\'%s\'[ \t]\'+install\'/\'' % (pkgtype, pkg)

  run = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
  out, err = [ e.splitlines() for e in run.communicate() ]
  run.wait()

  if(len(out) == 0):
    pkg = re.sub('\\\+','',pkg)
    print 'Package %s is not present' % (pkg)
    print 'Run \'sudo apt-get install %s\' to install the package' % (pkg)
    failure = -1

  return failure

#
#Check the tool is installed or not by sending a 'which commandname'
#
def tool_install_check(toolname):
  isInstalled = 0

  command = 'which' + ' ' + toolname
  run = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
  out, err = [ e.splitlines() for e in run.communicate() ]
  run.wait()

  if(len(out) == 0):
    print '%s is not present' % (toolname)
    isInstalled = -1

  return isInstalled

def tool_or_package_check(command, commandtype):
  if (commandtype == 'package'):
    ret = package_check('dpkg', command)
  else:
    ret = tool_install_check(command)

  return ret;


def parse_table(specapp, table):
    failure = 0
    for app in table:
        cmd = app[0]

        if (cmd == specapp) or (specapp == 'all'):

	    #Check for the tool/package is installed or not, if no version information is provided
	    if len(app) <= 2:
                ver_cmd = "--version"

		if len(app) > 1:
        	   ver_cmd = app[1]

		ret = tool_or_package_check(cmd, ver_cmd)

		if ret == -1:
		  failure = 1

	    #Check for the first version comparision
  	    if len(app) > 2:
              ver_cmd = app[1]
              version_greater = app[2]
              ret = version_check(cmd, ver_cmd, version_greater)
              if ret == -1:
                 print '%s %s is not present. ' % (cmd, version_greater )
                 failure = 1

	    #Check for the second version
	    if (failure != 1) and (len(app) > 3):
               version_less = app[3]
               ret = version_check(cmd, ver_cmd, version_less)
	       if ret == -1:
                   print '%s %s is not present. ' % (cmd, version_less)
                   failure = 1
    return failure

def OSType():
  cmd = 'uname -m'
  run = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
  out, err = [ e.splitlines() for e in run.communicate() ]
  run.wait()

  return repr(out)

def main():
  failure = 0
  specapp = 'all'
  found = False
  thirdparty = True
  showhelp = False

  arguments = sys.argv
  arguments.pop(0) # Remove name of application from arguments

  print ('\n')

  if sys.version < '2.4':
    print 'This script needs Python2.4 or later'
    sys.exit(1)

  if len(arguments) > 0:
    for arg in arguments:
        if arg == '-h':
            showhelp = True
        else:
            specapp = arg

  if len(arguments) > 2 or showhelp:
    show_help()
    sys.exit(1)

  build_arch = OSType()
  if build_arch.find('64') == -1:
	  print('WARNING:')
	  print('You are attempting to build on a 32-bit system.')
	  print('Only 64-bit build environments are supported beyond froyo/2.2.')

  res_linux = parse_table(specapp, universal_application_table)
  if res_linux > 0:
      failure = 0
      print('WARNING:')
      print('Some required tool(s) were not found on your system.')
      print('Please contact your system administrator.\n')
      time.sleep (2)

  res_ste = parse_table(specapp, ste_application_table)
  if res_ste > 0:
      failure = 0
      print('WARNING:')
      print('Some required tool(s) were not found in your copy of the STE platform.')
      print('This indicates that either your copy of the STE platform is incomplete or')
      print('that the setup of the platform has not been run correctly.\n')
      time.sleep (1)

  res_android = parse_table(specapp, android_application_table)
  if res_android > 0:
      failure = 0
      print('WARNING:')
      print('Some required Android related tool(s) were not found on your system.')
      print('Please contact your system administrator.\n')
      time.sleep (1)

  res_3rd = parse_table(specapp, non_ste_application_table)
  if res_3rd > 0:
      failure = 0
      print('WARNING:')
      print('Some required tool(s) were not found.')
      print('The 3rd party components may not build correctly.\n')
      time.sleep (1)


  sys.exit(failure)

if __name__ == '__main__':
  try:
    main()
  except KeyboardInterrupt:
    sys.exit(1)
