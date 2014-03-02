#! /bin/sh
# Author: PER

export DTH9p
export mnt9p
export PATHSCRIPTHATS
VERSION="HATS_TAT_AUTOMATIC_DTH_TEST_V2.4"
################################################################################################
#VERSION                                | MODIFICATION                    |   AUTOR   | DATE   #
#---------------------------------------|---------------------------------|-----------|--------#
#HATS_TAT_AUTOMATIC_DTH_TEST_V1.0       | CRERATION                       |   PER     |20/09/11#
#HATS_TAT_AUTOMATIC_DTH_TEST_V1.1       | UPDATE EXEC SERVICES            |   PER     |20/09/11#
#HATS_TAT_AUTOMATIC_DTH_TEST_V1.2       | UPDATE EXEC with check path     |   PER     |20/09/11#
#HATS_TAT_AUTOMATIC_DTH_TEST_V1.3       | Add file resume result in path  |           |        #
#                                       | //var/log/"Feature"Result.log   |   PER     |20/09/11#
#HATS_TAT_AUTOMATIC_DTH_TEST_V1.4       | treat check file access         |           |        #
#                                       | compare string                  |   PER     |21/09/11#
#HATS_TAT_AUTOMATIC_DTH_TEST_V1.5       | Manage return error in function |   PER     |21/09/11#
#HATS_TAT_AUTOMATIC_DTH_TEST_V1.6       | Create function for lisibility  |   PER     |21/09/11#
#HATS_TAT_AUTOMATIC_DTH_TEST_V1.7       | update function header          |   PER     |22/09/11#
#HATS_TAT_AUTOMATIC_DTH_TEST_V1.8       | correction double // and add    |           |        #
#                                       | possibility to set path with DTH|   PER     |23/09/11#
#HATS_TAT_AUTOMATIC_DTH_TEST_V1.9       | update P1 test with scenarios   |   PER     |28/09/11#
#HATS_TAT_AUTOMATIC_DTH_TEST_V2.0       | update and correction P0 test   |   PER     |30/09/11#
#HATS_TAT_AUTOMATIC_DTH_TEST_V2.1       | change name file result hudson  |   PER     |18/10/11#
#HATS_TAT_AUTOMATIC_DTH_TEST_V2.2       | update result for jatt          |   PER     |24/10/11#
#HATS_TAT_AUTOMATIC_DTH_TEST_V2.3       | Add some improvement	          |   FPI     |05/04/12#
#HATS_TAT_AUTOMATIC_DTH_TEST_V2.4       | Add improvement on float compare|   LVI     |13/07/12#
################################################################################################

LogError="LogError.log"
ResultFile="result.txt"
DTHpathTest=$DTH9p/TEST
PATH_HUDSON="/var/log"
PATH_HUDSON_INIT="/var/log"
#'mount | grep 9p | awk '{print $3;}''
DTHPath='/mnt/9p/DTH/'
ListTestValueTable="LaunchTestValueUTable LaunchTestValueSTable"
ListTestValueSTable="S8 S16 S32 S64"
ListTestValueUTable="U8 U16 U32 U64"
ListTestSpecificTable="BitField File String Float Array"
BitFieldGroupNumber="BITFIELDU8BONEGROUP BITFIELDU8BTWOGROUPS BITFIELDU8BTHREEGROUPS BITFIELDU16BONEGROUP\
 BITFIELDU16BTWOGROUPS BITFIELDU16BTHREEGROUPS BITFIELDU32BONEGROUP BITFIELDU32BTWOGROUPS BITFIELDU32BTHREEGROUPS\
 BITFIELDU64BONEGROUP BITFIELDU64BTWOGROUPS BITFIELDU64BTHREEGROUPS"
StringType="EMPTYSTRING  ONEBYTESTRING  SIXTEENBYTESTRING"
FileType="EMPTYFILE HUNDREDBYTEFILE ONEBYTEFILE"
ArraySize="OneRowOneCol TwoRowsTwoCols"
ListUnsignedRangeValue="RANGE MIDDLEMAX FULLRANGE"
ListSignedRangeValue="RANGE NEGATIV POSITIV FULLRANGE"
CaseRange="InRange OutRange"
ListFeatures="ADC AUDIO CHARGE COEX CONN Debug GPS HDMI MODEM PWS RF SMC STORAGE TEST VIDEO iq_tunning AB DB"
ListBasicFeatures="ADC AUDIO CHARGE MODEM PWS RF SMC STORAGE VIDEO AB DB"
ListGlobalTest=" TEST ADC AUDIO CHARGE COEX CONN Debug GPS HDMI MODEM PWS RF SMC STORAGE TEST VIDEO iq_tunning AB DB"


################################################################################
#FUNCTION GenerateTestValue => generate value for P0 test
#GenerateTestValue $VAR_DTH_MAX $VAR_DTH_MIN InRange $TypeTest
################################################################################
GenerateTestValue () {
case $3 in
InRange)
     if [ `expr $2 \> 0` ]
     then
       if [ `expr $1 \> $2` ]
					then
					if [ $4 = 'Float' ]; then
					 testvalue=`echo "$1 - $2" | bc`
					else
					  testvalue=`expr $2 + 5` 
					fi
			 else
    			testvalue=0
			 fi
		  else
				if [ `expr $1 \> 0` ]
				then
				  if [ $4 = 'Float' ]; then	  
		    		testvalue=`echo "$1 + $2" | bc`
		    	else
		    		testvalue=`expr $1 + $2`
		    	fi
		    else
		    if [ $4 = 'Float' ]; then
		      testvalue=`echo "$2 + $1" | bc`
		    else
		    	testvalue=`expr $2 - $1`
		    fi
		    fi
		  fi   
;;
OutRange)
				if [ $4 = 'Float' ]; then
					testvalue=`echo "$1 + 1" | bc`
				else
					testvalue=`expr $1 + 1`
				fi		
;;
esac      
}
################################################################################
#FUNCTION CalculMaxValue => return max value use following type passed
################################################################################
CalculMaxValue () {
case $1 in
U8|u8)
MaxValueType=255
;;
U16|u16)
MaxValueType=65535
;;
U32|u32)
MaxValueType=4294967295
;;
U64|u64)
MaxValueType=18446744073709551616
;;
Float|float)
MaxValueType=2147483647
;;
S8|s8)
MaxValueType=127
;;
S16|s16)
MaxValueType=32767
;;
S32|s32)
MaxValueType=2147483647
;;
S64|s64)
MaxValueType=9223372036854775807
;;
*)
MaxValueType=0xBAADF00D
;;
esac
}
################################################################################
#FUNCTION CalculMinValue => return min value use following type passed
################################################################################
CalculMinValue () {
case $1 in
U8|u8)
MinValueType=0
;;
U16|u16)
MinValueType=0
;;
U32|u32)
MinValueType=0
;;
U64|u64)
MinValueType=0
;;
Float|float)
MinValueType=-2147483647
;;
S8|s8)
MinValueType=-127
;;
S16|s16)
MinValueType=-32767
;;
S32|s32)
MinValueType=-2147483647
;;
S64|s64)
MinValueType=-9223372036854775807
;;
*)
MinValueType=0xBAADF00D
;;
esac
}

################################################################################
# FUNCTION CompareData => compare two datas or datas range following type
# parameter 1 is data writed in READ command
################################################################################
CompareData()
{
case  $VAR_DTH_TYPE in
#compare two string
float)
           #check if there is value to compare or ranges
        data1=$(echo $data | cut -d'-' -f1)                                                                               
        data2=$(echo $data | cut -d'-' -f2)
        result=PASSED
        [[ -n "$data1" ]] && result=$(echo $data1 $VAR_READ_OUTPUT |
            awk '{if ($1 <= $2) print "PASSED"; else print "FAILED"}')
        [[ "$result" == "FAILED" ]] && return
        [[ -n "$data2" ]] && result=$(echo $data2 $VAR_READ_OUTPUT |
            awk '{if ($1 >= $2) print "PASSED"; else print "FAILED"}')
        ;;

string)
           #check if there is value to compare or ranges
        data1=$(echo $data | cut -d'-' -f1)                                                                               
        if [ -z "$data1" ]; then
        
        #case no value to compare
            result=PASSED
        else              
	      
           if [ "$data1" = "$VAR_READ_OUTPUT" ]; then
                result=PASSED
             else
                result=FAILED
            fi
        fi
        ;;
#compare two numbers               
*)      #check if there is value to compare or ranges
        data1=$(echo $data | cut -d'-' -f1)                                                                               
	      data2=$(echo $data | cut -d'-' -f2)

	      if [ "$data1" != "" ]; then
	         data1="$(echo $(($data1)))"
	      fi
        if [ "$data2" != "" ]; then
	         data2="$(echo $(($data2)))"
	      fi
        if [ "$data1" = "$data2" ]; then
	         data2=""
	      fi        
        if [ -z "$data1" ]; then
         if [ -z "$data2" ]; then
        #case no value to compare
            result=PASSED
          else
             data1="-""$data2"
             if [ "$VAR_READ_OUTPUT" = "$data1" ]; then
                #case one value to compare
                result=PASSED
             else
                result=FAILED
               fi 
          fi  
        else              
	      
           if [ -z "$data2" ]; then
             if [ "$VAR_READ_OUTPUT" = "$data1" ]; then
                #case one value to compare
                result=PASSED
             else
                result=FAILED
             fi
           else
           #case range values to compare
           if [ $data1 -lt $data2 ]; then 
             if [ $VAR_READ_OUTPUT -ge $data1 ]; then
                 if [ $VAR_READ_OUTPUT -le $data2 ]; then
                    result=PASSED
                 else
                    result=FAILED 
                 fi             
             else
               result=FAILED
             fi
            else
              echo "$0:Read $PATH: Range value is false"
              result=FAILED
            fi
          fi   
        fi
        ;;
        esac

}

################################################################################
# FUNCTION ResultTest => treat and check result in P0 test and update result test in file and stdout
# parameter 1 is type result
# parameter 2 resume command test
# parameter 3 value to write
# parameter 4 value read after write
################################################################################
ResultTest ()
{
	result=$3
	RESULT=$result
    if [ "$result" = "FAILED" ]; then
		echo -e "$2 is :.......................\033[31m$result \033[0m"
		echo -e "$2 is :.......................\033[31m$result \033[0m" >> $ResultFile
		echo -e "$2 is :.......................\033[31m$result \033[0m" >> $LogError
	else
		echo -e "$2 is :.......................\033[32m$result \033[0m"
		echo -e "$2 is :.......................\033[32m$result \033[0m" >> $ResultFile
	fi
}

################################################################################
# FUNCTION to display comment and title in file and stdout
# parameter 1 comment to display
################################################################################
Titre ()
{
echo -e "\033[34m*************************************************************\033[0m" 
echo -e "\033[34m*************************************************************\033[0m" 
echo -e "\033[34m**$1\033[0m"
echo -e "\033[34m*************************************************************\033[0m"
echo -e "\033[34m*************************************************************\033[0m"

echo -e "\033[34m*************************************************************\033[0m" >> $ResultFile
echo -e "\033[34m*************************************************************\033[0m" >> $ResultFile
echo -e "\033[34m**$1\033[0m">> $ResultFile
echo -e "\033[34m*************************************************************\033[0m">> $ResultFile
echo -e "\033[34m*************************************************************\033[0m">> $ResultFile
}
################################################################################
# FUNCTION getstatus : test mount features and give status on stdout
# $1: path and name of the DTH element
# $2: value to set, which should match the DTH argument's specification
# return 1 on bad argument count
# return 1 if not writable
################################################################################
getstatus()
{
TestMountDTHFeature
}
################################################################################
# FUNCTION write_dth : sets the value of a writable DTH element
# $1: path and name of the DTH element
# $2: value to set, which should match the DTH argument's specification
# return 1 on bad argument count
# return 1 if not writable
################################################################################
write_dth()
{
	local p=$1
	local v=$2
	type_dth $p
	local TestWrite=`ls -al "$1/value" | cut -c3`
	if [ "$TestWrite" = "w" ]; then
	    if [ "$VAR_DTH_TYPE" = "file" ]; then
			if [ -e $2 ]; then
				sk.sh dth w $p $v
			else
				echo -e "\033[31m$0 function write_dth : file doen't exist\033[0m"
            return 1
			fi
		else
			sk.sh dth w $p $v
		fi
	else
		echo -e "\033[31m$0:write_dth: not right to write\033[0m"
		return 1
	fi
  return 0      	
}
################################################################################
# FUNCTION read_dth :read the value of a file assuming its type is known.
# $1: path and name of the file to read
# $2: type name to decode the file
# $VAR_READ_OUTPUT: value returned
# return 1 on bad argument count
# return 1 if not readable
################################################################################
read_dth()
{
	if [ $# -lt 1 ]; then
		echo -e "\033[31m$0 :not enough argument for dth_read\033[0m"
		return 1
	fi

	local TestRead=`ls -al "$1/value" | cut -c2`
	if [ "$TestRead" = "r" ]; then
		local p=$1
		type_dth $p
		if [ "$VAR_DTH_TYPE" = "file" ]; then
			# just an octet stream, cat is enough
			$VAR_READ_OUTPUT=`cat "$p/value"`
		else
			# need to convert to human readable
			VAR_READ_OUTPUT=$(sk.sh dth r $p value)
		fi
		if [ -z "$VAR_READ_OUTPUT" ]; then
			if [ "$VAR_DTH_TYPE" != "string" ]; then
				if [ "$VAR_DTH_TYPE" != "file" ]; then
					echo -e "\033[31m$0:read_dth: nothing read\033[0m"
					return 1
				fi
			fi
		fi
	else
		echo -e "\033[31m$0:read_dth: not right to read\033[0m"
		return 1
	fi
	return 0
}
################################################################################
# FUNCTION exec_dth :execute a DTH action
# generaly, the command to perform should have been set before.
# Prefer exec_dth() which performs both operations, if the action has
# commands.
# $1: path and name of the DTH element
# return 1 on bad argument count
# return 1 if not executable
################################################################################
exec_dth()
{
	if [ $# -lt 1 ]; then
		echo -e "\033[31m$0:not enough argument for dth_exec\033[0m"
		return 1
	fi
	local TestExec=`ls -al "$1/value" | cut -c4`
	if [ "$TestExec" = "x" ]; then
		cat "$1/value"
	else
		echo -e "\033[31m$0:exec_dth: not right to execute\033[0m"
	fi
	return 0
}
################################################################################
# FUNCTION type_dth :get the type of a DTH action
# generaly, the command to perform should have been set before.
# Prefer exec_dth() which performs both operations, if the action has
# commands.
# $1: path and name of the DTH element
# return 1 on bad argument count
################################################################################
type_dth()
{
	if [ $# -lt 1 ]; then
		echo -e "\033[31m$0:not enough argument for type_dth\033[0m"
		return 1
	fi
	local p=$1
	VAR_DTH_TYPE=$(sk.sh dth r $p type)
	return 0
}
################################################################################
# FUNCTION getinfo_dth :get info of a DTH action => min, max, type and display on stdout
# $1: path and name of the DTH element
# return 1 on bad argument count
################################################################################
getinfo_dth()
{
	if [ $# -lt 1 ]; then
		echo -e "\033[31m$0:not enough argument for getinfo_dth\033[0m"
		return 1
	fi
	local p=$1
	VAR_DTH_TYPE=$(sk.sh dth r $p type)
	if [ "$VAR_DTH_TYPE" != file ]; then
		if [ -f $p/min ]; then
			VAR_DTH_MIN=$(sk.sh dth r $p min)
			VAR_DTH_MAX=$(sk.sh dth r $p max)
		else
			CalculMinValue $VAR_DTH_TYPE
			VAR_DTH_MIN=$MinValueType
			CalculMaxValue $VAR_DTH_TYPE
			VAR_DTH_MAX=$MaxValueType
		fi
	fi
	return 0
}
################################################################################
# FUNCTION CheckPathDTH :check path dth validity
# $1: path and name of the DTH element
# return 1 unknown path
# complete path by /mnt/9p/DTH if necessary
################################################################################
CheckPathDTH()
{
	if [ -d $1 ]; then
		PATH_DTH=$1
	else
		if [ -d "$DTH9p$1" ]; then
			PATH_DTH="$DTH9p$1"
		else
			if [ -d "$mnt9p$1" ]; then
				PATH_DTH="$mnt9p$1"
			else
				echo -e "\033[31m$0 : $1 unknown path DTH\033[0m"
				return 1
			fi
		fi
	fi
return 0    		
}
################################################################################
# FUNCTION InitEnvTestDTH : INIT GLOBAL VAR and PATH
# INIT var global DTH9p => path 9p DTH
# Initialise global var
################################################################################
InitEnvTestDTH()
{
 DTHPATH="" 
 DATAS=""
 RESULT="PASSED"
 NAMETEST=""
 NAMEFEATURE=""
 VAR_DTH_MIN=""
 VAR_DTH_MAX=""
 VAR_DTH_TYPE=""
 NAMEFEATURE=""
 RECORD_LOOP="NO"
 ValMem1=0
 ValMem2=0
 ValMem3=0
 ValMem4=0
 ValMem5=0
 if [ "$DTH9p" == "" ]
	then
		echo "initializing DTH 9p test toolkit"
		echo ""
		DTH9p=`mount | grep 9p | awk '{ print $3 }'`/DTH
		mnt9p=`mount | grep 9p | awk '{ print $3 }'`
	fi

 PATH_HUDSON="/var/log"
 PATH_FILE_LOCAL="/var/local/dthservices/dthltest"
 if [ -d $PATH_FILE_LOCAL ]; then
    PATHSCRIPTHATS="/usr/bin/scripts/hats_services"
 else
    PATH_FILE_LOCAL="/var/local/tat/tatltest"
    PATHSCRIPTHATS="/usr/bin/scripts/hats_services"
 fi
}
################################################################################
# FUNCTION TestMountDTHFeature :Services to test the mount of service
# result is passed or failed
################################################################################
TestMountDTHFeature()
{
Titre "read DTH path feature $1 mounted ?"

TestPath="$DTH9p/$1"
if [ -d $TestPath ]; then
		resultMount=PASSED
		ListTestToDoMounted="$ListTestToDoMounted $TestName"
else
    resultMount=FAILED
fi    		
ResultTest "mount" "mount of $TestPath" $resultMount
}
################################################################################
# FUNCTION testserv_dth : test one or all feature mounted
# test one or several specify 9p DTH services are well mounted
# parameter 1 : "" if test all feature
# parameter 1 : "featurename"
################################################################################
testserv_dth()
{
#InitEnvTestDTH
if [ $# -eq 1 ]; then 
 for param in $ListGlobalTest
 do
	TestMountDTHFeature $param
 done	
else
 param="$*"
 for param in $*
 do
 if [ $param != "TESTSERV" ]; then
	TestMountDTHFeature $param
fi
done	
fi
if [ -f $LogError ]; then rm $LogError; fi
}
################################################################################
# FUNCTION InterpreterScriptEntry : read eache line from script file
# parameter 1 : script file with scenarios
################################################################################
InterpreterScriptEntry()
{
#suppress all empty lines in entry file
sed '/^$/d' $1 > test.tmp

 while read line  
do   
   DecodeDTHCommand $line
done < test.tmp

rm  test.tmp

Titre "Resume Execution Scenario result in file result $ResultFile"
if [ -e $PATH_HUDSON ]; then
  cat $PATH_HUDSON >> $ResultFile
else
  echo "Warning: no $PATH_HUDSON created. Script probably not well formed."
fi
}
################################################################################
# FUNCTION ManageArrayElement : Manage command to write one element in array
# parameter 1 : datas : [row,col] value
################################################################################
ManageArrayElement()
{
	#treat data array entry (row,col)value and extract
    #filterRow=row, filterCol=col, valuearray=value
    local filterRow="$(echo $2 | cut -d'[' -f2 )"
	filterRow="$(echo $filterRow | cut -d',' -f1 )"
	local filterCol="$(echo $2 | cut -d'[' -f2 )"
	filterCol="$(echo $filterCol |  cut -d',' -f2 )"
	local valuearray=$filterCol
	filterCol="$(echo $filterCol |  cut -d']' -f1 )"
	valuearray=$(echo $valuearray |  cut -d']' -f2 )
	valuearray="$(echo $(($valuearray)))"
	#Concat RowColw
    local RowCol=`expr $filterRow \* $filterCol`
    local indice=`expr $RowCol + $filterCol`

	case $1 in
	WRITE)
		#read old array values before to write only the value changed on the table
        read_dth $PATH_DTH
           
        #data read is like x;y;z;w and modify to x y z w
        local i=0
		local readLocal=""
		for i in 1 2 3 4
		do
			readLocal="$readLocal $(echo $VAR_READ_OUTPUT | cut -d';' -f$i )"
		done
        #replace value in [a,b] and write complete datas array
		i=0
		local j=1
		local valuetowrite=""
		for i in $readLocal
		do
			if [ $indice = $j ]; then
				if [ "$valuetowrite" = "" ]; then
					valuetowrite=$valuearray
				else
					valuetowrite="$valuetowrite;$valuearray"
				fi
			else
				if [ "$valuetowrite" = "" ]; then
					valuetowrite="$i"
				else
					valuetowrite="$valuetowrite;$i"
				fi
			fi
			j=`expr $j + 1`
		done
		echo $valuetowrite
		;;
	READ)
		local valuetoread=$(echo $VAR_READ_OUTPUT  | cut -d';' -f$indice)
		echo $valuetoread
		;;
	esac
}
################################################################################
# FUNCTION DecodeDTHCommand : Decode command from script file or command in line
# command list in parameter :
# # comment
# #> comment to display in result file
# FEATURE « Name Feature +
# BEGIN  « test name +
# END « test name +
# READ « DTH path +
# READ « DTH path + « value +  
# READ « DTH path + « RangeMin – RangeMax + 
# READ « DTH path + [row,col]
# WRITE « DTH path + « value +
# WRITE « DTH path + [row,col]value
# WRITE « DTH path + row0Col0;row0Col1;…….
# LOCALEXE «  NameOfFile+
# GETINFO « PATH DTH +
# GETSTATUS
# TESTSERV « feature +
# TESTSERV « feature1 + « feature2 + …….  « feature9 +
# EXEC « DTH path +
# WAIT « seconds number +
# PAUSE
# PAUSE « comment +
# LOOP N
################################################################################
DecodeDTHCommand()
{

	case $1
	in
	
#-------------------------------------------------------------------------------
#FEATURE services : command to precise feature tested by one or several scenario
#Command to use : FEATURE « Name Feature +
#-------------------------------------------------------------------------------
	  FEATURE)
	      NAMEFEATURE="$*"
	      Titre "Test of Feature $NAMEFEATURE"
	  ;;
#-------------------------------------------------------------------------------
#BEGIN services : command to precise start of scenario
#Command to use : BEGIN  « test name +
#-------------------------------------------------------------------------------
    BEGIN)
        NAMETEST="$(echo "$*" | sed sZBEGINZ''Z)"
        RESULT=FAILED
        Titre "Start $NAMETEST"
    ;;
#-------------------------------------------------------------------------------
#END services : command to precise the end of scenario
#Command to use : END « test name +
#-------------------------------------------------------------------------------
    END)
        Titre "END TEST $NAMETEST :..........................$RESULT"
           echo "TEST $NAMETEST :..........................$RESULT" >> $PATH_HUDSON
        NAMETEST=""
    ;;
#-------------------------------------------------------------------------------
#Two cases following the command:
#First case 
#Write DTHPATH [row,col]valuetowrite  
#Second case 
#READ DTHPATH valuetowrite
#-------------------------------------------------------------------------------
		WRITE)
           CheckPathDTH $2
           if [ $? -gt 0 ]; then
		        result=FAILED
		        ResultTest "Other" "Write $PATH_DTH"  $result
		        return 1
		       else 
           data="$3"
           FirstCaracter=$(echo $data | cut -c1)
           #check if first caracters is ( then it is an array with one value changed
           case $FirstCaracter
           in
           '[')
             Valuetowrite=`ManageArrayElement $1 $data`
           ;;
           'V')
           local LastCaracter=$(echo $data | cut -c7)
               case $LastCaracter in
              1) Valuetowrite=$ValMem1 ;;
              2) Valuetowrite=$ValMem2 ;;
              3) Valuetowrite=$ValMem3 ;;
              4) Valuetowrite=$ValMem4 ;;
              5) Valuetowrite=$ValMem5 ;;
              esac
            ;;
            *)
             Valuetowrite=$3
            ;;
            esac

    	     write_dth $PATH_DTH $Valuetowrite

           if [ $? -gt 0 ]; then
		        result=FAILED
		        ResultTest "Other" "Write $PATH_DTH"  $result
		      else
		       ResultTest "Other" "WRITE $PATH_DTH $3"  PASSED
		       fi
		       fi
		;;
#-------------------------------------------------------------------------------
#Two cases following the command:
#First case 
#READ DTHPATH [row,col]  
#READ DTHPATH [row,col] valuewaited  
#READ DTHPATH [row,col] rangemin-rangemax
#Second case 
#READ DTHPATH 
#READ DTHPATH valuewaited  
#READ DTHPATH rangemin-rangemax
#-------------------------------------------------------------------------------       
		READ)
		    #read value to dth patch
		    CheckPathDTH $2
		    if [ $? -gt 0 ]; then
		    result=FAILED
		    return 1
		    else
        read_dth $PATH_DTH
        fi
        if [ $? -gt 0 ]; then
		     result=FAILED
		     ResultTest "Other" "Read $PATH_DTH"  $result
		     return 1
		    else 
		    data="$3"
		    FirstCaracter=$(echo $data | cut -c1)
          #check if first caracters is ( then it is an array with one value changed
          case $FirstCaracter
           in
           '[')
             Valuetoread=`ManageArrayElement $1 $data`
              VAR_READ_OUTPUT=$Valuetoread
		          #check if there is read result to compare
              data=$(echo "$*" | cut -d' ' -f4)
           ;;
           'V')
           local LastCaracter=$(echo $data | cut -c7)
              case $LastCaracter in
              1) ValMem1=$VAR_READ_OUTPUT ;;
              2) ValMem2=$VAR_READ_OUTPUT ;;
              3) ValMem3=$VAR_READ_OUTPUT ;;
              4) ValMem4=$VAR_READ_OUTPUT ;;
              5) ValMem5=$VAR_READ_OUTPUT ;;
              esac
               #check if there is read result to compare
              data=$(echo "$*" | cut -d' ' -f4)
            ;;
            *)
             #check if there is read result to compare
              data=$(echo "$*" | cut -d' ' -f3)
            ;;
            esac
        
        CompareData  $data
       
        if [ -z "$data1" ]; then
          if [ "$VAR_DTH_TYPE" = "file" ]; then
          testfile="$PATH_DTH"value
           if [ -e $testfile ]; then
          ResultTest "Other" "Read $PATH_DTH File "  PASSED
          else
           ResultTest "Other" "Read $PATH_DTH File "  FAILED
           fi
          else
          ResultTest "Other" "Read $PATH_DTH value is equal to $VAR_READ_OUTPUT "  $result
          fi
        else
          ResultTest "Other" "Read $PATH_DTH : $VAR_READ_OUTPUT to compare with  $*"  $result
        fi
        fi  
        ;;
#-------------------------------------------------------------------------------
#EXEC services : command to exec path DTH
#Command to use : EXEC « DTH path +
#-------------------------------------------------------------------------------        
		EXEC)		CheckPathDTH $2
		if [ $? -gt 0 ]; then
		 result=FAILED
		else 
    exec_dth $PATH_DTH
    if [ $? -gt 0 ]; then
		 result=FAILED
		fi
		fi ;;
#-------------------------------------------------------------------------------
#TESTSERV => check if all features are well mounted
#TESTSERV  FeatureNameList => check if features in list are well mounted
#-------------------------------------------------------------------------------
		TESTSERV)
     testserv_dth $*;;
#-------------------------------------------------------------------------------
#GETINFO => Command to have info DTH element : min, max and type
#  Command use : GETINFO « PATH DTH +
#-------------------------------------------------------------------------------
		GETINFO)
    CheckPathDTH $2
    if [ $? -gt 0 ]; then
		 result=FAILED
	 else
		 getinfo_dth $PATH_DTH
		 if [ $? -gt 0 ]; then
			 result=FAILED
		 else
			 echo "min = $VAR_DTH_MIN"
			 echo "max = $VAR_DTH_MAX"
			 echo "type = $VAR_DTH_TYPE"
		 fi
	 fi
	 ;;
#-------------------------------------------------------------------------------
#LOCALEXE => Command to execute exe file or shell file
#  Command use : LOCALEXE «  NameOfFile+
#-------------------------------------------------------------------------------
		LOCALEXEC) sh $2;;
		GETSTATUS) getstatus_dth $2
    ;;
#-------------------------------------------------------------------------------
#WAIT => command to wait next command execution several seconds
#  Command to use : WAIT « seconds number +
#-------------------------------------------------------------------------------
		WAIT) 
    sleep "$2" 
    ;;
#-------------------------------------------------------------------------------
#PAUSE => Command to pause between two sequences and continue after press key
#  Command to use : PAUSE
#  Command to use with comment to display on result file and screen: PAUSE « comment +
#-------------------------------------------------------------------------------
		PAUSE)
     echo "$*" | sed sZPAUSEZ''Z
     echo "When you're ready, push one key to continue the test"
     read touche < /dev/tty 
     ;;
#-------------------------------------------------------------------------------
#LOOP => Command to loop several sequences between LOOP and ENDLOOP
#  Command to use : LOOP N  and to the end of the loop ENDLOOP
#-------------------------------------------------------------------------------
     LOOP)
       case $2
	     in
	     END)
	     RECORD_LOOP="NO"
	     while `expr $1 \> 0`
	     do
        while read loop
        do
        DecodeDTHCommand $loop
        done < SaveLoop.txt
        N=`expr $N - 1`
        done
        rm  SaveLoop.txt
	     ;;
	     *)
	      RECORD_LOOP="YES"
        IN_LOOP="YES"
        N=$2
        ;;
        esac
     ;;
     P0_TEST)
       TEST_P0
     ;;
     NON_REG)
      echo "Before to start Non Reg Light Test don't forget: "
      echo "-to insert Sim card"
      echo "-to plug extension screen and sensor card"
      echo "Is it already done ? y or n ?"
      read touche < /dev/tty

      echo $touche
      case $touche in
      n | N)
      echo "you need to switch off board and do it before to start test"
      ;;
      y | Y)
      TEST_P0
      for ListTestToDo in CHARGE ADC GAS_GAUGE PWS AB DB PWS SMC AB_ALL DB_ALL
		  do
		  TEST_P1 $ListTestToDo
		  done
      for ListTestToDo in SD DISPLAY VIDEO ADC
		  do
		  TEST_P2 $ListTestToDo
		  done
      ;;
      *)
      echo "you need to switch off board and do it before to start test"
      ;;
      esac
     ;;
     IF)
      local LastCaracter=$(echo $2 | cut -c7)
          case $LastCaracter in
              1) local Valuetocheck=$ValMem1 ;;
              2) local Valuetocheck=$ValMem2 ;;
              3) local Valuetocheck=$ValMem3 ;;
              4) local Valuetocheck=$ValMem4 ;;
              5) local Valuetocheck=$ValMem5 ;;
          esac
        if [ "$Valuetocheck" = "$3" ]; then
           result=PASSED
        else
           result=FAILED
        fi

       ResultTest "Other" "$NAMETEST "  $result
     ;;
     RESULT)
     echo "$*" | sed sZRESULTZ''Z
     echo "When you're ready, push one key to continue the test"
     read touche < /dev/tty

     case $touche in
       y | y)
       result=PASSED
       ;;
       n | N)
       result=FAILED
       ;;
       *)
       result=FAILED
       ;;
       esac

      ResultTest "Other" "$NAMETEST "  $result
      ;;
		*)
#-------------------------------------------------------------------------------
# # Add comment putting a # at the beginning of the line
#-------------------------------------------------------------------------------		
        TestComment=$(echo $1 | cut -c1)		
		    if [ "$TestComment" = "#" ]; then
		    TestComment=$TestComment$(echo $1 | cut -c2)
#-------------------------------------------------------------------------------
# #> Add comment to display in result file and on screen putting a #> at the beginning of the line
#-------------------------------------------------------------------------------
		     if [ "$TestComment" = "#>" ]; then
		       echo "$*"
		       echo "$*" >> $ResultFile
		     fi
		     else
          echo -e "$1 command not supported"
          exit
         
        fi;;
	esac
if [ RECORD_LOOP == "YES" ];then
  echo $* >> SaveLoop.txt

fi
}
################################################################################
# FUNCTION InitTestFeature init and test path for one feature
# parameter 1 : feature name
################################################################################
InitTestFeature() 
{
NAMEFEATURE=$1
DTHpathTest=$DTH9p/$1
TestMountDTHFeature $1
DecodeDTHCommand FEATURE $1
}

################################################################################
# FUNCTION TEST P0
################################################################################
TEST_P0()
{
     hudsonfile="P0_TEST.txt"
     hudsonfile="$(echo $hudsonfile | cut -d'.' -f1 )"
     ResultFile="$(echo $1 | cut -d'.' -f1 )"
     PATH_HUDSON="$PATH_HUDSON/""$hudsonfile"".log"
     ResultFile="$ResultFile"".log"
     if [ -e $PATH_HUDSON ]; then
      rm $PATH_HUDSON
     fi
     echo "Version script interpreter HATS TAT script scenario : $VERSION" > $ResultFile
     echo "-----------------------------------------------------------------------------------" >> $ResultFile

InitTestFeature TEST
if [ $resultMount = "FAILED" ]; then
exit
fi

#-------------------------------------------------------------------------------
# Test U8 S8 U16 S16 U32 S32 U64 S64  
#-------------------------------------------------------------------------------
for ListTest in $ListTestValueTable; do
#check Signed or Unsigned type to test
case $ListTest in
LaunchTestValueUTable) 
ListTest1=$ListTestValueUTable
ListRangeValue=$ListUnsignedRangeValue
;;

LaunchTestValueSTable) 
ListTest1=$ListTestValueSTable
ListRangeValue=$ListSignedRangeValue
;;
esac

for TypeTest in $ListTest1; do
CalculMaxValue  $TypeTest
Titre "Reading  $TypeTest test DTH"
echo "max Value type $TypeTest $MaxValueType"
DecodeDTHCommand BEGIN $TypeTest
for RangeValue in $ListRangeValue; do

TypeTestRangeValue=$TypeTest$RangeValue
#Save dth path to test
PATH_DTH="$DTHpathTest/$TypeTest/$TypeTestRangeValue/"
Titre "Reading $TypeTestRangeValue test DTH"
#getinfo min max and type value
DecodeDTHCommand GETINFO $PATH_DTH

if [ -z "$VAR_DTH_MAX" ]; then
        testvalue=$MaxValueType
else
GenerateTestValue $VAR_DTH_MAX $VAR_DTH_MIN InRange $TypeTest
#Test write Min Value
DecodeDTHCommand WRITE $PATH_DTH "$VAR_DTH_MIN"
DecodeDTHCommand READ "$PATH_DTH" "$VAR_DTH_MIN"

#Test write Max Value
DecodeDTHCommand WRITE  $PATH_DTH "$VAR_DTH_MAX"
DecodeDTHCommand READ "$PATH_DTH" "$VAR_DTH_MAX"
fi
#Test Value between range
DecodeDTHCommand WRITE $PATH_DTH "$testvalue"
DecodeDTHCommand READ "$PATH_DTH" "$testvalue"

done
DecodeDTHCommand END $TypeTest
done   
done

#-------------------------------------------------------------------------------
# Specific test Bitfield STRING FILE Float
#-------------------------------------------------------------------------------

for TypeTest in $ListTestSpecificTable; do
echo -e "\033[34mspecific test $TypeTest\033[0m" 
Titre "Reading Signed $TypeTest test DTH"


case $TypeTest in
#-------------------------------------------------------------------------------
# BITFIELD
#-------------------------------------------------------------------------------
BitField)

for TypeGroup in $BitFieldGroupNumber; do
Titre "Reading Unsigned $TypeGroup test DTH"
PATH_DTH="$DTHpathTest/$TypeTest/$TypeGroup/"
DecodeDTHCommand BEGIN $TypeGroup
#getinfo min max and type value
DecodeDTHCommand GETINFO $PATH_DTH

GenerateTestValue $VAR_DTH_MAX $VAR_DTH_MIN InRange $TypeTest

#Test write Min Value
DecodeDTHCommand WRITE $PATH_DTH "$VAR_DTH_MIN"
DecodeDTHCommand READ "$PATH_DTH" "$VAR_DTH_MIN"

#Test write Max Value
DecodeDTHCommand WRITE $PATH_DTH "$VAR_DTH_MAX"
DecodeDTHCommand READ "$PATH_DTH" "$VAR_DTH_MAX"

#Test Value between range
DecodeDTHCommand WRITE $PATH_DTH "$testvalue"
DecodeDTHCommand READ "$PATH_DTH" "$testvalue"
DecodeDTHCommand END $TypeGroup
done  
;;
#-------------------------------------------------------------------------------
# STRING
#-------------------------------------------------------------------------------
String)

for TypeString in $StringType; do
PATH_DTH="$DTHpathTest/$TypeTest/$TypeString/"
DecodeDTHCommand BEGIN $TypeString
case $TypeString in 
EMPTYSTRING)
testvalue=''  
;;
ONEBYTESTRING)
testvalue='u'
;;

SIXTEENBYTESTRING)
testvalue="longueChaine"
;;
esac
DecodeDTHCommand WRITE $PATH_DTH "$testvalue"
DecodeDTHCommand READ "$PATH_DTH" "$testvalue"
DecodeDTHCommand END $StringType
done
;;
#-------------------------------------------------------------------------------
# FILE
#-------------------------------------------------------------------------------
File)
#FileType="EMPTYFILE HUNDREDBYTEFILE ONEBYTEFILE"
for TypeFile in $FileType; do
PATH_DTH="$DTHpathTest/$TypeTest/$TypeFile/"
DecodeDTHCommand BEGIN $TypeFile
case $TypeFile in 
EMPTYFILE)
testvalue="$PATH_FILE_LOCAL""/emptyfile.txt"
;;
HUNDREDBYTEFILE)
testvalue="$PATH_FILE_LOCAL""/hundredfile.txt"
;;
ONEBYTEFILE)
testvalue="$PATH_FILE_LOCAL""/onebytefile.txt"
;;
esac
DecodeDTHCommand WRITE $PATH_DTH $testvalue
DecodeDTHCommand READ "$PATH_DTH"
DecodeDTHCommand END $TypeFile

#echo  "write value is "
#cat $testvalue
#echo ""
#echo  "read value is"
#cat VAR_READ_OUTPUT
#echo ""
#ResultTest "File" "test DTH Write $PATH_DTH$testvalue" $VAR_READ_OUTPUT $testvalue
done
;;
#-------------------------------------------------------------------------------
# FLOAT
#-------------------------------------------------------------------------------
Float) 
ListRangeValue=$ListSignedRangeValue

CalculMaxValue  $TypeTest
Titre "Reading  $TypeTest test DTH"
echo "max Value type $TypeTest $MaxValueType"
for RangeValue in $ListRangeValue; do

TypeTestRangeValue=FLOAT$RangeValue

Titre "Reading $TypeTestRangeValue test DTH"
PATH_DTH="$DTHpathTest/$TypeTest/$TypeTestRangeValue/"
DecodeDTHCommand BEGIN $TypeTestRangeValue
#getinfo min max and type value
DecodeDTHCommand GETINFO $PATH_DTH

case $TypeTestRangeValue in
FLOATRANGE)
testvalue="3.640000"
;;
FLOATPOSITIV)
testvalue="3.640000"
;;
FLOATFULLRANGE)
testvalue="3.640000"
;;
FLOATNEGATIV)
testvalue="-3.640000"
;;
esac
#Test Value between range
DecodeDTHCommand WRITE $PATH_DTH "$testvalue"
DecodeDTHCommand READ "$PATH_DTH" "$testvalue"
DecodeDTHCommand END $TypeTestRangeValue
#echo "write value is $testvalue"
#echo ""
#echo "read value is"
#echo ""
#cat awReadValue
done
;;
#-------------------------------------------------------------------------------
# Array
#-------------------------------------------------------------------------------
Array)
for arrayType in $ListTestValueUTable
do
for arraysize in $ArraySize
do
if [ "$arrayType$arraysize" != "U64OneRowOneCol" ]; then
PATH_DTH="$DTHpathTest/$TypeTest/$arrayType/$arrayType$arraysize/"
DecodeDTHCommand BEGIN $arrayType$arraysize
case $arraysize in
OneRowOneCol)
testvalue=123 
;;
TwoRowsTwoCols)
testvalue="1;2;3;4"
;;
esac
DecodeDTHCommand WRITE $PATH_DTH "$testvalue"
DecodeDTHCommand READ "$PATH_DTH"
DecodeDTHCommand END $arrayType$arraysize
fi
done
done
;;
esac
done
Titre "Resume Execution Scenario result in file result $ResultFile"
cat $PATH_HUDSON >> $ResultFile
}


################################################################################
# FUNCTION TEST P1
################################################################################
TEST_P1()
{
PATH_HUDSON=$PATH_HUDSON_INIT
NAMEFEATURE=$1
NameSript="$PATHSCRIPTHATS/NonReg"$1"_P1.txt"
dos2unix $NameSript
hudsonfile=`basename "$NameSript"`
hudsonfile="$(echo $hudsonfile | cut -d'.' -f1 )"
ResultFile="$(echo $NameSript | cut -d'.' -f1 )"
PATH_HUDSON="$PATH_HUDSON/""$hudsonfile"".log"
 ResultFile="$ResultFile"".log"
if [ -e $NameSript ]; then
Titre "Test of Features $NAMEFEATURE" 
InterpreterScriptEntry $NameSript
else
echo  "$0 : $NameSript : path or file not found"
fi
}

################################################################################
# FUNCTION TEST P2
################################################################################
TEST_P2()
{
PATH_HUDSON=$PATH_HUDSON_INIT
NAMEFEATURE=$FeaturesToDo
NameSript="$PATHSCRIPTHATS/NonReg"$1"_P2.txt"
dos2unix $NameSript
hudsonfile=`basename "$NameSript"`
hudsonfile="$(echo $hudsonfile | cut -d'.' -f1 )"
ResultFile="$(echo $NameSript | cut -d'.' -f1 )"
PATH_HUDSON="$PATH_HUDSON/""$hudsonfile"".log"
 ResultFile="$ResultFile"".log"
if [ -e $NameSript ]; then
Titre "Test of Features $NAMEFEATURE" 
InterpreterScriptEntry $NameSript
else
echo  "$0 : $NameSript : path or file not found"
fi
}

################################################################################
# FUNCTION MAIN
################################################################################


#echo -n "Enter your login :"
#read nomlogin
#while [ "$nomlogin" = "" ] 
#do
#echo -n "Enter your login please"
#read nomlogin
#done

#echo "login is $nomlogin" > $ResultFile
#echo ""  >> $ResultFile
#echo  "Test DTH server"
InitEnvTestDTH

#Call script
if [ $# -lt 1 ]; then 
echo -n "Enter the result path and his name"
read ResultFile
if [ "$ResultFile" = "" ]
then
    ResultFile=DefaultFile.log   
fi
tempname="$(echo $ResultFile | cut -d'.' -f1 )"
PATH_HUDSON="$PATH_HUDSON""$tempname""Result"".log"
echo "Version script interpreter HATS TAT script scenario : $VERSION" > $ResultFile
echo "-----------------------------------------------------------------------------------" >> $ResultFile
 
echo  "choose your test"
echo ""
echo  "1-Full test and full features"
echo  "2-Test P0"
echo  "3-Test P1 (AB,DB,ADC,AUDIO,PWS,RF,SMC,CHARGE,VIDEO)"
echo  "4-Test P2 (AB,DB,ADC,AUDIO,PWS,RF,SMC,CHARGE,VIDEO)"
echo  "5-Test specific feature"
echo  "6-test your script file"
read MyChoice

case $MyChoice in
1)
		TEST_P0
		for ListTestToDo in $ListBasicFeatures
		do
		TestMountDTHFeature $ListTestToDo
		TEST_P1 $ListTestToDo
		TEST_P2 $ListTestToDo
		done
		
;;
2)

		TEST_P0
;;
3)
		for ListTestToDo in $ListBasicFeatures
		do
		TestMountDTHFeature $ListTestToDo
		TEST_P1 $ListTestToDo
		done
		
;;
4)
		for ListTestToDo in $ListBasicFeatures
		do
		TestMountDTHFeature $ListTestToDo
		TEST_P2 $ListTestToDo
		done
		
;;
5)
echo  "Select your features in the following list:"
i=0
for VarListFeatures in $ListFeatures
do
echo  "$i- $VarListFeatures"
i=`expr $i + 1`
done
i=0
read j
Find feature name choose
for VarListFeatures in $ListFeatures
do
if [ $j -eq $i ]
then
ListTestToDo = $VarListFeatures
break
fi
i=`expr $i + 1`
done

for feature in  $ListTestToDo
do
TEST_P1 $feature
TEST_P2 $feature
done
;;
6)
echo  "Enter the path and the name of your own script : "
read SriptFile
if [ -e $SriptFile ]; then
dos2unix $SriptFile
InterpreterScriptEntry  $SriptFile
else
echo  "$SriptFile : path or file not found"
fi
;;
*) echo -e "$MyChoice choice not supported"
exit
;;
esac
#call script with command
else
#pass scenarion script file to execute
if [ -e $1 ]; then
 hudsonfile=`basename "$1"`
 hudsonfile="$(echo $hudsonfile | cut -d'.' -f1 )"
 ResultFile="$(echo $1 | cut -d'.' -f1 )"
 PATH_HUDSON="$PATH_HUDSON/""$hudsonfile"".log"
 ResultFile="$ResultFile"".log"
 if [ -e $PATH_HUDSON ]; then 
   rm $PATH_HUDSON
 fi
 echo "Version script interpreter HATS TAT script scenario : $VERSION" > $ResultFile
 echo "-----------------------------------------------------------------------------------" >> $ResultFile

 dos2unix $1
InterpreterScriptEntry  $1
else
#Basic command to treat
	DecodeDTHCommand $*
fi
fi
