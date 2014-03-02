#!/bin/env perl
# vim:syntax=perl
use mmaudio;
use strict;
use warnings;

my $tag  = shift;

my $PROCESSOR   = MMVariables::getMMDSPPROCESSOR($ENV{PLATFORM});
my $useMmdspsim = mmaudio::useSimulator($PROCESSOR);
my $input;
my @inputlist;
my $reference;
my @referencelist;
my $cmd;
my @cmdlist;
my $chan;
my @chanlist;
my $fs;
my @fslist;
my $decoutput;
my @decoutputlist;
my $simcmd   = "simcmd_$PROCESSOR";
my $exe      = "$ENV{MMBUILDOUT}/bin/mmdsp-x/mdrc5b";
my $cmp      = $ENV{MMROOT} . "/audio/afm/tools/bin/Linux/comp";
my $rmsdiff  = $ENV{MMROOT} . "/tools/audio/bin/Linux/rmsdiff";
my $iter;
my $ntest;
    @inputlist =(
        "../../../streams/mdrc/streams/sin1k_48k_stereo.pcm",
        "../../../streams/mdrc/streams/sin1k_48k.pcm",
        "../../../streams/mdrc/streams/stereo_48k.pcm",
        "../../../streams/mdrc/streams/stereo_48k.pcm"
    );
    @referencelist=(
        "../../../streams/mdrc/streams/ref_op_IIR_x86_mmdsp_sin1k_stereo_48k.pcm",
        "../../../streams/mdrc/streams/ref_op_IIR_x86_mmdsp_sin1k_mono_48k.pcm",
        "../../../streams/mdrc/streams/ref_op_FIR_stereo_48k_nmf.pcm",
        "../../../streams/mdrc/streams/ref_op_IIR_stereo_48k_nmf.pcm"
    );
    @cmdlist =(
        "../../../streams/mdrc/streams/params_IIR_sin1k_stereo_48k",
        "../../../streams/mdrc/streams/params_IIR_sin1k_mono_48k",
        "../../../streams/mdrc/streams/params_FIR_stereo_48k",
        "../../../streams/mdrc/streams/params_IIR_stereo_48k"
    );
    @chanlist =(
        "2",
        "1",
        "2",
        "2"
    );
    @fslist =(
        "48000",
        "48000",
        "48000",
        "48000"
    );
    @decoutputlist=(
        "IIR_sin1k_stereo_48k_$PROCESSOR.pcm",
        "IIR_sin1k_mono_48k_$PROCESSOR.pcm",
        "FIR_stereo_48k_$PROCESSOR.pcm",
        "IIR_stereo_48k_$PROCESSOR.pcm"
    );

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
    $chan=$chanlist[$iter];
    $fs=$fslist[$iter];
    $decoutput=$decoutputlist[$iter];
    $iter++;
    if($useMmdspsim){
        mmaudio::genSimpleSimCmdfile($simcmd, $exe, "$cmd $input $decoutput $chan $fs 48");
        mmtest::system("mmdspsim -c $simcmd",0, "execution: decode $cmd $input $decoutput $chan $fs 48");
    }
    else{
        mmtest::system("$exe.exe $cmd $input $decoutput $chan $fs 48",0, "execution: $exe $cmd $input $decoutput $chan $fs 48");
    }
    mmtest::system("$cmp $decoutput $reference", 0, "$cmp $decoutput with $reference");
}
mmtest::concl();

