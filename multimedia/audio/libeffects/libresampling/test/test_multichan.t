#!/bin/env perl
# vim:syntax=perl


$functgen="../../tools/bin/Linux/functgen";
$deintlv="../../tools/bin/Linux/deintlv";
$convrt="../../tools/bin/Linux/convrt";
$evaldist="../../tools/bin/Linux/evaldist";


$PGM= "../standalone/bin/x86/SrcMultiChannel.exe";
@ftab   =("08","11" ,"12" ,"16" ,"22" ,"24" ,"32" ,"44" ,"48" ,"64" ,"88" ,"96" );
@fvaltab=(8000,11025,12000,16000,22050,24000,32000,44100,48000,64000,88200,96000);
@chlist=(0..7);
@pcmlist=("sig0.pcm","sig1.pcm","sig2.pcm","sig3.pcm","sig4.pcm","sig5.pcm","sig6.pcm","sig7.pcm");
@chfreq=("440","554","659","880","1108","1318","1760","2216"); # A major triad cords (A1,C1,E1,G1,A2,C2,E2,G2) 
@testfreqin=("44","32");
@testfreqout=("48","11");
$testno=0;
foreach $freqin (@testfreqin)
{
	$freqout=$testfreqout[$tstno];
	$frin=get_freq($freqin,9);
	$frout=get_freq($freqout,12);
	$iter=0;
	print "\n******* generate 8-channel input signal ********\n\n";
	foreach $chno (@chlist)
	{
		$fc=$chfreq[$iter];
		system("$functgen --fc=$fc --fs=$frin --nch=8 --chno=$chno multifreq$freqin.pcm");
		$iter=$iter+1;
	}
	system("$deintlv multifreq$freqin.pcm 8 @pcmlist");
	print "\n******* evaluate THD input signal ********\n\n";
	foreach $evalno (@pcmlist)
	{
		system("$convrt $evalno temp.bin -it b -ot b -iend b -oend l -inb 16 -onb 32");
		system("$evaldist temp.bin $frin");
	}
	print "\n******* execute SrcMultiChannel freqin= $freqin freqout= $freqout *******\n";
	system("$PGM multifreq$freqin.pcm multifreq$freqout.pcm  $freqin $freqout");
	system("$deintlv multifreq$freqout.pcm 8 @pcmlist");
	print "\n******* evaluate THD output signal ********\n\n";
	foreach $evalno (@pcmlist)
	{
		system("$convrt $evalno temp.bin -it b -ot b -iend b -oend l -inb 16 -onb 32");
		system("$evaldist temp.bin $frout");
	}
	foreach $evalno (@pcmlist)
	{
		system("rm -f $evalno");
	}
	$tstno=$tstno+1;
	system("rm -f temp.bin");
	system("mv multifreq$freqin.pcm ../../streams/.");
	system("mv multifreq$freqout.pcm ../../streams/.");
}



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

