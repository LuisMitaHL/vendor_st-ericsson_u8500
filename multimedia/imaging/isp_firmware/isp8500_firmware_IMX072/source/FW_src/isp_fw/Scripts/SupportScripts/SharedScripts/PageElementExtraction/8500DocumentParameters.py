#!/usr/local/bin/python
# -*- coding: utf-8 -*-
## This script is used for the generation of DeviceCommsLayer.py File
## The Generation of DeviceCommsLayer.py File involves these steps.
##  1.) Scanning of the Directory provided as the input.
##          A list of all the files present in the provided directory is generated
##          and is stored.
##  2.) All the files in the above stored List are parsed to save any DataType
##      Definition present in the files.
##      Here both the .c and .h are files are parsed for DataType definitions.
##      The DataType definitions are stored in a Dictionary.
##  3.) Next, the files are agained parsed to look for any Variables od the Stored DataTypes.
##      This List of Variables is actually the List of Firmware Pages.
##      The name of the Pages can also be found in the VirtualRegisterList.c
##  4.) Now, the Final Output file is to be written with the Dictionaries like
##      FirmwareVariableTypesByName , etc.

import os
import sys
import time
##import win32api

##############################################################################
#      A few Definitions of Functions used in theis Script.                  #
##############################################################################
def UpdateFirmwareVersionLLA():
    if HashDefineDictionary.has_key('FIRMWARE_VERSION_MAJOR'):
        FirmwareVersion['FIRMWARE_VERSION_MAJOR'] = GetLeafValueFromHashDefineDictionary('FIRMWARE_VERSION_MAJOR')
    if HashDefineDictionary.has_key('FIRMWARE_VERSION_MAJOR'):
        FirmwareVersion['FIRMWARE_VERSION_MINOR'] = GetLeafValueFromHashDefineDictionary('FIRMWARE_VERSION_MINOR')
    if HashDefineDictionary.has_key('FIRMWARE_VERSION_MICRO'):
        FirmwareVersion['FIRMWARE_VERSION_MICRO'] = GetLeafValueFromHashDefineDictionary('FIRMWARE_VERSION_MICRO')    
    if HashDefineDictionary.has_key('LOW_LEVEL_API_SENSOR'):
        FirmwareVersion['LOW_LEVEL_API_SENSOR'] = GetLeafValueFromHashDefineDictionary('LOW_LEVEL_API_SENSOR')
    if HashDefineDictionary.has_key('CAM_DRV_API_VERSION_MAJOR'):
        FirmwareVersion['CAM_DRV_API_VERSION_MAJOR'] = GetLeafValueFromHashDefineDictionary('CAM_DRV_API_VERSION_MAJOR')
    if HashDefineDictionary.has_key('CAM_DRV_API_VERSION_MINOR'):
        FirmwareVersion['CAM_DRV_API_VERSION_MINOR'] = GetLeafValueFromHashDefineDictionary('CAM_DRV_API_VERSION_MINOR')
    if HashDefineDictionary.has_key('CAM_DRV_DRIVER_VERSION_MAJOR'):
        FirmwareVersion['CAM_DRV_DRIVER_VERSION_MAJOR'] = GetLeafValueFromHashDefineDictionary('CAM_DRV_DRIVER_VERSION_MAJOR')
    if HashDefineDictionary.has_key('CAM_DRV_DRIVER_VERSION_MINOR'):
        FirmwareVersion['CAM_DRV_DRIVER_VERSION_MINOR'] = GetLeafValueFromHashDefineDictionary('CAM_DRV_DRIVER_VERSION_MINOR')


def AddDataTypeDefinitionsToVhcElementDefsFile(VhcElements):
    VhcElements.write("\n\n")
    VhcElements.write("/*****************************************************************************/\n")
    VhcElements.write("/************************ Data Type Definitions ******************************/\n")
    VhcElements.write("/*****************************************************************************/\n")
    VhcElements.write("\n")

    VhcElements.write("/// Unsigned 8 bit data type\n")
    VhcElements.write("typedef unsigned char       isp_uint8_t;\n\n")

    VhcElements.write("/// Signed 8 bit data type\n")
    VhcElements.write("typedef signed char         isp_int8_t;\n\n")

    VhcElements.write("/// Signed 16 bit data type\n")
    VhcElements.write("typedef short               isp_int16_t;\n\n")

    VhcElements.write("/// Unsigned 16 bit data type\n")
    VhcElements.write("typedef unsigned short      isp_uint16_t;\n\n")
    
    VhcElements.write("/// Signed 32 bit data type\n")
    VhcElements.write("typedef int                 isp_int32_t;\n\n")
    
    VhcElements.write("/// Unsigned 32 bit data type\n")
    VhcElements.write("typedef unsigned int        isp_uint32_t;\n\n")
    
    VhcElements.write("/// Signed 64 bit data type\n")
    VhcElements.write("typedef long long           isp_int64_t;\n\n")
    
    VhcElements.write("/// Unsigned 64 bit data type\n")
    VhcElements.write("typedef unsigned long long  isp_uint64_t;\n\n")
    
    VhcElements.write("/// 32 bit IEEE 754 floating point data type\n")
    VhcElements.write("typedef float               isp_float_t;\n\n")
    
    VhcElements.write("/// Boolean data type. Occupies 8 bits.\n")
    VhcElements.write("typedef isp_uint8_t             isp_bool_t;\n\n")
    
    #VhcElements.write("/*****************************************************************************/\n")






def ScanConfigurationHeaderDirectory(ListOfFilePaths):
    for filepath in ListOfFilePaths:
        if filepath.find('ConfigurationHeaders') != -1:
            if filepath.find('Platform.h') != -1 or filepath.find('Linker.ld') != -1 or filepath.find('ProjectConfiguration.h') != -1:
                pass
            else:
                ConfigurationHeaderFileList.append(filepath)
##    if DebugScript == '1':
##        DebugFile.write("\nConfigurationHeaderFileList = ")
##        for File in ConfigurationHeaderFileList:
##            DebugFile.write("\n" + File)

    for filepath in ConfigurationHeaderFileList:
        list = filepath.split('/')
        filename = list[len(list) - 1]
        if filename.find('_') == -1 and filename.find('PlatformSpecific.h') != -1:
            # filename should have 'PlatformSpecific.h'
            TempConfigurationHeaderModuleList.append(filename[:filename.find('PlatformSpecific.h')])
        else:
            TempConfigurationHeaderModuleList.append(filename.split('_')[0])

##    if DebugScript == '1':    
##        DebugFile.write("\nTempConfigurationHeaderModuleList = ")
##        for File in TempConfigurationHeaderModuleList:
##            DebugFile.write("\n" + File)

    for configuration_header_module in TempConfigurationHeaderModuleList:
        for module in ListOfTopDirectoriesInProjectPath:
            if module.upper() == configuration_header_module.upper():
                ActualConfigurationHeaderModuleList.append(module)
                ConfigurationHeaderModulesFilePathList.append(GetListOfFilesInDirectory(module))
                break
            else:
                continue

##    if DebugScript == '1':
##        DebugFile.write("\nActualConfigurationHeaderModuleList = ")
##        for File in ActualConfigurationHeaderModuleList:
##            DebugFile.write("\n" + File)
##
##        DebugFile.write("\nConfigurationHeaderModulesFilePathList = ")
##        for File in ConfigurationHeaderModulesFilePathList:
##            DebugFile.write("\n" + str(File))

def GetListOfFilesInDirectory(module_name):
    temp_var = ()
    index = 0
    count = 0
    ListOfFiles = []
    ListOfModuleDirectoryTuples =[]
    module_path = DirPathName + '/' + module_name
##    print '\n\nmodule_path = ' + str(module_path) + '\n'
    for temp_var in os.walk(module_path):
        ListOfModuleDirectoryTuples.append(temp_var)
##    print 'ListOfDirectoryTuples[0] = ' + str(ListOfModuleDirectoryTuples[0])

    num_of_files = len(ListOfModuleDirectoryTuples[0][2])
    for file_name in ListOfModuleDirectoryTuples[0][2]:
        if file_name.endswith('.c') or file_name.endswith('.h'):
            file_path = ListOfModuleDirectoryTuples[0][0] + '/' + file_name
            ListOfFiles.append(file_path)

##    if DebugScript == '1':
##        DebugFile.write("\n\nmodule_name  = " + module_name)
##        DebugFile.write("\nListOfFiles = ")
##        for File in ListOfFiles:
##            DebugFile.write("\n" + str(File))

    return ListOfFiles
            

def LookInputDependenciesInModule():
    index = 0
    # Performing Sanity Check here.
    ConfigurationHeaderFileListLength = len(ConfigurationHeaderFileList)
    ActualConfigurationHeaderModuleListLength = len(ActualConfigurationHeaderModuleList)
    if ConfigurationHeaderFileListLength == ActualConfigurationHeaderModuleListLength:
        pass
    else:
        print "ConfigurationHeaderFileListLength not equal to ActualConfigurationHeaderModuleListLength....... exiting.... \n"
        sys.exit(2)
    for index in range(ConfigurationHeaderFileListLength):
        input_dependency_list = []
        input_dependency_filepath = ConfigurationHeaderFileList[index]
        input_dependency_file = open(input_dependency_filepath, 'rb')
        list_of_lines = input_dependency_file.readlines()
        MultipleLineCommentEntered = 0
        for line in list_of_lines:
            CommentStartsAt = 0
            CommentEndsAt = 0
            list = line.split()
            SingleLineCommentAt = 0
            SingleLineCommentAt = line.find('//')
            if SingleLineCommentAt != -1:
                line = line[0:SingleLineCommentAt]
            if MultipleLineCommentEntered == 0:
                if line.strip().startswith('/*') == True:
                    CommentStartsAt = line.find('/*')
                    MultipleLineCommentEntered = 1
                    if line.find('*/')!= -1:
                        MultipleLineCommentEntered = 0
                        CommentEndsAt = line.find('*/')
                        line = line[0:CommentStartsAt] + line[CommentEndsAt + 2: ]
                        list = line.split()
            elif MultipleLineCommentEntered == 1:
                if line.find('*/')!= -1:
                    MultipleLineCommentEntered = 0
                    line = line[line.find('*/') + 2: ]

            if len(list) != 0 and MultipleLineCommentEntered == 0 and (line.strip()).startswith('//') != True:

                if line.find('define') != -1:
                    input_dependency = line.strip('# ').split()[1]

                    if input_dependency.find('(') != -1:
                        bracket_point = input_dependency.find('(')
                        input_dependency = input_dependency[:bracket_point]
                    input_dependency_list.append(input_dependency)
            input_dependency_file.close()

        if DebugScript == '1':
            DebugFile.write("\n\nmodule_name  = " + str(ActualConfigurationHeaderModuleList[index]))
            DebugFile.write("\n\ninput_dependency_list  = " + str(input_dependency_list))
            DebugFile.write("\n\nfile_list  = " + str(ConfigurationHeaderModulesFilePathList[index]))

        ScanModuleForInputDependency(input_dependency_list, ConfigurationHeaderModulesFilePathList[index], ActualConfigurationHeaderModuleList[index])

def ScanModuleForInputDependency(input_dependency_list, ListOfFilePath, module_name):
    global DiscrepancyFound
    for input_dependency in input_dependency_list:
        input_dependency_found = 0
        if input_dependency.endswith('_') or input_dependency.startswith('_'):
            # the input dependency is a Header Define, eg, ZOOM_IPINTERFACE_H_
            input_dependency_found = 1
            pass
        else:
            for filepath in ListOfFilePath:
                file = open(filepath, 'rb')
                list_of_lines = file.readlines()
                MultipleLineCommentEntered = 0
                for line in list_of_lines:
                    CommentStartsAt = 0
                    CommentEndsAt = 0
                    list = line.split()
                    SingleLineCommentAt = 0
                    SingleLineCommentAt = line.find('//')
                    if SingleLineCommentAt != -1:
                        line = line[0:SingleLineCommentAt]
                    if MultipleLineCommentEntered == 0:
                        if line.strip().startswith('/*') == True:
                            CommentStartsAt = line.find('/*')
                            MultipleLineCommentEntered = 1
                            if line.find('*/')!= -1:
                                MultipleLineCommentEntered = 0
                                CommentEndsAt = line.find('*/')
                                line = line[0:CommentStartsAt] + line[CommentEndsAt + 2: ]
                                list = line.split()
                    elif MultipleLineCommentEntered == 1:
                        if line.find('*/')!= -1:
                            MultipleLineCommentEntered = 0
                            line = line[line.find('*/') + 2: ]


                    if len(list) != 0 and MultipleLineCommentEntered == 0 and (line.strip()).startswith('//') != True:

                        if line.find(input_dependency) != -1:
                            input_dependency_found = 1
                            break
                        else:
                            continue
                file.close()
        
        if input_dependency_found == 0:
            DiscrepancyFound = 1
            ReportFile.write("\nInput Dependency --> " + input_dependency + " .....is not used in the module --> " + module_name)
            print "\nInput Dependency --> " + input_dependency + " .....is not used in the module --> " + module_name
            if input_dependency == input_dependency_list[len(input_dependency_list) - 1]:
                ReportFile.write('\n')
            
def IsFunctionDeclarationPresentInLine(Line):
    FunctionNamePresentInLine = 0
    for FunctionName in FunctionList:
        if Line.find(FunctionName) != -1:
            FunctionNamePresentInLine = 1
    if FunctionNamePresentInLine == 1:
        return True
    else:
        return False
def GenerateFunctionlist():
    FunctionListFilePath = sys.argv[3] + "//" + "Doxygen_FunctionList.txt"
    FunctionListFile = open(FunctionListFilePath, 'rb')
    ListOfInputs = FunctionListFile.readlines()

    for Line in ListOfInputs:
        if (Line.strip()).startswith('##'):
            pass
        else:
            List = Line.split()
            if len(List) != 0:
                FunctionName = List[2]
                FunctionList.append(FunctionName)

def GetEnumType(PageElement):
    global DiscrepancyFound
    # the PageElement Name would be of the following pattern
    # e_[Enum Typedef Name]_[ElementNeme]
    for DataType in DictionaryOfStructs.keys():
        if DataType.endswith('_te') == True or DataType.endswith('_tef') == True:
            EnumName = DataType[:DataType.find('_te')]
            if PageElement.lstrip('e_').startswith(EnumName) == True:
                return DataType
    print "\nCould not find Enum associated with this PageElement -> " + str(PageElement) + "\n"
    ReportFile.write("\nCould not find Enum associated with this PageElement -> " + str(PageElement) + "\n")
    DiscrepancyFound = 1
    return "None"

def LookForTypeDefSyntax(File, ListOfInputs):
    global DiscrepancyFound
    LineCount = 1
    LineEnd = 0
    Found = ""
    FoundStack = []
    DSLineStart = []
    index = 0
    DataStructureDefinitionEntered = 0
    TypedefDefinitionEntered = 0
    ScanningNextLine = 0
    MultipleLineCommentEntered = 0
    for Line in ListOfInputs:
        CommentStartsAt = 0
        CommentEndsAt = 0
        List = Line.split()
        SingleLineCommentAt = 0
        SingleLineCommentAt = Line.find('//')
        if SingleLineCommentAt != -1:
            Line = Line[0:SingleLineCommentAt]
        if MultipleLineCommentEntered == 0:
            if Line.strip().startswith('/*') == True:
                CommentStartsAt = Line.find('/*')
                MultipleLineCommentEntered = 1
                if Line.find('*/') != -1:
                    MultipleLineCommentEntered = 0
                    CommentEndsAt = Line.find('*/')
                    Line = Line[0:CommentStartsAt] + Line[CommentEndsAt + 2: ]
                    List = Line.split()
        elif MultipleLineCommentEntered == 1:
            if Line.find('*/') != -1:
                MultipleLineCommentEntered = 0
                CommentEndsAt = Line.find('*/')
                Line = Line[CommentEndsAt + 2: ] # PM: TBD: We do not handle the case where one comment ends and another comment starts on the same line. e.g. " ... */ Hello World /* ..."
                List = Line.split()


        if len(List) != 0 and MultipleLineCommentEntered == 0 and (Line.strip()).startswith('//') != True:

            if FirmwareVersionMajorFound == 0:
                FindFirmwareVersionMajor(Line)
            if FirmwareVersionMinorFound == 0:
                FindFirmwareVersionMinor(Line)
            if FirmwareVersionMicroFound == 0:
                FindFirmwareVersionMicro(Line)
             
            if Line.find('typedef') != -1 :
                index = 0
                DSLineStart = []
                DSLineStart.insert(index,LineCount)
                if Line.strip().endswith(';') == True:
                    SaveElementaryDataType(Line)
                else:
                    TypedefDefinitionEntered = 1

                    if Line.find('struct') != -1 or  Line.find('enum') != -1 or Line.find('union') != -1:
                        if Line.find('struct') != -1:
                            FoundStack.insert(index,"struct")
                        elif Line.find('enum') != -1:
                            FoundStack.insert(index,"enum")
                        else:
                            FoundStack.insert(index,"union")
                        index = index + 1
            elif TypedefDefinitionEntered == 1 and  len(FoundStack) <=1:
                if Line.find('struct') != -1 or  Line.find('enum') != -1 or Line.find('union') != -1:
                    if Line.find('struct') != -1:
                        FoundStack.insert(index,"struct")
                    elif Line.find('enum') != -1:
                        FoundStack.insert(index,"enum")
                    else:
                        FoundStack.insert(index,"union")
                    DSLineStart.insert(index,LineCount)
                    index = index + 1
                    
                if len(FoundStack) == 0:
                    Found = ""
                elif len(FoundStack) == 1:
                    Found = FoundStack[0]
                else:
                    #Composite DataType has been met
                    DataStructureDefinitionEntered = 1
                    
                if DataStructureDefinitionEntered == 0:

                    if Found !=  "":
                        if Line.find('}') != -1 or ScanningNextLine == 1:
                            if len(Line.strip()) > 1:
                                LineEnd = LineCount
                                if  Line.find(DictionaryOfTypedef[Found]) != -1:
                                    TypedefName = str(Line.strip().strip('}').strip().strip(';').strip().strip('\t').strip('\\')).strip().strip(';')
                                    if TypedefName.endswith('_Damper_ts'):
                                        #SaveDamperTypedefDefinition(DSLineStart[index - 1],LineEnd,TypedefName,ListOfInputs,File)
                                        print ""
                                    else:
                                        SaveTypedefDefinition(DSLineStart[index - 1],LineEnd,TypedefName,ListOfInputs,File)
                                else:
                                    print "Typedef Not Matching between Line Numbers " + str(DSLineStart[index - 1]) + " & " + str(LineEnd) + " in File -- " + "\n" + str(File)
                                    if (Mode == 'ModuleLevel' and File in ListOfModuleFilePaths) or (Mode == 'ProjectLevel' and DictionaryOfFilePathToFileName[File] not in ListOfToBeSkippedFileNames):
                                        ReportFile.write("\nTypedef Not Matching between Line Numbers " + str(DSLineStart[index - 1]) + " & " + str(LineEnd) + "in File -> " + str(File) + "\n")
                                        ReportFile.write("\nTypedef Not Matching between Line Numbers " + str(DSLineStart[index - 1]) + " & " + str(LineEnd) + "in File -> " + str(File) + "\n")
                                        ReportFile.write("\nidentifier              Rules for Naming    Examples \n ")
                                        ReportFile.write(" typedef struct           ends with _ts       MASTER_CCI_CONTROL_ts\n")
                                        ReportFile.write(" typedef union            ends with _tu       MASTER_CCI_ENABLE_ts\n ")
                                        ReportFile.write(" typedef enum             ends with _te       AFStats_WindowSystem_te\n")
                                        ReportFile.write(" struct                   ends with _s        fields_s\n ")
                                        ReportFile.write(" union                    ends with _u        x_u\n ")
                                        ReportFile.write("\n")
                                        DiscrepancyFound = 1
                                index = index - 1
                                FoundStack.pop()
                                DSLineStart.pop()
                                Found = ""
                                if index <=0:
                                    TypedefDefinitionEntered = 0
                                    ScanningNextLine = 0
                            else:
                                ScanningNextLine = 1
            elif (Line.find("struct") != -1) or (Line.find("enum") != -1) or (Line.find("union") != -1):

                DSLineStart.insert(index,LineCount)
                if Line.find("struct") != -1:
                    FoundStack.insert(index,"struct")
                if Line.find("enum") != -1:
                    FoundStack.insert(index,"enum")
                if Line.find("union") != -1:
                    FoundStack.insert(index,"union")
                    
                index = index + 1
                DataStructureDefinitionEntered = 1
                
            elif DataStructureDefinitionEntered == 1:
                if Line.find('}') != -1 or ScanningNextLine == 1:
                    if len(Line.strip()) > 1:
                        LineEnd = LineCount
                        if Line.find(DictionaryOfDataStructures[FoundStack[index - 1]]) != -1 :
                            DataStructureName = str(Line.strip().strip('}').strip().strip(';').strip())
                            SaveDataStructureDefinition(DSLineStart[index - 1],LineEnd,DataStructureName,ListOfInputs,File)
                        else:
                            print "Data Structure Definition Not Matching between Line Numbers " + str(DSLineStart[index - 1]) + " & " + str(LineEnd) + " in File -- " + "\n" + str(File)
                            if (Mode == 'ModuleLevel' and File in ListOfModuleFilePaths) or (Mode == 'ProjectLevel' and DictionaryOfFilePathToFileName[File] not in ListOfToBeSkippedFileNames):
                                ReportFile.write("\nData Structure Definition Not Matching between Line Numbers " + str(DSLineStart[index - 1]) + " & " + str(LineEnd) + " in File " + str(File) + "\n")
                                DiscrepancyFound = 1
                        index = index - 1
                        FoundStack.pop()
                        DSLineStart.pop()
                        if (TypedefDefinitionEntered == 1 and index == 1) or (TypedefDefinitionEntered == 0 and index == 0):
                            DataStructureDefinitionEntered = 0
                            ScanningNextLine = 0
                    else:
                        ScanningNextLine = 1

        LineCount = LineCount + 1

def UpdateFileWithAttributeAndValue(AttributeName, OriginalAttribute, value, enum, file):
    InputFile = open(file,'rb')
    ListOfInputs = InputFile.readlines()
    LastValidLine = FindLastValidLine(ListOfInputs)
    InputFile.close()

##    win32api.DeleteFile(file)
    
    WriteFile = open(file,'wb')
    CountOfLines = 0
    for Line in ListOfInputs:
        if CountOfLines == LastValidLine:
            WriteFile.write('#define ' + AttributeName + '\t\t' + str(DictionaryOfStructs[enum][OriginalAttribute]) + '\n')
        WriteFile.write(Line)
        CountOfLines = CountOfLines + 1
    WriteFile.close()

def FindLastValidLine(ListOfInputs):
    LastValidLine = len(ListOfInputs) - 1
    while (len(ListOfInputs[LastValidLine].strip()) == 0 and ListOfInputs[LastValidLine].find('#endif') == -1):
        LastValidLine = LastValidLine - 1
    return LastValidLine

def WriteFirmwareVariableSerialIndicesByNameDictionary(OutputFile):
    PageElementDictionary = {}
    IndicesList = []
    PageElementList = []
    index = 0
    OutputFile.write("		self.FirmwareVariableSerialIndicesByName = {	\n")
    for PageName in AllVirtualRegisterPageList:
        PageInArrayList = 0 
        PageInArrayList = IsPageInArrayDictionary(PageName)
        if PageInArrayList == 1:
            PageArrayName = GetPageArrayName(PageName)
            PageType = PageNameToPageTypeDictionary[GetPageArrayName(PageName)]
        else:
            PageType = PageNameToPageTypeDictionary[PageName]
        PageElementList = DictionaryOfPageTypeToListOfPageElements[PageType]
        PageElementDictionary = FirmwareVariableSerialIndicesByName[PageName]
        OutputFile.write("		\t'" + str(PageName[2:]) + "' : {")
        for PageElement in PageElementList:
            IndicesList = PageElementDictionary[PageElement]
            if PageElement.find('*') != -1:
                PageElement = PageElement.strip('*')
            OutputFile.write("'" + str(PageElement) + "' : (")
            for index in range(len(IndicesList)):
                OutputFile.write("0x%04x" %IndicesList[index] + ",")
            OutputFile.write("),")
        OutputFile.write("},\n")
    OutputFile.write("		}\n")

def WriteFirmwareVariableTypesByNameDictionary(OutputFile):
    OutputFile.write("		self.FirmwareVariableTypesByName = {	\n")
    
    for PageName in AllVirtualRegisterPageList:
        if PageName.find('[')!= -1:
            PageType = PageNameToPageTypeDictionary[PageName[:PageName.find('[')]]
        else:
            PageType = PageNameToPageTypeDictionary[PageName]
        PageElementList = DictionaryOfPageTypeToListOfPageElements[PageType]
        OutputFile.write("		\t'" + str(PageName[2:]) + "' : {")  # + str(FirmwareVariableTypesByName[PageName]) + ",\n")
        for PageElement in PageElementList:
            if PageElement.startswith('e_') == True:
                PageElementType = GetEnumType(PageElement)
            else:
                PageElementType = FirmwareVariableTypesByName[PageName][PageElement]
            if PageElement.find('*') != -1:
                PageElement = PageElement.strip('*')
            if PageElement.find('ptr') != -1:
                PageElementType = 'uint32_t'
            OutputFile.write("'" + str(PageElement) + "' : '" + PageElementType + "', ")
        OutputFile.write("},\n")
    OutputFile.write("		}\n")

def WriteEnumIntegerValueByStringDictionariesByTypeDictionary(OutputFile):
    OutputFile.write("		self.EnumIntegerValueByStringDictionariesByType = {	\n")
    for index in EnumIntegerValueByStringDictionariesByType.keys():
        OutputFile.write("		\t'" + str(index) + "' : " + str(EnumIntegerValueByStringDictionariesByType[index]) + ",\n")
    OutputFile.write("		}\n")

def WriteFlagIndexByStringDictsByTypeDictionary(OutputFile):
    OutputFile.write("		self.FlagIndexByStringDictsByType = {	\n")
    for index in FlagIndexByStringDictsByType.keys():
        OutputFile.write("		\t'" + str(index) + "' : " + str(FlagIndexByStringDictsByType[index]) + ",\n")
    OutputFile.write("		}\n")


def WriteHeaderToDeviceCommsLayerFile(OutputFileName,OutputFile):
    OutputFile.write("#\n")
    OutputFile.write("#              file:" + str(OutputFileName) + "\n")
    OutputFile.write("#        created by: Automatic Script     \n")
    OutputFile.write("#           purpose: defines a class that implements the device communucation\n")
    OutputFile.write("#                    layer in PST, an object of this class should be created\n")
    OutputFile.write("#                    and used to communicate with the device under test\n")
    OutputFile.write("#\n")
    OutputFile.write("#             notes: 1. this file should only be used with a device that\n")
    OutputFile.write("#                       contains firmware version as specified by \n")
    OutputFile.write("#  firmware version: v" + str(FirmwareVersion['MAJOR']) + "." + str(FirmwareVersion['MINOR']) + "." + str(FirmwareVersion['MICRO']) + " (parsed from source files)\n")
    OutputFile.write("#\n")
    OutputFile.write("\n")    
    OutputFile.write("        #########################################################\n")
    OutputFile.write("        ##                                                     ##\n")
    OutputFile.write("        ##  this file was automatically generated by a script  ##\n")
    OutputFile.write("        ##                                                     ##\n")
    OutputFile.write("        ##  if this file is out of date you are almost         ##\n")
    OutputFile.write("        ##  certainly better to create a new version using     ##\n")
    OutputFile.write("        ##  the script rather than editing this file           ##\n")
    OutputFile.write("        ##                                                     ##\n")
    OutputFile.write("        ##  if there is a bug in it or it is defficient in     ##\n")
    OutputFile.write("        ##  some other way then you should modify the script   ##\n")
    OutputFile.write("        ##  then recreate this file                            ##\n")
    OutputFile.write("        ##                                                     ##\n")
    OutputFile.write("        #########################################################\n")
    OutputFile.write("\n")
    OutputFile.write("# the class defined in this file is based on a class that implements comms\n")
    OutputFile.write("# features that are general to all VPIP devices (unless this file is being\n")
    OutputFile.write("# generated to support a legacy comms device in which case the class\n")
    OutputFile.write("# 'imported' below will the one customised for legacy comms)...\n")
    OutputFile.write("#\n")
    OutputFile.write("\n")
    OutputFile.write("from GeneralVpipDeviceCommsLayer import *\n")
    OutputFile.write("\n")    
    OutputFile.write("# declare a class that can be used to communicate with the VPIP device (whether\n")
    OutputFile.write("# an actual device or a simulated/emulated device), this class extends the\n")
    OutputFile.write("# 'general vpip comms device' class - adding to it all the details that are\n")
    OutputFile.write("# particular to this specific build of this specific VPIP device (again if this\n")
    OutputFile.write("# wrapper has been generated to support a legacy comms device this class may\n")
    OutputFile.write("# be extending the legacy comms superclass rather than the general VPIP comms\n")
    OutputFile.write("# class directly)...\n")
    OutputFile.write("#\n")
    OutputFile.write("\n")
    OutputFile.write("class DeviceComms(GeneralVpipDeviceComms):\n")
    OutputFile.write("	def __init__( self, commsTool ):\n")
    OutputFile.write("		# call the constructor of the base class, this defines a lot of\n")
    OutputFile.write("		# common data members...\n")
    OutputFile.write("		#\n")
    OutputFile.write("		GeneralVpipDeviceComms.__init__(self, commsTool )\n")
    OutputFile.write("		# we need to fill out two 2-dimensional dictionaries that define\n")
    OutputFile.write("		# all the enumerated types (flag collections and normal enums)\n")
    OutputFile.write("		# used in this firmware build...\n")
    OutputFile.write("		#\n")

def FindFirmwareVersionMajor(Line):
    global FirmwareVersionMajorFound
    if (Line.find('#define') != -1):
        if (Line.find('FIRMWARE_VERSION_MAJOR') != -1):
            # the current line has the definition of the firmware version (major)
            Words = Line.split()
            if (len(Words) != 3):
                print "Invalid firmware version (major) found... exiting... \n"
                sys.exit(2)
            else:
                
                # found a line with the FIRMWARE_VERSION_MAJOR string
                # check to see if the FIRMWARE_VERSION_MAJOR has been multiply defined
                if (FirmwareVersion.has_key('MAJOR') and Words[0] == '#define' and FirmwareVersion['MAJOR'] != -1):
                    print "Multiple firmware versions (major) found... exiting... \n"
                    sys.exit(2)
                
                FirmwareVersion['MAJOR'] = Words[2]
                FirmwareVersionMajorFound = 1

def FindFirmwareVersionMinor(Line):
    global FirmwareVersionMinorFound
    if (Line.find('#define') != -1):
        if (Line.find('FIRMWARE_VERSION_MINOR') != -1):
            # the current line has the definition of the firmware version (minor)
            Words = Line.split()
            if (len(Words) != 3):
                print "Invalid firmware version (minor) found... exiting... \n"
                sys.exit(2)
            else:
                
                # found a line with the FIRMWARE_VERSION_MINOR string
                # check to see if the FIRMWARE_VERSION_MINOR has been multiply defined
                if (FirmwareVersion.has_key('MINOR') and Words[0] == '#define' and FirmwareVersion['MINOR'] != -1):
                    print "Multiple firmware versions (minor) found... exiting... \n"
                    sys.exit(2)
                
                FirmwareVersion['MINOR'] = Words[2]
                FirmwareVersionMinorFound = 1
                

def FindFirmwareVersionMicro(Line):
    global FirmwareVersionMinorFound
    if (Line.find('#define') != -1):
        if (Line.find('FIRMWARE_VERSION_MICRO') != -1):
            # the current line has the definition of the firmware version (micro)
            Words = Line.split()
            if (len(Words) != 3):
                print "Invalid firmware version (micro) found... exiting... \n"
                sys.exit(2)
            else:
                
                # found a line with the FIRMWARE_VERSION_MICRO string
                # check to see if the FIRMWARE_VERSION_MICRO has been multiply defined
                if (FirmwareVersion.has_key('MICRO') and Words[0] == '#define' and FirmwareVersion['MICRO'] != -1):
                    print "Multiple firmware versions (micro) found... exiting... \n"
                    sys.exit(2)
                
                FirmwareVersion['MICRO'] = Words[2]
                FirmwareVersionMicroFound = 1

def GenerateEnumAndEnumFlagDictionary():
    for datatype in DictionaryOfStructs.keys():
        if datatype.endswith('_te') == True:
            #Then this datatype is a enum declaration.
            EnumIntegerValueByStringDictionariesByType[datatype] = DictionaryOfStructs[datatype]
        if datatype.endswith('_tef') == True:
            #Then this datatype is a enum declaration.
            FlagIndexByStringDictsByType[datatype] = DictionaryOfStructs[datatype]

def IsPageInArrayDictionary(PageName):
    if PageName.find('[') != -1 and PageName.find(']') != -1:
        return 1
    else:
        return 0

def GetPageArrayName(PageName):
    if PageName in PageArrayDictionary.keys():
        return PageName
    else:
        return  PageName[:PageName.find('[')]
    
def GenerateFirmwareVariableSerialIndicesAndTypesByNameDictionary(Base, VirtualRegisterPageList):
##    if BaseAddress == 0:
##        WMM_PageList = False
##    else:
##        WMM_PageList = True
    BaseAddress = 0x0000
    for PageName in VirtualRegisterPageList:
        PageInArrayList = 0 
        PageInArrayList = IsPageInArrayDictionary(PageName)
        if PageName in PageList or PageInArrayList == 1:
            if PageInArrayList == 1:
                PageArrayName = GetPageArrayName(PageName)
                NumberOfPages = int(PageArrayDictionary[PageArrayName])
            else:
                NumberOfPages = 1
            for count in range(NumberOfPages):
                Index = VirtualRegisterPageList.index(PageName)
                BaseAddress = Index << 6
                TempDictionary = {}
                PageElementList = []
                index = 0
                FirmwareVariableSerialIndicesByName[PageName] = {}
                FirmwareVariableTypesByName[PageName] = {}
                if PageInArrayList == 1:
                    PageType = PageNameToPageTypeDictionary[GetPageArrayName(PageName)]
                else:
                    PageType = PageNameToPageTypeDictionary[PageName]
                if PageType.endswith('_te') == False and PageType.endswith('_tef') == False:
                    if SizeOf(PageType) > 64:
##                        print "Size Of Page -> " + str(PageType) + " exceeds 64."
                        ReportFile.write("\nSize Of Page -> " + str(PageType) + " exceeds 64." + "\n")
                        DiscrepancyFound = 1
                    if CheckAlignmentConstraintOfStruct(PageType) == 0:
                        ReportFile.write("\nAlignmentConstraints Voilated in PageType -> " + str(PageType))
                        print "Exiting...."
                        sys.exit(2)
                    AlignmentConstraint = GetAlignmentConstraintOfPage(PageType)
                    TempDictionary = DictionaryOfStructs[PageType]
                    PageElementList = DictionaryOfPageTypeToListOfPageElements[PageType]
                    for index in range(len(PageElementList)):
                        PageElement = PageElementList[index]
                        if PageElement.find('*') != -1:
                            PageElement = PageElement.strip('*')
                        
                        PageElementType = DictionaryOfStructs[PageType][PageElement]
                        SizeOfPageElement = SizeOf(PageElementType)
                        if PageElementType.endswith('_ts'):
                            ListOfStructToBeAddedInMapFile.append(PageElementType)
                        IndicesList = GenerateSerialIndicesForPageElement(SizeOfPageElement,BaseAddress, Base)
                        BaseAddress = BaseAddress + SizeOfPageElement
                        FirmwareVariableSerialIndicesByName[PageName][PageElement] = IndicesList
                        FirmwareVariableTypesByName[PageName][PageElement] = PageElementType
                        if PageElement.startswith('e_'):
                            EnumType = GetEnumType(PageElement)
                            if Base == 0:
                                if DictOfEnumsInVirtualRegister.has_key(EnumType) == False:
                                   DictOfEnumsInVirtualRegister[EnumType] = EnumType
                            else:
                                if DictOfEnumsInVirtualRegister_WMM.has_key(EnumType) == False and DictOfEnumsInVirtualRegister.has_key(EnumType) == False:
                                   DictOfEnumsInVirtualRegister_WMM[EnumType] = EnumType
                    if BaseAddress%AlignmentConstraint != 0:
                        BaseAddress = ((BaseAddress/AlignmentConstraint) + 1)*AlignmentConstraint

def GetAlignmentConstraintOfPage(PageType):
    global DiscrepancyFound
    TempDictionary = {}
    PageElementList = []
    AlignmentConstraint = 0
    TempDictionary = DictionaryOfStructs[PageType]
    PageElementList = DictionaryOfPageTypeToListOfPageElements[PageType]
    if len(PageElementList) == 0:
        return -1
    FirstElement = PageElementList[0]
    DataType = TempDictionary[FirstElement]
    if DataType == 'void':
        if FirstElement.find('ptr') != -1:
            AlignmentConstraint = SizeOf('uint32_t')
        else:
##            print "Mismatch Statement with DataType -> " + str(DataType) + " and PageElement -> " + str(FirstElement)
            ReportFile.write("\nMismatch Statement with DataType -> " + str(DataType) + " and PageElement -> " + str(FirstElement) + "\n")
            print "Exiting......."
            DiscrepancyFound = 1
            sys.exit(2)
    elif DataType.endswith('_ts') == True:
        AlignmentConstraint = GetAlignmentConstraintOfPage(DataType)
    elif DataType.endswith('_tu') == True:
        return 0
    else:
        AlignmentConstraint = SizeOf(DataType)

    return AlignmentConstraint

def CheckAlignmentConstraintOfAllStructs():
    for Tuple in ListOfPageTypeNumOfElementsListOfPageElements:
        Struct = Tuple[0]
        if Struct.endswith('_ts') == True:# and IsDataTypeComposite(Struct) == "False":
            CheckAlignmentConstraintOfStruct(Struct)

def CheckAlignmentConstraintOfStruct(Struct):
    AlignmentConstraintsVoilated = 0
    AlignmentConstraint = GetAlignmentConstraintOfPage(Struct)
    if AlignmentConstraint == 0:
        AlignmentConstraintsVoilated = 1
    elif AlignmentConstraint == -1:
        AlignmentConstraintsVoilated = 0
    else:
##        print "AlignmentConstraints Voilated in Struct-> " + str(Struct)
        TempDictionary = DictionaryOfStructs[Struct]
        PageElementList = DictionaryOfPageTypeToListOfPageElements[Struct]
##        if DebugScript == '1':
##            DebugFile.write("\nTempDictionary-> " + str(TempDictionary) + "\n")
##            DebugFile.write("\nPageElementList-> " + str(PageElementList) + "\n")
        for PageElement in PageElementList:
            SizeOfPageElement = FindSizeOfPageElement(PageElement, TempDictionary)
            if SizeOfPageElement == 0:
                AlignmentConstraintsVoilated = 1
            else:
                Index = PageElementList.index(PageElement)
                for NewPageElement in PageElementList[:Index]:
                    SizeOfNewPageElement = FindSizeOfPageElement(NewPageElement, TempDictionary)
                    if SizeOfNewPageElement == 0:
                        AlignmentConstraintsVoilated = 1
                    elif SizeOfNewPageElement >= SizeOfPageElement:
                        pass
                    else:
                        AlignmentConstraintsVoilated = 1

    if AlignmentConstraintsVoilated == 1:
        print "Alignment Constraints Voilated in PageType --> " + str(Struct)
        return 0
    else:
        return 1

def FindSizeOfPageElement(PageElement, TempDictionary):
    global DiscrepancyFound
    DataType = TempDictionary[PageElement]
    if PageElement.find('ptr') != -1:
        SizeOfPageElement = SizeOf('uint32_t')
    elif PageElement.find('ptr') == -1 and DataType == 'void':
##        print "Mismatch Statement with DataType -> " + str(DataType) + " and PageElement -> " + str(PageElement)
        ReportFile.write("\nMismatch Statement with DataType -> " + str(DataType) + " and PageElement -> " + str(PageElement) + "\n")
        DiscrepancyFound = 1
        print "Exiting......."
        sys.exit(2)
    elif DataType.endswith('_ts') == True:
        SizeOfPageElement = GetAlignmentConstraintOfPage(DataType)
    elif DataType.endswith('_tu') == True:
        return 0
    else:
        SizeOfPageElement = SizeOf(DataType)

    return SizeOfPageElement        
    
    
def GenerateSerialIndicesForPageElement(SizeOfPageElement,BaseAddress, Base):
    list = []
    index = 0
    for index in range(int(SizeOfPageElement)):
        list.append(Base + BaseAddress + index)
    
    return list

def SizeOf(DataType):
    Size = 0
    if DataType in DictionaryOfDataTypeSize.keys() or DataType in DictionaryOfStructs.keys() or DataType == 'void':
        if DataType.endswith('_te') == True or DataType.endswith('_tef') == True:
        #The DataType is of Enum Type
            Size = SizeOf('uint8_t')
        elif DataType.endswith('_ts') == True:
            TempDictionary = {}
            PageElementList = []
            TempDictionary = DictionaryOfStructs[DataType]
            PageElementList = DictionaryOfPageTypeToListOfPageElements[DataType]
            for PageElement in PageElementList:
                DataType = TempDictionary[PageElement]
                if PageElement.find('ptr') != -1:
                    Size = Size + SizeOf('uint32_t')
                else:
                    Size = Size + SizeOf(DataType)
        elif DataType.endswith('_tu') == True:
            TempDictionary = {}
            PageElementList = []
            TempDictionary = DictionaryOfStructs[DataType]
            PageElementList = DictionaryOfPageTypeToListOfPageElements[DataType]
            for PageElement in PageElementList:
                DataType = TempDictionary[PageElement]
                if PageElement.find('ptr') != -1:
                    SizeOfDataType = SizeOf('uint32_t')
                else:
                    SizeOfDataType = SizeOf(DataType)                    
                if Size < SizeOfDataType:
                    Size = SizeOfDataType
        elif DataType == 'void':
            # assuming the variable to be a pointer
            Size = SizeOf('uint32_t')
        else:        
            Size = DictionaryOfDataTypeSize[DataType]
        return Size
    else:
        if DebugScript == '1':
            DebugFile.write("\nUnknown DataType-> " + DataType + ".\n")
        return 0

def CreatePageElementInfo(FileName):
    global DiscrepancyFound
    BoundaryString = '//-------------------------------------------------------------------------------------------------'
    BoundaryString2 = '//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++'

    try:
        ReadFile = open(FileName,"rb")
    except:
        print "File" +  FileName + "cannot be opened for Reading... \n"
        print "Exiting......."
        sys.exit(2)

    ListOfInputs = ReadFile.readlines()
    ListOfLinesOfPageElementInfo = []

    BoundaryStringStartFound = 0
    ClearBelowLines = 0
    ClearNow = 0

    if FileName.endswith('_WMM.c'):
        ListOfLinesOfPageElementInfo.append('\n\nconst PageElementInfo_ts g_PageElementInfo_WMM[ ] = {\n')
    else:
        ListOfLinesOfPageElementInfo.append('\n\nconst PageElementInfo_ts g_PageElementInfo[ ] = {\n')
    LineCount = 1
    LineStart = 1
    skip_next_line = 0
    for Line in ListOfInputs:
        skip_this_line = 0
        #Now Find for the Boundary String.
        if Line.startswith(BoundaryString):
            if BoundaryStringStartFound == 0:
                BoundaryStringStartFound = 1
            else:
                ClearBelowLines = 1
                ListOfLinesOfPageElementInfo.append('\n};\n')
                ListOfLinesOfPageElementInfo.reverse()

        elif ClearBelowLines != 1:
            if BoundaryStringStartFound == 1:
                Line = Line.strip('//')
                List = Line.split()
                if len(List)>0:
                    if List[0] == '#if' and len(List) >= 2:
                        macro_notted = 0
                        if List[1].startswith('!'):
                            List[1] = List[1].strip('!')
                            macro_notted = 1
                        if ((GetLeafValueFromHashDefineDictionary(List[1]) == '0' and macro_notted == 0) or (GetLeafValueFromHashDefineDictionary(List[1]) != '0' and macro_notted == 1)):
                            skip_next_line = 1
                        skip_this_line = 1
                    if List[0] == '#endif':
                        skip_next_line = 0
                        skip_this_line = 1
                if (len(List)>0 and skip_next_line == 0 and skip_this_line == 0):
                    PageName = List[0]
                    if PageName.find('[') != -1:
                        PageType = PageNameToPageTypeDictionary[PageName[:PageName.find('[')]]
                    else:
                        PageType = PageNameToPageTypeDictionary[PageName]
                    ListOfPageElements = DictionaryOfPageTypeToListOfPageElements[PageType]
                    SizeOf32BitData = 0
                    SizeOf16BitData = 0

                    for PageElement in ListOfPageElements:
                        PageElementType = DictionaryOfStructs[PageType][PageElement]
                        SizeOfPageElement = SizeOf(PageElementType)
                        if SizeOfPageElement == 4:
                            SizeOf32BitData = SizeOf32BitData + 4
                        elif SizeOfPageElement == 2:
                            SizeOf16BitData = SizeOf16BitData + 2

                    ListOfLinesOfPageElementInfo.append('		{(void *) &' + str(PageName) + ",\t" + str(SizeOf32BitData) + ",\t" + str(SizeOf16BitData) + ",\t" + str(List[1]) + ",\t" + str(List[2]) + "\t},\n" )
        if ClearBelowLines == 1:
            if Line.startswith(BoundaryString2):
                ClearNow = 1
                LineEnd = LineCount
                
        LineCount = LineCount + 1
    ListOfInputs = ListOfInputs[:LineEnd]
    ReadFile.close()

    if BoundaryStringStartFound != 1:
        ReportFile.write("\nBoundary String \n" + BoundaryString + "\nnot found in the file " + FileName)
        DiscrepancyFound = 1
    if ClearNow != 1:
        ReportFile.write("\nBoundary String2 \n" + BoundaryString2 + "\nnot found in the file " + FileName)
        DiscrepancyFound = 1
        
    try:
        AppendFile = open(FileName,"wb")
    except:
        print "File" +  FileName + "cannot be opened for Writing... \n"
        print "Exiting......."
        sys.exit(2)

    for Line in ListOfInputs:
        AppendFile.write(Line)

    while len(ListOfLinesOfPageElementInfo) != 0:
        AppendFile.write(ListOfLinesOfPageElementInfo.pop())

    AppendFile.close()

def ScanVirtualRegisterListFile():
    FileName = DirPathName + "/Misc/VirtualRegisterList.c"
    CreatePageElementInfo(FileName)

    try:
        InputFile = open(FileName,"rb")
    except:
        print "File" +  FileName + "cannot be opened for Reading... \n"
        print "Exiting......."
        sys.exit(2)
    
    ListOfInputs = InputFile.readlines()
    MultipleLineCommentEntered = 0
    SingleLineCommentAt = 0
    skip_next_line = 0
    for Line in ListOfInputs:
        skip_this_line = 0
        List = Line.split()
        if Line.strip().startswith('/*') == True:
            MultipleLineCommentEntered = 1
        elif MultipleLineCommentEntered == 1:
            if Line.find('*/')!= -1:
                MultipleLineCommentEntered = 0        

        SingleLineCommentAt = Line.find('//')
        if SingleLineCommentAt != -1:
            Line = Line[0:SingleLineCommentAt]
        
        if len(List) != 0 and (Line.strip()).startswith('//') != True and MultipleLineCommentEntered == 0:
            Line = Line.strip().strip('{(void *)').strip().strip(',').strip().strip('}').strip()

            PageNameFound = Line.startswith('&')

            if List[0] == '#if' and len(List) >= 2:
                macro_notted = 0
                if List[1].startswith('!'):
                    List[1] = List[1].strip('!')
                    macro_notted = 1
                if ((GetLeafValueFromHashDefineDictionary(List[1]) == '0' and macro_notted == 0) or (GetLeafValueFromHashDefineDictionary(List[1]) != '0' and macro_notted == 1)):
                    skip_next_line = 1
                skip_this_line = 1
            if List[0] == '#endif':
                skip_next_line = 0
                skip_this_line = 1

            if PageNameFound == True and skip_next_line == 0 and skip_this_line == 0:
                # A Page Entry has been reached
                List = Line.split()
                if len(List) !=0:
                    if len(List[0]) == 1:
                        PageName = List[1]
                    else:
                        PageName = List[0][1:]
                PageInfo = List[len(List) -1]
                VirtualRegisterPageList.append(PageName.strip(','))
                VirtualRegisterPageInfoList.append(PageInfo)

def ScanVirtualRegisterListFile_WMM():
    FileName = DirPathName + "/Misc/VirtualRegisterList_WMM.c"
    CreatePageElementInfo(FileName)

    try:
        InputFile = open(FileName,"rb")
    except:
        print "File" +  FileName + "cannot be opened for Reading... \n"
        print "Exiting......."
        sys.exit(2)
    
    ListOfInputs = InputFile.readlines()
    MultipleLineCommentEntered = 0
    SingleLineCommentAt = 0
    
    for Line in ListOfInputs:
        List = Line.split()
        if Line.strip().startswith('/*') == True:
            MultipleLineCommentEntered = 1
        elif MultipleLineCommentEntered == 1:
            if Line.find('*/')!= -1:
                MultipleLineCommentEntered = 0        

        SingleLineCommentAt = Line.find('//')
        if SingleLineCommentAt != -1:
            Line = Line[0:SingleLineCommentAt]
        
        if len(List) != 0 and (Line.strip()).startswith('//') != True and MultipleLineCommentEntered == 0:
            Line = Line.strip().strip('{(void *)').strip().strip(',').strip().strip('}').strip()

            PageNameFound = Line.startswith('&')
            if PageNameFound == False:
                continue
            else:
                # A Page Entry has been reached
                List = Line.split()
                if len(List) !=0:
                    if len(List[0]) == 1:
                        PageName = List[1]
                    else:
                        PageName = List[0][1:]
                PageInfo = List[len(List) -1]
                VirtualRegisterPageList_WMM.append(PageName.strip(','))
                VirtualRegisterPageInfoList_WMM.append(PageInfo)

def UpdateHashDefineDictionary(Line):
    List = Line.split()
    if len(List)==3 and List[2].find('.') == -1:
        #The Hash Define is of the form ------> #define Variable Value
        List[2] = List[2].strip('()')
        if List[1] in DeviceParametersList:
            List[2] = str(List[2])
        HashDefineDictionary[List[1]] = List[2]

def UpdateHashDefineList():
    for index in HashDefineDictionary.keys():
##        if DebugScript == '1':
##            DebugFile.write("\nindex = " + str(index))
        if index.startswith('(')==True:
            index = index[1:len(index) - 1]
        LeafValue = GetLeafValueFromHashDefineDictionary(index).strip('UL')
##        if DebugScript == '1':
##            DebugFile.write("\nLeafValue = " + str(LeafValue))
        if LeafValue != "None":
            HashDefineList.append((index,LeafValue))

def GetLeafValueFromHashDefineDictionary(Input):
    if Input.startswith('(')==True and Input.endswith(')')==True:
        Input = Input[1:len(Input) - 1]
    if Input in HashDefineDictionary.keys():
        if HashDefineDictionary[Input].isalnum()== True and HashDefineDictionary[Input].isalpha()== False and HashDefineDictionary[Input] not in HashDefineDictionary.keys():
            return HashDefineDictionary[Input]
        ReturnValue = GetLeafValueFromHashDefineDictionary(HashDefineDictionary[Input])
        return ReturnValue
    else:
        return "None"

def GenerateHashDefineList(ListOfFilePaths):
##    print "\nNow Generating Hash Define List\n"
    if DebugScript == '1':
        DebugFile.write("\nNow Generating Hash Define List\n")
    for File in ListOfFilePaths:
        try:
            InputFile = open(File,"rb")
        except IOError:
            print "File" +  File + " cannot be opened for Reading... \n"
            print "Exiting......."
            sys.exit(2)
        
        ListOfInputs = InputFile.readlines()
        MultipleLineCommentEntered = 0
        SingleLineCommentAt = 0
        for Line in ListOfInputs:
##            if DebugScript == '1':
##                DebugFile.write("\nLine = " + Line)
            if Line.find('/*') != -1:
                MultipleLineCommentEntered = 1
                if Line.find('*/') != -1:
                    MultipleLineCommentEntered = 0
                    Line = Line[:Line.find('/*')] + Line[Line.find('*/') + 2: ]
            if MultipleLineCommentEntered == 1:
                if Line.find('*/')!= -1:
                    MultipleLineCommentEntered = 0
                    Line = Line[Line.find('*/') + 2: ]
            SingleLineCommentAt = Line.find('//')
            if SingleLineCommentAt != -1:
                Line = Line[0:SingleLineCommentAt]

            if MultipleLineCommentEntered == 0 and Line.find('#define') != -1:
                UpdateHashDefineDictionary(Line)
##    print "\nNow Updating Hash Define List.\n"
    if DebugScript == '1':
        DebugFile.write("\nNow Updating Hash Define List.\n")
    UpdateHashDefineList()    
    
def GeneratePageList(File,ListOfInputs):
    global DiscrepancyFound
##    if DebugScript == '1':
##        DebugFile.write("\nFile = " + str(File))
    MultipleLineCommentEntered = 0
    SingleLineCommentAt = 0
    ColonAt = 0
    for Line in ListOfInputs:
##        if DebugScript == '1':
##            DebugFile.write("\nLine = " + Line)
        if Line.strip().startswith('/*') == True:
            MultipleLineCommentEntered = 1
        if MultipleLineCommentEntered == 1:
            if Line.find('*/')!= -1:
                MultipleLineCommentEntered = 0

        SingleLineCommentAt = Line.find('//')
        if SingleLineCommentAt != -1:
            Line = Line[0:SingleLineCommentAt]

        ColonAt = Line.find(':')
        if ColonAt != -1:
            Line = Line[0:ColonAt].strip()

        List = Line.split()
##        if DebugScript == '1':
##            DebugFile.write("\nList = " + str(List))

##        if (Line.find('#define') != -1):
##            UpdateHashDefineDictionary(Line)
        if len(List) != 0 and (Line.strip()).startswith('//') != True and MultipleLineCommentEntered == 0 and List[0] != 'extern' and IsFunctionDeclarationPresentInLine(Line) == False:
##            if DebugScript == '1':
##                DebugFile.write("\nList2 = " + str(List))
            index = count = 0
            for item in List:
##                if DebugScript == '1':
##                    DebugFile.write("\nitem = " + str(item))
                if item in DictionaryOfStructs.keys():
                    index = count
                count = count + 1
            if List[index] in DictionaryOfStructs.keys():
                datatype = List[index]
##                if DebugScript == '1':
##                    DebugFile.write("\ndatatype = " + str(datatype))
                if datatype.endswith('_te') == True:
##                    if DebugScript == '1':
##                        DebugFile.write("\ndatatype " + str(datatype) + " is of enum type.\nThe datatype should be 'uint8_t'")
                    ReportFile.write("\ndatatype " + str(datatype) + " is of enum type.\nThe datatype should be 'uint8_t'" + "\n")
                    DiscrepancyFound = 1
                count = index + 1
##                if DebugScript == '1':
##                    DebugFile.write("\ncount = " + str(count))
                while count < len(List):
##                    if DebugScript == '1':
##                        DebugFile.write("\ncount in loop = " + str(count) + " List[count] = " + str(List[count]))
                    if List[count] == ',':
                        count = count + 1
                    elif List[count] == "XDATA" or List[count] == "DATA" or List[count] == "CODE" or List[count] == "SDA" or List[count] == "const":
                        count = count + 1
                    elif List[count] == '=':
                        count = count + 1
                    elif List[count] == '*':
                        count = count + 1
                    elif List[count] != ';' or (List[count].find(']') != -1 and  List[count].find('[') != -1):
##                        if DebugScript == '1':
##                            DebugFile.write("\nList[count] = " + str(List[count]))
                        if List[count].find(']') != -1 and  List[count].find('[') != -1:
                            PageArrayName = List[count].strip(',').strip('=').strip(';')[:List[count].find('[')]
                            PageList.append(PageArrayName)
                            if List[count][List[count].find('[')+1:List[count].find(']')].strip().isdigit() == True:
                                LengthOfArray = List[count][List[count].find('[')+1:List[count].find(']')].strip()
##                                if DebugScript == '1':
##                                    DebugFile.write("\nList[count][List[count].find('[')+1:List[count].find(']')].strip() = " + List[count][List[count].find('[')+1:List[count].find(']')].strip())
##                                    DebugFile.write("\nLengthOfArray = " + str(LengthOfArray))
                            else:
                                LengthOfArray = FindLengthOfArray(Line, ListOfInputs)
##                                if DebugScript == '1':
##                                    DebugFile.write("\nFindLengthOfArray")
##                                    DebugFile.write("\nLengthOfArray = " + str(LengthOfArray))
                            PageArrayDictionary[PageArrayName] = LengthOfArray
                            PageNameToPageTypeDictionary[PageArrayName] = datatype
                        else:
                            PageList.append(List[count].strip(',').strip('=').strip(';'))
                            PageNameToPageTypeDictionary[List[count].strip(',').strip('=').strip(';')] = datatype
                        count = count + 1
                    elif List[count] == ';':
                        count = count + 1
            elif List[index] in DictionaryOfDataTypeSize.keys():
                count = index + 1
                while count < len(List):
                    if List[count] == ',':
                        count = count + 1
                    elif List[count] == "XDATA" or List[count] == "DATA" or List[count] == "CODE" or List[count] == "SDA" or List[count] == "const":
                        count = count + 1
                    elif List[count] == '=':
                        count = count + 1
                        break
                    elif List[count] == '*':
                        count = count + 1
                    elif List[count] != ';':
                        if List[count].startswith('e_') == True:
                            if GetEnumType(List[count]) != 'None' and List[count].strip('e_') != List[count].strip('e_').capitalize():
                                pass
                            else:
##                                print List[count] + " .........Does Not Follow Naming Conventions in File -> " + str(File) + "\n"
                                if (Mode == 'ModuleLevel' and File in ListOfModuleFilePaths) or (Mode == 'ProjectLevel' and DictionaryOfFilePathToFileName[File] not in ListOfToBeSkippedFileNames):
                                    ReportFile.write('\n' + List[count] + " .........Does Not Follow Naming Conventions in File -> " + str(File) + "\n")
                                    DiscrepancyFound = 1
                                
                        elif List[count].startswith(DictionaryOfDataStructures[List[index]]) or List[count].startswith("g" + DictionaryOfDataStructures[List[index]]) or List[count].startswith(",g" + DictionaryOfDataStructures[List[index]]) or List[count].startswith("," + DictionaryOfDataStructures[List[index]]) or List[count].startswith("*ptr" + DictionaryOfDataStructures[List[index]]) or List[count].startswith("*ptr" + "g" + DictionaryOfDataStructures[List[index]]) or List[count].startswith("ptr" + DictionaryOfDataStructures[List[index]]) or List[count].startswith("ptr" + "g" + DictionaryOfDataStructures[List[index]]):
                            pass
                        else:
##                            print List[count] + " .........Does Not Follow Naming Conventions in File -> " + str(File) + "\n"
                            if (Mode == 'ModuleLevel' and File in ListOfModuleFilePaths) or (Mode == 'ProjectLevel' and DictionaryOfFilePathToFileName[File] not in ListOfToBeSkippedFileNames):
                                ReportFile.write('\n' + List[count] + " .........Does Not Follow Naming Conventions in File -> " + str(File) + "\n")
                                DiscrepancyFound = 1
                    count = count + 1    

def FindLengthOfArray(Line, ListOfInputs):
    Index = ListOfInputs.index(Line)

    BraceCount = 0
    NumberOfElementsInArray = 0
    MultipleLineCommentEntered = 0

    for count1 in range(Index,len(ListOfInputs),10):
        CombinedString = ''
        Start = count1
        End = count1 + 10
        if End > len(ListOfInputs):
            End = len(ListOfInputs)
        for Line in ListOfInputs[Start:End]:
            SingleLineCommentAt = Line.find('//')
            if SingleLineCommentAt != -1:
                Line = Line[0:SingleLineCommentAt]
                  
            if Line.strip().startswith('/*') == True:
                MultipleLineCommentEntered = 1
            elif MultipleLineCommentEntered == 1:
                if Line.find('*/')!= -1:
                    MultipleLineCommentEntered = 0
            
            if MultipleLineCommentEntered == 0 and Line.find('*/') == -1:
                CombinedString = CombinedString + Line

        LengthOfCombinedString = len(CombinedString)

        for char in CombinedString:
            if char == '{':
                BraceCount = BraceCount + 1
            if char == '}':
                BraceCount = BraceCount - 1
                if BraceCount == 1:
                    NumberOfElementsInArray = NumberOfElementsInArray + 1
                if BraceCount == 0:
                    break

    return NumberOfElementsInArray

def SaveTypedefDefinition(LineStart,LineEnd,TypedefName,ListOfInputs,File):
    global DiscrepancyFound
    if DebugScript == '1':
        DebugFile.write('\nTypedefName = ' + TypedefName)
    TempDictionary = {}
    ListOfPageElements = []
    NumOfPageElements = 0
    EnumCount = 0
    SingleLineCommentAt = 0
    ColonAt = 0
    CompositeDataTypeFound = 0
    StartOfTypedefDefinition = LineStart
    skip_next_line = 0
    skip_this_line = 0
    while LineStart<LineEnd-1:
        skip_this_line = 0
        Line = ListOfInputs[LineStart]
        SingleLineCommentAt = Line.find('//')
        if SingleLineCommentAt != -1:
            Line = Line[0:SingleLineCommentAt]
            
        Line = Line.strip().strip('{').strip().strip('}').strip().strip(';').strip().strip(',').strip().strip('\\').strip().strip(';')
        if TypedefName.endswith('_Damper_ts'):
            print Line
        ColonAt = Line.find(':')
        if ColonAt != -1:
            Line = Line[0:ColonAt].strip()
        List = Line.split()
        if Line.find("enum") != -1 or Line.find("struct") != -1 or Line.find("union") != -1:
            CompositeDataTypeFound = 1
##            print "Composite DataType Declaration."
            if (Mode == 'ModuleLevel' and File in ListOfModuleFilePaths) or (Mode == 'ProjectLevel' and DictionaryOfFilePathToFileName[File] not in ListOfToBeSkippedFileNames):
                ReportFile.write("\n\nComposite DataType Declaration Found in File " + str(File) + " at lines between " + str(StartOfTypedefDefinition) + " & " + str(LineEnd) + "\n")
                DiscrepancyFound = 1

        if len(List) != 0 and (Line.strip()).startswith('//') != True and CompositeDataTypeFound == 0:
            if List[0] == '#if' and len(List) >= 2:
                macro_notted = 0
                if List[1].startswith('!'):
                    List[1] = List[1].strip('!')
                    macro_notted = 1
                if ((GetLeafValueFromHashDefineDictionary(List[1]) == '0' and macro_notted == 0) or (GetLeafValueFromHashDefineDictionary(List[1]) != '0' and macro_notted == 1)):
                    skip_next_line = 1
                skip_this_line = 1
            if List[0] == '#endif':
                skip_next_line = 0
                skip_this_line = 1
##            if DebugScript == '1':
##                DebugFile.write("\nskip_next_line = " + str(skip_next_line))
##                if len(List) >=2:
##                    DebugFile.write("\nList[1] = " + str(List[1]))
##                    DebugFile.write("\nValue = " + str(GetLeafValueFromHashDefineDictionary(List[1])))
            if skip_next_line == 0 and skip_this_line == 0:
                if TypedefName.endswith('_ts') or TypedefName.endswith('_tu'):
##                    if DebugScript == '1':
##                        DebugFile.write("\nList = " + str(List))
                    if len(List) >= 2:
                        if len(List) == 2:
                            List[1] = List[1].strip('*')
                        else:
                            if List[1] != '*':
    ##                            print "\nProcessed a typedef structure with unknown format\n"
                                ReportFile.write("\nProcessed a typedef structure with unknown format\n")
                                DiscrepancyFound = 1
                            else:
                                List.remove(List[1])
##                        if DebugScript == '1':
##                            DebugFile.write('\nList[1] = ' + str(List[1]))
                        if IsElementAnArray(List[1]) == 1:
                            ElementName = GetElementArrayName(List[1])
                            SizeOfArray = GetArraySize(List[1])
                            Count = 0
                            while (Count < SizeOfArray):
                                Element = ElementName + "[" + str(Count) + "]"
                                ListOfPageElements.append(Element)
                                Count = Count + 1
                                TempDictionary[Element] = List[0]
                            NumOfPageElements = NumOfPageElements + SizeOfArray
                        else:
                            ElementName = List[1]
                            ListOfPageElements.append(List[1])
                            NumOfPageElements = NumOfPageElements + 1
                            if ElementName.startswith('ptr') == True:
                                TempDictionary[ElementName] = 'uint32_t'
                                if List[0].endswith('_ts') == True:
                                    ListOfStructToBeAddedInMapFile.append(List[0])
                            else:
                                TempDictionary[ElementName] = List[0]
##                        if DebugScript == '1':
##                            DebugFile.write("\nElementName = " + str(ElementName))
##                            DebugFile.write("\nListOfPageElements = " + str(ListOfPageElements))
##                            DebugFile.write("\nNumOfPageElements = " + str(NumOfPageElements))
##                            DebugFile.write("\nTempDictionary = " + str(TempDictionary))

                elif TypedefName.endswith('_te') or TypedefName.endswith('_tef'):
                    List = Line.split('=')
                    if len(List) == 1:
                        TempDictionary[List[0].strip(' ,')] = EnumCount
                        EnumCount = EnumCount + 1
                        NumOfPageElements = NumOfPageElements + 1
                        ListOfPageElements.append(List[0].strip(' ,'))
                    elif len(List) == 2:
                        # PM: Modified to take into account cases like "CamDrvState_e_UnInitialize = 0, /* Un-initialized state */" in an enum
                        List[1] = List[1].strip(' ,').split(',')[0]
                        if (List[1].strip(' ,').startswith('0x') == True):
                            #assignment is in hex
                            #convert to int form
                            EnumCount = int(List[1].strip(' ,'),16)
                        else:
                            EnumCount = int(List[1].strip(' ,'),10)
                        
                        TempDictionary[List[0].strip(' ,')] = EnumCount
                        #EnumCount = int(List[1].strip('='),16)
                        EnumCount = EnumCount + 1
                        NumOfPageElements = NumOfPageElements + 1
                        ListOfPageElements.append(List[0].strip(' ,'))
    ##                elif len(List) == 3:
    ##                    TempDictionary[List[0]] = List[2]
    ##                    EnumCount = int(List[2],16)
    ##                    EnumCount = EnumCount + 1
    ##                    NumOfPageElements = NumOfPageElements + 1
    ##                    ListOfPageElements.append(List[0])
                
        LineStart = LineStart + 1
    
    if (CompositeDataTypeFound == 0):
        DictionaryOfStructs[TypedefName] = TempDictionary
        DictionaryOfPageTypeToListOfPageElements[TypedefName] = ListOfPageElements
        ListOfPageTypeNumOfElementsListOfPageElements.append((TypedefName,NumOfPageElements,ListOfPageElements,File))

def IsElementAnArray(Element):
    if Element.find('[') != -1 and Element.find(']') != -1:
        return 1
    else:
        return 0

def GetElementArrayName(Element):
    return  Element[:Element.find('[')]

def GetArraySize(Array):
    ArrayLengthString = Array[Array.find('[')+1:Array.find(']')].strip()
        
    if ArrayLengthString.isdigit() == True or ArrayLengthString.startswith('0x') == True or ArrayLengthString.startswith('0X') == True:
        ReturnValue = ArrayLengthString
    else:
        ReturnValue = HashDefineDictionary[ArrayLengthString]

    return ConvertStringToInteger(ReturnValue)

def ConvertStringToInteger(String):
    if String.isdigit() == True:
        return int(String,10)
    elif String.startswith('0x') == True or String.startswith('0X') == True:
        return int(String,16)
    
def SaveDataStructureDefinition(LineStart,LineEnd,DataStructureName,ListOfInputs,File):
    global DiscrepancyFound
    TempDictionary = {}
    ListOfPageElements = []
    NumOfPageElements = 0
    EnumCount = 0
    SingleLineCommentAt = 0
    ColonAt = 0
    CompositeDataTypeFound = 0
    StartOfDataStructureDefinition = LineStart
    while LineStart<LineEnd-1:
        Line = ListOfInputs[LineStart]
        SingleLineCommentAt = Line.find('//')
        if SingleLineCommentAt != -1:
            Line = Line[0:SingleLineCommentAt]
            
        Line = Line.strip().strip('{').strip().strip('}').strip().strip(';').strip().strip(',').strip()
        ColonAt = Line.find(':')
        if ColonAt != -1:
            Line = Line[0:ColonAt].strip()
        List = Line.split()
        if Line.find("enum") != -1 or Line.find("struct") != -1 or Line.find("union") != -1:
            CompositeDataTypeFound = 1
            print "Composite DataType Declaration."
            if (Mode == 'ModuleLevel' and File in ListOfModuleFilePaths) or (Mode == 'ProjectLevel' and DictionaryOfFilePathToFileName[File] not in ListOfToBeSkippedFileNames):
                ReportFile.write("\nComposite DataType Declaration Found in File " + str(File) + " at lines between " + str(StartOfDataStructureDefinition) + " & " + str(LineEnd) + "\n")
                DiscrepancyFound = 1

        if len(List) != 0 and (Line.strip()).startswith('//') != True and CompositeDataTypeFound == 0:
            if DataStructureName.endswith('_s') or DataStructureName.endswith('_u'):
                if len(List) == 2:
                    TempDictionary[List[1]] = List[0]
                    ListOfPageElements.append(List[1])
                    NumOfPageElements = NumOfPageElements + 1

            elif DataStructureName.endswith('_e') or DataStructureName.endswith('_ef'):
                if len(List) == 1:
                    TempDictionary[List[0]] = EnumCount
                    EnumCount = EnumCount + 1
                    NumOfPageElements = NumOfPageElements + 1
                    ListOfPageElements.append(List[0])
                elif len(List) == 2:
                    TempDictionary[List[0].strip('=')] = List[1].strip('=')
                    EnumCount = int(List[1].strip('='),16)
                    EnumCount = EnumCount + 1
                    NumOfPageElements = NumOfPageElements + 1
                    ListOfPageElements.append(List[0].strip('='))
                elif len(List) == 3:
                    TempDictionary[List[0]] = List[2]
                    EnumCount = int(List[2],16)
                    EnumCount = EnumCount + 1
                    NumOfPageElements = NumOfPageElements + 1
                    ListOfPageElements.append(List[0])
                
        LineStart = LineStart + 1
    if (CompositeDataTypeFound == 0):
        DictionaryOfStructs[DataStructureName] = TempDictionary
        DictionaryOfPageTypeToListOfPageElements[DataStructureName] = ListOfPageElements
        ListOfPageTypeNumOfElementsListOfPageElements.append((DataStructureName,NumOfPageElements,ListOfPageElements,File))
    if DebugScript == '1':
        DebugFile.write("\nDataStructureName = " + str(DataStructureName))
        DebugFile.write("\nTempDictionary = " + str(TempDictionary))
        DebugFile.write("\nListOfPageElements = " + str(ListOfPageElements))
    
def SaveElementaryDataType(Line):
    Line = Line.strip().strip(';').strip()
    List = Line.split()
    index = 1
    DataType = ""
    while index < len(List)-1:
        DataType = DataType + " " + str(List[index])
        index = index + 1
    DictionaryOfElementaryDataTypes[List[len(List) - 1]] = DataType.strip()

def GenerateMapFile(OutputFileName, VirtualRegisterPageList, VirtualRegisterPageInfoList):
    # Map File Header
##    OutputFileName = "V2WReg.map"
    if sys.argv[4]=="None":
        OutputFilePath = DirPathName + "//" + OutputFileName
    else:
        OutputFilePath = sys.argv[4] + "//" + OutputFileName

    OutputFile = open( OutputFilePath, "wb" )

    OutputFile.write("//\n")
    OutputFile.write("//              file: " + OutputFileName + " \n")
    OutputFile.write("//  firmware version: v" + str(FirmwareVersion['MAJOR']) + "." + str(FirmwareVersion['MINOR']) + "." + str(FirmwareVersion['MICRO']) + " (parsed from source files)\n")
    OutputFile.write("//\n")
    OutputFile.write("\n")
    OutputFile.write("$MAP\n")
    OutputFile.write("$COMMS_TYPE " + '"' + "MAILBOX" + '"' + "\n")
    OutputFile.write("$PARAM " + '"' + "8500v1" + '"' + "\n")
    OutputFile.write("Hex\n")
    OutputFile.write("Children\n")
    OutputFile.write("\n")
    OutputFile.write("Device \"8500 v" + str(FirmwareVersion['MAJOR']) + "." + str(FirmwareVersion['MINOR']) + "." + str(FirmwareVersion['MICRO']) + '"' + " Address 0x20")
    OutputFile.write("\n")

    #PageInfo[0] contains Name of the Page
    #PageInfo[1] contains a string "READ_ONLY_PAGE" or "READ_WRITE_PAGE"
    for Page in VirtualRegisterPageList:
        PageInfo = VirtualRegisterPageInfoList[VirtualRegisterPageList.index(Page)]
        PageType = ""
        PageElementList = []
        Index = -1
        if PageInfo == "READ_ONLY_PAGE":
            OutputFile.write("\tGroup \"" + str(Page[2:]) + " [read only]" + "\"\n")
        else:
            OutputFile.write("\tGroup \"" + str(Page[2:]) + "\"\n")
        if IsPageInArrayDictionary(Page) == 1:
            PageType = PageNameToPageTypeDictionary[GetPageArrayName(Page)]
        else:    
            PageType = PageNameToPageTypeDictionary[Page]
        PageElementList = DictionaryOfPageTypeToListOfPageElements[PageType]
        
        NumOfPageElements = len(PageElementList)
        for count in range(NumOfPageElements):
            PageElement = PageElementList[count]
            ListOfSerialIndices = FirmwareVariableSerialIndicesByName[Page][PageElement]
            OutputFile.write("\t\tItem \"" + str(PageElement) + "\" Index " + "0x%04x" %ListOfSerialIndices[0])
            PageElementType = DictionaryOfStructs[PageType][PageElement]
            SizeOfPageElement = SizeOf(PageElementType)
            if PageElementType.endswith('_te') or PageElementType.endswith('_tef'):
                #SizeOfPageElement is 1
    ##            OutputFile.write(" (7:0) Type \"uint8_t\" Lut \"" + str(PageElementType) + "\"\n")
                OutputFile.write(" NumBytes " + "1 ToggleEndian\n")
            elif PageElement.startswith('e_') == True:
    ##            OutputFile.write(" (7:0) Type \"uint8_t\" Lut \"" + GetEnumType(PageElement) + "\"\n")
                OutputFile.write(" NumBytes " + "1 ToggleEndian " + "Lut " + '"' + GetEnumType(PageElement) + '"' + "\n")
            elif PageElement.find('ptr') != -1:
    ##            OutputFile.write(" (31:0) Type \"" + 'uint32_t' + "\"\n")
                OutputFile.write(" NumBytes " + "4 ToggleEndian\n")
            elif PageElement.startswith('f_') == True:
                OutputFile.write(" NumBytes " + "4 ToggleEndian Float\n")
            else:
                if SizeOfPageElement == 1:
    ##                OutputFile.write(" (7:0) Type \"" + str(PageElementType) + "\"\n")
                    OutputFile.write(" NumBytes " + "1 ToggleEndian")
                elif SizeOfPageElement == 2:
    ##                OutputFile.write(" (15:0) Type \"" + str(PageElementType) + "\"\n")
                    OutputFile.write(" NumBytes " + "2 ToggleEndian")
                elif SizeOfPageElement == 4:
    ##                OutputFile.write(" (31:0) Type \"" + str(PageElementType) + "\"\n")
                    OutputFile.write(" NumBytes " + "4 ToggleEndian")
                if PageElement.startswith('s') == True:
                    OutputFile.write(" Signed" + " \n")
                else:
                    OutputFile.write(" \n")
        OutputFile.write("\tGroup End\n\n")
        
    OutputFile.write("End\n")

    for enum in EnumIntegerValueByStringDictionariesByType.keys():
        ListOfEnumEntries = DictionaryOfPageTypeToListOfPageElements[enum]
        NumberOfEntries = len(ListOfEnumEntries)
        OutputFile.write("\n[" + str(enum) + "]\n")
        OutputFile.write("entries = " + str(NumberOfEntries) + "\n")
        for index in range(NumberOfEntries):
            OutputFile.write("attr" + str(index) + " = " + str(ListOfEnumEntries[index]) + "\n")
            OutputFile.write("val" + str(index) + " = " + str(EnumIntegerValueByStringDictionariesByType[enum][ListOfEnumEntries[index]]) + "\n")

    for enum in FlagIndexByStringDictsByType.keys():
        ListOfEnumEntries = DictionaryOfPageTypeToListOfPageElements[enum]
        NumberOfEntries = len(ListOfEnumEntries)
        OutputFile.write("\n[" + str(enum) + "]\n")
        OutputFile.write("entries = " + str(NumberOfEntries) + "\n")
        for index in range(NumberOfEntries):
            OutputFile.write("attr" + str(index) + " = " + str(ListOfEnumEntries[index]) + "\n")
            OutputFile.write("val" + str(index) + " = " + str(FlagIndexByStringDictsByType[enum][ListOfEnumEntries[index]]) + "\n")

def GenerateTestFile(OutputFileName_TestName, VirtualRegisterPageList, DictOfEnumsInVirtualRegisterList, ListOfStructToBeAddedInMapFile = []):
    if sys.argv[4]=="None":
        OutputFilePath_TestFile = DirPathName + "//" + OutputFileName_TestName
    else:
        OutputFilePath_TestFile = sys.argv[4] + "//" + OutputFileName_TestName
    OutputFile_TestElements = open(OutputFilePath_TestFile, 'wb')

    OutputFile_TestElements.write("#ifndef _" + OutputFileName_TestName.replace('.', '_').upper() + "_ \n")
    OutputFile_TestElements.write("#define _" + OutputFileName_TestName.replace('.', '_').upper() + "_\n")
    OutputFile_TestElements.write("//\n")
    OutputFile_TestElements.write("//              file: " + OutputFileName_TestName + " \n")
    
    for File in CompleteListOfFilePaths:
        try:
            InputFile = open(File,"rb")
        except:
            print "File" +  File + "cannot be opened for Reading... \n"
            print "Exiting......."
            sys.exit(2)
            
        ListOfInputs = InputFile.readlines()
        #DictionaryOfDataStructures
        #LookForTypeDefSyntax(File, ListOfInputs)
   

def GenerateVhcElementsFile(OutputFileName_VhcElements, VirtualRegisterPageList, DictOfEnumsInVirtualRegisterList, ListOfStructToBeAddedInMapFile = []):
    ##OutputFileName_VhcElements = "baseline.h"

    if sys.argv[4]=="None":
        OutputFilePath_VhcElements = DirPathName + "//" + OutputFileName_VhcElements
    else:
        OutputFilePath_VhcElements = sys.argv[4] + "//" + OutputFileName_VhcElements

	OutputFile_VhcElements = open(OutputFilePath_VhcElements, 'wb')
	

    # CopyRight Information Generation
    OutputFile_VhcElements.write("/*\n")
    OutputFile_VhcElements.write(" *  © ST Microelectronics Pvt. Ltd, 2010 - All rights reserved\n")
    OutputFile_VhcElements.write(" *  Reproduction and Communication of this document is strictly prohibited\n")
    OutputFile_VhcElements.write(" *  unless specifically authorized in writing by ST-Microelectronics\n")
    OutputFile_VhcElements.write(" *\n")
    OutputFile_VhcElements.write(" */\n\n")

    OutputFile_VhcElements.write("/*\n")
    OutputFile_VhcElements.write(" * Copyright (C) ST-Ericsson SA 2012. All rights reserved.\n")
    OutputFile_VhcElements.write(" * The features are joint developement between ST-Ericsson SA and ST Microelectronics Pvt. Ltd. \n")
    OutputFile_VhcElements.write(" * This code is ST-Ericsson proprietary and confidential.\n")
    OutputFile_VhcElements.write(" * Any use of the code for whatever purpose is subject to\n")
    OutputFile_VhcElements.write(" * specific written permission of ST-Ericsson SA.\n")
    OutputFile_VhcElements.write(" */\n\n")

    OutputFile_VhcElements.write("#ifndef _" + OutputFileName_VhcElements.replace('.', '_').upper() + "_ \n")
    OutputFile_VhcElements.write("#define _" + OutputFileName_VhcElements.replace('.', '_').upper() + "_\n")
    OutputFile_VhcElements.write("//\n")
	
    OutputFile_VhcElements.write("//  FW_" + str(FirmwareVersion['MAJOR']) + "." + str(FirmwareVersion['MINOR']) + "." + str(FirmwareVersion['MICRO']) + "_SENSOR_" + str(FirmwareVersion['LOW_LEVEL_API_SENSOR']) + "_LLA_" + str(FirmwareVersion['CAM_DRV_API_VERSION_MAJOR']) + "." + str(FirmwareVersion['CAM_DRV_API_VERSION_MINOR'])+ "_LLCD_" + str(FirmwareVersion['CAM_DRV_DRIVER_VERSION_MAJOR']) + "." + str(FirmwareVersion['CAM_DRV_DRIVER_VERSION_MINOR']) + " (parsed from source files)\n")
    OutputFile_VhcElements.write("//  version info ->       FIRMWARE_VERSION_MAJOR : " + str(FirmwareVersion['MAJOR']) + " " + "Incremented With Compatibility break" + " (parsed from source files)\n")
    OutputFile_VhcElements.write("//  version info ->       FIRMWARE_VERSION_MINOR : " + str(FirmwareVersion['MINOR']) + " " + "Incremented With Feature addition w/o any break in compatibility" + " (parsed from source files)\n")
    OutputFile_VhcElements.write("//  version info ->       FIRMWARE_VERSION_MICRO : " + str(FirmwareVersion['MICRO']) + " " + "Incremented WithBug fixes" + " (parsed from source files)\n")
    OutputFile_VhcElements.write("//  version info ->         LOW_LEVEL_API_SENSOR : " + str(FirmwareVersion['LOW_LEVEL_API_SENSOR']) + " " + "customer identification" + " (parsed from source files)\n")
    OutputFile_VhcElements.write("//  version info ->  LOW_LEVEL_API_MAJOR_VERSION : " + str(FirmwareVersion['CAM_DRV_API_VERSION_MAJOR']) +" " + "Incremented when there is compatibility break in API and ISP FW update is necessary" + " (parsed from source files)\n")
    OutputFile_VhcElements.write("//  version info ->  LOW_LEVEL_API_MINOR_VERSION : " + str(FirmwareVersion['CAM_DRV_API_VERSION_MINOR']) + " " + "Incremented when there is change in API or feature addition that is backward compatible" + " (parsed from source files)\n")
    OutputFile_VhcElements.write("//  version info ->LOW_LEVEL_CAMERA_DRIVER_MAJOR : " + str(FirmwareVersion['CAM_DRV_DRIVER_VERSION_MAJOR']) + " " + "Incremented with every release" + " (parsed from source files)\n")
    OutputFile_VhcElements.write("//  version info ->LOW_LEVEL_CAMERA_DRIVER_MINOR : " + str(FirmwareVersion['CAM_DRV_DRIVER_VERSION_MINOR']) + " " + "Incremented each time there is bug fix on old API" + " (parsed from source files)\n")

    OutputFile_VhcElements.write("//\n\n")
    OutputFile_VhcElements.write("// List all the page elements and enum types used in the page element definition\n")
    OutputFile_VhcElements.write("//\n\n")

    OutputFile_VhcElements.write("\n\n// *****************************************\n")
    OutputFile_VhcElements.write("//       Enums used by Page Elements\n")
    OutputFile_VhcElements.write("// *****************************************\n")
    OutputFile_VhcElements.write("// *****************************************\n")
    #sort_by_value(EnumIntegerValueByStringDictionariesByType)
    #sorted(EnumIntegerValueByStringDictionariesByType.items(),sortfunc)
    #sorted_list = [x for x in EnumIntegerValueByStringDictionariesByType.iteritems()]
    #sorted_list.sort(key=lambda x: x[0]) # sort by key
    #for key in sorted(EnumIntegerValueByStringDictionariesByType):
    #    EnumIntegerValueByStringDictionariesByTypeSTE[key]=EnumIntegerValueByStringDictionariesByType[key]
    #    print "key, =>" + str(key) +" Values "+str(EnumIntegerValueByStringDictionariesByType[key])
	

    KS= EnumIntegerValueByStringDictionariesByType.keys()
    KS.sort()

    #for enum in EnumIntegerValueByStringDictionariesByTypeSTE.keys():
    for enum in KS:
        #print " STE_Noida Enum " +   str(enum) + " EnumIntegerValueByStringDictionariesByType  "+str(EnumIntegerValueByStringDictionariesByType[enum]) + "\n"
        if (DictOfEnumsInVirtualRegisterList.has_key(enum) == True):
            ListOfEnumEntries = DictionaryOfPageTypeToListOfPageElements[enum]
            NumberOfEntries = len(ListOfEnumEntries)
            OutputFile_VhcElements.write("\ntypedef enum\n" + "{\n")
            for index in range(NumberOfEntries):
                OutputFile_VhcElements.write("\t" + str(ListOfEnumEntries[index]) + " = " + str(EnumIntegerValueByStringDictionariesByType[enum][ListOfEnumEntries[index]]) + ",\n")
            OutputFile_VhcElements.write("} " + str(enum) +";\n")
    for enum in FlagIndexByStringDictsByType.keys():
        if (DictOfEnumsInVirtualRegisterList.has_key(enum) == True):
            ListOfEnumEntries = DictionaryOfPageTypeToListOfPageElements[enum]
            NumberOfEntries = len(ListOfEnumEntries)
            OutputFile_VhcElements.write("\ntypedef enum\n" + "{\n")
            for index in range(NumberOfEntries):
                OutputFile_VhcElements.write("\t" + str(ListOfEnumEntries[index]) + " = " + str(FlagIndexByStringDictsByType[enum][ListOfEnumEntries[index]]) + ",\n")
            OutputFile_VhcElements.write("} " + str(enum) +";\n")
    if len(ListOfStructToBeAddedInMapFile) != 0:
        if OutputFileName_VhcElements == "baseline.h":
            AddDataTypeDefinitionsToVhcElementDefsFile(OutputFile_VhcElements)
        
        for struct in ListOfStructToBeAddedInMapFile:
            ListOfPageElements = DictionaryOfPageTypeToListOfPageElements[struct]
            OutputFile_VhcElements.write("\ntypedef struct\n" + "{\n")
            for page_element in ListOfPageElements:
                if page_element.startswith('ptr'):
                    new_page_element = '* ' + page_element
                else:
                    new_page_element = page_element
                pe_type = 'isp_' + str(DictionaryOfStructs[struct][page_element])   
                OutputFile_VhcElements.write("\t" + pe_type + "\t" + str(new_page_element) + ";\n")   
            OutputFile_VhcElements.write("} " + str(struct) +";\n")
    OutputFile_VhcElements.write("\n\n// *****************************************\n")
    OutputFile_VhcElements.write("//       Page Elements\n")
    OutputFile_VhcElements.write("// *****************************************\n")
    OutputFile_VhcElements.write("// *****************************************\n")

    for Page in VirtualRegisterPageList:
        PageInArrayList = 0 
        PageInArrayList = IsPageInArrayDictionary(Page)
        if PageInArrayList == 1:
            PageArrayName = GetPageArrayName(Page)
            PageType = PageNameToPageTypeDictionary[GetPageArrayName(Page)]
            PageIndex = "_" + Page[Page.find('[')+1: Page.find(']')] + "_"
            PageName = PageArrayName
        else:
            PageType = PageNameToPageTypeDictionary[Page]
            PageIndex = "_"
            PageName = Page
        PageElementList = DictionaryOfPageTypeToListOfPageElements[PageType]
        PageElementDictionary = FirmwareVariableSerialIndicesByName[Page]
        OutputFile_VhcElements.write("\n// page '" + str(Page) + "'\n\n")
        for PageElement in PageElementList:
            IndicesList = PageElementDictionary[PageElement]
            PageElementIndex = "_"
            if IsElementAnArray(PageElement) == 1:
                PageElementIndex = "_" + str(GetArraySize(PageElement)) + "_"
                PageElement = GetElementArrayName(PageElement)

            for Index in range(len(IndicesList)):
                OutputFile_VhcElements.write("#define " + str(PageName[2:]) + str(PageIndex) + str(PageElement) + str(PageElementIndex) + "Byte" + str(Index) + "\t\t" + "0x%04x" %IndicesList[Index] + "\n")        

    OutputFile_VhcElements.write("#endif	// _" + OutputFileName_VhcElements.replace('.', '_').upper() + "_\n")
    OutputFile_VhcElements.close()            

def GenerateSensorIdFile(OutputFileName_VhcElements):
    ##OutputFileName = "SensorId.h"
    if sys.argv[4]=="None":
        OutputFilePath_VhcElements = DirPathName + "//" + OutputFileName_VhcElements
    else:
        OutputFilePath_VhcElements = sys.argv[4] + "//" + OutputFileName_VhcElements

	OutputFile_VhcElements = open(OutputFilePath_VhcElements, 'wb')

    OutputFile_VhcElements.write("#define LOW_LEVEL_API_SENSOR " + str(FirmwareVersion['LOW_LEVEL_API_SENSOR']) + "\n")

    OutputFile_VhcElements.close()            	
####################################################################################
#      A few Global Dictionaries and Lists used in theis Script.                   #
####################################################################################
    
DictionaryOfDataStructures = {
    'int8_t'    : 's8',
    'uint8_t'   : 'u8',
    'int16_t'   : 's16',
    'uint16_t'  : 'u16',
    'int32_t'   : 's32',
    'uint32_t'  : 'u32',
    'int64_t'   : 's64',
    'uint64_t'  : 'u64',
    'float_t'   : 'f',
    'bool_t'    : 'bo',
    'struct'    : '_s',
    'enum'      : '_e',
    'union'     : '_u',
    'Flag_e'    : 'f'
    
    }

ListOfDataStructures = [
    'int8_t'    ,
    'uint8_t'   ,
    'int16_t'   ,
    'uint16_t'  ,
    'int32_t'   ,
    'uint32_t'  ,
    'int64_t'   ,
    'uint64_t'  ,
    'float_t'   ,
    'bool_t'    ,
    'Flag_e'

    ]

DictionaryOfDataTypeSize = {
    'int8_t'    : 1,
    'uint8_t'   : 1,
    'int16_t'   : 2,
    'uint16_t'  : 2,
    'int32_t'   : 4,
    'uint32_t'  : 4,
    'int64_t'   : 8,
    'uint64_t'  : 8,
    'float_t'   : 4,
    'bool_t'    : 1,
    'Flag_e'    : 1
    }

DictionaryOfTypedef = {
    'struct'      : '_ts',
    'enum'      : '_te',
    'union'      : '_tu'

    }

PageList = []
PageArrayDictionary = {}
DictionaryOfStructs = {}
DictionaryOfElementaryDataTypes = {}
DictOfEnumsInVirtualRegister = {}
DictOfEnumsInVirtualRegister_WMM = {}
ListOfStructToBeAddedInMapFile = ['AFStats_HostZoneConfigPercentage_ts','AFStats_HostZoneStatus_ts']
# assumming this structure is declared in advance
ListOfStructToBeAddedInWMMMapFile = ['FrameParamStatus_ts','FrameParamStatus_Extn_ts','FrameParamStatus_Af_ts','Sensor_Output_Mode_ts']
ListOfFilePaths = []
CompleteListOfFilePaths = []
AllVirtualRegisterPageList = []
VirtualRegisterPageList = []
VirtualRegisterPageList_WMM = []
AllVirtualRegisterPageInfoList = []
VirtualRegisterPageInfoList = []
VirtualRegisterPageInfoList_WMM = []
FirmwareVariableSerialIndicesByName = {}
FirmwareVariableTypesByName = {}
PageNameToPageTypeDictionary = {}
FlagIndexByStringDictsByType = {}
EnumIntegerValueByStringDictionariesByType = {}
FirmwareVersionMajorFound = 0
FirmwareVersionMinorFound = 0
FirmwareVersionMicroFound = 0
global DiscrepancyFound
DiscrepancyFound = 0
FirmwareVersion = {'MAJOR':-1,
                   'MINOR':-1,
                   'MICRO':-1,
                   'LOW_LEVEL_API_SENSOR':-1,
                   'CAM_DRV_API_VERSION_MAJOR':-1,
                   'CAM_DRV_API_VERSION_MINOR':-1,
                   'CAM_DRV_DRIVER_VERSION_MAJOR':-1,
                   'CAM_DRV_DRIVER_VERSION_MINOR':-1,
                   }
ListOfPageTypeNumOfElementsListOfPageElements = []
DictionaryOfPageTypeToListOfPageElements = {}
HashDefineList = []
HashDefineListFromTypedefEnum = []
FunctionList = []
ConfigurationHeaderFileList = []
TempConfigurationHeaderModuleList = []
ActualConfigurationHeaderModuleList = []
ConfigurationHeaderModulesFilePathList = []
ListOfTopDirectoriesInProjectPath = []
HashDefineDictionary = {
##    'DeviceParameters' : 'DeviceParameters',
##                        'uwDeviceId' : 'uwDeviceId',
##                        'bFirmwareVersionMajor' : 'bFirmwareVersionMajor',
##                        'bFirmwareVersionMinor' : 'bFirmwareVersionMinor',
##                        'bHardwareVersionMajor' : 'bHardwareVersionMajor',
##                        'bHardwareVersionMinor' : 'bHardwareVersionMinor',
##                        'commsType' : 'MailBox',
##                        'Endian' : 'BigEndian'
                        }
DeviceParametersList = ['DeviceParameters',
                        'uwDeviceId',
                        'bFirmwareVersionMajor',
                        'bFirmwareVersionMinor',
                        'bHardwareVersionMajor',
                        'bHardwareVersionMinor'
                        ]
DamperParamsList = ['dimension',
                    'count_0',
                    'count_1',
                    'params',
                    'IP_name',
                    'addr'
                    ]
DamperParamsDictionary = {}

#################################################################################
#      The Script starts from here.                                             #
#################################################################################

DirPathName = sys.argv[1]
ModuleName = sys.argv[2]
ModulesToBeSkipped = sys.argv[5]# contains the list of modules to be skipped. The ModuleNames must be separated by commas with no spaces
Mode = sys.argv[6]
DebugScript = sys.argv[7]
ReportFileName = "CodingConventionsReport.txt"

ReportFilePath = sys.argv[3] + "//" + ReportFileName

ReportFile = open(ReportFilePath,"wb")

if DebugScript == '1':
    DebugFilePath = sys.argv[3] + "//" + "Debug.txt"
    DebugFile = open(DebugFilePath,"ab")

ReportFile.write("\n## This is a Coding Conventions Report File.")
ReportFile.write("\n## It reports, if there is any deviation from the Coding Guidelines.")
ReportFile.write("\n## Discrepancies found are listed below:")
ReportFile.write("\n##\n")
ReportFile.write("\n## Scanning Module -> " + str(ModuleName) + "\n")


#################################################################################
#      Scanning the Project Directory                                           #
#################################################################################

d=()
index = 0
DirectoryTuples = []
FileName = ""
FilePath = ""
ModulePath = ""
ListOfDirectoryTuples = []
ListOfModuleDirectoryTuples = []
ListOfToBeSkippedModuleDirectoryTuples = []
ListOfToBeSkippedFileNames =[]
DictionaryOfFilePathToFileName = {}

##if Mode == "ProjectLevel":
ListOfModulesToBeSkipped = []
ListOfModulesToBeSkipped = ModulesToBeSkipped.split(',')
    
ListOfToBeSkippedModulePaths = []

# Collect list of each directory and its contents in ListOfDirectoryTuples as (dirpath, dirnames, filenames).
for d in os.walk(DirPathName):
    ListOfDirectoryTuples.append(d)
##
##      len(ListOfDirectoryTuples) gives the number of directories scanned including the Input Directory
for index in range(len(ListOfDirectoryTuples)):
    for count in range(len(ListOfDirectoryTuples[index][2])):
        FileName = str(ListOfDirectoryTuples[index][2][count])
        ###ListOfFilePaths.append(FileName)
##        if Mode == "ProjectLevel":
        file_path = str(ListOfDirectoryTuples[index][0]) + "/" + FileName
        if file_path.endswith('.c') or file_path.endswith('.h'):
            CompleteListOfFilePaths.append(file_path)

        List = []
        module_name = ''
        if os.name == 'nt':
            List = ListOfDirectoryTuples[index][0].split('\\')
        else :
            List = ListOfDirectoryTuples[index][0].split('/')
        module_name = List[len(List) - 1]
        if module_name in ListOfModulesToBeSkipped:
            pass
        else:
            
            FilePath = str(ListOfDirectoryTuples[index][0]) + "/" + FileName
            if FilePath.endswith('.c') or FilePath.endswith('.h'):
                ListOfFilePaths.append(FilePath)
                DictionaryOfFilePathToFileName[FilePath] = FileName
##        else:
##            FilePath = str(ListOfDirectoryTuples[index][0]) + "/" + FileName
##            if FilePath.endswith('.c') or FilePath.endswith('.h'):
##                ListOfFilePaths.append(FilePath)
##                DictionaryOfFilePathToFileName[FilePath] = FileName
            

##if DebugScript == '1':
##    DebugFile.write("\nListOfDirectoryTuples = ")
##    for Tuple in ListOfDirectoryTuples:
##        DebugFile.write("\n" + str(Tuple))
##
##    DebugFile.write("\nListOfFilePaths = ")
##    for File in ListOfFilePaths:
##        DebugFile.write("\n" + File)

if Mode == "ModuleLevel":
    ModulePath = ListOfDirectoryTuples[0][0] + "/" + ListOfDirectoryTuples[0][1][ListOfDirectoryTuples[0][1].index(ModuleName)]
    for d in os.walk(ModulePath):
        ListOfModuleDirectoryTuples.append(d)

    ListOfModuleFilePaths = []
    for index in range(len(ListOfModuleDirectoryTuples)):
        for count in range(len(ListOfModuleDirectoryTuples[index][2])):
            FileName = str(ListOfModuleDirectoryTuples[index][2][count])
            FilePath = str(ListOfModuleDirectoryTuples[index][0]) + "/" + FileName
            if FilePath.endswith('.c') or FilePath.endswith('.h'):
                ListOfModuleFilePaths.append(FilePath)

if Mode == "ProjectLevel":
    ListOfModulesToBeSkipped = []
    ListOfToBeSkippedModulePaths = []

    ListOfModulesToBeSkipped = ModulesToBeSkipped.split(',')

    for ModuleToBeSkipped in ListOfModulesToBeSkipped:
        ListOfToBeSkippedModulePaths.append(ListOfDirectoryTuples[0][0] + "/" + ListOfDirectoryTuples[0][1][ListOfDirectoryTuples[0][1].index(ModuleToBeSkipped)])

    for SkippedModulePath in ListOfToBeSkippedModulePaths:
        for d in os.walk(SkippedModulePath):
            ListOfToBeSkippedModuleDirectoryTuples.append(d)

    ListOfToBeSkippedModuleFilePaths = []
    for index in range(len(ListOfToBeSkippedModuleDirectoryTuples)):
        for count in range(len(ListOfToBeSkippedModuleDirectoryTuples[index][2])):
            FileName = str(ListOfToBeSkippedModuleDirectoryTuples[index][2][count])
            FilePath = str(ListOfToBeSkippedModuleDirectoryTuples[index][0]) + "/" + FileName
            if FilePath.endswith('.c') or FilePath.endswith('.h'):
                ListOfToBeSkippedModuleFilePaths.append(FilePath)
                ListOfToBeSkippedFileNames.append(FileName)

##    if DebugScript == '1':
##        DebugFile.write("\nListOfToBeSkippedModuleFilePaths = ")
##        for File in ListOfToBeSkippedModuleFilePaths:
##            DebugFile.write("\n" + File)

####################################################################################
#      List of Files in the Project Directory has been generated here.             #
####################################################################################

####################################################################################
#     Scanning the ConfigurationHeader Directory for Input Dependency Files        #
#     if, Mode = PictorLevel, else scanning the mentioned input dependency file.   #
####################################################################################

ListOfTopDirectoriesInProjectPath = ListOfDirectoryTuples[0][1]

ScanConfigurationHeaderDirectory(ListOfFilePaths)
LookInputDependenciesInModule()

####################################################################################
#      Parsing the Project Directory to Generate the Hash Define List.             #
####################################################################################

GenerateHashDefineList(CompleteListOfFilePaths)

####################################################################################
#      Updating the FirmwareVersion directory with LLA params                      #
####################################################################################

UpdateFirmwareVersionLLA()

####################################################################################
#      Parsing the Project Directory to Generate the Damper List.             #
####################################################################################





##for damper_name in DamperParamsDictionary.keys():
##    print "damper name = " + str(damper_name)
##    temp_dict = DamperParamsDictionary[damper_name]
##    for name in temp_dict.keys():
##        print str(name) + ' = ' + str(temp_dict[name])
##    print "\n"

# Update DAMPER_PLATFORM_SPECIFIC.H File with the appropriate address


####################################################################################
#      Now All the Files have to be Parsed for Searching DataType Definitions.     #
####################################################################################

for File in ListOfFilePaths:

    try:
        InputFile = open(File,"rb")
    except:
        print "File" +  File + "cannot be opened for Reading... \n"
        print "Exiting......."
        sys.exit(2)
    
    ListOfInputs = InputFile.readlines()
    if DebugScript == '1':
        DebugFile.write("\nLooking for Typedef Syntax in File - " + "\n" + str(File))
    LookForTypeDefSyntax(File, ListOfInputs)

GenerateFunctionlist()

if DebugScript == '1':
    DebugFile.write("\nChecking Whether FW MAJOR and FW MINOR are defined or not.")
if "MAJOR" not in FirmwareVersion.keys():
    print "\n FW VERSION MAJOR not Defined."
    ReportFile.write('\n FW VERSION MAJOR not Defined.')
if "MINOR" not in FirmwareVersion.keys():
    print '\n FW VERSION MINOR not Defined.'
    ReportFile.write('\n FW VERSION MINOR not Defined.')

if DebugScript == '1':
    DebugFile.write("\nFirmwareVersion = \n" + str(FirmwareVersion))
####################################################################################
#      Till Now the ListOfStructs is updated and Now the files can be parsed for   #
#      Generating the Page List                                                    #
####################################################################################

print "\nNow Generating PageList\n"
if DebugScript == '1':
    DebugFile.write("\nNow Generating PageList\n")
for File in ListOfFilePaths:
    try:
        InputFile = open(File,"rb")
    except IOError:
        print "File" +  File + "cannot be opened for Reading... \n"
        print "Exiting......."
        sys.exit(2)
    
    ListOfInputs = InputFile.readlines()
    GeneratePageList(File,ListOfInputs)

if DebugScript == '1':
    DebugFile.write('\nDictionaryOfStructs = \n')
    for struct in DictionaryOfStructs.keys():
        temp_dict = DictionaryOfStructs[struct]
        DebugFile.write("\n\n" + str(struct))
        for item in temp_dict.keys():
            DebugFile.write("\n\t" + str(item)  + " : " + str(temp_dict[item]))

    DebugFile.write('\n\nPageList = \n')
    for page in PageList:
        DebugFile.write("\n\t" + str(page))

    DebugFile.write('\n\nPageNameToPageTypeDictionary = \n')
    for page_name in PageNameToPageTypeDictionary:
        DebugFile.write("\n\t" + str(page_name) + " : " + str(PageNameToPageTypeDictionary[page_name]))

    DebugFile.write('\n\nListOfStructToBeAddedInMapFile = \n')
    for item in ListOfStructToBeAddedInMapFile:
        DebugFile.write("\n\t" + item)

    DebugFile.write('\n\nListOfStructToBeAddedInWMMMapFile = \n')
    for item in ListOfStructToBeAddedInWMMMapFile:
        DebugFile.write("\n\t" + item)

    #UpdateHashDefineList()
    DebugFile.write('\n\nHashDefineList = \n')
    for macro in HashDefineList:
        DebugFile.write('\n\t' + str(macro[0]) + ' : ' + str(macro[1]))
    for key in HashDefineDictionary.keys():
        DebugFile.write('\n\t' + str(key) + ' : ' + str(HashDefineDictionary[key]))

#######################################################################################
#####   Generate the HashDefineListFromTypedefEnum                                    #
#######################################################################################
####
####for PageType, NumberOfAttributes, ListOfAttributes, FilePath in ListOfPageTypeNumOfElementsListOfPageElements:
####    if PageType.endswith('_te') == True or PageType.endswith('_e') == True:
####        if FilePath.endswith('.h') == True:
####            pass
####        else:
####            print "FilePath " + FilePath + " should be a .h file\n"
####            print "Exiting... "
####            sys.exit(2)
####
####        for Attribute in ListOfAttributes:
####            HashDefineListFromTypedefEnum.append((Attribute, str(DictionaryOfStructs[PageType][Attribute]), PageType, FilePath))
####
##############################################################################################################
#####   Now Checking whether the Hash defines in the HashListFromTypedefEnum List are present in HashList    #
##############################################################################################################
####
####for attribute, value, enum, file in HashDefineListFromTypedefEnum:
####    EnumName = enum[:enum.find('_te')]
####    if attribute.startswith(EnumName) == True:
#####        AttributeName = attribute
####        print "\nAttribute " + str(attribute) + " from enum -> " + str(enum) + " in file -> \n" + str(file) + "\nstarts with the EnumName."
####        ReportFile.write("\nAttribute " + str(attribute) + " from enum -> " + str(enum) + " in file -> \n" + str(file) + "\n starts with the EnumName.")
####        print "Exiting... "
####        sys.exit(2)
####    else:
####        AttributeName = EnumName + "_" + attribute
####
####    if AttributeName in HashDefineDictionary.keys():
####        if value == HashDefineDictionary[AttributeName]:
####            pass
####        else:
####            ReportFile.write("\nAttribute " + str(AttributeName) + " already defined with value -> " + str(HashDefineDictionary[AttributeName]))
####            ReportFile.write("\nCould not initialize attribute -> " + str(Attribute) + " with its value -> " + str(value) + " from enum -> " + str(enum) + " in file -> \n" + str(file) + "\n")
####            DiscrepancyFound = 1
####    else:
####        HashDefineList.append((AttributeName, value))
####        UpdateFileWithAttributeAndValue(AttributeName, attribute, value, enum, file)

ScanVirtualRegisterListFile()
ScanVirtualRegisterListFile_WMM()
CheckAlignmentConstraintOfAllStructs()
GenerateFirmwareVariableSerialIndicesAndTypesByNameDictionary(0, VirtualRegisterPageList)
GenerateFirmwareVariableSerialIndicesAndTypesByNameDictionary(32768, VirtualRegisterPageList_WMM)
GenerateEnumAndEnumFlagDictionary()

if DebugScript == '1':
    DebugFile.write("\n\nFirmwareVariableTypesByName = ")
    for page_name in FirmwareVariableTypesByName.keys():
        temp_dict = FirmwareVariableTypesByName[page_name]
        DebugFile.write("\n\n" + str(page_name))
        for item in temp_dict.keys():
            DebugFile.write("\n\t" + str(item)  + " : " + str(temp_dict[item]))


######################################################################################
#   Now the DeviceCommsLayer is being written with its contents                      #
######################################################################################

OutputFileName = "DeviceCommsLayer_8500.py"
if sys.argv[4]=="None":
    OutputFilePath = DirPathName + "//" + OutputFileName
else:
    OutputFilePath = sys.argv[4] + "//" + OutputFileName

OutputFile = open( OutputFilePath, "wb" )

WriteHeaderToDeviceCommsLayerFile(OutputFileName,OutputFile)
WriteFlagIndexByStringDictsByTypeDictionary(OutputFile)
WriteEnumIntegerValueByStringDictionariesByTypeDictionary(OutputFile)

OutputFile.write("\n")
OutputFile.write("		# and fill out the 2-dimensional dictionary that defines the\n")
OutputFile.write("		# mapping between the names of firmware 'page.element' variables\n")
OutputFile.write("		# and their types...\n")
OutputFile.write("		#\n")
OutputFile.write("\n")


for item in VirtualRegisterPageList:
    AllVirtualRegisterPageList.append(item)
for item in VirtualRegisterPageList_WMM:
    AllVirtualRegisterPageList.append(item)

for item in VirtualRegisterPageInfoList:
    AllVirtualRegisterPageInfoList.append(item)
for item in VirtualRegisterPageInfoList_WMM:
    AllVirtualRegisterPageInfoList.append(item)

WriteFirmwareVariableTypesByNameDictionary(OutputFile)

OutputFile.write("\n")
OutputFile.write("		# then fill out the 2-dimensional dictionary that defines the\n")
OutputFile.write("		# mapping between the names of firmware 'page.element' variables\n")
OutputFile.write("		# and their i2c indices (individual indices or pairs of indices\n")
OutputFile.write("		# for 16-bit variables) - where there is more than one index\n")
OutputFile.write("		# they are listed in order of the increasing significance of the\n")
OutputFile.write("		# individual bytes (this is important as this order will be\n")
OutputFile.write("		# assumed later on when these variables are read or written to)...\n")
OutputFile.write("		#\n")
OutputFile.write("\n")

WriteFirmwareVariableSerialIndicesByNameDictionary(OutputFile)

OutputFile.write("\n")
OutputFile.write("		# we now call a method of our base class that generates inverse\n")
OutputFile.write("		# dictionaries...\n")
OutputFile.write("		#\n")
OutputFile.write("		self.generateInverseDictionaries()\n")
OutputFile.write("\n")

OutputFile.write("		# and finally define some expected features of the device\n")
OutputFile.write("		# under test that this 'device comms layer' is intended to be\n")
OutputFile.write("		# used with...\n")
OutputFile.write("		#\n")

OutputFile.write("		self.iExpectedDeviceId = 8500\n")
OutputFile.write("		self.iExpectedFirmwareVersionMajor = " + str(FirmwareVersion['MAJOR']) + "\n")
OutputFile.write("		self.iExpectedFirmwareVersionMinor = " + str(FirmwareVersion['MINOR']) + "\n")
OutputFile.write("\n")

OutputFile.write("\n")
for index in range(len(HashDefineList)):
    if HashDefineList[index][0].find('(') == -1:
        OutputFile.write("		self." + str(HashDefineList[index][0]) + "   =    "  + str(HashDefineList[index][1]) + "\n")
OutputFile.write("\n")

##OutputFile.write("		self.commsType = \"" + str(HashDefineDictionary['commsType']) + "\"\n")

##OutputFile.write("\n")
##OutputFile.write("		# information about the device endianess\n")
##OutputFile.write("		self.Endian = \"" + str(HashDefineDictionary['Endian']) + "\"\n")
##OutputFile.write("\n")

###   Now the DeviceParametersList
##OutputFile.write("\n")
##for index in range(len(DeviceParametersList)):
##    OutputFile.write("		self." + str(DeviceParametersList[index]) + "   =    \""  + str(HashDefineDictionary[DeviceParametersList[index]]) + "\"\n")
##OutputFile.write("\n")    


#####################################################################################
#   Now Generating the Parameters File used by the PostBuild.py Script File         #
#####################################################################################

#ParametersFile = open("./ParametersFile.txt","wb")
#ParametersFile.write("DirPathName = " + DirPathName + "\n")
#ParametersFile.write("ModuleName = " + ModuleName + "\n")
#ParametersFile.write("DeviceCommsLayerFilePath = " + OutputFilePath + "\n")
#ParametersFile.write("ReportFilePath = " + ReportFilePath + "\n")
#ParametersFile.close()


####################################################################################
#      Now the Map File Generation.                                                #
####################################################################################
######
#      For Map File generation is required VirtualRegisterPageList & the PageNameToPageTypeDictionary & ListOfPageTypeNumOfElementsListOfPageElements
#      DictionaryOfStructs & FirmwareVariableSerialIndicesByName Dictionary
######

GenerateMapFile("V2WReg.map", AllVirtualRegisterPageList, AllVirtualRegisterPageInfoList)
##GenerateMapFile("V2WReg_WMM.map", VirtualRegisterPageList_WMM, VirtualRegisterPageInfoList_WMM)



if DiscrepancyFound == 0:
    ReportFile.write("\nNo Discrepancies Found by 8500DocumentParameters.py Script File.\n\n\n")
else:
    ReportFile.write("\nThe above listed discrepancies were found by 8500DocumentParameters.py Script File in the Project.\n")

GenerateVhcElementsFile("baseline.h", VirtualRegisterPageList, DictOfEnumsInVirtualRegister, ListOfStructToBeAddedInMapFile)
GenerateVhcElementsFile("extension.h", VirtualRegisterPageList_WMM, DictOfEnumsInVirtualRegister_WMM, ListOfStructToBeAddedInWMMMapFile)
GenerateSensorIdFile("SensorId.h")
#GenerateTestFile("TestFile.h", VirtualRegisterPageList, DictOfEnumsInVirtualRegister, ListOfStructToBeAddedInMapFile)
