import string
import xml.dom.minidom
import os
import os.path
import sys

# Extract text data from a node

def getText(nodelist):
    rc = ""
    for node in nodelist:
        if node.nodeType == node.TEXT_NODE:
            rc = rc + node.data
    return rc

# Get the memory maps from the spirit xml file and transfer control to another function for processing of address blocks

def handleMemoryMaps(component):
    MemoryMaps = component.getElementsByTagName("spirit:memoryMaps")[0]
    MemoryMap = MemoryMaps.getElementsByTagName("spirit:memoryMap")[0]
    AddressBlocks = MemoryMap.getElementsByTagName("spirit:addressBlock")
    handleAddressBlocks(AddressBlocks)

# Processing required for address blocks or IPs and transfer control to another function for processing of registers

def handleAddressBlocks(AddressBlocks):

    global IP_Name
    wrapper_extern = open(os.path.join(destination_path, ("temp_" + file_name + '.c')),'w')
    for AddressBlock in AddressBlocks:
        IP_Name = AddressBlock.getAttribute("spirit:name")
        AddressBlock_baseAddress = AddressBlock.getElementsByTagName("spirit:baseAddress")[0]
        IP_baseAddress = getText(AddressBlock_baseAddress.childNodes)
        AddressBlock_range = AddressBlock.getElementsByTagName("spirit:range")[0]
        IP_range = getText(AddressBlock_range.childNodes)
        Registers = AddressBlock.getElementsByTagName("spirit:register")
        print "Processing: ", IP_Name
        wrapper_map.write('\n    Group "'+IP_Name+'"\n')
        PreProcessFile()
        handleRegisters(Registers)
        wrapper_extern.write('extern volatile ' + IP_Name + '_IP_ts ' + '*p_' + IP_Name + '_IP;\n')
        wrapper_reg.write('volatile '+IP_Name + '_IP_ts ' + '*p_' + IP_Name + '_IP = (' + IP_Name + '_IP_ts ' + '*)(' + 'BaseAddress' + ' + ' + IP_baseAddress + ');    //' + IP_Name + '\n\n')

        PostProcessFile()
        wrapper_map.write('\n    Group End\n\n')

    wrapper_extern = open(os.path.join(destination_path, ("temp_" + file_name + '.c')))
    wrapper.write('\n\n')
    for line in wrapper_extern:
        wrapper.write(line)
    wrapper_extern.close()
    os.remove(os.path.join(destination_path, ("temp_" + file_name + '.c')))

# Processing required for Registers and transfer control to another function for processing of register fields

def handleRegisters(Registers):
    global struct_list
    global BypassHeaders
    register_list = []
    current = 0
    next = current+1
    for Register in Registers:

        register_name = Register.getElementsByTagName("spirit:name")[0]
        struct_name = getText(register_name.childNodes)

        register_addressOffset = Register.getElementsByTagName("spirit:addressOffset")[0]
        struct_addressOffset = int(getText(register_addressOffset.childNodes),16)

        register_access = Register.getElementsByTagName("spirit:access")[0]
        register_size = Register.getElementsByTagName("spirit:size")[0]
        struct_size = getText(register_size.childNodes)
        Register_Fields = Register.getElementsByTagName("spirit:field")
        register_parameter = Register.getElementsByTagName("spirit:parameter")
        register_dim = Register.getElementsByTagName("spirit:dim")
        register_dim_size = None
        if(register_dim):
            BypassHeaders = 1
            register_dim_size = register_dim[0].childNodes[0].data
        else:
            BypassHeaders = 0

        register_list.append([struct_name, struct_addressOffset, register_dim_size])
        wrapper.write('\n\n//' + struct_name + '\n')
        addressOffset = int(Register.parentNode.getElementsByTagName("spirit:baseAddress")[0].childNodes[0].data,16) + struct_addressOffset
        if (not BypassHeaders):
            if(str(int(struct_size)/8) == 1):
                wrapper_map.write('\n\n        Item "'+struct_name+'"    Index '+ hex(addressOffset+int(BaseAddress,16)) + ' NumBytes ' + '\n')
            else:
                wrapper_map.write('\n\n        Item "'+struct_name+'"    Index '+ hex(addressOffset+int(BaseAddress,16)) + ' NumBytes ' + str(int(struct_size)/8) + '\n')

        try:
            size = int(Registers[next].getElementsByTagName("spirit:addressOffset")[0].childNodes[0].data,16) - int(Registers[current].getElementsByTagName("spirit:addressOffset")[0].childNodes[0].data,16)
            size*=8
            Data_Type = getDataType(size)
            current+=1
            next=current+1
        except IndexError:
            size = int(struct_size)
            Data_Type = getDataType(struct_size)

        if( struct_name in struct_list):
            wrapper.write('//The structure corresponding to this register has been already defined earlier.\n')
            wrapper_define.write('\n\n\n//' + struct_name)
            if(Register_Fields):
                handleFields(Register_Fields, struct_name,Data_Type)
            continue

        for parameter in register_parameter:
            if(parameter.getAttribute("spirit:name") == "longDescription"):
                register_description = parameter.childNodes[0].data
                wrapper.write("/*Description: " + register_description.strip() + "\n*/\n")

        startUnion()
        wrapper.write('{\n')

        register_reset = None
        if(Register.getElementsByTagName("spirit:reset")):
            register_reset = Register.getElementsByTagName("spirit:reset")[0]
            register_revalue = register_reset.getElementsByTagName("spirit:value")[0]
##            print getText(register_reset_value.childNodes)
        elif(Register.getElementsByTagName("spirit:resetValue")):
            register_reset = Register.getElementsByTagName("spirit:resetValue")[0]
##            print getText(register_reset.childNodes)

        insertElements('word',str(size))
        insertElements('data', struct_size)

        wrapper_define.write('\n\n\n//' + struct_name)
        if(Register_Fields):
            wrapper.write('    ')
            startStruct()
            wrapper.write('    {\n')

            if(BypassHeaders):
                wrapper_define.write(' : Ignoring header desciptors')


            handleFields(Register_Fields, struct_name, Data_Type)

            wrapper.write('    }')
            endStruct(struct_name + '_ts')
            
        handleRegisterLevelMacros(Register, struct_name, Data_Type)
            
        wrapper.write('}')
        endUnion(struct_name + '_tu')
        if(not( struct_name in struct_list)):
            struct_list.append(struct_name)


    wrapper.write('typedef ')
    startStruct()
    wrapper.write('{\n')

    previous_offset = 0
    current_offset = 0
    for member in register_list:
        Array_Size = '0'
        current_offset = member[1]
##        if('GRIDIRON_MEM_ELT_CAST' in member[0]):
##            print current_offset," ", previous_offset
        NumberOfPaddings = (current_offset-previous_offset)/4
        if(member[2]):
            Array_Size = member[2]
            NumberOfPaddings = NumberOfPaddings - int(Array_Size)
        else:
            NumberOfPaddings = NumberOfPaddings - 1
        if(NumberOfPaddings>1):
            insertElements('pad_' + member[0], None,getDataType(32),str(NumberOfPaddings))
        elif(NumberOfPaddings==1):
            insertElements('pad_' + member[0], None,getDataType(32))

        if(member[2]):
            insertElements(member[0], None, member[0]+'_tu',Array_Size)
        else:
            insertElements(member[0], None, member[0]+'_tu')

        previous_offset = current_offset

    wrapper.write('}')
    endStruct(IP_Name + '_IP_ts')

# Processing required for Register Fields and transfer control to another function for processing of register field values

def handleFields(Register_Fields, struct_name, Data_Type):
    previous_offset = 0
    current_offset = 0
    previous_bitWidth = 0
    reserve_flag = 0
    count = 0
 
    arg_name = []
    arg_expression = []
##    mask = 0

    for Field in Register_Fields:

        field_name = Field.getElementsByTagName("spirit:name")[0]
        elementName = getText(field_name.childNodes)
##        print elementName
        field_bitOffset = Field.getElementsByTagName("spirit:bitOffset")[0]
        bitOffset = getText(field_bitOffset.childNodes)
        current_offset = int(bitOffset)
##        print current_offset
        field_bitWidth = Field.getElementsByTagName("spirit:bitWidth")[0]
        bitWidth = getText(field_bitWidth.childNodes)
##        print bitWidth
        field_access = Field.getElementsByTagName("spirit:access")[0]
##        print getText(field_access.childNodes)
        Field_Values = Field.getElementsByTagName("spirit:values")
        reserved_flag = current_offset - previous_offset - previous_bitWidth

        if( not ( struct_name in struct_list)):
            if(reserved_flag):
                wrapper.write('    ')
                insertElements(('reserved' + str(count)), str(reserved_flag))
                count+=1
            wrapper.write('    ')
            insertElements(elementName, bitWidth)
            Field_description = Field.getElementsByTagName("spirit:description")
            if(Field_description):
                wrapper.write('        /* ' + '\n        '.join(Field_description[0].childNodes[0].data.split('\n\n')) + '*/\n')

            wrapper_define.write('\n\n')

        if((getText(field_access.childNodes) == 'read-write') or (getText(field_access.childNodes) == 'read-only')):
            macro_name_get = 'Get_' + IP_Name + '_' + struct_name + '_' + elementName + '()'
            macro_expression_get = 'p_' + IP_Name + '_IP' + '->' + struct_name + '.' + struct_name + '_ts.'+ elementName
            define_macro(macro_name_get, macro_expression_get)

        if((getText(field_access.childNodes) == 'read-write') or (getText(field_access.childNodes) == 'write-only')):
            arg_name.append(elementName)
            arg_expression.append('(' + Data_Type + ')' + elementName + '<<' + str(current_offset))
##            mask |= (pow(2,int(bitWidth))-1)<<current_offset
            if( not Field_Values):

                macro_name_set = 'Set_' + IP_Name + '_' + struct_name + '_' + elementName + '(x)'
                macro_expression_set = '(p_' + IP_Name + '_IP' + '->' + struct_name + '.' + struct_name + '_ts.'+ elementName + ' = x'  + ')'
                define_macro(macro_name_set, macro_expression_set)

##            else:
##                handleValues(Field_Values, elementName, struct_name)
                
        if(Field_Values):
            handleValues(Field_Values, elementName, struct_name)
        previous_offset = current_offset
        addressOffset = hex(int(Field.parentNode.parentNode.getElementsByTagName("spirit:baseAddress")[0].childNodes[0].data,16)+ int(Field.parentNode.getElementsByTagName("spirit:addressOffset")[0].childNodes[0].data,16))
        previous_bitWidth = int(bitWidth)

        if(not BypassHeaders):
            if(int(bitWidth)>1):
                getLimits(int(bitOffset),int(bitWidth),struct_name,elementName,addressOffset)
            else:
                wrapper_map.write('\n            Parent "'+struct_name+'" Item "'+elementName+'" ('+bitOffset+')')
            if(not Field_Values):
                pass
            else:
                wrapper_map.write('    Lut "'+struct_name+'_'+elementName+'_lut"')
                
    if((Field.parentNode.getElementsByTagName("spirit:access")[0].childNodes[0].data == "read-write") or (Field.parentNode.getElementsByTagName("spirit:access")[0].childNodes[0].data == "write-only")):
        macro_name = 'Set_' + IP_Name + '_' + struct_name + '(' + ','.join(arg_name) + ')'
        macro_expression = '(p_' + IP_Name + '_IP' + '->' + struct_name + '.' + 'word'  + ' = ' + ' | '.join(arg_expression) + ')'
        define_macro(macro_name, macro_expression)


# Processing required for Register Field Values

def handleValues(Field_Values, field_name, struct_name):
    global temp_mapString
    temp_mapString+=('    ['+struct_name+'_'+field_name+'_lut]\n    entries = '+str(len(Field_Values))+'\n')
    count = 0
    field_access = Field_Values[0].parentNode.getElementsByTagName("spirit:access")[0]
    for Value in Field_Values:

        value_name = Value.getElementsByTagName("spirit:name")[0]
        value_value = Value.getElementsByTagName("spirit:value")[0]
        macro_value = getText(value_value.childNodes)
        value_description = Value.getElementsByTagName("spirit:description")[0]
##        print getText(value_description.childNodes)

        if((getText(field_access.childNodes) == 'read-write') or (getText(field_access.childNodes) == 'read-only')):
            macro_name_compare = 'Is_' + IP_Name + '_' + struct_name + '_' + field_name + '_' + getText(value_name.childNodes) +'()'
            macro_expression_compare = '(p_' + IP_Name + '_IP' + '->' + struct_name + '.' + struct_name + '_ts.'+ field_name + ' == ' + field_name + '_' + getText(value_name.childNodes) + ')'
            define_macro(macro_name_compare, macro_expression_compare)

        if((getText(field_access.childNodes) == 'read-write') or (getText(field_access.childNodes) == 'write-only')):
            macro_name_set = 'Set_' + IP_Name + '_' + struct_name + '_' + field_name + '__' + getText(value_name.childNodes) +'()'
            macro_expression_set = '(p_' + IP_Name + '_IP' + '->' + struct_name + '.' + struct_name + '_ts.'+ field_name + ' = ' + field_name + '_' + getText(value_name.childNodes) + ')'
            define_macro(macro_name_set, macro_expression_set)

        if(not (struct_name in struct_list)):
            macro_name = field_name + '_' + getText(value_name.childNodes)
            define_macro(macro_name, macro_value)
            wrapper_define.write('    //' + getText(value_description.childNodes))

        temp_mapString+=('    attr'+str(count)+' = "'+getText(value_value.childNodes)+' : '+getText(value_name.childNodes)+'"\n'+'    val'+str(count)+' = '+getText(value_value.childNodes)+'\n')
        count+=1
    temp_mapString+=('    End\n\n')


def handleRegisterLevelMacros(Register, struct_name, Data_Type):
    
    arg_name = []
    arg_expression = []
    
    if((Register.getElementsByTagName("spirit:access")[0].childNodes[0].data == "read-write") or (Register.getElementsByTagName("spirit:access")[0].childNodes[0].data == "read-only")):
        macro_name = 'Get_' + IP_Name + '_' + struct_name + '(' + ')'
        macro_expression = 'p_' + IP_Name + '_IP' + '->' + struct_name + '.' + 'word'
        define_macro(macro_name, macro_expression)

    if((Register.getElementsByTagName("spirit:access")[0].childNodes[0].data == "read-write") or (Register.getElementsByTagName("spirit:access")[0].childNodes[0].data == "write-only")):
        macro_name = 'Set_' + IP_Name + '_' + struct_name + '_word' + '(x)'
        macro_expression = '(p_' + IP_Name + '_IP' + '->' + struct_name + '.' + 'word'  + ' = x)' #+ hex(mask) + '& x)'
        define_macro(macro_name, macro_expression)



def startUnion():
    wrapper.write('typedef union\n')

def endUnion(struct_name):
    wrapper.write(struct_name +';\n\n')

def startStruct():
    wrapper.write('struct\n')

def endStruct(struct_name):
    wrapper.write(struct_name +';\n\n')

def insertElements(elementName, bitWidth='0',Data_Type = '0', Array_Size = '0'):

    if(Array_Size != '0'):
        elementName = elementName + '[' + Array_Size + ']'
    if(Data_Type != '0'):
        wrapper.write('    ' + Data_Type + ' ' + elementName + ';\n')
    else:
        if(bitWidth == '8' or bitWidth == '16' or bitWidth == '32'):
            wrapper.write('    ' + str(getDataType(bitWidth)) + ' ' + elementName + ';\n')
        elif(int(bitWidth)>32):
            wrapper.write('    ' + str(getDataType('32')) + ' ' + elementName + ';\n')
        else:
            wrapper.write('    ' + str(getDataType(bitWidth)) + ' ' + elementName + ' :' + bitWidth + ';\n')

def define_macro(name, expression):
    global BypassHeaders
    if(not BypassHeaders):
        wrapper_define.write('\n#define ' + name + ' ' + expression)



def getDataType(size):

    size = int(size)
    if(size <= 8):
        return Data_Type8
    elif(size > 8 and size <=16):
        return Data_Type16
    elif(size > 16 and size <= 32):
        return Data_Type32
    else:
        return Data_Type32

def PreProcessFile():
    global wrapper_define
    wrapper_define = open(os.path.join(destination_path, (IP_Name + '.h')),'w')


def PostProcessFile():
    global wrapper_define
    wrapper_define.close()

    wrapper_define = open(os.path.join(destination_path, (IP_Name + '.h')))
    for line in wrapper_define:
        wrapper.write(line)
    wrapper_define.close()
    os.remove(os.path.join(destination_path, (IP_Name + '.h')))


# This function generates the entries in the hardware map file for v2wreg

def getLimits(bitOffset,bitWidth,struct_name,elementName,addressOffset):
    loLimit = bitOffset
    hiLimit = bitOffset+bitWidth-1

    wrapper_map.write('\n            Parent "'+struct_name+'" Item "'+elementName+'" ('+str(hiLimit)+':'+str(loLimit)+')')


dom = None
IP_Name = None
struct_list = []

# Get the arguments from command line

source_path = sys.argv[1]              #raw_input("Enter the source Path: ")
destination_path = sys.argv[2]         #raw_input("Enter the destination Path: ")
platform = sys.argv[3]                 #raw_input("Enter the platform file name: ")
Data_Type8 = sys.argv[4]               #raw_input("Enter the convention used for 8 bit data type: ")
Data_Type16 = sys.argv[5]              #raw_input("Enter the convention used for 16 bit data type: ")
Data_Type32 = sys.argv[6]              #raw_input("Enter the convention used for 32 bit data type: ")
BaseAddress = sys.argv[7]              #raw_input("Enter the base address of IP: ")
DeviceAddress = sys.argv[8]            #Device Address
CommsType = sys.argv[9]                #Comms Type
EndianType = sys.argv[10]              #BigEndian or LittleEndian
FileNamePrefix = sys.argv[11]

f = open(source_path)
dom = xml.dom.minidom.parse(f)          # Parsing the spirit xml file using minidom
f.close()

file_name = dom.getElementsByTagName("spirit:memoryMap")[0].getElementsByTagName("spirit:name")[0].childNodes[0].data

# Create the files which are needed to be generated automatically

wrapper = open(os.path.join(destination_path, (FileNamePrefix + 'hwReg' + '.h')),'w')
wrapper_reg = open(os.path.join(destination_path, (FileNamePrefix + 'hwReg' + '.c')),'w')
wrapper_map = open(os.path.join(destination_path, (FileNamePrefix + '_memorymap.map')),'w')
wrapper.write('#ifndef _' + FileNamePrefix + 'hwReg' + '_\n')
wrapper.write('#define _' + FileNamePrefix + 'hwReg' + '_\n\n')
wrapper_reg.write('#define ' + 'BaseAddress ' + BaseAddress + '\n\n')
wrapper.write('#include "' + platform + '"\n\n')
wrapper_reg.write('#include "' + FileNamePrefix + 'hwReg' + '.h'+ '"\n\n')
wrapper_map.write('$MAP\n$COMMS_TYPE "'+CommsType+'"\nHex\nChildren\n')
if(EndianType == 'LittleEndian'):
    wrapper_map.write('ToggleEndian\n')
wrapper_map.write('Device "'+FileNamePrefix+'" Address '+DeviceAddress+'\n')
wrapper_define = None
temp_mapString = ''

handleMemoryMaps(dom)                   # Start traversing nodes in the parse tree

wrapper_map.write('\nEnd\n')
wrapper_map.write(temp_mapString)
wrapper.write("\n#endif \n")
wrapper.write("\n")
