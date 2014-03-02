#!/bin/env perl
# vim:syntax=perl
use mmaudio;
use strict;
use warnings;
use Getopt::Long;
my $mmdsp=0;
my $mca9=0;
my $mM4=0;

GetOptions (
	'ca9'			=> \$mca9,
	'M4'			=> \$mM4
);

my $tag  = shift;
my $DSP_PROCESSOR = MMVariables::getMMDSPPROCESSOR($ENV{"PLATFORM"});
$mmdsp   = mmaudio::useSimulator($DSP_PROCESSOR);
if (($mca9)||($mM4))
{
	$mmdsp=0;
}
my $PROCESSOR = MMVariables::getMMPROCESSOR($ENV{"PLATFORM"});
my $input;
my @inputlist;
my $reference;
my @referencelist;
my $cmd;
my $args;
my @cmdlist;
my $decoutput;
my @decoutputlist;
my $retval;
my @retval_list;
my @bit_exact_list;
my $rmsarg;
my @argslist;
my @diffmaxblist;
my $diffmaxb_arg;
my $output   = "result_$DSP_PROCESSOR";
my $simcmd   = "simcmd_$DSP_PROCESSOR";
my $exe;
my @exe_list;
my $iter;
my $ntest;
my $RMSDIFF=$ENV{MMBUILDOUT} . "/tools/audio/bin/Linux/rmsdiff";
my $DIFFMAXB="../tools/bin/Linux/diffmaxb";
my $PGM ;


my $HIFI_OFF=0;
my $HIFI=1;
my $HIFI_LOCOEFS=2;
my $define_fidelity=$HIFI_OFF;
my $define_M4_LL_MSEC=0;
my $bit_exact=0;
open scan_hdl, '<../include/fidelity.h' or die;
while (<scan_hdl>) 
{
	if ($_ =~ /^#define hifi(_locoefs)?/) 
	{
		if ($1) 
		{
			$define_fidelity = $HIFI_LOCOEFS;
		}
		else
		{
			$define_fidelity = $HIFI;
		}
		last;
	}
}
close scan_hdl;
if ($mM4)
{
	open scan_hdl, '<../include/resample.h' or die;
	while (<scan_hdl>) 
	{
		if ($_ =~ /^#define M4_LL_MSEC?/) 
		{
			if ($0) 
			{
				$define_M4_LL_MSEC=1;
			}
			last;
		}
	}
	close scan_hdl;
}
if ($define_M4_LL_MSEC==1)
{
	@exe_list =(
		"diagnostic",
		"diagnostic",
		"diagnostic",
		"diagnostic"
	);
}
else
{
	@exe_list =(
		"diagnostic",
		"diagnostic",
		"diagnostic",
		"diagnostic",
		"diagnostic",
		"diagnostic",
		"diagnostic",
		"SrcMultiChannel",
		"SrcMultiChannel",
		"diagnostic",
		"diagnostic",
		"diagnostic",
		"SrcMultiChannel",
		"SrcMultiChannel",
		"diagnostic",
		"diagnostic"
	);
	if (($define_fidelity==$HIFI)||($define_fidelity==$HIFI_LOCOEFS))
	{
		@exe_list=(@exe_list,"diagnostic","SrcMultiChannel");
	};
}
if ($define_M4_LL_MSEC==1)
{
	@inputlist =(
		"../../../streams/samplerateconv/streams/sine1kHz_16.pcm",
		"../../../streams/samplerateconv/streams/BassISO_48_mono_ref.pcm",
		"../../../streams/samplerateconv/streams/sine1kHz_48.pcm",
		"../../../streams/samplerateconv/streams/BassISO_48_mono_ref.pcm"
	);
}
else
{
	@inputlist =(
		"../../../streams/samplerateconv/streams/Machado_44khzto48kHz_mono.pcm",
		"../../../streams/samplerateconv/streams/Machado_44khzto48kHz_mono.pcm",
		"../../../streams/samplerateconv/streams/Machado_44khzto48kHz_mono.pcm",
		"../../../streams/samplerateconv/streams/Machado_44khz_mono.pcm",
		"../../../streams/samplerateconv/streams/Machado_44khz_mono.pcm ",
		"../../../streams/samplerateconv/streams/Machado_44khzto48kHz_mono.pcm",
		"../../../streams/samplerateconv/streams/Machado_44khzto48kHz_mono.pcm",
		"../../../streams/samplerateconv/streams/multifreq44.pcm",
		"../../../streams/samplerateconv/streams/multifreq32.pcm",
		"../../../streams/samplerateconv/streams/techno_mono44.pcm",
		"../../../streams/samplerateconv/streams/sig08.pcm",
		"../../../streams/samplerateconv/streams/sig08.pcm",
		"../../../streams/samplerateconv/streams/multifreq8l.pcm",
		"../../../streams/samplerateconv/streams/multifreq48l.pcm",
		"../../../streams/samplerateconv/streams/sine44LR.pcm",
		"../../../streams/samplerateconv/streams/BassISO_16_mono.pcm"
	);
	if (($define_fidelity==$HIFI)||($define_fidelity==$HIFI_LOCOEFS))
	{
		@inputlist=(@inputlist,"../../../streams/samplerateconv/streams/sigin_48_44_hifi.pcm","../../../streams/samplerateconv/streams/multi44_24b.pcm");
	};
}

if ($define_M4_LL_MSEC==1)
{
	@referencelist =(
		"../../../streams/samplerateconv/streams/sine1kHz_16_48ref.pcm",
		"../../../streams/samplerateconv/streams/BassISO_16_mono_ref.pcm",
		"../../../streams/samplerateconv/streams/sine1kHz_48_16ref.pcm",
		"../../../streams/samplerateconv/streams/BassISO_48_mono_ref.pcm"
	);
}
else
{
	@referencelist=(
		"../../../streams/samplerateconv/streams/Machado_48khzto32kHz_mono.pcm",
		"../../../streams/samplerateconv/streams/Machado_48khzto11kHz_mono.pcm",
		"../../../streams/samplerateconv/streams/Machado_lowlatency_48to8kHz_mono.pcm",
		"../../../streams/samplerateconv/streams/Machado_44khzto48kHz_mono.pcm",
		"../../../streams/samplerateconv/streams/Machado_44khzto8kHz_mono.pcm",
		"../../../streams/samplerateconv/streams/Machado_lowlatency_48to8kHz_mono.pcm",
		"../../../streams/samplerateconv/streams/Machado_lowlatency_48to8kHz_mono.pcm",
		"../../../streams/samplerateconv/streams/multifreq48.pcm",
		"../../../streams/samplerateconv/streams/multifreq11.pcm",
		"../../../streams/samplerateconv/streams/techno_mono16.pcm",
		"../../../streams/samplerateconv/streams/sigref11.pcm",
		"../../../streams/samplerateconv/streams/sigref48.pcm",
		"../../../streams/samplerateconv/streams/ref_multifreq48l.pcm",
		"../../../streams/samplerateconv/streams/ref_multifreq8l.pcm",
		"../../../streams/samplerateconv/streams/ref_sine48LR.pcm",
		"../../../streams/samplerateconv/streams/BassISO_48_mono_ref.pcm"
	);
	if (($PROCESSOR eq "cortexA9")&&($mmdsp==0))
	{
		$referencelist[9]= "../../../streams/samplerateconv/streams/techno_mono16_ca9.pcm";
	}
	if (($define_fidelity==$HIFI)||($define_fidelity==$HIFI_LOCOEFS))
	{
		if ($define_fidelity!=$HIFI_LOCOEFS)
		{
			@referencelist=(@referencelist,"../../../streams/samplerateconv/streams/sigref_48_44H.pcm",	"../../../streams/samplerateconv/streams/refmulti48_24b.pcm");
		}
		else
		{
			@referencelist=(@referencelist,"../../../streams/samplerateconv/streams/sigref_48_44Hlc.pcm",	"../../../streams/samplerateconv/streams/refmulti48_24blc.pcm");
		}
	};
}
if ($define_M4_LL_MSEC==1)
{
	@cmdlist =(
		"16 48",
		"48 16",
		"48 16",
		"48 48"
	);
}
else
{
	@cmdlist =(
		"48 32",
		"48 11",
		"48 08",
		"44 48", 
		"44 08", 
		"48 08",
		"48 08",
		"44 48",
		"32 11",
		"44 16",
		"08 11",
		"08 48",
		"08 48",
		"48 08",
		"44 48",
		"16 48"
	);
	if (($define_fidelity==$HIFI)||($define_fidelity==$HIFI_LOCOEFS))
	{
		@cmdlist=(@cmdlist,"48 44",	"44 48");
	};
}
if ($define_M4_LL_MSEC==1)
{
	@argslist=(
		"l N N N Y",
		"l N N N Y",
		"l N N N Y",
		"l N N N Y"
	);
}
else
{
	@argslist=(
		"N N N N ",
		"N N N N ",
		"L N N N ",
		"N N N N ",
		"N N N N ",
		"L N N N ",
		"l N N N ",
		"N N N N ",
		"N N N N ",
		"N N N N ",
		"N N N N ",
		"l N N N ",
		"l N N N ",
		"l N N N ",
		"Y N N N ",
		"l N N N "
	);
	if (($PROCESSOR eq "cortexA9")&&($mmdsp==0))
	{
		$argslist[7]="N Y";
		$argslist[9]="N N N N Y"; #test for 16-bit input samples coming from framework
		$argslist[10]="N N N N Y"; #test for 16-bit input samples coming from framework
		$argslist[11]="l N N N Y"; #test for 16-bit input samples coming from framework
		$argslist[15]="l N N N Y"; #test for 16-bit input samples coming from framework
	}

	if (($define_fidelity==$HIFI)||($define_fidelity==$HIFI_LOCOEFS))
	{
		@argslist=(@argslist,"H N N Y","H");
	};
}
if ($define_M4_LL_MSEC==1)
{
	@diffmaxblist=(
		"--maxdiff=0",
		"--maxdiff=0",
		"--maxdiff=0",
		"--maxdiff=0"
	);
}
else
{
	@diffmaxblist=(
		"--maxdiff=1",
		"--maxdiff=1",
		"--maxdiff=1", #**3
		"--maxdiff=1",
		"--maxdiff=1",
		"--maxdiff=1", #**3
		"--maxdiff=1", #**3
		"--maxdiff=1",
		"--maxdiff=1",
		"--maxdiff=2",
		"--maxdiff=2",
		"--maxdiff=2",
		"--maxdiff=1", #**2
		"--maxdiff=1", #**2
		"--maxdiff=1",
		"--maxdiff=2"
	);
# ** are maxdiff values for using 16-bit coefs and 16-bit data for CA9 target
#if (($PROCESSOR eq "cortexA9")&&($mmdsp==0))
#{
#	$diffmaxblist[7]="--maxdiff=3";
#	$diffmaxblist[9]="--maxdiff=5";
#	$diffmaxblist[10]="--maxdiff=3";
#	$diffmaxblist[11]="--maxdiff=2";
#}
	if (($define_fidelity==$HIFI)||($define_fidelity==$HIFI_LOCOEFS))
	{
		@diffmaxblist=(@diffmaxblist,"--maxdiff=16 --nbytes=3","--maxdiff=16 --nbytes=3 --nch=8 --chantotest=all ");
	};
}

if ($define_M4_LL_MSEC==1)
{
	@decoutputlist=(
		"$output",
		"$output",
		"$output",
		"$output"
	);
}
else
{
	@decoutputlist=(
		"$output",
		"$output",
		"$output",
		"$output",
		"$output",
		"$output",
		"$output",
		"$output",
		"$output",
		"$output",
		"$output",
		"$output",
		"$output",
		"$output",
		"$output",
		"$output"
	);

	if (($define_fidelity==$HIFI)||($define_fidelity==$HIFI_LOCOEFS))
	{
		@decoutputlist=(@decoutputlist,"$output","$output");
	};
}

if ($define_M4_LL_MSEC==1)
{
	@retval_list=(
		"0",
		"0",
		"0",
		"0"
	);
}
else
{
	@retval_list=(
		"0",
		"0",
		"0",
		"0",
		"0",
		"0",
		"0",
		"0",
		"0",
		"0",
		"0",
		"0",
		"0",
		"0",
		"0",
		"0",
		"0"
	);
	if (($define_fidelity==$HIFI)||($define_fidelity==$HIFI_LOCOEFS))
	{
		@retval_list=(@retval_list,"0","0");
	};
}

if ($define_M4_LL_MSEC==1)
{
	@bit_exact_list=(
		1,
		1,
		1,
		0
	);
}
else
{
	@bit_exact_list=(
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0
	);
	if (($define_fidelity==$HIFI)||($define_fidelity==$HIFI_LOCOEFS))
	{
		@bit_exact_list=(@bit_exact_list,0,0);
	};
}



$ntest=0;
foreach $input (@inputlist)
{
	$ntest+=2;
}
mmtest::OSplan( $ntest );
mmtest::init($0,$tag);
$iter=0;
foreach $input (@inputlist)
{
	$reference=$referencelist[$iter];
	$cmd=$cmdlist[$iter];
	$args=$argslist[$iter];
	$decoutput=$decoutputlist[$iter];
	$retval=$retval_list[$iter];
	$diffmaxb_arg=$diffmaxblist[$iter];
	$exe="$ENV{MMBUILDOUT}/bin/mmdsp-x/";
	$exe="$exe$exe_list[$iter]";
	$bit_exact=$bit_exact_list[$iter];
	if ($mmdsp==1){
    	mmaudio::genSimpleSimCmdfile($simcmd, $exe, " $input $output $cmd $args ");
    	mmtest::system("mmdspsim -c $simcmd",$retval, "execution: decode  $input $output $cmd $args ");
	}
	else{
		if ($PROCESSOR eq "cortexA9"){
			#-------- look for path to axf file ------------
			my $binpath;
			if ($exe_list[$iter] eq "diagnostic")
			{
				system("find -name '*diagnostic.axf' >>  binpathfile");
			}
			else
			{
				system("find -name '*SrcMultiChannel.axf' >>  binpathfile");
			}
			open $binpath, "binpathfile";
			$PGM = <$binpath>;
			chop $PGM;
			close ($binpath);
			system("rm binpathfile");
			#-----------------------------------------------
			mmtest::system("ca9sim". " $PGM $input $output $cmd $args " , $retval,$retval,"ca9sim". " $PGM $input $output $cmd $args " , $retval);
		}
		else {
    		mmtest::system("$exe.exe $input $output $cmd $args ",$retval, "execution: $exe $input $output $cmd $args ");
		}
	}
	if ($bit_exact)
	{
		mmtest::system("cmp $decoutput $reference", 0, "cmp $decoutput with $reference");
	}
	else
	{
		mmtest::system("$DIFFMAXB $diffmaxb_arg $decoutput $reference", 0, "diffmaxb $diffmaxb_arg $decoutput with $reference");
	}
	$iter++;
}
mmtest::concl();

