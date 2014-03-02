#!/bin/env perl
# vim:syntax=perl
use Colors;
use mmaudio;
use Getopt::Long;
my $mmdsp=0;
my $mca9=0;
GetOptions (
    'ca9'         => \$mca9,
);

$UNIX = (defined($ENV{OS}) && ($ENV{OS} =~ /(W|w)indows/) ? 0 : 1);
$STREAMDIR =  $ENV{"AUDIOSTREAMS"} ."/srcregression/reference2";
#$STREAMDIR = "/home/glassw/scratch";

$singen="../../tools/bin/Linux/singen";
$addsig="../../tools/bin/Linux/addsig";
$convrt="../../tools/bin/Linux/convrt";
$evaldist="../../tools/bin/Linux/evaldist";

$verbose = 1;
$iter = 0;
$nbsec = 1;
$fdont_use_mmtest=0;
$fgenrefstreams=0;
$nbfreqin=12;
$nbfreqout=9;
$fdsp=0;
$fCortex=0;
$fCortexbrd=0;
#------------------------------- options for testing --------------------------------------------------------------------------
$freq_in= "default";
$freq_out= "default";
$freq_in="48";#freq_in  = xx single or starting input frequency for testing the frequencies: 08,11,12,16,22,24,32,44,48,64,88,96
$freq_out="44";#freq_out = xx single or starting output frequency for testing the frequencies: 08,11,12,16,22,24,32,44,48
$lowlatency=0;#lowlatency = test lowlatency configuration
$lowlatency_fixed_time=0;#lowlatency = test lowlatency configuration with fixed input and output nsamples in msec
$lowripple=0;#lowripple = test lowripple configuration
$dohifi=0;#hifi = test high fidelity configuration
$dofrom=0;#dofrom = do all tests only starting from finxx and foutxx specified below, else all tests if nothing specified
$ntest=10000;#ntest = xx total number of tests to be executed. default all
$sample16=0;
#-------------------------------------------------------------------------------------------------------------------------------
my $HIFI_OFF=0;
my $HIFI=1;
my $HIFI_LOCOEFS=2;
my $define_fidelity=$HIFI_OFF;
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

if ($define_fidelity==$HIFI_OFF)
{
	$dohifi=0;
}
close scan_hdl;



$mmdsp   = mmaudio::useSimulator(MMVariables::getMMDSPPROCESSOR($ENV{"PLATFORM"}));
$processor = MMVariables::getMMPROCESSOR($ENV{"PLATFORM"});
if ($mca9)
{
	$mmdsp=0;
}
else
{
	$sample16=0;
}
if ($mmdsp==1)
{
	my $dspprocessor = MMVariables::getMMDSPPROCESSOR($ENV{"PLATFORM"});
	$BINDIR="../standalone/bin/"."$dspprocessor";
	$fdsp=1;
}
else
{
	$BIN_DIR = MMVariables::getBIN_DIR($ENV{"PLATFORM"});
	$BINDIR =  "../standalone/" . "$BIN_DIR";
	if (($processor eq "arm11")||($processor eq "cortexA9"))
	{
		$fCortex=1;
	}
}
$PGM= "$BINDIR/diagnostic";
@ftab   =("08","11" ,"12" ,"16" ,"22" ,"24" ,"32" ,"44" ,"48" ,"64" ,"88" ,"96" );
@fvaltab=(8000,11025,12000,16000,22050,24000,32000,44100,48000,64000,88200,96000);

if (get_freq($freq_in,$nbfreqin)<0)
{
	print "bad input frequency specification\n";
	die;
}
if (get_freq($freq_in,$nbfreqin)==0)
{
	@freqin_list=@ftab[0..$nbfreqin-1];
}
else
{
	if ($dofrom==0)
	{
		@freqin_list=$freq_in;
	}
	else
	{
		$i=0;
		$valid=0;
		foreach $fval (@ftab[0..$nbfreqin-1])
		{
			if ($freq_in eq $fval)
			{	 
				$valid=1;
			}
			if ($valid == 1)
			{
				@freqin_list[$i]=$fval;
				$i=$i+1;
			}
		}
	}
}
if (get_freq($freq_out,$nbfreqout)<0)
{
	print "bad output frequency specification\n";
	die;
}
if (get_freq($freq_out,$nbfreqout)==0)
{
	@freqout_list=@ftab[0..$nbfreqout-1];
}
else
{
	if ($dofrom==0)
	{
		@freqout_list=$freq_out;
	}
	else
	{
		$i=0;
		$valid=0;
		foreach $fval (@ftab[0..$nbfreqout-1])
		{
			if ($freq_out eq $fval)
			{	 
				$valid=1;
			}
			if ($valid == 1)
			{
				@freqout_list[$i]=$fval;
				$i=$i+1;
			}
		}
	}
}
if ($lowripple !=0)
{
	@config_list=(1);
}
if ($lowlatency!=0)
{
	@config_list=(@config_list,2);
}
if ($lowlatency_fixed_time!=0)
{
	@config_list=(@config_list,3);
}
if ($dohifi!=0)
{
	@config_list=(@config_list,4);
}

if (($dohifi==0)&&($lowripple==0)&&($lowlatency==0)&&($lowlatency_fixed_time==0))
{
	if ((($define_fidelity==$HIFI)||($define_fidelity==$HIFI_LOCOEFS))&&($sample16==0))
	{
		@config_list=(0,1,2,3,4);
	}
	else
	{
		@config_list=(0,1,2,3);
	}
}
#--------------------------------------------------------------------------------
system("rm -f SRCRES.txt\n");
system("rm -f TESTRES.txt\n");
system("rm -f srcres.txt\n");
system("rm -f srclog.txt\n");

$fnumtest=0;
foreach $freqin (@freqin_list) #These loops are only for determining the OSplan number of tests
{
	foreach $freqout (@freqout_list)
	{
		foreach $src_quality (@config_list)
		{
			if ($iter>=$ntest) {next;}
			if (($freqin  eq "07")&&($freqout ne "08")&&($freqout ne "07")) {next;}
			if (($freqout eq "07")&&($freqin  ne "08")&&($freqin  ne "07")) {next;}
#			if (($src_quality ==1)&&(($freqout ne "48")||($freqin eq "64")||($freqin eq "88")||($freqin eq "96")))	{next;}
			if (($src_quality ==1)&&(($freqout ne "48")||($freqin ne "44")))	{next;}
			if (($src_quality==2)||($src_quality==3))
			{
				if	((($freqin eq "48")&&($freqout eq "16"))||
					(($freqin eq "16")&&($freqout eq "08"))||
					(($freqin eq "48")&&($freqout eq "08"))||
					(($freqin eq "08")&&($freqout eq "16"))||
					(($freqin eq "16")&&($freqout eq "48"))||
					(($freqin eq "08")&&($freqout eq "48")))
				{
					# process
				}
				else
				{
					next;
				}
			}
			$fnumtest=$fnumtest+2;
		}
	}
}
my $tag  = shift;
mmtest::OSplan( $fnumtest );
mmtest::init($0,$tag);
foreach $freqin (@freqin_list)
{
	foreach $freqout (@freqout_list)
	{
		foreach $src_quality (@config_list)
		{
			if ($iter>=$ntest) {next;}
			if (($freqin  eq "07")&&($freqout ne "08")&&($freqout ne "07")) {next;}
			if (($freqout eq "07")&&($freqin  ne "08")&&($freqin  ne "07")) {next;}
#			if (($src_quality ==1)&&(($freqout ne "48")||($freqin eq "64")||($freqin eq "88")||($freqin eq "96")))	{next;}
			if (($src_quality ==1)&&(($freqout ne "48")||($freqin ne "44")))	{next;}
			if (($src_quality==2)||($src_quality==3))
			{
				if	((($freqin eq "48")&&($freqout eq "16"))||
					(($freqin eq "16")&&($freqout eq "08"))||
					(($freqin eq "48")&&($freqout eq "08"))||
					(($freqin eq "08")&&($freqout eq "16"))||
					(($freqin eq "16")&&($freqout eq "48"))||
					(($freqin eq "08")&&($freqout eq "48")))
				{
					# process
				}
				else
				{
					next;
				}
			}
			system("rm -f sig1.bin");
			system("rm -f sig2.bin");
			system("rm -f sig.bin");
			system("rm -f sig.pcm");
			system("rm -f sigout.pcm");
			system("rm -f simres");
			system("rm -f sigout.bin");
			$frin=get_freq($freqin,$nbfreqin);
			$frout=get_freq($freqout,$nbfreqout);
			$siglvl=-12;
			if ($frin<=$frout)
			{
				if ($src_quality==1)
				{  
					$fr_edge=$frin*.4534;
				}
				else
				{
					$fr_edge=$frin*.4;
				}
				$fr_alias=0;
				$siglvl_alias=-200;
			}
			else
			{
				$fr_edge=$frout*.4;
				$fr_alias=$frout*.625;
				if ($fr_alias>(.45*$frin))
				{
					$siglvl_alias=-200;
				}
				else
				{
					$siglvl_alias=$siglvl;
				}
			}
			system("$singen sig1.bin  1000  " . " $frin " ." $siglvl 0 " . " $nbsec ". " 0 >> srclog.txt" );
			system("$singen sig2.bin " . " $fr_edge " . " $frin " ." $siglvl 0 " . " $nbsec "." 0 >> srclog.txt");
			system("$singen sig3.bin " . " $fr_alias " . " $frin " ." $siglvl_alias 0 " . " $nbsec "." 0 >> srclog.txt");
			system("$addsig 3  sig1.bin sig2.bin sig3.bin sig.bin 1.0 >> srclog.txt");
#			if ($src_quality!=4)
#			{
#				system("$convrt sig.bin sig.pcm -it b -ot b -inb 32 -onb 16 -iend l -oend b >> srclog.txt");
#			}
#			else
			{
				system("$convrt sig.bin sig.pcm -it b -ot b -inb 32 -onb 24 -iend l -oend b >> srclog.txt");
			}
			print "---------- testing for fin: $freqin fout: $freqout src_quality: $src_quality ---------------\n";
			$fres=0;
			if ($fdsp!=0)
			{
				if (-z "cmd_mmdspsim") 
				{
					unlink("cmd_mmdspsim");
				}
				open(CMDSIM,">cmd_mmdspsim");
				print CMDSIM "file $PGM.elf\n";
				if ($src_quality ==1)
				{
					print CMDSIM "set args ". " sig.pcm sigout.pcm". " $freqin ". "$freqout " . "Y N N Y\n"; #low ripple
				}
				else
				{
					if ($src_quality==2)
					{
						print CMDSIM "set args ". " sig.pcm sigout.pcm". " $freqin ". "$freqout " . "L N N Y\n"; #low latency
					}
					else
					{ 
						if ($src_quality==3)
						{
							print CMDSIM "set args ". " sig.pcm sigout.pcm". " $freqin ". "$freqout " . "l N N Y\n"; #low latency with msecs
						}
						else
						{
							if ($src_quality==4)
							{
								print CMDSIM "set args ". " sig.pcm sigout.pcm". " $freqin ". "$freqout " . "H N N Y\n"; #high fidelity
							}
							else
							{
								print CMDSIM "set args ". " sig.pcm sigout.pcm". " $freqin ". "$freqout " . "N N N Y\n"; #standard quality
							}
						}
					}
				} 
				print CMDSIM "enable profile \n";
				print CMDSIM "break exit\n";
				print CMDSIM "run\n";
				$test_time=$nbsec*1000;
				$filename="$freqin"."_"."$freqout".".prf";
				print CMDSIM "info stacklimit\n";
				print CMDSIM "profile -duration "." $test_time  ". " -o ". "$filename\n";
				print CMDSIM "cont\n";
				close(CMDSIM);
				print "executing mmdspsim\n";
				mmtest::system("mmdspsim  -c cmd_mmdspsim > simres ",0, "execution: ");
			}
			else
			{
				my $argument;
				if ($src_quality==1)
				{
					$argument =  "Y N N Y"; #low ripple
				}
				else
				{
					if ($src_quality==2)
					{
						$argument =  "L N N Y"; #low latency
					}
					else
					{
						if ($src_quality==3)
						{
							$argument =  "l N N Y"; #low latency with msec
						}
						else
						{
							if ($src_quality==4)
							{
								$argument =  "H N N Y"; #hifi
							}
							else
							{
								$argument =  "N N N Y"; #standard quality
							}
						}
					}
				}
				if ($sample16 != 0)
				{
					$argument = ("$argument Y");
				}
				my $cmd;
				if ($fCortex!=0)
				{
					#-------- look for path to axf file ------------
					open(CMD_FINDFILE,">cmd_find");
					print CMD_FINDFILE "cd ..\n";
					print CMD_FINDFILE "find -name '*.axf' >>  ./test/binpathfile\n";
					print CMD_FINDFILE "cd test\n";
					close(CMD_FINDFILE);
					my $binpath;
					system("chmod 777 cmd_find");
					system("cmd_find");
					system("rm -f cmd_find");
					open $binpath, "binpathfile";
					$PGM = <$binpath>;
					chop $PGM;
					close ($binpath);
					system("rm -f binpathfile");
					#-----------------------------------------------
					if ($fCortexbrd!=0)
					{
						$cmd = "ca9brd". " ../$PGM". " sig.pcm sigout.pcm". " $freqin ". "$freqout $argument";
					}
					else
					{
						$cmd = "ca9sim". " ../$PGM". " sig.pcm sigout.pcm". " $freqin ". "$freqout $argument";
					}
				}
				else
				{
					$cmd = "$PGM.exe sig.pcm sigout.pcm". " $freqin ". "$freqout $argument";
				}
				blue "$cmd" if ($verbose);
				mmtest::system("$cmd",0, "execution: $cmd");
			}
			system("$convrt sigout.pcm sigout.bin -it b -ot b -inb 24 -onb 32 -iend b -oend l >> srclog.txt");
			if ($src_quality ==1)
			{
				$freq_config="$freqin"."_"."$freqout"."r"; #low ripple
			}
			else
			{
				if ($src_quality==2)
				{
					$freq_config="$freqin"."_"."$freqout"."l"; #low latency
				}
				else
				{
					if ($src_quality==3)
					{
						$freq_config="$freqin"."_"."$freqout"."l"; #low latency with msec
					}
					else
					{
						if($src_quality==4)
						{
							if ($define_fidelity==$HIFI)
							{
								$freq_config="$freqin"."_"."$freqout"."H"; #high fidelity
							}
							else
							{
								$freq_config="$freqin"."_"."$freqout"."Hlc"; #high fidelity low coefficient count
							}
						}
						else
						{
							$freq_config="$freqin"."_"."$freqout"."n"; #standard quality	
						}
					}
				}
			}
			system("echo $freq_config >> SRCRES.txt");
			$ref_filename="sigref_"."$freq_config".".pcm";
			$ref_filename_with_path="$STREAMDIR\/"."$ref_filename";
			if ($fgenrefstreams!=0)
			{
				system("cp sigout.pcm $ref_filename_with_path");
			}
			my $cmd;
			if ($sample16==0)
			{
				$cmd="../../tools/bin/Linux/diffmaxb --maxdiff=16 --nbytes=3 --endieness=bb sigout.pcm $ref_filename_with_path ";
			}
			else
			{
				$cmd="../../tools/bin/Linux/diffmaxb --maxdiff=2048 --nbytes=3 --endieness=bb sigout.pcm $ref_filename_with_path ";
			}
			$fres+=(system($cmd));
			blue "$cmd" if $verbose;
			mmtest::system("$cmd",0, "execution: $cmd");
			if ($fCortexbrd==0)
			#------------------- dont't evalute disto using board as sigout.bin is too small if cycle evaluation test was performed ------
			{
				if ($src_quality ==1 )
				{ 
					my $cmd="$evaldist sigout.bin" . " $frout " . ".5 0 --min_lvl=.15 --min_thd=70 --num_freq=3 >> SRCRES.txt ";
					blue "$cmd" if $verbose;
					$fres+= system($cmd);	
				}
				else
				{
					if (($src_quality==2)||($src_quality==3))
					{
						my $cmd="$evaldist sigout.bin" . " $frout " . ".01 0 --min_lvl=.15 --min_thd=50 >> SRCRES.txt ";
						blue "$cmd" if $verbose;
						$fres+= system($cmd);
					}
					else
					{
						my $cmd="$evaldist sigout.bin" . " $frout " . ".001 0 --min_lvl=.15 --min_thd=75 >> SRCRES.txt ";
						blue "$cmd" if $verbose;
						$fres+= system($cmd);
					}
				}
			}
			if ($fres)
			{
				system("echo error for $freq_config >> TESTRES.txt");
			}
			if ($fdsp!=0)
			{
				system("grep  \"total cycles:\" simres  >> SRCRES.txt");
			}
			$iter=$iter+1;
		}
	}
}
open $infile, "SRCRES.txt";
open $outfile, ">srcres.txt";
my $getconfig=1,$storeres=0;
my $temp;
while(<$infile>)
{
	$temp=$_;
	if ($getconfig==1)
	{
		$config=$temp;
		chop $config;
		$getconfig=0;
	}
	if ($fdsp==0)
	{
		if ($temp=~/THD:/) 
		{
			$pos=index($temp,"THD:");
			$thdval=substr($temp,$pos+4,10);
			$storeres=1;
		}
		if ($storeres==1)
		{
			printf $outfile "config: %s    thd: %s\n",$config,$thdval;
			$storeres=0;
			$getconfig=1;
		}
	}
	else
	{
		if ($temp=~/THD:/) 
		{
			$pos=index($temp,"THD:");
			$thdval=substr($temp,$pos+4,10);
		}
		if ($temp=~/total cycles:/)
		{
			$mipsval = $temp;
			$mipsval=substr($mipsval,14,8);
			$mipsval=$mipsval/($nbsec*1e6);
			$storeres=1;
		}
		if ($storeres==1)
		{
			printf $outfile "config: %s    thd: %s    mips: %s\n",$config,$thdval,$mipsval;
			$storeres=0;
			$getconfig=1;
		}
	}
}
close $infile;
close $outfile;
system("rm -f sig1.bin");
system("rm -f sig2.bin");
system("rm -f sig3.bin");
system("rm -f sig.bin");
#system("rm -f sig.pcm");
#system("rm -f sigout.pcm");
system("rm -f sigout.bin");
system("rm -f *.prf");
system("rm -f *.sort");
system("rm -f *.pcv");		
system("rm -f *.dcv");
system("rm -f SRCRES.txt");
system("rm -f simres");
system("rm -f srclog.txt");

mmtest::concl();
sub get_freq
{
	# returns freq in Hz or -1 if bad value specified or 0 if no value specified
	local($freq);
	local($fval,$nbfreq) = @_;
	local($listval);
	local($iter) = 0;
	$freq=-1;
	foreach $listval (@ftab)
	{
		if (($fval eq $listval)&&($iter<$nbfreq))
		{
			$freq=@fvaltab[$iter];
		}
		$iter=$iter+1;
	}
	if ($fval eq "default")
	{
		$freq=0;
	}
	return $freq;
}

