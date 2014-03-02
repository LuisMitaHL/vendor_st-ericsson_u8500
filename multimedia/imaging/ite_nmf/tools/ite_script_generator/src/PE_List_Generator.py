#***************************************
#file:PE_List_Generator.py
#date:20-Apr-11
#location:C:\Sagar\8.PE_list\src
#author: SAGAR GUPTA M.
#***************************************


import sys
import string
import os


TRACE_PATH = "../Input"
PE_DEF_PATH = "../inc"
OUTPUT_PATH = "../Output"


def Parse_File_for_PEAddress (sPath, sPEName):
    sPEAddress = ""
    
    nFileFD = open (sPath, 'r')
    sFileData = nFileFD.read ()
    nFileFD.close ()

    nPos = sFileData.find (sPEName)
    if nPos >= 0:
        sFileData = sFileData[ nPos: ]

        nPos = sFileData.find ("0x")        
        if (nPos >= 0):
            sPEAddress = sFileData [ nPos: ]

            nPos = sPEAddress.find ("\n")            
            if (nPos >= 0):
                sPEAddress = sPEAddress [ :nPos ]

            nPos = sPEAddress.find (" ")                
            if (nPos >= 0):
                sPEAddress = sPEAddress [ :nPos ]
            sPEAddress = sPEAddress.strip ()
        
    return sPEAddress


def Get_PEAddress_From_PEName (sPEName, sPEDefPath):
    sDirList = os.listdir (sPEDefPath)
    sDirList.sort()

    for sFileName in sDirList:
        sPath = sPEDefPath + "/" + sFileName
        if os.path.isfile(sPath):
            sPEAddress = Parse_File_for_PEAddress (sPath, sPEName)
            if (sPEAddress != ""):
                break
        else:
            continue
    
    return sPEAddress


def Parse_File_for_PEName (sPath, sPEAddress):
    sPEName = ""
    
    nFileFD = open (sPath, 'r')
    sFileData = nFileFD.read ()
    nFileFD.close ()

    nPos = sFileData.find (sPEAddress)
    if nPos >= 0:
        sPEName = sFileData [ :nPos-1 ]

        nPos = sPEName.rfind ("#define")
        if (nPos >= 0):        
            sPEName = sPEName [ nPos+len ("#define"): ]
            sPEName = sPEName.strip ()
 
    return sPEName


def Get_PEName_From_PEAddress (sPEAddress, sPEDefPath):
    sDirList = os.listdir (sPEDefPath)
    sDirList.sort()

    for sFileName in sDirList:
        sPath = sPEDefPath + "/" + sFileName
        if os.path.isfile(sPath):
            sPEName = Parse_File_for_PEName (sPath, sPEAddress)
            if (sPEName != ""):
                break
        else:
            continue
    
    return sPEName


#Isolate the Address, Value of Write Element and get its name and viceversa form the include(.h) file
def Get_Write_Element_Details (sLine, sSearchElement):
    sPEName = ""
    sAddress = ""
    sValue = ""
    
    nPos = sLine.find (sSearchElement)
    if (nPos >= 0):
        sLine = sLine [ nPos: ]
    
        nCount = sLine.count ("0x")
        if (nCount == 1):
            if (sLine.find (":") >= 0):
                sPEName = sLine [ sLine.find (":"): ]              
                if (sPEName.find (" ") >= 0):
                    sPEName = sPEName [ sPEName.find (" ")+1: ]
                    if (sPEName.find (" ") >= 0):
                        sPEName = sPEName [ :sPEName.find (" ") ]
                        sPEName = sPEName.strip ()
                        sAddress = Get_PEAddress_From_PEName (sPEName, PE_DEF_PATH)
            if (sLine.find ("0x") >= 0):                        
                sValue = sLine [ sLine.find ("0x"): ]
                if (sValue.find (" ") >= 0):                
                    sValue = sValue [ :sValue.find (" ") ]
                    sValue = sValue.strip ()
                    sValue = sValue.rstrip ("n")
                    sValue = sValue.rstrip ("\\")
        elif (nCount == 2):
            if (sLine.find ("0x") >= 0):
                sAddress = sLine [ sLine.find ("0x"): ]
                if (sAddress.find (" ") >= 0):
                    sAddress = sAddress [ :sAddress.find (" ") ]
                if (sAddress.find ("(") >= 0):
                    sAddress = sAddress [ :sAddress.find ("(") ]
                sAddress = sAddress + " "
                sAddress = sAddress.strip ()
                
                sPEName = Get_PEName_From_PEAddress (sAddress, PE_DEF_PATH)

                if (sLine.find("0x") >= 0):                
                    sValue = sLine [ sLine.find ("0x")+len ("0x"): ]
                    if (sValue.find ("0x") >= 0):
                        sValue = sValue [ sValue.find ("0x"): ]
                        if (sValue.find (" ") >= 0):
                            sValue = sValue [ :sValue.find (" ") ]
                        sValue = sValue.strip ()
                        sValue = sValue.rstrip ("\n")
                        sValue = sValue.rstrip ("n")
                        sValue = sValue.rstrip ("\\")

    return "#Write PE<" + sPEName + ":" + sAddress + "> with value:" + sValue + "\n" + "writePE " + sAddress + " " + sValue + "\n\n"


#Isolate the Address of Read Element and get its name form the include(.h) file
def Get_Read_Element_Details (sLine, sSearchElement):
    sPEName = ""
    sPEAddress = ""
    
    if (sLine.find (sSearchElement) >= 0):
        sPEAddress = sLine [ sLine.find (sSearchElement): ]
        if (sPEAddress.find ("0x") >= 0):
            sPEAddress = sPEAddress [ sPEAddress.find ("0x"): ]
            if (sPEAddress.find (" ") >= 0):
                sPEAddress = sPEAddress [ :sPEAddress.find (" ") ]
            sPEAddress = sPEAddress.strip ()
            sPEAddress = sPEAddress.rstrip ('\n')
            
            sPEName = Get_PEName_From_PEAddress (sPEAddress, PE_DEF_PATH)

    return "#Read PE<" + sPEName + ":" + sPEAddress + ">\n" + "readPE " + sPEAddress + "\n\n"


def Open_Output_File (OUTPUT_PATH, sIPFileName):
    sOPFileName = sIPFileName[:len(sIPFileName)-4] + ".ite"

    sPath = OUTPUT_PATH + '/' + sOPFileName
    nFileFD = open (sPath, 'w')
    
    return nFileFD


#Search for the PE Addr & Values in the each file.
def Parse_File_for_Search_Elements (sFilePath, sIPFileName):
    nFileFD = open (sFilePath, 'r')
    nOutputFileFD = Open_Output_File (OUTPUT_PATH, sIPFileName)
    
    while 1:
        sFileLine = nFileFD.readline()
        if not sFileLine:
            break
        
        if ((sFileLine.find ("PE Read") >= 0)):
            nOutputFileFD.write (Get_Read_Element_Details (sFileLine, "PE Read"))
        elif (sFileLine.find ("PE written") >= 0):
            nOutputFileFD.write (Get_Write_Element_Details (sFileLine, "PE written"))
        elif (sFileLine.find ("Write PE") >= 0):
            nOutputFileFD.write (Get_Write_Element_Details (sFileLine, "Write PE"))

    nOutputFileFD.close ()  
    nFileFD.close ()


#Search for the PE Addr & Values in the Trace Files from the Trace directory.
def Scan_Directory_for_Search_Elements (sDirPath):
    sDirList = os.listdir (sDirPath)
    sDirList.sort ()

    for sFileName in sDirList:
        sPath = sDirPath + "/" + sFileName
        if os.path.isfile (sPath):
            Parse_File_for_Search_Elements (sPath, sFileName)
        else:
            continue


#Search for the PE Addr & Values in the Trace Files.
if __name__ == "__main__":
    Scan_Directory_for_Search_Elements (TRACE_PATH)