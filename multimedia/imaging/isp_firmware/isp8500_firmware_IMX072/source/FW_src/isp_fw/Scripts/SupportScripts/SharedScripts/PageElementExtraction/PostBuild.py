import os
import sys
import distutils.file_util

DeviceCommsLayerFilePath = ""
DeviceParamsDirectory = ""
ReportFilePath = ""
BuildArtifactName = ""
OutputDirectory = ""
SXTOOLCHAINPATH = ""
#DirPathName = ""
#ModuleName = ""
#################################################################################################

##  convert the cde4 generated to cde16 format
def ConvertCDE4ToCDE16(File):
    try:
            cde4File = open(File, "rb")
    except IOError:
            print "cde4 file cannot be opened\n"
            sys.exit(2)
    
            cde4Lines = cde4File.readlines()
            cde4File.close()
    
            cde16Lines = []
            count = 0
    
            for count in range (len(cde4Lines)/4):
    
                b0 = cde4Lines[(4 * count)].lstrip(' ').rstrip('\n')
                b1 = cde4Lines[(4 * count) + 1].lstrip(' ').rstrip('\n')
                b2 = cde4Lines[(4 * count) + 2].lstrip(' ').rstrip('\n')
                b3 = cde4Lines[(4 * count) + 3].lstrip(' ').rstrip('\n')
                cde16Line = b3 + b2 + b1 + b0
                cde16Lines.append(' ' + cde16Line + "\n")
    
            try:
                cde16File = open(File,'wb')
                cde16File.writelines(cde16Lines)
                cde16File.close()
            except:
                print "Error writing cde16 file...\n"
                print "WARNING: The CDE file might be inconsistent......."
                print "Exiting......."
                sys.exit(2)
            
#################################################################################################

## Script starts here ##
try:
    Mode = sys.argv[1]
except:
    print 'The mode("ProjectLevel" or "ModuleLevel") to be run is unspecified.\nExiting.......'
    sys.exit(2)

if Mode == 'ProjectLevel':
    try:

        OutputDirectory = sys.argv[3]
        BuildArtifactName = sys.argv[4]
        SensorType = sys.argv[6]
        GenerateBinary = True
        print "Binary file will be generated"
        print "Map file will be copied..."

    except:

        GenerateBinary = False
        print "No binary will be generated"
        print "Map file will NOT be copied..."


# get the current working directory
CurrentWorkingDirectory = os.getcwd()

DeviceParamsDirectory = sys.argv[5] + '/DeviceParams'
DeviceParamsDirectory.replace("/", "//")

ReportFilePath = sys.argv[5] + '/Reports'
ReportFilePath.replace("/", "//")

# invoke document parameters script
DeviceCommsLayerFilePath = DeviceParamsDirectory + "//DeviceCommsLayer_8500.py"

try:
    OutputFile = open(DeviceCommsLayerFilePath,'ab')
except:
    print "File " +  OutputFile + " cannot be opened for appending...\n"
    print "Exiting......."
    sys.exit(2)


# absolute path of DeviceSpecificMethods.py
try:
    ReadWriteDefinitionFileName = sys.argv[2]
except:
    print "Nothing to append to the device wrapper file, exiting"
    sys.exit(0)

# open the read write definition file
try:
    ReadWriteDefinitionFile = open(ReadWriteDefinitionFileName,'rb')
except:
    print "File " +  ReadWriteDefinitionFileName + " cannot be opened for appending...\n"
    print "Exiting......."
    sys.exit(2)


# read the read-write definition file
ReadWriteFunctionLines = ReadWriteDefinitionFile.readlines()

# now append these functions into the device params file
OutputFile.write("\t###############################################\n")
OutputFile.write("\t#     Device Specific Methods                 #\n")
OutputFile.write("\t#                                             #\n")
OutputFile.write("\t#     These methods specify how the device    #\n")
OutputFile.write("\t#     read and write mechanism are            #\n")
OutputFile.write("\t#     implemented                             #\n")
OutputFile.write("\t###############################################\n")
OutputFile.write("\n")

OutputFile.writelines(ReadWriteFunctionLines)

OutputFile.close()
ReadWriteDefinitionFile.close()


if Mode == 'ProjectLevel':
    if (True == GenerateBinary):
        print "Generating binaries now..."
        
        ################################

        ## for "IPM" section
        # now generate the binary corresponding
        ExecutionString = SXTOOLCHAINPATH + "stxp70v3-objcopy -format=bin "
        ExecutionString += OutputDirectory + "/" + BuildArtifactName + ".out "
        ExecutionString += "-start=0x400000 -end=0x40FFFF -o "
        ExecutionString += OutputDirectory + "/" + "Isp8500_" + SensorType + "_fw_split" + ".bin"
        
        os.system(ExecutionString)

        # generate the cde file
        BinaryFilePath = OutputDirectory + "/" + "Isp8500_" + SensorType + "_fw_split" + ".bin"
        CDEFilePath = OutputDirectory + "/" + "Isp8500_" + SensorType + "_fw_split" + ".cde"

        ExecutionString = "od " + BinaryFilePath + " -w4 -v -t x4 -An >" + CDEFilePath

        os.system(ExecutionString)

        ConvertCDE4ToCDE16(CDEFilePath)

        #################################

        ##  for "EXT_IPM2" section
        # now generate the binary corresponding
        ExecutionString = SXTOOLCHAINPATH + "stxp70v3-objcopy -format=bin "
        ExecutionString += OutputDirectory + "/" + BuildArtifactName + ".out "
        ExecutionString += "-start=0x20000000 -end=0x2003FFFF -o "
        ExecutionString += OutputDirectory + "/" + "Isp8500_" + SensorType + "_fw_ext_ddr" + ".bin"

        os.system(ExecutionString)

        # generate the cde file
        BinaryFilePath = OutputDirectory + "/" + "Isp8500_" + SensorType + "_fw_ext_ddr" + ".bin"
        CDEFilePath = OutputDirectory + "/" + "Isp8500_" + SensorType + "_fw_ext_ddr" + ".cde"

        ExecutionString = "od " + BinaryFilePath + " -w4 -v -t x4 -An >" + CDEFilePath

        os.system(ExecutionString)

        ConvertCDE4ToCDE16(CDEFilePath)

        ################################

        ##  for "EXT_IPM" section
        # now generate the binary corresponding to EXT_IPM section
        ExecutionString = SXTOOLCHAINPATH + "stxp70v3-objcopy -format=bin "
        ExecutionString += OutputDirectory + "/" + BuildArtifactName + ".out "
        ExecutionString += "-start=0x10001000 -end=0x1000FFFF -o "						# leaving out reserved (4k) from 64k i.e. 60k
        ExecutionString += OutputDirectory + "/" + "Isp8500_" + SensorType + "_fw_ext" + ".bin"
        
        os.system(ExecutionString)

        # generate the cde file
        BinaryFilePath = OutputDirectory + "/" + "Isp8500_" + SensorType + "_fw_ext" + ".bin"
        CDEFilePath = OutputDirectory + "/" + "Isp8500_" + SensorType + "_fw_ext" + ".cde"

        ExecutionString = "od " + BinaryFilePath + " -w4 -v -t x4 -An >" + CDEFilePath

        os.system(ExecutionString)

        ConvertCDE4ToCDE16(CDEFilePath)

        ###################################

        ##   for "IDM" section
        ExecutionString = SXTOOLCHAINPATH + "stxp70v3-objcopy -format=bin "
        ExecutionString += OutputDirectory + "/" + BuildArtifactName + ".out "
        ExecutionString += "-start=0x0000 -end=0x1FFF -o "
        ExecutionString += OutputDirectory + "/" + "Isp8500_" + SensorType + "_fw_data" + ".bin"
                
        os.system(ExecutionString)

        # generate the cde file
        BinaryFilePath = OutputDirectory + "/" + "Isp8500_" + SensorType + "_fw_data" + ".bin"
        CDEFilePath = OutputDirectory + "/" + "Isp8500_" + SensorType + "_fw_data" + ".cde"

        ExecutionString = "od " + BinaryFilePath + " -w4 -v -t x4 -An >" + CDEFilePath

        os.system(ExecutionString)

        ConvertCDE4ToCDE16(CDEFilePath)

        ######################################

        # generate the assembly file
        ToolChain = SXTOOLCHAINPATH + "stxp70v3-objdump"
        ElfFilePath = OutputDirectory + "/" + BuildArtifactName + ".out"
        OutputFilePath = OutputDirectory + "/" + BuildArtifactName + "_Code" + ".s"
        Extension = " -Mextension=fpx "
        ExecutionString = ToolChain + " -d " + ElfFilePath + Extension + " -o " + OutputFilePath
        os.system(ExecutionString)

        # generate the size of various sections, functions in the .out file
        ToolChain = SXTOOLCHAINPATH + "stxp70v3-size"
        ElfFilePath = OutputDirectory + "/" + BuildArtifactName + ".out"
        OutputFilePath = ReportFilePath + "/" + BuildArtifactName + "_Size.txt"
        ExecutionString = ToolChain + " -all -func -sect " + ElfFilePath + " > " + OutputFilePath
        os.system(ExecutionString)
 
        # generate the size of various sections, functions in the .out file
        ToolChain = SXTOOLCHAINPATH + "stxp70v3-objdump"
        Switches = " -disassemble -reloc -header -pgheader -scheader -symbol  -csource -Mextension-sectinfo -all-symbols "
        ElfFilePath = OutputDirectory + "/" + BuildArtifactName + ".out"
        OutputFilePath = ReportFilePath + "/" + BuildArtifactName + "_ObjDump.txt"
        ExecutionString = ToolChain + Switches + ElfFilePath + " -o " + OutputFilePath
        os.system(ExecutionString)

        print "Performing bin comapare..."
        os.chdir("..//Scripts//SupportScripts//SharedScripts//PageElementExtraction")
        ExecutionString = "python bin_compare.py" + " " + OutputDirectory
        os.system(ExecutionString)
