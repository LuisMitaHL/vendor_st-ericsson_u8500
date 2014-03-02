############################################################################################################
# This script is used to perform sanity checks in the *.c and *.h files so as to ensure that all the       #
# functions are documented according to DOXYGEN Coding Guidelines.                                         #
#                                                                                                          #
# It checks that                                                                                           #
#   * every doxygen comment has a ingroup command in it if defgroup is not defined                         #
#   * every function documentation has a brief or details, callgraph, callergraph and return command       #
############################################################################################################

import os
import sys
import time

def UpdateFunctionList(Line, LineNum, File):
    Line = Line.strip().strip('*').strip()[3:].strip()
    Words = Line.split()
    if Words[0] == 'static':
        # TBD: Why using a static function is an issue?
        print("\n\nFile: " + str(File))
        print("\nEncountered a static function at line " + str(LineNum))

        ReportFile.write("\n\nFile: " + str(File))
        ReportFile.write("\nEncountered a static function at line " + str(LineNum))

        print "\nDoxygen Sanity Checks: Exiting (please check)"
        sys.exit(2)

    ReturnType = Words[0]
    Line = Line.strip(ReturnType).strip()
    FunctionName = Line[:Line.find('(')].strip()
    FunctionListFile.write("\nFunctionName = " + str(FunctionName) + "; ReturnType = " + str(ReturnType))

################################################################################################
#      The script starts from here                                                             #
################################################################################################

print "\nDoxygen Sanity Checks: Starting ..."

DirPathName = sys.argv [1]
ModuleName = sys.argv[2]
ReportDirPathName  = sys.argv[3]
ModulesToBeSkipped = sys.argv[4] # Contains the list of modules to be skipped; the module mames must be separated by commas with no spaces
Mode = sys.argv[5]

if os.name == "nt":
    PATH_DELIM = "\\"
else:
    PATH_DELIM = "/"

################################################################################################
#      Scanning the Project Directory                                                          #
################################################################################################

ReportFileName       = "Doxygen_Report.txt"
FunctionListFileName = "Doxygen_FunctionList.txt"

ReportFilePath       = ReportDirPathName + PATH_DELIM + ReportFileName
FunctionListFilePath = ReportDirPathName + PATH_DELIM + FunctionListFileName

ReportFile = open(ReportFilePath, "wb")
FunctionListFile = open(FunctionListFilePath, "wb")

ReportFile.write("\n## This is a Doxygen Report File.")
ReportFile.write("\n## It reports, if there is any information missing in the Doxygen Comment.")
ReportFile.write("\n##")
ReportFile.write("\n## It checks that")
ReportFile.write("\n##   * every doxygen comment has a ingroup command in it if defgroup is not defined")
ReportFile.write("\n##   * every function documentation has a brief or details, callgraph, callergraph and return command")
ReportFile.write("\n##")
ReportFile.write("\n## Discrepancies found are listed below:")
ReportFile.write("\n##\n")
ReportFile.write("\n## Scanning Module -> " + str(ModuleName) + "\n")

FunctionListFile.write("\n## This is a Doxygen Function List File.")
FunctionListFile.write("\n## It lists all the functions that have been checked by DoxygenSanityChecks.py script; this is just for debug purpose")
FunctionListFile.write("\n##\n")

d=()
index = 0
count = 0
FileName = ""
FilePath = ""
ListOfDirectoryTuples = []
ListOfFilePaths = []
ListOfToBeSkippedModuleDirectoryTuples = []
ListOfModulesToBeSkipped = []
ListOfToBeSkippedModulePaths = []
ListOfToBeSkippedModuleFilePaths = []

# Collect list of each directory and its contents in ListOfDirectoryTuples as (dirpath, dirnames, filenames).
if Mode == "ModuleLevel":
    for d in os.walk(DirPathName + PATH_DELIM + ModuleName):
        ListOfDirectoryTuples.append(d)
else:
    for d in os.walk(DirPathName):
        ListOfDirectoryTuples.append(d)

for index in range(len(ListOfDirectoryTuples)):
    for count in range(len(ListOfDirectoryTuples[index][2])):
        FileName = str(ListOfDirectoryTuples[index][2][count])
        FilePath = str(ListOfDirectoryTuples[index][0]) + PATH_DELIM + FileName
        if FilePath.endswith('.c') or FilePath.endswith('.h'):
            ListOfFilePaths.append(FilePath)


# Find out the names of the files to skip
if Mode == "ProjectLevel":
    ListOfModulesToBeSkipped = ModulesToBeSkipped.split(',')

    for index in ListOfModulesToBeSkipped:
        ListOfToBeSkippedModulePaths.append(ListOfDirectoryTuples[0][0] + PATH_DELIM + ListOfDirectoryTuples[0][1][ListOfDirectoryTuples[0][1].index(index)])

    for index in ListOfToBeSkippedModulePaths:
        for d in os.walk(index):
            ListOfToBeSkippedModuleDirectoryTuples.append(d)

    for index in range(len(ListOfToBeSkippedModuleDirectoryTuples)):
        for count in range(len(ListOfToBeSkippedModuleDirectoryTuples[index][2])):
            FileName = str(ListOfToBeSkippedModuleDirectoryTuples[index][2][count])
            FilePath = str(ListOfToBeSkippedModuleDirectoryTuples[index][0]) + PATH_DELIM + FileName
            if FilePath.endswith('.c') or FilePath.endswith('.h'):
                ListOfToBeSkippedModuleFilePaths.append(FilePath)

GlobalDefineGroupCommandPresent = 0
DiscrepancyFound = 0

for File in ListOfFilePaths:
    if File in ListOfToBeSkippedModuleFilePaths:
        pass
    else:
        # Read the contents of the file in ListOfInputs
        InputFile = open(File,"r")
        ListOfInputs = InputFile.readlines()
        InputFile.close()

        DoxyGenCommentFound = 0

        LineCount = 1
        LineStart = 0
        LineEnd = 0

        # TBD: Not used for the moment
        FileCommandPresent = 0

        DefineGroupCommandPresent = 0

        IngroupCommandFound = 0

        InsideFunctionDocumentation = 0
        CallGraphCommandFound = 0
        CallerGraphCommandFound = 0
        ReturnCommandFound = 0
        FunctionDocumentationPresent = 0

        UnbalancedIfStatement = 0
        
           
        for Line in ListOfInputs:
            # TBD: Basic assumption here is that way the comments are written is correct as per C syntax
            #
            #
            # Comment style
            #
            # /**
            #  * Line 1
            #  * Line 2
            #  */
            #
            # or
            #
            # /****
            #  * Line 1
            #  * Line 2
            #  ****/
            if (Line.strip()).startswith('/**') == True or (Line.strip()).startswith('/*') == True: # TBD: Is the second startswith syntax as per doxygen?
                DoxyGenCommentFound = 1
                LineStart = LineCount
                LineEnd = 0
            else:
                if (DoxyGenCommentFound == 1):
                    LineEnd = LineEnd + 1

                    if (Line.strip('* ')).startswith('\ingroup') == True:
                        IngroupCommandFound = 1
                    elif (Line.strip('* ')).startswith('\\fn') == True:
                        UpdateFunctionList(Line, LineStart + LineEnd, File)
                        InsideFunctionDocumentation = 1
                    elif (Line.strip('* ')).startswith('\callgraph') == True and InsideFunctionDocumentation == 1 :
                        CallGraphCommandFound = 1
                    elif (Line.strip('* ')).startswith('\callergraph') == True and InsideFunctionDocumentation == 1 :
                        CallerGraphCommandFound = 1
                    elif (Line.strip('* ')).startswith('\\return') == True and InsideFunctionDocumentation == 1 :
                        ReturnCommandFound = 1
                    elif ((Line.strip('* ')).startswith('\\brief') == True or (Line.strip()).startswith('\details') == True )and InsideFunctionDocumentation == 1 :
                        FunctionDocumentationPresent = 1
                    elif ((Line.strip('* ')).startswith('\if')):
                        UnbalancedIfStatement = UnbalancedIfStatement + 1
                    elif (Line.strip('* ')).startswith('\endif'):
                        UnbalancedIfStatement = UnbalancedIfStatement - 1
                    elif (Line.strip('* ')).startswith('\\file'):
                        FileCommandPresent = 1
                    elif (Line.strip('* ')).startswith('\defgroup'):
                        DefineGroupCommandPresent = 1
                        GlobalDefineGroupCommandPresent = 1
                    elif (Line.strip()).startswith('*/') == True or (Line.strip()).endswith('*/') == True: # TBD: Is the endswith syntax as per doxygen?

                        DoxyGenCommentFound = 0

                        if (IngroupCommandFound == 0 and DefineGroupCommandPresent == 0):
                            ReportFile.write("\n\nFile: " + str(File))
                            ReportFile.write("\nCommand missing: ingroup between lines " + str(LineStart) + " and " + str(LineStart + LineEnd))
                            DiscrepancyFound = 1
                        if CallGraphCommandFound == 0 and InsideFunctionDocumentation == 1:
                            ReportFile.write("\n\nFile: " + str(File))
                            ReportFile.write("\nCommand missing: callgraph between lines " + str(LineStart) + " and " + str(LineStart + LineEnd))
                            DiscrepancyFound = 1
                        if CallerGraphCommandFound == 0 and InsideFunctionDocumentation == 1:
                            ReportFile.write("\n\nFile: " + str(File))
                            ReportFile.write("\nCommand missing: callergraph between lines " + str(LineStart) + " and " + str(LineStart + LineEnd))
                            DiscrepancyFound = 1
                        if ReturnCommandFound == 0 and InsideFunctionDocumentation == 1:
                            ReportFile.write("\n\nFile: " + str(File))
                            ReportFile.write("\nCommand missing: return between lines " + str(LineStart) + " and " + str(LineStart + LineEnd))
                            DiscrepancyFound = 1
                        if FunctionDocumentationPresent == 0 and InsideFunctionDocumentation == 1:
                            ReportFile.write("\n\nFile: " + str(File))
                            ReportFile.write("\nCommand missing: brief or details between lines " + str(LineStart) + " and " + str(LineStart + LineEnd))
                            DiscrepancyFound = 1
                        if (UnbalancedIfStatement <> 0):
                            ReportFile.write("\n\nFile: " + str(File))
                            ReportFile.write("\nCommand missing: if or endif between lines " + str(LineStart) + " and " + str(LineStart + LineEnd))
                            DiscrepancyFound = 1

                        IngroupCommandFound = 0
                        
                        InsideFunctionDocumentation = 0
                        CallGraphCommandFound = 0
                        CallerGraphCommandFound = 0
                        ReturnCommandFound = 0
                        FunctionDocumentationPresent = 0

                        UnbalancedIfStatement = 0

            LineCount = LineCount + 1

        if (FileCommandPresent == 0):
            ReportFile.write("\n\nFile: " + str(File))
            ReportFile.write("\nCommand missing: file")
            DiscrepancyFound = 1
            
if (GlobalDefineGroupCommandPresent == 0):
    ReportFile.write("\n\nCommand missing: defgroup in the project")
    DiscrepancyFound = 1

if DiscrepancyFound == 0:
    ReportFile.write("\nNo Discrepancies Found\n")

ReportFile.write("\n\n## Doxygen Sanity Checks: Done")

ReportFile.close()
FunctionListFile.close()

print "\nDoxygen Sanity Checks: Done"

    
