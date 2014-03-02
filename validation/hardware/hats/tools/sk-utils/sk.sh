#!/bin/sh

#	Copyright (C) ST-Ericsson 2011
#	This file provides a centralized way to access registers and perform boolean operations
#	Author:	gerald.baeza@stericsson.com
#	Modified: nicolas.perrin@stericsson.com

# Add new command in this list and implement it below (one function per command)
commands="db ab av i2c ddr dth bool"

company="ST-Ericsson"

print_usage()
{
	echo -e "              sk Command Manual"
	echo -e ""
	echo -e "\033[1mNAME\033[0m"
	echo -e "     sk - Swiss Knife, unified wrapper for hardware registers access and boolean operations"
	echo -e ""
	echo -e "\033[1mSYNOPSIS\033[0m"
	echo -e "     \033[1msk\033[0m [peripheral] [[h][instance]] [[r][w][add][sub][and][or][set][clr]] [address] [value(s)]"
	echo -e ""
	echo -e "\033[1mDESCRIPTION\033[0m"
	echo -e "   \033[1mList of supported values for \033[4mperipheral\033[1m\033[0m:"
	echo -e "      $commands"
	echo -e ""
	echo -e "\033[1mOPTIONS\033[0m"
	echo -e "   \033[1mGeneric program information\033[0m"
	echo -e "     Calling \033[1msk\033[0m with no option displays this manual page"
	echo -e ""
	echo -e "     \033[1mh\033[0m"
	echo -e "              Display the \033[1mh\033[0melp page for the given \033[4mperipheral\033[0m"
	echo -e ""
	echo -e "     \033[1minstance\033[0m"
	echo -e "              Used in case of multi \033[1minstance\033[0ms \033[4mperipheral\033[0m"
	echo -e ""
	echo -e "   \033[1mProgram operations\033[0m"
	echo -e "     Return values are displayed without any other information, on 32 bits and hexadecimal ('0x' prefix)"
	echo -e "     This allows direct variable allocation from \033[1msk\033[0m tool returned values (see \033[1mEXAMPLE\033[0m below)"
	echo -e ""	
	echo -e "     \033[1mr\033[0m"
	echo -e "              Perform a \033[1mr\033[0mead access to \033[4mperipheral\033[0m (\033[4minstance\033[0m number) at the given \033[4maddress\033[0m"
	echo -e ""
	echo -e "     \033[1mw\033[0m"
	echo -e "              Perform a \033[1mw\033[0mrite access of \033[4mvalue(s)\033[0m to \033[4mperipheral\033[0m (\033[4minstance\033[0m number) at the given \033[4maddress\033[0m"
	echo -e ""
	echo -e "     \033[1madd\033[0m, \033[1msub\033[0m, \033[1mand\033[0m, \033[1mor\033[0m, \033[1mset\033[0m, \033[1mclr\033[0m"
	echo -e "              Boolean operations, only available with \033[4mperipheral\033[0m=bool"
	echo -e ""
	echo -e "   \033[1mProgram parameters\033[0m"
	echo -e "     \033[1maddress\033[0m"
	echo -e "              Address or offset to access, given in decimal (no prefix) or hexadecimal ('0x' prefix)"
	echo -e ""
	echo -e "     \033[1mvalues\033[0m"
	echo -e "              Given in decimal (no prefix), hexadecimal ('0x' prefix) or binary ('b' prefix)"
	echo -e ""
	echo -e "\033[1mEXAMPLE\033[0m"
	echo -e "     The example below can be entered line per line in the console or pasted in a shell script"
	echo -e ""
	echo -e "     \033[1maddress=0x80005004\033[0m"
	echo -e "          Set the address of the register to modify"
	echo -e "     \033[1mcur_value=\`sk db r \$address\`\033[0m"
	echo -e "          Read current value"
	echo -e "     \033[1mnew_value=\`sk bool clr 9 \$cur_value\`\033[0m"
	echo -e "          Clear bit 9"
	echo -e "     \033[1msk db w \$address \$new_value\033[0m"
	echo -e "          Write back the new value"
	echo -e "     \033[1msk uart  -t <device name> -r <baud rate> -b <bits> -p <parity> -s <stop> -f <flow control>\033[0m"
	echo -e "          version : -v no_argument"
	echo -e "          debug : -d no_argument"
	echo -e "          help : -h no_argument"
	echo -e "          tty : -t <device name>"
	echo -e "          baud_rate : -r [0-4000000]"
	echo -e "          bits : -b [5,6,7,8]"
	echo -e "          parity : -p [even,odd,none]"
	echo -e "          stop : -s [1,2]"
	echo -e "          flow control: -f [XonXoff, Hw, none]"
	echo -e ""
	echo -e "\033[1mBUGS\033[0m"
	echo -e "   \033[1mBugs reporting\033[0m"
	echo -e "     Send your bug reports to your usual $company customer support contact"
	echo -e ""
	echo -e "\033[1mCOPYRIGHT\033[0m"
	echo -e "     Copyright (c) 2011 $company"
	echo -e ""
}

peripheral_db()
{
	case $2
	in
		"r" | "R")	sk-db R "$3" | cut -d'=' -f3 | cut -d' ' -f2;;
		"w" | "W")	sk-db W "$3" "$4";;
		*)	echo -e "Usage: \033[1msk\033[0m db [h] <[r][w]> <address> [value]";
			exit;;
	esac
}

peripheral_ab()
{
	case $2
	in
		"r" | "R")	sk-ab R "$3" | cut -d'=' -f3 | cut -d' ' -f2;;
		"w" | "W")	sk-ab W "$3" "$4";;
		*)	echo -e "Usage: \033[1msk\033[0m ab [h] <[r][w]> <address> [value]";
			exit;;
	esac
}

peripheral_av()
{
	case $2
	in
		"r" | "R")	dthhdmi R "$3" | cut -d'=' -f3 | cut -d' ' -f2;;
		"w" | "W")	dthhdmi W "$3" "$4";;
		*)	echo -e "Usage: \033[1msk\033[0m av [h] <[r][w]> <address> [value]";
			exit;;
	esac
}

peripheral_i2c()
{
        echo sk-i2c "$*"
        shift
        case "$2"
        in
                "r" | "R")      sk-i2c "$1" R "$3" "$4" "$5";;
                "w" | "W")      sk-i2c $*;;
                *)      echo -e "Usage: \033[1msk\033[0m i2c <instance> r <address slave> <value> <count>";
                        echo -e "Usage: \033[1msk\033[0m i2c <instance> w <address slave> <value> <count>";
                        exit;;
        esac
}
peripheral_ddr()
{ 
       	regddr2read "$1";
}

peripheral_dth()
{
	path=$(echo $3 | cut -f3 -d' ');
	list=$(echo $3 | cut -d' ' -f4-);
#	type=$(cat "$path"type);
	Type=`cat $path/type | awk -F\( '{ print $1 }'`
	case $2
	in
		"r"|"R")      
			case $list in
				value) value="$path"/value;;
				min) value="$path"/min;;
				max) value="$path"/max;;
				type) value="$path"/type;;
				*)	echo -e "Usage: \033[1msk\033[0m dth [r] <path> <[value][min][max][type]> ";
					exit;;
			esac
			if [ "$list" = "type" ]
			then
			   cat $value | awk -F\( '{ print $1 }';
			else
			if [ "$Type" = "file" ]
			then
				cat "$value";
			else
				cat "$value" | dthfilter -r "$Type" ;
			fi
      fi;;
		"w"|"W")
			value="$path"/value;
			if [ "$Type" = "string" ]
			then
				echo -n "$list" > "$value";
			else
			if [ "$Type" = "file" ]
			then
			  if [ -f $value ]; then
			    cat "$list" > "$value";
			  else
			    echo " /$2 is to be an existing filename";
			  fi
			else
				echo "$list" | dthfilter -w "$Type" > "$value";
			fi
      fi;;
		*)	echo -e "Usage: \033[1msk\033[0m dth [h] <[r][w]> <path> [value]";
				exit;;
	esac
}

peripheral_bool()
{
	case $2
	in
		add)	sk-utils 0 "$3" "$4";;
		sub)	sk-utils 1 "$3" "$4";;
		and)	sk-utils 2 "$3" "$4";;
		or)	sk-utils 3 "$3" "$4";;
		set)	sk-utils 4 "$3" "$4";;
		clr)	sk-utils 5 "$3" "$4";;
		*)	echo -e "Usage: \033[1msk\033[0m bool [h] <[add][sub][and][or][set][clr]> <value1> <value2>";
			echo -e "NB: for \033[1mset\033[0m and \033[1mclr\033[0m operations, the first \033[4mvalue(s)\033[0m gives the bit to modify in second \033[4mvalue(s)\033[0m";
			exit;;
	esac
}

peripheral_uart()
{
	sk-uart $*
}


# Everything starts here...
# Count number of parameters and display man page if none
if [ "$#" -eq "0" ] 
then
	print_usage
else
	# Proceed if the command is supported
	case $1
	in
		db)	peripheral_db "$1" "$2" "$3" "$4";;
		ab)	peripheral_ab "$1" "$2" "$3" "$4";;
		av)	peripheral_av "$1" "$2" "$3" "$4";;
		i2c)	peripheral_i2c $*;;
		ddr)	peripheral_ddr "$2";;
		dth)	peripheral_dth "$1" "$2" "$*";;
		bool)	peripheral_bool "$1" "$2" "$3" "$4";;
		uart)   peripheral_uart "$2" "$3" "$4" "$5" "$6" "$7" "$8" "$9";;
		*)	echo -e "\033[1m\033[31msk error:\033[0m\033[0m $1 is not a supported peripheral";
			print_usage;
			exit;;
	esac
fi



