#! /bin/sh
# Author: PER

ResultFile="tattest.out"
DTHpathTest='/mnt/9p/DTH/TEST'
#'mount | grep 9p | awk '{print $3;}''
DTHPath='/mnt/9p/DTH/'
ListTestValueTable="LaunchTestValueUTable LaunchTestValueSTable"
ListTestValueSTable="S8 S16 S32 S64"
ListTestValueUTable="U8 U16 U32 U64 "
ListTestSpecificTable="BitField File String"
BitFieldGroupNumber="BITFIELDU8BONEGROUP BITFIELDU8BTWOGROUPS BITFIELDU8BTHREEGROUPS BITFIELDU16BONEGROUP\
 BITFIELDU16BTWOGROUPS BITFIELDU16BTHREEGROUPS BITFIELDU32BONEGROUP BITFIELDU32BTWOGROUPS BITFIELDU32BTHREEGROUPS\
 BITFIELDU64BONEGROUP BITFIELDU64BTWOGROUPS BITFIELDU64BTHREEGROUPS"
StringType="EMPTYSTRING  ONEBYTESTRING  SIXTEENBYTESTRING"
FileType="EMPTYFILE HUNDREDBYTEFILE ONEBYTEFILE"
ListUnsignedRangeValue="RANGE MIDDLEMAX FULLRANGE"
ListSignedRangeValue="RANGE NEGATIV POSITIV FULLRANGE"
CaseRange="InRange OutRange"
ListFeatures="AB ADC AUDIO CHARGE COEX CONN DB Debug GPS HDMI MODEM PWS RF SMC STORAGE TEST VIDEO iq_tunning"

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

CalculMaxValue () {
case $1 in
U8)
MaxValueType=255
;;
U16)
MaxValueType=65535
;;
U32)
MaxValueType=4294967295
;;
U64)
MaxValueType=18446744073709551616
;;
Float)
MaxValueType=18446744073709551616
;;
S8)
MaxValueType=127
;;
S16)
MaxValueType=32767
;;
S32)
MaxValueType=2147483647
;;
S64)
MaxValueType=9223372036854775807
;;
esac
}

ResultTest ()
{
result=FAILED
case $1 in

mount)
if [ -d $TestPath ]; then
		result=PASSED
	else
		result=FAILED
	fi
;;
NumberInRange)
if [ `expr $3 = $4` ]
     then
			result=PASSED
else
			result=FAILED
fi
;;			
NumberOutRange)
if [ `expr $4 \> $MaxValueType` ]; then
 			if [ `expr $3 = 0` ]
 				then
 					result=PASSED
 				else
 					result=FAILED
 				fi
 else
 			if [ `expr $4 != $3` ]
 				then
 					result=PASSED
 				else
 					result=FAILED
 				fi
 	fi
;;
String)
if [ "$3" = "$4" ]
 				then
 					result=PASSED
 				else
 					result=FAILED
 				fi
;;
File)
if [ "$3" = "$4" ]
 				then
 					result=PASSED
 				else
 					result=FAILED
 				fi
;;
esac
			echo "$2 is .......................:$result"
			echo "$2 is .......................:$result" >> $ResultFile
}

Titre ()
{
echo "*************************************************************" 
echo "*************************************************************" 
echo "**$1"
echo "*************************************************************"
echo "*************************************************************"

echo "************************************************************" >> $ResultFile
echo "************************************************************" >> $ResultFile
echo "**$1">> $ResultFile
echo "************************************************************">> $ResultFile
echo "************************************************************">> $ResultFile
}


#echo -n "Enter your login : "
#read nomlogin
#echo -n "Enter the result path and his name "
#read ResultFile

####################################
# Test features mounted under 9p ###
####################################

Titre 'read DTH path feature mounted'
for TestName in $ListFeatures; do
TestPath="$DTHPath$TestName"
ResultTest "mount" "mount of $TestPath"
done

#####################################################################################
##Test U8 S8 U16 S16 U32 S32 U64 S64 Float ##########################################
#####################################################################################
for ListTest in $ListTestValueTable; do
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
for RangeValue in $ListRangeValue; do
if [ "$TypeTest" = "Float" ]; then
TypeTestRangeValue=FLOAT$RangeValue
else
TypeTestRangeValue=$TypeTest$RangeValue
fi
Titre "Reading $TypeTestRangeValue test DTH"

MinValue=$(sk.sh dth r $DTHpathTest/$TypeTest/$TypeTestRangeValue/ min)
MaxValue=$(sk.sh dth r $DTHpathTest/$TypeTest/$TypeTestRangeValue/ max) 

for range in $CaseRange
do
GenerateTestValue $MaxValue $MinValue $range $TypeTest

if [ $range = "InRange" ]; then
#Test write Min Value
sk.sh dth w $DTHpathTest/$TypeTest/$TypeTestRangeValue/ $MinValue
awReadValue=$(sk.sh dth r $DTHpathTest/$TypeTest/$TypeTestRangeValue/ value)
ResultTest "NumberInRange" "test DTH Write $DTHpathTest/$TypeTest/$TypeTestRangeValue/ MinValue=$MinValue and read is $awReadValue" $awReadValue $testvalue

#Test write Min Value
sk.sh dth w $DTHpathTest/$TypeTest/$TypeTestRangeValue/ $MaxValue
awReadValue=$(sk.sh dth r $DTHpathTest/$TypeTest/$TypeTestRangeValue/ value)
ResultTest "NumberInRange" "test DTH Write $DTHpathTest/$TypeTest/$TypeTestRangeValue/ MaxValue=$MaxValue and read is $awReadValue" $awReadValue $testvalue
fi

#Test Value between range
sk.sh dth w $DTHpathTest/$TypeTest/$TypeTestRangeValue/ $testvalue
awReadValue=$(sk.sh dth r $DTHpathTest/$TypeTest/$TypeTestRangeValue/ value)
case $range in
 InRange)	
 num="NumberInRange"
 ;;
 #Test Value between over range
 OutRange)
 num="NumberOutRange" 
 ;;
 esac
 ResultTest "$num" "test DTH Write $range $DTHpathTest/$TypeTest/$TypeTestRangeValue/ $testvalue and read is $awReadValue" $awReadValue $testvalue
done
done
done   
done

###########################################################
#Specific test Bitfield STRING FILE #######################
###########################################################

for TypeTest in $ListTestSpecificTable; do
echo "specific test $TypeTest" 
Titre "Reading Signed $TypeTest test DTH"


case $TypeTest in
BitField)

for TypeGroup in $BitFieldGroupNumber; do
Titre "Reading Unsigned $TypeGroup test DTH"

MinValue=$(sk.sh dth r $DTHpathTest/$TypeTest/$TypeGroup/ min)
MaxValue=$(sk.sh dth r $DTHpathTest/$TypeTest/$TypeGroup/ max) 

for range in $CaseRange; do
GenerateTestValue $MaxValue $MinValue $range $TypeGroup

if [ $range = "InRange" ]; then
#Test write Min Value
sk.sh dth w $DTHpathTest/$TypeTest/$TypeGroup/ $MinValue
awReadValue=$(sk.sh dth r $DTHpathTest/$TypeTest/$TypeGroup/ value)
ResultTest "NumberInRange" "test DTH Write $DTHpathTest/$TypeTest/$TypeGroup/ MinValue=$MinValue and read is $awReadValue" $awReadValue $testvalue

#Test write Min Value
sk.sh dth w $DTHpathTest/$TypeTest/$TypeGroup/ $MaxValue
awReadValue=$(sk.sh dth r $DTHpathTest/$TypeTest/$TypeGroup/ value)
ResultTest "NumberInRange" "test DTH Write $DTHpathTest/$TypeTest/$TypeGroup/ MaxValue=$MaxValue and read is $awReadValue" $awReadValue $testvalue
fi

sk.sh dth w $DTHpathTest/$TypeTest/$TypeGroup/ $testvalue
awReadValue=$(sk.sh dth r $DTHpathTest/$TypeTest/$TypeGroup/ value)
case $range in
 InRange)	
 num="NumberInRange"
 ;;
 #Test Value between over range
 OutRange)
 num="NumberOutRange" 
 ;;
 esac
 ResultTest "$num" "test DTH Write $DTHpathTest/$TypeTest/$TypeGroup/ $testvalue and read is $awReadValue" $awReadValue $testvalue
done
done  
;;

String)

for TypeString in $StringType; do
case $TypeString in 
EMPTYSTRING)
testvalue=''
sk.sh dth w $DTHpathTest/$TypeTest/$TypeString/ $testvalue
awReadValue=$(sk.sh dth r $DTHpathTest/$TypeTest/$TypeString/ value)  
;;
ONEBYTESTRING)
testvalue='u'
sk.sh dth w $DTHpathTest/$TypeTest/$TypeString/ $testvalue
awReadValue=$(sk.sh dth r $DTHpathTest/$TypeTest/$TypeString/ value)
;;

SIXTEENBYTESTRING)
testvalue="longueChaine"
sk.sh dth w $DTHpathTest/$TypeTest/$TypeString/ $testvalue
awReadValue=$(sk.sh dth r $DTHpathTest/$TypeTest/$TypeString/ value)
;;
esac
ResultTest "String" "test DTH Write $DTHpathTest/$TypeTest/$TypeString/ $testvalue and read is $awReadValue" $awReadValue $testvalue
done
;;

esac
done