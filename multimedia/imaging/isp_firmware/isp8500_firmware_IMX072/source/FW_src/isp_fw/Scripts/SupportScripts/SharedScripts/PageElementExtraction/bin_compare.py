import os
import sys
import binhex
import binascii

Offset_Opcode_Dict = {}
Opcode_String_IPM = ''
Opcode_String_EXT = ''
BinFile_String_IPM = ''
BinFile_String_EXT = ''

input_file = open(sys.argv[1] + '/PictorProject_Code.s', 'rb')
list_of_lines = []
list_of_lines = input_file.readlines()

for line in list_of_lines:
    list = line.split()
    if len(list) > 2 and list[0].find(':') != -1:
        var = list[1].lstrip('0x')
##        print var
##        print len(var)
        if len(var) < 8:
            count_less_8 = 8 - len(var)
            for index in range(count_less_8):
                var = '0' + var
##                print var
        if int(list[0].strip(':'), 16) < int('10000000', 16):
            Opcode_String_IPM = Opcode_String_IPM +  var
        else:
            Opcode_String_EXT = Opcode_String_EXT +  var
##        os.system('pause')
##print Opcode_String_IPM

##print Opcode_String_EXT

input_file.close()

##input_file = open('./PictorProject_IPM_Code.bin', 'rb')
##output_file = open('./output.txt', 'wb')
##list_of_lines = []
##list_of_lines = input_file.readlines()
##count = 10
##for line in list_of_lines:
##    if count > 0:
##        count = count - 1
####        output_file.write(binascii.a2b_qp(line))
##        for s in line[0:100]:
##            var = binascii.a2b_hex(line[0:2])
##            output_file.write(var)

##binhex.binhex('./PictorProject_IPM_Code.bin', './output.txt')

input_file = open(sys.argv[1] + '/Isp8500_primary_fw_split.bin', 'rb')
list_of_lines = []
list_of_lines = input_file.readlines()
count = 0
for line in list_of_lines:
    for s in line:
##        if count < 8:
        var = hex(ord(s)).lstrip('0x')
        var = str(var).lstrip('0x')
##        print var
##        print s
        if len(var) == 0:
            var = '00'
        elif len(var) == 1:
            var = '0' + var
        BinFile_String_IPM = BinFile_String_IPM + var
##        print BinFile_String_IPM
##        count = count + 1

input_file.close()

##binhex.binhex('./PictorProject_EXTM_Code.bin', './input.txt')

input_file = open(sys.argv[1] + '/Isp8500_primary_fw_ext.bin', 'rb')
list_of_lines = []
list_of_lines = input_file.readlines()
for line in list_of_lines:
    for s in line:
        var = hex(ord(s)).lstrip('0x')
        var = str(var).lstrip('0x')
        if len(var) == 0:
            var = '00'
        elif len(var) == 1:
            var = '0' + var

        BinFile_String_EXT = BinFile_String_EXT + var

input_file.close()

new_string = ''
count_IPM = len(BinFile_String_IPM)
for index in range(count_IPM/4):
    string = BinFile_String_IPM[index*8: index*8+8]
    new_string = new_string + string[6:8] + string[4:6] + string[2:4] + string[0:2]

BinFile_String_IPM = new_string

new_string = ''
count_EXT = len(BinFile_String_EXT)
for index in range(count_EXT/4):
    string = BinFile_String_EXT[index*8: index*8+8]
    new_string = new_string + string[6:8] + string[4:6] + string[2:4] + string[0:2]
BinFile_String_EXT = new_string

##count = len(Opcode_String_IPM)/80
##for index in range(count):
##    print "IPM Code:"
##    print Opcode_String_IPM[80*index:80*(index + 1)]
##    print BinFile_String_IPM[80*index:80*(index + 1)]
####    os.system('pause')

##print "EXT Code:"
##print Opcode_String_EXT
##print BinFile_String_EXT
##
##if Opcode_String_IPM == BinFile_String_IPM:
##    print "Opcode_String_IPM match"
##else:
##    print "Opcode_String_IPM do not match"
##if Opcode_String_EXT == BinFile_String_EXT:
##    print "BinFile_String_EXT match"
##else:
##    print "Opcode_String_EXT do not match"

len_IPM_bin = len(BinFile_String_IPM)
len_IPM_op = len(Opcode_String_IPM)

print "len_IPM_bin = " + str(len_IPM_bin)
print "len_IPM_op = " + str(len_IPM_op)

len_EXT_bin = len(BinFile_String_EXT)
len_EXT_op = len(Opcode_String_EXT)

print "len_EXT_bin = " + str(len_EXT_bin)
print "len_EXT_op = " + str(len_EXT_op)


if BinFile_String_IPM[:len(Opcode_String_IPM)] == Opcode_String_IPM:
    print "The IPM stream in bin file and assembler file match."
else:
    print "The IPM stream in bin file and assembler file do not match."

if BinFile_String_EXT[:len(Opcode_String_EXT)] == Opcode_String_EXT:
    print "The EXT stream in bin file and assembler file match."
else:
    print "The EXT stream in bin file and assembler file do not match."

##print BinFile_String_IPM[len(Opcode_String_IPM):]
##os.system('pause')
##print BinFile_String_EXT[len(Opcode_String_EXT):]

