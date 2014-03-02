###################################################################
#
#   File Name       : PreBuild.py
#   Author          : Rohit Upadhyay (MMC-Imaging, Greater NOIDA)
#   Functionality   : Page element creation script for 8500
#
#
#
###################################################################

# Date  09/08/2007: - Initial Release.




import sys
import string
import os
import getopt
import sys
import time


def Usage():
    print "Invalid input options"
    print "Usage : "
    print "Relevent Options:"
    print "================="
    print "-t (required): Specifies the name of the top level project code directory"
    print "-m (required): Specifies the name of the Project or Module"
    print "-s (required): Specifies the name of the module to be skipped"
    print "-M (required): Specifies the mode of  working to the 8500Script. Must be specified as 'ModuleLevel' for module level compilation and 'ProjectLevel' for top level project compilation"
    print "-o (required): Specifies the absolute path of DeviceParams and Reports directories"
    print "-b (required if mode is 'ProjectLevel'): Specifies the base address of the ISP registers"
    print "-c (required if mode is 'ProjectLevel'): Specifies the base address of the CRM registers"
    print "-i (required if mode is 'ProjectLevel'): Specifies the base address of the IPP registers"
    print "-a (optional): Specifies whether the Pre Build file should generate the Pictor & CRM map files. Set to 1 for generating files, and to 0 for not generating. Default is 0"
    print "-d (optional): Specifies whether the scripting has to be done in debug mode."

def main(argv):

    TopLevelCodeDirectory = ""
    ModuleName = ""
    ModuleToBeSkipped = ""
    ProjectName = ""
    Mode = ""
    ISP_RegMemBase = '0xFFFFFFFF'
    CRM_RegMemBase = '0xFFFFFFFF'
    IPP_RegMemBase = '0xFFFFFFFF'
    Generate_MapFiles = '0'
    Debug_Script = '0'
    BuildGenDir = ""

    try:
        opts, args = getopt.getopt(argv, "t:m:s:M:o:b:c:a:i:d:")
    except getopt.GetoptError:
        print argv
        Usage()
        sys.exit(2)

    print argv

    for opt, arg in opts:
        if opt == '-t':
            TopLevelCodeDirectory = arg
        elif opt == '-m':
            ModuleName = arg
            ProjectName = arg
        elif opt == '-s':
            ModuleToBeSkipped = arg
        elif opt == '-M':
            Mode = arg
        elif opt == '-o':
            BuildGenDir = arg
        elif opt == '-b':
            ISP_RegMemBase = arg
        elif opt == '-c':
            CRM_RegMemBase = arg
        elif opt == '-i':
            IPP_RegMemBase = arg
        elif opt == '-a':
            Generate_MapFiles = str(arg)
        elif opt == '-d':
            Debug_Script = str(arg)


    if TopLevelCodeDirectory == "" or ModuleName == "" or Mode == "" or ModuleToBeSkipped == "" or BuildGenDir == "":
        Usage()
        sys.exit(2)

    if ((Mode == 'ProjectLevel') and ((ISP_RegMemBase == '0xFFFFFFFF') or (CRM_RegMemBase == '0xFFFFFFFF') or (IPP_RegMemBase == '0xFFFFFFFF'))):
        Usage()
        sys.exit(2)

    # get the current working directory
    CurrentWorkingDirectory = os.getcwd()

    # invoke the Pictor hwreg def generator script if the mode is "ProjectLevel"
    if (Mode == 'ProjectLevel') and (Generate_MapFiles == '1'):
        os.chdir("..//Scripts//SupportScripts//SharedScripts//HwRegGen")

        PictorProjectDirectory = "../../../../" + ProjectName + "Project"
        RegDefXMLFilePath = PictorProjectDirectory + "//MemoryMap//"

        ## now translate the Pictor registers
        RegDefXMLFile = RegDefXMLFilePath + ProjectName + ".xml"
        HwRegGenExecString = "python final_minidom.py " + RegDefXMLFile + " " + RegDefXMLFilePath + " Platform.h" + " uint8_t uint16_t uint32_t " + str(ISP_RegMemBase).rstrip('L') + ' 0x20' + ' CCI_32' + ' LittleEndian' + ' Pictor'
        os.system(HwRegGenExecString)

        ## now translate the CRM registers
        RegDefXMLFile = RegDefXMLFilePath + "CRM.xml"
        HwRegGenExecString = "python final_minidom.py " + RegDefXMLFile + " " + RegDefXMLFilePath + " Platform.h" + " uint8_t uint16_t uint32_t " + str(CRM_RegMemBase).rstrip('L') + ' 0x20' + ' CCI_32' + ' LittleEndian' + ' CRM'
        os.system(HwRegGenExecString)

        ## now translate the IPP registers
        RegDefXMLFile = RegDefXMLFilePath + "IPP.xml"
        HwRegGenExecString = "python final_minidom.py " + RegDefXMLFile + " " + RegDefXMLFilePath + " Platform.h" + " uint8_t uint16_t uint32_t " + str(IPP_RegMemBase).rstrip('L') + ' 0x20' + ' CCI_32' + ' LittleEndian' + ' IPP'
        os.system(HwRegGenExecString)

        # change back to the original directory
        os.chdir(CurrentWorkingDirectory)

    # invoke document parameters script
    if Mode == 'ProjectLevel':
        os.chdir("..//Scripts//SupportScripts//SharedScripts//PageElementExtraction")
    else:
        os.chdir("..//Scripts//SupportScripts//SharedScripts")

    ProjectDirectory = TopLevelCodeDirectory

    ReportDirectory = BuildGenDir + '/Reports'
    ReportDirectory.replace("/", "//")

    DeviceParamsDirectory = BuildGenDir + '/DeviceParams'
    DeviceParamsDirectory.replace("/", "//")

    DoxygenSanityChecksExecutionString = "python DoxygenSanityChecks.py" + " " + ProjectDirectory + " " + ModuleName + " " + ReportDirectory + " " + ModuleToBeSkipped + " " + Mode + " " + Debug_Script
    os.system(DoxygenSanityChecksExecutionString)

    DocumentParamExecutionString = "python 8500DocumentParameters.py" + " " + ProjectDirectory + " " + ModuleName + " " + ReportDirectory + " " + DeviceParamsDirectory + " " + ModuleToBeSkipped + " " + Mode + " " + Debug_Script
    os.system(DocumentParamExecutionString)

#    os.chdir("..//PCLint")
#    PCLintExecutionString = "python Lintconfig.py" + " " + ProjectDirectory + " " + ModuleName + " " + Mode
#    os.system(PCLintExecutionString)

    os.chdir(CurrentWorkingDirectory)

if __name__ == "__main__":
    main(sys.argv[1:])
