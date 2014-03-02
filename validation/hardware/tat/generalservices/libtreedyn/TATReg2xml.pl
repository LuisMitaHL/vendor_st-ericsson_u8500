#################################################################################################
# TATReg2xml.pl
# Convert an header file which contain register parameters into a .xml file
# Author: Guillaume Voleau, Dina Sokolova
# Date: 24-March-2009
#http://gnb301368.gnb.st.com/Utilisation%20du%20generateur%20de%20code%20I2C.htm
#perl TATReg2Ini.pl <ICName> 	 1 inifile per 1 section
#perl TATReg2Ini.pl all    	 15 inifile per 15 section 
#################################################################################################
	
# Global Variables
use Switch;
no strict 'vars';
use Getopt::Long;

$DEVICE = "";
$DEVICEVERSION = "";
$OUTFILE = "";
$INPFILE = "";
$MODULE = "";

$registerSize = 0;
$maxvalueSize = 0;

$read=1;

$doall=0;
$onedesfile=0;

$a = 0;         #used to inc bit description array
$b = 0;			#used to inc typedef array
$item = 0;		#used to inc item array (bit + reg)
$itemmax = 0;
$itemold = 0;

$bmax=0;
$bold=0;

$section=0;		#used to save multiple input file

$opt_debug = 0;

sub debug_print {
	my ($str) = @_;
	
	if ( $opt_debug == 1 ) {
		print $str;
	}
}

if ($#ARGV == -1)
{
	&usage;
	die "program error";
}

Getopt::Long::Configure( "bundling_override" );
$result = GetOptions ("discard=s" => \$discard_reg_param,
					  "ab=s" => \$ab_chip,
					  "path=s" => \$path,
					  "version=s" => \$version,
					  "db=s" => \$db_chip,
					  "header=s" => \$OUTPUTPATH,
					  "debug" => \$opt_debug);

if (defined($db_chip))
{
	if (!defined($version))
	{
		&usage;
		die "program error";
	}
	else
	{
		$DEVICE = "DB";
		$INPUTPATH=$path;
		$DEVICENAME = $db_chip;
		$INTPUTPATH="";
		$DEVICEVERSION = "$version";
		$registerSize = 32;
		$maxvalueSize = 4294967295;
		@discard_reg = split(/,/,$discard_reg_param);
	}
}
elsif (defined("$ab_chip"))
{
	if (!defined($path))
	{
		&usage;
		die "program error";
	}
	else
	{
		$OUTPUTPATH="xmlfiles";
		$INPUTPATH=$path;
		$ICName = "all";
		$DEVICE = "AB";
		$DEVICENAME = $ab_chip;
		$registerSize = 8;
		$maxvalueSize = 255;
	}
}
else
{
	&usage;
	die "program error";
}

print "Prepare $DEVICE $DEVICENAME $DEVICEVERSION registers...\n";

##########################  read input files   #########################################################
while ($read)
{	
	$k = 0;				#used to inc element of BIT_VALUE array
	
	$start_read_BIT_VAL=0;  #used to start reading BIT value
	$start_read_BIT_NAME=0; #used to start reading BIT name
	
	%BIT_ARRAY= ();		#array to save parameter from input file: index = 1 field bit
	%BIT_TMP_ARRAY= ();
	%BIT_ENUM_ARRAY=();	#array to save bit value from input file: index = 1 enum
	%SECTION_ARRAY=();	#array to save bit value from input file: index = 1 section
	
	@BIT_VALUE =0;		#used to start reading BIT value
	@BIT_CLEAN = 0;  #used to delete empty descriptions
	$REG_PRE_ADDR=0;	#used to compare new reg found
	
	##########################  select input files   #########################################################
	
	if ($DEVICE eq "AB")
	{
		if (($ICName=~"all") || ($doall>0))
		{
			$doall++;
			switch ($doall) {	
					case 1 {$ICName="access";}
					case 2 {$ICName="adc";}
					case 3 {$ICName="audio";}
					case 4 {$ICName="charg";}
					case 5 {$ICName="ctrl";}
					case 6 {$ICName="gauge";}
					case 7 {$ICName="gpio";}
					case 8 {$ICName="int";}
					case 9 {$ICName="latch";}
					case 10 {$ICName="mint";}
					case 11 {$ICName="misc";}
					case 12 {$ICName="pwmo";}
					case 13 {$ICName="regu2";}
					case 14 {$ICName="regul";}
					case 15 {$ICName="rtc";}
					case 16 {$ICName="sim";}
					case 17 {$ICName="tvout";}
					case 18 {$ICName="usb"; $read =0;}
			}
		}
		else
		{
			$read =0;
		}
		
		switch ($ICName) {
	
			case "access"
			{
				$ICName = "ACCESS";
				$input_file_name = $INPUTPATH."/ab".$DEVICENAME."_Accessory_types.h";
				if (!$onedesfile) {$des_xml_file_name = $OUTPUTPATH."/AB".$DEVICENAME."_access.xml";}
				
			}
			case "adc"
			{
				$ICName = "ADC";
				$input_file_name = $INPUTPATH."/ab".$DEVICENAME."_ADC_types.h";
				if (!$onedesfile) {$des_xml_file_name = $OUTPUTPATH."/AB".$DEVICENAME."_adc.xml";}
			}
			case "audio"
			{
				$ICName = "AUDIO";
				$input_file_name = $INPUTPATH."/ab".$DEVICENAME."_Audio_types.h";
				if (!$onedesfile) {$des_xml_file_name = $OUTPUTPATH."/AB".$DEVICENAME."_audio.xml";}
			}
			case "charg"
			{
				$ICName = "CHARGER";
				$input_file_name = $INPUTPATH."/ab".$DEVICENAME."_Charger_types.h";
				if (!$onedesfile) {$des_xml_file_name = $OUTPUTPATH."/AB".$DEVICENAME."_charger.xml";}
			}
			case "ctrl"		
			{
				$ICName = "CTRL";
				$input_file_name = $INPUTPATH."/ab".$DEVICENAME."_Control_types.h";
				if (!$onedesfile) {$des_xml_file_name = $OUTPUTPATH."/AB".$DEVICENAME."_ctrl.xml";}
			}
			case "gauge"
			{
				$ICName = "GAUGE";
				$input_file_name = $INPUTPATH."/ab".$DEVICENAME."_Gauge_types.h";
				if (!$onedesfile) {$des_xml_file_name = $OUTPUTPATH."/AB".$DEVICENAME."_gauge.xml";}
			}
			case "gpio"
			{
				$ICName = "GPIO";
				$input_file_name = $INPUTPATH."/ab".$DEVICENAME."_GPIO_types.h";
				if (!$onedesfile) {$des_xml_file_name = $OUTPUTPATH."/AB".$DEVICENAME."_gpio.xml";}
			}
			case "int"
			{
				$ICName = "INT";
				$input_file_name = $INPUTPATH."/ab".$DEVICENAME."_Interrupt_types.h";
				if (!$onedesfile) {$des_xml_file_name = $OUTPUTPATH."/AB".$DEVICENAME."_int.xml";}
			}
			case "latch"
			{
				$ICName = "LATCH";
				$input_file_name = $INPUTPATH."/ab".$DEVICENAME."_Latch_types.h";
				if (!$onedesfile) {$des_xml_file_name = $OUTPUTPATH."/AB".$DEVICENAME."_latch.xml";}
			}
			case "mint"
			{
				$ICName = "M_INT";
				$input_file_name = $INPUTPATH."/ab".$DEVICENAME."_MaskInterrupt_types.h";
				if (!$onedesfile) {$des_xml_file_name = $OUTPUTPATH."/AB".$DEVICENAME."_m_int.xml";}
			}
			case "misc"
			{
				$ICName = "MISC";
				$input_file_name = $INPUTPATH."/ab".$DEVICENAME."_Miscellanous_types.h";
				if (!$onedesfile) {$des_xml_file_name = $OUTPUTPATH."/AB".$DEVICENAME."_misc.xml";}
			}
			case "pwmo"
			{
				$ICName = "PWMO";
				$input_file_name = $INPUTPATH."/ab".$DEVICENAME."_PwmOut_types.h";
				if (!$onedesfile) {$des_xml_file_name = $OUTPUTPATH."/AB".$DEVICENAME."_pwmo.xml";}
			}
			case "regu2"
			{
				$ICName = "REGU2";
				$input_file_name = $INPUTPATH."/ab".$DEVICENAME."_Regu2_types.h";
				if (!$onedesfile) {$des_xml_file_name = $OUTPUTPATH."/AB".$DEVICENAME."_regu2.xml";}
			}
			case "regul"
			{
				$ICName = "REGUL";
				$input_file_name = $INPUTPATH."/ab".$DEVICENAME."_Regul_types.h";
				if (!$onedesfile) {$des_xml_file_name = $OUTPUTPATH."/AB".$DEVICENAME."_regul.xml";}
			}
			case "rtc"
			{
				$ICName = "RTC";
				$input_file_name = $INPUTPATH."/ab".$DEVICENAME."_RTC_types.h";
				if (!$onedesfile) {$des_xml_file_name = $OUTPUTPATH."/AB".$DEVICENAME."_rtc.xml";}
			}
			case "sim"
			{
				$ICName = "SIM";
				$input_file_name = $INPUTPATH."/ab".$DEVICENAME."_SIM_types.h";
				if (!$onedesfile) {$des_xml_file_name = $OUTPUTPATH."/AB".$DEVICENAME."_sim.xml";}
			}
			case "tvout"
			{
				if ($DEVICENAME eq "8500")
				{
					$ICName = "TVOUT";
					$input_file_name = $INPUTPATH."/ab".$DEVICENAME."_TVOut_types.h";
					if (!$onedesfile) {$des_xml_file_name = $OUTPUTPATH."/AB".$DEVICENAME."_tvout.xml";}
				}
				else
				{
					printf("TVOUT not supported\n");
				}
			}
			case "usb"
			{
				$ICName = "USB";
				$input_file_name = $INPUTPATH."/ab".$DEVICENAME."_USB_types.h";
				if (!$onedesfile) {$des_xml_file_name = $OUTPUTPATH."/AB".$DEVICENAME."_usb.xml";}
			}
			else 
			{
				printf("error ON IC_Name=%s\n", $ICName);	
			} 	
		}
	
	}
	if ($DEVICE eq "DB")
	{
		$ICName = "allDB";
		if (($DEVICEVERSION eq "V1") || ($DEVICEVERSION eq "V2"))
		{
			if ($INTPUTPATH eq "")
			{
				$input_file_name = "DB".$DEVICENAME."_".$DEVICEVERSION.".per";
				$des_xml_file_name = "DB".$DEVICENAME."_".$DEVICEVERSION.".xml";
			}
			else
			{
				$input_file_name = $INPUTPATH."/DB".$DEVICENAME."_".$DEVICEVERSION.".per";
				$des_xml_file_name = $INPUTPATH."/DB".$DEVICENAME."_".$DEVICEVERSION.".xml";
			}
		}
		elsif ($DEVICENAME eq "x600")
		{
			if ($INTPUTPATH eq "")
			{
				$input_file_name = "x600.per";
				$des_xml_file_name = "x600.xml";
			}
			else
			{
				$input_file_name = "$INPUTPATH/x600.per";
				$des_xml_file_name = "$INPUTPATH/x600.xml";
			}
		}
		$read = 0;
	}
	printf("IC_Name=%s, inputFile=%s, outputFile=%s\n", $ICName, $input_file_name , $des_xml_file_name);
	#my @pwd = qx{pwd};
	#print @pwd;
	
	##########################  parse input files   #########################################################
	$check_bit_name = 0;
	if ($DEVICE eq "AB")
	{
		#If the input file is readeable
		if (-r ($input_file_name )) 
		{
			open INPFILE, $input_file_name  or die "Cannot open $input_include_file_name  : $!\n";	
			
			#For each line of the file
			foreach $inline (<INPFILE>)
			{	             		
				# remove the newline from $line.
				chomp($inline);          	
				$inline =~ s/\r//g;
				#print "XXXXX $inline\n";
				#$ppp = <STDIN>;
				
				###############################
				#check bit_name if it is in a new line
				if (($check_bit_name == 1) && ($inline =~ /^(\/\/)(\s+)(\w+)/))
				{
					$BIT_NAME = $3;
					${"BIT_ARRAY_$item"}{bit_name}=$BIT_NAME;
					$item++;
					$check_bit_name = 0;
					#print "Checked bit name on new line\n";
					next;	
				}
				
				#start typedef num
				if ($inline =~ /^(typedef enum )(.*)(\s+{)/)
				{
					$TYPEDEFTMP = $2;
					#print "found type def enum : $TYPEDEFTMP!\n";
					$start_read_BIT_ENUM = 1;
					next;
				}
				
				# end of typedef enum
				if (($inline =~ /^(} )(\w+)(_T ;)/) && ($start_read_BIT_ENUM == 1))
				{
					$start_read_BIT_ENUM = 0;
					$size_max = scalar @BIT_VALUE;
					${"BIT_ENUM_ARRAY_$b"}{$bit_size_value}=$size_max;	 #store number of elements of enum
					#print "was BIT_ENUM_ARRAY_$b(bit_size_value) = $size_max\n";
					for ($bitvalue=0; $bitvalue<$size_max ; $bitvalue++)
					{
						${"BIT_ENUM_ARRAY_$b"}{$bitvalue}=@BIT_VALUE[$bitvalue];
					}
					${"BIT_ENUM_ARRAY_$b"}{bit_name}=$TYPEDEFTMP;
					#print "was TYPEDEF_NAME_TMP = $TYPEDEFTMP\n";
					$TYPEDEFTMP = ${"BIT_ENUM_ARRAY_$b"}{bit_name};
					#print "was TYPEDEF_NAME_TMP = $TYPEDEFTMP\n";
					@BIT_VALUE=0;
					$k=0;
					$b++;
					next;
				}
				
				#read the content of typedef enum 
				if ($start_read_BIT_ENUM == 1)
				{
					$inline=~s/\s+//g;			
					$inline=~s/,//g;			
					@BIT_VALUE[$k]=$inline;
					#print "store bit enum value = $inline\n";
					$k++;
					next;
				}
				
				#store register description from comment
				if ($inline =~ /^(\/\/)(\s+)(Register)(\s+)(.*)(0x)(.*)(,)(\s+)(Bits)(.*)(:)(.*)(,)(\s+)(typedef)(\s+)(.*)/)
				{ 
					$REGISTER_NAME=$5;
					$REGISTER_ADDR=$7;
					$BIT_NUM_MIN=$11; 
					$BIT_NUM_MAX=$13;   
					$BIT_NAME=$18;
					${"BIT_ARRAY_$item"}{bit_num_min}=$BIT_NUM_MIN;
					${"BIT_ARRAY_$item"}{bit_num_max}=$BIT_NUM_MAX;
					${"BIT_ARRAY_$item"}{reg_addr}=$REGISTER_ADDR;
					$REGISTER_NAME =~ s/\s+$//;
					${"BIT_ARRAY_$item"}{reg_name}=lc($REGISTER_NAME);
					${"BIT_ARRAY_$item"}{bit_name}=$BIT_NAME;
					if ($BIT_NAME eq "")
					{
						$check_bit_name = 1;
					}
					else
					{
						$REGISTER_NAME = ${"BIT_ARRAY_$item"}{reg_name};
				  	$item++;
				  }
					#print "BIT_NUM_MIN=$BIT_NUM_MIN, BIT_NUM_MAX=$BIT_NUM_MAX, REGISTER_ADDR=$REGISTER_ADDR, REGISTER_NAME=$REGISTER_NAME, BIT_NAME=$BIT_NAME\n";
					next;
				}
				
				#maybe store register even if they have no bit associated
				if (($inline =~ /^(#define )(.*)(_REG 0x)(.*)/) && (1 == 0))
				{
					$REGISTER_ADDR=$4;
					$REGISTER_NAME=$2;
					$REGISTER_NAME=~s/\_//g;
					$REGISTER_NAME = lc($REGISTER_NAME);
					if (length($REGISTER_ADDR) == 3)
					{
						#print "add 0 for $REGISTER_NAME$ at $REGISTER_ADDR";
						$REGISTER_ADDR = "0".$REGISTER_ADDR;
					}
					#print "found REG_ADDR : REG_NAME = $REGISTER_ADDR : $REGISTER_NAME\n";
					if ($item == 0)
					{
						#print "first register, so we add it\n";
						${"BIT_ARRAY_$item"}{reg_addr}=$REGISTER_ADDR;
						${"BIT_ARRAY_$item"}{reg_name}=lc($REGISTER_NAME);
						${"BIT_ARRAY_$item"}{bit_name}="empty";
						${"BIT_ARRAY_$item"}{bit_num_min}=0;
						$item++;
					}
					else 
					{
						$previous = $item-1;
						if (lc($REGISTER_NAME) ne ${"BIT_ARRAY_$previous"}{reg_name})
						{
							#print "not first register, but new one, so we add it\n";
							${"BIT_ARRAY_$item"}{reg_addr}=$REGISTER_ADDR;
							${"BIT_ARRAY_$item"}{reg_name}=lc($REGISTER_NAME);
							${"BIT_ARRAY_$item"}{bit_name}="empty";
							${"BIT_ARRAY_$item"}{bit_num_min}=0;
							$item++;
						}
					}
					next;
				}	
				###############################
			}
	
			$itemfortri = $itemold;
			$itemmax = $item-$itemold ;
			$bmax = $b-$bold;
			$bold = $b;
			close INPFILE;
			debug_print "itemmax = $itemmax, itemfortri = $itemfortri, item = $item, itemold = $itemold\n";
	
			for ($ij = $itemold; $ij < $item; $ij++) 
			{
				$REG_NAME = ${"BIT_ARRAY_$ij"}{reg_name};
				$REG_ADDR = ${"BIT_ARRAY_$ij"}{reg_addr};
				$BIT_NUM_MIN = ${"BIT_ARRAY_$ij"}{bit_num_min};	
			  	$BIT_NAME = ${"BIT_ARRAY_$ij"}{bit_name};	
				$BIT_NUM_MAX = ${"BIT_ARRAY_$ij"}{bit_num_max};	
				debug_print "$ij: REG_NAME = $REG_NAME, REG_ADDR = $REG_ADDR : BIT_NUM_MIN = $BIT_NUM_MIN, BIT_NUM_MAX = $BIT_NUM_MAX, BIT_NAME = $BIT_NAME\n";
			}
		
			#delete empty and double registers for current section
			#empty regs could appear if #define _REG is before defines of bitfileds
			#double reg could appear if there is comment for 0:7 bits and then for different bitfields
			if ($itemmax > 1)
			{
				#print "check for empty or double\n";
				$checknext = 0;
				$checknextempty = 0;
				$deletecnt = 0;
				for ($ii = $itemold; $ii < $item; $ii++)
				{
					$REG_NAME = ${"BIT_ARRAY_$ii"}{reg_name};
					$REG_ADDR = ${"BIT_ARRAY_$ii"}{reg_addr};
					$BIT_NUM_MIN = ${"BIT_ARRAY_$ii"}{bit_num_min};	
		  			$BIT_NAME = ${"BIT_ARRAY_$ii"}{bit_name};	
					$BIT_NUM_MAX = ${"BIT_ARRAY_$ii"}{bit_num_max};	
		  			debug_print "will check $ii : REG_NAME = $REG_NAME at $REG_ADDR with BIT_NUM_MIN=$BIT_NUM_MIN BIT_NUM_MAX=$BIT_NUM_MAX and BIT_NAME = $BIT_NAME with checknext=$checknext\n";
		  			if ($checknext == 1)
					{
						$checknext = 0;
						if ($REG_ADDR eq $PRE_REG_ADDR)
						{
							@BIT_CLEAN[$deletecnt] = $ii-1;
							debug_print "should delete previous item : @BIT_CLEAN[$deletecnt]\n";
							$deletecnt++;
						}
						elsif ($checknextempty == 1)
						{
							$checknextempty = 0;
							@BIT_CLEAN[$deletecnt] = $ii-1;
							debug_print "should delete previous item : @BIT_CLEAN[$deletecnt]\n";
							$deletecnt++;
						}
					}
					elsif ( ($BIT_NUM_MIN == 0) && (($BIT_NUM_MAX == 7) || ($BIT_NAME eq "empty")))
		  		{
		  			$checknext = 1;
					$checknextempty = 1;
		  			$PRE_REG_ADDR = $REG_ADDR;
		  			debug_print "found $ii : REG_NAME = $REG_NAME at $REG_ADDR with BIT_NUM_MIN=$BIT_NUM_MIN BIT_NUM_MAX=$BIT_NUM_MAX and BIT_NAME = $BIT_NAME, so PRE_REG_ADDR = $PRE_REG_ADDR\n";
		  		}
				}
			}
			debug_print "before delete : itemmax = $itemmax, itemfortri = $itemfortri, item = $item, itemold = $itemold\n";
			debug_print "should delete $deletecnt items : ";
			for ($jj = 0; $jj < $deletecnt; $jj++)
			{
				$delit = @BIT_CLEAN[$jj];
				debug_print "$delit, ";
			}
			debug_print "\n";
			for ($jj = $deletecnt-1; $jj >= 0; $jj--)
			{
				debug_print "will delete @BIT_CLEAN[$jj]\n";
				for ($jd = @BIT_CLEAN[$jj]; $jd < ($item - 1); $jd++)
				{
					$jc = $jd+1;
					${"BIT_ARRAY_$jd"}{reg_name} = ${"BIT_ARRAY_$jc"}{reg_name};
					${"BIT_ARRAY_$jd"}{reg_addr} = ${"BIT_ARRAY_$jc"}{reg_addr};
					${"BIT_ARRAY_$jd"}{bit_num_min} = ${"BIT_ARRAY_$jc"}{bit_num_min};;
					${"BIT_ARRAY_$jd"}{bit_num_max} = ${"BIT_ARRAY_$jc"}{bit_num_max};
					${"BIT_ARRAY_$jd"}{bit_name} = ${"BIT_ARRAY_$jc"}{bit_name};
				}
			}
			$itemmax = $itemmax - $deletecnt;
			$item = $item - $deletecnt;
			debug_print "after delete : itemmax = $itemmax, itemfortri = $itemfortri, item = $item, itemold = $itemold\n";
			@BIT_CLEAN = 0;
			$deletecnt = 0;
			for ($ii = $itemold; $ii < $item; $ii++)
			{
				$REG_NAME = ${"BIT_ARRAY_$ii"}{reg_name};
				$REG_ADDR = ${"BIT_ARRAY_$ii"}{reg_addr};
				$BIT_NUM_MIN = ${"BIT_ARRAY_$ii"}{bit_num_min};	
		  		$BIT_NAME = ${"BIT_ARRAY_$ii"}{bit_name};	
				$BIT_NUM_MAX = ${"BIT_ARRAY_$ii"}{bit_num_max};	
		  		debug_print "$ii : REG_NAME = $REG_NAME at $REG_ADDR with BIT_NUM_MIN=$BIT_NUM_MIN BIT_NUM_MAX=$BIT_NUM_MAX and BIT_NAME = $BIT_NAME\n";
			}
			#redo the order of elements
			if ($itemmax > 1)
			{
				#for current section we have elements from $itemfortri to $item-1 = $itemmax normally
				#print "there are > 1 elements, maybe should reorder\n";
				$checkregname = $itemfortri;
				while ($checkregname < $item)
				{
					$REG_NAME_cur = ${"BIT_ARRAY_$checkregname"}{reg_name};
					debug_print "for REG_NAME_cur = $REG_NAME_cur\n";
					$endcurreg = $checkregname; #start cur reg is stored in checkregname
					$REG_NAME_e = ${"BIT_ARRAY_$endcurreg"}{reg_name};
					while (($REG_NAME_cur eq $REG_NAME_e) && ($endcurreg < $item))
					{
						#still the same register
						$endcurreg++;
						debug_print "checkregname = $checkregname, endcurreg = $endcurreg, REG_NAME_e = $REG_NAME_e\n";
						$REG_NAME_e = ${"BIT_ARRAY_$endcurreg"}{reg_name};
					}
					$endcurreg--;
					#cur register is from checkregname to endcurreg included
					$sizereg = $endcurreg - $checkregname;
					if ($sizereg > 0)
					{
						debug_print "should check for tri : $REG_NAME_cur from $checkregname to $endcurreg\n";
						if (${"BIT_ARRAY_$checkregname"}{bit_num_min}	> ${"BIT_ARRAY_$endcurreg"}{bit_num_max})
						{
							debug_print "should tri for size=$sizereg\n";
							for ($i=0; $i <= $sizereg; $i++)
							{
								#copy in a temp buffer
								$srcidx = $checkregname + $i;
								${"BIT_TMP_ARRAY_$i"}{bit_num_min} = ${"BIT_ARRAY_$srcidx"}{bit_num_min};
								${"BIT_TMP_ARRAY_$i"}{bit_num_max} = ${"BIT_ARRAY_$srcidx"}{bit_num_max};
								${"BIT_TMP_ARRAY_$i"}{bit_name} = ${"BIT_ARRAY_$srcidx"}{bit_name};
							}
							for ($i=0; $i <= $sizereg; $i++)
							{
								#inverse copy from temp buffer
								$srcidx = $checkregname + $i;
								$invi = $sizereg - $i;
								debug_print "$REG_NAME_cur : exchange $srcidx with $invi\n";
								${"BIT_ARRAY_$srcidx"}{bit_num_min} = ${"BIT_TMP_ARRAY_$invi"}{bit_num_min};
								${"BIT_ARRAY_$srcidx"}{bit_num_max} = ${"BIT_TMP_ARRAY_$invi"}{bit_num_max};
								${"BIT_ARRAY_$srcidx"}{bit_name} = ${"BIT_TMP_ARRAY_$invi"}{bit_name};
							}
						}
					}
					$checkregname = $endcurreg + 1;
					%BIT_TMP_ARRAY = ();
				}
			}
			debug_print "after reorder : itemmax = $itemmax, itemfortri = $itemfortri, item = $item, itemold = $itemold\n";
			for ($ii = $itemold; $ii < $item; $ii++)
			{
				$REG_NAME = ${"BIT_ARRAY_$ii"}{reg_name};
				$REG_ADDR = ${"BIT_ARRAY_$ii"}{reg_addr};
				$BIT_NUM_MIN = ${"BIT_ARRAY_$ii"}{bit_num_min};	
		  		$BIT_NAME = ${"BIT_ARRAY_$ii"}{bit_name};	
				$BIT_NUM_MAX = ${"BIT_ARRAY_$ii"}{bit_num_max};	
		  		debug_print "$ii : REG_NAME = $REG_NAME at $REG_ADDR with BIT_NUM_MIN=$BIT_NUM_MIN BIT_NUM_MAX=$BIT_NUM_MAX and BIT_NAME = $BIT_NAME\n";
			}
			${"SECTION_ARRAY_$section"}{icname}=$ICName;
			${"SECTION_ARRAY_$section"}{bit_max}=$itemmax;
			${"SECTION_ARRAY_$section"}{enum_max}=$bmax;
			${"SECTION_ARRAY_$section"}{outputfile} = "AB".$DEVICENAME.".xml";
			$itemold = $item;
			$section++;
		}
		else 
		{
			print "ERROR CAN NOT READ $input_file_name \n";
			$read = 0;
		}	
	}
	
	if ($DEVICE eq "DB")
	{
		$startBits = 0;
		if (-r ($input_file_name )) 
		{
			open INPFILE, $input_file_name  or die "Cannot open $input_include_file_name  : $!\n";
			# nexttree = 1 => just read ;====, wait for tree name
			# nexttree = 2 => just read tree name, wait for ;==== and after wait for bits name
			# nexttree = 3 => tree excluded;==== wait for another one
			$nexttree = 0; 
			$section = 0;
			foreach $inline (<INPFILE>)
			{	
				# remove the newline from $line.
				chomp($inline);
				#print "XXXXX $inline";
				#$sss = <STDIN>;
				if ($startBits == 0)
				{      
					#print "No bits to read, nexttree = $nexttree\n";
					if ($nexttree == 1)
					{
						#print "In the tree\n";
						if ($inline =~ m/instance/ )
						{
							$nexttree = 2;
						}
						else 
						{
							#print "In the tree without instance\n";
							$inline =~ s/tree \"//;
							$inline =~ s/\"//;
							$inline =~ s/\s+//;
							$ICName = $inline;
							$nexttree = 2;
							for ($i=0;$i<=$#discard_reg;$i++) {
								if ($ICName =~ m/$discard_reg[$i]/ )
								{
									$nexttree = 3;
									last;
								}
							}
							#print "retrieved Name = $ICName\n";
						}
					}
					elsif (($inline =~ m/(;=======)/ ) || ($inline =~ m/(;\*\*\*\*\*\*\*)/ ))
						{
							#print "TAG !\n";
							if ($nexttree == 2) #closing tag
							{
								$nexttree = 0;
								$startBits = 1;
								#print "closing TAG !\n";
							}
							elsif ($nexttree == 3)
							{
								$startBits = 1;
							}
							else #entering tag
							{
								$nexttree = 1;
								$startBits = 0;
								#print "entering TAG !\n";
							}
						}
				}
				else #read bits
				{
					#print "bits to read\n";
					if ($nexttree == 3)
					{
						if (($inline =~ m/(;=======)/ ) || ($inline =~ m/(;\*\*\*\*\*\*\*)/ ))
							{
								#print "next tree!\n\n";
								$nexttree = 1;
								$startBits = 0;
								$itemmax = $item-$itemold ;
								$itemold = $item;
								${"SECTION_ARRAY_$section"}{icname}=$ICName;
							  	${"SECTION_ARRAY_$section"}{outputfile}=$des_xml_file_name ;
							  	${"SECTION_ARRAY_$section"}{bit_max}=$itemmax;
								$section++;
							}
					}
					else
					{
						if (($inline =~ m/(;=======)/ ) || ($inline =~ m/(;\*\*\*\*\*\*\*)/ ))
							{
								#print "next tree!\n\n";
								$nexttree = 1;
								$startBits = 0;
								$itemmax = $item-$itemold ;
								$itemold = $item;
								${"SECTION_ARRAY_$section"}{icname}=$ICName;
								${"SECTION_ARRAY_$section"}{outputfile}=$des_xml_file_name ;
								${"SECTION_ARRAY_$section"}{bit_max}=$itemmax;
								$section++;
							}
						else
						{
							if (($inline =~ /(base sd:0x)(\w+)/) ||
							    ($inline =~ /(base edap:0x)(\w+)/) ||
							    ($inline =~ /(base eapb:0x)(\w+)/)	)
							{
								$REGISTER_ADDR=$2;
							}
							if ($inline =~ /(tree \")(\w+)/)
							{
									$REGISTER_NAME=$2;
							}
							if ($inline =~ m/rgroup/)
							{
								$REGISTER_ACCESS=1;
							}
							elsif ($inline =~ m/wgroup/)
							{
								$REGISTER_ACCESS=2;
							}
							elsif ($inline =~ m/group/)
							{
								$REGISTER_ACCESS=0;
							}
							if ($inline =~ /(bitfld.long 0 )(\w+)(.)/)
							{
								${"BIT_ARRAY_$item"}{reg_addr}=$REGISTER_ADDR;
								${"BIT_ARRAY_$item"}{reg_name}=$REGISTER_NAME;
								${"BIT_ARRAY_$item"}{reg_access}=$REGISTER_ACCESS;

								$BIT_NUM_MIN = $2;
								${"BIT_ARRAY_$item"}{bit_num_min}=$BIT_NUM_MIN;
								$inline = substr($inline, $+[0]);
								#print "$inline\n";
								if ($inline =~ /( \" )([\w|\W]+)( \" )/)
								{
									$BIT_NUM_MAX = $BIT_NUM_MIN;
									$BIT_NAME = $2;
									$BIT_NAME =~ s/\[/\_/;
									$BIT_NAME =~ s/\]/\_/;
									#print "$BIT_NAME\n";
									${"BIT_ARRAY_$item"}{bit_num_max}=$BIT_NUM_MAX;
									${"BIT_ARRAY_$item"}{bit_name}=$BIT_NAME;
									#$inline = substr($inline, $+[0]);
								}
								if ($inline =~ /(--)(\w+)(. \" )([\w|\W]+)( \"  \" \")/)
								{
									$BIT_NUM_MAX = $2;
									$BIT_NAME = $4;
									$BIT_NAME =~ s/\[/\_/;
									$BIT_NAME =~ s/\]/\_/;
									${"BIT_ARRAY_$item"}{bit_num_max}=$BIT_NUM_MAX;
									${"BIT_ARRAY_$item"}{bit_name}=$BIT_NAME;
									#$inline = substr($inline, $+[0]);
								}
								#print "REGISTER_ADDR = $REGISTER_ADDR , REGISTER_NAME = $REGISTER_NAME , BIT_NUM_MIN = $BIT_NUM_MIN , BIT_NUM_MAX = $BIT_NUM_MAX, BIT_NAME = $BIT_NAME \n";
								$item++;
							}
						}
					}
				}
			}
			close INPFILE;
			$itemmax = $item-$itemold ;
			$itemold = $item;
			${"SECTION_ARRAY_$section"}{icname}=$ICName;
			${"SECTION_ARRAY_$section"}{outputfile}=$des_xml_file_name ;
			${"SECTION_ARRAY_$section"}{bit_max}=$itemmax;
			$section++; #last section
		}
		else 
		{
			print "ERROR CAN NOT READ $input_file_name \n";
			$read = 0;
		}
	}
}  # //while ($read)//	

#################### PRINT OUTPUT file ###########################################################
$sectionmax=$section;
print "number of section : $sectionmax\n";
$item = 0;

$e = 0; 		#used to inc [register] array
			
$item = 0; 		#used to inc field array
$a = 0;         #used to inc bit description array
$b = 0;			#used to inc typedef array
$c = 0;			#used to inc reg array
$enumstop = 0;	    #enum max 
$size_bitfield = 0;
$fileopened = 0;
$DTH_PATH = "";

if (1 == 0)
{
	print "=========================\n";
	for ($section=0; $section<$sectionmax; $section++)
	{
		$ICName = ${"SECTION_ARRAY_$section"}{icname};
		$enummax =${"SECTION_ARRAY_$section"}{enum_max};
		$itemmax = ${"SECTION_ARRAY_$section"}{bit_max};
		$itemmax = $itemmax + $item;
		print "ICName = $ICName : itemmax = $itemmax, enummax = $enummax\n";
		for ($item; $item<$itemmax; $item++) 
		{
			$REG_NAME = ${"BIT_ARRAY_$item"}{reg_name};
			$REG_ADDR = ${"BIT_ARRAY_$item"}{reg_addr};
			$BIT_NUM_MIN = ${"BIT_ARRAY_$item"}{bit_num_min};	
		  $BIT_NAME = ${"BIT_ARRAY_$item"}{bit_name};	
			$BIT_NUM_MAX = ${"BIT_ARRAY_$item"}{bit_num_max};	
			print "$item : REG_NAME = $REG_NAME, REG_ADDR = $REG_ADDR : BIT_NUM_MIN = $BIT_NUM_MIN, BIT_NAME = $BIT_NAME, BIT_NUM_MAX = $BIT_NUM_MAX\n";
		}
		for ($b;  $b < $enummax; $b++)
		{
			$sizeT = ${"BIT_ENUM_ARRAY_$b"}{$bit_size_value};
			$nameT = ${"BIT_ENUM_ARRAY_$b"}{bit_name};
			print "enum for size=$sizeT name = $nameT\n";
			for ($bitvalue = 0; $bitvalue < $sizeT; $bitvalue++)
			{
				$valT = ${"BIT_ENUM_ARRAY_$b"}{$bitvalue};
				#print "$valT, ";
			}
			#print "\n";
		}
	}
	$item = 0;
	$b = 0;
}


$section=0;
if (($DEVICE eq "DB") || ($DEVICE eq "AB"))
{
	if (($DEVICE eq "DB") && (defined($OUTPUTPATH)))
	{
		open OUTFILE_HEADER, '>'.$OUTPUTPATH  or die "Cannot open :$!\n";
		print OUTFILE_HEADER "/* DB8500 Registers\n   Generated file. Do not edit !!! */\n\n";
	}
	$des_xml_file_name=${"SECTION_ARRAY_$section"}{outputfile}; 
	open OUTFILE, '>'.$des_xml_file_name or die "Cannot open $des_xml_file_name : $!\n";
	print OUTFILE "<?xml version=\"1.0\"?>\n" ;
	print OUTFILE "<dthapi>\n" ;
	$modulename = "tatl".lc($DEVICE);
	print OUTFILE "<module name=\"$modulename\">\n";
	&get_configs($DEVICE);
}
$closearg = 0;
$PRE_BIT_NUM_MAX = $registerSize-1;
$cnt = 0;
for ($section=0; $section<$sectionmax; $section++)
{
	$ICName = ${"SECTION_ARRAY_$section"}{icname};
	$DTH_PATH = "/DTH/$DEVICE/Registers/$ICName";
	#if ($DEVICE eq "AB")
	#{
	#	$closearg = 0;
	#	$des_xml_file_name=${"SECTION_ARRAY_$section"}{outputfile}; 
	#	open OUTFILE, '>'.$des_xml_file_name or die "Cannot open $des_xml_file_name : $!\n";
	#	print OUTFILE "<?xml version=\"1.0\"?>\n" ;
	#	print OUTFILE "<dthapi>\n" ;
	#	print OUTFILE "<module name=\"$ICName\"\n";
	#}
	$itemmax = ${"SECTION_ARRAY_$section"}{bit_max};
	$itemmax = $itemmax + $item;
	$enumstart=$enumstop;
	$enumstop=(${"SECTION_ARRAY_$section"}{enum_max})+$enumstart;	
	#print "for section $ICName : itemmax = $itemmax, enumstart = $enumstart, enumstop = $enumstop\n";
		

	$REG_PRE_ADDR = XXXX;
		
	# for each field found
	for ($item; $item<$itemmax; $item++) 
	{
		#test if new register 	
	  $REG_NEW_ADDR= ${"BIT_ARRAY_$item"}{reg_addr};
	  if ($REG_NEW_ADDR ne $REG_PRE_ADDR)
	  {
	  	$REG_PRE_ADDR = $REG_NEW_ADDR;
	  	#print "$REG_NEW_ADDR : PRE_BIT_NUM_MAX = $PRE_BIT_NUM_MAX, size_bitfield = $size_bitfield\n";
	  	if ($PRE_BIT_NUM_MAX < ($registerSize-1))
	  	{
	  		#last (BIT_NUM_MAX..31] bits are reserved for previous register
			#print "$REG_NAME : reserved begin from $BIT_NUM_MAX+1 to $registerSize-1 : res_$cntbitres\n";
	  		&reserved($BIT_NUM_MAX+1, $registerSize-1);
	  	}
		$cntbitres = 0;
	  	if ($closearg == 1)
	  	{
	  		print OUTFILE "</argument>\n";
	  		print OUTFILE "</action>\n";
			#print "close arguments\n";
	  	}
	  	$closearg = 1;
	  	$REG_NAME = ${"BIT_ARRAY_$item"}{reg_name};
		$REG_ACCESS = ${"BIT_ARRAY_$item"}{reg_access};
		$hexaddr = "0x".$REG_NEW_ADDR;
		print OUTFILE "<action path=\"$DTH_PATH/$REG_NAME\" type=\"U8\" user_data=\"ACT_$REG_NEW_ADDR:$hexaddr";
		print OUTFILE "\" set=\"Dth$DEVICE";
		print OUTFILE "_set\" exec=\"Dth$DEVICE";
		print OUTFILE "_exec\">\n";
		print OUTFILE "<enum>\n";
		if (($DEVICE eq "DB") && (defined($OUTPUTPATH)))
		{
			$reg_name_header = $REG_NAME;
			$reg_name_header =~ s/^\d//;
			$reg_name_header =~ s/-/_/g;
			$ICName_header = $ICName;
			$ICName_header =~ s/-/_/g;
			print OUTFILE_HEADER "#define $ICName_header\_$reg_name_header\t$hexaddr\n";
		}
		#print "REG_NAME : REG_ACCESS , $REG_NAME : $REG_ACCESS\n";
		if ($REG_ACCESS == 1)
		{
			print OUTFILE "<enum_item value=\"$DEVICE";
			print OUTFILE "_READ:0\">READ</enum_item>\n";
		}
		elsif ($REG_ACCESS == 2)
		{
			print OUTFILE "<enum_item value=\"$DEVICE";
			print OUTFILE "_WRITE:1\">WRITE</enum_item>\n";
		}
		else
		{
			print OUTFILE "<enum_item value=\"$DEVICE";
			print OUTFILE "_READ:0\">READ</enum_item>\n";
			print OUTFILE "<enum_item value=\"$DEVICE";
			print OUTFILE "_WRITE:1\">WRITE</enum_item>\n";
		}				
		print OUTFILE "</enum>\n";
		print OUTFILE "<info>Shape: Vertical\\nHorizontal : 3\\nLabel Action:$REG_NAME:0x$REG_NEW_ADDR\\n</info>\n";
		#on start bitfield, main argument
		#print "bitfield containter for $REG_NAME\n";
		print OUTFILE "<argument path=\"$DTH_PATH/$REG_NAME/Fields\" type=\"U$registerSize";
		print OUTFILE "B\" user_data=\"OUT_VALUE\" get=\"Dth$DEVICE";
		print OUTFILE "_get\" set=\"Dth$DEVICE";
		print OUTFILE "_set\">\n";
		print OUTFILE "<range min=\"0\" max=\"$maxvalueSize\"/>\n";
		print OUTFILE "<info>Label Bitfield: $REG_NAME\\nElementperline : 4\\ndynamic Control : true</info>\n";
		$firstbitfield = 1;
		$BIT_NUM_MIN = ${"BIT_ARRAY_$item"}{bit_num_min};	
		$BIT_NAME = ${"BIT_ARRAY_$item"}{bit_name};	
		$BIT_NUM_MAX = ${"BIT_ARRAY_$item"}{bit_num_max};	
		$BIT_DES= ${"BIT_ARRAY_$item"}{bit_des};
		if ($BIT_NUM_MIN > 0)
		{
			#first [0..BIT_NUM_MIN) are reserved
			&reserved(0, $BIT_NUM_MIN-1);
			#print "$REG_NAME : reserved from 0 to $BIT_NUM_MIN-1 : res_$cntbitres\n";
			$cntbitres++; 
		}
		$PRE_BIT_NUM_MAX = $BIT_NUM_MIN;
	}
	  	$BIT_NUM_MIN = ${"BIT_ARRAY_$item"}{bit_num_min};	
	  	$BIT_NAME = ${"BIT_ARRAY_$item"}{bit_name};	
		$BIT_NUM_MAX = ${"BIT_ARRAY_$item"}{bit_num_max};	
		$BIT_DES= ${"BIT_ARRAY_$item"}{bit_des};
		#print "$REG_NAME : bitfield $BIT_NAME\n";
		if (($BIT_NUM_MIN - $PRE_BIT_NUM_MAX) > 1)
		{
		 	#bits (PRE_BIT_NUM_MAX..BIT_NUM_MIN) are reserved
		 	&reserved($PRE_BIT_NUM_MAX+1, $BIT_NUM_MIN-1);
			#print "$REG_NAME : reserved from $PRE_BIT_NUM_MAX+1 to $BIT_NUM_MIN-1 : res_$cntbitres\n";
			$cntbitres++;
		}
		$PRE_BIT_NUM_MAX = $BIT_NUM_MAX;
		$size_bitfield = $BIT_NUM_MAX - $BIT_NUM_MIN + 1;
		#print current bitfield
		if ($DEVICE eq "AB")
		{
		 	#search for enum
		  $b=$enumstart;	
			for ($b; $b<$enumstop; $b++) 
			{	
				$BIT_ENUM_NAME=${"BIT_ENUM_ARRAY_$b"}{bit_name};
				if ($BIT_NAME eq $BIT_ENUM_NAME)
				{
					@BIT_VALUE = 0;
					$size_max=${"BIT_ENUM_ARRAY_$b"}{$bit_size_value};
					for ($bitvalue=0;$bitvalue<$size_max ; $bitvalue++)
					{
						@BIT_VALUE[$bitvalue]=${"BIT_ENUM_ARRAY_$b"}{$bitvalue};
					}
					$enumfound =1;
					#print "enumfound for $REG_NAME: BIT_NAME: $BIT_NAME BIT_NUM_MIN $BIT_NUM_MIN    BIT_NUM_MAX= $BIT_NUM_MAX \n";
					last;
				}
			}
			$BIT_NUM_MIN =~ s/\s//;
			print OUTFILE "<argument path=\"$DTH_PATH/$REG_NAME/Fields/$BIT_NAME$BIT_NUM_MIN\" type=\"BITFIELD_";
			print OUTFILE "$size_bitfield";
			print OUTFILE "_bits\" user_data=\"BTF_$REG_NAME_$BIT_NAME\" get=\"Dth$DEVICE";
			print OUTFILE "_get\" set=\"Dth$DEVICE";
			print OUTFILE "_set\">\n";
			if($enumfound)
			{
				$enumfound = 0;
				
				print OUTFILE "<enum>\n";
				$sizeenum = scalar @BIT_VALUE;
				for ($ce = 0; $ce < $sizeenum; $ce++)
				{
					print OUTFILE "<enum_item value=\"$ce\">@BIT_VALUE[$ce]</enum_item>\n";
				}
				print OUTFILE "</enum>\n";
				if ($BIT_NUM_MAX eq $BIT_NUM_MIN)
				{ 
					print OUTFILE "<info>Label Bitfield:$BIT_NAME : bit $BIT_NUM_MAX</info>\n";
				}
				else
				{
					print OUTFILE "<info>Label Bitfield:$BIT_NAME : bits $BIT_NUM_MAX-$BIT_NUM_MIN</info>\n";
				}
			}
			elsif ($size_bitfield == 1) #enum for size=2
			{
				print OUTFILE "<enum>\n";
				print OUTFILE "<enum_item>0</enum_item>\n";
				print OUTFILE "<enum_item>1</enum_item>\n";
				print OUTFILE "</enum>\n";
				print OUTFILE "<info>Label Bitfield:$BIT_NAME : bit $BIT_NUM_MIN</info>\n";
			}
			else #enum for > 2
			{
				$maxRange = 2**$size_bitfield - 1;
		  		print OUTFILE "<range min=\"0\" max=\"$maxRange\" />\n";
		  		print OUTFILE "<info>Label Bitfield:$BIT_NAME : bits $BIT_NUM_MAX-$BIT_NUM_MIN\\nDisplay : spinbox</info>\n";
			}
		}
		if ($DEVICE eq "DB")
		{
			print OUTFILE "<argument path=\"$DTH_PATH/$REG_NAME/Fields/$BIT_NAME\" type=\"BITFIELD_";
			print OUTFILE "$size_bitfield";
			print OUTFILE "_bits\" user_data=\"BTF_$REG_NAME_$BIT_NAME\" get=\"Dth$DEVICE";
			print OUTFILE "_get\" set=\"Dth$DEVICE";
			print OUTFILE "_set\">\n";
			if ($size_bitfield == 1)
			{
				#print enum 0, 1
				print OUTFILE "<enum>\n";
				print OUTFILE "<enum_item>0</enum_item>\n";
				print OUTFILE "<enum_item>1</enum_item>\n";
				print OUTFILE "</enum>\n";
				print OUTFILE "<info>Label Bitfield:$BIT_NAME : bit $BIT_NUM_MIN</info>\n";
			}
			else
			{
			 	#print a range
			 	$maxRange = 2**$size_bitfield - 1;
			 	print OUTFILE "<range min=\"0\" max=\"$maxRange\" />\n";
			 	print OUTFILE "<info>Label Bitfield:$BIT_NAME : bits $BIT_NUM_MAX-$BIT_NUM_MIN\\nDisplay : spinbox</info>\n";

			}
		}
		print OUTFILE "</argument>\n";
	}
	if ($PRE_BIT_NUM_MAX < ($registerSize-1))
	{
	 	#very last (BIT_NUM_MAX..31/7] bits are reserved for last register
	 	&reserved($BIT_NUM_MAX+1, $registerSize-1);
		#print "$REG_NAME : reserved fin from $BIT_NUM_MAX+1 to $registerSize-1 : res_$cntbitres\n";
		$PRE_BIT_NUM_MAX = $registerSize-1;
	}
}
if (($DEVICE eq "DB") || ($DEVICE eq "AB"))
{
	print OUTFILE "</argument>\n";
	print OUTFILE "</action>\n";
	print OUTFILE "</module>\n";
	print OUTFILE "</dthapi>\n";
	close OUTFILE;
	if (($DEVICE eq "DB") && (defined($OUTPUTPATH)))
	{
		close OUTFILE_HEADER;
	}
}

##########################  sub programs   #########################################################

sub usage {
	print "TATReg2xml -db -version ED/V1/V2 <-discard REG1,REG2> <-debug>\n";
	print "TATReg2xml -ab ab85000_headers_path <-debug>\n";
}

sub reserved
{
	$sizebitfield = $_[1] - $_[0] + 1; # 2-5 => bitfield_4_bits
	$range = 2**$sizebitfield - 1;
	print OUTFILE "<argument path=\"$DTH_PATH/$REG_NAME/Fields/res_$cntbitres\" type=\"BITFIELD_$sizebitfield";
	print OUTFILE "_bits\" user_data=\"RES_$REG_NAME";
	print OUTFILE "_res_$cntbitres\">\n";
	print OUTFILE "<range min=\"0\" max=\"$range\"/>\n";
	if ($sizebitfield == 1)
	{
		print OUTFILE "<info>Label Bitfield:res bit $_[0]\\nDisplay : spinbox</info>\n";
	}
	else
	{
		print OUTFILE "<info>Label Bitfield:res bits $_[0]-$_[1]\\nDisplay : spinbox</info>\n";
	}
	print OUTFILE "</argument>\n";
}

sub get_configs
{
	$BASEBAND = $_[0];
	$startCopy = "end";
	if ($BASEBAND eq "AB") 
	{
		$config_file_name = "analogbaseband.xml";
	}
	else 
	{
		$config_file_name = "digitalbaseband.xml";
	}
	if (-r ($input_file_name )) 
	{
		open INPFILE, $config_file_name  or print "Cannot open $config_file_name : $!\n";	
		foreach $inline (<INPFILE>)
		{	        
			$inline =~ s/^\s+//;     		
			# remove the newline from $line.
			#chomp($inline);
			#print "XXXXX $inline\n";
			#print "startCopy = $startCopy\n";
			#$ppp = <STDIN>;
			if ($inline =~ m/module/ )
			{
				switch ($startCopy) {
					case "end" {$startCopy = "begin";}
					case "begin" {$startCopy = "end";}
				}			
			}
			else
			{
				if ($startCopy eq "begin")
				{
					print OUTFILE "$inline";
				}
			}
		}
	}
	else 
	{
		print "ERROR CAN NOT READ $input_file_name \n";
	}
}
