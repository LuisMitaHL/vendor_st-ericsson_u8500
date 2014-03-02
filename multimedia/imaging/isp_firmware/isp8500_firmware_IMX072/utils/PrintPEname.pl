#perl PrintPEname.pl

eval 'exec perl $0 $*'
    if 0;

########################################################
# This code generate a c file which extract the PE from baseline.h 
# Created by Amel ZENATI.
# Start it without arg to have a online command summary
########################################################
    
use strict;
use File::Basename;

#Script constants, paths,...

my @VhcElementdef;
my $define;
my $def="#define";
my @version;
my $ver="firmware version";

########################################################

   
open(baseline,@ARGV[0]);
 
    while (<baseline>)
   { 
     &Version; 
 
   }
close(baseline);


########################################################
&OpenDotcFile;

    $define = $_;
    
   
open(baseline,@ARGV[0]);
 
	my $addr = 0;
	my $name;
	while (<baseline>)
   	{ 
     		if($define =  /$def/){
       			@VhcElementdef = split (/$def/,$_);
       			my @token = split (/\b/,$_);
			if (hex(@token[5]) != 0x8000){	
       				$name = @token[3];
				$name =~ s/_Byte0//;
				for ( my $i=$addr; $i<hex(@token[5]); $i+=1 ){ printf C ("   \"*** PAGE ELEMENT NOT FOUND ***\",\n");}
				$addr = hex(@token[5])+1;
    				printf C ("   \"%s\",\n", $name);
  			}
     		}
    	}
printf C ("};");

close(baseline);

&CloseDotcFile;

########################################################
sub Version

  { 

    if(index($_,$ver)>=0){

    @version = split (/\s/,$_); 
    
    }

  }
 
########################################################
sub CloseDotcFile
{
    close C;
}

########################################################
sub OpenDotcFile
   
  { 
  
  my $c_file_name;
    $c_file_name = @ARGV[1] . "/" . @ARGV[2] . "_PEList.h";
    ##$c_file_name = "../source/xp70.cpp";

    open (C,">$c_file_name");

    print C <<__END__;
 /*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

__END__

printf C ("%s\n", "static const char* " . @ARGV[2] . "_KPeNamesList[] =");

print C <<__END__;
{
__END__

}

