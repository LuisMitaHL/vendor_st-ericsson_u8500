#!/bin/env perl
# vim:syntax=perl
use mmaudio;
use MMVariables;
use strict;
use warnings;
use Getopt::Long;


# Should be removed later
my $mmdsp=0;

my $PROCESSOR =  MMVariables::getMMPROCESSOR($ENV{PLATFORM});

GetOptions (
    'mmdsp'         => \$mmdsp,
);

$mmdsp=1;

my $tag  = shift;

if ($mmdsp) {
    $PROCESSOR =  MMVariables::getMMDSPPROCESSOR($ENV{PLATFORM});
}


my $useSimulator   = mmaudio::useSimulator($PROCESSOR);
my $simulator = "";

$simulator = "arm11sim" if ($PROCESSOR eq "arm11");
$simulator = "ca9sim" if ($PROCESSOR eq "cortexA9");
$simulator = "mmdspsim" if ($PROCESSOR =~ m/mmdsp_\d*/);



#$simulator = "ca9brd"; #Used for MIPS count

my $output   = "result_$PROCESSOR";
my $simcmd   = "simcmd_$PROCESSOR";
my $exe;
my %bin;
%bin = ( "x86_cortexA9-linux" , "$ENV{MMBUILDOUT}/bin/x86_cortexA9/volctrl",
         "u8500_v2-android" ,   "$ENV{MMBUILDOUT}/bin/cortexA9-x-android-x/volctrl",
         "x86-linux",           "$ENV{MMBUILDOUT}/bin/x86/volctrl",
         "x86_mmdsp-linux",     "$ENV{MMBUILDOUT}/bin/x86_mmdsp/volctrl",
         "u9540_v1-android" ,   "$ENV{MMBUILDOUT}/bin/cortexA9-x-android-x/volctrl" );
$exe = $bin{$ENV{PLATFORM}};
if ($mmdsp) {
	$exe="$ENV{MMBUILDOUT}/bin/mmdsp-x/volctrl";
}

my $iter;
my $ntest;
my $RMSDIFF=$ENV{MMBUILDOUT} . "/tools/audio/bin/Linux/rmsdiff";

my @inputlist =(
    "../../../streams/volctrl/streams/sin440.pcm",
    "../../../streams/volctrl/streams/sin440.pcm",
    "../../../streams/volctrl/streams/sin440.pcm",
    "../../../streams/volctrl/streams/sin440.pcm",
    "../../../streams/volctrl/streams/sin440.pcm",
    "../../../streams/volctrl/streams/sin440.pcm"
);

my @referencelist;
my @cmdlist;
if  (($PROCESSOR eq "x86_mmdsp"))
{
    @referencelist=(
        "../../../streams/volctrl/streams/ref_test1_unix.pcm",
        "../../../streams/volctrl/streams/ref_test3.pcm",
        "../../../streams/volctrl/streams/ref_test4_unix.pcm",
        "../../../streams/volctrl/streams/new_ref_test5.pcm"
    );
}
else {
    @referencelist=(
        "../../../streams/volctrl/streams/ref_test1.pcm",
        "../../../streams/volctrl/streams/ref_test3.pcm",
        "../../../streams/volctrl/streams/ref_test4.pcm",
        "../../../streams/volctrl/streams/new_ref_test5.pcm"
	)
}

if  (($PROCESSOR eq "x86") || ($PROCESSOR eq "cortexA9") || ($PROCESSOR =~ s/arm//))
{
    @referencelist=(
        "../../../streams/volctrl/streams/ref_test1_float.pcm",
        "../../../streams/volctrl/streams/ref_test3_float.pcm",
        "../../../streams/volctrl/streams/ref_test4_float.pcm",
        "../../../streams/volctrl/streams/ref_test5_float.pcm",
        "../../../streams/volctrl/streams/ref_test1_float.pcm",
        "../../../streams/volctrl/streams/ref_testmulti_float.pcm"


    );
# multichannel=0,nb_bit_in,nb_channel_in,downmix,db_ramp,gll,glr,grl,grr,alphal,alphar,block_size
# multichannel=1,nb_bit_in,nb_channel_in,db_ramp,g0,g1,..g7,alpha0,alpha1,..alpha7,block_size
	@cmdlist =(
    	["0", "16","2","0","1","-3.0", "-114.0",  "-114.0",  "-6.0",  "20000","20000",  "64"],
    	["0", "16","1","0","1","3.0",  "-114.0",  "-114.0",  "-114.0",    "0",    "0",  "64"],
    	["0", "16","2","0","1","-6.0", "-1.0",    "-24.0" ,  "-12.0" ,"25000","25000",  "64"],
    	["0", "16","2","1","1","0.0",  ,"-114.0", "-114.0",  "0.0",    "5000", "5000",  "64"],
    	["1", "16","2","1","-3.0","-6.0","20000","20000","64"], 
    	["1", "16","6","1","-3.0","-6.0","-9","-12","-15","-18","20000","20000","20000","20000","20000","20000","60"]
	);
}
else
{
	@cmdlist =(
    	["0", "16","2","0","1","-3.0", "-114.0",  "-114.0",  "-6.0",  "20000","20000",  "64"],
    	["0", "16","1","0","1","3.0",  "-114.0",  "-114.0",  "-114.0",    "0",    "0",  "64"],
    	["0", "16","2","0","1","-6.0", "-1.0",    "-24.0" ,  "-12.0" ,"25000","25000",  "64"],
    	["0", "16","2","1","1","0.0",  ,"-114.0", "-114.0",  "0.0",    "5000", "5000",  "64"]
	);
}


my @comments = (
    "common use case : stereo with no cross gain",
    "basic mono case",
    "complete cross gain",
    "downmix case with stereo",
	"2 multichannel test for x86 target only",
	"6 multichannel test for x86 target only"
);

my @rmsarglist=(
    "",
    "",
    "",
    "",
    "",
	"",
	""
);

my @outputlist=(
    "$output"."_Common",
    "$output"."_BasicMono",
    "$output"."_Complete",
    "$output"."_StereoDownmix",
	"$output"."_Common_nch",
	"$output"."_Common_nch6"
);

my @retvallist=(
    "0",
    "0",
    "0",
    "0",
	"0",
	"0"
);

$ntest=@cmdlist*2;
#$ntest=0;
mmtest::OSplan( $ntest );
mmtest::init($0,$tag);
$iter=0;
my $cmd;
#if(0)
{
foreach  $cmd (@cmdlist)
{
	my $input=$inputlist[$iter];
    if (-s("$outputlist[$iter]_param.txt") || -z("param_$iter.txt")) {
        unlink("$outputlist[$iter]_param.txt");
    }
    if (open (FILE, ">$outputlist[$iter]_param.txt")) {
        print FILE "multichannel: $cmdlist[$iter][0];\n";
		if ($cmdlist[$iter][0] eq "0")
		{
        	print FILE "nb_bit_in: $cmdlist[$iter][1];\n";
        	print FILE "nb_channel_in: $cmdlist[$iter][2];\n";
        	print FILE "downmix: $cmdlist[$iter][3];\n";
        	print FILE "db_ramp: $cmdlist[$iter][4];\n";
        	print FILE "gll: $cmdlist[$iter][5];\n";
        	print FILE "glr: $cmdlist[$iter][6];\n";
       		print FILE "grl: $cmdlist[$iter][7];\n";
        	print FILE "grr: $cmdlist[$iter][8];\n";
        	print FILE "alphal: $cmdlist[$iter][9];\n";
        	print FILE "alphar: $cmdlist[$iter][10];\n";
        	print FILE "block_size: $cmdlist[$iter][11];\n";
		}
		else
		{
       		print FILE "nb_bit_in: $cmdlist[$iter][1];\n";
        	print FILE "nb_channel_in: $cmdlist[$iter][2];\n";
			my $nch=$cmdlist[$iter][2];
			print FILE "db_ramp: $cmdlist[$iter][3];\n";
			my $num=4;
			my $i;
			for ($i=0;$i<$nch;$i++)
			{
				print FILE "g$i: $cmdlist[$iter][$num];\n"; 
				$num++;
			}
			for ($i=0;$i<$nch;$i++)
			{
				print FILE "alpha$i: $cmdlist[$iter][$num];\n"; 
				$num++;
			}
			print FILE "block_size: $cmdlist[$iter][$num];\n";
		}
        if($useSimulator){
            if ($simulator eq "mmdspsim") {
                mmaudio::genSimpleSimCmdfile($simcmd, $exe, " $input $outputlist[$iter].raw $outputlist[$iter]_param.txt");
                mmtest::system("mmdspsim -c $simcmd",$retvallist[$iter], "execution: perform $comments[$iter] $input $output");
            }
            else {
                mmtest::system("$simulator $exe.axf  $input $outputlist[$iter].raw $outputlist[$iter]_param.txt",$retvallist[$iter], "execution: perform $comments[$iter] $input $output");
            }
        }
        else{
            mmtest::system("$exe.exe $input $outputlist[$iter].raw $outputlist[$iter]_param.txt",$retvallist[$iter], "execution: perform $comments[$iter] $input $outputlist[$iter]");
        }
        mmtest::system("$RMSDIFF $rmsarglist[$iter] $outputlist[$iter].raw $referencelist[$iter]", 0, "rmsdiff $rmsarglist[$iter] $outputlist[$iter].raw with $referencelist[$iter]");
    $iter++;
    }
}
}
mmtest::concl();

