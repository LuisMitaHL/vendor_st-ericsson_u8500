import re,sys,xlwt,os                                                                       #import modules,add if not present
result_workbook = xlwt.Workbook()                                                           #open an excel workbook
testfilename=sys.argv[1]                                                                    #get log file name
result_file_path=sys.argv[2]                                                                #get path where result is to be stored
result_file_name=result_file_path+'\\result.xls'
if not os.path.exists(testfilename):                                                        #check if trace file present
    print '\n\n\nINPUT LOG FILE NOT FOUND \n\n'
    sys.exit()
if not os.path.exists(result_file_path):
    os.mkdir(result_file_path)
    file=open(result_file_name,'w')                                                         #open the test file
    file.close()

file=open(testfilename,'r')                                                                 #open the test file
contents_of_trace_file=file.read()                                                          #store its contents
file.close()                                                                                #close the file


style = xlwt.easyxf("", "###,###")                                                          #1000 seperator enable


font = xlwt.Font()                                                                          #Create Style for headings
font.bold = True
style_for_heading = xlwt.XFStyle()
style_for_heading.font = font
pattern = xlwt.Pattern()
pattern.pattern = xlwt.Pattern.SOLID_PATTERN
pattern.pattern_fore_colour = 7
style_for_heading.pattern = pattern
borders = xlwt.Borders()
borders.left = xlwt.Borders.THIN
borders.right = xlwt.Borders.THIN
borders.top = xlwt.Borders.THIN
borders.bottom = xlwt.Borders.THIN
style_for_heading.borders = borders

starting_indicator='start_xti_profiler_'                                                    #define starting_indicator
stopping_indicator='stop_xti_profiler_'                                                     #define stopping_indicator
#if you want to change the keywords, do so above

column_A_heading='NAME'                                                                     #define column_A_heading
column_B_heading='TIME (ns)'                                                                #define column_B_heading
column_C_heading='MACHINE CYCLES'                                                           #define column_C_heading
#if you want to change the headings of result file, do so above

def time_calculator(interrupt_name):                                                        #define a function to get the time taken
    lines = tuple(open(testfilename, 'r'))                                                  #take lines as a tuple
    row=1                                                                                   #intialise row counter
    flag=0                                                                                  #initialse flag as 0
    sheet.write(0,0,column_A_heading,style_for_heading)                                     #write column A heading
    sheet.write(0,1,column_B_heading,style_for_heading)                                     #write column B heading
    temp_array_time=[]                                                                      #temporary array to store data
    sum_temp_array_time=0                                                                   #declare array sum as 0
    temp_array_mcycle=[]                                                                    #temporary array to store data
    sum_temp_array_mcycle=0                                                                 #declare array sum as 0


    for line in lines:                                                                      #for every line in file
        substring_start=starting_indicator+interrupt_name                                   #starting interrupt label
        substring_stop=stopping_indicator+interrupt_name                                    #stopping interrupt label
        if re.search(substring_start,line):                                                 #if line contains starting interrupt label
            start_time=line.split()[0].rstrip(':')                                          #store start time
            flag=1                                                                          #set flag
        if re.search(substring_stop,line) and flag:                                         #if line contains stopping interrupt label
            stop_time=line.split()[0].rstrip(':')                                           #store stop time
            time_taken=long(stop_time)-long(start_time)                                     #calculate difference

            temp_array_time.append(time_taken)
            sum_temp_array_time=sum_temp_array_time+time_taken
            mc_cycles=line.split()[-1].strip(':')                                           #get machine cycle
            if mc_cycles==substring_stop:                                                   #compare if it is same as stop label or m/c cycle
                                                                                            #is absent
                sheet.write(row,0,interrupt_name)                                           #store interrupt name in column 0
                sheet.write(row,1,time_taken,style)                                         #store time in column 1
                print interrupt_name,'    ',time_taken                                      #print result in command prompt
            else:
                if mc_cycles :
                    sheet.write(row,0,interrupt_name)                                       #store interrupt name in column A
                    sheet.write(row,1,time_taken,style)                                     #store time in column B
                    machine_cycles=int(mc_cycles)
                    sheet.write(row,2,machine_cycles,style)                                 #store time in column C
                    print interrupt_name,'    ',time_taken,'    ',machine_cycles            #print result in command prompt
                    temp_array_mcycle.append(machine_cycles)
                    sum_temp_array_mcycle=sum_temp_array_mcycle+machine_cycles
                else:
                    print interrupt_name,'    ',time_taken
                    sheet.write(row,0,interrupt_name)                                       #store interrupt name in column A
                    sheet.write(row,1,time_taken,style)                                     #store time in column B
            row += 1                                                                        #increment row counter
            flag=0                                                                          #reset flag
    sheet.col(4).width=int(3500)                                                            #set width of column
    sheet.col(5).width=int(3500)                                                            #set width of column
    sheet.col(6).width=int(3500)                                                            #set width of column
    sheet.write(0,4,'Min. Time (ns)',style_for_heading)                                     #write mimimum column. heading
    sheet.write(1,4,min(temp_array_time),style)                                             #write minimum time taken
    sheet.write(0,5,'Max. Time (ns)',style_for_heading)                                     #write maximum column. heading
    sheet.write(1,5,max(temp_array_time),style)                                             #write maximum time taken
    sheet.write(0,6,'Avg. Time (ns)',style_for_heading)                                     #write average column. heading
    sheet.write(1,6,float(sum_temp_array_time/len(temp_array_time)),style)                  #write average time taken

    if len(temp_array_mcycle) is not 0:
        sheet.write(2,4,'Min. Cycles',style_for_heading)                                    #write mimimum column. heading
        sheet.write(3,4,min(temp_array_mcycle),style)                                       #write minimum time taken
        sheet.write(2,5,'Max. Cycles',style_for_heading)                                    #write maximum column. heading
        sheet.write(3,5,max(temp_array_mcycle),style)                                       #write maximum time taken
        sheet.write(2,6,'Avg. Cycles',style_for_heading)                                    #write average column. heading
        sheet.write(3,6,float(sum_temp_array_mcycle/len(temp_array_mcycle)),style)          #write average time taken
        sheet.write(0,2,column_C_heading,style_for_heading)

interrupt_name_array=[]                                                                     #declare an empty array to store name of interrupts
text_split=contents_of_trace_file.split()                                                   #split and store the contents of file

for word in text_split:                                                                     #for every word in file
    if word.startswith(starting_indicator):                                                 #if it starts with the starting indicator
        interrupt_name=word[len(starting_indicator):]                                       #store interrupt name
        if interrupt_name in interrupt_name_array:                                          #ignore if already stored
            continue
        else:
            interrupt_name_array.append(interrupt_name)                                     #add the name to the array
    else:
        continue                                                                            #ignore else
interrupt_name_array.sort(key=str.lower)                                                    #sort the list alphabetically

if len(interrupt_name_array)==0:                                                            #if no interrupt found
    print '\n\nNO COMPATIBLE TRACE(S) FOUND'                                                #print 'NO COMPATIBLE TRACE(S) FOUND'
else:
    for interrupt_name in interrupt_name_array:                                             #for every diff. interrupt name
        sheet = result_workbook.add_sheet(interrupt_name[:31])                              #create a new sheet
        col0_size=len(interrupt_name)
        col1_size=len(column_B_heading)
        col2_size=len(column_C_heading)
        sheet.col(0).width=int(500 * col0_size)                                             #set width of column
        sheet.col(1).width=int(700 * col1_size)                                             #set width of column
        sheet.col(2).width=int(325 * col2_size)                                             #set width of column
        time_calculator(interrupt_name)                                                     #call the function
    result_workbook.save(result_file_name)                                                  #save the file at desired path