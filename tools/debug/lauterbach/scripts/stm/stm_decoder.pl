#!/usr/bin/perl -w
################################################################################
# stm_decoder.pl
# ==============
# Author:  P. Langlais (plang), <philippe PT langlais AT linaro PT org>
#
# STM trace interpreter/decoder
#
# Creation Date: 2011
# Last Change: Wed May 25 11:00 2011 CEST
################################################################################
use Getopt::Std;
use FindBin;
use threads;
use threads::shared;
use Term::ANSIColor qw(:constants);
$Term::ANSIColor::AUTORESET = 1;
use lib "$FindBin::Bin";  # To find perl modules in bin dir
use File::Temp qw(tempfile tempdir);

# Import submodules
use PRCMU;
use Ftrace;
use ContextSwitch;

$VERSION="1.0b1";

$USAGE=<<END;
Usage of STE STM trace interpreter/decoder:
$0
    [-v]          : verbose
    [-p pipeName] : use a named pipe
    [-g]          : force GUI
    (input STM raw log file| no parameter for GUI)
    (input PRCMU header file)
    (input kernel directory)
END

# GLobal Variables
# -----------------
use vars qw($OS_win);
use vars qw($opt_h $opt_p $opt_g $pipeName $input);
use vars qw($inputFile $outputFile $consoleChannel $logType $t0);
use vars qw($MW $exePath $optionsFile $rootPath);
use vars qw(%midDisp %channelMidDisp);

my $stop : shared;

# Data packet channels buffers / masters (hash key $masterID#$channelNo)
my %channelsBufs;

use subs qw(myDie dprint dprintf ReadSTMoptions SetSTMoption);

# input log type
use constant FILE=>0;
use constant PIPE=>1;
use constant SOCKET=>2;

# Specific channel Numbers
use constant MMIO_TRACE_CHANNEL=>244;
use constant FUNC_GRAPH_CHANNEL=>245;
use constant TRACE_BRANCH_CHANNEL=>246;
use constant BLK_DEV_IO_TRACE_CHANNEL=>247;
use constant TRACE_BPRINTK_CHANNEL=>248;
use constant TRACE_PRINTK_CHANNEL=>249;
use constant USER_STACK_TRACE_CHANNEL=>250;
use constant STACK_TRACE_CHANNEL=>251;
use constant WAKEUP_CHANNEL=>252;
use constant CTX_SWITCH_CHANNEL=>253;
use constant FTRACE_CHANNEL=>254;
use constant CONSOLE_CHANNEL=>255;
use constant NMF_MPCEE_CHANNEL=>100;
use constant NMF_CM_CHANNEL=>101;
use constant NMF_HOSTEE_CHANNEL=>151;

# STM master IDs list
use constant C_A9_0=>0;
use constant C_A9_1=>1;
use constant SVA=>2;
use constant SIA=>3;
use constant SIA_XP70=>4;
use constant PRCMU=>5;
use constant MCSBAG=>9;
%MasterIDs = (
	&C_A9_0=>"Cortex_A9.0",
	&C_A9_1=>"Cortex_A9.1",
	&SVA=>"SVA.MMDSP",
	&SIA=>"SIA.MMDSP",
	&SIA_XP70=>"SIA.XP70",
	&PRCMU=>"PRCMU",
	&MCSBAG=>"MCSBAG"
);

# DataTypes for STM trace
my %stmDataTypes = (
	0x02=>"OVF",
	0x04=>"D8", 0x05=>"D16", 0x06=>"D32", 0x07=>"D64",
	0x14=>"D8MTS", 0x15=>"D16MTS", 0x16=>"D32MTS", 0x17=>"D64MTS",
	0x22=>"OVF",
	0x24=>"D8M", 0x25=>"D16M", 0x26=>"D32M", 0x27=>"D64M",
);

@protocols = qw(RAW OST_XTIV3);

#==============================================================================
# BEGIN
#==============================================================================
BEGIN
{
	$OS_win = ($^O =~ /^MSWin/) ? 1 : 0;
	if ($OS_win) {
		eval "use Win32::Pipe"; $noPipe = 1 if $@;
	}
	eval "use Tk"; $noGUI = 1 if $@;
	eval "use Tk::widgets qw(Dialog ErrorDialog ROText BrowseEntry LabFrame Balloon chooseDirectory)";
} # End BEGIN

#==============================================================================
# Main
#==============================================================================

die $USAGE if (!Getopt::Std::getopts('hvp:g') || $opt_h || $#ARGV>2);

$exePath = $0;
$exePath =~ tr,\\,/,;
$exePath =~ s,[^/]+$,,;

# Initializations
# ---------------
# Read STM Decoder config options
$optionsFile = $OS_win ? "$ENV{USERPROFILE}/STMoptions.cfg" : "$ENV{HOME}/.STMoptions.cfg";
print STDERR "Options file: $optionsFile\n";
ReadSTMoptions($optionsFile);
# Overwrite parameter if given in cmdline
$inputFile = $ARGV[0] if ($#ARGV == 0);;
$prcmuFile = $ARGV[1] if ($#ARGV == 1);;
$kernelDir = $ARGV[2] if ($#ARGV == 2);;

if (defined $opt_p) {
	$pipe = CreatePipe($opt_p);
	$startAcq = 1;
	$logType = PIPE;
	$input = "$pipe";
	printf STDERR ("Read named pipe: %s\n", $pipe);
} else {
	$startAcq = 0;
	$logType = FILE;
	$input = "$inputFile";
	printf STDERR ("Read logfile: %s\n", $inputFile);
}

PRCMU::Init("$prcmuFile");
Ftrace::Init("$kernelDir");
ContextSwitch::Init();

if (scalar(@ARGV) == 0 || defined $opt_g)  # No args => GUI
{
	if (defined $noGUI)
	{
		warn $USAGE;
		die("NB: to have GUI you need to install Tk module in your PERL install\n");
	}
	StartGUI();
}
else
{
	# Command line interface
	# Reset all filters to ON
	foreach my $mid (keys(%MasterIDs)) {
		$midDisp{$mid} = 1;
		for (my $i=0; $i<256; $i++) {
			$channelMidDisp{$MasterIDs{$mid}."#$i"} = 1;
		}
	}

	Stm_decode_on_stdout($input, $inputFile, $logType);
	exit 0;
}

#==============================================================================
# Subroutines
#==============================================================================
sub Stm_record_decoder
{
	my ($record) = @_;
	my ($tsLow, $tsMid, $tsHigh, $dt, $ch, $mid, $data);
	my ($ts, $dispTS);

	if ($protocol eq "RAW") { # Use by Lauterbach combiprobe
		my ($tsu);
		($tsu, $tsLow, $tsMid, $tsHigh, $dt, $ch, $mid, $data)
			= unpack("CVvCCCCa*", $record);

		if (!defined($dt) || !exists($stmDataTypes{$dt})) {
			warn "Bad STM $protocol trace format\n";
			return undef;
		}

		if ($stmDataTypes{$dt} eq "OVF") {
			return RED "STM TRACE OVERFLOW\n";
		}
	}
	else { # OST XTIv3 FIDO protocol
		my $port;
		($port, $mid, $ch, $tsHigh, $tsLow, $data) = unpack("CCCNNa*", $record);
		if (length($data) != (length($record)-11)) {
			warn "Bad STM $protocol trace format (bad length)\n";
			return undef;
		}
	}
	return undef unless $midDisp{$mid}; # MasterID filter
	return undef unless $channelMidDisp{$MasterIDs{$mid}."#$ch"}; # Channel filter

	# Time stamp given ?
	if ($protocol eq "RAW") {
		if (($dt & 0x10) || $mid == PRCMU) {
			$ts = ($tsLow + ($tsMid*2**32) + ($tsHigh*2**48)) * 0.000000020;
			$t0 = $ts if (!defined($t0));
			dprintf("%016.9fs %6s %11s:%02X : %s\n", $ts-$t0,
				$stmDataTypes{$dt}, $MasterIDs{$mid}, $ch, $data);
			$dispTS = sprintf("[%016.9f %1d:%02X]", $ts-$t0, $mid, $ch);
		}
	}
	else { # OST XTIv3 Fido protocol
		$ts = ($tsLow + (($tsHigh & 0x0FFFFFFF) * 2**32)) * 0.000000001;
		$t0 = $ts if (!defined($t0));
		dprintf("%016.9fs %11s:%02X : %s\n", $ts-$t0,
			$MasterIDs{$mid}, $ch, $data);
		$dispTS = sprintf("[%016.9f %1d:%02X]", $ts-$t0, $mid, $ch);
	}

	# Accumulate data till data with time stamp => end of packet
	$channelsBufs{"$mid#$ch"} .= $data;

	return undef unless defined $dispTS;

	# Now decode the packet
	$data = $channelsBufs{"$mid#$ch"};
	delete $channelsBufs{"$mid#$ch"}; # Reset buffer

	my $dispData = $dispTS;

	# Display format depend on Master ID & channelID
	if ($mid == C_A9_0 || $mid == C_A9_1) {
		$ch += 256 if ($mid == C_A9_1); # channel number > 256 for A9#1

		# Workaround bad channels #, try recognizion/content
		if (length($data) == 8 && substr($data, 7, 1) eq chr(0xC0)) {
			if (substr($data, 3, 1) eq chr(0xC0)) {
				$ch = $ftraceChannel;
			}
			elsif (substr($data, 3, 1) eq chr(0x00)) {
				$ch = $funcGraphChannel;
			}
		}
		elsif (length($data) == 28 && substr($data, 7, 1) eq chr(0xC0)
				&& substr($data, 3, 1) eq chr(0x00)) {
				$ch = $funcGraphChannel;
		}
		elsif (length($data) >= 12 && (substr($data, 7, 1) eq chr(0xC0))) {
			printf "size %d\n", length($data);
			$ch = $traceBprintkChannel;
		}
		elsif (length($data) >= 4 && (substr($data, 3, 1) eq chr(0xC0))) {
			$ch = $tracePrintkChannel;
		}
		elsif (length($data) == 16 && $data =~ /[\000\001]/) {
			$ch = $ctxtSwitchChannel; # or $wakeupChannel
		}
		# end workaround

		if ($ch == $consoleChannel) {
			$dispData .= $data;
		}
		elsif ($ch == $ftraceChannel) {
			$dispData .= Ftrace::Decode($data);
		}
		elsif ($ch == $funcGraphChannel) {
			$dispData .= Ftrace::GraphDecode($data);
		}
		elsif ($ch == $ctxtSwitchChannel) {
			$dispData .= ContextSwitch::Decode($data);
		}
		elsif ($ch == $wakeupChannel) {
			$dispData .= ContextSwitch::Decode($data, "+");
		}
		elsif ($ch == $stackTraceChannel) {
			$dispData .= Ftrace::DecodeStack($data);
		}
		elsif ($ch == $tracePrintkChannel) {
			my ($caller, $string) = unpack("VA", $data);
			my ($symb, $off) = Ftrace::FindNearestSymbol($caller);
			$dispData .= $symb.(($off!=0) ? sprintf("+0x%X: ", $off) : ": ")
						.$string;
		}
		elsif ($ch == $traceBprintkChannel) {
			# FIXME doesn't works for 64 bits data type, needs to parse format
			my ($fmtAddr, $caller, @args) = unpack("VVV*", $data);
			my ($symb, $off) = Ftrace::FindNearestSymbol($caller);
			$dispData .= $symb.(($off!=0) ? sprintf("+0x%X-> ", $off) : "-> ")
						.sprintf(Ftrace::FindString($fmtAddr), @args);
		}
		else {
				$dispData .= $data;
		}
	}
	elsif ($mid == PRCMU) {
		$dispData .= PRCMU::Decode($data)."\n";
	}
	elsif ($mid == MCSBAG) {
	        # only display trace content in hexa
	        $data =~ s/(.|\n)/ sprintf("%02x ",ord $1)/eg;
		$dispData .= "SBAG=".$data."\n";
	}
	else {
		$dispData .= $data;
	}
	return $dispData;
}

# STM trace decoder from pipe or file output to STDOUT
sub Stm_decode_on_stdout
{
	my ($input, $rawFile, $type) =@_;
	my ($record, $data, $size);
	undef $t0; # Reset initial timestamp

	if (! -e "$input") {
		Display("Pipe or File $input doesn't exist");
		return;
	}

	open(LF, "+<$input") || die "Can't open $input: $!\n";
	binmode(LF);

	if ($type != FILE) {
		# Save in STM raw log input file
		rename $rawFile, "$rawFile.old" if (-f $rawFile);
		open (RAW, ">$rawFile");
		binmode RAW;
		RAW->autoflush(1);
	}
	if ($protocol eq "RAW") {
		while (read(LF, $size, 1))
		{
			last if (defined $stop);
			$size = ord($size);
			if ($size) {
				read LF, $record, $size;
				print RAW chr($size).$record if ($type != FILE);
				my $dispData = Stm_record_decoder($record);
				print $dispData if defined $dispData;
			}
		}
	}
	else { # OST XTIv3 FIDO protocol
		while (read(LF, $data, 4))
		{
			last if (defined $stop);
			my ($v, $entityID, $protID, $size) =  unpack("CCCC", $data);
			if ($v != 0x10 && $entityID !=0 && $protID != 0x84) {
				warn "Bad STM $protocol trace format\n";
				last;
			}
			print RAW $data if ($type != FILE);
			if ($size == 0) {
				if (read(LF, $data, 4) == 4) {
					($size) = unpack("V", $data);
					print RAW $data if ($type != FILE);
				}
				else {
					last;
				}
			}
			read LF, $record, $size;
			print RAW $record if ($type != FILE);
			my $dispData = Stm_record_decoder($record);
			print $dispData if defined $dispData;
		}
	}
	undef $stop;
	close(LF);
	close RAW if ($type != FILE);
}

# STM trace decoder from file to file
sub Stm_decode
{
	my ($input, $output) = @_;
	my ($record, $size);
	undef $t0; # Reset initial timestamp

	if (! -f "$input") {
		Display("File $input doesn't exist");
		return;
	}

	rename $output, "$output.old" if (-f $output);
	open(OUTPUT, ">$output") || myDie "Can't create $output\n";
	open(LF, "+<$input") || myDie "Can't open $input\n";
	binmode(LF);

	if ($protocol eq "RAW") {
		while (read(LF, $size, 1))
		{
			$size = ord($size);
			if ($size) {
				read LF, $record, $size;
				my $dispData = Stm_record_decoder($record);
				print OUTPUT "$dispData" if (defined $dispData);
			}
		}
	}
	else { # OST XTIv3 FIDO protocol
		while (read(LF, $data, 4))
		{
			last if (defined $stop);
			my ($v, $entityID, $protID, $size) =  unpack("CCCC", $data);
			if ($v != 0x10 && $entityID !=0 && $protID != 0x84) {
				warn "Bad STM $protocol trace format\n";
				last;
			}
			if ($size == 0) {
				if (read(LF, $data, 4) == 4) {
					($size) = unpack("V", $data);
				}
				else {
					last;
				}
			}
			read LF, $record, $size;
			my $dispData = Stm_record_decoder($record);
			print OUTPUT "$dispData" if (defined $dispData);
		}
	}
	close(LF);
	close OUTPUT;

	if ((stat($output))[7] != 0) {
		ViewFile("$output");
		DisplayStatus("Result in $output");
	}
	else {
		DisplayStatus("No data recorded");
	}
	return;
}

sub Stm_thread_decode
{
	my ($input, $inputFile, $logType) = @_;
	print STDERR "Decoder thread started\n";
	Stm_decode_on_stdout($input, $inputFile, $logType);
	print STDERR "Decoder thread stopped\n";
}

sub StartAcquisition
{
	$input = CreatePipe($pipeName);
	DisplayStatus("Acquisition active on pipe $input");
	# launch background decoder job
	$thr = threads->new(\&Stm_thread_decode, $input, $inputFile, $logType);
	$thr->detach;
}

# GUI entry point
sub StartGUI
{
	$opt_g=1; # We use GUI now
	# Build Main window
	# ------------------
	$MW = Tk::MainWindow->new;
	$MW->optionAdd('*BorderWidth' => 1);

	# Create specific font and tags
	#$MW->fontCreate(C_small, -size=>9);
	$MW->fontCreate(C_bold, -size=>10, -weight=>"bold");
	if ($OS_win)
	{
		$MW->fontCreate(C_fix, -family=>Terminal, -size=>9);
	}
	else
	{
		$MW->fontCreate(C_fix, -family=>Monospace, -size=>9);
	}

	$MW->configure(-menu => my $menubar = $MW->Menu);
	$MW->title("STEricsson Ux500 STM trace decoder - Version: $VERSION");

	# Create balloon widget
	my $B = $MW->Balloon();

	# Menu File
	$menuFile = $menubar->Menu();
	$menubar->add("cascade", -label=>File, -underline=>0, -menu=>$menuFile);
	$menuFile->add("command", -label => 'Set log File', -command=>\&SetLogFile, -underline=>0);
	$menuFile->add("separator");
	for ( my $i = 0; $i < scalar(@oldOpenFiles) ; $i++) {
		my $file = $oldOpenFiles[$i];
		if (-f $file) {
			$menuFile->add("command", -label => $file,
				-command=>sub {
					$inputFile = $input = $file;
					SetSTMoption("inputFile");
					$bFile->configure(-text=>"$inputFile");
				}
				, -underline=>0);
		}
	}

	# Menu Help
	my $help = $menubar->cascade(qw/-label Help -underline 0 -menuitems/ =>
		[ [command    => '~About'], ]);

	# Create all the dialogs here
	my $DIALOG_ABOUT = $MW->Dialog(
		-title			=> 'About STM Decoder',
		-bitmap			=> 'info',
		-default_button => 'OK',
		-buttons		=> ['OK'],
		-justify		=> 'center',
		-text			=> "STM Trace Decoder\n".
		"for ST-Ericsson UX500 platforms\n\n" .
		"Author: P. Langlais\n" .
		"Contributors: S. Barandao\n\n" .
		"Version $VERSION\n\n" .
		"2011"
	);

	$help->cget(-menu)->entryconfigure('About',
		-command => [$DIALOG_ABOUT => 'Show'],
	);

	# main, content & info frames
	$fmain = $MW->Frame->grid(qw/-sticky nsew/);
	$fcontent = $MW->Frame->grid(qw/-sticky nsew/);
	$finfo = $MW->Frame->grid(qw/-sticky nsew/);

	# Control/Config frame
	my $fcontrol = $fmain->Labelframe(-text => "Control/Config",
		-foreground=>'brown',-font=>C_bold,
		-pady=>2, -padx=>2)->pack(-fill=>'both', -side=>'top');

	my(@pl) = qw/-padx 1 -side left -expand both/;
	$bFile = $fcontrol->Button(-text=>"Input: $inputFile",
		-command=>\&SetLogFile)->pack(-fill=>'both', -side=>'top');
	$B->attach($bFile, -balloonposition=>'mouse', -msg=>"Click to change input file");

	$bOutput = $fcontrol->Button(-text=>"Output: $outputFile",
		-command=>\&SetOutputFile)->pack(-fill=>'both', -side=>'top');
	$B->attach($bOutput, -balloonposition=>'mouse', -msg=>"Click to change output logfile");

	$bKernel = $fcontrol->Button(-text=>"Kernel root: $kernelDir",
		-command=>\&SetKernelDir)->pack(-fill=>'both', -side=>'top');
	$B->attach($bKernel, -balloonposition=>'mouse', -msg=>"Click to change kernel root directory");

	my $fFilter = $fcontrol->Labelframe(-text => "Display",
		-foreground=>'brown',
		-font=>C_bold, -pady=>2, -padx=>2)->pack(-fill=>'both', -side=>'top');
	$fFilter->Checkbutton(-variable=>\$midDisp{&C_A9_0},
		-command=>sub {
			SetSTMoption("C_A9_0", $midDisp{&C_A9_0});
			$midDisp{&C_A9_1} = $midDisp{&C_A9_0};
			SetSTMoption("C_A9_1", $midDisp{&C_A9_1});
			if ($midDisp{&C_A9_0}) {
				$bMid{&C_A9_0}->configure(-state=>"normal");
			}
			else {
				$bMid{&C_A9_0}->configure(-state=>"disabled");
			}
		})->pack(@pl);
	$bMid{&C_A9_0} = $fFilter->Button(-text=>'Cortex-A9s',
		-command=>sub {ChannelsConfig(C_A9_0)})->pack(@pl);

	$fFilter->Checkbutton(-variable=>\$midDisp{&PRCMU},
		-command=>sub {
			SetSTMoption("PRCMU", $midDisp{&PRCMU});
			if ($midDisp{&PRCMU}) {
				$bMid{&PRCMU}->configure(-state=>"normal");
			}
			else {
				$bMid{&PRCMU}->configure(-state=>"disabled");
			}
		})->pack(@pl);
	$bMid{&PRCMU} = $fFilter->Button(-text=>'PRCMU',
		-command=>sub {ChannelsConfig(PRCMU)})->pack(@pl);

	$fFilter->Checkbutton(-variable=>\$midDisp{&SVA},
		-command=>sub {
			SetSTMoption("SVA", $midDisp{&SVA});
			if ($midDisp{&SVA}) {
				$bMid{&SVA}->configure(-state=>"normal");
			}
			else {
				$bMid{&SVA}->configure(-state=>"disabled");
			}
		})->pack(@pl);
	$bMid{&SVA} = $fFilter->Button(-text=>'SVA', 
		-command=>sub {ChannelsConfig(SVA)})->pack(@pl);

	$fFilter->Checkbutton(-variable=>\$midDisp{&SIA},
		-command=>sub {
			SetSTMoption("SIA", $midDisp{&SIA});
			if ($midDisp{&SIA}) {
				$bMid{&SIA}->configure(-state=>"normal");
			}
			else {
				$bMid{&SIA}->configure(-state=>"disabled");
			}
		})->pack(@pl);
	$bMid{&SIA} = $fFilter->Button(-text=>'SIA', 
		-command=>sub {ChannelsConfig(SIA)})->pack(@pl);

	$fFilter->Checkbutton(-variable=>\$midDisp{&SIA_XP70},
		-command=>sub {
			SetSTMoption("SIA_XP70", $midDisp{&SIA_XP70});
			if ($midDisp{&SIA_XP70}) {
				$bMid{&SIA_XP70}->configure(-state=>"normal");
			}
			else {
				$bMid{&SIA_XP70}->configure(-state=>"disabled");
			}
		})->pack(@pl);
	$bMid{&SIA_XP70} = $fFilter->Button(-text=>'SIA.XP70', 
		-command=>sub {ChannelsConfig(SIA_XP70)})->pack(@pl);

	$fFilter->Checkbutton(-variable=>\$midDisp{&MCSBAG},
		-command=>sub {
			SetSTMoption("MCSBAG", $midDisp{&MCSBAG});
			if ($midDisp{&MCSBAG}) {
				$bMid{&MCSBAG}->configure(-state=>"normal");
			}
			else {
				$bMid{&MCSBAG}->configure(-state=>"disabled");
			}
		})->pack(@pl);
	$bMid{&MCSBAG} = $fFilter->Button(-text=>'MCSBAG', 
		-command=>sub {ChannelsConfig(MCSBAG)})->pack(@pl);

	foreach my $mid (keys(%MasterIDs)) {
		next if ($mid == C_A9_1);
		if ($midDisp{$mid}) {
				$bMid{$mid}->configure(-state=>"normal");
		}
		else {
				$bMid{$mid}->configure(-state=>"disabled");
		}
	}

	$fcontrol->Radiobutton(-text=>'File', -variable=>\$logType,
		-command=> sub {
			$bAcqStart->configure(-state=>"disabled");
			$bAcqStop->configure(-state=>"disabled");
			$bDecode->configure(-state=>"normal");
		},
		-value=>&FILE)->pack(@pl);
	$bDecode = $fcontrol->Button(-text=>"DECODE",
		-command=>sub {Stm_decode($inputFile, $outputFile)})->pack(@pl);
	$B->attach($bDecode, -balloonposition=>'mouse', -msg=>"Click to decode STM log file");

	$fcontrol->Radiobutton(-text=>'Pipe', -variable=>\$logType,
		-command=> sub {
			$bAcqStart->configure(-state=>"normal");
			$bAcqStop->configure(-state=>"normal");
			$bDecode->configure(-state=>"disabled");
		},
		-value=>&PIPE)->pack(@pl);
	$fcontrol->Entry(-width=>8, -textvariable=>\$pipeName,
		-validate        => 'focus',
		-validatecommand=>sub {SetSTMoption("pipeName")})->pack(@pl);
	# Acquisition subframe
	my $facq = $fcontrol->Labelframe(-text => "Acquisition",
		-font=>C_bold, -pady=>2, -padx=>2)->pack(@pl);
	$bAcqStop = $facq->Radiobutton(-text=>'STOP', -variable=>\$startAcq,
		-command=> sub {
			$stop = 1;
			DisplayStatus("Acquisition stopped");
		},
		-value=>0)->pack(@pl);
	$bAcqStart = $facq->Radiobutton(-text=>'START', -variable=>\$startAcq,
		-command=>\&StartAcquisition,
		-value=>1)->pack(@pl);

	$protMenu = $fcontrol->Optionmenu(-options =>[sort(@protocols)],
		-variable=>\$protocol,
		-command=>sub {
				SetSTMoption("protocol");
		}
	)->pack(@pl, -anchor=>'e');
	$B->attach($protMenu, -balloonposition=>'mouse',
		-msg=>"Select your trace protocol here");

	$bExit = $fcontrol->Button(-text=>"EXIT",
		-command=>\&exit)->pack(@pl, -anchor=>'e');
	$B->attach($bExit, -balloonposition=>'mouse', -msg=>"Click to Exit");

	if ($logType == PIPE) {
		$bAcqStart->configure(-state=>"normal");
		$bAcqStop->configure(-state=>"normal");
	}
	else {
		$bAcqStart->configure(-state=>"disabled");
		$bAcqStop->configure(-state=>"disabled");
	}

	# APE / Cortex-A9 frame
	my @popt = qw(-side left -expand 1 -fill x);
	my $fape = $fcontent->Labelframe(-text => "APE / Cortex-A9s",
		-foreground=>'brown',
		-font=>C_bold, -pady=>2, -padx=>2)
			->grid(qw/-sticky nsew -column 0 -row 0/);
	# Console subframe
	my $fconsole = $fape->Labelframe(-text => "Console", -foreground=>'blue',
		-font=>C_bold, -pady=>2, -padx=>2)->grid(qw/-sticky nsew -column 0 -row 0/);
	my $fsubconsDisp = $fconsole->Frame->pack(@l);
	$fsubconsDisp->Checkbutton(-text=>'Show console',
		-variable=>\$channelMidDisp{$MasterIDs{&C_A9_0}."#$consoleChannel"},
		-command=>sub {SetSTMoption($MasterIDs{&C_A9_0}."#$consoleChannel", 
			$channelMidDisp{$MasterIDs{&C_A9_0}."#$consoleChannel"}
		)})->pack(@popt);
	$fconsole->Label(-text=>"Channel: ")->pack(@popt);
	$fconsole->Entry(-width=>3, -textvariable=>\$consoleChannel,
		-validate        => 'focus',
		-validatecommand=>sub {SetSTMoption("consoleChannel")})->pack(@popt);
	# trace_printk subframe
	my $ftracePrintk = $fape->Labelframe(-text => "trace_printk", -foreground=>'blue',
		-font=>C_bold, -pady=>2, -padx=>2)->grid(qw/-sticky nsew -column 1 -row 0/);
	my $fsubtrPrintk = $ftracePrintk->Frame->pack(@l);
	$fsubtrPrintk->Checkbutton(-text=>'Show trace_printk',
		-variable=>\$channelMidDisp{$MasterIDs{&C_A9_0}."#$tracePrintkChannel"},
		-command=>sub {
			SetSTMoption($MasterIDs{&C_A9_0}."#$tracePrintkChannel", 
				$channelMidDisp{$MasterIDs{&C_A9_0}."#$tracePrintkChannel"});
			SetSTMoption($MasterIDs{&C_A9_0}."#$traceBprintkChannel", 
				$channelMidDisp{$MasterIDs{&C_A9_0}."#$tracePrintkChannel"});
		})->pack(@popt);
	$ftracePrintk->Label(-text=>"Channel printk: ")->pack(@popt);
	$ftracePrintk->Entry(-width=>3, -textvariable=>\$tracePrintkChannel,
		-validate        => 'focus',
		-validatecommand=>sub {SetSTMoption("tracePrintkChannel")})->pack(@popt);
	$ftracePrintk->Label(-text=>"Channel bprintk: ")->pack(@popt);
	$ftracePrintk->Entry(-width=>3, -textvariable=>\$traceBprintkChannel,
		-validate        => 'focus',
		-validatecommand=>sub {SetSTMoption("traceBprintkChannel")})->pack(@popt);
	# Ftrace subframe
	my $ftrace = $fape->Labelframe(-text => "Ftrace", -foreground=>'blue',
		-font=>C_bold, -pady=>2, -padx=>2)->grid(qw/-sticky nsew -column 2 -row 0/);
	my $fsubftraDisp = $ftrace->Frame->pack(@l);
	$fsubftraDisp->Checkbutton(-text=>'Show ftrace',
		-variable=>\$channelMidDisp{$MasterIDs{&C_A9_0}."#$ftraceChannel"},
		-command=>sub {SetSTMoption($MasterIDs{&C_A9_0}."#$ftraceChannel", 
			$channelMidDisp{$MasterIDs{&C_A9_0}."#$ftraceChannel"}
		)})->pack(@popt);
	$ftrace->Label(-text=>"Channel: ")->pack(@popt);
	$ftrace->Entry(-width=>3, -textvariable=>\$ftraceChannel,
		-validate        => 'focus',
		-validatecommand=>sub {SetSTMoption("ftraceChannel")})->pack(@popt);
	# Context Switch subframe
	my $fCtxSwitch = $fape->Labelframe(-text => "Context Switch", -foreground=>'blue',
		-font=>C_bold, -pady=>2, -padx=>2)->grid(qw/-sticky nsew -column 0 -row 1/);
	my $fsubCtxDisp = $fCtxSwitch->Frame->pack(@l);
	$fsubCtxDisp->Checkbutton(-text=>'Show Context Switch',
		-variable=>\$channelMidDisp{$MasterIDs{&C_A9_0}."#$ctxtSwitchChannel"},
		-command=>sub {
			SetSTMoption($MasterIDs{&C_A9_0}."#$ctxtSwitchChannel", 
				$channelMidDisp{$MasterIDs{&C_A9_0}."#$ctxtSwitchChannel"});
			SetSTMoption($MasterIDs{&C_A9_0}."#$wakeupChannel", 
				$channelMidDisp{$MasterIDs{&C_A9_0}."#$ctxtSwitchChannel"});
		})->pack(@popt);
	$fCtxSwitch->Label(-text=>"Channel Ctx: ")->pack(@popt);
	$fCtxSwitch->Entry(-width=>3, -textvariable=>\$ctxtSwitchChannel,
		-validate        => 'focus',
		-validatecommand=>sub {SetSTMoption("ctxtSwitchChannel")})->pack(@popt);
	$fCtxSwitch->Label(-text=>"Channel Wakeup: ")->pack(@popt);
	$fCtxSwitch->Entry(-width=>3, -textvariable=>\$wakeupChannel,
		-validate        => 'focus',
		-validatecommand=>sub {SetSTMoption("wakeupChannel")})->pack(@popt);
	# Stack trace subframe
	my $fStackTrace = $fape->Labelframe(-text => "Stack Trace", -foreground=>'blue',
		-font=>C_bold, -pady=>2, -padx=>2)->grid(qw/-sticky nsew -column 1 -row 1/);
	my $fsubStackDisp = $fStackTrace->Frame->pack(@l);
	$fsubStackDisp->Checkbutton(-text=>'Show Stack Trace',
		-variable=>\$channelMidDisp{$MasterIDs{&C_A9_0}."#$stackTraceChannel"},
		-command=>sub {SetSTMoption($MasterIDs{&C_A9_0}."#$stackTraceChannel", 
			$channelMidDisp{$MasterIDs{&C_A9_0}."#$stackTraceChannel"}
		)})->pack(@popt);
	$fStackTrace->Label(-text=>"Channel: ")->pack(@popt);
	$fStackTrace->Entry(-width=>3, -textvariable=>\$stackTraceChannel,
		-validate        => 'focus',
		-validatecommand=>sub {SetSTMoption("stackTraceChannel")})->pack(@popt);
	# NMF subframe
	my $fnmf = $fape->Labelframe(-text => "NMF", -foreground=>'blue',
		-font=>C_bold, -pady=>2, -padx=>2)->grid(qw/-sticky nsew -column 2 -row 1/);
	my $fsubNmfDisp = $fnmf->Frame->pack(@l);
	$fsubNmfDisp->Checkbutton(-text=>'Display NMF data', -variable=>\$nmfDisp,
		-command=>sub {
			SetSTMoption("nmfDisp");
			SetSTMoption($MasterIDs{&C_A9_0}."#$nmfMpceeChannel", $nmfDisp);
			SetSTMoption($MasterIDs{&C_A9_0}."#$nmfCmChannel", $nmfDisp);
			SetSTMoption($MasterIDs{&C_A9_0}."#$nmfHosteeChannel", $nmfDisp);
		})->pack(@popt);
	my $fsubNmfMpcee = $fnmf->Frame->pack(@l);
	$fsubNmfMpcee->Label(-text=>"MPCEE Channel: ")->pack(@popt);
	$fsubNmfMpcee->Entry(-width=>3, -textvariable=>\$nmfMpceeChannel,
		-validate        => 'focus',
		-validatecommand=>sub {SetSTMoption("nmfMpceeChannel")})->pack(@popt);
	my $fsubNmfCm = $fnmf->Frame->pack(@l);
	$fsubNmfCm->Label(-text=>"CM Channel: ")->pack(@popt);
	$fsubNmfCm->Entry(-width=>3, -textvariable=>\$nmfCmChannel,
		-validate        => 'focus',
		-validatecommand=>sub {SetSTMoption("nmfCmChannel")})->pack(@popt);
	my $fsubNmfHostee = $fnmf->Frame->pack(@l);
	$fsubNmfHostee->Label(-text=>"HOSTEE Channel: ")->pack(@popt);
	$fsubNmfHostee->Entry(-width=>3, -textvariable=>\$nmfHosteeChannel,
		-validate        => 'focus',
		-validatecommand=>sub {SetSTMoption("nmfHosteeChannel")})->pack(@popt);

# TODO
#    # PRCMU frame
#    my $fprcmu = $fcontent->Labelframe(-text => "PRCMU",
#        -font=>C_bold, -pady=>2, -padx=>2)
#            ->grid(qw/-sticky nsew -column 1 -row 0/);
#
#    # SIA frame
#    my $fsia = $fcontent->Labelframe(-text => "SIA",
#        -font=>C_bold, -pady=>2, -padx=>2)
#            ->grid(qw/-sticky nsew -column 0 -row 1/);

#    # SVA frame
#    my $fsva = $fcontent->Labelframe(-text => "SVA",
#        -font=>C_bold, -pady=>2, -padx=>2)
#            ->grid(qw/-sticky nsew -column 1 -row 1/);

#    # SIA.XP70 frame
#    my $fsia_xp70 = $fcontent->Labelframe(-text => "SIA.XP70",
#        -font=>C_bold, -pady=>2, -padx=>2)
#            ->grid(qw/-sticky nsew -column 0 -row 2/);

#    # MCSBAG frame
#    my $fmcsbag = $fcontent->Labelframe(-text => "MCSBAG",
#        -font=>C_bold, -pady=>2, -padx=>2)
#            ->grid(qw/-sticky nsew -column 1 -row 2/);

	# main GUI loop
	Tk::MainLoop;
}

sub SetKernelDir
{
	my $dir = $MW->chooseDirectory(-initialdir=>"$kernelDir");
	if (defined $dir)
	{
		$dir =~ s,\\,/,g;  # Change to Perl format
		$kernelDir = $dir;
		SetSTMoption("kernelDir");
		Ftrace::Init("$kernelDir");
		$bKernel->configure(-text=>"Kernel root: $kernelDir");
	}
}

sub SetOutputFile
{
	my $type = [['Decoded Log Files','*.log'],['Any Files','*.*']];
	my $file = $MW->getSaveFile(-title=>"Output log file",
			-filetypes=>$type, -initialdir=>"$ENV{HOME}/",
			-initialfile => 'output',
			-defaultextension => '.log');
	if (defined $file)
	{
		$file =~ s,\\,/,g;  # Change to Perl format
		$outputFile = $file;
		SetSTMoption("outputFile");
		$bOutput->configure(-text=>"Output: $outputFile");
	}
}

sub SetLogFile
{
	my $dir = $inputFile;
	my $file;
	$dir =~ s,/,\\,g if $OS_win;  # Change dir to DOS format
	$dir =~ s,[^/\\]+$,,;
	$dir = "." if ( !-d $dir);
	my $type = [['Binary Files','*.bin'],['Any Files','*.*']];
	$file = $MW->getOpenFile(-title=>"Open a raw log file", -filetypes=>$type, -initialdir=>"$dir");
	if (defined $file && -f $file)
	{
		$file =~ s,\\,/,g;  # Change dir to Perl format
		$input = $inputFile = $file;
		if (join(';',@oldOpenFiles) !~ /$file/)
		{
			unshift @oldOpenFiles, $file;
			undef $oldOpenFiles[10] if defined $oldOpenFiles[10];
			$menuFile->insert(3, "command", -label => $file,
				-command=>sub {
					$bFile->configure(-text=>"$inputFile");
				}
				, -underline=>0);
		}
		SetSTMoption("inputFile");
		SetSTMoption("oldOpenFiles");
		$bFile->configure(-text=>"Input: $inputFile");
	}
}

# Display Info or error
# ---------------------
sub DisplayStatus
{
	my ($text, $color) = @_;

	$color = "brown" unless defined $color;
	if ( defined $infoentry)
	{
		destroy $infoentry;
		undef $infoentry;
	}
	if (defined $text)
	{
		$infoentry = $finfo->Label(-font=>C_bold, -fg=>$color, -text=>$text, -wraplength=>800)->pack;
	}
	$finfo->update; # Refresh screen
}

#==============================================================================
# Utilities
#==============================================================================
## Debug print
sub dprint
{
	print STDERR @_ if ( defined $opt_v );
}

sub dprintf
{
	printf STDERR @_ if ( defined $opt_v );
}

sub Display
{
	DisplayStatus (@_) if ( defined $MW );
	print STDERR ("@_\n");
}

sub myDie
{
	DisplayStatus (@_) if ( defined $MW );
	print STDERR ("@_\n");
	print STDERR "Press return to close or kill me...\n";
	my $input = <STDIN>;
	exit 1;
}

# Create given named pipe
sub CreatePipe
{
	my ($pipeName) = (@_);

	if ($OS_win && !defined($noPipe)) {
		# pipe name format on the same machine: \\.\PIPE\Pipe Name
		$pipeName =~ s,.*\\,,g;
		new Win32::Pipe($pipeName) || die "Can't Create Named Pipe\n";
		$pipe = "\\\\.\\PIPE\\$pipeName";
	}
	elsif (!$OS_win) { # on UNIX like system
		$pipe = "/tmp/$pipeName";
		if (!-p $pipe) {
			require POSIX;
			POSIX::mkfifo($pipe, 0666) || die "Can't mkfifo $pipe $!\n";
		}
	} else {
		die "Pipe not supported\n";
	}
	return $pipe;
}

# Configuration file functions
sub ReadSTMoptions
{
	if (open(OPTIONS,"<$_[0]"))
	{
		while (<OPTIONS>)
		{
			if (/^\s*([\w\#\.]+)\s*=\s*(.+)$/) # $1 = option name & $2 = value
			{
				my ($var, $val) = ($1, $2);
				dprint "Read STM decoder option: $var = $val\n";
				if ($var eq "oldOpenFiles")
				{
					@oldOpenFiles = split(';', $val);
				}
				else
				{
					# Special name with # => for %channelMidDisp
					if ($var =~ /\b([A-Z0-9\._]+)#(\d+)\b/) {
						$channelMidDisp{$var} = $val;
					}
					# Special case all uppercase var name => for %midDisp
					elsif ($var =~ /\b[A-Z0-9\._]+\b/) {
						$midDisp{&$var} = $val;
					}
					else {
						eval "\$$var = \"$val\"";
					}
				}
			}
		}
		close OPTIONS;
	}
	# Set default values
	if (!defined ($protocol)) { $protocol = "RAW"; }
	if (!defined ($inputFile)) { $inputFile = "trace.bin"; }
	if (!defined ($outputFile)) { $outputFile = "$ENV{HOME}/stm_output.log"; }
	if (!defined ($pipeName)) { $pipeName = "STMpipe"; }
	if (!defined ($consoleChannel)) { $consoleChannel = CONSOLE_CHANNEL; }
	if (!defined ($tracePrintkChannel)) { $tracePrintkChannel = TRACE_PRINTK_CHANNEL; }
	if (!defined ($traceBprintkChannel)) { $traceBprintkChannel = TRACE_BPRINTK_CHANNEL; }
	if (!defined ($ftraceChannel)) { $ftraceChannel = FTRACE_CHANNEL; }
	if (!defined ($funcGraphChannel)) { $funcGraphChannel = FUNC_GRAPH_CHANNEL; }
	if (!defined ($ctxtSwitchChannel)) { $ctxtSwitchChannel = CTX_SWITCH_CHANNEL; }
	if (!defined ($wakeupChannel)) { $wakeupChannel = WAKEUP_CHANNEL; }
	if (!defined ($stackTraceChannel)) { $stackTraceChannel = STACK_TRACE_CHANNEL; }
	if (!defined ($kernelDir)) { $kernelDir = "$ENV{ANDROID_BUILD_TOP}/out/target/product/u9540/obj/kernel"; }
	if (!defined ($console)) { $console = 0; }
	if (!defined ($nmfDisp)) { $nmfDisp = 0; }
	if (!defined ($nmfMpceeChannel)) { $nmfMpceeChannel = NMF_MPCEE_CHANNEL; }
	if (!defined ($nmfCmChannel)) { $nmfCmChannel = NMF_CM_CHANNEL; }
	if (!defined ($nmfHosteeChannel)) { $nmfHosteeChannel = NMF_HOSTEE_CHANNEL; }
	if (!defined ($prcmuDisp)) { $prcmuDisp = 0; }
	if (!defined ($prcmuFile)) { $prcmuFile = "$ENV{ANDROID_BUILD_TOP}/vendor/st-ericsson/boot/xloader/boot_images/xloader/9540a0_ste/src/numerics.h"; }

	# Set Default filters to ON for all MID & channels
	foreach my $mid (keys(%MasterIDs)) {
		for (my $i=0; $i<256; $i++) {
			if (! defined $channelMidDisp{$MasterIDs{$mid}."#$i"}) {
				$channelMidDisp{$MasterIDs{$mid}."#$i"} = 1;
			}
		}
	}
}

sub SetSTMoption
{
	my ($option, $value) = @_;
	local $/ = undef; # To read in one shot

	eval "\$$option = \$value" if (defined $value && $option !~ /#/);
	eval "\$value = \$$option" if !defined $value;
	my $content = "";
	my $newContent;

	if ( -f "$optionsFile" )
	{
		open(OPTIONS, "<$optionsFile");
		$content = <OPTIONS>;
		close OPTIONS;
	}
	if ($option eq "oldOpenFiles")
	{
		$newContent = "$option = ".join(';', @oldOpenFiles);
	}
	else
	{
		$newContent = "$option = $value";
	}
	if (! ($content =~ s,^\s*$option =.*$,$newContent,m))
	{
		$content .= "$newContent\n";
	}
	dprint ("Save option $content\n");
	open (OPTIONS, ">$optionsFile");
	print OPTIONS $content;
	close OPTIONS;
}

# View or edit file content functions
# ------------------------------------
sub ViewFile 
{
    # View a file content

    my($file) = @_;

    if (not Exists $VIEW) {
        $VIEW = $MW->Toplevel;
        $VIEW->iconname('file');
        my $view_buttons = $VIEW->Frame;
        $view_buttons->pack(qw/-side bottom -expand 1 -fill x/);
        my $view_buttons_dismiss = $view_buttons->Button(
            -text    => 'Dismiss',
            -command => [$VIEW => 'withdraw'],
        );
        $view_buttons_dismiss->pack(qw/-side left -expand 1/);
        $VIEW_TEXT = $VIEW->Scrolled('Text',
            qw/-scrollbars e -height 40 -setgrid 1/);
        $VIEW_TEXT->pack(qw/-side left -expand 1 -fill both/);
    } else {
        $VIEW->deiconify;
        $VIEW->raise;
    }
    $VIEW->title("File: $file");
    $VIEW_TEXT->configure(-state=>'normal');
    $VIEW_TEXT->delete(qw/1.0 end/);
    open(VIEW, "<$file") or warn "Cannot open file $file: $!";
    {
        local $/ = undef;
        $VIEW_TEXT->insert('1.0', <VIEW>);
    }
    close VIEW;
    $VIEW_TEXT->markSet(qw/insert 1.0/);
    $VIEW_TEXT->configure(-state=>'disabled');

} # end ViewFile


# Select/Unselect channels
# ------------------------
sub ChannelsConfig 
{
    my ($mid) = @_;

	my(@pl) = qw/-padx 1 -side left -expand both/;
    if (not Exists $CHANNELS) {
        $CHANNELS = $MW->Toplevel;
        $CHANNELS->iconname('STM Channels');
        my $channels_buttons = $CHANNELS->Frame;
        $channels_buttons->pack(qw/-side bottom -expand 1 -fill x/);
        $channels_buttons->Button(-text    => 'Select All',
            -command => sub {
				for (my $ch=0; $ch<256; $ch++) {
					$channelMidDisp{$MasterIDs{$mid}."#$ch"} = 1;
					SetSTMoption($MasterIDs{$mid}."#$ch", 
								$channelMidDisp{$MasterIDs{$mid}."#$ch"});
				}
				if ($mid == C_A9_0) {
					for (my $ch=0; $ch<256; $ch++) {
						$channelMidDisp{$MasterIDs{&C_A9_1}."#$ch"} = 1;
						SetSTMoption($MasterIDs{&C_A9_1}."#$ch", 
							$channelMidDisp{$MasterIDs{&C_A9_1}."#$ch"});
					}
				}
			})->pack(@pl);
        $channels_buttons->Button(-text    => 'Unselect All',
            -command => sub {
				for (my $ch=0; $ch<256; $ch++) {
					$channelMidDisp{$MasterIDs{$mid}."#$ch"} = 0;
					SetSTMoption($MasterIDs{$mid}."#$ch", 
								$channelMidDisp{$MasterIDs{$mid}."#$ch"});
				}
				if ($mid == C_A9_0) {
					for (my $ch=0; $ch<256; $ch++) {
						$channelMidDisp{$MasterIDs{&C_A9_1}."#$ch"} = 0;
						SetSTMoption($MasterIDs{&C_A9_1}."#$ch", 
							$channelMidDisp{$MasterIDs{&C_A9_1}."#$ch"});
					}
				}
			})->pack(@pl);
        $channels_buttons->Button(-text    => 'Close',
            -command => [$CHANNELS => 'withdraw'])->pack(@pl);

		for (my $i=0; $i<16; $i++) {
			my $frame = $CHANNELS->Frame->pack(qw/-side top -expand 1 -fill x/);
			for (my $j=0; $j<16; $j++) {
				my $ch = $i*16+$j;
				my $text = sprintf("%03d", $ch);
				$frame->Checkbutton(
					-text=>"$text",
					-variable=>\$channelMidDisp{$MasterIDs{$mid}."#$ch"},
					-command=>sub {
						SetSTMoption($MasterIDs{$mid}."#$ch", 
								$channelMidDisp{$MasterIDs{$mid}."#$ch"})
				})->pack(@pl)
			}
		}
		if ($mid == C_A9_0) {
			my $frame = $CHANNELS->Frame->pack(qw/-side top -expand 1 -fill x/);
			$frame->Label(-text=>"Cortex A9.1")->pack(@pl);
			for (my $i=0; $i<16; $i++) {
				my $frame = $CHANNELS->Frame->pack(qw/-side top -expand 1 -fill x/);
				for (my $j=0; $j<16; $j++) {
					my $ch = $i*16+$j;
					my $text = sprintf("%03d", $ch);
					$frame->Checkbutton(
						-text=>"$text",
						-variable=>\$channelMidDisp{$MasterIDs{&C_A9_1}."#$ch"},
					-command=>sub {
						SetSTMoption($MasterIDs{&C_A9_1}."#$ch", 
							$channelMidDisp{$MasterIDs{&C_A9_1}."#$ch"})
					})->pack(@pl)
			}
		}
	}
    } else {
        $CHANNELS->deiconify;
        $CHANNELS->raise;
    }
    $CHANNELS->title("Channels Config for $MasterIDs{$mid}");
} # end ViewFile
